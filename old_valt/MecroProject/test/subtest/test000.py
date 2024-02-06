import pyautogui as pa
from PIL import ImageGrab
from time import sleep


def test000():
    x, y = 1726, 182
    xstr, ystr = str(x).zfill(4), str(y).zfill(4)
    str1 = 'MZ' + xstr + ystr
    print(str1)


def test001():
    str1 = '='
    str2 = str()
    for i in range(0, 60):
        str2 += str1
    print(str2)


def test002():
    screen = ImageGrab.grab()
    checkPoint = [[373, 322], [445, 321]]
    pixel_list = list()
    for point in checkPoint:
        po = pa.position(point[0], point[1])
        pix = screen.getpixel(po)
        pixel_list.append(pix)
    del screen
    print(pixel_list)
    if pixel_list == [(0, 0, 0), (0, 0, 0)]:
        print("checked")


if __name__ == "__main__":
    test002()
