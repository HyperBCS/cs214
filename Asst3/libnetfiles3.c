#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include "libnetfiles.h"
#include <fcntl.h>

#define PORT 25565
#define MAX_SOCKS 10

struct sockaddr_in serv_addr;

struct readM {
	int port;
	int read;
	int error;
	ssize_t nbytes;
	char * buf;
} readerM;

extern int errno;
extern int h_errno;

int mode = -1;
int sockfd;
socklen_t addrLen;
char ip[100];
char * hname;

void error(char *msg){
  perror(msg);
  exit(1);
}

int netserverinit(char * hostname, int filemode){
	mode = filemode;
	hname = hostname;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	// remember to set errno
	if(gethostbyname(hostname) == NULL){
		h_errno = HOST_NOT_FOUND;
		printf("HOST NOT FOUND\n");
		mode = -1;
		return -1;
	}
	struct hostent *server;
	server = gethostbyname(hostname);
	bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
	return 0;
}

int makeSock(int port){
	struct sockaddr_in serv_addr2;
	socklen_t addrLen2;
	serv_addr2.sin_family = AF_INET;
	serv_addr2.sin_port = htons(port);
	// remember to set errno
	struct hostent *server;
	server = gethostbyname(hname);
	bcopy((char *)server->h_addr, 
         (char *)&serv_addr2.sin_addr.s_addr,
         server->h_length);
	int sockf = socket(AF_INET, SOCK_STREAM, 0);
	if(sockf < 0){
		printf("[%d] %s\n",errno,strerror(errno));
		exit(0);
	}
	addrLen = sizeof(serv_addr2);
	inet_ntop(AF_INET, &serv_addr2.sin_addr, ip, sizeof ip);
	int status = connect(sockf, (struct sockaddr *) &serv_addr2, addrLen);
	if(status < 0){
		printf("[%d] %s\n",errno,strerror(errno));
		exit(1);
	}
	return sockf;
}



int netopen(const char * pathname,int flags){
	if(flags < 0 || flags > 2){
		h_errno = INVALID_FILE_MODE;
		printf("Invalid flags.\n");
		return -1;
	}
	if(mode == -1){
		h_errno = HOST_NOT_FOUND;
		return -1;
	}
	char filename[256];
	int error;
	int err;
	int t = 1;
	strcpy(filename,pathname);
	// init server addr and client addr
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[%d] %s\n",errno,strerror(errno));
		exit(0);
	}
	addrLen = sizeof(serv_addr);
	inet_ntop(AF_INET, &serv_addr.sin_addr, ip, sizeof ip);
	int status = connect(sockfd, (struct sockaddr *) &serv_addr, addrLen);
	if(status < 0){
		printf("[%d] %s\n",errno,strerror(errno));
		exit(1);
	}
	// send type
	int type = send(sockfd, &t, sizeof(type), 0);
	// send mode
	int count_mode = send(sockfd, &mode, sizeof(mode), 0);
	// send flag
	int count_flags = send(sockfd, &flags, sizeof(flags), 0);
	// send path
	int countF = send(sockfd, filename, 256, 0);
	int retnB = 0;
	while(retnB < sizeof(error)){
		retnB += recv(sockfd, &error, sizeof(error), 0);
		if(retnB == 0){
			printf("NO DATA\n");
			close(sockfd);
			return -1;
		}
	}
	int retnE = 0;
	while(retnE < sizeof(err)){
		retnE += recv(sockfd, &err, sizeof(err), 0);
		if(retnE == 0){
			printf("NO DATA\n");
			close(sockfd);
			return -1;
		}
	}
	if(error == -1){
		errno = err;
		printf("[%d] %s\n",error,strerror(errno));
	}
	close(sockfd);
	return error;
}

