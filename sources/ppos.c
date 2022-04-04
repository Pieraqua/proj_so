#include "ppos.h"
#include "ppos_core.h"

filatask_t *filaTarefas;

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
