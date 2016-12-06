#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>

#define PORT 25565

struct sockaddr_in serv_addr;

extern int errno;
int mode = -1;

void error(char *msg){
  perror(msg);
  exit(1);
}

int netserverinit(char * hostname, int filemode){
	mode = filemode;
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	// remember to set errno
	if(gethostbyname(hostname) == NULL){
		printf("Host does not exist\n");
		return -1;
	}
	struct hostent *server;
	server = gethostbyname(hostname);
	bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
	return 0;
}



int netopen(const char * pathname,int flags){
	// remember to set errno
	if(mode == -1){
		return -1;
	}
	int sockfd;
	char ip[100];
	char filename[256];
	int error;
	int err;
	int t = 1;
	strcpy(filename,pathname);
	// init server addr and client addr
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("ERROR: Could not open socket, please check connection!\n");
		exit(0);
	}
	socklen_t addrLen = sizeof(serv_addr);
	inet_ntop(AF_INET, &serv_addr.sin_addr, ip, sizeof ip);
	printf("Connecting to %s\n",ip);
	int status = connect(sockfd, (struct sockaddr *) &serv_addr, addrLen);
	if(status < 0){
		printf("Could not connect to server.\n");
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
	int retnB = recv(sockfd, &error, sizeof(error), 0);
	int retnE = recv(sockfd, &err, sizeof(err), 0);
	if(error == -1){
		errno = err;
		printf("[%d] %s\n",error,strerror(errno));
	}
	close(sockfd);
	return error;
}

ssize_t netread(int fildes, void *buf, size_t nbyte){
	// remember to set errno
	if(fildes >=0){
		return -1;
	}
	int sockfd;
	char ip[100];
	int err;
	int t = 2;
	// init server addr and client addr
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("ERROR: Could not open socket, please check connection!\n");
		exit(0);
	}
	socklen_t addrLen = sizeof(serv_addr);
	inet_ntop(AF_INET, &serv_addr.sin_addr, ip, sizeof ip);
	printf("Connecting to %s\n",ip);
	int status = connect(sockfd, (struct sockaddr *) &serv_addr, addrLen);
	if(status < 0){
		printf("Could not connect to server.\n");
		exit(1);
	}
	// send type
	int type = send(sockfd, &t, sizeof(type), 0);
	// send fdes
	int count_fdes = send(sockfd, &fildes, sizeof(fildes), 0);
	//send nbytes
	int count_nbyte = send(sockfd, &nbyte, sizeof(nbyte), 0);
	//receive error first
	int retnE = recv(sockfd, &err, sizeof(err), 0);
	if(retnE != sizeof(int)){
		printf("Could not receive error!\n");
	}
	if(err != 0){
		errno = err;
		printf("[%d] %s\n",err,strerror(errno));
		close(sockfd);
		return -1;
	}
	printf("ERR: %d\n",err);
	int retnB = 0;
	while(retnB < nbyte){
		int tmp;
		tmp = recv(sockfd, buf, nbyte, 0);
		buf += tmp;
		retnB += tmp;
		printf("Received: %d/%zd\n",retnB,nbyte);
	}
	close(sockfd);
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
	char * filename = "f.txt";
	int fdd = netopen(filename,O_RDONLY);
	 //FILE * file = fopen(filename, "r");
	 //fseek(file, 0L, SEEK_END);
	 //int size = ftell(file);
	// printf("SIZE: %d\n",size);
	// //printf("FD: %d\n",fdd);
	// sleep(1);
	 char * txt = calloc(1,100);
	netread(fdd,txt,1000000);
	// char * file_n = "music2.mp3";
 //    FILE * fp = fopen (file_n, "w+");
 //    fwrite(txt, sizeof(char), size, fp);
    //fclose(fp);
  //  fclose(file);
}


// int netread(int fd, void * buff, size_t nbyte){
// 	int sockfd;
// 	char * msg = argv[2];
// 	char * hostname = argv[1];
// 	char ip[100];
// 	struct hostent *server;
// 	// init server addr and client addr
// 	struct sockaddr_in serv_addr;
// 	sockfd = socket(AF_INET, SOCK_STREAM, 0);
// 	if(sockfd < 0){
// 		printf("ERROR: Could not open socket, please check connection!\n");
// 		exit(0);
// 	}
// 	serv_addr.sin_family = AF_INET;
// 	serv_addr.sin_port = htons(PORT);
// 	if(gethostbyname(hostname) == NULL){
// 		printf("Host does not exist\n");
// 		exit(1);
// 	}
// 	server = gethostbyname(hostname);
// 	bcopy((char *)server->h_addr, 
//          (char *)&serv_addr.sin_addr.s_addr,
//          server->h_length);
// 	socklen_t addrLen = sizeof(serv_addr);
// 	inet_ntop(AF_INET, &serv_addr.sin_addr, ip, sizeof ip);
// 	printf("Connecting to %s\n",ip);
// 	int status = connect(sockfd, (struct sockaddr *) &serv_addr, addrLen);
// 	if(status < 0){
// 		printf("Could not connect to server.\n");
// 		exit(1);
// 	}
// 	int count = send(sockfd, msg, strlen(msg), 0);
// 	printf("Bytes Sent: %d\n",count);
// 	close(sockfd);
// }