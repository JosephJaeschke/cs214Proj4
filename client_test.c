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
#include <ftw.h>
#include <pthread.h>
#include <errno.h>

#define SIZEOF_STRING 10000

int sockfd;
struct sockaddr_in servaddr;
int number_of_files=0;
pthread_t threads[10000];
int thread_index=0;
pthread_mutex_t m;
pthread_mutex_t socklock;
char* type_global;

struct arg
{
	char* filePath;
	char* sortType;
};

void talk(char* fpath,char* type)
{
	pthread_mutex_lock(&socklock);
	//printf("talk\n");
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	fflush(stdout);
	char * sendline = malloc(SIZEOF_STRING);
 
    	if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1)
	{
		printf("ERROR : Failed to connect to server [%s]\n",strerror(errno));
		pthread_mutex_unlock(&socklock);
		exit(EXIT_FAILURE);
	}

	char* filename1 = malloc(100);
	
	strncpy(filename1, fpath, strlen(fpath));
	filename1[strlen(fpath) + 1] = '\0';
//FileName	//printf("%s\n", filename1);
	FILE * fp = fopen(filename1, "r");
	int row_position = 0;
	int j = 0;
	int file_count = 0;
	int i = 0;
	char c = getc(fp);
	while (c != EOF)
	{
		if(c == '\n')
		{
			file_count++;
		}
		i++;
		c = getc(fp);
    	}


	fclose(fp);
	char * str_file = malloc(sizeof(char) * (i + 1));
	FILE * fp1 = fopen(filename1, "r");
	if(fp1 == NULL)
	{
		pthread_mutex_unlock(&socklock);
		exit(1);
	}
	c = getc(fp1);
	i=0;
	while (c != EOF)
	{
		str_file[i] = c;
		i++;
		c = getc(fp1);
   	}
	if(fp1 != NULL)
	{
		fclose(fp1);	
	}
	if(filename1 != NULL)
	{
		free(filename1);
	}	
	
	str_file[i] = '\0';	
	
	int sentn = htonl(file_count);
	char * recvline = malloc(100);
	//read(sockfd,recvline,100);
	//printf("first write -> %d\n",ntohl(sentn));
	write(sockfd,&sentn,sizeof(sentn));   	
	read(sockfd,recvline,100);
	//printf("second write -> %d\n",i);
	write(sockfd,&i,sizeof(i));
	read(sockfd,recvline,100);
	int index1 = 0;
	int index2 = 0;

	for(j = 0; j < file_count; j++)
    	{
		//sleep(1);
		bzero(sendline, 10000);
		while(str_file[index2] != '\n')
		{
			index2++;
		}
		index2++;
		strncpy(sendline,str_file+index1, index2 - index1);
		write(sockfd,sendline,strlen(sendline)+1);
		index1 = index2; 
		read(sockfd,recvline,100);
	}
	//read(sockfd,recvline,100);    
	//printf("\nFile Sent\n");
	pthread_mutex_unlock(&socklock);
	return;
}

int isCSV(const char* name)
{
	char* temp=strdup(name);
	char* ext=strrchr(temp,'.');
	if(ext!=NULL&&strcmp(ext,".csv")==0)
	{
		if(temp!=NULL)
		{
			free(temp);
		}
		return 1;
	}
	if(temp!=NULL)
	{
		free(temp);
	}
	return 0;
}

void* threadify(void* arguments)
{
	struct arg* args=(struct arg*)arguments;
	if(isCSV(args->filePath))
	{
		talk(args->filePath,args->sortType);
	}
	return "";
}

int search(const char* fpath,const struct stat* sb,int tflag)
{
	struct arg* args=malloc(sizeof(struct arg));
	args->filePath=strdup(fpath);
	args->sortType=strdup(type_global);
	pthread_mutex_lock(&m);
	pthread_create(&threads[thread_index++],NULL,&threadify,(void*)args);
	pthread_mutex_unlock(&m);
	return 0;
}

int count_files(const char* fpath,const struct stat* sb,int tflag)
{
	number_of_files++;
	return 0;
}

