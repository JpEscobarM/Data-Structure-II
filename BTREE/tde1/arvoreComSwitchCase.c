/*
 * �rvore B (B-Tree) em C � vers�o est�vel (CLRS)
 * - Grau m�nimo t (t >= 2).
 * - M�x. chaves por n�: 2*t - 1; m�n. (n�o-raiz): t - 1.
 * - Opera��es: criar, buscar, inserir, remover, travessia, imprimir, destruir.
 * - Chaves int, sem duplicatas (duplicata � ignorada).
 *
 * Compilar:  gcc -std=c99 -O2 -Wall -Wextra -pedantic btree_ptbr.c -o btree
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

/* ------------ Estruturas ------------ */

typedef struct NoB {
    int  *chaves;           // tamanho: 2*t - 1
    int   n;                // n� de chaves em uso
    bool  folha;
    struct NoB **filhos;    // tamanho: 2*t
    int   t;                // grau m�nimo
} NoB;

typedef struct {
    NoB *raiz;
    int  t;
} ArvoreB;

/* ------------ Prototipos (API) ------------ */
ArvoreB *ab_criar(int t);
void     ab_destruir(ArvoreB *T);
NoB     *ab_buscar(ArvoreB *T, int k, int *pos_out);
void     ab_inserir(ArvoreB *T, int k);
void     ab_remover(ArvoreB *T, int k);
void     ab_em_ordem(ArvoreB *T);
void     ab_imprimir(ArvoreB *T);

/* ------------ Internos ------------ */
static NoB *no_criar(int t, bool folha);
static void no_destruir(NoB *x);

static NoB *busca_no(NoB *x, int k, int *pos_out);

static void split_filho(NoB *x, int i, NoB *y);
static void inserir_nao_cheio(NoB *x, int k);

static int  encontrar_posicao(NoB *x, int k);
static void remover_chave_no(ArvoreB *T, NoB *x, int k);
static void remover_em_folha(NoB *x, int idx);
static void remover_em_interno(NoB *x, int idx);
static int  predecessor(NoB *x, int idx);
static int  sucessor(NoB *x, int idx);
static void completar_filho(NoB *x, int idx);
static void emprestar_do_anterior(NoB *x, int idx);
static void emprestar_do_proximo(NoB *x, int idx);
static void fundir_filhos(NoB *x, int idx);

static void percorre_em_ordem(NoB *x);
static void imprime_no(NoB *x, int prof);

/* ------------ Implementa��o ------------ */

static NoB *no_criar(int t, bool folha) {
    NoB *x = (NoB*)malloc(sizeof(NoB));
    if (!x) { perror("malloc"); exit(EXIT_FAILURE); }
    x->t = t;
    x->n = 0;
    x->folha = folha;
    x->chaves = (int*)malloc(sizeof(int) * (2*t - 1));
    x->filhos = (NoB**)malloc(sizeof(NoB*) * (2*t));
    if (!x->chaves || !x->filhos) { perror("malloc"); exit(EXIT_FAILURE); }
    for (int i = 0; i < 2*t; ++i) x->filhos[i] = NULL;
    return x;
}

ArvoreB *ab_criar(int t) {
    if (t < 2) { fprintf(stderr, "t deve ser >= 2\n"); exit(EXIT_FAILURE); }
    ArvoreB *T = (ArvoreB*)malloc(sizeof(ArvoreB));
    if (!T) { perror("malloc"); exit(EXIT_FAILURE); }
    T->t = t;
    T->raiz = no_criar(t, true);
    return T;
}

static void no_destruir(NoB *x) {
    if (!x) return;
    if (!x->folha) {
        for (int i = 0; i <= x->n; ++i) no_destruir(x->filhos[i]);
    }
    free(x->chaves);
    free(x->filhos);
    free(x);
}

void ab_destruir(ArvoreB *T) {
    if (!T) return;
    no_destruir(T->raiz);
    free(T);
}

/* ---- Busca ---- */

static NoB *busca_no(NoB *x, int k, int *pos_out) {
    int i = 0;

    // 1) Avança i até achar a 1ª posição cujo valor é >= k
    //    (equivalente a um "lower_bound")
    while (i < x->n && k > x->chaves[i]) i++;

    // 2) Se a chave está no nó atual, achamos!
    if (i < x->n && k == x->chaves[i]) {
        if (pos_out) *pos_out = i;  // devolve o índice no nó
        return x;                   // devolve o nó onde k está
    }

    // 3) Se é folha e não achou, não existe na árvore
    if (x->folha) return NULL;

    // 4) Caso contrário, desce para o filho "correto" e continua a busca
    return busca_no(x->filhos[i], k, pos_out);
}
NoB *ab_buscar(ArvoreB *T, int k, int *pos_out) {
    return (T && T->raiz) ? busca_no(T->raiz, k, pos_out) : NULL;
}

