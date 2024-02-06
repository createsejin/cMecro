import socket


def test000(int1):
    print("test")
    int1 += 10
    return int1


def test001():
    print(socket.gethostbyname(socket.gethostname()))


if __name__ == '__main__':
    test001()
