// server.c -- 使用UDP实现广播
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>

#define SERVER_PORT 8000
#define CLIENT_PORT 9000	// 重要
#define MAXLINE 1500

#define BROADCAST_IP "192.168.0.104"

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in serv_addr, clie_addr;
	char buf[MAXLINE];

	// 构造用于UDP通信的套接字
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 本地任意IP INADDR_ANY = 0
	serv_addr.sin_port = htons(SERVER_PORT);

	bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	int flag = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &flag, sizeof(flag));

	// 构造client地址 IP+端口 192.168.0.255+9000
	bzero(&clie_addr, sizeof(clie_addr));
	clie_addr.sin_family = AF_INET;
	inet_pton(AF_INET, BROADCAST_IP, &clie_addr.sin_addr.s_addr);
	clie_addr.sin_port = htons(CLIENT_PORT);

	int i = 0;
	while (1)
	{
		sprintf(buf, "Drink %d glasses of water\n", i++);
		// fgets(buf, sizeof(buf), stdin);
		sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&clie_addr, sizeof(clie_addr));
		sleep(1);
	}
	close(sockfd);

	return 0;
}
