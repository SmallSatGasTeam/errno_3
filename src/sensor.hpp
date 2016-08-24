#ifndef SENSOR_H
#define SENSOR_H

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
  virtual bool init() = 0;
  const char* getName(){return name;}
  // virtual static SensorReading* validateReading(){} TODO

  const uint8_t id;
  const char* name;
};

uint8_t Sensor::_id = 0;

class TempSensor: public Sensor {
public:
  TempSensor(const char* name, uint8_t address)
    :Sensor(name), sensor(sensor), address(address){}

  bool read(float* buff){
    buff[0] = sensor.readTempC();
    buff[1] = (float) NULL;
    return true;
  }
  bool init(){return sensor.begin(address);}

protected:
  Adafruit_MCP9808 sensor;
  uint8_t address;
};


class BaroSensor: public Sensor {
public:
  BaroSensor(const char* name, uint8_t address)
    :Sensor(name), address(address){}

  bool read(float* buff){
    sensor.readBaro();
    buff[0] = sensor.getPressure();
    buff[1] = sensor.getAltitude();
    buff[2] = (float) NULL;
    return true;
  }

  bool init(){
    sensor.initial(address);
    return true;
  }

protected:
  CoolSatBaro sensor;
  uint8_t address;
};

class LightSensor: public Sensor {
public:
  LightSensor(const char* name, uint8_t pin)
    :Sensor(name), pin(pin){}

  bool read(float* buff){
    const float TOVOLT = .0048; //converts sensor output to volts
    const float TOLUX = 776897.0; //converts to lux
    const float TOLUXPWR = -1.206; //converts to lux
    float volt = analogRead(pin) * TOVOLT;
    float RLDR = (1000.0 * (5 - volt )) / volt;
    float lux = TOLUX * (pow(RLDR, TOLUXPWR));
    buff[0] = lux;
    buff[1] = NULL;
  }

  bool init(){return true;}

protected:
  uint8_t pin;
};

class UVSensor: public Sensor {
public:
  UVSensor(const char* name, uint8_t pin)
    :Sensor(name), pin(pin){}

  bool read(float* buff){
    int v = analogRead(pin); // reads value
    float uv = 5 / 1023.0 * v * 10;
    buff[0] = uv;
    buff[1] = (float) NULL;
  }

  bool init(){return true;}
protected:
  uint8_t pin;
};

class GyroSensor: public Sensor {
public:
  GyroSensor(const char* name):Sensor(name){}

  bool read(float* buff){
  
  imu::Vector<3> acceleration = sensor.getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
    buff[0] = acceleration.x();
    buff[1] = acceleration.y();
    buff[2] = acceleration.z();
    buff[3] = (float) NULL;

    return true;
  }

  bool init(){
  	if (!sensor.begin()){return false;}
  	sensor.setExtCrystalUse(true);
    return true;
  }

protected:
  Adafruit_BNO055 sensor;
};

class GPSSensor: public Sensor {
public:
  GPSSensor(const char* name):Sensor(name){}

  bool read(float* buff){
    buff[0] = gps.location.lat();
    buff[1] = gps.location.lng();
    buff[2] = (float) NULL;
    return true;
  }

  bool init(){
    return true;
  }
protected:
  TinyGPSPlus gps;
};


#endif
