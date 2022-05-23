/* Paulo Sergio Avila Junior e Mariana Gomes Luz */
/* Aula 9 - First Fit */

#include <unistd.h>
#include <malloc.h>

#define BLOCSIZE 1 << 24
char *maiorFragmento;
char *menorFragmento;

typedef struct
{
    queue_t *prev, *next; // ponteiros para usar em filas
    char *inicio, fim;
    int tamanho;
} bloco_t;

int main()
{
    char *c;
    int i;
    bool vetorRepresentacao
        produz blocos de tamanho aleatorio
            ve tamanho medio dos blocos
                gerencia os blocos
                    retorna numero de fragmentos livres,
        tamanho do menor fragmento, tamanho do maior fragmento, tamanho médio dos fragmentos;
}

char *achaArea(bloco, memoria)
{
    acha memoria ainda não usada que o bloco cabe
}

void gerenciamento(bloco)
{
    areaEscolhida = achaArea(bloco, memoria);
}