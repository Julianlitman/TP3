#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

//#include <sys/socket.h>
//#include <netinet/in.h>
#include <netdb.h> 

int getFileList(int fd, const char* directory);
int sendFile(int fd, const char* directory, const char* filename);
void logger(const char *text);
int startClient();

int main()
{
	startClient();
/*    printf("Hello world!\n");
    
    creat("test_tp.txt", 0777);
	//fd file descriptor. -1 error.
	int fd = open("test_tp.txt", O_RDWR | O_TRUNC);    
    getFileList(fd,"./");

    sendFile(fd,"./","salida.txt");
    close(fd);
*/
    return EXIT_SUCCESS;
}

void logger(const char *text) {
  printf("%s\n", text);
}

int startClient(){
	int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];


    portno = 3456;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){    	    	
        logger("Al abrir socket(106)");
        return 0;
    }

    server = gethostbyname("localhost");
    if (server == NULL) {
        //fprintf(stderr,"ERROR, no such host\n");
        logger("No se encontro el host (107)");
        close(sockfd); // creoq ue no va;
        return 0;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    {
        logger("ERROR connecting");
        close(sockfd);
        return 0;

	}    
    logger("ACA YA ESTOY CONECTADO!\n");

/*
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error("ERROR writing to socket");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    */
    close(sockfd);
    return 1;
}


int sendFile(int fd, const char* directory, const char* filename)
{
	char fullpath[1024];
	unsigned char buffer[1024];
	int read_size = 0;
	int write_size = 0;

	strcpy(fullpath,directory);
	strcat(fullpath, filename);

	int readfd = open(fullpath, O_RDONLY);
	if(readfd == 0){
		perror("Al leer archivo (104)");
		return 0;
	}

	lseek(readfd, 0, SEEK_SET);
		
	while((read_size = read(readfd, buffer, 1024)) > 0)
	{
		write_size = write(fd, buffer, read_size);
		if(write_size == -1)
		{
			perror("Al escribir en fd (105)");
			break;
		}
	}

	if(read_size < 0)
	{
		perror("Al leer archivo (106)");		
	}
	
	close(readfd);
	return 1;
}

int getFileList(int fd, const char* directory)
{

	struct dirent* direntry;
	struct stat statinfo;	    
 	DIR* dir;

 	dir = opendir(directory); //TODO recibirlo por argumento.

 	if(dir == NULL){
 		perror("Al abrir directorio (100)"); 		
 		return 0;
 	}

 	direntry = readdir(dir);
	if(direntry == NULL){
		perror("Al leer directorio (101)");
		closedir(dir);
		return 0;
	}

//statinfo = (struct stat*) malloc(sizeof(struct stat));
	char fullpath[1024];
	char buffer[1024];


	while(direntry != NULL){		
		if(strcmp(direntry->d_name, ".") != 0 &&  strcmp(direntry->d_name, "..") != 0)
		{
			strcpy(fullpath,directory);
			strcat(fullpath, direntry->d_name);

			if(stat(fullpath, &statinfo) == -1){
				perror("Al obtener informacion(103)");	
			}else{
				sprintf(buffer,"%s\t%lld\n\0" ,direntry->d_name,(long long) statinfo.st_size);
				write(fd, buffer, strlen(buffer));
			} 	
		}

		direntry = readdir(dir);			
	}
	
	sprintf(buffer, "\n\0");
	write(fd, buffer, strlen(buffer));

	closedir(dir);
//	free(statinfo);
	return 1;	
}