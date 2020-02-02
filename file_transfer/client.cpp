// socket客户端
#include <stdio.h>
#include <string.h>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024

int main(int argc, char** argv)
{
		FILE *fp;
		if (argc != 2)
		{
				printf("usage: ./client <ipaddress>\n");
				return 0;
		}

		int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		sockaddr_in serv_addr;
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = PF_INET;
		serv_addr.sin_port = htons(5000);
		if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr) <= 0)
		{
				printf("inet_pton error for %s\n", argv[1]);
				return 0;
		}

		// 输入文件名，看文件是否能创建成功
		/*
		char filename[100] = {0};
		printf("Input filename to save:");
		fgets(filename, 100, stdin);
		FILE *fp = fopen(filename, "ab");	// 以二进制方式打开（创建）文件
		if (fp == NULL)
		{
				printf("Cannot open file, press any key to exit!\n");
				exit(0);
		}*/
		if ((fp = fopen("1.cpp", "ab")) == NULL)
		{
				printf("File.\n");
				close(sock);
				exit(1);
		}

		if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0)
		{ perror("connect"); close(sock); return -1; }

		char buffer[BUF_SIZE];
		int nCount;
		while (1) {
				nCount = read(sock, buffer, BUF_SIZE);
				if (nCount == 0)
						break;
				fwrite(buffer, 1, nCount, fp);
		}
		buffer[nCount] = '\0';
		/*
		while ((nCount = recv(sock, buffer, BUF_SIZE, 0)) >0)
				fwrite(buffer, nCount, 1, fp);
				*/
		puts("File transfer success!");

		// 文件接收完毕直接关闭套接字，不需要调用shutdown()
		fclose(fp);
		close(sock);

		return 0;
}
