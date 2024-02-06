from pynput.keyboard import Listener, Key, KeyCode
from time import sleep
from serial import Serial, SerialException
import pickle
import win32gui
import sys


def getSerial():
    j = 3
    while True:
        try:
            port = 'COM' + str(j)
            ser = Serial(port, 9600)
            print("Arduino is connected to the {} port successfully.".format(port))
            return ser
        except SerialException:
            j += 1
ser = getSerial()
delay = 0.010
window_name = '디아블로 III'
flag03 = False  # end program
debug01 = False  # debug 모드 flag
debug_pix = False  # pixel operation debug
pressKey_released = False  # press 모드 핫키의 release 신호
pause_mecro = False
is_active = False
i1, i2, i3, i4 = 0, 0, 0, 0  # 스레드 객체의 최초 실행, 오직 한 번만 실행을 위해 사용되는 변수이다.
i6, i7 = 0, 0
keySet = set()  # pynput Keyboard Listener가 감지하는 키를 저장하는 집합이다.
pre_order = list()  # 이전 명령 number를 저장하기 위한 리스트다. 크기는 2를 유지한다.


hotkey = {  # '핫키: 기능설명': [{KeyCode, KeyCode}, 2, 0, 1]
    #                                                |  |  |-> 다른키와 같이 눌러져도 실행됨.
    #                                                |  |-> 눌렀다 떼면 종료되는 키 : 비권장. 블로킹 이슈 걸림.
    #                                                |-> order number
    'Ctrl + Esc: exit program': [{Key.ctrl_l, Key.esc}, -2, 0, 0],
    'F2 + Esc: exit to command mode': [{Key.esc, Key.f2}, -1, 0, 0],
    'w: left click': [{KeyCode(char='w')}, 1, 0, 1],
    'W: left click': [{KeyCode(char='W')}, 1, 0, 1],
    'e: right click': [{KeyCode(char='e')}, 2, 0, 1],
    'E: right click': [{KeyCode(char='E')}, 2, 0, 1],
}


def operator_(order):
    if order == 1:
        if sensitiveKeyLocking():
            return
        else:
            left_click()
    elif order == 2:
        if sensitiveKeyLocking():
            return
        else:
            right_click()


def left_click():
    click('press')
    sleep(delay)
    click('release')
    return


def right_click():
    click('press', button='r')
    sleep(delay)
    click('release', button='r')
    return


def sensitiveKeyLocking():
    current_window = win32gui.GetWindowText(win32gui.GetForegroundWindow())
    if current_window == window_name:
        return False
    else:
        return True


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


def lineGenarator(mark, num):
    str2 = str()
    for i in range(0, num):
        str2 += mark
    print(str2)


def on_press(key):
    global keySet, pre_order, i1, pressKey_released
    keySet.add(key)
    pressKey_released, pressKey_released_1, pressKey_released_2 = False, False, False
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
            elif check and (len(keySet) == len(trigger[0])) and trigger[2:] == [1, 0]:
                pressKey_released = True
                order = trigger[1]
                operator_(order)
            elif check and trigger[2:] == [1, 1]:
                order = trigger[1]
                pressKey_released = True
                operator_(order)
            if debug01:
                print("release key =", key)
    i1 = 0
    keySet.clear()


def keyboardListener():  # 키보드 입력을 감지한다. 별도의 스레드 위에서 작동한다.
    with Listener(on_press=on_press, on_release=on_release) as listener:
        print("Keyboard listener started.")
        listener.join()


def commander():
    global flag03, debug01, debug_pix, i4, ser
    i = 0
    while True:
        if flag03:
            ser.close()
            sys.stdout.flush()
            exit(0)
        else:
            if i == 0:
                command = 'start'  # 최초에는 키보드 리스너를 start 한다.
                i += 1
            else:  # 이스케이프 핫키(Esc)를 눌러서 command 모드로 진입한다.
                command = input(">>> ").lower()
            if command == 'start':
                keyboardListener()
            elif command == 'hotkey':
                for key in hotkey.keys():
                    print(key)
            elif command == 'end':  # 프로그램을 종료한다.
                ser.close()
                sys.stdout.flush()
                exit(0)
            elif command == 'debug 0' or command == 'debug 1':  # 디버그 모드
                # 0: 디버그 모드 Off, 1: 디버그 모드 On
                int1 = int(command.strip()[-1])
                if int1 == 0:
                    debug01 = False
                elif int1 == 1:
                    debug01 = True
                keyboardListener()
            elif command == 'debug':  # 디버그 모드 토글링
                debug01 = not debug01
                keyboardListener()
            elif command == 'debug pix':  # 픽셀 피커 디버그 모드 토글링
                debug_pix = not debug_pix
                keyboardListener()
            elif command == 'img':  # grab image number 지정
                while True:
                    try:
                        with open('F:\GTA5\MecroProject\Mecro\i4.dat', 'rb') as file_read:
                            i4 = pickle.load(file_read)
                        print("현재 i4 =", i4)
                        cmd = input("숫자 입력 >>> :")
                        if cmd == 'cancel':
                            break
                        else:
                            i4 = int(cmd)
                            with open('F:\GTA5\MecroProject\Mecro\i4.dat', 'wb') as file_write:
                                pickle.dump(i4, file_write)
                            print("img file name number =", i4)
                            break
                    except ValueError:
                        print("숫자를 입력하세요.")
                keyboardListener()
            else:
                print("올바른 명령어를 입력하세요.")


if __name__ == "__main__":
    commander()
