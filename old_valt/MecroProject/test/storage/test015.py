from time import sleep
from threading import Thread


class Flag01:
    def __init__(self):
        self.flag = False

    def showFlag(self):
        print("Flag =", self.flag)

    def returnFlag(self):
        return self.flag

    def switch(self):
        self.flag = not self.flag


class Operator:
    def __init__(self, flag):
        self.flag = flag

    def showFlag(self):
        self.flag.showFlag()

    def operation(self):
        while True:
            print("clicking")
            sleep(0.5)
            if not self.flag.returnFlag:
                break


def th3(operator_):
    operator_.operation()


def th2(operator_):
    while True:
        operator_.showFlag()
        sleep(1)


def th1(flag):
    operator_ = Operator(flag)
    operThread2 = Thread(target=th3, args=(operator_,))
    statThread = Thread(target=th2, args=(operator_,))
    statThread.start()
    operThread2.start()


def testMain000():
    flag = Flag01()
    flag.showFlag()
    operThread = Thread(target=th1, args=(flag,))
    operThread.start()
    sleep(5)
    print("switch")
    flag.switch()
    sleep(3)
    print("switch")
    flag.switch()


def testMain001():
    flag = Flag01()
    flag.showFlag()
    operThread = Thread(target=th1, args=(flag,))
    operThread.start()
    sleep(5)
    print("switch")
    flag.switch()


if __name__ == "__main__":
    testMain001()
