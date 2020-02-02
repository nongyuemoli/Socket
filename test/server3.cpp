// socket服务端
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(void)
{

		// 创建套接字
		int servSock = socket(AF_INET, SOCK_STREAM, 0);

		// 绑定套接字
		sockaddr_in sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));	// 每个字节都用0填充
		sockAddr.sin_family = PF_INET;	// 使用ipv4地址
		sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	// 具体IP地址
		sockAddr.sin_port = htons(5000);	// 端口
		bind(servSock, (struct sockaddr *)&sockAddr, sizeof(sockAddr));

		// 进入监听状态
		if (listen(servSock, 5) != 0) { perror("listen"); close(servSock); return -1; }
		
		// 接收客户端的连接
		int clientfd;	// 客户端的socket
		int socklen = sizeof(struct sockaddr_in);	// struct sockaddr_in的大小
		struct sockaddr_in clientaddr;	// 客户端的地址信息
		
		// 于客户端通信
		char buffer[1024];
		while (1)
		{
				clientfd = accept(servSock, (struct sockaddr *)&clientaddr, (socklen_t*)&socklen);
				printf("客户端(%s)已连接。\n", inet_ntoa(clientaddr.sin_addr));
				int strLen = recv(clientfd, buffer, sizeof(buffer), 0);	// 接收客户端发来的数据
				send(clientfd, buffer, strLen, 0);	// 将数据返回

				memset(buffer, 0, sizeof(buffer));	// 重置缓冲区
		}

		// 关闭套接字
		close(clientfd);
		close(servSock);

		return 0;
}
