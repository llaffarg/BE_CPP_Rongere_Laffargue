#include "Classe.h"
#include <Arduino.h>
#include <iostream>
#include <map>
#include <list>
#include <string>
using namespace std;
// Load Wi-Fi library
#include "ESP8266WiFi.h"


///////////////////////////////////Déclaration des variables

int compteur=100;
bool premier = true;

float tempwanted = 22.0;

actionneur relais(D5);                                                   //

// Set web server port number to 80
WiFiServer UnServeur(80);
serveur monServeur(UnServeur);

capteur captemp(A0);

void setup() {

  //---------------------------------------------------------------------------------------------------Wifi infos
  // on créer une liste de réseau wifi pour lesquels on stock le nom, le mdp et des infos sur le type de réseau
  // Ainsi on peut stocker les réseaux fréquemment utiliser pour changer facilement de cible de connextion
  std::map<String,String> network1;
  std::map<String,String> network2;
  std::map<String, String>::iterator it;
  
  network1["1-SSID"]="12";
  network1["2-PASSWORD"]="Lenny2308";
  network1["3-INFOS"]="point d'acces mobile de lenny";

  network2["1-SSID"]="NASA";
  network2["2-PASSWORD"]="0000";
  network2["3-INFOS"]="réseau de la NASA il me semble";

  list<std::map<String,String>> networks;
  list<std::map<String,String>>::iterator list_it;

  networks.push_back(network1);
  networks.push_back(network2);
  
//-----------------------------------------------------------------------------------------------------
//Setup BW
  Serial.begin(115200);

//-----------------------------------------------Setup pins
  relais.setStatut(LOW);

//-----------------------------------------------------------------------------------------------------  
//Connect to Wi-Fi network with SSID and PASSWORD

  list_it = networks.begin();
  it = (*list_it).begin();

  
  try{
    reseau monReseau(it);
  
    it = (*list_it).begin();
  
    monReseau.connection();
  
// Print local IP address 
    monReseau.printIPAddress();

// Start Web Server
    monServeur.startServeur();
  }
  catch(int e){
    if (e==ConnectionFailed){
      Serial.println("The connection has failed after 30 attempts");
    }
    exit(-1);
  }
}

void loop(){
  try{
    compteur ++;
  
    delay(10);
    if (captemp < tempwanted){
        relais.setStatut(HIGH);
      }
      else{
        relais.setStatut(LOW);
      }
// Create a client and listen for incoming clients
    monServeur.serveurListen();   
  
// Wait a client 
    if(monServeur.waitClient()){
    
// A new client is connected, get the request
      monServeur.setRequest();

// -----------------------------------requete de temperature 

      if ((monServeur.getRequest()).indexOf("/moins") != -1) {
        tempwanted--;
        monServeur.killRequest();
      } 
      if ((monServeur.getRequest()).indexOf("/plus") != -1){
        tempwanted++;
        monServeur.killRequest();
      }
      if (tempwanted>35 or tempwanted<20){
        throw OutOfRange;
      }
    
// Display GPIO status
      if (premier){
        premier = false;
        monServeur.createHTML(captemp.getTemperature(), tempwanted);
      } 
    
//    if (compteur >10){
//      compteur = 0;
//
//      monServeur.replaceValues(captemp.getTemperature(), tempwanted);
//    }
    }
    else{
      premier=true;
    }
  }
  catch(int e){
    if (e==OutOfRange){
      Serial.println("Temperature wanted is not reachable. Therefor, it has been reset to default value: 30.0 °C.");
      tempwanted=30.0;
    }
  }
  Serial.println(captemp.getTemperature());
}

//requete de type 192.168.43.199/moins
