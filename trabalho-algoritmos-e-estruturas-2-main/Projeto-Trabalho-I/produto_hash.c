#include "hash.h"
#include "produto.h"
#include "util.h"
#include "arquivo_indice.h"
#include "produto_hash.h"

#include <stdio.h>

indice_hash produtos_hash;

indice_hash cria_indice_produto_hash() {
    Produto produto;
    chave_t chave;
    valor_t valor = 0;
    indice_hash indice = cria_indice_hash();

    FILE *arquivo = fopen(ARQUIVO_PRODUTOS, "rb");

    long long posicao_arquivo;

    if (arquivo == NULL) {
        fprintf(stderr, "Ocorreu um erro ao abrir o arquivo de produtos.\n");
        exit(1);
    }

    while (fread(&produto, sizeof(Produto), 1, arquivo) == 1) {
        chave = atol(produto.id_produto);
        insere_indice_hash(&indice, chave, valor);
        valor = ftell(arquivo);
    }

    fclose(arquivo);

    return indice;
}

void organiza_indice_produto_hash() {
    FILE *arquivo = fopen("IndiceProdutoHash.bin", "rb");
    
    if (!arquivo) {
        // não existe, criamos
        arquivo = fopen("IndiceProdutoHash.bin", "wb");
        produtos_hash = cria_indice_produto_hash();

        salva_indice_hash(arquivo, produtos_hash);
    }
    else {
        // existe, carregamos
        produtos_hash = carrega_indice_hash(arquivo);
    }
    
    fclose(arquivo);
}

Produto busca_produto_hash(char id_produto[20]) {

    Produto produto;
    FILE *arquivo = fopen(ARQUIVO_PRODUTOS, "rb");
    chave_t chave = atol(id_produto);
    valor_t valor;

    if (!arquivo) {
        fprintf(stderr, "Deu pau.\n");
        exit(1);
    }

    valor = *busca_indice_hash(&produtos_hash, chave);

    fseek(arquivo, valor, SEEK_SET);
    fread(&produto, sizeof(Produto), 1, arquivo);

    fclose(arquivo);

    return produto;
}
