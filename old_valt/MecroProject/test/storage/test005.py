from pynput.keyboard import Listener, Key, KeyCode

store = set()

HOT_KEYS = {
    'print_hello': [{Key.alt_l, KeyCode(char='1')}, 1],
    'print_hihi': [{Key.alt_l, KeyCode(char='2')}, 2],
    'test_hotkey001': [{KeyCode(vk=97), KeyCode(vk=98)}, 3]
}


def print_hello():
    print('hello, World!!')
    return


def print_hihi():
    print('hihi')
    return


def test_func():
    print('hehe')


def execute_func(num):
    if num == 1:
        print_hello()
    elif num == 2:
        print_hihi()
    elif num == 3:
        test_func()


def handleKeyPress(key):
    store.add(key)


def handleKeyRelease(key):
    for action, trigger in HOT_KEYS.items():
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
