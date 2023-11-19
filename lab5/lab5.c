/*2 Найти максимальный M и минимальный элемент m массива А и составить
множество чисел, лежащих в интервале (m, M) и не содержащихся в массиве А.*/
#include <stdio.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdbool.h>


int shmid;
int semid;
struct sembuf Plus1 = {0,1,0};
struct sembuf Minus1 = {0,-1,0};
int rr;


struct mymem {
    int max;
    int min;
} *mem_sum;

void zad(int p, int* A,int n) {
    int i,m = 1000;
    int mx = 0;
    int begin = rr*p;
    int end = begin+rr;
    if (p==3) {end = n;}
    for (i=begin;i<end;i++) {
        if (A[i] < m) m=A[i];
        if (A[i] > mx) mx=A[i];
    }
    semop(semid, &Minus1, 1);
    if (m < mem_sum->min) mem_sum->min = m;
    if (mx > mem_sum->max) mem_sum->max = mx;
    semop(semid,&Plus1,1);
}


int main() {
    int n = 0;
    shmid = shmget(IPC_PRIVATE, 2, IPC_CREAT|0666);
    if (shmid < 0) {
        printf("Something going wrong\n");
        return 1;
    }
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT|0666);
    if (semid < 0) {
        printf("Something going wrong\n");
        return 1;
    }
    printf("Enter size of Array n: \n");
    scanf("%i",&n);
    int A[n];
    printf("Enter Array:\n");
    for (int i = 0; i < n; i++) {
        scanf("%d", &A[i]);
    }
    rr=n/4;
    semop( semid, &Plus1, 1);
    mem_sum = shmat(shmid,NULL,0);
    mem_sum ->max=0;
    mem_sum->min=1000;

    for(int i =0; i<3; i++) {
        if (fork()==0) {
            zad(i,A,n);
            return 1;
        }
    }
    zad(3,A,n);
    for (int i=0; i<3;i++) wait(NULL);
    printf("\nmax: %i",mem_sum->max);
    printf("\nmin: %i",mem_sum->min);
    printf("\nArray: \n");
    for (int j = mem_sum->min; j < mem_sum->max; j++) {
        bool k= false;
        for(int i = 0; i< n; i++) {
            if(j==A[i]) {
                k=true;
                break;
            }
        }
        if (k == false) {
            printf("%d ",j);
        }
    }
    printf("\n");
    return 0;
}
