import serial
import pylab as py
import numpy as np
serialPort='/dev/ttyUSB0'
baudRate=9600
fPath='output.csv'
py.interactive(True)



outFile=open(fPath,'w')
ser=serial.Serial(serialPort,baudRate)

data=[]
number=np.array(range(0,1000))


i=0
while True:
    i+=1
    line=ser.readline()
    try:
        line=line.decode()
    except UnicodeDecodeError:
        print "decode",line
        continue
    #outFile.write(line)
    try:
        #line=int(line)
        data.append(int(line))
        if int(line)>500: print line
    except ValueError:
        print line
        continue

    if len(data)>200: data=data[1:]
    #py.plot(i,line,"o")
    py.plot(data)
    py.pause(0.01)
    py.gcf().clear()

