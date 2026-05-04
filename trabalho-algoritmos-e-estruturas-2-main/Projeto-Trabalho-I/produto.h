#ifndef PRODUTO_H_INCLUDED
#define PRODUTO_H_INCLUDED
#include "types.h"

void adicionar_produto(ListaProduto *lp, Produto novoProduto);
void adicionar_index_produto(ListaIndexProduto *li, Produto novoProduto, int posicao);
void imprimir_lista_index_produto(ListaIndexProduto *li);
void imprimir_lista_produtos(ListaProduto *lp);
void lerProdutos(const char *nome_arquivo, ListaIndexProduto *li);

int cria_indice_produto(ListaIndexProduto *li, int *nivel, int *qtdBlocos, int tamanhoBloco);
int organiza_indice_produto(void);

int pesquisa_por_id_produto(char idProduto[20], int nivel, int *posicao);
int inserir_produto(Produto p, int nivel);
void excluir_produto(char id[20], int nivel);

Produto busca_por_produto(char id_produto[20], int nivel);

#endif // PRODUTO_H_INCLUDED
