#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <pthread.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

#define PORT 25565
#define MAX_CLIENTS 10

extern int errno;
pthread_mutex_t lock;

struct fdl {
	struct fdl * next;
    char * filename;
    int mode;
    int fd;
    int file_mode;
}arg;

struct sockets {
	int port;
	int client;
	int part;
	int clientSock;
	int fd;
	ssize_t nbytes;
	char * buf;
} sock;

struct sockets socks[10];

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
	pthread_mutex_lock(&lock);
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
			pthread_mutex_unlock(&lock);
			return 0x0;
		}
	}
	if((mode1 || mode0) && mode == 2){
		pthread_mutex_unlock(&lock);
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
		pthread_mutex_unlock(&lock);
		return &possible->fd;
	}
	pthread_mutex_unlock(&lock);
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

int is_regular_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
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
		if(count_mode == 0){
			continue;
		}
	}
	// receive flags(read only,write,wr)
	int count_flags = 0;
	while(count_flags < sizeof(int)){
		count_flags += recv(client, &flagBuf, sizeof(int), 0);
		if(count_flags == 0){
			continue;
		}
	}
	//path name
	int count_f = 0;
	while(count_f < 256){
		int tmp = 0;
		tmp = recv(client, recvBuf, 256, 0);
		count_f += tmp;
		if(tmp == 0){
			continue;
		}
	}
	//check for modes
	if(is_regular_file(recvBuf)){
		o = open(recvBuf,flagBuf);
		if(o != -1){
			bad = conflict(recvBuf,mode,flagBuf);
		}
		if(bad == 0){
			printf("Conflict\n");
			// permision denied fix
			error= EACCES;
			o = -1;
		} else if(bad){
			if(o == -1){
				error = errno;
			} else{
				o *= -10;
				*bad = o;
			}
		}
	}else{
		o = -1;
		error = EISDIR;
	}
	printf("Flag: %d\nMode: %d\nFilename: %s\n",flagBuf,mode,recvBuf);
	int retnB = send(client, &o, sizeof(o), 0);
	int retnE = send(client, &error, sizeof(error), 0);
	listfiles();
	return o;
}

// int getclients(int port){
// 	int sockfd;
// 	char ipstr[INET6_ADDRSTRLEN];
// 	// init server addr and client addr
// 	struct sockaddr_in serv_addr;
// 	// Attempt to create socket
// 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
// 	// If socket creation fails exit
// 	if(sockfd < 0){
// 		printf("ERROR: Could not open socket, please check connection!\n");
// 		return -1;
// 	}
// 	// Assign socket_addr varibles
// 	serv_addr.sin_family = AF_INET;
// 	serv_addr.sin_port = htons(port);
// 	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
// 	socklen_t addrLen = sizeof(serv_addr);
// 	// Attempt to bind to the socket.
// 	int bind_s = bind(sockfd, (struct sockaddr *) &serv_addr, addrLen);
// 	if(bind_s == -1){
// 		printf("Could not bind to socket. Please check open connections.\n");
// 		return -1;
// 	}
// 	int status = listen(sockfd, MAX_CLIENTS);
// 	printf("Listening\n");
// 		// We listen until we accept a connection.
// 		int client = accept(sockfd, (struct sockaddr *) &serv_addr, &addrLen);
// 		int * c = (int*)calloc(1,sizeof(int));
// 		*c = client;
// 		// Information about client
// 		inet_ntop(AF_INET, &serv_addr.sin_addr, ipstr, sizeof ipstr);
// 		printf("Connection established with %s\n",ipstr);
// 	return client;
// }

// void * mread(void * s){
// 	struct sockets * socket = (struct sockets *)s;
// 	int filedes = socket->fd;
// 	int client = socket->clientSock;
// 	int nbytes = socket->nbytes;
// 	char * readBuf = socket->buf;
// 	int error;
// 	int re;
// 	if(filedes % 10 == 0){
// 		filedes /= -10;
// 		re = read(filedes,readBuf,nbytes);
// 		printf("Read Bytes: %d\n",re);
// 	} else{
// 		re = -1;
// 	}
// 	if(re == -1){
// 		error = errno;
// 		if(error == 0){
// 			error = -1;
// 		}
// 	} else{
// 		error = 0;
// 	}
// 	// send error
// 	int retnE = send(client, &error, sizeof(error), 0);
// 	if(re == -1){
// 		free(readBuf);
// 		return 0;
// 	}
// 	// send bytes read
// 	int readB = send(client, &re, sizeof(re),0);
// 	// send data
// 	int retnB = send(client, readBuf, nbytes, 0);
// 	printf("Sent: %d\n",retnB);
// 	free(readBuf);
// 	return 0;
// }

