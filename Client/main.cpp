/************************************************************************
> Tcp transform file client
> 将文件上传到服务器
************************************************************************/
#if 1

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <winsock2.h> 

#define SERVER_PORT 8087 
#define SERVER_IP "127.0.0.1" 
#define BUFFER_SIZE 1024 
#define FILE_NAME_SIZE 512 
#define FILE_SIZE 10
#pragma comment(lib, "ws2_32.lib") 
#pragma warning( disable : 4996)

int main()
{
	// 初始化socket dll 
	WSADATA wsaData;
	if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("Init socket dll error:%d\n", WSAGetLastError());
		exit(1);
	}

	//创建socket 
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socketServer)
	{
		printf("Create Socket Error:%d\n", WSAGetLastError());
		exit(1);
	}

	//创建服务器地址 
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(SERVER_PORT);

	//连接到服务器
	if (SOCKET_ERROR == connect(socketServer, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("Can Not Connect To Server IP:%d\n", WSAGetLastError());
		exit(1);
	}

	//输入文件名 
	char file_name[FILE_NAME_SIZE + 1];
	memset(file_name, 0, FILE_NAME_SIZE + 1);
	printf("Please Input File Name On Server: ");
	scanf("%s", &file_name);

	//发送文件名
	if (send(socketServer, file_name, FILE_NAME_SIZE + 1, 0) < FILE_NAME_SIZE + 1)	//注意：这里发送的是整个数组
	{
		printf("Send file name fail:%d\n", WSAGetLastError());
		exit(1);
	}

	//准备发送数据
	char buffer[BUFFER_SIZE];
	FILE * fp = fopen(file_name, "rb"); //打开指定文件
	if (NULL == fp)
	{
		printf("File: %s Not Found\n", file_name);
		exit(1);
	}
	else
	{
		//发送文件大小信息
		fseek(fp, 0, SEEK_END); //定位到文件末 
		int total = ftell(fp); //文件长度
		char file_size[FILE_SIZE+1];
		memset(file_size, 0, FILE_SIZE+1);
		itoa(total, file_size, 10);
		if (send(socketServer, file_size, FILE_SIZE+1, 0) < FILE_SIZE+1)		//注意：这里发送的是整个数组
		{
			printf("Send file size fail:%d\n", WSAGetLastError());
			exit(1);
		}
		fseek(fp, 0, SEEK_SET);

		//进度条设计
		printf("\nprogress:\n");
		for (int i = 0; i < 100; i++)
			printf(" ");
		printf("||\r");

		//发送数据过程
		int length = 0;			//记录当前从文件读取到的数据长度
		int count = 0;
		int lastProgress = 0;
		memset(buffer, 0, BUFFER_SIZE);
		while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
		{
			//发送数据
			if (send(socketServer, buffer, length, 0) < length)	//注意：这里传送的数据大小由length来决定，即使读出来多少就传多少
			{
				printf("Send File: %s Failed:%d\n", file_name, WSAGetLastError());
				exit(0);
			}
			memset(buffer, 0, BUFFER_SIZE);

			//显示上传进度
			count += length;
			int progress = (float)count / total * 100;
			if (progress > lastProgress)
			{
				for (int i = 0; i < progress - lastProgress; i++)
					printf("=");
				lastProgress = (float)count / total * 100;
			}

		}
		printf("\n\n");

		fclose(fp);
		printf("File: %s Transfer Successful!\n", file_name);
	}

	closesocket(socketServer);
	WSACleanup();
	system("pause");
	return 0;
}

#endif