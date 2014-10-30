/*  Make the necessary includes and set up the variables.  */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LEN_ACDE 4
#define LEN_ACDE0 5
#define LEN_77XX  2
#define LEN_MAGIC 4

int printBuf(unsigned char* buf,int len) {
	int i;
	for(i=0;i<len;i++) {
		printf("buf[%d] 0x%x\n",i,  buf[i]);
	}
}

void printTime() {
	time_t timep;
	time(&timep);
	printf("\n%s\n",asctime(localtime(&timep)));
}

int main(int argc, char * argv[])
{
	int run;
	int sockfd;
	int len;
	int i;
	struct sockaddr_in address;
	int result;

	unsigned char * addr = "192.168.43.1";
	int port = 8000;

	unsigned char ACDE0005[5] = {0xAC, 0xED, 0x00, 0x05, 0};
	unsigned char MAGIC_NUMBER[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0};
	unsigned char readbuf[128] = {0};
	//data
	unsigned int latiH;
	unsigned int latiL;
	unsigned int longtiH;
	unsigned int longtiL;

	double latitude;
	double longitude;

	if(argc == 3) {
		addr = argv[1];
		port = atoi(argv[2]);
	}

	printf("addr:%s, port:%d\n", addr, port);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr(addr);
	address.sin_port = htons(port);
	len = sizeof(address);

	result = connect(sockfd, (struct sockaddr *)&address, len);

	if(result == -1) {
		perror(" oops: client2");
		exit(1);
	}

	if(read(sockfd, readbuf,LEN_ACDE0) <=0) {
		perror("oops: client2");
		exit(1);
	}

	printBuf(readbuf,LEN_ACDE0);
	if(strstr(readbuf, ACDE0005)) {
		printf("find ACDE0005\n");
	} else {
		printf("not acde00005, continue\n");
		exit(0);
	}

	run = 1;
	while(run) {
		printTime();
		// find magic number 
		int find = 4;
		while(find) {
			if(read(sockfd, readbuf,1)<1) {
				perror(" oops: client2");
				exit(1);
			}

			printBuf(readbuf,1);
			if(0xFF == readbuf[0]) {
				find--;
				printf("find once:%d\n",find);
			}else {
				find=4;
			}
		}
		// at least will recevie 4 int, the length is 16 bytes
		if(read(sockfd, readbuf,16) <=0) {
			perror(" oops: client2");
			exit(1);
		}


		memcpy(&latiH, readbuf, 4);
		memcpy(&latiL, readbuf +4, 4);
		memcpy(&longtiH, readbuf+8, 4);
		memcpy(&longtiL, readbuf+12, 4);

		latiH = ntohl(latiH);
		latiL = ntohl(latiL);
		longtiH = ntohl(longtiH);
		longtiL = ntohl(longtiL);

		printf("latiH:%u, latiL:%u, longtiH:%d, longtiL:%d\n", latiH, latiL, longtiH, longtiL);

		latitude=((double)latiL / 100000000) + latiH;
		longitude = ((double)longtiL / 100000000) + longtiH;
		printf("latitude:%f, longitude:%f\n", latitude, longitude);
	}
	close(sockfd);
	exit(0);
}
