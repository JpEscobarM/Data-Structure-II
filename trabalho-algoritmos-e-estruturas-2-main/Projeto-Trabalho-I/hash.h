#if !defined(__hash_h__)
#define __hash_h__

#include "util.h"

#define TAMANHO_DICIONARIO (50)

typedef struct indice_hash_node_t {
    chave_t chave;
    valor_t valor;
    int contagem;
    struct indice_hash_node_t   *proximo;
} indice_hash_node;

typedef struct indice_hash_t {
    int                 tamanho;
    indice_hash_node    **valores;
} indice_hash;

// cria índice hash com capacidade para *tamanho_dicionario* elementos
indice_hash cria_indice_hash();

// insere a chave com o determinado valor no índice
void insere_indice_hash(indice_hash *indice, chave_t chave, valor_t valor);

// busca chave dentro do índice, retornando NOT_FOUND caso não encontre
valor_t* busca_indice_hash(indice_hash *indice, chave_t chave);

// remove a chave do índice
void remove_indice_hash(indice_hash *indice, chave_t chave);

// exclui todas as chaves do índice
void deleta_indice_hash(indice_hash *indice);

#endif // __hash_h__
