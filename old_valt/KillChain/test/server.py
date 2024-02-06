import socket
import pickle
from threading import Thread


socket_list = list()
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


def send_data(data):
    b_data = pickle.dumps(data)
    for client_socket in socket_list:
        client_socket.send(b_data)


def receiver(client_socket):
    while True:
        b_data = client_socket.recv(1024)
        if len(b_data) > 0 or b_data is not None:
            L = pickle.loads(b_data)
            result = operator_(L)
            send_data(result)


def operator_(L):
    if L[0] == 1:
        print("client request test call")
        result = [1, 0, 1]
    else:
        print("L =", L)
        result = -1
    return result


def threading_receiver():
    global socket_list
    while True:
        server_socket.listen(0)
        client_socket, client_addr = server_socket.accept()
        client_addr_key = client_addr[0] + ':' + str(client_addr[1])
        print(client_addr_key, 'is connected.')
        socket_list.append(client_socket)
        receiver_thread = Thread(target=receiver, args=(client_socket,), daemon=True)
        receiver_thread.start()


def init_server():
    global server_socket
    ip = socket.gethostbyname(socket.gethostname())
    port = 12345
    addr = (ip, port)
    server_socket.bind(addr)
    print("server socket binding complete.")


if __name__ == '__main__':
    init_server()
    threading_receiver()
