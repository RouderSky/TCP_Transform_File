/************************************************************************
> Tcp transform file client
> ���ļ��ϴ���������
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
	// ��ʼ��socket dll 
	WSADATA wsaData;
	if (SOCKET_ERROR == WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("Init socket dll error:%d\n", WSAGetLastError());
		exit(1);
	}

	//����socket 
	SOCKET socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == socketServer)
	{
		printf("Create Socket Error:%d\n", WSAGetLastError());
		exit(1);
	}

	//������������ַ 
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(SERVER_PORT);

	//���ӵ�������
	if (SOCKET_ERROR == connect(socketServer, (LPSOCKADDR)&server_addr, sizeof(server_addr)))
	{
		printf("Can Not Connect To Server IP:%d\n", WSAGetLastError());
		exit(1);
	}

	//�����ļ��� 
	char file_name[FILE_NAME_SIZE + 1];
	memset(file_name, 0, FILE_NAME_SIZE + 1);
	printf("Please Input File Name On Server: ");
	scanf("%s", &file_name);

	//�����ļ���
	if (send(socketServer, file_name, FILE_NAME_SIZE + 1, 0) < FILE_NAME_SIZE + 1)	//ע�⣺���﷢�͵�����������
	{
		printf("Send file name fail:%d\n", WSAGetLastError());
		exit(1);
	}

	//׼����������
	char buffer[BUFFER_SIZE];
	FILE * fp = fopen(file_name, "rb"); //��ָ���ļ�
	if (NULL == fp)
	{
		printf("File: %s Not Found\n", file_name);
		exit(1);
	}
	else
	{
		//�����ļ���С��Ϣ
		fseek(fp, 0, SEEK_END); //��λ���ļ�ĩ 
		int total = ftell(fp); //�ļ�����
		char file_size[FILE_SIZE+1];
		memset(file_size, 0, FILE_SIZE+1);
		itoa(total, file_size, 10);
		if (send(socketServer, file_size, FILE_SIZE+1, 0) < FILE_SIZE+1)		//ע�⣺���﷢�͵�����������
		{
			printf("Send file size fail:%d\n", WSAGetLastError());
			exit(1);
		}
		fseek(fp, 0, SEEK_SET);

		//���������
		printf("\nprogress:\n");
		for (int i = 0; i < 100; i++)
			printf(" ");
		printf("||\r");

		//�������ݹ���
		int length = 0;			//��¼��ǰ���ļ���ȡ�������ݳ���
		int count = 0;
		int lastProgress = 0;
		memset(buffer, 0, BUFFER_SIZE);
		while ((length = fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0)
		{
			//��������
			if (send(socketServer, buffer, length, 0) < length)	//ע�⣺���ﴫ�͵����ݴ�С��length����������ʹ���������پʹ�����
			{
				printf("Send File: %s Failed:%d\n", file_name, WSAGetLastError());
				exit(0);
			}
			memset(buffer, 0, BUFFER_SIZE);

			//��ʾ�ϴ�����
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