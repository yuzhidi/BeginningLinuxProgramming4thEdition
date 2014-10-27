/*  Make the necessary includes and set up the variables.  */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int sockfd;
    int len;
	int i;
    struct sockaddr_in address;
    int result;
	unsigned char acde0005[5] = {0xAC, 0xED, 0x00, 0x05, 0};
	unsigned char readbuf[128] = {0};
/*  Create a socket for the client.  */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

/*  Name the socket, as agreed with the server.  */

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = 8000;
    len = sizeof(address);

/*  Now connect our socket to the server's socket.  */

    result = connect(sockfd, (struct sockaddr *)&address, len);

    if(result == -1) {
        perror("oops: client2");
        exit(1);
    }

/*  We can now read/write via sockfd.  */

//    write(sockfd, &ch, 1);
    read(sockfd, &readbuf, 4);

	for(i =0;i<4;i++) {
		printf("char from server = %x\n", readbuf[i]);
	}
    close(sockfd);
    exit(0);
}
