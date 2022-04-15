/* Mariana Gomes Luz e Paulo Sergio Avila */
/*    RA: 1607910   ///   RA: 2022664     */

#include <stdio.h>
#include "queue.h"
#include "ppos.h"
#include <ucontext.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

#define STACKSIZE 64 * 1024 /* tamanho de pilha das threads */
//printfs de debug
//#define PRINTDEBUG
#define PRONTA 0
#define TERMINADA 1
#define SUSPENSA 2

/* Fila que contem todas as tarefas */
task_t *filaTarefas;
/* Ponteiro para a tarefa atualmente ativa */
task_t *tarefaAtual;
/* Fila que contem as tarefas atualmente prontas */
task_t *filaProntas;
/* Ponteiro para a tarefa de usuário que acabou de rodar*/
task_t *tarefaPrevia;
/* Estrutura que contem as informacoes da tarefa main */
task_t mainTask;
/* Tarefa dispatcher */
task_t dispatcherTask;
/* Maior id criado ate agora */
int maiorId = 0;
/* Contador de tarefas */
int taskCont = 0;
/* Variável Global Timer */
int time = 0;
#define QUANTUM_VAL 20
/* Quantum de tempo da tarefa atual */
int quantum = QUANTUM_VAL;

/* Struct de tratamento de sinal */
struct sigaction action;
/* Struct de inicializacao do timer */
struct itimerval timer;

/* Declaracao de funcoes */
/* Funcao da tarefa dispatcher */
static void dispatcher();
/* Funcao que retorna a proxima tarefa a ser executada */
static task_t *scheduler();
/* Funcao de handler do systick */
void systick();

// Inicializar as variáveis e o buffer do printf
void ppos_init()
{
    char *stack;

    //Inicialização das variáveis
    filaTarefas = NULL;
    mainTask.next = NULL;
    mainTask.prev = NULL;

    mainTask.id = 0;                 // define a id da task
    mainTask.status = 0;             // Define o status da task a ser criada como Pronta
    getcontext(&(mainTask.context)); // Copia o contexto atual e copia para contextoTask
    stack = malloc(STACKSIZE);
    if (stack)
    {
        mainTask.context.uc_stack.ss_sp = stack;
        mainTask.context.uc_stack.ss_size = STACKSIZE;
        mainTask.context.uc_stack.ss_flags = 0; // Deixa como zero a máscara das flags
        mainTask.context.uc_link = 0;           // Quando a task terminar ela não tem para onde voltar, então deixa como 0
    }
    else
    {
        perror("Erro na criação da pilha: ");
        exit(1);
    }
    mainTask.preemptable = 0; // Define a variavel preempable da task do elemento de fila como 0, ou seja não preemptável
    tarefaAtual = &mainTask;

    task_create(&dispatcherTask, dispatcher, 0);
    dispatcherTask.status = PRONTA;

    queue_remove((queue_t **)&filaProntas, (queue_t *)&dispatcherTask);
    queue_append((queue_t **)&filaTarefas, (queue_t *)&dispatcherTask);
    queue_append((queue_t **)&filaTarefas, (queue_t *)&mainTask);

    taskCont = 0;

    //Inicilização do buffer do rintf
    setvbuf(stdout, 0, _IONBF, 0);

    //Inicializacao do temporizador
    action.sa_handler = systick;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGALRM, &action, 0) < 0)
    {
        perror("Erro em sigaction: ");
        exit(1);
    }

    timer.it_value.tv_usec = 1000; // primeiro disparo, em micro-segundos
    timer.it_value.tv_sec = 0;     // primeiro disparo, em segundos

    timer.it_interval.tv_usec = 1000; // um disparo por milisegundo
    timer.it_interval.tv_sec = 0;

    if (setitimer(ITIMER_REAL, &timer, 0) < 0)
    {
        perror("Erro em setitimer: ");
        exit(1);
    }
}

