#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "paquetes.h"



struct paquete *crear_paquete(char* buffer)
{
	long pos = 0;
	struct paquete *a;
	a = (struct paquete*)malloc(sizeof(struct paquete));

	a->accion 	 = (char)(*(buffer+pos));
	pos = pos + sizeof(char);
	a->user_dest = (int)(*(buffer+pos));
	pos = pos + sizeof(int);
	a->user_orig = (int)(*(buffer+pos));
	pos = pos + sizeof(int);
	a->longitud = (int)(*(buffer+pos));
	pos = pos + sizeof(int);
	memcpy(a->contenido,(buffer+pos),a->longitud);
	//En vez de usar char[] podemos hacerlo dinamicamente con char*
	return a;

	//Probar reemplazar todo esto por un memcpy!
}
