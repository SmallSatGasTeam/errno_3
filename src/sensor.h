#ifndef READ_SENSORS_H
#define READ_SENSORS_H

void initialize_temp(Adafruit_MCP9808* sensor){
  if (!sensor->begin()) {
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
}

void read_temp(Adafruit_MCP9808* sensor){
  Serial.println(sensor->readTempC());
  Serial3.println(sensor->readTempC());
}

void initialize_baro(CoolSatBaro* sensor){
  sensor->initial(0x76);
}

void read_baro(CoolSatBaro* sensor){
  sensor->readBaro();
  Serial.println(sensor->getPressure());
}

#endif
