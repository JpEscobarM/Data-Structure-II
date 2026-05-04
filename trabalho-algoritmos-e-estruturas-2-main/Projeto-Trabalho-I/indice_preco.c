#include "indice_preco.h"
#include "produto.h"

#define FLOAT_TO_CHAVE(x) ((long) ((x) * 100.0))

indice_arvorebp *indice_precos = NULL;

void inicializa_indice_precos() {
    Produto produto;
    FILE *produtos = fopen(ARQUIVO_PRODUTOS, "rb");
    chave_t chave;
    valor_t valor;

    if (produtos == NULL) {
        fprintf(stderr, "Ocorreu um erro ao abrir o arquivo de produtos.\n");
        exit(1);
    }

    while (fread(&produto, sizeof(Produto), 1, produtos) == 1) {
        chave = FLOAT_TO_CHAVE(atof(produto.preco));
        valor = atol(produto.id_produto);

        indice_precos = insere_indice_arvorebp(indice_precos, chave, valor);
    }

    fclose(produtos);
}

void organiza_indice_preco() {
    
    printf("\nCriando Arquivo de Indice de Preços...\n");
    if (indice_precos == NULL) {
        indice_precos = cria_indice_arvorebp();
    }
    
    // verifica se o indice já existe
    FILE *preco_indice = fopen("IndicePrecos.bin", "rb");

    if (preco_indice == NULL) {
        // arquivo não existe, devemos criá-lo
        preco_indice = fopen("IndicePrecos.bin", "wb");

        inicializa_indice_precos();
        salva_indice_arvorebp(preco_indice, indice_precos);
    }
    else {
        // arquivo já existe, apenas o carregamos na memória
        indice_precos = carrega_indice_arvorebp(preco_indice, indice_precos);
    }

    fclose(preco_indice);
}

void mostra_produtos_em_faixa(float valor_minimo, float valor_maximo, int nivel_produto) {
    chave_t min = FLOAT_TO_CHAVE(valor_minimo);
    chave_t max = FLOAT_TO_CHAVE(valor_maximo);
    Produto produto;
    char produto_id[20];

    iterador_arvorebp iterador = busca_primeiro_maior_ou_igual(indice_precos, min);

    while (intervalo_valido_iterador_arvorebp(iterador, max)) {
        memset(produto_id, 0, 20);
        sprintf(produto_id, "%ld", valor_iterador_arvorebp(iterador));

        produto = busca_por_produto(produto_id, nivel_produto);

        if (is_null(produto.id_produto)) {
            printf("Ocorreu um problema ao buscar o produto %s\n", produto_id);
        }
        else {
            printf("Encontrado produto %s valendo %s reais.\n", produto.material, produto.preco);
        }
        
        avanca_iterador_arvorebp(&iterador);
    }
}