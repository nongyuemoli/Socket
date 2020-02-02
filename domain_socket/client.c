// client.c -- 本地套客户端, 封装错误函数
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/un.h>
#include <stddef.h>

#include "wrap.h"

#define SERV_ADDR "serv.socket"
#define CLIE_ADDR "clie.socket"

int main(int argc, char *argv[])
{
		int cfd, len;
		struct sockaddr_un serv_addr, clie_addr;
		char buf[4096];

		cfd = Socket(AF_UNIX, SOCK_STREAM, 0);

		bzero(&clie_addr, sizeof(clie_addr));
		clie_addr.sun_family = AF_UNIX;
		strcpy(clie_addr.sun_path, CLIE_ADDR);
		
		len = offsetof(struct sockaddr_un, sun_path) + strlen(clie_addr.sun_path);	// clie_addr total len
		
		unlink(CLIE_ADDR);
		Bind(cfd, (struct sockaddr *)&clie_addr, len);	// 客户端也需要bind,不能依赖自动绑定
		
		bzero(&serv_addr, sizeof(serv_addr));
		serv_addr.sun_family = AF_UNIX;
		strcpy(serv_addr.sun_path, SERV_ADDR);

		len = offsetof(struct sockaddr_un, sun_path) + strlen(serv_addr.sun_path);	// serv_addr total len

		Connect(cfd, (struct sockaddr *)&serv_addr, len);

		while (fgets(buf, sizeof(buf), stdin) != NULL)
		{
				write(cfd, buf, strlen(buf));
				len = read(cfd, buf, sizeof(buf));
				write(STDOUT_FILENO, buf, len);
		}

		close(cfd);

		return 0;
}