// Cria uma nova tarefa
//      - Um descritor de tarefa aponta para o programa principal
int task_create(task_t *task, void (*start_routine)(void *), void *arg)
{
    char *stack;
    //if(ID==MAXINT)
    maiorId++;
    task->id = maiorId;           // define a id da task
    task->status = 0;             // Define o status da task a ser criada como Pronta
    task->timeTaskCreate = time;  // Tempo em que a tarefa foi criada de acordo com a variavel global time em milisegundos
    task->timeTaskExit = 0;       // Tempo em que a tarefa foi encerrada de acordo com a variavel global time em milisegundos
    task->timeProcessor = 0;      // Tempo total do tempo que usou o processador
    task->timeBeginProcessor = 0; // Tempo que começou a usar o processador em dada ativacao
    task->timeEndProcessor = 0;   // Tempo que terminou de usar o processador em dada ativacao
    task->activations = 0;        // Tanto de vezes que foi ativada
    getcontext(&(task->context)); // Copia o contexto atual e copia para contextoTask
    stack = malloc(STACKSIZE);
    if (stack)
    {
        task->context.uc_stack.ss_sp = stack;
        task->context.uc_stack.ss_size = STACKSIZE;
        task->context.uc_stack.ss_flags = 0; // Deixa como zero a máscara das flags
        task->context.uc_link = 0;           // Quando a task terminar ela  e x p l o d e
    }
    else
    {
        perror("Erro na criação da pilha: ");
        exit(1);
    }
    task->preemptable = 0; // Define a variavel preempable da task do elemento de fila como 0, ou seja não preemptável

    task->next = NULL;
    task->prev = NULL;
    // Quando a task terminar ela não tem para onde voltar, então deixa como 0
    makecontext(&(task->context), (void (*)(void))start_routine, 1, (char *)arg); // Cria o contexto da task no endereço do contextoTask
    // Define o contexto da task no elemento de fila como o contextoTask
    task->preemptable = 0; // Define a variavel preempable da task do elemento de fila como 0, ou seja não preemptável
    /* Adiciona a tarefa para a fila de tarefas prontas */
    queue_append((queue_t **)(&filaProntas), (queue_t *)task);
    //queue_append((queue_t **)(&filaTarefas), (queue_t *)task);
#ifdef PRINTDEBUG
    printf("task_create: criou tarefa %i\n", task->id);
#endif
    taskCont = taskCont + 1;
    task->prioDinamica = 0;
    task->prioEstatica = 0;
    return task->id;
}

//Muda para outra tarefa, transfere o processador para a tarefa indicada.
int task_switch(task_t *task)
{
    task_t *proxima = filaTarefas, *atual = tarefaAtual;

    if (task != &dispatcherTask)
    {
        if (atual == NULL)
        {
            fprintf(stderr, "Fila vazia?\n");
            return -3;
        }
        for (;;)
        {
            proxima = proxima->next;
            if (proxima == task)
                break;
            if (proxima == NULL)
            {
                fprintf(stderr, "Elemento nulo detectado - task_switch\n");
                return -1;
            }
            if (proxima == filaTarefas)
            {
                fprintf(stderr, "Tarefa nao encontrada, suspensa? - task_switch\n");
                return -2;
            }
            if (proxima == task)
                break;
        }
    }
    else
    {
        proxima = &dispatcherTask;
    }
#ifdef PRINTDEBUG
    printf("task_switch: trocando contexto %i -> %i\n", atual->id, proxima->id);
#endif
    /* Setamos a tarefa a ser trocada como a tarefa ativa */
    tarefaAtual = proxima;

    /* Voltamos o quantum para o valor maximo */
    quantum = QUANTUM_VAL;
    /* E trocamos de contexto, salvando o contexto atual */
    atual->timeEndProcessor = time; // A tarefa anterior termina
    atual->timeProcessor = atual->timeProcessor + (atual->timeEndProcessor - atual->timeBeginProcessor);
    tarefaAtual->timeBeginProcessor = time; // Tempo que começou a usar o processador em dada ativacao
    tarefaAtual->activations++;             // Tanto de vezes que foi ativada
    swapcontext(&(atual->context), &(proxima->context));
    /* Retorno com sucesso */

    return 0;
}

