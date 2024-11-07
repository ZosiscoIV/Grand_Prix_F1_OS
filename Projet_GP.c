#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include "temps.h"

int num_pilotes[20] = {1, 11, 44, 63, 16, 55, 4, 81, 14, 18, 10, 31, 23, 2, 22, 3, 77, 24, 20, 27};

void voiture(int i){

  // ouverture d'un fichier pour stocker les temps
  int fichier = open("temps_stockage.txt", O_CREAT|O_APPEND|O_RDWR, 0666);
  if (fichier == -1) {
    perror("Erreur lors de l'ouverture du fichier temps_stockage");
    exit(EXIT_FAILURE);
  }
  else {
    close(fichier);
  }


  float s1 = temps_secteur(SECTEUR1);
  afficher_temps(s1, num_pilotes[i]);
  stocker_temps(s1, num_pilotes[i], i, 1, fichier);

  float s2 = temps_secteur(SECTEUR2);
  afficher_temps(s2, num_pilotes[i]);
  stocker_temps(s2, num_pilotes[i], i, 2, fichier);

  float s3 = temps_secteur(SECTEUR3);
  afficher_temps(s3, num_pilotes[i]);
  stocker_temps(s3, num_pilotes[i], i, 3, fichier);

  close(fichier); //fermeture après utilisation
}



int main() {
  int child_pid;
  int i;
  int size_pilotes = sizeof(num_pilotes)/sizeof(num_pilotes[0]);
  

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
  for (i = 0; i < size_pilotes; i++) {
    afficher_temps(temps_totaux[i], num_pilotes[i]);
  }

  return 0;
}
