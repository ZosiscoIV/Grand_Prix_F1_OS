#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include "temps.h"

#define NOMBRE_PILOTES_BASE 20
#define NOMBRE_PILOTES_Q2 15
#define NOMBRE_PILOTES_Q3 10

// Remplacement des tableaux statiques par l'ouverture et la lecture depuis des fichiers
// (les fichiers sont à ajouter à l'avenir, il faut les lire ici)
int* num_pilotes;  // Déclaration des tableaux comme pointeurs
int* num_pilotes_q2;
int* num_pilotes_q3;

// Compteur de rédacteurs
int writer_count = 0;
// Identifiants des sémaphores
int mutex, read_lock;

// Initialisation des sémaphores
void init_semaphores() {
    // Création de deux sémaphores
    mutex = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (mutex == -1) {
        perror("Erreur lors de la création du sémaphore du compteur");
        exit(EXIT_FAILURE);
    }

    read_lock = semget(IPC_PRIVATE, 1, IPC_CREAT | 0666);
    if (read_lock == -1) {
        perror("Erreur lors de la création du sémaphore rédacteur");
        exit(EXIT_FAILURE);
    }

    // Initialisation du sémaphore lecteur à 1 (seul un lecteur peut lire)
    if (semctl(mutex, 0, SETVAL, 1) == -1) {
        perror("Erreur d'initialisation du sémaphore du compteur");
        exit(EXIT_FAILURE);
    }

    // Initialisation du sémaphore rédacteur à 1 (seul un rédacteur peut écrire)
    if (semctl(read_lock, 0, SETVAL, 1) == -1) {
        perror("Erreur d'initialisation du sémaphore rédacteur");
        exit(EXIT_FAILURE);
    }
}



// Fonction pour effectuer une opération de P (attente sur le sémaphore)
void P(int semid) {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = -1;  // Opération P : décrémentation
    sb.sem_flg = 0;
    if (semop(semid, &sb, 1) == -1) {
        perror("Erreur lors de l'opération P sur le sémaphore");
        exit(EXIT_FAILURE);
    }
}


// Fonction pour effectuer une opération de V (signal sur le sémaphore)
void V(int semid) {
    struct sembuf sb;
    sb.sem_num = 0;
    sb.sem_op = 1;  // Opération V : incrémentation
    sb.sem_flg = 0;
    if (semop(semid, &sb, 1) == -1) {
        perror("Erreur lors de l'opération V sur le sémaphore");
        exit(EXIT_FAILURE);
    }
}



// Fonction pour sauvegarder les pilotes dans le fichier
void sauvegarder_pilotes(voit *pilotes, int nombre_pilotes, const char *fichier) {
    FILE *f = fopen(fichier, "w");
    if (!f) {
        perror("Erreur d'ouverture du fichier pour sauvegarde");
        exit(EXIT_FAILURE);
    }
    
    // Sauvegarde des 15 premiers pilotes ou 10 premiers
    int i;
    for (i = 0; i < nombre_pilotes; i++) {
        fprintf(f, "%d\n", pilotes[i].num_p); // Sauvegarde des numéros des pilotes dans l'ordre
    }

    fclose(f); // Fermeture du fichier après écriture
}

void sauvegarder_5_derniers_pilotes(int *tableau, int taille, const char *fichier) {
    FILE *f = fopen(fichier, "a");  // Ouvre en mode "append" pour ajouter à la fin
    if (!f) {
        perror("Erreur d'ouverture du fichier pour ajouter les derniers pilotes");
        exit(EXIT_FAILURE);
    }

    // Sauvegarde des 5 derniers pilotes
    int i;
    for (i = taille - 5; i < taille; i++) {
        fprintf(f, "%d\n", tableau[i]); // Sauvegarde des numéros des pilotes
    }

    fclose(f);  // Fermeture du fichier après écriture
}


// Fonction pour lire les numéros des pilotes depuis un fichier
void lire_pilotes(const char *fichier, int *tableau, int taille) {
    FILE *f = fopen(fichier, "r");
    if (!f) {
        perror("Erreur d'ouverture du fichier");
        exit(EXIT_FAILURE);
    }
    int i;
    for (i = 0; i < taille; i++) {
        fscanf(f, "%d", &tableau[i]); // Remplissage du tableau avec les numéros de pilotes
    }

    fclose(f); // Fermeture du fichier après lecture
}

