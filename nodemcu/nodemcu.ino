#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <FirebaseArduino.h>
#include "DHT.h"
//-------- Configurações Terremoto -----------
#define SHAKE_SENSOR D2
//-------- Configurações Gas -----------
#define GASPIN A0
//-------- Configurações DHT -----------
#define DHTTYPE DHT11
//---------firebase---------
#define FIREBASE_HOST "tcc-teste-7b3c9.firebaseio.com"
#define FIREBASE_AUTH "c6jrx5LLIU8HkKfOAcGdx92ReXVFeHMKiTaHBq7m"
//------------ configuracao DHT11 --------------
#define DHTPIN D4
DHT dht(DHTPIN, DHTTYPE);
//---------- configuracao sensor chuva -------
#define CHUVA_SENSOR D5
//---------- configuracao sensor movimento -------
#define PIRSENSOR D6
//---------- sensor fogo ----------------
#define FOGO_SENSOR D7
//-------- Configurações de Wi-fi-----------
char* SSID = "Virus";
char* PASSWORD =  "piraporinha";
ESP8266WebServer server(80);
float Temperatura = 0;
float Umidade = 0;
float gas_value = 0;

void setup()
{
  dht.begin();
  initSerial();
  SetupPir();
  SetupGas();
  SetupShake();
  SetupChuva();
  SetupChama();
  initFirebase();
  //---- Conecta ao Wi-Fi ----
  initWiFi();
  server.on("/temperatura", temperatura); //Associa a função (/) ao void temperatura
  server.begin();                 // Inicia a comunicação com o servidor
  Serial.println("Servidor on-line");
}

void loop()
{
  lerDHT11();
  delay(500);
  pushfirebaseDHT11();
  delay(500);
  lerPIR();
  delay(500);
  lerFOGO();
  delay(500);
  lerGas();
  delay(500);
  lerChuva();
  delay(500);
  lerShake();
  delay(500);
  server.handleClient();
  delay(1500);
}

void SetupPir(){
  pinMode(PIRSENSOR, INPUT);
}

void SetupGas(){
  pinMode(GASPIN, INPUT);
}

void SetupChuva(){
  pinMode(CHUVA_SENSOR, INPUT);
}

void SetupChama(){
  pinMode(FOGO_SENSOR, INPUT);
}

void SetupShake(){
  pinMode(SHAKE_SENSOR, INPUT);
}

void initSerial(){
    Serial.begin(9600);
}

void initFirebase(){
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void initWiFi(){
    delay(100);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    reconectWiFi();
}

void reconectWiFi(){
    //se já está conectado a rede WI-FI, nada é feito. 
    //Caso contrário, são efetuadas tentativas de conexão
    if (WiFi.status() == WL_CONNECTED)
        return;
         
    WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI
     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
   
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.print(SSID);
    Serial.println("IP obtido: ");
    Serial.println(WiFi.localIP());
}

void lerDHT11(){
  Umidade = dht.readHumidity();    // leitura de Umidade
  Temperatura = dht.readTemperature(); // leitura de Temperatura
}
void lerPIR(){
  long state = digitalRead(PIRSENSOR);
  if (state == LOW){
    String Detectado = "Detectado";
    Firebase.pushString("PirSensor", Detectado);
    Serial.println("Movimento Detectado ");
  }
  else{
    String Detectado = "Não Detectado";
   // Firebase.pushString("PirSensor", Detectado);
    Serial.println("Movimento Não Detectado ");
  }
}
void lerFOGO(){
  if (digitalRead(FOGO_SENSOR) == HIGH){
    String Fogo_Detectado = "Fogo Não Detectado";
    //Firebase.pushString("Fogo_Sensor", Fogo_Detectado);
    Serial.println("Fogo Não Detectado ");

  }
  else{
    String Fogo_Detectado = "Fogo Detectado";
    Firebase.pushString("Fogo_Sensor", Fogo_Detectado);
    Serial.println("Movimento Detectado ");
  }
}

void lerGas(){
  gas_value = analogRead(GASPIN);
  if (gas_value > 280 && gas_value < 350){
    String gas_string = "Perigo Nivel baixo de GAS Detectado";
    Firebase.pushString("Gas_Sensor", gas_string);
    Firebase.pushFloat("Gas_Sensor_Nivel", gas_value);
  }
  if (gas_value >= 350 && gas_value < 450){
    String gas_string = "Perigo Nivel Medio de GAS Detectado";
    Firebase.pushString("Gas_Sensor", gas_string);
    Firebase.pushFloat("Gas_Sensor_Nivel", gas_value);
     }
  if (gas_value >= 450){
    String gas_string = "Perigo Nivel Critico de GAS Detectado";
    Firebase.pushString("Gas_Sensor", gas_string);
    Firebase.pushFloat("Gas_Sensor_Nivel", gas_value);
  }
    else{
    String gas_string = "Gas não Detectado";
    //Firebase.pushString("Fogo_Sensor", Fogo_Detectado);
    Serial.println("Gas não Detectado ");
  }
}

void lerChuva(){
  if (digitalRead(CHUVA_SENSOR) == LOW){
    String Chuva_Detectado = "Chuva Detectada";
    Firebase.pushString("Chuva_Sensor", Chuva_Detectado);
    Serial.println("Chuva Detectado ");

  }
  else{
    String Chuva_Detectado = "Chuva não Detectado";
    //Firebase.pushString("Fogo_Sensor", Fogo_Detectado);
    Serial.println("Chuva não Detectado ");
  }
}

void lerShake(){
  if (digitalRead(SHAKE_SENSOR) == LOW){
    String Shake_Detectado = "Tremor Detectado";
    Firebase.pushString("Shake_Sensor", Shake_Detectado);
    Serial.println("Tremor Detectado ");

  }
  else{
    String Shake_Detectado = "Tremor não Detectado";
    //Firebase.pushString("Fogo_Sensor", Fogo_Detectado);
    Serial.println("Tremor não Detectado ");
  }
}

void temperatura(){
  String message = "";
  message = "Temperatura = ";      // Escreve Temperatura
  message += Temperatura;                    // Exibi o valor de t
  message += "\n";                 // Pula uma linha entre as funções
  message += " Umidade = ";        // Escreve Umidade
  message += Umidade;                    // Exibi o valor de h
  server.send(200, "text/plain", message); // Retorna a resposta HTTP 
}

void pushfirebaseDHT11(){
  Firebase.pushFloat("Temperatura", Temperatura);
  Firebase.pushFloat("Umidade", Umidade); 
  } 
