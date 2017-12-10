/*Required Headers*/
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>



void write_test(FILE * fp, char * str){ //test function to be replaced with merge/etc
	//printf("%s", str);
	fprintf(fp, "%s", str);
	fflush(fp);
}


int main(int argc, char ** argv)
{
 
    char * str = malloc(10000);
    int listen_fd, comm_fd;
 
    struct sockaddr_in servaddr; //init stuff
 
 
    bzero( &servaddr, sizeof(servaddr)); //zero at addresse
 
    //int port = atoi(argv[1]); 
	
	
	if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) //check if sockets being used
	{
		// print error message with perror()
		perror("socket");
		// exit your server
		exit(EXIT_FAILURE);
	}


	servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(0); //set to a random port which isn't being used
 
    bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)); //bind program to that port
 	
	socklen_t len = sizeof(servaddr);
	
	if (getsockname(listen_fd, (struct sockaddr *)&servaddr, &len) == -1) {
		    perror("getsockname");
			    return 0;
	} //checl of socket binded correctly

	printf("Running On Port Number: %d\n", ntohs(servaddr.sin_port)); //get port

    listen(listen_fd, 10); //allow for max 10 connections (not sure how it changes threads)
 
    comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL); //start listening on connection 
 
    int size = 0;

	char * recv = malloc(100);
	strcpy(recv, "hello");
	

	FILE * out_file = fopen("files_sorted", "w"); //file to write to
	fflush(out_file);
	fclose(out_file);

	while (1) {

	   	size = 0;
		
		if (read(comm_fd, &size , sizeof(size)) == 0){ //get size lines of file
			printf("[-] Disconnected from client %d\n", listen_fd); //to be changed to ip?
 				comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
				continue;	
		}

		printf("[+] Connect to client %d\n", listen_fd); //to be changed to ip?
		
		out_file = fopen("files_sorted", "w");

	
		int j = 0;
		for (j = 0; j < ntohl(size); j++)
	    {
	 		
			bzero(str, 10000); 
			 
    	    if(read(comm_fd,str,10000) == 0) { //go through each line of csv, and get the line
				printf("[-] Disconnected from client %d\n", listen_fd); //to be changed to ip?
 				comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
				continue;
			}
			else {
    	    	write_test(out_file, str);
 				fflush(out_file);
				write(comm_fd, recv, strlen(recv)+1); //send back a signal to show that it's done recieving data(lets it be in order)	
			}
		 
   	 }
	 fclose(out_file);
	}
	
	printf("\n");
	
	close(listen_fd);
	
	exit(EXIT_SUCCESS);
}
