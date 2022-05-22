/* Paulo Sergio Avila Junior e Mariana Gomes Luz */
/* Aula 8 - produtor/consumidor */

#include <stdlib.h>
#include <queue.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>

/* Funcao das threads */
#define STACKSIZE 64 * 1024 /* tamanho de pilha das threads */
typedef struct
{
	queue_t *prev, *next; // ponteiros para usar em filas
	int elemento;
} filaint_t;

void print_elem(void *ptr)
{
	printf("%d", ((filaint_t *)ptr)->elemento);
}
filaint_t *filaInteiros;
int tamanhoFila = 0;

/* funcao de pop */
int pop_filaint(filaint_t **fila)
{
	if(!tamanhoFila)
		return -1;
	if (fila == NULL)
	{
		fprintf(stderr, "fila nula?\n");
		return -1;
	}

	filaint_t *atual = *fila;

	queue_remove((queue_t **)fila, (queue_t *)atual);
	tamanhoFila--;

	int numero = atual->elemento;
	free(atual);

	return numero;
}

/* funcao de push */
int push_filaint(filaint_t **fila, int numero)
{
	filaint_t *atual = malloc(sizeof(filaint_t));
	atual->elemento = numero;
	
	if(tamanhoFila >= 5)
		return -1;

	if (queue_append((queue_t **)fila, (queue_t *)atual))
	{
		fprintf(stderr, "erro push!\n");
		return -1;
	}
	tamanhoFila++;

	return 0;
}

sem_t semEscritor;
sem_t semLeitor;
sem_t semUsando;

int usando = 0;
int querUsar = 0;

void travaSemaforoLeitor()
{
	//sem_wait(&semUsando);
	sem_wait(&semEscritor);
	//sem_post(&semUsando);
	
}

void travaSemaforoEscritor()
{
	sem_wait(&semEscritor);
}
void liberaSemaforoEscritor()
{
	sem_post(&semEscritor);
}

void liberaSemaforo()
{
	sem_post(&semEscritor);
}

void* escritor(void* arg)
{
	int argumento = *(int*)arg;
	while(1){
	travaSemaforoEscritor();

	int numero = rand()%100;
	printf("Escritor %d removendo %d e adicionando %d\n", argumento, filaInteiros->elemento, numero);

	pop_filaint(&filaInteiros);
	push_filaint(&filaInteiros, numero);

	liberaSemaforoEscritor();
	}
	pthread_exit(NULL);
}

void* leitor(void* arg)
{
	int argumento = *(int*)arg;
	
	while(1){
		travaSemaforoLeitor();
		int soma = 0;
		int n = 0;
		filaint_t* atual = filaInteiros;

		printf("Leitor %d imprimindo \n", argumento);
		do{
			printf("Leitor %d: %d\n", argumento, atual->elemento);
			soma += atual->elemento;
			atual = (filaint_t*)atual->next;
			n++;
		}while(atual != filaInteiros);
		
		printf("Leitor %d média: %d\n", argumento, soma/n);

		liberaSemaforo();
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
	pthread_t thread3;
	pthread_t thread4;
	pthread_t thread5;

	/* Alimenta a fila */
	for (int i = 0; i < 10; i++)
	{
		push_filaint(&filaInteiros, rand()%100);
	}

	/* inicializa semaforo */
	sem_init(&semEscritor, 0, 1);
	sem_init(&semLeitor, 0, 1);
	sem_init(&semUsando,0,1);

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	int i = 0;
	int arg1 = 1;
	int err = pthread_create(&thread1, &attr, escritor, &arg1);
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
	int arg2 = 2;
	err = pthread_create(&thread2, &attr, escritor, &arg2);
	// Check if thread is created sucessfuly
	if (err)
	{
		fprintf(stderr, " A thread1 não foi criada\n");
		return err;
	}
	else
	{
		printf("A thread 2 foi criada com a Thread ID de : %i\n", err);
	}
	int arg3 = 3;
	err = pthread_create(&thread3, &attr, leitor, &arg3);
	// Check if thread is created sucessfuly
	if (err)
	{
		fprintf(stderr, " A thread1 não foi criada\n");
		return err;
	}
	else
	{
		printf("A thread 3 foi criada com a Thread ID de : %i\n", err);
	}

	int arg4 = 4;
	err = pthread_create(&thread4, &attr, leitor, &arg4);
	// Check if thread is created sucessfuly
	if (err)
	{
		fprintf(stderr, " A thread2 não foi criada\n");
		return err;
	}
	else
		printf("A thread 4 foi criada com a Thread ID de : %i\n", err);
	int arg5 = 5;
	err = pthread_create(&thread5, &attr, leitor, &arg5);
	// Check if thread is created sucessfuly
	if (err)
	{
		fprintf(stderr, " A thread1 não foi criada\n");
		return err;
	}
	else
	{
		printf("A thread 5 foi criada com a Thread ID de : %i\n", err);
	}
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
