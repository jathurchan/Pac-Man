
#include "functions.h"


int vtsDeDir[4][2] = {      // ATTENTION : 0 : x et 1 : y --- ou --- 0 : j et 1 : i
        {-1, 0},    // 0 : Gauche
        {0, -1},    // 1 : Haut
        {1, 0},     // 2 : Droite
        {0, 1},     // 3 : Bas
};

int nvDirInterdite[4] = {2, 3, 0, 1};   // utilisé pour interdire nouvelle direction opposée à la direction en attente

int prefNvDir[4] = {1, 0, 3, 2};    // Haut, Gauche, Bas puis enfin Droite

int calc_distEuc(int x1, int y1, int x2, int y2){
    return (int)(sqrt( 1.0* ( (x1-x2) * (x1-x2) + (y1-y2) * (y1-y2) ) ));
}

// uint32_t paletteSprites[10]={0x00000000, LCD_COLOR_YELLOW, LCD_COLOR_RED, LCD_COLOR_LIGHTMAGENTA, LCD_COLOR_CYAN, LCD_COLOR_ORANGE, LCD_COLOR_WHITE, LCD_COLOR_BLACK, LCD_COLOR_DARKBLUE, LCD_COLOR_LIGHTGRAY};


// ======== Initialisation du terrain ========

Terrain init_terrain(int x, int y, int coteCase, int tabT[H_JEU][L_JEU]){

    Terrain ter;
    ter.x = x;  // cf Miro (image)
    ter.y = y;

    ter.nbPtsRes = 244; // Nombre de points restants dans la grille à récupérer (cf Gamasutra)

    for (int i = 0; i < H_JEU; i++ ){
        for (int j = 0; j < L_JEU; j++){
            ter.grille[i][j].i = i;
            ter.grille[i][j].j = j;

            ter.grille[i][j].x = ter.x + j* coteCase;
            ter.grille[i][j].y = ter.y + i* coteCase;

            ter.grille[i][j].type = tabT[i][j];

            if (tabT[i][j] == 1){       // des points dans toutes les cases autorisées dans un premier temps
                ter.grille[i][j].contenu = 1;
            } else {
                ter.grille[i][j].contenu = 0;   // cases vides et interdites ne contiennent rien
            }
        }
    }

    for (int j = 0; j <= 5; j++){   // pas de points dans le tunnel (gauche)
        ter.grille[17][j].contenu = 0;
    }

    for (int j = 22; j < L_JEU; j++){   // pas de points dans le tunnel (droite)
        ter.grille[17][j].contenu = 0;
    }

    for (int i = 12; i <= 21; i++){     // pas de points au milieu, autour de la maison des monstres
        for (int j = 7; j <= 20; j++){
            ter.grille[i][j].contenu = 0;
        }
    }
    
    ter.grille[26][13].contenu = 0;
    ter.grille[26][14].contenu = 0;
    ter.grille[22][9].contenu = 0;
    ter.grille[22][18].contenu = 0;


    // Placement des énergisants

    ter.grille[6][1].contenu = 2;   // en haut à gauche
    ter.grille[6][26].contenu = 2;  // en haut à droite
    ter.grille[26][1].contenu = 2;  // en bas à gauche
    ter.grille[26][26].contenu = 2; // en bas à droite


    // ---- Initialisation des tableaux de pointeurs (voisins et voisinsF) ----

    // 1. Première initialisation

    for (int i = 0; i < H_JEU; i++ ){
        for (int j = 0; j < L_JEU; j++){
            if (ter.grille[i][j].type == 1){    // case actuelle autorisée ?

                if (i > 0){ // une case en haut existe ?
                    if (ter.grille[i-1][j].type == 1) { // case au-dessus autorisée ?
                        ter.grille[i][j].voisins[1] = &ter.grille[i-1][j];  // pointeur de la case du haut
                    } else {
                        ter.grille[i][j].voisins[1] = NULL;
                    }
                }

                if (j > 0){ // une case à gauche existe ?
                    if (ter.grille[i][j-1].type == 1) { // case à gauche autorisée ?
                        ter.grille[i][j].voisins[0] = &ter.grille[i][j-1];  // pointeur de la case de gauche
                    } else {
                        ter.grille[i][j].voisins[0] = NULL;
                    }
                }

                if (i < H_JEU){ // une case en bas existe ?
                    if (ter.grille[i+1][j].type == 1) { // case en bas autorisée ?
                        ter.grille[i][j].voisins[3] = &ter.grille[i+1][j];  // pointeur de la case du bas
                    } else {
                        ter.grille[i][j].voisins[3] = NULL;
                    }
                }

                if (j < L_JEU){ // une case à droite existe ?
                    if (ter.grille[i][j+1].type == 1) { // case à droite autorisée ?
                        ter.grille[i][j].voisins[2] = &ter.grille[i][j+1];  // pointeur de la case de droite
                    } else {
                        ter.grille[i][j].voisins[2] = NULL;
                    }
                }

            }
        }
    }

    // 2. Relier les cases aux extrémités (tunnel)

    ter.grille[17][0].voisins[1] = &ter.grille[17][L_JEU-1];    // la case tout à gauche

    ter.grille[17][L_JEU-1].voisins[3] = &ter.grille[17][0];    // la case tout à droite
    

    return ter;
}


