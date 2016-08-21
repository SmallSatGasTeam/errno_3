#ifndef SENSOR_H
#define SENSOR_H
#define READING_BUFFER_LENGTH 100

#include <stdint.h>

struct SensorReading{
  float* readings;
  SensorReading* next;
  SensorReading(float* readings = NULL, SensorReading* next = NULL)
    :readings(readings), next(next){}
};

class Sensor {
public:
  Sensor(const char* sensor_name):name(sensor_name), id(Sensor::nextId()){}
  static uint8_t nextId(){ return _id++; }
  static uint8_t _id;

  virtual bool read(float* buff) = 0;
  virtual bool init();
  const char* getName(){return name;}
  // virtual static SensorReading* validateReading(){} TODO

  const uint8_t id;
protected:
  const char* name;
};

uint8_t Sensor::_id = 0;

class TempSensor: public Sensor {
public:
  TempSensor(const char* name, Adafruit_MCP9808* sensor, uint8_t address)
    :Sensor(name), sensor(sensor), address(address){}

  bool read(float* buff){
    buff[0] = sensor->readTempC();
    buff[1] = (float) NULL;
    return true;
  }
  bool init(){return sensor->begin(address);}

protected:
  Adafruit_MCP9808* sensor;
  uint8_t address;
};


class BaroSensor: public Sensor {
  BaroSensor(const char* name, CoolSatBaro* sensor, uint8_t address)
    :Sensor(name), sensor(sensor), address(address){}

  bool read(float* buff){
    sensor->readBaro();
    buff[0] = sensor->getPressure();
    buff[1] = sensor->getAltitude();
    return true;
  }

  bool init(){
    sensor->initial(address);
    return true;
  }

protected:
  CoolSatBaro* sensor;
  uint8_t address;
};

class LightSensor: public Sensor {};
class UVSensor: public Sensor {};
class GyroSensor: public Sensor{};
class GPSSensor: public Sensor{};

#endif
