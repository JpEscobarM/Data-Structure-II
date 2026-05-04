#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "huffman.h"

void huffman_compactar_arquivo(const char *nomeBinario) {
    TabelaFrequencia tabela;
    Lista listaFrequencia;
    No *arvore = NULL;
    char **dicionario = NULL;
    char *texto = NULL;
    char *codificado = NULL;

    printf("\n[Huffman] Organizando tabela de frequencia para %s...\n", nomeBinario);
    texto = organiza_tabela_frequencia(&tabela, (char *)nomeBinario);
    if (!texto) {
        printf("[Huffman] Erro ao organizar tabela de frequencia para %s.\n", nomeBinario);
        return;
    }

    printf("[Huffman] Montando lista encadeada de frequencia...\n");
    organiza_lista_encadeada_frequencia(&listaFrequencia, &tabela);

    printf("[Huffman] Montando arvore de Huffman...\n");
    arvore = montar_arvore(&listaFrequencia);

    printf("[Huffman] Criando dicionario de codificacao...\n");
    organiza_dicionario(arvore, &dicionario);

    printf("[Huffman] Codificando texto...\n");
    codificado = codificar(dicionario, (unsigned char *)texto);
    if (!codificado) {
        printf("[Huffman] Erro na codificacao.\n");
        return;
    }

    printf("[Huffman] Gravando arquivo compactado (compactado.jp)...\n");
    compactar((unsigned char *)codificado,nomeBinario);

    printf("[Huffman] Compactacao concluida para %s.\n", nomeBinario);
    // Aqui poderia dar free em texto, codificado, dicionario, arvore, etc.
}

void huffman_descompactar_arquivo(const char *nomeBinario) {
    TabelaFrequencia tabela;
    Lista listaFrequencia;
    No *arvore = NULL;
    char *texto = NULL;

    printf("\n[Huffman] Reconstruindo arvore de Huffman a partir de %s...\n", nomeBinario);
    texto = organiza_tabela_frequencia(&tabela, (char *)nomeBinario);
    if (!texto) {
        printf("[Huffman] Erro ao organizar tabela de frequencia para %s.\n", nomeBinario);
        return;
    }

    organiza_lista_encadeada_frequencia(&listaFrequencia, &tabela);
    arvore = montar_arvore(&listaFrequencia);

    printf("[Huffman] Descompactando arquivo compactado.jp...\n");
    printf("========= SAIDA DESCOMPACTADA =========\n");
    descompactar(arvore,nomeBinario);
    printf("\n========= FIM DA SAIDA =========\n");
}

static int append_campo(char **ptexto, int *ptamanho, const char *campo) {
    int lenCampo = (int)strlen(campo);
    int novoTam = *ptamanho + lenCampo;

    char *tmp = realloc(*ptexto, novoTam + 1);
    if (!tmp) {
        perror("realloc");
        free(*ptexto);
        *ptexto = NULL;
        *ptamanho = 0;
        return 0;
    }

    *ptexto = tmp;

    // copia o campo direto para o final da string atual
    memcpy(*ptexto + *ptamanho, campo, lenCampo);

    *ptamanho = novoTam;
    (*ptexto)[*ptamanho] = '\0';

    return 1;
}

//========== FUNCOES DE LEITURA
char *ler_pedidos_tabela_frequencia(const char *nome_arquivo) {
    FILE *f = fopen(nome_arquivo, "rb");
    if (!f) { perror("Erro ao abrir pedido.bin"); return NULL; }

    char *texto = malloc(1);
    if (!texto) {
        perror("malloc");
        fclose(f);
        return NULL;
    }
    texto[0] = '\0';
    int tamanho = 0;

    Pedido p;

    char stringTemp[256];
     //printf("\nlendo arquivo pedido...");
    while (fread(&p, sizeof(Pedido), 1, f) == 1 ) {
        p.data[10]='\0';
        p.id_pedido[19]='\0';
        p.preco[9]='\0';

        //printf("%s %s %s", p.id_pedido,p.data,p.preco);
         snprintf(stringTemp, sizeof(stringTemp), "%s;%s;%s;\n",
                 p.id_pedido,
                 p.data,
                 p.preco);


        if (!append_campo(&texto, &tamanho,stringTemp))
        {
            printf("\nERRO AO LER PEDIDO");
            fclose(f);
            free(texto);
            return NULL;
        }
    }

    fclose(f);
    //printf("\nTerminou de ler pedido...");

    return texto;
}

