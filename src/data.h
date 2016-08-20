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
  T getGyro_accel(char); /* Usage: getGyro_accel('x'), getGyro_accel('y'), etc */
  T getGyro_euler(char);
  T getGPS(char); /* getGPS('p'), getGPS('l') -- p for "phi" and l for "lambda" - Google it */ 
  T getTime(char); /* getTime('h'), getTime('m'), getTime('s') */
  bool deployed() { return boomDeployed; }
 
  // -- Sets -- //
  void setTempIn(T data) { tempIn = data; }
  void setTempEx(T data) { tempEx = data; }
  void setPressure(T data) { pressure = data; }
  void setAltitude(T data) { altitude = data; }
  void setLux(T data) { lux = data; }
  void setUV(T data) { uv = data; }
  void setGyro_accel(T, char); /* setGyro_accel(value, 'x'), setGyro_accel(value, 'y') */
  void setGyro_euler(T, char);
  void setGPS(T, char); /* setGPS(value, 'p'), setGPS(value, 'l') */
  void setTime(T, char);/* setTime(value, 'h'), setTime(value, 'm'), etc. */
  void setDeployed(bool state) { boomDeployed = state; }
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

// ----------- Gets ------------ //

template <typename T>
T Data<T>::getGyro_accel(char c)
{
  switch (c)
  {
    case 'x': return accelX;
    case 'y': return accelY;
    case 'z': return accelZ;
    default: return 0;
  }
}

template <typename T>
T Data<T>::getGyro_euler(char c)
{
  switch (c)
  {
    case 'x': return eulerX;
    case 'y': return eulerY;
    case 'z': return eulerZ;
    default: return 0;
  }
}

template <typename T>
T Data<T>::getGPS(char c)
{
  switch (c)
  {
    case 'p': return lat;
    case 'l': return lng;
    default: return 0;
  }
}

template <typename T>
T Data<T>::getTime(char unit)
{
  switch (unit)
  {
    case 'h': return hour;
    case 'm': return minute;
    case 's': return second;
    default: return 0;
  }
}

// ----------- Sets ------------ //

template <typename T>
void Data<T>::setGyro_accel(T data, char c)
{
  switch (c)
  {
    case 'x': accelX = data;
      break;
    case 'y': accelY = data;
      break;
    case 'z': accelZ = data;
      break;
    default: return;
  }
  return;
}

template <typename T>
void Data<T>::setGyro_euler(T data, char c)
{
  switch (c)
  {
    case 'x': eulerX = data;
      break;
    case 'y': eulerY = data;
      break;
    case 'z': eulerZ = data;
      break;
    default: return;
  }
  return;
}

template <typename T>
void Data<T>::setGPS(T data, char c)
{
  switch (c)
  {
    case 'p': lat = data;
      break;
    case 'l': lng = data;
      break;
    default: return;
  }
  return;
}

template <typename T>
void Data<T>::setTime(T data, char c)
{
  switch (c)
  {
    case 'h': hour = data;
      break;
    case 'm': minute = data;
      break;
    case 's': second = data;
      break;
    default: return;
  }
  return;
}

#endif
