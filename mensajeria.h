#ifndef FUNCIONES_MENSAJERIA_H_INCLUDED
#define FUNCIONES_MENSAJERIA_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h> 
#include <errno.h> 
#include <sys/ipc.h>
#include <pthread.h>
#include <sys/msg.h>

// agrego h
#include "defines.h"
#include "paquetes.h"
#include "clientes.h"

void al_recibir_mensaje(nodo_clientes *un_cliente);
int read_message(nodo_clientes* un_cliente);




#endif  
