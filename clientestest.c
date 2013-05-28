#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clientes.h"

int main()
{
	nodo_clientes* uno_a_buscar = NULL;
	nodo_clientes* lista_clientes = NULL;
	nodo_clientes* un_cliente = NULL;
	un_cliente = crear_cliente(2);
	agregar_cliente(&lista_clientes,un_cliente);
	un_cliente = NULL;
	un_cliente = crear_cliente(3);
	agregar_cliente(&lista_clientes,un_cliente);
	un_cliente = NULL;
	un_cliente = crear_cliente(4);
	agregar_cliente(&lista_clientes,un_cliente);

	listar_clientes(lista_clientes);

	quitar_cliente(&lista_clientes,3);

	listar_clientes(lista_clientes);

	un_cliente = NULL;
	un_cliente = crear_cliente(1);
	agregar_cliente(&lista_clientes,un_cliente);

	quitar_cliente(&lista_clientes,2);

	listar_clientes(lista_clientes);

	uno_a_buscar = buscar_cliente(lista_clientes,1);

	if(uno_a_buscar != NULL)
	{
		printf("Se encontro el cliente %d \n", (*uno_a_buscar).id);
	}else
	{
		printf("No se encontro nada \n");
	}

return 0;
}