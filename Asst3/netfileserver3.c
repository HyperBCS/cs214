#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <pthread.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/stat.h>
#include <signal.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

#define PORT 25565
#define MAX_CLIENTS 10
#define MAX_SOCKETS 10
#define MAX_STREAMS_PER_CLIENT 4
#define TIMEOUT 2

extern int errno;
extern int h_errno;
pthread_mutex_t lock;
pthread_mutex_t queuetex;
pthread_mutex_t socktex;
pthread_mutex_t conf;
pthread_mutex_t conf2;

struct fdl {
	struct fdl * next;
    char * filename;
    int mode;
    int fd;
    int file_mode;
}arg;

struct clientQ{
	struct clientQ * next;
	int client;
	int mode;
	int readtype;
	struct timeval start;
	int filedes[2];
} cQ;

struct fileQ{
	char * filename;
	struct fileQ * next;
	struct clientQ * queue;

} files;

struct sockets {
	int port;
	int client;
	int part;
	int clientSock;
	int fd;
	int read;
	ssize_t nbytes;
	char * buf;
	struct sockaddr_in serv_addr;
	socklen_t addrLen;

} sock;

struct sockets socks[10];

// cant make a linked list
struct fdl * of;
struct fileQ * fQ;


void error(char *msg){
  perror(msg);
  exit(1);
}

struct fileQ * findFile(char * filename){
	struct fileQ * ff = fQ;
	struct fileQ * prev = ff;
	int i;
	pthread_mutex_lock(&queuetex);
	for(i = 0;ff != 0;ff = ff->next){
		if(!strcmp(ff->filename,filename)){
			pthread_mutex_unlock(&queuetex);
			return ff;
		}
		prev = ff;
	}
	prev->next = calloc(1,sizeof(files));
	prev = prev->next;
	prev->filename = filename;
	pthread_mutex_unlock(&queuetex);
	return prev;
}

int queueFile(struct fileQ * ff, int client, int mode, int readtype){
	printf("QUEUEING\n");
	pthread_mutex_lock(&queuetex);
	struct clientQ * cc = ff->queue;
	struct clientQ * prev = ff->queue;
	int fd = 0;
	if(cc == 0){
		ff->queue = calloc(1,sizeof(cQ));
		cc = ff->queue;
		cc->client = client;
		gettimeofday(&cc->start, NULL);
		cc->mode = mode;
		cc->readtype = readtype;
		int status = pipe(cc->filedes);
		if(status < 0){
			printf("ERROR PIPING???\n");
		}
		fd = (cc->filedes)[0];
		pthread_mutex_unlock(&queuetex);
		return fd;
	}
	int i = 0;
	for(i = 0;cc != 0;cc = cc->next){
		prev = cc;
	}
	prev->next = calloc(1,sizeof(cQ));
	prev = prev->next;
	prev->client = client;
	gettimeofday(&prev->start, NULL);
	prev->mode = mode;
	prev->readtype = readtype;
	int status = pipe(prev->filedes);
	if(status < 0){
		printf("ERROR PIPING???\n");
	}
	fd = (prev->filedes)[0];
	pthread_mutex_unlock(&queuetex);
	return fd;
}



