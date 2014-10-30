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

int main()
{
	int run;
	int sockfd;
	int len;
	int i;
	struct sockaddr_in address;
	int result;
	unsigned char ACDE0005[5] = {0xAC, 0xED, 0x00, 0x05, 0};
	unsigned char ACDE000500[6] = {0xAC, 0xED, 0x00, 0x05,0x00, 0};
	unsigned char MAGIC_NUMBER[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0};
	unsigned char readbuf[128] = {0};
	//data
	unsigned int latiH;
	unsigned int latiL;
	unsigned int longtiH;
	unsigned int longtiL;

	double latitude;
	double longitude;
	//	unsigned long testLong;
	/*  Create a socket for the client.  */

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/*  Name the socket, as agreed with the server.  */

	address.sin_family = AF_INET;
	//address.sin_addr.s_addr = inet_addr("10.120.0.81");
	address.sin_addr.s_addr = inet_addr("192.168.43.1");
	//	address.sin_addr.s_addr = inet_addr("10.120.0.148");
	address.sin_port = htons(8000);
	len = sizeof(address);

	/*  Now connect our socket to the server's socket.  */

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
	if(strstr(readbuf, ACDE000500)) {
		printf("find ACDE000500\n");
	} else if(strstr(readbuf, ACDE0005)) {
		printf("find ACDE0005\n");
	} else {
		printf("not acde00005, continue\n");
		exit(0);
	}
	run = 1;
	/*  We can now read/write via sockfd.  */
	while(run) {
		printTime();
		// 77:18  
		int find = 4;
		while(find) {
			read(sockfd, readbuf,1);
			printBuf(readbuf,1);
			if(0xFF == readbuf[0]) {
				find--;
				printf("find once:%d\n",find);
			}else {
				find=4;
			}
		}
		// at least will recevie 5 int, the length is 20 bytes
		//
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
