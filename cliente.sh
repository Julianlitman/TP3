clear;
gcc -o cliente cliente.c mensajeria_cliente.c clientes.c paquetes.c mensajeria.c -pthread -Wall; 
cp cliente ./cliente2
./cliente ;
