#include <stdio.h>
#include <stdlib.h>
#define ORDEM 2
#define BAD -99999
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

/*==========================CRIAR N�===========*/

No *criaNo(No *pai, int folha, int debug){
    No *novo = malloc(sizeof(No));
    if(debug) printf("\nENDERECO DO NO CRIADO: %p", novo);

    if(!novo)
    {
        printf("\n|criaNo| erro: nao foi possivel alocar memoria para novo n�");
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

        if (debug) printf(">> Chave %d n�o encontrada na folha.\n", chave);

        return atual;//RETORNA A FOLHA IGUAL PARA INSER��O POIS MESMO SE A CHAVE NAO ESTA PRESENTE
        //A CHAVE DEVERIA ESTAR NAQUELA FOLHA
    }
}

No *buscaIndice(No *atual, int chave, int debug) {
    //fun��o usada para verificar se uma chave
    if (!atual) return NULL;

    if (debug) {
        printf("\n>> Visitando no:");
        imprimeNo(atual);
    }

    int i = 0;

    if (!atual->folha) {

        while (i < atual->qtdChaves && chave >= atual->chaves[i]) {
            if (atual->chaves[i] == chave) {
                if (debug) printf(">> Chave %d encontrada na folha.\n", chave);
                return atual;
            }
            i++;
        }
        if (atual->filhos[i] != NULL)
        {
            return buscaIndice(atual->filhos[i], chave, debug);
        }
        else
            return NULL;
    } else {

        if (debug) printf(">> Chave %d n�o encontrada na folha.\n", chave);
    return NULL;
    }
}

/*===============================================================*/


/*=========================INSER��O=============================*/

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

void splitRaizFolha(Arvore *arv,No *raiz)
{
    No *novoIrmao = criaNo(NULL,raiz->folha,0);

    int divisor = raiz->chaves[ORDEM]; //PEGA A CHAVE  DO MEIO QUE IRA VIRAR INDICE

    int i= raiz->qtdChaves-1;

    while(i >= ORDEM) //i>= pois precisa pegar a chave divisora tamb�m, respeitando a regra das folhas e N�'s internos(apenas de indice)
    {
        addChaveEmFolha(novoIrmao,raiz->chaves[i]);
        raiz->qtdChaves--; //quando chegar na posicao "ORDEM" ja "apaga" do no antigo o divisor
        i--;
    }

    raiz->prox = novoIrmao; //encadeira os irmaos seguindo conceito de b+


    No *novaRaiz = criaNo(NULL,0,0);

    novaRaiz->chaves[novaRaiz->qtdChaves] = divisor; //sobe o divisor pra servir tamb�m de indice.
    novaRaiz->qtdChaves++;

    novaRaiz->filhos[0]= raiz;
    novaRaiz->filhos[1] = novoIrmao; // valor da posicao ORDEM servindo como indice e novo irmao guardando chaves >= ORDEM
    novaRaiz->pai = NULL;

    raiz->pai = novaRaiz; //atualiza o pai dos filhos
    novoIrmao->pai = novaRaiz; //atualiza o pai dos filhos


    arv->raiz = novaRaiz; //atualiza a nova raiz

}

void addChaveEmNoInterno(No *interno, int chave, No *filhoEsq, No *filhoDir)
{
    int i = interno->qtdChaves-1;
    while(i >= 0 && chave < interno->chaves[i])
    {
        interno->chaves[i+1] = interno->chaves[i];//insere a chave de forma ordenada

        if(!interno->folha) //garante que tem filhos / n�o � um n� folha
        {
            //OBSERVA��O PARA QUESTIONAMENTOS FUTUROS: novo->filhos = malloc(sizeof(No*)*(MAX_FILHOS+1)); //+1 para overflow de filho
            //a quantidade de filhos de uma ordem 2 por exemplo � 6 pois o calculo � (2*ordem)+1 como no define e ainda +1 para overflow
            //portanto nao corre risco de inser��o invalido

            interno->filhos[i+2] = interno->filhos[i+1]; //mantem os filhos ordenados

        }
        i--;
    }
    i++;
    interno->chaves[i] = chave;
    interno->qtdChaves++;
    if(!interno->folha)
    {
    interno->filhos[i] = filhoEsq;
    interno->filhos[i+1] = filhoDir;

    filhoEsq->pai = interno;
    filhoDir->pai = interno;
    }
}



