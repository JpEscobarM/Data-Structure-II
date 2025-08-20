#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define ORDEM 1
#define NUM_CHAVES 2*ORDEM
#define NUM_FILHOS (2*ORDEM)+1
#define VAZIO -99999
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
    int qtdFilhos = NUM_FILHOS;



    if(novo != NULL)
    {
        novo->qtdChaves = 0;
        novo->pai = NULL;


      novo->chaves = malloc(sizeof(int)*(NUM_CHAVES+1));


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
                if(v[i] < v[j])
                {
                    aux = v[j];
                    v[j] = v[i];
                    v[i] = aux;

                }
            }
        }

}

int verificaFolha(No *n)
{

     n->folha = 1;
    for(int i =0; i < NUM_FILHOS ; i++)
    {
        if(n->filhos[i] != NULL)
        {
            n->folha = 0;
            return 0;
        }


    }


return 1;
}

int adicionaChave(No *no,int chave)
{

    if(no->qtdChaves == NUM_CHAVES+1)
    {
        printf("\nNumero maximo de chaves adicionadas em um NO - OVERFLOW");
        return 0;
    }
    else
    {
    no->chaves[no->qtdChaves] = chave;
    no->qtdChaves++;

    ordenaChaves(no->chaves,no->qtdChaves);

        if(no->qtdChaves >NUM_CHAVES)
        {
            return 2; //FAZER SPLIT
        }
     return 1;
    }

}

int *separa(int *inicio,int tam)
{
    int *meio;

   if(tam%2 == 0 )
   {

       meio = (inicio+((tam/2)-1));
   }
    else
    {
      tam-1;
     meio = (inicio+(tam/2));
    }
        return meio;
}


void split(No *no, No *novoIrmao)
{

    int *meio = separa(no->chaves,no->qtdChaves);

    for(int i = (meio - no->chaves)+1 ; i < no->qtdChaves; i++ )
    {
        adicionaChave(novoIrmao,no->chaves[i]);
        no->chaves[i] = VAZIO;
    }


}

void inserir(No **raiz, int chave)
{

    if(*raiz)
    {


    No *noBusca = busca(*raiz,chave);

        if(noBusca == NULL)
        {
            printf("\nerro ao buscar no");
        }
        else
        {


             ordenaChaves(noBusca->chaves, noBusca->qtdChaves);

             for(int i = 0 ; i < noBusca->qtdChaves; i ++)
             {
                 if(noBusca->chaves[i] == chave) //SE A CHAVE ESTÁ EM UM NÓ RETORNADO PELA BUSCA A INSERCAO É INVALIDA
                 {

                     printf("\nInserção invalida chave já presente em um no");
                 }
             }
             verificaFolha(noBusca);

            if(noBusca->folha && noBusca->qtdChaves < NUM_CHAVES) //É FOLHA E TEM ESPAÇO PARA MAIS UMA CHAVE
            {
              adicionaChave(noBusca,chave);

            }

            if(noBusca->qtdChaves > NUM_CHAVES){ //SPLIT

                if(noBusca->pai == NULL) //RAIZ
                {
                    No *novaRaiz = criaNo();
                    No *irmao = criaNo();

                    //FUNCAO SPLIT PAASSANDO (noBusca,NoIrmao,PAI OU NULL):
                }


            }

        }
    }
    else
    {
        printf("\nErro raiz nula");
    }


}

void printNo(No *no)
{
    printf("\nQuantidade de chaves atual do no: %d",no->qtdChaves);

    printf("\ne folha: %d",no->folha);

    printf("\nChaves-> ");
    for(int i =0; i < no->qtdChaves; i ++)
    {
        printf("|%d| ",no->chaves[i]);
    }
}

int main()
{


    No *no = criaNo();
    No *irmao = criaNo();


    adicionaChave(no,1);

    adicionaChave(no,2);

    adicionaChave(no,3);

    split(no,irmao);


    return 0;
}
