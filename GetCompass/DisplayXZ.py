import serial
import tkinter   # The python tcl/tk uiconstruction api
import math
top = tkinter.Tk()     # The tcl/tk object
canvas = tkinter.Canvas(top,bg="bisque",height=800,width=800)
canvas.pack()
top.update()

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

sx = 0
sy = 0

while (True):
    x = ser.read(4)
    if (x.__len__() == 4):

        if (x[1] >= 128):
            newx = -(32768 * 2 -(x[1] * 256 + x[0]))
        else:
            newx = (x[1] * 256 + x[0])
        print(newx, end=",", flush=True)

        if (x[3] >= 128):
            newz = -(32768 * 2 -(x[3] * 256 + x[2]))
        else:
            newz = (x[3] * 256 + x[2])
        print(newz, end="=", flush=True)

        newsy = math.atan2(newx, newz)*100
        print(newsy, end=" ", flush=True)
        canvas.create_line(sx, 400 + sy, sx + 1, 400 + newsy, fill="red")
        if(sx > 800):
            sx = 0
        sx = sx + 1
        sy = newsy
        top.update()