/* ---- Split (filho y de x na posi��o i) ----
   y tem 2*t - 1 chaves (cheio).
   z recebe as t-1 maiores de y; mediana (t-1) sobe para x. */
static void split_filho(NoB *x, int i, NoB *y) {
    int t = y->t;
    assert(y->n == 2*t - 1);

    NoB *z = no_criar(t, y->folha);
    z->n = t - 1;

    // Copia chaves (t-1 �ltimas) de y para z
    for (int j = 0; j < t - 1; ++j)
        z->chaves[j] = y->chaves[j + t];

    // Copia filhos (t �ltimos) se n�o for folha
    if (!y->folha) {
        for (int j = 0; j < t; ++j)
            z->filhos[j] = y->filhos[j + t];
    }

    y->n = t - 1; // y fica com t-1 chaves (as menores)

    // Move filhos de x para abrir espa�o
    for (int j = x->n; j >= i + 1; --j)
        x->filhos[j + 1] = x->filhos[j];
    x->filhos[i + 1] = z;

    // Move chaves de x para abrir espa�o � mediana
    for (int j = x->n - 1; j >= i; --j)
        x->chaves[j + 1] = x->chaves[j];

    // Sobe a mediana de y (posi��o t-1)
    x->chaves[i] = y->chaves[t - 1];
    x->n += 1;
}

/* ---- Inser��o ---- */
static void inserir_nao_cheio(NoB *x, int k) {
    int i = x->n - 1;

    if (x->folha) {
        while (i >= 0 && x->chaves[i] > k) {
            x->chaves[i + 1] = x->chaves[i];
            i--;
        }
        if (i >= 0 && x->chaves[i] == k) return; // sem duplicata
        x->chaves[i + 1] = k;
        x->n += 1;
    } else {
        while (i >= 0 && x->chaves[i] > k) i--;
        i++;
        if ((i < x->n && x->chaves[i] == k) || (i-1 >= 0 && x->chaves[i-1] == k))
            return;

        if (x->filhos[i]->n == 2*x->t - 1) {
            split_filho(x, i, x->filhos[i]);
            if (k > x->chaves[i]) i++;
            else if (k == x->chaves[i]) return;
        }
        inserir_nao_cheio(x->filhos[i], k);
    }
}

void ab_inserir(ArvoreB *T, int k) {
    NoB *r = T->raiz;
    if (r->n == 2*T->t - 1) {
        NoB *s = no_criar(T->t, false);
        T->raiz = s;
        s->filhos[0] = r;
        split_filho(s, 0, r);
        int i = (k > s->chaves[0]) ? 1 : 0;
        if (k == s->chaves[0]) return;
        inserir_nao_cheio(s->filhos[i], k);
    } else {
        inserir_nao_cheio(r, k);
    }
}

/* ---- Remo��o ---- */
static int encontrar_posicao(NoB *x, int k) {
    int idx = 0;
    while (idx < x->n && x->chaves[idx] < k) idx++;
    return idx;
}

static void remover_em_folha(NoB *x, int idx) {
    for (int i = idx + 1; i < x->n; ++i)
        x->chaves[i - 1] = x->chaves[i];
    x->n -= 1;
}

static int predecessor(NoB *x, int idx) {
    NoB *cur = x->filhos[idx];
    while (!cur->folha) cur = cur->filhos[cur->n];
    return cur->chaves[cur->n - 1];
}

static int sucessor(NoB *x, int idx) {
    NoB *cur = x->filhos[idx + 1];
    while (!cur->folha) cur = cur->filhos[0];
    return cur->chaves[0];
}

static void fundir_filhos(NoB *x, int idx) {
    int t = x->t;
    NoB *esq = x->filhos[idx];
    NoB *dir = x->filhos[idx + 1];

    // puxa chave separadora de x para o meio de esq
    esq->chaves[t - 1] = x->chaves[idx];

    // copia chaves de dir
    for (int j = 0; j < dir->n; ++j)
        esq->chaves[t + j] = dir->chaves[j];

    // copia filhos de dir
    if (!esq->folha) {
        for (int j = 0; j <= dir->n; ++j)
            esq->filhos[t + j] = dir->filhos[j];
    }

    esq->n += dir->n + 1;

    // remove chave/ponteiro de x
    for (int j = idx + 1; j < x->n; ++j)
        x->chaves[j - 1] = x->chaves[j];
    for (int j = idx + 2; j <= x->n; ++j)
        x->filhos[j - 1] = x->filhos[j];

    x->n -= 1;

    free(dir->chaves);
    free(dir->filhos);
    free(dir);
}

