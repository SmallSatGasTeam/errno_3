#include <Arduino_FreeRTOS.h>

#include <Wire.h>
#include <CoolSatBaro.h>
#include <Adafruit_MCP9808.h>

// define two tasks for Blink & AnalogRead
void TaskBlink( void *pvParameters );
void TaskAnalogRead( void *pvParameters );

void TaskSensorRead(void *pvParameters);

// the setup function runs once when you press reset or power the board
void setup() {

  // Now set up two tasks to run independently.
  xTaskCreate(
    TaskBlink
    ,  (const portCHAR *) "Blink"   // A name just for humans
    ,  128  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL
    ,  2  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

//  xTaskCreate(
//    TaskAnalogRead
//    ,  (const portCHAR *) "AnalogRead"
//    ,  128  // Stack size
//    ,  NULL
//    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
//    ,  NULL );

xTaskCreate(
    TaskSensorRead
    ,  (const portCHAR *) "ReadSensors"
    ,  128  // Stack size
    ,  NULL
    ,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL );

  // Now the task scheduler, which takes over control of scheduling individual tasks, is automatically started.
}

void loop()
{
  // Empty. Things are done in Tasks.
}

/**
 * Sensor Read Functions
 */

void read_temp(Adafruit_MCP9808* sensor){
  Serial.println(sensor->readTempC());
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

  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);

  for (;;)
  {
    // read the input on analog pin 0:
    int sensorValue = analogRead(A0);
    // print out the value you read:
    Serial.println("Analog read");
    vTaskDelay(1);  // one tick delay (15ms) in between reads for stability
  }
}


void TaskSensorRead(void *pvParameters){

  // Task Setup
  Wire.begin(); //Begining everying on our I2C Bus
  Serial.begin(9600);

  // Create sensor instances
  Adafruit_MCP9808 sensor_temp = Adafruit_MCP9808();
  CoolSatBaro myBaro;

  // Initialize Sensors
  if (!sensor_temp.begin()) {
    Serial.println("Couldn't find MCP9808!");
    while (1);
  }
  myBaro.initial(0x76);

  int readIntervals[] = {1000,10}; // How often to execute in milliseconds
  unsigned int lastRead[2]; // To store last read time

  for(;;){
    unsigned int now = millis();

    if(now - lastRead[0] > readIntervals[0]){
      readIntervals[0] = now;

      read_temp(&sensor_temp);
      myBaro.readBaro();
      Serial.println(myBaro.getPressure());

    } else if(now - lastRead[1] > readIntervals[1]) {
      readIntervals[1] = now;

    }
  }
}
