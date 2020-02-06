// server.c -- select tcp socket server 将客户端发送的字符串变成大写,封装错误函数
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "wrap.h"

#define PORT 5000		// 端口号

int main(int argc, char *argv[])
{
	int i, j, n, maxi;

	int nready, client[FD_SETSIZE];		// 自定义数组client, 防止遍历1024个文件描述符 FD_SETSIZE默认1024
	int maxfd, listenfd, connfd, sockfd;
	char buf[BUFSIZ], str[INET_ADDRSTRLEN];	// #define INET_ADDRSTRLEN 16

	struct sockaddr_in serv_addr, clie_addr;
	socklen_t clie_addr_len;
	fd_set rset, allset;				// rset 读事件文件描述符集合 allset用来暂存

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);	// 使用tcp

	// 清零
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;			// Ipv4
	serv_addr.sin_port = htons(PORT);	// 端口 
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 所有本地IP

	// 端口复用
	int opt = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	// 绑定socket
	Bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	// 同时只有20个建立连接
	Listen(listenfd, 20);

	maxfd = listenfd;					// 起初listenfd 即为最大文件描述符

	maxi = -1;							// 将来用作client[]的下标，初始值指向0个元素之前下标位置
	for (i = 0; i < FD_SETSIZE; ++i)	
		client[i] = -1;					// 用-1初始化client[]

	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);			// 构造select监控文件描述符

	while (1)
	{	
		rset = allset;					// 每次循环都从新设置select监控信号集
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);
		if (nready < 0)
			perr_exit("select error");

		// 说明有新的客户端链接请求
		if (FD_ISSET(listenfd, &rset))
		{
			clie_addr_len = sizeof(clie_addr);
			connfd = Accept(listenfd, (struct sockaddr *)&clie_addr, &clie_addr_len);
			printf("received from %s at port: %d\n", 
					inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, str, sizeof(str)),
					ntohs(clie_addr.sin_port));

			for (i = 0; i < FD_SETSIZE; ++i)
				if (client[i] < 0)		// 找client[]中没有使用的位置
				{
					client[i] = connfd;	// 保存accept返回的文件描述符到client[]里
					break;
				}

			if (i == FD_SETSIZE)		// 达到select能监控的文件个数上限1024
			{
				fputs("too many clients\n", stderr);
				exit(1);
			}

			FD_SET(connfd, &allset);	// 向监控文件描述符集合allset添加新的文件描述符connfd
			if (connfd > maxfd)
				maxfd = connfd;			// select第一个参数需要

			if (i > maxi)
				maxi = i;				// 保证maxi存的总是client[]最后一个元素下标

			if (--nready == 0)
				continue;
		}

		// 检测哪个clients有数据就绪
		for (i = 0; i <= maxi; ++i)
		{
			if ((sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &rset))
			{
				if ((n = Read(sockfd, buf, sizeof(buf))) == 0)
				{
					Close(sockfd);		// 当client关闭链接时，服务端也关闭对应链接
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}
				else if (n > 0) 
				{
					for (j = 0; j < n; j++)
						buf[j] = toupper(buf[j]);
					// sleep(10);
					Write(sockfd, buf, n);
				}
				if (--nready == 0)
					break;				// 跳出for, 但还在while中
			}
		}
	}

	Close(listenfd);

	return 0;
}
