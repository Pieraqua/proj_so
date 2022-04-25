/* Paulo Sergio Avila Junior e Mariana Gomes Luz */
/* Aula 7 - espera ocupada */

#include <stdlib.h>
#include <queue.h>
#define STACKSIZE 64 * 1024 /* tamanho de pilha das threads */
typedef struct
{
	int *prev, *next; // ponteiros para usar em filas
	int elemento;
} filaint_t;
/* Funcao das threads */
filaint_t *filaInteiros;
void threadFxn()
{
	int antigo;
	int *elemento;

	/* Retira primeiro elemento da fila e salva em uma variavel */
	antigo = pop_filaInt(filaInteiros);

	/* Cria valor novo aleatorio */
	elemento = rand() % 100;

	/* Poe novo valor na fila */
	push_filaInt(filaInteiros, elemento);

	/* Imprime a operacao realizada */
	printf("Retirado o elemento %i da fila, e adicionado o elemento %i da fila.\n", antigo, elemento);
}

void main()
{
	/* Cria thread 1 */
	pthread_t thread1;
	/* Cria thread 2 */
	pthread_t thread2;

	int stderr = pthread_create(&thread1, NULL, &threadFxn, NULL);
	// Check if thread is created sucessfuly
	if (stderr)
	{
		fprintf(stderr, " A thread1 não foi criada");
		return stderr;
	}
	else
	{
		printf("A thread 1 foi criada com a Thread ID de : %i", stderr);
	}
	stderr = pthread_create(&thread2, NULL, &threadFxn, NULL);
	// Check if thread is created sucessfuly
	if (stderr)
	{
		fprintf(stderr, " A thread2 não foi criada");
		return stderr;
	}
	else
		printf("A thread 2 foi criada com a Thread ID de : %i", stderr);
	exit(0);
}
