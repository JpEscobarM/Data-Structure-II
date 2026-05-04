#include "hash.h"

#include <stdlib.h>
#include <string.h>

indice_hash_node *cria_node(chave_t chave, valor_t valor) {
    indice_hash_node *node = malloc(sizeof(indice_hash_node));

    node->chave = chave;
    node->valor = valor;
    node->contagem = 1;
    node->proximo = NULL;

    return node;
}

indice_hash_node* busca_indice_hash_node(indice_hash *indice, chave_t chave) {
    indice_hash_node *node;
    int idx = chave % indice->tamanho;

    node = indice->valores[idx];

    // iteramos toda a lista enquanto não encontrarmos a mesma chave
    while (node && node->chave != chave) node = node->proximo;

    return node;
}

void libera_node(indice_hash_node *node) {
    free(node);
}

void libera_lista_node(indice_hash_node *node) {
    if (node) {
        libera_lista_node(node->proximo);
    }

    libera_node(node);
}

indice_hash_node *remove_node(indice_hash_node *node, chave_t chave) {
    indice_hash_node *proximo;

    if (!node) {
        return NULL;
    }

    if (node->chave == chave) {
        proximo = node->proximo;
        libera_node(node);
        return proximo;
    }

    node->proximo = remove_node(node->proximo, chave);
    return node;
}


indice_hash cria_indice_hash() {
    indice_hash indice;

    indice.tamanho = TAMANHO_DICIONARIO;
    indice.valores = calloc(indice.tamanho, sizeof(valor_t));

    return indice;
}

void insere_indice_hash(indice_hash *indice, chave_t chave, valor_t valor) {
    indice_hash_node *node;
    indice_hash_node *novo;

    int idx = chave % indice->tamanho;

    // elemento já existe
    node = busca_indice_hash_node(indice, chave); 
    if (node) {
        if (valor)

        node->contagem++;
        return;
    }

    node = indice->valores[idx];
    novo = cria_node(chave, valor);

    // verifica se houve colisão...
    if (!node) {
        // ...se não, insere na posição diretamente e retorna
        indice->valores[idx] = novo;
        return;
    }

    novo->proximo = indice->valores[idx];
    indice->valores[idx] = novo;
}

valor_t* busca_indice_hash(indice_hash *indice, chave_t chave) {
    indice_hash_node *node = busca_indice_hash_node(indice, chave);

    // se o nó é nulo, então varremos toda a lista sem encontrar nosso elemento
    // retorna nulo
    if (!node) {
        return NULL;
    }

    // retorna valor encontrado
    return &node->valor;
}

void remove_indice_hash(indice_hash *indice, chave_t chave) {
    indice_hash_node *node;
    int idx = chave % indice->tamanho;

    indice->valores[idx] = remove_node(indice->valores[idx], chave);
}

void deleta_indice_hash(indice_hash *indice) {
    int i;

    for (i = 0; i < indice->tamanho; i++) {
        libera_node(indice->valores[i]);
    }

    free(indice->valores);
    indice->tamanho = 0;
}