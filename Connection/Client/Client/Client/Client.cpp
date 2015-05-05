#include "stdafx.h"
#include < winsock2.h > 
#include < ws2tcpip.h >
#include < iostream >

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_SERVER_PORT 27015
#define DEFAULT_BUFLEN 512
#define	DEFAULT_SERVER_IP "192.168.137.1"


int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "Start work, motherfucker!" << std::endl;

	//Инициализируем WSA
	WSADATA wsaData;
	int result;
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		std::cout << "Mistake in function WSAStartup: " << result << std::endl;
		getchar();
		return 1;
	}

	// Создаем сокет для отправки нашего Ip среверу
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "Mistake in function socket(): " << WSAGetLastError() << std::endl;
		getchar();
		WSACleanup();
		return 1;
	}

	// Пытаемя приконнектиться к серверу

	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	inet_pton(AF_INET, DEFAULT_SERVER_IP, &(clientService.sin_addr));
	clientService.sin_port = htons(DEFAULT_SERVER_PORT);

	std::cout << "Trying to connect to " << DEFAULT_SERVER_IP << std::endl;

	result = connect(clientSocket, (reinterpret_cast< SOCKADDR* >(&clientService)), sizeof(clientService));
	if (result != 0)
	{
		std::cout << "Mistake in function connect(): " << WSAGetLastError() << std::endl;
		getchar();
		WSACleanup();
		return 1;
	}

	std::cout << "Successful connection!" << std::endl;

	//	char data[512];

	return 0;
}
