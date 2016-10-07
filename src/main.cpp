#ifndef UNIT_TEST  // Enable unit tests

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
#include <uCamII.h>
#include <MedianFilter.h>
#include "messages.h" // Defines incoming data header
#include "sensor.h"

// define sensors
Adafruit_MCP9808 sensor_temp_ex = Adafruit_MCP9808();
Adafruit_MCP9808 sensor_temp_in = Adafruit_MCP9808();
Adafruit_BNO055 sensor_gyro = Adafruit_BNO055();
CoolSatBaro sensor_baro;
TinyGPSPlus sensor_gps;
UCAMII camera(Serial1, &Serial);

File file;

// define pins
const int WIRE_CUTTER = 12;

// define tasks
void TaskSensorReadStandard(void *pvParameters);
void TaskSensorReadFast(void *pvParameters);
void TaskDeployBoom(void *pvParameters);
void TaskGPSRead(void *pvParameters);

// define semaphores
SemaphoreHandle_t xOutputSemaphore;
SemaphoreHandle_t xSDSemaphore;

const int num_files = 11;

char *file_names[] = {"baro.csv", "temp_in.csv", "temp_ex.csv", "light.csv", "uv.csv",
                      "gps.csv",  "gyro.csv",    "camera.csv",  "boom.csv",  "time_stamp.csv", "stack.csv"};

File files[num_files];

Stream *input_streams[] = {&Serial, &Serial3, (Stream *)nullptr};

/**
 *Global setup should occur here
 */

void setup()
{

  Serial.begin(9600);
  Serial1.begin(115200);
  Serial2.begin(9600);
  Serial3.begin(9600);

  Serial.println("\n\nInitializing SD card...");
  if (!SD.begin(46))
  {
    Serial.println("\nSD card failed to initialize!");
  }
  Serial.println("\nSD Initialized\n\n");

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

  Wire.begin(); // Begining everying on our I2C Bus

  // Initialize sensors
  // These functions should be defined in sensor.h
  initialize_temp_ex(&sensor_temp_ex, Serial);
  initialize_temp_in(&sensor_temp_in, Serial);
  initialize_baro(&sensor_baro, Serial);

  // Initialize switches
  pinMode(WIRE_CUTTER, OUTPUT);

  // Now set up two tasks to run independently
  xTaskCreate(
      TaskSensorReadStandard, (const portCHAR *)"ReadSensors", 700 // Stack size
      ,
      nullptr,
      1 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,
      nullptr);

  xTaskCreate(
      TaskSensorReadFast, (const portCHAR *)"ReadSensorsFast", 512 // Stack size
      ,
      nullptr,
      1 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
      ,
      nullptr);

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

  Stream *out[] = {&Serial, &Serial3, (Stream *)nullptr};
  message_out("barometer\ttemp-in\ttemp-ex\tlight\tuv\ttimestamp\n", out);

 // sensor_out(&analyze, read_stack, file_names[10], out);

  for (;;)
  {
    vTaskDelay(1000 / portTICK_PERIOD_MS);\
    sensor_out(&analyze, read_stack, file_names[10], nullptr);
    sensor_out(&sensor_baro, read_baro, file_names[0], out);
    sensor_out(&sensor_temp_in, read_temp, file_names[1], out);
    sensor_out(&sensor_temp_ex, read_temp, file_names[2], out);
    sensor_out((void *)nullptr, read_light, file_names[3], out);
    sensor_out((void *)nullptr, read_uv, file_names[4], out);
    sensor_out((void *)nullptr, read_timestamp, file_names[9], out);
    sensor_out(&sensor_gps, read_gps, file_names[5], out);
    checkBattery();
    message_out("\n", out);
  }
}

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

// bool shouldDeploy(bool hasDeployed, bool* confirmed, float pressure ){
//   return true;
// }

void TaskDeployBoom(void *pvParameters)
{
  (void)pvParameters;

  StackAnalyzer analyze(nullptr, "DeployBoom");

  const float DEPLOY_MIN_PRESSURE = 30.0;
  const float DEPLOY_MAX_PRESSURE = 44.0;

  Stream *out[] = {&Serial, &Serial3, (Stream *)nullptr};
  Stream *camera_out[] = {(Stream *)nullptr};

  boom.deployed = false;

  char *camera_messages[] = {"\n****************Camera Taking Photo*****************\n",
                             "\n****************Camera Done Taking Photo*****************\n"};

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

    char received_message = 0;
    // We don't expect to receive commands from two streams at the same time. So this
    // overwriting of the message shouldn't be a problem.

    for (char i = 0; input_streams[i] != nullptr; ++i)
    {
      if (input_streams[i]->available())
      {
        received_message = input_streams[i]->read();
        while (input_streams[i]->available())
        {
          input_streams[i]->read();
        }
        // Clear the rest of the buffer
      }
    }

    if (received_message == DEPLOY_BOOM)
    {
      critical_out((void *)nullptr, print_confirm, file_names[8], out);
      deployInitiated = true;
    }

    if (received_message == CONFIRM_DEPLOY && deployInitiated == true)
      deployConfirmed = true;

    if (received_message == CANCEL_DEPLOY && deployInitiated == true)
    {
      critical_out((void *)nullptr, print_cancel, file_names[8], out);
      deployInitiated = false;
    }

    // If boom hasn't deployed yet AND ('y' was pressed OR pressure is within range)
    if (!boom.deployed && (deployConfirmed == true || (valPressure <= DEPLOY_MAX_PRESSURE && valPressure > DEPLOY_MIN_PRESSURE)))
    {
      critical_out((void *)nullptr, print_boom, file_names[8], out);
      digitalWrite(WIRE_CUTTER, HIGH); // INITIATE THERMAL INCISION
      vTaskDelay(3000 / portTICK_PERIOD_MS);
      digitalWrite(WIRE_CUTTER, LOW); // Disengage
      vTaskDelay(1000 / portTICK_PERIOD_MS);
      boom.deployed = true;

      // take picture after boom deployment
      critical_out(&camera, read_camera, file_names[7], camera_out, out, camera_messages);
    }

    if (received_message == TAKE_PHOTO)
    {
      critical_out(&camera, read_camera, file_names[7], camera_out, out, camera_messages);
    }
    
    sensor_out(&analyze, read_stack, file_names[10], nullptr);
  }
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
