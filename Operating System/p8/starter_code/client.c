#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <stdio.h>


#include "client.h"
#include "message.h"

#define RETRY_TIME 5
#define TIMEOUT_INTERVAL 1
#define MAX_MESSAGE_SIZE 1024

#define debug 1

// initializes the RPC connection to the server
struct rpc_connection 
RPC_init(int src_port, int dst_port, char dst_addr[])
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);

    struct sockaddr_storage addr;
    socklen_t addrlen;
    populate_sockaddr(AF_INET, dst_port, dst_addr, &addr, &addrlen);

    // initialize the rpc_connection
    struct rpc_connection rpc = 
    {
        .recv_socket = init_socket(src_port),
        .seq_number = 0,
        .client_id = rand(),
        .dst_addr = *((struct sockaddr *)(&addr)),
        .dst_len = addrlen
    };

    // struct message new_msg = 
    // {
    //     .op_code = RPC_OP_INIT,
    //     .index = 0,
    //     .value = 0,
    //     .client_id = rpc.client_id,
    //     .seq_number = rpc.seq_number
    // };
    // char s_message[MAX_MESSAGE_SIZE];
    // memcpy(&s_message, &new_msg, sizeof(new_msg));
    // send_packet(rpc.recv_socket, rpc.dst_addr, rpc.dst_len, s_message, sizeof(new_msg));

    // if(debug) fprintf(stdout, "RPC init finished, src_port: %d, dst: %s:%d, client id: %d\n", src_port, dst_addr, dst_port, rpc.client_id);
    // fflush(stdout);
    return rpc;
}

// Sleeps the server thread for a few seconds
void 
RPC_idle(struct rpc_connection *rpc, int time)
{
    rpc->seq_number += 1;
    struct message new_msg = 
    {
        .op_code = RPC_OP_IDLE,
        .index = 0,
        .value = time,
        .client_id = rpc->client_id,
        .seq_number = rpc->seq_number
    };
    char s_message[MAX_MESSAGE_SIZE];
    memcpy(&s_message, &new_msg, sizeof(new_msg));
    send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, s_message, sizeof(new_msg));

    struct packet_info recv_packet;
    struct message recv_msg;
    for(int i = 0; i < RETRY_TIME; i++)
    {
        if(debug) fprintf(stdout, "sending seq: %d\n", new_msg.seq_number);
        recv_packet = receive_packet_timeout(rpc->recv_socket, TIMEOUT_INTERVAL);
        if(recv_packet.recv_len != -1)
        {
            recv_msg = *(struct message *)(recv_packet.buf);
            if(recv_msg.op_code == RPC_OP_IDLE)
                return;
            else if(recv_msg.op_code == RPC_ACK)
            {
                i = 0;
                printf("Server is busy, trying to connect...\n");
                fflush(stdout);
                sleep(TIMEOUT_INTERVAL);
            }
        }
        
        if(debug) fprintf(stdout, "recieved length: %d\n", recv_packet.recv_len);
        if(debug) fprintf(stdout, "recieved op_code: %d\n", recv_msg.op_code);
        send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, s_message, sizeof(new_msg));
    }
    RPC_close(rpc);
    fprintf(stderr, "Server connection is offline.\n");
    fflush(stderr);
    exit(1);
}

// gets the value of a key on the server store
int 
RPC_get(struct rpc_connection *rpc, int key)
{
    rpc->seq_number += 1;
    struct message new_msg = 
    {
        .op_code = RPC_OP_GET,
        .index = key,
        .value = 0,
        .client_id = rpc->client_id,
        .seq_number = rpc->seq_number
    };
    char s_message[MAX_MESSAGE_SIZE];
    memcpy(&s_message, &new_msg, sizeof(new_msg));
    send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, s_message, sizeof(new_msg));

    struct packet_info recv_packet;
    struct message recv_msg;
    for(int i = 0; i < RETRY_TIME; i++)
    {
        recv_packet = receive_packet_timeout(rpc->recv_socket, TIMEOUT_INTERVAL);
        if(recv_packet.recv_len == -1)
            continue;
        recv_msg = *(struct message *)(recv_packet.buf);
        if(recv_msg.op_code == RPC_OP_GET)
            return recv_msg.value;
        else if(recv_msg.op_code == RPC_ACK)
        {
            i = 0;
            sleep(TIMEOUT_INTERVAL);
            printf("Server is busy, trying to connect...\n");
        }
        if(debug) fprintf(stdout, "recieved op_code: %d\n", recv_msg.op_code);
        send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, s_message, sizeof(new_msg));
    }
    RPC_close(rpc);
    fprintf(stderr, "Server connection is offline.\n");
    fflush(stderr);
    exit(1);
}

// sets the value of a key on the server store
int 
RPC_put(struct rpc_connection *rpc, int key, int value)
{
    rpc->seq_number += 1;
    struct message new_msg = 
    {
        .op_code = RPC_OP_PUT,
        .index = key,
        .value = value,
        .client_id = rpc->client_id,
        .seq_number = rpc->seq_number
    };
    char s_message[MAX_MESSAGE_SIZE];
    memcpy(&s_message, &new_msg, sizeof(new_msg));
    send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, s_message, sizeof(new_msg));

    struct packet_info recv_packet;
    struct message recv_msg;
    for(int i = 0; i < RETRY_TIME; i++)
    {
        recv_packet = receive_packet_timeout(rpc->recv_socket, TIMEOUT_INTERVAL);
        if(recv_packet.recv_len == -1)
            continue;
        recv_msg = *(struct message *)(recv_packet.buf);
        if(recv_msg.op_code == RPC_OP_PUT)
            return recv_msg.value;
        else if(recv_msg.op_code == RPC_ACK)
        {
            i = 0;
            sleep(TIMEOUT_INTERVAL);
            printf("Server is busy, trying to connect...\n");
        }
        if(debug) fprintf(stdout, "recieved op_code: %d\n", recv_msg.op_code);
        send_packet(rpc->recv_socket, rpc->dst_addr, rpc->dst_len, s_message, sizeof(new_msg));
    }
    RPC_close(rpc);
    fprintf(stderr, "Server connection is offline.\n");
    fflush(stderr);
    exit(1);
}

// closes the RPC connection to the server
void 
RPC_close(struct rpc_connection *rpc)
{
    close_socket(rpc->recv_socket);
}