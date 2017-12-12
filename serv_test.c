/*Required Headers*/
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t m;
pthread_mutex_t socklock;
pthread_t* threadIDs;
int threadIndex=0;
struct sockaddr_in servaddr; //init stuff
int listen_fd, comm_fd;

void write_test(FILE * fp, char * str) //test function to be replaced with merge/etc
{
	//printf("%s", str);
	fprintf(fp, "%s", str);
	fflush(fp);
}

void* rec(void* args)
{	
	pthread_mutex_lock(&socklock);
	char * str = malloc(10000);
   	int size = 0;
	int fileSize=0;
	char* recv=malloc(100);
	strcpy(recv,"hello");
	if (read(comm_fd, &size, sizeof(size)) == 0) //get size lines of file
	{
		printf("[-] Disconnected from client %d\n", listen_fd); //to be changed to ip?
		free(str);
		pthread_mutex_unlock(&socklock);
		return "no";
	}
	write(comm_fd,"q",2);
	printf("[+] Connect to client %d\n", listen_fd); //to be changed to ip?
	if(ntohl(size)<0)
	{
		//this is not a file
		printf("dump\n");
		//do the dump thing and send over sorted of all files
		pthread_mutex_unlock(&socklock);
		return "yo";
	}
	if (read(comm_fd, &fileSize, sizeof(fileSize)) == 0) //get size lines of file
	{
		printf("[-] Disconnected from client %d\n", listen_fd); //to be changed to ip?
		free(str);
		pthread_mutex_unlock(&socklock);
		return "no";
	}
	write(comm_fd,"q",2);
	FILE* out_file = fopen("files_sorted.csv", "w");
	char* file=malloc(size);
	char* whole=malloc(fileSize);	
	strcpy(recv,"hello");
	int j = 0;
	int len=0;
	for (j = 0; j < ntohl(size); j++)
    	{
 		
		bzero(str, 10000); 
       		if(read(comm_fd,str,10000) == 0) //go through each line of csv, and get the line
		{
			printf("[-] Disconnected from client %d\n", listen_fd); //to be changed to ip?
 			comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
			continue;
		}
		else
		{
			len+=sprintf(whole+len,"%s",str);
 			fflush(out_file);
			write(comm_fd, recv, strlen(recv)+1); //send back a signal to show that it's done recieving data(lets it be in order)	
		}
		 
    	}
	fprintf(out_file,"%s",whole);
 	fclose(out_file);
	free(str);
	free(file);
	free(whole);
	pthread_mutex_unlock(&socklock);
	return "no";
}

int main(int argc, char** argv)
{
	threadIDs=malloc(sizeof(pthread_t)*5); //change 5 to how many files  
 
 
    	bzero( &servaddr, sizeof(servaddr)); //zero at addresse
 
    	//int port = atoi(argv[1]); 
	
	
	if((listen_fd=socket(AF_INET,SOCK_STREAM,0))<=0) //check if sockets being used
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
	
	if (getsockname(listen_fd, (struct sockaddr *)&servaddr, &len) == -1)
	{
		perror("getsockname");
		return 0;
	} //checl of socket binded correctly

	printf("Running On Port Number: %d\n", ntohs(servaddr.sin_port)); //get port

  	listen(listen_fd, 10); //allow for max 10 connections (not sure how it changes threads)
 

    	while(comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL)) //start listening on connection 
	{
		printf("got one\n");
		void* state;
		pthread_mutex_lock(&m);
		pthread_t index=threadIDs[threadIndex++];
		pthread_mutex_unlock(&m);
		pthread_create(&index,NULL,&rec,NULL);
		pthread_join(index,&state);
		char* d=(char*)state;
		if(d[0]=='y')
		{
			break;
		}
	}
	
	printf("\n");
	
	close(listen_fd);
	
	exit(EXIT_SUCCESS);
}
