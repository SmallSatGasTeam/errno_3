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
#include "sensor.hpp"

// define sensors
// Adafruit_MCP9808 sensor_temp_ex = Adafruit_MCP9808();
// Adafruit_MCP9808 sensor_temp_in = Adafruit_MCP9808();
// Adafruit_BNO055  sensor_gyro = Adafruit_BNO055();
// CoolSatBaro sensor_baro;
// TinyGPSPlus sensor_gps;
// UCAMII camera(Serial1, &Serial);

TempSensor temp_in("temp_in", 0x18);
TempSensor temp_out("temp_out", 0x1D);


Sensor* sensors[] = {&temp_in, &temp_out, (Sensor*) NULL};

//TODO Add read sensor method
//TODO figure out sprintf
//TODO test sd card writing

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

const int num_files = 10;

// char* file_names[] = {
// 	"baro.csv",
// 	"temp_in.csv",
// 	"temp_ex.csv",
// 	"light.csv",
// 	"uv.csv",
// 	"gps.csv",
// 	"gyro.csv",
// 	"camera.csv",
// 	"boom.csv",
// 	"time_stamp.csv"
// };

File files[num_files];

Stream* input_streams[] = {&Serial, &Serial3, (Stream*) NULL };

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



	// for(int i = 0; i < num_files; i++){
	// 	files[i] = SD.open(file_names[i], FILE_WRITE);
	// }

	// initialize Mutex
	if (xOutputSemaphore == NULL) {
		xOutputSemaphore = xSemaphoreCreateMutex();
		if(xOutputSemaphore){ xSemaphoreGive(xOutputSemaphore);}
	}

	if (xSDSemaphore == NULL) {
		xSDSemaphore = xSemaphoreCreateMutex();
		if(xSDSemaphore){ xSemaphoreGive(xSDSemaphore);}
	}

	Wire.begin(); //Begining everying on our I2C Bus

	// Initialize sensors
	// These functions should be defined in sensor.h
	// initialize_temp_ex(&sensor_temp_ex, Serial);
	// initialize_temp_in(&sensor_temp_in, Serial);
	// initialize_baro(&sensor_baro, Serial);

	// Initialize switches
	pinMode(WIRE_CUTTER, OUTPUT);

	// Now set up two tasks to run independently
	xTaskCreate(
			TaskSensorReadStandard
			,  (const portCHAR *) "ReadSensors"
			,  700  // Stack size
			,  NULL
			,  1  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
			,  NULL );

	xTaskCreate(
			TaskSensorReadFast
			,  (const portCHAR *) "ReadSensorsFast"
			,  512  // Stack size
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

	xTaskCreate(
			TaskDeployBoom
			,  (const portCHAR *) "Deploy Boom"
			,  1024 // Stack size
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

void TaskSensorReadStandard(void *pvParameters){
	(void) pvParameters;

	/*
		 File has to be open when task starts in order to write data to log. We will
		 close it for now, and have each sensor open and close it to ensure we don't
		 corrupt our filesystem.
		 */
	for(int i = 0; i < num_files; i++){ files[i].close(); }

	Stream* out[] = {&Serial, &Serial3, (Stream*) NULL};
        message_out("barometer\ttemp-in\ttemp-ex\tlight\tuv\ttimestamp\n", out);
	for(;;){
		// vTaskDelay( 1000 / portTICK_PERIOD_MS );
		// sensor_out(&sensor_baro, read_baro, file_names[0], out);
		// sensor_out(&sensor_temp_in, read_temp,file_names[1], out);
		// sensor_out(&sensor_temp_ex, read_temp,file_names[2], out);
		// sensor_out((void*) NULL, read_light,file_names[3], out);
		// sensor_out((void*) NULL, read_uv, file_names[4], out);
		// sensor_out((void*) NULL, read_timestamp, file_names[9], out);
		// sensor_out(&sensor_gps,read_gps,file_names[5],out);
		// checkBattery();
    // message_out("\n", out);
	}
}

void TaskSensorReadFast(void *pvParameters)
{
	(void) pvParameters;

	Stream* outputs[] = {(Stream*) NULL};
	// initialize_gyro(&sensor_gyro, Serial);

	for (;;) // A Task shall never return or exit.
	{
		// sensor_out(&sensor_gyro, read_gyro, file_names[6], outputs);
		// vTaskDelay( 50 / portTICK_PERIOD_MS );
	}
}

void TaskDeployBoom(void *pvParameters){
	(void) pvParameters;

	Stream* out[] = {&Serial, &Serial3, (Stream*) NULL};
	Stream* camera_out[] = {(Stream*) NULL};
	bool deployed = false;

	float pressure;
        char* camera_messages[] = {
          "\n****************Camera Taking Photo*****************\n",
          "\n****************Camera Done Taking Photo*****************\n"
        };

	for(;;)
	{

		// pressure = sensor_baro.getPressure();

		char received_message = 0;
		// We don't expect to receive commands from two streams at the same time. So this
		// Overwriting the message shouldn't be a problem.
		for(char i = 0; input_streams[i] != NULL; i++){
			if(input_streams[i]->available()){
				received_message = input_streams[i]->read();
				while(input_streams[i]->available()){input_streams[i]->read();} // Clear the rest of the buffer
			}
		}

		// if 'b' is pressed OR (pressure falls below 44 AND boom hasn't deployed yet)
		if(received_message == DEPLOY_BOOM || ((pressure <= 44 && pressure > 30) && deployed == false)){
			// critical_out((void*) NULL, print_boom, file_names[8], out);
			digitalWrite(WIRE_CUTTER, HIGH); // INITIATE THERMAL INCISION
			vTaskDelay( 3000 / portTICK_PERIOD_MS );
			digitalWrite(WIRE_CUTTER, LOW); // Disengage
			vTaskDelay( 1000 / portTICK_PERIOD_MS );
			deployed = true;

			//take picture after boom deployment
			// critical_out(&camera, read_camera, file_names[7], camera_out, out, camera_messages);
		}


		if(received_message == TAKE_PHOTO){
		    // critical_out(&camera, read_camera, file_names[7], camera_out, out, camera_messages);
		}
	}
}

void TaskGPSRead(void *pvParameters)
{
	(void) pvParameters;
	Stream* outputs[] = {(Stream*) NULL};
	for(;;)
	{
		while (Serial2.available()){
			// sensor_gps.encode(Serial2.read());
		}
		vTaskDelay(500/portTICK_PERIOD_MS);
	}
}
