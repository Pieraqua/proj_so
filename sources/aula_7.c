/* Paulo Sergio Avila Junior e Mariana Gomes Luz */
/* Aula 7 - espera ocupada */

#include <stdlib.h>
#include <queue.h>
#include <pthread.h>
#include <stdio.h>

/* Funcao das threads */
#define STACKSIZE 64 * 1024 /* tamanho de pilha das threads */
typedef struct
{
	queue_t *prev, *next; // ponteiros para usar em filas
	int elemento;
} filaint_t;

filaint_t* filaInteiros;

void print_elem(void* ptr)
{
	printf("%d ", ((filaint_t*) ptr)->elemento);
}

/* funcao de pop */
int pop_filaint(filaint_t **fila)
{
	if (fila == NULL)
	{
		fprintf(stderr, "fila nula?\n");
		return -1;
	}

	filaint_t *atual = *fila;

	queue_remove((queue_t **)fila, (queue_t *)atual);

	int numero = atual->elemento;
	free(atual);

	return numero;
}

/* funcao de push */
int push_filaint(filaint_t **fila, int numero)
{
	filaint_t *atual = malloc(sizeof(filaint_t));
	atual->elemento = numero;
	if (queue_append((queue_t **)fila, (queue_t *)atual))
	{
		fprintf(stderr, "erro push!\n");
		return -1;
	}

	return 0;
}

void *threadFxn(void *arg)
{
	int antigo;
	int elemento;
	int i = 0;
	for (i = 0; i < 100; i++)
	{
		/* Retira primeiro elemento da fila e salva em uma variavel */
		antigo = pop_filaint(&filaInteiros);

		/* Cria valor novo aleatorio */
		elemento = rand() % 100;

		/* Poe novo valor na fila */
		push_filaint(&filaInteiros, elemento);

		/* Imprime a operacao realizada */
		printf("Retirado o elemento %i da fila, e adicionado o elemento %i da fila.\n", antigo, elemento);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	pthread_attr_t attr;
	long status;
	/* Cria thread 1 */
	pthread_t thread1;
	/* Cria thread 2 */
	pthread_t thread2;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	filaInteiros = malloc(sizeof(filaint_t));

	filaInteiros->elemento = rand()%100;
	filaInteiros->prev = (queue_t*)&filaInteiros;
	filaInteiros->next = (queue_t*)&filaInteiros;

	int i = 0;
	// filaInteiros = malloc(sizeof(filaint_t));
	// filaInteiros->next = filaInteiros;
	// filaInteiros->prev = filaInteiros;
	// filaInteiros->elemento = rand();
	for (i = 0; i < 10; i++)
		push_filaint(&filaInteiros, rand() % 100);

	int err = pthread_create(&thread1, &attr, threadFxn, NULL);
	// Check if thread is created sucessfuly
	if (err)
	{
		fprintf(stderr, " A thread1 não foi criada\n");
		return err;
	}
	else
	{
		printf("A thread 1 foi criada com a Thread ID de : %i\n", err);
	}
	err = pthread_create(&thread2, &attr, threadFxn, NULL);
	// Check if thread is created sucessfuly
	if (err)
	{
		fprintf(stderr, " A thread2 não foi criada\n");
		return err;
	}
	else
		printf("A thread 2 foi criada com a Thread ID de : %i\n", err);

	status = pthread_join(thread1, NULL);
	if (status)
	{
		perror("pthread_join");
		exit(1);
	}

	status = pthread_join(thread2, NULL);
	if (status)
	{
		perror("pthread_join");
		exit(1);
	}

	pthread_exit(NULL);
}
