#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <netdb.h> 
// agrego h
#include "mensajeria.h"

//#include <sys/socket.h>
//#include <netinet/in.h>

void* menu_cliente(void * arg);
int getFileList(int fd, const char* directory);
int sendFile(int fd, const char* directory, const char* filename);
void logger(const char *text);
int startClient();
nodo_clientes un_cliente;
int msgid;

int main()
{

	msgid = msgget(IPC_PRIVATE,IPC_CREAT|0777);
	printf("%ld, %ld, %ld, %ld \n",sizeof(int), sizeof(char), sizeof(struct paquete), sizeof(struct contenido));
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
	int sockfd, portno;
	pthread_t thid;
	void * retval;

    struct sockaddr_in serv_addr;
    struct hostent *server;


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

	//meter thread
    pthread_create(&thid,NULL, &menu_cliente, NULL);
    logger("ACA YA ESTOY CONECTADO!\n");
    //pthread_join(thid, &retval);

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
	un_cliente.id = sockfd;
	un_cliente.buffer_pos = 0;

	struct mensaje un_mensaje;
	struct paquete un_paquete;
	struct contenido un_contenido;
        

	while(1)
	{
				   	
		int leido = read_message(&un_cliente);
		//printf("%d <-lei esto \n", leido);	
		int ret2 = msgrcv(msgid, &un_mensaje, ((sizeof(struct mensaje)) - (sizeof(long))), 1, 0 | IPC_NOWAIT); 
		if (ret2 > 0)
		{
			switch(un_mensaje.accion)
			{
				case ACCION_LISTAR:
				    un_paquete.accion = ACCION_LISTAR;
				    un_paquete.user_dest = 0;
					un_paquete.user_orig = sockfd;
					un_paquete.longitud = 0;
					send(sockfd, &un_paquete, sizeof(struct paquete), 0 );
					send(sockfd, &un_contenido, sizeof(struct contenido), 0 );
					break;

				case ACCION_PEDIDO:
				    un_paquete.accion = ACCION_PEDIDO;
				    un_paquete.user_dest = un_mensaje.user_dest;
					un_paquete.user_orig = sockfd;
					un_paquete.longitud = strlen(un_mensaje.nombre);
					strcpy(un_contenido.contenido,un_mensaje.nombre);
					send(sockfd, &un_paquete, sizeof(struct paquete), 0 );
					send(sockfd, &un_contenido, sizeof(struct contenido), 0 );	
					break;

				case ACCION_SALIR:
					pthread_join(thid,NULL);
					//Limpiar conexiones
					exit(0);
					break;

			}
		}

		if(leido < 1) //me voy a dormir
		{
			sleep(1);
		}
	}

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

/*int getFileList(int fd, const char* directory)
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
*/
void* menu_cliente(void * arg)
{

	int Opcion;
	struct mensaje un_mensaje;
	int ret;

	while(1)
	{

		printf("Elija una opcion \n");
		printf("1- Ver el listado de usuarios conectados \n");	
		printf("2- Enviar un archivo \n");
		printf("3- Salir del programa \n");
		scanf("%d", &Opcion);
		un_mensaje.mtype = 1; //(DESTINATARIO)


		switch(Opcion)
		{
			case 1:
				un_mensaje.accion = ACCION_LISTAR;
				ret = msgsnd(msgid,&un_mensaje, ((sizeof(struct mensaje)) - (sizeof(long))), 0);
				//Le resto lo que es obligatorio
				//Muestra listado de usuarios conectados, tienen que estar en una lista
				break;
			
			case 2:
			//Elije cliente y le envia un archivo
				printf("Ingrese el ID del destinatario \n");
				scanf("%d", &un_mensaje.user_dest);
				printf("Ingrese el nombre del archivo \n");
				scanf("%s", &un_mensaje.nombre);
				un_mensaje.accion = ACCION_PEDIDO;
				ret = msgsnd(msgid,&un_mensaje, ((sizeof(struct mensaje)) - (sizeof(long))), 0);
				break;

			case 3:
				un_mensaje.accion = ACCION_SALIR;
				ret = msgsnd(msgid,&un_mensaje, ((sizeof(struct mensaje)) - (sizeof(long))), 0);
				return NULL;
				break;

			default:
				printf("Opcion no valida \n");
				break;
		}
	}

}