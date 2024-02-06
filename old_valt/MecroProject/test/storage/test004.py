from pynput.keyboard import Listener, Key, KeyCode

store = set()

HOT_KEYS = {
    'print_hello': {Key.alt_l, KeyCode(char='1')},
    'print_hihi': {Key.alt_l, KeyCode(char='2')}
}

hotkeyBind = {
    'print_hello': 1,
    'print_hihi': 2
}


def print_hello():
    print('hello, World!!')
    return


def print_hihi():
    print('hihi')
    return


def execute_func(num):
    if num == 1:
        return print_hello()
    elif num == 2:
        return print_hihi()


def handleKeyPress(key):
    store.add(key)


i = 0


def handleKeyRelease(key):
    global i
    for action, trigger in HOT_KEYS.items():
        print([True if triggerKey in store else False for triggerKey in trigger], ", i =", i)
        i += 1
        CHECK = all([True if triggerKey in store else False for triggerKey in trigger])
        if CHECK:
            try:
                func = eval(action)
                if callable(func):
                    func()
            except NameError as err:
                print(err, "-errlog")
    if key == Key.esc:
        return False
    elif key in store:
        store.remove(key)


with Listener(on_press=handleKeyPress, on_release=handleKeyRelease) as listener:
    listener.join()