void splitNoInterno(Arvore *arv,No *atual)
{
    int meio = ORDEM;
    int i;

    No *novoIrmao = criaNo(atual->pai, 0, 0);

    int chavePromovida = atual->chaves[meio];

    // Move as chaves e filhos para o novo irm�o
    for (i = meio + 1; i < atual->qtdChaves; i++)
    {
        novoIrmao->chaves[novoIrmao->qtdChaves++] = atual->chaves[i];
    }

    for (i = meio + 1; i <= atual->qtdChaves; i++)
    {
        novoIrmao->filhos[i - (meio + 1)] = atual->filhos[i];
        if (novoIrmao->filhos[i - (meio + 1)]) {
            No *filho = novoIrmao->filhos[i - (meio + 1)];
            filho->pai = novoIrmao;
        }
    }

    // Atualiza o n�mero de chaves do n� atual meio = ORDEM
    atual->qtdChaves = meio;

    // Se o n� atual for a raiz
    if (atual->pai == NULL)
    {
        No *novaRaiz = criaNo(NULL, 0, 0);
        arv->raiz = novaRaiz;

        // Insere a chave promovida e filhos na nova raiz
        novaRaiz->chaves[0] = chavePromovida;
        novaRaiz->filhos[0] = atual;
        novaRaiz->filhos[1] = novoIrmao;
        novaRaiz->qtdChaves = 1;

        atual->pai = novaRaiz;
        novoIrmao->pai = novaRaiz;
    }
    else
    {
        // Caso contr�rio, adiciona ao pai atual
        addChaveEmNoInterno(atual->pai, chavePromovida, atual, novoIrmao);

        // Se o pai tamb�m ficou cheio, split recursivo
        No *pai = atual->pai;
        if (pai->qtdChaves > MAX_CHAVES)
        {
            splitNoInterno(arv,pai);
        }
    }
}

void splitFolha(Arvore *arv, No *atual)
{

    No *novoIrmao = criaNo(atual->pai,atual->folha,0);


    int divisor = atual->chaves[ORDEM];//guarda a chave que sera usada como indice
    int i= atual->qtdChaves-1;
    while( i>= ORDEM) //TODAS AS CHAVES ATE A ORDEM
    {
        addChaveEmFolha(novoIrmao,atual->chaves[i]);
        atual->qtdChaves--;
        i--;
    }

    if(atual->prox != NULL)
    {

       novoIrmao->prox = atual->prox; //novo irmao a direita mantem o proximo que ja estava

    }
    atual->prox = novoIrmao; // atual aponta pro novo irmao

    addChaveEmNoInterno(atual->pai,divisor,atual,novoIrmao); //chave na posicao ORDEM sobe pro pai

    No *pai = atual->pai;

    if(pai->qtdChaves > MAX_CHAVES)//se o pai ficar cheio, desencadeia split em N�'s internos
    {
        splitNoInterno(arv,pai);
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
        //Caso II - ordem 2 -> max de chaves igual a 4 com + 1 para overflow ou seja se qtdChaves > MAX_CHAVES == 5 valores na folha, precisa de split
        //FAZER SPLIT
        //1- CASO RAIZ REALIZA SPLIT NORMAL DIVINDO EM 2 E CRIA NOVA RAIZ
        if(retorno->pai == NULL && retorno->folha) //RAIZ
        {
            splitRaizFolha(arv,retorno);
        }
        else //SPLIT COMUM QUE SEPARA A FOLHA
        {
            splitFolha(arv,retorno);
        }


    }

}

/*============================================================*/

/*=====================FUN��ES PARA DELE��O====================*/

