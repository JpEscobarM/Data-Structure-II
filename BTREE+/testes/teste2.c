/*
 * B+ Tree em C — inserção, busca, impressão e remoção (borrow/merge).
 * Convenções:
 *   - chaves em 1..count
 *   - filhos em 0..count (apenas nós internos)
 *   - folhas encadeadas via 'next' para varredura sequencial
 *   - duplicatas não permitidas
 *
 * Como compilar:
 *   gcc -O2 -Wall bpt.c -o bpt
 * Como rodar:
 *   ./bpt
 *
 * Observação: ajuste a ordem alterando ORDEM (ex.: 2, 3, 4...). MAX = 2*ORDEM.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* ===== Parâmetros da árvore ===== */
#define ORDEM 1              /* MIN = ORDEM, MAX = 2*ORDEM */
#define MAX   (2*ORDEM)
#define MIN   (ORDEM)

/* ===== Estruturas ===== */
typedef struct BPlusNode {
    int is_leaf;                         /* 1 = folha, 0 = interno */
    int count;                           /* qtde de chaves armazenadas */
    int key[MAX + 2];                    /* usamos 1..count; "+2" dá folga em operações */
    struct BPlusNode* child[MAX + 2];    /* filhos 0..count (apenas em nós internos) */
    struct BPlusNode* next;              /* encadeamento de folhas */
} BPlusNode;

static BPlusNode* root = NULL;

/* ===================================================================== */
/*                               Utilitários                              */
/* ===================================================================== */

static BPlusNode* createNode(int is_leaf) {
    BPlusNode* n = (BPlusNode*)calloc(1, sizeof(BPlusNode));
    n->is_leaf = is_leaf;
    n->count   = 0;
    n->next    = NULL;
    for (int i = 0; i < MAX + 2; ++i) n->child[i] = NULL;
    return n;
}

static bool leaf_has_key(BPlusNode* leaf, int k) {
    for (int i = 1; i <= leaf->count; ++i) if (leaf->key[i] == k) return true;
    return false;
}

/* em nó interno, retorna o índice de filho [0..count] a seguir para a chave k */
static int find_child_idx(BPlusNode* n, int k) {
    /* separadores são "first_key(child[i])"; seguimos à direita enquanto k >= key[i] */
    int i = 1;
    while (i <= n->count && k >= n->key[i]) i++;
    return i - 1;
}

/* insere k numa folha (sem checar overflow) */
static void insert_into_leaf(BPlusNode* leaf, int k) {
    int i = leaf->count;
    while (i >= 1 && k < leaf->key[i]) {
        leaf->key[i + 1] = leaf->key[i];
        i--;
    }
    leaf->key[i + 1] = k;
    leaf->count++;
}

/* insere (k, newChild) num interno na posição 'pos' (onde a CHAVE deve entrar) */
static void insert_into_internal_at(BPlusNode* n, int pos, int k, BPlusNode* newChild) {
    for (int i = n->count; i >= pos; --i) {
        n->key[i + 1]   = n->key[i];
        n->child[i + 1] = n->child[i];
    }
    n->key[pos]   = k;
    n->child[pos] = newChild;   /* newChild fica à direita da chave inserida */
    n->count++;
}

/* menor chave da subárvore (folha mais à esquerda) */
static int first_key_subtree(const BPlusNode* n) {
    while (n && !n->is_leaf) n = n->child[0];
    return (n && n->count) ? n->key[1] : 0;
}

/* recalcula separadores: para interno p, key[i] = first_key(child[i]) */
static void refresh_separators(BPlusNode* p) {
    if (!p || p->is_leaf) return;
    for (int i = 1; i <= p->count; ++i) p->key[i] = first_key_subtree(p->child[i]);
}

/* ===================================================================== */
/*                                  Split                                 */
/* ===================================================================== */

typedef struct { int upKey; BPlusNode* right; } SplitResult;

/* divide folha cheia (count == MAX+1). Sobe a primeira chave da folha direita. */
static SplitResult split_leaf(BPlusNode* leaf) {
    BPlusNode* right = createNode(1);
    int total = leaf->count;               /* esperado: MAX+1 */
    int leftCount = (total + 1) / 2;       /* ceil((MAX+1)/2) */
    int ri = 1;

    for (int i = leftCount + 1; i <= total; ++i) {
        right->key[ri++] = leaf->key[i];
    }
    right->count = ri - 1;

    leaf->count = leftCount;

    /* encadeamento de folhas */
    right->next = leaf->next;
    leaf->next  = right;

    SplitResult s = { .upKey = right->key[1], .right = right };
    return s;
}

