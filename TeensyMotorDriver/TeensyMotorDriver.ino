// ---------------------------------------------------------------- //
// Author : @NoWayCall
// Using Arduino IDE 2.0.1
// Last tested on 20-12-2022
// ---------------------------------------------------------------- //
//                         --- NOTES ---
/*
 * The target position is set to X steps relative to the
 * current position. The move command of the controller 
 * moves the motor to the target position.  
*/
// ---------------------------------------------------------------- //

// ----- Include -----
#include "TeensyStep.h"
#include <math.h>
#include <Wire.h>

// ----- Statics -----

Stepper MoteurDroit(36, 35);   // STEP at pin 36, DIR at pin 35
Stepper MoteurGauche(23, 22);  // STEP at pin 23, DIR at pin 22

StepControl controller;  // Use default settings

//Valeurs physiques des composants (utilisées pour les calculs de steps)
float DiamRoue = 88.65;   //Diamètre de la roue en mm (ici 88.6mm)
float RapportReduc = 3;   //Rapport de réduction Moteur-Roue (ici 1:3)
float Microstep = 16;     //Microstepping paramétré sur le driver (ici 1/16)
float StepperRes = 1.8;   //Résolution du moteur (ici 1.8° par Step en FullStep)
float DistRoues = 322.5;  //Distance entre les deux deux roues (largeur du robot) permettant de connaitre le diamètre du cercle de rotation du robot
float FactCorr = 1;       // on fait une rotation de 360° puis on mesure XXX° et on fait le rapport XXX/360 = FactCorr puis essai-erreur pour ajuster
//float My_PI = 3.1415926535897932384626433832795; // définition de PI (normalement dans les librairies de bases)

struct StepsMoteurs {
  int StepsMoteurGauche;
  int StepsMoteurDroit;
};

// ########### SETUP ############

void setup() {
  // Setup communication
  Wire2.begin(4);                 // join i2c bus with address #4
  Wire2.onReceive(receiveEvent);  // register event
  Serial.begin(9600);            // start serial for output
  // Setup Moteurs Teensystep lib                                
  MoteurGauche
    .setMaxSpeed(25000)         // stp/s      valeur max:  25000 @12V ~  64000 @24V
    .setAcceleration(30000)     // stp/s^2    valeur max:  50000 @12V ~ 150000 @24V
    .setInverseRotation(true);  // Inversion de la rotation
  MoteurDroit
    .setMaxSpeed(25000)       // stp/s      valeur max:  25000 @12V ~  64000 @24V
    .setAcceleration(30000);  // stp/s^2    valeur max:  50000 @12V ~ 150000 @24V

  delay(1000); //Délai de début nécessaire pour laisser le temps au drivers moteurs de tirer le courant 
}


// ########### LOOP ############

void loop() {
  //WAIT

  //---------

  Demo();
  delay(15000);

  //---------

  // struct StepsMoteurs StepsMouvement; //déclaration locale de la structure de mouvements
  // StepsMouvement = DegreeToStep(90) ; //calculs de steps

  // MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);  // Set target position
  // MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);  // Set target position
  // controller.move(MoteurGauche,MoteurDroit);    // Do the move

  //   StepsMouvement = MilliToStep(100) ; //calculs de steps
  // MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);  // Set target position
  // MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);  // Set target position
  // controller.move(MoteurGauche,MoteurDroit);    // Do the move

  //---------
}

// ########### CLASSES ############

//Conversion de mm de déplacements en nombre de steps
struct StepsMoteurs MilliToStep(int Milli) {

  struct StepsMoteurs StepsACalculer;  //déclaration locale de la structure à renvoyer

  float StepsPourUnTourDeRoue = (360.00 / StepperRes) * Microstep * RapportReduc;
  float PerimetreDeRoue = M_PI * DiamRoue;  //My_PI si ca marche pas (lib math)
  float StepsPourUnMm = (360.00 / PerimetreDeRoue) * (StepsPourUnTourDeRoue / 360.00);
  int Steps = round(Milli * StepsPourUnMm);
  //On peut compacter tout en une ligne mais c'est plus clair ainsi.

  StepsACalculer.StepsMoteurGauche = Steps;
  StepsACalculer.StepsMoteurDroit = Steps;

  return StepsACalculer;
}

//Conversion d'angles en degrés en nombre de steps
struct StepsMoteurs DegreeToStep(int Degree) {

  struct StepsMoteurs StepsACalculer;  //déclaration locale de la structure à renvoyer
  //Facteur de correction ci dessous car problème de précision parfois sur les rotations
  float LongueurArc = (DistRoues * FactCorr * M_PI * Degree) / 360.00;  //My_PI si ca marche pas (lib math) %%%% facteur de 1.01 de correction
  //(C'est le même code que MilliToStep remis en une ligne)
  float StepsPourUnMm = ((360.00 / StepperRes) * Microstep * RapportReduc) / (M_PI * DiamRoue);  //My_PI si ca marche pas (lib math)

