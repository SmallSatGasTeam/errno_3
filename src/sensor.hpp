#ifndef SENSOR_H
#define SENSOR_H
#include <stdint.h>

class Sensor {
public:
  Sensor(const char* sensor_name):name(sensor_name){}
  virtual bool read(char* buff) = 0;
  virtual bool init();
protected:
  const char* name;
};

class TempSensor: public Sensor {
public:
  TempSensor(const char* name, Adafruit_MCP9808* sensor, uint8_t address)
    :Sensor(name), sensor(sensor), address(address){}
  bool read(char* buff){
    if(!buff) return false;
    return sprintf(buff, "%f");
  }
  bool init(){return sensor->begin(0x18);}
protected:
  Adafruit_MCP9808* sensor;
  uint8_t address;
};

void read_sensor(Sensor* sensor, Stream* outputs, char* buff){
  if(!sensor) return;
  sensor->read(buff);
  for(uint8_t i = 0; outputs[i] != NULL; i++){
    outputs[i]->print(buff)
  }
}

#endif
