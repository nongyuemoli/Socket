// client.c -- socket客户端, 封装错误函数
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "wrap.h"

#define SERV_IP "116.62.144.96"
#define SERV_PORT 5000

int main(int argc, char * argv[])
{
		int sfd, len;
		struct sockaddr_in serv_addr;
		char buf[BUFSIZ];

		sfd = Socket(AF_INET, SOCK_STREAM, 0);

		bzero(&serv_addr, sizeof(serv_addr));
		serv_addr.sin_family = AF_INET;
		inet_pton(AF_INET, SERV_IP, &serv_addr.sin_addr.s_addr);
		serv_addr.sin_port = htons(SERV_PORT);

		connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

		while (1)
		{
				fgets(buf, sizeof(buf), stdin);
				int r = Write(sfd, buf, strlen(buf));
				printf("write r ===== %d\n", r);
				len = Read(sfd, buf, sizeof(buf));
				printf("read len ===== %d\n", len);
				Write(STDOUT_FILENO, buf, len);
		}

		close(sfd);

		return 0;
}
