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



void course(int i){

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


  float total = s1 + s2 + s3;
  voit temporary_voit = {num, s1, s2, s3, total, pit, out};

  int shared_memory = shmget(69, 20*sizeof(voit), 0666);
  voit *tab_voit = shmat(shared_memory, 0, 0666);

  tab_voit[i] = temporary_voit;

  //afficher_temps(total, num_pilotes[i]);


  close(fichier); // Fermeture après utilisation
  sleep(1);
}


void lecture_tri_affichage(voit *tableau_voit, int size_pilotes) {
  int i;
  voit lecture_voit[20]; // Crée un tableau pour stocker tout les temps

  memcpy(lecture_voit, tableau_voit, 20*sizeof(voit)); // Fait une copie de la mémoire partagée

  BubbleSort(lecture_voit, 20); // Trie le tableau des temps

  printf("-------------------------------------------------\n");
  for (i = 0; i < size_pilotes; i++) {
    if (i == 0) {
      afficher_temps(lecture_voit[i].temps_total, lecture_voit[i].num_p, lecture_voit[i].temps_total);
    }
    else {
      afficher_temps(lecture_voit[i].temps_total, lecture_voit[i].num_p, lecture_voit[i-1].temps_total);
    }
  }
  printf("-------------------------------------------------\n"); // Affiche les temps
}






int main() {
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


  // Simulation de plusieurs tours
  for (h = 0; h < 10; h++) {
    // Création des processus fils
    for(i = 0; i < size_pilotes; i++) {
      child_pid = fork();
  
      if(child_pid == -1) {
        perror("Erreur lors du fork");
        exit(EXIT_FAILURE);
      }
  
      if(child_pid == 0){
        course(i);
        exit(EXIT_SUCCESS);  // Le processus enfant se termine après avoir simulé la voiture
      }
    }
    
    // Processus parent attend que tous les enfants se terminent
    for (l = 0; l < size_pilotes; l++) {
        wait(NULL);  // Attend chaque processus enfant pour éviter les zombies
    }
    // Lecture et affichage des temps en mémoire partagée
    lecture_tri_affichage(tab_voit, size_pilotes);
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
