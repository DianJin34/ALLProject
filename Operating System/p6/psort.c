#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <fcntl.h>
#include <string.h>

const int MIN_THREAD_HANDLE_len = 100;

struct
{
    volatile int key;
    volatile char value[96];
} typedef entry;

struct
{
    entry *start;
    bool is_thread;
    int len;
} typedef merge_sort_data;

// data size
entry *input_file_data;
// array
entry *filestatesfer;
int data_size;
// thread pool
pthread_mutex_t lock;
int max_thread_count;
int curr_thread_count;

void merge(entry *start, long len)
{
    entry *sorted_list = filestatesfer + (start - input_file_data);
    int sorted_list_size = 0;

    int low = 0;
    int high = len / 2;

	// printf("merge1: n1 %d, n2 %d, length %d\n", n1, n2, length);
	// printf("merge1: i %d, j %d \n", i, j);
	// printf("merge1: h %d, l %d, m %d \n", high, low, mid);
	// merge left and right in ascending order
    while (low < len / 2 && high < len)
    {
        int key_a = (start[low].key);
        int key_b = (start[high].key);
        if (key_a < key_b)
        {
            sorted_list[sorted_list_size++] = start[low++];
        }
        else
        {
            sorted_list[sorted_list_size++] = start[high++];
        }
    }
    while (low < len / 2)
    {
        sorted_list[sorted_list_size++] = start[low++];
    }

    while (high < len)
    {
        sorted_list[sorted_list_size++] = start[high++];
    }

    for (size_t i = 0; i < len; i++)
    {
        start[i] = sorted_list[i];
    }
}

void *merge_sort(void *arg)
{
    pthread_t thread;
    merge_sort_data *task;
    merge_sort_data task_1;
    merge_sort_data task_2;

    task = (merge_sort_data *)arg;

    if (task->len == 1)
    {
        goto sort_end;
    }
    if (task->len == 2)
    {
        int key_a = task->start[0].key;
        int key_b = task->start[1].key;
        // in reverse order, switch
        if (key_b < key_a)
        {
            entry temp = task->start[0];
            task->start[0] = task->start[1];
            task->start[1] = temp;
        }
        goto sort_end;
    }

    task_1.start = task->start;
    task_1.len = task->len / 2;
    task_1.is_thread = false;

    task_2.start = task->start + (task->len / 2);
    task_2.len = task->len - task->len / 2;
    task_2.is_thread = false;

    if (curr_thread_count == max_thread_count || task_1.len < MIN_THREAD_HANDLE_len)
    {
        thread = 0;
        merge_sort(&task_1);
    }
    else
    {
        pthread_mutex_lock(&lock);
        if (curr_thread_count < max_thread_count)
        {
            curr_thread_count++;
            pthread_mutex_unlock(&lock);
            task_1.is_thread = true;
            if (pthread_create(&thread, NULL, merge_sort, &task_1) != 0)
            {
                exit(1);
            }
        }
        else
        {
            pthread_mutex_unlock(&lock);
            thread = 0;
            merge_sort(&task_1);
        }
    }

    merge_sort(&task_2);

    if (thread != 0)
    {
        pthread_join(thread, NULL);
        pthread_mutex_lock(&lock);
        curr_thread_count--;
        pthread_mutex_unlock(&lock);
    }
    merge(task->start, task->len);

sort_end:
    return 0;
}

int main(int argc, char const *argv[])
{
    FILE *input_file;
    FILE *output_file;
    struct stat file_stats;
    int fd;
    long size;
    struct timeval start, end;

    // initialization
    max_thread_count = atoi(argv[3]);
    curr_thread_count = 1;
    pthread_mutex_init(&lock, NULL);

    if ((input_file = fopen(argv[1], "r")) == NULL)
    {
        exit(1);
    }

    struct stat filestates;
    fd = fileno(input_file);
    fstat(fd, &filestates);
    size = filestates.st_size;

    fd = fileno(input_file);
    fstat(fd, &file_stats);
    input_file_data = (entry *)mmap(NULL, file_stats.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);

    data_size = size / 100;
    filestatesfer = malloc(data_size * sizeof(entry));

    merge_sort_data task;
    task.is_thread = false;
    task.start = input_file_data;
    task.len = data_size;

    gettimeofday(&start, NULL);
    merge_sort(&task);
    gettimeofday(&end, NULL);
    double time_taken = end.tv_sec - start.tv_sec + (double)(end.tv_usec - start.tv_usec) / 1000000;
    printf("%f s\n", time_taken);

    if ((output_file = fopen(argv[2], "w+")) == NULL)
    {
        exit(1);
    }
    fd = fileno(output_file);
    if (ftruncate(fd, size) != 0)
    {
        exit(1);
    }
    fstat(fd, &file_stats);
    char *output_file_data;

    if ((output_file_data = mmap(0, file_stats.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        exit(1);
    }
    memcpy(output_file_data, input_file_data, data_size * 100l);
    fsync(fd);
    return 0;
}
