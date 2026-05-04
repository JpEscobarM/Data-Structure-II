#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bptree.h"

iterador_arvorebp iterador_vazio() {
    return (iterador_arvorebp){0};
}

indice_arvorebp* _indice_arvorebp_cria_node(int folha) {
    indice_arvorebp *node = calloc(1, sizeof(indice_arvorebp));
    node->folha = folha;

    return node;
}

indice_arvorebp* cria_indice_arvorebp() {
    return _indice_arvorebp_cria_node(1);
}

indice_arvorebp_node* busca_folha_indice_arvorebp(indice_arvorebp_node *raiz, chave_t chave) {
    int i;

    if (!raiz) {
        return NULL;
    }

    while (!raiz->folha) {
        for (i = 0; i < raiz->num_chaves && chave >= raiz->chaves[i]; i++);

        raiz = raiz->filhos[i];
    }

    return raiz;
}

valor_t busca_indice_arvorebp(indice_arvorebp* raiz, chave_t chave) {
    return valor_iterador_arvorebp(busca_muitos_indice_arvorebp(raiz, chave));
}

iterador_arvorebp busca_primeiro_maior_ou_igual(indice_arvorebp_node *raiz, chave_t chave_inicial) {
    int i;
    iterador_arvorebp it = iterador_vazio();
    if (!raiz) return it;

    indice_arvorebp_node *folha = busca_folha_indice_arvorebp(raiz, chave_inicial);
    if (!folha) return it;

    // achar o primeiro índice com chave >= chave_inicial
    for (i = 0; i < folha->num_chaves && folha->chaves[i] < chave_inicial; i++);

    if (i == folha->num_chaves) {
        // passa à próxima folha
        folha = folha->proximo;
        i = 0;

        if (!folha) return it;
    }

    it.folha = folha;
    it.i_atual = i;
    it.chave = folha->chaves[i];
    return it;
}

iterador_arvorebp busca_muitos_indice_arvorebp(indice_arvorebp_node* raiz, chave_t chave) {
    iterador_arvorebp iterador;
    if (!raiz) {
        return iterador_vazio();
    }

    indice_arvorebp_node *folha = busca_folha_indice_arvorebp(raiz, chave);
    
    if (!folha) {
        return iterador_vazio();
    }

    for (int i = 0; i < folha->num_chaves; i++) {
        if (folha->chaves[i] == chave) {
            iterador.i_atual = i;
            iterador.chave = chave;
            iterador.folha = folha;

            return iterador;
        }
    }

    return iterador_vazio();
}

void inserir_em_folha(indice_arvorebp_node *folha, chave_t chave, valor_t valor) {
    int i, j;

    for (i = 0; i < folha->num_chaves && folha->chaves[i] < chave; i++);
    
    if (i < folha->num_chaves && folha->chaves[i] == chave) {
        return;
    }

    for (j = folha->num_chaves; j > i; j--) {
        folha->chaves[j] = folha->chaves[j-1];
        folha->valores[j] = folha->valores[j-1];
    }

    folha->chaves[i] = chave;
    folha->valores[i] = valor;
    folha->num_chaves++;
}

void _indice_arvorebp_inserir(indice_arvorebp_node *no, chave_t chave, indice_arvorebp_node *filho_direito) {
    int i;

    for (i = 0; i < no->num_chaves && no->chaves[i] < chave; i++);
    
    for (int j = no->num_chaves; j > i; j--) {
        no->chaves[j] = no->chaves[j-1];
        no->filhos[j+1] = no->filhos[j];
    }

    no->chaves[i] = chave;
    no->filhos[i+1] = filho_direito;
    no->num_chaves++;
    
    if (filho_direito) {
        filho_direito->pai = no;
    }
}

indice_arvorebp_node* indice_arvorebp_split(indice_arvorebp_node *folha) {
    int i, j, corte = (MAX_CHAVES + 1) / 2;
    indice_arvorebp_node *nova_folha = _indice_arvorebp_cria_node(1);
    
    for (i = corte, j = 0; i < folha->num_chaves; i++, j++) {
        nova_folha->chaves[j] = folha->chaves[i];
        nova_folha->valores[j] = folha->valores[i];
        nova_folha->num_chaves++;
    }

    folha->num_chaves = corte;
    nova_folha->proximo = folha->proximo;

    folha->proximo = nova_folha;
    nova_folha->pai = folha->pai;

    return nova_folha;
}