void BubbleSort(voit a[], int array_size){
  int i, j;
  voit temp;
  for (i = 0; i < (array_size - 1); ++i){
    for (j = 0; j < array_size - 1 - i; ++j){
      if (a[j].temps_total > a[j+1].temps_total){
        temp = a[j+1];
        a[j+1] = a[j];
        a[j] = temp;
      }
    }
  }
}

// Affichage grille de départ
void affichage_grille(int participants) {
  int i;
  for (i = participants; i > 0; i--) {
    if ((i % 2) == 0) {
      printf("n°%2d |__%2d__|\n", i, num_pilotes[i-1]);
    } else {
      printf("                 n°%2d |__%2d__|\n", i, num_pilotes[i-1]);
    }
  }
}  



void course(int i, bool isQualif, int participants, int tour_fait){
  // Demande d'accès pour un rédacteur
  P(mutex);                   // Entrée en section critique
  writer_count++;
  if (writer_count == 1) {    // Premier rédacteur bloque la lecture
    P(read_lock);
  }
  V(mutex);                   // Sortie de section critique


  // Ouverture d'un fichier pour stocker les temps
  // int fichier = open("temps_stockage.txt", O_CREAT|O_APPEND|O_RDWR, 0666);
  // if (fichier == -1) {
  //   perror("Erreur lors de l'ouverture du fichier temps_stockage");
  //   exit(EXIT_FAILURE);
  // }
  // else {
  //   close(fichier);
  // }

  int num;

  // Lecture dynamique des numéros de pilotes en fonction du nombre de participants
  if (participants == 20) {
    num = num_pilotes[i];
  } else if (participants == 15) {
    num = num_pilotes_q2[i];
  } else if (participants == 10) {
    num = num_pilotes_q3[i];
  } else {
    num = 0;
  }
 
  float s1 = temps_secteur(SECTEUR1, 2);
  float s2 = temps_secteur(SECTEUR2, 4);
  float s3 = temps_secteur(SECTEUR3, 1);

  int pid = getpid();
  srand(i + pid + time(NULL));
  bool pit = (rand() % 6 == 0);
  bool v_out = (rand() % 100 == 0);

  if (pit) {
    s3 = s3 + 25;
  }
  float tour = s1 + s2 + s3;

  int shared_memory = shmget(6969, participants*sizeof(voit), 0666);
  voit *tab_voit = shmat(shared_memory, 0, 0666);
  
  

  if (tab_voit[i].out) {
    v_out = true;
  }

  float stock_total = tab_voit[i].temps_total;
  if (!isQualif) {
    stock_total = stock_total + tour;
  } else {
    if (stock_total > tour || stock_total == 0)
      stock_total = tour;
  }

  if (!v_out || tab_voit[i].num_p == 0) {
    voit temporary_voit = {num, s1, s2, s3, tour, stock_total, pit, v_out};
    tab_voit[i] = temporary_voit;
  } else if (v_out && isQualif) {
    tab_voit[i].out = v_out;
  } else if (v_out && !tab_voit[i].out) {
    tab_voit[i].out = v_out;
    tab_voit[i].temps_total = 59959 - tour_fait ;
  } else {
    tab_voit[i].out = v_out;
  }

  // close(fichier); // Fermeture après utilisation

  // Libération du semaphore pour les rédacteurs
  P(mutex);                   // Entrée en section critique
  writer_count--;
  if (writer_count == 0) {    // Dernier rédacteur libère la lecture
    V(read_lock);
  }
  V(mutex);                   // Sortie de section critique
  sleep(1);
}

