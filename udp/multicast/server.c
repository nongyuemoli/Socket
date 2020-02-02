// server.c -- 使用UDP实现组播
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>

#define SERVER_PORT 8000
#define CLIENT_PORT 9000	// 重要
#define MAXLINE 1500

#define GROUP "239.0.0..2"

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in serv_addr, clie_addr;
	char buf[MAXLINE] = "nongyue\n";
	struct ip_mreqn group;

	// 构造用于UDP通信的套接字
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 本地任意IP INADDR_ANY = 0
	serv_addr.sin_port = htons(SERVER_PORT);

	bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	inet_pton(AF_INET, GROUP, &group.imr_multiaddr);	// 设置组地址
	inet_pton(AF_INET, "0.0.0.0", &group.imr_address);	// 本地任意IP
	group.imr_ifindex = if_nametoindex("eth0");			// 给出网卡名，转换为对应编号：eth0 --> 编号 命令:ip ad

	setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_IF, &group, sizeof(group));	// 组播权限

	// 构造client地址 IPv4 IP+端口 239.0.0.2+9000
	bzero(&clie_addr, sizeof(clie_addr));
	clie_addr.sin_family = AF_INET;
	inet_pton(AF_INET, GROUP, &clie_addr.sin_addr.s_addr);
	clie_addr.sin_port = htons(CLIENT_PORT);

	int i = 0;
	while (1)
	{
		sprintf(buf, "nongyue %d\n", i++);
		// fgets(buf, sizeof(buf), stdin);
		sendto(sockfd, buf, strlen(buf), 0, (struct sockaddr *)&clie_addr, sizeof(clie_addr));
		sleep(1);
	}
	close(sockfd);

	return 0;
}
