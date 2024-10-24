#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include<unistd.h>


float temps_S1() {
  int i;
  int j;
  int temps_base_S1 = 25;
  float temps_pilotes[20];
  float tableau[20][1];//[lignes][colonnes]
  int num_pilotes[20] = {1, 11, 44, 63, 16, 55, 4, 81, 14, 18, 10, 31, 23, 2, 22, 3, 77, 24, 20, 27};
  int nbr_pilote = sizeof(num_pilotes)/sizeof(num_pilotes[0]);
  srand(time(NULL));
  for (i = 0; i < nbr_pilote; i++) {
    for (j = 0; j<1; j++){ //j<nbr colonne
      float nombre_aleatoire = rand() % 3000 + 1;  // Génère un nombre pseudo-aléatoire

      float temps_total_S1 = temps_base_S1 + nombre_aleatoire/1000;
      printf("Pilote n°%d : temps --> %.3f\n", i + 1, temps_total_S1);
      temps_pilotes[i] = temps_total_S1;
      tableau[i][j] = temps_total_S1;
      sleep(1);
    }
  }
  printf("\nAffichage du tableau (5 lignes et 1 colonne) :\n");
  printf("Pilote\t\tTemps\n");
  for (i = 0; i < nbr_pilote; i++) {
    printf("Pilote %d\t", num_pilotes[i]);
    for (j = 0; j < 1; j++) {
      printf("%.3f\t", tableau[i][j]);  // Affichage des temps avec 3 décimales
    }
    printf("\n");
  }

  return 0;
}



int main() {
  temps_S1();
  //printf("%f", temps_S1());
  return 0;
}