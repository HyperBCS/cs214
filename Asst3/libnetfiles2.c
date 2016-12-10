#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "libnetfiles.h"
#include <fcntl.h>

#define PORT 25565

struct sockaddr_in serv_addr;

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
		printf("HOST NOT FOUND\n");
		mode = -1;
		return -1;
	}
	struct hostent *server;
	server = gethostbyname(hostname);
	bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
	// // init server addr and client addr
	// sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// if(sockfd < 0){
	// 	printf("ERROR: Could not open socket, please check connection!\n");
	// 	exit(0);
	// }
	// addrLen = sizeof(serv_addr);
	// inet_ntop(AF_INET, &serv_addr.sin_addr, ip, sizeof ip);
	return 0;
}

int makeSock(int client, int port){
	socklen_t addrLen2;
	serv_addr2.sin_family = AF_INET;
	serv_addr2.sin_port = htons(PORT);
	// remember to set errno
	if(gethostbyname(hostname) == NULL){
		printf("HOST NOT FOUND\n");
		mode = -1;
		return -1;
	}
	struct hostent *server;
	server = gethostbyname(hostname);
	bcopy((char *)server->h_addr2, 
         (char *)&serv_addr2.sin_addr.s_addr,
         server->h_length);
	int * sockf = calloc(1,sizeof(int));
	sockf = socket(AF_INET, SOCK_STREAM, 0);
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

ssize_t netread(int fildes, void *buf, size_t nbyte){
	signal(SIGPIPE, SIG_IGN);
	// remember to set errno
	if(mode == -1){
		h_errno = HOST_NOT_FOUND;
		return -1;
	}
	// remember to set errno
	if(fildes >=0){
		return -1;
	}
	int err;
	int recvM;
	int ports[2];
	int newsock[2];
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
	if(nbyte > 2048){
		while(recvP < sizeof(ports)){
			int tmp;
			tmp = recv(sockfd, &ports+recvP, sizeof(ports), 0);
			recvP += tmp;
			if(retnE == 0){
				errno = ECONNRESET;
				close(sockfd);
				return -1;
			}
		}
		int i = 0;
		pthread_t tid[2];
		for(i = 0;i < 2;i++){
			if(port[i] != 0){
				newsock[i] = makeSock(port[i]);
				pthread_create(&tid[i], NULL, begin, NULL);
			}
		}

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
	printf("ERR: %d\n",err);
	int retnB = 0;
	errno = 0;
	while(retnB < nbyte){
		char c;
		int tmp;
		tmp = recv(sockfd, buf, nbyte, 0);
		if(tmp == 0 && retnB < nbyte){
			errno = ECONNRESET;
			close(sockfd);
			return -1;
		}
		buf += tmp;
		retnB += tmp;
		printf("Received: %d/%zd\n",retnB,nbyte);
	}
	if(err == -1){
		recvB = -1;
	}
	close(sockfd);
	return recvB;
}

ssize_t netwrite(int fildes, const void *buf, size_t nbyte){
	signal(SIGPIPE, SIG_IGN);
	// remember to set errno
	if(mode == -1){
		h_errno = HOST_NOT_FOUND;
		return -1;
	}
	// remember to set errno
	if(fildes >=0){
		return -1;
	}
	int err;
	int recvM;
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
	// send data
	int sent_data = send(sockfd, buf, nbyte, 0);
	if(errno == SIGPIPE){
		errno = ECONNRESET;
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
	printf("ERR: %d\n",err);
	if(err == -1){
		recvB = -1;
	}
	close(sockfd);
	return recvB;
}

int netclose(int fildes){
	if(mode == -1){
		h_errno = HOST_NOT_FOUND;
		return -1;
	}
	// remember to set errno
	if(fildes >=0){
		return -1;
	}
	int err;
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
	printf("ERR: %d\n",err);
	return err;
}

int main(int argc, char ** argv){
	// if(argc != 3){
	// 	printf("Incorrect arguments\n");
	// 	exit(1);
	// }
	// char * hostname = argv[1];
	char * hostname = "localhost";
	netserverinit(hostname,1);
	char * filename = "song.mp3";
	char * filename2 = "song2.mp3";
	int fdd = netopen(filename,O_RDONLY);
	int fdd2 = netopen(filename2,O_RDWR);
	 FILE * file = fopen(filename, "r");
	 fseek(file, 0L, SEEK_END);
	 int size = ftell(file);
	printf("SIZE: %d\n",size);
	//printf("FD: %d\n",fdd);
	sleep(1);
	 char * txt = calloc(1,size);
	netread(fdd,txt,size);
	netwrite(fdd2,txt,size);
   fclose(file);
}