#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

#define PORT 3456
#define UNIX_SOCK_PATH "/tmp/lab3.sock"
#define MAX_BUFFER_SIZE  1024
#define ESTADO_RECIBIENDO 1
#define ESTADO_ENVIANDO   2
#define ESTADO_ESPERANDO  3
#define ESTADO_CANCELANDO 4
#define ESTADO_ESPERANDO_ENVIAR 5
#define ACCION_PEDIDO    'P'
#define ACCION_ECHO      'E'
#define ACCION_CANCELAR  'C'
#define ACCION_AVISADO   'A' //Te avisa que vas a recibir
#define ACCION_LISTAR    'L'
#define ACCION_IMPRIMIR  'I'
#define ACCION_SALIR     'S'
#define ACCION_FIN    	 'F'
#define ACCION_MANDAR    'M'
#define ACCION_EMPEZAR   'J'
#define TAMANO_NOMBRE_ARCHIVO    100
#define TAMANO_MAXIMO_PAQUETE    1024
#define TAMANO_HEADER_PAQUETE 	 13
#define TAMANO_BODY_PAQUETE 	 1011
#define TAMANO_BUFFER_CLIENTE    2048
#define TAMANO_CONTENIDO 1011   



struct mensaje
{
long mtype;
char nombre[100];
char accion;
int user_dest;
};

#endif // DEFINES_H_INCLUDED
