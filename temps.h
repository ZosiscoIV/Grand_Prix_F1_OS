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
  float temps_tour;
  float temps_total;
  bool in_pit;
  bool out;
} voit;


typedef struct {
    voit best_s1;
    voit best_s2;
    voit best_s3;
    voit best_tour;
} bestTimes;



float temps_secteur(float baseTime, int modifier) {
  int i;
  int pid = getpid();
  srand(pid + modifier + time(NULL)); // Utilise comme seed de création de nombre aléatoire le pid du processus en cours d'exécution
  float nombre_aleatoire = rand() % 7000 + 1;  // Génère un nombre pseudo-aléatoire
  float temps_S = nombre_aleatoire/1000 + baseTime;
  return temps_S;
}


void afficher_temps(voit voiture, voit voiture_avant) {
  float diff, t_sec, tot_sec;
  int t_min, tot_min;
  char pit_out[4];
  diff = voiture.temps_total - voiture_avant.temps_total;
  t_min = (int)(voiture.temps_total / 60);
  t_sec = voiture.temps_total - (t_min * 60);

  tot_min = (int)(voiture.temps_tour / 60);
  tot_sec = voiture.temps_tour - (tot_min * 60);

  if (voiture.out) {
    sprintf(pit_out, "out");
  } else if (voiture.in_pit) {
    sprintf(pit_out, "pit");
  } else {
    sprintf(pit_out, "   ");
  }

  


  printf("| Pilote n°%-2d | %2d min %6.3f sec | %+7.3f | %2d min %6.3f sec | %s      |\n", voiture.num_p, t_min, t_sec, diff, tot_min, tot_sec, pit_out);
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
