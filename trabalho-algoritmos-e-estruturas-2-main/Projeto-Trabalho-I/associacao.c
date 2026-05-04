#include "associacao.h"
#include "arquivo_indice.h"

#include <stdlib.h>

indice_arvorebp* indice_associacao;

void carrega_indice_associacao(FILE *arquivo) {
    Associacao assoc;
    chave_t chave;
    valor_t valor;

    exclui_indice_arvorebp(indice_associacao);

    indice_associacao = cria_indice_arvorebp();

    while (fread(&assoc, sizeof(Associacao), 1, arquivo)) {
        chave = str_para_long(assoc.id_pedido);
        valor = str_para_long(assoc.id_produto);

        insere_indice_arvorebp(indice_associacao, chave, valor);
    }
}

void salva_indice_associacao_recur(FILE *arquivo, indice_arvorebp_node *arvore) {
    int i;

    if (!arvore) {
        return;
    }

    for (i = 0; i <= arvore->num_chaves; i++) {
        if (!arvore->folha) {
            salva_indice_arvorebp(arquivo, arvore->filhos[i]);
        }

        if (i < arvore->num_chaves) {
            fprintf(arquivo, "%19ld\0", arvore->chaves[i]);
            fprintf(arquivo, "%19ld\0", arvore->valores[i]);
        }
    }
}

void salva_indice_associacao(FILE *arquivo) {
    salva_indice_associacao_recur(arquivo, indice_associacao);
}

lista_produtos* insere_lista(id_produto produto, lista_produtos* proximo) {
    lista_produtos *elem = (lista_produtos*) malloc(sizeof(lista_produtos));

    elem->id_produto = produto;
    elem->proximo = proximo;

    return elem;
}

void limpa_lista_produtos(lista_produtos* lista) {
    if (!lista) {
        return;
    }

    limpa_lista_produtos(lista->proximo);
    free(lista);
}

void insere_associacao_no_indice(Associacao associacao) {
    chave_t chave = str_para_long(associacao.id_pedido);
    valor_t valor = str_para_long(associacao.id_produto);

    indice_associacao = insere_indice_arvorebp(indice_associacao, chave, valor);
}

lista_produtos* produtos_associados_a_pedido(id_pedido pedido) {
    iterador_arvorebp iterador;
    lista_produtos *lista = NULL;

    iterador = busca_muitos_indice_arvorebp(indice_associacao, pedido);

    while (possui_valor_iterador_arvorebp(iterador)) {
        lista = insere_lista(valor_iterador_arvorebp(iterador), lista);
        avanca_iterador_arvorebp(&iterador);
    }

    return lista;
}