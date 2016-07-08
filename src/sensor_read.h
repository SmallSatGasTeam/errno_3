#ifndef READ_SENSORS_H
#define READ_SENSORS_H

void read_temp(Adafruit_MCP9808* sensor){
  Serial.println(sensor->readTempC());
  Serial3.println(sensor->readTempC());
}

void read_baro(CoolSatBaro* sensor){
  sensor->readBaro();
  Serial.println(sensor->getPressure());
}

#endif
