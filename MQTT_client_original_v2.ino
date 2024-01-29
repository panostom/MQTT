// Define libraries
//-----------------------------
#include <WiFi.h>
#include "calculations.h"
#include "classSensor.h"
#include "mqtt_client.h"
//-----------------------------
//-----------------------------
unsigned long measPreviousMillis = 0;
bool startComputation=false;  //na 3ekinaei upologismoys afou ftiaxei ta oria
bool errorPubBounds=false;
bool errorPubCrcData=false;

void setVariables(String);
char crcDataMessage[20];

void callback(char*, byte*, unsigned int);


MQTT_client mqttclient;
Sensor sensor; //class sensor


void callback(char* topic, byte* payload, unsigned int length) {
  char p[length + 1]; //gia na xwraei termatikos xarakthras
  memcpy(p, payload, length);
  p[length] = '\0'; //termatikos xarakthras
  String message(p);  //metatrepei to char array se string class, ara to payload
  String topic_str(topic);

  // Print received messages and topics
  //------
  Serial.println("");
  Serial.println("Callback function");
  Serial.println("Topic: ");
  Serial.println(topic);

  Serial.println("Message: ");
  Serial.println(message);
  //------
  //------

  // Check received messages and topics
  //------
  if (topic_str == topicData) {
    Serial.println("o server elabe configuration, apanthse " + topic_str);
    setVariables(message);
    Compute_CRC8_bounds(crcDataMessage,sensor.get_delay(),sensor.get_temperature(),sensor.get_humidity());    //kalw ton upologismo tou crc kai ftiaxnw to mhnyma dataCrc
    errorPubCrcData=mqttclient.sendMessageCrcData(crcDataMessage);  //publish crcdata
    mqttclient.loop();
  }
  if (topic_str == topicControl) {
    if(message == "updateConfig"){
      Serial.println("broker send updateConfig, stamatw deigmatolhpsia mexri na parw nea oria");
      startComputation=false; //stamataw deigmatolhpsia mexri na parw to nea oria
      mqttclient.sendMessageBounds();  //publish gia na steilw sendConfig
      mqttclient.loop();
    }
    else if(message == "startMeasurements"){
      startComputation=true;
    }
    else if(message == "crcError"){
      Serial.println("error at crc");
    }
  }
}

void setVariables(String configurationCode){    //ftiaxnw ta oria kathe fora pou ta pairnw, upologizw crc kai stelnw crcData
  Serial.println("setvariables"+String(configurationCode));
  char p[10];
  char temp[3];
  temp[2]= '\0';
  configurationCode.toCharArray(p,10);
  temp[0]=p[0];
  temp[1]=p[1];
  sensor.set_delay_bound(atoi(temp));
  Serial.println("delay= " + sensor.get_delay());

  temp[0]=p[2];
  temp[1]=p[3];
  sensor.set_temp_bound(atoi(temp));
  Serial.println("temp= " + sensor.get_temperature());

  temp[0]=p[4];
  temp[1]=p[5];
  sensor.set_hum_bound(atoi(temp));
  Serial.println("humidity= " + sensor.get_humidity());
}

//-------------------------------------------------------------------------------------

void setup() {
  // put your setup code here, to run once:

  // Connect to WiFi and establish serial connection
  //------
  Serial.begin(115200);
  delay(1000);

  Serial.println();

  mqttclient.connect2Wifi(); //sundesh sto WiFi

  // Connect to MQTT broker and subscribe to topics
  mqttclient.set_topic();
  mqttclient.set_callback(callback);
  mqttclient.initialize();
  
  mqttclient.mqttReconnect();  //1h sundesh kai subscribe
  Serial.println("1h sundesh mqtt reconnect");
  
  sensor.initialise(23);//give the pin of dht22

  errorPubBounds=mqttclient.sendMessageBounds();  //publish gia na parw oria
  
  mqttclient.loop();  
}


void loop() {
  // put your main code here, to run repeatedly:
  
  unsigned long currentMillis = 0;
  
  if(errorPubBounds){       //auto to kanw false otan eimai sigouros oti phra ta oria, ara sto callback
    mqttclient.mqttReconnect();
    Serial.println("errorPubBounds sundesh mqtt reconnect");
    mqttclient.sendMessageBounds();
  }                       
  else if(errorPubCrcData){
    mqttclient.mqttReconnect();
    Serial.println("errorPubCrcData sundesh mqtt reconnect");
    mqttclient.sendMessageCrcData(crcDataMessage);
  }
  //------
  //------
  //execute computeTempAndHumidity every delay seconds
  currentMillis = millis();
  if(startComputation){
    if ((currentMillis - measPreviousMillis) >=  (sensor.get_delay()*1000)){
      sensor.take_measurements();     
      measPreviousMillis=currentMillis;
    }
  }
  
  mqttclient.loop();
}
