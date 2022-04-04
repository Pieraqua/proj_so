#include "ppos.h"

// Inicializar as variáveis e o buffer do printf
void ppos_init()
{
    //Inicialização das variáveis

    //Inicilização do buffer do printf
    setvbuf(stdout, 0, _IONBF, 0);
}

// Cria uma nova tarefa
//      - Um descritor de tarefa aponta para o programa principal
int task_create(task_t *task, void (*start_routine)(void *), void *arg)
{
}

//Muda para outra tarefa, transfere o processador para a tarefa indicada.
int task_switch(task_t *task)
{
}

// Termina a tarefa e volta para a tarefa 'main'.
//      - Deve ser usado a função swap_task para voltar para o main
void task_exit(task_t *task)
{
}

// Retorna o identificador da tarefa atual.
//      - Para a main deve ser 0.
//      - Não podem existir duas tarefas com o mesmo nome
int task_id()
{
}
