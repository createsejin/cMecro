# import threading
from mecro_packge.mecro004 import Arduino001
from pynput.keyboard import Listener, Key, KeyCode
from time import sleep, time
from threading import Thread
from serial import Serial, SerialException
from PIL import ImageGrab
import socket
import pickle
import win32gui
import mss
import mss.tools
import pyautogui as pa
from mecro003 import Arduino000
import os
import sys

# from pprint import pprint
"""
python shell command :

import sys
sys.path.append('F:\\GTA5\\MecroProject\\Mecro')
exec(open("F:\\GTA5\\MecroProject\\Mecro\\mecro002.py", 'rt', encoding='UTF8').read())

아두이노 하드리셋 : 빈 스케치 스크립트 준비, 업로드 후 0.5초 뒤에 리셋 버튼 클릭
스레드 이슈가 있었다.. opreator를 스레드에 올려서 동작시켜야한다는 것이었다. 
"""
# 전역변수 영역
# pixel dict
bbox_dict = {
    'inital_window_move': {
        1: (1726, 182), 2: (967, 141)
    },
    'window setting check': (2, 129, 128, 146),
    'interactive menu': (30, 265, 461, 957),
    'inter_menu_check': (39, 270, 179, 293),
    'snack': (35, 270, 82, 294),
    'mission': (40, 537, 448, 560),
    'inside_check': (38, 342, 177, 368),
    'target': (40, 345, 84, 479),
    'temp1': (1000, 489, 1469, 1106),
    'chat': (1225, 1183, 1284, 1202),
}
pixel_dict = {
    'inside_check': {
        'nomal': {
            'cp': [(13, 12), (14, 10), (26, 6), (30, 20)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240)],
        },
        'inside': {
            'cp': [(6, 6), (12, 9), (71, 18), (15, 18)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240)],
        },
        'luxury': {
            'cp': [(19, 7), (61, 6), (61, 10), (56, 15)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240)],
        },
        'office': {
            'cp': [(13, 18), (29, 20), (129, 13), (110, 15)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240)],
        },
    },
    'window setting check': {
        'check': {
            'cp': [(5, 4), (8, 5), (9, 10), (10, 13), (13, 7), (120, 4), (27, 8)],
            'pc': [(181, 196, 164), (91, 119, 64), (49, 120, 1), (49, 124, 2), (32, 53, 15),
                   (204, 255, 249), (95, 198, 249)],
        }
    },
    'snack': {
        'check': {
            'cp': [(14, 4), (35, 11), (40, 9), (41, 16), (23, 19)],
            'pc': [(42, 102, 172), (38, 93, 157), (45, 110, 185), (45, 110, 185), (41, 101, 170)],
        },
        'bp': {
            'cp': [(7, 4), (14, 12), (36, 8), (19, 19)],
            'pc': [(45, 110, 185), (45, 110, 185), (45, 110, 185), (45, 109, 184)],
        },
    },
    'sparrow spawnable': {
        'cp': [(98, 351), (98, 329), (311, 327), (384, 315), (238, 326)],
        'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240)],
    },
    'get H_state': {
        'nomal': {
            'cp': [(35, 351), (140, 388), (286, 351), (308, 351)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240)],
        },
        'motor': {
            'cp': [(21, 362), (216, 361), (336, 388), (293, 382), (402, 358), (192, 380)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240),
                   (240, 240, 240), (240, 240, 240)],
        },
        'CEO': {
            'cp': [(212, 360), (105, 389), (46, 356), (46, 380), (139, 374)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240)],
        },
        'support': {
            'cp': [(41, 351), (138, 366), (156, 365), (177, 365)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240)],
        },
        'support2': {
            'cp': [(14, 361), (36, 362), (314, 356), (213, 360)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240)],
        },
        'employee': {
            'cp': [(73, 355), (304, 363), (227, 358), (376, 367)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240)],
        },
        'newcomer': {
            'cp': [(17, 354), (49, 356), (174, 364), (90, 398), (296, 383)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240)],
        }
    },
    'mission': {
        'check': {
            'cp': [(200, 5), (227, 15), (281, 4), (394, 4), (150, 8), (268, 12)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240),
                   (240, 240, 240)],
        }
    },
    'inter_menu_check': {
        'check': {
            'cp': [(6, 5), (19, 9), (124, 19), (133, 18)],
            'pc': [(42, 102, 172), (45, 109, 184), (44, 107, 181), (45, 110, 185)],
        }
    },
    'target': {
        1: {
            'cp': [(16, 7), (11, 9), (36, 9), (32, 44), (24, 3), (32, 84)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240),
                   (240, 240, 240), (240, 240, 240)],
        },
        2: {
            'cp': [(11, 47), (36, 48), (26, 40), (4, 52), (16, 45), (32, 122)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240),
                   (240, 240, 240)],
        },
        3: {
            'cp': [(36, 84), (16, 82), (4, 81), (11, 84), (28, 92), (35, 92)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240),
                   (240, 240, 240), (240, 240, 240)],
        },
        4: {
            'cp': [(16, 120), (14, 115), (11, 122), (27, 117), (36, 121), (19, 85)],
            'pc': [(240, 240, 240), (240, 240, 240), (240, 240, 240), (240, 240, 240),
                   (240, 240, 240), (240, 240, 240)],
        }
    },
    'chat': {
        'check': {
            'cp': [(5, 1), (2, 17), (11, 11), (39, 16), (57, 1)],
            'pc': [(155, 155, 155), (154, 154, 154), (153, 153, 153), (155, 155, 155), (155, 155, 155)],
        }
    },
}
def getSerial():
    j = 3
    while True:
        try:
            port = 'COM' + str(j)
            ser = Serial(port, 9600)
            print("Arduino is connected to the {} port successfully.".format(port))
            return ser
        except SerialException:
            j += 1
