// client.c -- 使用UDP实现广播
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define SERVER_PORT 8000
#define CLIENT_PORT 9000
#define MAXLINE 4096

int main(int argc, char *argv[])
{
	struct sockaddr_in local_addr;
	int confd;
	ssize_t len;
	char buf[MAXLINE];

	// 创建一个socket
	confd = socket(AF_INET, SOCK_DGRAM, 0);

	// 初始化本地端地址
	bzero(&local_addr, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &local_addr.sin_addr.s_addr);
	local_addr.sin_port = htons(CLIENT_PORT);

	int ret = bind(confd, (struct sockaddr *)&local_addr, sizeof(local_addr));	// 显式绑定不能省略
	if (ret == 0)
		printf("..bind ok...\n");

	while (1)
	{
		len = recvfrom(confd, buf, sizeof(buf), 0, NULL, 0);
		write(STDOUT_FILENO, buf, len);
	}
	close(confd);

	return 0;
}