char *ler_produtos_tabela_frequencia(const char *nome_arquivo)
{
    FILE *f = fopen(nome_arquivo, "rb");
    if (!f) {
        perror("Erro ao abrir produto.bin");
        return NULL;
    }

    char *texto = malloc(1);
    if (!texto) {
        perror("malloc");
        fclose(f);
        return NULL;
    }
    texto[0] = '\0';
    int tamanho = 0;

    Produto p;
    char stringTemp[256];
    //printf("\nlendo arquivo produto...");
    while (fread(&p, sizeof(Produto), 1, f) == 1) {


        p.id_produto[19] = '\0';
        p.alias[20]      = '\0';
        p.preco[9]       = '\0';
        p.genero[1]      = '\0';
        p.cor[10]        = '\0';
        p.material[10]   = '\0';
        p.joia[10]       = '\0';


        snprintf(stringTemp, sizeof(stringTemp), "%s;%s;%s;%s;%s;%s;%s;\n",
                 p.id_produto,
                 p.alias,
                 p.preco,
                 p.genero,
                 p.cor,
                 p.material,
                 p.joia);

        if (!append_campo(&texto, &tamanho,stringTemp))
        {
            printf("\nERRO AO LER PRODUTO");
            fclose(f);
            free(texto);
            return NULL;
        }

    }
    fclose(f);
   // printf("\nTerminou de ler produto...");
    return texto;
}


void inicializa_tabela_frequencia(TabelaFrequencia *t)
{
    t->size =256;
    for(int i = 0 ; i < t->size; i++)
    {
     t->table[i] = 0;

    }

}

/*Conta a frequencia de cada carcater usando unsigned int para ser de 0...255
*/
void conta_string(char *s, TabelaFrequencia *t)
{
    for (int i = 0; s[i] != '\0'; i++) {

        unsigned char c = (unsigned char) s[i];
        t->table[c]++;
    }
}

/*adiciona caracteres do pedido na tabela de frequencia*/
void adiciona_frequencia_pedido(Pedido p, TabelaFrequencia *t)
{
    conta_string(p.id_pedido,t);
    conta_string(p.data,t);
    conta_string(p.preco,t);
}
/*funcao que adiciona a contagem de strings do produto na tabela de frequencia*/
void adiciona_frequencia_produto(Produto p, TabelaFrequencia *t)
{
    conta_string(p.alias,t);
    conta_string(p.cor,t);
    conta_string(p.genero,t);
    conta_string(p.id_produto,t);
    conta_string(p.joia,t);
    conta_string(p.material,t);
    conta_string(p.preco,t);
}
void adiciona_frequencia_texto(char *texto, TabelaFrequencia *t)
{
    conta_string(texto,t);
}


void imprime_tabela_frequencia(TabelaFrequencia *t)
{
    for (int i = 0; i < t->size; i++) {
        if (t->table[i] != 0) {
            printf("'%c' (%d) apareceu %d vezes\n",i,i,t->table[i]);
        }
    }
}



void conta_buffer(unsigned char *buf, long tam, TabelaFrequencia *t) {
    for (long i = 0; i < tam; i++) {
        unsigned char c = buf[i];
        t->table[c]++;
    }
}

/*funcao que organiza a tabela de frequencia lendo o arquivo de pedidos e arquivo de produtos*/
char *organiza_tabela_frequencia(TabelaFrequencia *t, char *nome_arquivo)
{
    inicializa_tabela_frequencia(t);
    char *texto = NULL;

    if (strcasecmp(nome_arquivo, "produto.bin") == 0)     // iguais
    {
        texto = ler_produtos_tabela_frequencia("produto.bin");
    }
    else if (strcasecmp(nome_arquivo, "pedido.bin") == 0)
    {
        texto = ler_pedidos_tabela_frequencia("pedido.bin");
    }
    else
    {
        printf("Arquivo inválido: %s\n", nome_arquivo);
        return NULL;
    }

     if (!texto) {
        printf("Erro ao gerar texto\n");
        return NULL;
    }


    adiciona_frequencia_texto(texto,t);

    return texto;  // por enquanto só produto.bin
}


/*==========================================
    FUNCOES LISTA ENCADEADA PARA CRIACAO DA AROVRE
==========================================*/

void criar_lista(Lista *lista){
    lista->inicio = NULL;
    lista->tam = 0;
}