ser = getSerial()
delay = 0.015
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
listener = object()
th1 = object()
flag01 = False  # clicking
flag02 = False  # cursor location 토글 flag
flag03 = False  # end program
disconnect = False
remote_mode = False
debug01 = False  # debug 모드 flag
debug_pix = False  # pixel operation debug
flag05 = False  # sleeping 용 플래그
pressKey_released = False  # press 모드 핫키의 release 신호
pressKey_released_1 = False
pressKey_released_2 = False
flag09 = False  # pause mecro
pause_mecro = False
pause_mecro2 = False
is_active = False  # 매크로의 동작중 다른 매크로의 동작을 Locking 한다.
chat_mode = False
gta_window_name = 'Grand Theft Auto V'
i1, i2, i3, i4 = 0, 0, 0, 0  # 스레드 객체의 최초 실행, 오직 한 번만 실행을 위해 사용되는 변수이다.
i6, i7 = 0, 0
motorOrCEO = 0  # 노멀:0, 모싸: 1, CEO: 2
keySet = set()  # pynput Keyboard Listener가 감지하는 키를 저장하는 집합이다.
pre_order = list()  # 이전 명령 number를 저장하기 위한 리스트다. 크기는 2를 유지한다.
H_state = 1  # GTA의 상호작용 메뉴의 상태를 나타낸다. 1: 노말, 2: CEO or 모싸, 3: 지원조직
arduino000 = Arduino000(ser)
arduino001 = Arduino001(ser, delay)
# GTA를 먼저 실행시킨 후에 매크로를 실행시킨 뒤, GTA윈도우 윗부분을 누르고 윈도우 세팅을 해줘야
# 매크로 키입력이 먹힌다.
hotkey = {  # '핫키: 기능설명': [{KeyCode, KeyCode}, 2, 0, 1]
    #                                                |  |  |-> 다른키와 같이 눌러져도 실행됨.
    #                                                |  |-> 눌렀다 떼면 종료되는 키 : 비권장. 블로킹 이슈 걸림.
    #                                                |-> order number
    'Ctrl + Esc: exit program': [{Key.ctrl_l, Key.esc}, -2, 0, 0],
    'F2 + Esc: exit to command mode': [{Key.esc, Key.f2}, -1, 0, 0],
    'pause: 무한 클릭': [{Key.pause}, 1, 1, 0],
    'scrollLock: 부계정 원격 무한 클릭': [{Key.scroll_lock}, 1.02, 1, 0],
    'Esc + F4: GTA 강제 종료': [{Key.esc, Key.f4}, 1.1, 0, 0],
    'N + M: get H_state, motorOrCEO 1': [{KeyCode(char='N'), KeyCode(char='M')}, 1.01, 0, 0],
    'N + M: get H_state, motorOrCEO 2': [{KeyCode(char='n'), KeyCode(char='m')}, 1.01, 0, 0],
    'F11 + Backspace: 잠수': [{Key.backspace, Key.f11}, 2, 0, 0],  # F11 + Backspace
    'l + .: get pixel color': [{KeyCode(char='l'), KeyCode(char='.')}, 2.01, 0, 0],
    'l + ;: Grab Image in scope': [{KeyCode(char='l'), KeyCode(char=';')}, 2.2, 0, 0],
    'l + \': Grab Image get pixel color': [{KeyCode(char='l'), KeyCode(char='\'')}, 2.3, 0, 0],
    '; + \': test scaning screen': [{KeyCode(char=';'), KeyCode(char='\'')}, 2.4, 0, 0],
    'F2: MC': [{Key.f2}, 3, 0, 0],  # sensitive key
    'F3: CEO': [{Key.f3}, 4, 0, 0],
    '[ or {: 차량 시동1': [{KeyCode(char='[')}, 4.5, 0, 1],
    '[ or {: 차량 시동2': [{KeyCode(char='{')}, 4.5, 0, 1],
    '] or }: 차량 문 닫기1': [{KeyCode(char=']')}, -4.5, 0, 1],
    '] or }: 차량 문 닫기2': [{KeyCode(char='}')}, -4.5, 0, 1],
    'F9+ F10: GTA 윈도우 세팅': [{Key.f9, Key.f10}, 6, 0, 0],

    'Num0 + Num1: H1': [{KeyCode(vk=96), KeyCode(vk=97)}, 7, 0, 0],
    'Num0 + Num2: H2': [{KeyCode(vk=96), KeyCode(vk=98)}, 7.1, 0, 0],
    'Num0 + Num3: H3': [{KeyCode(vk=96), KeyCode(vk=99)}, 7.2, 0, 0],
    'Backspace + F10: 스타일 복원': [{Key.backspace, Key.f10}, 7.21, 0, 0],
    'Backspace + F9: CEO MC 복장': [{Key.backspace, Key.f9}, 7.22, 0, 0],
    'Backspace + F8: 항공 헬멧': [{Key.backspace, Key.f8}, 7.221, 0, 0],
    '*: 열감지 모드': [{KeyCode(char='*')}, 7.23, 0, 1],
    'Num1 + Num4: motorOrCEO nomal': [{KeyCode(vk=97), KeyCode(vk=100)}, 7.3, 0, 0],
    'Num2 + Num5: motorOrCEO = Motor': [{KeyCode(vk=98), KeyCode(vk=101)}, 7.4, 0, 0],
    'Num3 + Num6: motorOrCEO = CEO': [{KeyCode(vk=99), KeyCode(vk=102)}, 7.5, 0, 0],

    'rShift + F11: 커스텀 무기 활성화 비활성화': [{Key.shift_r, Key.f11}, 8.3, 0, 0],
    'rShift + F10: 작업 숨기기 활성화 비활성화': [{Key.shift_r, Key.f10}, 8.4, 0, 0],
    'Ctrl + Z: 그레네이드 런처 탄구입': [{Key.ctrl_l, KeyCode(char='\x1a')}, 11, 0, 0],
    'Ctrl + C: RPG 탄구입': [{Key.ctrl_l, KeyCode(char='\x03')}, 11.1, 0, 0],
    'Ctrl + X: 호밍런처 탄구입': [{Key.ctrl_l, KeyCode(char='\x18')}, 11.2, 0, 0],
    'Ctrl + B: 점폭 구입': [{Key.ctrl_l, KeyCode(char='\x02')}, 11.3, 0, 0],

    'caps + F1: 스패로우 소환': [{Key.caps_lock, Key.f1}, 12, 0, 0],
    '2 + F1: 스패로우 보관': [{KeyCode(char='2'), Key.f1}, -12, 0, 0],
    'caps + F2: 모싸 바이크 소환 or 보관': [{Key.caps_lock, Key.f2}, 12.2, 0, 0],
    'caps + F3: 버자드 소환': [{Key.caps_lock, Key.f3}, 12.1, 0, 0],
    'Shift + caps + F3: 볼라투스 소환': [{Key.shift_l, Key.caps_lock, Key.f3}, 12.3, 0, 0],
    'caps + F4: 딩이 요청': [{Key.caps_lock, Key.f4}, 12.4, 0, 0],
    '5 + F4: 딩이 보관': [{KeyCode(char='5'), Key.f4}, -12.4, 0, 0],
    'F4: 개인 이동수단 보관': [{Key.f4}, 12.5, 0, 0],
    'caps + F5: 산체스 소환': [{Key.caps_lock, Key.f5}, 12.6, 0, 0],
    'caps + shift + F5: 샤프터 LWB 소환': [{Key.caps_lock, Key.shift_l, Key.f5}, 12.7, 0, 0],

    'Ctrl + F1: 코사트카 요청': [{Key.ctrl_l, Key.f1}, 13, 0, 0],
    'Ctrl + Shift + F1: 코사트카 보관': [{Key.ctrl_l, Key.shift_l, Key.f1}, -13, 0, 0],
    'Ctrl + Alt + F1: 코사트카 재배치': [{Key.alt_r, Key.f1}, 13.05, 0, 0],
    'Ctrl + F2: 테러바이트 요청': [{Key.ctrl_l, Key.f2}, 13.1, 0, 0],
    'Ctrl + Shift + F2: 테러바이트 보관': [{Key.ctrl_l, Key.shift_l, Key.f2}, -13.1, 0, 0],
    'Ctrl + F3: 어벤져 요청': [{Key.ctrl_l, Key.f3}, 13.2, 0, 0],
    'Ctrl + Shift + F3: 어벤져 보관': [{Key.ctrl_l, Key.shift_l, Key.f3}, -13.2, 0, 0],
    'Ctrl + F4: 테바 옵투 요청': [{Key.ctrl_l, Key.f4}, 13.3, 0, 0],
    'Ctrl + F5: 팬텀 웨지 요청': [{Key.ctrl_l, Key.f5}, 13.4, 0, 0],
    'Ctrl + F6: 스피도 커스텀 요청': [{Key.ctrl_l, Key.f6}, 13.5, 0, 0],

    '`: 스낵창': [{KeyCode(char='`')}, 14, 0, 1],
    '~: 스낵창': [{KeyCode(char='~')}, 14, 0, 1],
    'tab: 방탄복': [{Key.cmd}, 14.1, 0, 1],
    '\: 스낵 및 방탄복 확인1': [{KeyCode(char='\\')}, 14.2, 0, 0],
    '\: 스낵 및 방탄복 확인2': [{KeyCode(char='|')}, 14.2, 0, 0],
    'Alt + z: 불샤크 투하1': [{Key.alt_l, KeyCode(char='z')}, 15, 0, 0],
    'Alt + z: 불샤크 투하2': [{Key.alt_l, KeyCode(char='Z')}, 15, 0, 0],
    'Alt + x: 방탄복 투하1': [{Key.alt_l, KeyCode(char='x')}, 16, 0, 0],
    'Alt + x: 방탄복 투하2': [{Key.alt_l, KeyCode(char='X')}, 16, 0, 0],

    'F5: Fingerprint hacking solution': [{Key.f5}, 17, 0, 0],
    # 'F6: 전투장비 열감지 활성화': [{Key.f6}, 19, 0, 0],
    'F10: 스카프 입기': [{Key.f10}, 18.01, 0, 0],
    'F11: paste text': [{Key.f11}, 18, 0, 0],

    # 부계정 원격 조정
    # 'F5 + F8: remote control mode toggling': [{Key.f5, Key.f8}, 20, 0, 0],
    # 'f: remote F': [{KeyCode(char='f')}, 20.01, 0, 0],
    # 'F: remote F': [{KeyCode(char='F')}, 20.01, 0, 0],
    '/ + Num1: remote F': [{KeyCode(char='/'), KeyCode(vk=97)}, 21, 0, 0],
    '/ + Enter: remote Enter': [{KeyCode(char='/'), Key.enter}, 21.01, 0, 0],
    '/ + Up: remote AU': [{KeyCode(char='/'), Key.up}, 21.02, 0, 0],
    '/ + Down: remote AU': [{KeyCode(char='/'), Key.down}, 21.03, 0, 0],
    '/ + Right: remote AR': [{KeyCode(char='/'), Key.right}, 21.04, 0, 0],
    '/ + Left: remote AL': [{KeyCode(char='/'), Key.left}, 21.05, 0, 0],
    '/ + Backspace: remote BS': [{KeyCode(char='/'), Key.backspace}, 21.06, 0, 0],

}


