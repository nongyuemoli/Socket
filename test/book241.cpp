// socket通讯客户端
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main(void)
{
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);	// 创建客户端的socket

		struct hostent* h;	// ip地址信息的数据结构
		if ( (h = gethostbyname("192.168.43.230")) == 0)
		{ perror("gethostbyname"); close(sockfd); return -1; }
		// 把服务端的地址和端口转换为数据结构
		struct sockaddr_in servaddr;	
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(5000);
		memcpy(&servaddr.sin_addr, h->h_addr, h->h_length);

		// 向服务器发起连接请求
		if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
		{ perror("connect"); close(sockfd); return -1; }

		char strbuffer[1024];

		// 与服务端通讯，发送一个报文后等待回复，然后再发送下一个报文
		for (int ii = 0; ii < 5; ii++)
		{
				memset(strbuffer, 0, sizeof(strbuffer));
				sprintf(strbuffer, "这是第%s个报文。", ii);
				if (send(sockfd, strbuffer, strlen(strbuffer), 0) <= 0) break;
				printf("发送：%s\n", strbuffer);

				memset(strbuffer, 0, sizeof(strbuffer));
				if (recv(sockfd, strbuffer, sizeof(strbuffer), 0) <= 0) break;
				printf("接受：%s\n", strbuffer);
		}

		close(sockfd);

		return 0;
}
