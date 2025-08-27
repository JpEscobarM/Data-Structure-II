#include <stdio.h>
#include <stdlib.h>
#define ORDEM 1
#define NUM_CHAVES 2*ORDEM
#define NUM_FILHOS (2*ORDEM)+1
#define VAZIO -99999

typedef struct no
{

    int *chaves;
    int qtdChaves;
    int folha;

    struct no **filhos;

    int grau;

} No;

typedef struct
{
    No *raiz;
    int grau;

} Arvore;


// ---PROTOTIPOS---
void removerEmInterno(No *src,int index);
void removerChaveNo(Arvore *arv, No *src, int chave);




void doxyblock()
{

}

//FUN��ES DE INSER��OI INICIAM AQUI//

/**
 * @brief aloca dinamicamente um No e inicializa variaveis.
 * @param INT 1 = folha | 0 = n�o folha.
 * @return ponteiro para No.
 */
No *criarNo(int folha)
{
    No *novo = malloc(sizeof(No));
    if(novo == NULL)
    {
        perror("<noCriar>");
        exit(EXIT_FAILURE);
    }

    novo->grau = NUM_FILHOS;
    novo->qtdChaves =0;
    novo->folha = folha;
    novo->chaves = malloc(sizeof(int)*NUM_CHAVES);
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
    return novo;
}

/**
 * @brief cria uma nova �rvore.
 * @param nenhum.
 * @return ponteiro para nova Arvore.
 */
Arvore *criarArvore()
{
    Arvore *novaArvore = malloc(sizeof(Arvore));

    if(novaArvore == NULL)
    {
        perror("<criarArvore>");
        exit(EXIT_FAILURE);

    }
    novaArvore->grau = NUM_FILHOS;
    novaArvore->raiz = criarNo(1);
    return novaArvore;
}


/**
 * @brief Libera a mem�ria de um n� e seus filhos em uma �rvore.
 * @param no Ponteiro para o n� a ser apagado.
 * @return Nenhum valor retornado.
 *
 * Esta fun��o libera recursivamente a mem�ria alocada para um n� e seus filhos.
 *apaga todos em cascata.
 * @note Certifique-se de que o n� n�o seja usado ap�s a chamada desta fun��o.
 */
void apagarNo(No *no)
{

    if(no == NULL)
    {
        return;
    }
    if(!no->folha)
    {
        for(int i = 0 ; i < no->qtdChaves; i ++)
        {
            apagarNo(no->filhos[i]);
        }
    }
    free(no->chaves);
    free(no->filhos);
    free(no);
}

/**
* @brief apaga a raiz da arvore e seus N�'s em cascata.
* @param ponteiro para tipo Arvore que ser� apagada.
* @return nenhum.
*/
void apagarArvore(Arvore *arv)
{
    if(arv == NULL)
    {
        return;
    }
    apagarNo(arv->raiz);
    free(arv);
}

/**
* @brief fun��o recursiva que procura uma chave entre os n�'s da arvore.
* @param No src = N� a ser procurado | int chave = chave procurada | int pos_out = indice da chave
* @return ponteiro para N� onde achou a chave OU NULL se n�o achou.
*/
No *buscarNo(No *src, int chave, int *pos_out)
{
    int i = 0;

    while(i < src->qtdChaves && chave > src->chaves[i])
    {
        i++;
    }
    //1 - SE A CHAVE ESTA NO INDICE i DO N� ATUAL ENT�O RETORNA O MESMO.
    if(i < src->qtdChaves && chave == src->chaves[i])
    {
        if(pos_out) *pos_out = i; //INDICA A POSICAO CORRETA DA CHAVE NO N�.
        return src;
    }

    //2- SE A O N� ATUAL � FOLHA E N�O ACHOU A CHAVE RETORNA NULL.
    if(src->folha) return NULL;

    //CONTINUA PROCURANDO
    return buscarNo(src->filhos[i], chave, pos_out);
}

/**
* @brief buscar chave na arvore.
* @param No src = N� a ser procurado | int chave = chave procurada | int pos_out = indice da chave
* @return ponteiro para N� onde achou a chave OU NULL se n�o achou.
*/
No *buscarEmArvore(Arvore *arv, int chave, int *pos_out)
{

    return (arv && arv->raiz) ? buscarNo(arv->raiz,chave,pos_out) : NULL;
}