def operator_(order):
    global flag01, flag02, debug01, flag05, flag09, pause_mecro, pause_mecro2, \
        i1, i2, i3, i6, i7, pre_order, H_state, motorOrCEO, remote_mode
    if debug01:
        print("order =", order)
        print("pre_order =", pre_order)
    if len(pre_order) == 2 and pre_order[0] == 2 and order != 2 and is_active:
        pause_mecro = True
        while True:
            if not is_active:
                break
    elif len(pre_order) == 2 and pre_order[0] == 2.01 and order != 2.01 and is_active:
        pause_mecro = True
        while True:
            if not is_active:
                break
    # 해킹시에 반드시 tab을 눌러야 하므로 블로킹을 적용했다.
    elif len(pre_order) == 2 and pre_order == [17, 14.1]:
        print("blocking tab operation")
        return
    elif pre_order == [2, 2]:
        pause_mecro = True
        return
    elif pre_order == [2.01, 2.01]:
        pause_mecro = True
        return
    elif is_active:
        pause_mecro = True
        while True:
            if not is_active:
                break
    if order == 1:
        if pre_order == [2, 1]:
            pause_mecro = True
            return
        elif pre_order == [2.01, 1]:
            pause_mecro = True
            return
        elif not pressKey_released:
            if i6 == 0:
                i6 += 1
                pause_mecro = False
                th = Thread(target=clicking, daemon=True)
                th.start()
        elif pressKey_released:
            pause_mecro = True
    elif order == 1.02:
        if not pressKey_released:
            print("-> remote control : send clicking signal")
            send_data([order, 0])
        elif pressKey_released:
            print("-> remote control : send end clicking signal")
            send_data([order, 1])
    elif order == 1.01:
        getHstate()
    elif order == 1.1:
        print("-> kill GTA process.")
        os.system('taskkill /f /im GTA5.exe')
    elif order == 2:
        pause_mecro = False
        if is_active:
            pause_mecro = True
        else:
            th = Thread(target=sleeping, daemon=True)
            th.start()
    elif order == 2.01:
        pause_mecro = False
        if is_active:
            print("pause_mecro")
            pause_mecro = True
        else:
            th = Thread(target=get_pixel_color, daemon=True)
            th.start()
    elif order == 2.2:  # bb1: 무기 이름 bbox, bb2: 무기 슬롯 number bbox
        imageGrab_scope()
    elif order == 2.3:
        imageGrab_get_pixel_color()
    elif order == 2.4:
        scaning_screen_test()
    elif order == 3:  # F2 : 모싸
        if sensitiveKeyLocking():
            return
        else:
            if H_state == 2 and (motorOrCEO == 1 or motorOrCEO == 2):
                th = Thread(target=disassemble, daemon=True)
                th.start()
            else:
                th = Thread(target=beBoss, args=(7,), daemon=True)
                th.start()
    elif order == 4:  # F3 : CEO
        if H_state == 2 and (motorOrCEO == 1 or motorOrCEO == 2):
            th = Thread(target=disassemble, daemon=True)
            th.start()
        else:
            th = Thread(target=beBoss, args=(6,), daemon=True)
            th.start()
    elif order == 4.5:
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=startCarEngine, daemon=True)
            th.start()
    elif order == -4.5:
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=closeDoors, daemon=True)
            th.start()
    elif order == 6:  # / * : GTA window setting
        th = Thread(target=windowsSetting, daemon=True)
        th.start()
    elif order == 7:
        if sensitiveKeyLocking():
            return
        else:
            H_state = 1
            print("H_state =", H_state)
    elif order == 7.1:
        if sensitiveKeyLocking():
            return
        else:
            H_state = 2
            print("H_state =", H_state)
    elif order == 7.2:
        if sensitiveKeyLocking():
            return
        else:
            H_state = 3
            print("H_state =", H_state)
    elif order == 7.21:
        th = Thread(target=restore_style, daemon=True)
        th.start()
    elif order == 7.22:
        th = Thread(target=ceo_mc_style, daemon=True)
        th.start()
    elif order == 7.221:
        th = Thread(target=air_helmet, daemon=True)
        th.start()
    elif order == 7.23:
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=thermal_mode, daemon=True)
            th.start()
    elif order == 7.3:
        if sensitiveKeyLocking():
            return
        else:
            motorOrCEO = 0
            print("motorOrCEO = ", motorOrCEO, "\nnomal", sep='')
    elif order == 7.4:
        if sensitiveKeyLocking():
            return
        else:
            motorOrCEO = 1
            print("motorOrCEO = ", motorOrCEO, "\nMC", sep='')
    elif order == 7.5:
        if sensitiveKeyLocking():
            return
        else:
            motorOrCEO = 2
            print("motorOrCEO = ", motorOrCEO, "\nCEO", sep='')
    elif order == 8.2:
        th = Thread(target=dropWeapons, daemon=True)
        th.start()
    elif order == 8.3:  # rShift + F11: 커스텀 무기 활성화 비활성화
        th = Thread(target=togglingCustomWeapons, daemon=True)
        th.start()
    elif order == 8.4:
        th = Thread(target=hide_job, daemon=True)
        th.start()
    elif order == 11:  # 그레네이드
        if sensitiveKeyLocking():
            return
        else:
            # 애초에 탭키에 pynput 핫키가 실려있어서 keyboard freezing 이슈가 있었다. ㅅㅂ
            # 만약 이런식으로 핫키가 중복된다면 이 명령을 실행할때 핫키 동작이 일어나지 않도록 차단해야한다.
            th = Thread(target=heavyWeapon, args=(0,), daemon=True)
            th.start()
    elif order == 11.1:  # RPG
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=heavyWeapon, args=(1,), daemon=True)
            th.start()
    elif order == 11.2:  # 호밍런처
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=heavyWeapon, args=(-1,), daemon=True)
            th.start()
    elif order == 11.3:  # 점폭
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=get_sticky_bomb, daemon=True)
            th.start()
    elif order == 12:
        th = Thread(target=call_Sparrow, daemon=True)
        th.start()
    elif order == -12:
        th = Thread(target=saveSparrow, daemon=True)
        th.start()
    elif order == 12.1:
        th = Thread(target=spawnBuzzard, args=(motorOrCEO, False), daemon=True)
        th.start()
    elif order == 12.2:
        th = Thread(target=spawnOrSaveMotorcycle, args=(motorOrCEO,), daemon=True)
        th.start()
    elif order == 12.3:
        th = Thread(target=spawnBuzzard, args=(motorOrCEO, True), daemon=True)
        th.start()
    elif order == 12.4:
        th = Thread(target=call_Dinghy, daemon=True)
        th.start()
    elif order == -12.4:
        th = Thread(target=save_Dinghy, daemon=True)
        th.start()
    elif order == 12.5:
        th = Thread(target=store_vehicle, daemon=True)
        th.start()
    elif order == 12.6:
        th = Thread(target=call_Sanchez, daemon=True)
        th.start()
    elif order == 12.7:
        th = Thread(target=call_SchafterLWB, daemon=True)
        th.start()
    elif order == 13:
        th = Thread(target=call_Kosatka, daemon=True)
        th.start()
    elif order == -13:
        th = Thread(target=store_Kosatka, daemon=True)
        th.start()
    elif order == 13.05:
        th = Thread(target=relocation_Kosatka, daemon=True)
        th.start()
    elif order == 13.1:
        th = Thread(target=call_Terrorbyte, daemon=True)
        th.start()
    elif order == -13.1:
        th = Thread(target=store_Terrorbyte, daemon=True)
        th.start()
    elif order == 13.2:
        th = Thread(target=call_Avenger, daemon=True)
        th.start()
    elif order == -13.2:
        th = Thread(target=store_Avenger, daemon=True)
        th.start()
    elif order == 13.3:
        th = Thread(target=call_Oppressor, daemon=True)
        th.start()
    elif order == 13.4:
        th = Thread(target=call_Phantom_Wedge, daemon=True)
        th.start()
    elif order == 13.5:
        th = Thread(target=call_speedo_custom, daemon=True)
        th.start()
    elif order == 14:
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=snack, daemon=True)
            th.start()
    elif order == 14.1:
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=bulletproof_jacket, daemon=True)
            th.start()
    elif order == 14.2:
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=check_inventory, daemon=True)
            th.start()
    elif order == 15:
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=bull_shark, args=(motorOrCEO,), daemon=True)
            th.start()
    elif order == 16:
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=drop_bull_jacket, args=(motorOrCEO,), daemon=True)
            th.start()
    elif order == 17:
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=fpSolution, daemon=True)
            th.start()
    elif order == 18:
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=paste_text, daemon=True)
            th.start()
    elif order == 18.01:
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=wearing_scarf, daemon=True)
            th.start()
    elif order == 19:
        if sensitiveKeyLocking():
            return
        else:
            th = Thread(target=activate_thermal_mode, daemon=True)
            th.start()
    elif order == 20:
        remote_mode = not remote_mode
        if remote_mode:
            print("-> remote mode : On")
        else:
            print("-> remote mode : Off")
    elif 20 < order < 21:
        if remote_mode:
            remote_mode_control(order)
        else:
            return
    elif order == 21:
        print("-> remote control : F press")
        send_data([1, 0])
    elif 21 < order < 21.1:
        remote_21(order)
    elif order == 22:
        if remote_mode:
            if not pressKey_released_1:
                pause_mecro = False
                th = Thread(target=press_key_test, daemon=True)
                th.start()
            elif pressKey_released_1:
                pause_mecro = True
        else:
            return
    elif order == 23:
        if remote_mode:
            if not pressKey_released_2:
                pause_mecro2 = False
                th = Thread(target=press_key_test2, daemon=True)
                th.start()
            elif pressKey_released_2:
                pause_mecro2 = True
        else:
            return
    return


def enter_pressed():
    global chat_mode
    if chat_mode:
        chat_mode = False
        print("chat_mode =", chat_mode)
        return
    else:
        return


def close_chatting():
    global chat_mode
    if chat_mode:
        chat_mode = False
        print("chat_mode =", chat_mode)
        return
    else:
        return


def check_chatting():
    global chat_mode
    if chat_mode:
        return
    else:
        sleep(0.3)
        chat_mode = chat_pixel_check()
        print("chat_mode =", chat_mode)
        return


def combat_mode():
    global is_active
    is_active = True
    click('press')
    while True:
        if pause_mecro:
            break
        sleep(0.020)
    click('release')
    is_active = False
    print("-> combat mode on")
    return


