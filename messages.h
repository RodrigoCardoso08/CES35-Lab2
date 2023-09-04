#ifndef MESSAGES_H
#define MESSAGES_H

struct Msg1_Server {
    int opcode;
};

struct Msg1_ClientResponse {
    int opcode;  // 1
    int droneId;
    float x, y, z;
};

struct Msg2_Server {
    int opcode;  // 2
    int targetDroneId;
};

struct Msg2_ClientResponse {
    int opcode;  // 2
    int droneId;
    float x, y, z;
    float vx, vy, vz;
};

struct Msg3_Server {
    int opcode;  // 3
    int targetDroneId;
    float dx, dy, dz;
};

struct Msg3_ClientResponse {
    int opcode;  // 3
    bool success;  // true para confirmação, false para erro
};
union Message {
    int opcode;
    Msg1_Server msg1_server;
    Msg2_Server msg2_server;
    Msg3_Server msg3_server;
    Msg1_ClientResponse msg1_client;
    Msg2_ClientResponse msg2_client;
    Msg3_ClientResponse msg3_client;
};

#endif  // MESSAGES_H
