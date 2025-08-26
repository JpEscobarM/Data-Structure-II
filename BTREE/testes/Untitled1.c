
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define ORDEM 1              // << troque aqui se quiser (1 funciona, mas é bem limitado)
#define NUM_CHAVES (2*ORDEM)    // máx. de chaves por nó (pela sua convenção)
#define NUM_FILHOS (NUM_CHAVES+1)
#define VAZIO      (-99999)

typedef struct no {
    int   qtdChaves;    // chaves em uso (0..NUM_CHAVES). ATENÇÃO: alocamos +1 para overflow temporário
    int  *chaves;       // tamanho alocado: NUM_CHAVES+1
    struct no *pai;
    struct no **filhos; // tamanho: NUM_FILHOS
    bool  folha;
} No;

/* ---------- util ---------- */

// índice da "mediana" conforme sua regra:
// par -> tam/2 - 1  (ex.: 4 -> 1)
// ímpar -> (tam-1)/2 (ex.: 5 -> 2)
 int separa_idx(int tam) {
    return (tam % 2 == 0) ? (tam/2 - 1) : ((tam - 1)/2);
}

// posição para descer (primeiro índice i com chaves[i] >= k)
int lower_bound(const int *v, int n, int k) {
    int i = 0;
    while (i < n && v[i] < k) i++;
    return i;
}

/* ---------- criação / destruição ---------- */

No *criaNo(void) {
    No *n = (No*)malloc(sizeof(No));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    n->qtdChaves = 0;
    n->pai = NULL;
    n->folha = true;                          // todo nó nasce folha
    n->chaves = (int*)malloc(sizeof(int)*(NUM_CHAVES+1)); // +1 p/ overflow temporário
    n->filhos = (No**) malloc(sizeof(No*)*NUM_FILHOS);
    if (!n->chaves || !n->filhos) { perror("malloc"); exit(EXIT_FAILURE); }
    for (int i = 0; i < NUM_FILHOS; ++i) n->filhos[i] = NULL;
    return n;
}

void destruir_rec(No *x) {
    if (!x) return;
    if (!x->folha) {
        for (int i = 0; i <= x->qtdChaves; ++i) destruir_rec(x->filhos[i]);
    }
    free(x->chaves);
    free(x->filhos);
    free(x);
}

/* ---------- busca ---------- */

No *busca(No *x, int k) {
    if (!x) return NULL;
    int i = lower_bound(x->chaves, x->qtdChaves, k);
    if (i < x->qtdChaves && x->chaves[i] == k) return x;      // achou no próprio nó
    if (x->folha) return x;                                   // folha: retorna onde deveria estar
    return busca(x->filhos[i], k);
}

/* ---------- split (divide um nó em dois irmãos) ---------- */
/* Retorna a mediana que deve "subir" para o pai.
   - 'no' (esquerda) mantém as chaves menores que a mediana.
   - 'novoIrmao' (direita) recebe as maiores.
   - filhos também são redistribuídos. */
int split(No *no, No *novoIrmao) {
    if (!no || !novoIrmao) return VAZIO;

    int n = no->qtdChaves;      // número atual de chaves (pode estar com overflow NUM_CHAVES+1, mas nossa inserção evita)
    int m = separa_idx(n);      // índice da mediana
    int valorMeio = no->chaves[m];

    // novo irmão herda tipo/pai
    novoIrmao->folha = no->folha;
    novoIrmao->pai   = no->pai;

    // ---- mover chaves da direita para o irmão ----
    int rightCount = n - (m + 1); // quantas chaves vão para a direita
    for (int j = 0; j < rightCount; ++j)
        novoIrmao->chaves[j] = no->chaves[m + 1 + j];
    novoIrmao->qtdChaves = rightCount;

    // ---- mover filhos (se interno): índices (m+1)..n  => total rightCount+1 ----
    if (!no->folha) {
        for (int j = 0; j <= rightCount; ++j) {
            novoIrmao->filhos[j] = no->filhos[m + 1 + j];
            no->filhos[m + 1 + j] = NULL;
            if (novoIrmao->filhos[j]) novoIrmao->filhos[j]->pai = novoIrmao;
        }
    }

    // ---- ajustar nó da esquerda ----
    no->qtdChaves = m;           // fica com as menores (0..m-1). A mediana "sobe"

    return valorMeio;
}

/* ---------- inserir (top-down, estilo CLRS) ---------- */

