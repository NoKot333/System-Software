// 25 а) Найти сумму индекса строки с максимальным элементом матрицы с индексом 
// столбца с минимальным элементом матрицы. Входные данные: целое 
// положительное число n, массив чисел А размерности nxn. Использовать два 
// процесса. 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <limits.h>
#include <signal.h>

int msgid; //для хранения дескриптора очереди сообщений

struct Message //структура для сообщений
{
    long mtype; 
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

int find_max2D(int **array, int rows, int columns, size_t *out_index_row, size_t *out_index_column)
{
    int max = INT_MIN;

    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < columns; j++)
        {
            if (array[i][j] > max)
            {
                *out_index_row = i;
                *out_index_column = j;
                max = array[i][j];
            }
        }
    }

    return max;
}

int find_min2D(int **array, int rows, int columns, size_t *out_index_row, size_t *out_index_column)
{
    int min = INT_MAX;

    for (size_t i = 0; i < rows; i++)
    {
        for (size_t j = 0; j < columns; j++)
        {
            if (array[i][j] < min)
            {
                *out_index_row = i;
                *out_index_column = j;
                min = array[i][j];
            }
        }
    }

    return min;
}

int get_message_queue()
{
    // создать очередь сообщений
    int msgid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);

    //если не удалось создать очередь сообщений, завершить выполнение
    if (msgid < 0) {
        fprintf(stdout, "\nError");
        return 0;
    }

    return msgid;
}

void release_message_queue(int message_queue_id)
{
    struct msqid_ds info;
    msgctl(message_queue_id, IPC_RMID, &info);
}

void recieve_message(int signum)
{ 
    signal(SIGUSR1, recieve_message);

    msgrcv(msgid, &message, sizeof(struct Message), 0, 0); //тип получаемого сообщения не важен
    release_message_queue(msgid);
}

void terminated_by_alarm(int signum)
{
    signal(SIGALRM, recieve_message);

    printf("Terminated by alarm\n");   
    release_message_queue(msgid);

    exit(EXIT_FAILURE);
}

int main() 
{    
    signal(SIGALRM, terminated_by_alarm);

    FILE *input = fopen("in7a.txt", "r");

    int N, alarm_delay;
    fscanf(input, "%d %d", &N, &alarm_delay);
    alarm(alarm_delay);

    int **A = allocate_array2D(N, N);
    read_file_to_array2D(input, A, N, N);

    fclose(input);

    msgid = get_message_queue();

    if(fork() == 0)
    {
        size_t max_row, max_column;
        find_max2D(A, N, N, &max_row, &max_column);

        message.mtype = 1; //установить тип сообщения в 1
        message.mdata = max_row; //записать вычисленную сумму в сообщение 

        msgsnd(msgid, &message, sizeof(struct Message), 0); //послать сообщение в очередь 

        sleep(1);
        kill(getppid(), SIGUSR1);

        exit(EXIT_SUCCESS);
    }
    else
    {
        size_t min_row, min_column;
        find_min2D(A, N, N, &min_row, &min_column);

        signal(SIGUSR1, recieve_message);
        pause();

        //добавить данные сообщения к результату
        size_t result = min_column + message.mdata; 
        display_array2D(A, N, N);
        printf("max number is on row %ld\n" 
                "min number is on column %ld\n" 
                "sum is %ld\n", message.mdata, min_column, result);
    }

    return EXIT_SUCCESS;
}
