from time import sleep
import pickle
from pprint import pprint as pp
import mss
import mss.tools
import sys
import os


def test000():
    str1 = 'Waiting to connect to the server'
    str2 = '.'
    lis = list()
    dot = 3 + 1
    for j in range(0, dot):
        str3 = str1 + str2 * j
        lis.append(str3)
    i = 0
    while True:
        print('\r', lis[i], end='', sep='')
        sleep(0.4)
        i += 1
        if i == dot:
            i = 0


def test001():
    str1 = 'Waiting to connect to the server'
    str2 = '.'
    str1 = str1 + str2*3
    print(str1)


def test002():
    user = {'name': 'Andrew K. Johnson', 'score': 199, 'location': [38.189323, 127.3495672]}

    # save data
    # with open('user.pickle', 'wb') as fw:
    #     pickle.dump(user, fw)

    # load data
    with open('user.pickle', 'rb') as fr:
        user_loaded = pickle.load(fr)

    # show data
    print(user_loaded['name'])


def test003():
    pixel_dict = dict()
    key = 1
    script = '무기 휠 1번 감지'
    checkPoint = [(941, 524), (945, 522), (945, 526), (945, 541)]
    pixel = [(254, 254, 254), (255, 255, 255), (255, 255, 255), (101, 101, 101)]
    list1 = [script, checkPoint, pixel]
    pixel_dict[key] = list1
    key = 2
    script = '추방 화면 pixel picker test'
    checkPoint = [(787, 769), (781, 761), (787, 755), (773, 753), (774, 769)]
    pixel = None
    list1 = [script, checkPoint, pixel]
    pixel_dict[key] = list1
    # pixel_dict[2][2] = [(255, 255, 255)]
    pp(pixel_dict)
    print(pixel_dict[1][1][0][1])
    #                |  |  |  |-> inner tuple index
    #                |  |  |-> inner list index
    #                |  |-> main list index = 0: script, 1: check point, 2: pixel color
    #                |-> key
    # save data
    with open('pixel_dict.dict', 'wb') as fw:
        pickle.dump(pixel_dict, fw)


def test004():
    pixel_dict = dict()
    with open('pixel_dict.dict', 'rb') as fr:
        pixel_dict = pickle.load(fr)
    pp(pixel_dict)


def test005():
    pixel_pointer_dict = {
        'inital_window_move': {
            1: (1726, 182), 2: (967, 141)
        },
        'weapon bbox': (846, 475, 1081, 512),
    }
    pixel_dict = {
        'Arm': {
            'pistol': {
                1: {
                    'sc': '더블액션 리볼버',
                    'cp': [(33, 9), (35, 26), (108, 14), (114, 18), (205, 19)],
                    'pc': [(179, 179, 179), (65, 65, 65), (99, 99, 99), (97, 97, 97), (94, 94, 94)]
                },
                2: {
                    'sc': 'AP 피스톨',
                    'cp': [(56, 29), (87, 18), (120, 27), (62, 13), (147, 18)],
                    'pc': [(170, 170, 170), (181, 181, 181), (118, 118, 118), (229, 229, 229), (217, 217, 217)]
                },
                3: {
                    'sc': '신호탄 총',
                    'cp': [(67, 9), (64, 24), (105, 17), (128, 14), (167, 17)],
                    'pc': [(220, 220, 220), (226, 226, 226), (214, 214, 214), (131, 131, 131), (207, 207, 207)]
                },
                4: {
                    'sc': '아토마이저',
                    'cp': [(19, 10), (72, 25), (214, 18), (136, 16), (162, 21)],
                    'pc': [(220, 220, 220), (145, 145, 145), (209, 209, 209), (118, 118, 118), (175, 175, 175)]
                }
            },
            'rifle': {

            },
            'machinegun': {

            },
        },
        'm': {

        },
    }


def test006():  # pickle 직렬화 바이트 파일 저장
    i4 = 12
    with open('i4.dat', 'wb') as file_write:
        pickle.dump(i4, file_write)


def test007():  # pickle 역직렬화로 바이트 파일 읽기
    with open('i4.dat', 'rb') as file_read:
        i4 = pickle.load(file_read)
        print(i4)
        print(type(i4))


def test008():
    num = 0
    for i in range(0, num):
        print("hello")


def test009():
    import win32gui
    window = ''
    gta = 'Grand Theft Auto V'
    while True:
        current_window = win32gui.GetWindowText(win32gui.GetForegroundWindow())
        if window != current_window:
            window = current_window
            print(window)
        sleep(1)


def test010():
    list1 = list()
    print(list1[0])


def test011():
    presented_b = (1037, 417, 1260, 745)
    image = mss.mss().grab(presented_b)  # type: ignore
    mss.tools.to_png(image.rgb, image.size, output='testshot00001.png')


def test012():
    str1 = '01'
    int1 = int(str1[:1])
    int2 = int(str1[-1])
    print("int1 =", int1)
    print("int2 =", int2)


def test013():
    print(os.system('tasklist'))
    os.system('taskkill /f /im GTA5.exe')


def test014():
    import clipboard
    result = clipboard.paste()
    print(type(result))
    print(result)
    for a in result:
        print(a)


if __name__ == "__main__":
    test014()