// divide o filho 'idx' do 'pai' se ele estiver cheio
static void split_child(No *pai, int idx) {
    No *y = pai->filhos[idx];
    No *z = criaNo();

    // y é dividido em y (esquerda) + z (direita)
    int med = split(y, z);

    // abre espaço no pai para o novo filho z
    for (int j = pai->qtdChaves; j >= idx + 1; --j)
        pai->filhos[j + 1] = pai->filhos[j];
    pai->filhos[idx + 1] = z;
    z->pai = pai;

    // abre espaço no pai para a mediana
    for (int j = pai->qtdChaves - 1; j >= idx; --j)
        pai->chaves[j + 1] = pai->chaves[j];

    pai->chaves[idx] = med;
    pai->qtdChaves++;
    pai->folha = false;
}

// insere em nó não-cheio (garantido pelo chamador)
static void inserir_nao_cheio(No *x, int k) {
    // evita duplicata rápida (opcional)
    int lb = lower_bound(x->chaves, x->qtdChaves, k);
    if (lb < x->qtdChaves && x->chaves[lb] == k) return; // ignora duplicata

    if (x->folha) {
        // abre espaço e insere
        int i = x->qtdChaves - 1;
        while (i >= 0 && x->chaves[i] > k) {
            x->chaves[i + 1] = x->chaves[i];
            --i;
        }
        x->chaves[i + 1] = k;
        x->qtdChaves++;
    } else {
        int i = lower_bound(x->chaves, x->qtdChaves, k);
        // se o filho onde vamos descer está cheio, divide antes de descer
        if (x->filhos[i]->qtdChaves == NUM_CHAVES) {
            split_child(x, i);
            // depois da divisão, decide qual dos dois filhos descer
            if (k > x->chaves[i]) i++;
            else if (k == x->chaves[i]) return; // duplicata
        }
        inserir_nao_cheio(x->filhos[i], k);
    }
}

// API de inserção
void inserir(No **praiz, int k) {
    if (!*praiz) {
        *praiz = criaNo();
        (*praiz)->chaves[0] = k;
        (*praiz)->qtdChaves = 1;
        return;
    }

    No *r = *praiz;

    // se a raiz está cheia, cria nova raiz e divide
    if (r->qtdChaves == NUM_CHAVES) {
        No *s = criaNo();
        s->folha = false;
        s->filhos[0] = r;
        r->pai = s;
        *praiz = s;

        split_child(s, 0);

        int i = (k > s->chaves[0]) ? 1 : 0;
        if (k == s->chaves[0]) return; // duplicata
        inserir_nao_cheio(s->filhos[i], k);
    } else {
        inserir_nao_cheio(r, k);
    }
}

/* ---------- travessia / impressão ---------- */

void em_ordem_rec(No *x) {
    if (!x) return;
    int i;
    for (i = 0; i < x->qtdChaves; ++i) {
        if (!x->folha) em_ordem_rec(x->filhos[i]);
        printf("%d ", x->chaves[i]);
    }
    if (!x->folha) em_ordem_rec(x->filhos[i]);
}

void em_ordem(No *raiz) {
    if (!raiz) { puts("(vazia)"); return; }
    em_ordem_rec(raiz);
    putchar('\n');
}

void imprime_rec(No *x, int prof) {
    if (!x) return;
    if (!x->folha) imprime_rec(x->filhos[x->qtdChaves], prof + 1);
    for (int d = 0; d < prof; ++d) printf("    ");
    printf("[");
    for (int i = 0; i < x->qtdChaves; ++i) {
        printf("%d", x->chaves[i]);
        if (i + 1 < x->qtdChaves) printf(" ");
    }
    printf("]\n");
    for (int i = x->qtdChaves - 1; i >= 0; --i)
        if (!x->folha) imprime_rec(x->filhos[i], prof + 1);
}

void imprime(No *raiz) {
    if (!raiz) { puts("(vazia)"); return; }
    imprime_rec(raiz, 0);
}

/* ---------- demo ---------- */

int main(void) {
    No *raiz = NULL;

    // teste simples: provoca splits
    int v[] = {1,2,3,4,5,6,7,8,9,10,-1,-2};
    int n = 12;

    for (int i = 0; i < n; ++i) inserir(&raiz, v[i]);

    puts("Em ordem:");
    em_ordem(raiz);

    puts("\nArvore (formatada):");
    imprime(raiz);

    destruir_rec(raiz);
    return 0;
}
