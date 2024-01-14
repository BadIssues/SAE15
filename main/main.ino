#include "carMotors.h"            // Bibliothèque permettant la gestion des moteurs
#include "DeviceDriverSet_xxx0.h" // Bibliothèque permettant la gestion des capteurs
#include "FastLED.h"              // Bibliothèque permettant la gestion des LEDs

#define PIN_RBGLED 4              // Définition d'une constante du pin de la led
#define NUM_LEDS 1                // Définition d'une constante du nombre de leds
#define NCOLORS 4                 // Définition d'une constante du nombre de couleurs
CRGB leds[NUM_LEDS];              // Instancier l'objet 

uint32_t tabColors[NCOLORS];      // Instancier l'objet 

uint32_t color(uint8_t r, uint8_t g, uint8_t b) {         // Permet de convertir décimal en binaire 
  return (((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}
DeviceDriverSet_ITR20001 AppITR20001; // Instancier l'objet AppITR20001 pour les capteurs
CarMotors engine;                     // Instancier l'objet engine pour les moteurs

// Définir la constante SEUIL en fonction de la plage de valeurs de vos capteurs
const int SEUIL = 700; 
int direction = 0;                    // Mémoire pour sauvegarder la précédente direction
int croisementPasse = 0;              // Mémoire pour incrémenter le nombre de croisement rencontré
bool croisementTraite = false;        // Mémoire pour détecter un changement d'état

void setup() {
  AppITR20001.DeviceDriverSet_ITR20001_Init();                                   // Appelle au constructeur de la classe pour l'objet pour l'initier
  engine.init(240);                                                              // Appelle au constructeur de la classe pour l'objet pour l'initier
  FastLED.addLeds<NEOPIXEL, PIN_RBGLED>(leds, NUM_LEDS);                         // 
  FastLED.setBrightness(10);                                                     //Définit la "puissance" d'éclairement a 10%
  tabColors[0] = color(255,0,0);                                                 //Définit la couleur rouge pour les tours
  tabColors[1] = color(0,255,0);                                                 //Définit la couleur vert pour les tours
  tabColors[2] = color(0,0,255);                                                 //Définit la couleur bleue pour les tours
  tabColors[3] = color(255,215,0);                                               //Définit la couleur jaune pour les tours
  FastLED.showColor(tabColors[0]);                                               //Change la couleur de la LED a rouge 
  delay(3000);                                                                   //Fait patienter l'éxecution pendant 3secondes avant le démmarage
  FastLED.showColor(tabColors[1]);                                               //Change la couleur de la LED à vert pour signaler le départ 
}

void DetectionCroisement(int analogValueL, int analogValueM,int analogValueR)    //Fonction permettant de détecter les croisements et d'incrémenter 
{
  if (analogValueL >= SEUIL && analogValueM >= SEUIL && analogValueR >= SEUIL) { // Condition pour détecter un croisement tel que les 3 capteurs capte les bandes noirs
    if (!croisementTraite) {                                                     // Cette condition vérifie si le croisement a déjà été traité. Si non, alors le bloc de code à l'intérieur sera exécuté.                                                        
      croisementPasse++;                                                         // Incrémentation du nombre de tours
      croisementTraite = true;                                                   // Passe la variable croisementTraite à vrai pour éviter une répétition
      GestionTour();                                                             // Appeller la fonction GestionTour afin de mettre a jour les LEDs
    }
  } else {
    croisementTraite = false;                                                    // Si les valeurs des capteurs ne dépassent pas le seuil, cela signifie que les capteurs ne détectent pas une bande noire. Dans ce cas, marquer le croisement comme non traité pour permettre une nouvelle détection lorsqu'une bande noire sera à nouveau détectée.
  }
}

void GestionMotorisation(int analogValueL,int analogValueM,int analogValueR)    // Fonction permettant de gérer les moteurs en fonction des entrées des 3 capteurs
{
  float leftDrive = 0;                                                          // Initier un coefficient pour les roues gauche à chaque appel de la fonction 
  float rightDrive = 0;                                                         // Initier un coefficient pour les roues droite à chaque appel de la fonction 

  if (analogValueM > SEUIL) {                                                   // Capteur du milieu capte la ligne donc La ligne est centrée, avancer
    leftDrive = 1;                                                              // Coefficient moteur gauche à 1 : 100% de la vitesse
    rightDrive = 1;                                                             // Coefficient moteur droite à 1 : 100% de la vitesse
  } else if (analogValueL > SEUIL) {                                            // Capteur de gauche capte la ligne donc La ligne est à gauche, tourner à gauche
    leftDrive = 0.9;                                                            // Coefficient moteur gauche à 0.9 : 90% de la vitesse
    rightDrive = 0.5;                                                           // Coefficient moteur droite à 0.5 : 50% de la vitesse
    direction = 1;                                                              // Met la mémoire à gauche pour le système de reprise

  } else if (analogValueR > SEUIL) {                                            // Capteur de droite capte la ligne donc La ligne est à droite, tourner à droite
    leftDrive = 0.5;                                                            // Coefficient moteur gauche à 0.5 : 50% de la vitesse
    rightDrive = 0.9;                                                           // Coefficient moteur droite à 0.9 : 90% de la vitesse
    direction = 2;                                                              // Met la mémoire à droite pour le système de reprise

  } else if (analogValueL > SEUIL && analogValueM > SEUIL) {                    // Capteur de gauche ainsi que celle du milieu capte la ligne donc La ligne est légérement  à gauche, tourner à gauche
    leftDrive = 1;                                                              // Coefficient moteur gauche à 1 : 100% de la vitesse
    rightDrive = 0.9;                                                           // Coefficient moteur droite à 0.9 : 90% de la vitesse
    direction = 1;                                                              // Met la mémoire à gauche pour le système de reprise

  } else if (analogValueR > SEUIL && analogValueM > SEUIL) {                    // Capteur de droite ainsi que celle du milieu capte la ligne donc La ligne est légérement  à droite, tourner à droite
    leftDrive = 0.9;                                                            // Coefficient moteur gauche à 0.9 : 90% de la vitesse
    rightDrive = 1;                                                             // Coefficient moteur droite à 1 : 100% de la vitesse
    direction = 2;                                                              // Met la mémoire à droite pour le système de reprise

  } else if (analogValueM <= SEUIL && analogValueL <= SEUIL && analogValueR <= SEUIL) { // Système de reprise : permet de continuer à prendre l'ancienne trajectoire dans le cas ou la ligne est perdu
    switch (direction) {
      case 1:
        // La ligne est à gauche, tourner à gauche. Ci-dessous les coefficients pour la reprise
        leftDrive = 0.9;
        rightDrive = 0.1;
        break;
      case 2:
        // La ligne est à droite, tourner à droite. Ci-dessous les coefficients pour la reprise
        leftDrive = 0.1;
        rightDrive = 0.9;
        break;
    }
  }

  // Appliquer nos coefficients de conduite calculés
  engine.drive(leftDrive, rightDrive);
}

void GestionTour()                                                                //Fonction permettant de choisir la couleur de la LED en fonction du nombre de croisementPasse/2 réalisé
{
  switch(croisementPasse)                                                         //Switch permettant de choisir un cas en fonction de notre variable croisementPasse
  {
    case 1 :                                                                      
      FastLED.showColor(tabColors[1]);                                            //Choix de la couleur 2 dans notre tableau vert
      break;
    case 2 :
      FastLED.showColor(tabColors[2]);                                            //Choix de la couleur 3 dans notre tableau bleu
      break;
    case 4 :
      FastLED.showColor(tabColors[3]);                                            //Choix de la couleur 4 dans notre tableau jaune
      break;
    case 6 :
      FastLED.showColor(tabColors[0]);                                            //Choix de la couleur 1 dans notre tableau rouge
      break;
  }
}


void loop() 
{
  if (croisementPasse < 6) //Condition permettant de réaliser notre parcours quand nombre de croisement réalisé est en dessous de 6
  {
    GestionMotorisation(AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_L(),AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_M(),AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_R());
    DetectionCroisement(AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_L(),AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_M(),AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_R());
  }else if (croisementPasse == 6){ //Condition permettant de reculer lors du dépassement du croisement = à 6 et recule jusqu'a revenir sur la ligne
    engine.setSpeed(50);
    engine.goBackward();
    DetectionCroisement(AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_L(),AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_M(),AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_R());
  } else if (croisementPasse>6){
    engine.stop();
  }
}