//mutex lock this
int conflict(char * filename, int mode, int readtype, int client, int rdMode){
	int i;
	int mode0;
	struct fdl * fds = of;
	struct fdl * possible = 0;
	struct fileQ * ff = 0;
	int mode1 = 0;
	int fd = 0;
	int write1 = 0;
	pthread_mutex_lock(&lock);
	if(fQ == 0){
		printf("Adding file to list\n");
		fQ = calloc(1,sizeof(files));
		fQ->filename = filename;
	}
	ff = findFile(filename);

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
			if(rdMode == 0){
				fd = queueFile(ff,client,mode,readtype);
				pthread_mutex_unlock(&lock);
				return fd;
			}
			pthread_mutex_unlock(&lock);
			return -1;
		}
	}
	if((mode1 || mode0) && mode == 2){
		if(rdMode == 0){
			fd = queueFile(ff,client,mode,readtype);
			pthread_mutex_unlock(&lock);
			return fd;
		}
		pthread_mutex_unlock(&lock);
		return -1;
	}
	fds = possible;
	if(rdMode == 0){
		if((mode1 == 0) || (mode1 == 1 && write1 == 1 && readtype == O_RDONLY) || (mode1 == 1 && write1 == 0) || fds == 0){
			errno = 0;
			int ff = open(filename,readtype) * -10;
			if(errno != 0 || ff == -1){
				return -1;
			}
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
			possible->fd = ff;
			return possible->fd;
		} else{
			fd = queueFile(ff,client,mode,readtype);
			pthread_mutex_unlock(&lock);
			return fd;
		}
	} else{
		if((mode1 == 0) || (mode1 == 1 && write1 == 1 && readtype == O_RDONLY) || (mode1 == 1 && write1 == 0) || fds == 0){
			pthread_mutex_unlock(&lock);
			return 0;
		} else{
			pthread_mutex_unlock(&lock);
			return -1;
		}
	}
	pthread_mutex_unlock(&lock);
	return -1;
	
}

void listfiles(){
	struct fdl * fds = of;
	printf("-------FILES-------\n");
	for(;fds != NULL;fds = fds->next){
		printf("FILE: %s\nTRANSFER MODE: %d\nFD: %d\nFILE MODE: %d\n\n",fds->filename,fds->mode,fds->fd,fds->file_mode);
	}
	printf("-------------------\n");
}

int is_regular_file(const char *path){
    struct stat path_stat;
    int err;
    err = stat(path, &path_stat);
    if(err == -1){
    	return err;
    } else{
    	if(S_ISDIR(path_stat.st_mode)) {
        	errno = EISDIR;
        	return 1;
	    } else{
	    	return 0;
	   	}
    }
}

int netopen(int client){
	char * recvBuf = calloc(1,256);
	int flagBuf;
	int mode;
	int error;
	int i;
	int bad;
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
	if(!is_regular_file(recvBuf)){
		errno = 0;
		pthread_mutex_lock(&conf);
		bad = conflict(recvBuf,mode,flagBuf, client,0);
		pthread_mutex_unlock(&conf);
		if(errno != 0){
			error = errno;
			printf("[%d] %s\n",errno,strerror(errno));
			o = -1;
		}
		if(bad > 0 && errno == 0){
			while(1){
				int pp = 0;
				printf("Conflict\n");
				printf("PIPE: %d\n",bad);
				read(bad,&pp,sizeof(pp));
				if(pp == -1){
					printf("TIMEOUT\n");
					o = -1;
					errno = EWOULDBLOCK;
					send(client, &o, sizeof(o), 0);
					send(client, &errno, sizeof(errno), 0);
					return errno;
				}
				printf("PP: %d\n",pp);
				pthread_mutex_lock(&conf2);
				bad = conflict(recvBuf,mode,flagBuf, client,0);
				printf("BAD %d\n",bad);
				pthread_mutex_unlock(&conf2);
				if(bad <= 0){
					o = bad;
					break;
				}
			}
			// permision denied fix
		} else{
			if(o == -1){
				error = errno;
			} else{
				o = bad;
			}
		}
	}else{
		o = -1;
		error = errno;
	}
	printf("Flag: %d\nMode: %d\nFilename: %s\n",flagBuf,mode,recvBuf);
	int retnB = send(client, &o, sizeof(o), 0);
	int retnE = send(client, &error, sizeof(error), 0);
	listfiles();
	return o;
}

int getclients(struct sockets * s, int port){
	int sockfd;
	// init server addr and client addr
	struct sockaddr_in serv_addr;
	// Attempt to create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// If socket creation fails exit
	if(sockfd < 0){
		printf("ERROR: Could not open socket, please check connection!\n");
		return -1;
	}
	// Assign socket_addr varibles
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	socklen_t addrLen = sizeof(serv_addr);
	// Attempt to bind to the socket.
	int bind_s = bind(sockfd, (struct sockaddr *) &serv_addr, addrLen);
	if(bind_s == -1){
		printf("Could not bind to socket. Please check open connections.\n");
		return -1;
	}
	s->serv_addr = serv_addr;
	s->addrLen = addrLen;
	return sockfd;
}

