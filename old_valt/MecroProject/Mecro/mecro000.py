import threading
from pynput.keyboard import Listener, Key, KeyCode
from time import sleep
from threading import Thread
import pyautogui as pa
from serial import Serial, SerialException
from PIL import ImageGrab


"""
cmd command :
F:\GTA5\MecroProject\Mecro\mecro000.py
-> 명령 프롬프트(cmd)에서는 제대로 동작을 안함. python shell 64bit에서는 매우 잘 동작됨.
python shell command :
exec(open("F:\\GTA5\\MecroProject\\Mecro\\mecro000.py", 'rt', encoding='UTF8').read())
아두이노 하드리셋 : 빈 스케치 스크립트 준비, 업로드 후 0.5초 뒤에 리셋 버튼 클릭
"""

# 전역변수 영역


def getSerial():
    j = 3
    while True:
        try:
            port = 'COM' + str(j)
            ser = Serial(port, 9600)
            print("{} 포트에 연결 성공".format(port))
            return ser
        except SerialException:
            j += 1


ser = getSerial()
listener = object()
th1 = object()
flag01 = False  # clicking 토글을 위한 flag
flag02 = False  # cursor location 토글 flag
flag03 = False  # end program
debug01 = True  # debug 모드 flag
flag05 = False  # sleeping 용 플래그
flag06 = False  # locker
i1, i2, i3 = 0, 0, 0  # 스레드 객체의 최초 실행, 오직 한 번만 실행을 위해 사용되는 변수이다.
keySet = set()  # pynput Keyboard Listener가 감지하는 키를 저장하는 집합이다.
pre_order = list()  # 이전 명령 number를 저장하기 위한 리스트다. 크기는 2를 유지한다.
H_state = 1  # GTA의 상호작용 메뉴의 상태를 나타낸다.
hotkey = {  # 핫키 지정. 자세한 구현방법은 아래 코멘트 참조.
    # 특수키는 Key.ctrl_l로 쓰고, 그외 키는 KeyCode(vk=29) 로 쓴다.
    # 만약에 이래도 안맞으면 keySet과 밑에 trigger[0] 키값 비교하면서 적절히 해주면 됨.
    # 입력값인 keySet에서 '<Key.ctrl_l: <162>>' 이런식으로 나오면 'Key.ctrl_l' 요걸 써주고,
    # '<3>' 요런식으로 나오면 'KeyCode(vk=3)' 이렇게 써주면 된다.
    # 입력 KeySet에서 '*', 'f' 이렇게 나오면 그냥 'KeyCode(char='*')' 이런식으로 써주면 된다.
    # pause 키 처럼 컨트롤을 같이 누르면 아예 키코드가 달라지는 경우도 있으니 특수키 사용시에는 주의하자.
    # pause 키의 경우 그냥 누르려면 'Key.pause', 컨트롤과 같이 누르는 경우는 'KeyCode(vk=3)'을 사용해야한다.
    # Ctrl + F의 경우 그냥 F만 누르면 'KeyCode(char='f')'로 하면 되지만 Ctrl과 같이 누르는 경우에는 키코드가
    # 아예 달라진다. 이 경우에는 입력되는 KeySet을 보고 'KeyCode(char='\x06')'과 같이 실제 입력값과
    # 똑같이 맞춰줘야 한다. 여기에서 '\x06'은 헥사코드로 두 자리씩 끊어서 총 여섯자리의 16진수로 값을
    # 나타나게 된다.
    # 입력값인 KeySet에 맞게 'trigger Set(trigger[0])'을 정확하게 구현해야 제대로 작동한다. **
    # 그리고 참고로 pynput 1.6.8 버전 이후로는 이상하게도 KeyCode(vk=66)같이 버츄얼 키코드로 나타내는 키는
    # 제대로 작동하지 않으므로 해당 버그가 해결되기 전까지는 1.6.8버전으로 사용해야 한다.
    # 두번째 인자는 order_number이다.
    # 오른쪽 컨트롤키의 경우 영어 키셋에서는 Ctrl_r로, 한글 키셋에서는 키코드 <25>으로 작동한다.
    'Ctrl + Esc: exit program': [{Key.ctrl_l, Key.esc}, -2],  # end p1rogram
    ' ` + Esc: exit to command mode': [{Key.esc, KeyCode(char='`')}, -1],  # interruption or escape, Esc + '`'
    'pause: 무한 광클': [{Key.pause}, 1],  # pause
    'F11 + Backspace: 잠수': [{Key.backspace, Key.f11}, 2],  # F11 + Backspace
    '* -: pixel picker test': [{KeyCode(char='*'), KeyCode(char='-')}, 2.1],
    'F2: 모터사이클 두목': [{Key.f2}, 3],
    'F3: CEO': [{Key.f3}, 4],
    'Ctrl + F2: 모싸 및 CEO 해제': [{Key.ctrl_l, Key.f2}, 3.1],
    '[ + ]: get cursor location': [{KeyCode(char='['), KeyCode(char=']')}, 5],  # '['+']'
    '/ + *: GTA 윈도우 세팅': [{KeyCode(char='/'), KeyCode(char='*')}, 6],
    'Num0 + Num1: H1': [{KeyCode(vk=96), KeyCode(vk=97)}, 7],
    'Num0 + Num2: H2': [{KeyCode(vk=96), KeyCode(vk=98)}, 7.1],
    'Num0 + Num3: H3': [{KeyCode(vk=96), KeyCode(vk=99)}, 7.2],
    'Ctrl + F11: 무기 드롭': [{Key.f11}, 8.2],
}


