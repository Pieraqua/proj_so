/* Paulo Sergio Avila Junior e Mariana Gomes Luz */
/* Aula 8 - semaforos */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <queue.h>
/* Funcao das threads */
#define STACKSIZE 64 * 1024 /* tamanho de pilha das threads */
#define NUMFILO 5

sem_t s_saleiro;
sem_t s_hashis[NUMFILO];
int numRefeicoes = 0;
pthread_mutex_t mut;

typedef struct
{
    queue_t *prev, *next;   // ponteiros para usar em filas
    sem_t *palitoDireita;   //
    sem_t *palitoEsquerda;  //
    pthread_t funcaoThread; //
} filafilosofo_t;

filafilosofo_t *filaFilosofo;

// sem_t hashi[NUMFILO];
pthread_t filosofos[NUMFILO];

void *threadFxn(void *arg)
{
    int numero = (long int)arg;
    int hashiDireita = (long int)arg;
    int hashiEsquerda = ((long int)arg + 1) % NUMFILO;
    while (1)
    {
        /* meditação */
        printf("Filosofo %i está meditando.\n\n", numero);
        sleep(0);
        /* pegando o hashi direito*/
        sem_wait(&s_saleiro);
        sem_wait(&s_hashis[hashiDireita]);
        printf("Filosofo %i pega o hashi da direita %i.\n\n", numero, hashiDireita);
        /* pegando o hashi esquerdo*/
        sem_wait(&s_hashis[hashiEsquerda]);
        printf("Filosofo %i pega o hashi da direita %i.\n\n", numero, hashiEsquerda);
        /* comendo */
        sem_post(&s_saleiro);
        printf("Filosofo %i está comendo.\n\n", numero);
        numRefeicoes = numRefeicoes + 1;
        sleep(0);
        printf("Filosofo %i terminou de comer.\n\n", numero);
        sem_post(&s_hashis[hashiDireita]);
        sem_post(&s_hashis[hashiEsquerda]);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    long i, status;
    sem_init(&s_saleiro, 0, 1);

    /* buffer do print não zoar*/
    setvbuf(stdout, 0, _IONBF, 0);
    /* hashis
    for(i=0; i<NUMFILO; i++)
    {
        sem_init(&hashi[i], 0, 1);
    }*/
    /* filosofos */
    for (i = 0; i < NUMFILO; i++)
    {
        status = pthread_create(&filosofos[i], NULL, threadFxn, (void *)i);
        if (status)
        {
            fprintf(stderr, " A thread não foi criada\n");
            exit(1);
        }
        sem_init(&s_hashis[i], 0, 1);
    }
    while (1)
    {
        sleep(1);
        pthread_mutex_lock(&mut);
        printf("Refeições por segundo: %d\n", numRefeicoes);
        numRefeicoes = 0;
        pthread_mutex_unlock(&mut);
    }
    pthread_exit(NULL);
}