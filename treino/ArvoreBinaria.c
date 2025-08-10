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

No* removerNo(No **raiz, int chave){

    if(*raiz == NULL)
    {
        printf("\n valor nao encontrado");
               return NULL;
    }
    else
    {
        //procurar no remover
        if((*raiz)->num == chave)
        {
            //remover no folha
            if((*raiz)->esq == NULL && (*raiz)->dir == NULL)
            {
                printf("\nNo folha removido: %d",(*raiz)->num);
                free((*raiz));

                return NULL;
            }
            else //remover no com filho
            {
               if((*raiz)->esq != NULL && (*raiz)->dir != NULL) //SE O NO TEM 2 FILHOS
               {
                   //AO REMOVER NO COM DOIS FILHOS ==>>
                    //ELEMENTO MAIS A DIREITA DA SUBARVORE A ESQUERDA
                    // OU ELEMENTO MAIS A ESQUERDA DA SUBARVORE A DIREITA
                No *aux = (*raiz)->esq;
                while ( aux->dir != NULL)
                {
                    aux = aux->dir;
                }
                (*raiz)->num = aux->num; //TROCA O CONTEUDO DO NO REMOVIDO PELO MAIS A DIREITA
                aux->num = chave;
                printf("\n No com dois filhos removido");
                (*raiz)->esq = removerNo(&(*raiz)->esq,chave);//CHAMA RECURSIVO PRA REMOVER O NO FOLHA COM A CHAVE
                return *raiz;
               }
               else //SE TIVER 1 FILHO
               {
                    No *aux;
                    if((*raiz)->esq != NULL)
                    {
                        aux =(*raiz)->esq;
                    }
                    else
                    {
                        aux =(*raiz)->dir;
                    }
                    printf("\nNo removido: %d",(*raiz)->num);
                    free(*raiz);
                    return aux;
               }

            }
        }
        else
        {
            if( chave < (*raiz)->num)
            {
                (*raiz)->esq = removerNo(&(*raiz)->esq,chave);
            }
            else
            {
                (*raiz)->dir = removerNo(&(*raiz)->dir,chave);
            }
            return *raiz;
        }
    }


}


int main()
{

    No *raiz = NULL;
    No *busca = NULL;
    int op,valor;

    srand(time(NULL));

 /*   for(int i = 0 ; i < 15; i ++)
    {

        inserirV2(&raiz,rand()%100);
    }
*/
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
         printf("\n| 7 - REMOVER");
         printf("\n");
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
        case 7:
              printf("\nDIGITE O VALOR A SER REMOVIDO:_");
            scanf("%d", &valor);
             raiz = removerNo(&raiz,valor);
            break;

        }

    }



    return 0;
}
