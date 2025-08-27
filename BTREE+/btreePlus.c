#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define D 2
#define MAX_CHAVES     (2 * D)
#define MAX_FILHOS     (MAX_CHAVES + 1)
#define CAP_CHAVES     (MAX_CHAVES + 1)
#define CAP_FILHOS     (MAX_FILHOS + 1)

typedef struct No {
    bool e_folha;
    int  n;
    int  chaves[CAP_CHAVES];
    struct No *filhos[CAP_FILHOS];
    struct No *prox;
    struct No *pai;
} No;

typedef struct {
    No *raiz;
    No *primeira_folha;
} ArvoreBMais;

static No *novo_no(bool e_folha) {
    No *n = (No *)calloc(1, sizeof(No));
    n->e_folha = e_folha;
    return n;
}

static ArvoreBMais *abp_criar(void) {
    ArvoreBMais *t = (ArvoreBMais *)calloc(1, sizeof(ArvoreBMais));
    t->raiz = novo_no(true);
    t->primeira_folha = t->raiz;
    return t;
}

static void abp_liberar_no(No *n) {
    if (!n) return;
    if (!n->e_folha) {
        for (int i = 0; i <= n->n; ++i) abp_liberar_no(n->filhos[i]);
    }
    free(n);
}

static void abp_destruir(ArvoreBMais *t) {
    if (!t) return;
    abp_liberar_no(t->raiz);
    free(t);
}

static int busca_posicao(const int *chaves, int n, int k) {
    int lo = 0, hi = n;
    while (lo < hi) {
        int mid = (lo + hi) >> 1;
        if (chaves[mid] < k) lo = mid + 1; else hi = mid;
    }
    return lo;
}

static No *encontrar_folha(No *raiz, int chave) {
    No *p = raiz;
    while (p && !p->e_folha) {
        int i = busca_posicao(p->chaves, p->n, chave);
        if (i < p->n && chave >= p->chaves[i]) i++;
        p = p->filhos[i];
    }
    return p;
}

static bool abp_buscar(ArvoreBMais *t, int chave, int *out_valor) {
    No *folha = encontrar_folha(t->raiz, chave);
    if (!folha) return false;
    int i = busca_posicao(folha->chaves, folha->n, chave);
    if (i < folha->n && folha->chaves[i] == chave) {
        if (out_valor) *out_valor = folha->chaves[i];
        return true;
    }
    return false;
}

static void inserir_no_pai(ArvoreBMais *t, No *esq, int chave_sep, No *dir);

static void inserir_na_folha_sem_dividir(No *folha, int chave) {
    int i = busca_posicao(folha->chaves, folha->n, chave);
    if (i < folha->n && folha->chaves[i] == chave) return;
    for (int j = folha->n; j > i; --j) folha->chaves[j] = folha->chaves[j-1];
    folha->chaves[i] = chave;
    folha->n++;
}

static void dividir_folha_e_inserir(ArvoreBMais *t, No *folha, int chave) {
    int temp_chaves[CAP_CHAVES];
    int i = 0, j = 0;
    while (i < folha->n && folha->chaves[i] < chave) temp_chaves[j++] = folha->chaves[i++];
    temp_chaves[j++] = chave;
    while (i < folha->n) temp_chaves[j++] = folha->chaves[i++];

    No *nova_folha = novo_no(true);

    int qtd_esq  = D;
    int qtd_dir  = CAP_CHAVES - qtd_esq;

    folha->n = qtd_esq;
    for (i = 0; i < qtd_esq; ++i) folha->chaves[i] = temp_chaves[i];

    nova_folha->n = qtd_dir;
    for (i = 0, j = qtd_esq; i < qtd_dir; ++i, ++j) nova_folha->chaves[i] = temp_chaves[j];

    nova_folha->prox = folha->prox;
    folha->prox = nova_folha;
    nova_folha->pai = folha->pai;

    int separador = nova_folha->chaves[0];
    inserir_no_pai(t, folha, separador, nova_folha);

    if (t->primeira_folha == folha && chave < separador) t->primeira_folha = folha;
}

