#include "ppos.h"
#include "ppos_core.h"
#define STACKSIZE 64 * 1024 /* tamanho de pilha das threads */

task_t *filaTarefas;
task_t *tarefaAtual;
task_t mainTask;
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
    queue_append((queue_t **)(&filaTarefas), ((queue_t *)(&mainTask)));
    //Inicilização do buffer do printf
    setvbuf(stdout, 0, _IONBF, 0);
}

// Cria uma nova tarefa
//      - Um descritor de tarefa aponta para o programa principal
int task_create(task_t *task, void (*start_routine)(void *), void *arg)
{
    char *stack;

    int maiorId = 0;
    //if(ID==MAXINT)
    task_t *primeiraTask = filaTarefas;
    if (filaTarefas != NULL)
    {
        task_t *atual = filaTarefas->next; // atual, recebe a task do elemento de fila atual
        maiorId = filaTarefas->id;         // maiorId vai servir para definir o proximo id a ser usado,

        while (atual->next != primeiraTask) // Acha a maior task->id de todos os elementos de fila
        {
            if (atual->id > maiorId)
            {
                maiorId = atual->id;
            }
            atual = atual->next;
        }
    }
    task->id = maiorId + 1;       // define a id da task
    task->status = 0;             // Define o status da task a ser criada como Pronta
    getcontext(&(task->context)); // Copia o contexto atual e copia para contextoTask
    stack = malloc(STACKSIZE);
    if (stack)
    {
        task->context.uc_stack.ss_sp = stack;
        task->context.uc_stack.ss_size = STACKSIZE;
        task->context.uc_stack.ss_flags = 0; // Deixa como zero a máscara das flags
        task->context.uc_link = &mainTask.context;           // Quando a task terminar ela volta para o main
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

    queue_append((queue_t **)(&filaTarefas), (queue_t *)task);
    printf("task_create: criou tarefa %i", task->id);
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
    /* Setamos a tarefa a ser trocada como a tarefa ativa */
    tarefaAtual = proxima;
    /* E trocamos de contexto, salvando o contexto atual */
    printf("task_switch: trocando contexto %i -> %i", filaTarefas->id, task->id);

    swapcontext(&(atual->context), &(proxima->context));
    /* Retorno com sucesso */

    return 0;
}

// Termina a tarefa e volta para a tarefa 'main'.
//      - Deve ser usado a função swap_task para voltar para o main
void task_exit(int exit_code)
{
    task_t *atual = tarefaAtual;//, *proxima = filaTarefas;
    if (filaTarefas == NULL)
    {
        fprintf(stderr, "Fila vazia? - task_exit\n");
    }
    for (;;)
    {
        proxima = proxima->next;
        if (proxima == NULL)
        {
            fprintf(stderr, "Elemento nulo detectado - task_exit\n");
        }
        if (proxima == filaTarefas)
        {
            fprintf(stderr, "Main nao encontrada - task_exit\n");
        }
        /* Se encontrou a main, break */
        if (proxima->id == 0)
            break;
    }
    /* Removemos a tarefa e desalocamos */
    if (filaTarefas->id != 0)
        free((filaTarefas->context.uc_stack.ss_sp));
    queue_remove((queue_t **)&filaTarefas, ((queue_t *)(atual)));
    /* Trocamos para a main */
    printf("task_exit: tarefa %i sendo encerrada", filaTarefas->id);
    if (task_switch(&mainTask) < 0)
        fprintf(stderr, "Erro ao trocar para a main - task_exit\n");
}

// Retorna o identificador da tarefa atual.
//      - Para a main deve ser 0.
//      - Não podem existir duas tarefas com o mesmo nome
int task_id()
{
    return filaTarefas->id;
}