// ======== Affichage du terrain ========

// void afficher_mur(int i, int j, Terrain *ter, int sprMur[COTE_CASE][COTE_CASE]){
    
//     for (int k = 0; k < COTE_CASE; k++){
//         for (int l = 0; l < 8; l++){
            
//             int x = ter->grille[i][j].x + l;    // position exacte du pixel à colorier
//             int y = ter->grille[i][j].y + k;
            
//             if (sprMur[k][l] == 1){
//                 BSP_LCD_DrawPixel(y, x, LCD_COLOR_DARKBLUE);
//             } else {
//                 BSP_LCD_DrawPixel(y, x, LCD_COLOR_BLACK);
//             }
            
//         }
//     }
    
// }

// void afficher_terrain(Terrain *ter, int lSprMurs[8][4][COTE_CASE][COTE_CASE]){ 
//     afficher_mur(3,0,ter,lSprMurs[4][0]);
//     afficher_mur(3,27,ter,lSprMurs[4][1]);
//     afficher_mur(33,0,ter,lSprMurs[4][3]);
//     afficher_mur(33,27,ter,lSprMurs[4][2]);
//     BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
//     BSP_LCD_FillRect(ter->grille[17][0].y,ter->grille[17][0].x,COTE_CASE,COTE_CASE);
//     BSP_LCD_FillRect(ter->grille[17][27].y,ter->grille[17][27].x,COTE_CASE,COTE_CASE);
//     for (int k=4; k<=11;k++){
//         afficher_mur(k,0,ter,lSprMurs[3][0]);
//         afficher_mur(k,27,ter,lSprMurs[3][2]);
//     }
//     afficher_mur(12,0,ter,lSprMurs[4][3]);
//     afficher_mur(12,27,ter,lSprMurs[4][2]);
    
//     afficher_mur(16,0,ter,lSprMurs[3][1]);
//     afficher_mur(16,27,ter,lSprMurs[3][1]);
    
//     afficher_mur(18,0,ter,lSprMurs[3][3]);
//     afficher_mur(18,27,ter,lSprMurs[3][3]);
    
//     afficher_mur(22,0,ter,lSprMurs[4][0]);
//     afficher_mur(22,27,ter,lSprMurs[4][1]);
    
//     for (int k=23; k<=26;k++){
//         afficher_mur(k,0,ter,lSprMurs[3][0]);
//         afficher_mur(k,27,ter,lSprMurs[3][2]);
//     }
//     afficher_mur(27,0,ter,lSprMurs[7][3]);
//     afficher_mur(27,27,ter,lSprMurs[6][2]);
    
//     afficher_mur(28,0,ter,lSprMurs[6][0]);
//     afficher_mur(28,27,ter,lSprMurs[7][1]);
    
//     for (int k=29; k<=32;k++){
//         afficher_mur(k,0,ter,lSprMurs[3][0]);
//         afficher_mur(k,27,ter,lSprMurs[3][2]);
//     }   
//     for (int i = 1; i < H_JEU-1 ; i++){
//         for (int j=1; j<L_JEU-1 ; j++){
            
//             if (ter->grille[i][j].type == 2){   // mur ?
            
//                 for (int d = 0; d < 4; d++){    // pour chaque direction
                    
//                     int a = vtsDeDir[d][0];
//                     int b = vtsDeDir[d][1];
                    
//                     int d1 = (d+1) % 4; // direction orthogonale obtenue par rotation de 90° à droite
//                     int a1 = vtsDeDir[d1][0];
//                     int b1 = vtsDeDir[d1][1];
                    
