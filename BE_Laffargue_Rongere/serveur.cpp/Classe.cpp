#include "Classe.h"

composant::composant(int P) : pin (P){
}

void composant::setPin(int P){
  pin=P;
}

int composant::getPin(){
  return pin;
}

actionneur::actionneur(int P) : composant(P){
  statut = 0;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, statut);
}

void actionneur::setStatut(int S){
  statut=S;
  digitalWrite(pin, statut);
}

int actionneur::getStatut(){
  return statut;
}

capteur::capteur(int P):composant(P){
  B=4275;
  R0=100000;
  pinMode(pin, OUTPUT);
  getTemperature();
}

float capteur::getTemperature(){
  int a = analogRead(pin);
  float R= 1023.0/a-1.0;
  R = R0*R;
  temperature=1.0/(log(R/R0)/B+1/298.15)-273.15;
  return temperature;
}

boolean capteur::operator < (float T){
  return((this->temperature)<T);
}
boolean capteur::operator > (float T){
  return((this->temperature)>T);
}

reseau::reseau(std::map<String, String>::iterator it){
  ssid = it->second;
  it++;
  password = it->second;
  it++;
  infos = it->second;
}

void reseau::connection(){
  Serial.print("Connecting to ");
  Serial.println(ssid);   //SSID
  WiFi.begin(ssid, password);  //SSID , PASSWORD

  Serial.print(" \n Network info ");
  Serial.print(infos);  //INFOS
  int Attempts=0;
  
  while ((WiFi.status() != WL_CONNECTED)) {
    delay(500);
    Serial.print("a");
    Attempts++;
    if (Attempts>=30){
      throw ConnectionFailed;
    }
  }
}

void reseau::printIPAddress(){
  Serial.println("b");
  Serial.println("WiFi connected at IP address:");
  Serial.println(WiFi.localIP());
}

serveur::serveur(WiFiServer S):server(S){
}
void serveur::startServeur(){
  server.begin();
}
void serveur::serveurListen(){
  if (!client) client = server.available();
}
boolean serveur::waitClient(){
  return client.available();
}
void serveur::setRequest(){
  request = client.readStringUntil('\r');
  Serial.println(request);
}
void serveur::killRequest(){
  request="";
}
String serveur::getRequest(){
  return request;
}

void serveur::createHTML(float TempSensor, float TempWanted){
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: keep-alive");
  client.println("");
   
     //-------------------------------------------------------------------------------DOC HTML
     
  client.println("<!DOCTYPE html>");
      client.println("<html>");
      client.println("<head>         <meta charset=\"utf-8\" />         <title>gestion de la température</title>         <style>       #grad1 {        height: 100%;         background-color: orange;         background: linear-gradient(13deg,#E78B2A, #E1563B);        border: 1px solid;        width : 90%;        height : 500px;         cellpadding : 5px;        cellspacing : 5px;        padding: 20px;        border-radius: 25px;        text-align: center;               }       #grad0 {        height: 100%;         background-color: #2A2727;        background: linear-gradient( #2A2727, #030202);             }             td {      border-width:2px;       border-style:solid;       border-color:black;       width:50%;      }             th {      border-width:2px;       border-style:solid;       border-color:#262423;       width:50%;      }           </style>     </head>      <body id = \"grad0\">     <FONT size=\"6\" face=\"Verdana\">      <table id = \"grad1\" align = \"center\">         <thead>           <tr>            <th colspan=\"2\">Gestion de la température de la boite</th>          </tr>         </thead>        <tbody>           <tr>            <td>TEMP <a href=\\\"/moins\\\" style=\"color:#F7C482\">-</a><br></td>            <td>TEMP <a href=\\\"/plus\\\" style=\"color:#F7C482\">+</a><br></td>           </tr>           <tr id =\"parenttempc\">            <td>Température Cible</td>            <td id=\"tempc\">");
  
      client.println(TempWanted);
      
      client.println("   °C</td>           </tr>           <tr id = \"parenttempa\">            <td>Température actuelle</td>             <td id =\"tempa\">");
      
      client.println(TempSensor);
  
      client.println("   °C</td>           </tr>         </tbody>      </table>    </FONT><BR>     </body>");
      //client.println("</html>");
  
      //-------------------------------------------------------------------------------FIN DOC HTML
      Serial.println("");
}

void serveur::replaceValues(float TempSensor, float TempWanted){
  client.println("<script>  var td1 = document.getElementById(\'tempc\');   td1.parentNode.removeChild(td1);     var tr1 = document.getElementById(\'parenttempc\');     var td2 = document.createElement(\'td\');     td2.setAttribute(\"id\",\"tempc\");     tr1.appendChild(td2);     var tdText = document.createTextNode(\"");
  client.println(TempWanted);
  client.println(" °C\");     td2.appendChild(tdText);               var td3 = document.getElementById(\'tempa\');   td3.parentNode.removeChild(td3);     var tr2 = document.getElementById(\'parenttempa\');     var td4 = document.createElement(\'td\');     td4.setAttribute(\"id\",\"tempa\");     tr2.appendChild(td4);     tdText = document.createTextNode(\"");
  client.println(TempSensor);
  client.println("test °C\");     td4.appendChild(tdText);      </script>");
}
