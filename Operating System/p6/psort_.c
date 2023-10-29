#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

int max_thread_count;
int thread_count;
pthread_mutex_t lock;

// thread parameters
struct merge_sort_task
{
	int low;
	int high;
	struct entry *a;
};

struct entry
{
	volatile int key;
	volatile char value[96];
};

// merge function for merging two parts
void merge(struct entry *a, int low, int high)
{


	// n1 is size of left side and n2 is size of right side
	int length = high - low;
	int n1 = length / 2;
	int mid = low + n1;
	int n2 = length - n1;

	struct entry *left = (struct entry *)malloc(n1 * sizeof(struct entry));
	struct entry *right = (struct entry *)malloc(n2 * sizeof(struct entry));

	int i;
	int j;

	// storing values in left part
	for (i = low; i < mid; i++)
		left[i - low] = a[i];
	// storing values in right part
	for (i = mid; i < high; i++)
		right[i - mid] = a[i];

	int k = low;

	i = j = 0;

	// printf("merge1: n1 %d, n2 %d, length %d\n", n1, n2, length);
	// printf("merge1: i %d, j %d \n", i, j);
	// printf("merge1: h %d, l %d, m %d \n", high, low, mid);
	// merge left and right in ascending order
	while (i < n1 && j < n2)
	{
		// printf("merge2\n");
		if (left[i].key <= right[j].key)
		{
			// printf("merge2.5\n");
			a[k++] = left[i++];
		}
		else
		{
			// printf("merge3.5\n");
			// printf("%d\n", right[j].key);
			// printf("%d\n", a[k].key);
			// a[k].key = 1;
			// printf("merge4\n");
			a[k++] = right[j++];
		}
	}
	// insert remaining values from left
	while (i < n1)
		a[k++] = left[i++];

	// insert remaining values from right
	while (j < n2)
		a[k++] = right[j++];

	free(left);
	free(right);
}

// thread function
void *merge_sort(void *arg)
{
	int low;
	int high;

	struct merge_sort_task *task;
	struct merge_sort_task task_1;
	struct merge_sort_task task_2;
	pthread_t threads;

	task = (struct merge_sort_task *)arg;
	// calculating low and high
	low = task->low;
	high = task->high;

	int length = high - low;
	if (length == 1)
		return 0;

	// evaluating mid point
	int mid = low + (high - low) / 2;

	task_1.a = task->a;
	task_1.low = low;
	task_1.high = mid;

	task_2.a = task->a;
	task_2.low = mid;
	task_2.high = high;
	/*...*/

	pthread_mutex_lock(&lock);
	if (thread_count < max_thread_count)
	{
		thread_count++;
		pthread_mutex_unlock(&lock);
		if (pthread_create(&threads, NULL, merge_sort, (void *)&task_1) != 0)
		{
			exit(1);
		}
	}
	else
	{
		pthread_mutex_unlock(&lock);
		threads = 0;
		merge_sort(&task_1);
	}

	merge_sort(&task_2);

	if (threads)
	{
		pthread_join(threads, NULL);
		pthread_mutex_lock(&lock);
		thread_count--;
		pthread_mutex_unlock(&lock);
	}
	merge(task->a, low, high);
	return 0;
}

// driver
int main(int argc, char **argv)
{
	if (argc != 4)
	{
		exit(1);
	}
	int MAX_ARRAY_ELEMENTS = 0;

	FILE *in_file = fopen(argv[1], "r");
	FILE *out_file = fopen(argv[2], "w+");
	int fd;
	struct stat filestates;
	struct merge_sort_task task;
    struct timeval start, end;

	max_thread_count = atoi(argv[3]);

	thread_count = 1;
	pthread_mutex_init(&lock, 0);

	if (in_file == NULL)
	{
		printf("Error! Could not open file\n");
		exit(1); // must include stdlib.h
	}

	fd = fileno(in_file);
	fstat(fd, &filestates);
	MAX_ARRAY_ELEMENTS = filestates.st_size / 100;
	struct entry *input_file_data = (struct entry *)mmap(NULL, filestates.st_size, PROT_WRITE | PROT_READ, MAP_PRIVATE, fd, 0);
	task.high = MAX_ARRAY_ELEMENTS;
	task.low = 0;
	task.a = input_file_data;
	// allocate the array
	// test for files not existing.

	
    gettimeofday(&start, NULL); 
	merge_sort(&task);
    gettimeofday(&end, NULL);
    double time_taken = end.tv_sec - start.tv_sec + (double)(end.tv_usec - start.tv_usec) / 1000000;
    printf("%f s\n", time_taken);	

	fd = fileno(out_file);
	if(ftruncate(fd, filestates.st_size)!= 0)
	{
		exit(1);
	}
	struct entry* output_file_data;

	// int index = 0;
	if ((output_file_data = mmap(0, filestates.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        exit(1);
    }
	memcpy(output_file_data, input_file_data, filestates.st_size);
    // for (size_t i = 0; i < MAX_ARRAY_ELEMENTS; i++)
    // {
    //     output_file_data[i] = ((struct entry*)input_file_data)[i];
    // }
	fsync(fd);
	fclose(out_file);
	fclose(in_file);
	return 0;
}