def serial_begin():
    global ser
    j = 3
    while True:
        try:
            port = 'COM' + str(j)
            ser = Serial(port, 9600)
            print("{} 포트에 연결 성공".format(port))
            break
        except SerialException:
            j += 1


# 실시간 커서 위치 찍어주는 프로그램
def get_realtime_cursor_location():
    global flag02
    sleep(0.5)
    while flag02:
        x, y = pa.position()
        positionStr = 'X: ' + str(x).rjust(4) + ' Y: ' + str(y).rjust(4)
        if flag02:
            print('\r', positionStr, end='', sep='')
    print("")
    return


def operator_(order):
    global flag01, flag02, debug01, debug01, flag05, flag06, i1, i2, i3, pre_order, H_state, listener, ser

    th2 = Th2()  # clicking 루핑 스레드
    th3 = Th3()  # cursor location 루핑 스레드
    th2.daemon = True
    th3.daemon = True
    th4 = Thread(target=sleeping, daemon=True)
    th5 = Thread(target=dropWeapons, daemon=True)
    if pre_order != [5, 1] and pre_order != [5, 5]:
        # 커서 찍는 프로그램이 콘솔 카운터 방식이라 아래의 코드들을
        # 프린트 할때 뭔가 이상하다. 그러므로 [5, 1] 명령이 들어오면
        # 콘솔 print를 출력하지 않게 했다.
        if debug01:
            print("pre_order =", pre_order)
            print("order =", order)
            lineGenarator('=', 60)

    #   operator를 실행하기 전에 매크로의 시작과 종료를 감지하여 매크로가 종료되지 않았다면
    # 기존 매크로와 충돌하지 않게끔 하는 장치가 필요하다.
    # 지속매크로가 지속될때 다른 매크로를 실행하면 지속 매크로를 종료하고 다른
    # 매크로를 실행시킨다.
    if th2 is None:  # th2 스레드 객체가 생성되지 않으면 이 if문을 pass로 빠져나가게 한다.
        pass
    elif th3 is None:
        pass
    elif th4 is None:
        pass
    # 지속작업 중에 다른 명령값이 떨어지면 지속작업을 멈추고 명령값을 실행시킨다.
    elif (flag01 or th2.is_alive()) and order != 1:
        flag01 = not flag01
    #   그러니까 5번이 실행된 상태에서 1번을 누르면 pre_order = [5, 1]이 되고 order = 1이고,
    # flag02 = True고 th3이 살아있으므로 아래의 코드를 실행하여 flag02를 False로 바꾼다.
    # 하지만 다시 그 아래의 if order == 1 부분이 또 실행되어 flag02를 다시 True로 바꾼다.
    # 그래서 멈추지 않는 것이다. => 해결됨.
    elif (flag02 or th3.is_alive()) and order != 5:
        if pre_order == [5, 1]:  # 예외적으로 pre_order가 [5, 1]인 상태, 즉 정상적인 과정이면
            # 플래그를 뒤집지 않고 그냥 pass하게 하였다.
            pass
        else:
            flag02 = not flag02
    elif (flag05 or th4.is_alive()) and order != 2:
        if pre_order == [2, 1]:
            pass
        else:
            flag05 = not flag05

    # 여기서부터 order를 수행한다.
    if order == 1:
        if pre_order == [5, 1]:
            flag02 = not flag02
        elif pre_order == [2, 1]:
            flag05 = not flag05

        elif pre_order[0] == 1 or pre_order[1] == 1:
            flag01 = not flag01  # pause키를 누를때마다 flag를 뒤집는다.
            if i1 == 0:  # 이하 코드는 최초에만 실행된다.
                th2.start()  # 최초로 생성된 스레드 객체가 최초로 start한다.
                i1 += 1
            if flag01 and not th2.is_alive():  # flag01가 다시 pause키에 의해 True로 뒤집히고,
                # 최초로 생성된 스레드가 죽었을때 아래 코드를 실행한다.
                del th2  # 스레드가 종료되면 더이상 이 스레드는 재사용이 불가능하므로 삭제한다.
                th2 = Th2()  # 앞서 만든 threading.Thread를 상속한 클래스가 새로운 스레드 객체를
                # 새로운 변수 th2에 생성한다. 그러면 th2는 새로운 스레드 객체가 된다.
                th2.daemon = True  # deamon 스레드
                th2.start()  # 새로운 스레드 객체이므로 start가 가능하다.
    elif order == 2:  # 잠수 기능 우 Ctrl + F11
        flag05 = not flag05
        if i3 == 0:
            th4.start()
            i3 += 1
        if flag05 and not th4.is_alive():
            del th4
            th4 = Thread(target=sleeping, daemon=True)
            th4.start()
    elif order == 2.1:  # * - : 픽셀 픽커 테스트
        grabImage()
    elif order == 3:  # F2 모싸
        motorCycle()
    elif order == 3.1:  # Ctrl + F2 모싸 및 CEO 해제
        disassemble()
    elif order == 4:  # F3
        beCEO()
    elif order == 5:
        flag02 = not flag02
        if i2 == 0:
            th3.start()
            i2 += 1
        if flag02 and not th3.is_alive():
            del th3
            th3 = Th3()
            th3.daemon = True
            th3.start()
    elif order == 6:  # windows setting GTA 초기 창모드 설정. 추후 픽셀 픽커 등을 이용하기 위해
        # 정확한 위치로 GTA 창을 옮길 필요가 있다. 또한 GTA 이외에도 화세 프로그램이나 크롬 사이징 조절
        # 등 여러가지 최초 창을 여기저기로 분배하는 작업을 자동으로 수행하게끔 한다.
        # 여기부터는 실제 GTA에서 사용하는 매크로를 작성한다.
        # 일단은 시리얼 통신 원활하게 되는지 테스트.
        print("windows setting operated")
        windowsSetting()
    elif order == 7:
        H_state = 1
        print("H_state =", H_state)
    elif order == 7.1:
        H_state = 2
        print("H_state =", H_state)
    elif order == 7.2:
        H_state = 3
        print("H_state =", H_state)
    elif order == 8.2:
        # listener.stop()
        # if listener.is_alive():
        #     print("listener killed")
        # del listener
        th5.start()  # 결국에는 스레딩이 문제였다. 리스너가 작동하는 상태에서 제어권을 점유해버리면 키 입력에
        # 프리징이 걸리게 된다. 제어권을 점유하는 스레드를 아예 따로 만들어서 하니 매우 잘 작동한다.
        # 메인 제어권을 침범하지 않도록 별도로 스레딩하여 메인 제어권에 영향을 가지 않게끔 하는 방식이다.
        # 씨발 이거면 그냥 서버 그딴거는 그냥 개 생 뻘짓이었다. 그냥 스레드 제어권 문제였다. 씨이발. 개 좆같네 ㅋㅋ
        # 그리고 어차피 스레드 객체는 재사용할 수 있다. 


