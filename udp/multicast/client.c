// client.c -- 使用UDP实现组播
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <net/if.h>

#define SERVER_PORT 8000
#define CLIENT_PORT 9000

#define GROUP "239.0.0.2"

int main(int argc, char *argv[])
{
	struct sockaddr_in local_addr;
	int confd;
	ssize_t len;
	char buf[BUFSIZ];

	struct ip_mreqn group;	// 组播结构体

	// 创建一个socket
	confd = socket(AF_INET, SOCK_DGRAM, 0);

	// 初始化本地端地址
	bzero(&local_addr, sizeof(local_addr));
	local_addr.sin_family = AF_INET;
	inet_pton(AF_INET, "0.0.0.0", &local_addr.sin_addr.s_addr);
	local_addr.sin_port = htons(CLIENT_PORT);

	bind(confd, (struct sockaddr *)&local_addr, sizeof(local_addr));	// 显式绑定不能省略

	inet_pton(AF_INET, GROUP, &group.imr_multiaddr);	// 设置组地址
	inet_pton(AF_INET, "0.0.0.0", &group.imr_address);	// 本地任意IP
	group.imr_ifindex = if_nametoindex("eth0");			// 给出网卡名，转换为对应编号：eth0 --> 编号 命令:ip ad

	setsockopt(confd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &group, sizeof(group));	// 将本客户端加入组播

	while (1)
	{
		len = recvfrom(confd, buf, sizeof(buf), 0, NULL, 0);
		write(STDOUT_FILENO, buf, len);
	}
	close(confd);

	return 0;
}
