#include <stdio.h>
#include <stdlib.h>
#define ORDEM 2

#define MAX_CHAVES (2*ORDEM)

#define MIN_CHAVES (ORDEM)

#define MAX_FILHOS ((MAX_CHAVES)+1)

typedef struct
{
    int *chaves;
    int qtdChaves;

    struct no *pai;
    struct no **filhos;

    int folha;

    struct no *prox;

}No;


typedef struct
{

    No *raiz;


}Arvore;


void imprimeNo(No *imprimir)
{
    if (!imprimir) {
        printf("\n[!] No nulo\n");
        return;
    }

    printf("\n\n====================");
    printf("\nNO EM %p", imprimir);
    printf("\n--------------------");


    if (imprimir->qtdChaves == 0) {
        printf("\nChaves: [vazio]");
    } else {
        printf("\nChaves: [");
        for (int i = 0; i < imprimir->qtdChaves; i++) {
            printf("%d", imprimir->chaves[i]);
            if (i < imprimir->qtdChaves - 1) printf(", ");
        }
        printf("]");
    }

    printf("\nPai: %p", imprimir->pai);


    if (!imprimir->folha && imprimir->filhos != NULL) {
        printf("\nFilhos:");

        int j = 0;
        while (j < MAX_FILHOS + 1 && imprimir->filhos[j] != NULL) {
            printf("\n   Filho[%d] -> %p", j, (void *)imprimir->filhos[j]);
            j++;
        }

        printf("\nTotal de filhos: %d", j);
    } else {
        printf("\nEste no e uma folha (sem filhos)");
    }

    printf("\n====================\n");
}

/*==========================CRIAR NÓS===========*/

No *criaNo(No *pai, int folha, int debug){
    No *novo = malloc(sizeof(No));
    if(debug) printf("\nENDERECO DO NO CRIADO: %p", novo);

    if(!novo)
    {
        printf("\n|criaNo| erro: nao foi possivel alocar memoria para novo nó");
        return NULL;
    }

    novo->pai = pai;
    novo->folha = folha;
    novo->qtdChaves =0;
    novo->filhos = NULL;
    novo->prox = NULL;

    novo->chaves = malloc(sizeof(int)*(MAX_CHAVES+1)); //+1 para overflow de chave

    if(!folha)
    {
        if(debug)printf("\n|criaNo| vem de um irmao nao folha(no interno)\n");

        novo->filhos = malloc(sizeof(No*)*(MAX_FILHOS+1)); //+1 para overflow de filho

        for(int i=0; i < MAX_FILHOS+1;i++)
        {
            novo->filhos[i]= NULL;
        }

    }

    if(debug) imprimeNo(novo);

return novo;
}

No *criaNoTeste(int debug) {

    No *folha1 = criaNo(NULL, 1, 0);
    folha1->chaves[0] = 5;
    folha1->chaves[1] = 10;
    folha1->chaves[2] = 15;
    folha1->qtdChaves = 3;
    imprimeNo(folha1);


    No *folha2 = criaNo(NULL, 1, 0);
    folha2->chaves[0] = 20;
    folha2->chaves[1] = 30;
    folha2->chaves[2] = 35;
    folha2->qtdChaves = 3;
    imprimeNo(folha2);

    No *folha3 = criaNo(NULL, 1, 0);
    folha3->chaves[0] = 40;
    folha3->chaves[1] = 45;
    folha3->chaves[2] = 55;
    folha3->qtdChaves = 3;
    imprimeNo(folha3);


    No *folha4 = criaNo(NULL, 1, 0);
    folha4->chaves[0] = 60;
    folha4->chaves[1] = 70;
    folha4->chaves[2] = 80;
    folha4->qtdChaves = 3;
    imprimeNo(folha4);

    // Encadeamento das folhas (ponteiro para a próxima)
    folha1->prox = folha2;
    folha2->prox = folha3;
    folha3->prox = folha4;

    // Criar nó raiz
    No *raiz = criaNo(NULL, 0, 0);
    raiz->chaves[0] = 20;
    raiz->chaves[1] = 40;
    raiz->chaves[2] = 60;
    raiz->qtdChaves = 3;
    imprimeNo(raiz);

    raiz->filhos[0] = folha1;
    raiz->filhos[1] = folha2;
    raiz->filhos[2] = folha3;
    raiz->filhos[3] = folha4;

    // Atualiza ponteiro para o pai nas folhas
    folha1->pai = raiz;
    folha2->pai = raiz;
    folha3->pai = raiz;
    folha4->pai = raiz;

    return raiz;
}


