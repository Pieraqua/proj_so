/* Paulo Sergio Avila Junior e Mariana Gomes Luz */
/* Aula 9 - First Fit */

#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdbool.h>

#define BLOCSIZE 1 << 24
int maiorFragmento = 0;
int menorFragmento = 4096;
int medioFragmento = 0;
int livreFragmento = 0;
bool vetorRepresentacao[4096];

void fragmento()
{
    int contador = 0;
    int i = 0;
    for (i = 0; i < 4096; i++)
    {
        if (vetorRepresentacao[i] == false)
        {
            contador++;
        }
        else
        {
            if (contador > maiorFragmento)
            {
                maiorFragmento = contador;
            }
            if (contador < menorFragmento && contador != 0)
            {
                menorFragmento = contador;
            }
            if (vetorRepresentacao[i - 1] == 0)
            {
                livreFragmento = livreFragmento + 1;
            }
            medioFragmento = contador + medioFragmento;
            contador = 0;
        }
    }
    if (vetorRepresentacao[4096] == false)
    {
        if (contador > maiorFragmento)
        {
            maiorFragmento = contador;
        }
        if (contador < menorFragmento)
        {
            menorFragmento = contador;
        }
        if (vetorRepresentacao[i - 1] == 0)
        {
            livreFragmento = livreFragmento + 1;
        }
        medioFragmento = contador + medioFragmento;
        contador = 0;
    }
    if (livreFragmento != 0)
    {
        medioFragmento = medioFragmento / livreFragmento;
    }
    else
    {
        medioFragmento = 0;
    }
}

void gerenciamento(int bloco)
{
    int contador = 0;
    int i = 0;
    int j = 0;
    for (i = 0; i < 4096; i++)
    {
        if (vetorRepresentacao[i] == false)
        {
            contador++;
        }
        else
        {
            contador = 0;
        }
        if (contador >= bloco)
        {
            for (j = 0; j < bloco; j++)
            {
                vetorRepresentacao[i - contador + j] = true;
            }
            break;
        }
    }
}

int main()
{
    int i;
    int tamanhoBloco = 0;
    for (i = 0; i < 4096; i++)
    {
        vetorRepresentacao[i] = false;
    }
    for (i = 0; i <= 10; i++)
    {
        tamanhoBloco = rand() % 409;
        gerenciamento(tamanhoBloco);
    }
    fragmento();
    printf("O tamanho do maior fragmento é %i\n\nO tamanho do menor fragmento é %i\n\nO tamanho médio dos fragmentos é %i\n\nA quantidade de fragmentos livre é %i", maiorFragmento, menorFragmento, medioFragmento, livreFragmento);
    return 0;
}
