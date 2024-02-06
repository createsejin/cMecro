from pynput.keyboard import Listener, Key, KeyCode
from serial import Serial
import time

ser = Serial('COM6', 9600)
store = set()
HOT_KEYS = {
    'press_1': [{Key.alt_l, KeyCode(char='1')}, 1],
    'press_2': [{Key.alt_l, KeyCode(char='2')}, 2],
    'clicking': [{KeyCode(vk=97), KeyCode(vk=98)}, 4]
}
pressKey = {3, 4}


def execute_func(num):
    if num == 1:
        pass
    elif num == 2:
        pass
    elif num == 3:
        clicking()
    elif num == 4:
        test001()


def clicking():
    pass


def test001():
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
        if trigger[1] in pressKey:
            CHECK = all([True if triggerKey in store else False for triggerKey in trigger[0]])
            if CHECK:
                try:
                    num = trigger[1]
                    if callable(execute_func(num)):
                        execute_func(num)
                except NameError as err:
                    print(err, "-errlog")


def handleKeyRelease(key):
    for action, trigger in HOT_KEYS.items():
        if trigger[1] not in pressKey:
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
