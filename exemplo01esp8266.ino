/*
 * Curso - Primeiro passos com ESP8266nodemcu e iot
 * Professor: Wilson Massami Ishihara
 * SENAI-SC de Rio do Sul
 * Exemplo - MQTT - IOT
 * Assina tópico no servidor MQTT
 * Nome do arquivo: exemploesp8266
 * Objetivo: aciona um led ligado a D1 do ESP8266 
 * versão: 1.0
 */


#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define L1 D1   //pino de saida para acionamento da Lampada L1

//informações da rede WIFI
const char* ssid = "**************";      //SSID da rede WIFI
const char* password =  "**************";    //senha da rede wifi

const char* mqttServer = "**************";                           //server
const char* mqttUser = "**************";                              //user
const  char* mqttPassword = "****************************";      //password
const int mqttPort = 1883;                                         //port

const char* mqttTopicSub ="**************";            //tópico que sera assinado

 
WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
 
  Serial.begin(115200);
  pinMode(L1, OUTPUT);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Conectando ao WiFi..");
  }
  Serial.println("Conectado na rede WiFi");
   
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 
  while (!client.connected()) 
  {
    Serial.println("Conectando ao Broker MQTT...");
     
    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {
      Serial.println("Conectado");  
       
    } else 
  {
      Serial.print("falha estado  ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  //subscreve no tópico
  client.subscribe(mqttTopicSub);
 
}
 
void callback(char* topic, byte* payload, unsigned int length) {

  //armazena msg recebida em uma sring
  payload[length] = '\0';
  String strMSG = String((char*)payload);

  Serial.print("Mensagem chegou do tópico: ");
  Serial.println(topic);
  Serial.print("Mensagem:");
  Serial.print(strMSG);
  Serial.println();
  Serial.println("-----------------------");
  
  //aciona saída conforme msg recebida 
  if (strMSG == "0"){         //se msg "1"
     digitalWrite(L1, LOW);  //coloca saída em LOW para ligar a Lampada - > o módulo RELE usado tem acionamento invertido. Se necessário ajuste para o seu modulo
  }else if (strMSG == "1"){   //se msg "0"
     digitalWrite(L1, HIGH);   //coloca saída em HIGH para desligar a Lampada - > o módulo RELE usado tem acionamento invertido. Se necessário ajuste para o seu modulo
  }
 
}

//função pra reconectar ao servido MQTT
void reconect() {
  //Enquanto estiver desconectado
  while (!client.connected()) 
  {
    Serial.print("Tentando conectar ao servidor MQTT");
        
    bool conectado = strlen(mqttUser) > 0 ?
                     client.connect("ESP8266Client", mqttUser, mqttPassword) :
                     client.connect("ESP8266Client");

    if(conectado) {
      Serial.println("Conectado!");
      //subscreve no tópico
      client.subscribe(mqttTopicSub, 1); //nivel de qualidade: QoS 1
    } else {
      Serial.println("Falha durante a conexão.Code: ");
      Serial.println( String(client.state()).c_str());
      Serial.println("Tentando novamente em 10 s");
      //Aguarda 10 segundos 
      delay(10000);
    }
  }
}
 
void loop() {
  if (!client.connected()) {
    reconect();
  }
  client.loop();
}