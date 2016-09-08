#ifndef SENSORS_H
#define SENSORS_H

#include "data.h"
#include "list.h"

// ------------ FreeRTOS Handling ------------ //

void read_timestamp(void *dummy, Stream *output);

bool message_peek(Stream **stream, char message, char &read_count, char num_readers)
{
  for (char i = 0; stream[i] != nullptr; i++)
  {
    if (stream[i]->peek() == message)
    {
      while (stream[i]->available())
      {
        stream[i]->read();
      }
      read_count = 0;
      return true;
    }
  }

  if (++read_count >= num_readers)
  {
    read_count = 0;
    for (char i = 0; stream[i] != nullptr; i++)
    {
      while (stream[i]->available())
      {
        stream[i]->read();
      }
    }
  }
  return false;
}

extern SemaphoreHandle_t xOutputSemaphore;
extern SemaphoreHandle_t xSDSemaphore;
extern File file;

template <typename F, typename S>
inline void sensor_out(S sensor, F func, char *file_name, Stream **outputs, char priority = 5)
{

  if (xSemaphoreTake(xOutputSemaphore, (TickType_t)priority) == pdTRUE)
  {
    for (int i = 0; outputs[i] != nullptr; i++)
    {
      func(sensor, outputs[i]);
      outputs[i]->print('\t');
    }
    xSemaphoreGive(xOutputSemaphore);
  }

  if (xSemaphoreTake(xSDSemaphore, (TickType_t)5) == pdTRUE)
  {
    file = SD.open(file_name, FILE_WRITE);
    read_timestamp((void *)nullptr, &file);
    file.print(", ");
    func(sensor, &file);
    file.println();
    file.close();
    xSemaphoreGive(xSDSemaphore);
  }
}

template <typename F, typename S>
inline void critical_out(S sensor, F func, char *file_name, Stream **outputs,
                         Stream **alert_outputs = (Stream **)nullptr,
                         char **status_messages = (char **)NULL)
{
  if (status_messages && alert_outputs)
  {
    for (int i = 0; alert_outputs[i] != nullptr; i++)
    {
      alert_outputs[i]->print(status_messages[0]);
    }
  }

  while (xSemaphoreTake(xOutputSemaphore, (TickType_t)15) != pdTRUE)
  {
    ;
  }
  for (int i = 0; outputs[i] != nullptr; i++)
  {
    func(sensor, outputs[i]);
    outputs[i]->print('\t');
  }
  xSemaphoreGive(xOutputSemaphore);

  while (xSemaphoreTake(xSDSemaphore, (TickType_t)15) != pdTRUE)
  {
    ;
  }
  file = SD.open(file_name, FILE_WRITE);
  read_timestamp((void *)nullptr, &file);
  file.print(", ");
  func(sensor, &file);
  file.println();
  file.close();
  xSemaphoreGive(xSDSemaphore);

  if (status_messages && alert_outputs)
  {
    for (int i = 0; alert_outputs[i] != nullptr; i++)
    {
      alert_outputs[i]->print(status_messages[1]);
    }
  }
}

inline void message_out(char *message, Stream **outputs)
{
  if (xSemaphoreTake(xOutputSemaphore, (TickType_t)5) == pdTRUE)
  {
    for (int i = 0; outputs[i] != nullptr; i++)
    {
      outputs[i]->println(message);
    }
    xSemaphoreGive(xOutputSemaphore);
  }
}

//----------- Temperature sensors ------------//

void initialize_temp_ex(Adafruit_MCP9808 *sensor, Stream &output)
{
  if (!sensor->begin(0x18))
  {
    output.println("Couldn't find external MCP9808!");
  }
}

void initialize_temp_in(Adafruit_MCP9808 *sensor, Stream &output)
{
  if (!sensor->begin(0x1D))
  {
    output.println("Couldn't find internal MCP9808!");
  }
}

void read_temp(Adafruit_MCP9808 *sensor, Stream *output)
{
  float val = sensor->readTempC();
  output->print(val);

  if (sensor->begin(0x18))
    temp.external = val;
  else
    temp.internal = val;
}

//------------ Barometer ------------//

void initialize_baro(CoolSatBaro *sensor, Stream &output) { sensor->initial(0x76); }

void read_baro(CoolSatBaro *sensor, Stream *output)
{
  sensor->readBaro();
  float val = sensor->getPressure();
  float alt = sensor->getAltitude();
  output->print(val);
  output->print(',');
  output->print(alt);

  baro.pressure = val;
  baro.altitude = alt;
}

//------------ Light & UV sensors ------------//

void read_light(void *dummy, Stream *output)
{
  float lightPin = 15;           // analog light pin #
  float volt = 0.0;              // voltage (volts)
  float RLDR = 0.0;              // resistance (ohms)
  float lux = 0.0;               // brightness (lumens/m2)
  const float TOVOLT = .0048;    // converts sensor output to volts
  const float TOLUX = 776897.0;  // converts to lux
  const float TOLUXPWR = -1.206; // converts to lux

  volt = analogRead(lightPin) * TOVOLT;
  RLDR = (1000.0 * (5 - volt)) / volt;
  lux = TOLUX * (pow(RLDR, TOLUXPWR));

  output->print(lux);

  light.lux = lux;
}

void read_uv(void *dummy, Stream *output)
{
  const int uvPin = 1;       // UV sensor pin
  int v = analogRead(uvPin); // reads value
  float uv = 5 / 1023.0 * v * 10;
  output->print(uv);

  light.uv = uv;
}

//------------ Gyroscope ------------//