def call_speedo_custom():
    global is_active
    is_active = True
    is_active = arduino001.call_speedo_custom(H_state)
    return


def wearing_scarf():
    global is_active
    is_active = True
    is_active = arduino001.wearing_scarf(H_state)
    return


def air_helmet():
    global is_active
    is_active = True
    is_active = arduino001.air_helmet(H_state)
    return


def remote_mode_control(order):
    log = str()
    if order == 20.01:
        log = 'remote control : F'
    send_data([order, log])
    return


def press_key_test():
    global i6
    print("-> press key")
    # keyPress('w')
    print("keyPress('w')")
    while True:
        sleep(0.02)
        if pause_mecro:
            print("-> release key")
            # keyRelease('w')
            print("keyRelease('w')")
            i6 = 0
            return
        else:
            continue


def press_key_test2():
    global i7
    print("-> press key")
    # keyPress('d')
    print("keyPress('d')")
    while True:
        sleep(0.02)
        if pause_mecro2:
            print("-> release key")
            # keyRelease('d')
            print("keyRelease('d')")
            i7 = 0
            return
        else:
            continue


def remote_21(order):
    if order == 21.01:
        print("-> remote control : Enter")
    elif order == 21.02:
        print("-> remote control : Arrow Up")
    elif order == 21.03:
        print("-> remote control : Arrow Down")
    elif order == 21.04:
        print("-> remote control : Arrow Right")
    elif order == 21.05:
        print("-> remote control : Arrow Left")
    elif order == 21.06:
        print("-> remote control : Backspace")
    send_data([order, 0])
    return


def send_data(L):
    b_data = pickle.dumps(L)
    try:
        client_socket.send(b_data)
    except OSError:
        print("서버와 연결되지 않았기 때문에 명령을 실행할 수 없습니다. 서버와 연결해 주세요.")
        return


def receiver():
    global client_socket
    try:
        while True:
            b_data = client_socket.recv(1024)
            respL = pickle.loads(b_data)
            operator_for_receiver(respL)
            if disconnect:
                print("disconnect server connection.")
                client_socket.close()
                return
    except ConnectionResetError:
        print("server is closed.")
        client_socket.close()
        return -1


def operator_for_receiver(L):
    if L[0] == 1 and L[1] == 1:
        pass
    elif L[0] == 2 and L[1] == 1:
        print(L[2])
    elif L[0] == 3 and L[1] == 1:
        print(L[2])


def init_client():
    global client_socket
    host_ip = '110.15.1.193'
    port = 12345
    addr = (host_ip, port)
    client_socket.connect(addr)
    print("server connected")
    send_data([3, 0, 0])


def get_sticky_bomb():
    global is_active
    is_active = True
    is_active = arduino001.get_sticky_bomb(H_state)
    return


def call_SchafterLWB():
    global is_active
    is_active = True
    is_active = arduino001.call_SchafterLWB(H_state, motorOrCEO)
    return


def call_Sanchez():
    global is_active
    is_active = True
    is_active = arduino001.call_Sanchez(H_state, motorOrCEO)
    return


def relocation_Kosatka():
    global is_active
    is_active = True
    is_active = arduino001.relocation_Kosatka(H_state)
    return


def ready_teleport():
    global is_active
    is_active = True
    is_active = arduino001.ready_teleport()
    return


def paste_text():
    global is_active, chat_mode
    is_active = True
    is_active = arduino001.paste_text()
    return


def activate_thermal_mode():
    global is_active
    is_active = True
    is_active = arduino001.activate_thermal_mode(H_state)
    return


def thermal_mode():
    global is_active
    is_active = True
    is_active = arduino001.thermal_mode(H_state)
    return


def call_Phantom_Wedge():
    global is_active
    is_active = True
    is_active = arduino001.call_Phantom_Wedge(H_state)
    return


def ceo_mc_style():
    global is_active, H_state, motorOrCEO
    is_active = True
    is_active, H_state, motorOrCEO = arduino001.ceo_mc_style(H_state, motorOrCEO)
    return


def store_vehicle():
    global is_active
    is_active = True
    is_active = arduino001.store_vehicle(H_state)
    return


def save_Dinghy():
    global is_active
    is_active = True
    is_active = arduino001.save_dinghy(H_state)
    return


def call_Dinghy():
    global is_active
    is_active = True
    is_active = arduino001.call_dinghy(H_state)
    return


def restore_style():
    global is_active
    data = pixel_dict['target']
    bb = bbox_dict['target']
    is_active = True
    keyPress('m')
    sleep(0.072)
    keyRelease('m')
    sleep(0.22)
    result = mss_scan(data, bb)
    if result == -1:
        print("Nothing matches.")
    else:
        print("result =", result)
    down = result + 2
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 3):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    for i in range(0, 2):
        keyPress('AL')
        sleep(delay)
        keyRelease('AL')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('m')
    sleep(delay)
    keyRelease('m')
    sleep(delay)
    print("-> Restoring style")
    is_active = False
    return


def fpSolution():
    global is_active
    is_active = True
    print("-> Fingerprint hacking solution started.")
    arduino000.fingerPrint_hacking()
    is_active = False
    return


def mss_scan(data, bbox):
    sct_img = mss.mss().grab(bbox)  # type: ignore
    for key, item in data.items():
        pixel_list = list()
        cp = item['cp']
        pc = item['pc']
        if debug_pix:
            print("key =", key)
            print("cp =", cp)
            print("pc =", pc)
        for point in cp:
            pix = sct_img.pixel(point[0], point[1])
            pixel_list.append(pix)
        if debug_pix:
            print("PC =", pixel_list)
        if pc == pixel_list:
            return key
        else:
            continue
    return -1


def call_Oppressor():
    global is_active
    is_active = True
    keyPress('m')
    sleep(0.072)
    keyRelease('m')
    sleep(delay)
    down = H_state - 1 + 5
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 2):
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 2):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('m')
    sleep(delay)
    keyRelease('m')
    sleep(delay)
    is_active = False
    return


def hide_job():
    global is_active
    is_active = True
    keyPress('m')
    sleep(0.07)
    keyRelease('m')
    sleep(delay)
    for i in range(0, 8):
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(delay)
    for i in range(0, 2):
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
    keyPress('AR')
    sleep(delay)
    keyRelease('AR')
    sleep(delay)
    keyPress('m')
    sleep(delay)
    keyRelease('m')
    sleep(delay)
    is_active = False
    return


def dropWeapons():
    global is_active
    is_active = True
    bb = bbox_dict['weapon']
    data = pixel_dict['weapon']
    result = scaning_screen_ver2(data['pistol'], bb)
    print("result =", result)
    is_active = False
    return


def drop_bull_jacket(case):
    global is_active, H_state, motorOrCEO
    is_active = True
    bb = bbox_dict['mission']
    data = pixel_dict['mission']
    num = 6
    if case == 0:  # 평민인 경우
        if num == 7:
            print("-> be the MC")
        else:
            print("-> be the CEO")
        keyPress('m')
        sleep(0.072)
        keyRelease('m')
        sleep(delay)
        for i in range(0, num):
            keyPress('AD')
            sleep(delay)
            keyRelease('AD')
            sleep(delay)
        for i in range(0, 2):
            keyPress('ET')
            sleep(delay)
            keyRelease('ET')
            sleep(delay)
        if num == 7:
            motorOrCEO = 1
        elif num == 6:
            motorOrCEO = 2
        H_state = 2
        print("H_state =", H_state)
        if motorOrCEO == 0:
            print("nomal state")
        elif motorOrCEO == 1:
            print("MC")
        elif motorOrCEO == 2:
            print("CEO")
        else:
            print("motorOrCEO =", motorOrCEO)
        sleep(0.2)
    elif case == 1:  # 모싸인 경우
        print("-> disassemble")
        keyPress('m')
        sleep(0.072)
        keyRelease('m')
        sleep(delay)
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(0.2)
        res = mss_scan(data, bb)
        if res == 'check':
            print("진행중인 임무가 있으므로 방탄복을 투하할 수 없습니다.")
            is_active = False
            return
        else:
            print('not match')
            keyPress('ET')
            sleep(delay)
            keyRelease('ET')
            sleep(delay)
            if num == 7:
                print("-> be the MC")
            elif num == 6:
                print("-> be the CEO")
            sleep(0.08)
            keyPress('m')
            sleep(0.072)
            keyRelease('m')
            sleep(delay)
            for i in range(0, num):
                keyPress('AD')
                sleep(delay)
                keyRelease('AD')
                sleep(delay)
            for i in range(0, 2):
                keyPress('ET')
                sleep(delay)
                keyRelease('ET')
                sleep(delay)
            if num == 7:
                motorOrCEO = 1
            elif num == 6:
                motorOrCEO = 2
            H_state = 2
            print("H_state =", H_state)
            if motorOrCEO == 0:
                print("nomal state")
            elif motorOrCEO == 1:
                print("MC")
            elif motorOrCEO == 2:
                print("CEO")
            else:
                print("motorOrCEO =", motorOrCEO)
            sleep(0.2)
    elif case == 2:
        pass  # CEO인 경우 pass
    sleep(delay)
    keyPress('m')
    sleep(0.072)
    keyRelease('m')
    sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 3):
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 3):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    print("-> Bulletproof Jacket droped")
    is_active = False
    return


