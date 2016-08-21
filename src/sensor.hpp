#ifndef SENSOR_H
#define SENSOR_H
#define READING_BUFFER_LENGTH 100

#include <stdint.h>

class Sensor {
public:
  Sensor(const char* sensor_name):name(sensor_name){}
  virtual bool read(float* buff) = 0;
  virtual bool init();
  const char* getName(){return name;}
protected:
  const char* name;
};

class TempSensor: public Sensor {
public:
  TempSensor(const char* name, Adafruit_MCP9808* sensor, uint8_t address)
    :Sensor(name), sensor(sensor), address(address){}
  bool read(float* buff){
    // TODO null terminate buffer of floats
    // if(!buff) return false;
    // return sprintf(buff, "%f");
    return false;
  }
  bool init(){return sensor->begin(0x18);}
protected:
  Adafruit_MCP9808* sensor;
  uint8_t address;
};

struct SensorReading{
  float* readings;
  SensorReading* next;
};

class SensorReader{
public:
  SensorReader(Sensor** sensors, SensorReading* values):sensors(sensors), values(values){
    for(uint8_t i = 0; sensors[i] != (Sensor*) NULL; i++){
      // Do stuff to initialize files
    }
  }

  void read(Sensor* sensor, Stream** outputs, float* buff){
    // Fill buffer
    sensor->read(buff);
    

    //TODO log stuff
    uint8_t i;
    char* reading = "";

    char temp[20];
    for(i = 0; buff[i] != NULL && i < READING_BUFFER_LENGTH; i++){
      reading = strcat(reading, strcat(",", dtostrf(buff[i], 0, 2, temp)));
    }

    for(i = 0; outputs[i]!= NULL; i++){ outputs[i]->print(reading); }

    //TODO write file
  }

  // float* getValue(const char* sensor_name){
  //   int8_t sensor_index = findSensor(sensor_name);
  //   if(sensor_index > 0){return values[sensor_index];}
  //   return "*";
  // }

protected:

  int8_t findSensor(const char* sensor_name){
    for(uint8_t i = 0; sensors[i] != NULL; i++){
      if(strcmp(sensors[i]->getName(), sensor_name) == 0){
        return i;
      }
    }
    return -1;
  }


  Sensor** sensors;
  SensorReading* values;
};

// void SensorReader::read(Sensor* sensor, Stream** outputs, float* buff){
//   if(!sensor) return;
//   sensor->read(buff);
//   for(uint8_t i = 0; outputs[i]!= NULL; i++){
//     // outputs[i]->print(buff);
//   }
// }

#endif
