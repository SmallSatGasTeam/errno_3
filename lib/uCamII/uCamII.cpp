/*
 *    uCam.cpp
 *
 *    http://www.4dsystems.com.au/productpages/uCAM-II/downloads/uCAM-II_datasheet_R_1_4.pdf
 */

#include "uCamII.h"
//#define cameraDebugSerial




// From http://pastebin.com/ZuMu48w7

byte _SYNC_COMMAND[6] = {0xAA, 0x0D, 0x00, 0x00, 0x00, 0x00};
byte _RESET[6] = {0xAA, 0x08, 0x00, 0x00, 0x00, 0x00};
byte _SYNC_ACK_REPLY[6] = {0xAA, 0x0E, 0x0D, 0x00, 0x00, 0x00};
byte _SYNC_ACK_REPLY_EXT[6] = {0xAA, 0x0D, 0x00, 0x00, 0x00, 0x00};
byte _SYNC_FINAL_COMMAND[6] = {0xAA, 0x0E, 0x00, 0x00, 0xF5, 0x00};

byte _INITIAL_COMMAND[6] = {0xAA, 0x01, 0x00, 0x07, 0x07, 0x07};
byte _GENERIC_ACK_REPLY[6] = {0xAA, 0x0E, 0x00, 0x00, 0x00, 0x00};
byte _PACK_SIZE[6] = {0xAA, 0x06, 0x08, UCAMII_BUF_SIZE + 6, 0x00, 0x00};
byte _SNAPSHOT[6] = {0xAA, 0x05, 0x00, 0x00, 0x00, 0x00};
byte _GET_PICTURE[6] = {0xAA, 0x04, 0x01, 0x00, 0x00, 0x00};

UCAMII::UCAMII(Stream& cameraSerial, Stream* debugSerial):
  camera_serial(cameraSerial), debug_serial(debugSerial){
  this->image_pos = 0;
  this->package_no = 0;
}

boolean UCAMII::init()
{
  for (int i = 0; i < 6; i++) {
      camera_serial.write(_RESET[i]);
}
delay(500);
#ifdef cameraDebugSerial
  debug_serial->println("Intitial is starting to be sent");
#endif
  if (this->attempt_sync())
  {
#ifdef cameraDebugSerial
	debug_serial->println("\n\rCam has ACKED the SYNC");
#endif
    return true;
  }
  return false;
}


int UCAMII::attempt_sync()
{
  int attempts = 0;
  byte cam_reply;
  int ack_success = 0;
  int last_reply = 0;
  int incrementTime = 0;

  while (attempts < 60 && ack_success == 0) {
    // Flush
    while (camera_serial.available()) camera_serial.read();
    delay(1000);
#ifdef cameraDebugSerial
    debug_serial->println("Trying...");
    debug_serial->println("Sending SYNC...");
#endif
    for (int i = 0; i < 6; i++) {
      camera_serial.write(_SYNC_COMMAND[i]);
    }

    if (this->wait_for_bytes(_SYNC_ACK_REPLY) ) {
      if (this->wait_for_bytes(_SYNC_ACK_REPLY_EXT)) {
        delay(5 + incrementTime);
#ifdef cameraDebugSerial
        debug_serial->println("\r\nSending FINAL SYNC...");
#endif
        for (int i = 0; i < 6; i++) {
          camera_serial.write(_SYNC_FINAL_COMMAND[i]);
        }
        return 1;
      }
    }
      attempts++;
      incrementTime++;
  }
  attempts = 0;
  incrementTime = 0;
  debug_serial->println("Failed!");
  return 0;
}


// Return number of packages ready
int UCAMII::numberOfPackages() {
  return this->imageSize / UCAMII_BUF_SIZE;
}


int UCAMII::send_initial() {

  // flush
  while (camera_serial.available() > 0) {
    camera_serial.read();
  }

  delay(100);

#ifdef cameraDebugSerial
  debug_serial->println("Sending INITIALISE...");
#endif
  for (int i = 0; i < 6; i++) {
    camera_serial.write(_INITIAL_COMMAND[i]);
  }
  // @todo why 500 delay?
  delay(500);
  if (this->wait_for_bytes(_GENERIC_ACK_REPLY)) {
#ifdef cameraDebugSerial
    debug_serial->println("INITIALISE success");
#endif
    return 1;
  }

#ifdef cameraDebugSerial
  debug_serial->println("INITIALISE fail");
#endif

  return 0;
}

boolean UCAMII::wait_for_bytes(byte command[6]) {
  byte cam_reply;
  int i = 0;
  int received;
  short found_bytes;
  found_bytes = 0;
  // @todo millis() wait, millis() wrap around watch out
  // unsigned long start = millis();

#ifdef cameraDebugSerial
  debug_serial->print("\r\nWAIT: ");
  for (i = 0; i < 6; i++) {
    debug_serial->print("0x");
    debug_serial->print(command[i], HEX);
    debug_serial->print(" ");
  }
  i = 0;
#endif

  debug_serial->println("\nBefore Loop");
  debug_serial->print("Serial1: ");
  debug_serial->println(camera_serial.available());
  while (camera_serial.available()) {
    debug_serial->println("Before read");
    cam_reply = camera_serial.read();
   debug_serial->println("Cam reply: ");
   debug_serial->println(cam_reply);
    if (i < 6 ) {
      if ((cam_reply == command[i]) || command[i] == 0x00) {
        found_bytes++;
        i++;
      }
    }
#ifdef cameraDebugSerial
    debug_serial->print("\r\nGOT : ");
    debug_serial->print("0x");
    debug_serial->print(cam_reply, HEX);
    debug_serial->println(" ");
#endif
    received++;
    if (found_bytes == 6) {
      return true;
    }
  }
  return false;
}



