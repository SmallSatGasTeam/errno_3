#ifndef SENSORS_H
#define SENSORS_H

#define FILENAME "sensors.txt"
//----------- Temperature sensors ------------//

void initialize_temp_ex(Adafruit_MCP9808* sensor, Stream& output){
  if (!sensor->begin(0x18)) {
    output.println("Couldn't find external MCP9808!");
    while (1);
  }
}

void initialize_temp_in(Adafruit_MCP9808* sensor, Stream& output){
  if (!sensor->begin(0x1D)) {
    output.println("Couldn't find internal MCP9808!");
    while (1);
  }
}

void read_temp(Adafruit_MCP9808* sensor, Stream& output){
  File file = SD.open(FILENAME, FILE_WRITE);
  output.print(sensor->readTempC());
  output.print("\t");
  file.println(sensor->readTempC());
  file.print("\t");
  file.close();
}

//------------ Barometer ------------//

void initialize_baro(CoolSatBaro* sensor, Stream& output){
  sensor->initial(0x76);
}

void read_baro(CoolSatBaro* sensor, Stream& output){
  File file = SD.open(FILENAME, FILE_WRITE);
  sensor->readBaro();
  output.print(sensor->getPressure());
  output.print("\t");
  file.print(sensor->getPressure());
  file.print("\t");
  file.close();
}

//------------ Light & UV sensors ------------//

void read_light(Stream& output){
    File file = SD.open(FILENAME, FILE_WRITE);
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

    output.print(lux);
    output.print("\t");
    file.print(lux);
    file.print("\t");
    file.close();
}

void read_uv(Stream& output){
    File file = SD.open(FILENAME, FILE_WRITE);
    const int uvPin = 1; // UV sensor pin
    float uv = 0.0; // default
    uv = analogRead(uvPin); // reads value

    output.print(uv);
    output.print("\t");
    file.print(uv);
    file.print("\t");
    file.close();
}

//------------ Gyroscope ------------//

void initialize_gyro(Adafruit_BNO055* gyro, Stream& output){
	if (!gyro->begin()){
		output.println("Couldn't detect BNO055 gyroscope ... Check your wiring or I2C ADDR!");
		while(1);
	}
	gyro->setExtCrystalUse(true);
}

void read_gyro(Adafruit_BNO055* gyro, Stream& output){

	imu::Vector<3> acceleration = gyro->getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);

	output.print("\t");
	output.print(acceleration.x());
	output.print("\t");
	output.print(acceleration.y());
	output.print("\t");
	output.print(acceleration.z());

	imu::Vector<3> euler = gyro->getVector(Adafruit_BNO055::VECTOR_EULER);

	output.print("\t");
	output.print(euler.x());
	output.print("\t");
	output.print(euler.y());
	output.print("\t");
	output.print(euler.z());

	delay(100); // Delay of 100ms
}

//------------- GPS ---------------//

//GPS must be constantly be fed characters
void smartDelay(unsigned long ms, TinyGPSPlus* gps, Stream& input)
{
  unsigned long start = millis();
  do
  {
    while (input.available()){
      gps->encode(input.read());
    }
  } while (millis() - start < ms);
}

//prints GPS values to desired locations
void printFloat(float val, bool valid, int len, int prec, File* file, Stream* output)
{
  if (!valid)
  {
    while (len-- > 1)
      file->print('*');
      file->print(' ');
      output->print('*');
      output->print(' ');
  }
  else
  {
    file->print(val, prec);
    output->print(val,prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      file->print(' ');
      output->print(' ');
  }
}

void read_gps(TinyGPSPlus* gps, Stream& output){

	File file = SD.open(FILENAME, FILE_WRITE);
	file.print("\t");
	output.print("\t");
	printFloat(gps->location.lat(),gps->location.isValid(),11,6,&file,&output);
	file.print("\t"); output.print("\t");
	printFloat(gps->location.lng(),gps->location.isValid(),12,6,&file,&output);
	file.print("\t");
	output.print("\t");
	smartDelay(1000, gps, output);

}
//------------ Clock ------------//

void printTime(int time, Stream& output)
{
	if (time >= 0 && time < 10){ // Prefaces times less than 10 with a 0
		output.print("0");		 // e.g., converts "12:8:9" to "12:08:09"
		output.print(time);
	}
	else output.print(time);
}

void timestamp(Stream& output)
{
	tmElements_t tm; // magic getter of time from TimeLib.h

	output.print("\t");

	if (RTC.read(tm)){
		printTime(tm.Hour, output);
		output.print(":");
		printTime(tm.Minute, output);
		output.print(":");
		printTime(tm.Second, output);
	}
	else output.println("Error: Failed to fetch time");
}
#endif
