#include<Arduino.h>
#include "ST25DVSensor.h"
#include <Wire.h>
#include "i2c_device.h"

#define DEV_I2C Wire
ST25DV st25dv(-1, -1, &DEV_I2C);

void setup() {

  // Initialize serial for output.
   SerialUSB.begin();
  while (!SerialUSB) {
    ; // Wait for USB Serial to connect
  }
  SerialUSB.println("SerialUSB working!");

  initi2c();
  i2cScan();
  initnfcdevice(st25dv);

  Serial.println("Tag init");
  writeURI("yottavolt.github.io/web_nfc_demo/?temp1=-18&temp2=-12&temp3=5&temp4=20&temp5=32&temp6=39", st25dv);
}


void loop() {  
    static bool done = false;
    if (!done) {
    Serial.println("Tag init");
    writeURI("yottavolt.github.io/web_nfc_demo/?temp1=-18&temp2=-12&temp3=5&temp4=20&temp5=32&temp6=39", st25dv);
    done = true;  // Mark as done
  }

  delay(2500);

  String uri_read;
  if(st25dv.readText(&uri_read)) {        //if no text nothing has been changed, if text then new data has been written by application, then update data
    SerialUSB.println("No-Update");
  }
  else{
    SerialUSB.println("Updated data detected");
    done = false;
    SerialUSB.println(uri_read.c_str());
  }

    

} 


