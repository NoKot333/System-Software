#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

void execCommand()
{
    int st;
    signal(SIGCHLD,SIG_IGN);
    if(fork() == 0)
    {
        printf("[pid %d] Enter command: ",getpid());
        fflush(stdin);
        char *temp = NULL, command[255];
        scanf("%s", temp);
        fgets(command, sizeof command, stdin);
        system(command);
        fflush(stdin);
        exit(0);
    }
    wait(&st);

}

void execScenario()
{
    int st; signal(SIGCHLD,SIG_IGN);
    char result[255];
    strcpy(result,"bash ./lab11.sh "); 
    printf("[pid %d] Enter argument: ",getpid());
    fflush(stdin);
    char *temp = NULL, argument[255];
    scanf("%s", temp);
    fgets(argument, sizeof argument, stdin);
    strcat(result,argument);
    if(fork()==0)
    {
        system(result);
        printf("[pid %d] Mode ended by child process\n",getpid());
        exit(0);
    }
    wait(&st);
}

void doVariantTask()
{
    int st;
    signal(SIGCHLD,SIG_IGN);
    if(fork() == 0)
    {
        printf("[pid %d] amount of users: ",getpid());
        fflush(stdin);
        system("who | sort -u | wc -l");
        fflush(stdin);
        exit(0);
    }
    wait(&st);
}

int main(int argc, char **argv)
{
    while(1)
    {
        int choise = 0;
	    printf("1) Literally command line\n2) Check full paths of directories\n3) Check how much is working right now\n4) Exit\n");
	    do
	    {
		printf(">> ");
		scanf("%i", &choise);
	    }
	    while(choise < 1 || choise > 4);
        switch(choise)
        {
            case 1:
            execCommand();
            break;
            case 2:
            execScenario();
            break;
            case 3:
            doVariantTask();
            break;
            default:
            return 0;
        }
    }
    return 0;
}

