from pynput.keyboard import Listener, Key

store = set()


def handleKeyPress(key):
    store.add(key)
    print('Press: {}'.format(store))


def handleRelease(key):
    print('Released {}'.format(key))
    if key in store:
        store.remove(key)
    if key == Key.esc:
        return False


with Listener(on_press=handleKeyPress, on_release=handleRelease) as listener:
    listener.join()
