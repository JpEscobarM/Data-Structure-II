// PROGRAMA EM C PARA IMPLEMENTAR ÁRVORE B+
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define  ORDEM 1
#define MIN_FILHOS (2*ORDEM)+1
// GRAU MÍNIMO (DEFINE A FAIXA DE QUANTIDADE DE
// CHAVES)

typedef struct Node {
    // VETOR DE CHAVES
    int* keys;
    // GRAU MÍNIMO (DEFINE A FAIXA DE QUANTIDADE DE CHAVES)
    int t;
    // VETOR DE PONTEIROS PARA FILHOS
    struct Node** children;
    // NÚMERO ATUAL DE CHAVES
    int n;
    // INDICA SE O NÓ É FOLHA OU NÃO
    bool leaf;
    // PONTEIRO PARA A PRÓXIMA FOLHA
    struct Node* next;
} Node;

typedef struct BTree {
    // PONTEIRO PARA O NÓ RAIZ
    Node* root;
    // GRAU MÍNIMO
    int t;
} BTree;

// FUNÇÃO PARA CRIAR UM NOVO NÓ DE ÁRVORE B+
Node* createNode(int t, bool leaf)
{
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->t = t;
    newNode->leaf = leaf;
    newNode->keys = (int*)malloc((2 * t - 1) * sizeof(int));
    newNode->children
        = (Node**)malloc((2 * t) * sizeof(Node*));
    newNode->n = 0;
    newNode->next = NULL;
    return newNode;
}

// FUNÇÃO PARA CRIAR UMA NOVA ÁRVORE B+
BTree* createBTree(int t)
{
    BTree* btree = (BTree*)malloc(sizeof(BTree));
    btree->t = t;
    btree->root = createNode(t, true);
    return btree;
}

// FUNÇÃO PARA EXIBIR A ÁRVORE B+ E IMPRIMIR SUAS CHAVES
void display(Node* node)
{
    if (node == NULL)
        return;
    int i;
    for (i = 0; i < node->n; i++) {
        if (!node->leaf) {
            display(node->children[i]);
        }
        printf("%d ", node->keys[i]);
    }
    if (!node->leaf) {
        display(node->children[i]);
    }
}

// FUNÇÃO PARA BUSCAR UMA CHAVE NA ÁRVORE B+
bool search(Node* node, int key) {
    if (!node) return false;
    while (!node->leaf) {
        int i = 0;
        while (i < node->n && key >= node->keys[i]) i++;
        node = node->children[i];
    }
    for (int i = 0; i < node->n; ++i)
        if (node->keys[i] == key) return true;
    return false;
}



// FUNÇÃO PARA DIVIDIR O FILHO DE UM NÓ DURANTE A INSERÇÃO
void splitChild(Node* parent, int i, Node* child) {
    int t = child->t;

    if (child->leaf) {
        // --- SPLIT DE FOLHA (B+) ---
        Node* newLeaf = createNode(t, true);

        int split = t;                     // deixa t chaves na esquerda
        int rightCount = child->n - split; // resto vai pra direita

        // move metade alta para a nova folha
        for (int j = 0; j < rightCount; ++j)
            newLeaf->keys[j] = child->keys[split + j];

        child->n   = split;
        newLeaf->n = rightCount;

        // encadeia folhas (lista ligada de folhas)
        newLeaf->next = child->next;
        child->next   = newLeaf;

        // abre espaço no pai para novo ponteiro
        for (int j = parent->n; j >= i + 1; --j)
            parent->children[j + 1] = parent->children[j];
        parent->children[i + 1] = newLeaf;

        // Sobe **cópia** do separador: a MENOR chave da nova folha
        int sep = newLeaf->keys[0];
        for (int j = parent->n - 1; j >= i; --j)
            parent->keys[j + 1] = parent->keys[j];
        parent->keys[i] = sep;
        parent->n += 1;
        return;
    }

    // --- SPLIT DE NÓ INTERNO (igual ao B-tree) ---
    Node* newChild = createNode(t, false);
    newChild->n = t - 1;
    for (int j = 0; j < t - 1; ++j)
        newChild->keys[j] = child->keys[j + t];
    for (int j = 0; j < t; ++j)
        newChild->children[j] = child->children[j + t];

    int promoted = child->keys[t - 1];
    child->n = t - 1;

    for (int j = parent->n; j >= i + 1; --j)
        parent->children[j + 1] = parent->children[j];
    parent->children[i + 1] = newChild;

    for (int j = parent->n - 1; j >= i; --j)
        parent->keys[j + 1] = parent->keys[j];
    parent->keys[i] = promoted;
    parent->n += 1;
}


