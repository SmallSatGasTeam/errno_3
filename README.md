# Errno 3


A Cubesat deployment system based on FreeRTOS to test the functionality of the Aeroboom in a low pressure enviornment. 

Errno 3 will be responsible for handeling tasks and making sure the data and sensors are all working correctly as one unit. We will be reading UV Light, Light, Temperature from inside the payload, Temperature from outside of the payload, Actual Pressure in Millibars, Altitude, Speed, Angles, and a GPS to tell us the location. 

The trick to Errno 3 is using a RFD900 Radio with 900Mhz Radio antenna's to broadcast back down to us to give us the information that we need to read. Simply put, it will spit out lines of data back to us in a nice format for the user to read. The hard part is sending a signal to the Errno 3 payload in order to deploy the Aeroboom at the appropriate altitude. In simple terms, you would press 1 to take a picture with the UCAMM II Camera that is onboard. And press 9 to deploy the boom.

### ONCE YOU PRESS 9 YOU WILL RECEIVE A WARNING REQUESTING CONFIRMATION ON DEPLOYING THE BOOM, YOU MUST PRESS 'y' TO CONFIRM. YES, LOWER CASE y. ###

Once the Aeroboom is deployed, we will be given a 20 second delay in order to give enough time for the Aeroboom to inflate and cure itself in the UV light provided by the sun. The camera will then take 5 pictures in a row. (We chose 5 because the camera is very inconsistent with taking photos. If we at least get 1 of the 5 then we have succeeded. Testing has shown that the camera does not operate very well in cold enviornments.) 

The hardest part is getting a picture back down to us on earth. We are still working on this part of the project and have yet not found a simple way to broadcast it back to us over the 900Mhz radio frequency. 

Note: We do not take credit for the libraries that are provided, all of these libraries are available to the public and are free to use.
