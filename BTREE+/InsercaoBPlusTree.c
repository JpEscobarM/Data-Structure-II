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

/*==========================CRIAR NÓ===========*/

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
        //A CHAVE DEVERIA ESTAR NAQUELA FOLHA
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

void splitRaizFolha(Arvore *arv,No *raiz)
{
    No *novoIrmao = criaNo(NULL,raiz->folha,0);

    int divisor = raiz->chaves[ORDEM]; //PEGA A CHAVE  DO MEIO QUE IRA VIRAR INDICE

    int i= raiz->qtdChaves-1;

    while(i >= ORDEM) //i>= pois precisa pegar a chave divisora também, respeitando a regra das folhas e Nó's internos(apenas de indice)
    {
        addChaveEmFolha(novoIrmao,raiz->chaves[i]);
        raiz->qtdChaves--; //quando chegar na posicao "ORDEM" ja "apaga" do no antigo o divisor
        i--;
    }

    raiz->prox = novoIrmao; //encadeira os irmaos seguindo conceito de b+


    No *novaRaiz = criaNo(NULL,0,0);

    novaRaiz->chaves[novaRaiz->qtdChaves] = divisor; //sobe o divisor pra servir também de indice.
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

        if(!interno->folha) //garante que tem filhos / não é um nó folha
        {
            //OBSERVAÇÃO PARA QUESTIONAMENTOS FUTUROS: novo->filhos = malloc(sizeof(No*)*(MAX_FILHOS+1)); //+1 para overflow de filho
            //a quantidade de filhos de uma ordem 2 por exemplo é 6 pois o calculo é (2*ordem)+1 como no define e ainda +1 para overflow
            //portanto nao corre risco de inserção invalido

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

    // Move as chaves e filhos para o novo irmão
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

    // Atualiza o número de chaves do nó atual meio = ORDEM
    atual->qtdChaves = meio;

    // Se o nó atual for a raiz
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
        // Caso contrário, adiciona ao pai atual
        addChaveEmNoInterno(atual->pai, chavePromovida, atual, novoIrmao);

        // Se o pai também ficou cheio, split recursivo
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

    if(pai->qtdChaves > MAX_CHAVES)//se o pai ficar cheio, desencadeia split em NÓ's internos
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

/*=====================FUNÇÕES PARA DELEÇÃO====================*/

int removerChaveFolha(No *atual, int chave)
{
    int valorIndefinido = BAD;
    int posicaoChave = valorIndefinido;

    if (!atual || atual->qtdChaves == 0) return -1;

    // Localiza a posição da chave
    for (int i = 0; i < atual->qtdChaves; i++)
    {
        if (atual->chaves[i] == chave)
        {
            posicaoChave = i;
            break;
        }
    }

    // Chave não encontrada
    if (posicaoChave == valorIndefinido) return -1;

    // Desloca as chaves à esquerda
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
        printf("\n[ERRO]<procuraIndiceFilho> Filho ou Pai é NULL\n");
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

void deletarChave(Arvore *arv, int chave)
{
    No *pai;
    No *folha = busca(arv->raiz, chave, 0);

    if (!folha)
    {
        printf("\n<deletarChave> Nao foi possivel encontrar a chave na arvore.\n");
        return;
    }

    // Verifica se a chave está na folha retornada
    int chaveEncontrada = 0;
    for (int i = 0; i < folha->qtdChaves; i++)
    {
        if (folha->chaves[i] == chave)
        {
            chaveEncontrada = 1;
            break;
        }
    }

    if (!chaveEncontrada)
    {
        printf("\n<deletarChave> Chave %d nao encontrada no nó folha.\n", chave);
        return;
    }

    // CASO I - Chave presente apenas no nó folha e ele tem mais que o mínimo
    if (folha->folha)
    {
        if (folha->pai != NULL)
        {
            pai = folha->pai;

            int estaEmIndice = 0;
            for (int i = 0; i < pai->qtdChaves; i++)
            {
                if (pai->chaves[i] == chave)
                {
                    estaEmIndice = 1;
                    break;
                }
            }

            if (!estaEmIndice && folha->qtdChaves > ORDEM)
            {
                if (removerChaveFolha(folha, chave) == BAD)
                {
                    printf("\n<removerChaveFolha> Erro ao remover chave %d do nó folha %p\n", chave, folha);
                }
                else
                {
                    printf("\nChave %d removida com sucesso do nó folha %p\n", chave, folha);
                }
                return;
            }
            else //CASO II - Há um número mínimo exato de chaves no nó == ORDEM.
            {
                pai = folha->pai;
                int indiceDoFilho = procuraIndiceFilho(folha, pai);

                No *irmaoEsq = (indiceDoFilho > 0) ? pai->filhos[indiceDoFilho - 1] : NULL;
                No *irmaoDir = (indiceDoFilho < pai->qtdChaves) ? pai->filhos[indiceDoFilho + 1] : NULL;

                removerChaveFolha(folha, chave);

                if (irmaoEsq && irmaoEsq->qtdChaves > ORDEM)
                {
                    int chaveEmprestada = irmaoEsq->chaves[irmaoEsq->qtdChaves - 1];
                    addChaveEmFolha(folha, chaveEmprestada);
                    removerChaveFolha(irmaoEsq, chaveEmprestada);

                    // Atualiza índice no pai com a nova menor chave do 'folha'
                    pai->chaves[indiceDoFilho - 1] = folha->chaves[0];
                }
                else if (irmaoDir && irmaoDir->qtdChaves > ORDEM)
                {
                    int chaveEmprestada = irmaoDir->chaves[0];
                    addChaveEmFolha(folha, chaveEmprestada);
                    removerChaveFolha(irmaoDir, chaveEmprestada);

                    // Atualiza índice no pai com a nova menor chave do irmão direito
                    pai->chaves[indiceDoFilho] = irmaoDir->chaves[0];
                }
                else
                {
                    printf("<redistribuicao> Nenhum irmão com chaves para emprestar\n");
                    // aqui viria o passo de fusão, que seria o "Caso III"
                }
            }
        }
        else// É FOLHA E RAIZ
        {
            // Caso a raiz seja folha (único nó na árvore)
            if (folha->qtdChaves > 1)
            {
                if (removerChaveFolha(folha, chave) == BAD)
                {
                    printf("\n<removerChaveFolha> Erro ao remover chave %d da raiz folha\n", chave);
                }
                else
                {
                    printf("\nChave %d removida com sucesso da raiz folha %p\n", chave, folha);
                }
                return;
            }
            else
            {
                // Última chave da raiz (único elemento na árvore)
                arv->raiz = NULL;
                free(folha);
                printf("\n[!] Chave %d removida. Arvore agora esta vazia.\n", chave);
                return;
            }
        }
    }
}

/*============================================================*/


/*=====================FUNÇÕES PARA TESTE======================*/

No *criaArvoreTeste(int debug) {

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

    // Confirma se folhas estão encadeadas
    No *folha = arv.raiz->filhos[0];
    while (folha != NULL) {
        printf("\n[ENCAD] Folha %p -> prox: %p\n", folha, folha->prox);
        folha = folha->prox;
    }

    printf("\n==== FIM DO TESTE ====\n");
}


void imprimeArvore(No *no, int nivel) {
    if (!no) return;

    printf("\nNível %d | %s | End: %p | Pai: %p\n", nivel, no->folha ? "Folha" : "Interno", no, no->pai);
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

    // Navega até a folha mais à esquerda
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
    printf("\n==== INICIANDO TESTE DE INSERÇÃO COM SPLITS ====\n");

    Arvore arv;
    arv.raiz = NULL;

    int chavesParaInserir[] = {10, 20, 5, 15, 25, 30, 7, 50, 45, 60, 70, 40, 3, 2, 4};
    int qtd = sizeof(chavesParaInserir) / sizeof(chavesParaInserir[0]);

    for (int i = 0; i < qtd; i++) {
        printf("\n>> Inserindo chave %d\n", chavesParaInserir[i]);
        inserir(&arv, chavesParaInserir[i], 0);
    }

    printf("\n==== ÁRVORE FINAL ====\n");
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

void testeDelecao()
{
    printf("\n========== INICIANDO TESTE DE DELECAO ==========\n");

    Arvore arv;
    arv.raiz = NULL;

    int valores[] = {5, 15, 20, 25, 30, 35, 45, 55};
    int total = sizeof(valores) / sizeof(valores[0]);

    // Inserção
    printf("\n[Inserindo valores para teste de delecao...]\n");
    for (int i = 0; i < total; i++)
    {
        inserir(&arv, valores[i],0);
    }

    printf("\nEstado da arvore após insercoes:\n");
    imprimeArvore(arv.raiz,0); // ou printArvore se preferir

    // === CASO I ===
    printf("\n[CASO I] Remover 55 (chave esta em folha com mais que ORDEM)\n");
    deletarChave(&arv, 55);
     imprimeArvore(arv.raiz,0);

    // === CASO II - Redistribuição com irmão esquerdo ===
    printf("\n[CASO II-A] Remover 5 (folha com exatamente ORDEM chaves, redistribuição com irmao esquerdo)\n");
    deletarChave(&arv, 5);
    imprimeArvore(arv.raiz,0);

    // === CASO II - Redistribuição com irmão direito ===
    printf("\n[CASO II-B] Remover 15 (redistribuicao com irmao direito)\n");
    deletarChave(&arv, 15);
   imprimeArvore(arv.raiz,0);

    // === CASO RAIZ É FOLHA ===
    printf("\n[CASO RAIZ FOLHA] Criando nova arvore com poucas chaves\n");

    Arvore arvMinima;
    arvMinima.raiz = NULL;
    inserir(&arvMinima, 100,0);
    inserir(&arvMinima, 200,0);

    imprimeArvore(arvMinima.raiz,0);

    printf("\nRemovendo 200 (ainda sobra 100 na raiz/folha)\n");
    deletarChave(&arvMinima, 200);
    imprimeArvore(arvMinima.raiz,0);

    printf("\nRemovendo 100 (arvore ficara vazia)\n");
    deletarChave(&arvMinima, 100);
    imprimeArvore(arvMinima.raiz,0);

    printf("\n========== FIM DO TESTE DE DELECAOO ==========\n");
}


/*============================================================*/

int main()
{

   // No *raizTeste = criaArvoreTeste(1);

   // No *resultadoBuscaTeste = busca(raizTeste,45,1);

    testeInsercaoSplit2();

    testeDelecao();

return 0;
}
