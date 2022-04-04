#include "ppos.h"

void ppos_init()
{

/* desativa o buffer da saida padrao (stdout), usado pela função printf */
setvbuf (stdout, 0, _IONBF, 0) ;

}

int task_create(task_t *task, void (*start_routine)(void *), void *arg)
{

}

int task_switch(task_t *task)
{

}

void task_exit(task_t *task)
{

}

int task_id()
{

}
