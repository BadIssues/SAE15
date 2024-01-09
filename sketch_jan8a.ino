#include "carMotors.h"
#include "DeviceDriverSet_xxx0.h"

DeviceDriverSet_ITR20001 AppITR20001;
CarMotors engine;

// Définir la constante SEUIL en fonction de la plage de valeurs de vos capteurs
const int SEUIL = 500; // Remplacez cette valeur par celle appropriée
int direction = 0;

void setup() {
  Serial.begin(9600);
  AppITR20001.DeviceDriverSet_ITR20001_Init();
  engine.init(255);
}

void loop() {
  static unsigned long print_time = 0;
  if (millis() - print_time > 500) {
    print_time = millis();
    Serial.print("ITR20001_getAnaloguexxx_L=");
    Serial.println(AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_L());
    Serial.print("ITR20001_getAnaloguexxx_M=");
    Serial.println(AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_M());
    Serial.print("ITR20001_getAnaloguexxx_R=");
    Serial.println(AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_R());
  }

  int analogValueL = AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_L();
  int analogValueM = AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_M();
  int analogValueR = AppITR20001.DeviceDriverSet_ITR20001_getAnaloguexxx_R();

  // Ajoutez des conditions pour ajuster la direction en fonction des lectures des capteurs
  if (analogValueM > SEUIL) {
    // La ligne est centrée, avancer
    engine.setSpeed(255);
    engine.goForward();
    direction = 0;

  } else if (analogValueL > SEUIL) {
    // La ligne est à gauche, tourner à gauche
    engine.setSpeed(150);
    engine.drive(1, 0.5);

    direction = 1;
  } else if (analogValueR > SEUIL) {
    // La ligne est à droite, tourner à droite
    engine.setSpeed(150);
    engine.drive(0.5, 1);

    direction = 2;
  } else if (analogValueM <= SEUIL && analogValueL <= SEUIL && analogValueR <= SEUIL) {
    // Aucun capteur ne détecte la ligne, arrêter le moteur ou effectuer une action appropriée
    switch(direction) {
      case 0:
      /*engine.setSpeed(150);
      engine.goBackward();
      delay(25);
      break;*/
      case 1:
      engine.setSpeed(180);
      engine.turnLeft();
      delay(25);
      break;
      case 2:
      engine.setSpeed(180);
      engine.turnRight();
      delay(25);
      break;
    } 
  }
}