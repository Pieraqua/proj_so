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


typedef struct
{
    queue_t *prev, *next;   // ponteiros para usar em filas
    sem_t *palitoDireita;   // 
    sem_t *palitoEsquerda;  //
    pthread_t funcaoThread; //
} filafilosofo_t;


filafilosofo_t *filaFilosofo;

sem_t hashi[NUMFILO];
pthread_t filosofos[NUMFILO];

void *threadFxn(void *arg)
{
    int numero=(long int) arg;
    int hashiDireita=(long int)arg;
    int hashiEsquerda=((long int)arg+1)%NUMFILO;
    while(1)
    {
        printf("Filosofo %i está meditando. \n\n", numero);
        sleep(2);
        printf("Filosofo %i está tentando pegar o hashi %i\n\n", numero, hashiEsquerda);
        sem_wait(&hashi[hashiEsquerda]);
        printf("Filosofo %i pegou o hashi a sua esquerda, o hashi  %i\n\n", numero, hashiEsquerda);    
        printf("Filosofo %i está tentando pegar o hashi %i\n\n", numero, hashiDireita);
        sem_wait(&hashi[hashiDireita]);
        printf("Filosofo %i pegou o hashi a sua direita, o hashi %i\n\n", numero, hashiDireita);    
        printf("Filosofo %i está comendo. \n\n", numero);
        sleep(2);
        printf("Filosofo %i larga o hashi %i\n\n", numero, hashiEsquerda);
        sem_post(&hashi[hashiEsquerda]);
        printf("Filosofo %i larga o hashi %i\n\n", numero, hashiDireita);
        sem_post(&hashi[hashiDireita]);
    }
    pthread_exit(NULL);
}



int main(int argc, char *argv[])
{
    long i, status;

    /* buffer do print não zoar*/
    setvbuf(stdout, 0, _IONBF, 0);
    /* hashis */
    for(i=0; i<NUMFILO; i++)
    {
        sem_init(&hashi[i], 0, 1);
    }
    /* filosofos */
    for(i=0; i<NUMFILO; i++)
    {
        status= pthread_create(&filosofos[i], NULL, threadFxn, (void *) i);
        if (status)
        {
            fprintf(stderr, " A thread não foi criada\n");
            exit(1);
        }        
    }    
    pthread_exit(NULL);
}