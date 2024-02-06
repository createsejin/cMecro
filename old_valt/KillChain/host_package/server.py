import socket
import pickle
from threading import Thread


ip = socket.gethostbyname(socket.gethostname())
port = 12345
addr = (ip, port)
host_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host_socket.bind(addr)
host_socket_name = host_socket.getsockname()
kill_chain = False
print("host socket =", host_socket_name[0] + ':' + str(host_socket_name[1]))
user = dict()


def get_client_socket():
    host_socket.listen()
    client_socket, client_addr = host_socket.accept()  # 호스트에 클라이언트가 접속할때까지 대기한다.
    incoming = '-> ' + client_addr[0] + ':' + str(client_addr[1]) + ' connected.'
    print(incoming)
    return client_socket, client_addr


def threading_service():
    global user
    while True:
        client_socket, client_addr = get_client_socket()
        key = client_addr[0] + ':' + str(client_addr[1])
        user[key] = [None, client_socket]
        service_thread = Thread(target=service, args=(client_socket, client_addr), daemon=True)
        service_thread.start()


def service(client_socket, client_addr):
    addr = client_addr[0] + ':' + str(client_addr[1])

    def sendData(ils, socket):
        send_data = pickle.dumps(ils)
        socket.send(send_data)

    while True:
        try:
            recvdata = client_socket.recv(1024)
        except ConnectionResetError:
            print(client_addr[0] + ':' + str(client_addr[1]))
            print("An existing connection was forcibly closed by the remote host")
            client_socket.close()
            print("client socket", client_addr[0] + ':' + str(client_addr[1]), "is closed.")
            return
        if len(recvdata) > 0 or recvdata is not None:
            try:
                L = pickle.loads(recvdata)
                result = operator_(L)
                for key, item in user.items():
                    sendData(result, item[1])
            except EOFError:
                client_socket.close()
                print("client socket", client_addr[0] + ':' + str(client_addr[1]), "is closed.")
                print("nickname: '{}'님이 프로그램을 종료했습니다.".format(user[addr][0]))
                return


def operator_(L):
    global user, kill_chain
    if L == [1, 0, 0]:
        print("kill process")
        kill_chain = True
        result = [1, 0, 1]
    elif L[0] == 2:
        print("nickname: '{}' is connected.".format(L[1]))
        for key in user.keys():
            if user[key][0] is None:
                user[key][0] = L[1]
        result = [2, 0, 1]
    else:
        print("L =", L)
        result = -1
    return result
