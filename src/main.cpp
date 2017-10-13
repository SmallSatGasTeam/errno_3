#ifndef UNIT_TEST  // Enable unit tests

const float DEPLOY_MIN_PRESSURE = 10.0;
const float DEPLOY_MAX_PRESSURE = 44.0;

#include <Adafruit_BNO055.h>
#include <Adafruit_MCP9808.h>
#include <Adafruit_Sensor.h>
#include <Arduino_FreeRTOS.h>
#include <CoolSatBaro.h>
#include <DS1307RTC.h>
#include <SD.h>
#include <SPI.h>
#include <TimeLib.h>
#include <TinyGPS++.h>
#include <Wire.h>
#include <semphr.h> // add the FreeRTOS functions for Semaphores (or Flags).
//#include <uCamII.h>
#include <MedianFilter.h>
#include <BoomDeploy.hpp>
#include "messages.h" // Defines incoming data header
#include "sensor.h"

// define sensors
Adafruit_MCP9808 sensor_temp_ex = Adafruit_MCP9808();
Adafruit_MCP9808 sensor_temp_in = Adafruit_MCP9808();
Adafruit_BNO055 sensor_gyro = Adafruit_BNO055();
CoolSatBaro sensor_baro;
TinyGPSPlus sensor_gps;
//UCAMII camera(Serial1, &Serial);

File file;

// define pins
const int WIRE_CUTTER = 12;
const int SECONDARY_WIRE_CUTTER = 6; // TODO: define the pin here for a secondary wire cutter
const int BOOM_SWITCH = 30;

// define tasks
void TaskSensorReadStandard(void *pvParameters);
void TaskSensorReadFast(void *pvParameters);
void TaskDeployBoom(void *pvParameters);
void TaskGPSRead(void *pvParameters);

void cutWire(int delay, int wirePin, Stream **out);

// define semaphores (mutex)
SemaphoreHandle_t xOutputSemaphore;
SemaphoreHandle_t xSDSemaphore;

const int num_files = 13;

char *file_names[] = {"baro.csv", "temp_in.csv", "temp_ex.csv", "light.csv", "uv.csv",
                      "gps.csv",  "gyro.csv",    "camera.csv",  "boom.csv",  "time_stamp.csv", 
                      "median.csv",  "stack.csv", "voltage.csv"};

File files[num_files];

Stream *input_streams[] = {&Serial, &Serial3, (Stream *)nullptr};

/**
 *Global setup should occur here
 */
// the setup function runs once when you press reset or power the board
void setup()
{

  Serial.begin(9600);
  Serial1.begin(115200);
  Serial2.begin(9600);
  Serial3.begin(9600);

  // Serial.println("\n\nInitializing SD card...");
  if (!SD.begin(46))
  {
    // Serial.println("\nSD card failed to initialize!");
  }
  // Serial.println("\nSD Initialized\n\n");

  for (int i = 0; i < num_files; i++)
  {
    files[i] = SD.open(file_names[i], FILE_WRITE);
  }

  // initialize Mutex
  if (xOutputSemaphore == nullptr)
  {
    xOutputSemaphore = xSemaphoreCreateMutex();
    if (xOutputSemaphore)
    {
      xSemaphoreGive(xOutputSemaphore);
    }
  }

  if (xSDSemaphore == nullptr)
  {
    xSDSemaphore = xSemaphoreCreateMutex();
    if (xSDSemaphore)
    {
      xSemaphoreGive(xSDSemaphore);
    }
  }

  Wire.begin(); // Beginning everying on our I2C Bus

  // Initialize sensors
  // These functions should be defined in sensor.h
  initialize_temp_ex(&sensor_temp_ex, Serial);
  initialize_temp_in(&sensor_temp_in, Serial);
  initialize_baro(&sensor_baro, Serial);

  // Initialize switches
  pinMode(WIRE_CUTTER, OUTPUT);
  pinMode(SECONDARY_WIRE_CUTTER, OUTPUT); // TODO: secondary wire cutter
  pinMode(BOOM_SWITCH, INPUT);

  // Now set up multiple tasks to run independently
  xTaskCreate(
      TaskSensorReadStandard, (const portCHAR *)"ReadSensors", 1212 // Stack size
      ,
      nullptr,
      1 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,
      nullptr);

      /*
  xTaskCreate(
      TaskSensorReadFast, (const portCHAR *)"ReadSensorsFast", 512 // Stack size
      ,
      nullptr,
      1 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,
      nullptr);
      */

  xTaskCreate(
      TaskGPSRead, (const portCHAR *)"GPSRead", 512 // Stack size
      ,
      nullptr,
      1 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,
      nullptr);

  xTaskCreate(
      TaskDeployBoom, (const portCHAR *)"Deploy Boom and take photos", 1024 // Stack size
      ,
      nullptr,
      1 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,
      nullptr);

  // Now the task scheduler, which takes over control of scheduling individual tasks, is
  // automatically started.
}
void loop()
{
  // Empty. Things are done in Tasks.
}

