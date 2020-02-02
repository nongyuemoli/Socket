// server.c -- 使用本地套 domain server 将客户端发送的字符串变成大写,封装错误函数
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <strings.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <ctype.h>
#include <stddef.h>

#include "wrap.h"

#define SERV_ADDR "serv.socket"

int main(int argc, char *argv[])
{
		int lfd, cfd, len, size, i;
		struct sockaddr_un serv_addr, clie_addr;
		char buf[4096];

		lfd = Socket(AF_UNIX, SOCK_STREAM, 0);	// 本地套随便使用哪种协议

		// 清零
		bzero(&serv_addr, sizeof(serv_addr));
		serv_addr.sun_family = AF_UNIX;
		strcpy(serv_addr.sun_path, SERV_ADDR);

		len = offsetof(struct sockaddr_un, sun_path) + strlen(serv_addr.sun_path);	// serv_addr total len

		unlink(SERV_ADDR);						// 确保bind之前serv.socket文件不存在，bind会创建该文件
		Bind(lfd, (struct sockaddr *)&serv_addr, len);

		// 同时只有20个建立连接
		Listen(lfd, 20);

		printf("Accept----\n");

		while (1)
		{
				len = sizeof(clie_addr);
				cfd = Accept(lfd, (struct sockaddr *)&clie_addr, (socklen_t *)&len);

				len -= offsetof(struct sockaddr_un, sun_path);		// 得到文件名长度
				clie_addr.sun_path[len] = '\0';

				printf("client bind filename %s\n", clie_addr.sun_path);

				while ((size = read(cfd, buf,  sizeof(buf))) > 0) 
				{
					for (i = 0; i < size; ++i)
							buf[i] = toupper(buf[i]);
					write(cfd, buf, size);
				}
				close(cfd);
		}
		close(lfd);

		return 0;
}