def bull_shark(case):
    global is_active, H_state, motorOrCEO
    is_active = True
    bb = bbox_dict['mission']
    data = pixel_dict['mission']
    num = 6
    if case == 0:  # 평민인 경우
        if num == 7:
            print("-> be the MC")
        else:
            print("-> be the CEO")
        keyPress('m')
        sleep(0.072)
        keyRelease('m')
        sleep(delay)
        for i in range(0, num):
            keyPress('AD')
            sleep(delay)
            keyRelease('AD')
            sleep(delay)
        for i in range(0, 2):
            keyPress('ET')
            sleep(delay)
            keyRelease('ET')
            sleep(delay)
        if num == 7:
            motorOrCEO = 1
        elif num == 6:
            motorOrCEO = 2
        H_state = 2
        print("H_state =", H_state)
        if motorOrCEO == 0:
            print("nomal state")
        elif motorOrCEO == 1:
            print("MC")
        elif motorOrCEO == 2:
            print("CEO")
        else:
            print("motorOrCEO =", motorOrCEO)
        sleep(0.2)
    elif case == 1:  # 모싸인 경우
        print("-> disassemble")
        keyPress('m')
        sleep(0.072)
        keyRelease('m')
        sleep(delay)
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(0.2)
        res = mss_scan(data, bb)
        if res == 'check':
            print("진행중인 임무가 있으므로 불샤크를 투하할 수 없습니다.")
            is_active = False
            return
        else:
            print('not match')
            keyPress('ET')
            sleep(delay)
            keyRelease('ET')
            sleep(delay)
            if num == 7:
                print("-> be the MC")
            elif num == 6:
                print("-> be the CEO")
            sleep(0.08)
            keyPress('m')
            sleep(0.072)
            keyRelease('m')
            sleep(delay)
            for i in range(0, num):
                keyPress('AD')
                sleep(delay)
                keyRelease('AD')
                sleep(delay)
            for i in range(0, 2):
                keyPress('ET')
                sleep(delay)
                keyRelease('ET')
                sleep(delay)
            if num == 7:
                motorOrCEO = 1
            elif num == 6:
                motorOrCEO = 2
            H_state = 2
            print("H_state =", H_state)
            if motorOrCEO == 0:
                print("nomal state")
            elif motorOrCEO == 1:
                print("MC")
            elif motorOrCEO == 2:
                print("CEO")
            else:
                print("motorOrCEO =", motorOrCEO)
            sleep(0.2)
    elif case == 2:
        pass  # CEO인 경우 pass
    sleep(delay)
    keyPress('m')
    sleep(0.072)
    keyRelease('m')
    sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 3):
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('AD')
    sleep(delay)
    keyRelease('AD')
    sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    print("-> bull shark droped")
    is_active = False
    return


def bulletproof_jacket():
    global is_active
    data = pixel_dict['snack']
    bb = bbox_dict['snack']
    data2 = pixel_dict['inter_menu_check']
    bb2 = bbox_dict['inter_menu_check']
    is_active = True
    result = mss_scan(data, bb)
    if result == 'check':  # 스낵창 띄운 상태에서 방탄복 단축키 눌렀을 경우.
        keyPress('BS')
        sleep(delay)
        keyRelease('BS')
        sleep(delay)
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(delay)
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
        for i in range(0, 3):
            keyPress('AU')
            sleep(delay)
            keyRelease('AU')
            sleep(delay)
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
        keyPress('m')
        sleep(delay)
        keyRelease('m')
        sleep(delay)
        print("-> bulletproof jacket")
        is_active = False
        return
    elif result == -1:
        keyPress('m')
        sleep(0.070)
        keyRelease('m')
        sleep(delay)
        down = H_state - 1 + 2
        for i in range(0, down):
            keyPress('AD')
            sleep(delay)
            keyRelease('AD')
            sleep(delay)
        result2 = mss_scan(data2, bb2)
        if result2 == 'check':
            keyPress('ET')
            sleep(delay)
            keyRelease('ET')
            sleep(delay)
            for i in range(0, 3):
                keyPress('AD')
                sleep(delay)
                keyRelease('AD')
                sleep(delay)
            keyPress('ET')
            sleep(delay)
            keyRelease('ET')
            sleep(delay)
            for i in range(0, 3):
                keyPress('AU')
                sleep(delay)
                keyRelease('AU')
                sleep(delay)
            keyPress('ET')
            sleep(delay)
            keyRelease('ET')
            sleep(delay)
            keyPress('m')
            sleep(delay)
            keyRelease('m')
            sleep(delay)
            print("-> bulletproof jacket")
            is_active = False
            return
        else:
            print("-> 상호작용 메뉴가 아니므로 작동 중지")
            is_active = False
            return


def check_inventory():
    global is_active
    is_active = True
    data = pixel_dict['snack']
    bb = bbox_dict['snack']
    data2 = pixel_dict['target']
    bb2 = bbox_dict['target']
    data3 = pixel_dict['inter_menu_check']
    bb3 = bbox_dict['inter_menu_check']
    is_active = arduino001.check_inventory(data, bb, data2, bb2, data3, bb3)
    return


def snack():
    global is_active
    data = pixel_dict['snack']
    bb = bbox_dict['snack']
    is_active = True
    result = mss_scan(data, bb)
    if result == 'check':
        keyPress('m')
        sleep(delay)
        keyRelease('m')
        sleep(delay)
        print("-> close snack menu")
        is_active = False
        return
    elif result == -1:
        keyPress('m')
        sleep(0.070)
        keyRelease('m')
        sleep(delay)
        down = H_state - 1 + 2
        for i in range(0, down):
            keyPress('AD')
            sleep(delay)
            keyRelease('AD')
            sleep(delay)
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
        for i in range(0, 4):
            keyPress('AD')
            sleep(delay)
            keyRelease('AD')
            sleep(delay)
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
        print("-> open snack menu")
        is_active = False
        return
    else:
        print("result =", result)
        is_active = False
        return


def call_Kosatka():
    global is_active
    is_active = True
    keyPress('m')
    sleep(0.070)
    keyRelease('m')
    sleep(delay)
    down = H_state - 1 + 5
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('AU')
    sleep(delay)
    keyRelease('AU')
    sleep(delay)
    for i in range(0, 2):
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
    print("-> call Kosatka")
    is_active = False
    return


def store_Kosatka():
    global is_active
    is_active = True
    keyPress('m')
    sleep(0.070)
    keyRelease('m')
    sleep(delay)
    down = H_state - 1 + 5
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('AU')
    sleep(delay)
    keyRelease('AU')
    sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 3):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    for i in range(0, 2):
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
    print("-> store Kosatka")
    is_active = False
    return


def call_Terrorbyte():
    global is_active
    is_active = True
    keyPress('m')
    sleep(0.072)
    keyRelease('m')
    sleep(delay)
    down = H_state - 1 + 5
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 2):
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(delay)
    for i in range(0, 2):
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
    print("-> call Terrorbyte")
    is_active = False
    return


def store_Terrorbyte():
    global is_active
    is_active = True
    keyPress('m')
    sleep(0.070)
    keyRelease('m')
    sleep(delay)
    down = H_state - 1 + 5
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 2):
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('AD')
    sleep(delay)
    keyRelease('AD')
    sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    print("-> store Terrorbyte")
    is_active = False
    return


def call_Avenger():
    global is_active
    is_active = True
    keyPress('m')
    sleep(0.070)
    keyRelease('m')
    sleep(delay)
    down = H_state - 1 + 5
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 2):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    for i in range(0, 2):
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
    print("-> call Avenger")
    is_active = False
    return


def store_Avenger():
    global is_active
    is_active = True
    keyPress('m')
    sleep(0.070)
    keyRelease('m')
    sleep(delay)
    down = H_state - 1 + 5
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 2):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('AD')
    sleep(delay)
    keyRelease('AD')
    sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    print("-> store Avenger")
    is_active = False
    return


def closeDoors():
    global is_active
    data2 = pixel_dict['inter_menu_check']
    bb2 = bbox_dict['inter_menu_check']
    is_active = True
    keyPress('m')
    sleep(0.072)
    keyRelease('m')
    sleep(delay)
    down = H_state - 1 + 4
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
        if i == 1:
            result2 = mss_scan(data2, bb2)
            if result2 == 'check':
                pass
            else:
                print("-> 상호작용 메뉴가 아니므로 작동 중지")
                is_active = False
                return
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 7):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('m')
    sleep(delay)
    keyRelease('m')
    sleep(delay)
    print("-> close car doors")
    is_active = False
    return


def sensitiveKeyLocking():
    current_window = win32gui.GetWindowText(win32gui.GetForegroundWindow())
    if current_window == gta_window_name:
        return False
    else:
        return True


