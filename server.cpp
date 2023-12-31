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
#include <sstream>
#include <algorithm>

#define SERVER_PORT 8080 /* arbitrary, but client & server must agree*/
#define QUEUE_SIZE 10

struct Neighbor
{
   int id;
   int socketId;
   bool is_active;
   float x, y, z;
   float vx, vy, vz;
};

void UpdateNeighbor(std::vector<Neighbor> &neighbors, const Neighbor &updated_neighbor)
{
   auto it = std::find_if(neighbors.begin(), neighbors.end(), [&updated_neighbor](const Neighbor &n)
                          { return n.id == updated_neighbor.id; });

   if (it != neighbors.end())
   {
      it->x = updated_neighbor.x;
      it->y = updated_neighbor.y;
      it->z = updated_neighbor.z;
      it->vx = updated_neighbor.vx;
      it->vy = updated_neighbor.vy;
      it->vz = updated_neighbor.vz;
   }
   else
   {
      printf("Warning: Drone with ID %d not found. Ignoring update.\n", updated_neighbor.id);
   }
}

void UpdateOrInsertNeighbor(std::vector<Neighbor> &neighbors, const Neighbor &new_neighbor)
{
   auto it = std::find_if(neighbors.begin(), neighbors.end(), [&new_neighbor](const Neighbor &n)
                          { return n.id == new_neighbor.id; });

   if (it != neighbors.end())
   {
      it->x = new_neighbor.x;
      it->y = new_neighbor.y;
      it->z = new_neighbor.z;
      it->vx = new_neighbor.vx;
      it->vy = new_neighbor.vy;
      it->vz = new_neighbor.vz;
      it->is_active = true;
   }
   else
   {
      neighbors.push_back(new_neighbor);
   }
}

void handleUserInput(std::vector<int> &client_sockets)
{
   std::string userInput;
   while (true)
   {
      std::getline(std::cin, userInput);
      std::istringstream iss(userInput);
      char command;
      int droneId;
      float dx, dy, dz;
      iss >> command;
      switch (command)
      {
      case '1':
      {
         Message msg;
         msg.opcode = 1;
         for (int client_socket : client_sockets)
         {
            write(client_socket, &msg, sizeof(msg));
         }
      }
      break;
      case '2':
      {
         if (iss >> droneId)
         {
            Message msg;
            msg.opcode = 2;
            msg.msg2_server.targetDroneId = droneId;
            for (int client_socket : client_sockets)
            {
               write(client_socket, &msg, sizeof(msg));
            }
         }
         else
         {
            std::cout << "Por favor, forneça um ID de drone válido após o comando 2." << std::endl;
         }
      }
      break;
      case '3':
      {
         if (iss >> droneId >> dx >> dy >> dz)
         {
            Message msg;
            msg.opcode = 3;
            msg.msg3_server.targetDroneId = droneId;
            msg.msg3_server.dx = dx;
            msg.msg3_server.dy = dy;
            msg.msg3_server.dz = dz;
            for (int client_socket : client_sockets)
            {
               write(client_socket, &msg, sizeof(msg));
            }
         }
         else
         {
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

void handleClient(int sa, std::vector<Neighbor> &neighbors)
{
   while (true)
   {
      Message msg;
      int bytesRead = read(sa, &msg, sizeof(Message));
      if (bytesRead <= 0)
      {
         printf("Client disconnected or error occurred.\n");
         close(sa);
         return;
      }
      int opcode = msg.opcode;
      if (opcode == 1)
      {
         Neighbor new_neighbor;
         new_neighbor.id = msg.msg1_client.droneId;
         new_neighbor.x = msg.msg1_client.x;
         new_neighbor.y = msg.msg1_client.y;
         new_neighbor.z = msg.msg1_client.z;
         new_neighbor.is_active = true;
         UpdateOrInsertNeighbor(neighbors, new_neighbor);
         printf("Neighbor: id=%d, x=%f, y=%f, z=%f\n",
                new_neighbor.id, new_neighbor.x, new_neighbor.y, new_neighbor.z);
      }
      else if (opcode == 2)
      {
         Neighbor updated_neighbor;
         updated_neighbor.id = msg.msg2_client.droneId;
         updated_neighbor.x = msg.msg2_client.x;
         updated_neighbor.y = msg.msg2_client.y;
         updated_neighbor.z = msg.msg2_client.z;
         updated_neighbor.vx = msg.msg2_client.vx;
         updated_neighbor.vy = msg.msg2_client.vy;
         updated_neighbor.vz = msg.msg2_client.vz;
         UpdateNeighbor(neighbors, updated_neighbor);
         printf("Info about drone id=%d:\n x=%f\n y=%f\n z=%f\n vx=%f\n vy=%f\n vz=%f\n",
                updated_neighbor.id, updated_neighbor.x, updated_neighbor.y, updated_neighbor.z,
                updated_neighbor.vx, updated_neighbor.vy, updated_neighbor.vz);
      }
      else if (opcode == 3)
      {
         if (msg.msg3_client.success)
            printf("Reposicionamento realizado com sucesso!\n");
         else
            printf("Falha no reposicionamento\n");
      }
      else
      {
         printf("Comando desconhecido \n");
      }
   }
}

int main(int argc, char *argv[])
{
   printf("Server Started! Waiting for client connection... \n");
   int s, b, l, fd, sa, bytes, on = 1;
   struct sockaddr_in channel;

   std::vector<int> client_sockets;
   std::vector<Neighbor> neighbors;
   std::vector<std::thread> client_threads;

   memset(&channel, 0, sizeof(channel));
   channel.sin_family = AF_INET;
   channel.sin_addr.s_addr = htonl(INADDR_ANY);
   channel.sin_port = htons(SERVER_PORT);

   s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (s < 0)
   {
      printf("socket call failed");
      exit(-1);
   }
   setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));

   b = bind(s, (struct sockaddr *)&channel, sizeof(channel));
   if (b < 0)
   {
      printf("bind failed");
      exit(-1);
   }

   l = listen(s, QUEUE_SIZE);
   if (l < 0)
   {
      printf("listen failed");
      exit(-1);
   }
   std::thread userInputThread(handleUserInput, std::ref(client_sockets));
   while (1)
   {
      sa = accept(s, 0, 0);
      if (sa < 0)
      {
         printf("accept failed");
         exit(-1);
      }
      printf("Client connected with sa = %d\n", sa);
      client_sockets.push_back(sa);
      client_threads.push_back(std::thread(handleClient, sa, std::ref(neighbors)));
   }
   userInputThread.join();
}
