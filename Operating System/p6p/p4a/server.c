#include "cs537.h"
#include "request.h"

// 
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// CS537: Parse the new arguments too

int top, bottom;
int *list;
pthread_cond_t thread_empty;
pthread_cond_t thread_full;
int num_buffer;
void* get_thread(void*);
void hand_thread_helper();
pthread_mutex_t thread_mutex;
int thread_count;


void getargs(int *port, int *nthreads, int argc, char *argv[])
{
    if (argc != 4) 
    {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    *nthreads = atoi(argv[2]);
    num_buffer = atoi(argv[3]);
    *port = atoi(argv[1]);
    if(*nthreads < 0 || num_buffer < 0) 
      exit(1);
}


int main(int argc, char *argv[])
{
    // init the threads
    pthread_mutex_init(&thread_mutex, NULL);
    pthread_cond_init(&thread_empty, NULL);
    pthread_cond_init(&thread_full, NULL);

    // init the count, top, and bottom which can be used to track
    thread_count = 0;
    top = 0;
    bottom = 0;

    int listenfd, connfd, port, clientlen, nthreads;
    struct sockaddr_in clientaddr;

    //printf("hihihi\n");

    getargs(&port, &nthreads, argc, argv);
    pthread_t *threadIDS = (pthread_t*)malloc(sizeof(pthread_t)*nthreads);
    list = (int*)malloc(sizeof(int)*num_buffer);

    listenfd = Open_listenfd(port);

    // create thread by number of thread in args
    while(nthreads > 0)
    {
        //printf("hihiwqei\n");
        pthread_create(&threadIDS[nthreads], NULL, get_thread, NULL);
        nthreads --;
    }

    // 
    while (1) 
    {
        // use socket to get the client and connect
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

        // start to perform the thread task
        pthread_mutex_lock(&thread_mutex);
        
        //printf("hihihi, wooowo%d\n", clientlen);
        while(thread_count >= num_buffer)
        {
            pthread_cond_wait(&thread_full, &thread_mutex);
        }
        //printf("hihihi, wooasasdasf%d\n", clientlen);
        // list trace the connfd for each, and add thread count when the connfd is added to list
        list[bottom] = connfd;
        bottom++;
        thread_count++;
        // reset when exceed the number of buffer
        if(bottom >= num_buffer)
            bottom = 0;

        //printf("bottom is: %d\n", bottom);
        pthread_cond_signal(&thread_empty);
        pthread_mutex_unlock(&thread_mutex);
        // thread finish
    }
}

void* get_thread(void* port)
{
    while(1)
    {
      pthread_mutex_lock(&thread_mutex);

      while(thread_count == 0)
      {
           pthread_cond_wait(&thread_empty, &thread_mutex); 
      }
      // method pull out the connfd and remove the thread after done 
      hand_thread_helper();
    }
}

// this method helps handle the request, right after unlock and close the thread
void hand_thread_helper()
{
    thread_count--;
    int connfd = list[top];
    // pop the the connfd
    list[top] --;
    top++;

    //reset after search through
    if(top >= num_buffer)
        top = 0;

    // finish the thread then request it since it will be faster
    pthread_cond_signal(&thread_full);
    pthread_mutex_unlock(&thread_mutex);
    requestHandle(connfd);
    close(connfd);
}


    


 
