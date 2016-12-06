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

struct fdl {
	struct fdl * next;
    char * filename;
    int mode;
    int fd;
    int file_mode;
}arg;

// cant make a linked list
struct fdl * of;

void error(char *msg){
  perror(msg);
  exit(1);
}

//mutex lock this
int * conflict(char * filename, int mode, int readtype){
	int i;
	int mode0;
	struct fdl * fds = of;
	struct fdl * possible = 0;
	int mode1 = 0;
	int write1 = 0;
	for(;fds != 0;fds = fds->next){
		if(fds->next == 0){
			possible = fds;
		}
		if(!strcmp(fds->filename,filename) && fds->mode == 0){
			write1 = 1;
			mode0 = 1;
			continue;
		}
		if(!strcmp(fds->filename,filename) && fds->mode == 1 && fds->file_mode == O_RDONLY){
			mode1 = 1;
			continue;
		}
		if(!strcmp(fds->filename,filename) && fds->mode == 1 && fds->file_mode != O_RDONLY){
			write1 = 1;
			mode1 = 1;
			continue;
		}

		if(!strcmp(fds->filename,filename) && fds->mode == 2){
			return 0x0;
		}
	}
	if((mode1 || mode0) && mode == 2){
		return 0x0;
	}
	fds = possible;
	if((mode1 == 0) || (mode1 == 1 && write1 == 1 && readtype == O_RDONLY) || (mode1 == 1 && write1 == 0) || fds == 0){
		if(fds == 0){
			of = calloc(1,sizeof(arg));
			possible = of;
		} else{
			possible->next = calloc(1,sizeof(arg));
			possible = possible->next;
		}
		possible->filename = filename;
		possible->mode = mode;
		possible->file_mode = readtype;
		return &possible->fd;
	}
	return 0x0;
	
}

void listfiles(){
	struct fdl * fds = of;
	printf("-------FILES-------\n");
	for(;fds != NULL;fds = fds->next){
		printf("FILE: %s\nMODE: %d\nFD: %d\nFILE MODE: %d\n\n",fds->filename,fds->mode,fds->fd,fds->file_mode);
	}
	printf("-------------------\n");
}

int netopen(int client){
	char * recvBuf = calloc(1,256);
	int flagBuf;
	int mode;
	int error;
	int omode;
	int i;
	int * bad;
	int o;
	// receive connection mode (exclusive, transaction, etc)
	int count_mode = 0;
	while(count_mode < sizeof(int)){
		count_mode += recv(client, &mode, sizeof(int), 0);
	}
	// receive flags(read only,write,wr)
	int count_flags = 0;
	while(count_flags < sizeof(int)){
		count_flags += recv(client, &flagBuf, sizeof(int), 0);
	}
	//path name
	int count_f = 0;
	while(count_f < 256){
	count_f += recv(client, recvBuf, 256, 0);
	}
	//check for modes
	o = open(recvBuf,flagBuf);
	if(o != -1){
		bad = conflict(recvBuf,mode,flagBuf);
	}
	if(bad == 0){
		printf("Conflict\n");
		// permision denied fix
		error= 1;
		o = -1;
	} else if(bad){
		if(o == -1){
			error = errno;
		} else{
			o *= -10;
			*bad = o;
		}
	}
	printf("Flag: %d\nMode: %d\nFilename: %s\n",flagBuf,mode,recvBuf);
	int retnB = send(client, &o, sizeof(o), 0);
	int retnE = send(client, &error, sizeof(error), 0);
	listfiles();
	return o;
}

int netread(int client){
	char * readBuf;
	int filedes;
	int error;
	ssize_t nbytes;
	int o;
	int re;
	// receive file descriptor
	int count_fdes = 0;
	while(count_fdes < sizeof(int)){
		count_fdes += recv(client, &filedes, sizeof(int), 0);
	}
	// receive # of bytes to read
	int count_nbyte = 0;
	while(count_nbyte < sizeof(ssize_t)){
		count_nbyte += recv(client, &nbytes, sizeof(ssize_t), 0);
	}
	//printf("FD: %d\nNBYTE: %zd\n",filedes,nbytes);
	readBuf = calloc(1,nbytes);
	if(filedes % 10 == 0){
		filedes /= -10;
		re = read(filedes,readBuf,nbytes);
		printf("Read Bytes: %d\n",re);
	} else{
		re = -1;
	}
	if(re == -1){
		error = errno;
		if(error == 0){
			error = -1;
		}
	} else{
		error = 0;
	}
	int retnE = send(client, &error, sizeof(error), 0);
	if(re == -1){
		free(readBuf);
		return -1;
	}
	int retnB = send(client, readBuf, nbytes, 0);
	printf("Sent: %d\n",retnB);
	free(readBuf);
	return o;
}


void * begin(void * c){
	int client = *(int*)c;
	int t;
	int type = 0;
	while(type < sizeof(t)){
		type += recv(client, &t, sizeof(t), 0);
	}
		printf("Request type: %d\n",t);
		switch(t){
			case 1:
				netopen(client);
				break;
			case 2:
				netread(client);
				break;
			case 3:
				break;
			case 4:
				break;
		}
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