void initialize_gyro(Adafruit_BNO055 *sensor_gyro, Stream &output)
{
  if (!sensor_gyro->begin())
  {
    output.println("Couldn't detect BNO055 gyroscope ... Check your wiring or I2C ADDR!");
  }
  sensor_gyro->setExtCrystalUse(true);
}

void read_gyro(Adafruit_BNO055 *sensor_gyro, Stream *output)
{
  imu::Vector<3> acceleration = sensor_gyro->getVector(Adafruit_BNO055::VECTOR_ACCELEROMETER);

  output->print(acceleration.x());
  output->print(",");
  output->print(acceleration.y());
  output->print(",");
  output->print(acceleration.z());
  output->print(",");

  imu::Vector<3> euler = sensor_gyro->getVector(Adafruit_BNO055::VECTOR_EULER);

  output->print(euler.x());
  output->print(",");
  output->print(euler.y());
  output->print(",");
  output->print(euler.z());
  output->print(",");

  gyro.accelX = acceleration.x();
  gyro.accelY = acceleration.y();
  gyro.accelZ = acceleration.z();

  gyro.eulerX = euler.x();
  gyro.eulerY = euler.y();
  gyro.eulerZ = euler.z();

  delay(100); // Delay of 100ms TODO needed?
}

//------------- GPS ---------------//

// GPS must be constantly be fed characters (done in controlling task)

void read_gps(TinyGPSPlus *sensor_gps, Stream *output)
{

  output->print(sensor_gps->location.lat(), 8);
  output->print(",");
  output->print(sensor_gps->location.lng(), 8);

  gps.lat = sensor_gps->location.lat();
  gps.lng = sensor_gps->location.lng();
}
//------------ Clock ------------//

void printTime(int time, Stream *output)
{
  if (time >= 0 && time < 10)
  {                     // Prefaces times less than 10 with a 0
    output->print("0"); // e.g., converts "12:8:9" to "12:08:09"
    output->print(time);
  }
  else
    output->print(time);
}

void read_timestamp(void *dummy, Stream *output)
{
  tmElements_t tm; // magic getter of time from TimeLib.h

  if (RTC.read(tm))
  {
    printTime(tm.Hour, output);
    output->print(":");
    printTime(tm.Minute, output);
    output->print(":");
    printTime(tm.Second, output);
  }
  else
    output->println("Error: Failed to fetch time");

  time.hour = tm.Hour;
  time.minute = tm.Minute;
  time.second = tm.Second;
}

//------------ Boom ------------//

void print_boom(void *dummy, Stream *output)
{
  output->print("\n\n***************** !!! DEPLOYING BOOM !!! **********************\n\n");
}

void print_confirm(void *dummy, Stream *output)
{
  output->print("\n\n***************** DEPLOYMENT COMMAND DETECTED. PRESS 'y' TO CONFIRM OR 'n' TO "
                "CANCEL **********************\n\n");
}

void print_cancel(void *dummy, Stream *output)
{
  output->print("\n\n***************** Deployment cancelled **********************\n\n");
}

//------------ Battery ------------//

void checkBattery()
{
  const int batteryPin = 0;
  const int powerOff = 10;
  float battery = 0.0;

  pinMode(powerOff, OUTPUT);
  digitalWrite(powerOff, HIGH);

  battery = analogRead(batteryPin);
  battery = (battery * .00475) * 2;

  if (battery <= 6.3)
  {
    digitalWrite(powerOff, LOW);
    delay(1000);
  }
}

// ----------------- Camera -------------- //
void read_camera(UCAMII *camera, Stream *output)
{
  short x = 0;
  int bytes;
  if (camera->init())
  {
    camera->takePicture();
    while (bytes = camera->getData())
    {
      for (x = 0; x < bytes; x++)
      {
        output->print("0x");
        output->print(camera->imgBuffer[x], HEX);
        output->print(" ");
      }
    }
    output->println("\n\n\n\n");
  }
}

// ------------ Data Validation ------------ //

void swap(float & a, float & b)
{
  auto temp = a;
  a = b;
  b = temp;
}

int partition(LinkedList<float> & list, int start, int end, int pivot)
{
  swap(list[pivot], list[end - 1]);
  int firstGreater = start;
  for (int i = start; i < end - 1; ++i)
  {
    if (list[i] < = list [end - 1])
    {
      swap(list[i], list[firstGreater]);
      ++firstGreater;
    }
  }
  swap(list[end - 1], list[firstGreater]);
  return firstGreater;
}

int getPivot(LinkedList<float> & list, int start, int end)
{
  auto first = list[start];
  auto last = list[end - 1];
  auto mid = list[(start + end) / 2];
  if ((first <= last && last <= mid) || (mid <= last && last <= first)) return end - 1;
  else if ((last <= first && first <= mid) || (mid <= first && first <= last)) return start;
  else if ((first <= mid && mid <= last) || (last <= mid && mid <= first)) return (start + end)/2;
  else return mid;
}

void quickSort(LinkedList<float> & list, int start, int end)
{
  if (end - start <= 1) return;
  auto pivot = getPivot(list, start, end);
  pivot = partition(list, start, end, pivot);

  quickSort(list, start, pivot);
  quickSort(list, pivot, end);
}

float getMedian(float reading, const int RANGE)
{
  static LinkedList<float> list;
  auto median = 0;

  list.push_back(reading);
  auto size = list.size();

  if (size >= RANGE)
  {
    auto tempList = list;
    quickSort(tempList, 0, tempList.size()); 
    median = tempList[ size / 2 ];

    list.pop_front();
  }
  
  return median;
}

#endif

