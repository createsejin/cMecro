import serial
import time
from pynput.keyboard import Listener, Key


def handlePress(key):
    print('Press: {}'.format(key))


def handleRelease(key):
    print('Released {}'.format(key))
    if key == Key.esc:
        return False


with Listener(on_press=handlePress, on_release=handleRelease) as listener:
    listener.join()
