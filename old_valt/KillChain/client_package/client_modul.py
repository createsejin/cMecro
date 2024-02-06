from pynput.keyboard import Listener, Key
# import os
import pickle
import re
import socket
# from threading import Thread


client_socket = object()
nickname = str()
flag03 = False  # end program
debug01 = False  # debug 모드
pressKey_released = False
i1 = 0
keySet = set()
pre_order = list()
hotkey = {
    'Ctrl + Esc: 프로그램 종료': [{Key.ctrl_l, Key.esc}, -2, 0, 0],
    'Esc + F4: GTA 강제 종료': [{Key.esc, Key.f4}, 1, 0, 0],
}

def commander():
    ip_re = '^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$'
    while True:
        print("호스트의 IP를 입력하십시오.")
        host_ip = input(">>> ")
        if re.search(ip_re, host_ip) is None:
            print("올바른 IP 형식을 입력하십시오.")
            continue
        else:
            print("호스트에게 표시될 자신의 닉네임을 입력하십시오.")
            nickname = input(">>> ")
            get_nickname(nickname)
            client_socket = get_client_socket(host_ip)
            keyboardListener(client_socket)
            return


def get_client_socket(ip):
    port = 12345
    addr = (ip, port)
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.settimeout(3)
    str1 = 'Waiting to connect to the host'
    str2 = '.'
    lis = list()
    dot = 3 + 1
    for j in range(0, dot):
        str3 = str1 + str2 * j
        lis.append(str3)
    i = 0
    while True:
        try:
            client_socket.connect(addr)
            print("\n-> SUCCESS: connect to host")
            break
        except ConnectionRefusedError:
            print('\r', lis[i], end='', sep='')
            i += 1
            if i == dot:
                i = 0
            port += 1
        except socket.timeout:
            print("호스트와 연결 지연 시간이 초과되었습니다. 호스트 IP를 다시 확인해 주세요.")
            commander()
    return client_socket


def get_nickname(input_):
    nickname = input_
    print("your nickname =", nickname)


def operator_(order):
    if order == 1:
        input("실행 >>> ")
        print("-> kill GTA process.")
        resp = transData([1, 0, 0])
        print("resp =", resp)
        # os.system('taskkill /f /im GTA5.exe')


keyboardListener_stop = False
def transData(trans):
    global keyboardListener_stop
    send_data = pickle.dumps(trans)
    try:
        client_socket.send(send_data)
    except ConnectionResetError:
        print("호스트가 프로그램을 종료하였습니다.")
        keyboardListener_stop = True
        return -1
    recvData = client_socket.recv(1024)
    respL = pickle.loads(recvData)
    return respL


def on_press(key):
    global keySet, pre_order, i1, pressKey_released
    if keyboardListener_stop:
        return False
    keySet.add(key)
    pressKey_released = False
    if i1 == 0:
        i1 += 1
        if debug01:
            print("press key =", key)
            print("keySet =", keySet)
        for action, trigger in hotkey.items():
            check = all([True if triggerKey in keySet else False for triggerKey in trigger[0]])
            if debug01 and check and trigger[2] == 0:  # debug
                print("action =", action)
                print("press key =", key)
                print("keySet =", keySet)
                print("trigger =", trigger[0])
                print([True if triggerKey in keySet else False for triggerKey in trigger[0]])
                lineGenarator('-', 60)
            elif check and trigger[2] == 1:
                if debug01:
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
                operator_(order)


def on_release(key):
    global pre_order, flag03, pressKey_released, keySet, i1
    if keyboardListener_stop:
        return False
    if debug01:
        print("release keySet =", keySet)
    for action, trigger in hotkey.items():
        check = all([True if triggerKey in keySet else False for triggerKey in trigger[0]])
        if check:
            if check and trigger[1] == -1 and (len(keySet) == len(trigger[0])):
                keySet.clear()
                return False
            elif check and trigger[1] == -2 and (len(keySet) == len(trigger[0])):
                flag03 = True
                keySet.clear()
                client_socket.close()
                return False
            elif check and trigger[2] == 0 and trigger[3] == 1:
                order = trigger[1]
                pre_order.append(order)
                if len(pre_order) >= 3:
                    pre_order = pre_order[1:]
                operator_(order)
            elif check and (len(keySet) == len(trigger[0])) and trigger[2] == 0:
                order = trigger[1]
                pre_order.append(order)
                if len(pre_order) >= 3:
                    pre_order = pre_order[1:]
                operator_(order)
            elif check and (len(keySet) == len(trigger[0])) and trigger[2] == 1:
                pressKey_released = True
                order = trigger[1]
                operator_(order)
            if debug01:
                print("release key =", key)
    i1 = 0
    keySet.clear()


def keyboardListener(input_client_socket):  # 키보드 입력을 감지한다. 별도의 스레드 위에서 작동한다.
    global client_socket
    client_socket = input_client_socket
    transData([2, nickname, 0])
    with Listener(on_press=on_press, on_release=on_release) as listener:
        print("-> Keyboard listener started.")
        listener.join()


def lineGenarator(mark, num):
    str2 = str()
    for i in range(0, num):
        str2 += mark
    print(str2)
