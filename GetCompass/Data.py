import serial
import math
import struct
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
bytes = 6
posFail = 0
pos = 0
lastRead = 0
xPos = ["0x0", "0x0"];
yPos = ["0x0", "0x0"];
zPos = ["0x0", "0x0"];

while (True):
  c = ser.read(bytes)
  #print(c)
  if (c.__len__() == bytes):
   #While it has not found an empty value
   while(posFail != 1):
     #Keeps iterating if there is no empty value
     if(c[pos] == 0x80):
       pos = (pos + 1) % bytes
       if(lastRead == 1):
         posFail = 1
       lastRead = 1
          
     #Otherwise is not 0x80
     else:
       lastRead = 0
       pos = (pos + 1) % bytes
  #Prints out the values, good place to check phasing is working
  print(c)
  
  #Checks to see if the pos is somewhere where it has something like '0x00/0x80/0x80/x88/x88/x88'
  #It's removing the 0x80's
  if((pos%bytes) < 3):
    c = c[(pos): (pos+4)] 
  #If it does not then it gets the split bytes and concatenates them together
  else:
    diff = bytes - pos
    #X position
    xPos = c[pos : pos + diff]
    #Z position
    zPos = c[0 : (pos - 2)]
    #both values together
    e = b"".join([xPos, zPos])
    c = e
  #Unpacks the bytes and gets the values, it also only works with bits of 4? So I assume 8 would work
  results = struct.unpack('>HH', c)

  angle = math.atan2(results[0], results[1])*180/math.pi
  #Prints out the values
  print(results)
  print("Angle is: ", angle)
  

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

