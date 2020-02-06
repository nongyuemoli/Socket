// server.c -- tcp socket server 将客户端发送的字符串变成大写
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <strings.h>

#define IP "127.0.0.1"	// IP地址
#define PORT 5000		// 端口号

int main(int argc, char *argv[])
{
	int lfd, cfd;		// lfd服务器socket, cfd客户端socket
	struct sockaddr_in serv_addr, clie_addr;
	socklen_t clie_addr_len;
	char buf[BUFSIZ], clie_ip[BUFSIZ];	//  4k
	int n, i, ret;

	lfd = socket(AF_INET, SOCK_STREAM, 0);	// 使用tcp
	if (lfd == -1)
	{
		perror("socket error");
		exit(1);
	}

	// 清零
	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;			// Ipv4
	serv_addr.sin_port = htons(PORT);	// 端口 
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 所有本地IP

	// 绑定socket
	ret = bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (ret == -1)
	{
		perror("bind error");
		exit(1);
	}

	// 同时只有128个建立连接
	ret = listen(lfd, 128);
	if (ret == -1)
	{
		perror("listen error");
		exit(1);
	}

	clie_addr_len = sizeof(clie_addr);
	cfd = accept(lfd, (struct sockaddr *)&clie_addr, &clie_addr_len);

	printf("client IP: %s, client port: %d\n", 
			inet_ntop(AF_INET, &clie_addr.sin_addr.s_addr, clie_ip, sizeof(clie_ip)),
			ntohs(clie_addr.sin_port));

	while (1)
	{
		n = read(cfd, buf,  sizeof(buf));
		for (i = 0; i < n; ++i)
			buf[i] = toupper(buf[i]);
		write(cfd, buf, n);
	}

	close(lfd);
	close(cfd);

	return 0;
}