int removerChaveFolha(No *atual, int chave)
{
    int valorIndefinido = BAD;
    int posicaoChave = valorIndefinido;

    if (!atual || atual->qtdChaves == 0) return -1;

    // Localiza a posi��o da chave
    for (int i = 0; i < atual->qtdChaves; i++)
    {
        if (atual->chaves[i] == chave)
        {
            posicaoChave = i;
            break;
        }
    }

    // Chave n�o encontrada
    if (posicaoChave == valorIndefinido) return -1;

    // Desloca as chaves � esquerda
    for (int i = posicaoChave; i < atual->qtdChaves - 1; i++)
    {
        atual->chaves[i] = atual->chaves[i + 1];
    }

    atual->qtdChaves--;

    return chave; // sucesso: retorna a chave removida
}

int procuraIndiceFilho(No *filho, No *pai)
{
    if (!filho || !pai)
    {
        printf("\n[ERRO]<procuraIndiceFilho> Filho ou Pai � NULL\n");
        return BAD;
    }

    for (int i = 0; i <= pai->qtdChaves; i++) // <= pois pode haver qtdChaves+1 filhos
    {
        if (pai->filhos[i] == filho)
        {
            return i;
        }
    }

    printf("\n[ERRO]<procuraIndiceFilho> Filho %p nao encontrado no vetor de filhos do pai %p\n", filho, pai);
    return BAD;
}

int sucessor(No *src, int index)
{
    No *aux = src->filhos[index+1];
    while(!aux->folha) aux = aux->filhos[0];
    return aux->chaves[0];
}

int predecessor(No *src, int index)
{
    No *aux = src->filhos[index-1];
    while(!aux->folha) aux = aux->filhos[aux->qtdChaves];
    return aux->chaves[aux->qtdChaves-1];
}

void removerFilho(No *pai, No *filho)
{
    if (!pai || !filho)
    {
        printf("[ERRO]<removerFilho> Pai ou filho nulo\n");
        return;
    }

    int idx = procuraIndiceFilho(filho, pai);
    if (idx == BAD)
    {
        printf("[ERRO]<removerFilho> Filho n�o encontrado no pai\n");
        return;
    }

    // Desloca filhos e chaves para a esquerda
    for (int i = idx; i < pai->qtdChaves; i++)
    {
        pai->filhos[i] = pai->filhos[i + 1];
        pai->chaves[i] = pai->chaves[i + 1];
    }

    // �ltimo ponteiro de filho
    pai->filhos[pai->qtdChaves] = pai->filhos[pai->qtdChaves + 1];
    pai->filhos[pai->qtdChaves + 1] = NULL;

    pai->qtdChaves--;
}


int verificaChave(No *atual, int chave)
{

    int chaveEncontrada = 0;
    for (int i = 0; i < atual->qtdChaves; i++)
    {
        if (atual->chaves[i] == chave)
        {
            chaveEncontrada = 1;
            break;
        }
    }
    return chaveEncontrada;
};

