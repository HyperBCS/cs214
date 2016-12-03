#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 25565

void error(char *msg){
  perror(msg);
  exit(1);
}

// TO SEND A FILE WE SHOULD SEND THE LENGTH OF FILE FIRST, THEN THE ACTUAL FILE. THIS WAY THE SERVER KNOWS HOW MUCH DATA TO EXPECT.

int main(int argc, char ** argv){
	// if(argc != 2){
	// 	printf("ERROR: Invalid number of arguments\n");
	// 	exit(1);
	// }
	// init ports and socket variables
	int sockfd;
	char recvBuf[256];
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
	int status = listen(sockfd, 10);
	printf("Listening on port %d\n",PORT);
		while(1){
		// We listen until we accept a connection.
		int client = accept(sockfd, (struct sockaddr *) &serv_addr, &addrLen);
		// Information about client
		inet_ntop(AF_INET, &serv_addr.sin_addr, ipstr, sizeof ipstr);
		printf("Connection established with %s\n",ipstr);
		int count = recv(client, recvBuf, 256, 0);
		printf("Received String: %s\n",recvBuf);
		close(client);
	}



}