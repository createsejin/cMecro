from pynput.keyboard import Listener, Key, KeyCode

keyStore = set()
hotkey = {
    'press alt+1': {Key.alt_l, KeyCode(char='1')},
    'test000': {Key.ctrl_l, Key.shift_l, KeyCode(vk=49)}
    # 특수키는 Key.ctrl_l로 쓰고, 그외 키는 KeyCode(vk=29) 로 쓴다.
    # 만약에 이래도 안맞으면 keyStore와 밑에 트리거 키값 비교하면서 적절히 해주면 됨.
}


def hotkeyIndex(input_):
    if input_ == 'press alt+1':
        return 1
    elif input_ == 'press alt+2':
        return 2
    elif input_ == 'test000':
        return 3
    else:
        return 0


def handleKeyPress(key):
    keyStore.add(key)


i = 0


def handleKeyRelease(key):
    global i
    for action, trigger in hotkey.items():
        print("keyStore =", keyStore)
        print("trigger =", trigger)
        print([True if triggerKey in keyStore else False for triggerKey in trigger])
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
