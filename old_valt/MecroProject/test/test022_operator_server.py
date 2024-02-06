import socket
import pickle
from serial import Serial, SerialException
from time import sleep
from threading import Thread


def server():
    global L
    addr = ('localhost', 12346)
    print("server service started.")
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind(addr)
        server_socket.listen()
        client_socket, client_addr = server_socket.accept()
        incoming = addr[0] + ':' + str(addr[1]) + ' connected.'
        print(incoming)

        def sendData(ils):
            send_data = pickle.dumps(ils)
            client_socket.send(send_data)
        while True:
            recvdata = client_socket.recv(65535)
            if len(recvdata) > 0:
                L = pickle.loads(recvdata)
                print(L)
                if L == ['exit program']:
                    sendData(['server end'])
                    server_socket.close()
                    client_socket.close()
                    ser.close()
                    print("End program")
                    exit(0)
                elif L == [1, 0, 0]:
                    sendData([1])
                    executeOrder(L[0])
                elif L == [1, 1, 0]:
                    sendData([1, 1])
                    executeOrder(1.1)
                elif L[0] == 2:
                    sendData([2])


def executeOrder(order):
    global flag01
    th1 = Thread(target=clicking, daemon=True)
    if order == 1:
        print("pause 눌러짐")
        flag01 = True
        th1.start()
    elif order == 1.1:
        print("pause 떼어짐")
        flag01 = False
        del th1


def clicking():
    while flag01:
        click('press')
        sleep(0.020)
        click('release')
        sleep(0.020)
    return 


def dropWeapons():  # 성능 확실함. 대신 두개의 프로그램을 열어야 한다는게 좀.. 시부레..
    keyPress('-')
    sleep(0.064)
    keyRelease('-')
    sleep(0.064)
    # 피스톨 버림
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('-')
    sleep(0.064)
    keyRelease('-')
    sleep(0.5)
    # 컴뱃피스톨 버림
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('-')
    sleep(0.064)
    keyRelease('-')
    sleep(0.5)
    # 헤비 피스톨 버림
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('-')
    sleep(0.064)
    keyRelease('-')
    sleep(0.5)
    # 빈티지 피스톨 버림
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('-')
    sleep(0.064)
    keyRelease('-')
    sleep(0.5)
    # 헤비 리볼버 버림


def keyPress(sub2):  # 키보드 눌림 신호
    sub1 = 'KD'
    trans = sub1 + sub2
    transSerial(trans)


def keyRelease(sub2):  # 키보드 릴리즈 신호
    sub1 = 'KU'
    trans = sub1 + sub2
    transSerial(trans)


def transSerial(str1):
    global ser
    ser.write(str1.encode())


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


def move(x, y):
    str1 = 'MX'
    xstr, ystr = str(x).zfill(4), str(y).zfill(4)
    str1 = str1 + xstr + ystr
    transSerial(str1)


def moveTo(x, y, i):
    xstr, ystr = str(x).zfill(4), str(y).zfill(4)
    if i == 0:
        start = 'MZ'
    else:
        start = 'Mz'
    str1 = start + xstr + ystr
    transSerial(str1)


if __name__ == "__main__":
    flag01 = False  # clicking용 플래그
    L = list()
    ser = object()
    j = 3
    while True:
        try:
            port = 'COM' + str(j)
            ser = Serial(port, 9600)
            print("{} 포트에 연결 성공".format(port))
            break
        except SerialException:
            j += 1
    server()
