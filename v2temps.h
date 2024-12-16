#ifndef TEMPS_H
#define TEMPS_H

// Définition des secteurs
#define SECTEUR1 1
#define SECTEUR2 2
#define SECTEUR3 3

// Structure représentant une voiture
typedef struct {
    int num;      // Numéro du pilote
    float s1;     // Temps secteur 1
    float s2;     // Temps secteur 2
    float s3;     // Temps secteur 3
    float total;  // Temps total
} voit;

// Prototypes des fonctions
float temps_secteur(float baseTime, int modifier);


#endif
