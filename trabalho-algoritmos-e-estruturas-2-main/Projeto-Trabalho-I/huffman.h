#ifndef HUFFMAN_H_INCLUDED
#define HUFFMAN_H_INCLUDED
#include "types.h"

typedef struct
{
    int size;
    int table[256];

}TabelaFrequencia;


/*==================================================
   BLOCO AUXILIAR HUFFMAN
=================================================*/
void huffman_compactar_arquivo(const char *nomeBinario);
void huffman_descompactar_arquivo(const char *nomeBinario);

/*==================================================
   ESTRUTURA DE NO USADA PARA CRIAR A FILA E ARVORE
=================================================*/
typedef struct no{
    unsigned char caracter;
    int frequencia;
    struct no *esq, *dir, *proximo;
}No;
/*==================================================
   ESTRUTURA DE LISTA
=================================================*/
typedef struct{
    No *inicio;
    int tam;
}Lista;


//=========================
// FUN��ES DE LEITURA
//=========================
char *ler_pedidos_tabela_frequencia(const char *nome_arquivo);
char *ler_produtos_tabela_frequencia(const char *nome_arquivo);


//=========================
// FUN��ES DE FREQU�NCIA
//=========================
void inicializa_tabela_frequencia(TabelaFrequencia *t);
char *organiza_tabela_frequencia(TabelaFrequencia *t, char *nome_arquivo);
void imprime_tabela_frequencia(TabelaFrequencia *t);

void adiciona_frequencia_produto(Produto p, TabelaFrequencia *t);
void adiciona_frequencia_pedido(Pedido p, TabelaFrequencia *t);
void adiciona_frequencia_texto(char *texto, TabelaFrequencia *t);

void conta_string(char *s, TabelaFrequencia *t);


//==============
//FUN��ES LISTA ENCADEADA DE FREQUENCIA
//==============
void criar_lista(Lista *lista);
void inserir_ordenado(Lista *lista, No *no);
void preencher_lista(TabelaFrequencia *t, Lista *lista);
void imprimir_lista(Lista *lista);
No* remove_no_inicio(Lista *lista);
void organiza_lista_encadeada_frequencia(Lista *l,TabelaFrequencia *t);

//==============
//AROVRE DE HUFFMAN
//==============
No* montar_arvore(Lista *lista);

//==============
//DICIONARIO
//==============
void organiza_dicionario(No *raiz, char ***dicionario);
void imprime_dicionario(char **dicionario);

//============================
//FUNCOES CODIFICAR STRING
//============================
char* codificar(char **dicionario, unsigned char *texto);
int calcula_tamanho_string(char **dicionario, unsigned char *texto);

//============================
//FUNCOES DECODIFICAR STRING
//============================
char* decodificar(unsigned char texto[], No *raiz);

//============================
//COMPACTAR
//============================
void compactar(unsigned char str[],char *nome);


//============================
//DESCOMPACTAR
//============================
unsigned int eh_bit_um(unsigned char byte, int i);
void descompactar(No *raiz, char *nome);

#endif // HUFFMAN_H_INCLUDED