bool UCAMII::takePicture() {
  if (send_initial()) {
    if (this->set_package_size()) {
      if (this->do_snapshot()) {
        if (get_picture()) {
          return 1;
        }
      }
    }
  }
}


int UCAMII::set_package_size() {

  byte ack[6] = {0xAA, 0x0E, 0x06, 0x00, 0x00, 0x00};

  delay(100);

#ifdef cameraDebugSerial
  debug_serial->println("Sending packet size...");
#endif

  for (int i = 0; i < 6; i++) {
    camera_serial.write(_PACK_SIZE[i]);
#ifdef cameraDebugSerial
    debug_serial->print(_PACK_SIZE[i], HEX);
    debug_serial->print(" ");
#endif

  }
  // @todo why 500 delay?
  delay(500);
  if (this->wait_for_bytes(ack)) {
#ifdef cameraDebugSerial
    debug_serial->println("\r\npacket size success");
#endif
    return 1;
  }

#ifdef cameraDebugSerial
  debug_serial->println("packet size fail");
#endif

  return 0;
}

int UCAMII::do_snapshot() {

  byte ack[6] = {0xAA, 0x0E, 0x05, 0x00, 0x00, 0x00};

  delay(100);

#ifdef cameraDebugSerial
  debug_serial->println("Sending snapshot...");
#endif


  for (int i = 0; i < 6; i++)
  {
    camera_serial.write(_SNAPSHOT[i]);
  }
  // @todo why 500 delay?
  delay(500);
  if (this->wait_for_bytes(ack)) {
#ifdef cameraDebugSerial
    debug_serial->println("snapshot success");
#endif
    return 1;
  }

#ifdef cameraDebugSerial
  debug_serial->println("snapshot fail");
#endif

  return 0;

}

int UCAMII::get_picture() {

  byte ack[6] = {0xAA, 0x0E, 0x04, 0x00, 0x00, 0x00};
  unsigned long imageSize;
  short i;

  delay(100);

#ifdef cameraDebugSerial
  debug_serial->println("Sending get picture...");
#endif

  for (int i = 0; i < 6; i++)
  {
    camera_serial.write(_GET_PICTURE[i]);
  }
  // @todo why 500 delay?
  delay(500);
  if (this->wait_for_bytes(ack)) {
#ifdef cameraDebugSerial
    debug_serial->println("picture success");
#endif
    // get the 6 bytes ACK
    for (i = 0; i <= 5; i++) {
      ack[i] = 0;
      while (!camera_serial.available());

      ack[i] = camera_serial.read();
      // last 3 bytes are the image size
#ifdef cameraDebugSerial
      debug_serial->print(i, DEC);
      debug_serial->print(" value: ");
      debug_serial->println(ack[i], HEX);
#endif
    }

    imageSize = 0;
    imageSize = (imageSize << 8) | ack[5];
    imageSize = (imageSize << 8) | ack[4];
    imageSize = (imageSize << 8) | ack[3];

    this->imageSize = imageSize;
    this->image_pos = this->imageSize;
    if (imageSize > 0) {
      return 1;
    }
  }

#ifdef cameraDebugSerial
  debug_serial->println("picture fail");
#endif

  return 0;
}

// @todo return false if time exceeded
int UCAMII::getData() {

  unsigned char high = (unsigned char)(this->package_no >> 8);
  unsigned char low  = this->package_no & 0xff;
  byte my_ack[6] = {0xAA, 0x0E, 0x00, 0x00, low, high};

  int i = 0;
  byte s;

  int bytes;

  // request bytes
  for (int i = 0; i < 6; i++) {
    camera_serial.write(my_ack[i]);
  }


  // Set number of bytes we should wait for
  if (this->image_pos < UCAMII_BUF_SIZE) {
    bytes = this->image_pos + 6;
  } else {
    bytes = UCAMII_BUF_SIZE + 6;
  }
#ifdef cameraDebugSerial
//    debug_serial->print("REMAINING: ");
//    debug_serial->print(this->image_pos, DEC);
//    debug_serial->print(" BYTES PER CHUNK: ");
//    debug_serial->println(bytes, DEC);


#endif

  for (i = 0; i < bytes; i++) {
      while (!camera_serial.available());
      // wait for bytes
          s = camera_serial.read();

          // Skip first 4 and last 2, Page 10 of the datasheet
          if (i >= 4 && i < bytes - 2) {
#ifdef cameraDebugSerial
              //debug_serial->print("*");
#endif
              this->imgBuffer[i - 4] = s;
              this->image_pos--; //hitting 0 before it needs to hit 0? add print statements
          }
#ifdef cameraDebugSerial
          //debug_serial->print(this->imgBuffer[s], HEX);
          //debug_serial->print(HEX);
          //debug_serial->print(" ");


#endif
      }


#ifdef cameraDebugSerial
      debug_serial->println("");
#endif

  this->package_no++;
  if(this->image_pos <= 0) {
    // send the final thank you goodbye package
    my_ack[4] = 0xF0;
    my_ack[5] = 0xF0;
    for (int i = 0; i < 6; i++) {
      camera_serial.write(my_ack[i]);
    }
      if(this->image_pos == 0) {
          for(int i = 0;i<6;i++){
              camera_serial.write(_RESET[i]);
          }
          //debug_serial->println("RESET SUCCESSFUL");
          this->image_pos = 0;
          this->package_no = 0;
      }
  }
  return bytes-6;

}
