import serial
import time

ser = serial.Serial('COM5', 9600)

if ser.readable():
    while True:
        val = input(">>> ")
        if val == '1':
            time.sleep(3)
            val = val.encode('utf-8')
            ser.write(val)
            val = '3'
            val = val.encode('utf-8')
            ser.write(val)
            print('a')
            time.sleep(0.5)
        elif val == '2':
            time.sleep(3)
            val = val.encode('utf-8')
            ser.write(val)
            print('b')
            val = '3'
            val = val.encode('utf-8')
            ser.write(val)
            time.sleep(0.5)
        elif val == '3':
            val = val.encode('utf-8')
            ser.write(val)
            print('stop Arduino')
