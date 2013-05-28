// gcc  -o server server.c -ggdb -Wall

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
// sockets => 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/un.h>
// select => 
#include <sys/time.h>
#include <sys/select.h>
#include "clientes.h"
#include "defines.h"
#include "paquetes.h"

int tcp_socket_server, udp_socket_server, unix_socket_server, maxfd;
fd_set readset, tempset;
nodo_clientes* clientes_conectados;
void al_conectar_cliente(int id);
void al_desconectar_cliente(int id);


void procesar_buffer_cliente(nodo_cliente* un_cliente)
{
  while( un_cliente->buffer_pos > 12)
  {
  
    int longitud_paquete = (int)(un_cliente->buffer[9]);
    int longitud_total = longitud_paquete + 12; //12 es el header
    if(longitud_total < un_cliente->buffer_pos)
    {
      struct paquete un_paquete = crear_paquete(un_cliente->buffer);  
      
      char [TAMANO_BUFFER_CLIENTE] buffer_aux;
      
      un_cliente->buffer_pos = un_cliente->buffer_pos - longitud_total;
      memcpy(buffer_aux,un_cliente->buffer[longitud_total],un_cliente->buffer_pos);
      memcpy(un_cliente->buffer,buffer_aux, un_cliente->buffer_pos);    
      //Llamada al evento se recibio un paquete HACER el metodo de adentro destruye el paquete HACER
    }
    else
    {
      break;
    }


  }
}
void logger(const char *text) {
  printf("%s\n", text);

}



int start_tcp_server(){
  logger("starting TCP server");
  // Create the server-side socket
  tcp_socket_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (tcp_socket_server < 0) {
    perror("TCP socket");
    return 0;
  }
  maxfd = tcp_socket_server;

  int flag = 1;
  int result = setsockopt( tcp_socket_server, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int) );
  if (result < 0) {
    perror("TCP setsockopt(TCP_NODELAY)");
    return 0;
  }

  result = setsockopt( tcp_socket_server, SOL_SOCKET, SO_REUSEADDR, (char *)&flag, sizeof(int) );
  if (result < 0) {
    perror("TCP setsockopt(SO_REUSEADDR)");
    return 0;
  }

  result = setsockopt( tcp_socket_server, SOL_SOCKET, SO_KEEPALIVE, (char *)&flag, sizeof(int) );
  if (result < 0) {
    perror("TCP setsockopt(SO_KEEPALIVE)");
    return 0;
  }
  
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Bind socket to the appropriate port and interface (INADDR_ANY)
  if ( bind( tcp_socket_server, (struct sockaddr *)&address, sizeof(address) ) < 0 ) {
    perror("TCP bind");
    return 0;
  }

  // Listen for incoming connection(s)
  if ( listen( tcp_socket_server, 1 ) < 0 ) {
    perror("TCP listen");
    return 0;
  }

  logger("TCP server started");
  return 1;
}

int start_udp_server(){
  logger("starting UDP server");
  udp_socket_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (udp_socket_server < 0) {
    perror("UDP socket");
    return 0;
  }
  maxfd = udp_socket_server;

  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  // Bind socket to the appropriate port and interface (INADDR_ANY)
  if ( bind( udp_socket_server, (struct sockaddr *)&address, sizeof(address) ) < 0 ) {
    perror("UDP bind");
    return 0;
  }

  logger("UDP server started");
  return 1;
}

int start_unix_socket_server(){
  logger("starting Unix server");
  unix_socket_server = socket(AF_UNIX, SOCK_STREAM, 0);
  if (unix_socket_server < 0) {
    perror("UDP socket");
    return 0;
  }
  maxfd = unix_socket_server;

  struct sockaddr_un local;
  local.sun_family = AF_UNIX;
  strcpy(local.sun_path, UNIX_SOCK_PATH);
  unlink(local.sun_path);
  size_t len = strlen(local.sun_path) + sizeof(local.sun_family);

  // Bind socket to the appropriate port and interface (INADDR_ANY)
  if ( bind( unix_socket_server, (struct sockaddr *)&local, len ) < 0 ) {
    perror("Unix bind");
    return 0;
  }

  // Listen for incoming connection(s)
  if ( listen( unix_socket_server, 1 ) < 0 ) {
    perror("Unix listen");
    return 0;
  }  

  logger("Unix server started");
  return 1;
}

void stop_tcp_server(){
  close(tcp_socket_server);
  logger("TCP server stopped");
}

void stop_udp_server(){
  close(udp_socket_server);
  logger("UDP server stopped");
}

void stop_unix_socket_server(){
  close(unix_socket_server);
  logger("Unix server stopped");
}