bestTimes lecture_tri_affichage(voit *tableau_voit, int size_pilotes, bestTimes best_times, int participants, bool isQualif) {

  // Attente pour les lecteurs
  P(read_lock);  // Lecteur prend le semaphore

  int i;
  voit lecture_voit[participants]; // Crée un tableau pour stocker tout les temps

  memcpy(lecture_voit, tableau_voit, participants*sizeof(voit)); // Fait une copie de la mémoire partagée

  BubbleSort(lecture_voit, participants); // Trie le tableau des temps

  printf("-----------------------------------------------------------------------------\n");
  for (i = 0; i < size_pilotes; i++) {
    if (lecture_voit[i].temps_s1 < best_times.best_s1.temps_s1) {
      best_times.best_s1 = lecture_voit[i];
    }
    if (lecture_voit[i].temps_s2 < best_times.best_s2.temps_s2) {
      best_times.best_s2 = lecture_voit[i];
    }
    if (lecture_voit[i].temps_s3 < best_times.best_s3.temps_s3) {
      best_times.best_s3 = lecture_voit[i];
    }
    if (lecture_voit[i].temps_tour < best_times.best_tour.temps_tour) {
      best_times.best_tour = lecture_voit[i];
    }
    if (i == 0) {
      afficher_temps(lecture_voit[i], lecture_voit[i], isQualif);
    }
    else {
      afficher_temps(lecture_voit[i], lecture_voit[i-1], isQualif);
    }
  }
  printf("| s1 n°%2d : %6.3f, s2 n°%2d : %6.3f, s3 n°%2d : %6.3f, tour n°%2d : %6.3f  |\n", best_times.best_s1.num_p, best_times.best_s1.temps_s1, best_times.best_s2.num_p, best_times.best_s2.temps_s2, best_times.best_s3.num_p, best_times.best_s3.temps_s3, best_times.best_tour.num_p, best_times.best_tour.temps_tour);
  printf("-----------------------------------------------------------------------------\n"); // Affiche les temps

  // Libération du semaphore pour les lecteurs
  V(read_lock);  // Lecteur libère le semaphore
  return best_times;
}

int grand_prix(int nbr_tours, bool qualif, int participants_course, bool show_grid) {
  int child_pid;
  int h, i, j, k, l;
  int size_pilotes = participants_course; //sizeof(num_pilotes)/sizeof(num_pilotes[0]);

  // Allocation dynamique des numéros de pilotes
  num_pilotes = (int*) malloc(NOMBRE_PILOTES_BASE * sizeof(int)); // Allocation mémoire pour num_pilotes
  num_pilotes_q2 = (int*) malloc(NOMBRE_PILOTES_BASE * sizeof(int)); // Allocation mémoire pour num_pilotes_q2
  num_pilotes_q3 = (int*) malloc(NOMBRE_PILOTES_Q2 * sizeof(int)); // Allocation mémoire pour num_pilotes_q3

  // Lecture des numéros de pilotes depuis les fichiers
  if (qualif) {
    lire_pilotes("q1.txt", num_pilotes, NOMBRE_PILOTES_BASE);
  } else {
    lire_pilotes("ordre_course.txt", num_pilotes, NOMBRE_PILOTES_BASE);
  }
  lire_pilotes("q2.txt", num_pilotes_q2, NOMBRE_PILOTES_Q2);
  lire_pilotes("q3.txt", num_pilotes_q3, NOMBRE_PILOTES_Q3);

  // Création de la mémoire partagée
  int shared_memory = shmget(6969, 20*sizeof(voit), IPC_CREAT | 0666);
  if (shared_memory == -1) {
    perror("Erreur lors de la création de la mémoire partagée");
    exit(EXIT_FAILURE);
  }
  voit *tab_voit = shmat(shared_memory, 0, 0666);
  if (tab_voit == (voit *)-1) {
        perror("Erreur lors de l'attachement de la mémoire partagée");
        exit(EXIT_FAILURE);
  }

  // Initialisation de la mémoire partagée
  for (i = 0; i < participants_course; i++) {
    tab_voit[i] = (voit){0};
  }

  bestTimes best_times = {0};  // Initialisation des meilleurs temps
  best_times.best_s1.temps_s1 = best_times.best_s2.temps_s2 = best_times.best_s3.temps_s3 = best_times.best_tour.temps_tour = 1000;
  
  // Affichage grille de départ
  if (show_grid) {
    affichage_grille(participants_course);
  }

  // Simulation de plusieurs tours
  for (h = 0; h < nbr_tours; h++) {
    // Création des processus fils
    for(i = 0; i < size_pilotes; i++) {
      child_pid = fork();
  
      if(child_pid == -1) {
        perror("Erreur lors du fork");
        exit(EXIT_FAILURE);
      }
  
      if(child_pid == 0){
        course(i, qualif, participants_course, h);
        exit(EXIT_SUCCESS);  // Le processus enfant se termine après avoir simulé la voiture
      }
    }
    
    // Processus parent attend que tous les enfants se terminent
    for (l = 0; l < size_pilotes; l++) {
        wait(NULL);  // Attend chaque processus enfant pour éviter les zombies
    }
    // Lecture et affichage des temps en mémoire partagée
    best_times = lecture_tri_affichage(tab_voit, size_pilotes, best_times, participants_course, qualif);
    sleep(1);
  }
  voit lecture_voit[participants_course]; // Crée un tableau pour stocker tout les temps

  memcpy(lecture_voit, tab_voit, participants_course*sizeof(voit)); // Fait une copie de la mémoire partagée

  BubbleSort(lecture_voit, participants_course); // Trie le tableau des temps

  if (participants_course == 20 && qualif) {
    sauvegarder_pilotes(lecture_voit, 20, "q2.txt");  // Sauvegarde les 15 premiers après q2
  } else if (participants_course == 15) {
    sauvegarder_pilotes(lecture_voit, 15, "q3.txt");  // Sauvegarde les 10 premiers après q3
  } else if (participants_course == 10) {
    sauvegarder_pilotes(lecture_voit, 10, "ordre_course.txt");  // Sauvegarde l'ordre des 10 premiers

    lire_pilotes("q2.txt", num_pilotes_q2, NOMBRE_PILOTES_BASE);
    lire_pilotes("q3.txt", num_pilotes_q3, NOMBRE_PILOTES_Q2);
    // Sauvegarder les 5 derniers du q2
    sauvegarder_5_derniers_pilotes(num_pilotes_q3, NOMBRE_PILOTES_Q2, "ordre_course.txt");
    // Sauvegarder les 5 derniers du q1
    sauvegarder_5_derniers_pilotes(num_pilotes_q2, NOMBRE_PILOTES_BASE, "ordre_course.txt");
  }


  // Détacher la mémoire partagée
  if (shmdt(tab_voit) == -1) {
    perror("Erreur lors du détachement de la mémoire partagée");
    exit(EXIT_FAILURE);
  }
  if (shmctl(shared_memory, IPC_RMID, NULL) == -1) {
    perror("Erreur lors de la suppression de la mémoire partagée");
    exit(EXIT_FAILURE);
  }

  if (semctl(mutex, 0, IPC_RMID) == -1) {
    perror("Erreur lors de la suppression du sémaphore lecteur");
    exit(EXIT_FAILURE);
  }
  if (semctl(read_lock, 0, IPC_RMID) == -1) {
    perror("Erreur lors de la suppression du sémaphore rédacteur");
    exit(EXIT_FAILURE);
  }



  // Libération de la mémoire allouée dynamiquement
  free(num_pilotes);  // Libère la mémoire allouée pour num_pilotes
  free(num_pilotes_q2);  // Libère la mémoire allouée pour num_pilotes_q2
  free(num_pilotes_q3);  // Libère la mémoire allouée pour num_pilotes_q3

  return 0;
}

