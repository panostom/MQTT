#ifndef __MQTT_CLIENT__
#define __MQTT_CLIENT__

#include <WiFi.h>
#include <PubSubClient.h>
#include "calculations.h"

// MQTT connection parameters
//------
const char* MQTT_USER = "user5"; // *** Replace with your username, e.g. user2, user3 etc. (leave it as it is for the test)
const char* MQTT_CLIENT = "user5";  // *** Must be unique, replace with your username, e.g. user2, user3 etc. (same as MQTT_USER)
const char* MQTT_ADDRESS = "esp-32.zapto.org";
//------
// WiFi connection parameters (WPA security protocol) 
const char* WIFI_SSID     = "COSMOTE-040B82";
const char* WIFI_PASSWORD = "N2FtSUuvHavkETxh";
//------
char topicControl[150], topicData[150],topicDataCrc[150];


class MQTT_client{
  public:
  void connect2Wifi();
  void topicSubscribe();
  void initialize();
  void set_callback(void (*f)(char *topic, byte *payload, unsigned int length));
  void mqttReconnect();
  bool sendMessageCrcData(char *);
  bool sendMessageBounds();
  void set_topic();
  void loop();
  bool connected();
  
  private:
    WiFiClient wifiClient;
    PubSubClient client;
};

void MQTT_client::loop(){
  
  client.loop();
  
  // Reconnect to WiFi if not connected
  if (WiFi.status() != WL_CONNECTED) {
    connect2Wifi();
  }
  
  // Reconnect to MQTT broker if not connected
  //------
  if (!client.connected()) {
    mqttReconnect();
    Serial.println("1h loop sundesh mqtt reconnect");
  }
}

bool MQTT_client::connected(){
  return client.connected();
}

void MQTT_client::initialize(){
  client.setClient(wifiClient);
  client.setServer(MQTT_ADDRESS,1883);
}

void MQTT_client::set_callback(void (*f)(char *topic, byte *payload, unsigned int length)){
  client.setCallback(f);
}

void MQTT_client::connect2Wifi(){
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Wait for WiFi...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  return;
}

void MQTT_client::set_topic(){
    // Define MQTT topic names
  sprintf(topicControl, "%s/%s", MQTT_USER, "control");
  sprintf(topicData, "%s/%s", MQTT_USER, "data");
  sprintf(topicDataCrc, "%s/%s", MQTT_USER, "dataCrc");
}

void MQTT_client::topicSubscribe() { //synarthsh gia subscribe
  if(client.connected()) {
    Serial.println("Subscribe to MQTT topics: ");
    Serial.println(topicControl);     //emfanish kai apostolh twn topics
    Serial.println(topicData);
    Serial.println(topicDataCrc);
    client.subscribe(topicControl);
    client.subscribe(topicData);
    client.subscribe(topicDataCrc);
    client.loop();
  }  
}

void MQTT_client::mqttReconnect() {
  // Loop until we're reconnected
  Serial.print("Wait for MQTT broker...");
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(MQTT_CLIENT, "samos", "karlovasi33#")) { //an sundethei epituxws
      Serial.println("MQTT connected");
      topicSubscribe();
    } 
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
bool MQTT_client::sendMessageBounds(){ //stelnw sendconfig h crcData
  if (client.connected()) { // if MQTT server connected
    client.publish(topicControl, "sendConfig");
    Serial.println("");
    Serial.println("**************************");
    Serial.println("Message published");
    return false;
  }
  else{
    Serial.println("publish Bounds message problem");
    return true;
  }
}  

bool MQTT_client::sendMessageCrcData(char *crcDataMessage){
  if (client.connected()) { // if MQTT server connected
    client.publish(topicDataCrc, crcDataMessage);
    Serial.println("");
    Serial.println("**************************");
    Serial.println(crcDataMessage);
    Serial.println("Message published");
    return false;
  }
  else{
    Serial.println("publish Crc data message problem");
    return true;
  }
}


#endif
