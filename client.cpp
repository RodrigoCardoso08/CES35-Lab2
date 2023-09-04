#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <string>
#include "messages.h"

#define SERVER_PORT 8080 /* arbitrary, but client & server must agree */
#define BUFSIZE 4096     /* block transfer size */

struct ClientInfo
{
    std::string hostName;
    int id;
    float x, y, z;
    float vx, vy, vz;
};

ClientInfo parseArguments(int argc, char **argv)
{
    ClientInfo info;
    info.hostName = argv[1];
    info.id = std::atof(argv[2]);
    info.x = std::atof(argv[3]);
    info.y = std::atof(argv[4]);
    info.z = std::atof(argv[5]);
    info.vx = std::atof(argv[6]);
    info.vy = std::atof(argv[7]);
    info.vz = std::atof(argv[8]);
    return info;
}

int main(int argc, char **argv)
{
    int c, s, bytes;
    char buf[BUFSIZE];          /* buffer for incoming file */
    struct hostent *h;          /* info about server */
    struct sockaddr_in channel; /* holds IP address */
    if (argc != 9)
    {
        printf("Usage: client hostName id x y z vx vy vz\n");
        exit(-1);
    }
    ClientInfo clientInfo = parseArguments(argc, argv);

    h = gethostbyname(argv[1]);
    if (h)
    {
        printf("Host name: %s\n", h->h_name);
        printf("Address type: %s\n", (h->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
        char **addr_list = h->h_addr_list;
        for (int i = 0; addr_list[i] != NULL; i++)
        {
            struct in_addr *inaddr = (struct in_addr *)addr_list[i];
            printf("IP Address #%d: %s\n", i, inet_ntoa(*inaddr));
        }
        char **alias_list = h->h_aliases;
        for (int i = 0; alias_list[i] != NULL; i++)
        {
            printf("Alias #%d: %s\n", i, alias_list[i]);
        }
    }
    else
    {
        printf("gethostbyname failed to locate %s\n", argv[1]);
    }

    if (!h)
    {
        printf("gethostbyname failed to locate %s", argv[1]);
        exit(-1);
    }

    s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s < 0)
    {
        printf("socket call failed");
        exit(-1);
    }
    memset(&channel, 0, sizeof(channel));
    channel.sin_family = AF_INET;
    memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
    channel.sin_port = htons(SERVER_PORT);
    c = connect(s, (struct sockaddr *)&channel, sizeof(channel));
    if (c < 0)
    {
        printf("connect failed");
        exit(-1);
    }

    // Escutar mensagens do servidor
    while (1)
    {
        printf("on while\n");
        Message msg;
        bytes = read(s, &msg, sizeof(Message));
        if (bytes <= 0)
            exit(0);
        printf("opcode = %d\n", msg.opcode);
        switch (msg.opcode)
        {
        case 1: // "Quem está aí?"
        {
            Message response;
            response.opcode = 1;
            response.msg1_client.droneId = clientInfo.id;
            response.msg1_client.x = clientInfo.x;
            response.msg1_client.y = clientInfo.y;
            response.msg1_client.z = clientInfo.z;
            //   response.msg1_client.vx = clientInfo.vx;
            //   response.msg1_client.vy = clientInfo.vy;
            //   response.msg1_client.vz = clientInfo.vz;
            write(s, &response, sizeof(response));
            break;
        }
        case 2:
        {
            Message response;
            response.opcode = 2;
            response.msg2_client.droneId = clientInfo.id;
            response.msg2_client.x = clientInfo.x;
            response.msg2_client.y = clientInfo.y;
            response.msg2_client.z = clientInfo.z;
            response.msg2_client.vx = clientInfo.vx;
            response.msg2_client.vy = clientInfo.vy;
            response.msg2_client.vz = clientInfo.vz;
            write(s, &response, sizeof(response));
            break;
        }
        case 3:
        {
            Message response;
            response.opcode = 3;
            if (msg.msg3_server.targetDroneId == clientInfo.id)
            {
                clientInfo.x += msg.msg3_server.dx;
                clientInfo.y += msg.msg3_server.dy;
                clientInfo.z += msg.msg3_server.dz;
                response.msg3_client.success = true;
            }
            else
                response.msg3_client.success = false;
            write(s, &response, sizeof(response));
            break;
        }
        }
    }
}
