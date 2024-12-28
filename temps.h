#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <string.h>

#define SECTEUR1 25
#define SECTEUR2 20
#define SECTEUR3 30

//float temps_s1[20] = {0};
//float temps_s2[20] = {0};
//float temps_s3[20] = {0};
//float temps_totaux[20] = {0};

typedef struct {
  int num_p;
  float temps_s1;
  float temps_s2;
  float temps_s3;
  float temps_total;
  bool in_pit;
  bool out;
} voit;



float temps_secteur(float baseTime, int modifier) {
  int i;
  int pid = getpid();
  srand(pid + modifier + time(NULL)); // Utilise comme seed de création de nombre aléatoire le pid du processus en cours d'exécution
  float nombre_aleatoire = rand() % 7000 + 1;  // Génère un nombre pseudo-aléatoire
  float temps_S = nombre_aleatoire/1000 + baseTime;
  return temps_S;
}


void afficher_temps(float temps, int num_pilote, float temps_precedent) {
  float diff;
  diff = temps - temps_precedent;
  printf("| Pilote n°%d	| %.3f sec | + %.3f |\n", num_pilote, temps, diff);
}

void stocker_temps(float temps, int num_pilote, int i, int secteur, int fichier) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Pilote %d Secteur %d : %.3f sec\n", num_pilote, secteur, temps);
    write(fichier, buffer, strlen(buffer));
}

    /*switch (secteur) {
        case 1:
            temps_s1[i] = temps;
            break;
        case 2:
            temps_s2[i] = temps;
            break;
        case 3:
            temps_s3[i] = temps;
            break;
    }
    temps_totaux[i] = temps_s1[i] + temps_s2[i] + temps_s3[i];*/




  /*int fich = open("temps_stockage.txt", O_CREAT|O_APPEND|O_RDWR, 0666);
  if (fich == -1) {
    perror("Erreur lors de l'ouverture du fichier temps_stockage");
    exit(1);
  }
  else {
    close(fich);
  }
  printf("pas fait\n");*/
