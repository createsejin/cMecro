import socket
import pickle
from threading import Thread


socket_list = list()
user = dict()
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# pyinstaller --icon=./icon/server_icon.ico server.py
# pyinstaller --icon=./icon/chain.ico client.py


def init_server():
    global server_socket
    ip = socket.gethostbyname(socket.gethostname())
    port = 12345
    addr = (ip, port)
    server_socket.bind(addr)
    print("server socket binding complete.")
    print("서버 프로그램이 멤버의 클라이언트 프로그램과의 연결을 위해 대기합니다...")


def send_data(data):
    b_data = pickle.dumps(data)
    for client_socket in socket_list:
        client_socket.send(b_data)


exit_by_close = False


def receiver(client_socket, client_addr):
    global user
    while True:
        try:
            b_data = client_socket.recv(1024)
            if len(b_data) > 0 or b_data is not None:
                L = pickle.loads(b_data)
                result = operator_(L)
                send_data(result)
        except ConnectionResetError:
            key = client_addr[0] + ':' + str(client_addr[1])
            if not exit_by_close:
                nickname = user[key]
                print("nickname: '{}'님이 프로그램을 종료하셨습니다.".format(nickname))
                del(user[key])
                result = get_current_member()
                print(result)
                client_socket.close()
                socket_list.remove(client_socket)
                print("client socket = {} is closed.".format(key))
                return
            else:
                client_socket.close()
                socket_list.remove(client_socket)
                print("client socket = {} is closed.".format(key))
                return


def threading_receiver():
    global socket_list, user
    while True:
        server_socket.listen(0)
        client_socket, client_addr = server_socket.accept()
        client_addr_key = client_addr[0] + ':' + str(client_addr[1])
        print(client_addr_key, 'is connected.')
        socket_list.append(client_socket)
        user[client_addr_key] = None
        receiver_thread = Thread(target=receiver, args=(client_socket, client_addr), daemon=True)
        receiver_thread.start()


def get_current_member():
    if len(user) == 0:
        result = "현재 접속 중인 멤버가 없습니다."
    else:
        str1 = "현재 접속 중인 멤버 = ["
        for nickname in user.values():
            str1 += '\'' + nickname + '\', '
        result = str1[:-2] + "]"
    return result


def operator_(L):
    global user, exit_by_close
    if L[0] == 1:
        print("nickname: '{}'님이 GTA 프로세스 종료를 요청하셨습니다.".format(L[1]))
        result = [-1, L[1], 1]
    elif L[0] == 2:
        print("nickname: '{}'님이 접속하셨습니다.".format(L[1]))
        for key in user.keys():
            if user[key] is None:
                user[key] = L[1]
        current_member = get_current_member()
        print(current_member)
        result = [-2, L[1], current_member, 1]
    elif L[0] == 3:
        exit_by_close = True
        find_key = None
        for key, nickname in user.items():
            if L[1] == nickname:
                find_key = key
                break
            else:
                continue
        del(user[find_key])
        current_member = get_current_member()
        print("nickname: '{}'님이 프로그램을 종료하셨습니다.".format(L[1]))
        print(current_member)
        result = [-3, L[1], current_member, 1]
    elif L[0] == 4:
        print("nickname: '{}'님이 신호 TEST를 요청하셨습니다.".format(L[1]))
        current_member = get_current_member()
        result = [-4, L[1], 1, current_member]
    else:
        print("L =", L)
        result = -1
    return result


# exec(open("F:\\GTA5\\KillChain\\server.py", 'rt', encoding='UTF8').read())
if __name__ == '__main__':
    init_server()
    threading_receiver()
