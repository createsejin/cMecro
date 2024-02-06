from serial import Serial, SerialException
from time import sleep

j = 3
while True:
    try:
        port = 'COM'+str(j)
        ser = Serial(port, 9600)
        print("{} 포트에 연결 성공".format(port))
        break
    except SerialException:
        j += 1

while True:
    command = input(">>> ")

    ser.write(command.encode())
    sleep(0.1)
    if ser.readable():
        resp = ser.readline()
        print(resp[:-1].decode())
