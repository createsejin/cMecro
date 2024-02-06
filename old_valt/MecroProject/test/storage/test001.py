import serial
import time

ser = serial.Serial('COM5', 9600)


def keySerial(input_):
    if input_ == 'a':
        return 1
    elif input_ == 'b':
        return 2
    elif input_ == 'KEY_RIGHT_ARROW':
        return 4


def test000():
    if ser.readable():
        val = input(">>> ")
        if val == '1':
            time.sleep(3)
            val = val.encode('utf-8')
            ser.write(val)
            pause()
            print('a')
            time.sleep(0.5)
        elif val == '2':
            time.sleep(3)
            val = val.encode('utf-8')
            ser.write(val)
            print('b')
            pause()
            time.sleep(0.5)
        elif val == '0':
            pause()
            print('stop Arduino and Program')
            exit(0)
        elif val == '4':
            time.sleep(3)
            val = val.encode('utf-8')
            ser.write(val)
            print('→')
            pause()
            time.sleep(0.5)


def pause():
    val = '0'
    val = val.encode('utf-8')
    ser.write(val)


if __name__ == "__main__":
    while True:
        test000()
