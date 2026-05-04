#include "teste_performance.h"

#include "util.h"
#include "produto.h"
#include "produto_hash.h"

#include <stdio.h>

#define NUM_PRODUTOS 100

int carrega_produtos(char id_produtos[NUM_PRODUTOS][20]) {
    int i = 0, j = 0;
    Produto produto;
    FILE *arquivo = fopen(ARQUIVO_PRODUTOS, "rb");
    
    printf("Carregando produtos...\n");

    if (!arquivo) {
        return 0;
    }

    while (fread(&produto, sizeof(Produto), 1, arquivo) == 1
            && i < NUM_PRODUTOS) {
        for (j = 0; j < 20; j++) {
            id_produtos[i][j] = produto.id_produto[j];
        }
        i++;
    }

    fclose(arquivo);
    return i;
}

void carrega_produtos_indice_arquivo(char id_produtos[NUM_PRODUTOS][20], int lidos, int nivel_produto) {
    int i, sucessos = 0, erros = 0;
    Produto p;

    for (i = 0; i < lidos; i++) {
        p = busca_por_produto(id_produtos[i], nivel_produto);

        if (is_null(p.id_produto)) {
            erros++;
        } else {
            sucessos++;
        }
    }

    printf("Busca de produtos por arquivo. Sucessos: %d, erros: %d.\n", sucessos, erros);
}

void carrega_produtos_indice_hash(char id_produtos[NUM_PRODUTOS][20], int lidos) {
    int i, sucessos = 0, erros = 0;
    Produto p;

    for (i = 0; i < lidos; i++) {
        p = busca_produto_hash(id_produtos[i]);

        if (is_null(p.id_produto)) {
            erros++;
        } else {
            sucessos++;
        }
    }

    printf("Busca de produtos por hash. Sucessos: %d, erros: %d.\n", sucessos, erros);
}

void teste_performance(int nivel_produto) {
    int numero_produtos = 0;
    int lidos;
    char id_produtos[NUM_PRODUTOS][20];

    printf("Teste de performance de indices\n");

    MEDIR_TEMPO("Carregando produtos para testes", {
        lidos = carrega_produtos(id_produtos);
    });
    putchar('\n');

    if (lidos == 0) {
        fprintf(stderr, "Nao foi possível ler o arquivo de produtos\n.");
        return;
    }

    printf("Testes realizados com %d produtos.\n", lidos);

    MEDIR_TEMPO("Buscando produtos pelo indice de arquivo",
        carrega_produtos_indice_arquivo(id_produtos, lidos, nivel_produto));

    putchar('\n');

    MEDIR_TEMPO("Buscando produtos pelo indice de hash",
        carrega_produtos_indice_hash(id_produtos, lidos));
}