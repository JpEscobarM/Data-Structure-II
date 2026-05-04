#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned long id_pedido;
typedef unsigned long id_produto;

typedef struct {
    char data[11];
    char id_pedido[20];
    char preco[10];
} Pedido;

typedef struct {
    char id_pedido[20];
    int  posicao;
    int  excluido;
    int  elo;
    int  qtdExtensao;
    int  tamExtensao;
} IndexPedido;

typedef struct {
    IndexPedido *listaIndex;
    int qtdIndex;
} ListaIndexPedido;

typedef struct{
    Pedido *lista;
    int qtdPedido;
} ListaPedido;

typedef struct{
    int posicaoInicio;
} ArquivoIndice;

typedef struct {
    char id_pedido[20];
    char id_produto[20];
} Associacao;

typedef struct {
    char id_produto[20];
    char alias[21];
    char preco[10];
    char genero[2];
    char cor[11];
    char material[11];
    char joia[11];
} Produto;

typedef struct{
    Produto *lista;
    int qtdProduto;
} ListaProduto;

typedef struct {
    char id_produto[20];
    int  posicao;
    int  excluido;
    int  elo;
    int  qtdExtensao;
    int  tamExtensao;
} IndexProduto;

typedef struct {
    IndexProduto *listaIndex;
    int qtdIndex;
} ListaIndexProduto;

typedef struct {
    char id_pedido[20];
    int  start;   /* posição inicial no arquivo AssociacaoOrdenada.bin */
    int  count;   /* quantidade de associações desse id_pedido */
} IndexAssociacao;

typedef struct {
    char id_produto[20];
    int  posicao;  /* usado em níveis >1 para apontar bloco no nível inferior */
    int  start;    /* nível 1: início no arquivo ordenado */
    int  count;    /* nível 1: quantidade no arquivo ordenado */
} IndexAssocProd;


#endif // TYPES_H_INCLUDED