def test000():
    while True:
        cmd = input("input>>> ")
        if cmd == 'end':
            return
        else:
            dropWeapons()


def dropWeapons2():
    global flag06, ser
    str1 = 'c010000'
    ser.write(str1.encode())
    sleep(3.275)
    flag06 = True


def dropWeapons():  # Ctrl + F11 : 무기 버림
    global ser
    keyPress('-')
    sleep(0.064)
    keyRelease('-')
    sleep(0.5)
    # 피스톨 버림
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('-')
    sleep(0.064)
    keyRelease('-')
    sleep(0.5)
    # 컴뱃피스톨 버림
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('-')
    sleep(0.064)
    keyRelease('-')
    sleep(0.5)
    # 헤비 피스톨 버림
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('-')
    sleep(0.064)
    keyRelease('-')
    sleep(0.5)
    # 빈티지 피스톨 버림
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('1')
    sleep(0.064)
    keyRelease('1')
    sleep(0.064)
    keyPress('-')
    sleep(0.064)
    keyRelease('-')
    sleep(0.5)
    # 헤비 리볼버 버림
    # ser.close()


checkPoint = [[941, 524], [945, 522], [945, 526], [945, 541]]
result1 = [(254, 254, 254), (255, 255, 255), (255, 255, 255), (101, 101, 101)]


