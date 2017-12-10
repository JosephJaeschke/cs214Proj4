#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
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
	int sizeof_string = 10000;
    char * sendline = malloc(sizeof_string);
    
	struct sockaddr_in servaddr;
 
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    bzero(&servaddr,sizeof servaddr);
 
    int port = atoi(argv[2]);
	servaddr.sin_family=AF_INET;

    servaddr.sin_port=htons(port);
 
    inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));
 
    if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1){
		printf("ERROR : Failed to connect to server\n");
		exit(EXIT_FAILURE);
	}


	char * filename1 = malloc(100);
	
	strncpy(filename1, argv[1], sizeof(argv[2]));
	filename1[sizeof(argv[1]) + 1] = '\0';

	printf("%s\n", filename1);
	FILE * fp = fopen(filename1, "r");
	int row_position = 0;
	int j = 0;
	int file_count = 0;
	int i = 0;

	char c = getc(fp);
	
	while (c != EOF) {
		if(c == '\n'){
			file_count++;
		}
		i++;
		c = getc(fp);
    }


	fclose(fp);
	

	char * str_file = malloc(sizeof(char) * (i + 1));
	
	FILE * fp1 = fopen(filename1, "r");
	
	if(fp1 == NULL){
	}
	c = getc(fp1);
	
	i=0;
	while (c != EOF) {
		str_file[i] = c;
		i++;
		c = getc(fp1);
    }
   
	if(fp1 != NULL)
	{
		fclose(fp1);	
	}
	if(filename1 != NULL){

	free(filename1);
	}	
	
	
	str_file[i] = '\0';	
	
	int sentn = htonl(file_count);
	
	char * recvline = malloc(100);
	//read(sockfd,recvline,100);
        	
	write(sockfd,&sentn ,sizeof(sentn));   	

	int index1 = 0;
	int index2 = 0;

	
    for(j = 0; j < file_count; j++)
    {
        
		//sleep(1);
		bzero(sendline, 10000);
		
		while(str_file[index2] != '\n'){
			index2++;
		}
		index2++;

		strncpy(sendline,str_file+index1, index2 - index1);

        //fgets(sendline,100,stdin); /*stdin = 0 , for standard input */
		//printf("%s", sendline); 
        
		write(sockfd,sendline,strlen(sendline)+1);
       	
		index1 = index2; 
		
		read(sockfd,recvline,100);
        //printf("%s",recvline);
    }
	
	//read(sockfd,recvline,100);    
	printf("\nFile Sent\n");
	return 0; 
}
