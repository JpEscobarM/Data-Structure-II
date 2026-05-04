#if !defined(__btree_h__)
#define __btree_h__

#include "./util.h"

#define ORDEM 10

#define MAX_CHAVES (ORDEM - 1)
#define MIN_CHAVES ((ORDEM % 2) ? (ORDEM/2) : (ORDEM/2 - 1))

typedef struct indice_arvorebp_node {
    int folha;
    int num_chaves;
    chave_t chaves[MAX_CHAVES];
    valor_t valores[MAX_CHAVES];
    struct indice_arvorebp_node *pai;
    struct indice_arvorebp_node *filhos[ORDEM];
    struct indice_arvorebp_node *proximo;
} indice_arvorebp_node;

typedef indice_arvorebp_node indice_arvorebp;

typedef struct iterador_arvorebp_t {
    int i_atual;
    chave_t chave;
    indice_arvorebp_node *folha;
} iterador_arvorebp;

// árvore B+

indice_arvorebp* cria_indice_arvorebp();

indice_arvorebp *insere_indice_arvorebp(indice_arvorebp *raiz, chave_t chave, valor_t valor);

iterador_arvorebp busca_muitos_indice_arvorebp(indice_arvorebp_node *raiz, chave_t chave);

iterador_arvorebp busca_primeiro_maior_ou_igual(indice_arvorebp_node *raiz, chave_t chave_inicial);

valor_t busca_indice_arvorebp(indice_arvorebp *raiz, chave_t chave);

void remove_indice_arvorebp(indice_arvorebp_node *folha, chave_t chave);

void exclui_indice_arvorebp(indice_arvorebp *raiz);

void imprimir_arvore(indice_arvorebp *raiz);

// iterador

void avanca_iterador_arvorebp(iterador_arvorebp *iterador);

int possui_valor_iterador_arvorebp(iterador_arvorebp iterador);

int intervalo_valido_iterador_arvorebp(iterador_arvorebp it, chave_t chave_final);

chave_t valor_iterador_arvorebp(iterador_arvorebp iterador);

#endif // __btree_h__