//INSERE ORDENADO NA LISTA ENCADEADA COM BASE NA FREQUENCIA
void inserir_ordenado(Lista *lista, No *no){
    No *aux;
    // a lista est� vazia?
    if(lista->inicio == NULL){
        lista->inicio = no;
    }
    // tem frequência menor que o início da lista
    else if(no->frequencia < lista->inicio->frequencia){
        no->proximo = lista->inicio;
        lista->inicio = no;
    }
    else{ // insere no meio ou no fim da lista
        aux = lista->inicio;
        while(aux->proximo && aux->proximo->frequencia <= no->frequencia)
            aux = aux->proximo;
        no->proximo = aux->proximo;
        aux->proximo = no;
    }
    lista->tam++; // incrementa o tamanho da lista
}
//FUNCAO QUE INICIALIZA LISTA ENCADEADA DE FREQUENCIA A PARTIR DA TABELA DE FREQUENCIA
void preencher_lista(TabelaFrequencia *t, Lista *lista){
    int i;
    No *novo;
    for(i = 0; i < t->size; i++){ // percorre toda a tabela
        if(t->table[i] > 0){ // se quantidade for maior que zero
            novo = malloc(sizeof(No));
            if(novo){
                novo->caracter = i;// posicao na tabela ja é o caracter
                novo->frequencia = t->table[i];
                novo->dir = NULL;
                novo->esq = NULL;
                novo->proximo = NULL;
                inserir_ordenado(lista, novo);
            }
            else{
                printf("\tERRO ao alocar memoria em preencher_lista!\n");
                break;
            }
        }
    }
}

void imprimir_lista(Lista *lista){
    No *aux = lista->inicio;
    printf("\n\tLista ordenada: Tamanho: %d\n", lista->tam);
    while(aux){
        printf("\tCaracter: %c Frequ�ncia: %d\n", aux->caracter, aux->frequencia);
        aux = aux->proximo;
    }
}

/*
     Função para remover um nó da lista encadeada
*/
No* remove_no_inicio(Lista *lista){
    No *aux = NULL;
    if(lista->inicio){
        aux = lista->inicio;
        lista->inicio = aux->proximo;
        aux->proximo = NULL;
        lista->tam--;
    }
    return aux;
}
void organiza_lista_encadeada_frequencia(Lista *l,TabelaFrequencia *t)
{
    //printf("\norganizando lista encadeada de frequencia...");
    criar_lista(l);
    preencher_lista(t,l);
   // printf("\nfinalizou...");
}

/*
     Procedimento para montar a árvore de huffman.
*/
No* montar_arvore(Lista *lista){
    No *primeiro, *segundo, *novo;
    while(lista->tam > 1){
        primeiro = remove_no_inicio(lista);
        segundo = remove_no_inicio(lista);
        novo = malloc(sizeof(No));
        if(novo){
            novo->caracter = '+';
            novo->frequencia = primeiro->frequencia + segundo->frequencia;
            novo->esq = primeiro;
            novo->dir = segundo;
            novo->proximo = NULL;
            inserir_ordenado(lista, novo);
        }
        else{
            printf("\n\tERRO ao alocar memoria em montar_arvore!\n");
            break;
        }
    }
    return lista->inicio;
}

/*
      Procedimento para imprimir na tela a árvore de huffman.
*/
void imprimir_arvore(No *raiz, int tam){
    if(raiz->esq == NULL && raiz->dir == NULL)
        printf("\tFolha: %c\tAltura: %d\n", raiz->caracter, tam);
    else{
        imprimir_arvore(raiz->esq, tam + 1);
        imprimir_arvore(raiz->dir, tam + 1);
    }
}


//=====MONTANDO DICIONARIO=====
//EXEMPLO
//'1' = 0010
//'2' = 0011
//'3' = 00110
/*
     Procedimento para descobrir a altura da árvore
*/
int altura_arvore(No *raiz){
    int esq, dir;
    if(raiz == NULL)
        return -1;
    else{
        esq = altura_arvore(raiz->esq) + 1;
        dir = altura_arvore(raiz->dir) + 1;
        if(esq > dir)
            return esq;
        else
            return dir;
    }
}
/*
     Função para alocar memória dinamicamente para o dicionário
*/
char** aloca_dicionario(int colunas){
    char **dicionario;
    int i;
    dicionario = malloc(sizeof(char*) * 256);
    for(i = 0; i < 256; i++)
        dicionario[i] = calloc(colunas, sizeof(char));
    return dicionario;
}
/*
      Procedimento para preencher o dicionário
*/
void gerar_dicionario(char **dicionario, No *raiz, char *caminho, int colunas){
    char esquerda[colunas], direita[colunas];
    if(raiz->esq == NULL && raiz->dir == NULL)
        strcpy(dicionario[raiz->caracter], caminho);
    else{
        strcpy(esquerda, caminho);
        strcpy(direita, caminho);
        strcat(esquerda, "0");
        strcat(direita, "1");
        gerar_dicionario(dicionario, raiz->esq, esquerda, colunas);
        gerar_dicionario(dicionario, raiz->dir, direita, colunas);
    }
}
/*
     Procedimento para imprimir o dicionário na tela
*/
void imprime_dicionario(char **dicionario){
    int i;
    printf("\n\tDicionario:");
    for(i = 0; i < 255; i++){
        if(strlen(dicionario[i]) > 0)
            printf("\t[%c] %3d: %s\n",i,i, dicionario[i]);
    }
}