# 그냥 노멀한 경우, 공세 CEO인 경우, 공세 MC인 경우, 신습 CEO, 모싸의 경우
# 페습 지원조직, 튜습 지원조직
# CEO의 직원으로 참여한 경우, MC의 직원으로 참여한 경우.
def getHstate():
    # H_state = 1: nomal, 2: CEO or MC, 3: support og
    # motorOrCEO = 0: nomal, 1: MC, 2: CEO
    global debug_pix, H_state, motorOrCEO
    # debug_pix = True
    bbox = bbox_dict['interactive menu']
    data = pixel_dict['get H_state']
    sleep(0.2)
    result = mss_scan(data, bbox)
    if result == -1:
        print("does not match any data set")
    else:
        if result == 'nomal':
            print("nomal detected")
            H_state = 1
            motorOrCEO = 0
        elif result == 'motor':
            print("MC detected")
            H_state = 2
            motorOrCEO = 1
        elif result == 'CEO':
            print("CEO detected")
            H_state = 2
            motorOrCEO = 2
        elif result == 'support':
            H_state = 3
            print("Support Organization detected")
        elif result == 'support2':
            H_state = 3
            print("Support Organization detected")
        elif result == 'employee':
            print("Employee detected")
            H_state = 2
        elif result == 'newcomer':
            print("Newcomer detected")
            H_state = 2
            motorOrCEO = 1
        else:
            print(result)
    print("H_state =", H_state)
    if motorOrCEO == 0:
        print("nomal state")
    elif motorOrCEO == 1:
        print("MC")
    elif motorOrCEO == 2:
        print("CEO")
    else:
        print("motorOrCEO =", motorOrCEO)
    return


def startCarEngine():
    global is_active
    data2 = pixel_dict['inter_menu_check']
    bb2 = bbox_dict['inter_menu_check']
    is_active = True
    keyPress('m')
    sleep(0.072)
    keyRelease('m')
    sleep(delay)
    down = H_state - 1 + 4
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
        if i == 1:
            result2 = mss_scan(data2, bb2)
            if result2 == 'check':
                pass
            else:
                print("-> 상호작용 메뉴가 아니므로 작동 중지")
                is_active = False
                return
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 7):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('AR')
    sleep(delay)
    keyRelease('AR')
    sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 4):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('AR')
    sleep(0.222)
    keyRelease('AR')
    sleep(delay)
    keyPress('m')
    sleep(delay)
    keyRelease('m')
    sleep(delay)
    print("-> start car engine")
    is_active = False
    return


def spawnOrSaveMotorcycle(case):
    global is_active, H_state, motorOrCEO
    bb = bbox_dict['mission']
    data = pixel_dict['mission']
    is_active = True
    num = 7
    # case = motorOrCEO
    if case == 0:  # 평민인 경우
        if num == 7:
            print("-> be the MC")
        else:
            print("-> be the CEO")
        keyPress('m')
        sleep(0.080)
        keyRelease('m')
        sleep(delay)
        for i in range(0, num):
            keyPress('AD')
            sleep(delay)
            keyRelease('AD')
            sleep(delay)
        for i in range(0, 2):
            keyPress('ET')
            sleep(delay)
            keyRelease('ET')
            sleep(delay)
        if num == 7:
            motorOrCEO = 1
        elif num == 6:
            motorOrCEO = 2
        H_state = 2
        print("H_state =", H_state)
        if motorOrCEO == 0:
            print("nomal state")
        elif motorOrCEO == 1:
            print("MC")
        elif motorOrCEO == 2:
            print("CEO")
        else:
            print("motorOrCEO =", motorOrCEO)
        sleep(0.3)
    elif case == 1:  # 모싸일 경우
        pass
    elif case == 2:  # CEO일 경우
        print("-> disassemble")
        keyPress('m')
        sleep(0.07)
        keyRelease('m')
        sleep(delay)
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(delay)
        res = mss_scan(data, bb)
        if res == 'check':
            print("진행중인 임무가 있으므로 CEO를 해체할 수 없습니다.")
            is_active = False
            return
        else:
            keyPress('ET')
            sleep(delay)
            keyRelease('ET')
            sleep(0.082)
            if num == 7:
                print("-> be the MC")
            elif num == 6:
                print("-> be the CEO")
            keyPress('m')
            sleep(0.070)
            keyRelease('m')
            sleep(delay)
            for i in range(0, num):
                keyPress('AD')
                sleep(delay)
                keyRelease('AD')
                sleep(delay)
            for i in range(0, 2):
                keyPress('ET')
                sleep(delay)
                keyRelease('ET')
                sleep(delay)
            if num == 7:
                motorOrCEO = 1
            elif num == 6:
                motorOrCEO = 2
            H_state = 2
            print("H_state =", H_state)
            if motorOrCEO == 0:
                print("nomal state")
            elif motorOrCEO == 1:
                print("MC")
            elif motorOrCEO == 2:
                print("CEO")
            else:
                print("motorOrCEO =", motorOrCEO)
            sleep(0.3)
    keyPress('m')
    sleep(0.070)
    keyRelease('m')
    sleep(delay)
    for i in range(0, 2):
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
    print("-> save or spawn Motorcycle")
    is_active = False
    return


def spawnBuzzard(case, volatus):
    global is_active, H_state, motorOrCEO
    bb = bbox_dict['mission']
    data = pixel_dict['mission']
    is_active = True
    num = 6
    if case == 0:  # 평민인 경우
        if num == 7:
            print("-> be the MC")
        else:
            print("-> be the CEO")
        keyPress('m')
        sleep(0.080)
        keyRelease('m')
        sleep(delay)
        for i in range(0, num):
            keyPress('AD')
            sleep(delay)
            keyRelease('AD')
            sleep(delay)
        for i in range(0, 2):
            keyPress('ET')
            sleep(delay)
            keyRelease('ET')
            sleep(delay)
        if num == 7:
            motorOrCEO = 1
        elif num == 6:
            motorOrCEO = 2
        H_state = 2
        print("H_state =", H_state)
        if motorOrCEO == 0:
            print("nomal state")
        elif motorOrCEO == 1:
            print("MC")
        elif motorOrCEO == 2:
            print("CEO")
        else:
            print("motorOrCEO =", motorOrCEO)
        sleep(0.2)
    elif case == 1:  # 모싸인 경우
        print("-> disassemble")
        keyPress('m')
        sleep(0.07)
        keyRelease('m')
        sleep(delay)
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(delay)
        res = mss_scan(data, bb)
        if res == 'check':
            print("진행중인 임무가 있으므로 버자드를 호출할 수 없습니다.")
            is_active = False
            return
        else:
            print('not match')
            keyPress('ET')
            sleep(delay)
            keyRelease('ET')
            sleep(delay)
            if num == 7:
                print("-> be the MC")
            elif num == 6:
                print("-> be the CEO")
            sleep(0.08)
            keyPress('m')
            sleep(delay)
            keyRelease('m')
            sleep(delay)
            for i in range(0, num):
                keyPress('AD')
                sleep(delay)
                keyRelease('AD')
                sleep(delay)
            for i in range(0, 2):
                keyPress('ET')
                sleep(delay)
                keyRelease('ET')
                sleep(delay)
            if num == 7:
                motorOrCEO = 1
            elif num == 6:
                motorOrCEO = 2
            H_state = 2
            print("H_state =", H_state)
            if motorOrCEO == 0:
                print("nomal state")
            elif motorOrCEO == 1:
                print("MC")
            elif motorOrCEO == 2:
                print("CEO")
            else:
                print("motorOrCEO =", motorOrCEO)
            sleep(0.2)
    elif case == 2:
        pass  # CEO인 경우 그냥 바로 소환하면 된다.
    sleep(delay)
    keyPress('m')
    sleep(0.07)
    keyRelease('m')
    sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 2):
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    num = 4
    if volatus:
        num += 5
    for i in range(0, num):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    if volatus:
        print("-> Volatus is called.")
    else:
        print("-> Buzzard is called.")
    is_active = False
    return


def saveSparrow():
    global is_active
    is_active = True
    keyPress('m')
    sleep(0.07)
    keyRelease('m')
    sleep(delay)
    down = H_state - 1 + 5
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('AU')
    sleep(delay)
    keyRelease('AU')
    sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    for i in range(0, 3):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    for i in range(0, 2):
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    print("-> sparrow saved")
    is_active = False
    return


def call_Sparrow():
    global is_active
    is_active = True
    keyPress('m')
    sleep(0.072)
    keyRelease('m')
    sleep(delay)
    down = H_state - 1 + 5
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('AU')
    sleep(delay)
    keyRelease('AU')
    sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('AD')
    sleep(delay)
    keyRelease('AD')
    sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    print("-> call sparrow")
    is_active = False
    return


def heavyWeapon(num):
    global is_active
    is_active = True
    keyPress('m')
    sleep(0.072)
    keyRelease('m')
    sleep(delay)
    down = H_state + 1
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 5):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('AD')
    sleep(delay)
    keyRelease('AD')
    sleep(delay)
    if num < 0:
        num = -num
        for i in range(0, num):
            keyPress('AL')
            sleep(delay)
            keyRelease('AL')
            sleep(delay)
    else:
        for i in range(0, num):
            keyPress('AR')
            sleep(delay)
            keyRelease('AR')
            sleep(delay)
    for i in range(0, 2):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('m')
    sleep(delay)
    keyRelease('m')
    sleep(delay)
    if num == 0:
        print("-> Grenade Launcher Ammo Refilled")
    elif num == 1:
        print("-> Horming Launcher Ammo Refilled")
    is_active = False


