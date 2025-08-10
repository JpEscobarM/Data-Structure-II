#include <stdio.h>
#include <stdlib.h>
#include <time.h>>


typedef struct no
{
    int num;


    struct no *esq;
    struct no *dir;

} No;


No* inserirV1(No **raiz, int num)
{


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

void inserirV2(No **raiz,int num)
{

    if(*raiz == NULL)
    {

        *raiz = malloc(sizeof(No));
        (*raiz)->num = num;
        (*raiz)->dir = NULL;
        (*raiz)->esq = NULL;
    }
    else
    {

        if(num < (*raiz)->num)
        {

            inserirV2(&(*raiz)->esq,num);

        }
        else
        {
            inserirV2(&(*raiz)->dir,num);
        }
    }

}

void inserirV3(No **raiz, int num)
{
    No *aux = *raiz;


    while(aux)
    {
        if(num < aux->num) raiz = &(aux->esq);
        else raiz = &(aux->dir);

        aux = *raiz;
    }
    aux = malloc(sizeof(No));
    aux->num = num;
    aux->dir = NULL;
    aux->esq = NULL;

    *raiz = aux;

}


//BUSCAS
No* buscarV1(No **raiz,int num)
{
    if(*raiz)
    {
        if(num == (*raiz)->num)
        {
            return *raiz;
        }
        else if (num < (*raiz)->num)
        {
            return buscarV1(&(*raiz)->esq, num);
        }
        else
        {
            return buscarV1(&(*raiz)->dir,num);
        }
    }
    else
    {

        return NULL;
    }
}


void imprimirV1(No **raiz) //IMPRESSAO PRÉ ORDEM
{
    if(*raiz)
    {
        printf("%d ",(*raiz)->num);
        imprimirV1(&(*raiz)->esq);
        imprimirV1(&(*raiz)->dir);

    }

}

void imprimirV2(No **raiz) //ORDENADA
{
    if(*raiz)
    {

        imprimirV2(&(*raiz)->esq);
        printf("%d ",(*raiz)->num);
        imprimirV2(&(*raiz)->dir);

    }

}

int quantidadeNo(No **raiz)
{

    if(*raiz == NULL)
    {

        return 0;
    }
    else
    {
        return 1 + quantidadeNo(&(*raiz)->esq) + quantidadeNo(&(*raiz)->dir);

    }
}

int quantidadeFolhas(No **raiz)
{

    if(*raiz == NULL)
    {

        return 0;
    }
    else if((*raiz)->esq  == NULL && (*raiz)->dir  == NULL)
    {
        return 1;
    }
    else
    {
        return 0 + quantidadeFolhas(&(*raiz)->esq) + quantidadeFolhas(&(*raiz)->dir);
    }
}

int altura (No **raiz)
{

    if(*raiz == NULL)
    {
        return -1;
    }
    else
    {
        int esq = altura(&(*raiz)->esq);
        int dir = altura(&(*raiz)->dir);
        return (esq > dir ? esq : dir) + 1 ;
    }
}


int main()
{

    No *raiz = NULL;
    No *busca = NULL;
    int op,valor;

    srand(time(NULL));

    for(int i = 0 ; i < 15; i ++)
    {

        inserirV2(&raiz,rand()%100);
    }

    op = 1;



    while(op != 0 )
    {
        printf("\nEscolha uma opcao: ");
        printf("\n 0 - SAIR  \n");
        printf("\n| 1 - INSERIR ");
        printf("\n| 2 - IMPRIMIR  ");
        printf("\n| 3 - BUSCAR ");
        printf("\n| 4 - ALTURA  ");
        printf("\n| 5 - NRO DE NOS");
         printf("\n| 6 - NRO DE FOLHAS");
        scanf("%d", &op);
        switch (op)
        {
        case 0:
            printf("\nSaindo...");
            break;
        case 1:
            printf("\nDIGITE O VALOR:_");
            scanf("%d", &valor);
            inserirV3(&raiz,valor);

            break;
        case 2:
            imprimirV2(&raiz);
            break;
        case 3:
            printf("\nDIGITE O VALOR A SER BUSCADO:_");
            scanf("%d", &valor);
            busca = buscarV1(&raiz,valor);

            if(busca)
            {
                printf("\nValor encontrado: %d", busca->num);
            }
            else
            {

                printf("\nValor nao encontrado na arvore");
            }

            break;
        case 4:
            printf("\n ALTURA: %d", altura(&raiz));
            break;

        case 5:
            printf("\nNOS: %d", quantidadeNo(&raiz));
            break;
        case 6:
            printf("\FOLHAS: %d", quantidadeFolhas(&raiz));
            break;

        }

    }



    return 0;
}
