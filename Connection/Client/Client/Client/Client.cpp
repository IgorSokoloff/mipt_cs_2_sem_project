// VirusConnectionSystem.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include < winsock2.h > 
#include < ws2tcpip.h >
#include < iostream >

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_SERVER_PORT 27015
#define DEFAULT_BUFLEN 512
#define	DEFAULT_SERVER_IP "192.168.137.1"
#define bufsize 512


int _tmain(int argc, _TCHAR* argv[])
{
	std::cout << "Start work, motherfucker!!!" << std::endl;
	char OUR_DEFAULT_PORT_CHAR[] = "27015";  //сюда стоит прикрутить проверку, не занят ли стандартный порт. Но пока тупо используем его.
	//Инициализируем WSA
	WSADATA wsaData;
	int result;
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		std::cout << "Error in function WSAStartup: " << result << std::endl;
		getchar();
		return 1;
	}

	// Создаем сокет для отправки нашего Ip среверу
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "Error in function socket() while trying to create it first time: " << WSAGetLastError() << std::endl;
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
	while (true)
	{
		result = connect(clientSocket, (reinterpret_cast<SOCKADDR*>(&clientService)), sizeof(clientService));
		if (result == 0)
			break;
	}

	/*if (result != 0)
	{
	std::cout << "Error in function connect() while trying to connect to server first time: " << WSAGetLastError() << std::endl;
	getchar();
	WSACleanup();
	return 1;
	}*/

	std::cout << "Successful connection!" << std::endl;

	//Слушаем, что нам пришлют. Набор комманд:
	//0-закрываем соединение, переходим в режим прослушивания;
	//1-закрываем соединение, завершем работу;
	//2-отсылаем смайл :)
	while (true)
	{
		// получаем данные
		char buf[bufsize];
		char x = buf[bufsize];
		int r = recv(clientSocket, buf, bufsize, 0);
		if (r > 0)
		{
			//std::cout << "Got " << r << " bytes" << std::endl;
			int i = 0;
			std::cout << "Got Message: ";
			while (buf[i] != x && i <= 512)
			{
				std::cout << buf[i];
				i++;
			}
			std::cout << std::endl;
		}
		else if (r == 0)
		{
			std::cout << "Disconnect" << std::endl;
			WSACleanup();
			return 0;
		}
		else
		{
			std::cout << "Error in function recv(): " << WSAGetLastError() << std::endl;
			WSACleanup();
			return 1;
		}

		// Если прислали 2 - отсылаем смайл
		if (buf[0] == '2')
		{
			std::cout << "got 2, now I'll send a piece of happyness" << std::endl;
			char data[] = ":)";
			result = -1;
			result = send(clientSocket, data, static_cast< int >(strlen(data)), 0);
			if (result < 0)
			{
				std::cout << "Error in function send() while sanding information: " << WSAGetLastError() << std::endl;
				return 1;
			}
			continue;
		}
		// Если прислали 0 - отключаемся и начинаем слушать.
		if (buf[0] == '0')
		{
			std::cout << "Got 0, now I'll start listenning..." << std::endl;

			closesocket(clientSocket);

			struct addrinfo *result = NULL, *ptr = NULL, hints;
			ZeroMemory(&hints, sizeof(hints));
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			hints.ai_flags = AI_PASSIVE;



			int iResult = getaddrinfo(NULL, OUR_DEFAULT_PORT_CHAR, &hints, &result);
			if (iResult != 0)
			{
				std::cout << "Error at getaddrinfo while creating listenning socket: " << iResult << std::endl;
				WSACleanup();
				return 1;
			}

			SOCKET listenSocket = INVALID_SOCKET;
			listenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
			if (listenSocket == INVALID_SOCKET)
			{
				std::cout << "Error at socket() while creating listenning socket: " << WSAGetLastError() << std::endl;
				freeaddrinfo(result);
				WSACleanup();
				return 1;
			}

			iResult = bind(listenSocket, result->ai_addr, result->ai_addrlen);
			if (iResult == SOCKET_ERROR)
			{
				std::cout << "Bind listenning socket failed with error: " << WSAGetLastError() << std::endl;
				freeaddrinfo(result);
				closesocket(listenSocket);
				WSACleanup();
				return 1;
			}

			std::cout << "Listening..." << std::endl;

			if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
			{
				std::cout << "Listen failed with error: " << WSAGetLastError() << std::endl;
				closesocket(listenSocket);
				WSACleanup();
				return 1;
			}

			freeaddrinfo(result);


			clientSocket = INVALID_SOCKET;

			clientSocket = accept(listenSocket, NULL, NULL);
			if (clientSocket == INVALID_SOCKET)
			{
				std::cout << "Accept failed with error: " << WSAGetLastError() << std::endl;
				closesocket(listenSocket);
				WSACleanup();
				return 1;
			}

		}
		if (buf[0] == '1')
		{
			std::cout << "Got 1, now I'll close meyself." << std::endl;
			closesocket(clientSocket);
			WSACleanup();
			return 0;
		}

	}

	return 0;
}

