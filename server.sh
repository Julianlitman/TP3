clear;
gcc -o server server.c clientes.c paquetes.c mensajeria_server.c -pthread -Wall; 
./server ;