/*--------------------------------------------------*/
/*---------------------- Tasks ---------------------*/
/*--------------------------------------------------*/

void TaskSensorReadStandard(void *pvParameters)
{
  (void)pvParameters;
  StackAnalyzer analyze(nullptr, "SensorReadStandard");

  /*
     File has to be open when task starts in order to write data to log. We will
     close it for now, and have each sensor open and close it to ensure we don't
     corrupt our filesystem.
     */
  for (int i = 0; i < num_files; i++)
  {
    files[i].close();
  }

  initialize_gyro(&sensor_gyro, Serial);

  // Stream *out[] = {&Serial, &Serial3, (Stream *)nullptr};
  Stream *out[] = {&Serial3, (Stream *)nullptr, (Stream *)nullptr};

  
  message_out("\n barometer\ttemp-in\ttemp-ex\tlight\tuv\ttimestamp\tvoltage\t", out);
  for (;;)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    sensor_out(&analyze, read_stack, file_names[11], nullptr);
    sensor_out(&sensor_baro, read_baro, file_names[0], out);
    sensor_out(&sensor_temp_in, read_temp_in, file_names[1], out);
    sensor_out(&sensor_temp_ex, read_temp_ex, file_names[2], out);
    sensor_out((void *)nullptr, read_light, file_names[3], out);
    sensor_out((void *)nullptr, read_uv, file_names[4], out);
    sensor_out((void *)nullptr, read_timestamp, file_names[9], out);
    checkBattery();
    sensor_out((void *)nullptr, print_voltage, file_names[12], out);
    sensor_out(&sensor_gps, read_gps, file_names[5], out);
    sensor_out(&sensor_gyro, read_gyro, file_names[6], out);
    
    if (checkBoomSwitch(BOOM_SWITCH))
    {
      message_out("open", out);
    }
    else 
    {
      message_out("closed", out);
    }

    message_out("", out); // for correct spacing, since it uses println()
  }
}

/*
void TaskSensorReadFast(void *pvParameters)
{
  (void)pvParameters;
  StackAnalyzer analyze(nullptr, "SensorReadFast");

  Stream *outputs[] = {(Stream *)nullptr};
  initialize_gyro(&sensor_gyro, Serial);

  for (;;) // A Task shall never return or exit.
  {
    sensor_out(&sensor_gyro, read_gyro, file_names[6], outputs);
    sensor_out(&analyze, read_stack, file_names[10], nullptr);
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
*/

// bool shouldDeploy(bool hasDeployed, bool* confirmed, float pressure ){
//   return true;
// }

