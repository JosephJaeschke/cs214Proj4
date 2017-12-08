//////////////////////////////////////////////
//		Create Your Own Server				//
//		@ by Hanxiong Chen					//
//////////////////////////////////////////////


//////// Part 1: Headers and Macros //////////
/* C Library Stuff */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/* Socket Stuff */
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* Pthread Library */
#include <pthread.h>

/* Macros (define your macros below) */
#define MAX_NUM_THREAD 10
/********* Start Coding Here *********/
// You need a port number here
#define PORT 9000
// Need more? Add yours

/********* Coding End ***********/












/********** Helper functions ***********/
int status = 1; // for server running status
int num_of_thread = 0; // currently created num of threads

typedef struct tid_type
{
	pthread_t tid;
	int isFree;
	int socketfd;
}tid_type;

/* Global thread id pool */
tid_type tid_pool[MAX_NUM_THREAD];
	
void init_tid_pool()
{
	for(int i = 0; i < MAX_NUM_THREAD; i++)
	{
		tid_pool[i].isFree = 1;
		tid_pool[i].socketfd = -1;
	}
}

/* return the index of available tid in pool */
int get_tid()
{
	for (int i = 0; i < MAX_NUM_THREAD; i++)
	{
		if (tid_pool[i].isFree == 1)
		{
			return i;
		}
	}
	return -1;
}

void release_tid(int index)
{
	tid_pool[index].isFree = 1;
}
/********** Helper functions end *********/



//////// Part 3: Service function ////////////

/* Define your service function to serve requests */

// In this demo code, our server only give response
// messages to the client
void * service(void *args)
{
	// this is the socket for our server 
	// to talk to client
	int index = (int)args;
	int client_socket = tid_pool[index].socketfd;
	// define two buffers, receive and send
	char send_buf[256] = "\nThank you for file\n";
	char recv_buf[256];
	/* STEP 5: receive data */
	// use read system call to read data 
	read(client_socket, recv_buf, 256);
	long fsize=strtol(recv_buf,NULL,10);
	printf("[r] make room for %lu bytes\n",fsize);
	char filebuff[fsize];
	// replace receive buffer with your buffer name
//	printf("[r] Reading from client: %s\n", recv_buf);
	FILE* out=fopen("server_dump.csv","w");
	long n=0;
	n=read(client_socket,filebuff+n,fsize);
	printf("received %lu bytes",n);
	printf("\nafter loop\n");
	/*
	int n=0;
	while(n!=fsize)
	{
		printf("1");
		n+=recv(client_socket,filebuff,fsize,0);
		printf("received %d bytes\n",n);
		if(n<=0)
		{
			printf("err\n");
			perror("read");	
		}
	}
	*/
	//printf("[r] %s\n\n",filebuff);
	fclose(out);
	/* STEP 6: send data */
	// prepare your sending data
	// use write system call to send data
	write(client_socket, send_buf, 256);

	printf("[s] Data sent\n===\n");

	/* STEP 7: close socket */
	close(client_socket);

	/* Need to add lock here */
	release_tid(index);
	num_of_thread--;
}

/////// Part 2: Main Function ///////////////

int main(int argc, char **argv)
{
	/* optional: You can add args checking 
	 * here. That means you allow user to
	 * enter some information such as port
	 * number 
	 * */
	
	/********** Start Coding Here ***********/
	// Let's define some useful variables first
	// We need a server socket and a client socket
	int server_sock, client_sock;
	// Then we need a sockaddr_in struct to hold
	// socket information
	struct sockaddr_in address;

	/* STEP 1: create socket and setup sockaddr_in */
	
	// you can call socket(AF_INET, SOCK_STREAM, 0)
	// to create a socket
	// REMEMBER: ALWAYS CHECK FAILURE WHEN YOU DO I/O
	if ((server_sock = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
	{
		// print error message with perror()
		perror("socket");
		// exit your server
		exit(EXIT_FAILURE);
	}

	// setup the sockaddr_in struct you defined above
	// sin_family = AF_INET
	// sin_addr.s_addr = INADDR_ANY
	// sin_port = htons(port number)
	address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	/* STEP 2: bind */
	// Bind is to associate your socket (the file de\
	// scriptor -- an integer) with your server addr
	// and port number. These info is already defined
	// in sockaddr_in struct above.
	
	// You can call 
	// bind(socket, (struct sockaddr*)&server_addr, sizeof sockaddr struct)
	if (bind(server_sock, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		// perror() report error message
		perror("bind");
		// close socket
		close(server_sock);
		// exit your program
		exit(EXIT_FAILURE);
	}

	/* STEP 3: Listen */
	// Now we have a binded socket, we can use this 
	// socket to listen on a port
	
	// You can call
	// int listen(int socket, int backlog)
	if (listen(server_sock, 0) < 0)
	{
		// perror() report error message
		perror("listen");
		// close socket
		close(server_sock);
		// exit your program
		exit(EXIT_FAILURE);
	}

	printf("Waiting for connections...\n");

	init_tid_pool();
	// we use a while loop to keep waiting for connections
	while (status)
	{
		/* STEP 4: create connection/accept connection request */
		// use client socket to accept client request
		// accept(int server_socket, NULL, NULL)
		// you can setup the second and third arguments other 
		// than NULL
		client_sock = accept(server_sock, NULL, NULL);
		// check if accept() is successful or not
		if (client_sock < 0)
		{
			// perror() report error message
			perror("accept");
			// close socket
			close(server_sock);
			// exit your program
			exit(EXIT_FAILURE);
		}

		// replace client socket below with your client socket variable name
		printf("[+] Connect to client %d\n", client_sock); 

		// the last argument is tricky not a good way to do, just demo
		// replace client socket with your client socket variable name
		if (num_of_thread < MAX_NUM_THREAD)
		{
			int i = get_tid();
			// replace client socket here
			tid_pool[i].socketfd = client_sock;
			pthread_create(&tid_pool[i].tid, NULL, service, (void *)i);
			/* You need to add lock here */
			num_of_thread++;
			/* add lock above */
			pthread_detach(tid_pool[i].tid);
		}

		/* what if num_of_thread >= MAX_NUM_THREAD? 
		 * You need to think about a way to solve this
		 * problem
		 * CV/semaphore ... or other better solution
		 * */
		else
		{
			// just dirty code for demo ;)
			while (num_of_thread >= MAX_NUM_THREAD)
			{
				sleep(1);
			}
		}
	}

	/* clean up */
	close(server_sock);
	return 0;
}
