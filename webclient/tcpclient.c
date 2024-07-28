#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>

#define SERVER_PORT 80

#define MAXLINE 4096
#define SA struct sockaddr

void err_n_die(const char *fmt, ...);

int main(int argc, char **argv) {
	int sockfd, n;	
	int sendbytes;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE];
	char recvline[MAXLINE];

	if (argc != 2)
		err_n_die("Usage: %s <server address>", argv[0]);

	//criando o socket, AF_INET eh Address Family Internet, stream eh pra stream :p, 0 tcp
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
		err_n_die("Error while creating the socket");

	//setando o valor de address pra zero
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);

	//converte o ip pra bin
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		err_n_die("inet_pton error for %s ", argv[1]);

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		err_n_die("Connection failed");

	sprintf(sendline, "GET / HTTP/1.1\r\n\r\n");
	sendbytes = strlen(sendline);

	//mandando a request
	if (write(sockfd, sendline, sendbytes) != sendbytes)
		err_n_die("Write error");

	while ( (n = read(sockfd, recvline, MAXLINE-1)) > 0 ) {
		printf("%s", recvline);
		memset(recvline, 0, MAXLINE);
	}
	if (n < 0)
		err_n_die("Read error");

	exit(0);
}

void err_n_die(const char *fmt, ...) {
	int errno_save;
	va_list ap;

	// Como qualquer coisa pode escrever em errno salvo aqui, pra garantir ser salvo o certo
	errno_save = errno;

	va_start(ap, fmt);
	vfprintf(stdout, fmt, ap);
	fprintf(stdout, "\n");
	fflush(stdout);

	if (errno_save != 0) {
		fprintf(stdout, "(errno = %d) : %s\n", errno_save,
					strerror(errno_save));
		fprintf(stdout, "\n");
		fflush(stdout);
	}
	va_end(ap);

	exit(1);
}