void deletarChave(Arvore *arv, int chave)
{


    No *folha = busca(arv->raiz, chave, 0);
    if (!folha)
    {
        printf("\n[ERRO] <deletarChave> Chave %d n�o encontrada na �rvore.\n", chave);
        return;
    }

    // Verifica se a chave est� na folha retornada
    int chaveEncontrada = verificaChave(folha,chave);


    if (!chaveEncontrada)
    {
        printf("\n[ERRO] <deletarChave> Chave %d n�o est� no n� folha.\n", chave);
        return;
    }
       // folha com ordem chaves e � raiz (unico caso em que folha ou n� podem ter qtdChaves < ORDEM
    No *pai = folha->pai;
    if (!pai)
    {

        removerChaveFolha(folha, chave);

        if (folha->qtdChaves == 0)
        {
            free(folha);
            arv->raiz = NULL;
            printf("\n[CASO RAIZ-FOLHA] Chave %d removida. �rvore agora est� vazia.\n", chave);
        }
        else
        {
            printf("\n[CASO RAIZ-FOLHA] Chave %d removida da raiz folha. Restam %d chaves.\n", chave, folha->qtdChaves);
        }
        return;
    }

    // Folha com mais que ORDEM chaves
    if (folha->qtdChaves > ORDEM)
    {
        removerChaveFolha(folha, chave);
        printf("\n[CASO I] Chave %d removida de folha com > ORDEM chaves.\n", chave);
        return;
    }




    //SE O NO FOLHA TEM qtdChaves == ORDEM CHAVES PROCURA IRMAO PRA PEGAR EMPRESTADO
    int idxFilho = procuraIndiceFilho(folha, pai);
    No *irmaoEsq = (idxFilho > 0) ? pai->filhos[idxFilho - 1] : NULL;
    No *irmaoDir = (idxFilho < pai->qtdChaves) ? pai->filhos[idxFilho + 1] : NULL;

     printf("%\n\tENDERECO DO IRMAO ESQUERDO: %p", irmaoEsq);

     printf("%\tENDERECO DO IRMAO DIREITO: %p\n", irmaoDir);



    if (irmaoEsq && irmaoEsq->qtdChaves > ORDEM)
    {
        removerChaveFolha(folha, chave);

        int chaveEmprestada = irmaoEsq->chaves[irmaoEsq->qtdChaves - 1]; //maior chave do irmao esquerdo

        removerChaveFolha(irmaoEsq, chaveEmprestada);
        addChaveEmFolha(folha, chaveEmprestada);

         pai->chaves[idxFilho-1] = chaveEmprestada; //chave emprestada pois na posicao  s� idxFilho s� podem existir valores

    printf("\n[CASO II-B] Redistribui��o com irm�o esquerdo. Chave emprestada: %d\n", chaveEmprestada);
        return;
    }
    else if (irmaoDir && irmaoDir->qtdChaves > ORDEM)
    {
         removerChaveFolha(folha, chave);

        int chaveEmprestada = irmaoDir->chaves[0]; //menor chave do irmao direito

        removerChaveFolha(irmaoDir, chaveEmprestada);
        addChaveEmFolha(folha, chaveEmprestada);

        // Atualiza separador no pai com nova menor do irm�o direito
        pai->chaves[idxFilho] = irmaoDir->chaves[0];

        printf("\n[CASO II-B] Redistribui��o com irm�o direito. Chave emprestada: %d\n", chaveEmprestada);
        return;
    }


    //implementar concatena��o arvore b+: caso nao possa pegar emprestado dos irmaos,
    //junta as duas folhas
    //arvore b+ passo 1: a chave presente no n� pai e que dividia as duas folhas � simplesmente apagada (s� servia de indice)
    //passo 2: reorganiza o n� pai, (uma chave foi apagada entao as chaves e filhos a partir da posicao da chave apagada voltam uma casa
    //passo 3: verifica se o n� pai ficou com qtdChaves< Ordem (n� interno de indice)tenha ficado com qtdChaves < ORDEM, deve realizar uma concatena��o assim como na arvoreb
    //concatena��o arvore b: junta dois n�s com atual->qtdChaves + irmao->qtdChaves < ORDEM ( a soma das duas chaves precisa ser menor que a ORDEM, pois vai descer a chave de indice que separa os dois irmaos
    //se nao for possivel realizar a concatena��o de arvore b, tamb�m reorganiza pegando emprestado dos irmaos.

}



/*============================================================*/


/*=====================FUN��ES PARA TESTE======================*/


void testeInsercaoSplit() {
    printf("\n==== INICIANDO TESTE DE INSERCAO COM SPLIT NA RAIZ ====\n");

    Arvore arv;
    arv.raiz = NULL;

    int chavesParaInserir[] = {10, 20, 5, 15, 25};
    int qtd = sizeof(chavesParaInserir) / sizeof(chavesParaInserir[0]);

    for (int i = 0; i < qtd; i++) {
        printf("\n>> Inserindo chave %d\n", chavesParaInserir[i]);
        inserir(&arv, chavesParaInserir[i], 0);
    }

    printf("\n==== ESTRUTURA FINAL DA ARVORE ====\n");
    imprimeNo(arv.raiz);
    if (arv.raiz->filhos[0]) {
        imprimeNo(arv.raiz->filhos[0]);
    }
    if (arv.raiz->filhos[1]) {
        imprimeNo(arv.raiz->filhos[1]);
    }

    // Confirma se folhas est�o encadeadas
    No *folha = arv.raiz->filhos[0];
    while (folha != NULL) {
        printf("\n[ENCAD] Folha %p -> prox: %p\n", folha, folha->prox);
        folha = folha->prox;
    }

    printf("\n==== FIM DO TESTE ====\n");
}