def grabImage():  # * -
    print("image picker test")
    screen = ImageGrab.grab()
    checkPoint = [[941, 524], [945, 522], [945, 526], [945, 541]]
    pixel_list = list()
    for point in checkPoint:
        po = pa.position(point[0], point[1])
        pix = screen.getpixel(po)
        pixel_list.append(pix)
    del screen
    # print(pixel_list)
    result1 = [(254, 254, 254), (255, 255, 255), (255, 255, 255), (101, 101, 101)]
    if pixel_list == result1:
        print("all checked")
    else:
        print("is not")


def disassemble():  # Ctrl + F2
    print("클럽 및 시큐어 에이전시 해체")
    keyPress('m')
    keyRelease('m')
    sleep(0.032)
    keyPress('ET')
    sleep(0.032)
    keyRelease('ET')
    sleep(0.032)
    keyPress('AU')
    sleep(0.032)
    keyRelease('AU')
    sleep(0.032)
    keyPress('ET')
    sleep(0.032)
    keyRelease('ET')
    sleep(0.032)
    H_state = 1
    print("H_state =", H_state)


def beCEO():
    global H_state
    print("-> be CEO")
    keyPress('m')
    keyRelease('m')
    sleep(0.032)
    for i in range(0, 6):
        keyPress('AD')
        sleep(0.032)
        keyRelease('AD')
        sleep(0.032)
    for i in range(0, 2):
        keyPress('ET')
        sleep(0.032)
        keyRelease('ET')
        sleep(0.032)
    H_state = 2
    print("H_state =", H_state)


def motorCycle():  # F2
    global H_state
    print("-> be motorcyle boss")
    keyPress('m')
    sleep(0.080)
    keyRelease('m')
    sleep(0.032)
    for i in range(0, 7):
        keyPress('AD')
        sleep(0.032)
        keyRelease('AD')
        sleep(0.032)
    for i in range(0, 2):
        keyPress('ET')
        sleep(0.032)
        keyRelease('ET')
        sleep(0.032)
    H_state = 2
    print("H_state =", H_state)


def testArduino():  # num1
    print('-> CapsLock order operated')
    for i in range(0, 3):
        keyPress('m')
        keyRelease('m')
        sleep(0.050)
    keyPress('ET')
    sleep(0.050)
    keyRelease('ET')
    sleep(0.050)
    keyPress('m')
    sleep(0.050)
    keyRelease('m')
    sleep(0.050)


def keyPress(sub2):  # 키보드 눌림 신호
    sub1 = 'KD'
    trans = sub1 + sub2
    transSerial(trans)


def keyRelease(sub2):  # 키보드 릴리즈 신호
    sub1 = 'KU'
    trans = sub1 + sub2
    transSerial(trans)


def sleeping():  # F11 + Backspace
    while True:
        print("sleeping...")
        move(200, 0)
        sleep(0.5)
        move(-200, 0)
        sleep(0.5)
        for i in range(0, 300):
            sleep(1)
            if not flag05:
                print("wake up")
                return


def move(x, y):
    str1 = 'MX'
    xstr, ystr = str(x).zfill(4), str(y).zfill(4)
    str1 = str1 + xstr + ystr
    transSerial(str1)


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


def windowsSetting():  # '/' + '*'
    moveTo(1726, 182, 0)  # 최초에는 커서가 home 위치(0,0)으로 반드시 이동해야 하므로 세번째 인자에
    #                      0을 전달한다.
    sleep(1.5)
    click('press')
    sleep(0.06)
    moveTo(967, 141, 1)  # 이후에는 커서가 home으로 다시 갈 필요가 없으므로 세번째 인자에 1을 전달하여
    #                     home으로 가지 않고 바로 다음 위치로 이동한다.
    sleep(1.0)
    click('release')
    # 여기까지 테스트 완료.


def transSerial(str1):
    ser.write(str1.encode())


def transNrespSerial(str1):
    ser.write(str1.encode())
    if ser.readable():
        resp = ser.readline()
        result = resp[:-1].decode()
        print(result)


def moveTo(x, y, i):
    xstr, ystr = str(x).zfill(4), str(y).zfill(4)
    if i == 0:
        start = 'MZ'
    else:
        start = 'Mz'
    str1 = start + xstr + ystr
    transSerial(str1)


def clicking():
    global flag01
    while flag01:  # 전역변수 flag01에 따라 스레드 객체 내에서 동작한다.
        # 만약 스레드 작동중 pause키에 의해 다시 operator가 호출되면 flag가 뒤집혀
        # 작동을 중지하고
        click('press')
        sleep(0.020)
        click('release')
        sleep(0.020)
    return  # while문을 탈출하여 return을 하므로써 스레드가 종료된다.


