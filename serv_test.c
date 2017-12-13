/*Required Headers*/
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <arpa/inet.h>
#include "sorter_server.h"

pthread_mutex_t m;
pthread_mutex_t socklock;
pthread_t* threadIDs;
int threadIndex=0;
struct sockaddr_in servaddr; //init stuff
int listen_fd, comm_fd;
char* token;
char header='0';

void mergeStr(CSVRow* arr,CSVRow* help, int lptr,int rptr,int llimit,int rlimit,int num)
{
    int i=lptr,j=llimit,k=0;
    while(i<=rptr && j<=rlimit) 
    {
	if(strcmp(arr[i].data,arr[j].data)==0)
	{
		if(arr[i].point<arr[j].point)
		{
        	    strcpy(help[k].data,arr[i].data);
        	    help[k].point=arr[i].point;
        	    strcpy(help[k].string_row,arr[i].string_row);
        	    k++;
        	    i++;
		}
        	else
		{
        	    strcpy(help[k].data,arr[j].data);
        	    help[k].point=arr[j].point;
        	    strcpy(help[k].string_row,arr[j].string_row);
        	    k++;
        	    j++;
		}
	}
        else if(strcmp(arr[i].data,arr[j].data)<0)
	{
            strcpy(help[k].data,arr[i].data);
            help[k].point=arr[i].point;
            strcpy(help[k].string_row,arr[i].string_row);
            k++;
            i++;
	}
        else
	{
            strcpy(help[k].data,arr[j].data);
            help[k].point=arr[j].point;
            strcpy(help[k].string_row,arr[j].string_row);
            k++;
            j++;
	}
    }

    while(i<=rptr)
    {
            strcpy(help[k].data,arr[i].data);
            help[k].point=arr[i].point;
            strcpy(help[k].string_row,arr[i].string_row);
            k++;
            i++;
    }
    while(j<=rlimit) 
    {
            strcpy(help[k].data,arr[j].data);
            help[k].point=arr[j].point;
            strcpy(help[k].string_row,arr[j].string_row);
            k++;
            j++;
    }
    for(i=lptr,j=0;i<=rlimit;i++,j++)
    {
	        strcpy(arr[i].data,help[j].data);
		arr[i].point=help[j].point;
		strcpy(arr[i].string_row,help[j].string_row);
    }
}

void sortStr(CSVRow* a,CSVRow *b, int i,int j,int num)
{
    int mid;
    if(i<j)
    {
        mid=(i+j)/2;
        sortStr(a,b,i,mid,num); 
        sortStr(a,b,mid+1,j,num);
        mergeStr(a,b, i,mid,mid+1,j,num); 
    }
}

void mergeInt(CSVRow* arr,CSVRow* help, int lptr,int rptr,int llimit,int rlimit,int num)
{
    int i=lptr,j=llimit,k=0;
    while(i<=rptr && j<=rlimit) 
    {
	if(strtof(arr[i].data,NULL)==strtof(arr[j].data,NULL))
	{
		if(arr[i].point<arr[j].point)
		{
        	    strcpy(help[k].data,arr[i].data);
        	    help[k].point=arr[i].point;
        	    strcpy(help[k].string_row,arr[i].string_row);
        	    k++;
        	    i++;
		}
        	else
		{
        	    strcpy(help[k].data,arr[j].data);
        	    help[k].point=arr[j].point;
        	    strcpy(help[k].string_row,arr[j].string_row);
        	    k++;
        	    j++;
		}
	}
        else if(strtof(arr[i].data,NULL)<strtof(arr[j].data,NULL))
	{
			strcpy(help[k].data,arr[i].data);
            help[k].point=arr[i].point;
            strcpy(help[k].string_row,arr[i].string_row);
            k++;
            i++;
	}
        else
	{
            strcpy(help[k].data,arr[j].data);
            help[k].point=arr[j].point;
            strcpy(help[k].string_row,arr[j].string_row);
            k++;
            j++;
	}
    }
    while(i<=rptr) 
    {
            strcpy(help[k].data,arr[i].data);
            help[k].point=arr[i].point;
            strcpy(help[k].string_row,arr[i].string_row);
            k++;
            i++;
    }
    while(j<=rlimit) 
    {
            strcpy(help[k].data,arr[j].data);
            help[k].point=arr[j].point;
            strcpy(help[k].string_row,arr[j].string_row);
            k++;
            j++;
    }
    for(i=lptr,j=0;i<=rlimit;i++,j++)
    {
        strcpy(arr[i].data,help[j].data);
		arr[i].point=help[j].point;
		strcpy(arr[i].string_row,help[j].string_row);
    }
}