void TaskDeployBoom(void *pvParameters)
{
  (void)pvParameters;

  const float DEPLOY_MIN_PRESSURE = 30.0;
  const float DEPLOY_MAX_PRESSURE = 44.0;

  // Stream *out[] = {&Serial, &Serial3, (Stream *)nullptr};
  Stream *out[] = {&Serial3, (Stream *)nullptr, (Stream *)nullptr};
  //Stream *camera_out[] = {(Stream *)nullptr};

  StackAnalyzer analyze(nullptr, "TaskDeployBoom");

  boom.deployed = false;

  //char *camera_messages[] = {"\n****************Camera Taking Photo*****************\n",
  //                           "\n****************Camera Done Taking Photo*****************\n"};

  bool deployInitiated = false;
  bool deployConfirmed = false;

  const int filterOrder = 32;
  float readingsByTime[filterOrder] = {0};
  float readingsByValue[filterOrder] = {0};
  MedianFilter<float> filter(readingsByTime, readingsByValue, filterOrder);

  for (;;)
  {
    filter.addDataPoint(baro.pressure);
    float valPressure = filter.getFilteredDataPoint();
    baro.median = valPressure;

    // sensor_out((void *)nullptr, print_median, file_names[10], out);

    char received_message = getMessage(out);

    switch (received_message) {
      case DEPLOY_BOOM:
        critical_out((void *)nullptr, print_confirm, file_names[8], out);
        deployInitiated = true;
      break;

      case CONFIRM_DEPLOY:
        if(deployInitiated == true){
          deployConfirmed = true;
        }
      break;

      case CANCEL_DEPLOY:
        if(deployInitiated){
          critical_out((void *)nullptr, print_cancel, file_names[8], out);
          deployInitiated = false;
        }
      break;

      /*
      case TAKE_PHOTO:
        Serial.println("picture");
      //   critical_out(&camera, read_camera, file_names[7], camera_out, out, camera_messages);
      break;
      */
    }

    // bool shouldDeployBoom(bool deployed, bool initialized, bool confirmed, float pressure)
    // If boom hasn't deployed yet AND ('y' was pressed OR pressure is within range)
    // if (!boom.deployed && (deployConfirmed == true || (valPressure <= DEPLOY_MAX_PRESSURE && valPressure > DEPLOY_MIN_PRESSURE)))
    if(shouldDeployBoom(boom.deployed, deployInitiated, deployConfirmed, valPressure) == true)
    { 
      bool primaryCutter = false;
      int delayTime = 3000;

      // making sure it heats up primary wire at least once
      cutWire(delayTime, WIRE_CUTTER, out);

      while (checkBoomSwitch(BOOM_SWITCH) == false && delayTime <= 5000)
      {
        if (primaryCutter)
        {
          cutWire(delayTime, WIRE_CUTTER, out);
        }
        else
        {
          cutWire(delayTime, SECONDARY_WIRE_CUTTER, out); // TODO: implement secondary wire cutter
        }

        if (checkBoomSwitch(BOOM_SWITCH) == false && primaryCutter == false)
        {
          delayTime += 500; // increment by half a second more
        }
        
        primaryCutter = !primaryCutter; // boolean toggle
      }

      if (checkBoomSwitch(BOOM_SWITCH) == false && delayTime > 5000)
      {
        critical_out((void *)nullptr, print_boom_failure, file_names[8], out);
      }
      else
      {
        critical_out((void *)nullptr, print_boom, file_names[8], out);

        // take picture after boom deployment
        // critical_out(&camera, read_camera, file_names[7], camera_out, out, camera_messages);
      }

      boom.deployed = true;
      deployInitiated = false;
      deployConfirmed = false;
    }

    sensor_out(&analyze, read_stack, file_names[11], nullptr);
  }
}

void cutWire(int delay, int wirePin, Stream **out)
{
  digitalWrite(wirePin, HIGH); // INITIATE THERMAL INCISION
  vTaskDelay(delay / portTICK_PERIOD_MS);
  digitalWrite(wirePin, LOW); // Disengage
  vTaskDelay(1000 / portTICK_PERIOD_MS);

  Serial.println("Picture");
}

void TaskGPSRead(void *pvParameters)
{ 
  (void)pvParameters;

  StackAnalyzer analyze(nullptr, "GPSRead");

  Stream *outputs[] = {(Stream *)nullptr};
  for (;;)
  {
    while (Serial2.available())
    {
      sensor_gps.encode(Serial2.read());
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
    sensor_out(&analyze, read_stack, file_names[10], nullptr);
  }
}

#endif
