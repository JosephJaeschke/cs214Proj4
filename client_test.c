#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include<string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>


int main(int argc,char **argv)
{
    int sockfd,n;
	int sizeof_string = 100;
    char * sendline = malloc(sizeof_string);
    
	struct sockaddr_in servaddr;
 
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof servaddr);
 
    servaddr.sin_family=AF_INET;
    servaddr.sin_port=htons(22000);
 
    inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));
 
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1){
		printf("ERROR : Failed to connect to server\n");
		exit(EXIT_FAILURE);
	}
 
    while(1)
    {
        bzero( sendline, 100);
        fgets(sendline,100,stdin); /*stdin = 0 , for standard input */
 
        write(sockfd,sendline,strlen(sendline)+1);
        //read(sockfd,recvline,100);
        //printf("%s",recvline);
    }
 
}