void sortInt(CSVRow* a,CSVRow* b, int i,int j,int num)
{
    int mid;
    if(i<j)
    {
        mid=(i+j)/2;
        sortInt(a,b, i,mid,num); 
        sortInt(a,b, mid+1,j,num); 
        mergeInt(a,b, i,mid,mid+1,j,num); 
    }
}

void callMe(int size,char type,CSVRow* arr, CSVRow* b)
{
	if(type=='i')
	{
		sortInt(arr,b, 1,size-1,size);
	}
	else
	{
		sortStr(arr,b, 1,size-1,size);
	}
	return;
}

void trim(char* str)
{
	char * t = malloc(strlen(str));
	int i =0;
	int j=0;
	for(i=0;i<strlen(str);i++)
	{
		if(isspace(str[i])==0)
		{
			t[j]=str[i];
			j++;
		}
	}
	t[j]='\0';
	strcpy(str,t);
	free(t);
}



void doTheSort()
{
	FILE* fp=fopen("files_sorted.txt","r");
	int file_count = 0;
	char c = 0;
	int i = 0;
	char * str_file = malloc(10);
	int row_position = 0;
	int j;
	//fprintf(stdout, "%s\n", token);		
	
	c = fgetc(fp);
	while (c != EOF) 
	{
		//printf("%c\n",c);
		str_file = realloc(str_file, (i+1) * sizeof(char));	
		str_file[i] = c;
		if(c == '\n')
		{
			file_count++;
		}
		i++;
		c = fgetc(fp);
    	}
        
	fclose(fp);
	
	str_file[i] = '\0';
	
	
	CSVRow *movies = malloc(file_count * sizeof(CSVRow));
	//token = strtok(str_file, "\n");
	
	for(j = 0; j < file_count; j++)
	{
		movies[j].data = malloc(10000);
		movies[j].point = j;
		movies[j].string_row = malloc(10000);
	}

	CSVRow* help=malloc(sizeof(CSVRow)*file_count*2);    //array used for merging
    	for(j =0;j<file_count;j++)
    	{
		help[j].data=malloc(10000);
		help[j].point=j;
		help[j].string_row=malloc(10000);
    	}

	//CSVRow *tempy = malloc(file_count * sizeof(CSVRow));
	//token = strtok(str_file, "\n");
	
	//for(int j = 0; j < file_count; j++){
	//	tempy[j].data = malloc(1000);
	//	tempy[j].point = j;
	//	tempy[j].string_row = malloc(1000);
	//}


	int temp = 0;
	int count = 0;
	int index = 0;
	int comma_position_max = 0;
	int p1 = 0;
	int p2 = 0;
	int char_found = 0;
	int comma_number = 0;
	char * check_token = malloc(1000);
	c = 0;
	for(j = 0; j < i; j++)
	{
		if(str_file[j] == '\n')
		{
			//printf("a\n");
			strncpy(movies[count].string_row, str_file+temp,j-temp+1);
			movies[count].string_row[j-temp+1] = '\0';
			if (count == 0)
			{
				c = movies[count].string_row[index];
				for(index = 0; index<strlen(movies[count].string_row) ; index++)
				{
					//fprintf(stdout, "%c\n", c);
					c = movies[count].string_row[index];
					if(c == ',' || movies[count].string_row[index+1] == '\n')
					{
						if( movies[count].string_row[index+1] == '\n')
						{
							index++;
						}
						comma_position_max++;
						if(index == p1 || index == p1+1)
						{
							check_token = "\0";
						}
						else
						{
							strncpy(check_token, movies[count].string_row+p1,index-p1);
							check_token[index-p1] =  '\0';
							//fprintf(stdout, "[%s] , [%s]\n", check_token, token);
						}
						if(strcmp(check_token,token) == 0)
						{
							char_found = 1;
							//fprintf(stdout, "[%s] , [%s]\n", check_token, token);
							break;
						}
						p1 = index+1;
						if( movies[count].string_row[index+1] == '\n')
						{
							index--;
						}

					}
					//printf("%d\n %c",char_found, c);
				}
				if(char_found == 0)
				{
					//fprintf(stderr, "ERROR: <Selected item was not found in parameters>\n");
					free(check_token);
					free(movies);
					free(token);
					free(str_file);

					return;
				}
				//fprintf(stdout,"%d : %d\n",char_found , comma_position_max);
				//fprintf(stdout, "[%s] : [%s] \n",token, check_token);
				strcpy(movies[count].data, token);
				movies[count].data[strlen(token)+1] = '\0';
			}
			else
			{
				//fprintf(stdout, "%d \n ", count);
				comma_number = 0;
				index = 0;
				p1 = 0;
				c = movies[count].string_row[index];
				for(index = 0; index<strlen(movies[count].string_row); index++)
				{
					//fprintf(stdout, "%c\n", c);
					c = movies[count].string_row[index];
					if(c == ',' && index+1 != strlen(movies[count].string_row) && movies[count].string_row[index+1] == '"')
					{
							
						comma_number++;
						if((index == p1) && (comma_number == comma_position_max))
						{
							//movies[count].data = "0\0";
							break;
						}
						else if(comma_number == comma_position_max)
						{
							strncpy(movies[count].data, movies[count].string_row+p1,index-p1);
							//fprintf(stdout, "[%s] , [%s]\n", check_token, token);
							//movies[count].data[index-p1] = '\0';
							//fprintf(stdout, "%d: %s\n",count, movies[count].data);	
							trim(movies[count].data);
							break;
						}
						p1 = index+1;
						
						index = index+2;
						int x;
						for(x = 0; c != '"'; index++)
						{
							c = movies[count].string_row[index];
						}	
						
						c = movies[count].string_row[index];
						//fprintf(stdout,"%c\n" , c);
						comma_number++;
						if((index == p1) && (comma_number == comma_position_max))
						{
							//movies[count].data = "0\0";
							break;
						}
						else if(comma_number == comma_position_max)
						{
							strncpy(movies[count].data, movies[count].string_row+p1,index-p1);
							//fprintf(stdout, "[%s] , [%s]\n", check_token, token);
							//movies[count].data[index-p1] = '\0';
							//fprintf(stdout, "%d: %s\n",count, movies[count].data);	
							trim(movies[count].data);
							break;
						}
						p1 = index+1;
						index++;
						c = movies[count].string_row[index];
	
					}
					if(c == ',' || movies[count].string_row[index+1] == '\n')
					{

						if( movies[count].string_row[index+1] == '\n')
						{
							index++;
						}

						comma_number++;
						if((index == p1) && (comma_number == comma_position_max))
						{
							//movies[count].data = "NULL";
							break;
						}
						else if(comma_number == comma_position_max)
						{
							strncpy(movies[count].data, movies[count].string_row+p1,index-p1);
							//fprintf(stdout, "[%s] , [%s]\n", check_token, token);
							//movies[count].data[index-p1] = '\0';
							//fprintf(stdout, "%d: %s\n",count, movies[count].data);	
							trim(movies[count].data);
							break;
						}
						p1 = index+1;
						if( movies[count].string_row[index+1] == '\n')
						{
							index--;
						}

					}
				}
			}
			
			temp = j+1;
			count++;
		}
	}
	
	char type = 'i';
	int k;
	for(j = 1; j < file_count; j++)
	{
		for(k = 0; movies[j].data[k] != '\0'; k++)
		{
			if(!(isdigit(movies[j].data[k])))
			{
				if(movies[j].data[k] != '.' || movies[j].data[k] != '-')
				{
					type = 's';	
				}
			}
		}
	}
	//printf("%d \n", type);
	//mergesort(movies,1,file_count-1,file_count);
	callMe(file_count,type,movies,help);
	//printf("heyo\n");
	//printf("\n");
	int big=0;
	for(j=0;j<file_count;j++)
	{
		big+=strlen(movies[j].string_row);
	}
	int len=0;
	char* sorted_output=malloc(big);
	for(j = 0; j < file_count; j++)
	{
		//printf("j:%d\n",j);
		len+=sprintf(sorted_output+len,"%s",movies[j].string_row);
		//printf("[%s]\n", movies[j].data);
	}
	//FILE* dump=fopen("server_dump.csv","w");
	//fprintf(dump,"%s",sorted_output);
	//fclose(dump);
	
//============================================================ SEND FILE BACK
	//printf("talk\n");
	//comm_fd=socket(AF_INET,SOCK_STREAM,0);
	char * sendline = malloc(10000);
 
	
	row_position = 0;
	j = 0;

	free(str_file);
	
	int sentn = htonl(file_count);
	char * recvline = malloc(100);
	char* resp=malloc(2);
	//read(sockfd,recvline,100);
	//printf("first write -> %d\n",sentn);
	write(comm_fd,&sentn,sizeof(sentn));   	
	read(comm_fd,resp,2);
	//printf("second write -> %d\n",i);
	write(comm_fd,&big,sizeof(big));
	read(comm_fd,resp,2);
	int index1 = 0;
	int index2 = 0;
	//printf("start sending file\n");

	for(j = 0; j < file_count; j++)
    	{
		bzero(sendline, 10000);
		while(sorted_output[index2] != '\n')
		{
			index2++;
		}
		index2++;
		strncpy(sendline,sorted_output+index1, index2 - index1);
		write(comm_fd,sendline,strlen(sendline)+1);
		index1 = index2; 
		read(comm_fd,recvline,100);
	}
	//read(sockfd,recvline,100);    
	//printf("\nFile Sent\n");
	return;
//============================================================ SEND FILE BACK





	for(j = 0; j < file_count; j++)
	{
		free(movies[j].data);
		free(help[j].data);
		//movies[j].point = j;
		free(movies[j].string_row);
		free(help[j].string_row);
	}


	free(check_token);
	free(movies);
	free(help);
	free(token);
	free(str_file);
	free(sorted_output);
	return;

}

