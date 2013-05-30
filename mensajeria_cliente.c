#include "mensajeria.h"

void al_recibir_mensaje(nodo_clientes *un_cliente)
{
   printf("%c <-accion cliente",un_cliente->un_paquete.accion );

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
   do 
   {
      if(un_cliente->buffer_pos < sizeof(struct paquete))
      {
        result = recv(un_cliente->id, &(un_cliente->un_paquete) + un_cliente->buffer_pos, 
          sizeof(struct paquete) - un_cliente->buffer_pos, 0);
        if(result > 0)
        {
          un_cliente->buffer_pos += result;
        }
      }
      else
      {
          result = recv(un_cliente->id, &(un_cliente->un_contenido) + (un_cliente->buffer_pos - sizeof(struct paquete)), 
            sizeof(struct contenido) - un_cliente->buffer_pos + sizeof(struct paquete) , 0);
          if(result > 0)
        {
          un_cliente->buffer_pos += result;
        }
      }
   } while (result == -1 && errno == EINTR);
   if(un_cliente->un_paquete.longitud + sizeof(struct paquete) == un_cliente->buffer_pos  && un_cliente->buffer_pos >= 0)
   {

    al_recibir_mensaje(un_cliente);
    un_cliente->buffer_pos = 0;
   }

   return result;  
}