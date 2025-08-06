#include <stdio.h>
#include <stdlib.h>

int main()
{

    int *vet;
    int tamanho;
    printf("\nDigite um tamanho para o vetor: ");
    scanf("%d",&tamanho);

    vet = malloc(sizeof(int)*tamanho);

    for(int i = 0 ; i < tamanho; i++)
    {

        *(vet + i ) = 0;
    }

    for(int i = 0 ; i < tamanho; i++)
    {

        printf("%d",*(vet + i ));
    }



return 0;
}