void imprimeArvore(No *no, int nivel) {
    if (!no) return;

    printf("\nN�vel %d | %s | End: %p | Pai: %p\n", nivel, no->folha ? "Folha" : "Interno", no, no->pai);
    printf("Chaves: ");
    for (int i = 0; i < no->qtdChaves; i++) {
        printf("%d ", no->chaves[i]);
    }
    printf("\n");

    if (!no->folha) {
        for (int i = 0; i <= no->qtdChaves; i++) {
            imprimeArvore(no->filhos[i], nivel + 1);
        }
    }
}

void verificaEncadeamentoFolhas(No *raiz) {
    if (!raiz) return;

    // Navega at� a folha mais � esquerda
    No *folha = raiz;
    while (!folha->folha) {
        folha = folha->filhos[0];
    }

    printf("\n--- Verificando encadeamento das folhas ---\n");

    while (folha != NULL) {
        printf("Folha %p: ", folha);
        for (int i = 0; i < folha->qtdChaves; i++) {
            printf("%d ", folha->chaves[i]);
        }
        printf("-> %p\n", folha->prox);
        folha = folha->prox;
    }
}

void testeInsercaoSplit2() {
    printf("\n==== INICIANDO TESTE DE INSER��O COM SPLITS ====\n");

    Arvore arv;
    arv.raiz = NULL;

    int chavesParaInserir[] = {10, 20, 5, 15, 25, 30, 7, 50, 45, 60, 70, 40, 3, 2, 4};
    int qtd = sizeof(chavesParaInserir) / sizeof(chavesParaInserir[0]);

    for (int i = 0; i < qtd; i++) {
        printf("\n>> Inserindo chave %d\n", chavesParaInserir[i]);
        inserir(&arv, chavesParaInserir[i], 0);
    }

    printf("\n==== �RVORE FINAL ====\n");
    imprimeArvore(arv.raiz, 0);

    printf("\n==== ENCCADEAMENTO DE FOLHAS ====\n");
    verificaEncadeamentoFolhas(arv.raiz);

    printf("\n==== BUSCA VALIDANDO TODAS AS CHAVES INSERIDAS ====\n");

    for (int i = 0; i < qtd; i++) {
        No *n = busca(arv.raiz, chavesParaInserir[i], 0);
        int achou = 0;
        for (int j = 0; j < n->qtdChaves; j++) {
            if (n->chaves[j] == chavesParaInserir[i]) {
                achou = 1;
                break;
            }
        }
        if (achou) {
            printf("[OK] Chave %d encontrada na folha %p\n", chavesParaInserir[i], n);
        } else {
            printf("[ERRO] Chave %d NAO encontrada corretamente!\n", chavesParaInserir[i]);
        }
    }

    printf("\n==== FIM DO TESTE ====\n");
}



