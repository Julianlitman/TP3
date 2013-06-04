#include "mensajeria.h"


void al_recibir_mensaje(nodo_clientes *un_cliente)
{
  struct cliente *cliente_dest:
  printf("%c <-accion",un_cliente->un_paquete.accion );
  switch(un_cliente->un_paquete.accion)
 {
 
 case ACCION_PEDIDO:
      if (un_cliente->estado == ESTADO_ESPERANDO)
      {
          cliente_dest = buscar_cliente(clientes_conectados,un_cliente->paquete.user_dest);
          if (cliente_dest->estado == ESTADO_ESPERANDO)
          {
            un_cliente->estado = ESTADO_ENVIANDO;
            cliente_dest->estado = ESTADO_RECIBIENDO;
            un_paquete.accion = ACCION_LISTAR;
            un_paquete.user_dest = 0;
            un_paquete.user_orig = sockfd;
            un_paquete.longitud = 0;
            send(sockfd, &un_paquete, sizeof(struct paquete), 0 );
            send(sockfd, &un_contenido, sizeof(struct contenido), 0 );
          }
      {  
      }
   break;

 case ACCION_ECHO:   
  
    break;
 
 case ACCION_CANCELAR:
    
    break;  
 
 case ACCION_IMPRIMIR:
  
    break;  
 
 case ACCION_LISTAR:    
  listar_clientes_remoto(clientes_conectados, un_cliente->id);
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
   if(sizeof(struct contenido) + sizeof(struct paquete) == un_cliente->buffer_pos)
   {

    al_recibir_mensaje(un_cliente);
    un_cliente->buffer_pos = 0;
   }

   return result;  
}