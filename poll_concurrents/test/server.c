// server.c -- poll tcp socket server 将客户端发送的字符串变成大写,封装错误函数
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <ctype.h>

#include "wrap.h"

#define MAXLINE 80
#define PORT 5000		// 端口号
#define OPEN_MAX 1024

int main(int argc, char * argv[])
{
	int i, j, maxi, listenfd, connfd, sockfd;

	int nready;		// 接受poll返回值，记录满足监听事件的fd个数
	ssize_t n;

	char buf[MAXLINE], str[INET_ADDRSTRLEN];	// #define INET_ADDRSTRLEN 16

	struct sockaddr_in serv_addr, clie_addr;
	socklen_t clie_addr_len;
	struct pollfd client[OPEN_MAX];

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);	// 使用tcp

	// 端口复用
	int opt = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	// 清零
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;			// Ipv4
	serv_addr.sin_port = htons(PORT);	// 端口 
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 所有本地IP

	// 绑定socket
	Bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	// 同时只有128个建立连接
	Listen(listenfd, 128);

	client[0].fd = listenfd;	// 要监听的第一个文件描述符存入client[0]
	client[0].events = POLLIN;	// listenfd监听普通读事件

	for (i = 1; i < OPEN_MAX; i++)
		client[i].fd = -1;

	maxi = 0;							// client[]数组有效元素中最大元素下标

	for ( ; ; )
	{
		nready = poll(client, maxi+1, -1);	// 阻塞监听是否有客户端链接请求

		// 说明有新的客户端链接请求
		if (client[0].revents & POLLIN)
		{
			clie_addr_len = sizeof(clie_addr);
			connfd = Accept(listenfd, (struct sockaddr *)&clie_addr, &clie_addr_len);
			printf("received from %s at port: %d\n", 
					inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, str, sizeof(str)),
					ntohs(clie_addr.sin_port));

			for (i = 1; i < OPEN_MAX; ++i)
				if (client[i].fd < 0)		// 找client[]中没有使用的位置
				{
					client[i].fd = connfd;	// 保存accept返回的文件描述符到client[]里
					break;
				}

			if (i == OPEN_MAX)		// 达到poll能监控的文件个数上限1024
				perr_exit("too many clients");

			client[i].events = POLLIN;	// 设置刚刚返回的connfd,监控读事件
			if (i > maxi)
				maxi = i;				// 保证maxi存的总是client[]最后一个元素下标

			if (--nready == 0)
				continue;
		}

		// 检测哪个clients有数据就绪
		for (i = 1; i <= maxi; ++i)
		{
			if ((sockfd = client[i].fd) < 0)
				continue;

			if (client[i].revents & POLLIN)
			{
				if ((n = Read(sockfd, buf, MAXLINE)) < 0)
				{
					if (errno == ECONNRESET)
					{
						printf("client[%d] aborted connection\n", i);
						Close(sockfd);		// 当client关闭链接时，服务端也关闭对应链接
						client[i].fd = -1;
					}
					else
						perr_exit("read error");
				}
				else if (n == 0) 
				{
					printf("client[%d] closed connection\n", i);
					Close(sockfd);		// 当client关闭链接时，服务端也关闭对应链接
					client[i].fd = -1;
				}
				else
				{
					for (j = 0; j < n; j++)
						buf[j] = toupper(buf[j]);
					Write(sockfd, buf, n);
				}
				if (--nready == 0)
					break;				// 跳出for, 但还在while中
			}
		}
	}

	return 0;
}
