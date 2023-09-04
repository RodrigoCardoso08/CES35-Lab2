#include <sys/fcntl.h>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include "messages.h"
#include <thread>
#include <iostream>

#define SERVER_PORT 8080  /* arbitrary, but client & server must agree*/
#define BUF_SIZE 4096  /* block transfer size */
#define QUEUE_SIZE 10

struct Neighbor {
   int id;
   // std::string hostName;
   float x, y, z;  // coordenadas
   float vx, vy, vz;
};

void handleUserInput(std::vector<int>& client_sockets) {
    std::string userInput;
    while (true) {
        std::getline(std::cin, userInput);  // Ler uma linha inteira
        std::istringstream iss(userInput);
        char command;
        int droneId;
        iss >> command;  // Ler o primeiro caractere (comando)
        switch (command) {
            case '1':
                {
                    Msg1_Server msg;
                    msg.opcode = 1;
                    for (int client_socket : client_sockets) {
                        write(client_socket, &msg, sizeof(msg));
                    }
                }
                break;
            case '2':
                {
                    if (iss >> droneId) {  // Tentar ler o ID do drone
                        Msg2_Server msg;
                        msg.opcode = 2;
                        msg.targetDroneId = droneId;
                        for (int client_socket : client_sockets) {
                            write(client_socket, &msg, sizeof(msg));
                        }
                    } else {
                        std::cout << "Por favor, forneça um ID de drone válido após o comando 2." << std::endl;
                    }
                }
                break;
            case '3':
                {
                    if (iss >> droneId >> dx >> dy >> dz) {  // Tentar ler o ID do drone e os deslocamentos
                        Msg3_Server msg;
                        msg.opcode = 3;
                        msg.targetDroneId = droneId;
                        msg.dx = dx;
                        msg.dy = dy;
                        msg.dz = dz;
                        for (int client_socket : client_sockets) {
                            write(client_socket, &msg, sizeof(msg));
                        }
                    } else {
                        std::cout << "Por favor, forneça um ID de drone e deslocamentos válidos após o comando 3." << std::endl;
                    }
                }
                break;
            default:
                std::cout << "Comando desconhecido." << std::endl;
                break;
        }
    }
}

int main(int argc, char *argv[])
{ 
   printf("Server Started! \n");
   int s, b, l, fd, sa, bytes, on = 1;
   struct sockaddr_in channel;
   
   std::vector<int> client_sockets;
   std::vector<Neighbor> neighbors;

   memset(&channel, 0, sizeof(channel));
   channel.sin_family = AF_INET;
   channel.sin_addr.s_addr = htonl(INADDR_ANY);
   channel.sin_port = htons(SERVER_PORT);

   s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (s < 0) {printf("socket call failed"); exit(-1);}
   setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
   
   b = bind(s, (struct sockaddr *) &channel, sizeof(channel));
   if (b < 0) {printf("bind failed"); exit(-1);}
   
   l = listen(s, QUEUE_SIZE);
   if (l < 0) {printf("listen failed"); exit(-1);}
   std::thread userInputThread(handleUserInput, std::ref(client_sockets));
   while (1) {
      printf("começo do while\n");
      sa = accept(s, 0, 0);
      if (sa < 0) {printf("accept failed"); exit(-1);}
      printf("sa = %d\n", sa);
      client_sockets.push_back(sa);
      
      while(1){
         Message msg;
        int bytesRead = read(sa, &msg, sizeof(Message));
        if (bytesRead <= 0) {
            printf("Client disconnected or error occurred.\n");
            break;  // Sai do loop se o cliente desconectar ou ocorrer um erro
        }
        switch (msg.opcode) {
            case 1: {
                  Neighbor new_neighbor;
                  new_neighbor.id = msg.msg1_client.droneId;
                  new_neighbor.x = msg.msg1_client.x;
                  new_neighbor.y = msg.msg1_client.y;
                  new_neighbor.z = msg.msg1_client.z;
                  new_neighbor.vx = msg.msg1_client.vx;
                  new_neighbor.vy = msg.msg1_client.vy;
                  new_neighbor.vz = msg.msg1_client.vz;
                  neighbors.push_back(new_neighbor);
                  printf("Received message: id=%d, x=%f, y=%f, z=%f, vx=%f, vy=%f, vz=%f\n", 
                        new_neighbor.id, new_neighbor.x, new_neighbor.y, new_neighbor.z, 
                        new_neighbor.vx, new_neighbor.vy, new_neighbor.vz);
                break;
               }
            case 2:  // Solicitar mais dados
                // Responder com velocidade
                break;
            case 3:  // Reposicionar
                // Executar a ordem e responder com confirmação ou erro
                break;
            default:
                // Código de operação desconhecido
                break;
        }
      }
      // close(sa); /* close connection */
   }
   userInputThread.join();
}

