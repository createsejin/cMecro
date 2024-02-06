from pynput.keyboard import Listener, Key, KeyCode
from serial import Serial, SerialException

j = 3
while True:
    try:
        port = 'COM'+str(j)
        ser = Serial(port, 9600)
        print("{} 포트에 연결 성공".format(port))
        break
    except SerialException:
        j += 1

keySet = set()
hotkey = {
    'exit': [{Key.ctrl_l, Key.esc}, -1],  # interruption or escape
    'clicking': [{Key.pause}, 1],
    'clicking_break': [{Key.ctrl_l, Key.pause}, 2],
    'testKey': [{Key.ctrl_l, KeyCode(vk=66)}, 3]
}


def execute_order(num):
    if num == 1:
        clicking()
    elif num == 2:
        pause()
    elif num == 3:
        testKey()


def testKey():
    sendSeri('2')
    respSeri()
    pause()


i = 0


def clicking():
    global i
    sendSeri('1')
    i += 1
    print("i =", i)
    respSeri()


def sendSeri(data):
    print("send data =", data)
    data = data.encode('utf-8')
    ser.write(data)


def respSeri():
    if ser.readable():
        print("readSerial : [", end='')
        response = ser.readline()
        print(response[:-1].decode(), end='')
        print("]")


def pause():
    data = '0'
    data = data.encode('utf-8')
    ser.write(data)
    respSeri()


def on_press(key):
    keySet.add(key)


def on_release(key):
    for action, trigger in hotkey.items():
        check = all([True if triggerKey in keySet else False for triggerKey in trigger[0]])
        if check and trigger[1] == -1:
            return False
        elif check:
            execute_order(trigger[1])
            print("order_number =", trigger[1])
    if key in keySet:
        keySet.remove(key)
    if 'a' in keySet:
        keySet.remove('a')
    print(keySet)


with Listener(on_press=on_press, on_release=on_release) as listener:
    listener.join()
