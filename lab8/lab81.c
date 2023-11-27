#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sched.h>
#include <limits.h>
#define NUMSTACK 10000

int **arr_a;
int *arr_res;
int k;

// Чтение двумерного массива
void read_array(FILE *in, int **arr, int n, int k)
{
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < k; j++)
		{
			fscanf(in, "%d", &arr[i][j]);
		}
	}
}

// Вывод двумерного массива
void print_array_A(int **arr, int n, int k)
{
	printf("Array A:\n");
	for (size_t i = 0; i < n; i++)
	{
		for (size_t j = 0; j < k; j++)
		{
			printf("%5d ", arr[i][j]);
		}
		printf("\n");
	}
}

void print_array_B(int *arr, int n)
{
	printf("\nArray B:\n");
	for (size_t i = 0; i < n; i++)
	{
		printf("%5d ", arr[i]);
	}
	printf("\n");
}

void free_array(int **arr, int n)
{
	for (size_t i = 0; i < n; i++)
	{
		free(arr[i]);
	}
	free(arr);
}

int get_min(int **arr, int n, int k)
{
	int min = INT_MAX;
	for (size_t i = 0; i < k; i++)
	{
		if (arr[n][i] < min)
		{
			min = arr[n][i];
		}
	}
	return min;
}

int get_max(int *arr, int n)
{
	int max = INT_MIN;
	for (size_t i = 0; i < n; i++)
	{
		if (arr[i] > max)
		{
			max = arr[i];
		}
	}
	return max;
}

int work(void *param)
{
    int i = *(int *)param;
	arr_res[i] = get_min(arr_a, i, k);
    return 0;
}

int main()
{
	int n;
	// Открытие файла и запись размеров массива
	FILE* ist = fopen("in81.txt", "r");
	fscanf(ist, "%d %d", &n, &k);
	
	// Создание и запись массива А
	arr_a = malloc(sizeof(int *) * n);
	for (size_t i = 0; i < n; i++)
	{
		arr_a[i] = malloc(sizeof(int) * k);
	}
	read_array(ist, arr_a, n, k);
	
	fclose(ist); 
	
	arr_res = malloc(sizeof(int) * n);

	int stack[NUMSTACK];
	int* param = malloc(sizeof(int)*n);

	// Создание n процессов
	for (size_t i = 0; i < n; i++)
	{
		param[i] = i;
		clone(work, (void*)(stack+1000-1), CLONE_VM|CLONE_VFORK, param+i);
	}
	
	// Вывод массивов
	print_array_A(arr_a, n, k);
	print_array_B(arr_res, n);
	
	// Нахождение и вывод максимального элемента в массиве В
	int max = get_max(arr_res, n);
	printf("\nMaximum element is %d\n", max);

	// Освобождение памяти
	free_array(arr_a, n);
	free(arr_res);

	return 0;
}
