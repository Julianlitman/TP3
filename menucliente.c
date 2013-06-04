#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>	
#include <signal.h>
#include <pthread.h>

int menu_cliente(void * arg);

int menu_cliente(void * arg)
{
int Opcion;
printf("Elija una opcion \n");
printf("1- Ver el listado de usuarios conectados \n");	
printf("2- Enviar un archivo \n");
printf("3- Salir del programa \n");
scanf("%d", &Opcion);

switch(Opcion)
{
	case 1:
	//Muestra listado de usuarios conectados, tienen que estar en una lista
	break;
	
	case 2:
	//Elije cliente y le envia un archivo
	break;

	case 3:
	printf("Saliendo del cliente \n");
	exit(0);
	break;

	default:
	break;
}
return Opcion;
}