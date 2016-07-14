#include <Arduino_FreeRTOS.h>
#include <semphr.h>  // add the FreeRTOS functions for Semaphores (or Flags).

#include <Wire.h>
#include <CoolSatBaro.h>
#include <Adafruit_MCP9808.h>
#include <uCamII.h>

#include "sensor.h"
#include "messages.h" // Defines incoming data headers

// define tasks
void TaskBlink( void *pvParameters ); //TODO remove this test task
void TaskAnalogRead( void *pvParameters ); //TODO remove this test task
void TaskSensorRead(void *pvParameters);
void TaskCamera(void *pvParameters);

// define semaphores
SemaphoreHandle_t xSerialSemaphore;

/**
 * Global setup should occur here
 */
void setup() {

  Serial.begin(9600);

  if (xSerialSemaphore == NULL)  // Check to confirm that the Serial Semaphore has not already been created.
  {
    xSerialSemaphore = xSemaphoreCreateMutex();  // Create a mutex semaphore we will use to manage the Serial Port
    if (xSerialSemaphore){
      xSemaphoreGive(xSerialSemaphore);  // Make the Serial Port available for use, by "Giving" the Semaphore.
    }
  }

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskBlink
    ,  (const portCHAR *) "Blink"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

 xTaskCreate(
   TaskAnalogRead
   ,  (const portCHAR *) "AnalogRead"
   ,  128  // Stack size
   ,  NULL
   ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
   ,  NULL );

xTaskCreate(
    TaskSensorRead
    ,  (const portCHAR *) "ReadSensors"
    ,  256  // Stack size
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

xTaskCreate(
    TaskCamera
    ,  (const portCHAR *) "Take Photos"
    ,  256  // Stack size
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

void TaskAnalogRead(void *pvParameters)  // This is a task.
{
  (void) pvParameters;

  for (;;)
  {

    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )
    {
      Serial.println("Analog read Test Task Read");
      xSemaphoreGive( xSerialSemaphore ); // Now free or "Give" the Serial Port for others.
    }
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}


void TaskSensorRead(void *pvParameters){
  (void) pvParameters;

  // Task Setup
  Wire.begin(); //Begining everying on our I2C Bus

  // Create sensor instances
  Adafruit_MCP9808 sensor_temp = Adafruit_MCP9808();
  CoolSatBaro sensor_baro;

  // Initialze sensors
  // These functions should be defined in sensor.h
  initialize_temp(&sensor_temp);
  initialize_baro(&sensor_baro);

  int readIntervals[] = {1000,10}; // How often to execute in milliseconds
  unsigned int lastRead[2]; // To store last read time

  for(;;){
    unsigned int now = millis();

    if(now - lastRead[0] > readIntervals[0]){
      lastRead[0] = now;
      if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
        // Safe to use serial print here
        read_temp(&sensor_temp);
        read_baro(&sensor_baro);
        Serial.println("Test Task Read Sensors");

        xSemaphoreGive( xSerialSemaphore );
      }
    } else if(now - lastRead[1] > readIntervals[1]) {
      lastRead[1] = now;

    }
  }
}

void TaskCamera(void *pvParameters){
  (void) pvParameters;


  UCAMII camera;
  short x = 0;
  int bytes;

  bool taken = false; //HACK

  for(;;){
    // semaphore
    //   if incomingMessage
    //       // Take photo
    //       // transmit back

    if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE ){
      // Safe to use serial print here

      // if(Serial.peek() == TAKE_PHOTO){ //HACK
      if(!taken){
        taken = true; //HACK
        //take photo
        //transmit back
        if (camera.init()) {
          camera.takePicture();
          Serial.print("Image size: ");
          Serial.println(camera.imageSize, DEC);
          Serial.print("number of packages: ");
         // Serial.println(camera.numberOfPackages(), DEC);

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

      xSemaphoreGive( xSerialSemaphore );
    }


  }
}