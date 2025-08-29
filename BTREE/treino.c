#include <stdio.h>
#include <stdlib.h>
#define ORDEM 2
#define NUM_CHAVES 2*ORDEM
#define NUM_FILHOS (2*ORDEM)+1
#define VAZIO -99999

typedef struct no
{

    int *chaves;
    int qtdChaves;
    int folha;

    struct no **filhos;



} No;

typedef struct
{
    No *raiz;


} Arvore;


// ---PROTOTIPOS---
void removerEmInterno(No *src,int index);
void removerChaveNo(Arvore *arv, No *src, int chave);




void doxyblock()
{

}

//FUNÇÕES DE INSERÇÃOI INICIAM AQUI//

/**
 * @brief aloca dinamicamente um No e inicializa variaveis.
 * @param INT 1 = folha | 0 = não folha.
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
 * @brief cria uma nova árvore.
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

    novaArvore->raiz = criarNo(1);
    return novaArvore;
}


/**
 * @brief Libera a memória de um nó e seus filhos em uma árvore.
 * @param no Ponteiro para o nó a ser apagado.
 * @return Nenhum valor retornado.
 *
 * Esta função libera recursivamente a memória alocada para um nó e seus filhos.
 *apaga todos em cascata.
 * @note Certifique-se de que o nó não seja usado após a chamada desta função.
 */
void apagarNo(No *no)
{

    if(no == NULL)
    {
        return;
    }
    if(no->folha == 0)
    {
        for(int i = 0 ; i <= no->qtdChaves; i ++)
        {
            apagarNo(no->filhos[i]);
        }
    }
    free(no->chaves);
    free(no->filhos);
    free(no);
}

/**
* @brief apaga a raiz da arvore e seus Nó's em cascata.
* @param ponteiro para tipo Arvore que será apagada.
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
* @brief função recursiva que procura uma chave entre os nó's da arvore.
* @param No src = Nó a ser procurado | int chave = chave procurada | int pos_out = indice da chave
* @return ponteiro para Nó onde achou a chave OU NULL se não achou.
*/
No *buscarNo(No *src, int chave, int *pos_out)
{
    int i = 0;

    while(i < src->qtdChaves && chave > src->chaves[i])
    {
        i++;
    }
    //1 - SE A CHAVE ESTA NO INDICE i DO NÓ ATUAL ENTÃO RETORNA O MESMO.
    if(i < src->qtdChaves && chave == src->chaves[i])
    {
        if(pos_out) *pos_out = i; //INDICA A POSICAO CORRETA DA CHAVE NO NÓ.
        return src;
    }

    //2- SE A O NÓ ATUAL É FOLHA E NÃO ACHOU A CHAVE RETORNA NULL.
    if(src->folha) return NULL;

    //CONTINUA PROCURANDO
    return buscarNo(src->filhos[i], chave, pos_out);
}

/**
* @brief buscar chave na arvore.
* @param No src = Nó a ser procurado | int chave = chave procurada | int pos_out = indice da chave
* @return ponteiro para Nó onde achou a chave OU NULL se não achou.
*/
No *buscarEmArvore(Arvore *arv, int chave, int *pos_out)
{

    return (arv && arv->raiz) ? buscarNo(arv->raiz,chave,pos_out) : NULL;
}