static void inserir_no_pai(ArvoreBMais *t, No *esq, int chave_sep, No *dir) {
    if (esq->pai == NULL) {
        No *raiz = novo_no(false);
        raiz->chaves[0] = chave_sep;
        raiz->filhos[0] = esq;
        raiz->filhos[1] = dir;
        raiz->n = 1;
        esq->pai = dir->pai = raiz;
        t->raiz = raiz;
        return;
    }

    No *pai = esq->pai;
    int idx = 0;
    while (idx <= pai->n && pai->filhos[idx] != esq) idx++;

    for (int j = pai->n; j > idx; --j) pai->chaves[j] = pai->chaves[j-1];
    for (int j = pai->n + 1; j > idx + 1; --j) pai->filhos[j] = pai->filhos[j-1];

    pai->chaves[idx] = chave_sep;
    pai->filhos[idx + 1] = dir;
    dir->pai = pai;
    pai->n++;

    if (pai->n <= MAX_CHAVES) return;

    int temp_chaves[CAP_CHAVES];
    No *temp_filhos[CAP_FILHOS];
    for (int i = 0; i < pai->n; ++i) temp_chaves[i] = pai->chaves[i];
    for (int i = 0; i <= pai->n; ++i) temp_filhos[i] = pai->filhos[i];

    int meio = D;
    int promover = temp_chaves[meio];
    No *no_direito = novo_no(false);

    pai->n = meio;
    for (int i = 0; i < meio; ++i) pai->chaves[i] = temp_chaves[i];
    for (int i = 0; i <= meio; ++i) pai->filhos[i] = temp_filhos[i];

    int r_keys = MAX_CHAVES - meio;
    no_direito->n = r_keys;
    for (int i = 0; i < r_keys; ++i) no_direito->chaves[i] = temp_chaves[meio + 1 + i];
    for (int i = 0; i <= r_keys; ++i) {
        no_direito->filhos[i] = temp_filhos[meio + 1 + i];
        if (no_direito->filhos[i]) no_direito->filhos[i]->pai = no_direito;
    }

    inserir_no_pai(t, pai, promover, no_direito);
}

void abp_inserir(ArvoreBMais *t, int chave) {
    No *folha = encontrar_folha(t->raiz, chave);
    if (folha->n < MAX_CHAVES) {
        inserir_na_folha_sem_dividir(folha, chave);
    } else {
        dividir_folha_e_inserir(t, folha, chave);
    }
}

static void abp_imprimir_folhas(ArvoreBMais *t) {
    No *p = t->primeira_folha;
    while (p) {
        printf("[");
        for (int i = 0; i < p->n; ++i) {
            printf("%d%s", p->chaves[i], (i+1<p->n?", ":""));
        }
        printf("] ");
        p = p->prox;
    }
    printf("\n");
}

static void abp_intervalo(ArvoreBMais *t, int lo, int hi) {
    No *folha = encontrar_folha(t->raiz, lo);
    while (folha && (folha->n == 0 || folha->chaves[folha->n-1] < lo)) folha = folha->prox;
    while (folha) {
        for (int i = 0; i < folha->n; ++i) {
            int k = folha->chaves[i];
            if (k > hi) return;
            if (k >= lo) printf("%d ", k);
        }
        folha = folha->prox;
    }
}

void abp_imprimir_arvore(ArvoreBMais *t) {
    if (!t->raiz) return;

    No *fila[1000];
    int front = 0, back = 0;
    fila[back++] = t->raiz;

    while (front < back) {
        int nivel = back - front;
        for (int i = 0; i < nivel; i++) {
            No *n = fila[front++];
            printf("%s(", n->e_folha ? "Folha" : "Interno");
            for (int j = 0; j < n->n; j++) {
                printf("%d%s", n->chaves[j], (j+1<n->n?", ":""));
            }
            printf(")  ");
            if (!n->e_folha) {
                for (int j = 0; j <= n->n; j++) {
                    if (n->filhos[j]) fila[back++] = n->filhos[j];
                }
            }
        }
        printf("\n");
    }
}

int main(void) {
    ArvoreBMais *t = abp_criar();

    int dados[] = {1,2,4,9,15,20,30,40,46,47,51,52,56,60,65,70,80,83};
    int nd = (int)(sizeof(dados)/sizeof(dados[0]));

    for (int i = 0; i < nd; ++i) abp_inserir(t, dados[i]);

    printf("Estrutura da árvore:\n");
    abp_imprimir_arvore(t);

    printf("Folhas em ordem:\n");
    abp_imprimir_folhas(t);

    int v;
    if (abp_buscar(t, 60, &v)) printf("Buscar 60: encontrado %d\n", v);
    else printf("Buscar 60: não encontrado\n");

    printf("Intervalo [30..60]: ");
    abp_intervalo(t, 30, 60);
    printf("\n");

    abp_destruir(t);
    return 0;
}
