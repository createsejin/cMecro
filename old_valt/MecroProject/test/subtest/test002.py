from serial import Serial, SerialException
from time import sleep


def test000():
    """
    delay(32);
    Keyboard.releaseAll();
    delay(32);
    Keyboard.press('-');
    delay(32);
    Keyboard.releaseAll();
    delay(500);
    // 1
    Keyboard.press('1');
    delay(32);
    Keyboard.releaseAll();
    delay(32);
    Keyboard.press('-');
    delay(32);
    Keyboard.releaseAll();
    delay(500);
    for (int i = 0; i < 3; i++) {
        Keyboard.press('1');
        delay(32);
        Keyboard.releaseAll();
        delay(64);
        Keyboard.press('1');
        delay(32);
        Keyboard.releaseAll();
        delay(32);
        Keyboard.press('-');
        delay(32);
        Keyboard.releaseAll();
        delay(500);
    """
    op = (32*3 + 500)*2 + (32*4 + 64 + 500)*3
    print(op)


ser = object()


def test001():
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
    while True:
        cmd = input(">>> ")
        if cmd == '1':
            sleep(3)
            str1 = 'c010000'
            ser.write(str1.encode())
            if ser.readable():
                resp = ser.readline()
                result = resp[:-1].decode()
                print(result)
        elif cmd == '2':
            dropWeapons()
        elif cmd == 'end':
            ser.close()
            exit(0)


def dropWeapons():
    keyPress('-')
    sleep(0.064)
    keyRelease('-')
    sleep(0.064)
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


if __name__ == "__main__":
    test001()