def chat_pixel_check():
    bbox = bbox_dict['chat']
    data = pixel_dict['chat']
    check_list = list()
    img = mss.mss().grab(bbox)  # type: ignore
    for key, item in data.items():
        pixel_list = list()
        cp = item['cp']
        if debug_pix:
            print("cp =", cp)
        for point in cp:
            pix = img.pixel(point[0], point[1])
            pixel_list.append(pix)
        if debug_pix:
            print("pc =", pixel_list)
        for pixels in pixel_list:
            for pix in pixels:
                if 152 <= pix <= 155:
                    check_list.append(True)
                else:
                    check_list.append(False)
        if all(check_list):
            return True
        else:
            return False
    return False


def get_pixel_color():  # l + .
    global is_active, i4
    is_active = True
    print("-> get pixel color")
    while True:
        x, y = pa.position()
        point = (x, y, x+1, y+1)
        one_pix = mss.mss().grab(point)  # type: ignore
        pix_color = one_pix.pixel(0, 0)
        output_str = str(pix_color)
        if pause_mecro:
            print("")
            print("-> end get pixel color")
            is_active = False
            return
        sleep(delay)
        print('\r', output_str, end='', sep='')


def sleeping():  # F11 + Backspace
    global is_active, H_state, motorOrCEO
    is_active = True
    if H_state > 1 or motorOrCEO > 0:
        print("-> disassemble")
        keyPress('m')
        sleep(0.07)
        keyRelease('m')
        sleep(delay)
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
        keyPress('AU')
        sleep(delay)
        keyRelease('AU')
        sleep(delay)
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
        H_state = 1
        motorOrCEO = 0
        print("H_state =", H_state)
        if motorOrCEO == 0:
            print("nomal state")
        elif motorOrCEO == 1:
            print("MC")
        elif motorOrCEO == 2:
            print("CEO")
        else:
            print("motorOrCEO =", motorOrCEO)
        sleep(0.3)
    print("-> sleeping")
    while True:
        print("sleeping...")
        move(200, 0)
        sleep(0.5)
        move(-200, 0)
        sleep(0.5)
        for i in range(0, 300):
            sleep(1)
            if pause_mecro:
                print("-> wake up")
                is_active = False
                return


def togglingCustomWeapons():
    print("toggling customized weapons.")
    keyPress('m')
    sleep(0.07)
    keyRelease('m')
    sleep(delay)
    down = H_state + 1
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    for i in range(0, 6):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('m')
    sleep(delay)
    keyRelease('m')
    sleep(delay)
    return


def disassemble():  # Ctrl + F2
    global H_state, is_active, motorOrCEO
    is_active = True
    print("-> disassemble")
    keyPress('m')
    sleep(0.07)
    keyRelease('m')
    sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    keyPress('AU')
    sleep(delay)
    keyRelease('AU')
    sleep(delay)
    keyPress('ET')
    sleep(delay)
    keyRelease('ET')
    sleep(delay)
    H_state = 1
    motorOrCEO = 0
    print("H_state =", H_state)
    if motorOrCEO == 0:
        print("nomal state")
    elif motorOrCEO == 1:
        print("MC")
    elif motorOrCEO == 2:
        print("CEO")
    else:
        print("motorOrCEO =", motorOrCEO)
    is_active = False
    return


# 테바나 어벤져, 시설, 오피스 내에서도 CEO 및 모싸 전환이 자유로워야한다.
# 이거는 무조건 이미지 스캐닝으로 해결하는게 맞다.
# 시설, 격납고, 벙커, 오피스, 사업장, 스패, 반반, 클럽하우스, 레코드 스튜디오, LS 자동차 모임
# 일반 아파트, 이클립스 아파트
# 코사트카, 테바, 어벤져, 요트
# 아케이드, 나이트클럽, 사무소, 튜닝샵,
# 특이한곳 : 이클립스 아파트, 오피스
def beBoss(num):  # F2
    global H_state, motorOrCEO, is_active, debug_pix
    data = pixel_dict['inside_check']
    bb = bbox_dict['inside_check']
    is_active = True
    if num == 7:
        print("-> be the MC")
    else:
        print("-> be the CEO")
    down = num
    keyPress('m')
    sleep(0.072)
    keyRelease('m')
    sleep(0.22)
    result = mss_scan(data, bb)
    if result == 'nomal':
        print("inside monitor = nomal")
        pass
    elif result == 'inside':
        print("inside monitor = inside")
        down = num + 1
    elif result == 'luxury':
        print("inside monitor = Luxury apartment")
        down = num + 2
    elif result == 'office':
        print("inside monitor = Office")
        down = num + 2
    elif result == -1:
        print("result =", result)
    else:
        print("result =", result)
    for i in range(0, down):
        keyPress('AD')
        sleep(delay)
        keyRelease('AD')
        sleep(delay)
    for i in range(0, 2):
        keyPress('ET')
        sleep(delay)
        keyRelease('ET')
        sleep(delay)
    if num == 7:
        motorOrCEO = 1
    elif num == 6:
        motorOrCEO = 2
    H_state = 2
    print("H_state =", H_state)
    if motorOrCEO == 0:
        print("nomal state")
    elif motorOrCEO == 1:
        print("MC")
    elif motorOrCEO == 2:
        print("CEO")
    else:
        print("motorOrCEO =", motorOrCEO)
    is_active = False
    return


def dropWeapon_rifle():
    global flag09
    i, j = 0, 0
    while i < 7:
        res = False
        type(res)
        i += 1
        if flag09:  # pause 키로 매크로를 중간에 중단시킬 수 있다.
            flag09 = False
            return
        sleep(0.05)
        keyPress('2')
        sleep(0.04)
        # res = scaning_screen(pix_list_rifle, bb1)
        sleep(0.04)
        keyRelease('2')
        if flag09:
            flag09 = False
            return
        sleep(0.062)
        if res:
            keyPress('-')
            sleep(0.32)
            keyRelease('-')
            if flag09:
                flag09 = False
                return
            sleep(0.32)
            print("drop")
        else:
            if flag09:
                flag09 = False
                return
            print("not")
            continue


def scaning_screen(plist, bb):
    screen = ImageGrab.grab(bbox=bb)
    for inner in plist:
        pixel_list = list()
        cp = inner[0]
        pc = inner[1]
        if debug_pix:
            print("cp =", cp)
            print("pc =", pc)
        for point in cp:
            pix = screen.getpixel(point)
            pixel_list.append(pix)
        if debug_pix:
            print("PC =", pixel_list)
        if pc == pixel_list:
            return False  # 일치하는 경우
        else:
            continue
    return True  # 불일치


def scaning_screen_ver2(data, bbox):
    screen = ImageGrab.grab(bbox=bbox)
    for key, item in data.items():
        pixel_list = list()
        cp = item['cp']
        pc = item['pc']
        if debug_pix:
            print("key =", key)
            print("cp =", cp)
            print("pc =", pc)
        for point in cp:
            pix = screen.getpixel(point)
            pixel_list.append(pix)
        if debug_pix:
            print("PC =", pixel_list)
        if pc == pixel_list:
            return key
        else:
            continue
    return -1


def scaning_screen_full_screen(data):
    screen = ImageGrab.grab()
    for key, item in data.items():
        pixel_list = list()
        cp = item['cp']
        pc = item['pc']
        if debug_pix:
            print("key =", key)
            print("cp =", cp)
            print("pc =", pc)
        for point in cp:
            pix = screen.getpixel(point)
            pixel_list.append(pix)
        if debug_pix:
            print("PC =", pixel_list)
        if pc == pixel_list:
            return key
        else:
            continue
    return -1


def scaning_screen_save_test(data, bbox):
    global i4
    screen = ImageGrab.grab(bbox=bbox)
    for key, item in data.items():
        pixel_list = list()
        cp = item['cp']
        pc = item['pc']
        if debug_pix:
            print("key =", key)
            print("cp =", cp)
            print("pc =", pc)
        for point in cp:
            pix = screen.getpixel(point)
            pixel_list.append(pix)
        if debug_pix:
            print("PC =", pixel_list)
        if pc == pixel_list:
            save_img(screen)
            return key
        else:
            continue
    save_img(screen)
    return -1


def save_img(screen):  #
    with open('F:\GTA5\MecroProject\Mecro\i4.dat', 'rb') as file_read:
        i4 = pickle.load(file_read)
    print("file name number =", i4)
    str1 = str(i4)
    str2 = str1.zfill(4)
    file_name = 'test' + str2
    i4 += 1
    screen.save('F:\GTA5\MecroProject\Mecro\img\{}.png'.format(file_name))
    print("image saved")
    with open('F:\GTA5\MecroProject\Mecro\i4.dat', 'wb') as file_write:
        pickle.dump(i4, file_write)


# 이미지 스캐닝
def scaning_screen_test():  # 이미지 스캔 값 중복 여부 테스트 " ; + ' "
    global debug_pix
    debug_pix = True
    data = pixel_dict['snack']
    bb = bbox_dict['snack']
    start = time()
    result = mss_scan(data, bb)
    if result == 'check':
        print("All checked")
    elif result == -1:
        print("no match")
    else:
        print("result =", result)
    res_time = time() - start
    print("time =", res_time)


