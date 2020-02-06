// server.c -- 多线程socket服务
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <fcntl.h>

#include "wrap.h"

#define MAXLEN 8192
#define SERV_PORT 5000

struct s_info {
	struct sockaddr_in clie_addr;		// 定义结构体，将地址结构和cfd捆绑
	int cfd;
};

void * do_work(void * arg)
{
	int n, i;
	struct s_info * ts = (struct s_info*)arg;
	char buf[MAXLEN];
	char str[INET_ADDRSTRLEN];		// #define INET_ADDRSTRLEN 16 用"[ + d" 查看

	while (1)
	{
		n = Read(ts->cfd, buf, MAXLEN);
		if (n == 0)		// client close
		{
			printf("the client %d closed...\n", ts->cfd);
			return 0;
		}
		printf("received from %s at port %d\n",
				inet_ntop(AF_INET, &(*ts).clie_addr.sin_addr, str, sizeof(str)),
				ntohs((*ts).clie_addr.sin_port));

		for (i = 0; i < n; ++i)
			buf[i] = toupper(buf[i]);

		write(ts->cfd, buf, n);
		write(STDOUT_FILENO, buf, n);
	}

	Close(ts->cfd);

	return (void *)0;
}

int main(int argc, char *argv[])
{
	int lfd, cfd;
	struct sockaddr_in serv_addr, clie_addr;
	socklen_t clie_addr_len;
	pthread_t tid;
	struct s_info ts[256];			// 根据最大线程数创建结构体数组
	int i = 0;

	lfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	// inet_pton(AF_INET, "192.168.43.230", &serv_addr.sin_addr.s_addr);

	Bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	Listen(lfd, 128);

	printf("Accepting client connect ...\n");

	while (1)
	{
		clie_addr_len = sizeof(clie_addr);
		cfd = Accept(lfd, (struct sockaddr *)&clie_addr, &clie_addr_len);
		ts[i].clie_addr = clie_addr;
		ts[i].cfd = cfd;

		// 达到线程最大数时，pthread_create出错处理，增加服务器稳定性
		pthread_create(&tid, NULL, do_work, (void*)&ts[i]);
		pthread_detach(tid);		// 子线程分离，防止僵尸线程产生
	}

	return 0;
}
