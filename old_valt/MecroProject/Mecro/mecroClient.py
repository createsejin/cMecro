import socket
import pickle
from threading import Thread
from serial import Serial, SerialException
from time import sleep


client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host_ip = '110.15.1.193'
port = 12345
delay = 0.015
address = (host_ip, port)
pause_mecro = False
i6 = 0


def get_serial():
    j = 3
    while True:
        try:
            com_port = 'COM' + str(j)
            serial_ = Serial(com_port, 9600)
            print("Arduino is connected to the {} port successfully.".format(com_port))
            return serial_
        except SerialException:
            j += 1


ser = get_serial()


def init_client():
    global client_socket
    client_socket.connect(address)
    send_data([2, 0, 0])


def send_data(L):
    b_data = pickle.dumps(L)
    client_socket.send(b_data)


def receiver():
    global client_socket
    try:
        while True:
            b_data = client_socket.recv(1024)
            respL = pickle.loads(b_data)
            operator_(respL)
    except ConnectionResetError:
        print("server is closed.")
        client_socket.close()
        return -1


def operator_(L):
    global i6, pause_mecro
    if L == [1, 1]:
        th = Thread(target=remote_f)
        th.start()
    elif L == [1.02, False]:
        if i6 == 0:
            i6 += 1
            pause_mecro = False
            th = Thread(target=clicking)
            th.start()
    elif L == [1.02, True]:
        pause_mecro = True
    elif L[0] == 2:
        print(L[2])
    elif L[0] == 3:
        print(L[2])
    elif 21 < L[0] < 21.1:
        if L[0] == 21.01:
            press_enter()
        else:
            press_arrow(L[0])
    else:
        print("L =", L)
    return


def press_enter():
    print("-> remote control : Enter")
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    return


def press_arrow(num):
    if num == 21.02:
        print("-> remote control : Arrow Up")
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        return
    elif num == 21.03:
        print("-> remote control : Arrow Down")
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        return
    elif num == 21.04:
        print("-> remote control : Arrow Right")
        keyPress('AR')
        sleep(delay)
        keyRelease('AR')
        return
    elif num == 21.05:
        print("-> remote control : Arrow Left")
        keyPress('AL')
        sleep(delay)
        keyRelease('AL')
        return
    elif num == 21.06:
        print("-> remote control : Backspace")
        keyPress('BS')
        sleep(delay)
        keyRelease('BS')
        return


def remote_f():
    print("remote control : F press requested")
    keyPress('f')
    sleep(delay)
    keyRelease('f')
    return


def test_arduino():
    for i in range(0, 5):
        keyPress('m')
        sleep(delay)
        keyRelease('m')
        sleep(delay)
    for i in range(0, 2):
        click('press')
        sleep(0.020)
        click('release')
        sleep(0.020)
    return


def clicking():
    global i6
    print("-> clicking start")
    while True:
        click('press')
        sleep(0.020)
        click('release')
        sleep(0.020)
        if pause_mecro:
            print("-> clicking end")
            i6 = 0
            return
        else:
            continue


def transSerial(str1):
    global ser
    ser.write(str1.encode())


def keyPress(sub2):  # 키보드 눌림 신호
    sub1 = 'KD'
    trans = sub1 + sub2
    transSerial(trans)


def keyRelease(sub2):  # 키보드 릴리즈 신호
    sub1 = 'KU'
    trans = sub1 + sub2
    transSerial(trans)


def click(command, button='l'):
    trans = str()
    if button == 'r':
        if command == 'click':
            trans = 'RC'
        elif command == 'press':
            trans = 'RH'
        elif command == 'release':
            trans = 'RR'
    elif button == 'l':
        if command == 'click':
            trans = 'LC'
        elif command == 'press':
            trans = 'LH'
        elif command == 'release':
            trans = 'LR'
    elif button == 'm':
        pass
    transSerial(trans)


if __name__ == "__main__":
    init_client()
    receiver_thread = Thread(target=receiver)
    receiver_thread.start()