void * mread(void * s){
	struct sockets * socket = (struct sockets *)s;
	int filedes = socket->fd;
	int sockfd = socket->client;
	struct sockaddr_in serv_addr = socket->serv_addr;
	socklen_t addrLen = socket->addrLen;
	char ipstr[INET6_ADDRSTRLEN];
	int status = listen(sockfd, MAX_CLIENTS);
		// We listen until we accept a connection.
		int client = accept(sockfd, (struct sockaddr *) &serv_addr, &addrLen);
		// Information about client
		inet_ntop(AF_INET, &serv_addr.sin_addr, ipstr, sizeof(ipstr));


	if(client < 0){
		printf("[%d] %s\n",h_errno,strerror(h_errno));
		printf("MULTI ERROR CLIENT PANIC\n");
		return 0;
	}
	int nbytes = socket->nbytes;
	char * readBuf = socket->buf;
	int error = 0;
	int re = socket->read;
	// send error
	int retnE = send(client, &error, sizeof(error), 0);
	if(re == -1){
		free(readBuf);
		return 0;
	}
	// send bytes read
	int readB = send(client, &re, sizeof(re),0);
	// send data
	int retnB = send(client, readBuf, nbytes, 0);
	printf("Sent: %d\n",retnB);
	free(readBuf);
	return 0;
}

