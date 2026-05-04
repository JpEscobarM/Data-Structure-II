#if !defined(__produto_hash_h__)
#define __produto_hash_h__

#include "hash.h"
#include "produto.h"

indice_hash cria_indice_produto_hash();

void organiza_indice_produto_hash();

Produto busca_produto_hash(char id_produto[20]);

#endif // __produto_hash_h__
