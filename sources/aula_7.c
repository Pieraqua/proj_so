/* Paulo Sergio Avila Junior e Mariana Gomes Luz */
/* Aula 7 - espera ocupada */

#include <stdlib.h>
#include <queue.h>
queue_t *filaThread;

/* funcao de pop */
int pop_filaint(filaint_t* fila)
{
	if(!fila)
	{
		fprintf(stderr, "Fila nula?");
		return -1;
	}

	filaint_t* atual = filaThread;
	queue_remove((queue_t**)&filaThread, (queue_t*)atual);

	int numero = atual->elemento;
	free(atual);

	return numero;
}

/* funcao de push */
int push_filaint(filaint_t* fila, int numero)
{
	filaint_t* atual = malloc(sizeof(filaint_t));

	if(queue_append((queue_t**)&fila, (queue_t*)atual))
	{
		printf("erro push!");
		return -1;
	}

	return 0;
}

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
	pthread_attr_t attr;
	long status;
	/* Cria thread 1 */
	pthread_t thread1;
	/* Cria thread 2 */
	pthread_t thread2;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	int stderr = pthread_create(&thread1, &attr, threadFxn, NULL);
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
	stderr = pthread_create(&thread2, &attr, threadFxn, NULL);
	// Check if thread is created sucessfuly
	if (stderr)
	{
		fprintf(stderr, " A thread2 não foi criada");
		return stderr;
	}
	else
		printf("A thread 2 foi criada com a Thread ID de : %i", stderr);
	
	status = pthread_join(thread1, NULL);
	if(status)
	{
		perror("pthread_join");
		exit(1);
	}

	status = pthread_join(thread2, NULL);
	if(status)
	{
		perror("pthread_join");
		exit(1);
	}

	pthread_exit(NULL);
}
