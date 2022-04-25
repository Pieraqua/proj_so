/* Paulo Sergio Avila Junior e Mariana Gomes Luz */
/* Aula 7 - espera ocupada */

#include <stdlib.h>
#include <queue.h>
/* Funcao das threads */
void threadFxn()
{
	queue_t **filaThread;
	/* Retira primeiro elemento da fila e salva em uma variavel */
	int queue_append(queue_t * *queue, queue_t * elem);
	int queue_remove(queue_t * *queue, queue_t * elem);

	/* Cria valor novo aleatorio */

	/* Poe novo valor na fila */

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