//                     if ((ter->grille[i+b][j+a].type==1)&&(ter->grille[i+b1][j+a1].type==2)&&(ter->grille[i-b][j-a].type==2)&&(ter->grille[i-b1][j-a1].type==2)){
//                         afficher_mur(i,j,ter,lSprMurs[0][d]);    
                
//                     } else if ((ter->grille[i+b][j+a].type==1)&&(ter->grille[i+b+b1][j+a+a1].type==1)&&(ter->grille[i+b1][j+a1].type==1)&&(ter->grille[i-b+b1][j-a+a1].type==1)&&(ter->grille[i-b][j-a].type==2)&&(ter->grille[i-b-b1][j-a-a1].type==2)&&(ter->grille[i-b1][j-a1].type==2)&&(ter->grille[i+b-b1][j+a-a1].type==1)){
//                         afficher_mur(i,j,ter,lSprMurs[1][d]);      
                
//                     } else if ((ter->grille[i+b][j+a].type==2)&&(ter->grille[i+b+b1][j+a+a1].type==1)&&(ter->grille[i+b1][j+a1].type==2)&&(ter->grille[i-b+b1][j-a+a1].type==2)&&(ter->grille[i-b][j-a].type==2)&&(ter->grille[i-b-b1][j-a-a1].type==2)&&(ter->grille[i-b1][j-a1].type==2)&&(ter->grille[i+b-b1][j+a-a1].type==2)){
//                         afficher_mur(i,j,ter,lSprMurs[2][d]);
                    
//                     } else if ((ter->grille[i+b][j+a].type==0)&&(ter->grille[i+b1][j+a1].type==2)&&(ter->grille[i-b][j-a].type==1)&&(ter->grille[i-b1][j-a1].type==2)){
//                         afficher_mur(i,j,ter,lSprMurs[3][d]);
                        
//                     } else if ((ter->grille[i+b][j+a].type==0)&&(ter->grille[i+b+b1][j+a+a1].type==0)&&(ter->grille[i+b1][j+a1].type==0)&&(ter->grille[i-b+b1][j-a+a1].type==0)&&(ter->grille[i-b][j-a].type==2)&&(ter->grille[i-b-b1][j-a-a1].type==1)&&(ter->grille[i-b1][j-a1].type==2)&&(ter->grille[i+b-b1][j+a-a1].type==0)){
//                         afficher_mur(i,j,ter,lSprMurs[4][d]);
                    
//                     } else if ((ter->grille[i+b][j+a].type==1)&&(ter->grille[i+b+b1][j+a+a1].type==1)&&(ter->grille[i+b1][j+a1].type==1)&&(ter->grille[i-b+b1][j-a+a1].type==1)&&(ter->grille[i-b][j-a].type==2)&&(ter->grille[i-b-b1][j-a-a1].type==0)&&(ter->grille[i-b1][j-a1].type==2)&&(ter->grille[i+b-b1][j+a-a1].type==1)){
//                         afficher_mur(i,j,ter,lSprMurs[5][d]); 
                        
//                     } else if ((ter->grille[i+b][j+a].type==0)&&(ter->grille[i+b+b1][j+a+a1].type==0)&&(ter->grille[i+b1][j+a1].type==2)&&(ter->grille[i-b+b1][j-a+a1].type==2)&&(ter->grille[i-b][j-a].type==2)&&(ter->grille[i-b-b1][j-a-a1].type==1)&&(ter->grille[i-b1][j-a1].type==2)&&(ter->grille[i+b-b1][j+a-a1].type==0)){
//                         afficher_mur(i,j,ter,lSprMurs[6][d]);         
                
//                     } else if ((ter->grille[i+b][j+a].type==2)&&(ter->grille[i+b+b1][j+a+a1].type==0)&&(ter->grille[i+b1][j+a1].type==0)&&(ter->grille[i-b+b1][j-a+a1].type==0)&&(ter->grille[i-b][j-a].type==2)&&(ter->grille[i-b-b1][j-a-a1].type==1)&&(ter->grille[i-b1][j-a1].type==2)&&(ter->grille[i+b-b1][j+a-a1].type==2)){
//                         afficher_mur(i,j,ter,lSprMurs[7][d]);
//                     }
//                 }
                
