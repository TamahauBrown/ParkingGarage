#include "MicroBit.h"

MicroBit uBit;

// Variables for manual park entry
//int locX = 0;
//int locY = 0;
int park = 0;
int lastLetter = 7;
char parkLetter[] ={'A','B','C','D','E','F','G','H'}; 
bool needPark = true;
//MicroBitImage i("1,0,0,0,0\n");
//MicroBitImage clear("0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n0,0,0,0,0\n"); 


// Enum for the directions the car can move
enum Direction {Forward, Left, Right, Stop};


// Method to light the red LED lights on the front
void lights(int leftLED, int rightLED){

    // Display light to match which motors should be going forward
    uBit.io.P8.setDigitalValue(leftLED);
    uBit.io.P12.setDigitalValue(rightLED);
}

void flashLights(int counter, int left, int right){
  int rightLED;
  int leftLED;
  int flashDuration = 200;
  
  // Make the lights flash
  if(counter % flashDuration < flashDuration/2){
      leftLED = left;
      rightLED = right;
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
  int slow = 0;
  
  char lWCom[10];  // left wheel command to send
  char rWCom[10];  // right wheel command to send
  // Which motor (0 = left, 2 = right)
  lWCom[0] = 0;
  rWCom[0] = 2;
  
  // command[1] => Direction (0 -> forward, 2 -> backwards), default to forward
  lWCom[1] = 0;
  rWCom[1] = 0;
  // command[2] => Speed of wheel (0 or go)
  lWCom[2] = 0;
  rWCom[2] = 0;

  if(d == Forward){
    lWCom[2] = go;
    rWCom[2] = go;
    lights(1, 1); // Both lights on
    spinCounter = 0; // We have stopped turning/spinning
  }
  else if(spinCounter > 5000){ // If lost and just spinning
    d = Stop; // Stop wheels
    uBit.display.print('?');
    flashLights(spinCounter, 1, 1); // Flash lights
    spinCounter++;
  }
  else if(d == Left){
    //lWCom[2] = go;
    //lWCom[1] = 1; // Reverse left wheel
    lWCom[2] = slow;
    
    rWCom[2] = go;
    
    flashLights(spinCounter, 1, 0); // Left light on
    spinCounter++;
  }
  else if(d == Right){
    lWCom[2] = go;
    
    //rWCom[2] = go;
    //rWCom[1] = 1; // Reverse right wheel
    rWCom[2] = slow;
    
    flashLights(spinCounter, 0, 1); // right light on
    spinCounter++;
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

// The three following methods are for manual park entry
void setXonButtonA(MicroBitEvent)
{
  /*
    locX++;
    if (locY == 0){
      locX %= 5;
    }
    else if (locX > 3){
      locX = 1;
    }
  */
  if(park == lastLetter){
    park = 0;
  }
  else{
    park++;
  }
}

void setYonButtonB(MicroBitEvent)
{
  /*
  if (locY == 0){
    locY = 2;
    if(locX == 0){
      locX = 1;
    }
    else if(locX == 4){
      locX = 3;
    }
  }
  else{
    locY = 0;
  }
  */
  if(park == 0){
    park = lastLetter;
  }
  else{
    park--;
  }
}

void setPark(MicroBitEvent)
{
  /*if (locY == 2 && (locX == 0 || locX == 4)){// there are no parks here
    uBit.display.print('?');
    uBit.sleep(800);
  }
  else{
    needPark = false;
  }*/
  needPark = false;
}

/*void onData(MicroBitEvent)
{
  // Get recieved message into PacketBuffer
  uint8_t rxdata[10];
  int num = uBit.radio.datagram.recv(rxdata, 10);

  // Check checksum
  if((rxdata[4] == 0x11 && rxdata[5] == 0x11 &&
      rxdata[6] == 0x11 && rxdata[7] == 0x11)){
    
  }
  }*/

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
  uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_LONG_CLICK, manualCalibrate);

  //---------------------------------------------------

  // Get designated park
  
  // Get from radio
  //uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, onData);
  // Enter manually
  uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, setXonButtonA);
  uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, setYonButtonB);
  uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_LONG_CLICK, setPark);
  // Set parking lot
  while (needPark){
    //uBit.display.print(clear);
    //uBit.display.print(i, locX, locY);
    uBit.display.print(parkLetter[park]);
    uBit.sleep(100);
  }  

  // Predetermined routes to car parks
  int pathSize = 4;
  Direction path[pathSize];
  
  Direction A[pathSize] = {Left, Forward, Stop, Stop};
  Direction B[pathSize] = {Left, Right, Left, Stop};
  Direction C[pathSize] = {Left, Right, Forward, Left};
  Direction D[pathSize] = {Right, Left, Right, Stop};
  Direction E[pathSize] = {Right, Forward, Stop, Stop};
  Direction F[pathSize] = {Left, Right, Right, Stop};
  Direction G[pathSize] = {Forward, Stop, Stop, Stop};
  Direction H[pathSize] = {Right, Left, Left, Stop};
  Direction def[pathSize] = {Stop, Stop, Stop, Stop};

  
  switch(parkLetter[park]) {
    case 'A' :
      for (int i = 0; i < pathSize; i++) {
	path[i] = A[i];
      }
      break;
    case 'B' :
      for (int i = 0; i < pathSize; i++) {
	path[i] = B[i];
      }
      break;
    case 'C' :
      for (int i = 0; i < pathSize; i++) {
	path[i] = C[i];
      }
      break;
    case 'D' :
      for (int i = 0; i < pathSize; i++) {
	path[i] = D[i];
      }
      break;
    case 'E' :
      for (int i = 0; i < pathSize; i++) {
	path[i] = E[i];
      }
      break;
    case 'F' :
      for (int i = 0; i < pathSize; i++) {
	path[i] = F[i];
      }
      break;
    case 'G' :
      for (int i = 0; i < pathSize; i++) {
	path[i] = G[i];
      }
      break;
    case 'H' :
      for (int i = 0; i < pathSize; i++) {
	path[i] = H[i];
      }
      break;
    default :
      for (int i = 0; i < pathSize; i++) {
	path[i] = def[i];
      }
  }


  int intersection = 0;

  int leftLED = 0;
  int rightLED = 0;

  Direction d = Stop;

  int spinCounter = 0; // Counts how long car has been turning for

  int leftSensor;
  int rightSensor;
  
  while (true) {
  
    uBit.display.print('=');   // Display letter saying okay to microbit

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
	uBit.display.scroll(intersection);   // Display intersection number to microbit
	if(path[intersection] == Forward){
	  lights(1,1);
	  uBit.display.print('^');   // Display going forward to microbit
	  uBit.sleep(1000);
	}
	else if(path[intersection] == Left){
	  uBit.display.print('>');   // Display going left to microbit
	  for(int i = 0; i < 5; i++){
	    lights(1,0);
	    uBit.sleep(100);
	    lights(0,0);
	    uBit.sleep(100);
	  }
	}
	else if(path[intersection] == Right){
	  uBit.display.print('<');   // Display going right to microbit
	  for(int i = 0; i < 5; i++){
	    lights(0,1);
	    uBit.sleep(100);
	    lights(0,0);
	    uBit.sleep(100);
	  }
	}

	// Operate the motor during the intersection
	if (path[intersection] == Forward){ // Go forward at intersection
	  /*while(leftSensor == 0 && rightSensor == 1){ // while black, white
	    leftSensor = uBit.io.P13.getDigitalValue();
	    rightSensor = uBit.io.P14.getDigitalValue();

	    spinCounter = motors(Left, spinCounter);
	    }*/
	  
	  spinCounter = motors(Forward, spinCounter);
	  uBit.sleep(100);
	  
	
	  while (true){ // while in intersection
	    
	    leftSensor = uBit.io.P13.getDigitalValue();

	    rightSensor = uBit.io.P14.getDigitalValue();

	    if (leftSensor == 1 && rightSensor == 0){ // if white, black
	      d = Stop; // Out of intersection
	      break;
	    }
	    else if (leftSensor == 0 && rightSensor == 1){ // if black, white
	      d = Forward; // Go forward
	    }
	    else if (leftSensor == 0 && rightSensor == 0){ // else if black, black
	      d = Right;
	    }
	    else if (leftSensor == 1 && rightSensor == 1){// else if white, white
	      //d = Left;
	      d = Right;
	    }
	    else { // else white, black
	      // Left the intersection
	      break;
	    }

	    spinCounter = motors(d, spinCounter);
	  }
	}
	    
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	else if (path[intersection] == Left){ // Go right at intersection
	    
	  while (true){ // while in intersection
	    
	    leftSensor = uBit.io.P13.getDigitalValue();
	    rightSensor = uBit.io.P14.getDigitalValue();
	      
	    if(leftSensor == 1 && rightSensor == 1){// If, white, white
	      // We are out of the intersection
	      break;
	    }
	    // otherwise keep turning
	    spinCounter = motors(Left, spinCounter);
	  }
	    
	  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	}
	else if (path[intersection] == Right){ // Go right at intersection
	    
	  while(leftSensor == 0 && rightSensor == 1){// while black, white
	    leftSensor = uBit.io.P13.getDigitalValue();
	    rightSensor = uBit.io.P14.getDigitalValue();
	    // move into intersection, aim for white white inside intersection
	    spinCounter = motors(Right, spinCounter);
	  }
	    
	  while(true){
	    if(leftSensor == 1 && rightSensor == 0){// Until back on track: white, black
	      break;
	    }
	    else if(leftSensor == 1 && rightSensor == 1){// If, white, white
	      // go forward toward other corner of intersection than we entered
	      d = Right;
	    }
	    else if(rightSensor == 0){ // if right sensor sees black
	      // swing the left sensor around so turn
	      d = Left;
	    }
	    else if(leftSensor == 0){ // if left sensor sees black
	      // swing the right sensor around so turn
	      d = Right;
	    }
	    else{ // else we are lost
	      d = Stop;
	    }
	    spinCounter = motors(d, spinCounter);
	      
	    leftSensor = uBit.io.P13.getDigitalValue();
	    rightSensor = uBit.io.P14.getDigitalValue();
	  }
		 
	}
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	else{ // Else park
	  d = Stop;
	  spinCounter = motors(Stop, spinCounter);
	  uBit.display.print('P'); //TODO display parking lot letter
	  break;
	}
	d = Stop; // After intersection clear last direction and stop
	intersection++; // Out of current intersection 
      }
      else{ // Else emergency stop
	d = Stop;  // Stop wheels
	uBit.display.print('!');
      }
    }

  //------------------------------------------------------------------------------
    
  spinCounter = motors(d, spinCounter);  // Implement set direction


  // Send compass calibration value to PC over serial port

  
    
  /*uBit.init();uint8_t buffer[10];  // Bigger than needed
  
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


