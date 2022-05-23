/* Paulo Sergio Avila Junior e Mariana Gomes Luz */
/* Aula 9 - Next Fit */

#include <unistd.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>
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
            if ((vetorRepresentacao[i - 1] == 0))
            {
                livreFragmento = livreFragmento + 1;
            }
            medioFragmento = contador + medioFragmento;
            contador = 0;
        }
    }
    if (vetorRepresentacao[4095] == false)
    {
        if (contador > maiorFragmento)
        {
            maiorFragmento = contador;
        }
        if (contador < menorFragmento)
        {
            menorFragmento = contador;
        }
        // if (vetorRepresentacao[i - 1] == 0)
        //{
        //     livreFragmento = livreFragmento + 1;
        // }
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
    int melhorFragmento = 0;
    int tamanhoMelhorFragmento = 4097;
    for (i = 0; i < 4096; i++)
    {

        if (vetorRepresentacao[i] == false)
        {
            contador++;
        }
        else
        {
            if (contador >= bloco)
            {
                if (tamanhoMelhorFragmento > contador)
                {
                    melhorFragmento = i - contador;
                    tamanhoMelhorFragmento = contador;
                }
            }
            contador = 0;
        }
    }

    if (contador >= bloco)
    {
        if (tamanhoMelhorFragmento > contador)
        {
            melhorFragmento = i - contador;
            tamanhoMelhorFragmento = contador;
        }
        contador = 0;
    }
    for (j = melhorFragmento; j < (bloco + melhorFragmento); j++)
    {
        vetorRepresentacao[j] = true;
    }
}

int main()
{
    int i;
    int tamanhoBloco = 0;
    srand(time(NULL));
    for (i = 0; i < 4096; i++)
    {
        vetorRepresentacao[i] = false;
    }
    for (i = 0; i <= 10; i++)
    {
        tamanhoBloco = rand() % 409;
        printf("%d\n", tamanhoBloco);
        gerenciamento(tamanhoBloco);
    }
    fragmento();
    printf("O tamanho do maior fragmento é %i\n\nO tamanho do menor fragmento é %i\n\nO tamanho médio dos fragmentos é %i\n\nA quantidade de fragmentos livre é %i\n\n", maiorFragmento, menorFragmento, medioFragmento, livreFragmento);
    return 0;
}