void mnetread(int client,int filedes,ssize_t nbytes){
	int i = 0;
	int canbind = 0;
	int ports[MAX_SOCKETS];
	pthread_t tid[MAX_SOCKETS];
	bzero(ports,sizeof(ports));
	int clients[MAX_SOCKETS];
	struct sockets * socker[MAX_SOCKETS];
	pthread_mutex_lock(&socktex);
	printf("PORTS: ");
	for(i = 0;i < MAX_SOCKETS;i++){
		if(canbind == MAX_STREAMS_PER_CLIENT){
			break;
		}
		if(socks[i].clientSock == 0){
			socks[i].clientSock = client;
			socks[i].part = canbind;
			socks[i].fd = filedes;
			socker[canbind] = &socks[i];
			ports[canbind] = socks[i].port;
			canbind++;
			printf("%d ",socks[i].port);
		}
	}
	printf("\n");
	pthread_mutex_unlock(&socktex);
	if(canbind == 0){
		ports[0] = -1*ECONNRESET;
		send(client, &ports, sizeof(ports),0);
		printf("CANNOT BIND ANY MORE SOCKETS!\n");
		return;
	}
	errno = 0;
	int first = 1;
	for(i = 0;i < canbind;i++){
		int nb = nbytes / canbind;
		if(first){
			nb += nbytes % canbind;
			first = 0;
		}
		char * buff = calloc(1,nb);
		socker[i]->read = read(filedes,buff,nb);
		socker[i]->buf = buff;
		socker[i]->nbytes = nb;
	}
	// remember to send nbyte
	if(errno != 0){
		printf("[%d] %s\n",errno,strerror(errno));
		pthread_mutex_lock(&socktex);
		for(i = 0;i<canbind;i++){
			socker[i]->clientSock = 0;
		}
		pthread_mutex_unlock(&socktex);
		ports[0] = errno * -1;
		send(client, &ports, sizeof(ports),0);
		return;
	}
	// send ports to client
	int sendP = send(client, &ports, sizeof(ports),0);
	for(i = 0;i < canbind;i++){
		pthread_create(&tid[i], NULL, mread, (void*)socker[i]);
	}
	for(i = 0;i < canbind;i++){
		pthread_join(tid[i],0);
	}
	pthread_mutex_lock(&socktex);
	for(i = 0;i<canbind;i++){
		socker[i]->clientSock = 0;
	}
	pthread_mutex_unlock(&socktex);
	return;
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
	// if large bytes thread method
	if(nbytes > 2048){
		if(filedes % 10 == 0){
			filedes /= -10;
		} else{
			re = -1;
			printf("DO LATER\n");
		}
		mnetread(client, filedes,nbytes);
		printf("Client %d done reading\n",client);
		return 0;
	}
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

void * mwrite(void * s){
	struct sockets * socket = (struct sockets *)s;
	int filedes = socket->fd;
	int sockfd = socket->client;
	struct sockaddr_in serv_addr = socket->serv_addr;
	socklen_t addrLen = socket->addrLen;
	char ipstr[INET6_ADDRSTRLEN];
	int status = listen(sockfd, MAX_CLIENTS);
		// We listen until we accept a connection.
		int client = accept(sockfd, (struct sockaddr *) &serv_addr, &addrLen);
		// Information about client
		inet_ntop(AF_INET, &serv_addr.sin_addr, ipstr, sizeof(ipstr));


	if(client < 0){
		printf("PANIC CLIENT MULTI ERR\n");
		return 0;
	}
	errno = 0;
	int nbytes = socket->nbytes;
	char * readBuf = socket->buf;
	char * buf = readBuf;
	int buffBytes = 0;
	while(buffBytes < nbytes){
		int tmp = 0;
		tmp = recv(client, buf, nbytes, 0);
		buffBytes += tmp;
		buf += tmp;
		if(tmp == 0 && buffBytes < nbytes){
			break;
		}
	}
	// send error
	int retnE = send(client, &errno, sizeof(errno), 0);
	// remember to free later
	close(client);
	return 0;
}

void mnetwrite(int client,int filedes,ssize_t nbytes){
	int i = 0;
	int canbind = 0;
	int ports[MAX_SOCKETS];
	bzero(ports,sizeof(ports));
	int clients[MAX_SOCKETS];
	pthread_t tid[MAX_SOCKETS];
	struct sockets * socker[MAX_SOCKETS];
	pthread_mutex_lock(&socktex);
	printf("PORTS: ");
	for(i = 0;i < MAX_SOCKETS;i++){
		if(canbind == MAX_STREAMS_PER_CLIENT){
			break;
		}
		if(socks[i].clientSock == 0){
			socks[i].clientSock = client;
			socks[i].part = canbind;
			socks[i].fd = filedes;
			socker[canbind] = &socks[i];
			ports[canbind] = socks[i].port;
			canbind++;
			printf("%d ",socks[i].port);
		}
	}
	printf("\n");
	pthread_mutex_unlock(&socktex);
	if(canbind == 0){
		ports[0] = -1*ECONNRESET;
		send(client, &ports, sizeof(ports),0);
		printf("CANNOT BIND ANY MORE SOCKETS!\n");
		return;
	}
	errno = 0;

	int first = 1;
	for(i = 0;i < canbind;i++){
		int nb = nbytes / canbind;
		if(first){
			nb += nbytes % canbind;
			first = 0;
		}
		char * buff = calloc(1,nb);
		socker[i]->buf = buff;
		socker[i]->nbytes = nb;
	}
	// remember to send nbyte
	if(errno != 0){
		pthread_mutex_lock(&socktex);
		for(i = 0;i<canbind;i++){
			socker[i]->clientSock = 0;
		}
		pthread_mutex_unlock(&socktex);
		ports[0] = errno * -1;
		send(client, &ports, sizeof(ports),0);
		return;
	}
	// send ports to client
	int sendP = send(client, &ports, sizeof(ports),0);

	for(i = 0;i < canbind;i++){
		pthread_create(&tid[i], NULL, mwrite, (void*)socker[i]);
	}
	for(i = 0;i < canbind;i++){
		pthread_join(tid[i],0);
	}
	errno = 0;
	// do some negative errno stuff
	int re = 0;
	int written = 0;
	for(i = 0;i < canbind;i++){
		re = write(filedes, socker[i]->buf,socker[i]->nbytes);
		if(re == -1){
			re = -1*errno;
			written = re;
			break;
		}
		written += re;
	}
	// send bytes written
	int readB = send(client, &written, sizeof(written),0);
	pthread_mutex_lock(&socktex);
	for(i = 0;i<canbind;i++){
		close(socker[i]->clientSock);
		socker[i]->clientSock = 0;
	}
	pthread_mutex_unlock(&socktex);
	return;
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

		// This is cancer
	// if large bytes thread method
	if(nbytes > 2048){
		if(filedes % 10 == 0){
			filedes /= -10;
		} else{
			re = -1;
			printf("DO LATER\n");
		}
		mnetwrite(client, filedes,nbytes);
		printf("Client %d done writing\n",client);
		return 0;
	}

	//receive data to write
	readBuf = calloc(1,nbytes);
	char * buf = readBuf;
	int buffBytes = 0;
	while(buffBytes < nbytes){
		int tmp = 0;
		tmp = recv(client, buf, nbytes, 0);
		buffBytes += tmp;
		buf += tmp;
		if(tmp == 0 && buffBytes < nbytes){
			return 0;
		}
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
	return;
}

int netclose(int client){
	int filedes;
	char * filename = 0;
	int error = 0;
	int re;

	int count_fdes = 0;
	// receive fd to close
	while(count_fdes < sizeof(int)){
		count_fdes += recv(client, &filedes, sizeof(int), 0);
		if(count_fdes == 0){
			continue;
		}
	}
	struct fdl * fds = of;
	for(;fds != NULL;fds = fds->next){
		if(fds->fd == filedes){
			filename = fds->filename;
		}
	}
	if(filename == 0){
		printf("No file\n");
		error = EBADF;
		send(client, &error, sizeof(error), 0);
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
		struct fileQ * ff = findFile(filename);
		pthread_mutex_lock(&queuetex);
		struct clientQ * cc = ff->queue;
		struct clientQ * ccP = ff->queue;
		if(cc == 0){
			printf("NO QUEUE\n");
		}
		pthread_mutex_unlock(&queuetex);
		for(;cc != NULL;cc = cc->next){
			pthread_mutex_lock(&conf);
			int confl = conflict(filename,cc->mode,cc->readtype,0,1);
			pthread_mutex_unlock(&conf);
			pthread_mutex_lock(&queuetex);
			if(confl == 0){
				filedes = (cc->filedes)[1];
				printf("WRITING TO %d\n",filedes);
				write(filedes,&confl,sizeof(confl));
				if(ff->queue == cc){
					ff->queue = cc->next;
				} else{
					ccP->next = cc->next;
				}
				free(cc);
			}
			pthread_mutex_unlock(&queuetex);
			ccP = cc;
		}
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

//mutex
void * bigBrother(){
	while(1){
		pthread_mutex_lock(&queuetex);
		int death = -1;
		struct fileQ * ff = fQ;
		if(ff != 0){
			struct clientQ * cc = ff->queue;
			struct clientQ * ccP = ff->queue;
			for(;ff != 0;ff = ff->next){
				cc = ff->queue;
				if(cc == 0){
					continue;
				}
				for(;cc != 0;cc = cc->next){
					struct timeval start,end;
					gettimeofday(&end, NULL);
					start = cc->start;
					if(end.tv_sec - start.tv_sec > TIMEOUT){
						int filedes = (cc->filedes)[1];
						printf("BB: Removing an item from the queue\n");
						write(filedes,&death,sizeof(death));
						if(cc == ff->queue){
							ff->queue = cc->next;
						} else{
							ccP->next = cc->next;
						}
							free(cc);
					}
					ccP = cc;
				}
			}
		}
		pthread_mutex_unlock(&queuetex);
		sleep(3);
	}
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
	pthread_t monitor;
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
	for(i = 0;i < MAX_SOCKETS;i++){
		socks[i].port = pp;
		socks[i].client = getclients(&socks[i],pp);
		socks[i].clientSock = 0;
		pp++;
	}
	// start monitor thread
	pthread_create(&monitor, NULL, bigBrother, NULL);
	// After we successfully bind start listening. This is blocking.
	int status = listen(sockfd, MAX_CLIENTS);
	printf("Listening on port %d\n",PORT);
		while(1){
		// We listen until we accept a connection.
		int client = accept(sockfd, (struct sockaddr *) &serv_addr, &addrLen);
		int * c = (int*)calloc(1,sizeof(int));
		*c = client;
		// Information about client
		inet_ntop(AF_INET, &serv_addr.sin_addr, ipstr, sizeof(ipstr));
		printf("Connection established with %s\n",ipstr);
		pthread_create(&tid, NULL, begin, (void*)c);
		
	}
}