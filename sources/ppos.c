#include "ppos.h"
#include "ppos_core.h"

filatask_t *filaTarefas;

typedef struct filatask_t
{
    struct filatask_t *prev; // ptr para usar cast com queue_t
    struct filatask_t *next; // ptr para usar cast com queue_t
    task_t *task;
    // outros campos podem ser acrescidos aqui
} filatask_t;

filatask_t *filaTarefas;
// Inicializar as variáveis e o buffer do printf
void ppos_init()
{
    //Inicialização das variáveis
    struct task_t *prev, *next; // ponteiros para usar em filas
    int id = -1;                // identificador da tarefa
    ucontext_t context;         // contexto armazenado da tarefa
    short status = 0;           // pronta, rodando, suspensa, ...
    short preemptable = 0;      // pode ser preemptada?
    //Inicilização do buffer do printf
    setvbuf(stdout, 0, _IONBF, 0);
}

// Cria uma nova tarefa
//      - Um descritor de tarefa aponta para o programa principal
int task_create(task_t *task, void (*start_routine)(void *), void *arg)
{
    //if(ID==MAXINT)
    task_t *primeiraTask = filaTarefas->task;
    ucontext_t contextoTask;
    if (filaTarefas->task == NULL)
    {
        fprintf(stderr, "Fila vazia?\n");
        return -1;
    }

    task_t *atual = filaTarefas->next->task; // atual, recebe a task do elemento de fila atual
    int maiorId = filaTarefas->task->id;     // maiorId vai servir para definir o proximo id a ser usado,

    while (atual->next != primeiraTask) // Acha a maior task->id de todos os elementos de fila
    {
        if (atual->id > maiorId)
        {
            maiorId = atual->id;
        }
        atual = atual->next;
    }
    task->id = maiorId + 1;                                    // define a id da task
    filatask_t *taskCriada = malloc(sizeof(filatask_t));       // Cria o elemento de fila que irá conter o task
    taskCriada->task = task;                                   // Define o task do elemento de fila como o task a ser criado
    taskCriada->task->status = 0;                              // Define o status da task a ser criada como Pronta
    getcontext(&(contextoTask));                               // Copia o contexto atual e copia para contextoTask
    contextoTask.uc_stack.ss_flags = 0;                        // Deixa como zero a máscara das flags
    contextoTask.uc_link = 0;                                  // Quando a task terminar ela não tem para onde voltar, então deixa como 0
    makecontext(&contextoTask, start_routine, 1, (char *)arg); // Cria o contexto da task no endereço do contextoTask
    taskCriada->task->context = contextoTask;                  // Define o contexto da task no elemento de fila como o contextoTask
    taskCriada->task->preemptable = 0;                         // Define a variavel preempable da task do elemento de fila como 0, ou seja não preemptável

    queue_append((&filaTarefas), taskCriada);
}

//Muda para outra tarefa, transfere o processador para a tarefa indicada.
int task_switch(task_t *task)
{

	filatask_t *proxima = filaTarefas, *atual = filaTarefas;		

    	for(;;)
    	{
		proxima = proxima->next;
		if(proxima == NULL){
	       		fprintf(stderr, "Elemento nulo detectado - task_switch\n");
			return -1;
		}
		if(proxima == filaTarefas){
			fprintf(stderr, "Tarefa nao encontrada - task_switch\n");
			return -2;
		}
		if(proxima == task) break;
    	}
	/* Setamos a tarefa a ser trocada como a tarefa ativa */
	filaTarefas = proxima;
	/* E trocamos de contexto, salvando o contexto atual */
	swapcontext(&(atual->context), &(proxima->context));
	/* Retorno com sucesso */
	return 0;
}

// Termina a tarefa e volta para a tarefa 'main'.
//      - Deve ser usado a função swap_task para voltar para o main
void task_exit(int exit_code)
{
	filatask_t *atual = filaTarefas, *proxima = filaTarefas;
    	for(;;)
    	{
		proxima = proxima->next;
		if(proxima == NULL){
	       		fprintf(stderr, "Elemento nulo detectado - task_switch\n");
			return -1;
		}
		if(proxima == filaTarefas){
			fprintf(stderr, "Main nao encontrada - task_switch\n");
			return -2;
		}
		/* Se encontrou a main, break */
		if(proxima->id == 0) break;
    	}
	/* Removemos a tarefa e desalocamos */
	queue_remove(&filaTarefas, atual);
	free(atual);

	/* Trocamos para a main */
	if(swap_task(proxima) < 0) fprintf(stderr, "Erro ao trocar para a main - task_exit\n");
}

// Retorna o identificador da tarefa atual.
//      - Para a main deve ser 0.
//      - Não podem existir duas tarefas com o mesmo nome
int task_id()
{
	return filaTarefas->id;
}