//             } else {
//                 if (ter->grille[i][j].type == 0) {  // vide ?
//                     BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
//                     BSP_LCD_FillRect(ter->grille[i][j].y,ter->grille[i][j].x,COTE_CASE,COTE_CASE);
//                 } else {    // ter->grille[i][j].type == 1
//                     BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
//                     BSP_LCD_FillRect(ter->grille[i][j].y,ter->grille[i][j].x,COTE_CASE,COTE_CASE);
                    
//                     if (ter->grille[i][j].contenu == 1) {
                        
//                         BSP_LCD_SetTextColor(LCD_COLOR_LIGHTYELLOW);
//                         int Cx = ter->grille[i][j].x+3;
//                         int Cy = ter->grille[i][j].y+3;
//                         BSP_LCD_FillRect(Cy,Cx,2,2);
                        
//                     } else if (ter->grille[i][j].contenu == 2) {
                        
//                         BSP_LCD_SetTextColor(LCD_COLOR_LIGHTYELLOW);
//                         int Cx = ter->grille[i][j].x+2;
//                         int Cy = ter->grille[i][j].y+2;
//                         BSP_LCD_FillRect(Cy,Cx,4,4);;
                        
//                     }
//                 }
//             }
            
//         }
//     }
// }


// ======== Pac-Man ========

PacMan init_pacMan(int x, int y, int dir){

    PacMan pacMan;

    // A partir des paramètres

    pacMan.x = x;
    pacMan.y = y;
    pacMan.dir = dir;

    // Par défaut

    pacMan.nvDir = -1;
    pacMan.score = 0;
    pacMan.nbVies = 3;

    return pacMan;
}

// void acquerir_nvDir(TS_StateTypeDef *tST, PacMan *pm, int jsX, int jsY){
//     BSP_TS_GetState(tST);
//     if (tST->touchDetected) {
        
//         uint16_t x = tST->touchX[0], y = tST->touchY[0];
        
        
//         if ((jsX < 5000) || (jsX  > 60000) || (jsY < 5000) || (jsY  > 60000)) {
//             if ((jsX < 5000) && (jsY >= 5000) && (jsY <= 60000)) {  // Droite
//                 pm->nvDir = 1;
//             }
//         }
//     } else {
    
        
//     }
    
// }

// void afficher_ctrl_pacMan(PacMan *pm){
    
//     if (pm->nvDir == 2) {
//         BSP_LCD_SetTextColor(LCD_COLOR_DARKCYAN);
//     } else {
//         BSP_LCD_SetTextColor(LCD_COLOR_LIGHTCYAN);
//     }
    
//     BSP_LCD_FillRect(373,158,44,44);    // Droite 2
    
//     if (pm->nvDir == 1) {
//         BSP_LCD_SetTextColor(LCD_COLOR_DARKCYAN);
//     } else {
//         BSP_LCD_SetTextColor(LCD_COLOR_LIGHTCYAN);
//     }
    
//     BSP_LCD_FillRect(329,114,44,44);    // Haut 1
    
//     if (pm->nvDir == 0) {
//         BSP_LCD_SetTextColor(LCD_COLOR_DARKCYAN);
//     } else {
//         BSP_LCD_SetTextColor(LCD_COLOR_LIGHTCYAN);
//     }
    
//     BSP_LCD_FillRect(373,70,44,44);     // Gauche 0
    
//     if (pm->nvDir == 3) {
//         BSP_LCD_SetTextColor(LCD_COLOR_DARKCYAN);
//     } else {
//         BSP_LCD_SetTextColor(LCD_COLOR_LIGHTCYAN);
//     }
    
//     BSP_LCD_FillRect(417,114,44,44);    // Bas 3
    
// }

void reperer_pacMan_terrain(PacMan *pm){    // Calculer i, j et k, l

    // i et j
    pm->i = (pm->y - POS_TERRAIN) / COTE_CASE;
    pm->j = (pm->x - POS_TERRAIN) / COTE_CASE;

    // k et l
    pm->k = (pm->y - POS_TERRAIN) % COTE_CASE;
    pm->l = (pm->x - POS_TERRAIN) % COTE_CASE;

}

