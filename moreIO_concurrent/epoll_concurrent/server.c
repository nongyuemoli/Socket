// server.c -- epoll tcp socket server 将客户端发送的字符串变成大写,封装错误函数
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <ctype.h>

#include "wrap.h"

#define MAXLINE 8192 
#define PORT 5000		// 端口号
#define OPEN_MAX 5000

int main(int argc, char * argv[])
{
	int i, listenfd, connfd, sockfd;
	int n, num = 0; 
	ssize_t nready, efd, res;
	char buf[MAXLINE], str[INET_ADDRSTRLEN];	// #define INET_ADDRSTRLEN 16
	socklen_t clie_addr_len;

	struct sockaddr_in serv_addr, clie_addr;
	struct epoll_event tep, ep[OPEN_MAX];	// tep: epoll_ctl参数 ep[]: epoll_wait参数

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

	efd = epoll_create(OPEN_MAX);		// 创建epoll模型，efd指向红黑树根节点
	if (efd == -1)
		perr_exit("epoll_create error");

	tep.events = EPOLLIN; tep.data.fd = listenfd;			// 指定lfd的监听时间为“读”
	res = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &tep);	// 将lfd及对应的结构体设置到树上，efd可找到该树
	if (res == -1)
		perr_exit("epoll_ctl error");

	for ( ; ; )
	{
		nready = epoll_wait(efd, ep, OPEN_MAX, -1);	// 阻塞监听是否有客户端链接请求

		if (nready == -1)
			perr_exit("epoll_wait error");

		for (i = 0; i < nready; i++)
		{
			if (!(ep[i].events & EPOLLIN))	// 如果不是“读”事件，继续循环
				continue;

			if (ep[i].data.fd == listenfd)	// 判断满足事件的fd是不是lfd
			{
				clie_addr_len = sizeof(clie_addr);
				connfd = Accept(listenfd, (struct sockaddr *)&clie_addr, &clie_addr_len);	// 接受链接

				printf("received from %s at port: %d\n", 
						inet_ntop(AF_INET, &clie_addr.sin_addr, str, sizeof(str)),
						ntohs(clie_addr.sin_port));
				printf("cfd %d---client %d\n", connfd, ++num);

				tep.events = EPOLLIN; tep.data.fd = connfd;
				res = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &tep);
				if (res == -1)
					perr_exit("epoll_ctl error");
			}
			else							// 不是lfd
			{
				sockfd = ep[i].data.fd;
				n = Read(sockfd, buf, MAXLINE);

				if (n == 0)
				{
					res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
					if (res == -1)
						perr_exit("epoll_ctl error");
					Close(sockfd);
					printf("client[%d] close connection\n", sockfd);
				}
				else if (n < 0)
				{
					perror("read n < 0 error: ");
					res = epoll_ctl(efd, EPOLL_CTL_DEL, sockfd, NULL);
					Close(sockfd);
				}
				else
				{
					for (i = 0; i < n; i++)
						buf[i] = toupper(buf[i]);

					Write(STDOUT_FILENO, buf, n);
					Write(sockfd, buf, n);
				}
			}
		}

	}

	return 0;
}
