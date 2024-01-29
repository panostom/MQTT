#ifndef __CLASSSENSOR__
#define __CLASSSENSOR__
#include <DHTesp.h> 

#define MAX_VALUES 5

class Sensor
{
  public:
    bool initialise(unsigned int);
    void set_temp_bound(unsigned int);
    void set_hum_bound(unsigned int);
    void set_delay_bound(unsigned int);
    unsigned int get_temperature();
    unsigned int get_humidity();
    unsigned int get_delay();
    bool take_measurements();

  private:
    DHTesp  dht;
    unsigned int boundTemperature=18; //default
    unsigned int boundHumidity=50;
    unsigned int boundDelay=10;
    float temp_measurements[MAX_VALUES]; //pinakas metrhsewn
    int added_temp_values=0;
    bool is_buffer_full=false;
    void computeRate();
    void add_to_temp_array(float);
};

bool Sensor::initialise(unsigned int pin) {
  // Initialize temperature sensor
  dht.setup(pin, DHTesp::DHT22);
  Serial.println("DHT initiated");
  return true;
}

void Sensor::set_temp_bound(unsigned int temp){
  if(!isnan(temp)) boundTemperature=temp;
}

void Sensor::set_hum_bound(unsigned int hum){
  if(!isnan(hum)) boundHumidity=hum;
}

void Sensor::set_delay_bound(unsigned int _delay){
  if(!isnan(_delay)) boundDelay=_delay; 
}

unsigned int Sensor::get_temperature(){
  return boundTemperature;
}

unsigned int Sensor::get_humidity(){
  return boundHumidity;
}

unsigned int Sensor::get_delay(){
  return boundDelay;
}

void Sensor::add_to_temp_array(float temperature){
  for(int j=0;j<MAX_VALUES-1;j++){   //prwta shift aristera
    temp_measurements[j]=temp_measurements[j+1];
  }
  temp_measurements[MAX_VALUES-1]=temperature; //meta eisagwgh neas timhs
  added_temp_values++;
  if(added_temp_values==MAX_VALUES){
    is_buffer_full=true;
  }
  if(is_buffer_full){
    computeRate();
  }
}

bool Sensor::take_measurements(){
  TempAndHumidity newValues=dht.getTempAndHumidity();   //struct
  if(isnan(newValues.temperature) || isnan(newValues.humidity)){
    Serial.println("I can't read values from sensor");
    return false;
  }
  Serial.println("|Temperature is:\t"+String(newValues.temperature)+"oC|"+"\t|Humidity is:\t"+String(newValues.humidity)+"%|");
  if(newValues.temperature>boundTemperature) Serial.println("Temperature high!");
  if(newValues.humidity>boundHumidity) Serial.println("Humidity high! ");
  add_to_temp_array(newValues.temperature);
  return true;
}  

void Sensor::computeRate() {        //thermokrasia sthn parodo tou xronou,x:xronos,f(x):thermokrasia
  double sum_x = 0, sum_y = 0, sum_xy = 0, sum_xx = 0;
  for (int j = 0; j < MAX_VALUES; j++) {
    sum_x = sum_x + j * boundDelay; //xronikes stigmes
    sum_y = sum_y + (double)temp_measurements[j]; //thermokrasies
    sum_xx = sum_xx + ((j * boundDelay) * (j * boundDelay));
    sum_xy = sum_xy + (double)temp_measurements[j] * (j * boundDelay);
  }

  double slope = ((sum_x * sum_y) - (MAX_VALUES * sum_xy)) / ((sum_x * sum_x) - (MAX_VALUES * sum_xx));
  double b = (sum_y - (slope * sum_x)) / MAX_VALUES;
  Serial.println("y=" + String(slope) + "x + " + '(' + String(b) + ')');
}

#endif
