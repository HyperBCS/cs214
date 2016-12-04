#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

#define PORT 25565
#define MAX_CLIENTS 10

extern int errno;

struct sockets {
    char * filename;
    int mode;
    int file_mode;
}arg;
struct sockets of[10];

void error(char *msg){
  perror(msg);
  exit(1);
}

int conflict(char * filename, int mode, int readtype){
	
}

int netopen(int client){
	char recvBuf[256];
	int flagBuf;
	int mode;
	int error;
	int i;
	int count_mode = recv(client, &mode, sizeof(int), 0);
	int count_flags = recv(client, &flagBuf, sizeof(int), 0);
	int count_f = recv(client, recvBuf, 256, 0);
	//check for modes
	conflict(recvBuf,mode,flagBuf);
	int o = open(recvBuf,flagBuf);
	if(o == -1){
		error = errno;
	}
	printf("Flag: %d\nMode: %d\nFilename: %s\n",flagBuf,mode,recvBuf);
	int retnB = send(client, &o, sizeof(o), 0);
	int retnE = send(client, &error, sizeof(error), 0);
	return o;
}


void * begin(void * c){
	int client = *(int*)c;
	int t;
	int type = recv(client, &t, sizeof(t), 0);
	printf("Request type: %d\n",t);
	switch(t){
		case 1:
			netopen(client);
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			break;
	}
	close(client);
	free(c);
	return 0;
}


int main(int argc, char ** argv){
	// if(argc != 2){
	// 	printf("ERROR: Invalid number of arguments\n");
	// 	exit(1);
	// }
	// init ports and socket variables
	
	pthread_t tid;
	int sockfd;
	char ipstr[INET6_ADDRSTRLEN];
	// init server addr and client addr
	struct sockaddr_in serv_addr;
	// Attempt to create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// If socket creation fails exit
	if(sockfd < 0){
		printf("ERROR: Could not open socket, please check connection!\n");
		exit(1);
	}
	// Assign socket_addr varibles
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	socklen_t addrLen = sizeof(serv_addr);
	// Attempt to bind to the socket.
	int bind_s = bind(sockfd, (struct sockaddr *) &serv_addr, addrLen);
	if(bind_s == -1){
		printf("Could not bind to socket. Please check open connections.\n");
		exit(0);
	}
	// After we successfully bind start listening. This is blocking.
	int status = listen(sockfd, MAX_CLIENTS);
	printf("Listening on port %d\n",PORT);
		while(1){
		// We listen until we accept a connection.
		int client = accept(sockfd, (struct sockaddr *) &serv_addr, &addrLen);
		int * c = (int*)calloc(1,sizeof(int));
		*c = client;
		// Information about client
		inet_ntop(AF_INET, &serv_addr.sin_addr, ipstr, sizeof ipstr);
		printf("Connection established with %s\n",ipstr);
		pthread_create(&tid, NULL, begin, (void*)c);
		
	}



}