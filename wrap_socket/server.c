// server.c -- tcp socket server 将客户端发送的字符串变成大写,封装错误函数
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>

#include "wrap.h"

#define PORT 5000		// 端口号

int main(int argc, char *argv[])
{
	int sfd, cfd;		// sfd服务器socket, cfd客户端socket
	char buf[BUFSIZ], clie_ip[BUFSIZ];	//  4k
	int len, i;

	struct sockaddr_in serv_addr, clie_addr;
	socklen_t clie_addr_len;

	sfd = Socket(AF_INET, SOCK_STREAM, 0);	// 使用tcp

	// 清零
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;			// Ipv4
	serv_addr.sin_port = htons(PORT);	// 端口 
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 所有本地IP

	// 绑定socket
	Bind(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	// 同时只有128个建立连接
	Listen(sfd, 2);

	printf("wait for client connect ...\n");

	clie_addr_len = sizeof(clie_addr_len);
	cfd = Accept(sfd, (struct sockaddr *)&clie_addr, &clie_addr_len);
	printf("cfd = ---- %d\n", cfd);

	printf("client IP: %s, client port: %d\n", 
			inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, clie_ip, sizeof(clie_ip)),
			ntohs(clie_addr.sin_port));

	while (1)
	{
		len = Read(cfd, buf,  sizeof(buf));
		Write(STDOUT_FILENO, buf, len);

		for (i = 0; i < len; ++i)
			buf[i] = toupper(buf[i]);
		write(cfd, buf, len);
	}

	close(sfd);
	close(cfd);

	return 0;
}
