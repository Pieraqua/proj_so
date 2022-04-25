/* Paulo Sergio Avila Junior e Mariana Gomes Luz */
/* Aula 7 - espera ocupada */

#include <stdlib.h>
#include <queue.h>
queue_t *filaThread;

/* Funcao das threads */

void threadFxn()
{
	queue_t *elemento;
	/* Retira primeiro elemento da fila e salva em uma variavel */
	queue_remove(&filaThread, filaThread);

	/* Cria valor novo aleatorio */
	elemento = malloc(8 *);

	/* Poe novo valor na fila */
	queue_append(&filaThread, filaThread);

	/* Imprime a operacao realizada */
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