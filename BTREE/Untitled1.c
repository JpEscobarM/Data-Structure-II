#include <stdio.h>
#include <stdlib.h>
#define ORDEM 1
#define MAX_CHAVES (2*ORDEM)
#define MAX_FILHOS (MAX_CHAVES+1)
#define MIN_CHAVES ORDEM
#define MIN_FILHOS ORDEM+1


typedef struct no
{
    int quantidadeChaves;
    int *chaves;
    int folha;
    struct no **filhos;
    struct no *pai;

}No;



No *criaNo(No *pai,int folha){

    No *novo = malloc(sizeof(No));

    if(!novo)
    {
        return NULL;
    }

    novo->quantidadeChaves = 0;
    novo->chaves = malloc(sizeof(int)*(MAX_CHAVES+1)); // MAIS UM ESPAÇO PARA O OVERFLOW
    novo->folha = folha;
    novo->filhos = malloc(sizeof(No*)*MAX_FILHOS);
    novo->pai = pai;

      for (int i = 0; i < MAX_FILHOS; i++) {
        novo->filhos[i] = NULL;
    }


    return novo;
}

typedef struct
{
    No *raiz;

}Arvore;


No *busca(No *atual,int chave)
{

    if(!atual)
    {
        return NULL;
    }
    int i =0;

    while(i < atual->quantidadeChaves && chave > atual->chaves[i])
    {
        i++;
    }

    if(atual->chaves[i] == chave && i < atual->quantidadeChaves)//ENCONTROU
    {
        return atual;
    }
    else if( atual->filhos[i] != NULL) //PERCORREU E NAO ENCONTROU
    {
       return busca(atual->filhos[i], chave);
    }
    else
    {
        return atual;//NO ERA FOLHA E NAO ACHOU A CHAVE, RETORNA ONDE
        //A CHAVE DEVERIA ESTAR
    }

}


void inserir(No *raiz,int chave)
{
/*
EXECUTAR ALGORITIMO DE BUSCA
SE ACHOU A CHAVE A INSERÇÃO É INVALIDA
SE NAO ACHOU ENTAO INSERE
*/


   No *noBusca = busca(raiz,chave);

    for(int i = 0 ; i < noBusca->quantidadeChaves ; i++)
    {
        if(noBusca->chaves[i] == chave)
        {
            printf("\nINSERÇÃO INVALIDA: CHAVE JA EXISTENTE\n");
            return;
        }
    }

    int i = noBusca->quantidadeChaves - 1;

    while (i >= 0 && chave < noBusca->chaves[i]) {
        noBusca->chaves[i+1] = noBusca->chaves[i]; //SHIFT A DIREITA
        i--;
    }


    int posicaoChave = i+1;
    noBusca->chaves[posicaoChave] = chave;
    noBusca->quantidadeChaves++;




    if(noBusca->quantidadeChaves >  MAX_CHAVES)
    {
        //SPLIT
        //SOBE O VALOR NA POSICAO ORDEM
        //DE ORDEM+1 VAO PRO NOVO IRMAO

    }

}

int main()
{
   Arvore *arvore = malloc(sizeof(Arvore));

   No *teste = criaNo(NULL,1);




   arvore->raiz = criaNo(teste,teste->folha);

    return 0;
}
