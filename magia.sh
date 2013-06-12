clear;
gcc -o cliente cliente.c mensajeria_cliente.c clientes.c paquetes.c mensajeria.c -pthread -Wall; 
gcc -o server server.c clientes.c paquetes.c mensajeria_server.c mensajeria.c -pthread -Wall;