  int Steps = round(LongueurArc * StepsPourUnMm);
  //C'est le même code que MilliToStep


  StepsACalculer.StepsMoteurGauche = Steps;
  StepsACalculer.StepsMoteurDroit = -Steps;

  return StepsACalculer;
}

// Fonction qui s'exécute à chaque fois que des données sont reçues du maître.
// cette fonction est enregistrée comme un événement, voir setup()
void receiveEvent (int howMany)
 {
if(howMany == 2)
  {
    uint16_t DataReceived  = Wire2.read() << 8; 
    DataReceived |= Wire2.read();
    //Serial.println(DataReceived);
    DataToAction(DataReceived);
  }
    else
  {
    Serial.print("Unexpected number of bytes received: ");
    Serial.println(howMany);
  }

 }  

void DataToAction (int Data) {
  struct StepsMoteurs StepsMouvement; //déclaration locale de la structure de mouvements  

  if(Data < 10000)
  {
    //nothing
    Serial.println("Unexpected Value");
  }
  else if(Data >= 10000 && Data < 20000) //Avancer
  {
    Serial.println("Avancer");
    Data = Data - 10000;
    Serial.println(Data); 
    //evenementiel fonctionne pas?
    StepsMouvement = MilliToStep(Data) ; //calculs de steps
    MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);  // Set target position
    MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);  // Set target position
    controller.move(MoteurGauche,MoteurDroit);    // Do the move
  }
  else if(Data >= 20000 && Data < 30000) //Reculer
  {
    Serial.println("Reculer");    
    Data = -(Data - 20000); 
    Serial.println(Data);
    StepsMouvement = MilliToStep(Data) ; //calculs de steps
    MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);  // Set target position
    MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);  // Set target position
    controller.move(MoteurGauche,MoteurDroit);    // Do the move
  }
  else if(Data >= 30000 && Data < 40000) // Rotation Horaire
  {
    Data = Data - 30000; 
    StepsMouvement = MilliToStep(Data) ; //calculs de steps
    MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);  // Set target position
    MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);  // Set target position
    controller.move(MoteurGauche,MoteurDroit);    // Do the move
  }
  else if(Data >= 40000 && Data < 50000) // Rotation Anti-Horaire
  {
    Data = -(Data - 40000); 
    StepsMouvement = MilliToStep(Data) ; //calculs de steps
    MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);  // Set target position
    MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);  // Set target position
    controller.move(MoteurGauche,MoteurDroit);    // Do the move
  }  
  else if(Data >= 50000 && Data < 60000) // STOP
  {
    void stop();
  }  
  else if(Data >= 60000) // EMERGENCYSTOP
  {
    void emergencyStop();
  }  
}


// ########### DEMO ############
void Demo() {
  struct StepsMoteurs StepsMouvement;  //déclaration locale de la structure de mouvements
                                       //--
  StepsMouvement = MilliToStep(500);
  MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);
  MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);
  controller.move(MoteurGauche, MoteurDroit);  // Do the move
  delay(200);
  //--
  StepsMouvement = DegreeToStep(-90);
  MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);
  MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);
  controller.move(MoteurGauche, MoteurDroit);  // Do the move
  delay(200);
  //--
  StepsMouvement = MilliToStep(750);
  MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);
  MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);
  controller.move(MoteurGauche, MoteurDroit);  // Do the move
  delay(200);
  //--
  StepsMouvement = DegreeToStep(-90);
  MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);
  MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);
  controller.move(MoteurGauche, MoteurDroit);  // Do the move
  delay(200);
  //--
  StepsMouvement = MilliToStep(400);
  MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);
  MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);
  controller.move(MoteurGauche, MoteurDroit);  // Do the move
  delay(200);
  //--
  StepsMouvement = MilliToStep(-400);
  MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);
  MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);
  controller.move(MoteurGauche, MoteurDroit);  // Do the move
  delay(200);
  //--
  StepsMouvement = DegreeToStep(-90);
  MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);
  MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);
  controller.move(MoteurGauche, MoteurDroit);  // Do the move
  delay(200);
  //--
  StepsMouvement = MilliToStep(750);
  MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);
  MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);
  controller.move(MoteurGauche, MoteurDroit);  // Do the move
  delay(200);
  //--
  StepsMouvement = DegreeToStep(270);
  MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);
  MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);
  controller.move(MoteurGauche, MoteurDroit);  // Do the move
  delay(200);
  //--
  StepsMouvement = MilliToStep(-500);
  MoteurGauche.setTargetRel(StepsMouvement.StepsMoteurGauche);
  MoteurDroit.setTargetRel(StepsMouvement.StepsMoteurDroit);
  controller.move(MoteurGauche, MoteurDroit);  // Do the move
  delay(200);
  //--
}
