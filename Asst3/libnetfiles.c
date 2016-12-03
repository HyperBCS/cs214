#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#define PORT 25565

void error(char *msg){
  perror(msg);
  exit(1);
}

int main(int argc, char ** argv){
	if(argc != 3){
		printf("Incorrect arguments\n");
		exit(1);
	}
	int sockfd;
	char * msg = argv[2];
	char * hostname = argv[1];
	char ip[100];
	struct hostent *server;
	// init server addr and client addr
	struct sockaddr_in serv_addr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("ERROR: Could not open socket, please check connection!\n");
		exit(0);
	}
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	if(gethostbyname(hostname) == NULL){
		printf("Host does not exist\n");
		exit(1);
	}
	server = gethostbyname(hostname);
	bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
	socklen_t addrLen = sizeof(serv_addr);
	inet_ntop(AF_INET, &serv_addr.sin_addr, ip, sizeof ip);
	printf("Connecting to %s\n",ip);
	int status = connect(sockfd, (struct sockaddr *) &serv_addr, addrLen);
	if(status < 0){
		printf("Could not connect to server.\n");
		exit(1);
	}
	int count = send(sockfd, msg, strlen(msg), 0);
	printf("Bytes Sent: %d\n",count);
	close(sockfd);
}