void * multiread(void * st){
	struct readM * str = (struct readM *) st;
	int port = str->port;
	int client = makeSock(port);
	int nbyte = str->nbytes;
	char * buf = str->buf;
	int err;
	int recvM;

	//receive error first
	int retnE = 0;
	while(retnE < sizeof(err)){
		retnE += recv(client, &err, sizeof(err), 0);
		if(retnE == 0){
			str->error = ECONNRESET;
			close(sockfd);
			return 0;
		}
	}
	if(retnE != sizeof(int)){
		printf("Could not receive error!\n");
		return 0;
	}
	if(err != 0){
		errno = err;
		printf("[%d] %s\n",err,strerror(errno));
		close(client);
		return 0;
	}
	// received byte number
	int recvB = 0;
	while(recvB < sizeof(recvB)){
		recvB += recv(client, &recvM, sizeof(recvM), 0);
		if(recvB == 0){
			errno = ECONNRESET;
			close(client);
			return 0;
		}
	}
	str->read = recvM;
	int retnB = 0;
	errno = 0;
	while(retnB < nbyte){
		char c;
		int tmp;
		errno = 0;
		tmp = recv(client, buf, nbyte, 0);
		if(tmp == 0 && retnB < nbyte){
			str->error = ECONNRESET;
			close(client);
			return 0;
		} else if(tmp == -1){
			str->error = errno;
			close(client);
			return 0;
		}
		buf += tmp;
		retnB += tmp;
	}
	printf("Received: %d/%zd\n",retnB,nbyte);
	if(err == -1){
		recvB = -1;
	}
	close(client);
	return 0;
}

ssize_t netread(int fildes, void *buf, size_t nbyte){
	signal(SIGPIPE, SIG_IGN);
	// remember to set errno
	if(mode == -1){
		h_errno = HOST_NOT_FOUND;
		return -1;
	}
	// remember to set errno
	if(fildes >=0 || fildes % 10 != 0 || buf == 0){
		errno = EBADF;
		return -1;
	}
	int err;
	int recvM;
	int ports[MAX_SOCKS];
	pthread_t tid[MAX_SOCKS];
	struct readM readSock[MAX_SOCKS];
	int t = 2;
	// init server addr and client addr
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[%d] %s\n",errno,strerror(errno));
		exit(0);
	}
	addrLen = sizeof(serv_addr);
	inet_ntop(AF_INET, &serv_addr.sin_addr, ip, sizeof ip);
	int status = connect(sockfd, (struct sockaddr *) &serv_addr, addrLen);
	if(status < 0){
		printf("[%d] %s\n",errno,strerror(errno));
		exit(1);
	}
	// send type
	int type = send(sockfd, &t, sizeof(type), 0);
	// send fdes
	int count_fdes = send(sockfd, &fildes, sizeof(fildes), 0);
	//send nbytes
	int count_nbyte = send(sockfd, &nbyte, sizeof(nbyte), 0);
	//receive ports first if size > 2048
	int recvP = 0;
	int tmp = 0;
	if(nbyte > 2048){
		while(recvP < sizeof(ports)){
			tmp = recv(sockfd, &ports+tmp, sizeof(ports), 0);
			recvP += tmp;
			if(recvP == 0){
				errno = ECONNRESET;
				close(sockfd);
				return -1;
			}
		}
		int i = 0;
		int count = 0;
		printf("PORTS: ");
		for(i = 0;i < MAX_SOCKS;i++){
			if(ports[i]> 0){
				printf("%d ",ports[i]);
				readSock[i].port = ports[i];
				count++;
			} else if (ports[i] < 0){
				errno = ports[i]*-1;
				printf("[%d] %s\n",errno,strerror(errno));
				return -1;
			}
		}
		printf("\n");
	int first = 1;
	for(i = 0;i < count;i++){
		int nb = nbyte / count;
		if(first){
			nb += nbyte % count;
			first = 0;
		}
		readSock[i].error = 0;
		readSock[i].buf = calloc(1,nb);
		readSock[i].nbytes = nb;
	}
		// make sock
		for(i = 0;i < count;i++){
				pthread_create(&tid[i], NULL, multiread, (void*)&readSock[i]);
		}
		for(i = 0;i<count;i++){
			if(tid[i] != 0){
				pthread_join(tid[i],0);
			}
		}
		int read = 0;
		char * tmpbuf = buf;
		for(i = 0;i<count;i++){
			if(tid[i] != 0){
				if(readSock[i].error != 0){
					errno = readSock[i].error;
					return -1;
				}
				read += readSock[i].read;
				memcpy(tmpbuf,readSock[i].buf,readSock[i].nbytes);
				tmpbuf += readSock[i].nbytes;
			}
		}
		return read;
	}
	//receive error first
	int retnE = 0;
	while(retnE < sizeof(err)){
		retnE += recv(sockfd, &err, sizeof(err), 0);
		if(retnE == 0){
			errno = ECONNRESET;
			close(sockfd);
			return -1;
		}
	}
	if(retnE != sizeof(int)){
		printf("Could not receive error!\n");
		return -1;
	}
	if(err != 0){
		errno = err;
		printf("[%d] %s\n",err,strerror(errno));
		close(sockfd);
		return -1;
	}
	// received byte number
	int recvB = 0;
	while(recvB < sizeof(recvB)){
		recvB += recv(sockfd, &recvM, sizeof(recvM), 0);
		if(recvB == 0){
			errno = ECONNRESET;
			close(sockfd);
			return -1;
		}
	}
	int retnB = 0;
	errno = 0;
	while(retnB < nbyte){
		char c;
		int tmp;
		errno = 0;
		tmp = recv(sockfd, buf, nbyte, 0);
		if(tmp == 0 && retnB < nbyte){
			errno = ECONNRESET;
			close(sockfd);
			return -1;
		} else if(tmp == -1){
			close(sockfd);
			return -1;
		}
		buf += tmp;
		retnB += tmp;
		printf("Received: %d/%zd\n",retnB,nbyte);
	}
	if(err == -1){
		recvM = -1;
	}
	close(sockfd);
	return recvM;
}

