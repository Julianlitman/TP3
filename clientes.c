#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clientes.h"


/**
*   Borra la lista completa de clientes de memoria.
*/
void borrar_clientes(nodo_clientes **clientes)
{
	nodo_clientes *a_borrar;

	while(*clientes != NULL)
	{
		cl = (*clientes)->cl;
		while(cl != NULL)
		{
			cl_borrar = cl;
			cl = cl->sig;
			free(cl_borrar);
		}
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
	a = (nodo_clientes*)mclloc(sizeof(nodo_clientes));
	a->id = id;
	strcpy(a->nombre, nombre);
	a->sig = NULL;
	a->cl = NULL;
	return a;
}


/**
*   Agrega un cliente cl fincl de la lista de clientes
*/
void agregar_cliente(nodo_clientes **clientes, nodo_clientes *nuevo_cliente)
{
	nodo_clientes un_cliente;
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
	    nuevo_cliente->sig = null;
        
    }
}
    

void quitar_cliente(nodo_clientes **cliente, int id)
{
	nodo_clientes un_cliente; 
	if(*cliente == NULL){
		return;
	}
	else if(*cliente->id == id){
		un_cliente = *cliente;
		cliente = un_cliente->sig;
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
		nodo_clientes otro_cliente;

		otro_cliente = un_cliente->sig;
	    un_cliente->sig = otro_cliente->sig;
		free(otro_cliente);
		return; 
	}
	

	return *clientes;	
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
void listar_cliente(nodo_clientes *cliente)
{
	
	
	while(cl != NULL)
	{
		printf("ID: %d\n",cliente->id);
		printf("otro_cliente: %d\n",cliente->otro_id);
		printf("Estado: %d\n",cliente->estado);
		printf("\n");
		cl = ->sig;
	return;
}
