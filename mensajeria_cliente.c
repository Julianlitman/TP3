#include "mensajeria.h"

void al_recibir_mensaje(nodo_clientes *un_cliente)
{
   printf("%c <-accion cliente\n",un_cliente->un_paquete.accion );

  switch(un_cliente->un_paquete.accion)
 {
 
 case ACCION_AVISADO:
    //HACER
    //Le avisa que va a empezar a recibir en el contenido viene el nombre del archivo
    //Crea el archivo y abre un puntero al archivo
    
    un_cliente->un_contenido.contenido[un_cliente->un_paquete.longitud] = '\0';
    if(creat(un_cliente->un_paquete.nombre,0777) != -1)
    {
      int fd = open(un_cliente->un_paquete.nombre, O_RDWR | O_TRUNC); //Abre archivo
      if(fd!= -1)
      {
        un_cliente->fd_archivo = fd; 
        un_cliente->estado = ESTADO_RECIBIENDO;
        break;
      }  
    }  //Crea archivo. Recibe nombre y permisos.
    
    //si fallo algo cancelo.
    enviar_cancelar(un_cliente->id, un_cliente->un_paquete.user_orig);
    break;  

  case ACCION_MANDAR: 

    read(un_cliente->fd_archivo, )  
    
    write(un_cliente->fd_archivo, un_cliente->un_contenido, un_cliente->un_paquete.longitud);
    break;  
  case ACCION_EMPEZAR:
    //Da la orden para comenzar el envio.
    break; 
 
 case ACCION_CANCELAR:
    if(un_cliente->estado == ESTADO_RECIBIENDO || un_cliente->estado == ESTADO_ENVIANDO)
    {
      close(un_cliente->fd_archivo);
      un_cliente->estado = ESTADO_ESPERANDO;
      un_cliente->otro_id = 0;
    }
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