void write_test(FILE * fp, char * str) //test function to be replaced with merge/etc
{
	//printf("%s", str);
	fprintf(fp, "%s", str);
	fflush(fp);
}

void* rec(void* args)
{	
	//printf("rec\n");
	pthread_mutex_lock(&socklock);
	char * str = malloc(10000);
   	int size = 0;
	int fileSize=0;
	char* recv=malloc(100);
	strcpy(recv,"hello");
	if (read(comm_fd, &size, sizeof(size)) == 0) //get size lines of file
	{
		//printf("1[-] Disconnected from client %d\n", listen_fd); //to be changed to ip?
		free(str);
		pthread_mutex_unlock(&socklock);
		return "no";
	}
	//printf("-- %d\n",ntohl(size));
	write(comm_fd,"q",2);
	//printf("[+] Connect to client %d\n", listen_fd); //to be changed to ip?
	if(ntohl(size)==758185984)
	{
		//this is not a file
		//printf("dump\n");
		//do the dump thing and send over sorted of all files
		pthread_mutex_unlock(&socklock);
		doTheSort();
		//printf("dd\n");
		header='0';
		return "yo";
	}
	if (read(comm_fd, &fileSize, sizeof(fileSize)) == 0) //get size lines of file
	{
		//printf("2[-] Disconnected from client %d\n", listen_fd); //to be changed to ip?
		free(str);
		pthread_mutex_unlock(&socklock);
		return "no";
	}
	write(comm_fd,"q",2);
	FILE* out_file = fopen("files_sorted.txt", "a");
	char* file=malloc(ntohl(size));
	char* whole=malloc(ntohl(fileSize));	
	strcpy(recv,"hello");
	int j = 0;
	int len=0;
	for (j = 0; j < ntohl(size); j++)
    	{
 		
		bzero(str, 10000); 
       		if(read(comm_fd,str,10000) == 0) //go through each line of csv, and get the line
		{
			//printf("3[-] Disconnected from client %d\n", listen_fd); //to be changed to ip?
 			comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
			continue;
		}
		else
		{
			if(j!=0||header=='0')
			{
				header='1';
				len+=sprintf(whole+len,"%s",str);
			}
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
	if(argc==2&&strcmp(argv[1],"-h")==0)
	{
		printf("./sorter_server -p <port>\n");
		return 0;
	}
	if(argc!=3)
	{
		printf("ERROR: Incorrect number of arguments\n");
		printf("./sorter_server -p <port>\n");
		return 0;
	}
	if(strcmp(argv[1],"-p")!=0)
	{
		printf("ERROR: Incorrect number of arguments\n");
		printf("./sorter_server -p <port>\n");
		return 0;
	}

	threadIDs=malloc(sizeof(pthread_t)*5); //change 5 to how many files  
 	char state='0';
	bzero( &servaddr, sizeof(servaddr)); //zero at addresse
	 
	//int port = atoi(argv[1]); 
	
		
	if((listen_fd=socket(AF_INET,SOCK_STREAM,0))<=0) //check if sockets being used
	{
		// print error message with perror()
		perror("socket");
		// exit your server
		exit(EXIT_FAILURE);
	}

	printf("Received connections from: ");
	fflush(stdout);
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htons(INADDR_ANY);
	servaddr.sin_port = htons(strtol(argv[2],NULL,10)); //set to a random port which isn't being used
	while(1)
	{
		bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)); //bind program to that port
		
		socklen_t len = sizeof(servaddr);
		
		//if (getsockname(listen_fd, (struct sockaddr *)&servaddr, &len) == -1)
	//	{
	//		perror("getsockname");
	//		return 0;
	//	} //checl of socket binded correctly
//
//		printf("Running On Port Number: %d\n", ntohs(servaddr.sin_port)); //get port

		listen(listen_fd, 10); //allow for max 10 connections (not sure how it changes threads)
		
		FILE* fp=fopen("files_sorted.txt","w");
		fflush(fp);
		fclose(fp);
		token=malloc(30);

		struct sockaddr* client_addr;
		struct sockaddr* addr;
		int sizeaddr=sizeof(addr);
		comm_fd = accept(listen_fd, (struct sockaddr*)&addr, &sizeaddr);
		struct sockaddr_in* sai=(struct sockaddr_in*)&addr;
		//struct sockaddr_in* pV4Addr = (struct sockaddr_in*)&client_addr;
		//struct in_addr ipAddr = pV4Addr->sin_addr;
		//char str[INET_ADDRSTRLEN];
		//inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN );
//		printf("%s,",client_addr->sa_data);
		char *ip = inet_ntoa(sai->sin_addr);
//		printf("fsf\n");
//		fflush(stdout);
		printf("%s,",ip);
		fflush(stdout);
		if (read(comm_fd, token, sizeof(token)) == 0) //get size lines of file
		{
			//printf("[-] Disconnected from client %d\n", listen_fd); //to be changed to ip?
			free(token);
			pthread_mutex_unlock(&socklock);
			exit(EXIT_FAILURE);
		}
		write(comm_fd,"q",2);

		while(comm_fd = accept(listen_fd, (struct sockaddr*) NULL, NULL)) //start listening on connection 
		{
			//printf("got one\n");
			void* ret;
			pthread_mutex_lock(&m);
			pthread_t index=threadIDs[threadIndex++];
			pthread_mutex_unlock(&m);
			pthread_create(&index,NULL,&rec,NULL);
			pthread_join(index,&ret);
			char* val=(char*)ret;
			if(val[0]=='y')
			{
	/////			state='1';
				break;
				/*
				if (read(comm_fd, token, sizeof(token)) == 0) //get size lines of file
				{
					printf("[-] Disconnected from client %d\n", listen_fd); //to be changed to ip?
					free(token);
					pthread_mutex_unlock(&socklock);
					exit(EXIT_FAILURE);
				}
				write(comm_fd,"q",2);
				*/
			}
		}

	}	
	printf("\n");
	
	close(listen_fd);
	
	exit(EXIT_SUCCESS);
}