// void mnetread(int client,int filedes,ssize_t nbytes){
// 	int i;
// 	int canbind;
// 	int ports[2];
// 	int clients[2];
// 	char * buff1;
// 	char * buff2;
// 	//mutex
// 	for(i = 0;i < 10;i++){
// 		if(canbind == 2){
// 			break;
// 		}
// 		if(socks[i].client == 0){
// 			socks[i].client = client;
// 			socks[i].part = canbind;
// 			socks[i].fd = filedes;
// 			canbind++;
// 			ports[canbind] = socks[i].port;
// 		}
// 	}
// 	if(canbind == 0){
// 		printf("CANNOT BIND ANY MORE SOCKETS!\n");
// 		return;
// 	}
// 	if(nbytes % 2 == 1 && canbind ==2){
// 		buff1 = calloc(1,(nbytes / 2)+1);
// 		buff2 = calloc(1,nbytes / 2);
// 		read(filedes,buff1,(nbytes / 2)+1);
// 		read(filedes,buff2,nbytes / 2);
// 		socks[0].buf = buff1;
// 		socks[1].buf = buff2;
// 	} else if(nbytes % 2 == 0 && canbind ==2){
// 		buff1 = calloc(1,nbytes / 2);
// 		buff2 = calloc(1,nbytes / 2);
// 		read(filedes,buff1,nbytes / 2);
// 		read(filedes,buff2,nbytes / 2);
// 		socks[0].buf = buff1;
// 		socks[1].buf = buff2;
// 	} else if(canbind == 1){
// 		buff1 = calloc(1,nbytes);
// 		read(filedes,buff1,nbytes / 2);
// 		socks[0].buf = buff1;
// 	}
// 	for(i = canbind;i>0;i--){
// 		clients[i] = getclients(ports[i]);
// 		if(clients[i] == -1){
// 			printf("COULD NOT BIND MULTI SOCKET\n");
// 			return;
// 		}
// 	}
// 	pthread_t tid[2];
// 	if(canbind == 1){
// 		pthread_create(&tid[0], NULL, mread, (void*)&socks[0]);
// 		pthread_join(tid[1],0);
// 	} else if(canbind == 2){
// 		pthread_create(&tid[0], NULL, mread, (void*)&socks[0]);
// 		pthread_create(&tid[1], NULL, mread, (void*)&socks[1]);
// 		pthread_join(tid[1],0);
// 	}
// }

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
		if(count_fdes == 0){
			continue;
		}
	}
	// receive # of bytes to read
	int count_nbyte = 0;
	while(count_nbyte < sizeof(ssize_t)){
		count_nbyte += recv(client, &nbytes, sizeof(ssize_t), 0);
		if(count_nbyte == 0){
			continue;
		}
	}
	// This is cancer
	// // if large bytes thread method
	// if(nbytes > 2048){
	// 	mnetread(client, filedes,nbytes);
	// 	return 0;
	// }
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
	// send error
	int retnE = send(client, &error, sizeof(error), 0);
	if(re == -1){
		free(readBuf);
		return -1;
	}
	// send bytes read
	int readB = send(client, &re, sizeof(re),0);
	// send data
	int retnB = send(client, readBuf, nbytes, 0);
	printf("Sent: %d\n",retnB);
	free(readBuf);
	return 0;
}

int netwrite(int client){
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
		if(count_fdes == 0){
			continue;
		}
	}
	// receive # of bytes to write
	int count_nbyte = 0;
	while(count_nbyte < sizeof(ssize_t)){
		count_nbyte += recv(client, &nbytes, sizeof(ssize_t), 0);
		if(count_nbyte == 0){
			continue;
		}
	}
	//receive data to write
	readBuf = calloc(1,nbytes);
	char * buf = readBuf;
	int buffBytes = 0;
	while(buffBytes < nbytes){
		int tmp = 0;
		tmp = recv(client, buf, nbytes, 0);
		if(tmp == 0){
			continue;
		}
		buffBytes += tmp;
		buf += tmp;
	}
	if(filedes % 10 == 0){
		filedes /= -10;
		printf("FD: %d\n",filedes);
		re = write(filedes,readBuf,nbytes);
		printf("Written Bytes: %d\n",re);
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
	// send error
	int retnE = send(client, &error, sizeof(error), 0);
	if(re == -1){
		free(readBuf);
		return -1;
	}
	// send bytes written
	int readB = send(client, &re, sizeof(re),0);
	free(readBuf);
	return o;
}

void deleteNode(int key){
	struct fdl * temp = of;
	struct fdl * prev = temp;
	pthread_mutex_lock(&lock);
	if(temp != NULL && temp->fd == key){
		of = temp->next;
		free(temp);
		pthread_mutex_unlock(&lock);
		return;
	}

	while(temp != NULL && temp->fd != key){
		prev = temp;
		temp = temp->next;
	}

	if(temp == NULL){
		pthread_mutex_unlock(&lock);
		return;
	}

	prev->next = temp->next;
	pthread_mutex_unlock(&lock);
	free(temp);
}

int netclose(int client){
	int filedes;
	int error;
	int re;
	int count_fdes = 0;
	// receive fd to close
	while(count_fdes < sizeof(int)){
		count_fdes += recv(client, &filedes, sizeof(int), 0);
		if(count_fdes == 0){
			continue;
		}
	}
	//close FD and delete node. prob need mutex
	if(filedes % 10 == 0){
		filedes /= -10;
		printf("FD: %d\n",filedes);
		re = close(filedes);
		if(re != -1){
			int fd = filedes * -10;
			deleteNode(fd);
		}
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
	listfiles();
	// send error
	int retnE = send(client, &error, sizeof(error), 0);
	return error;
}


void * begin(void * c){
	int client = *(int*)c;
	int t;
	int type = 0;
	while(type < sizeof(t)){
		type += recv(client, &t, sizeof(t), 0);
		if(type == 0){
			continue;
		}
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
				netwrite(client);
				break;
			case 4:
				netclose(client);
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
	signal(SIGPIPE, SIG_IGN);
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
	// lets setup the socket array
	int i = 0;
	int pp = PORT + 1;
	for(i = 0;i < 10;i++){
		socks[i].port = pp;
		pp++;
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