void maj_pos_pacMan(PacMan *pm, Terrain *ter){  // à un instant t donné, mettre à jour la position de Pac-Man en fonction de dir, nvDir et des voisins de la case associée

    // ---- 1 mettre à jour dir au bon moment ----

    if (pm->nvDir != -1){   // une nouvelle direction renseignée ?

        Case *pCaseNvDir = ter->grille[pm->i][pm->j].voisins[pm->nvDir];    // par déf, pointeur NULL si la case voisine selon nvDir n'est pas autorisée

        if ( (((pm->nvDir % 2 == 0) && (pm->k == 4)) || ((pm->nvDir % 2 == 1) && (pm->l == 3))) && (pCaseNvDir != NULL) ){  // cf pseudo-code avec commentaires
            pm->dir = pm->nvDir;
            pm->nvDir = -1;     // réinitialisation de nvDir importante
        }
    }

    // ---- 2 Faire avancer d'un pixel ----

    if ((pm->nvDir == -1) || (pm->nvDir == pm->dir) || !(ter->grille[pm->i][pm->j].voisins[pm->nvDir])){    // 3ème teste si mur devant, selon nvDir

        Case *pCaseDir = ter->grille[pm->i][pm->j].voisins[pm->dir];    // pointeur de la case voisine selon dir <!>

        // cf pseudo-code avec commentaires
        if ((pCaseDir) || ((pm->dir == 0) && (pm->l > 3)) || ((pm->dir == 1) && (pm->k > 4)) || ((pm->dir == 2) && (pm->l < 3)) || ((pm->dir == 3) && (pm->k < 4))){

            pm->vX = vtsDeDir[pm->dir][0];
            pm->vY = vtsDeDir[pm->dir][1];

        } else {    // on ne peut plus avancer

            pm->vX = 0;
            pm->vY = 0;

        }
    } else {

        if (((pm->dir == 0) && (pm->l > 3)) || ((pm->dir == 1) && (pm->k > 4)) || ((pm->dir == 2) && (pm->l < 3)) || ((pm->dir == 3) && (pm->k < 4))){  // Pac-Man se dirige actuellement vers le centre de la case ?

            pm->vX = vtsDeDir[pm->nvDir][0] + vtsDeDir[pm->dir][0];
            pm->vY = vtsDeDir[pm->nvDir][1] + vtsDeDir[pm->dir][1];

        } else {    // sinon

            pm->vX = vtsDeDir[pm->nvDir][0] - vtsDeDir[pm->dir][0];
            pm->vY = vtsDeDir[pm->nvDir][1] - vtsDeDir[pm->dir][1];

        }
    }

    pm->x += pm->vX;    // mettre à jour x et y à partir de vX et vY
    pm->y += pm->vY;

    // ---- 2 bis S'occuper du cas de téléportation ----
    if ((pm->x==34) && (pm->y==164) && (pm->dir==0)){
        pm->x=230;
    } else if ((pm->x==243) && (pm->y==164) && (pm->dir==2)){
        pm->x=40;
    }
    // ---- 3 Calculer de nouveau i, j, k et l ----
    reperer_pacMan_terrain(pm);
}

// void afficher_pacMan(PacMan *pm, int lSprPM[4][COTE_ACTEUR][COTE_ACTEUR]){
//     for (int u = 0; u < COTE_ACTEUR; u++) {
//         for (int v = 0; v < COTE_ACTEUR; v++) {
            
//             if (lSprPM[pm->dir][u][v] == 1) {
//                 BSP_LCD_DrawPixel(pm->y - 7 + v,pm->x - 7 + u,paletteSprites[1]);
//             }
//         }
//     }
// }

void manger(PacMan *pm, Terrain *ter){
    int i = pm->i;
    int j = pm->j;
    if (pm->k==4 && pm->l==3){
        if (ter->grille[i][j].contenu==1){
            ter->grille[i][j].contenu=0;
        }else if (ter->grille[i][j].contenu==2){
            ter->grille[i][j].contenu=0;
        }
    }
}

void calculer_score(PacMan *pm, Terrain *ter){
        int BONBON = 0;
        int ENERGISANT = 0;
        for (int a=0 ; a<H_JEU ; a++){
            for (int b=0; b<L_JEU ; b++){
                if (ter->grille[a][b].contenu==1){
                    BONBON += 1;
                } else if (ter->grille[a][b].contenu==2){
                    ENERGISANT += 1;
                }
            }
       } 
        pm->score= (242-BONBON)*100 + 400*(4-ENERGISANT);
}


// ======== Maison ========