class Th2(threading.Thread):  # 스레드 클래스 상속
    def __init__(self):
        super().__init__()

    def run(self):  # 스레드 클래스의 run 메소드 오버라이드
        clicking()


class Th3(threading.Thread):
    def __init__(self):
        super().__init__()

    def run(self):
        get_realtime_cursor_location()


def lineGenarator(mark, num):
    str2 = str()
    for i in range(0, num):
        str2 += mark
    print(str2)


def on_press(key):
    global keySet, pre_order
    keySet.add(key)
    if debug01:
        print("press key =", key)
        print("keySet =", keySet)
    for action, trigger in hotkey.items():
        check = all([True if triggerKey in keySet else False for triggerKey in trigger[0]])
        if debug01 and check:  # 입력 감지값인 keySet과 핫키의 trigger의 동일 여부를 분석한다.
            print("action =", action)
            print("press key =", key)
            print("keySet =", keySet)
            print("trigger =", trigger[0])
            print([True if triggerKey in keySet else False for triggerKey in trigger[0]])
            lineGenarator('-', 60)


def on_release(key):
    global pre_order, flag03, keySet
    for action, trigger in hotkey.items():
        check = all([True if triggerKey in keySet else False for triggerKey in trigger[0]])
        if check:
            #   핫키 trigger[0] 집합에 있는 triggerKey(원소들)가 입력 감지값인 keySet에 모두 있으면 True를
            # 반환하는 함수이다. 만약 두 개의 키가 필요한 키라면 릴리즈 할때 릴리즈 부분이 두번 실행되지만
            # 마지막 줄에 keySey이 최초 먼저 띄어진 키에서 clear되기 때문에 두번째 실행에서는 check가
            # False가 되어 operator_를 실행하지 않는다.
            # 프로그램을 command 상태로 돌리는 이스케이프 핫키이다. '`' + Esc
            if check and trigger[1] == -1 and (len(keySet) == len(trigger[0])):
                keySet.clear()
                return False  # 리스너에 False를 리턴하여 리스너를 종료한다. Ctrl + Esc
            elif check and trigger[1] == -2 and (len(keySet) == len(trigger[0])):
                flag03 = True
                keySet.clear()
                return False
            elif check and (len(keySet) == len(trigger[0])):
                print("release key =", key)
                #   지정된 핫키와 일치하는 action의 order number를 operator_에 전달하여
                # 필요한 작업을 실행한다.
                # 릴리즈 하는 키가 두 개여도 최초 릴리즈에서 keySet이 clear되므로 이 부분은 한번만 실행된다.
                order = trigger[1]
                pre_order.append(order)  # 이전 order number를 저장한다.
                if len(pre_order) >= 3:
                    pre_order = pre_order[1:]
                # print("pre_order =", pre_order)
                operator_(order)  # operator_ 함수가 할당된 order를 실행한다.
    keySet.clear()  # 키가 릴리즈 되면 keySet을 clear


def keyboardListner():  # 키보드 입력을 감지한다. 별도의 스레드 위에서 작동한다.
    global listener, flag06
    flag06 = False
    with Listener(on_press=on_press, on_release=on_release) as listener:
        listener.join()


def commander():
    global flag03, debug01
    i = 0
    while True:
        if flag03:
            exit(0)
        else:
            if i == 0 or flag06:
                command = 'start'  # 최초에는 키보드 리스너를 start 한다.
                i += 1
            else:  # 이스케이프 핫키(Esc)를 눌러서 command 모드로 진입한다.
                command = input(">>> ")
            if command == 'start':
                main000()
            elif command == 'end':  # 프로그램을 종료한다.
                exit(0)
            elif command == 'debug 0' or command == 'debug 1':  # 디버그 모드
                # 0: 디버그 모드 Off, 1: 디버그 모드 On
                int1 = int(command.strip()[-1])
                if int1 == 0:
                    debug01 = False
                elif int1 == 1:
                    debug01 = True
                main000()
            elif command == 'debug':  # 디버그 모드 토글링 -> 핫키 모니터링은 꺼짐.
                debug01 = not debug01
                main000()
            else:
                print("올바른 명령어를 입력하세요.")


def main000():
    global th1
    th1 = Thread(target=keyboardListner, daemon=True)  # 키보드 리스너 스레드
    print("Keyboard Listener started")
    th1.start()
    th1.join()
    if not th1.is_alive():
        print("th1 is killed")


if __name__ == "__main__":
    commander()
