#include <stdio.h>
#include "huffman.h"


//========== FUNCOES DE LEITURA
void ler_pedidos_tabela_frequencia(const char *nome_arquivo,TabelaFrequencia *t) {
    FILE *f = fopen(nome_arquivo, "rb");
    if (!f) { perror("Erro ao abrir pedido.bin"); return; }

    Pedido p;
    while (fread(&p, sizeof(Pedido), 1, f) == 1 ) {
        p.data[10]='\0'; p.id_pedido[19]='\0'; p.preco[9]='\0';

        adiciona_frequencia_pedido(p,t);
    }

    fclose(f);
}

void ler_produtos_tabela_frequencia(const char *nome_arquivo, TabelaFrequencia *t)
{
    FILE *f = fopen(nome_arquivo, "rb");
    if (!f) { perror("Erro ao abrir produto.bin"); return; }

    Produto p;
    while (fread(&p, sizeof(Produto), 1, f) == 1) {
        p.id_produto[19]='\0'; p.alias[20]='\0'; p.preco[9]='\0';
        p.genero[1]='\0'; p.cor[10]='\0'; p.material[10]='\0'; p.joia[10]='\0';
        adiciona_frequencia_produto(p,t);
    }

    fclose(f);
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

void imprime_tabela_frequencia(TabelaFrequencia *t)
{
    for (int i = 0; i < t->size; i++) {
        if (t->table[i] != 0) {
            printf("'%c' (%d) apareceu %d vezes\n",i,i,t->table[i]);
        }
    }
}

unsigned char* carregar_arquivo_como_texto(const char* nome, int* size_out) {
    FILE *f = fopen(nome, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    fseek(f, 0, SEEK_SET);

    unsigned char *buffer = malloc(tamanho + 1);
    if (!buffer) { fclose(f); return NULL; }

    fread(buffer, 1, tamanho, f);
    fclose(f);

    buffer[tamanho] = '\0';   // garante comportamento de string
    *size_out = tamanho;

    return buffer;
}

void conta_buffer(unsigned char *buf, long tam, TabelaFrequencia *t) {
    for (long i = 0; i < tam; i++) {
        unsigned char c = buf[i];
        t->table[c]++;
    }
}

/*funcao que organiza a tabela de frequencia lendo o arquivo de pedidos e arquivo de produtos*/
void organiza_tabela_frequencia(char *texto,int tamanho,TabelaFrequencia *t)
{
     inicializa_tabela_frequencia(t);

    /*TENTANDO COM ARQUIVOS BINARIOS
    char *arquivoProduto ="produto.bin";
     ler_produtos_tabela_frequencia(arquivoProduto,t);
     char *arquivoPedido = "pedido.bin";
    ler_pedidos_tabela_frequencia(arquivoPedido,t);

    */

   conta_buffer(texto,tamanho,t);
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
    // a lista está vazia?
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
        printf("\tCaracter: %c Frequência: %d\n", aux->caracter, aux->frequencia);
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
    criar_lista(l);
    preencher_lista(t,l);
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
    printf("\n\tDicionario:\n");
    for(i = 0; i < 255; i++){
        if(strlen(dicionario[i]) > 0)
            printf("\t[%c] %3d: %s\n",i,i, dicionario[i]);
    }
}


void organiza_dicionario(No *raiz, char ***dicionario)
{
   int nro_colunas = altura_arvore(raiz);
    *dicionario = aloca_dicionario(nro_colunas+2);
    gerar_dicionario(*dicionario,raiz,"",nro_colunas);

}


/*
     Função para calcular e retornar o tamanho do texto codificado
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
    char *codigo = calloc(tam, sizeof(char));

    for (int i = 0; texto[i] != '\0'; i++) {
        strcat(codigo, dicionario[texto[i]]);
    }
    return codigo;
}

char* codificar_buffer(char **dicionario, unsigned char *buf, long tam_buf) {
    long tamanho_final = 0;

    // calcula tamanho exato
    for (long i = 0; i < tam_buf; i++)
        tamanho_final += strlen(dicionario[buf[i]]);

    char *saida = calloc(tamanho_final + 1, 1);

    long pos = 0;
    for (long i = 0; i < tam_buf; i++) {
        char *bits = dicionario[buf[i]];
        long len = strlen(bits);
        memcpy(saida + pos, bits, len);
        pos += len;
    }

    saida[pos] = '\0';
    return saida;
}

//testar?? funciona??
char* decodificar(No *raiz, char *texto_codificado) {
    No *atual = raiz;

    int tamanho = strlen(texto_codificado);
    char *resultado = calloc(10000000, sizeof(char)); // ajuste depois
    int pos = 0;

    for (int i = 0; i < tamanho; i++) {

        if (texto_codificado[i] == '0')
            atual = atual->esq;
        else
            atual = atual->dir;

        if (atual->esq == NULL && atual->dir == NULL) {
            resultado[pos++] = atual->caracter;
            atual = raiz;
        }
    }

    resultado[pos] = '\0';
    return resultado;
}
