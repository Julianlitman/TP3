#include "mensajeria.h"

void al_recibir_mensaje(nodo_clientes *un_cliente)
{
   printf("%c <-accion cliente\n",un_cliente->un_paquete.accion );

  switch(un_cliente->un_paquete.accion)
 {
 
 case ACCION_ACEPTADO:
    //HACER
    break;  
 
 case ACCION_CANCELAR:
    
    break;  
 
 case ACCION_IMPRIMIR:
    un_cliente->un_contenido.contenido[un_cliente->un_paquete.longitud] = '\0'; 
    printf("%s",un_cliente->un_contenido.contenido);
    break;    
  }
} 

int read_message(nodo_clientes* un_cliente){
  int result;
      if(un_cliente->buffer_pos < sizeof(struct paquete))
      {
          //printf("ID: %d \n", un_cliente->id);
          //printf("buffer pos %d\n ", un_cliente->buffer_pos);
          result = recv(un_cliente->id, &(un_cliente->un_paquete) + un_cliente->buffer_pos, 
          sizeof(struct paquete) - un_cliente->buffer_pos, MSG_DONTWAIT);
        if(result > 0)
        {
          un_cliente->buffer_pos += result;
        }

      }
      else
      {
          result = recv(un_cliente->id, &(un_cliente->un_contenido) + (un_cliente->buffer_pos - sizeof(struct paquete)), 
            sizeof(struct contenido) - un_cliente->buffer_pos + sizeof(struct paquete) , MSG_DONTWAIT);
          if(result > 0)
        {
          un_cliente->buffer_pos += result;
        }
      }

   if(sizeof(struct contenido) + sizeof(struct paquete) == un_cliente->buffer_pos)
   {

      al_recibir_mensaje(un_cliente);
      un_cliente->buffer_pos = 0;
   }
   //printf("Saliendo de read message\n");
   return result;  
}