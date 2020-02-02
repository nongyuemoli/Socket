#include <stdio.h>
#include <string.h>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main(){
    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(5000);  //端口
   
    //读取服务器传回的数据
    char bufferSend[40];
	char bufferRecv[40];
  
	while (1)
	{
			//创建套接字
			int sockfd = socket(AF_INET, SOCK_STREAM, 0);
			connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
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
