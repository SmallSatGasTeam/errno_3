#ifndef SENSORS_H
#define SENSORS_H

//----------- Temperature sensors ------------//

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

//------------ Barometer ------------//

void initialize_baro(CoolSatBaro* sensor){
  sensor->initial(0x76);
}

void read_baro(CoolSatBaro* sensor){
  sensor->readBaro();
  Serial.print("\t");
  Serial.print(sensor->getPressure());
}

//------------ Light & UV sensors ------------//

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

void read_uv(){
    const int uvPin = 1; // UV sensor pin
    float uv = 0.0; // default
    uv = analogRead(uvPin); // reads value
    
    Serial.print("\t");
    Serial.print(uv);
}

//------------ Gyroscope ------------//

void initialize_gyro(Adafruit_BNO055* gyro){
	if (!gyro->begin()){
		Serial.println("Couldn't detect BNO055 gyroscope ... Check your wiring or I2C ADDR!");
		while(1);
	}
	gyro->setExtCrystalUse(true);
}

void read_gyro(Adafruit_BNO055* gyro){

	imu::Vector<3> acceleration = gyro->getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);
	
	Serial.print("\t");
	Serial.print(acceleration.x());
	Serial.print("\t");
	Serial.print(acceleration.y());
	Serial.print("\t");
	Serial.print(acceleration.z());

	imu::Vector<3> euler = gyro->getVector(Adafruit_BNO055::VECTOR_EULER);

	Serial.print("\t");
	Serial.print(euler.x());
	Serial.print("\t");
	Serial.print(euler.y());
	Serial.print("\t");
	Serial.print(euler.z());

	delay(100); // Delay of 100ms 
}

#endif
