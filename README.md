# ERRNO
## Elevated Real-time Research Near-Earth Observer

A CubeSat deployment system based on FreeRTOS to test the functionality of an inflatable aeroboom in a high-altitude, low-pressure environment to be used as method of passive CubeSat stabilization.

ERRNO 3 is responsible for handling tasks to ensure that data and sensors are functioning correctly as one unit. Sensors
read UV light, visible light, internal and external temperature, barometric pressure, altitude, speed and acceleration, gyroscopic orientation, and GPS location. 

ERRNO 3 will be tested by launching it via weather baloon to a maximum altitude of roughly 120,000 ft.  We use a RFD900 radio with 900Mhz antennas to broadcast sensor data from the payload to our team on the ground as well as to send it commands in order to take photos and deploy the aeroboom.

Following aeroboom deployment, the camera (pointed towards the aeroboom) will proceed to capture five photos to provide proof of a successful boom deployment.

We are currently working on a method to broadcast photos from the payload to receivers on the ground. We are still experimenting with various methods of accomplishing this.

Note: We do not take credit for the libraries that are provided; all of these libraries are available to the public and are free to use.

## Documentation

Pressing 'c' will take a photo, and pressing 'b' will deploy the aeroboom.
