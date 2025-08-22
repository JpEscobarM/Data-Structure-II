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


      novo->chaves = malloc(sizeof(int)*(NUM_CHAVES+1)); //NUM DE CHAVES 2*ORDEM + 1 (O +1 É ESPAÇO PRO VALOR OVERFLOW)


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

        if(n->filhos[0] != NULL)
        {
            n->folha = 0;
            return 0;
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


int split(No *no, No *novoIrmao)
{

    int *meio = separa(no->chaves,no->qtdChaves);
    int j=0;
    int k=0; //INDICE DOS FILHOS DO NOVO IRMAO
    for(int i = (meio - no->chaves)+1 ; i < no->qtdChaves; i++ )
    {
        adicionaChave(novoIrmao,no->chaves[i]); //PASSA AS CHAVES PRO IRMAO
        no->chaves[i] = VAZIO; //DECLARA O VALOR COMO VAZIO

        if(no->filhos[i]!=NULL) //PASSA OS FILHOS PRO IRMAO SE TIVER FILHOS
        {
            novoIrmao->filhos[k] = no->filhos[i];
            k++;
            no->filhos[i] = NULL;
        }
        j++;
    }

    if(no->pai != NULL)
    {
        novoIrmao->pai = no->pai;
    }

    no->qtdChaves-=j;


int valorMeio = *meio;
*meio = VAZIO;
no->qtdChaves--;
return valorMeio; //RETORNA O VALOR DO MEIO QUE IRA SUBIR PARA O PAI
}

void adicionaFilho(No *pai, No *filho)
{
    int i=0;
    if( pai == NULL)
    {
        printf("\nerro em adicionaFIlho, PAI NULL");

    }
    else if(filho == NULL)
    {
        printf("\n erro em adicionaFilho, FILHO NULL");
    }
    while(pai->filhos[i] != NULL)
    {
        i++;
    }
    pai->filhos[i] = filho;
    filho->pai = pai;
}

int inserir(No **raiz, int chave)
{

    if(!(*raiz))
    {
        *raiz = criaNo();
        adicionaChave(&(*raiz), chave);
     return 1;
    }
        No *noBusca = busca(&(*raiz),chave);

        if(noBusca == NULL)
        {
            printf("\nerro ao buscar no");
        }
        else
        {

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
            else //NAO TEM ESPACO, PRECISA FAZER SPLIT
            {

                adicionaChave(noBusca,chave); //CHAVE ADICIONADA POIS TEM ESPAÇO PARA OVERFLOW
                if(noBusca->pai == NULL) //PAI DO NO BUSCADO É NULO PORTANTO É RAIZ
                {
                    No *novaRaiz = criaNo();
                    No *irmao = criaNo();

                    //FUNCAO SPLIT PAASSANDO (noBusca,NoIrmao,PAI OU NULL):
                   int meio = split(noBusca,irmao);
                    adicionaChave(novaRaiz,meio);

                    adicionaFilho(novaRaiz,noBusca);
                    adicionaFilho(novaRaiz,irmao);

                }



            }



        }



return 0;
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


    No *novo = malloc(sizeof(No));
    No *irmaoT = malloc(sizeof(No));
    int qtdFilhos = NUM_FILHOS;



        novo->qtdChaves = 0;
        novo->pai = NULL;

        irmaoT->qtdChaves = 0;
        irmaoT->pai = NULL;

      novo->chaves = malloc(sizeof(int)*(NUM_CHAVES+1)); //NUM DE CHAVES 2*ORDEM + 1 (O +1 É ESPAÇO PRO VALOR OVERFLOW)
         irmaoT->chaves = malloc(sizeof(int)*(NUM_CHAVES+1));

        novo->filhos = malloc(sizeof(No*)*qtdFilhos);
        for(int i = 0; i < qtdFilhos ; i ++)
        {
            *(novo->filhos + i)  = NULL;
        }
         irmaoT->filhos = malloc(sizeof(No*)*qtdFilhos);
        for(int i = 0; i < qtdFilhos ; i ++)
        {
            *(irmaoT->filhos + i)  = NULL;
        }


    novo->filhos[0] =malloc(sizeof(No));
    novo->filhos[1] =malloc(sizeof(No));
    novo->filhos[2] =malloc(sizeof(No));

    adicionaChave(novo,1);
    adicionaChave(novo,23);
    adicionaChave(novo,4);


    split(novo,irmaoT);

    return 0;
}