// Termina a tarefa e volta para a tarefa 'main'.
//      - Deve ser usado a função swap_task para voltar para o main
void task_exit(int exit_code)
{
    task_t *atual = tarefaAtual; //, *proxima = filaTarefas;
    if (filaTarefas == NULL)
    {
        fprintf(stderr, "Fila vazia? - task_exit\n");
    }
    atual->timeTaskExit = time;
    /* Removemos a tarefa e desalocamos */
    //if (atual->id != 0)
    //   free((atual->context.uc_stack.ss_sp));
    //queue_remove((queue_t **)&filaTarefas, ((queue_t *)(atual)));
    /* Trocamos para a main */
#ifdef PRINTDEBUG
    printf("task_exit: tarefa %i sendo encerrada\n", atual->id);
#endif
    atual->timeTaskExit = time;
    printf("Task %i Exit : Execution Time %i ms  . Processor time %i ms  . %i activations.  \n ", atual->id, (atual->timeTaskExit - atual->timeTaskCreate), atual->timeProcessor, atual->activations);
    atual->status = TERMINADA;
    if (task_switch(&dispatcherTask) < 0)
        fprintf(stderr, "Erro ao trocar para o dispatcher - task_exit\n");
    //taskCont = taskCont - 1;
}

// Retorna o identificador da tarefa atual.
//      - Para a main deve ser 0.
//      - Não podem existir duas tarefas com o mesmo nome
int task_id()
{
    return tarefaAtual->id;
}

// Corpo da Tarefa Dispatcher
//  -Passa o controle para a tarefa da vez
static void dispatcher()
{
    task_t *proxima;
    while (taskCont > 0)
    {
        proxima = scheduler();
        tarefaPrevia = proxima;
        if (proxima != NULL)
        {
            queue_remove((queue_t **)&filaProntas, (queue_t *)proxima);
            queue_append((queue_t **)&filaTarefas, (queue_t *)proxima);
            task_switch(proxima);
            switch (proxima->status)
            {
            case PRONTA:
                /* Poe no fim da fila de prontas */
                queue_remove((queue_t **)&filaTarefas, (queue_t *)proxima);
                queue_append((queue_t **)&filaProntas, (queue_t *)proxima);
                break;
            case TERMINADA:
                //tira da fila
                /* code */
                queue_remove((queue_t **)&filaTarefas, ((queue_t *)(proxima)));

                free((proxima->context.uc_stack.ss_sp));

                taskCont = taskCont - 1;
                break;
            case SUSPENSA:
                break;
            }
        }
    }
    task_switch(&mainTask);
}

// Função task_yield
// Passa o controle da CPU de volta para o dispatcher
// Interessante usar o task_switch apontando para a tarefa Dipatcher
void task_yield()
{
    tarefaAtual->status = PRONTA;
    task_switch(&dispatcherTask);
}

static task_t *scheduler()
{
    task_t *escolhida = filaProntas;
    task_t *contador;
    //Scheaduler ainda não foi rodado se tarefaPrevia==NULL
    if (tarefaPrevia != NULL)
    {
        tarefaPrevia->prioDinamica = tarefaPrevia->prioEstatica;
    }
    if (filaProntas == NULL)
    {
        return NULL;
    }
    filaProntas->prioDinamica = filaProntas->prioDinamica - 1;
    contador = filaProntas->next;
    while (contador != filaProntas)
    {
        if (contador != tarefaPrevia)
        {
            contador->prioDinamica = contador->prioDinamica - 1;
        }
        //envelhece todas as tarefas que nao foram a que foi rodada previamente return filaProntas;
        if (contador->prioDinamica < escolhida->prioDinamica)
        {
            escolhida = contador;
        }
        contador = contador->next;
    }
    return (escolhida);
}

void task_setprio(task_t *task, int prio)
{
    if (prio > 20 || prio < -20)
    {
        fprintf(stderr, "Prioridade a ser setada não está de acordo com os parâmetros\n");
        return;
    }
    if (task == NULL)
    {
        tarefaAtual->prioDinamica = prio;
        tarefaAtual->prioEstatica = prio;
        return;
    }
    task->prioDinamica = prio;
    task->prioEstatica = prio;
}

int task_getprio(task_t *task)
{
    if (task == NULL)
    {
        return (tarefaAtual->prioEstatica);
    }
    return task->prioEstatica;
}

void systick()
{
    time++;
    /* Reduzir o quantum da tarefa atual em 1 */
    if (tarefaAtual->id > 1)
    {
        quantum--;
        /* Caso seja 0, trocar de tarefa */
        if (!quantum)
        {
            quantum = QUANTUM_VAL;
            if (task_switch(&dispatcherTask) < 0)
                fprintf(stderr, "Erro ao trocar para o dispatcher - systick\n");
        }
    }
}

unsigned int systime()
{
    return time;
}