/**
* @brief realiza um split quando o n� chega a capacidade maxima de chaves.
* @param No *pai = pai do n� orig | int j = posicao de *orig em *pai | No *orig = n� que ser�
*realizado o split
* @return nenhum.
*/
void split_filho(No *pai, int j, No *orig)
{

    if (orig->qtdChaves != NUM_CHAVES)
    {
        // Retorna pois s� vai executar o split quando um n� estiver cheio
        return;
    }

    No *novo = criarNo(orig->folha);
    // qtdChaves inicia em 0 (de criarNo), e s� setamos ap�s c�pia

    // Copia ORDEM maiores chaves
    for (int i = 0; i < ORDEM; i++)
    {
        novo->chaves[i] = orig->chaves[i + ORDEM];
    }
    novo->qtdChaves = ORDEM;

    // Copia ORDEM+1 filhos (se n�o folha)
    if (!orig->folha)
    {
        for (int i = 0; i <= ORDEM; i++)
        {
            novo->filhos[i] = orig->filhos[i + ORDEM];
            orig->filhos[i + ORDEM] = NULL;
        }
    }

    orig->qtdChaves = ORDEM;  // orig fica com ORDEM chaves

    // Ajusta filhos no pai
    for (int i = pai->qtdChaves; i >= j + 1; i--)
    {
        pai->filhos[i + 1] = pai->filhos[i];
    }
    pai->filhos[j + 1] = novo;

    // Ajusta chaves no pai
    for (int i = pai->qtdChaves - 1; i >= j; i--)
    {
        pai->chaves[i + 1] = pai->chaves[i];
    }

    // Insere mediana (posi��o ORDEM)
    pai->chaves[j] = orig->chaves[ORDEM];
    pai->qtdChaves += 1;
}


/**
 * @brief Insere uma chave em um n� n�o cheio da �rvore B
 *
 * Se for folha, insere direto na posi��o correta.
 * Se n�o for folha, desce para o filho adequado.
 * Faz split do filho se ele estiver cheio.
 * Evita inserir chaves duplicadas.
 *
 * @param src   N� atual da �rvore B
 * @param chave Valor a ser inserido
 */

void inserirNaoCheio(No *src, int chave)
{
    int i = src->qtdChaves -1; //INDICE DA ULTIMA CHAVE INSERIDA

    if(src->folha)
    {
        while( i >= 0 && src->chaves[i] > chave)
        {
            //CASO A CHAVE TENHA UM VALOR MENOR DO QUE O VALOR ATUAL
            //PASSA O VALOR PRO LADO
            src->chaves[i + 1] = src->chaves[i];
            i--;
        }
        if( i >= 0 && src->chaves[i] == chave) //EVITAR DUPLICATAS
        {
            return;
        }
        src->chaves[i+1]= chave;
        src->qtdChaves++;
    }
    else
    {
        while(i>= 0 && src->chaves[i]> chave)
        {
            i--;
        }
        i++;
        if((i< src->qtdChaves && src->chaves[i] == chave) || (i-1 >= 0 && src->chaves[i-1] == chave))
        {
            return;
        }
        if(src->filhos[i]->qtdChaves == NUM_CHAVES)
        {
            split_filho(src,i,src->filhos[i]);
            if(chave > src->chaves[i])
            {
                i++;
            }
            else if(chave == src->chaves[i])
            {
                return;
            }
        }
        inserirNaoCheio(src->filhos[i],chave);
    }
}


/**
* @brief insere uma chave na arvore.
* @param *arv = ponteiro para arvore | chave = valor que sera inserido
* @return nenhum.
*/
void inserirNaArvore(Arvore *arv, int chave)
{
    No *raiz = arv->raiz; // APONTA PRO NO ATUAL NA RAIZ
    if(raiz->qtdChaves == NUM_CHAVES)
    {
        No *novaRaiz = criarNo(0);

        arv->raiz = novaRaiz;

        novaRaiz->filhos[0] =raiz; //RAIZ VIRA FILHO DA NOVA RAIZ

        split_filho(novaRaiz,0,raiz);

        int i = (chave > novaRaiz->chaves[0]) ? 1 : 0;

        if (chave == novaRaiz->chaves[0]) return;
        inserirNaoCheio(novaRaiz->filhos[i], chave);
    }
    else
    {
        inserirNaoCheio(raiz,chave);
    }

}
//FUN��ES DE INSER��O TERMINAM AQUI//


//INICIO FUN��ES DE REMO��O

