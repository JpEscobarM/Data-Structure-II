#ifndef SORT_H_INCLUDED
#define SORT_H_INCLUDED

#include "types.h"

typedef int (*cmp_fn)(const void*, const void*);

int comparaProduto(const void *a, const void *b);
int comparaPedido(const void *a, const void *b);
int comparaIndexPedido(const void *a, const void *b);
int comparaIndexProduto(const void *a, const void *b);
int cmp_assoc_by_pedido(const void *a, const void *b);
int cmp_assoc_by_prod(const void *a, const void *b);

#endif // SORT_H_INCLUDED
