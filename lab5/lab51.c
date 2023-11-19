/*2 Найти наибольший элемент на главной диагонали, и наименьший элемент на
побочной диагонали, заменить элемент, стоящий на пересечении диагоналей на сумму двух
найденных значений.*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int findMaxOnMainDiagonal(int** arr, int size) {
    int max = arr[0][0];

    for (int i = 1; i < size; i++) {
        if (arr[i][i] > max) {
            max = arr[i][i];
        }
    }

    printf("Наибольший элемент на главной диагонали: %d\n", max);
    return max;
}

int findMinOnSecondaryDiagonal(int** arr, int size) {
    int min = arr[0][size - 1];

    for (int i = 1; i < size; i++) {
        if (arr[i][size - 1 - i] < min) {
            min = arr[i][size - 1 - i];
        }
    }

    printf("Наименьший элемент на побочной диагонали: %d\n", min);
    return min;
}

void printMatrix(int** arr, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%i ", arr[i][j]);
        }
        printf("\n");
    }
}


int main() {
	int size;
	FILE* in = fopen("in51.txt","r");
	fscanf(in, "%d",&size);
    int* arr[size];
    for (int i = 0; i < size; i++) {
    	arr[i] = (int*)malloc(size*sizeof(int));
    }
    for (int i = 0; i < size; i++) {
    	for (int j = 0; j < size; j++) {
    		fscanf(in,"%i",&arr[i][j]);
    	}
    }

    pid_t pid = fork();

    if (pid == -1) {
        printf("Ошибка при создании потока\n");
        return 1;
    } else if (pid == 0) {
        // Процесс-потомок
        findMinOnSecondaryDiagonal(arr,size);
        exit(0);
    } else {
        // Процесс-родитель
        wait(NULL);
        findMaxOnMainDiagonal(arr,size);
    }

    printf("Исходная матрица:\n");
    printMatrix(arr,size);

    int max = findMaxOnMainDiagonal(arr,size);
    int min = findMinOnSecondaryDiagonal(arr,size);

    arr[size / 2][size / 2] = max + min;

    printf("Матрица с измененным элементом:\n");
    printMatrix(arr,size);

    return 0;
}


