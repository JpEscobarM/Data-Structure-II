#if !defined(__escreve_h__)
#define __escreve_h__

#include <stdio.h>

#include "./hash.h"
#include "./bptree.h"

// funções relacionadas à manipulação dos arquivos de índices

// salva indice hash no arquivo
void salva_indice_hash(FILE *arquivo, indice_hash indice);

// carrega indice hash do arquivo
indice_hash carrega_indice_hash(FILE *arquivo);

// salva indice árvore B+ no arquivo
void salva_indice_arvorebp(FILE *arquivo, indice_arvorebp *arvore);

// carrega indice árvore B+ do arquivo
indice_arvorebp *carrega_indice_arvorebp(FILE *arquivo, indice_arvorebp *arvore);

#endif // __escreve_h__
