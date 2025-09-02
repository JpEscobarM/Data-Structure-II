
#include <stdio.h>
#include <stdlib.h>
#define ORDEM 2
#define NUM_CHAVES (2*ORDEM)
#define NUM_FILHOS (NUM_CHAVES+1)
#define VAZIO -99999
#define MIN_CHAVES ORDEM

typedef struct no
{

    int *chaves;
    int qtdChaves;
    int folha;
    struct no *pai;
    struct no **filhos;



} No;

typedef struct
{
    No *raiz;


} Arvore;



No *criarNo(int folha)
{
    No *novo = malloc(sizeof(No));
    if(novo == NULL)
    {
        perror("<noCriar>");
        exit(EXIT_FAILURE);
    }


    novo->qtdChaves =0;
    novo->folha = folha;
    novo->chaves = malloc(sizeof(int)*(NUM_CHAVES+1)); //OVERFLOW PRO NÓ
    novo->filhos = malloc(sizeof(No*)*NUM_FILHOS);

    if( novo->chaves == NULL || novo->filhos == NULL)
    {
        perror("<noCriar>");
        exit(EXIT_FAILURE);
    }

    for(int i =0 ; i < NUM_FILHOS; i ++ )
    {
        novo->filhos[i] = NULL;
    }

    for(int i =0 ; i < NUM_CHAVES+1; i ++ )
    {
        novo->chaves[i] = VAZIO;
    }

    return novo;
}


int addChave(No *atual,int k)
{
    int i =atual->qtdChaves-1;

    while(i >= 0 && k < atual->chaves[i])
    {
        atual->chaves[i+1] = atual->chaves[i];
        i--;
    }


    if(atual->chaves[i+1] == k)
    {
        return;
    }

    int posicao = i+1;
    atual->chaves[posicao] = k;

    atual->qtdChaves++;

return posicao;
}


No *buscarNo(No *no, int ch)
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
    } else if (no->filhos[i] != NULL) {
         return buscarNo(no->filhos[i], ch);
    } else return no;

    } else return NULL; //nó é NULL, não há como buscar

}

void imprimeNo( No *atual)
{
    if (!atual) { printf("(no NULL)\n"); return; }

    printf("No %p  folha=%d  qtdChaves=%d  pai=%p\n",
           &atual, atual->folha, atual->qtdChaves, &atual->pai);


    printf("  chaves do no: ");
    if (atual->qtdChaves == 0) {
        printf("(vazio)");
    } else {
        for (int i = 0; i < atual->qtdChaves; ++i) {
            printf("%d%s", atual->chaves[i], (i+1 < atual->qtdChaves) ? " " : "");
        }
    }
    printf("\n");


    if (!atual->folha) {
        printf("  filhos usados: ");
        for (int i = 0; i <= atual->qtdChaves; ++i) {
            printf("[%d]=%p ", i, &atual->filhos[i]);
        }
        printf("\n");
    }
}

void bfs(No *atual,int nivel)

