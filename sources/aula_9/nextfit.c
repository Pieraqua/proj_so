/* Paulo Sergio Avila Junior e Mariana Gomes Luz */
/* Aula 9 - Next Fit */

#include <unistd.h>
#include <malloc.h>

#define BLOCSIZE 1 << 24
char *blocoAnterior;

int main()
{
    char *c;
    int i;

    produz blocos de tamanho aleatorio
        gerencia os blocos retorna numero de fragmentos livres,
        tamanho do menor fragmento, tamanho do maior fragmento, tamanho médio dos fragmentos
}

char *achaArea(bloco, memoria)
{
    acha memoria ainda não usada que o bloco cabe
}

void gerenciamento(bloco)
{
    areaEscolhida = achaArea(bloco, blocoAnterior);
}