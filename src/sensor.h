#ifndef SENSORS_H
#define SENSORS_H

void read_timestamp(void* dummy, Stream* output);

bool message_peek(Stream** stream, char message, char &read_count, char num_readers){
  // if any streams have input
  for(char i = 0; stream[i] != NULL; i++){
    if(stream[i]->peek() == message){
      stream[i]->read();
      read_count = 0;
      return true;
    }
  }
  
  if(++read_count >= num_readers){
    read_count = 0;
    for(char i = 0; stream[i] != NULL; i++){
      stream[i]->read(); 
    }
  } 
  return false;
}

extern SemaphoreHandle_t xOutputSemaphore;
extern SemaphoreHandle_t xSDSemaphore;
extern File file;

template <typename F, typename S>
inline void sensor_out(S sensor, F func, char* file_name, Stream** outputs){
 
	if ( xSemaphoreTake( xOutputSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
    for(int i = 0; outputs[i] != NULL; i++){ 
			func(sensor, outputs[i]);
			outputs[i]->println(); 
    }
    xSemaphoreGive(xOutputSemaphore);
	}

  if ( xSemaphoreTake( xSDSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
    file = SD.open(file_name, FILE_WRITE);
		read_timestamp((void*) NULL, &file);
		file.print(", ");
    func(sensor, &file);
    file.println();
    file.close();
    xSemaphoreGive(xSDSemaphore);
  }
}
template <typename F, typename S>
void print_sensor(S sensor, F func, char header, Stream** outputs){
 // File file_baro = SD.open("baro.csv", FILE_WRITE);

// if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE &&
//  xSemaphoreTake( xRadioSemaphore, ( TickType_t ) 5 ) == pdTRUE  ){
    for(int i = 0; outputs[i] != NULL; i++){
      outputs[i]->print(header); outputs[i]->print(':');
      func(sensor, outputs[i]);
      outputs[i]->println();
    }
//  xSemaphoreGive( xRadioSemaphore );
//  xSemaphoreGive( xSerialSemaphore );
//  }
}

//----------- Temperature sensors ------------//

void initialize_temp_ex(Adafruit_MCP9808* sensor, Stream& output){
  if (!sensor->begin(0x18)) {
    output.println("Couldn't find external MCP9808!");
  }
}

void initialize_temp_in(Adafruit_MCP9808* sensor, Stream& output){
  if (!sensor->begin(0x1D)) {
    output.println("Couldn't find internal MCP9808!");
  }
}

void read_temp(Adafruit_MCP9808* sensor, Stream* output ){
  float val = sensor->readTempC();
  output->print(val);
}

//------------ Barometer ------------//

void initialize_baro(CoolSatBaro* sensor, Stream& output){
  sensor->initial(0x76);
}

void read_baro(CoolSatBaro* sensor, Stream* output){
  sensor->readBaro();
  float val = sensor->getPressure();
  float alt = sensor->getAltitude();
  output->print(val); output->print(','); output->print(alt);
}

//------------ Light & UV sensors ------------//

void read_light(void* dummy, Stream* output){
    float lightPin = 15; //analog light pin #
    float volt = 0.0; //voltage (volts)
    float RLDR = 0.0; //resistance (ohms)
    float lux = 0.0; //brightness (lumens/m2)
    const float TOVOLT = .0048; //converts sensor output to volts
    const float TOLUX = 776897.0; //converts to lux
    const float TOLUXPWR = -1.206; //converts to lux

    volt = analogRead(lightPin) * TOVOLT;
    RLDR = (1000.0 * (5 - volt )) / volt;
    lux = TOLUX * (pow(RLDR, TOLUXPWR));

    output->print(lux);
}

void read_uv(void* dummy, Stream* output){
  const int uvPin = 1; // UV sensor pin
  float uv = 0.0; // default
  uv = analogRead(uvPin); // reads value

  output->print(uv);
}

//------------ Gyroscope ------------//

void initialize_gyro(Adafruit_BNO055* gyro, Stream& output){
	if (!gyro->begin()){
		output.println("Couldn't detect BNO055 gyroscope ... Check your wiring or I2C ADDR!");
	}
	gyro->setExtCrystalUse(true);
}

void read_gyro(Adafruit_BNO055* gyro, Stream* output){

	imu::Vector<3> acceleration = gyro->getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);

	output->print(acceleration.x());
	output->print(",");
	output->print(acceleration.y());
	output->print(",");
	output->print(acceleration.z());
	output->print(",");

	imu::Vector<3> euler = gyro->getVector(Adafruit_BNO055::VECTOR_EULER);

	output->print(euler.x());
	output->print(",");
	output->print(euler.y());
	output->print(",");
	output->print(euler.z());
	output->print(",");

	// delay(100); // Delay of 100ms TODO needed?
}

//------------- GPS ---------------//

//GPS must be constantly be fed characters
void smartDelay(unsigned long ms, TinyGPSPlus* gps, Stream* input)
{
  unsigned long start = millis();
  do
  {
    while (input->available()){
      gps->encode(input->read());
    }
  } while (millis() - start < ms);
}

//prints GPS values to desired locations
void printFloat(float val, bool valid, int len, int prec, Stream* output)
{
  if (!valid)
  {
    while (len-- > 1){
     
     // file->print('*');
     // file->print(' ');
     // output->print('*');
     //  output->print(' ');
     
    }
  }
  else
  {
    output->print(val,prec);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i=flen; i<len; ++i)
      output->print(' ');
  }
}

void read_gps(TinyGPSPlus* gps, Stream* output){
//	printFloat(gps->location.lat(),gps->location.isValid(),11,6, output);
	output->print(gps->location.lat());
	output->print(",");
	output->print(gps->location.lng());
//	printFloat(gps->location.lng(),gps->location.isValid(),12,6, output);
//	smartDelay(1000, gps, output); //TODO which serial port?
}
//------------ Clock ------------//

void printTime(int time, Stream* output){
	if (time >= 0 && time < 10){ // Prefaces times less than 10 with a 0
		output->print("0");		 // e.g., converts "12:8:9" to "12:08:09"
		output->print(time);
	}
	else output->print(time);
}

void read_timestamp(void* dummy, Stream* output){	
	tmElements_t tm; // magic getter of time from TimeLib.h

	if (RTC.read(tm)){
		printTime(tm.Hour, output);
		output->print(":");
		printTime(tm.Minute, output);
		output->print(":");
		printTime(tm.Second, output);
	}
	else output->println("Error: Failed to fetch time");
}

//------------ Boom ------------//

void print_boom(void* dummy, Stream* output)
{
     output->print("***************** DEPLOYING BOOM **********************");
}

//------------ Battery ------------//

void checkBattery(){
const int batteryPin = 0;
const int powerOff = 10;
float battery = 0.0;

pinMode(powerOff, OUTPUT);
digitalWrite(powerOff, HIGH);

battery = analogRead(batteryPin);
battery = (battery * .00475) * 2;

    if (battery <= 6.3){
	digitalWrite(powerOff, LOW);
	delay(1000);
    } 

}

#endif
