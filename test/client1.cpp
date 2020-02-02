#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
int main(){
    //创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    //向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  //具体的IP地址
    serv_addr.sin_port = htons(1234);  //端口
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
   
		char strbuffer[1024];

		// 与服务端通讯，发送一个报文后等待回复，然后再发送下一个报文
		for (int ii = 0; ii < 5; ii++)
		{
				memset(strbuffer, 0, sizeof(strbuffer));
				if (send(sock, strbuffer, strlen(strbuffer), 0) <= 0) break;
				printf("发送：%s\n", strbuffer);

				memset(strbuffer, 0, sizeof(strbuffer));
				if (recv(sock, strbuffer, sizeof(strbuffer), 0) <= 0) break;
				printf("接受：%s\n", strbuffer);
		}
   
    //关闭套接字
    close(sock);
    return 0;
}
