#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/types.h>
#include "temps.h"

// Numéros des pilotes
int num_pilotes[20] = {1, 11, 44, 63, 16, 55, 4, 81, 14, 18, 10, 31, 23, 2, 22, 3, 77, 24, 20, 27};

// Fonction pour trier les pilotes selon leur temps total
void trier_voitures(voit voitures[], int taille) {
    for (int i = 0; i < taille - 1; i++) {
        for (int j = 0; j < taille - i - 1; j++) {
            if (voitures[j].total > voitures[j + 1].total) {
                voit temp = voitures[j];
                voitures[j] = voitures[j + 1];
                voitures[j + 1] = temp;
            }
        }
    }
}

// Fonction simulant une voiture
void voiture(int i) {
    // Génération des temps aléatoires pour chaque secteur
    srand(getpid() + time(NULL) + i);  // Utilisation de srand() avec PID, time et un offset pour chaque processus enfant

    float s1 = temps_secteur(SECTEUR1, rand() % 5 + 1); // Secteur 1 avec un facteur aléatoire
    float s2 = temps_secteur(SECTEUR2, rand() % 5 + 1); // Secteur 2 avec un facteur aléatoire
    float s3 = temps_secteur(SECTEUR3, rand() % 5 + 1); // Secteur 3 avec un facteur aléatoire
    float total = s1 + s2 + s3;

    // Création de la structure de voiture
    voit temporary_voit = {num_pilotes[i], s1, s2, s3, total};

    // Récupération et attachement à la mémoire partagée
    int shared_memory = shmget(69, 20 * sizeof(voit), 0666);
    if (shared_memory == -1) {
        perror("Erreur lors de la récupération de la mémoire partagée");
        exit(EXIT_FAILURE);
    }
    voit *tab_voit = shmat(shared_memory, NULL, 0);
    if (tab_voit == (voit *)-1) {
        perror("Erreur lors de l'attachement à la mémoire partagée");
        exit(EXIT_FAILURE);
    }

    // Écriture dans la mémoire partagée
    tab_voit[i] = temporary_voit;

    // Détachement de la mémoire partagée
    shmdt(tab_voit);
}




int main() {
    int size_pilotes = sizeof(num_pilotes) / sizeof(num_pilotes[0]);

    // Initialisation du générateur de nombres aléatoires
    srand(time(NULL));  // Initialisation dans le processus parent pour tous les processus enfants

    // Création de la mémoire partagée
    int shared_memory = shmget(69, 20 * sizeof(voit), IPC_CREAT | 0666);
    if (shared_memory == -1) {
        perror("Erreur lors de la création de la mémoire partagée");
        exit(EXIT_FAILURE);
    }
    voit *tab_voit = shmat(shared_memory, NULL, 0);
    if (tab_voit == (voit *)-1) {
        perror("Erreur lors de l'attachement à la mémoire partagée");
        exit(EXIT_FAILURE);
    }

    // Initialisation de la mémoire partagée
    for (int i = 0; i < size_pilotes; i++) {
        tab_voit[i] = (voit){0};
    }

    // Création des processus enfants
    for (int i = 0; i < size_pilotes; i++) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("Erreur lors du fork");
            exit(EXIT_FAILURE);
        }
        if (pid == 0) {
            voiture(i);
            exit(EXIT_SUCCESS);
        }
    }

    // Attente de la fin des processus enfants
    for (int i = 0; i < size_pilotes; i++) {
        wait(NULL);
    }

    // Copie et tri des données
    voit lecture_voit[20];
    memcpy(lecture_voit, tab_voit, size_pilotes * sizeof(voit));
    trier_voitures(lecture_voit, size_pilotes);

    // Affichage des résultats triés
    printf("Classement des pilotes :\n");
    for (int i = 0; i < size_pilotes; i++) {
        printf("Pilote %d : Temps total = %.2f (Secteurs : %.2f, %.2f, %.2f)\n",
               lecture_voit[i].num,
               lecture_voit[i].total,
               lecture_voit[i].s1,
               lecture_voit[i].s2,
               lecture_voit[i].s3);
    }

    // Détachement et suppression de la mémoire partagée
    shmdt(tab_voit);
    shmctl(shared_memory, IPC_RMID, NULL);

    return 0;
}
