#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")
bool RecvAll(SOCKET sock, char* buffer, int size)
{
	int received = 0;

	while (received < size)
	{
		int ret = recv(sock, buffer + received, size - received, 0);

		if (ret <= 0)
			return false;

		received += ret;
	}

	return true;
}
bool SendAll(SOCKET sock, const char* buffer, int size)
{
	int sent = 0;

	while (sent < size)
	{
		int ret = send(sock, buffer + sent, size - sent, 0);

		if (ret == SOCKET_ERROR)
			return false;

		sent += ret;
	}

	return true;
}




int main(void)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	SOCKET listenSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in serverAddr = {};
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(7777);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(listenSock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
	listen(listenSock, SOMAXCONN);

	std::cout << "Echo Server Start Port Info : 7777\n";
	
	SOCKET clientSock = accept(listenSock, nullptr, nullptr);
	std::cout << "Client Connected. \n";

	while (true)
	{
		
	}
}