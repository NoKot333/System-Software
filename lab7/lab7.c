/*16 а) Найти сумму индекса строки с максимальным 
элементом матрицы с индексом столбца с минимальным 
элементом матрицы. Входные данные: целое положительное 
число n, массив чисел А размерности nxn. Использовать 
два процесса. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <limits.h>
#include <signal.h>

struct Message //структура для сообщений
{
    long mtype; //тип сообщения 
    size_t mdata; //данные сообщения
} message;


int **allocate_array2D(int rows, int columns)
{
    int **array = malloc(sizeof(int *) * rows);
    for (size_t i = 0; i < rows; i++)
    {
        array[i] = malloc(sizeof(int) * columns);
    }

    return array;
}

void free_array2D(int **array, int rows)
{
    for (size_t i = 0; i < rows; i++)
    {
        free(array[i]);
    }
    free(array);
}

void read_file_to_array2D(FILE *file, int **array, int rows, int columns)
{
    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < columns; j++)
        {
            fscanf(file, "%d", &array[i][j]);
        }
    }
}

void display_array(int *array, int size)
{
    for (size_t i = 0; i < size; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");
}

void display_array2D(int **array, int rows, int columns)
{
    for (size_t i = 0; i < rows; i++)
    {
        display_array(array[i], columns);
    }
}

int find_max2D(int **array, int size)
{
    int max = INT_MIN;

    size_t index_row = 0;
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            if (array[i][j] > max)
            {
                index_row = i;
                max = array[i][j];
            }
        }
    }

    return index_row;
}

int find_min2D(int **array, int size)
{
    int min = INT_MAX;
    size_t index_column = 0;
    for (size_t i = 0; i < size; i++)
    {
        for (size_t j = 0; j < size; j++)
        {
            if (array[j][i] < min)
            {
                index_column = j;
                min = array[j][i];
            }
        }
    }

    return index_column;
}

void release_message_queue(int message_queue_id)
{
    struct msqid_ds info;
    msgctl(message_queue_id, IPC_RMID, &info);
}

void set_reliable_signal(int signum, void(*sighandler)(int))
{
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    act.sa_handler = sighandler;
    sigaction(signum, &act, 0);
}

void recieve_message(int signum, int msgid)
{
    signal(SIGUSR1, recieve_message);
    msgrcv(msgid, &message, sizeof(struct Message), 1, 0);
    release_message_queue(msgid);
}

void terminated_by_alarm(int signum, int msgid)
{
    signal(SIGALRM, recieve_message);

    printf("Terminated by alarm\n");
    release_message_queue(msgid);

    exit(EXIT_FAILURE);
}

int main() 
{    
//Выбор режима работы
	int mode;
	printf("0 - Non-trust signal\n1 - Trust signal: ");
	scanf("%d", &mode);
	
	if (mode == 0)
		signal(SIGALRM,terminated_by_alarm);
	else
		set_reliable_signal(SIGALRM,terminated_by_alarm);
		
    FILE *input = fopen("in7.txt", "r");

    int N,time_alarm;
    fscanf(input, "%d %d", &N, &time_alarm);
	alarm(time_alarm);
    int **A = allocate_array2D(N, N);
    read_file_to_array2D(input, A, N, N);

    fclose(input);

    int msgid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);

    if (msgid < 0) {
        fprintf(stdout, "\nError");
        return 0;
    }


    if(fork() == 0)
    {
        size_t max_row;
        max_row = find_max2D(A, N);

        message.mtype = 1;
        message.mdata = max_row; 

        msgsnd(msgid, &message, sizeof(struct Message), 0); 
	
		sleep(1);
		kill(getppid(),SIGUSR1);
        return 0;
    }
    
    size_t min_column;
    min_column = find_min2D(A, N);
	if (mode == 1)
		signal(SIGUSR1, recieve_message);
	else
		set_reliable_signal(SIGUSR1, recieve_message); 
		
	pause();
			
    size_t result = min_column + message.mdata; 

    display_array2D(A, N, N);
    printf("index of row with max: %ld\n" 
                "index of column with min: %ld\n" 
                "sum equal %ld\n", message.mdata, min_column, result);
    
    release_message_queue(msgid);

    return 0;
}

