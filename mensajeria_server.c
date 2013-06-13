#include "mensajeria.h"


void enviar_cancelar(nodo_clientes *un_cliente)
{
  printf("cliente destino %d \n",un_cliente->otro_id);
  struct paquete un_paquete;
  struct contenido un_contenido;
  un_paquete.accion = ACCION_CANCELAR;
  un_paquete.user_dest = un_cliente->otro_id;
  un_paquete.user_orig = un_cliente->id;
  un_cliente->estado = ESTADO_ESPERANDO;
  un_paquete.longitud = 0;
  send(un_cliente->otro_id, &un_paquete, sizeof(struct paquete), 0 );
  send(un_cliente->otro_id, &un_contenido, sizeof(struct contenido), 0 );   
}

void al_recibir_mensaje(nodo_clientes *un_cliente)
{
  struct clientes *cliente_dest = NULL;
  struct paquete un_paquete;
  struct contenido un_contenido;
  char linea_buffer[100];
  switch(un_cliente->un_paquete.accion)
  {
 
 case ACCION_PEDIDO:
      if (un_cliente->estado == ESTADO_ESPERANDO)
      {
          cliente_dest = buscar_cliente(clientes_conectados,un_cliente->un_paquete.user_dest);

          if (cliente_dest != NULL && cliente_dest->estado == ESTADO_ESPERANDO)
          {
            
            cliente_dest->otro_id = un_cliente->id;
            cliente_dest->estado = ESTADO_RECIBIENDO;
            un_paquete.accion = ACCION_AVISADO;
            un_paquete.user_dest = cliente_dest->id;
            un_paquete.user_orig = un_cliente->id;
            un_paquete.longitud = un_cliente->un_paquete.longitud;
            send(cliente_dest->id, &un_paquete, sizeof(struct paquete), 0 );
            send(cliente_dest->id, &un_cliente->un_contenido, sizeof(struct contenido), 0 );

            un_cliente->otro_id = cliente_dest->id;
            un_cliente->estado = ESTADO_ENVIANDO;
            un_paquete.accion = ACCION_EMPEZAR;
            un_paquete.user_dest = un_cliente->id;
            un_paquete.user_orig = cliente_dest->id;
            un_paquete.longitud = 0;
            send(un_cliente->id, &un_paquete, sizeof(struct paquete), 0 );
            send(un_cliente->id, &un_contenido, sizeof(struct contenido), 0 );
          }
          else
          {

            sprintf(linea_buffer,"No se puede realizar la transaccion solicitada (100) \n");
            imprimirRemoto(linea_buffer,un_cliente->id);
            enviar_cancelar(un_cliente);
            
          }
        
      }
   break;

    case ACCION_MANDAR:
      if (un_cliente->estado == ESTADO_ENVIANDO)
      {
          cliente_dest = buscar_cliente(clientes_conectados,un_cliente->otro_id);
          printf("soy el id del cliente destino -> %d \n", cliente_dest->id);
          if (cliente_dest != NULL && cliente_dest->estado == ESTADO_RECIBIENDO)
          {
            send(cliente_dest->id, &un_cliente->un_paquete, sizeof(struct paquete), 0 );
            send(cliente_dest->id, &un_cliente->un_contenido, sizeof(struct contenido), 0 );

          }
          else
          {
            sprintf(linea_buffer,"No se puede realizar la transaccion solicitada (101) \n");
            imprimirRemoto(linea_buffer,un_cliente->id); 
          }
        
      }
   break;
 
 case ACCION_CANCELAR:
    
    if (un_cliente->otro_id > 0)
      {
            cliente_dest = buscar_cliente(clientes_conectados,un_cliente->otro_id);
            un_cliente->estado = ESTADO_ESPERANDO;
            
            if (cliente_dest != NULL)
            {
             //Lo hice funcion_cancelar
              enviar_cancelar(un_cliente);
              un_cliente->otro_id = 0;
              cliente_dest->estado = ESTADO_ESPERANDO;
              cliente_dest->otro_id = 0;
            }
      }
    break;  
 
 case ACCION_LISTAR:    
  listar_clientes_remoto(clientes_conectados, un_cliente->id);
    break;

 case ACCION_FIN:    
    if (un_cliente->estado == ESTADO_ENVIANDO)
      {
          cliente_dest = buscar_cliente(clientes_conectados,un_cliente->otro_id);
          if (cliente_dest != NULL && cliente_dest->estado == ESTADO_RECIBIENDO)
          {
            un_cliente->cant_archivos += 1;
            cliente_dest->otro_id = 0;
            cliente_dest->estado = ESTADO_ESPERANDO;
            un_cliente->otro_id = 0;
            un_cliente->estado = ESTADO_ESPERANDO;
            send(cliente_dest->id, &un_cliente->un_paquete, sizeof(struct paquete), 0 );
            send(cliente_dest->id, &un_cliente->un_contenido, sizeof(struct contenido), 0 );

          }
          else
          {
            sprintf(linea_buffer,"No se puede realizar la transaccion solicitada (101) \n");
            imprimirRemoto(linea_buffer,un_cliente->id); 
          }
        
      }
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

