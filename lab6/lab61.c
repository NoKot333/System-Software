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

struct Message //структура для сообщений
{
    long mtype; //тип сообщения 
    size_t mdata; //данные сообщения
};


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
            if (array[i][j] < min)
            {
                index_column = j;
                min = array[i][j];
            }
        }
    }

    return index_column;
}

int main() 
{    
    FILE *input = fopen("in61.txt", "r");

    int N;
    fscanf(input, "%d", &N);

    int **A = allocate_array2D(N, N);
    read_file_to_array2D(input, A, N, N);

    fclose(input);

    int msgid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);

    //если не удалось создать очередь сообщений, завершить выполнение
    if (msgid < 0) {
        fprintf(stdout, "\nError");
        return 0;
    }

    struct Message message; 

    if(fork() == 0)
    {
        size_t max_row;
        max_row = find_max2D(A, N);

        message.mtype = 1; //установить тип сообщения в 1
        message.mdata = max_row; //записать вычисленную сумму в сообщение 

        //послать сообщение в очередь 
        msgsnd(msgid, &message, sizeof(struct Message), 0); 

        return 0;
    }
    else
    {
        size_t min_column;
        min_column = find_min2D(A, N);

        //тип получаемого сообщения не важен
        msgrcv(msgid, &message, sizeof(struct Message), 0, 0); 

        //добавить данные сообщения к результату
        size_t result = min_column + message.mdata; 

        display_array2D(A, N, N);
        printf("index of row with max: %ld\n" 
                "index of column with min: %ld\n" 
                "sum equal %ld\n", message.mdata, min_column, result);
    }

    return 0;
}