static void emprestar_do_anterior(NoB *x, int idx) {
    NoB *filho = x->filhos[idx];
    NoB *irm = x->filhos[idx - 1];

    // desloca filho p/ direita
    for (int i = filho->n - 1; i >= 0; --i)
        filho->chaves[i + 1] = filho->chaves[i];
    if (!filho->folha) {
        for (int i = filho->n + 1; i >= 0; --i)
            filho->filhos[i + 1] = filho->filhos[i];
    }

    // traz chave do pai
    filho->chaves[0] = x->chaves[idx - 1];
    if (!filho->folha)
        filho->filhos[0] = irm->filhos[irm->n];

    // move �ltima chave do irm�o para o pai
    x->chaves[idx - 1] = irm->chaves[irm->n - 1];

    filho->n += 1;
    irm->n -= 1;
}

static void emprestar_do_proximo(NoB *x, int idx) {
    NoB *filho = x->filhos[idx];
    NoB *irm = x->filhos[idx + 1];

    filho->chaves[filho->n] = x->chaves[idx];
    if (!filho->folha)
        filho->filhos[filho->n + 1] = irm->filhos[0];

    x->chaves[idx] = irm->chaves[0];

    for (int i = 1; i < irm->n; ++i)
        irm->chaves[i - 1] = irm->chaves[i];
    if (!irm->folha) {
        for (int i = 1; i <= irm->n; ++i)
            irm->filhos[i - 1] = irm->filhos[i];
    }

    filho->n += 1;
    irm->n -= 1;
}

static void completar_filho(NoB *x, int idx) {
    int t = x->t;
    if (idx != 0 && x->filhos[idx - 1]->n >= t) {
        emprestar_do_anterior(x, idx);
    } else if (idx != x->n && x->filhos[idx + 1]->n >= t) {
        emprestar_do_proximo(x, idx);
    } else {
        if (idx != x->n) fundir_filhos(x, idx);
        else             fundir_filhos(x, idx - 1);
    }
}

static void remover_em_interno(NoB *x, int idx) {
    int k = x->chaves[idx];
    int t = x->t;

    if (x->filhos[idx]->n >= t) {
        int pred = predecessor(x, idx);
        x->chaves[idx] = pred;
        remover_chave_no(NULL, x->filhos[idx], pred);
    } else if (x->filhos[idx + 1]->n >= t) {
        int succ = sucessor(x, idx);
        x->chaves[idx] = succ;
        remover_chave_no(NULL, x->filhos[idx + 1], succ);
    } else {
        fundir_filhos(x, idx);
        remover_chave_no(NULL, x->filhos[idx], k);
    }
}

static void remover_chave_no(ArvoreB *T, NoB *x, int k) {
    int idx = encontrar_posicao(x, k);

    if (idx < x->n && x->chaves[idx] == k) {
        if (x->folha) remover_em_folha(x, idx);
        else          remover_em_interno(x, idx);
    } else {
        if (x->folha) return; // n�o encontrado

        bool ultimo = (idx == x->n);
        if (x->filhos[idx]->n == x->t - 1)
            completar_filho(x, idx);

        if (ultimo && idx > x->n)
            remover_chave_no(T, x->filhos[idx - 1], k);
        else
            remover_chave_no(T, x->filhos[idx], k);
    }
}

void ab_remover(ArvoreB *T, int k) {
    if (!T || !T->raiz) return;
    NoB *r = T->raiz;

    remover_chave_no(T, r, k);

    if (r->n == 0) {
        NoB *antiga = r;
        if (r->folha) {
            T->raiz = NULL; // ficou vazia
            no_destruir(antiga);
        } else {
            T->raiz = r->filhos[0]; // reduz altura
            antiga->filhos[0] = NULL;
            no_destruir(antiga);
        }
    }
}

/* ---- Visualiza��o ---- */
static void percorre_em_ordem(NoB *x) {
    int i;
    for (i = 0; i < x->n; ++i) {
        if (!x->folha) percorre_em_ordem(x->filhos[i]);
        printf("%d ", x->chaves[i]);
    }
    if (!x->folha) percorre_em_ordem(x->filhos[i]);
}
void ab_em_ordem(ArvoreB *T) {
    if (!T || !T->raiz) { puts("(vazia)"); return; }
    percorre_em_ordem(T->raiz);
    putchar('\n');
}

static void imprime_no(NoB *x, int prof) {
    if (!x) return;
    if (!x->folha) imprime_no(x->filhos[x->n], prof + 1);

    for (int d = 0; d < prof; ++d) printf("    ");
    printf("[");
    for (int i = 0; i < x->n; ++i) {
        printf("%d", x->chaves[i]);
        if (i + 1 < x->n) printf(" ");
    }
    printf("]\n");

    for (int i = x->n - 1; i >= 0; --i)
        if (!x->folha) imprime_no(x->filhos[i], prof + 1);
}
void ab_imprimir(ArvoreB *T) {
    if (!T || !T->raiz) { puts("(vazia)"); return; }
    imprime_no(T->raiz, 0);
}

