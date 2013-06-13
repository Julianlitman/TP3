#ifndef CLIENTES_H_INCLUDED
#define CLIENTES_H_INCLUDED
#include "defines.h"
#include "paquetes.h"

typedef struct clientes
{
	int id;
	int otro_id;
	int fd_archivo;
	int estado;
	int cant_archivos;
	int buffer_pos; //Hasta donde escribio
	//HACER IP
	char nombre_archivo[TAMANO_NOMBRE_ARCHIVO];
	struct paquete un_paquete;
	struct contenido un_contenido;
	struct clientes *sig;
}nodo_clientes;

void borrar_clientes(nodo_clientes **clientes);
nodo_clientes *crear_cliente(int id); 
void agregar_cliente(nodo_clientes **clientes, nodo_clientes *nuevo_cliente);
nodo_clientes *buscar_cliente(nodo_clientes *clientes, int id);
void quitar_cliente(nodo_clientes **cliente, int id);
int imprimirRemoto(char* buffer,int fd);
void listar_clientes(nodo_clientes *cliente);
void listar_clientes_remoto(nodo_clientes *cliente, int fd);
nodo_clientes* clientes_conectados;


#endif // CLIENTES_H_INCLUDED