Maison init_maison(tab4C xySMsn, int tmpLim){

    Maison msn;


    // ---- Initialisation du graphe ----

    for (int i=0; i < 4; i++){

        msn.graphe[i].s = i;    // Initialisation de l'indice du sommet i

        msn.graphe[i].x = xySMsn[i][0]; // Initialisation des coordonnées du sommet i
        msn.graphe[i].y = xySMsn[i][1];

    }

    // Sommets suivants (le sommet 0 n'a pas de sommet suivant)
    msn.graphe[1].suivS = &msn.graphe[0];   // 1 -> 0
    msn.graphe[2].suivS = &msn.graphe[1];   // 2 -> 1
    msn.graphe[3].suivS = &msn.graphe[1];   // 3 -> 1

    // Directions des fantômes à partir de chaque sommet
    msn.graphe[0].dirF = 0; // gauche (à la sortie de la maison)
    msn.graphe[1].dirF = 1; // haut
    msn.graphe[2].dirF = 2; // droite
    msn.graphe[3].dirF = 0; // gauche


    // ---- Initialisation du reste ----

    for (int i=0; i < 4; i++){  // tous les monstres sont en attente (dans la maison)
        msn.ftmEnAttente[i] = 1;
    }

    msn.typeCUtil = 0;  // On commence avec des compteurs individuels
    msn.ftmFav = 0; // Blinky part en premier
    msn.cntPtsG = 0;

    msn.tempsEcoule = 0;    // Temps écoulé depuis le dernier point mangé par Pac-Man
    msn.tempsLimite = tmpLim;

    return msn;
}


// ======== Fantômes ========

Fantome init_fantome(int f, tab4C xySMsn, tab4C ijCCblSD, int sRepos, tab4I lCsPtsI){

    Fantome ftm;

    ftm.f = f;  // indice du fantôme

    ftm.x = xySMsn[f][0];   // position initale du fantôme
    ftm.y = xySMsn[f][1];
    
    ftm.dir = 1;

    ftm.mode = 0;   // mode initial du fantôme : SE DISPERSER
    ftm.estMort = 0;
    ftm.estDansMaison = 1;

    ftm.cISD = ijCCblSD[f][0];  // case cible en mode SE DISPERSER (initialisé une bonne fois pour toute)
    ftm.cJSD = ijCCblSD[f][1];

    ftm.cIC = ftm.cISD; // car mode initial du fantôme : SE DISPERSER
    ftm.cJC = ftm.cJSD;

    ftm.estEnAttente = 1;   // en attente d'ordre de départ

    ftm.sAct = f;   // sAct = indice du sommet (initialement)
    ftm.sRepos = sRepos;    // le même que sAct initialement sauf pour Blinky

    ftm.cntPts = 0; // Aucun point mangé par Pac-Man
    ftm.limCntPoints = lCsPtsI[f];

    ftm.chgtModeExt = 0;    // pas de changement de mode

    return ftm;

}

void aut_dep_fantomes(Maison *msn, Fantome *ftms){  // Un tableau est considéré comme un pointeur

    if (msn->ftmEnAttente[0] == 1){ // Blinky est en attente

        ftms[0].estEnAttente = 0;   // Blinky n'est plus en attente
        msn->ftmEnAttente[0] = 0;   // Enlever Blinky des fantômes en attente

    }

    // ---- Déterminer le favori parmi les fantômes en attente ----

    int iFav = 1;   // indice du favori (Blinky immédiatement exclus)

    while ((iFav < 4) && (msn->ftmEnAttente[iFav] == 0)) {  // iFav : 4 (aucun en attente)
        iFav += 1;
    }

    // ---- Autoriser le départ d'un fantôme si les conditions pour sont réunies ----

    if (iFav < 4) { // au moins un fantôme en attente (sinon rien à faire)

        if (msn->typeCUtil == 0){   // compteurs individuels utilisés

            if (ftms[iFav].cntPts == ftms[iFav].limCntPoints){  // limite atteinte pour le favori
                ftms[iFav].estEnAttente = 0;
                msn->ftmEnAttente[iFav] = 0;
            }

        } else {    // compteur global utilisé et a priori tous les fantômes en attente

            if (msn->cntPtsG == 7){ // Pinky
                ftms[1].estEnAttente = 0;
                msn->ftmEnAttente[1] = 0;
            }

            if (msn->cntPtsG == 17){    // Inky
                ftms[2].estEnAttente = 0;
                msn->ftmEnAttente[2] = 0;
            }

            if (msn->cntPtsG == 32){    // retour aux compteurs individuels
                msn->typeCUtil = 0;
            }

        }

        if (msn->tempsEcoule == msn->tempsLimite){  // Pac-Man n'a rien mangé depuis un moment => forcer le départ du fantôme favori

            ftms[iFav].estEnAttente = 0;
            msn->ftmEnAttente[iFav] = 0;

            msn->tempsEcoule = 0;   // reinitialisation du tempsEcoule
        }
    }

    // ---- Incrémenter de 1 le temps écoulé depuis le dernier point mangé ----

    msn->tempsEcoule += 1;

}

