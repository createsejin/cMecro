import socket
import pickle
from threading import Thread


client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


def receiver():
    while True:
        b_data = client_socket.recv(1024)
        respL = pickle.loads(b_data)
        operator_(respL)


def operator_(L):
    if L == [1, 0, 1]:
        print("test call =", L)


def send_data(L):
    b_data = pickle.dumps(L)
    client_socket.send(b_data)


def init_client():
    global client_socket
    ip = '110.15.2.9'
    port = 12345
    addr = (ip, port)
    client_socket.connect(addr)


def test000():
    while True:
        input(">>> ")
        send_data([1, 0, 0])


if __name__ == '__main__':
    init_client()
    receiver_thread = Thread(target=receiver, daemon=True)
    receiver_thread.start()
    test000()
