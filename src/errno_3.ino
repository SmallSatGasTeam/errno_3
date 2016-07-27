#define FILENAME "sensors.txt"

#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <CoolSatBaro.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MCP9808.h>
#include <Adafruit_BNO055.h>
#include <uCamII.h>
#include <TinyGPS++.h>
#include <TimeLib.h>
#include <DS1307RTC.h>
#include "sensor.h"
#include "messages.h" // Defines incoming data header

// define sensors 
Adafruit_MCP9808 sensor_temp_ex = Adafruit_MCP9808();
Adafruit_MCP9808 sensor_temp_in = Adafruit_MCP9808();
Adafruit_BNO055  sensor_gyro = Adafruit_BNO055();
CoolSatBaro sensor_baro;
TinyGPSPlus sensor_gps;
UCAMII camera(Serial1, &Serial);

File file;

// define tasks
void TaskBlink( void *pvParameters ); //TODO remove this test task
void TaskAnalogRead( void *pvParameters ); //TODO remove this test task
void TaskSensorReadStandard(void *pvParameters);
void TaskSensorReadFast(void *pvParameters);
void TaskGPSRead(void *pvParameters);
void TaskCamera(void *pvParameters);

// define semaphores
SemaphoreHandle_t xOutputSemaphore;
SemaphoreHandle_t xSDSemaphore;

 const int num_files = 7;
 char* file_names[] = {"baro.csv", "temp_in.csv", "temp_ex.csv", "light.csv", "uv.csv" ,"gps.csv", "gyro.csv", "camera.csv"};

 File files[num_files];

/**
 *Global setup should occur here
 */
void setup() {
 
  Serial.begin(9600);
  Serial1.begin(115200);
  Serial2.begin(9600);
  Serial3.begin(9600);

  Serial.println("Initializing SD Card");
  if(!SD.begin(46)){
    Serial.println("SD card failed to initialize!");
  }
  Serial.println("SD Initialized");
 
 for(int i = 0; i < num_files; i++){
   files[i] = SD.open(file_names[i], FILE_WRITE);
 }
 
 // initialize Mutex  
 if (xOutputSemaphore == NULL) {
    xOutputSemaphore = xSemaphoreCreateMutex(); 
    if(xOutputSemaphore){ xSemaphoreGive(xOutputSemaphore);}
  }
 
 if (xSDSemaphore == NULL) {
    xSDSemaphore = xSemaphoreCreateMutex(); 
    if(xSDSemaphore){ xSemaphoreGive(xSDSemaphore);}
  }

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskBlink
    ,  (const portCHAR *) "Blink"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

 xTaskCreate(
   TaskAnalogRead
   ,  (const portCHAR *) "AnalogRead"
   ,  600  // Stack size
   ,  NULL
   ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
   ,  NULL );

xTaskCreate(
    TaskSensorReadStandard
    ,  (const portCHAR *) "ReadSensors"
    ,  512  // Stack size
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

xTaskCreate(
    TaskSensorReadFast
    ,  (const portCHAR *) "ReadSensorsFast"
    ,  350  // Stack size
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

xTaskCreate(
    TaskCamera
    ,  (const portCHAR *) "Take Photos"
    ,  350 // Stack size
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

xTaskCreate(
    TaskGPSRead
    ,  (const portCHAR *) "GPSRead"
    ,  512  // Stack size
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}
void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/


void TaskBlink(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);

  for (;;) // A Task shall never return or exit.
  {
    digitalWrite(13, HIGH);   // turn the LED on (HIGH is the voltage level)
    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
    digitalWrite(13, LOW);    // turn the LED off by making the voltage LOW
    vTaskDelay( 1000 / portTICK_PERIOD_MS ); // wait for one second
  }
}

void read_test(void* a, Stream* output){
 output->println("Analog read Test Task Read");
}

void TaskAnalogRead(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  Stream* outs[] = {&Serial, (Stream*) NULL };
  
  for (;;)
  {
      sensor_out((void*) NULL, read_test, file_names[6], outs);  
      vTaskDelay(20);  // one tick delay (15ms) in between reads for stability
  }
}

void TaskSensorReadStandard(void *pvParameters){
  (void) pvParameters;

  // Task Setup
  Wire.begin(); //Begining everying on our I2C Bus

  // Initialze sensors
  // These functions should be defined in sensor.h
  initialize_temp_ex(&sensor_temp_ex, Serial);
  initialize_temp_in(&sensor_temp_in, Serial);
  initialize_baro(&sensor_baro, Serial);

  /*
    File has to be open when task starts in order to write data to log. We will
    close it for now, and have each sensor open and close it to ensure we don't
    corrupt our filesystem.
   */
  for(int i = 0; i < num_files; i++){ files[i].close(); }

  Stream* out[] = {&Serial, &Serial3, (Stream*) NULL};      
  for(;;){
    vTaskDelay( 1000 / portTICK_PERIOD_MS );
    sensor_out(&sensor_baro, read_baro, file_names[0], out);   
    sensor_out(&sensor_temp_in, read_temp,file_names[1], out);
    sensor_out(&sensor_temp_ex, read_temp,file_names[2], out); 
    sensor_out((void*) NULL, read_light,file_names[3], out);
    sensor_out((void*) NULL, read_uv, file_names[4], out);
    checkBattery();
    
 //   sensor_out(&sensor_gps, read_gps, file_names[5], out);
  }
}

void TaskCamera(void *pvParameters){
  (void) pvParameters;

  short x = 0;
  int bytes;
  for(;;){
  vTaskDelay(1);
    if ( xSemaphoreTake( xOutputSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
       // Safe to use serial print here
       if(Serial.peek() == TAKE_PHOTO){
         Serial.read();
        if (camera.init()) {
          camera.takePicture();
          Serial.print("Image size: ");
          Serial.println(camera.imageSize, DEC);
          Serial.print("number of packages: ");
          Serial.println(camera.numberOfPackages(), DEC);

          while ( bytes = camera.getData() ) {
            for (x = 0; x < bytes; x++) {
              Serial.print("0x");
              Serial.print(camera.imgBuffer[x], HEX);
              Serial.print(" ");
            }
            Serial.println("");
          }
          Serial.println("done downloading");

        }
      }
    xSemaphoreGive( xOutputSemaphore );
   }
  }
}


void TaskSensorReadFast(void *pvParameters)
{
  (void) pvParameters;

  Stream* outputs[] = {&Serial, (Stream*) NULL};
  initialize_gyro(&sensor_gyro, Serial);

  for (;;) // A Task shall never return or exit.
  {
    sensor_out(&sensor_gyro, read_gyro, file_names[6], outputs);
    vTaskDelay( 50 / portTICK_PERIOD_MS ); 
  }
}

void TaskGPSRead(void *pvParameters)
{
	(void) pvParameters;

/*
	Stream* outputs[] = {&Serial, (Stream*) NULL};
//	printFloat(gps->location.lat(),gps->location.isValid(),11,6, output);
//	printFloat(gps->location.lng(),gps->location.isValid(),12,6, output);
	for(;;)
	{
		sensor_out(&sensor_gps,read_gps,file_names[5],outputs);
		//while (Serial.available()){
		// sensor_gps.encode(Serial.read());
		//}
		vTaskDelay(1000/portTICK_PERIOD_MS);
	}*/
}