void testeEmprestaEsquerdoDireito()
{
    printf("\n========== INICIANDO TESTE DE EMPRESTAR DIREITO==========\n");

    Arvore arv;
    arv.raiz = NULL;

    // Inserir chaves para gerar v�rias folhas e casos variados

     int chavesParaInserir[] = {10, 20, 5, 15, 25, 30, 7, 50, 45, 60, 70, 40, 3, 2, 4};
    int qtd = sizeof(chavesParaInserir) / sizeof(chavesParaInserir[0]);



    printf("\n[Inserindo valores para compor a �rvore inicial...]\n");
    for (int i = 0; i < qtd; i++)
    {
        inserir(&arv, chavesParaInserir[i], 0);
    }

    printf("\n=== �rvore ap�s inser��es iniciais ===\n");
    imprimeArvore(arv.raiz, 0);

    // === CASO I ===
    printf("\n[CASO I] Remover 70(folha tem mais que ORDEM chaves, s� remove)\n");
    deletarChave(&arv, 70);
    imprimeArvore(arv.raiz, 0);

    // === CASO II-A ===
    printf("\n[CASO II-A] Remover 60 \n");
    deletarChave(&arv, 60);
    imprimeArvore(arv.raiz, 0);

    printf("\n[CASO II-A] Remover 50 (ir� precisar de redistribui��o com irm�o esquerdo)\n");
    deletarChave(&arv, 50);
    imprimeArvore(arv.raiz, 0);

    printf("\n========== INICIANDO TESTE DE EMPRESTAR DO IRMAO DIREITO==========\n");
    inserir(&arv,60,0);
    inserir(&arv,70,0);
    imprimeArvore(arv.raiz, 0);

        printf("\n[CASO II-A] Remover 30 \n");
        deletarChave(&arv, 30);
     imprimeArvore(arv.raiz, 0);

}

void testeDelecao()
{
    printf("\n========== INICIANDO TESTE DE DELE��O (ORDEM 2) ==========\n");

    Arvore arv;
    arv.raiz = NULL;

    // Inserir chaves para gerar v�rias folhas e casos variados
    int valores[] = {10, 20, 30, 40, 50, 60, 70, 80, 90};
    int total = sizeof(valores) / sizeof(valores[0]);

    printf("\n[Inserindo valores para compor a �rvore inicial...]\n");
    for (int i = 0; i < total; i++)
    {
        inserir(&arv, valores[i], 0);
    }

    printf("\n=== �rvore ap�s inser��es iniciais ===\n");
    imprimeArvore(arv.raiz, 0);

    // === CASO I ===
    printf("\n[CASO I] Remover 90 (folha tem mais que ORDEM chaves, s� remove)\n");
    deletarChave(&arv, 90);
    imprimeArvore(arv.raiz, 0);

    // === CASO II-A ===
    printf("\n[CASO II-A] Remover 80 (ir� precisar de redistribui��o com irm�o esquerdo)\n");
    deletarChave(&arv, 80);
    imprimeArvore(arv.raiz, 0);

    // === CASO II-B ===
    printf("\n[CASO II-B] Remover 10 (ir� precisar de redistribui��o com irm�o direito)\n");
    deletarChave(&arv, 10);
    imprimeArvore(arv.raiz, 0);

    // === CASO III-A ===
    printf("\n[CASO III-A] Remover 30 (fus�o com irm�o esquerdo)\n");
    deletarChave(&arv, 30);
    imprimeArvore(arv.raiz, 0);

    // === CASO III-B ===
    printf("\n[CASO III-B] Remover 20 (fus�o com irm�o direito)\n");
    deletarChave(&arv, 20);
    imprimeArvore(arv.raiz, 0);

    // === CASO RAIZ-VIRA-FOLHA ===
    printf("\n[CASO RAIZ FOLHA] Criando �rvore m�nima e esvaziando...\n");

    Arvore arvMin;
    arvMin.raiz = NULL;
    inserir(&arvMin, 100, 0);
    inserir(&arvMin, 200, 0);

    imprimeArvore(arvMin.raiz, 0);

    printf("\nRemovendo 200 (sobra 100 na raiz folha)\n");
    deletarChave(&arvMin, 200);
    imprimeArvore(arvMin.raiz, 0);

    printf("\nRemovendo 100 (�rvore ficar� vazia)\n");
    deletarChave(&arvMin, 100);
    imprimeArvore(arvMin.raiz, 0);

    printf("\n========== FIM DO TESTE DE DELE��O ==========\n");
}


/*============================================================*/

int main()
{

    //testeInsercaoSplit2();

    //testeDelecao();

     testeEmprestaEsquerdoDireito();

    testeDelecao();

return 0;
}
