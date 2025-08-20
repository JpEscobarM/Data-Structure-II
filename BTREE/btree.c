#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define ORDEM 1


typedef struct no
{
    int qtdChaves;

    int *chaves;

    struct no *pai; //adicional
    struct no **filhos;

    bool folha; //adicional

} No;


No *criaNo()
{



    No *novo = malloc(sizeof(No));
    int qtdFilhos = (2*ORDEM) + 1;



    if(novo)
    {
        novo->qtdChaves = 0;
        novo->pai = NULL;



        novo->filhos = malloc(sizeof(No*)*qtdFilhos);
        for(int i = 0; i < qtdFilhos ; i ++)
        {
            *(novo->filhos + i)  = NULL;
        }





    }
    else
    {
        printf("\nErro ao alocar novo no");
    }
    return novo;
}
No *busca(No *no, int ch)
{
    if (no != NULL)
    {
        int i = 0;
        while (i < no->qtdChaves && ch > no->chaves[i])
        {
            i++;
        }
        if (i < no->qtdChaves && ch == no->chaves[i])
        {
            return no; // encontrou chave
        }
        else if (no->filhos[i] != NULL)
        {
            return busca(no->filhos[i], ch);
        }
        else return no;   //nó era folha -- não existem mais
        //nós a buscar, então retorna o nó onde a chave deveria estar
    }
    else return NULL;   //nó é NULL, não há como buscar
}

void ordenaChaves(int *v,int tam)
{

int aux;
        for(int i = 0; i < tam ; i ++)
        {
            for(int j =0; j < tam ; j ++)
            {
                if(v[i] > v[j])
                {
                    aux = v[j];
                    v[j] = v[i];
                    v[i] = aux;

                }
            }
        }

}

void inserir(No **raiz, int x)
{

    if(*raiz)
    {


    No *noBusca = busca(*raiz,x);

        if(noBusca == NULL)
        {
            printf("\nerro ao buscar no");
        }
        else
        {


             ordenaChaves(noBusca->chaves, noBusca->qtdChaves);



        }
    }
    else
    {
        printf("\nErro raiz nula");
    }


}

int main()
{

    No *raiz = NULL;

    int v[5] = {2,4,5,1,3};

    ordenaChaves(v,5);


    return 0;
}
