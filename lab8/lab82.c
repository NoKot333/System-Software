#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <limits.h>
#include <linux/sched.h>

//
typedef struct Node
{
    int value;
    struct Node *next;
} Node;

Node **arr_nodes;
int count = 0;
int index_nodes = -1;

void node_add(Node** head, int value)
{
	Node* tmp = (Node*)malloc(sizeof(Node));
	tmp->value = value;
	tmp->next = (*head);
	(*head) = tmp;
}

void read_array(FILE *ist, Node **arr, int n)
{
	for (size_t i = 0; i < n; i++)
	{
		Node* head = (Node*)malloc(sizeof(Node));
        int k;
        fscanf(ist, "%d", &k);
        for (size_t j = 0; j < k; j++)
        {
            int tmp_value;
            fscanf(ist, "%d", &tmp_value);
            node_add(&head, tmp_value);
        }
        arr[i] = head;
	}
}

Node **allocate_array(int n)
{
	Node **arr = malloc(sizeof(Node*) * n);
	return arr;
}

void free_array(Node **arr, int n)
{
    for (size_t i = 0; i < n; i++)
    {
        Node* head = arr[i];
        while (head->next)
        {
            Node * prev = NULL;
            prev = head;
            head = head->next;
            free(prev);
        }
        free(head);
    }
}

void get_count_pos_num_node(void* param)
{
	int n = *(int *)param;
	const Node* head = arr_nodes[n];
	int tmp_count = 0;
	while (head->next)
	{
		if (head->value > 0) {
			tmp_count = tmp_count + 1;
		}
		head = head->next;
	}

	if (tmp_count > count)
	{
		count = tmp_count;
		index_nodes = n;
	}

}

void print_node_list(int i)
{
	const Node* head = arr_nodes[i];
	while (head->next)
	{
		printf("%d ", head->value);
		head = head->next;
	}
	printf("\n");
}

int main()
{
	FILE* ist = fopen("in82.txt", "r");
	int n;
	fscanf(ist, "%d", &n);
	
	// Создание и запись массива А
	arr_nodes = allocate_array(n);
	read_array(ist, arr_nodes, n);
	fclose(ist);

	int stack[1000];
	int param[n];

	for (size_t i = 0; i < n; i++)
	{
		param[i] = i;
		clone(get_count_pos_num_node, (void*)(stack+1000-1), CLONE_VM|CLONE_VFORK, param+i);
	}
	
	printf("All nodes: \n");
	for (size_t i = 0; i < n; i++)
	{
		print_node_list(i);
	}
	printf("\nMaximum count is %d, index array nodes is %d\n", count, index_nodes);
	print_node_list(index_nodes);
	
	free_array(arr_nodes, n);

	return 0;
}
