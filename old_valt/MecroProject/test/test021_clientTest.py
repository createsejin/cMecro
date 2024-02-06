import socket
import pickle


if __name__ == "__main__":
    addr = ('localhost', 12346)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect(addr)
        while True:
            cmd = input(">>> ")
            if cmd == '1':
                L = [1, 1, 2]
                send_data = pickle.dumps(L)
                client_socket.send(send_data)
                recv_data = client_socket.recv(1024)
                respL = pickle.loads(recv_data)
                print(respL)
            elif cmd == '2':
                L = [2, 2, 3]
                send_data = pickle.dumps(L)
                client_socket.send(send_data)
                recv_data = client_socket.recv(1024)
                respL = pickle.loads(recv_data)
                print(respL)
