
// #include "mbed.h"
// #include "stm32746g_discovery_lcd.h"
// #include "stm32746g_discovery_ts.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// ======== Paramètres du terrain ========

#define H_JEU 36    // Hauteur du jeu
#define L_JEU 28    // Largeur du jeu
#define COTE_CASE 8 // Longueur d'un côté d'une case
#define POS_TERRAIN 24  // Position du terrain sur l'écran (en pixels)


// ======== Taille des acteurs ========

#define COTE_ACTEUR 15


// ======== Position initiale de Pac-Man ========

#define PM_PIN_X 135    // +24 pixels (origine du terrain)
#define PM_PIN_Y 236


typedef int tab4I[4];   // Tableau de 4 entiers


int calc_distEuc(int x1, int y1, int x2, int y2);


// ======== Terrain ========

typedef struct Case Case;

struct Case{

    int i, j;   // Indices de la case dans la grille
    int x, y;   // Position exacte de la case (utile pour l'affichage)
    int type;   // 0 : case vide, 1 : case autorisée, 2 : case interdite
    int contenu;    // 0 : Ne contient rien, 1 : contient un point et 2 : contient un énergisant

    // Indice 0 : gauche, indice 1 : haut, indice 2 : droite et indice 3 : bas
    Case *voisins[4];   // Pointeurs des cases accessibles à partir de cette case (pour PacMan)

};

typedef struct Terrain Terrain;

struct Terrain{

    int x, y;   // Coordonnées du début de la grille
    int nbPtsRes;   // Nombre de points restants à récupérer
    int nbNrjRes;   // Nombre d'énergisants restants à récupérer
    Case grille[H_JEU][L_JEU];

};

Terrain init_terrain(int x, int y, int coteCase, int tabT[H_JEU][L_JEU]);


// ======== Affichage du terrain ========

void afficher_mur(int i, int j, Terrain *ter, int sprMur[COTE_CASE][COTE_CASE]);

void afficher_terrain(Terrain *ter, int lSprMurs[8][4][COTE_CASE][COTE_CASE]);


// ======== Pac-Man ========

typedef struct PacMan PacMan;

struct PacMan{

    int x, y;   // Postion actuelle du centre de Pac-Man
    int i, j;   // Case associée à la position actuelle
    int k, l;   // Position dans la case (i, j)

    int dir;    // Direction actuelle ( 0, 1, 2 ou 3 )

    int vX, vY; // Vitesse actuelle [-1, 0 : gauche], [0, -1 : haut], [1, 0 : droite] et [0, 1 : bas]

    int nvDir;  // Nouvelle direction envisagée, demandée par le contrôleur ( -1 si aucune sinon 0, 1, 2 ou 3)

    int mort;   // 0 si Pac-Man actuellement en vie et 1 sinon
    int immunise;   // Strictement positif si énergisant en cours et 0 sinon ( minuteur )

    int score;  // Score actuelle
    int nbVies; // Nombre de vies restantes

};

PacMan init_pacMan(int x, int y, int dir);

// void acquerir_nvDir(TS_StateTypeDef *tST, PacMan *pm, int jsX, int jsY);

void afficher_ctrl_pacMan(PacMan *pm);

void reperer_pacMan_terrain(PacMan *pm);    // Calculer i, j et k, l

void maj_pos_pacMan(PacMan *pm, Terrain *ter);  // A un instant t donné, mettre à jour la position de Pac-Man en fonction de dir, nvDir et des voisins de la case associée

void afficher_pacMan(PacMan *pm, int lSprPM[4][COTE_ACTEUR][COTE_ACTEUR]);

void manger(PacMan *pm, Terrain *ter);

void calculer_score(PacMan *pm, Terrain *ter);


// ======== Maison ========

typedef int tab4C[4][2];    // Tableau de 4 couples d'entiers

typedef struct Sommet Sommet;

struct Sommet{