int main(int argc,char **argv)
{
    char* in_dir=malloc(1000); //the -d parameter
	strcpy(in_dir,"./\0");
	type_global=malloc(100); //the -c parameter
	strcpy(type_global,"none");
	char* out_dir=malloc(1000); //the -o paramter
   	strcpy(out_dir,"./\0"); 
	int port = -1; //the -p paramter
	char* host=malloc(1000); //the -h parameter
	strcpy(host,"bad\0");
	if(argc==2&&strcmp(argv[2],"-help")==0)
	{
		printf("./sorter_client -c <column> -h <host name> -p <port>\nOptional: -o <output dir> -d <input dir>");
		return 0;
	}
	if(argc!=7&&argc!=9&&argc!=11)
	{
		printf("ERROR: Incorrect number of arguments\n");
		printf("./sorter_client -c <column> -h <host name> -p <port>\nOptional: -o <output dir> -d <input dir>");
		return 0;
	}
	int i;
	for(i=0;i<argc;i++)
	{
		if(strcmp(argv[i],"-c") == 0)
		{
			if((i % 2) == 0)
			{
				fprintf(stderr, "<ERROR> : Incorrect format, for more information , please use  $ ./sorter -help \n");
				return 0;
			}
			strcpy(type_global, argv[i+1]);
		}
		if(strcmp(argv[i], "-d") == 0)
		{
			if((i % 2) == 0)
			{
				fprintf(stderr, "<ERROR> : Incorrect format, for more information , please use  $ ./sorter -help \n");
				return 0;
			}
		strcpy(in_dir, argv[i+1]);
		}
		if(strcmp(argv[i], "-o") == 0)
		{
			if((i % 2) == 0)
			{
				fprintf(stderr, "<ERROR> : Incorrect format, for more information , please use  $ ./sorter -help \n");
				return 0;
			}
			strcpy(out_dir, argv[i+1]);
		}
		if(strcmp(argv[i], "-p") == 0)
		{
			if((i % 2) == 0)
			{
				fprintf(stderr, "<ERROR> : Incorrect format, for more information , please use  $ ./sorter -help \n");
				return 0;
			}
			port=strtol(argv[i+1],NULL,10);
		}
		if(strcmp(argv[i], "-h") == 0)
		{
			if((i % 2) == 0)
			{
				fprintf(stderr, "<ERROR> : Incorrect format, for more information , please use  $ ./sorter -help \n");
				return 0;
			}
			strcpy(host, argv[i+1]);
		}
	


	}
	if(strcmp(type_global,"none")==0||port==-1||strcmp(host,"bad\0") == 0)
	{
		printf("ERROR: Must specify a cloumn, a port, and a host\n");
		return 0;
	}

	char* junk=malloc(2);

	bzero(&servaddr,sizeof servaddr);
 
	servaddr.sin_family=AF_INET;

    	servaddr.sin_port=htons(port);
    	inet_pton(AF_INET,host,&(servaddr.sin_addr));

	int x=ftw(in_dir,count_files,0);
	number_of_files++;
	//printf("Found %d files\n",number_of_files);
//	threads=malloc(sizeof(pthread_t)*number_of_files);
	//if(threads=NULL)
	//{
	//	fprintf(stderr,"ERROR : Too many expected threads\n");
	//	free(in_dir);
	//	free(threads);
	//	exit(1);
	//}

	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1)
	{
		printf("ERROR : Failed to connect to server [%s]\n",strerror(errno));
		pthread_mutex_unlock(&socklock);
		exit(EXIT_FAILURE);
	}
	//printf("bepfre write\n");
	//printf("%s\n", type_global);
	write(sockfd,type_global,strlen(type_global));
	read(sockfd,junk,2);

	char * junk2 = malloc(2);
	write(sockfd,out_dir,strlen(out_dir));
	read(sockfd,junk2,2);


	if(ftw(in_dir,search,0)==-1)
	{
		fprintf(stderr,"ERROR : Problem in file tree walk\n");
		free(in_dir);
		exit(1);
	}

	int a;
	for(a=0;a<thread_index;a++)
	{
		pthread_join(threads[a],NULL);
	}

	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1)
	{
		printf("ERROR : Failed to connect to server [%s]\n",strerror(errno));
		pthread_mutex_unlock(&socklock);
		exit(EXIT_FAILURE);
	}
	char* recv=malloc(100);
	write(sockfd,"-1",2);
	read(sockfd,recv,2);

//==================================================================GET SORTED FILE
	//printf("rec\n");
	char * str = malloc(10000);
   	int size = 0;
	int fileSize=0;

	strcpy(recv,"hello");
	
	if (read(sockfd, &size, sizeof(size)) == 0) //get size lines of file
	{
//		printf("1[-] Disconnected from client %d\n", listen_fd); //to be changed to ip?
		free(str);
		exit(EXIT_FAILURE);
	}
	//printf("first read -> %d\n",size);
	int q=write(sockfd,"q",2);
	//printf("[+] Connect to client %d\n", listen_fd); //to be changed to ip?
	if (read(sockfd, &fileSize, sizeof(fileSize)) == 0) //get size lines of file
	{
		//printf("2[-] Disconnected from client %d\n", listen_fd); //to be changed to ip?
		free(str);
		exit(EXIT_FAILURE);
	}
	//printf("second read -> %d\n",fileSize);
	write(sockfd,"q",2);
	char* file=malloc(ntohl(size));
	char* whole=malloc(ntohl(fileSize));	
	strcpy(recv,"hello");
	int j = 0;
	int len=0;
	//printf("start getting file\n");
	for (j = 0; j < ntohl(size); j++)
    	{
 		
		bzero(str, 10000); 
       		if(read(sockfd,str,10000) == 0) //go through each line of csv, and get the line
		{
			//printf("3[-] Disconnected from client %d\n", listen_fd); //to be changed to ip?
 			//sockfd = accept(listen_fd, (struct sockaddr*) NULL, NULL);
			continue;
		}
		else
		{
			len+=sprintf(whole+len,"%s",str);
			write(sockfd, recv, strlen(recv)+1); //send back a signal to show that it's done recieving data(lets it be in order)	
		}
		 
    	}
	free(str);
	free(file);
//==================================================================GET SORTED FILE
	char* fname=malloc(1000);
	if(strcmp(out_dir, "./\0") == 0){
		//printf("a");
		sprintf(fname,"%sAllFiles-sorted-%s.csv",out_dir, type_global);
	}
	else
		//printf("b");
		sprintf(fname,"%s/AllFiles-sorted-%s.csv",out_dir, type_global);

	FILE* done=fopen(fname,"w");
	fprintf(done,"%s",whole);
	free(whole);
	fclose(done);
	return 0; 
}
