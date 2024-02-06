import pickle
import socket
import threading
from pynput.keyboard import Listener, Key, KeyCode
from time import sleep
from threading import Thread


def lineGenarator(mark, num):
    str2 = str()
    for i in range(0, num):
        str2 += mark
    print(str2)


def operator_(order):
    if order == 1:
        print("test key order operated !!!")
        reqL = [1, 1, 2]
        send_data = pickle.dumps(reqL)
        soc.send(send_data)
        recvData = soc.recv(1024)
        respL = pickle.loads(recvData)
        print(respL)


def on_press(key):
    global keySet, pre_order
    keySet.add(key)
    if debug01:
        print("press key =", key)
        print("keySet =", keySet)
    for action, trigger in hotkey.items():
        check = all([True if triggerKey in keySet else False for triggerKey in trigger[0]])
        if debug01 and check:  # 입력 감지값인 keySet과 핫키의 trigger의 동일 여부를 분석한다.
            print("action =", action)
            print("press key =", key)
            print("keySet =", keySet)
            print("trigger =", trigger[0])
            print([True if triggerKey in keySet else False for triggerKey in trigger[0]])
            lineGenarator('-', 60)


def on_release(key):
    global pre_order, flag03, keySet
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
            elif check and (len(keySet) == len(trigger[0])):
                print("release key =", key)
                order = trigger[1]
                pre_order.append(order)
                if len(pre_order) >= 3:
                    pre_order = pre_order[1:]
                operator_(order)
    keySet.clear()


def keyboardListener():  # 키보드 입력을 감지한다. 별도의 스레드 위에서 작동한다.
    with Listener(on_press=on_press, on_release=on_release) as listener:
        listener.join()


def commander():
    while True:
        cmd = input(">>> ")
        if cmd == 'start':
            connectSocket()
            keyboardListener()
        elif cmd == 'end':
            exit(0)
        elif cmd == 'test':
            pass
        else:
            print("Please enter a correct command.")


def connectSocket():
    global soc
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as soc:
        soc.connect(('localhost', 12346))


if __name__ == "__main__":
    soc = object()
    flag03 = False  # end program
    debug01 = True  # debug 모드 flag
    listener = object()
    keySet = set()
    pre_order = list()
    H_state = 1
    hotkey = {
        'Ctrl + Esc: exit program': [{Key.ctrl_l, Key.esc}, -2, 0],  # end p1rogram
        ' ` + Esc: exit to command mode': [{Key.esc, KeyCode(char='`')}, -1, 0],  # interruption or escape, Esc + '`'
        'pause: ': [{Key.pause}, 1, 0],  # pause
    }
    commander()
