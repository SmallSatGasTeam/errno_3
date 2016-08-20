#ifndef DATA_H
#define DATA_H

template <typename T>
class Data
{
public:
  Data() {}
  // -- Gets -- //
  T getTempIn() { return tempIn; }
  T getTempEx() { return tempEx; }
  T getPressure() { return pressure; }
  T getAltitude() { return altitude; }
  T getLux() { return lux; }
  T getUV() { return uv; }
  T getGyro_accel(char&); /* Usage: getGyro_accel('x'), getGyro_accel('y'), etc */
  T getGyro_euler(char&);
  T getGPS(char&); /* getGPS('p'), getGPS('l') -- p for "phi" and l for "lambda" - Google it */ 
  T getTime(char&); /* getTime('h'), getTime('m'), getTime('s') */
  bool boomDeployed() { return boomDeployed; }
 
  // -- Sets -- //
  void setTempIn(T data) { tempIn = data; }
  void setTempEx(T data) { tempEx = data; }
  void setPressure(T data) { pressure = data; }
  void setAltitude(T data) { altitude = data; }
  void setLux(T data) { lux = data; }
  void setUV(T data) { uv = data; }
  void setGyro_accel(T, char&); /* setGyro_accel(value, 'x'), setGyro_accel(value, 'y') */
  void setGyro_euler(T, char&);
  void setGPS(T, char&); /* setGPS(value, 'p'), setGPS(value, 'l') */
  void setTime(T, char&);/* setTime(value, 'h'), setTime(value, 'm'), etc. */
  void setBoomDeployed(bool state) { boomDeployed = state; }
private:
  T tempIn;
  T tempEx;
  T pressure;
  T altitude;
  T lux;
  T uv;
  bool boomDeployed;
  T accelX;
  T accelY;
  T accelZ;
  T eulerX;
  T eulerY;
  T eulerZ;
  T lat;
  T lng;
  T hour;
  T minute;
  T second;
};

#endif
