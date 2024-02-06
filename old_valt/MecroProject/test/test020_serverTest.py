import socket
import pickle


if __name__ == "__main__":
    addr = ('localhost', 12346)
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind(addr)
        server_socket.listen()
        client_socket, client_addr = server_socket.accept()
        while True:
            recvdata = client_socket.recv(1024)
            L = pickle.loads(recvdata)
            print(L)
            if L[0] == 1:
                L2 = [1, 0, 0]
                send_data = pickle.dumps(L2)
                client_socket.send(send_data)
            elif L[0] == 2:
                L2 = [2, 0, 0]
                send_data = pickle.dumps(L2)
                client_socket.send(send_data)
