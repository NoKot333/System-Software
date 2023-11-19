/*2 Задан массив двусвязных списков. Вывести на экран список, не содержащий
нулевых элементов.*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

typedef struct Node {
    int data;
    struct Node* prev;
    struct Node* next;
} Node;

void printList(Node* head) {
    Node* current = head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}

bool containZeros(Node* head) {
    Node* current = head;

    while (current != NULL) {
        if (current->data == 0) {
           return true;
        }
        current = current->next;
    }
    return false;
}


int main() {
    // Открываем файл для чтения
    FILE *file = fopen("in52.txt", "r");
    if (file == NULL) {
        printf("Failed to open file");
        return 1;
    }

    // Создаем массив двусвязных списков
    int numLists;
    fscanf(file, "%d", &numLists);
	int listLength;
    fscanf(file, "%d", &listLength);
    Node** lists = (Node**)malloc(numLists * sizeof(Node*));

    // Чтение списков из файла
    for (int i = 0; i < numLists; i++) {

        Node* head = NULL;
        Node* tail = NULL;

        for (int j = 0; j < listLength; j++) {
            int value;
            fscanf(file, "%d", &value);

            Node* newNode = (Node*)malloc(sizeof(Node));
            newNode->data = value;
            newNode->prev = tail;
            newNode->next = NULL;

            if (head == NULL) {
                head = newNode;
                tail = newNode;
            } else {
                tail->next = newNode;
                tail = newNode;
            }
        }

        lists[i] = head;
    }

    fclose(file);

    // Создаем дочерние процессы для обработки списков
    pid_t pid;
    for (int i = 0; i < numLists; i++) {
        pid = fork();
        if (pid < 0) {
            perror("Failed to create child process");
            exit(1);
        } else if (pid == 0) {
            // Дочерний процесс
            if (!containZeros(lists[i])) {
            printList(lists[i]);
            }
            exit(0);
        }
    }

    // Ожидаем завершения всех дочерних процессов
    for (int i = 0; i < numLists; i++) {
        wait(NULL);
    }

    // Освобождаем память, занятую списками
    for (int i = 0; i < numLists; i++) {
        Node* current = lists[i];
        while (current != NULL) {
            Node* next = current->next;
            free(current);
            current = next;
        }
    }
    free(lists);
    return 0;
}
