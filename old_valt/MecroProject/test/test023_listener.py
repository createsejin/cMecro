import socket
import pickle
import threading
from pynput.keyboard import Listener, Key, KeyCode
from time import sleep
from threading import Thread


def lineGenarator(mark, num):
    str2 = str()
    for i in range(0, num):
        str2 += mark
    print(str2)


def on_press(key):
    global keySet, pre_order, i1, flag07
    keySet.add(key)
    flag07 = False
    if i1 == 0:
        i1 += 1
        if debug01:
            print("press key =", key)
            print("keySet =", keySet)
        for action, trigger in hotkey.items():
            check = all([True if triggerKey in keySet else False for triggerKey in trigger[0]])
            if debug01 and check and trigger[2] == 0:
                print("action =", action)
                print("press key =", key)
                print("keySet =", keySet)
                print("trigger =", trigger[0])
                print([True if triggerKey in keySet else False for triggerKey in trigger[0]])
                lineGenarator('-', 60)
            elif check and trigger[2] == 1:
                print("action =", action)
                print("press key =", key)
                print("keySet =", keySet)
                print("trigger =", trigger[0])
                print([True if triggerKey in keySet else False for triggerKey in trigger[0]])
                lineGenarator('-', 60)
                order = trigger[1]
                pre_order.append(order)
                if len(pre_order) >= 3:
                    pre_order = pre_order[1:]
                transportOrder(order)


def on_release(key):
    global pre_order, flag03, flag07, keySet, i1
    for action, trigger in hotkey.items():
        check = all([True if triggerKey in keySet else False for triggerKey in trigger[0]])
        if check:
            if check and trigger[1] == -1 and (len(keySet) == len(trigger[0])):
                keySet.clear()
                return False
            elif check and trigger[1] == -2 and (len(keySet) == len(trigger[0])):
                flag03 = True
                keySet.clear()
                return False
            elif check and (len(keySet) == len(trigger[0])) and trigger[2] == 0:
                print("release key =", key)
                order = trigger[1]
                pre_order.append(order)
                if len(pre_order) >= 3:
                    pre_order = pre_order[1:]
                transportOrder(order)
            elif check and (len(keySet) == len(trigger[0])) and trigger[2] == 1:
                flag07 = True
                order = trigger[1]
                pre_order.append(order)
                if len(pre_order) >= 3:
                    pre_order = pre_order[1:]
                transportOrder(order)
    i1 = 0
    keySet.clear()


def keyboardListener():  # 키보드 입력을 감지한다. 별도의 스레드 위에서 작동한다.
    with Listener(on_press=on_press, on_release=on_release) as listener:
        print("Keyboarc Listener started")
        listener.join()


def transData(trans):
    send_data = pickle.dumps(trans)
    client_socket.send(send_data)
    recvData = client_socket.recv(65535)
    respL = pickle.loads(recvData)
    return respL


def transportOrder(order):
    if order == 1:
        if not flag07:
            print("test key pressed")
            resp = transData([1, 0, 0])
            print(resp)
        elif flag07:
            print("test key released")
            resp = transData([1, 1, 0])
            print(resp)
    elif order == 2:
        transData([2, 0, 0])


def commander():
    global flag03, debug01
    i = 0
    while True:
        if flag03:
            transData(['exit program'])
            exit(0)
        else:
            if i == 0:
                command = 'start'  # 최초에는 키보드 리스너를 start 한다.
                i += 1
            else:  # 이스케이프 핫키(Esc)를 눌러서 command 모드로 진입한다.
                command = input(">>> ")
            if command == 'start':
                keyboardListener()
            elif command == 'end':  # 프로그램을 종료한다.
                transData(['exit program'])
                exit(0)
            elif command == 'debug 0' or command == 'debug 1':  # 디버그 모드
                # 0: 디버그 모드 Off, 1: 디버그 모드 On
                int1 = int(command.strip()[-1])
                if int1 == 0:
                    flag04 = False
                elif int1 == 1:
                    flag04 = True
                keyboardListener()
            elif command == 'debug':  # 디버그 모드 토글링 -> 핫키 모니터링은 꺼짐.
                flag04 = not flag04
                keyboardListener()
            else:
                print("올바른 명령어를 입력하세요.")


if __name__ == "__main__":
    flag03 = False  # end program
    debug01 = False  # debug 모드 flag
    flag07 = False  # press 모드 핫키의 release 신호
    i1 = 0
    listener = object()
    pre_order = list()
    keySet = set()
    hotkey = {  # [0]: 핫키, [1]: order number, [2]: press = 1, release = 0
        'Ctrl + Esc: exit program': [{Key.ctrl_l, Key.esc}, -2, 0],  # end program
        ' ` + Esc: exit to command mode': [{Key.esc, KeyCode(char='`')}, -1, 0],  # interruption or escape, Esc + '`'
        'pause: 무한 클릭': [{Key.pause}, 1, 1],  # pause
        'Ctrl + pause: ': [{Key.ctrl_l, KeyCode(vk=3)}, 2, 0],
        '*': [{KeyCode(char='*')}, 3, 1],
    }
    addr = ('localhost', 12346)
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect(addr)
    commander()
