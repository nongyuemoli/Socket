// client.c -- socket客户端
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define SERV_PORT 5000

int main(int argc, char *argv[])
{
	int cfd;
	struct sockaddr_in serv_addr;
	// socklen_t serv_addr_len;
	char buf[BUFSIZ];
	int n;

	cfd = socket(AF_INET, SOCK_STREAM, 0);

	if (argc != 2)
	{
		perror("Input server ip address");
		exit(-1);
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(SERV_PORT);
	inet_pton(AF_INET, argv[1], &serv_addr.sin_addr.s_addr);

	connect(cfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	while (1)
	{
		fgets(buf, sizeof(buf), stdin);
		write(cfd, buf, strlen(buf));
		n = read(cfd, buf, sizeof(buf));
		write(STDOUT_FILENO, buf, n);
	}

	close(cfd);

	return 0;
}
