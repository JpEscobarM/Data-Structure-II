#ifndef PEDIDO_H_INCLUDED
#define PEDIDO_H_INCLUDED
#include "types.h"

void adicionar_pedido(ListaPedido *lp, Pedido novoPedido);
void adicionar_index_pedido(ListaIndexPedido *li, Pedido novoPedido, int posicao);
void imprimir_lista_pedidos(ListaPedido *lp);
void imprimir_lista_index_pedido(ListaIndexPedido *li);
void lerPedidos(const char *nome_arquivo, ListaIndexPedido *li);

int cria_indice_pedido(ListaIndexPedido *li, int *nivel,int *qtdBlocos, int tamanhoBloco);
int organiza_indice_pedido(void);

int pesquisa_por_id_pedido(char idPedido[20], int nivel, int *posicao);
int inserir_pedido(Pedido p, int nivel);
void excluir_pedido(char id[20], int nivel);

void lerAssociacoes(const char *nome_arquivo); // mantida se precisar

#endif // PEDIDO_H_INCLUDED