// FUNÇÃO PARA INSERIR EM UM NÓ NÃO CHEIO
void insertNonFull(Node* node, int key)
{
    int i = node->n - 1;

    if (node->leaf) {
        while (i >= 0 && node->keys[i] > key) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
        node->n += 1;
    }
    else {
        while (i >= 0 && node->keys[i] > key) {
            i--;
        }
        i++;
        if (node->children[i]->n == 2 * node->t - 1) {
            splitChild(node, i, node->children[i]);
            if (node->keys[i] < key) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

// FUNÇÃO PARA INSERIR UMA CHAVE NA ÁRVORE B+
void insert(BTree* btree, int key)
{
    Node* root = btree->root;
    if (root->n == 2 * btree->t - 1) {
        Node* newRoot = createNode(btree->t, false);
        newRoot->children[0] = root;
        splitChild(newRoot, 0, root);
        insertNonFull(newRoot, key);
        btree->root = newRoot;
    }
    else {
        insertNonFull(root, key);
    }
}

// PROTÓTIPOS DE FUNÇÕES AUXILIARES USADAS EM
// DELETEKEY
void deleteKeyHelper(Node* node, int key);
int findKey(Node* node, int key);
void removeFromLeaf(Node* node, int idx);
int getPredecessor(Node* node, int idx);
void fill(Node* node, int idx);
void borrowFromPrev(Node* node, int idx);
void borrowFromNext(Node* node, int idx);
void merge(Node* node, int idx);

// FUNÇÃO PARA REMOVER UMA CHAVE DA ÁRVORE B+
void deleteKey(BTree* btree, int key)
{
    Node* root = btree->root;

    // CHAMA UMA FUNÇÃO AUXILIAR PARA REMOVER A CHAVE RECURSIVAMENTE
    deleteKeyHelper(root, key);

    // SE A RAIZ FICOU SEM CHAVES E POSSUI UM FILHO, TORNA O
    // PRIMEIRO FILHO A NOVA RAIZ
    if (root->n == 0 && !root->leaf) {
        btree->root = root->children[0];
        free(root);
    }
}

// FUNÇÃO AUXILIAR PARA REMOVER RECURSIVAMENTE UMA CHAVE DA ÁRVORE B+
void deleteKeyHelper(Node* node, int key)
{
    int idx = findKey(
        node, key); // ENCONTRA O ÍNDICE DA CHAVE NO NÓ

    // SE A CHAVE ESTÁ PRESENTE NESTE NÓ
    if (idx < node->n && node->keys[idx] == key) {
        if (node->leaf) {
            // SE O NÓ É FOLHA, SIMPLESMENTE REMOVE A CHAVE
            removeFromLeaf(node, idx);
        }
        else {
            // SE O NÓ NÃO É FOLHA, SUBSTITUI A CHAVE
            // PELO SEU PREDECESSOR/SUCESSOR
            int predecessor = getPredecessor(node, idx);
            node->keys[idx] = predecessor;
            // REMOVE RECURSIVAMENTE O PREDECESSOR
            deleteKeyHelper(node->children[idx],
                            predecessor);
        }
    }
    else {
        // SE A CHAVE NÃO ESTÁ NESTE NÓ, DESCE PARA O
        // FILHO APROPRIADO
        if (node->leaf) {
            // CHAVE NÃO ENCONTRADA NA ÁRVORE
            printf("Key %d not found in the B+ tree.\n",
                   key);
            return;
        }

        bool isLastChild = (idx == node->n);

        // SE O FILHO ONDE A CHAVE DEVERIA ESTAR
        // TEM MENOS QUE T CHAVES, PREENCHA ESSE FILHO
        if (node->children[idx]->n < node->t) {
            fill(node, idx);
        }

        // SE O ÚLTIMO FILHO FOI FUNDIDO, ELE DEVE TER
        // SIDO FUNDIDO COM O FILHO ANTERIOR

        // ENTÃO, PRECISAMOS REMOVER RECURSIVAMENTE A CHAVE DO
        // FILHO ANTERIOR
        if (isLastChild && idx > node->n) {
            deleteKeyHelper(node->children[idx - 1], key);
        }
        else {
            deleteKeyHelper(node->children[idx], key);
        }
    }
}
// FUNÇÃO PARA ENCONTRAR O ÍNDICE DE UMA CHAVE EM UM NÓ
int findKey(Node* node, int key)
{
    int idx = 0;
    while (idx < node->n && key > node->keys[idx]) {
        idx++;
    }
    return idx;
}

// FUNÇÃO PARA REMOVER UMA CHAVE DE UM NÓ FOLHA
void removeFromLeaf(Node* node, int idx)
{
    for (int i = idx + 1; i < node->n; ++i) {
        node->keys[i - 1] = node->keys[i];
    }
    node->n--;
}

// FUNÇÃO PARA OBTER O PREDECESSOR DE UMA CHAVE EM UM NÓ NÃO FOLHA
int getPredecessor(Node* node, int idx)
{
    Node* curr = node->children[idx];
    while (!curr->leaf) {
        curr = curr->children[curr->n];
    }
    return curr->keys[curr->n - 1];
}

// FUNÇÃO PARA PREENCHER O FILHO PRESENTE NA POSIÇÃO IDX-ÉSIMA
// DO NÓ
void fill(Node* node, int idx)
{
    if (idx != 0 && node->children[idx - 1]->n >= node->t) {
        borrowFromPrev(node, idx);
    }
    else if (idx != node->n
             && node->children[idx + 1]->n >= node->t) {
        borrowFromNext(node, idx);
    }
    else {
        if (idx != node->n) {
            merge(node, idx);
        }
        else {
            merge(node, idx - 1);
        }
    }
}

// FUNÇÃO PARA TOMAR EMPRESTADA UMA CHAVE DO FILHO ANTERIOR E MOVÊ-LA
// PARA O FILHO NA POSIÇÃO IDX
void borrowFromPrev(Node* node, int idx)
{
    Node* child = node->children[idx];
    Node* sibling = node->children[idx - 1];

    // MOVE TODAS AS CHAVES EM CHILD UMA POSIÇÃO À FRENTE
    for (int i = child->n - 1; i >= 0; --i) {
        child->keys[i + 1] = child->keys[i];
    }

    // SE CHILD NÃO É FOLHA, MOVE SEUS PONTEIROS DE FILHOS
    // UMA POSIÇÃO À FRENTE
    if (!child->leaf) {
        for (int i = child->n; i >= 0; --i) {
            child->children[i + 1] = child->children[i];
        }
    }

    // DEFINE A PRIMEIRA CHAVE DE CHILD IGUAL À CHAVE NODE->KEYS[IDX - 1]
    child->keys[0] = node->keys[idx - 1];

    // MOVE O ÚLTIMO FILHO DO IRMÃO COMO PRIMEIRO FILHO DE CHILD
    if (!child->leaf) {
        child->children[0] = sibling->children[sibling->n];
    }

    // MOVE A CHAVE DO IRMÃO PARA O PAI
    node->keys[idx - 1] = sibling->keys[sibling->n - 1];

    // INCREMENTA E DECREMENTA AS CONTAGENS DE CHAVES DE CHILD
    // E DO IRMÃO, RESPECTIVAMENTE
    child->n += 1;
    sibling->n -= 1;
}

// FUNÇÃO PARA TOMAR EMPRESTADA UMA CHAVE DO PRÓXIMO FILHO E MOVÊ-LA
// PARA O FILHO NA POSIÇÃO IDX
void borrowFromNext(Node* node, int idx)
{
    Node* child = node->children[idx];
    Node* sibling = node->children[idx + 1];

    // DEFINE A (T - 1)-ÉSIMA CHAVE DE CHILD COMO NODE->KEYS[IDX]
    child->keys[(child->n)] = node->keys[idx];

    // SE CHILD NÃO É FOLHA, MOVE SEUS PONTEIROS DE FILHOS
    // UMA POSIÇÃO À FRENTE
    if (!child->leaf) {
        child->children[(child->n) + 1]
            = sibling->children[0];
    }

    // DEFINE A CHAVE NA POSIÇÃO IDX DO PAI COMO A PRIMEIRA
    // CHAVE DO IRMÃO (SIBLING)
    node->keys[idx] = sibling->keys[0];

    // MOVE TODAS AS CHAVES DO IRMÃO UMA POSIÇÃO PARA TRÁS
    for (int i = 1; i < sibling->n; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
    }

    // SE O IRMÃO NÃO É FOLHA, MOVE SEUS PONTEIROS DE FILHOS
    // UMA POSIÇÃO PARA TRÁS
    if (!sibling->leaf) {
        for (int i = 1; i <= sibling->n; ++i) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }

    // INCREMENTA E DECREMENTA AS CONTAGENS DE CHAVES DE CHILD
    // E DO IRMÃO, RESPECTIVAMENTE
    child->n += 1;
    sibling->n -= 1;
}

// FUNÇÃO PARA FUNDIR O FILHO IDX DO NÓ COM O FILHO (IDX + 1)
// DO MESMO NÓ
void merge(Node* node, int idx)
{
    Node* child = node->children[idx];
    Node* sibling = node->children[idx + 1];

    // PUXA UMA CHAVE DO NÓ ATUAL E A INSERE NA
    // POSIÇÃO (T-1) DE CHILD
    child->keys[child->n] = node->keys[idx];

    // SE CHILD NÃO É FOLHA, MOVE SEUS PONTEIROS DE FILHOS
    // UMA POSIÇÃO À FRENTE
    if (!child->leaf) {
        child->children[child->n + 1]
            = sibling->children[0];
    }

    // COPIA AS CHAVES DO IRMÃO PARA CHILD
    for (int i = 0; i < sibling->n; ++i) {
        child->keys[i + child->n + 1] = sibling->keys[i];
    }

    // SE CHILD NÃO É FOLHA, COPIA TAMBÉM OS PONTEIROS DE FILHOS
    if (!child->leaf) {
        for (int i = 0; i <= sibling->n; ++i) {
            child->children[i + child->n + 1]
                = sibling->children[i];
        }
    }

    // MOVE TODAS AS CHAVES APÓS IDX NO NÓ ATUAL UMA POSIÇÃO
    // PARA TRÁS, PARA PREENCHER O ESPAÇO CRIADO AO MOVER
    // KEYS[IDX] PARA CHILD
    for (int i = idx + 1; i < node->n; ++i) {
        node->keys[i - 1] = node->keys[i];
    }

    // MOVE OS PONTEIROS DE FILHOS APÓS (IDX + 1) NO
    // NÓ ATUAL UMA POSIÇÃO PARA TRÁS
    for (int i = idx + 2; i <= node->n; ++i) {
        node->children[i - 1] = node->children[i];
    }

    // ATUALIZA A CONTAGEM DE CHAVES DE CHILD E DO NÓ ATUAL
    child->n += sibling->n + 1;
    node->n--;

    // LIBERA A MEMÓRIA OCUPADA PELO IRMÃO
    free(sibling);
}



// -----------------
// PRETTY PRINT (ROTACIONADA 90° PARA A ESQUERDA)
// -----------------

// IMPRIME INDENTAÇÃO (4 ESPAÇOS POR NÍVEL)
static void pp_indent(int level) {
    for (int i = 0; i < level * 4; ++i) putchar(' ');
}

// RECUSIVA: VISITA PRIMEIRO O FILHO MAIS À DIREITA, DEPOIS (CHAVE, FILHO À ESQUERDA)
static void printTreeRecRot(const Node *node, int level) {
    if (!node) return;

    // NÓ VAZIO (PODE OCORRER NA RAIZ APÓS MERGES/REDUÇÕES)
    if (node->n == 0) {
        pp_indent(level);
        printf("[]\n");
        return;
    }

    // PRIMEIRO O FILHO MAIS À DIREITA
    if (!node->leaf)
        printTreeRecRot(node->children[node->n], level + 1);

    // DEPOIS, DA DIREITA PARA A ESQUERDA: IMPRIME A CHAVE E A SUBÁRVORE À ESQUERDA
    for (int i = node->n - 1; i >= 0; --i) {
        pp_indent(level);
        printf("%d\n", node->keys[i]);
        if (!node->leaf)
            printTreeRecRot(node->children[i], level + 1);
    }
}

// FUNÇÃO PÚBLICA: IMPRIME A ÁRVORE TODA ROTACIONADA
void printTreeRotated(const BTree *T) {
    printf("\nB+ TREE (VISUALIZAÇÃO ROTACIONADA 90° PARA A ESQUERDA)\n");
    printf("MAIORES À ESQUERDA/TOPO, MENORES À DIREITA/BAIXO.\n\n");
    printTreeRecRot(T ? T->root : NULL, 0);
}



int main()
{
    BTree* btree = createBTree(MIN_FILHOS);

    // INSERE ELEMENTOS NA ÁRVORE B+
    for(int i = 0 ; i < 20; i ++)
    {
        insert(btree,i);
    }
    // IMPRIME A ÁRVORE B+
    printf("B+ Tree after insertion: ");
    display(btree->root);
    printf("\n");

    // BUSCA POR UMA CHAVE
    int key_to_search = 17;
    bool found = search(btree->root, key_to_search);

    if (found) {
        printf("Key %d found in the B+ tree.\n",
               key_to_search);
    }
    else {
        printf("Key %d not found in the B+ tree.\n",
               key_to_search);
    }

    // REMOVE ELEMENTO DA ÁRVORE B+
    deleteKey(btree, 17);

    // IMPRIME A ÁRVORE B+ APÓS A REMOÇÃO
    printf("B+ Tree after deletion: ");
    display(btree->root);
    printf("\n");

    deleteKey(btree, 15);
    deleteKey(btree, 14);
    deleteKey(btree, 9);

    // IMPRIME A ÁRVORE B+ APÓS A REMOÇÃO
    printf("B+ Tree after deletion: ");
    display(btree->root);
    printf("\n");


    found = search(btree->root, key_to_search);

    if (found) {
        printf("Key %d found in the B+ tree.\n",
               key_to_search);
    }
    else {
        printf("Key %d not found in the B+ tree.\n",
               key_to_search);
    }

    printTreeRotated(btree);

    return 0;
}
