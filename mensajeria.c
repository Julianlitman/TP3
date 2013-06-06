#include "mensajeria.h"

void enviar_cancelar(int cliente_origen, int cliente_destino)
{
  struct paquete un_paquete;
  struct contenido un_contenido;
  un_paquete.accion = ACCION_CANCELAR;
  un_paquete.user_dest = cliente_destino;
  un_paquete.user_orig = cliente_origen;
  un_paquete.longitud = 0;
  send(cliente_destino, &un_paquete, sizeof(struct paquete), 0 );
  send(cliente_destino, &un_contenido, sizeof(struct contenido), 0 );   
}