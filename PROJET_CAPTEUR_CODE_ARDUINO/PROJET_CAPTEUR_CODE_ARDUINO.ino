//BIBLIOTHEQUES

#include <SoftwareSerial.h>  //Bluetooth
#include <Wire.h>   //OLED
#include <Adafruit_SSD1306.h>   //OLED
#include <SPI.h>  //Potentiomètre
#include <stdlib.h>

//DEF & CST
#define baudrate 9600

  //Bluetooth:
#define rxPin 8
#define txPin 7

  //Encodeur rotatoire:
#define Encodclkpin 2 
#define Encoddtpin 3 
#define Encodswpin 4

  //OLED:
#define nombreDePixelsEnLargeur 128 
#define nombreDePixelsEnHauteur 64 
#define brocheResetOLED -1 
#define adresseI2CecranOLED 0x3C
#define nb_item 3 //Nombre de choix dans le menu

  //Potentiomètre digital:
#define MCP_NOP                 0b00000000
#define MCP_WRITE               0b00010001
#define MCP_SHTDWN              0b00100001

  //Flex sensor:
#define flexPin A1

  //Capteur graphite:
#define graphitePin A0

//VARIABLES

  //Bluetooth:
SoftwareSerial mySerial (rxPin, txPin);

  //Encodeur:
volatile int Pas_encod = 0; // variable pour stocker la lecture de l'etat du bouton (crans)
int etat_bouton = 0; // variable pour stocker la lecture de l'etat du bouton (enfoncé)
int Menu = 0;

  //Flex sensor:
float val_flexs = 0;
float Vflexs, Rflexs = 0.0;
const float VCC = 5.0;
const float Rdiv = 47000.0;
const float flatres = 24700.0;
const float bentres = 80000.0;

  //Capteur graphite:
unsigned long previousMillis = 0; //Pour faire une clock
int val_graph = 0;
float VGraph, RGraph = 0.0;
const float R1 = 100000;
const float R3 = 100000;
const float R5 = 10000;

  //OLED:
int choix = 0;
float lastval = -100.0;

  //Potentiomètre digital:
const int csPin = 10;
const int maxPositions = 256;
const long rAB = 47000;
const byte rWiper = 125;
const byte pot0 = 0x11;
const byte pot0shutdown = 0x21;
float R2;

Adafruit_SSD1306 ecranOLED (nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED);

void setup() { //Configuration et initialisation des différents composants 

  Serial.begin(9600);

  //OLED:
  if (!ecranOLED.begin (SSD1306_SWITCHCAPVCC, adresseI2CecranOLED)) { // Arrêt du programme (boucle infinie) si échec d'initialisation
    while(1);
  }

  //Calibration
  ecranOLED.clearDisplay();
  ecranOLED.setTextSize(1);
  ecranOLED.setTextColor(SSD1306_WHITE);
  ecranOLED.setCursor(0,0);
  ecranOLED.println(F("Calibration en cours..."));
  ecranOLED.println(F("Veuillez ne pas toucher"));
  ecranOLED.println(F("le capteur."));
  ecranOLED.display();

  //Pontentiometre digital :
  pinMode(csPin, OUTPUT);
  digitalWrite(csPin, HIGH);
  SPI.begin();
  Calibration(); 
  ecranOLED.clearDisplay();
  ecranOLED.setCursor(0,0);
  ecranOLED.println(F("Calibration OK !"));
  ecranOLED.display();
  delay(1000);

  //Encodeur rotatoire:
  pinMode(Encodclkpin, INPUT); 
  digitalWrite(Encodclkpin, HIGH); 
  pinMode(Encoddtpin, INPUT); 
  digitalWrite(Encoddtpin, HIGH);
  pinMode(Encodswpin, INPUT); 
  digitalWrite(Encodswpin, HIGH);
  attachInterrupt(0, doEncoder, RISING); // On met une interruption sur l'encodeur

  //Module Bluetooth:
  pinMode (rxPin,INPUT);
  pinMode (txPin,OUTPUT);
  mySerial.begin(baudrate);

  //Capteurs:
  pinMode(flexPin, INPUT);
  pinMode(graphitePin, INPUT);

  delay(500);

}

//LOOP

void loop() {                               // Boucle en continue 

  Afficher_Menu(); //Appelée toutes les 500ms

}

//FONCTIONS

float flexSensor(){                       // Renvoie la valeur de la résistance aux bornes du flex sensor
  float ADCflex = analogRead(flexPin);
  Vflexs = ADCflex * VCC / 1024.0;
  Rflexs = Rdiv * (VCC / Vflexs - 1.0);
  return Rflexs;
}

float graphiteSensor(){                   // Renvoie la valeur de la résisatnce aux bornes du capteur graphite
  float ADCgraph = analogRead(graphitePin); 
  float VGraph = ADCgraph * VCC / 1024.0;
  RGraph = Rdiv*(VCC/VGraph-1.0); 
  return RGraph;
}

float graphiteSensor_voltage(){           // Renvoie la valeur de la tension aux bornes du capteur graphite
  float ADCgraph = analogRead(graphitePin); 
  float VGraph = ADCgraph * VCC / 1024.0; 
  return VGraph;
}

void setPotWiper(int addr, int pas){     // Règle la résistance de sortie du potentiomètre digital
  pas = constrain(pas, 0, 255);
  digitalWrite(csPin, LOW);
  SPI.transfer(addr);
  SPI.transfer(pas);
  digitalWrite(csPin, HIGH);
  R2 = ((rAB * pas) / maxPositions) + rWiper;
}