{


    printf("\nNIVEL: %d", nivel);
    printf("\n[");
    for(int i =0;i<atual->qtdChaves;i++)
    {
        printf("(%d)",atual->chaves[i]);
    }
    printf("]\n");

    for(int i=0;i<NUM_FILHOS;i++)
    {
        if(atual->filhos[i] !=NULL)
        {
            bfs(atual->filhos[i],nivel+1);
        }
    }
}
void split(Arvore *arv, No *filhoPrincipal)
{

     if (!arv || !filhoPrincipal) return;

    int i;
    int j;
    No *novoIrmao = criarNo(filhoPrincipal->folha);
    No *novaRaiz = criarNo(0);


    if(arv && arv->raiz == filhoPrincipal) // SPLIT NA RAIZ
    {



        j=0;
        for(i = ORDEM+1; i < filhoPrincipal->qtdChaves ; i++)
        {
            novoIrmao->chaves[j] = filhoPrincipal->chaves[i];
            novoIrmao->qtdChaves++;

            filhoPrincipal->chaves[i] = VAZIO;
            j++;

        }
        filhoPrincipal->qtdChaves-= j ;

        novaRaiz->chaves[0]=filhoPrincipal->chaves[ORDEM];
        novaRaiz->qtdChaves++;
        filhoPrincipal->qtdChaves--;

        novaRaiz->filhos[0] = filhoPrincipal;
        novaRaiz->filhos[1] = novoIrmao;



        filhoPrincipal->pai = novaRaiz;
        novoIrmao->pai = novaRaiz;



        if(!arv->raiz->folha)
        {
            j=0;
            for(int i = ORDEM; i < NUM_FILHOS ; i++)
            {
                novoIrmao->filhos[j] = filhoPrincipal->filhos[i];
                filhoPrincipal->filhos[i] = NULL;
                j++;

            }
        }

        novaRaiz->pai = NULL;
        arv->raiz=novaRaiz;

    }//LIDANDO COM RAIZ
    else //NORMAL
    {

        No *pai = filhoPrincipal->pai;
        j=0;
        for(i = ORDEM+1; i < filhoPrincipal->qtdChaves ; i++)
        {
            novoIrmao->chaves[j] = filhoPrincipal->chaves[i];
            novoIrmao->qtdChaves++;

            filhoPrincipal->chaves[i] = VAZIO;
            j++;

        }
        filhoPrincipal->qtdChaves-= j ;

        int posicao = pai->qtdChaves;


        pai->chaves[posicao]=filhoPrincipal->chaves[ORDEM];
        pai->qtdChaves++;
        filhoPrincipal->chaves[ORDEM] = VAZIO;
        filhoPrincipal->qtdChaves--;

        if(!filhoPrincipal->folha)
        {
            j=0;
            for(int i = ORDEM; i < NUM_FILHOS ; i++)
            {
                novoIrmao->filhos[j] = filhoPrincipal->filhos[i];
                filhoPrincipal->filhos[i] = NULL;
                j++;

            }
        }

        pai->filhos[posicao] = novoIrmao;

    }



}

void inserir(Arvore *arv, int chave)
{

    if(!arv->raiz)
    {

        arv->raiz = criarNo(1);
        addChave(arv->raiz,chave);
        return;
    }

    No *noBuscado= buscarNo(arv->raiz,chave);

    if(noBuscado == NULL)
    {
        printf("BUSCA NULL EM INSERCAO");
        return;
    }

    for(int i = 0 ; i < noBuscado->qtdChaves;i++)
    {
        if(noBuscado->chaves[i] == chave)
        {
        printf("Chave ja adicionada");
            return;
        }
    }

    addChave(noBuscado,chave);

    if(noBuscado->qtdChaves > NUM_CHAVES)
    {
        split(arv,noBuscado);
    }






}

int main()
{
/*
    Arvore *arv = malloc(sizeof(Arvore));

    No *filho2 = criarNo(1);
    No *pai = criarNo(0);
    filho2->chaves[0] = 10;
    filho2->chaves[1] = 20;
    filho2->chaves[2] = 30;
    filho2->qtdChaves = 3;

    No *pai = criarNo(0);
    pai->chaves[0] = 5;
    pai->qtdChaves++;

    No *filho1 = criarNo(1);
    filho1->chaves[0] = 1;
    filho1->chaves[1] = 2;
    filho1->chaves[2] = 3 ;
    filho1->chaves[3] = 4 ;
    filho1->chaves[4] = 5 ;

    filho1->qtdChaves=5;

    arv->raiz = filho1;

    split(arv,filho1);
    bfs(arv->raiz,0);
    */

    Arvore *arv = malloc(sizeof(Arvore));
    arv->raiz = NULL;

    inserir(arv,1);
    inserir(arv,2);
    inserir(arv,3);
    inserir(arv,4);
    inserir(arv,5);
    inserir(arv,6);
    inserir(arv,7);
    inserir(arv,8);
    inserir(arv,9);

    bfs(arv->raiz,0);
    return 0;
}
