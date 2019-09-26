#include "MicroBit.h"

MicroBit uBit;

// Send compass calibration value to PC over serial port

int main() {

  uBit.compass.calibrate();
  Sample3D c = uBit.compass.getCalibration();

  uBit.init();uint8_t buffer[8];  // Bigger than needed

  *((int *)buffer) = c;
  uBit.serial.send(buffer,8);
  
  /*
  for (;;) {
    int x = uBit.accelerometer.getX();
    //uBit.serial.send(ManagedString(x)+"\n");
    *((int *)buffer) = x;
    uBit.serial.send(buffer,2);
    int y = uBit.accelerometer.getY();
    *((int *)buffer) = y;
    uBit.serial.send(buffer,2);
  }*/

  release_fiber();
}


