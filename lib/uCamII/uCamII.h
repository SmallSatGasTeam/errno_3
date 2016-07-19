/*
    uCam.h  
*/

#ifndef UCAMII_h
#define UCAMII_h
#define UCAMII_BUF_SIZE 24
#include <Arduino.h>
#include <Stream.h>

class UCAMII {

public:
    UCAMII(Stream& cameraSerial = Serial, Stream* debugSerial = (Stream*) NULL);
 //   UCAMII();
    boolean init();
    boolean takePicture();
    int numberOfPackages();
	
    unsigned long imageSize;

    int getData();    
    byte imgBuffer[UCAMII_BUF_SIZE];    // this is also set in _PACK_SIZE
    
private:
    unsigned long image_pos;
    int package_no;
    int send_initial();
    int set_package_size();
    int do_snapshot();
    int get_picture();
    int send_ack();
    int wait_for_sync();
    int attempt_sync();
    boolean wait_for_bytes(byte command[6]);
    Stream& camera_serial;
    Stream* debug_serial;
};

#endif
