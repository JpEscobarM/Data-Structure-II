#include <stdio.h>
#include <stdlib.h>

typedef struct no
{
    int num;


    struct no *esq;
    struct no *dir;

}No;


No* inserirV1(No **raiz, int num){


    if( *raiz == NULL)
    {

    No *novo = malloc(sizeof(No));

    novo->num = num;
    novo->dir = NULL;
    novo->esq = NULL;



     *raiz = novo;

     printf("Na funcao %d\n", (*raiz)->num);

     return novo;
    }

    if( num < (*raiz)->num)
    {
        (*raiz)->esq = inserirV1( &(*raiz)->esq, num);

    }
    else
    {

        (*raiz)->dir = inserirV1(&(*raiz)->dir, num);
    }

}

int
main
(
 )
 {

 No *raiz = NULL;

No *NoAlocado = NULL;


    raiz = inserirV1(&raiz,2);
            inserirV1(&raiz,3);

        inserirV1(&raiz,1);



      printf("%d", raiz->num);

return 0;
}
