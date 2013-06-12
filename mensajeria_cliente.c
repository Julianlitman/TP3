#include "mensajeria.h"

void al_recibir_accion_mandar(nodo_clientes *un_cliente)
{
    int escribi = 0;
    int acumulado = 0;
    if(un_cliente->estado != ESTADO_RECIBIENDO)
    {
        enviar_cancelar(un_cliente);
    }
    else
    {
      do
      {
        escribi = write(un_cliente->fd_archivo, un_cliente->un_contenido.contenido, un_cliente->un_paquete.longitud);
        acumulado = acumulado + escribi;
       }while(escribi != -1 && acumulado != un_cliente->un_paquete.longitud);
        
       if(escribi == -1)
       {
          enviar_cancelar(un_cliente);
       }
     }
}

void al_recibir_mensaje(nodo_clientes *un_cliente)
{
   printf("%c <-accion cliente\n",un_cliente->un_paquete.accion );

  switch(un_cliente->un_paquete.accion)
 {
 
 case ACCION_AVISADO:
    //Le avisa que va a empezar a recibir en el contenido viene el nombre del archivo
    //Crea el archivo y abre un puntero al archivo
    
    un_cliente->un_contenido.contenido[un_cliente->un_paquete.longitud] = '\0';
    if(creat(un_cliente->un_contenido.contenido,0777) != -1)
    {
      int fd = open(un_cliente->un_contenido.contenido, O_RDWR | O_TRUNC); //Abre archivo
      strcpy(un_cliente->nombre_archivo, un_cliente->un_contenido.contenido);
      if(fd!= -1)
      {
        un_cliente->fd_archivo = fd; 
        un_cliente->estado = ESTADO_RECIBIENDO;
        break;
      }  
    }  //Crea archivo. Recibe nombre y permisos.
    
    //si fallo algo cancelo.
    enviar_cancelar(un_cliente);
    break;  

  case ACCION_MANDAR: 
    //Tengo que guardar lo que me traiga
    //Si estoy en estado recibiendo y el fd != 0, intento guardar
    if(un_cliente->estado == ESTADO_RECIBIENDO && un_cliente->fd_archivo != 0)
    {
    al_recibir_accion_mandar(un_cliente);
    break;
    }
    enviar_cancelar(un_cliente);
    break;  
  case ACCION_EMPEZAR:
    //Da la orden para comenzar el envio.
    //o ya tengo el fd abieto de antes
  
    if(un_cliente->estado == ESTADO_ESPERANDO_ENVIAR)
    {
      // intentar abrir el archivo, si algo falla tirar un cancelar e ir a esperando
      //Sino pasar a estado enviando
      int fd = open(un_cliente->nombre_archivo, O_RDONLY); //Abre archivo
      if(fd!= -1)
      {
        un_cliente->buffer_pos = 0;
        un_cliente->fd_archivo = fd; 
        un_cliente->estado = ESTADO_ENVIANDO;
        break;   
      }
      else
      {
        perror("soy el error de cuando tratan de abrir el fd \n");
      }
    }
      enviar_cancelar(un_cliente); 
    break; 
 
    case ACCION_FIN:
    //Me avisa que termino el archivo, cierro fd. Paso a estado esperando.
    if(un_cliente->estado == ESTADO_RECIBIENDO || un_cliente->estado == ESTADO_ENVIANDO)
    {
      close(un_cliente->fd_archivo);
      un_cliente->estado = ESTADO_ESPERANDO;
      un_cliente->otro_id = 0;
    }
    break;

 case ACCION_CANCELAR:
    if(un_cliente->estado == ESTADO_RECIBIENDO || un_cliente->estado == ESTADO_ENVIANDO)
    {
      close(un_cliente->fd_archivo);
      if(un_cliente->estado == ESTADO_RECIBIENDO)
      {
          unlink(un_cliente->nombre_archivo);  
      }
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