/* ===== Helpers de entrada ===== */
#include <ctype.h>

static int ler_int(const char *prompt, int *out) {
    char buf[128];
    for (;;) {
        if (prompt) printf("%s", prompt);
        if (!fgets(buf, sizeof buf, stdin)) return 0;
        // pular espaços
        char *p = buf; while (isspace((unsigned char)*p)) p++;
        if (*p == '\0') continue;
        char *end;
        long v = strtol(p, &end, 10);
        if (p == end) { puts("Valor inválido. Tente novamente."); continue; }
        *out = (int)v;
        return 1;
    }
}

static void pausa_enter(void) {
    printf("Pressione ENTER para continuar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descarta */ }
}

/* ===== Menu interativo ===== */
int main(void) {
    ArvoreB *T = NULL;
    int t_atual = 0;  // 0 = não definido ainda

    for (;;) {
        puts("\n================= ARVORE B =================");
        printf("Ordem atual: %s\n", (t_atual >= 2) ? "t definida" : "(nao definida)");
        puts("1) Definir ordem pelo grau minimo t");
        puts("2) Definir ordem pela ordem M (maximo de filhos por no)");
        puts("3) Inserir chave");
        puts("4) Remover chave");
        puts("5) Buscar chave");
        puts("6) Listar em ordem (in-order)");
        puts("7) Imprimir estrutura (arvore)");
        puts("0) Sair");
        puts("============================================");

        int op;
        if (!ler_int("Opcao: ", &op)) break;

        switch (op) {
        case 1: { // definir por t
            int t;
            if (!ler_int("Informe t (grau minimo, t>=2): ", &t)) break;
            if (t < 2) { puts("t deve ser >= 2."); break; }
            if (T) { ab_destruir(T); T = NULL; }
            T = ab_criar(t);
            t_atual = t;
            printf("Arvore criada com t=%d (max chaves por no = %d).\n", t, 2*t - 1);
            pausa_enter();
        } break;

        case 2: { // definir por M (ordem, max filhos por nó)
            int M;
            if (!ler_int("Informe M (ordem/ max filhos por no, M>=4): ", &M)) break;
            if (M < 4) { puts("M deve ser >= 4 (implica t>=2)."); break; }
            // t = ceil(M/2.0). Para M par, t = M/2. Para M impar, arredonda pra cima.
            int t = (M + 1) / 2;
            if (T) { ab_destruir(T); T = NULL; }
            T = ab_criar(t);
            t_atual = t;
            printf("Arvore criada com ordem M=%d -> t=%d (max chaves por no = %d).\n", M, t, 2*t - 1);
            pausa_enter();
        } break;

        case 3: { // inserir
            if (!T) { puts("Defina a ordem primeiro (opcao 1 ou 2)."); pausa_enter(); break; }
            int k;
            if (!ler_int("Chave a inserir: ", &k)) break;
            ab_inserir(T, k);
            puts("Inserido (duplicatas sao ignoradas).");
            pausa_enter();
        } break;

        case 4: { // remover
            if (!T) { puts("Defina a ordem primeiro (opcao 1 ou 2)."); pausa_enter(); break; }
            int k;
            if (!ler_int("Chave a remover: ", &k)) break;
            ab_remover(T, k);
            puts("Remocao concluida (se existia).");
            pausa_enter();
        } break;

        case 5: { // buscar
            if (!T) { puts("Defina a ordem primeiro (opcao 1 ou 2)."); pausa_enter(); break; }
            int k, pos;
            if (!ler_int("Chave a buscar: ", &k)) break;
            NoB *n = ab_buscar(T, k, &pos);
            if (n) printf("Encontrado! Chave %d no indice %d do no (n=%d chaves).\n", k, pos, n->n);
            else   printf("Nao encontrado: %d\n", k);
            pausa_enter();
        } break;

        case 6: { // em ordem
            if (!T) { puts("Defina a ordem primeiro (opcao 1 ou 2)."); pausa_enter(); break; }
            puts("Em ordem:");
            ab_em_ordem(T);
            pausa_enter();
        } break;

        case 7: { // imprimir estrutura
            if (!T) { puts("Defina a ordem primeiro (opcao 1 ou 2)."); pausa_enter(); break; }
            puts("Arvore (formatada):");
            ab_imprimir(T);
            pausa_enter();
        } break;

        case 0: { // sair
            if (T) { ab_destruir(T); T = NULL; }
            puts("Saindo...");
            return 0;
        }

        default:
            puts("Opcao invalida.");
            pausa_enter();
        }
    }

    // fallback: se sair por EOF em ler_int
    if (T) ab_destruir(T);
    return 0;
}
