from pynput.keyboard import Listener, Key, KeyCode
from serial import Serial
from time import sleep

ser = Serial('COM6', 9600)
keyStore = set()
hotkey = {
    'press alt + 1': {Key.page_up},
    'press alt + 2': {Key.alt_l, KeyCode(char='1')},
    'test000': {KeyCode(char='g'), Key.ctrl_l}
}


def hotkeyIndex(input_):
    if input_ == 'press alt + 1':
        return 1
    elif input_ == 'press alt + 2':
        return 2
    elif input_ == 'test000':
        return 3


def execute_func(num):
    if num == 1:
        press_1()
    elif num == 2:
        press_2()
    elif num == 3:
        test000()


def test000():
    sendSerial('1')
    sleep(0.03)
    sendSerial('2')
    sleep(0.03)
    sendSerial('1')
    sleep(0.03)
    sendSerial('2')
    pause()


def press_1():
    data = '1'
    sendSerial(data)
    pause()


def press_2():
    data = '2'
    sendSerial(data)
    pause()


def sendSerial(data):
    data = data.encode('utf-8')
    ser.write(data)


def pause():
    val = '0'
    val = val.encode('utf-8')
    ser.write(val)


def handleKeyPress(key):
    keyStore.add(key)


i = 0


def handleKeyRelease(key):
    global i
    for action, trigger in hotkey.items():
        print([True if triggerKey in keyStore else False for triggerKey in trigger], ", i =", i)
        CHECK = all([True if triggerKey in keyStore else False for triggerKey in trigger])
        if CHECK:
            try:
                num = hotkeyIndex(action)
                print(num)
                # execute_func(num)
            except NameError as err:
                print(err, "-errlog")
    if key == Key.esc:
        return False
    elif key in keyStore:
        keyStore.remove(key)
    i += 1
    print("-----------------------------------------------")


with Listener(on_press=handleKeyPress, on_release=handleKeyRelease) as listener:
    listener.join()
