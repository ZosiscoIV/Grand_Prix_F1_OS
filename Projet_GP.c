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



void voiture(int i){

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


  close(fichier); // Fermeture après utilisation
}





int main() {
  int child_pid;
  int i;
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


  // Création des processus fils
  for(i = 0; i < size_pilotes; i++) {
    child_pid = fork();
  
    if(child_pid == -1) {
      perror("Erreur lors du fork");
      exit(EXIT_FAILURE);
    }
  
    if(child_pid == 0){
      voiture(i);
      exit(EXIT_SUCCESS);  // Le processus enfant se termine après avoir simulé la voiture
    }
  }
  


  // Processus parent attend que tous les enfants se terminent
  for (i = 0; i < size_pilotes; i++) {
      wait(NULL);  // Attend chaque processus enfant pour éviter les zombies
  }


  voit lecture_voit[20];

  memcpy(lecture_voit, tab_voit, 20*sizeof(voit));

  BubbleSort(lecture_voit, 20);


  for (i = 0; i < size_pilotes; i++) {
    afficher_temps(lecture_voit[i].temps_total, lecture_voit[i].num_p);
  }


  // Détacher la mémoire partagée
  shmdt(tab_voit);
  shmctl(shared_memory, IPC_RMID, NULL);

  return 0;
}
