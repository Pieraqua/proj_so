/* Paulo Sergio Avila Junior e Mariana Gomes Luz */
/* Aula 8 - semaforos */

#include <unistd.h>
#include <malloc.h>

#define BLOCSIZE 1 << 24

int main()
{
    char *c;
    int i;

    while (1)
    {
        fork();

        c = (char *)malloc(BLOCKSIZE);

        if (c)
        {
            for (i = 0; i < BLOCKSIZE; i++)
            {
                c[i] = i % 256;
            }
        }
        else
        {
            sleep(1);
        }
    }
}
