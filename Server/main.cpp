
/************************************************************************
> TCP transform file server
> ���տͻ����ϴ����ļ�
************************************************************************/
#if 1

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <winsock2.h> 

#define SERVER_PORT 8087 
#define SERVER_IP	"127.0.0.1" 
#define BUFFER_SIZE 1024 
#define FILE_NAME_SIZE 512 
#define FILE_SIZE 10
#pragma comment(lib, "ws2_32.lib") 
#pragma warning( disable : 4996)

int main()
{
	// ��ʼ��socket dll 
	WSADATA wsaData;
	if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("Init socket dll error:%d\n", WSAGetLastError());
		exit(1);
	}

	//Ϊ��ǰ����������һ��IP��ַ�Ͷ˿� 
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	server_addr.sin_port = htons(SERVER_PORT);

	// ���������õ�socket
	SOCKET socketListen = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socketListen)
	{
		printf("Create Socket Error:%d\n", WSAGetLastError());
		exit(1);
	}

	//ΪsocketListen��һ����ַ�Ͷ˿�
	if (SOCKET_ERROR == bind(socketListen, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("Server Bind Failed: %d\n", WSAGetLastError());
		exit(1);
	}

	//�������� 
	if (SOCKET_ERROR == listen(socketListen, 5))
	{
		printf("Server Listen Failed: %d\n", WSAGetLastError());
		exit(1);
	}

	//��ʼ���ܿͻ�������
	while (1)
	{
		printf("Listening To Client...\n");

		sockaddr_in client_addr;
		int len = sizeof(sockaddr_in);
		SOCKET socketClient = accept(socketListen, (sockaddr *)&client_addr, &len);
		if (INVALID_SOCKET == socketClient)
		{
			printf("Server Accept Failed: %d\n", WSAGetLastError());
			exit(1);
		}
		printf("��ǰ�пͻ��˽���\n");

		//�����ļ���
		char file_name[FILE_NAME_SIZE + 1];
		memset(file_name, 0, FILE_NAME_SIZE + 1);
		if (SOCKET_ERROR == recv(socketClient, file_name, FILE_NAME_SIZE + 1, 0))
		{
			printf("Recv file name fail:%d\n", WSAGetLastError());
		}
		
		//��ȡ�ļ���
		char* token = strtok(file_name, ":\\");
		char* only_name;
		while (token != NULL)
		{
			only_name = token;
			token = strtok(NULL, ":\\");
		}
		printf("�ͻ����ϴ��ļ���%s\n", only_name);

		//׼����������
		char buffer[BUFFER_SIZE];
		FILE * fp = fopen(only_name, "wb"); //���ļ�
		if (NULL == fp)
		{
			printf("File: %s Can Not Open To Write:%d\n", only_name, WSAGetLastError());
			exit(1);
		}
		else
		{
			//�����ļ���С��Ϣ
			char file_size[FILE_SIZE+1];
			memset(file_size, 0, FILE_SIZE+1);
			if (SOCKET_ERROR == recv(socketClient, file_size, FILE_SIZE+1, 0))
			{
				printf("Recv file size fail:%d\n", WSAGetLastError());
				exit(1);
			}
			printf("�ļ���СΪ��%s Btye\n", file_size);
			int total = atoi(file_size);

			//���������
			printf("\nprogress:\n");
			for (int i = 0; i < 100; i++)
				printf(" ");
			printf("||\r");

			//�������ݹ���
			int count = 0;			//��¼��ǰ�ѽ���������
			int length = 0;			//��¼��ǰ�ν��յ���������
			int lastProgress = 0;
			memset(buffer, 0, BUFFER_SIZE);
			while (count < total)			//ͨ�����յ�����������ȷ��ʲôʱ��ֹͣ���������ǿɿ���
			{
				if (SOCKET_ERROR == (length = recv(socketClient, buffer, BUFFER_SIZE, 0)))
				{
					printf("Recv file fail:%d\n", WSAGetLastError());
					exit(1);
				}

				count += length;

				//д���ļ�
				if (fwrite(buffer, sizeof(char), length, fp) < length)
				{
					printf("File: %s Write Failed\n", only_name);
					break;
				}
				memset(buffer, 0, BUFFER_SIZE);

				//��ʾ�ϴ�����
				int progress = (float)count / total * 100;
				if (progress > lastProgress)
				{
					for (int i = 0; i < progress - lastProgress; i++)
						printf("=");
					lastProgress = (float)count / total * 100;
				}
			}

			fclose(fp);
			printf("\n\nReceive File: %s From Server Successful!\n", only_name);
		}
	}
	printf("\n\n");

	closesocket(socketListen);
	WSACleanup();
	system("pause");
	return 0;
}

#endif