    int s;  // indice repérant le sommet
    int x, y;   // coordonnées du sommet
    Sommet *suivS;  // Sommet suivant si le fantôme sort

    int dirF;   // nouvelle direction des fantômes à partir de ce sommet
    
};

typedef struct Maison Maison;

struct Maison{

    Sommet graphe[4];

    // ---- Organiser les sorties de Pinky (1), Inky (2) et Clyde (3) de la maison ----

    int ftmEnAttente[4];    // pour la case i, 1 : fantôme i en attente et 0 : sinon

    int typeCUtil;  // 0 : compteurs individuels utilisés et 1 : compteur global utilisé

    int ftmFav; // -1 : aucun fantôme en attente, 0 : Blinky, 1 : Pinky, 2 : Inky, 3 : Clyde

    int cntPtsG;    // compteur de points global (utilisation temporaire après une perte de vie de Pac-Man)

    int tempsEcoule;    // temps écoulé depuis le dernier point mangé par Pac-Man
    int tempsLimite;
    
};

Maison init_maison(tab4C xySMsn, int tmpLim);


// ======== Fantômes ========

typedef struct Fantome Fantome;

struct Fantome{

    int f;  // indice du fantôme (0 : Blinky, 1, Pinky, 2 : Inky et 3 : Clyde)

    int x, y;   // Position exacte du fantôme

    int dir;    // Direction actuelle du fantôme
    int vX, vY; // Vitesse calculée à partir de dir pour mettre à jour x et y.

    int estMort;    // 0 : non et 1 : oui

    int estDansMaison;  // 0 : non et 1 : oui


    // ---- Utiles à l'extérieur de la maison ----

    int i, j;   // Case associée dans la grille
    int k, l;   // Position du centre dans la case

    int nvDir;  // direction du fantôme à partir du centre de la case suivante
    int dirEnAtt;   // nouvelle direction (en attente) à partir du centre de la case actuelle

    int mode;   // 0 : SE DISPERSER, 1 : CHASSER, 2 : FUIR

    int cISD, cJSD;     // Case cible du fantôme (en mode SE DISPERSER)

    int cIC, cJC;   // Case cible du fantôme (en mode CHASSER)


    // ---- Utiles à l'intérieur de la maison ----

    int estEnAttente;   // 0 : non et 1 : oui

    int sAct;   // indice du sommet associé à la position actuelle du fantôme dans la maison

    int sSuiv;  // indice du sommet suivant sur la trajectoire

    int sRepos; // indice du sommet correspondant à la position de repos du fantôme (où il revient lorsqu'il est mort)

    int cntPts; // compteur de points individuel : +1 si ce fantôme est le favori et si Pac-Man mange un point
    int limCntPoints;   // limite du compteur de points individuel

    int chgtModeExt;    // voit un changement de mode en étant dans la maison, 0 : non et 1 : oui

};

Fantome init_fantome(int f, tab4C xySMsn, tab4C ijCCblSD, int sRepos, tab4I lCsPtsI);

void aut_dep_fantomes(Maison *msn, Fantome *ftms);  // Un tableau est considéré comme un pointeur

void sortir_fantomes_maison(Maison *msn, Fantome *ftms);

void reperer_fantomes_terrain(Fantome *ftms);   // Calculer i, j, k et l

int det_nvDir_ftm(Fantome *ftm, Terrain *ter, PacMan *pm);

void maj_pos_fantomes(Fantome *ftms, Terrain *ter, PacMan *pm, tab4C klDCD);    // dernier paramètre : indices k,l du début de la case en fonction de dir

void afficher_fantomes(Fantome *ftms, int lSprFtms[4][4][COTE_ACTEUR][COTE_ACTEUR]);


// ======== Intéractions entre Pac-Man et Fantômes ========

void detecter_collision(PacMan *pm, Fantome *ftms, tab4C xySMsn, tab4C ijCCblSD, tab4I lCsPtsI);