void Calibration(){                     // Calibre le potentiomètre digital pour avoir une tensoin de 3V avec une tolérance de 0.2V
  float target = 3.0, tol = 0.2;
  int pas = 0;
  char chaine[10];

  do {
    setPotWiper(pot0, pas);
    pas += 5;
    delay(200);
  } while ((graphiteSensor_voltage() < (target - tol) || graphiteSensor_voltage() > (target + tol)) && pas <= 265);
  
  dtostrf(pas, sizeof(chaine)-1, 2, chaine);
  
  if (pas < 265) {
    Serial.println(F("Potentiometer calibrated at pasition : "));
    Serial.print(chaine);

    float val = graphiteSensor();
    dtostrf(val, sizeof(chaine)-1, 2, chaine);
    Serial.println(F("Value : "));
    Serial.print(chaine);
  }
  else {
    Serial.println(F("Potentiometer not calibrated at target 3V"));
  }
} 


void doEncoder(){                          // Lit la position de l'encodeur (lors de l'interruption)
  if ( (digitalRead(Encodclkpin)==HIGH) && (digitalRead(Encoddtpin)==HIGH) ) { 
    Pas_encod++;
  } 
  else if ( (digitalRead(Encodclkpin)==HIGH) && (digitalRead(Encoddtpin)==LOW) ) {  
    Pas_encod--;
  }   
}


void appui_bouton(){                      // Vérifie l'état du bouton de l'endodeur rotatoire (enfoncé ou non)
  etat_bouton = digitalRead(Encodswpin);
  if (etat_bouton == 0) {
    if (Menu == 0) {
      Menu++;
    }
    else {
      Menu--;
    }
  }
}


void Afficher_Liste_Menu(int selection){   //Affiche la liste des menus avec la surbrillance sur le bon titre, gère l'affichage sur le OLED

  ecranOLED.clearDisplay();   
  ecranOLED.setTextSize(2);   
  ecranOLED.setCursor(0, 0);
  ecranOLED.setTextColor (SSD1306_WHITE, SSD1306_BLACK);  
  ecranOLED.println (F("-MENU-"));
  ecranOLED.setTextSize(1);

  if (selection==0){
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    ecranOLED.println(F("Flex Sensor"));
    ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    ecranOLED.println(F("Graphite Sensor"));
    ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    ecranOLED.println(F("Calibrer"));
  }
  else if (selection==1){
    ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    ecranOLED.println(F("Flex Sensor"));
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    ecranOLED.println(F("Graphite Sensor"));
    ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    ecranOLED.println(F("Calibrer"));
  }
  else if (selection==2){
        ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    ecranOLED.println(F("Flex Sensor"));
    ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    ecranOLED.println(F("Graphite Sensor"));
    ecranOLED.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    ecranOLED.println(F("Calibrer"));

  }
  
  ecranOLED.display();
}


void Ecran_Flex(){                              //Affiche le menu du flex sensor 

  float val = flexSensor();
  char res[20];
  dtostrf (val, 1, 2, res);
  ecranOLED.clearDisplay();
  ecranOLED.setCursor(1, 0);
  ecranOLED.setTextColor (SSD1306_WHITE, SSD1306_BLACK);
  ecranOLED.setTextSize(2);
  ecranOLED.println(F("-Menu1-"));
  ecranOLED.setTextSize(1);
  ecranOLED.println (F("FlexSensor :"));
  ecranOLED.println(F("Resistance (Ohm):"));
  ecranOLED.println(res);
  ecranOLED.display();
}


void Ecran_Graphite(){                        //Affiche le menu du graphite sensor et envoie bluetooth
  float val = graphiteSensor();
  char res[20];
  dtostrf(val, 1, 2, res);
  ecranOLED.clearDisplay();
  ecranOLED.setCursor(1, 0);
  ecranOLED.setTextColor (SSD1306_WHITE, SSD1306_BLACK);
  ecranOLED.setTextSize(2);
  ecranOLED.println(F("-Menu2-"));
  ecranOLED.setTextSize(1);
  ecranOLED.println (F("GraphiteSensor :"));
  ecranOLED.println(F("Resistance (Ohm):"));
  ecranOLED.println(res);
  ecranOLED.display();
  mySerial.println(res);
}


void Afficher_Menu (){                       // Affiche les menus sur l'écran OLED
  unsigned long currentMillis = millis ();  // Sauvegarde la valeur du temps écoulé depuis le lancement du programme

  if (currentMillis - previousMillis >= 500){

    previousMillis = currentMillis;
    appui_bouton();
    choix = abs(Pas_encod) % nb_item;

    switch (abs(choix)){
      case 0 :
        if (Menu == 0) {
          Afficher_Liste_Menu(0);
        }
        else if (Menu == 1) {
          Ecran_Flex();
        }
        break;

      case 1:
        if (Menu == 0) {
          Afficher_Liste_Menu(1);
        }
        else if (Menu == 1) {
          Ecran_Graphite();
        }
        break;
      
      case 2 : 
        if (Menu == 0) {
          Afficher_Liste_Menu(2);
        }
        else if (Menu == 1) {
          ecranOLED.clearDisplay();
          ecranOLED.setCursor(0, 0);
          ecranOLED.setTextSize(1);
          ecranOLED.setTextColor(SSD1306_WHITE);
          ecranOLED.println(F("MODE CALIB"));
          ecranOLED.println(F(""));
          ecranOLED.println(F("Ne pas toucher"));
          ecranOLED.println(F("le capteur..."));
          ecranOLED.display();
          Calibration(); 
          ecranOLED.clearDisplay();
          ecranOLED.setCursor(0, 0);
          ecranOLED.println(F("CALIBRATION OK !"));
          ecranOLED.display();
          delay(3000);
          Menu = 0;
        }
        break;

      break;
    }
  }
}
