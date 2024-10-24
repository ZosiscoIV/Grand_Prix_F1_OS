#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SECTEUR1 25
#define SECTEUR2 20
#define SECTEUR3 30

float temps_secteur(float baseTime) {
  int i;
  int pid = getpid();
  srand(pid); // Utilise comme seed de création de nombre aléatoire le pid du processus en cours d'exécution
  float nombre_aleatoire = rand() % 3000 + 1;  // Génère un nombre pseudo-aléatoire
  float temps_S = nombre_aleatoire/1000 + baseTime;
  return temps_S;
}


void afficher_temps(float temps, int num_pilote) {
  printf("Pilote n°%d ---> %.3f sec\n", num_pilote, temps);
}