/*======================================================================*/



/*========================BUSCA=========================*/
No *busca(No *atual, int chave, int debug) {
    if (!atual) return NULL;

    if (debug) {
        printf("\n>> Visitando no:");
        imprimeNo(atual);
    }

    int i = 0;

    if (!atual->folha) {

        while (i < atual->qtdChaves && chave >= atual->chaves[i]) {
            i++;
        }


        if (atual->filhos[i] != NULL)
            return busca(atual->filhos[i], chave, debug);
        else
            return NULL;
    } else {

        for (i = 0; i < atual->qtdChaves; i++) {
            if (atual->chaves[i] == chave) {
                if (debug) printf(">> Chave %d encontrada na folha.\n", chave);
                return atual;
            }
        }

        if (debug) printf(">> Chave %d não encontrada na folha.\n", chave);

        return atual;//RETORNA A FOLHA IGUAL PARA INSERÇÃO POIS MESMO SE A CHAVE NAO ESTA PRESENTE
        //A ENTAO A CHAVE DEVERIA ESTAR NAQUELA FOLHA
    }
}

/*===============================================================*/


/*=========================INSERÇÃO=============================*/

void addChaveEmFolha(No *atual,int k)
{
    int i= atual->qtdChaves-1;
    while( i>= 0 && k < atual->chaves[i])
    {
        atual->chaves[i+1] = atual->chaves[i];
        i--;
    }

    i++;
    atual->chaves[i] = k;
    atual->qtdChaves++;



}

void splitRaizFolha(No *raiz)
{
    No *novoIrmao = criaNo(NULL,raiz->folha);

    int divisor = raiz->chaves[ORDEM]; //PEGA A CHAVE QUE DIVIDE OS NÓS

    int i= raiz->qtdChaves-1;
    int j=0;

    while(i > ORDEM)
    {
        novoIrmao->chaves[j] = raiz->chaves[i];
        i--;
    }



}

void inserir(Arvore *arv,int chave,int debug)
{
    if(arv->raiz == NULL)
    {
        arv->raiz= criaNo(NULL,1,0); //SE NAO EXISTIR RAIZ CRIA UMA
    }

    if(debug)
    {
        printf("\nNo atual na insercao: ");
        imprimeNo(arv->raiz);
    }

    No *retorno = busca(arv->raiz,chave,debug);

    if(!retorno)
    {
        printf("\nNao foi possivel buscar");
        return;
    }

    for(int i = 0 ; i < retorno->qtdChaves;i++)
    {
        if(retorno->chaves[i] == chave)
        {
            printf("\n<inserir>No ja presente na arvore, insercao invalida\n");
            return;
        }
    }


    addChaveEmFolha(retorno,chave);

    if(retorno->qtdChaves > MAX_CHAVES)
    {
        //Caso II - ordem 2 -> max de chaves igual a 4 com + 1 para overflow

        //FAZER SPLIT

        //CASO RAIZ REALIZA SPLIT NORMAL + NOVA RAIZ

        //SE NÃO SÓ CRIA IRMAO

        if(retorno->pai == NULL retorno->folha) //RAIZ
        {

        }
        else
        {

        }

    }

}

/*============================================================*/


int main()
{

    No *raizTeste = criaNoTeste(1);

    No *resultadoBuscaTeste = busca(raizTeste,45,1);

return 0;
}