/**
* @brief acha a posicao correta da chave no vetor de chaves do n�.
* @param *src = ponteiro para n� | chave = chave a ser comparada.
* @return int index.
*/
int encontrarPosicao(No *src, int chave)
{
    int index = 0;
    while(index < src->qtdChaves && src->chaves[index] < chave)
    {
        index++;
    }

    return index;
}

/**
* @brief desloca os No's a direita do index, para a esquerda, cobrindo a posicao apagada.
* @param *src = ponteiro para n� | index= indica a posicao do valor a ser apagado.
*
* Importante ressaltar que deve ser usado somente em folhas.
*
* @return nenhum.
*/
void removerEmFolha(No *src, int index)
{
    for(int i = index +1 ; i< src->qtdChaves; ++i)
    {
        src->chaves[i-1] = src->chaves[i];

    }
    src->qtdChaves--;
}


/**
* @brief retorna a maior chave da subarvore a esquerda.
* @param *src = ponteiro para n� | index= indica a posicao do valor a ser apagado.
*
* utilizado quando vamos remover uma chave de um n� nao folha.
*
* @return int.
*/
int predecessor(No *src, int index)
{
    No *aux = src->filhos[index];
    while(!aux->folha) aux = aux->filhos[aux->qtdChaves];
    return aux->chaves[aux->qtdChaves-1];
}

/**
* @brief retorna a menor chave da subarvore a direita do index indicado.
* @param *src = ponteiro para n� | index= indica a posicao do valor a ser apagado.
*
* utilizado quando vamos remover uma chave de um n� nao folha.
*
* @return int.
*/
int sucessor(No *src, int index)
{
    No *aux = src->filhos[index];
    while(!aux->folha) aux = aux->filhos[0];
    return aux->chaves[0];
}

/**
* @brief junta dois filhos, Esquerda e Direita do valor indicado pelo index.
* @param *src = ponteiro para n� | index= indica a posicao do valor no vetor de chaves.
*
* ATEN��O: quando ordem =1 a condi��o de jun��o nunca � verdade pois condi��o: P + Q nunca � menor que 2 X ORDEM
*   sendo P e Q a quantidade de chaves dos dois filhos a serem unidos.
* @return nenuhm.
*/
void fundirFilhos(No *src, int index)
{
    if(src == NULL)
    {
        printf("\nERRO:<fundirFilhos>n� src NULL<fundirFIlhos>");
        return;
    }

    if(index < 0 || index >= src->qtdChaves)
    {
        printf("\nERRO:<fundirFilhos>index > qtdChaves de src<fundirFIlhos>");
        return;
    }


    if(ORDEM < 2)
    {
        printf("\nERRO:<fundirFilhos>ordem d < 2 ou seja: cada filho tem \>=1 chave; P+Q < 2d nunca � verdade <fundirFIlhos>");
        return;
    }


    No *esq = src->filhos[index]; //MENORES QUE A CHAVE NA POSICAO [INDEX]
    No *dir = src->filhos[index+1];

    if(esq == NULL || dir == NULL)
    {
        printf("\nERRO:<fundirFilhos>ponteiro a esquerda ou a direita de index == NULL<fundirFIlhos>");
        return;
    }


    int p_esq = esq->qtdChaves;
    int q_dir = dir->qtdChaves;

    if(p_esq + q_dir >= NUM_CHAVES)
    {
        printf("\nERRO:<fundirFilhos>P + Q > 2d <fundirFIlhos>");
        return;
    }


    esq->chaves[p_esq++] = src->chaves[index];



    for (int j = 0; j < q_dir; ++j)
    {
        esq->chaves[p_esq + j] = dir->chaves[j];
    }



    if (esq->folha == 0)
    {
        for (int j = 0; j <= q_dir; ++j)
        {
            esq->filhos[p_esq + j] = dir->filhos[j];
        }
    }



    esq->qtdChaves = p_esq + q_dir; //SOMA AS CHAVES QUE FORAM PASSADAS DA DIREITA



    for (int j = index; j < src->qtdChaves - 1; ++j)
    {
        src->chaves[j] = src->chaves[j + 1];
    }



    for (int j = index + 1; j < src->qtdChaves; ++j)
    {
        src->filhos[j] = src->filhos[j + 1];
    }


    src->filhos[src->qtdChaves] = NULL;
    src->qtdChaves--;


    free(dir->chaves);
    free(dir->filhos);
    free(dir);
}





