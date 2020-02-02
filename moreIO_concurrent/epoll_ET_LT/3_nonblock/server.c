// server.c -- epoll非阻塞io
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>

#define MAXLINE 10
#define PORT 5000

int main(int argc, char * argv[])
{
	struct sockaddr_in serv_addr, clie_addr;
	socklen_t clie_addr_len;
	int listenfd, connfd;
	int efd;
	char buf[MAXLINE], str[INET_ADDRSTRLEN];	// #define INET_ADDRSTRLEN 16

	listenfd = socket(AF_INET, SOCK_STREAM, 0);	// 使用tcp

	// 清零
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;			// Ipv4
	serv_addr.sin_port = htons(PORT);	// 端口 
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 所有本地IP

	// 绑定socket
	bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	// 同时只有128个建立连接
	listen(listenfd, 128);

	struct epoll_event event;
	struct epoll_event resevent[10];	// epoll_wait就绪返回event
	int res, len;

	efd = epoll_create(10);

	// event.events = EPOLLIN | EPOLLET;	// ET边沿触发
	event.events = EPOLLIN;			// LT水平触发（默认）

	printf("Accepting connections ...\n");

	clie_addr_len = sizeof(clie_addr);
	connfd = accept(listenfd, (struct sockaddr *)&clie_addr, &clie_addr_len);
	printf("client IP: %s, port: %d\n",
			inet_ntop(AF_INET, &clie_addr.sin_addr, str, sizeof(str)),
			ntohs(clie_addr.sin_port));

	event.data.fd = connfd;
	epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &event);

	while (1)
	{
		res = epoll_wait(efd, resevent, 10, -1);
		printf("res %d\n", res);
		if (resevent[0].data.fd == connfd)
		{
			len = read(connfd, buf, MAXLINE/2);
			write(STDOUT_FILENO, buf, len);
		}
	}

	return 0;
}
