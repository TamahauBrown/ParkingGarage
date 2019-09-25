#include "MicroBit.h"
MicroBit uBit;
// Problem - send accelerometer values to PC over serial port
int main() {
 uBit.init();
 uint8_t buffer[10]; // Bigger than needed

for (;;) {
int x = uBit.accelerometer.getX();
int y = uBit.accelerometer.getY();
*((int *)buffer) = x;
*((int *)(buffer+2)) = y;
//TODO: Get Z and location and 
uBit.serial.send(buffer, 4);
}

 ////struct { int ax, ay, az; } accel;
 ////struct { short ax, ay, az; } accel;
 //accel.ax = uBit.accelerometer.getX();
 //accel.ay = uBit.accelerometer.getY();
 //accel.az = uBit.accelerometer.getZ();
 //uBit.serial.send((uint8_t *)(&accel), sizeof(accel));

 release_fiber();
}
