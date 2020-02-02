// Socket服务端
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(void)
{
		int listenfd = socket(AF_INET, SOCK_STREAM, 0);		// 创建服务端用于监听的socket

		// 把服务端用于通信的地址和端口绑定到socket
		struct sockaddr_in servaddr;	// 服务端地址信息的数据结构
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET; 	// 协议簇，只能是AF_INET
		servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	// 本机任何地址
		// servaddr.sin_addr.s_addr = inet_addr("192.168.43.230");
		servaddr.sin_port = htons(5000); // 绑定端口
		if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
		{
				perror("bind");
				close(listenfd);
				return -1;
		}

		// 把socket设置为监听模式
		if (listen(listenfd, 5) != 0) { perror("listen"); close(listenfd); return -1;}
		// 接受客户端的连接
		int clientfd;	// 客户端的socket
		int socklen = sizeof(struct sockaddr_in);	// struct sockaddr_in的大小
		struct sockaddr_in clientaddr;	// 客户端的地址信息
		clientfd = accept(listenfd, (struct sockaddr *)&clientaddr, (socklen_t*)&socklen);
		printf("客户端(%s)已连接。\n", inet_ntoa(clientaddr.sin_addr));

		// 与客户端通信，接受客户端发过来的报文后，回复ok
		char buffer[1024];
		while (1)
		{
				memset(buffer, 0, sizeof(buffer));
				if (recv(clientfd, buffer, sizeof(buffer), 0) <= 0) break;
				printf("接受：%s\n", buffer);

				strcpy(buffer, "ok");
				if (send(clientfd, buffer, strlen(buffer), 0) <= 0) break;
				printf("发送：%s\n", buffer);
		}

		close(listenfd); close(clientfd);	// 关闭socket
		
		return 0;
}
