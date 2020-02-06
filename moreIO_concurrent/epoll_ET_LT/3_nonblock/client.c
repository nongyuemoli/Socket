// client.c -- 非阻塞io
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MAXLINE 10
#define SERV_PORT 5000

int main(int argc, char *argv[])
{
	int sfd, i;
	struct sockaddr_in serv_addr;
	char buf[MAXLINE];
	char ch = 'a';

	sfd = socket(AF_INET, SOCK_STREAM, 0);

	if (argc != 2)
	{
		perror("Input server ip address");
		exit(-1);
	}

	bzero(&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);
	serv_addr.sin_port = htons(SERV_PORT);

	connect(sfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	while (1)
	{
		// aaaa\n
		for (i = 0; i < MAXLINE/2; i++)
			buf[i] = ch;
		buf[i-1] = '\n';
		ch++;
		// bbbb\n
		for (; i < MAXLINE; i++)
			buf[i] = ch;
		buf[i-1] = '\n';
		ch++;
		// aaaa\nbbbb\n
		write(sfd, buf, sizeof(buf));
		sleep(5);
	}
	close(sfd);

	return 0;
}
