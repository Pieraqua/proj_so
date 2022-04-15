// PingPongOS - PingPong Operating System
// Prof. Carlos A. Maziero, DINF UFPR
// Versão 1.4 -- Janeiro de 2022

// Estruturas de dados internas do sistema operacional

#ifndef __PPOS_DATA__
#define __PPOS_DATA__

#include <ucontext.h> // biblioteca POSIX de trocas de contexto

// Estrutura que define um Task Control Block (TCB)
typedef struct task_t
{
  struct task_t *prev, *next; // ponteiros para usar em filas
  int id;                     // identificador da tarefa
  ucontext_t context;         // contexto armazenado da tarefa
  short status;               // pronta, rodando, suspensa, ...
  short preemptable;          // pode ser preemptada?
  int prioEstatica;           // Prioridade Estatica
  int prioDinamica;           // Prioridade Dinamica, muda de acordo com envelhecimento
  int timeTaskCreate;         // Tempo em que a tarefa foi criada de acordo com a variavel global time em milisegundos
  int timeTaskExit;           // Tempo em que a tarefa foi encerrada de acordo com a variavel global time em milisegundos
  int timeProcessor;          // Tempo total do tempo que usou o processador
  int timeBeginProcessor;     // Tempo que começou a usar o processador em dada ativacao
  int timeEndProcessor;       // Tempo que terminou de usar o processador em dada ativacao
  int activations;            // Tanto de vezes que foi ativada
  // ... (outros campos serão adicionados mais tarde)
} task_t;

// estrutura que define um semáforo
typedef struct
{
  // preencher quando necessário
} semaphore_t;

// estrutura que define um mutex
typedef struct
{
  // preencher quando necessário
} mutex_t;

// estrutura que define uma barreira
typedef struct
{
  // preencher quando necessário
} barrier_t;

// estrutura que define uma fila de mensagens
typedef struct
{
  // preencher quando necessário
} mqueue_t;

#endif