int main(int argc, char **argv) {
  init_semaphores();
  int tours = atoi(argv[2]);
  if (argc > 1) {
      if (strcmp(argv[1], "gp") == 0) {
          grand_prix(tours, false, NOMBRE_PILOTES_BASE, true);
      } else if (strcmp(argv[1], "q1") == 0) {
          grand_prix(tours, true, NOMBRE_PILOTES_BASE, false);
      } else if (strcmp(argv[1], "q2") == 0) {
          grand_prix(tours, true, NOMBRE_PILOTES_Q2, false);
      } else if (strcmp(argv[1], "q3") == 0) {
          grand_prix(tours, true, NOMBRE_PILOTES_Q3, false);
      } else if (strcmp(argv[1], "p1") == 0) {
          grand_prix(tours, true, NOMBRE_PILOTES_BASE, false);
      } else if (strcmp(argv[1], "p2") == 0) {
          grand_prix(tours, true, NOMBRE_PILOTES_BASE, false);
      } else if (strcmp(argv[1], "p3") == 0) {
          grand_prix(tours, true, NOMBRE_PILOTES_BASE, false);
      } else if (strcmp(argv[1], "sprint") == 0) {
          grand_prix(tours, false, NOMBRE_PILOTES_BASE, false);
      } else {
          printf("Veuillez fournir un argument valide (ex: gp, q1, p2, sprint, ...).\n");
      }
  } else {
      printf("Veuillez fournir un argument (ex: gp, q1, p2, sprint, ...).\n");
  }
}
