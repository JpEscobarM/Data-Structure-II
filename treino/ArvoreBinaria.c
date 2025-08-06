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

void imprimirV1(No **raiz)
{
    if(*raiz)
    {
        printf("%d ",(*raiz)->num);
        imprimirV1(&(*raiz)->esq);
        imprimirV1(&(*raiz)->dir);

    }

}

void imprimirV2(No **raiz)
{
    if(*raiz)
    {

        imprimirV2(&(*raiz)->esq);
        printf("%d ",(*raiz)->num);
        imprimirV2(&(*raiz)->dir);

    }

}


int
main
(
 )
 {

 No *raiz = NULL;



    inserirV1(&raiz,2);
    inserirV1(&raiz,3);
    inserirV1(&raiz,1);



    imprimirV1(&raiz);

return 0;
}