/* divide interno cheio (count == MAX+1). Promove a mediana (B+). */
static SplitResult split_internal(BPlusNode* n) {
    BPlusNode* right = createNode(0);
    int total = n->count;                  /* MAX+1 */
    int m = (total + 1) / 2;               /* índice da chave promovida */
    int up = n->key[m];

    int ri = 1;
    right->child[0] = n->child[m];
    for (int i = m + 1; i <= total; ++i) {
        right->key[ri]   = n->key[i];
        right->child[ri] = n->child[i];
        ri++;
    }
    right->count = ri - 1;

    n->count = m - 1; /* esquerda fica com chaves 1..m-1 e filhos 0..m-1 */

    SplitResult s = { .upKey = up, .right = right };
    return s;
}

/* ===================================================================== */
/*                                 Inserção                               */
/* ===================================================================== */

static int insert_rec(BPlusNode* n, int k, int* upKey, BPlusNode** newChild) {
    if (n->is_leaf) {
        if (leaf_has_key(n, k)) return 0;  /* rejeita duplicatas */
        insert_into_leaf(n, k);
        if (n->count <= MAX) return 0;

        SplitResult s = split_leaf(n);
        *upKey    = s.upKey;
        *newChild = s.right;
        return 1; /* estourou e dividiu */
    } else {
        int pos = find_child_idx(n, k);
        int childSplit = insert_rec(n->child[pos], k, upKey, newChild);
        if (!childSplit) {
            /* pode ter alterado o first_key do filho */
            refresh_separators(n);
            return 0;
        }

        insert_into_internal_at(n, pos + 1, *upKey, *newChild);
        if (n->count <= MAX) {
            refresh_separators(n);
            return 0;
        }

        SplitResult s = split_internal(n);
        *upKey    = s.upKey;
        *newChild = s.right;
        return 1;
    }
}

void bpt_insert(int k) {
    if (!root) {
        root = createNode(1);
        root->key[1] = k;
        root->count  = 1;
        return;
    }
    int upKey; BPlusNode* newChild;
    int splitRoot = insert_rec(root, k, &upKey, &newChild);
    if (splitRoot) {
        BPlusNode* newRoot  = createNode(0);
        newRoot->count      = 1;
        newRoot->key[1]     = upKey;
        newRoot->child[0]   = root;
        newRoot->child[1]   = newChild;
        root = newRoot;
    } else if (!root->is_leaf) {
        refresh_separators(root);
    }
}

/* ===================================================================== */
/*                                   Busca                                */
/* ===================================================================== */

bool bpt_search(int k) {
    BPlusNode* n = root;
    while (n && !n->is_leaf) {
        int pos = find_child_idx(n, k);
        n = n->child[pos];
    }
    if (!n) return false;
    return leaf_has_key(n, k);
}

/* ===================================================================== */
/*                                Impressão                               */
/* ===================================================================== */

static void print_indent(int level) { for (int i = 0; i < level * 4; ++i) putchar(' '); }

static void printTreeRec(const BPlusNode* n, int level) {
    if (!n) return;

    if (n->is_leaf) {
        print_indent(level);
        printf("L[");
        for (int i = 1; i <= n->count; ++i) {
            printf("%d%s", n->key[i], (i < n->count ? " " : ""));
        }
        printf("]\n");
        return;
    }

    /* imprime primeiro a subárvore mais à direita */
    printTreeRec(n->child[n->count], level + 1);

    /* depois: separador e subárvore à esquerda (visualização rotacionada) */
    for (int i = n->count; i >= 1; --i) {
        print_indent(level);
        printf("|%d|\n", n->key[i]);
        printTreeRec(n->child[i - 1], level + 1);
    }
}

void printTree(const BPlusNode* r) {
    printf("\nB+ Tree (visualização rotacionada 90° p/ esquerda)\n");
    printf("Maiores à esquerda/topo; folhas marcadas como L[...]\n\n");
    printTreeRec(r, 0);
}

