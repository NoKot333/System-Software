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

void zad(int p, int* arr,int siz) {
    int m = 1000;
    int mx = 0;
    int begin = rr*p;
    int end = begin+rr;
    if (p==3) {end = siz;}
    for (int i=begin;i<end;i++) {
        if (arr[i] < m) m=arr[i];
        if (arr[i] > mx) mx=arr[i];
    }
    semop(semid, &Minus1, 1);
    if (m < mem_sum->min) mem_sum->min = m;
    if (mx > mem_sum->max) mem_sum->max = mx;
    semop(semid,&Plus1,1);
}


int main() {
    shmid = shmget(IPC_PRIVATE, 2, IPC_CREAT|0666);
    if (shmid < 0) {
        printf("Something going wrong\n");
        return 2;
    }
    semid = semget(IPC_PRIVATE, 1, IPC_CREAT|0666);
    if (semid < 0) {
        printf("Something going wrong\n");
        return 1;
    }
    int size;
	FILE* in = fopen("in5.txt","r");
	fscanf(in, "%d",&size);
    int* arr = (int*)malloc(size*sizeof(int));
    for (int j = 0; j < size; j++) {
    	fscanf(in,"%i",&arr[j]);
    }
    rr=size/4;
    semop( semid, &Plus1, 1);
    mem_sum = shmat(shmid,NULL,0);
    mem_sum ->max=0;
    mem_sum->min=1000;

    for(int i =0; i<3; i++) {
        if (fork()==0) {
            zad(i,arr,size);
            return 0;
        }
    }
    zad(3,arr,size);
    for (int i=0; i<3;i++) wait(NULL);
    printf("\nmax: %i",mem_sum->max);
    printf("\nmin: %i",mem_sum->min);
    printf("\nArray: \n");
    for (int j = mem_sum->min; j < mem_sum->max; j++) {
        bool k= false;
        for(int i = 0; i< size; i++) {
            if(j==arr[i]) {
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
