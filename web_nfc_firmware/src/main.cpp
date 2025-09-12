/**
 ******************************************************************************
 * @file    ST25DV_SimpleWrite.ino
 * @author  STMicroelectronics
 * @version V1.0.0
 * @date    22 November 2017
 * @brief   Arduino test application for the STMicrolectronics
 *          ST25DV NFC device.
 *          This application makes use of C++ classes obtained from the C
 *          components' drivers.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

/**
******************************************************************************
* How to use this sketch
*
* This sketch uses the I2C interface to communicate with the NFC device.
* It writes an NFC tag type URI (Uniform Resource Identifier).
* Choose the uri by changing the content of uri_write.
*
* When the NFC module is started and ready, the message "Sytstem init done!" is
* displayed on the monitor window. Next, the tag is written with the content 
* printed on the monitor window.
*
* You can use your smartphone to read the tag.
* On Android, check if NFC is activated on your smartphone. 
* Put your smartphone near the tag to natively read it. 
* The preferred Internet Browser is automatically opened with the provided URI.
******************************************************************************
*/


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

  // The wire instance used can be omitted in case you use default Wire instance
  if(st25dv.begin() == 0) {
    SerialUSB.println("System Init done!");
  } else {
    SerialUSB.println("System Init failed!");
    while(1);
  }


  //write uri
  const char uri_write_message[] = "google.de";       // Uri message to write in the tag
  const char uri_write_protocol[] = URI_ID_0x01_STRING; // Uri protocol to write in the tag
  String uri_write = String(uri_write_protocol) + String(uri_write_message);
  if(st25dv.writeURI(uri_write_protocol, uri_write_message, "")) {
    SerialUSB.println("Write failed!");
    while(1);
  }
    

}

void loop() {  
    SerialUSB.println("Looping...");
    delay(1000);
} 


