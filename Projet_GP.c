#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "temps.h"

void voiture(int i){
  int num_pilotes[20] = {1, 11, 44, 63, 16, 55, 4, 81, 14, 18, 10, 31, 23, 2, 22, 3, 77, 24, 20, 27};
  float s1 = temps_secteur(SECTEUR1);
  afficher_temps(s1, num_pilotes[i]);
}



int main() {
  int child_pid;
  int i;

  for(i = 0; i < 20; i++) {
    child_pid = fork();
  
    if(child_pid == -1) {
      perror("Erreur lors du fork");
      exit(EXIT_FAILURE);
    }
  
    if(child_pid == 0){
      voiture(i);
      exit(0);  // Le processus enfant se termine après avoir simulé la voiture
    }
  }
  
  // Processus parent attend que tous les enfants se terminent
  for (i = 0; i < 20; i++) {
      wait(NULL);  // Attend chaque processus enfant pour éviter les zombies
  }
  

  return 0;
}
