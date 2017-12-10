/*Required Headers*/
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char ** argv)
{
 
    char * str = malloc(10000);
    int listen_fd, comm_fd;
 
    struct sockaddr_in servaddr;
 
    //listen_fd = socket(AF_INET, SOCK_STREAM, 0);
 
    bzero( &servaddr, sizeof(servaddr));
 
    //int port = atoi(argv[1]); 
	
	
	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
	{
		// print error message with perror()
		perror("socket");
		// exit your server
		exit(EXIT_FAILURE);
	}


	servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(0);
 
    bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr));
 	
	socklen_t len = sizeof(servaddr);
	
	if (getsockname(listen_fd, (struct sockaddr *)&servaddr, &len) == -1) {
		    perror("getsockname");
			    return 0;
	}

	printf("Running On Port Number: %d\n", ntohs(servaddr.sin_port));

    listen(listen_fd, 10);
 
    comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
 
    int size = 0;

	char * recv = malloc(100);
	strcpy(recv, "hello");
	

	while (1) {

//		write(comm_fd, recv, strlen(recv)+1);
	
	   	size = 0;

		if (read(comm_fd, &size , sizeof(size)) == 0){
				printf("client disconnected\n");
 				comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
				continue;	
		}

		printf("[+] Connect to client %d\n", size); 
		
		//printf("Size is %d\n", ntohl(size));
	
		int j = 0;
		for (j = 0; j < ntohl(size); j++)
	    {
	 		
			bzero(str, 10000); 
			 
    	    if(read(comm_fd,str,10000) == 0) {
				printf("client disconnected\n");
 				comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
				continue;
			}
			else {
    	    	printf("%s",str);
 				write(comm_fd, recv, strlen(recv)+1);
	
			}
		 
   	 }
	}
	printf("\n");
	
	close(listen_fd);
	exit(EXIT_SUCCESS);
}
