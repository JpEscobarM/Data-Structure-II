#ifndef ASSOCIACAO_PROD_INDEX_H_INCLUDED
#define ASSOCIACAO_PROD_INDEX_H_INCLUDED



/* Constrói AssociacaoPorProduto.bin + IndiceAssocProdNivel-*.bin
   Retorna o maior nível N (>=1) ou -1 em erro. */
int organiza_indice_associacao_produto();

/* Busca multinível por id_produto. Retorna 0 e define *start/*count;
   -1 se não encontrado; -2 em erro. */
int pesquisa_por_id_associacao_produto(const char id_produto[20],
                                       int nivel,
                                       int *start,
                                       int *count);

/* Opcional: varre apenas o nível-1 e retorna o produto mais frequente (mais vendido). */
int produto_mais_vendido_via_indice(int nivelProduto, Produto *outProd, long *outQtd);


#endif // ASSOCIACAO_PROD_INDEX_H_INCLUDED