void printLeaves(void) {
    if (!root) { puts("(vazia)"); return; }
    const BPlusNode* n = root;
    while (n && !n->is_leaf) n = n->child[0];

    printf("Folhas (em ordem): ");
    while (n) {
        for (int i = 1; i <= n->count; ++i) printf("%d ", n->key[i]);
        n = n->next;
    }
    puts("");
}

/* ===================================================================== */
/*                                 Remoção                                */
/*    Casos: apagar na folha → tentar BORROW do irmão → MERGE;            */
/*           atualizar índices (sucessor) e encolher raiz se preciso.     */
/* ===================================================================== */

static int delete_from_leaf(BPlusNode* leaf, int k) {
    int pos = 0;
    for (int i = 1; i <= leaf->count; ++i) if (leaf->key[i] == k) { pos = i; break; }
    if (!pos) return 0;
    for (int i = pos; i < leaf->count; ++i) leaf->key[i] = leaf->key[i + 1];
    leaf->count--;
    return 1;
}

/* remove key[idx] e child[idx] do pai (compacta) */
static void parent_remove_at(BPlusNode* p, int idx) {
    for (int i = idx; i < p->count; ++i) p->key[i] = p->key[i + 1];
    for (int i = idx; i < p->count; ++i) p->child[i] = p->child[i + 1];
    p->child[p->count] = NULL;
    p->count--;
}

/* fusão de folhas: left <- right; remove separador do pai */
static void merge_leaves(BPlusNode* parent, int sepIdx, BPlusNode* left, BPlusNode* right) {
    for (int i = 1; i <= right->count; ++i)
        left->key[left->count + i] = right->key[i];
    left->count += right->count;
    left->next = right->next;
    free(right);

    parent_remove_at(parent, sepIdx);   /* remove separador do pai */
    refresh_separators(parent);         /* atualiza índices (sucessores) */
}

/* fusão de internos: left <- (fronteira) + right; remove separador do pai */
static void merge_internals(BPlusNode* parent, int sepIdx, BPlusNode* left, BPlusNode* right) {
    int L = left->count;

    /* fronteira no B+: primeira chave da subárvore direita */
    left->key[L + 1]   = first_key_subtree(right);
    left->child[L + 1] = right->child[0];

    for (int j = 1; j <= right->count; ++j) {
        left->key[L + 1 + j]   = right->key[j];
        left->child[L + 1 + j] = right->child[j];
    }
    left->count = L + 1 + right->count;
    free(right);

    parent_remove_at(parent, sepIdx);
    refresh_separators(left);
    refresh_separators(parent);
}

/* reequilibra quando child em 'pos' ficou < MIN:
   tenta BORROW do irmão direito (preferência), depois esquerdo; senão MERGE. */
static int rebalance_child_after_delete(BPlusNode* parent, int pos) {
    BPlusNode* child = parent->child[pos];
    const int minKeys = MIN;

    if (!child) return 0;
    if (child->count >= minKeys) {
        refresh_separators(parent);
        return 0;
    }

    BPlusNode* leftSib  = (pos > 0) ? parent->child[pos - 1] : NULL;
    BPlusNode* rightSib = (pos < parent->count) ? parent->child[pos + 1] : NULL;

    /* 1) BORROW do irmão direito */
    if (rightSib && rightSib->count > minKeys) {
        if (child->is_leaf) {
            child->key[child->count + 1] = rightSib->key[1];
            child->count++;
            for (int i = 1; i < rightSib->count; ++i) rightSib->key[i] = rightSib->key[i + 1];
            rightSib->count--;

            if (pos + 1 <= parent->count) parent->key[pos + 1] = first_key_subtree(rightSib);
            refresh_separators(parent);
        } else {
            child->key[child->count + 1]   = first_key_subtree(rightSib->child[0]);
            child->child[child->count + 1] = rightSib->child[0];
            child->count++;

            rightSib->child[0] = rightSib->child[1];
            for (int i = 1; i < rightSib->count; ++i) {
                rightSib->key[i]   = rightSib->key[i + 1];
                rightSib->child[i] = rightSib->child[i + 1];
            }
            rightSib->child[rightSib->count] = NULL;
            rightSib->count--;

            refresh_separators(child);
            refresh_separators(rightSib);
            refresh_separators(parent);
        }
        return (parent != root && parent->count < MIN);
    }

    /* 2) BORROW do irmão esquerdo */
    if (leftSib && leftSib->count > minKeys) {
        if (child->is_leaf) {
            for (int i = child->count; i >= 1; --i) child->key[i + 1] = child->key[i];
            child->key[1] = leftSib->key[leftSib->count];
            child->count++;
            leftSib->count--;

            if (pos >= 1) parent->key[pos] = first_key_subtree(child);
            refresh_separators(parent);
        } else {
            for (int i = child->count; i >= 1; --i) child->key[i + 1] = child->key[i];
            for (int i = child->count; i >= 0; --i) child->child[i + 1] = child->child[i];
            child->child[0] = leftSib->child[leftSib->count];
            child->count++;
            leftSib->child[leftSib->count] = NULL;
            leftSib->count--;

            refresh_separators(child);
            refresh_separators(leftSib);
            refresh_separators(parent);
        }
        return (parent != root && parent->count < MIN);
    }

    /* 3) MERGE (não deu para emprestar) */
    if (rightSib) {
        if (child->is_leaf) merge_leaves(parent, pos + 1, child, rightSib);
        else                merge_internals(parent, pos + 1, child, rightSib);
    } else if (leftSib) {
        if (child->is_leaf) merge_leaves(parent, pos, leftSib, child);
        else                merge_internals(parent, pos, leftSib, child);
    }
    return (parent != root && parent->count < MIN);
}

