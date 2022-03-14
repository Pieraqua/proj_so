// Mariana Gomes Paulo Sérgio Ávila Júnior
#include <stdio.h>
#include "queue.h"
//------------------------------------------------------------------------------
// Conta o numero de elementos na fila
// Retorno: numero de elementos na fila

int queue_size(queue_t *queue)
{
    int contador = 0;
    queue_t *atual = queue;
    if (queue == NULL)
    {
        return 0;
    }
    do
    {
        atual = atual->next;
        contador++;
    } while (atual != queue);
    return contador;
}

//------------------------------------------------------------------------------
// Percorre a fila e imprime na tela seu conteúdo. A impressão de cada
// elemento é feita por uma função externa, definida pelo programa que
// usa a biblioteca. Essa função deve ter o seguinte protótipo:
//
// void print_elem (void *ptr) ; // ptr aponta para o elemento a imprimir

void queue_print(char *name, queue_t *queue, void print_elem(void *))
{
    queue_t *atual = queue;
    if (queue == NULL)
    {
        fprintf(stderr, "### Erro: tentou ler uma fila vazia\n");
        return;
    }
    printf("%s [", name);
    do
    {
        print_elem(atual);
        printf(" ");
        atual = atual->next;
    } while (atual != queue);
    printf("] \n")
}

//------------------------------------------------------------------------------
// Insere um elemento no final da fila.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - o elemento deve existir
// - o elemento nao deve estar em outra fila
// Retorno: 0 se sucesso, <0 se ocorreu algum erro

int queue_append(queue_t **queue, queue_t *elem)
{
    queue_t *primeiro, *ultimo;
    if (queue == NULL)
    {
        fprintf(stderr, "### Erro: tentou ler uma fila não existe\n");
        return -1;
    }
    if (elem == NULL)
    {
        fprintf(stderr, "### Erro: tentou adicionar um elemento que não existe\n");
        return -2;
    }
    if ((elem->next != NULL) || (elem->prev != NULL))
    {
        fprintf(stderr, "### Erro: tentou adicionar um elemento que já está em outra fila\n");
        return -3;
    }
    // Se a fila estava vazia
    if (*queue == NULL)
    {
        *queue = elem;
        (*queue)->next = elem;
        (*queue)->prev = elem;
        return 0;
    }
    primeiro = (*queue);
    ultimo = primeiro->prev;
    ultimo->next = elem;
    primeiro->prev = elem;
    elem->next = primeiro;
    elem->prev = ultimo;
    return 0;
}

//------------------------------------------------------------------------------
// Remove o elemento indicado da fila, sem o destruir.
// Condicoes a verificar, gerando msgs de erro:
// - a fila deve existir
// - a fila nao deve estar vazia
// - o elemento deve existir
// - o elemento deve pertencer a fila indicada
// Retorno: 0 se sucesso, <0 se ocorreu algum erro

int queue_remove(queue_t **queue, queue_t *elem)
{
    queue_t *atual, *inicial;
    if (queue == NULL)
    {
        fprintf(stderr, "### Erro: tentou ler uma fila não existe\n");
        return -1;
    }
    if (*queue == NULL)
    {
        fprintf(stderr, "### Erro: tentou ler uma fila vazia\n");
        return -2;
    }
    if (elem == NULL)
    {
        fprintf(stderr, "### Erro: tentou adicionar um elemento que não existe\n");
        return -3;
    }
    inicial = *queue;
    if (inicial == elem)
    {
        if (inicial->next == elem)
        {
            (*queue)->next = NULL;
            (*queue)->prev = NULL;
            (*queue) = NULL;
            return 0;
        }
        (*queue) = elem->next;
        (elem->prev)->next = (elem->next);
        (elem->next)->prev = (elem->prev);
        elem->prev = NULL;
        elem->next = NULL;
        return 0;
    }
    atual = inicial;
    while (atual->next != elem)
    {
        atual = atual->next;
        if (atual == NULL)
        {
            fprintf(stderr, "### Erro: A fila não está bem formada\n");
            return -4;
        }
        if (atual == inicial)
        {
            fprintf(stderr, "### Erro: O elemento não pertence a fila\n");
            return -5;
        }
    }
    (elem->prev)->next = (elem->next);
    (elem->next)->prev = (elem->prev);
    elem->prev = NULL;
    elem->next = NULL;
    return 0;
}
