#include "MicroBit.h"

MicroBit uBit;

// Send compass calibration value to PC over serial port

int main() {

  int leftLED = 0;
  int rightLED = 0;
  
  char command[10];
  command[0] = 0;    // Which motor (0 = left, 2 = right)
  command[1] = 0;    // Direction (0, 2)
  command[2] = 0;  // Speed max speed 128

  int leftSensor;
  int rightSensor;
  
  uBit.display.print('A');   // Display letter to microbit
  
  while (true) {

    // Blinking left LED
    /*uBit.io.P8.setDigitalValue(v);
    uBit.sleep(1000);
    if (v == 0) {
      v = 1;
    }
    else {
      v = 0;
      }*/

    leftSensor = uBit.io.P13.getDigitalValue();

    rightSensor = uBit.io.P14.getDigitalValue();

    // Operate a motor
    command[0] = 2;
    command[1] = 2;
    command[2] = 50;
    uBit.i2c.write(0x10 << 1, command, 3);

    uBit.sleep(100);
    command[2] = 0;
    uBit.i2c.write(0x10 << 1, command, 3);
  }
  
  /*uBit.init();uint8_t buffer[10];  // Bigger than needed

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


