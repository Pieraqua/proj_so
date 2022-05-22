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

sem_t s_buffer;
sem_t s_vaga;
sem_t s_item;
int hashis[NUMFILO]={1,1,1,1,1};
int num_vagas=0;
int num_item=5;

typedef struct
{
    queue_t *prev, *next;   // ponteiros para usar em filas
    sem_t *palitoDireita;   // 
    sem_t *palitoEsquerda;  //
    pthread_t funcaoThread; //
} filafilosofo_t;


filafilosofo_t *filaFilosofo;

//sem_t hashi[NUMFILO];
pthread_t filosofos[NUMFILO];

void *threadFxn(void *arg)
{
    int numero=(long int) arg;
    int hashiDireita=(long int)arg;
    int hashiEsquerda=((long int)arg+1)%NUMFILO;
    while(1)
    {
        /* meditação */
        printf("Filosofo %i está meditando. Vagas: %i Item: %i\n\n", numero, num_vagas, num_item);
        sleep(2);
        /* tentando pegar o hashi esquerdo*/ 
        printf("Filosofo %i tenta pegar o hashi %i. Vagas: %i Item: %i\n\n", numero, hashiEsquerda, num_vagas, num_item);
        sem_wait(&s_item);
        sem_wait(&s_buffer);
        if(hashis[hashiEsquerda]!=0)
        {
            printf("Filosofo %i pegou o hashi a sua esquerda, o hashi  %i. Vagas: %i Item: %i\n\n", numero, hashiEsquerda, num_vagas, num_item);    
            hashis[hashiEsquerda]=0;
            num_item=num_item-1;
            num_vagas=num_vagas+1;
            sem_post(&s_buffer);
            sem_post(&s_vaga);
            printf("Filosofo %i tenta pegar o hashi %i. Vagas: %i Item: %i\n\n", numero, hashiDireita, num_vagas, num_item);
            sem_wait(&s_item);
            sem_wait(&s_buffer);
            /* tentando pegar o hashi direito */
            if(hashis[hashiDireita]!=0)
            {
                hashis[hashiDireita]=0;
                num_item=num_item-1;
                num_vagas=num_vagas+1;
                printf("Filosofo %i pegou o hashi a sua direita, o hashi  %i. Vagas: %i Item: %i\n\n", numero, hashiDireita, num_vagas, num_item);    
                sem_post(&s_buffer);                
                sem_post(&s_vaga);
                printf("Filosofo %i está comendo. Vagas: %i Item: %i \n\n", numero, num_vagas, num_item);
                sleep(2);
                /* largando hashi esquerdo */
                printf("Filosofo %i larga o hashi %i. Vagas: %i Item: %i\n\n", numero, hashiEsquerda, num_vagas, num_item);
                sem_wait(&s_vaga);
                sem_wait(&s_buffer);
                hashis[hashiEsquerda]=1;
                num_item=num_item+1;
                num_vagas=num_vagas-1;                
                sem_post(&s_buffer);
                sem_post(&s_item);
                /* largando hashi direito */
                sem_wait(&s_vaga);
                sem_wait(&s_buffer);
                printf("Filosofo %i larga o hashi %i. Vagas: %i Item: %i\n\n", numero, hashiDireita, num_vagas, num_item);
                hashis[hashiDireita]=1;
                num_item=num_item+1;
                num_vagas=num_vagas-1;    
                sem_post(&s_buffer);
                sem_post(&s_item); 
            }   
            else
            { 
                /* largando hashi esquerdo porque não conseguiu pegar o hashi direito*/
                sem_wait(&s_vaga);
                sem_wait(&s_buffer);
                printf("Filosofo %i larga o hashi %i porque não conseguiu pegar o hashi %i. Vagas: %i Item: %i\n\n", numero, hashiEsquerda, hashiDireita, num_vagas, num_item);
                hashis[hashiEsquerda]=1;
                sem_post(&s_buffer);
                sem_post(&s_item); 
            }
        }
        else
        {
            sem_post(&s_buffer);
            sem_post(&s_item);
            printf("Filosofo %i não conseguiu pegar o hashi %i. Vagas: %i Item: %i\n\n", numero, hashiEsquerda, num_vagas, num_item);
        }

    }

    pthread_exit(NULL);
}



int main(int argc, char *argv[])
{
    long i, status;
	sem_init(&s_buffer, 0, 1);
	sem_init(&s_item, 0, 5);
	sem_init(&s_vaga, 0, 0);
    /* buffer do print não zoar*/
    setvbuf(stdout, 0, _IONBF, 0);
    /* hashis   
    for(i=0; i<NUMFILO; i++)
    {
        sem_init(&hashi[i], 0, 1);
    }*/
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