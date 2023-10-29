#ifndef MESSAGE_H
#define MESSAGE_H

#define RPC_OP_INIT 1
#define RPC_OP_PUT 2
#define RPC_OP_GET 3
#define RPC_OP_IDLE 4
#define RPC_ACK 5

struct message{
    int op_code;
    int index;
    int value;
    int client_id;
    int seq_number;
};


#endif