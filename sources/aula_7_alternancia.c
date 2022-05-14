/* Paulo Sergio Avila Junior e Mariana Gomes Luz */
/* Aula 7 - espera ocupada */

#include <stdlib.h>
#include <queue.h>
#include <pthread.h>
#include <stdio.h>
#include <signal.h>
#include <sys/time.h>

/* Funcao das threads */
#define STACKSIZE 64 * 1024 /* tamanho de pilha das threads */
/* Struct de tratamento de sinal */
struct sigaction action;
/* Struct de inicializacao do timer */
struct itimerval timer;
void systick();
typedef struct
{
	queue_t *prev, *next; // ponteiros para usar em filas
	int elemento;
} filaint_t;

int contadores[2] = {0, 0};

void print_elem(void *ptr)
{
	printf("%d", ((filaint_t *)ptr)->elemento);
}
filaint_t *filaInteiros;

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

int turn = 0;

void *threadFxn(void *arg)
{
	int argumento = *((int *)arg);
	int antigo;
	int elemento;
	while (1)
	{
		while (turn != argumento)
			; // busy waiting
		/* Retira primeiro elemento da fila e salva em uma variavel */
		antigo = pop_filaint(&filaInteiros);

		/* Cria valor novo aleatorio */
		elemento = rand() % 100;

		/* Poe novo valor na fila */
		push_filaint(&filaInteiros, elemento);

		contadores[argumento]++;

		/* Imprime a operacao realizada */
		printf("thread %i: tira %i da fila, põe %i, da ", argumento, antigo, elemento);
		queue_print("fila", (queue_t *)filaInteiros, print_elem);

		turn = (turn + 1) % 2;
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

	filaInteiros->elemento = rand() % 100;
	filaInteiros->prev = (queue_t *)filaInteiros;
	filaInteiros->next = (queue_t *)filaInteiros;

	int i = 0;
	// filaInteiros = malloc(sizeof(filaint_t));
	// filaInteiros->next = filaInteiros;
	// filaInteiros->prev = filaInteiros;
	// filaInteiros->elemento = rand();
	for (i = 0; i < 9; i++)
		push_filaint(&filaInteiros, rand() % 100);
	int arg1 = 0;
	int err = pthread_create(&thread1, &attr, threadFxn, &arg1);
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
	int arg2 = 1;

	// Inicializacao do temporizador
	action.sa_handler = systick;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;
	if (sigaction(SIGALRM, &action, 0) < 0)
	{
		perror("Erro em sigaction: ");
		exit(1);
	}

	timer.it_value.tv_usec = 100000; // primeiro disparo, em micro-segundos
	timer.it_value.tv_sec = 0;		 // primeiro disparo, em segundos

	timer.it_interval.tv_usec = 100000; // um disparo por milisegundo
	timer.it_interval.tv_sec = 0;

	if (setitimer(ITIMER_REAL, &timer, 0) < 0)
	{
		perror("Erro em setitimer: ");
		exit(1);
	}

	err = pthread_create(&thread2, &attr, threadFxn, &arg2);
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
void systick()
{
	// imprime qtd de insercoes
	printf("\n\nForam feitas %i insercoes/s + %i insercoes/s = %i insercoes/s\n\n", contadores[0] * 10, contadores[1] * 10, 10 * (contadores[0] + contadores[1]));
	// reseta contadores
	contadores[0] = 0;
	contadores[1] = 0;
}
