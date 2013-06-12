#include "mensajeria.h"

void enviar_cancelar(nodo_clientes *un_cliente)
{
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