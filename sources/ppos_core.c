#include "ppos.h"
#include "ppos_core.h"
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
/* Estrutura que contem as informacoes da tarefa main */
task_t mainTask;
/* Tarefa dispatcher */
task_t dispatcherTask;
/* Maior id criado ate agora */
int maiorId = 0;
int taskCont = 1;

/* Declaracao de funcoes */
static void dispatcher();

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
    queue_append((queue_t **)(&filaTarefas), (queue_t *)(&mainTask));

    task_create(&dispatcherTask, dispatcher, 0);
    dispatcherTask.status = 0;
    queue_remove((queue_t **)(&filaTarefas), (queue_t *)(&dispatcherTask));

    //Inicilização do buffer do printf
    setvbuf(stdout, 0, _IONBF, 0);
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
    /* Adiciona a tarefa para a fila de tarefas e a fila de tarefas prontas */
    queue_append((queue_t **)(&filaProntas), (queue_t *)task);
    queue_append((queue_t **)(&filaTarefas), (queue_t *)task);
#ifdef PRINTDEBUG
    printf("task_create: criou tarefa %i\n", task->id);
#endif
    taskCont = taskCont + 1;
    return task->id;
}

//Muda para outra tarefa, transfere o processador para a tarefa indicada.
int task_switch(task_t *task)
{
    task_t *proxima = filaTarefas, *atual = tarefaAtual;
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
            fprintf(stderr, "Tarefa nao encontrada - task_switch\n");
            return -2;
        }
        if (proxima == task)
            break;
    }
#ifdef PRINTDEBUG
    printf("task_switch: trocando contexto %i -> %i\n", atual->id, proxima->id);
#endif
    /* Setamos a tarefa a ser trocada como a tarefa ativa */
    tarefaAtual = proxima;
    /* E trocamos de contexto, salvando o contexto atual */

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

    /* Removemos a tarefa e desalocamos */
    //if (atual->id != 0)
    //   free((atual->context.uc_stack.ss_sp));
    //queue_remove((queue_t **)&filaTarefas, ((queue_t *)(atual)));
    /* Trocamos para a main */
#ifdef PRINTDEBUG
    printf("task_exit: tarefa %i sendo encerrada\n", atual->id);
#endif
    atual->status = TERMINADA;
    if (task_switch(&dispatcherTask) < 0)
        fprintf(stderr, "Erro ao trocar para a main - task_exit\n");
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
void dispatcher()
{
    task_t *proxima;
    while (taskCont > 0)
    {
        proxima = scheduler();
        if (proxima != NULL)
        {
            task_switch(proxima);
            switch (proxima->status)
            {
            case PRONTA:
                //roda fila
                filaProntas = filaProntas->next;
                break;
            case TERMINADA:
                //tira da fila
                /* code */
                queue_remove((queue_t **)&filaTarefas, ((queue_t *)(proxima)));
                queue_remove((queue_t **)&filaProntas, ((queue_t *)(proxima)));

                taskCont = taskCont - 1;
                break;
            case SUSPENSA:
                /* Removemos a tarefa e desalocamos */
                //sai da fila de prontas
                queue_remove((queue_t **)&filaProntas, ((queue_t *)(proxima)));
                break;
            }
        }
    }
    task_exit(0);
}

// Função task_yield
// Passa o controle da CPU de volta para o dispatcher
// Interessante usar o task_switch apontando para a tarefa Dipatcher
void task_yield()
{
    task_switch(&dispatcherTask);
}

static task_t *scheduler()
{
    return filaProntas;
}
