#include <ESP8266WiFi.h>
#include <PubSubClient.h>
void callback(char* subtopic, byte* payload, unsigned int payloadLength);

const char* ssid = "MLRITM@DGL";
const char* password = "digi@123";
#include "DHT.h"
#define DHTPIN D2    // what pin we're connected to
#define DHTTYPE DHT11   // define type of sensor DHT 11
DHT dht (DHTPIN, DHTTYPE);
 

#define ORG "rwcoys"
#define DEVICE_TYPE "NODEMCU5060"
#define DEVICE_ID "DELL"
#define TOKEN "9381575060"
String data;
char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char subtopic[] = "iot-2/cmd/home/fmt/String";
char pubtopic[] = "iot-2/evt/anil/fmt/json";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;
//Serial.println(clientID);

WiFiClient wifiClient;
PubSubClient client(server, 1883, callback, wifiClient);
#define led D1


void setup()
{
  Serial.begin(115200);
  Serial.println();
  pinMode(led,OUTPUT);
  wifiConnect();
  mqttConnect();
  dht.begin();
}


void loop() 
{
  
 if (!client.loop())
 {
    mqttConnect();
  }
delay(100);

float h = dht.readHumidity();
float t = dht.readTemperature();
if (isnan(h) || isnan(t))
{
Serial.println("Failed to read from DHT sensor!");
delay(1000);
return;
}

PublishData(t,h);
delay(1000);
}
  

void wifiConnect() {
  Serial.print("Connecting to "); Serial.print(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("nWiFi connected, IP address: "); Serial.println(WiFi.localIP());
}

void mqttConnect() {
  if (!client.connected()) {
    Serial.print("Reconnecting MQTT client to "); Serial.println(server);
    while (!client.connect(clientId, authMethod, token)) {
      Serial.print(".");
      delay(500);
    }
    initManagedDevice();
    Serial.println();
  }
}
  void PublishData(float temp, float humid){
 if (!!!client.connected()) {
 Serial.print("Reconnecting client to ");
 Serial.println(server);
 while (!!!client.connect(clientId, authMethod, token)) {
 Serial.print(".");
 delay(500);
 }
 Serial.println();
 }
 String payload = "{\"d\":{\"temperature\":";
  payload += temp;
  payload+="," "\"humidity\":";
  payload += humid;
  payload += "}}";
 Serial.print("Sending payload: ");
 Serial.println(payload);
  
 if (client.publish(pubtopic, (char*) payload.c_str())) {
 Serial.println("Publish ok");
 } else {
 Serial.println("Publish failed");
 }
}


void initManagedDevice() {
  if (client.subscribe(subtopic)) {
    Serial.println("subscribe to cmd OK");
  } else {
    Serial.println("subscribe to cmd FAILED");
  }
}


void callback(char* subtopic, byte* payload, unsigned int payloadLength) {
  Serial.print("callback invoked for topic: "); Serial.println(subtopic);

  for (int i = 0; i < payloadLength; i++) {
    data += (char)payload[i];
  }
Serial.println(data);
if(data == "light_on"){
  digitalWrite(led,LOW);
  Serial.println("Light is Switched ON");
}
else if(data == "light_off"){
  digitalWrite(led,HIGH);
  Serial.println("Light is Switched OFF");
}
data ="";
}