void sortir_fantomes_maison(Maison *msn, Fantome *ftms){

    for (int i = 0; i < 2; i++){    // Pour chaque fantôme      /// TEMPORAIRE POUR ELIMINER LE BUG
        if ((ftms[i].estDansMaison == 1) && (ftms[i].estEnAttente == 0) && (ftms[i].estMort == 0)){ // ce fantôme à faire sortir ?

            if (ftms[i].sAct == 0){ // déjà arrivée à la sortie (sommet 0 de la maison)

                ftms[i].estDansMaison = 0;

                if (ftms[i].chgtModeExt == 1){  // voit un changement de mode en étant dans la maison
                    ftms[i].dir = 3;    // droite
                    ftms[i].dirEnAtt = 3;
                    ftms[i].nvDir = 3;
                } else {
                    ftms[i].dir = 0;    // gauche
                    ftms[i].dirEnAtt = 0;
                    ftms[i].nvDir = 0;
                }

            } else {

                if ( (ftms[i].x == msn->graphe[ftms[i].sAct].x) && (ftms[i].y == msn->graphe[ftms[i].sAct].y) ){    // le fantôme exactement sur le sommet actuel ?

                    ftms[i].sSuiv = msn->graphe[ftms[i].sAct].suivS->s; // indice du sommet suivant mis à jour
                    ftms[i].dir = msn->graphe[ftms[i].sAct].dirF;   // nouvelle direction à partir de ce sommet actuel

                }

                ftms[i].x += vtsDeDir[ftms[i].dir][0];  // Faire avancer le fantôme considéré dans la direction dir
                ftms[i].y += vtsDeDir[ftms[i].dir][1];

                if ((ftms[i].x == msn->graphe[ftms[i].sSuiv].x) && (ftms[i].y == msn->graphe[ftms[i].sSuiv].y) ) {  // exactement sur le sommet suivant ?
                    ftms[i].sAct = ftms[i].sSuiv;   // Mis à jour de sAct
                }

            }
        }
    }

}

void reperer_fantomes_terrain(Fantome *ftms){   // Calculer i, j, k et l

    for (int f = 0; f < 4; f++){
        if (ftms[f].estDansMaison == 0){    // n'est plus dans la maison

            // i et j
            ftms[f].i = (ftms[f].y - POS_TERRAIN) / COTE_CASE;
            ftms[f].j = (ftms[f].x - POS_TERRAIN) / COTE_CASE;

            // k et l
            ftms[f].k = (ftms[f].y - POS_TERRAIN) % COTE_CASE;
            ftms[f].l = (ftms[f].x - POS_TERRAIN) % COTE_CASE;

        }
    }

}

