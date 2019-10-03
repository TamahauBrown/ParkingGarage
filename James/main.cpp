#include "MicroBit.h"

MicroBit uBit;

// \Enum for the directions the car can move
enum Direction {Forward, Left, Right, Stop, SharpLeft, LongRight};


// Method to light the red LED lights on the front
void lights(int leftLED, int rightLED){

    // Display light to match which motors should be going forward
    uBit.io.P8.setDigitalValue(leftLED);
    uBit.io.P12.setDigitalValue(rightLED);
}

void flashLights(int counter, int left, int right){
  int rightLED;
  int leftLED;
  int flashDuration = 500;

  uBit.display.print('?');
  
  // Make the lights flash
  if(counter % flashDuration < flashDuration/2){
    if(left == 1){
      leftLED = 1;
    }
    if(right == 1){
      rightLED = 1;
    }
  }
  else {
    rightLED = 0;
    leftLED = 0;
  }
  
  lights(leftLED, rightLED);
}

// Method to control the wheels
int motors(Direction d, int spinCounter){
  
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
    lights(1, 1); // Both lights on
    spinCounter = 0; // We have stopped turning/spinning
  }
  else if(spinCounter > 5000){ // If lost and just spinning
    d = Stop; // Stop wheels
    flashLights(spinCounter, 1, 1); // Flash lights
    spinCounter++;
  }
  else if(d == Left){
    lWCom[2] = 0;
    rWCom[2] = go;
    lights(1, 0); // Left light on
    spinCounter++;
  }
  else if(d == Right){
    lWCom[2] = go;
    rWCom[2] = 0;
    lights(0, 1); // Right light on
    spinCounter++;
  }
  else if(d == SharpLeft){
    lWCom[2] = go;
    // Reverse right wheel
    rWCom[1] = 1;
    rWCom[2] = go;
    flashLights(spinCounter, 1, 0); // Left light on
  }
  else { // Stop
    lWCom[2] = 0;
    rWCom[2] = 0;
    lights(0, 0); // Both lights off
    spinCounter = 0;
  }
  
  uBit.i2c.write(0x10 << 1, lWCom, 3);
  uBit.i2c.write(0x10 << 1, rWCom, 3);
  
  return spinCounter;
}

void manualCalibrate(MicroBitEvent)
{
  uBit.compass.calibrate();
}

