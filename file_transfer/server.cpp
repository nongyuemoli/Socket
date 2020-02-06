// socket文件传输服务端
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{

		int servSock = socket(AF_INET, SOCK_STREAM, 0);

		struct sockaddr_in serv_addr;
		memset(&serv_addr, 0, sizeof(serv_addr));
		serv_addr.sin_family = PF_INET;
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 本主机任意IP地址
		serv_addr.sin_port = htons(5000);
		if (bind(servSock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) != 0)
		{ perror("bind"); close(servSock); return -1; }

		// 进入监听状态
		if (listen(servSock, 5) != 0) { perror("listen"); close(servSock); return -1; }

		struct sockaddr_in clientaddr;
		int socklen = sizeof(struct sockaddr_in);
		int clientfd = accept(servSock, (struct sockaddr *)&clientaddr, (socklen_t*)&socklen);

		// 检查文件是否存在
		FILE *fp = fopen("/home/nongyue/c/Socket/client4.cpp", "rb");	// 以二进制方式打开文件
		if (fp == NULL)
		{
				printf("Cannot open file, press any key to exit!\n");
				close(servSock);
				exit(1);
		}
		// 循环发送数据，直到文件结尾
		char buffer[BUF_SIZE] = {0};
		while (!feof(fp)) {
				int len = fread(buffer, 1, BUF_SIZE, fp);
				if (len != write(servSock, buffer, len)){
						printf("write.\n");
						break;
				}
		}
		/*int nCount;
		while ((nCount = fread(buffer, 1, BUF_SIZE, fp)) >= 0)
				send(clientfd, buffer, nCount, 0);

		*/
		shutdown(clientfd, SHUT_WR);	// 断开输出流，向客户端发送FIN包
		recv(clientfd, buffer, BUF_SIZE, 0);	// 阻塞，等待客户端接收完毕

		fclose(fp);
		close(clientfd);
		close(servSock);

		return 0;
}
