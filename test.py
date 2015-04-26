import serial
import time
import matplotlib.pyplot as plt

ser = serial.Serial("/dev/ttyACM0", 9600)

data = []
while True:
    read = ser.readline()

    try:
        data.append(int(read))
    except:
        print 'meh'

    plt.plot(data)
    plt.draw()
    plt.ion()
    plt.show()