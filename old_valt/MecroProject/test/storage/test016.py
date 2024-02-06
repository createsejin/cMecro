from time import sleep


class Flag:
    def __init__(self):
        self.flag = False

    def showFlag(self):
        print("Flag =", self.flag)

    def toggle(self):
        self.flag = not self.flag


class Operator:
    def __init__(self, flag):
        self.flag = flag

    def showFlag(self):
        self.flag.showFlag()

    def operation(self):
        if self.flag.flag:
            print("doing")
        else:
            print("not doing")


def testMain000():
    flag = Flag()
    flag.showFlag()
    flag.toggle()
    flag.showFlag()
    operator_ = Operator(flag)
    for i in range(0, 51):
        operator_.operation()
        sleep(0.3)


if __name__ == "__main__":
    testMain000()
