// server.c -- 多进程socket服务
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <strings.h>
#include <sys/wait.h>

#include "wrap.h"

#define SERV_PORT 5000

void wait_child(int signo)
{
		while (waitpid(0, NULL, WNOHANG) > 0);

		return ;
}

int main(int argc, char *argv[])
{
		pid_t pid;
		int lfd, cfd;
		struct sockaddr_in serv_addr, clie_addr;
		socklen_t clie_addr_len;
		char buf[BUFSIZ], clie_ip[BUFSIZ];
		int n, i;

		lfd = Socket(AF_INET, SOCK_STREAM, 0);

		bzero(&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(SERV_PORT);
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		// inet_pton(AF_INET, "192.168.43.230", &serv_addr.sin_addr.s_addr);
		// 端口复用
		int opt = 1;
		setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
		Bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
		Listen(lfd, 128);

		while (1)
		{
				clie_addr_len = sizeof(clie_addr);
				cfd = Accept(lfd, (struct sockaddr *)&clie_addr, &clie_addr_len);
				printf("client IP: %s, port: %d\n",
								inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, clie_ip, sizeof(clie_ip)),
								ntohs(clie_addr.sin_port));

				pid = fork();
				if (pid < 0)
				{
						perror("fork error");
						exit(1);
				}
				else if (pid == 0)
				{
						close(lfd);
						break;
				}
				else
				{
						close(cfd);
						signal(SIGCHLD, wait_child);
				}
		}

		if (pid == 0)
		{
				while (1)
				{
						n = Read(cfd, buf, sizeof(buf));
						if (n == 0)		// client close
						{
								close(cfd);
								return 0;
						}
						else if (n == -1)
						{
								perror("read error");
								exit(1);
						}
						else
						{
								for (i = 0; i < n; ++i)
										buf[i] = toupper(buf[i]);
								write(cfd, buf, n);
								write(STDOUT_FILENO, buf, n);
						}
				}
		}

		return 0;
}
