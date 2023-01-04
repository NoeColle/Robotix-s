
// ---------------------------------------------------------------- //
// Author : @NoWayCall
// Using Arduino IDE 2.0.1
// Last tested on 20-12-2022
// ---------------------------------------------------------------- //


#include <Wire.h>

void setup()
{
  Serial.begin(9600);
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.println("=======================================================================================");
  Serial.println("                                                              |_| ");
  Serial.println("                 _____  _____ _____ ____        _            (* *)");
  Serial.println("                |_   _|/ ____|_   _|  _ \\      | |          __)#(__");
  Serial.println("                  | | | (___   | | | |_) | ___ | |_        ( )...( )(_)");
  Serial.println("                  | |  \\___ \\  | | |  _ < / _ \\| __|       || |_| ||//");
  Serial.println("                 _| |_ ____) |_| |_| |_) | (_) | |_     >==() | | ()/");
  Serial.println("                |_____|_____/|_____|____/ \\___/ \\__|        _(___)_");
  Serial.println("                                                           [-]   [-]");
  Serial.println("");
  Serial.println(" @NoWayCall                                                                   20/12/22");
  Serial.println("=======================================================================================");
  Serial.println("10000 = Avancer / 20000 = Reculer");
  Serial.println("30000 = Rotation Horaire / 40000 = Rotation Anti-Horaire");
  Serial.println("50000 = STOP / 60000 = EMERGENCYSTOP");
  Serial.println("---------------------------------------------------------------------------------------");
  Serial.println("Remarques: ");
  Serial.println("");
  Serial.println("STOP : Décélère les moteurs et attend que les moteurs soient arrêtés (fct. blocante).");
  Serial.println("");
  Serial.println("EMERGENCYSTOP : Arrête immédiatement les moteurs.");
  Serial.println("N'utilisez cette fonction qu'en cas d'urgence car, selon la vitesse du moteur,");
  Serial.println("elle entraînera probablement des pertes de pas."); 
  Serial.println("Une séquence de 'Homing' est fortement recommandée après un arrêt d'urgence.");
  Serial.println("---------------------------------------------------------------------------------------");
  Serial.println("Pour commander il suffit d'envoyer une valeur + la valeur désirée en mm ou degrés.");
  Serial.println("Exemple: Avancer de 200mm => 10200. Tourner de 45° dans le sens horaire => 30045");
  Serial.println("=======================================================================================");
  Serial.println("");
  Serial.println("=> Commande ?");
}

void loop()
{

  
  while (Serial.available() == 0) {
    //wait
  }
  volatile uint16_t Commande = Serial.parseInt();

  if (Commande != 0 ){
  Wire.beginTransmission(4); // transmit to device #4
  Wire.write(Commande >> 8);
  Wire.write(Commande & 255);       // send
  Wire.endTransmission();    // stop transmitting
  Serial.print("Commande ");
  Serial.print(Commande);
  Serial.println(" envoyée!");
  Serial.println("");
  Serial.println("=> Commande ?");
  }
}