int main() {

  // Set the compass calibration
  //---------------------------------------------------
  
  int xyzScale = 4;    //1024;
  int xCentre = 18435; //29744;
  int yCentre = 35367; //4294;
  int zCentre = 64023; //57464;
  int radius = 4537;   //55903;

  CompassCalibration cal = CompassCalibration();
  
  cal.scale.x = xyzScale;
  cal.scale.y = xyzScale;
  cal.scale.z = xyzScale;
  cal.centre.x = xCentre;
  cal.centre.y = yCentre;
  cal.centre.z = zCentre;
  cal.radius = radius;
  
  uBit.compass.setCalibration(cal);

  // Setup some button handlers to allow extra control with buttons.
  uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, manualCalibrate);

  //---------------------------------------------------

  // Predetermined routes to car parks
  int pathSize = 5;
  Direction A[pathSize] = {Left, Forward, Stop, Stop, Stop};
  Direction B[pathSize] = {Left, Right, Left, Stop, Stop};
  Direction C[pathSize] = {Left, Right, Forward, Left, Stop};
  Direction D[pathSize] = {Right, Left, Right, Stop, Stop};
  Direction E[pathSize] = {Right, Forward, Stop, Stop, Stop};
  Direction F[pathSize] = {Left, Right, Right, Stop, Stop};
  Direction G[pathSize] = {Right, Left, Forward, Left, Stop};
  Direction H[pathSize] = {Right, Left, Left, Stop, Stop};
  Direction path[pathSize];
  for(int i = 0; i < pathSize; i++){
    path[i] = A[i];
  }
  int intersection = 0;

  int leftLED = 0;
  int rightLED = 0;

  Direction d = Stop;

  int spinCounter = 0; // Counts how long car has been turning for

  int leftSensor;
  int rightSensor;
  
  uBit.display.print('G');   // Display letter to microbit
  
  while (true) {

    leftSensor = uBit.io.P13.getDigitalValue();

    rightSensor = uBit.io.P14.getDigitalValue();
    
    // Operate the motor
    if (leftSensor == 1 && rightSensor == 0){ // if white, black
      d = Forward; // Go forward
    }
    else if (leftSensor == 0 && rightSensor == 0){ // else if black, black
      d = Left; // Turn left
    }
    else if (leftSensor == 1 && rightSensor == 1){ // else if white, white
      d = Right; // Turn right
    }
    else { // else black, white

      // Met intersection
      if (intersection < pathSize){

	// Handling intersection
	//---------------------------------------------------------------------
	motors(Stop, 0);
	uBit.display.print(intersection);   // Display intersection number to microbit
	if(path[intersection] == Forward){
	  lights(1,1);
	  uBit.display.print('^');   // Display going forward to microbit
	  uBit.sleep(1000);
	}
	else if(path[intersection] == Left){
	  uBit.display.print('<');   // Display going left to microbit
	  for(int i = 0; i < 5; i++){
	    lights(1,0);
	    uBit.sleep(100);
	    lights(0,0);
	    uBit.sleep(100);
	  }
	}
	else if(path[intersection] == Right){
	  uBit.display.print('>');   // Display going right to microbit
	  for(int i = 0; i < 5; i++){
	    lights(0,1);
	    uBit.sleep(100);
	    lights(0,0);
	    uBit.sleep(100);
	  }
	}
	
	while (true){ // while in intersection
	    
	  leftSensor = uBit.io.P13.getDigitalValue();

	  rightSensor = uBit.io.P14.getDigitalValue();

	  // Operate the motor during the intersection
	  if (path[intersection] == Forward){ // Go forward at intersection
	    
	    if (leftSensor == 0 && rightSensor == 1){ // if black, white
	      d = Forward; // Go forward
	    }
	    else if (leftSensor == 0 && rightSensor == 0){ // else if black, black
	      d = Right; // Turn right
	    }
	    else if (leftSensor == 1 && rightSensor == 1){ // else if white, white
	      d = Left; // Turn left
	    }
	    else { // else white, black
	      // Left the intersection
	      break;
	    }

	    spinCounter = motors(d, spinCounter);
	    
	    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	  }
	  else if (path[intersection] == Left){ // Go right at intersection
	    //
	    if(leftSensor == 1 && rightSensor == 0){// If, white, black
	      // We are out of the intersection
	      intersection++;
	      break;
	    }
	    // otherwise keep turning
	    spinCounter = motors(SharpLeft, spinCounter);
	    
	    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	  }
	  else if (path[intersection] == Right){ // Go right at intersection
	    if(leftSensor == 1 && rightSensor == 0){// If, white, black
	      // We are out of the intersection
	      intersection++;
	      break;
	    }
	    
	    //TODO so just break
	    break;
	    
	  }
	  else { // else stop
	    d = Stop;  // Stop wheels
	    intersection++;
	    break;
	  }
	}
      }
      else{ // Else park
	d = Stop;  // Stop wheels
      }
    }

    //------------------------------------------------------------------------------
    
    spinCounter = motors(d, spinCounter);  // Implement set direction


    // Send compass calibration value to PC over serial port

  
    
    uBit.init();uint8_t buffer[10];  // Bigger than needed
    /*
    int h1 = uBit.compass.heading();
    *((int *)buffer) = h1;
    uBit.serial.send(buffer,2);

    int x = uBit.compass.getX();
    *((int *)buffer) = x;
    
    int z = uBit.compass.getZ();
    *((int *)(buffer+2)) = z;
    uBit.serial.send(buffer,4);
    */
    
  }
    
  release_fiber();
}


