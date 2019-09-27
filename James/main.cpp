#include "MicroBit.h"

MicroBit uBit;

// \Enum for the directions the car can move
enum Direction {Forward, Left, Right, Stop};

// Method to control the wheels
void motors(Direction d){
  
  int go = 50;  //forward speed, should be between 30 and 70
  
  char lWCom[10];  // left wheel command to send
  char rWCom[10];  // right wheel command to send
  // Which motor (0 = left, 2 = right)
  lWCom[0] = 0;
  rWCom[0] = 2;
  
  // command[1] => Direction (0 -> forward, 2 -> backwards), default to forward
  lWCom[1] = 0;
  rWCom[1] = 0;
  // command[2] => Speed of wheel (0 or go)

  if(d == Forward){
    lWCom[2] = go;
    rWCom[2] = go;
  }
  else if(d == Left){
    lWCom[2] = 0;
    rWCom[2] = go;
  }
  else if(d == Right){
    lWCom[2] = go;
    rWCom[2] = 0;
  }
  else { // Stop
    lWCom[2] = 0;
    rWCom[2] = 0;
  }
  
  uBit.i2c.write(0x10 << 1, lWCom, 3);
  uBit.i2c.write(0x10 << 1, rWCom, 3);
}

// Method to light the red LED lights on the front
void lights(int leftLED, int rightLED){

    // Display light to match which motors should be going forward
    uBit.io.P8.setDigitalValue(leftLED);
    uBit.io.P12.setDigitalValue(rightLED);
}

void flashLights(int counter){
  int rightLED;
  int leftLED;
  int flashDuration = 10;

  uBit.display.print('?');
  
  // Make the lights flash
  if(counter % flashDuration < flashDuration/2){
    rightLED = 1;
    leftLED = 1;
  }
  else {
    rightLED = 0;
    leftLED = 0;
  }
  
  lights(leftLED, rightLED);
}

int main() {

  // Set the compass calibration
  //---------------------------------------------------
  
  int xyzScale = 1024;
  int xCentre =  29744;
  int yCentre = 4294;
  int zCentre = 57464;
  int radius = 55903;

  CompassCalibration cal = CompassCalibration();
  
  cal.scale.x = xyzScale;
  cal.scale.y = xyzScale;
  cal.scale.z = xyzScale;
  cal.centre.x = xCentre;
  cal.centre.y = yCentre;
  cal.centre.z = zCentre;
  cal.radius = radius;
  
  uBit.compass.setCalibration(cal);

  //---------------------------------------------------

  int leftLED = 0;
  int rightLED = 0;

  Direction d = Stop;

  int spinCounter = 0; // Counts how long car has been turning for

  int leftSensor;
  int rightSensor;
  
  //uBit.display.print('A');   // Display letter to microbit
  
  while (true) {

    leftSensor = uBit.io.P13.getDigitalValue();

    rightSensor = uBit.io.P14.getDigitalValue();
    
    // Operate the motor
    if (leftSensor == 1 && rightSensor == 0){ // if white, black
      d = Forward; // Go forward
      lights(1, 1); // Both lights on
      spinCounter = 0;
    }
    else if(spinCounter > 5000){ // If lost and just spinning
      d = Stop; // Stop wheels
      flashLights(spinCounter); // Flash lights
    }
    else if (leftSensor == 0 && rightSensor == 0){ // else if black, black
      d = Right; // Turn right
      lights(0, 1); // Right light on
    }
    else if (leftSensor == 1 && rightSensor == 1){ // else if white, white
      d = Left; // Turn left
      lights(1, 0); // Left light on
    }
    else { // else black, white
      d = Stop;  // Stop wheels
      lights(0, 0); // Both lights off
    }
    
    motors(d);  // Implement set direction
    
    spinCounter++;

  
    // Send compass calibration value to PC over serial port

  
    /*
    uBit.init();uint8_t buffer[10];  // Bigger than needed

    int x = uBit.compass.getX();
    *((int *)buffer) = x;
    uBit.serial.send(buffer,2);
    
    int z = uBit.compass.getZ();
    *((int *)buffer) = z;
    uBit.serial.send(buffer,2);
    */
    
    release_fiber();
  }
}


