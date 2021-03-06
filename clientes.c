#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "clientes.h"
#include "paquetes.h"

/**
*   Borra la lista completa de clientes de memoria.
*/
void borrar_clientes(nodo_clientes **clientes)
{
	nodo_clientes *a_borrar;
	while(*clientes != NULL)
	{
		a_borrar = *clientes;
		*clientes = (*clientes)->sig;
		free(a_borrar);
	}
	return;
}

/**
*   cllocatea e inicicliza un nodo_clientes suelto.
*/
nodo_clientes *crear_cliente(int id)
{
	nodo_clientes *a;
	a = (nodo_clientes*)malloc(sizeof(nodo_clientes));
	a->id = id;
	a->otro_id = 0;
	a->estado = 0;
	a->cant_archivos = 0;
	a->buffer_pos = 0;
	a->sig = NULL;
	return a;
}


/**
*   Agrega un cliente cl fincl de la lista de clientes
*/
void agregar_cliente(nodo_clientes **clientes, nodo_clientes *nuevo_cliente)
{
	nodo_clientes* un_cliente;
	if(*clientes == NULL)
    {
        *clientes = nuevo_cliente;
    }
    else
    {
    	un_cliente = *clientes;
    	while(un_cliente->sig != NULL)
    	{
    		un_cliente = un_cliente->sig;	
    	}

	    un_cliente->sig = nuevo_cliente;
	    nuevo_cliente->sig = NULL;
        
    }
}
    

void quitar_cliente(nodo_clientes **cliente, int id)
{
	nodo_clientes* un_cliente; 
	if(*cliente == NULL)
	{
		return;
	}
	else if((*cliente)->id == id)
	{
		un_cliente = *cliente;
		*cliente = un_cliente->sig;
		free(un_cliente);
		return; 
	}

	un_cliente = *cliente;
	
	while(un_cliente->sig != NULL && un_cliente->sig->id != id)
	{
		un_cliente = un_cliente->sig;
	}


	if(un_cliente->sig != NULL)
	{
		nodo_clientes* otro_cliente;

		otro_cliente = un_cliente->sig;
	    un_cliente->sig = otro_cliente->sig;
		free(otro_cliente);
		return; 
	}
	

	return;	
}


/**
*   Busca un cliente por nro_lagajo
*/
nodo_clientes *buscar_cliente(nodo_clientes *clientes, int id)
{
	while(clientes != NULL)
	{
		if(clientes->id == id)
		{
			break;
		}
		clientes = clientes->sig;
	}

	return clientes;
}

/**
*   Lista clienteslibros de un cliente
*/
int imprimirRemoto(char* buffer,int fd)
{
	struct contenido un_contenido;
	memcpy(un_contenido.contenido, buffer, TAMANO_CONTENIDO);
	
	struct paquete response;
	response.accion = ACCION_IMPRIMIR;
	response.user_dest = fd;
	response.longitud = strlen(buffer);

	printf("Enviando al cliente: %d \n",fd);
	send(fd, &response, sizeof(struct paquete), 0);
	send(fd, &un_contenido, sizeof(struct contenido), 0);
	return 1;
}

void listar_clientes_remoto(nodo_clientes *cliente, int fd)
{
	char buffer[100];
	sprintf(buffer,"Imprimiendo clientes \n");
	imprimirRemoto(buffer,fd);
	char estado[50] = "";

	while(cliente != NULL)
	{

		 switch(cliente->estado)
		 {
		 	case 1:
		 		strcpy(estado,"Recibiendo");
		 		break;
		 	case 2:
		 		strcpy(estado,"Enviando");
		 		break;

		 	case 3:
		 		strcpy(estado,"Esperando");
		 		break;
			case 4:
		 		strcpy(estado,"Cancelando");
		 		break;		 			
		 	case 5:
		 	strcpy(estado,"Esperando para enviar");
		 		break;
		 }
		sprintf(buffer,"ID: %d Archivos enviados: %d  Estado: %s   \n",cliente->id ,cliente->cant_archivos, estado);
		imprimirRemoto(buffer,fd);
		cliente = cliente->sig;
	}
	
	return;
}

void listar_clientes(nodo_clientes *cliente)
{
	
	printf("Imprimiendo clientes \n");

	while(cliente != NULL)
	{
		printf("ID: %d\n",cliente->id);
		printf("otro_cliente: %d\n",cliente->otro_id);
		printf("Estado: %d\n",cliente->estado);
		printf("\n");
		cliente = cliente->sig;
	}
	
	return;
}

