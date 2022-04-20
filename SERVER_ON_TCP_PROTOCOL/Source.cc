#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib,"Ws2_32.lib")

int main() {
	WSADATA winsockData;
	WORD var = MAKEWORD(2, 2);
	int winsockOk = WSAStartup(var, &winsockData);
	if (winsockOk != 0) {
		std::cerr << "Initializing winsock went wrong " << "\n";
		return 1;
	}
	SOCKET winsockListening = socket(AF_INET, SOCK_STREAM, 0);
	if (winsockListening == INVALID_SOCKET) {
		std::cerr << "Socket cannot be created " << "\n";
		return 1;
	}
	sockaddr_in Bind;
	Bind.sin_family = AF_INET;
	Bind.sin_port = htons(54000);
	//inet_pton
	Bind.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(winsockListening, (sockaddr*)&Bind, sizeof(Bind));

	listen(winsockListening, SOMAXCONN);

	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(winsockListening, (sockaddr*)&client, &clientSize);
	char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXHOST);

	if (getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
		std::cout << host << " is connected on port: " << service << "\n";
	}
	else {
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " connected on port " << ntohs(client.sin_port) << "\n";
	}
	closesocket(winsockListening);

	char buf[4096];
	while (true) {
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(clientSocket, buf, 4096, 0);
		if (bytesReceived == SOCKET_ERROR) {
			std::cerr << "Error while receiving data." << "\n";
			break;
		}
		if (bytesReceived == 0) {
			std::cout << "Client was disconnected " << "\n";
			break;
		}

		std::cout << std::string(buf, 0, bytesReceived) << "\n";
		send(clientSocket, buf, bytesReceived + 1, 0);
	}

	closesocket(clientSocket);
	WSACleanup();


}