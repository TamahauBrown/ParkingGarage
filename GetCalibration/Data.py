import serial
import math
ser = serial.Serial()
ser.baudrate = 115200
ser.bytesize = 8
ser.stopbits = 1
ser.parity = 'N'
ser.timeout = 5
ser.port = "/dev/ttyACM0"
ser.xonxoff = 0
ser.rtscts = 0
ser.open()

while (True):
  c = ser.read(8)
  print(c)

'''
scale:
\x00\x04 => 4 * 256
\x00\x04
\x00\x04
centre xyz:
0 t  => 48 + 116 * 256
\xc6\x10 => 198 + 16 * 256
x\xe5 => 120 + 224 * 256
radius:
_\xda => 95 + 218 * 256
'''

