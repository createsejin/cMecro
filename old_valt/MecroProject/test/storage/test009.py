from pynput import keyboard


def on_active_h():
    print("H method active")


def on_active_i():
    print("I method active")


def on_active_j():
    print("test")


with keyboard.GlobalHotKeys({'<ctrl>+<alt>+h': on_active_h,
                             '<ctrl>+<alt>+i': on_active_i},
                            '<') as h:
    h.join()
