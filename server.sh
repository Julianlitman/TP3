clear;
gcc -o server server.c clientes.c paquetes.c mensajeria_server.c mensajeria.c -pthread -Wall; 
./server ;