indice_arvorebp_node* _indice_arvorebp_split(indice_arvorebp_node *no, chave_t *chave_promovida) {
    int i, j;
    int corte = (MAX_CHAVES + 1) / 2;
    chave_t chave_meio = no->chaves[corte];
    indice_arvorebp_node *novo = _indice_arvorebp_cria_node(0);

    *chave_promovida = chave_meio;

    for (i = corte + 1, j = 0; i < no->num_chaves; i++, j++) {
        novo->chaves[j] = no->chaves[i];
        novo->filhos[j] = no->filhos[i];
        
        if (novo->filhos[j]) {
            novo->filhos[j]->pai = novo;
        }

        novo->num_chaves++;
    }

    novo->filhos[j] = no->filhos[no->num_chaves];
    
    if (novo->filhos[j]) {
        novo->filhos[j]->pai = novo;
    }

    no->num_chaves = corte;
    novo->pai = no->pai;

    return novo;
}

indice_arvorebp *insere_indice_arvorebp(indice_arvorebp *raiz, chave_t chave, valor_t valor) {
    indice_arvorebp_node *folha, *nova_folha, *nova_raiz, *novo_interno, *pai, *n, *p;
    indice_arvorebp *r, *saida;

    chave_t nova_chave, promovida;
    
    if (!raiz) {
        r = _indice_arvorebp_cria_node(1);
        inserir_em_folha(r, chave, valor);
        return r;
    }

    folha = busca_folha_indice_arvorebp(raiz, chave);
    
    inserir_em_folha(folha, chave, valor);
    
    if (folha->num_chaves < MAX_CHAVES) {
        return raiz;
    }
    
    nova_folha = indice_arvorebp_split(folha);
    nova_chave = nova_folha->chaves[0];
    
    if (folha->pai == NULL) {
        nova_raiz = _indice_arvorebp_cria_node(0);
        nova_raiz->chaves[0] = nova_chave;
        nova_raiz->filhos[0] = folha;
        nova_raiz->filhos[1] = nova_folha;
        nova_raiz->num_chaves = 1;
        folha->pai = nova_raiz;
        nova_folha->pai = nova_raiz;
        return nova_raiz;
    }
    
    pai = folha->pai;

    _indice_arvorebp_inserir(pai, nova_chave, nova_folha);
    
    n       = pai;
    saida   = raiz;
    
    while (n) {
        if (n->num_chaves < MAX_CHAVES) break;
        promovida = 0;
        novo_interno = _indice_arvorebp_split(n, &promovida);
        if (n->pai == NULL) {
            nova_raiz = _indice_arvorebp_cria_node(0);
            nova_raiz->chaves[0] = promovida;
            nova_raiz->filhos[0] = n;
            nova_raiz->filhos[1] = novo_interno;
            nova_raiz->num_chaves = 1;
            n->pai = nova_raiz;
            novo_interno->pai = nova_raiz;
            saida = nova_raiz;
            break;
        } else {
            p = n->pai;
            _indice_arvorebp_inserir(p, promovida, novo_interno);
            n = p;
        }
    }

    return saida;
}

void exclui_indice_arvorebp(indice_arvorebp *raiz) {
    int i;

    if (!raiz) {
        return;
    }

    if (!raiz->folha) {
        for (i = 0; i <= raiz->num_chaves; i++) {
            exclui_indice_arvorebp(raiz->filhos[i]);
        }
    }

    free(raiz);
}

int indice_arvorebp_encontra_chave(indice_arvorebp_node *no, chave_t chave) {
    int i;
    for (i = 0; i < no->num_chaves && no->chaves[i] < chave; i++);
    return i;
}

void remove_indice_arvorebp(indice_arvorebp_node *folha, chave_t chave) {
    int i = indice_arvorebp_encontra_chave(folha, chave);

    if (i == folha->num_chaves || folha->chaves[i] != chave) return;

    for (; i < folha->num_chaves - 1; i++) {
        folha->chaves[i] = folha->chaves[i + 1];
        folha->valores[i] = folha->valores[i + 1];
    }
    folha->num_chaves--;
}

void funde_folhas_arvorebp(indice_arvorebp_node *esq, indice_arvorebp_node *dir, chave_t chave_pai) {
    for (int i = 0; i < dir->num_chaves; i++) {
        esq->chaves[esq->num_chaves + i] = dir->chaves[i];
        esq->valores[esq->num_chaves + i] = dir->valores[i];
    }
    esq->num_chaves += dir->num_chaves;
    esq->proximo = dir->proximo;
    free(dir);
}

void remove_interno_arvorebp(indice_arvorebp_node *no, int indice) {
    for (int i = indice; i < no->num_chaves - 1; i++) {
        no->chaves[i] = no->chaves[i + 1];
        no->filhos[i + 1] = no->filhos[i + 2];
    }
    no->num_chaves--;
}

