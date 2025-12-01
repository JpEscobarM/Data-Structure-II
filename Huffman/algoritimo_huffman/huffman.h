#ifndef HUFFMAN_H_INCLUDED
#define HUFFMAN_H_INCLUDED

typedef struct
{
    int size;
    int table[256];

}TabelaFrequencia;

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


//RETIRAR DEPOIS============================================
typedef struct {
    char data[11];
    char id_pedido[20];
    char preco[10];
} Pedido;

typedef struct {
    char id_produto[20];
    char alias[21];
    char preco[10];
    char genero[2];
    char cor[11];
    char material[11];
    char joia[11];
} Produto;

//=========================
// FUNÇÕES DE LEITURA
//=========================
void ler_pedidos_tabela_frequencia(const char *nome_arquivo,TabelaFrequencia *t);
void ler_produtos_tabela_frequencia(const char *nome_arquivo, TabelaFrequencia *t);


//=========================
// FUNÇÕES DE FREQUÊNCIA
//=========================
void inicializa_tabela_frequencia(TabelaFrequencia *t);
void organiza_tabela_frequencia(char *texto,int tamanho,TabelaFrequencia *t);
void imprime_tabela_frequencia(TabelaFrequencia *t);
void adiciona_frequencia_produto(Produto p, TabelaFrequencia *t);
void adiciona_frequencia_pedido(Pedido p, TabelaFrequencia *t);
void conta_string(char *s, TabelaFrequencia *t);


//==============
//FUNÇÕES LISTA ENCADEADA DE FREQUENCIA
//==============
void criar_lista(Lista *lista);
void inserir_ordenado(Lista *lista, No *no);
void preencher_lista(TabelaFrequencia *t, Lista *lista);
void imprimir_lista(Lista *lista);
No* remove_no_inicio(Lista *lista);


//==============
//AROVRE DE HUFFMAN
//==============
No* montar_arvore(Lista *lista);
void imprimir_arvore(No *raiz, int tam);

#endif // HUFFMAN_H_INCLUDED
