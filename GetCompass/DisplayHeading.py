import serial
import tkinter   # The python tcl/tk uiconstruction api
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
    x = ser.read(2)
    if (x.__len__() == 2):
        newsy = (x[1] * 256 + x[0]) / 100
        print(newsy, end=" ", flush=True)
        canvas.create_line(sx, 400 + sy, sx + 1, 400 + newsy, fill="red")
        sx = sx + 1
        sy = newsy
        top.update()
        if (sx > 800):
            sx = 0
            canvas.delete("all")
