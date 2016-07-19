#ifndef SENSORS_H
#define SENSORS_H

void initialize_temp_ex(Adafruit_MCP9808* sensor){
  if (!sensor->begin(0x18)) {
    Serial.println("Couldn't find external MCP9808!");
    while (1);
  }
}

void initialize_temp_in(Adafruit_MCP9808* sensor){
  if (!sensor->begin(0x1D)) {
    Serial.println("Couldn't find internal MCP9808!");
    while (1);
  }
}

void read_temp(Adafruit_MCP9808* sensor){
  Serial.print("\t");
  Serial.print(sensor->readTempC());
}

void initialize_baro(CoolSatBaro* sensor){
  sensor->initial(0x76);
}

void read_baro(CoolSatBaro* sensor){
  sensor->readBaro();
  Serial.print("\t");
  Serial.print(sensor->getPressure());
}

void read_light(){
   float lightPin = 15; //anlaog light pin #
   float volt = 0.0; //voltage (volts)
	float RLDR = 0.0; //resistance (ohms)
   float lux = 0.0; //brightness (lumens/m2)
   const float TOVOLT = .0048; //converts sesor output to volts
   const float TOLUX = 776897.0; //converts to lux
   const float TOLUXPWR = -1.206; //converts to lux

   volt = analogRead(lightPin) * TOVOLT;
   RLDR = (1000.0 * (5 - volt )) / volt;
	lux = TOLUX * (pow(RLDR, TOLUXPWR));
	
	Serial.print("\t");
   Serial.print(lux);
}
#endif
