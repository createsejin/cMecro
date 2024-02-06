import threading
from pynput.keyboard import Listener, Key, KeyCode
from time import sleep
from threading import Thread
import pyautogui as pa

# 전역변수 영역
flag01 = False  # clicking 토글을 위한 flag
flag02 = False  # cursor location 토글 flag
th2 = object()  # 스레드 객체를 전역변수로 활용하기 위한 빈 객체를 전역변수로 할당한다.
i1 = 0          # th2 스레드 객체의 최초 실행, 오직 한 번만 실행을 위해 사용되는 변수이다.
keySet = set()  # pynput Keyboard Listener가 감지하는 키를 저장하는 집합이다.
pre_order = list()
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
    'exit': [{Key.ctrl_l, Key.esc}, -1],  # interruption or escape, Ctrl + Esc
    'clicking': [{Key.pause}, 1],  # pause
    'clicking_break': [{Key.ctrl_l, KeyCode(vk=3)}, 2],  # Ctrl + pause
    'testKey': [{Key.ctrl_l, KeyCode(char='\x06')}, 3],  # Ctrl + F
    'testKey2': [{KeyCode(vk=97), KeyCode(char='*')}, 4],  # '* + Num1'
    'get cursor location': [{KeyCode(char='['), KeyCode(char=']')}, 5]
}


# 실시간 커서 위치 찍어주는 프로그램
def get_realtime_cursor_location():
    global flag02
    while flag02:
        x, y = pa.position()
        positionStr = 'X: ' + str(x).rjust(4) + ' Y: ' + str(y).rjust(4)
        print('\r', positionStr, end='', sep='')
    return


def operator_(order):
    global flag01, th2, i1, pre_order
    print("pre_order =", pre_order)
    print("order =", order)
    if order == 1:
        flag01 = not flag01  # pause키를 누를때마다 flag를 뒤집는다.
        if i1 == 0:  # 이하 코드는 최초에만 실행된다.
            th2.start()  # 최초로 생성된 스레드 객체가 최초로 start한다.
            i1 += 1
        if flag01 and not th2.is_alive():  # flag01가 다시 pause키에 의해 True로 뒤집히고,
            # 최초로 생성된 스레드가 죽었을때 아래 코드를 실행한다.
            th2.join()  # 얘는 이전의 스레드가 확실하게 종료할때까지 기다려주는 함수다.
            del th2  # 스레드가 종료되면 더이상 이 스레드는 재사용이 불가능하므로 삭제한다.
            th2 = Th2()  # 앞서 만든 threading.Thread를 상속한 클래스가 새로운 스레드 객체를
            # 새로운 변수 th2에 생성한다. 그러면 th2는 새로운 스레드 객체가 된다.
            th2.daemon = True  # deamon 스레드
            th2.start()  # 새로운 스레드 객체이므로 start가 가능하다.
    elif order == 2:
        print('break clicking')
    elif order == 3:
        print('test key')
    elif order == 4:
        print('test key 2')
    elif order == 5:
        get_realtime_cursor_location()


def checkFlag01():
    global flag01
    return flag01


def clicking():
    global flag01
    i = 0
    while flag01:  # 전역변수 flag01에 따라 스레드 객체 내에서 동작한다.
        # 만약 스레드 작동중 pause키에 의해 다시 operator가 호출되면 flag가 뒤집혀
        # 작동을 중지하고
        print('clicking', i)
        sleep(0.3)
        i += 1
    return  # while문을 탈출하여 return을 하므로써 스레드가 종료된다.


class Th2(threading.Thread):  # 스레드 클래스 상속
    def __init__(self):
        super().__init__()

    def run(self):  # 스레드 클래스의 run 메소드 오버라이드
        clicking()


def on_press(key):
    keySet.add(key)


def on_release(key):
    global pre_order, th2, flag01
    for action, trigger in hotkey.items():
        # 입력 감지값인 keySet과 핫키의 trigger의 동일 여부를 분석한다.
        print("release key =", key)
        print("keySet =", keySet)
        print(action, "trigger =", trigger[0])
        print(action, [True if triggerKey in keySet else False for triggerKey in trigger[0]])
        #   핫키 trigger[0] 집합에 있는 triggerKey(원소들)가 입력 감지값인 keySet에 모두 있으면 True를
        # 반환하는 함수이다. 만약 두 개의 키가 필요한 키라면 릴리즈 할때 릴리즈 부분이 두번 실행되지만
        # 마지막 줄에 keySey이 최초 먼저 띄어진 키에서 clear되기 때문에 두번째 실행에서는 check가
        # False가 되어 operator_를 실행하지 않는다.
        check = all([True if triggerKey in keySet else False for triggerKey in trigger[0]])
        # 프로그램을 command 상태로 돌리는 이스케이프 핫키이다. Ctrl + Esc
        if check and trigger[1] == -1:
            keySet.clear()
            return False  # 리스너에 False를 리턴하여 리스너를 종료한다.
        elif check:  # 지정된 핫키와 일치하는 action의 order number를 operator_에 전달하여
            #         필요한 작업을 실행한다.
            # 릴리즈 하는 키가 두 개여도 최초 릴리즈에서 keySet이 clear되므로 이 부분은 한번만 실행된다.
            order = trigger[1]
            pre_order.append(order)
            operator_(order)
    print("----------------------------------------")
    keySet.clear()


def keyboardListner():  # 키보드 입력을 감지한다. 별도의 스레드 위에서 작동한다.
    with Listener(on_press=on_press, on_release=on_release) as listener:
        listener.join()


def commander():
    i = 0
    while True:
        if i == 0:
            command = 'start'  # 최초에는 키보드 리스너를 start 한다.
            i += 1
        else:  # 이스케이프 핫키를 눌러서 command 모드로 진입한다.
            command = input(">>> ")
        if command == 'start':
            main000()
        elif command == 'end':
            exit(0)


def main000():
    global th2
    th1 = Thread(target=keyboardListner, daemon=True)  # 키보드 리스너 스레드
    th2 = Th2()  # clicking 스레드
    th2.daemon = True
    th1.start()
    th1.join()
    # 각 스레드들이 죽었는지 확인한다.
    if not th1.is_alive() and not th2.is_alive():
        print("thread1, thread2 killed")


if __name__ == "__main__":
    commander()