def imageGrab_scope():  # scope 스샷 찍어내기 'l + ;'
    global i4
    bb = bbox_dict['interactive menu']
    print("grab image in scope =", bb)
    with open('F:\GTA5\MecroProject\Mecro\i4.dat', 'rb') as file_read:
        i4 = pickle.load(file_read)
    print("file name number =", i4)
    str1 = str(i4)
    str2 = str1.zfill(4)
    file_name = 'test' + str2
    i4 += 1
    screen = ImageGrab.grab(bbox=bb)
    screen.save('F:\GTA5\MecroProject\Mecro\img\{}.png'.format(file_name))
    print("image saved")
    with open('F:\GTA5\MecroProject\Mecro\i4.dat', 'wb') as file_write:
        pickle.dump(i4, file_write)


def imageGrab_get_pixel_color():  # 정해진 scope 내에서 입력한 픽셀값의 컬러값 추출 "l + ' "
    print("get pixel color in scope")
    bb = bbox_dict['interactive menu']
    cp = pixel_dict['get H_state']['CEO']['cp']
    screen = ImageGrab.grab(bbox=bb)
    pc = list()
    for point in cp:
        pixel_color = screen.getpixel(point)
        pc.append(pixel_color)
    print("'cp': ", cp, ",", sep='')
    print("'pc': ", pc, ",", sep='')


def windowsSetting():  # '/' + '*'
    global is_active
    is_active = True
    data = pixel_dict['window setting check']
    bb = bbox_dict['window setting check']
    sleep(1)
    moveTo(1726, 182, 0)  # 최초에는 커서가 home 위치(0,0)으로 반드시 이동해야 하므로 세번째 인자에
    #                      0을 전달한다.
    sleep(1.5)
    click('press')
    sleep(0.06)
    moveTo(967, 141, 1)  # 이후에는 커서가 home으로 다시 갈 필요가 없으므로 세번째 인자에 1을 전달하여
    #                     home으로 가지 않고 바로 다음 위치로 이동한다.
    sleep(1.0)
    click('release')
    print("GTA window moved")
    sleep(0.3)
    key = mss_scan(data, bb)
    if key == 'check':
        print("The location of the window is correct.")
    else:
        print("The location of the window is not correct.")
        is_active = False
        return
    is_active = False
    return


def clicking():
    global i6
    print("-> clicking start")
    while True:
        click('press')
        sleep(0.020)
        click('release')
        sleep(0.020)
        if pause_mecro:
            print("-> clicking end")
            i6 = 0
            return
        else:
            continue


def keyPress(sub2):  # 키보드 눌림 신호
    sub1 = 'KD'
    trans = sub1 + sub2
    transSerial(trans)


def keyRelease(sub2):  # 키보드 릴리즈 신호
    sub1 = 'KU'
    trans = sub1 + sub2
    transSerial(trans)


def transSerial(str1):
    global ser
    ser.write(str1.encode())


def click(command, button='l'):
    trans = str()
    if button == 'r':
        if command == 'click':
            trans = 'RC'
        elif command == 'press':
            trans = 'RH'
        elif command == 'release':
            trans = 'RR'
    elif button == 'l':
        if command == 'click':
            trans = 'LC'
        elif command == 'press':
            trans = 'LH'
        elif command == 'release':
            trans = 'LR'
    elif button == 'm':
        pass
    transSerial(trans)


def move(x, y):
    str1 = 'MX'
    xstr, ystr = str(x).zfill(4), str(y).zfill(4)
    str1 = str1 + xstr + ystr
    transSerial(str1)


def moveTo(x, y, i):
    xstr, ystr = str(x).zfill(4), str(y).zfill(4)
    if i == 0:
        start = 'MZ'
    else:
        start = 'Mz'
    str1 = start + xstr + ystr
    transSerial(str1)


def on_press(key):
    global keySet, pre_order, i1, pressKey_released, pressKey_released_1, pressKey_released_2
    keySet.add(key)
    pressKey_released, pressKey_released_1, pressKey_released_2 = False, False, False
    if i1 == 0:
        i1 += 1
        if debug01:
            print("press key =", key)
            print("keySet =", keySet)
        for action, trigger in hotkey.items():
            check = all([True if triggerKey in keySet else False for triggerKey in trigger[0]])
            if debug01 and check and trigger[2] == 0:  # debug
                print("action =", action)
                print("press key =", key)
                print("keySet =", keySet)
                print("trigger =", trigger[0])
                print([True if triggerKey in keySet else False for triggerKey in trigger[0]])
                lineGenarator('-', 60)
            elif check and trigger[2] == 1:
                if debug01:
                    print("action =", action)
                    print("press key =", key)
                    print("keySet =", keySet)
                    print("trigger =", trigger[0])
                    print([True if triggerKey in keySet else False for triggerKey in trigger[0]])
                    lineGenarator('-', 60)
                order = trigger[1]
                pre_order.append(order)
                if len(pre_order) >= 3:
                    pre_order = pre_order[1:]
                operator_(order)


def on_release(key):
    global pre_order, flag03, pressKey_released, pressKey_released_1, pressKey_released_2, keySet, i1
    if debug01:
        print("release keySet =", keySet)
    for action, trigger in hotkey.items():
        check = all([True if triggerKey in keySet else False for triggerKey in trigger[0]])
        if check:
            if check and trigger[1] == -1 and (len(keySet) == len(trigger[0])):
                keySet.clear()
                return False
            elif check and trigger[1] == -2 and (len(keySet) == len(trigger[0])):
                flag03 = True
                keySet.clear()
                return False
            elif check and trigger[2] == 0 and trigger[3] == 1:
                order = trigger[1]
                pre_order.append(order)
                if len(pre_order) >= 3:
                    pre_order = pre_order[1:]
                operator_(order)
            elif check and (len(keySet) == len(trigger[0])) and trigger[2] == 0:
                order = trigger[1]
                pre_order.append(order)
                if len(pre_order) >= 3:
                    pre_order = pre_order[1:]
                operator_(order)
            elif check and (len(keySet) == len(trigger[0])) and trigger[2:] == [1, 0]:
                pressKey_released = True
                order = trigger[1]
                operator_(order)
            elif check and trigger[2:] == [1, 1]:
                order = trigger[1]
                if order == 22:
                    pressKey_released_1 = True
                elif order == 23:
                    pressKey_released_2 = True
                else:
                    pressKey_released = True
                operator_(order)
            if debug01:
                print("release key =", key)
    i1 = 0
    keySet.clear()


def keyboardListener():  # 키보드 입력을 감지한다. 별도의 스레드 위에서 작동한다.
    global listener
    with Listener(on_press=on_press, on_release=on_release) as listener:
        print("Keyboard listener started.")
        listener.join()


def lineGenarator(mark, num):
    str2 = str()
    for i in range(0, num):
        str2 += mark
    print(str2)


def commander():
    global flag03, debug01, debug_pix, i4, ser, disconnect
    i = 0
    while True:
        if flag03:
            ser.close()
            sys.stdout.flush()
            exit(0)
        else:
            if i == 0:
                command = 'start'  # 최초에는 키보드 리스너를 start 한다.
                i += 1
            else:  # 이스케이프 핫키(Esc)를 눌러서 command 모드로 진입한다.
                command = input(">>> ").lower()
            if command == 'start':
                keyboardListener()
            elif command == 'hotkey':
                for key in hotkey.keys():
                    print(key)
            elif command == 'end':  # 프로그램을 종료한다.
                ser.close()
                sys.stdout.flush()
                exit(0)
            elif command == 'debug 0' or command == 'debug 1':  # 디버그 모드
                # 0: 디버그 모드 Off, 1: 디버그 모드 On
                int1 = int(command.strip()[-1])
                if int1 == 0:
                    debug01 = False
                elif int1 == 1:
                    debug01 = True
                keyboardListener()
            elif command == 'debug':  # 디버그 모드 토글링
                debug01 = not debug01
                keyboardListener()
            elif command == 'debug pix':  # 픽셀 피커 디버그 모드 토글링
                debug_pix = not debug_pix
                Arduino001.debug_pix = debug_pix
                Arduino000.debug_pix = debug_pix
                keyboardListener()
            elif command == 'img':  # grab image number 지정
                while True:
                    try:
                        with open('F:\GTA5\MecroProject\Mecro\i4.dat', 'rb') as file_read:
                            i4 = pickle.load(file_read)
                        print("현재 i4 =", i4)
                        cmd = input("숫자 입력 >>> :")
                        if cmd == 'cancel':
                            break
                        else:
                            i4 = int(cmd)
                            with open('F:\GTA5\MecroProject\Mecro\i4.dat', 'wb') as file_write:
                                pickle.dump(i4, file_write)
                            print("img file name number =", i4)
                            break
                    except ValueError:
                        print("숫자를 입력하세요.")
                keyboardListener()
            elif command == 'server':
                init_client()
                receiver_thread = Thread(target=receiver, daemon=True)
                receiver_thread.start()
                keyboardListener()
            elif command == 'disconnect':
                disconnect = True
                send_data([4, 0])
                sleep(0.2)
                keyboardListener()
            else:
                print("올바른 명령어를 입력하세요.")


if __name__ == "__main__":
    commander()
