#ifndef CLIENTES_H_INCLUDED
#define CLIENTES_H_INCLUDED

typedef struct clientes
{
	int id;
	int otro_id;
	int estado;
	int tamano;
	int tamano_restante;
	struct clientes *sig;
}nodo_clientes;

void borrar_clientes(nodo_clientes **clientes);
nodo_clientes *crear_cliente(int id); 
void agregar_cliente(nodo_clientes **clientes, nodo_clientes *nuevo_cliente);
nodo_clientes *buscar_cliente(nodo_clientes *clientes, int id);
void quitar_cliente(nodo_clientes **cliente, int id);
void listar_clientes(nodo_clientes *cliente);
#endif // CLIENTES_H_INCLUDED
