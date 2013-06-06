clear;
gcc -o cliente cliente.c mensajeria_cliente.c clientes.c paquetes.c mensajeria.c -pthread -Wall; 
./cliente ;