void organiza_dicionario(No *raiz, char ***dicionario)
{
    int nro_colunas = altura_arvore(raiz);
   // printf("\nCriando Dicionario de mapeamento 0 e 1 ...");

    *dicionario = aloca_dicionario(nro_colunas+2);
    gerar_dicionario(*dicionario,raiz,"",nro_colunas);
    //printf("\nDicionario criado...");
}


/*
     Fun��o para calcular e retornar o tamanho do texto codificado
*/
int calcula_tamanho_string(char **dicionario, unsigned char *texto) {
    int i = 0, tam = 0;
    while (texto[i] != '\0') {
        tam += strlen(dicionario[texto[i]]);
        i++;
    }
    return tam + 1;
}


/*
    Função que codifica o texto. O retorno é o endereço da string codificada
*/
char* codificar(char **dicionario, unsigned char *texto) {
    int tam = calcula_tamanho_string(dicionario, texto);
    char *codigo = malloc((tam + 1) * sizeof(char));
    if (!codigo) {
        perror("malloc codificar");
        return NULL;
    }

    int k = 0; // posição de escrita em 'codigo'

    for (int i = 0; texto[i] != '\0'; i++) {
        unsigned char c = texto[i];
        char *code = dicionario[c];
        int len = (int)strlen(code);

        memcpy(codigo + k, code, len);
        k += len;
    }

    codigo[k] = '\0';
    return codigo;
}

/*
    Função para decodificar um texto
*/

char* decodificar(unsigned char texto[], No *raiz){
    int i = 0;


    No *aux = raiz;
    char temp[2];
    char *decodificado = calloc(strlen(texto), sizeof(char));


   // printf("\nDecoficando...");
    while(texto[i] != '\0'){
        if(texto[i] == '0')
            aux = aux->esq; // caminha para a esquerda
        else
            aux = aux->dir; // caminha para a direita

        // se for um nó folha concatena o caracter e volta para a raiz da árvore
        if(aux->esq == NULL && aux->dir == NULL){
            temp[0] = aux->caracter;
            temp[1] = '\0';
            strcat(decodificado, temp);
            aux = raiz;
        }

        i++;
    }
   // printf("\nDecoficado...");
    return decodificado;
}


/*
  Compactar --
*/
void compactar(unsigned char str[],char *nome){
    FILE *arquivo;
    if(strcasecmp(nome,"pedido.bin") == 0)
    {
     arquivo = fopen("pedido.jp", "wb");
    }
    else
    {
       arquivo = fopen("produto.jp", "wb");
    }

    int i = 0, j = 7;
    unsigned char mascara, byte = 0; // 00000000

    if(arquivo){
        while(str[i] != '\0'){
            mascara = 1;
            if(str[i] == '1'){
                mascara = mascara << j;
                byte = byte | mascara;
            }
            j--;

            if(j < 0){ // tem um byte formado
                fwrite(&byte, sizeof(unsigned char), 1, arquivo);
                byte = 0;
                j = 7;
            }

            i++;
        }
        if(j != 7) // tem um byte em formação
            fwrite(&byte, sizeof(unsigned char), 1, arquivo);
        fclose(arquivo);
    }
    else
        printf("\nErro ao abrir/criar arquivo em compactar\n");
}

/*
    Função para testar o bit i
*/
unsigned int eh_bit_um(unsigned char byte, int i){
    unsigned char mascara = (1 << i);
    return byte & mascara;
}

/*
    Função para ler o arquivo compactado e obter o texto original.
*/

void descompactar(No *raiz, char *nome){

     FILE *arquivo, *saidaDescompactacao;

     saidaDescompactacao = fopen("descompactado.txt","w");

    if(strcasecmp(nome,"pedido.bin") == 0 )
    {
     arquivo = fopen("pedido.jp", "rb");
    }
    else
    {
       arquivo = fopen("produto.jp", "rb");
    }
    No *aux = raiz;
    unsigned char byte;
    int i;

    if(arquivo){
        // enquanto conseguir ler do arquivo
        while(fread(&byte, sizeof(unsigned char), 1, arquivo)){
            for(i = 7; i >= 0; i--){
                if(eh_bit_um(byte, i))
                    aux = aux->dir;
                else
                    aux = aux->esq;

                if(aux->esq == NULL && aux->dir == NULL){
                    // printf("%c", aux->caracter);// imprime o caracter do nó folha
                    fputc(aux->caracter, saidaDescompactacao);
                    aux = raiz; // volta para a raiz da árvore
                }
            }
        }
        fclose(arquivo);
    }
    else
        printf("\nErro ao abrir arquivo em descompactar\n");
}
