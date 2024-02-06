from pynput.keyboard import Listener, Key, KeyCode
from serial import Serial
import time

ser = Serial('COM6', 9600)
store = set()
HOT_KEYS = {
    'press_1': [{Key.alt_l, KeyCode(char='1')}, 1],
    'press_2': [{Key.alt_l, KeyCode(char='2')}, 2],
    'clicking': [{Key.pause}, 3]
}
pressOperation = {3}


def execute_func(num):
    if num == 1:
        press_1()
    elif num == 2:
        press_2()
    elif num == 3:
        clicking()


def press_1():
    sendSerial('1')
    pause()


def press_2():
    sendSerial('2')
    pause()


def clicking():
    sendSerial('1')


def sendSerial(data):
    data = data.encode('utf-8')
    ser.write(data)


def pause():
    val = '0'
    val = val.encode('utf-8')
    ser.write(val)


def handleKeyPress(key):
    store.add(key)
    for action, trigger in HOT_KEYS.items():
        if trigger[1] in pressOperation:
            check = all([True if triggerKey in store else False for triggerKey in trigger[0]])
            if check:
                try:
                    num = trigger[1]
                    if callable(execute_func(num)):
                        execute_func(num)
                except NameError as err:
                    print(err)


def handleKeyRelease(key):
    for action, trigger in HOT_KEYS.items():
        if trigger[1] in pressOperation:
            pause()
        else:
            print([True if triggerKey in store else False for triggerKey in trigger[0]])
            CHECK = all([True if triggerKey in store else False for triggerKey in trigger[0]])
            if CHECK:
                try:
                    num = trigger[1]
                    if callable(execute_func(num)):
                        execute_func(num)
                except NameError as err:
                    print(err, "-errlog")
    if key == Key.esc:
        return False
    elif key in store:
        store.remove(key)


with Listener(on_press=handleKeyPress, on_release=handleKeyRelease) as listener:
    listener.join()


# def on_press(key):
#     store.add(key)
#     for action, trigger in HOT_KEYS.items():
#         if trigger[1] in pressOperation:
#             check = all([True if triggerKey in store else False for triggerKey in trigger[0]])
#             if check:
#                 try:
#                     num = trigger[1]
#                     if callable(execute_func(num)):
#                         execute_func(num)
#                 except NameError as err:
#                     print(err)
#
#
# def on_release(key):
#     pause()
#     if key == Key.esc:
#         return False
#     elif key in store:
#         store.remove(key)
#
#
# with Listener(on_press=on_press, on_release=on_release) as listener2:
#     listener2.join()
