
#include <stdio.h>
#include <stdlib.h>
#define tam 6

int mediaPares(int *v, int tamanho){

      int somaTotalPares = 0;
        int quantidadePares = 0;

    for(int i = 0 ;  i < tamanho ; i ++)
    {

        if(*(v+i)%2 == 0)
        {
            somaTotalPares+= *(v+i);
            quantidadePares++;
        }


    }



return (quantidadePares > 0)? somaTotalPares/quantidadePares : 0 ;
}


int mediaRecursivo(int *v, int tamanho, int soma, int quanti, int posicao){

     printf("\tvalor %d , soma %d, quantidade de pares %d, posicao atual %d\n",*(v+posicao), soma, quanti, posicao);

    if( posicao == tamanho  )
    {

        return soma/quanti;
    }





        if( *(v+posicao) % 2 == 0)
        {
            soma+= *(v+posicao);
            quanti++;
        }
        posicao++;
       return mediaRecursivo(v,tamanho,soma,quanti,posicao);


}

void teste()
{
    static int x= 0;

    printf("\nX = %d",x);
    x++;
}

int main()
{

    int vetor[tam] = {1,2,3,4,5,6};



     printf("Media dos pares do vetor: %d\n", mediaRecursivo(vetor,tam,0,0,0));


     for(int i = 0; i < 10; i ++)
     {
         teste();
     }

    return 0;
}
