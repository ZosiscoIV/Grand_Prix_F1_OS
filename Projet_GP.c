#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include "temps.h"


int num_pilotes[20] = {1, 11, 44, 63, 16, 55, 4, 81, 14, 18, 10, 31, 23, 2, 22, 3, 77, 24, 20, 27};



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



void course(int i, bool isQualif){

  // Ouverture d'un fichier pour stocker les temps
  int fichier = open("temps_stockage.txt", O_CREAT|O_APPEND|O_RDWR, 0666);
  if (fichier == -1) {
    perror("Erreur lors de l'ouverture du fichier temps_stockage");
    exit(EXIT_FAILURE);
  }
  else {
    close(fichier);
  }

  int num = num_pilotes[i];

  float s1 = temps_secteur(SECTEUR1, 2);
  //afficher_temps(s1, num_pilotes[i]);
  stocker_temps(s1, num_pilotes[i], i, 1, fichier);

  float s2 = temps_secteur(SECTEUR2, 4);
  //afficher_temps(s2, num_pilotes[i]);
  //stocker_temps(s2, num_pilotes[i], i, 2, fichier);

  float s3 = temps_secteur(SECTEUR3, 1);
  //afficher_temps(s3, num_pilotes[i]);
  //stocker_temps(s3, num_pilotes[i], i, 3, fichier);

  srand(i);
  float test = rand() % 21;
  bool pit = (rand() % 20 == 0);
  bool out = (rand() % 100 == 0);


  float tour = s1 + s2 + s3;

  int shared_memory = shmget(69, 20*sizeof(voit), 0666);
  voit *tab_voit = shmat(shared_memory, 0, 0666);
  
  float stock_total = tab_voit[i].temps_total;
  if (!isQualif) {
    stock_total = stock_total + tour;
  } else {
    if (stock_total > tour || stock_total == 0)
      stock_total = tour;
  }


  voit temporary_voit = {num, s1, s2, s3, tour, stock_total, pit, out};

  tab_voit[i] = temporary_voit;

  //afficher_temps(total, num_pilotes[i]);


  close(fichier); // Fermeture après utilisation
  sleep(1);
}


bestTimes lecture_tri_affichage(voit *tableau_voit, int size_pilotes, bestTimes best_times) {
  int i;
  voit lecture_voit[20]; // Crée un tableau pour stocker tout les temps

  memcpy(lecture_voit, tableau_voit, 20*sizeof(voit)); // Fait une copie de la mémoire partagée

  BubbleSort(lecture_voit, 20); // Trie le tableau des temps

  printf("---------------------------------------------------------------------------\n");
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
      afficher_temps(lecture_voit[i].temps_total, lecture_voit[i].num_p, lecture_voit[i].temps_total, lecture_voit[i].temps_tour);
    }
    else {
      afficher_temps(lecture_voit[i].temps_total, lecture_voit[i].num_p, lecture_voit[i-1].temps_total, lecture_voit[i].temps_tour);
    }
  }
  printf("| s1 n°%2d : %6.3f, s2 n°%2d : %6.3f, s3 n°%2d : %6.3f, tour n°%2d : %6.3f |\n", best_times.best_s1.num_p, best_times.best_s1.temps_s1, best_times.best_s2.num_p, best_times.best_s2.temps_s2, best_times.best_s3.num_p, best_times.best_s3.temps_s3, best_times.best_tour.num_p, best_times.best_tour.temps_tour);
  printf("---------------------------------------------------------------------------\n"); // Affiche les temps
  return best_times;
}




int grand_prix(int nbr_tours) {
  int child_pid;
  int h, i, j, k, l;
  int size_pilotes = sizeof(num_pilotes)/sizeof(num_pilotes[0]);


  // Création de la mémoire partagée
  int shared_memory = shmget(69, 20*sizeof(voit), IPC_CREAT | 0666);
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
  for (i = 0; i < 20; i++) {
    tab_voit[i] = (voit){0};
  }

  bestTimes best_times = {0};  // Initialisation des meilleurs temps
  best_times.best_s1.temps_s1 = best_times.best_s2.temps_s2 = best_times.best_s3.temps_s3 = best_times.best_tour.temps_tour = 1000;
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
        course(i, false);
        exit(EXIT_SUCCESS);  // Le processus enfant se termine après avoir simulé la voiture
      }
    }
    
    // Processus parent attend que tous les enfants se terminent
    for (l = 0; l < size_pilotes; l++) {
        wait(NULL);  // Attend chaque processus enfant pour éviter les zombies
    }
    // Lecture et affichage des temps en mémoire partagée
    best_times = lecture_tri_affichage(tab_voit, size_pilotes, best_times);
    sleep(1);
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


  return 0;
}




int q1(int nbr_tours) {
  int child_pid;
  int h, i, j, k, l;
  int size_pilotes = sizeof(num_pilotes)/sizeof(num_pilotes[0]);


  // Création de la mémoire partagée
  int shared_memory = shmget(69, 20*sizeof(voit), IPC_CREAT | 0666);
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
  for (i = 0; i < 20; i++) {
    tab_voit[i] = (voit){0};
  }

  bestTimes best_times = {0};  // Initialisation des meilleurs temps
  best_times.best_s1.temps_s1 = best_times.best_s2.temps_s2 = best_times.best_s3.temps_s3 = best_times.best_tour.temps_tour = 1000;
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
        course(i, true);
        exit(EXIT_SUCCESS);  // Le processus enfant se termine après avoir simulé la voiture
      }
    }
    
    // Processus parent attend que tous les enfants se terminent
    for (l = 0; l < size_pilotes; l++) {
        wait(NULL);  // Attend chaque processus enfant pour éviter les zombies
    }
    // Lecture et affichage des temps en mémoire partagée
    best_times = lecture_tri_affichage(tab_voit, size_pilotes, best_times);
    sleep(1);
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


  return 0;
}

void q2(int nbr_tours){
  printf("c'est ok q2!\n");
}

void q3(int nbr_tours){
  printf("c'est ok q3!\n");
}




int main(int argc, char **argv) {
  int tours = atoi(argv[2]);
  if (argc > 1) {
      if (strcmp(argv[1], "gp") == 0) {
          grand_prix(tours);
      } else if (strcmp(argv[1], "q1") == 0) {
          q1(tours);
      } else if (strcmp(argv[1], "q2") == 0) {
          q2(tours);
      } else if (strcmp(argv[1], "q3") == 0) {
          q3(tours);
      } else if (strcmp(argv[1], "p1") == 0) {
          q1(tours);
      } else if (strcmp(argv[1], "p2") == 0) {
          q1(tours);
      } else if (strcmp(argv[1], "p3") == 0) {
          q1(tours);
      } else if (strcmp(argv[1], "sprint") == 0) {
          grand_prix(tours);
      } else {
          printf("Veuillez fournir un argument valide (ex: grand_prix, q1, ...).\n");
      }
  } else {
      printf("Veuillez fournir un argument (ex: grand_prix, q1, ...).\n");
  }
}
