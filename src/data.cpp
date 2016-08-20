#include "data.h"

// ----------- Gets ------------ //

template <typename T>
T Data<T>::getGyro_accel(char& c)
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
T Data<T>::getGyro_euler(char& c)
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
T Data<T>::getGPS(char& c)
{
  switch (c)
  {
    case 'p': return lat;
    case 'l': return lng;
    default: return 0;
  }
}

template <typename T>
T Data<T>::getTime(char& unit)
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
void Data<T>::setGyro_accel(T data, char& c)
{
  switch (c)
  {
    case 'x': accelX = data;
      break;
    case 'y': accelY = data;
      break;
    case 'x': accelZ = data;
      break;
    default: return;
  }
  return;
}

template <typename T>
void Data<T>::setGyro_euler(T data, char& c)
{
  switch (c)
  {
    case 'x': eulerX = data;
      break;
    case 'y': eulerY = data;
      break;
    case 'x': eulerZ = data;
      break;
    default: return;
  }
  return;
}

template <typename T>
void Data<T>::setGPS(T data, char& c)
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
void Data<T>::setTime(T data, char& c)
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




