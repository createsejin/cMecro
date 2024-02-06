import socket
import pickle
from threading import Thread


socket_list = list()
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)


def init_server():
    global server_socket
    ip = socket.gethostbyname(socket.gethostname())
    port = 12345
    addr = (ip, port)
    server_socket.bind(addr)
    print("server socket binding complete.")
    print("server stand by for connecting client program.")


def send_data(data):
    b_data = pickle.dumps(data)
    for client_socket in socket_list:
        client_socket.send(b_data)


def receiver(client_socket, client_addr):
    while True:
        try:
            b_data = client_socket.recv(1024)
            if len(b_data) > 0 or b_data is not None:
                L = pickle.loads(b_data)
                result = operator_(L)
                send_data(result)
        except ConnectionResetError:
            key = client_addr[0] + ':' + str(client_addr[1])
            client_socket.close()
            socket_list.remove(client_socket)
            print("client socket = {} is closed.".format(key))
            return
        except EOFError:
            key = client_addr[0] + ':' + str(client_addr[1])
            client_socket.close()
            socket_list.remove(client_socket)
            print("client socket = {} is closed.".format(key))
            return


def operator_(L):
    if L[0] == 1:
        print('remote control : F press requested')
        result = [1, 1]
    elif L[:2] == [1.02, 0]:
        result = [1.02, False]
    elif L[:2] == [1.02, 1]:
        result = [1.02, True]
    elif L[0] == 2:
        result = [2, 1, 'sub client successfully connected']
        print(result[2])
    elif L[0] == 3:
        result = [3, 1, 'main client successfully connected']
        print(result[2])
    elif L[0] == 4:
        result = [4, 1]
        print('disconnect main client')
    elif 21 < L[0] < 21.1:
        print('remote control requested =', L[0])
        result = [L[0], 1]
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
        receiver_thread = Thread(target=receiver, args=(client_socket, client_addr), daemon=True)
        receiver_thread.start()


if __name__ == "__main__":
    init_server()
    threading_receiver()
