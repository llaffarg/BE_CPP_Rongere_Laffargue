#include <iostream>
#include <string>
#include <map>
#include <list>
#include "ESP8266WiFi.h"
using namespace std;

#if defined(ARDUINO_ARCH_AVR)
#define debug  Serial
#elif defined(ARDUINO_ARCH_SAMD) ||  defined(ARDUINO_ARCH_SAM)
#define debug  SerialUSB
#else
#define debug  Serial
#endif

const int OutOfRange=1;
const int ConnectionFailed=2;

class composant{
 protected:
  int pin;
 public:
  composant();
  composant(int P);
  
  int getPin();
  void setPin(int P);
  
  virtual ~composant(){};
  
};


class actionneur : public composant{
  public :
  actionneur(int pin);

  int getStatut();
  void setStatut(int val);
  
  ~actionneur(){};
  
  private :
  int statut; 
};

class capteur : public composant{
  public :
  capteur(int P);
  ~capteur(){};

  float getTemperature();

  boolean operator < (float T);
  boolean operator > (float T);
  
  private :
  float temperature;
  int B;
  int R0;
    
};

class reseau{
  private: 
  String ssid;
  String password;
  String infos;
  
  public:
  reseau(std::map<String, String>::iterator it);

  void connection();

  void printIPAddress();
};

class serveur{
  private:
  WiFiServer server;
  String request;
  WiFiClient client;
  public:
  serveur(WiFiServer S);
  void startServeur();
  void serveurListen();
  boolean waitClient();
  void setRequest();
  void killRequest();
  String getRequest();

  void createHTML(float TempSensor, float TempWanted);
  void replaceValues(float TempsSensor, float TempWanted);
};
