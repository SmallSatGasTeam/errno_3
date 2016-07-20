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
  File file = SD.open("sensors.txt", FILE_WRITE);
  Serial.print(sensor->readTempC());
  Serial.print("\t");
  file.println(sensor->readTempC());
  file.print("\t");
  file.close();
}

//------------ Barometer ------------//

void initialize_baro(CoolSatBaro* sensor){
  sensor->initial(0x76);
}

void read_baro(CoolSatBaro* sensor){
  File file = SD.open("sensors.txt", FILE_WRITE);
  sensor->readBaro();
  Serial.print(sensor->getPressure());
  Serial.print("\t");
  file.print(sensor->getPressure());
  file.print("\t");
  file.close();
}

//------------ Light & UV sensors ------------//

void read_light(){
    File file = SD.open("sensors.txt", FILE_WRITE);
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

   Serial.print(lux);
   Serial.print("\t");
   file.print(lux);
   file.print("\t");
   file.close();
}

void read_uv(){
    File file = SD.open("sensors.txt", FILE_WRITE);
    const int uvPin = 1; // UV sensor pin
    float uv = 0.0; // default
    uv = analogRead(uvPin); // reads value
    
    Serial.print(uv);
    Serial.print("\t");
    file.print(uv);
    file.print("\t");
    file.close();
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

//------------- GPS ---------------//

//GPS must be constantly be fed characters
void smartDelay(unsigned long ms,TinyGPSPlus* gps)
{
  unsigned long start = millis();
  do 
  {
    while (Serial2.available()){
      gps->encode(Serial2.read());
    }
  } while (millis() - start < ms);
}

//prints GPS values to desired locations
void printFloat(float val, bool valid, int len, int prec, File* file)
{
  if (!valid)
  {
    while (len-- > 1)
      file->print('*');
      file->print(' ');
      Serial.print('*');
      Serial.print(' ');
  }
  else
  {
    file->print(val, prec);
    Serial.print(val,prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      file->print(' ');
      Serial.print(' ');
  }
}

void read_gps(TinyGPSPlus* gps){
	
	File file = SD.open("sensors.txt", FILE_WRITE);
	file.print("\t");
	Serial.print("\t");
	printFloat(gps->location.lat(),gps->location.isValid(),11,6,&file);
	file.print("\t"); Serial.print("\t");
	printFloat(gps->location.lng(),gps->location.isValid(),12,6,&file);	
	file.print("\t");
	Serial.print("\t");
	smartDelay(1000,gps);

}
//------------ Clock ------------//

void printTime(int time)
{
	if (time >= 0 && time < 10){ // Prefaces times less than 10 with a 0
		Serial.print("0");		 // e.g., converts "12:8:9" to "12:08:09"
		Serial.print(time);
	}
	else Serial.print(time);
}

void timestamp()
{
	tmElements_t tm; // magic getter of time from TimeLib.h
	
	Serial.print("\t");

	if (RTC.read(tm)){
		printTime(tm.Hour);
		Serial.print(":");
		printTime(tm.Minute);
		Serial.print(":");
		printTime(tm.Second);
	}
	else Serial.println("Error: Failed to fetch time");
}
#endif
