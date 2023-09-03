#include <sys/fcntl.h>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 8080  /* arbitrary, but client & server must agree*/
#define BUF_SIZE 4096  /* block transfer size */
#define QUEUE_SIZE 10

struct Message {
    int opcode;
    char data[256];
};

struct Neighbor {
    int id;
    int x, y, z;  // coordenadas
};

int main(int argc, char *argv[])
{ 
   int s, b, l, fd, sa, bytes, on = 1;
   char buf[BUF_SIZE];  /* buffer for outgoing file */
   struct sockaddr_in channel;  /* holds IP address */
   
   std::vector<int> client_sockets;  // Lista para armazenar os soquetes dos clientes conectados
   std::vector<Neighbor> neighbors;

   /* Build address structure to bind to socket. */
   memset(&channel, 0, sizeof(channel));
   channel.sin_family = AF_INET;
   channel.sin_addr.s_addr = htonl(INADDR_ANY);
   channel.sin_port = htons(SERVER_PORT);
   /* Passive open. Wait for connection. */
   s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); /* create socket */
   if (s < 0) {printf("socket call failed"); exit(-1);}
   setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
   
   b = bind(s, (struct sockaddr *) &channel, sizeof(channel));
   if (b < 0) {printf("bind failed"); exit(-1);}
   
   l = listen(s, QUEUE_SIZE); /* specify queue size */
   if (l < 0) {printf("listen failed"); exit(-1);}
   /* Socket is now set up and bound. Wait for connection and process it. */
   while (1) {
      sa = accept(s, 0, 0); /* block for connection request */
      if (sa < 0) {printf("accept failed"); exit(-1);}

      client_sockets.push_back(sa);  // Adicione o novo cliente à lista
      
      Message firstMsg;
      firstMsg.opcode = 1;  // Opcode para "Quem está aí?"
      strcpy(firstMsg.data, "");

      for (int client_socket : client_sockets) {
         write(client_socket, &firstMsg, sizeof(firstMsg));
      }

      Message msg;
      read(sa, &msg, sizeof(Message));

      switch (msg.opcode) {
            case 1:  // Quem está aí?
               {
                  // Suponha que msg.data contém "id=1;x=10;y=20;z=30"
                  Neighbor new_neighbor;
                  sscanf(msg.data, "id=%d;x=%d;y=%d;z=%d", &new_neighbor.id, &new_neighbor.x, &new_neighbor.y, &new_neighbor.z);
                  neighbors.push_back(new_neighbor);
               }
                break;
            case 2:  // Solicitar mais dados
                // Responder com velocidade
                break;
            case 3:  // Reposicionar
                // Executar a ordem e responder com confirmação ou erro
                break;
            default:
                // Código de operação desconhecido
                break;
        }/* close file */
      close(sa); /* close connection */
   }   
}

