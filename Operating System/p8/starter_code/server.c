#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>

#include "server_functions.h"
#include "message.h"
#include "udp.h"

#define MAX_MESSAGE_SIZE 1024
#define MAXIMUM_CLIENTS 100
#define debug 1

int num_clients = 0;
int SEQ_NUMBER[MAXIMUM_CLIENTS], CLIENT_ID[MAXIMUM_CLIENTS];
struct message last[MAXIMUM_CLIENTS];
struct socket server_socket;

pthread_mutex_t value_lock;
pthread_t threads[MAXIMUM_CLIENTS];

int find_CID_pos(int client_id);
void parse_packet(struct packet_info *given_packet, struct sockaddr *addr, unsigned int *slen, int *recv_len, struct message **msg);
void *process_request(void *arg);
void *thread_put(void *arg);
void *thread_get(void *arg);
void *thread_idle(void *arg);

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);
    server_socket = init_socket(port);
    pthread_mutex_init(&value_lock, NULL);

    while (1)
    {
        struct packet_info p;
        struct packet_info *param;
        struct message *msg;
        struct sockaddr client_addr;
        unsigned int client_addr_len;
        int recv_len;

        p = receive_packet(server_socket);
        parse_packet(&p, &client_addr, &client_addr_len, &recv_len, &msg);
        msg = (struct message *)&p.buf;

        if (debug)
            fprintf(stdout, "Recieved client_id: %d, seq_number: %d, op_code: %d\n", msg->client_id, msg->seq_number, msg->op_code);

        int CID_pos;
        if ((CID_pos = find_CID_pos(msg->client_id)) == -1)
        {
            if (num_clients == MAXIMUM_CLIENTS)
                continue;
            CLIENT_ID[num_clients] = msg->client_id;
            SEQ_NUMBER[num_clients] = 0;
            CID_pos = num_clients;
            num_clients += 1;
            // if(msg->op_code != RPC_OP_INIT)
            // {
            //     continue;
            // }
        }
        fprintf(stdout, "seq num: %d %d\n", SEQ_NUMBER[CID_pos], msg->seq_number);
        if (msg->seq_number < SEQ_NUMBER[CID_pos])
        {
            continue;
        }
        else if (msg->seq_number == SEQ_NUMBER[CID_pos])
        {
            fprintf(stdout, "Receiving again\n");
            if (threads[CID_pos] == 0)
            {
                char s_message[MAX_MESSAGE_SIZE];
                memcpy(s_message, &last[CID_pos], sizeof(last[CID_pos]));
                send_packet(server_socket, client_addr, client_addr_len, s_message, sizeof(last[CID_pos]));
            }
            else
            {
                struct message new_msg = {.op_code = RPC_ACK};
                char s_message[MAX_MESSAGE_SIZE];
                memcpy(s_message, &new_msg, sizeof(new_msg));
                send_packet(server_socket, client_addr, client_addr_len, s_message, sizeof(new_msg));
            }
            continue;
        }
        // switch (msg->op_code)
        // {
        // case RPC_OP_INIT:
        //     if (num_clients == MAXIMUM_CLIENTS)
        //         continue;
        //     CLIENT_ID[num_clients] = msg->client_id;
        //     SEQ_NUMBER[num_clients] = msg->seq_number;
        //     num_clients += 1;
        //     break;
        // default:
        SEQ_NUMBER[CID_pos] = msg->seq_number;
        param = malloc(sizeof(struct packet_info));
        memcpy(param, &p, sizeof(struct packet_info));
        if (debug)
            fprintf(stdout, "Recieved client_id: %d, seq_number: %d, op_code: %d\n",
                    ((struct message *)param->buf)->client_id,
                    ((struct message *)param->buf)->seq_number,
                    ((struct message *)param->buf)->op_code);

        pthread_create(&threads[CID_pos], NULL, process_request, param);
        // printf("try create\n");
        // }
    }
}

