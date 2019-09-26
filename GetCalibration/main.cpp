#include "MicroBit.h"

MicroBit uBit;

// Send compass calibration value to PC over serial port

int main() {

  uBit.compass.calibrate();
  CompassCalibration c = uBit.compass.getCalibration();
  uBit.init();uint8_t buffer[30];  // Bigger than needed

  int xs = c.scale.x;
  int ys = c.scale.y;
  int zs = c.scale.z;
  int xc = c.centre.x;
  int yc = c.centre.y;
  int zc = c.centre.z;
  int r = c.radius;
  
  *((int *)buffer) = xs;
  *((int *)(buffer + 4)) = ys;
  *((int *)(buffer + 8)) = zs;
  *((int *)(buffer + 12)) = xc;
  *((int *)(buffer + 16)) = yc;
  *((int *)(buffer + 20)) = zc;
  *((int *)(buffer + 24)) = r;
  uBit.serial.send(buffer,28);

  release_fiber();
}


