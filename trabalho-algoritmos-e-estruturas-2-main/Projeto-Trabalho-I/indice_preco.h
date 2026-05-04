#if !defined(__indice_preco_h__)
#define __indice_preco_h__

#include "arquivo_indice.h"

extern indice_arvorebp *indice_precos;

void inicializa_indice_precos();
void organiza_indice_preco();
void mostra_produtos_em_faixa(float valor_minimo, float valor_maximo, int nivel_produto);

#endif // __indice_preco_h__