int find_CID_pos(int client_id)
{
    for (int i = 0; i < num_clients; i++)
    {
        if (CLIENT_ID[i] == client_id)
            return i;
    }
    return -1;
}

void parse_packet(struct packet_info *given_packet, struct sockaddr *addr, unsigned int *slen, int *recv_len, struct message **msg)
{
    *msg = (struct message *)given_packet->buf;
    *addr = given_packet->sock;
    *slen = given_packet->slen;
    *recv_len = given_packet->recv_len;
}

void *process_request(void *arg)
{
    // printf("thread start\n");
    struct packet_info p = *(struct packet_info *)arg; // receive_packet(server_socket);
    struct message *msg;
    struct sockaddr client_addr;
    unsigned int client_addr_len;
    int recv_len;
    parse_packet(&p, &client_addr, &client_addr_len, &recv_len, &msg);
    // int CID_pos = find_CID_pos(msg->client_id);

    if (debug)
        fprintf(stdout, "Recieved client_id: %d, seq_number: %d, op_code: %d\n",
                msg->client_id, msg->seq_number, msg->op_code);

    // int CID_pos = find_CID_pos(msg->client_id);

    switch (msg->op_code)
    {
    case RPC_OP_PUT:
        thread_put(arg);
        break;
    case RPC_OP_GET:
        thread_get(arg);
        break;
    case RPC_OP_IDLE:
        thread_idle(arg);
        break;
    }
    free(arg);
    return NULL;
}

void *
thread_put(void *arg)
{
    struct packet_info p = *(struct packet_info *)arg;
    struct message *msg;
    struct sockaddr client_addr;
    unsigned int client_addr_len;
    int recv_len;
    parse_packet(&p, &client_addr, &client_addr_len, &recv_len, &msg);

    pthread_mutex_lock(&value_lock);
    put(msg->index, msg->value);
    pthread_mutex_unlock(&value_lock);

    char s_message[MAX_MESSAGE_SIZE];
    memcpy(s_message, msg, sizeof(*msg));
    send_packet(server_socket, client_addr, client_addr_len, s_message, sizeof(*msg));

    int CID_pos = find_CID_pos(msg->client_id);
    last[CID_pos] = *msg;
    threads[CID_pos] = 0;

    pthread_exit(NULL);
    return NULL;
}

void *
thread_get(void *arg)
{
    struct packet_info p = *(struct packet_info *)arg;
    struct message *msg;
    struct sockaddr client_addr;
    unsigned int client_addr_len;
    int recv_len;
    parse_packet(&p, &client_addr, &client_addr_len, &recv_len, &msg);

    pthread_mutex_lock(&value_lock);
    msg->value = get(msg->index);
    pthread_mutex_unlock(&value_lock);

    char s_message[MAX_MESSAGE_SIZE];
    memcpy(s_message, msg, sizeof(*msg));
    send_packet(server_socket, client_addr, client_addr_len, s_message, sizeof(*msg));

    int CID_pos = find_CID_pos(msg->client_id);
    last[CID_pos] = *msg;
    threads[CID_pos] = 0;

    pthread_exit(NULL);
    return NULL;
}

void *thread_idle(void *arg)
{
    struct packet_info p = *(struct packet_info *)arg;
    struct message *msg;
    struct sockaddr client_addr;
    unsigned int client_addr_len;
    int recv_len;
    parse_packet(&p, &client_addr, &client_addr_len, &recv_len, &msg);

    idle(msg->value);

    char s_message[MAX_MESSAGE_SIZE];
    memcpy(s_message, msg, sizeof(*msg));
    send_packet(server_socket, client_addr, client_addr_len, s_message, sizeof(*msg));

    int CID_pos = find_CID_pos(msg->client_id);
    last[CID_pos] = *msg;
    threads[CID_pos] = 0;

    pthread_exit(NULL);
    return NULL;
}