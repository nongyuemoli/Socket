// socket客户端封装成函数
#include <stdio.h>
#include <string.h>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int connecttoserver(const char *serverip, const int port);

int main(){
	int sockfd = connecttoserver("127.0.0.1", 5000);
	if (sockfd <= 0) { printf("连接服务器失败，程序退出。\n"); return -1; }

    //读取服务器传回的数据
    char bufferSend[1024];
	char bufferRecv[1024];
  
	while (1)
	{
			//创建套接字
			sockaddr_in servaddr;
			// 向服务器发起连接请求
			if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0)
			{ perror("connect"); close(sockfd); return -1; }
			printf("Input a string:");
			fgets(bufferSend, 40, stdin);
			send(sockfd, bufferSend, sizeof(bufferSend), 0);

			// 接收服务器传回的数据
			recv(sockfd, bufferRecv, sizeof(bufferRecv), 0);
			// 输出接收到的数据
			printf("Message form server:%s\n", bufferRecv);

			memset(bufferSend, 0, sizeof(bufferSend));
			memset(bufferRecv, 0, sizeof(bufferRecv));
			
    		//关闭套接字
		    close(sockfd);
	}
    return 0;
}

// TCP客户端连服务端的函数， serverip-服务端ip, port通信端口
int connecttoserver(const char *serverip, const int port)
{
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);	// 创建客户端的socket
		sockaddr_in servaddr;
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(port);


		return sockfd;
}
