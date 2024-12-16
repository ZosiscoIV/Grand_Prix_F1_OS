#include "temps.h"
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

float temps_secteur(float baseTime, int modifier) {
    int pid = getpid();
    srand(pid + modifier + time(NULL));  // Utilisation du pid et d'un offset pour la graine du générateur
    float nombre_aleatoire = rand() % 7000 + 1;  // Génère un nombre aléatoire dans un intervalle
    float temps_S = nombre_aleatoire / 1000 + baseTime;  // Conversion en temps avec un ajustement basé sur baseTime
    return temps_S;
}
