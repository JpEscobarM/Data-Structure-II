#include "./arquivo_indice.h"

#include <stdlib.h>

void salva_indice_hash(FILE *arquivo, indice_hash indice) {
    int i;
    indice_hash_node *node;

    for (i = 0; i < indice.tamanho; i++) {
        node = indice.valores[i];

        while (node) {
            // escreve cada entrada do dicionário
            fwrite(&node->chave, sizeof(chave_t), 1, arquivo);
            fwrite(&node->valor, sizeof(valor_t), 1, arquivo);
            node = node->proximo;
        }
    }
}

indice_hash carrega_indice_hash(FILE *arquivo) {
    int i;
    chave_t chave;
    valor_t valor;
    indice_hash indice = cria_indice_hash();

    while ((fread(&chave, sizeof(chave_t), 1, arquivo) == 1)
        && (fread(&valor, sizeof(valor_t), 1, arquivo) == 1)) {
        // recarrega nós do arquivo
        insere_indice_hash(&indice, chave, valor);
    }

    return indice;
}

// salva indice árvore B+ no arquivo
void salva_indice_arvorebp(FILE *arquivo, indice_arvorebp *arvore) {
    int i;

    if (!arvore) {
        return;
    }

    while (!arvore->folha) {
        arvore = arvore->filhos[0];
    }

    if (!arvore) {
        fprintf(stderr, "Ocorreu um erro ao percorrer a árvore B+\n");
        exit(1);
    }

    while (arvore) {
        for (i = 0; i <= arvore->num_chaves; i++) {
            fwrite(&arvore->chaves[i], sizeof(chave_t), 1, arquivo);
            fwrite(&arvore->valores[i], sizeof(valor_t), 1, arquivo);
        }

        arvore = arvore->proximo;
    }
}

// carrega indice árvore B+ do arquivo
indice_arvorebp* carrega_indice_arvorebp(FILE *arquivo, indice_arvorebp *arvore) {
    chave_t chave;
    valor_t valor;

    while ((fread(&chave, sizeof(chave_t), 1, arquivo) == 1)
        && (fread(&valor, sizeof(valor_t), 1, arquivo) == 1)) {
        arvore = insere_indice_arvorebp(arvore, chave, valor);
    }

    return arvore;
}