/**
 *   @brief passa a chave do filho a esquerda para o pai, e do pai para o filho a direita.
 *   @param *src = ponteiro para n� | index= indica a posicao do valor no vetor de chaves.
  *  @return nenhum.
*/
void emprestarDoAnterior(No *src, int index)
{
    No *irmao = src->filhos[index-1]; // ESQ = P
    No *filho = src->filhos[index]; // DIR = Q



    if (index <= 0 || index > src->qtdChaves)
    {
        printf("\nERRO:<emprestarAnterior>[index] == NULL <emprestarAnterior>"); return;

    }

    if (irmao == NULL)
    {
        printf("\nERRO:<emprestarAnterior>no filho com [index-1] == NULL <emprestarAnterior>"); return;
    }


    if (filho == NULL)
    {
       printf("\nERRO:<emprestarAnterior>no filho com [index] == NULL <emprestarAnterior>"); return;
    }


    for (int i = filho->qtdChaves - 1; i >= 0; i--)
    {
        filho->chaves[i + 1] = filho->chaves[i];
    }


    if (filho->folha == 0) // TEM FILHOS
    {
        for (int i = filho->qtdChaves; i >= 0; i--)
        {
            filho->filhos[i + 1] = filho->filhos[i];
        }
    }


    filho->chaves[0] = src->chaves[index - 1];
    if (filho->folha == 0)
    {
        filho->filhos[0] = irmao->filhos[irmao->qtdChaves];
        irmao->filhos[irmao->qtdChaves] = NULL;
    }


    src->chaves[index - 1] = irmao->chaves[irmao->qtdChaves - 1];


    filho->qtdChaves += 1;
    irmao->qtdChaves -= 1;
}


/**
 *   @brief passa a chave do filho a direita para o pai, e do pai para o filho a esquerda.
 *   @param *src = ponteiro para n� | index= indica a posicao do valor no vetor de chaves.
  *  @return nenhum.
*/
void emprestaDoProximo(No *src, int index)
{

      No *irmao = src->filhos[index+1]; // DIR
    No *filho = src->filhos[index]; // ESQ



    if (index < 0 || index >= src->qtdChaves)
    {
        printf("\nERRO:<emprestaDoProximo>[index] == NULL <emprestaDoProximo>"); return;
    }
    if (irmao == NULL)
    {
        printf("\nERRO:<emprestaDoProximo>no filho com [index+1] == NULL <emprestaDoProximo>"); return;
    }


    if (filho == NULL)
    {
       printf("\nERRO:<emprestaDoProximo>no filho com [index] == NULL <emprestaDoProximo>"); return;
    }

    if (irmao->qtdChaves <= ORDEM) { printf("irm�o a direita n�o pode emprestar"); return; }
    if (filho->qtdChaves >= NUM_CHAVES) {printf("filho atual cheio"); return; }




    filho->chaves[filho->qtdChaves] = src->chaves[index];

    if (filho->folha == 0)
    {
        filho->filhos[filho->qtdChaves+1] = irmao->filhos[0];

    }

    src->chaves[index] = irmao->chaves[0];

    for(int i = 1 ;  i<irmao->qtdChaves ; i++) //SHIFT PARA ESQUERDA
    {
        irmao->chaves[i-1] = irmao->chaves[i];
    }

    if(irmao->folha == 0 )
    {
        for(int i = 1 ; i<= irmao->qtdChaves ; i++) //SHIFT PARA ESQUERDA
        {
            irmao->filhos[i-1] = irmao->filhos[i];
        }
    }

    filho->qtdChaves++;
    irmao->qtdChaves--;

}

void removerEmInterno(No *src, int index)
{
    if(src == NULL)
    {
          printf("\nERRO:<removerEmInterno>N� passado por parametro == NULL <removerEmInterno>"); return;
    }


    if(index < 0 || index>= src->qtdChaves)
    {
          printf("\nERRO:<removerEmInterno> index < 0 ou index>= src->qtdChaves <removerEmInterno>"); return;
    }


    int chave = src->chaves[index];

    if(src->filhos[index]->qtdChaves >= ORDEM+1)
    {
        int pred = predecessor(src,index);
        src->chaves[index] = pred;
        removerChaveNo(NULL, src->filhos[index],pred);
    }
    else if
    {

    }

}


void removerChaveNo(Arvore *arv, No *src, int chave)
{
    int index = encontrarPosicao(src,chave);

    if(index < src->qtdChaves && src->chaves[index] == chave)
    {
        if(src->folha)
        {
            removerEmFolha(src,index);
        }
        else
        {
            removerEmInterno()
        }
    }
}


int main()
{


    return 0;
}