int accept_new_clients(int socket){
  logger("New client in queue");
  //Aca agregar cliente a la lista. El int del socket es el filedescriptor del socket
  struct sockaddr_in addr;
  int len = sizeof(addr);
  int result = accept(socket, (struct sockaddr *)&addr, (socklen_t*)&len);
  if (result == -1) { 
    // These are not errors according to the manpage.
    if ((errno    == ENETDOWN || errno == EPROTO || errno == ENOPROTOOPT 
      || errno == EHOSTDOWN || errno == ENONET || errno == EHOSTUNREACH 
      || errno == EOPNOTSUPP || errno == ENETUNREACH))
      return result;
    else
      perror("accept error");
  }
  FD_SET(result, &readset);
  maxfd = (maxfd < result) ? result : maxfd;
  FD_CLR(socket, &tempset);
  al_conectar_cliente(result);
  //llamar funcion que signifique para nosotros que se conecto alguien nuevo
  return result;  
}

void read_udp_message(){
  logger("New UDP message");
  // recvfrom(s, buf, BUFLEN, 0, &si_other, &slen)
}

int read_message(int j){
  logger("New message");
  int result;
  char buffer[1024];
   do 
   {
      
      result = recv(j, buffer , MAX_BUFFER_SIZE, 0);
      
      if (result > 0)
      {
        //busco cliente y le mando lo que tiene el buffer al buffer del cliente
        nodo_clientes* un_cliente;
        un_cliente = buscar_cliente(clientes_conectados, j);
        if(un_cliente->buffer_pos + result > TAMANO_BUFFER_CLIENTE)
        {
          printf("Error: Se leyo mas de lo que el buffer puede recibir \n");
          exit(EXIT_FAILURE);
        }
      
        memcpy(un_cliente->buffer + un_cliente->buffer_pos, buffer, result);
        un_cliente->buffer_pos = un_cliente->buffer_pos + result;

        procesar_buffer_cliente(un_cliente);

      }
      else
      {
          printf("No recibi nada \n");
      }
   
   } while (result == -1 && errno == EINTR);

   return result;  
}

void listen_and_accept_new_clients(){
  int j, result;
  struct timeval tv;

  FD_ZERO(&readset);
  FD_SET(tcp_socket_server, &readset);
  FD_SET(udp_socket_server, &readset);
  FD_SET(unix_socket_server, &readset);

  do {
     memcpy(&tempset, &readset, sizeof(tempset));
     tv.tv_sec = 30;
     tv.tv_usec = 0;
     result = select(maxfd + 1, &tempset, NULL, NULL, &tv);

     if (result == 0) {
        logger("select() timed out!\n");
     } else if (result < 0 && errno != EINTR) {
        perror("select()");
     } else if (result > 0) {
        if (FD_ISSET(udp_socket_server, &tempset)) {
          read_udp_message();
        }

        if (FD_ISSET(tcp_socket_server, &tempset)) {
          accept_new_clients(tcp_socket_server);
        }

        if (FD_ISSET(unix_socket_server, &tempset)) {
          accept_new_clients(unix_socket_server);
        }

        for (j=0; j<maxfd+1; j++) {
          if (FD_ISSET(j, &tempset)) {
            int resul = read_message(j);
            if (resul == 0)
            {
              FD_CLR(j,&readset); 
              al_desconectar_cliente(j);
            }
            FD_CLR(j, &tempset);    
          }      // end if (FD_ISSET(j, &tempset))
        }      // end for (j=0;...)
        
     }      // end else if (result > 0)
  } while (1);

}

void al_conectar_cliente(int id)
{
  nodo_clientes* cliente_nuevo = NULL;
  cliente_nuevo = crear_cliente(id);
  cliente_nuevo-> estado = ESTADO_ESPERANDO;
  agregar_cliente(&clientes_conectados, cliente_nuevo);
  printf("Se conecto el cliente numero %d \n ", id);
  //FALTA HACER UN LOGGER CAPO
}

void al_desconectar_cliente(int id)
{
  quitar_cliente(&clientes_conectados, id);
  printf("Se desconecto el cliente numero %d \n ", id);
  //FALTA HACER UN LOGGER CAPO
}

void desserializar_mensaje()
{
  //Desarma el mensaje para darselo preparado a al_recibir_mensaje
}

void al_recibir_mensaje(int id, int otro_id /* Falta algo */ );


void stop_main(){
  stop_tcp_server();
  stop_udp_server();
  stop_unix_socket_server();  
}

// main function
int main(int argc, char *argv[]) {
  logger("Loading server.....");

  if (start_tcp_server() && start_udp_server() && start_unix_socket_server()) {
    logger("Server started successfully");
  } else {
    stop_main();
    return EXIT_FAILURE;
  }

  listen_and_accept_new_clients();
  stop_main();
  return EXIT_SUCCESS;
}

