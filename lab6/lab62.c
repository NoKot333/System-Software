/*16 б)Первое приложение считывает из файла произвольное
 число строк, содержащих  «Фамилию Должность Зарплату» 
 и отсылает эти данные второму приложению, которое 
 отображает фамилии и должности всех сотрудников с 
 максимальной зарплатой.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <limits.h>
#include <string.h>

struct Message //структура для сообщений
{
    long mtype; //тип сообщения (может требоваться тип long, а не int)
    char data[4096]; //данные сообщения
};

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

long readfile(char *filename, char **buffer)
{
    FILE *input = fopen(filename, "rb");

    fseek(input, 0, SEEK_END);
    long length = ftell(input);

    fseek(input, 0, SEEK_SET);
    *buffer = malloc(length + 1);

    fread(*buffer, 1, length, input);    
    fclose(input);

    buffer[length] = '\0';
    return length;
}

struct Employee
{
    char name[64];
    char function[64];
    int salary;
};

void print_employee(struct Employee *employee)
{
    printf("%s %s %d$\n", employee->name, employee->function, employee->salary);
}

void print_employees(struct Employee *employees, size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        print_employee(&employees[i]);
    }
}

struct Employee *parse_employers(char *string, size_t *length)
{
    size_t buffer_length = 1;
    struct Employee *employees = malloc(sizeof(struct Employee) * buffer_length);

    size_t current_index = 0;
    struct Employee *current;

    char ch;

    FILE *stream;
    int len = strlen(string);
    stream = fmemopen(string, strlen(string), "r");

    ch = string[len];
    
    do
    {
        if(!(current_index < buffer_length - 1))
        {
            buffer_length *= 2;
            employees = realloc(employees, sizeof(struct Employee) * buffer_length);
        }

        current = employees + current_index;
        current_index++;
    } while (fscanf(stream, "%s %s %d", current->name, current->function, &current->salary) != EOF);

    *length = current_index - 1;

    fclose(stream);

    return employees;
}

int get_max_salary(struct Employee *employees, size_t length)
{
    int max = INT_MIN;

    for (size_t i = 0; i < length; i++)
    {
        if (employees[i].salary > max)
        {
            max = employees[i].salary;
        }
    }
    
    return max;
}

void display_employees_with_max_salary(struct Employee *employees, size_t length)
{
    int max_salary = get_max_salary(employees, length);

    for (size_t i = 0; i < length; i++)
    {
       if (employees[i].salary == max_salary)
       {
           print_employee(&employees[i]);
       }
    }
}

int main()
{
    struct Message message; 
    //для хранения дескриптора очереди сообщений
    int message_queue_id = get_message_queue(); 

    if(fork() == 0)
    {
        char *text;
        long length = readfile("in62.txt", &text);

        message.mtype = 1; //установить тип сообщения в 1
        strcpy(message.data, text);

        //послать сообщение в очередь 
        msgsnd(message_queue_id, &message, sizeof(struct Message), 0); 

        free(text);
        exit(EXIT_SUCCESS);
    }
    else
    {
        //тип получаемого сообщения не важен
        msgrcv(message_queue_id, &message, sizeof(struct Message), 0, 0); 
        release_message_queue(message_queue_id);
        
        size_t length;
        struct Employee *employees = parse_employers(message.data, &length);

        printf("Employees with salary equal to max:\n");
        display_employees_with_max_salary(employees, length);

        free(employees);
    }

    return EXIT_SUCCESS;
}