# pynput 작동시에 아두이노가 제대로 작동하는지 확인하는 코드 -> 작동 잘됨.
import threading
from time import sleep
from threading import Thread
import socket
import pickle


def service(client_soc, addr):
    print("server service started.")
    incoming = addr[0] + ':' + str(addr[1]) + ' connected.'
    print(incoming)
    while True:
        recvdata = client_soc.recv(1024)
        if len(recvdata) > 0:
            L = pickle.loads(recvdata)
            if len(L) > 0:
                print(L)
                L2 = [1, 1, 1]
                data = pickle.dumps(L2)
                client_soc.send(data)


def set_socket():
    print("socket setting")
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as soc:
        soc.bind(('localhost', 1024))
        soc.listen()
        client_soc, addr = soc.accept()
        service_thread = Thread(target=service, args=(client_soc, addr), daemon=True)
        service_thread.start()
    print("setting end")
    print("service thread alive =", service_thread.is_alive())


def start():
    set_socket()


def commander():
    while True:
        cmd = input(">>> ")
        if cmd == 'start':
            start()
        elif cmd == 'end':
            exit(0)
        elif cmd == 'test':
            pass
        else:
            print("Please enter a correct command.")


if __name__ == "__main__":
    soc = object()
    commander()
