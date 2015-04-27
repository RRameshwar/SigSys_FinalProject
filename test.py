import serial
import time
import matplotlib.pyplot as plt

ser = serial.Serial("/dev/ttyACM0", 9600)

keeps = 10

exes = [val for val in range(keeps)]
data = [0 for val in range(keeps)]
p = [0 for val in range(keeps)]
i = [0 for val in range(keeps)]
d = [0 for val in range(keeps)]
t = [0 for val in range(keeps)]


plt.ion()
fig = plt.figure()
ax = fig.add_subplot(111)

resLine, = plt.plot(exes, data, 'y')
pLine, = plt.plot(exes, p, 'r')
iLine, = plt.plot(exes, i, 'b')
dLine, = plt.plot(exes, d, 'g')
tLine, = plt.plot(exes, t, 'k')

resLine.axes.set_ylim(-256, 256)
pLine.axes.set_ylim(-256, 256)
iLine.axes.set_ylim(-256, 256)
dLine.axes.set_ylim(-256, 256)
tLine.axes.set_ylim(-256, 256)

resLine.axes.set_label("Resistor")
pLine.axes.set_label("Prop")
iLine.axes.set_label("Int")
dLine.axes.set_label("Der")
tLine.axes.set_label("Total")

plt.legend(["Resistor", "Proportional", "Integral", "Derivative", "Total"], loc="lower left")
plt.draw()
'''
resLine, = ax.plot(exes, data, 'r-')
pLine, = ax.plot(exes, p, 'r-')
iLine, = ax.plot(exes, i, 'r-')
dLine, = ax.plot(exes, d, 'r-')
plt.legend(["Resistor", "Proportional", "Integral", "Derivative", "Location", "SouthEast"])
plt.ylim([0, 256])
'''

while True:
    #read = ser.readline()

    readVal = ser.readline()
    try:
    	read = [float(val) for val in readVal.split(',')]
    	pRead   = abs(read[0])
    	iRead   = read[1]
    	dRead   = read[2]
    	resRead = read[3]
    	#pRead, iRead, dRead, resRead = [float(val) for val in readVal.split(',')]
        p    = p[1:]    + [pRead]
        i    = i[1:]    + [iRead]
        d    = d[1:]    + [dRead]
        data = data[1:] + [resRead]
        t    = t[1:]    + [pRead + iRead + dRead]
        print(d)
    except:
        print 'meh'


    #plt.plot(data)
    #plt.plot(p)
    #plt.plot(i)
    #plt.plot(d)
    resLine.set_ydata(data)
    pLine.set_ydata(p)
    iLine.set_ydata(i)
    dLine.set_ydata(d)
    tLine.set_ydata(t)

    fig.canvas.draw()
