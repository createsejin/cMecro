import socket
import pickle
from threading import Thread
from pynput.keyboard import Listener, Key, KeyCode
import re
import os
import win32api


key1 = KeyCode(char='1')
client_socket = object()
host_ip = str()
nickname = str()
flag03 = False  # end program
debug01 = False  # debug 모드
pressKey_released = False
keyboardListener_stop = False
i1 = 0
keySet = set()
pre_order = list()
client_number = 3
client_char = str(client_number)
# 110.15.2.9
hotkey = {
    '` + n: n번 클라이언트 프로그램 종료': [{KeyCode(char='`'), KeyCode(char=client_char)}, -2, 0, 0],
    'Esc + n: n번 클라이언트 GTA 강제 종료': [{Key.esc, KeyCode(char=client_char)}, 1, 0, 0],
    'F2 + n: n번 클라이언트 신호 TEST': [{Key.f2, KeyCode(char=client_char)}, 2, 0, 0],
    # 'Ctrl + Esc: 프로그램 종료': [{Key.ctrl_l, Key.esc}, -2, 0, 0],
    # 'Esc + F1: 그타 강종': [{Key.esc, Key.f1}, 1, 0, 0],
    # 'F2: 신호 test': [{Key.f2}, 2, 0, 0],
}


def handle_exit(signal):
    print("exit", signal)
    try:
        send_data([3, nickname, 0])
    except AttributeError:
        print("AttributeError is raised.")
        return


win32api.SetConsoleCtrlHandler(handle_exit, True)


def receiver():
    global client_socket, keyboardListener_stop
    try:
        while True:
            b_data = client_socket.recv(1024)
            respL = pickle.loads(b_data)
            operate_for_receiver(respL)
            if flag03:
                client_socket.close()
                return
    except ConnectionResetError:
        print("호스트가 서버 프로그램을 종료하였습니다. 프로그램을 종료하려면 아무키나 누르십시오.")
        client_socket.close()
        keyboardListener_stop = True
        return -1


def send_data(L):
    b_data = pickle.dumps(L)
    client_socket.send(b_data)


def operate_for_receiver(L):
    if L[0] == -1:
        if nickname != L[1]:
            print("nickname: '{}'님이 GTA 프로세스 종료를 요청하셨습니다.".format(L[1]))
            print("-> Kill GTA process.")
            os.system('taskkill /f /im GTA5.exe')
        else:
            print("GTA 프로세스 종료를 요청했습니다.")
    elif L[0] == -2:
        if nickname != L[1]:
            print("nickname: '{}'님이 접속하셨습니다.".format(L[1]))
        else:
            print("호스트(ip='{}')의 서버 프로그램에 접속하였습니다.".format(host_ip))
        print(L[2])
    elif L[0] == -3:
        if nickname != L[1]:
            print("nickname: '{}'님이 프로그램을 종료하셨습니다.".format(L[1]))
        else:
            print("프로그램을 종료합니다.")
        print(L[2])
    elif L[0] == -4:
        if nickname != L[1]:
            print("nickname: '{}'님이 신호 TEST를 요청하셨습니다.".format(L[1]))
            print("신호 송수신 상태 양호.")
        else:
            print("신호 송수신 상태 양호.")


def keyboard_operator(order):
    if order == 1:
        print("-> Kill GTA process.")
        os.system('taskkill /f /im GTA5.exe')
        reqL = [1, nickname, 0]
        send_data(reqL)
    elif order == 2:
        print("-> signal test operated.")
        L = [4, nickname, 0]
        send_data(L)


def init_client():
    global client_socket
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    port = 12345
    addr = (host_ip, port)
    while True:
        try:
            client_socket.settimeout(15)
            print("Attempt to connect to the server program on the host.")
            client_socket.connect(addr)
            client_socket.settimeout(None)
            send_data([2, nickname, 0])
            break
        except TimeoutError:
            print("호스트의 서버 프로그램 연결 지연 시간이 초과되었습니다.\n"
                  "호스트의 서버 프로그램 접속 여부와 IP를 다시 확인해 주세요.")
            client_socket.close()
            commander()
        except socket.timeout:
            print("호스트의 서버 프로그램 연결 지연 시간이 초과되었습니다.\n"
                  "호스트의 서버 프로그램 접속 여부와 IP를 다시 확인해 주세요.")
            client_socket.close()
            commander()
        except OSError as e:
            print("OSError =", e)
            return


def lineGenarator(mark, num):
    str2 = str()
    for i in range(0, num):
        str2 += mark
    print(str2)


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
                keyboard_operator(order)


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
                print("-> end program")
                send_data([3, nickname, 0])
                flag03 = True
                keySet.clear()
                return False
            elif check and trigger[2] == 0 and trigger[3] == 1:
                order = trigger[1]
                pre_order.append(order)
                if len(pre_order) >= 3:
                    pre_order = pre_order[1:]
                keyboard_operator(order)
            elif check and (len(keySet) == len(trigger[0])) and trigger[2] == 0:
                order = trigger[1]
                pre_order.append(order)
                if len(pre_order) >= 3:
                    pre_order = pre_order[1:]
                keyboard_operator(order)
            elif check and (len(keySet) == len(trigger[0])) and trigger[2] == 1:
                pressKey_released = True
                order = trigger[1]
                keyboard_operator(order)
            if debug01:
                print("release key =", key)
    i1 = 0
    keySet.clear()


def main():
    init_client()
    receiver_thread = Thread(target=receiver, daemon=True)
    receiver_thread.start()
    with Listener(on_press=on_press, on_release=on_release) as listener:
        print("-> Keyboard Listener is started.")
        print("사용 전 반드시 미리 상의하여 TEST하여 서로의 GTA가 정상적으로 종료되는지 TEST 후 사용하시길 바랍니다.")
        print("서로의 신호 연결 상태를 TEST하려면 'F2'키를 누르십시오.")
        print("각 멤버에게 GTA 강제종료를 요청하려면 'Esc + F1'을 누르십시오.")
        listener.join()


def commander():
    global host_ip, nickname
    ip_re = '^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$'
    while True:
        print("호스트의 IP를 입력하십시오.")
        host_ip = input(">>> ")
        if re.search(ip_re, host_ip) is None:
            print("올바른 IP 형식을 입력하십시오.")
            continue
        else:
            while True:
                print("호스트에게 표시될 자신의 닉네임을 입력하십시오.")
                nickname = input(">>> ")
                if nickname == '' or nickname is None:
                    continue
                else:
                    main()
                    return


# 110.15.2.9
if __name__ == '__main__':
    commander()
