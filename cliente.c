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

void cerrar_todo(int signal);
void* menu_cliente(void * arg);
int getFileList(int fd, const char* directory);
int sendFile(int fd, const char* directory, const char* filename);
void logger(const char *text);
int startClient();
nodo_clientes un_cliente;
int msgid = 0;

void cancelar(){
	if(un_cliente.estado == ESTADO_RECIBIENDO || un_cliente.estado == ESTADO_ENVIANDO)
    {
      close(un_cliente.fd_archivo);
      if(un_cliente.estado == ESTADO_RECIBIENDO)
      {
          unlink(un_cliente.nombre_archivo);  
      }
    }
}

int main()
{

	signal(SIGINT,cerrar_todo);
	msgid = msgget(IPC_PRIVATE,IPC_CREAT|0777);
	printf("%ld, %ld, %ld, %ld \n",sizeof(int), sizeof(char), sizeof(struct paquete), sizeof(struct contenido));
	startClient();
	//Cierro la cola de mensajes


	msgctl(msgid,IPC_RMID,0);
    return EXIT_SUCCESS;
}

void logger(const char *text) {
  printf("%s\n", text);
}
void enviar_proximo_paquete(nodo_clientes *un_cliente)
{
	struct paquete un_paquete;
	struct contenido un_contenido;
	int leido = read(un_cliente->fd_archivo,un_contenido.contenido,TAMANO_BODY_PAQUETE);
	if (leido > 0)
	{
	  printf(".");
	  un_paquete.accion = ACCION_MANDAR;
	  un_paquete.user_orig = un_cliente->id;
	  un_paquete.user_dest = un_cliente->otro_id;
	  un_paquete.longitud = leido;
	  send(un_cliente->id, &un_paquete, sizeof(struct paquete), 0);
	  send(un_cliente->id, &un_contenido, sizeof(struct contenido), 0);
	}
	else if(leido == 0)
	{
	  printf("Fin de transmisión \n");
	  un_cliente->estado = ESTADO_ESPERANDO;
	  un_paquete.accion = ACCION_FIN;
	  un_paquete.user_orig = un_cliente->id;
	  un_paquete.user_dest = un_cliente->otro_id;
	  un_paquete.longitud = 0;
	  send(un_cliente->id, &un_paquete, sizeof(struct paquete), 0);
	  send(un_cliente->id, &un_contenido, sizeof(struct contenido), 0); 
	}
	else if(leido == -1)
	{
		//Da un error la lectura del archivo
		cancelar();
		enviar_cancelar(un_cliente);
	}
}

int startClient(){
	int sockfd, portno;
	pthread_t thid;
	

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
    un_cliente.estado = ESTADO_ESPERANDO;
	un_cliente.id = sockfd;
	un_cliente.buffer_pos = 0;

	struct mensaje un_mensaje;
	struct paquete un_paquete;
	struct contenido un_contenido;
    int nada_para_hacer = 1;   
    int salir = 0;
	while(salir == 0)
	{
		nada_para_hacer = 1;				   	
		int leido = read_message(&un_cliente);
		if (leido > 0 )
		{
			nada_para_hacer = 0;
		}else if (leido == 0)
		{
			break;
		}
		//printf("%d <-lei esto \n", leido);	
		int ret2 = msgrcv(msgid, &un_mensaje, ((sizeof(struct mensaje)) - (sizeof(long))), 1, 0 | IPC_NOWAIT); 
		if (ret2 > 0)
		{

			nada_para_hacer  = 0;
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
				    if(un_cliente.estado != ESTADO_ESPERANDO)
				    {
				    	printf("No puede pedir si no esta esperando. Mi estado es: %d \n",un_cliente.estado);
				    	break;
				    }
				    un_cliente.estado = ESTADO_ESPERANDO_ENVIAR;
				    un_paquete.accion = ACCION_PEDIDO;
				    un_cliente.otro_id = un_mensaje.user_dest;
				    un_paquete.user_dest = un_mensaje.user_dest;
					un_paquete.user_orig = sockfd;
					strcpy(un_cliente.nombre_archivo,un_mensaje.nombre);
					un_paquete.longitud = strlen(un_mensaje.nombre);
					strcpy(un_contenido.contenido,un_mensaje.nombre);
					send(sockfd, &un_paquete, sizeof(struct paquete), 0 );
					send(sockfd, &un_contenido, sizeof(struct contenido), 0 );	
					
					break;

				case ACCION_SALIR:
					//Limpiar conexiones
					salir = 1;
					
					break;

				case ACCION_CANCELAR:
					cancelar();
					enviar_cancelar(&un_cliente);
					break;		

			}

		}
		if (un_cliente.estado == ESTADO_ENVIANDO )
		{
				enviar_proximo_paquete(&un_cliente);
				nada_para_hacer = 0;
			//Manda un paquete

		}
		//

			if (nada_para_hacer == 1)
			{
				sleep(1);
			}
	}
	close(sockfd);
	pthread_join(thid,NULL);
    return 1;
}





void* menu_cliente(void * arg)
{

	int Opcion;
	struct mensaje un_mensaje;

	while(1)
	{

		printf("Elija una opcion \n");
		printf("1- Ver el listado de usuarios conectados \n");	
		printf("2- Enviar un archivo \n");
		printf("3- Cancelar transaccion \n");
		printf("4- Salir del programa \n");
		scanf("%d", &Opcion);
		un_mensaje.mtype = 1; //(DESTINATARIO)


		switch(Opcion)
		{
			case 1:
				un_mensaje.accion = ACCION_LISTAR;
				msgsnd(msgid,&un_mensaje, ((sizeof(struct mensaje)) - (sizeof(long))), 0);
				//Le resto lo que es obligatorio
				//Muestra listado de usuarios conectados, tienen que estar en una lista
				break;
			
			case 2:
			//Elije cliente y le envia un archivo
				printf("Ingrese el ID del destinatario \n");
				scanf("%d", &un_mensaje.user_dest);
				printf("Ingrese el nombre del archivo \n");
				scanf("%s", un_mensaje.nombre);
				un_mensaje.accion = ACCION_PEDIDO;
				msgsnd(msgid,&un_mensaje, ((sizeof(struct mensaje)) - (sizeof(long))), 0);
				break;

			case 3:
				un_mensaje.accion = ACCION_CANCELAR;
				msgsnd(msgid,&un_mensaje, ((sizeof(struct mensaje)) - (sizeof(long))), 0);
				break;
	
			case 4:
				un_mensaje.accion = ACCION_SALIR;
				msgsnd(msgid,&un_mensaje, ((sizeof(struct mensaje)) - (sizeof(long))), 0);
				return NULL;
				break;

			default:
				printf("Opcion no valida \n");
				break;
		}
	}

}

void cerrar_todo(int signal)
{
	//Cierra el archivo si esta abierto
	//Cierra conexion, cierra cola de mensajes, cierra socket y un exit con error
	printf("Cerrando todas las conexiones... \n");
	cancelar();
	//Cola de mensajes
	if (msgid > 0)
	{
		msgctl(msgid,IPC_RMID,0);
	}
	//Socket
	if(un_cliente.id > 0)
	{
		close(un_cliente.id);
	}
	exit(EXIT_SUCCESS);
}
