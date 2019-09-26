#include "MicroBit.h"

MicroBit uBit;

// Send compass calibration value to PC over serial port

int main() {

  int xyzScale = 1024;
  int xCentre =  29744;
  int yCentre = 4294;
  int zCentre = 57464;
  int radius = 55903

  int leftLED = 0;
  int rightLED = 0;
  
  char command[10];
  command[0] = 0;    // Which motor (0 = left, 2 = right)
  command[1] = 0;    // Direction (0, 2)
  command[2] = 0;  // Speed max speed 128
  int go = 50;

  int leftSensor;
  int rightSensor;
  
  //uBit.display.print('A');   // Display letter to microbit
  
  while (true) {

    leftSensor = uBit.io.P13.getDigitalValue();

    rightSensor = uBit.io.P14.getDigitalValue();

    // 0 = black, 1 = white
    if(leftSensor == 1){
      leftLED = 1;
    }
    else{
      leftLED = 0;
    }
    
    if(rightSensor == 0){
      rightLED = 1;
    }
    else{
      rightLED = 0;
    }

    // Display light to match which motors should be going forward
    uBit.io.P8.setDigitalValue(leftLED);
    uBit.io.P12.setDigitalValue(rightLED);
    
    // Operate the motor
    if (leftSensor == 1 && rightSensor == 0){ // if white, black so go forward
      // left wheel forward
      command[0] = 0;
      command[1] = 0;
      command[2] = go;
      uBit.i2c.write(0x10 << 1, command, 3);
      // right wheel forward
      command[0] = 2;
      command[1] = 0;
      command[2] = go;
      uBit.i2c.write(0x10 << 1, command, 3);
    }
    else if (leftSensor == 1 && rightSensor == 1){ // else if white, white so turn right
      // left wheel forward
      command[0] = 0;
      command[1] = 0;
      command[2] = go;
      uBit.i2c.write(0x10 << 1, command, 3);
      // right wheel stop
      command[0] = 2;
      command[1] = 0;
      command[2] = 0;
      uBit.i2c.write(0x10 << 1, command, 3);
    }
    else if (leftSensor == 0 && rightSensor == 0){ // else if black, black sp turn left
      // left wheel stop
      command[0] = 0;
      command[1] = 0;
      command[2] = 0;
      uBit.i2c.write(0x10 << 1, command, 3);
      // right wheel forward
      command[0] = 2;
      command[1] = 0;
      command[2] = go;
      uBit.i2c.write(0x10 << 1, command, 3);
    }
    else { // else black, white so stop
      // left wheel stop
      command[0] = 0;
      command[1] = 0;
      command[2] = 0;
      uBit.i2c.write(0x10 << 1, command, 3);
      // right wheel stop
      command[0] = 2;
      command[1] = 0;
      command[2] = 0;
      uBit.i2c.write(0x10 << 1, command, 3);
    }
    
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


