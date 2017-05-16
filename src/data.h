#ifndef DATA_H
#define DATA_H

struct temp_t 
{
  float internal;
  float external;
} temp;

struct baro_t
{
  float pressure;
  float altitude;
  float median;
} baro; 

struct light_t
{
  float lux;
  float  uv;
} light;

struct boom_t { 
  bool deployed; 
} boom;

struct gyro_t
{
  double accelX;
  double accelY;
  double accelZ;
  double eulerX;
  double eulerY;
  double eulerZ;
} gyro;

struct gps_t
{
  double lat;
  double lng;
} gps;

struct time
{
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} time;

struct battery_t
{
  float voltage;
} batt;

#endif