/* remoção recursiva:
   - desce à folha e remove;
   - ao subir, atualiza índices e tenta borrow/merge;
   retorna 1 se ESTE nó ficou < MIN (para o nível acima tratar) */
static int delete_rec(BPlusNode* n, int k) {
    if (!n) return 0;

    if (n->is_leaf) {
        int removed = delete_from_leaf(n, k);
        if (!removed) return 0;           /* chave não existe */
        if (n == root) return 0;          /* raiz-folha pode ficar com < MIN */
        return (n->count < MIN);
    }

    int pos = find_child_idx(n, k);
    int child_underflow = delete_rec(n->child[pos], k);

    if (child_underflow) {
        int parent_underflow = rebalance_child_after_delete(n, pos);
        if (n == root) return 0;
        if (parent_underflow) return 1;
    } else {
        /* mesmo sem underflow, a cópia/índice pode mudar */
        refresh_separators(n);
    }

    if (n != root && n->count < MIN) return 1;
    return 0;
}

/* API de remoção: cuida de encolher a raiz após fusões */
void bpt_delete(int k) {
    if (!root) return;

    (void)delete_rec(root, k);

    /* raiz interna vazia → desce um nível */
    if (root && !root->is_leaf && root->count == 0) {
        BPlusNode* old = root;
        root = root->child[0];
        free(old);
    }

    /* árvore pode ficar vazia */
    if (root && root->is_leaf && root->count == 0) {
        free(root);
        root = NULL;
    }
}

/* ===================================================================== */
/*                         (Opcional) liberar memória                     */
/* ===================================================================== */
void bpt_free_rec(BPlusNode* n) {
    if (!n) return;
    if (!n->is_leaf) {
        for (int i = 0; i <= n->count; ++i) bpt_free_rec(n->child[i]);
    }
    free(n);
}
void bpt_free(void) { bpt_free_rec(root); root = NULL; }

/* ===================================================================== */
/*                               Demo / Testes                            */
/* ===================================================================== */
int main(void) {
    /* Insere 0..29 */
    for (int i = 0; i < 100; ++i) bpt_insert(i);

    puts("Estado inicial:");
    printLeaves();
    printTree(root);

    /* Busca */
    printf("\nBusca 7: %s\n", bpt_search(7) ? "encontrado" : "nao encontrado");
    printf("Busca 31: %s\n", bpt_search(31) ? "encontrado" : "nao encontrado");

    /* Remover alguns valores alternados */
    for (int i = 0; i < 30; i += 2) bpt_delete(i);

    puts("\nDepois de remover pares:");
    printLeaves();
    printTree(root);

    /* Remover um range central */
    for (int i = 7; i <= 15; ++i) bpt_delete(i);

    puts("\nDepois de remover 7..15:");
    printLeaves();
    printTree(root);

    /* Verificações finais */
    printf("\nBusca 8: %s\n", bpt_search(8) ? "encontrado" : "nao encontrado");
    printf("Busca 17: %s\n", bpt_search(17) ? "encontrado" : "nao encontrado");

    /* liberar memória (opcional em ambientes controlados) */
    bpt_free();
    return 0;
}
