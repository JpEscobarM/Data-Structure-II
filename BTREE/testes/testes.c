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

    no->qtdChaves-=j;


int valorMeio = *meio;
*meio = VAZIO;
no->qtdChaves--;
return valorMeio; //RETORNA O VALOR DO MEIO QUE IRA SUBIR PARA O PAI
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