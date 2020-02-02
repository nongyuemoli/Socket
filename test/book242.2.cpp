// socket服务端,点对点的聊天功能
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int initserver(int port);

int main(void)
{
		int listenfd = initserver(5000);	// 服务端用于监听的socket
		if (listenfd <= 0) { printf("服务端初始化失败，程序退出。\n"); return -1;}
		// 接收客户端的连接
		int clientfd;	// 客户端的socket
		int socklen = sizeof(struct sockaddr_in);	// struct sockaddr_in的大小
		struct sockaddr_in clientaddr;	// 客户端的地址信息
		
		// 于客户端通信
		char bufferSend[1024];
		char bufferSecv[1024];
		while (1)
		{
				clientfd = accept(servSock, (struct sockaddr *)&clientaddr, (socklen_t*)&socklen);
			    recv(clientfd, bufferSecv, sizeof(bufferSecv), 0);	// 接收客户端发来的数据
				printf("%s:%s\n", inet_ntoa(clientaddr.sin_addr), bufferSecv);
				printf("Input a string:");
				fgets(bufferSend, sizeof(bufferSend), stdin);
				send(clientfd, bufferSend, sizeof(bufferSend), 0);	// 将数据返回

				memset(bufferSend, 0, sizeof(bufferSend));	// 重置缓冲区
				memset(bufferSecv, 0, sizeof(bufferSecv));	// 重置缓冲区

		}

		// 关闭套接字
		close(clientfd);
		close(servSock);

		return 0;
}

// 初始化服务端的socket，port为通信端口
int initserver(int)
{
		int listenfd = socket(AF_INET, SOCK_STREAM, 0);

		// 绑定套接字
		struct sockaddr_in sockAddr;
		memset(&sockAddr, 0, sizeof(sockAddr));	// 每个字节都用0填充
		sockAddr.sin_family = PF_INET;	// 使用ipv4地址
		sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	// 具体IP地址
		sockAddr.sin_port = htons(5000);	// 端口
		if (bind(servSock, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) != 0)
		{ perror("bind"); close(servSock); return -1; }

		// 进入监听状态
		if (listen(servSock, 5) != 0) { perror("listen"); close(servSock); return -1; }

		return listenfd;
}
