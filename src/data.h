#ifndef DATA_H
#define DATA_H

struct data
{
  float internal;
  float external;
  float pressure;
  float altitude;
  float lux;
  float  uv;
  bool deployed;
  double accelX;
  double accelY;
  double accelZ;
  double eulerX;
  double eulerY;
  double eulerZ;
  double lat;
  double lng;
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} temp, baro, light, boom, gyro, gps, time;

#endif
