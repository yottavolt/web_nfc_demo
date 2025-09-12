#ifndef I2C_DEVICE_H
#define I2C_DEVICE_H

#include <Arduino.h>  
//yeah i should really pass the required objects here and just not call them but eh it works

//scanning available i2c devices for checking connection
void i2cScan() {
  SerialUSB.println("Scanning I2C bus...");
  for (byte addr = 1; addr < 127; ++addr) {
    Wire.beginTransmission(addr);
    uint8_t error = Wire.endTransmission();
    if (error == 0) {
      SerialUSB.print("I2C device found at 0x");
      SerialUSB.println(addr, HEX);
    } else if (error == 4) {
      SerialUSB.print("Unknown error at 0x");
      SerialUSB.println(addr, HEX);
    }
  }
  SerialUSB.println("I2C scan done.");
}

//init pullups and wire for st25dv
void initi2c(){
  //enable pull ups fori²c, (not recommended for production, but im lazy for manual connection)
  pinMode(PB6, INPUT_PULLUP); // SCL
  pinMode(PB7, INPUT_PULLUP); // SDA

  //init i2c on PB6 SCL and PB7 SDA
  Wire.begin();
  SerialUSB.println("Wire init corrrect");

}

#endif // I2C_DEVICE