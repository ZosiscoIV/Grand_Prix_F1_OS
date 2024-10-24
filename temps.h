#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

float temps_secteur(float baseTime) {
  int i;
  srand(time(NULL)); // Utilise comme seed de création de nombre aléatoire le temps en seconde depuis janvier 1970
  float nombre_aleatoire = rand() % 3000 + 1;  // Génère un nombre pseudo-aléatoire
  temps_S = nombre_aléatoire/1000 + baseTime;
  return temps_S
}

int main() {
  return 0;
}