indice_arvorebp* rebalancea_arvorebp(indice_arvorebp *raiz, indice_arvorebp_node *no) {
    if (no == NULL) return raiz;

    if (no->num_chaves >= MIN_CHAVES || no->pai == NULL)
        return raiz;

    indice_arvorebp_node *pai = no->pai;
    int i;
    for (i = 0; i <= pai->num_chaves; i++)
        if (pai->filhos[i] == no) break;

    indice_arvorebp_node *esq = (i > 0) ? pai->filhos[i - 1] : NULL;
    indice_arvorebp_node *dir = (i < pai->num_chaves) ? pai->filhos[i + 1] : NULL;

    if (esq && esq->num_chaves > MIN_CHAVES) {
        // Pega emprestado da esquerda
        for (int j = no->num_chaves; j > 0; j--) {
            no->chaves[j] = no->chaves[j - 1];
            no->valores[j] = no->valores[j - 1];
        }
        no->chaves[0] = esq->chaves[esq->num_chaves - 1];
        no->valores[0] = esq->valores[esq->num_chaves - 1];
        esq->num_chaves--;
        pai->chaves[i - 1] = no->chaves[0];
        no->num_chaves++;
    }
    else if (dir && dir->num_chaves > MIN_CHAVES) {
        // Pega emprestado da direita
        no->chaves[no->num_chaves] = dir->chaves[0];
        no->valores[no->num_chaves] = dir->valores[0];
        no->num_chaves++;
        for (int j = 0; j < dir->num_chaves - 1; j++) {
            dir->chaves[j] = dir->chaves[j + 1];
            dir->valores[j] = dir->valores[j + 1];
        }
        dir->num_chaves--;
        pai->chaves[i] = dir->chaves[0];
    }
    else if (esq) {
        funde_folhas_arvorebp(esq, no, pai->chaves[i - 1]);
        remove_interno_arvorebp(pai, i - 1);
        free(no);
        raiz = rebalancea_arvorebp(raiz, pai);
    }
    else if (dir) {
        funde_folhas_arvorebp(no, dir, pai->chaves[i]);
        remove_interno_arvorebp(pai, i);
        free(dir);
        raiz = rebalancea_arvorebp(raiz, pai);
    }

    if (pai == raiz && pai->num_chaves == 0) {
        raiz = pai->filhos[0];
        raiz->pai = NULL;
        free(pai);
    }

    return raiz;
}

indice_arvorebp* remover_indice_arvorebp(indice_arvorebp *raiz, chave_t chave) {
    if (!raiz) return NULL;

    indice_arvorebp_node *folha = busca_folha_indice_arvorebp(raiz, chave);
    if (!folha) return raiz;

    remove_indice_arvorebp(folha, chave);
    raiz = rebalancea_arvorebp(raiz, folha);
    return raiz;
}

int possui_valor_iterador_arvorebp(iterador_arvorebp iterador) {
    if (!iterador.folha) {
        return 0;
    }

    return iterador.chave == iterador.folha->chaves[iterador.i_atual];
}

int intervalo_valido_iterador_arvorebp(iterador_arvorebp iterador, chave_t chave_final) {
    if (!iterador.folha) return 0;
    if (iterador.i_atual >= iterador.folha->num_chaves) return 0;

    return iterador.folha->chaves[iterador.i_atual] <= chave_final;
}

void avanca_iterador_arvorebp(iterador_arvorebp *iterador) {
    if (!iterador->folha) return;

    iterador->i_atual++;

    if (iterador->i_atual >= iterador->folha->num_chaves) {
        iterador->folha = iterador->folha->proximo;
        iterador->i_atual = 0;
    }

    if (iterador->folha) {
        iterador->chave = iterador->folha->chaves[iterador->i_atual];
    }
}


chave_t valor_iterador_arvorebp(iterador_arvorebp iterador) {
    if (!iterador.folha) {
        return NOT_FOUND;
    }

    return iterador.folha->valores[iterador.i_atual];
}

// exemplo de utilização

// int main(void) {
//     indice_arvorebp *raiz = NULL;

//     int valores[] = {20, 5, 15, 25, 30, 2, 8, 12, 27, 26, 18, 19};
    
//     int n = sizeof(valores)/sizeof(valores[0]);
    
//     printf("Construindo Árvore B+ (ORDEM=%d, MAX_CHAVES=%d)\n", ORDEM, MAX_CHAVES);
    
//     for (int i=0;i<n;i++) {
//         raiz = indice_arvorebp_inserir(raiz, valores[i], valores[i]*10);
//         printf("Após inserir %d:\n", valores[i]);
//         imprimir_arvore(raiz);
//         printf("---------------------------\n");
//     }

//     int buscas[] = {15, 27, 100};
//     for (int i=0;i<3;i++) {
//         int valor = indice_arvorebp_buscar(raiz, buscas[i]);
//         printf("Chave %d -> valor: %d\n", buscas[i], valor);
//     }

//     indice_arvorebp_limpar(raiz);
//     return 0;
// }