/**
* @brief realiza um split quando o nó chega a capacidade maxima de chaves.
* @param No *pai = pai do nó orig | int j = posicao de *orig em *pai | No *orig = nó que será
*realizado o split
* @return nenhum.
*/
void split_filho(No *pai, int j, No *orig)
{

    if (orig->qtdChaves != NUM_CHAVES)
    {
        // Retorna pois só vai executar o split quando um nó estiver cheio
        return;
    }

    No *novo = criarNo(orig->folha);
    // qtdChaves inicia em 0 (de criarNo), e só setamos após cópia

    // Copia ORDEM maiores chaves
    for (int i = 0; i < ORDEM; i++)
    {
        novo->chaves[i] = orig->chaves[i + ORDEM];
    }
    novo->qtdChaves = ORDEM;

    // Copia ORDEM+1 filhos (se não folha)
    if (!orig->folha)
    {
        for (int i = 0; i <= ORDEM; i++)
        {
            novo->filhos[i] = orig->filhos[i + ORDEM+1];


            orig->filhos[i + ORDEM+1] = NULL;

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

    // Insere mediana (posição ORDEM)
    pai->chaves[j] = orig->chaves[ORDEM];
    pai->qtdChaves += 1;
}


/**
 * @brief Insere uma chave em um nó não cheio da Árvore B
 *
 * Se for folha, insere direto na posição correta.
 * Se não for folha, desce para o filho adequado.
 * Faz split do filho se ele estiver cheio.
 * Evita inserir chaves duplicadas.
 *
 * @param src   Nó atual da Árvore B
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
//FUNÇÕES DE INSERÇÃO TERMINAM AQUI//


//INICIO FUNÇÕES DE REMOÇÃO

/**
* @brief acha a posicao correta da chave no vetor de chaves do nó.
* @param *src = ponteiro para nó | chave = chave a ser comparada.
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
* @param *src = ponteiro para nó | index= indica a posicao do valor a ser apagado.
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
* @param *src = ponteiro para nó | index= indica a posicao do valor a ser apagado.
*
* utilizado quando vamos remover uma chave de um nó nao folha.
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
* @param *src = ponteiro para nó | index= indica a posicao do valor a ser apagado.
*
* utilizado quando vamos remover uma chave de um nó nao folha.
*
* @return int.
*/
int sucessor(No *src, int index)
{
    No *aux = src->filhos[index+1];
    while(!aux->folha) aux = aux->filhos[0];
    return aux->chaves[0];
}

/**
* @brief junta dois filhos, Esquerda e Direita do valor indicado pelo index.
* @param *src = ponteiro para nó | index= indica a posicao do valor no vetor de chaves.
*
* ATENÇÃO: quando ordem =1 a condição de junção nunca é verdade pois condição: P + Q nunca é menor que 2 X ORDEM
*   sendo P e Q a quantidade de chaves dos dois filhos a serem unidos.
* @return nenuhm.
*/
void fundirFilhos(No *src, int index)
{
    if(src == NULL)
    {
        printf("\nERRO:<fundirFilhos>nó src NULL<fundirFIlhos>");
        return;
    }

    if(index < 0 || index >= src->qtdChaves)
    {
        printf("\nERRO:<fundirFilhos>index > qtdChaves de src<fundirFIlhos>");
        return;
    }


    if(ORDEM < 2)
    {
        printf("\nERRO:<fundirFilhos>ordem d < 2 ou seja: cada filho tem \>=1 chave; P+Q < 2d nunca é verdade <fundirFIlhos>");
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
 *   @param *src = ponteiro para nó | index= indica a posicao do valor no vetor de chaves.
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
 *   @param *src = ponteiro para nó | index= indica a posicao do valor no vetor de chaves.
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

    if (irmao->qtdChaves <= ORDEM) { printf("irmão a direita não pode emprestar"); return; }
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

void completarFilho(No *src, int index)
{
    if(index!= 0 && src->filhos[index-1]->qtdChaves >= ORDEM+1)
    {
        emprestarDoAnterior(src,index);
    }
    else if(index!= src->qtdChaves && src->filhos[index+1]->qtdChaves >= ORDEM+1)
    {
        emprestaDoProximo(src,index);
    }
    else
    {
        if(index != src->qtdChaves)
        {
            fundirFilhos(src,index);
        }
        else
        {
            fundirFilhos(src,index-1);
        }
    }

}


void removerEmInterno(No *src, int index)
{
    if(src == NULL)
    {
          printf("\nERRO:<removerEmInterno>Nó passado por parametro == NULL <removerEmInterno>"); return;
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
    else if(src->filhos[index+1]->qtdChaves >= ORDEM+1 )
    {

    int sucess = sucessor(src,index);

    src->chaves[index] = sucess;
    removerChaveNo(NULL,src->filhos[index+1],sucess);

    }
    else
    {
        fundirFilhos(src,index);
        removerChaveNo(NULL,src->filhos[index],chave);
    }

}


void removerChaveNo(Arvore *arv, No *src, int chave)
{
    int index = encontrarPosicao(src,chave);

    if(index < src->qtdChaves && src->chaves[index] == chave) //SE A CHAVE JÁ ESTÁ NO NÓ
    {
        if(src->folha)
        {
            removerEmFolha(src,index);
        }
        else
        {
            removerEmInterno(src,index);
        }
    }
    else
    {
        if(src->folha)
        {
            return; //CHAVE NAO ENCONTRADA
        }

        int ultimo = (index == src->qtdChaves);

        if(src->filhos[index]->qtdChaves == ORDEM)
        {
            completarFilho(src,index);
        }

        if(ultimo && index > src->qtdChaves)
        {
            removerChaveNo(arv,src->filhos[index-1],chave);
        }
        else
        {
            removerChaveNo(arv,src->filhos[index],chave);
        }
    }
}


void removerArvore(Arvore *arv, int chave)
{
    if(arv == NULL || arv->raiz == NULL)
    {
        return;
    }

    No *src =  arv->raiz;

    removerChaveNo(arv,src,chave);

    if(src->qtdChaves == 0)
    {
        No *antiga = src;

        if(src->folha)
        {
         arv->raiz = NULL;

         apagarNo(antiga);
        }
        else
        {
            arv->raiz = src->filhos[0];
            antiga->filhos[0] == NULL;
            apagarNo(antiga);
        }

    }

}



static void _printKeys(const No *n) {
    printf("[");
    for (int i = 0; i < n->qtdChaves; i++) {
        printf("%d", n->chaves[i]);
        if (i < n->qtdChaves - 1) printf(" ");
    }
    printf("]");
}

static void _imprimirEmOrdemNo(No *n) {
    if (!n) return;

    for (int i = 0; i < n->qtdChaves; i++) {
        if (!n->folha && n->filhos[i]) {
            _imprimirEmOrdemNo(n->filhos[i]);
        }
        printf("%d ", n->chaves[i]);
    }
    if (!n->folha && n->filhos[n->qtdChaves]) {
        _imprimirEmOrdemNo(n->filhos[n->qtdChaves]);
    }
}

static void _indent(int nivel) {
    for (int i = 0; i < nivel; i++) printf("  ");
}

static void _imprimirEstruturadoNo(No *n, int nivel) {
    if (!n) return;

    _indent(nivel);
    printf("nivel %d %s ", nivel, n->folha ? "(folha)" : "(interno)");
    _printKeys(n);
    printf("\n");

    if (!n->folha && n->filhos) {
        /* Em uma B-Tree com m chaves, há até m+1 filhos relevantes */
        for (int i = 0; i <= n->qtdChaves; i++) {
            if (n->filhos[i]) {
                _imprimirEstruturadoNo(n->filhos[i], nivel + 1);
            }
        }
    }
}

/* ---------- APIs públicas ---------- */

/* Em ordem (crescente) */
void imprimirEmOrdem(Arvore *arv) {
    if (!arv || !arv->raiz || arv->raiz->qtdChaves == 0) {
        printf("(arvore vazia)\n");
        return;
    }
    _imprimirEmOrdemNo(arv->raiz);
    printf("\n");
}

/* Por nivel (BFS) */
void imprimirPorNivel(Arvore *arv) {
    if (!arv || !arv->raiz || arv->raiz->qtdChaves == 0) {
        printf("(arvore vazia)\n");
        return;
    }

    /* Fila simples dinâmica de pares (No*, nivel) */
    size_t cap = 64, head = 0, tail = 0;
    No **q = (No**)malloc(cap * sizeof(No*));
    int *lvl = (int*)malloc(cap * sizeof(int));
    if (!q || !lvl) { perror("<imprimirPorNivel:malloc>"); free(q); free(lvl); return; }

    #define ENQUEUE(node, level) do { \
        if (tail >= cap) { \
            cap *= 2; \
            q = (No**)realloc(q, cap * sizeof(No*)); \
            lvl = (int*)realloc(lvl, cap * sizeof(int)); \
            if (!q || !lvl) { perror("<imprimirPorNivel:realloc>"); free(q); free(lvl); return; } \
        } \
        q[tail] = (node); \
        lvl[tail] = (level); \
        tail++; \
    } while(0)

    ENQUEUE(arv->raiz, 0);

    int curr = -1;
    while (head < tail) {
        No *n = q[head];
        int l = lvl[head];
        head++;

        if (l != curr) {
            curr = l;
            printf("\nNivel %d: ", curr);
        } else {
            printf(" | ");
        }

        _printKeys(n);

        if (!n->folha && n->filhos) {
            /* Enfileira até qtdChaves+1 filhos não-nulos */
            for (int i = 0; i <= n->qtdChaves; i++) {
                if (n->filhos[i]) ENQUEUE(n->filhos[i], l + 1);
            }
        }
    }
    printf("\n");

    free(q);
    free(lvl);
    #undef ENQUEUE
}

/* Estruturado (hierárquico) */
void imprimirEstruturado(Arvore *arv) {
    if (!arv || !arv->raiz || arv->raiz->qtdChaves == 0) {
        printf("(arvore vazia)\n");
        return;
    }
    _imprimirEstruturadoNo(arv->raiz, 0);
}

int main()
{

    int op = 1, chave;
    Arvore *arvore = criarArvore();

    while(op != 0)
    {
        printf("\n================= ARVORE B =================\n");
        printf("ORDEM: %d | MAX CHAVES: %d |MAX FILHOS: %d |\n",ORDEM,NUM_CHAVES,NUM_FILHOS);
        printf("===========================================\n");
        printf("[1]-INSERIR\n");
        printf("[2] - BUSCAR\n");
        printf("[3] - REMOVER\n");
        printf("[4] - IMPRIMIR EM ORDEM\n");
        printf("[5] - IMPRIMIR POR NIVEL (BFS)\n");
        printf("[6] - IMPRIMIR ESTRUTURADO (HIERARQUICO)\n");
        printf("DIGITE UMA OPCAO:_");
        scanf("%d",&op);

        switch(op)
        {
        case 1:

            printf("\ndigite a chave que sera inserida: ");
            scanf("%d",&chave);
            inserirNaArvore(arvore,chave);

            break;
        case 2:
        {
            printf("\nDigite a chave que deseja buscar: ");
            scanf("%d", &chave);
            int pos = -1;
            No *n = buscarEmArvore(arvore, chave, &pos);
            if (n != NULL)
            {
                printf("Chave %d ENCONTRADA no no, posicao %d.\n", chave, pos);
            }
            else
            {
                printf("Chave %d NAO encontrada.\n", chave);
            }
            break;
        }

        case 3:
        {
            printf("\nDigite a chave que deseja remover: ");
            scanf("%d", &chave);
            removerArvore(arvore, chave);
            printf("Remocao solicitada para a chave %d.\n", chave);
            break;
        }
        case 4:
            imprimirEmOrdem(arvore);
            break;

        case 5:
            imprimirPorNivel(arvore);
            break;

        case 6:
            imprimirEstruturado(arvore);
            break;

        case 0:
            printf("\nSaindo...\n");
            break;

        default:
            printf("\nOpcao invalida.\n");
            break;
        }

    }


    return 0;
}
