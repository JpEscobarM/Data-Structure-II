#if !defined(__associacao_h__)
#define __associacao_h__

#include <stdio.h>

#include "bptree.h"
#include "dados.h"
#include "util.h"

typedef struct lista_produtos_t {
    chave_t id_produto;
    struct lista_produtos_t *proximo;
} lista_produtos;

extern indice_arvorebp* indice_associacao;

void carrega_indice_associacao(FILE *arquivo);

void salva_indice_associacao(FILE *arquivo);

void insere_associacao_no_indice(Associacao associacao);

lista_produtos* produtos_associados_a_pedido(id_pedido pedido);

void limpa_lista_produtos(lista_produtos* lista);

#endif // __associacao_h__