int det_nvDir_ftm(Fantome *ftm, Terrain *ter, PacMan *pm){  // fantôme à l'extérieur de la maison

    //  ATTENTION : inverser 0 et 1 car i et j et non pas x et y
    Case *caseSuiv = &ter->grille[ftm->i + vtsDeDir[ftm->dirEnAtt][1]][ftm->j + vtsDeDir[ftm->dirEnAtt][0]];    // pointeur de la case suivante selon dirEnAtt

    if (ftm->mode == 0){    // mode : SE DISPERSER

        Case *caseCible = &ter->grille[ftm->cISD][ftm->cJSD];   // pointeur de la case cible

        int nvDir = -1;
        int distMin = 9999; // très grand

        for (int d = 0; d < 4; d++){    // nouvelle direction potentielle
            if (d != nvDirInterdite[ftm->dirEnAtt]){    // d n'est pas la direction opposée de dirEnAtt

                if (caseSuiv->voisins[d] != NULL){  // pas de mur selon d à partir de la case suivante

                    Case *candidat = caseSuiv->voisins[d];

                    int cibleCX = caseCible->x + (COTE_CASE / 2), cibleCY = caseCible->y + (COTE_CASE / 2); // x,y du centre de la case cible
                    int candidatCX = candidat->x + (COTE_CASE / 2), candidatCY = candidat->y + (COTE_CASE / 2); // x,y du centre du candidat

                    int dist = calc_distEuc(cibleCX, cibleCY, candidatCX, candidatCY);  // distance euclidienne entre le candidat et la case cible

                    if (dist < distMin){    // candidat plus proche de la case cible
                        nvDir = d;
                        distMin = dist;
                    } else {
                        if (dist == distMin) {  // candidat à la même distance de la case cible

                            int c = 0;
                            while ((c < 4) && !(prefNvDir[c] == d) && !(prefNvDir[c] == nvDir)){
                                c += 1;
                            }

                            if (c==4){      // mesure de sécurité
                                printf("--------------------------- ERREUR! ---------------------------");
                            }

                            if (prefNvDir[c] == d){ // Si d est apparu avant nvDir, alors cette direction est préférée
                                nvDir = d;
                                // distMin reste le même
                            }

                        }
                    }
                }
            }
        }

        return nvDir;

    }


    return -1;
}

void maj_pos_fantomes(Fantome *ftms, Terrain *ter, PacMan *pm, tab4C klDCD){

    reperer_fantomes_terrain(ftms); // mettre à jour les paramètres i, j, k et l des fantômes

    for (int f = 0; f < 2; f++){    ///// TEMPORAIRE POUR ELIMINER LE BUG
        if (ftms[f].estDansMaison == 0){    // n'est plus dans maison

            if ((ftms[f].k == 5) && (ftms[f].l == 4)){  // le fantôme est au centre de la case
                ftms[f].dir = ftms[f].dirEnAtt;
            }

            if ((ftms[f].k == klDCD[ftms[f].dir][0]) && (ftms[f].l == klDCD[ftms[f].dir][1])){  // le fantôme est au début de la case (dépend de dir)
                ftms[f].dirEnAtt = ftms[f].nvDir;
                ftms[f].nvDir = det_nvDir_ftm(&(ftms[f]), ter, pm); // déterminée en avance
            }

            ftms[f].x += vtsDeDir[ftms[f].dir][0];  // Faire avancer le fantôme considéré dans la direction dir
            ftms[f].y += vtsDeDir[ftms[f].dir][1];

        }
    }

}

// void afficher_fantomes(Fantome *ftms, int lSprFtms[4][4][COTE_ACTEUR][COTE_ACTEUR]){
    
//     for (int f = 0; f < 4; f++){    // pour chaque fantôme
//         for (int u = 0; u < COTE_ACTEUR; u++) {
//             for (int v = 0; v < COTE_ACTEUR; v++) {
                
//                 if (lSprFtms[f][ftms[f].dir][u][v] != 0) {
//                     BSP_LCD_DrawPixel(ftms[f].y - 7 + v, ftms[f].x - 7 + u, paletteSprites[ lSprFtms[f][ftms[f].dir][u][v] ]);
//                 }
//             }
//         }
//     }
// }


// ======== Intéractions entre Pac-Man et Fantômes ========

void detecter_collision(PacMan *pm, Fantome *ftms, tab4C xySMsn, tab4C ijCCblSD, tab4I lCsPtsI){
    for (int f = 0; f < 4; f++){    // pour chaque fantôme
        if ( (pm->i == ftms[f].i) && (pm->j == ftms[f].j) ) {   // même case ?
        
            if (pm->nbVies == 0) {
                
                ///////////////////////////////////////////////////
                
            } else {
                pm->nbVies = pm->nbVies - 1;
                
            }
            
            PacMan pMO = init_pacMan(PM_PIN_X, PM_PIN_Y, 0);
            pm = &(pMO);
            reperer_pacMan_terrain(pm);
            
            ftms[0] = init_fantome(0, xySMsn, ijCCblSD, 1, lCsPtsI);  // Blinky
            ftms[1] = init_fantome(1, xySMsn, ijCCblSD, 1, lCsPtsI);  // Pinky
            ftms[2] = init_fantome(2, xySMsn, ijCCblSD, 2, lCsPtsI);  // Inky
            ftms[3] = init_fantome(3, xySMsn, ijCCblSD, 3, lCsPtsI);  // Clyde
            
            
            
        }
    }
}