void * multiwrite(void * st){
	struct readM * str = (struct readM *) st;
	int port = str->port;
	int client = makeSock(port);
	int nbyte = str->nbytes;
	char * buf = str->buf;
	int err;
	int recvM;
	// send data
	errno = 0;
	int sent_data = send(client, buf, nbyte, 0);
	if(errno == SIGPIPE || errno != 0){
		if(errno == SIGPIPE){
			str->error = ECONNRESET;
			return 0;
		} else{
			str->error = errno;
			return 0;
		}
	}
	//receive error first
	int retnE = 0;
	while(retnE < sizeof(err)){
		retnE += recv(client, &err, sizeof(err), 0);
		if(retnE == 0){
			errno = ECONNRESET;
			close(client);
			return 0;
		}
	}
	if(retnE != sizeof(int)){
		printf("Could not receive error!\n");
		return 0;
	}
	if(err != 0){
		errno = err;
		printf("[%d] %s\n",err,strerror(errno));
		close(client);
		return 0;
	}
	close(client);
	return 0;
}

ssize_t netwrite(int fildes, const void *buf, size_t nbyte){
	signal(SIGPIPE, SIG_IGN);
	// remember to set errno
	if(mode == -1){
		h_errno = HOST_NOT_FOUND;
		return -1;
	}
	// remember to set errno
	if(fildes >=0 || fildes % 10 != 0 || buf == 0){
		errno = EBADF;
		return -1;
	}
	int err;
	int recvM;
	int recvP = 0;
	int tmp = 0;
	int ports[MAX_SOCKS];
	struct readM readSock[MAX_SOCKS];
	pthread_t tid[MAX_SOCKS];
	int t = 3;
	// init server addr and client addr
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[%d] %s\n",errno,strerror(errno));
		exit(0);
	}
	addrLen = sizeof(serv_addr);
	inet_ntop(AF_INET, &serv_addr.sin_addr, ip, sizeof ip);
	int status = connect(sockfd, (struct sockaddr *) &serv_addr, addrLen);
	if(status < 0){
		printf("[%d] %s\n",errno,strerror(errno));
		exit(1);
	}
	// send type
	int type = send(sockfd, &t, sizeof(type), 0);
	// send fdes
	int count_fdes = send(sockfd, &fildes, sizeof(fildes), 0);
	//send nbytes
	int count_nbyte = send(sockfd, &nbyte, sizeof(nbyte), 0);

	if(nbyte > 2048){
		//receive ports
		while(recvP < sizeof(ports)){
			tmp = recv(sockfd, &ports+recvP, sizeof(ports), 0);
			recvP += tmp;
			if(recvP == 0){
				errno = ECONNRESET;
				close(sockfd);
				return -1;
			}
		}
		int i = 0;
		int count = 0;
		printf("PORTS: ");
		for(i = 0;i < MAX_SOCKS;i++){
			if(ports[i]> 0){
				printf("%d ",ports[i]);
				readSock[i].port = ports[i];
				count++;
			}else if (ports[i] < 0){
				errno = ports[i]*-1;
				printf("[%d] %s\n",errno,strerror(errno));
				return -1;
			}
		}
		printf("\n");
	char * buffer = (char*)buf;
	int first = 1;
	for(i = 0;i < count;i++){
		int nb = nbyte / count;
		if(first){
			nb += nbyte % count;
			first = 0;
		}
		readSock[i].error = 0;
		readSock[i].buf = buffer;
		readSock[i].nbytes = nb;
		buffer += nb;
	}
		// make sock
		for(i = 0;i < count;i++){
				pthread_create(&tid[i], NULL, multiwrite, (void*)&readSock[i]);
		}
		for(i = 0;i<count;i++){
			if(tid[i] != 0){
				pthread_join(tid[i],0);
			}
		}
		for(i = 0;i < count;i++){
			if(readSock[i].error != 0){
				errno = readSock[i].error;
				return -1;
			}
		}
		//receive bytes written
		int recvB = 0;
		while(recvB < sizeof(recvB)){
			recvB += recv(sockfd, &recvM, sizeof(recvM), 0);
			if(recvB == 0){
				errno = ECONNRESET;
				close(sockfd);
				return -1;
			}
		}
		if(recvM < 0){
			errno = -1*recvM;
			printf("[%d] %s\n",errno,strerror(errno));
			return -1;
		}
		return recvM;
	}


	// send data
	errno = 0;
	int sent_data = send(sockfd, buf, nbyte, 0);
	if(errno == SIGPIPE){
		errno = ECONNRESET;
	} else if(sent_data == -1){
		return -1;
	}
	//receive error first
	int retnE = 0;
	while(retnE < sizeof(err)){
		retnE += recv(sockfd, &err, sizeof(err), 0);
		if(retnE == 0){
			errno = ECONNRESET;
			close(sockfd);
			return -1;
		}
	}
	if(retnE != sizeof(int)){
		printf("Could not receive error!\n");
		return -1;
	}
	if(err != 0){
		errno = err;
		printf("[%d] %s\n",err,strerror(errno));
		close(sockfd);
		return -1;
	}
	// received byte number
	int recvB = 0;
	while(recvB < sizeof(recvB)){
		recvB += recv(sockfd, &recvM, sizeof(recvM), 0);
		if(recvB == 0){
			errno = ECONNRESET;
			close(sockfd);
			return -1;
		}
	}
	if(err == -1){
		recvM = -1;
	}
	close(sockfd);
	return recvM;
}

int netclose(int fildes){
	if(mode == -1){
		h_errno = HOST_NOT_FOUND;
		return -1;
	}
	// remember to set errno
	if(fildes >=0 || fildes % 10 != 0){
		errno = EBADF;
		return -1;
	}
	int err = 0;
	int recvM;
	int t = 4;
	// init server addr and client addr
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("[%d] %s\n",errno,strerror(errno));
		exit(0);
	}
	addrLen = sizeof(serv_addr);
	inet_ntop(AF_INET, &serv_addr.sin_addr, ip, sizeof ip);
	int status = connect(sockfd, (struct sockaddr *) &serv_addr, addrLen);
	if(status < 0){
		printf("[%d] %s\n",errno,strerror(errno));
		exit(1);
	}
	// send type
	int type = send(sockfd, &t, sizeof(type), 0);
	// send fdes
	int count_fdes = send(sockfd, &fildes, sizeof(fildes), 0);
	//receive error first
	int retnE = 0;
	while(retnE < sizeof(err)){
		retnE += recv(sockfd, &err, sizeof(err), 0);
		if(retnE == 0){
			printf("NO DATA\n");
			return -1;
		}
	}
	if(err != 0){
		errno = err;
		err = -1;
	}
	return err;
}