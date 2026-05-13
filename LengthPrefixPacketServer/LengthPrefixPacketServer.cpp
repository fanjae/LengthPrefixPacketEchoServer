#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <string>

#pragma comment(lib, "ws2_32.lib")
bool RecvAll(SOCKET sock, char* buffer, int size) // 지정한 size 바이트를 전부 받을때까지 반복 수신
{
	int received = 0;

	while (received < size)
	{
		int ret = recv(sock, buffer + received, size - received, 0); 
		// buffer + received : 저장 시작 위치
		// size - received : 앞으로 더 받아야 할 바이트 수

		// ret == 0 : 연결 종료, recv < 0 : 연결 에러
		if (ret <= 0)
			return false;

		// 바이트 수 누적
		received += ret;
	}
	return true;
}
bool SendAll(SOCKET sock, const char* buffer, int size)  // 지정한 size 바이트를 전부 보낼때까지 반복 송신
{
	int sent = 0;

	while (sent < size)  
	{
		int ret = send(sock, buffer + sent, size - sent, 0);
		// buffer + received : 전송 시작 위치
		// size - sent : 앞으로 더 보내야할 바이트 수
		

		if (ret == SOCKET_ERROR) 
			return false;

		// 바이트 수 누적
		sent += ret;
	}

	return true;
}




int main(void)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData); // 소켓 초기화
	
	SOCKET listenSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 소켓 연결

	sockaddr_in serverAddr = {};
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_port = htons(7777);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(listenSock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
	listen(listenSock, SOMAXCONN);

	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	std::cout << "Echo Server Start Port Info : 7777\n";
	
	SOCKET clientSock = accept(listenSock, nullptr, nullptr);
	std::cout << "Client Connected. \n";

	while (true)
	{

		// 1. 헤더 길이 수신 처리
		uint32_t headLength = 0;

		if (!RecvAll(clientSock, reinterpret_cast<char*>(&headLength), sizeof(headLength)))
		{
			break;
		}

		// 네트워크 바이트 오더 -> 호스트 바이트 오더로 교체 한다.
		uint32_t bodyLength = ntohl(headLength); 

		std::cout << "Body Length : " << bodyLength << std::endl;
		if (bodyLength == 0 || bodyLength > 4096)
		{
			std::cout << "Invalid packet size : " << bodyLength << "\n";
			break;
		}
		

		// bodyLength 만큼 본문을 수신한다.
		std::vector<char> body(bodyLength);

		if (!RecvAll(clientSock, body.data(), bodyLength))
		{
			break;
		}

		std::string message(body.begin(), body.end());
		std::cout << "Received : " << message << "\n";

		// Echo 응답 보내기
		// 호스트 바이트 오더 -> 네트워크 바이트 오더.
		uint32_t echoLength = htonl(bodyLength);

		if (!SendAll(clientSock, reinterpret_cast<char*>(&echoLength), sizeof(echoLength)))
		{
			break;
		}

		if (!SendAll(clientSock, body.data(), bodyLength))
		{
			break;

		}
	}

	std::cout << "Client disconnected.\n";

	closesocket(clientSock);
	closesocket(listenSock);
	WSACleanup(); //윈속 라이브러리 정리


	return 0;
}