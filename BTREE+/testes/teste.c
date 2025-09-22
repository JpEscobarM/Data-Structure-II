// Deletion on a B+ Tree in C

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Default order
#define ORDER 5

typedef struct record {
  int value;
} record;

// No
typedef struct No {
  void **filhos;
  int *chaves;
  struct No *parent;
  bool folha;
  int num_chaves;
  struct No *prox;
} No;

int order = ORDER;
No *queue = NULL;
bool verbose_output = false;

// Enqueue
void enqueue(No *new_No);

// Dequeue
No *dequeue(void);
int height(No *const raiz);
int pathToLeaves(No *const raiz, No *filho);
void printLeaves(No *const raiz);
void printTree(No *const raiz);
void findAndPrint(No *const raiz, int key, bool verbose);
void findAndPrintRange(No *const raiz, int range1, int range2, bool verbose);
int findRange(No *const raiz, int key_start, int key_end, bool verbose,
        int returned_chaves[], void *returned_filhos[]);
No *findLeaf(No *const raiz, int key, bool verbose);
record *find(No *raiz, int key, bool verbose, No **leaf_out);
int cut(int length);

record *makeRecord(int value);
No *makeNo(void);
No *makeLeaf(void);
int getLeftIndex(No *parent, No *left);
No *insertIntoLeaf(No *leaf, int key, record *pointer);
No *insertIntoLeafAfterSplitting(No *raiz, No *leaf, int key,
                   record *pointer);
No *insertIntoNo(No *raiz, No *parent,
           int left_index, int key, No *right);
No *insertIntoNoAfterSplitting(No *raiz, No *parent,
                   int left_index,
                   int key, No *right);
No *insertIntoParent(No *raiz, No *left, int key, No *right);
No *insertIntoNewraiz(No *left, int key, No *right);
No *startNewTree(int key, record *pointer);
No *insert(No *raiz, int key, int value);
/*========================================================================  */




/* ==== PROT�TIPOS - FUN��ES DE DELE��O=== */
No *deleteKey(No *raiz, int key);
static No *deleteEntry(No *raiz, No *n, int key, void *pointer);
static No *adjustraiz(No *raiz);
static int getNeighborIndexFor(No *n);
static No *coalesceNos(No *raiz, No *n, No *neighbor,
                           int neighbor_index, int k_prime);
static No *redistributeNos(No *raiz, No *n, No *neighbor,
                               int neighbor_index, int k_prime_index, int k_prime);
static void removeEntryFromNo(No *n, int key, void *pointer);
/*========================================================================  */


// Enqueue
void enqueue(No *new_No) {
  No *c;
  if (queue == NULL) {
    queue = new_No;
    queue->prox = NULL;
  } else {
    c = queue;
    while (c->prox != NULL) {
      c = c->prox;
    }
    c->prox = new_No;
    new_No->prox = NULL;
  }
}

// Dequeue
No *dequeue(void) {
  No *n = queue;
  queue = queue->prox;
  n->prox = NULL;
  return n;
}

// Print the leaves
void printLeaves(No *const raiz) {
  if (raiz == NULL) {
    printf("Empty tree.\n");
    return;
  }
  int i;
  No *c = raiz;
  while (!c->folha)
    c = c->filhos[0];
  while (true) {
    for (i = 0; i < c->num_chaves; i++) {
      if (verbose_output)
        printf("%p ", c->filhos[i]);
      printf("%d ", c->chaves[i]);
    }
    if (verbose_output)
      printf("%p ", c->filhos[order - 1]);
    if (c->filhos[order - 1] != NULL) {
      printf(" | ");
      c = c->filhos[order - 1];
    } else
      break;
  }
  printf("\n");
}

// Calculate height
int height(No *const raiz) {
  int h = 0;
  No *c = raiz;
  while (!c->folha) {
    c = c->filhos[0];
    h++;
  }
  return h;
}

// Get path to raiz
int pathToLeaves(No *const raiz, No *filho) {
  int length = 0;
  No *c = filho;
  while (c != raiz) {
    c = c->parent;
    length++;
  }
  return length;
}

// Print the tree
void printTree(No *const raiz) {
  No *n = NULL;
  int i = 0;
  int rank = 0;
  int new_rank = 0;

  if (raiz == NULL) {
    printf("Empty tree.\n");
    return;
  }
  queue = NULL;
  enqueue(raiz);
  while (queue != NULL) {
    n = dequeue();
    if (n->parent != NULL && n == n->parent->filhos[0]) {
      new_rank = pathToLeaves(raiz, n);
      if (new_rank != rank) {
        rank = new_rank;
        printf("\n");
      }
    }
    if (verbose_output)
      printf("(%p)", n);
    for (i = 0; i < n->num_chaves; i++) {
      if (verbose_output)
        printf("%p ", n->filhos[i]);
      printf("%d ", n->chaves[i]);
    }
    if (!n->folha)
      for (i = 0; i <= n->num_chaves; i++)
        enqueue(n->filhos[i]);
    if (verbose_output) {
      if (n->folha)
        printf("%p ", n->filhos[order - 1]);
      else
        printf("%p ", n->filhos[n->num_chaves]);
    }
    printf("| ");
  }
  printf("\n");
}

// Find the No and print it
void findAndPrint(No *const raiz, int key, bool verbose) {
  No *leaf = NULL;
  record *r = find(raiz, key, verbose, NULL);
  if (r == NULL)
    printf("Record not found under key %d.\n", key);
  else
    printf("Record at %p -- key %d, value %d.\n",
         r, key, r->value);
}

// Find and print the range
void findAndPrintRange(No *const raiz, int key_start, int key_end,
             bool verbose) {
  int i;
  int array_size = key_end - key_start + 1;
  int returned_chaves[array_size];
  void *returned_filhos[array_size];
  int num_found = findRange(raiz, key_start, key_end, verbose,
                returned_chaves, returned_filhos);
  if (!num_found)
    printf("None found.\n");
  else {
    for (i = 0; i < num_found; i++)
      printf("Key: %d   Location: %p  Value: %d\n",
           returned_chaves[i],
           returned_filhos[i],
           ((record *)
            returned_filhos[i])
             ->value);
  }
}

// Find the range
int findRange(No *const raiz, int key_start, int key_end, bool verbose,
        int returned_chaves[], void *returned_filhos[]) {
  int i, num_found;
  num_found = 0;
  No *n = findLeaf(raiz, key_start, verbose);
  if (n == NULL)
    return 0;
  for (i = 0; i < n->num_chaves && n->chaves[i] < key_start; i++)
    ;
  if (i == n->num_chaves)
    return 0;
  while (n != NULL) {
    for (; i < n->num_chaves && n->chaves[i] <= key_end; i++) {
      returned_chaves[num_found] = n->chaves[i];
      returned_filhos[num_found] = n->filhos[i];
      num_found++;
    }
    n = n->filhos[order - 1];
    i = 0;
  }
  return num_found;
}

// Find the leaf
No *findLeaf(No *const raiz, int key, bool verbose) {
  if (raiz == NULL) {
    if (verbose)
      printf("Empty tree.\n");
    return raiz;
  }
  int i = 0;
  No *c = raiz;
  while (!c->folha) {
    if (verbose) {
      printf("[");
      for (i = 0; i < c->num_chaves - 1; i++)
        printf("%d ", c->chaves[i]);
      printf("%d] ", c->chaves[i]);
    }
    i = 0;
    while (i < c->num_chaves) {
      if (key >= c->chaves[i])
        i++;
      else
        break;
    }
    if (verbose)
      printf("%d ->\n", i);
    c = (No *)c->filhos[i];
  }
  if (verbose) {
    printf("Leaf [");
    for (i = 0; i < c->num_chaves - 1; i++)
      printf("%d ", c->chaves[i]);
    printf("%d] ->\n", c->chaves[i]);
  }
  return c;
}

record *find(No *raiz, int key, bool verbose, No **leaf_out) {
  if (raiz == NULL) {
    if (leaf_out != NULL) {
      *leaf_out = NULL;
    }
    return NULL;
  }

  int i = 0;
  No *leaf = NULL;

  leaf = findLeaf(raiz, key, verbose);

  for (i = 0; i < leaf->num_chaves; i++)
    if (leaf->chaves[i] == key)
      break;
  if (leaf_out != NULL) {
    *leaf_out = leaf;
  }
  if (i == leaf->num_chaves)
    return NULL;
  else
    return (record *)leaf->filhos[i];
}

int cut(int length) {
  if (length % 2 == 0)
    return length / 2;
  else
    return length / 2 + 1;
}

record *makeRecord(int value) {
  record *new_record = (record *)malloc(sizeof(record));
  if (new_record == NULL) {
    perror("Record creation.");
    exit(EXIT_FAILURE);
  } else {
    new_record->value = value;
  }
  return new_record;
}

No *makeNo(void) {
  No *new_No;
  new_No = malloc(sizeof(No));
  if (new_No == NULL) {
    perror("No creation.");
    exit(EXIT_FAILURE);
  }
  new_No->chaves = malloc((order - 1) * sizeof(int));
  if (new_No->chaves == NULL) {
    perror("New No chaves array.");
    exit(EXIT_FAILURE);
  }
  new_No->filhos = malloc(order * sizeof(void *));
  if (new_No->filhos == NULL) {
    perror("New No filhos array.");
    exit(EXIT_FAILURE);
  }
  new_No->folha = false;
  new_No->num_chaves = 0;
  new_No->parent = NULL;
  new_No->prox = NULL;
  return new_No;
}

No *makeLeaf(void) {
  No *leaf = makeNo();
  leaf->folha = true;
  return leaf;
}

int getLeftIndex(No *parent, No *left) {
  int left_index = 0;
  while (left_index <= parent->num_chaves &&
       parent->filhos[left_index] != left)
    left_index++;
  return left_index;
}

No *insertIntoLeaf(No *leaf, int key, record *pointer) {
  int i, insertion_point;

  insertion_point = 0;
  while (insertion_point < leaf->num_chaves && leaf->chaves[insertion_point] < key)
    insertion_point++;

  for (i = leaf->num_chaves; i > insertion_point; i--) {
    leaf->chaves[i] = leaf->chaves[i - 1];
    leaf->filhos[i] = leaf->filhos[i - 1];
  }
  leaf->chaves[insertion_point] = key;
  leaf->filhos[insertion_point] = pointer;
  leaf->num_chaves++;
  return leaf;
}

No *insertIntoLeafAfterSplitting(No *raiz, No *leaf, int key, record *pointer) {
  No *new_leaf;
  int *temp_chaves;
  void **temp_filhos;
  int insertion_index, split, new_key, i, j;

  new_leaf = makeLeaf();

  temp_chaves = malloc(order * sizeof(int));
  if (temp_chaves == NULL) {
    perror("Temporary chaves array.");
    exit(EXIT_FAILURE);
  }

  temp_filhos = malloc(order * sizeof(void *));
  if (temp_filhos == NULL) {
    perror("Temporary filhos array.");
    exit(EXIT_FAILURE);
  }

  insertion_index = 0;
  while (insertion_index < order - 1 && leaf->chaves[insertion_index] < key)
    insertion_index++;

  for (i = 0, j = 0; i < leaf->num_chaves; i++, j++) {
    if (j == insertion_index)
      j++;
    temp_chaves[j] = leaf->chaves[i];
    temp_filhos[j] = leaf->filhos[i];
  }

  temp_chaves[insertion_index] = key;
  temp_filhos[insertion_index] = pointer;

  leaf->num_chaves = 0;

  split = cut(order - 1);

  for (i = 0; i < split; i++) {
    leaf->filhos[i] = temp_filhos[i];
    leaf->chaves[i] = temp_chaves[i];
    leaf->num_chaves++;
  }

  for (i = split, j = 0; i < order; i++, j++) {
    new_leaf->filhos[j] = temp_filhos[i];
    new_leaf->chaves[j] = temp_chaves[i];
    new_leaf->num_chaves++;
  }

  free(temp_filhos);
  free(temp_chaves);

  new_leaf->filhos[order - 1] = leaf->filhos[order - 1];
  leaf->filhos[order - 1] = new_leaf;

  for (i = leaf->num_chaves; i < order - 1; i++)
    leaf->filhos[i] = NULL;
  for (i = new_leaf->num_chaves; i < order - 1; i++)
    new_leaf->filhos[i] = NULL;

  new_leaf->parent = leaf->parent;
  new_key = new_leaf->chaves[0];

  return insertIntoParent(raiz, leaf, new_key, new_leaf);
}

No *insertIntoNo(No *raiz, No *n,
           int left_index, int key, No *right) {
  int i;

  for (i = n->num_chaves; i > left_index; i--) {
    n->filhos[i + 1] = n->filhos[i];
    n->chaves[i] = n->chaves[i - 1];
  }
  n->filhos[left_index + 1] = right;
  n->chaves[left_index] = key;
  n->num_chaves++;
  return raiz;
}

No *insertIntoNoAfterSplitting(No *raiz, No *old_No, int left_index,
                   int key, No *right) {
  int i, j, split, k_prime;
  No *new_No, *filho;
  int *temp_chaves;
  No **temp_filhos;

  temp_filhos = malloc((order + 1) * sizeof(No *));
  if (temp_filhos == NULL) {
    exit(EXIT_FAILURE);
  }
  temp_chaves = malloc(order * sizeof(int));
  if (temp_chaves == NULL) {
    exit(EXIT_FAILURE);
  }

  for (i = 0, j = 0; i < old_No->num_chaves + 1; i++, j++) {
    if (j == left_index + 1)
      j++;
    temp_filhos[j] = old_No->filhos[i];
  }

  for (i = 0, j = 0; i < old_No->num_chaves; i++, j++) {
    if (j == left_index)
      j++;
    temp_chaves[j] = old_No->chaves[i];
  }

  temp_filhos[left_index + 1] = right;
  temp_chaves[left_index] = key;

  split = cut(order);
  new_No = makeNo();
  old_No->num_chaves = 0;
  for (i = 0; i < split - 1; i++) {
    old_No->filhos[i] = temp_filhos[i];
    old_No->chaves[i] = temp_chaves[i];
    old_No->num_chaves++;
  }
  old_No->filhos[i] = temp_filhos[i];
  k_prime = temp_chaves[split - 1];
  for (++i, j = 0; i < order; i++, j++) {
    new_No->filhos[j] = temp_filhos[i];
    new_No->chaves[j] = temp_chaves[i];
    new_No->num_chaves++;
  }
  new_No->filhos[j] = temp_filhos[i];
  free(temp_filhos);
  free(temp_chaves);
  new_No->parent = old_No->parent;
  for (i = 0; i <= new_No->num_chaves; i++) {
    filho = new_No->filhos[i];
    filho->parent = new_No;
  }

  return insertIntoParent(raiz, old_No, k_prime, new_No);
}

No *insertIntoParent(No *raiz, No *left, int key, No *right) {
  int left_index;
  No *parent;

  parent = left->parent;

  if (parent == NULL)
    return insertIntoNewraiz(left, key, right);

  left_index = getLeftIndex(parent, left);

  if (parent->num_chaves < order - 1)
    return insertIntoNo(raiz, parent, left_index, key, right);

  return insertIntoNoAfterSplitting(raiz, parent, left_index, key, right);
}

No *insertIntoNewraiz(No *left, int key, No *right) {
  No *raiz = makeNo();
  raiz->chaves[0] = key;
  raiz->filhos[0] = left;
  raiz->filhos[1] = right;
  raiz->num_chaves++;
  raiz->parent = NULL;
  left->parent = raiz;
  right->parent = raiz;
  return raiz;
}

No *startNewTree(int key, record *pointer) {
  No *raiz = makeLeaf();
  raiz->chaves[0] = key;
  raiz->filhos[0] = pointer;
  raiz->filhos[order - 1] = NULL;
  raiz->parent = NULL;
  raiz->num_chaves++;
  return raiz;
}

No *insert(No *raiz, int key, int value) {
  record *record_pointer = NULL;
  No *leaf = NULL;

  record_pointer = find(raiz, key, false, NULL);
  if (record_pointer != NULL) {
    record_pointer->value = value;
    return raiz;
  }

  record_pointer = makeRecord(value);

  if (raiz == NULL)
    return startNewTree(key, record_pointer);

  leaf = findLeaf(raiz, key, false);

  if (leaf->num_chaves < order - 1) {
    leaf = insertIntoLeaf(leaf, key, record_pointer);
    return raiz;
  }

  return insertIntoLeafAfterSplitting(raiz, leaf, key, record_pointer);
}









/* ==== IMPLEMENTA��O DE DELE��O ==== */

/* Remove chave 'key' da �rvore; se n�o existir, retorna raiz inalterada. */
No *deleteKey(No *raiz, int key) {
  No *leaf = NULL;
  record *r = find(raiz, key, false, &leaf);
  if (r == NULL || leaf == NULL) return raiz; // n�o existe

  // Em folha: 'pointer' � o pr�prio record armazenado
  return deleteEntry(raiz, leaf, key, (void *)r);
}

/* Remove (key,pointer) do n� n e resolve underflow (empr�stimo/fus�o). */
static No *deleteEntry(No *raiz, No *n, int key, void *pointer) {
  int min_chaves, neighbor_index, k_prime_index, k_prime;
  No *neighbor;
  /* 1) Remove a entrada do n� */
  removeEntryFromNo(n, key, pointer);

  /* 2) Caso base: se n � raiz, pode reduzir altura ou ficar vazia */
  if (n == raiz)
    return adjustraiz(raiz);

  /* 3) Verifica se n ainda satisfaz o m�nimo de chaves */
  if (n->folha)
    min_chaves = cut(order - 1) - 1;     // ceil((order-1)/2) - 1
  else
    min_chaves = cut(order) - 1;         // ceil(order/2) - 1

  if (n->num_chaves >= min_chaves)
    return raiz; // OK, nada a fazer

  /* 4) Caso underflow: tenta emprestar do vizinho; sen�o, funde */
  neighbor_index = getNeighborIndexFor(n);
  k_prime_index = (neighbor_index == -1) ? 0 : neighbor_index;
  neighbor = (neighbor_index == -1)
               ? (No *)n->parent->filhos[1]
               : (No *)n->parent->filhos[neighbor_index];
  k_prime = n->parent->chaves[k_prime_index];

  /* Capacidade ap�s fus�o para decidir coalesce x redistribute */
  int capacity = n->folha ? order : (order - 1);

  if (neighbor->num_chaves + n->num_chaves < capacity) {
    /* Fus�o (coalesce) com vizinho */
    return coalesceNos(raiz, n, neighbor, neighbor_index, k_prime);
  } else {
    /* Empr�stimo (redistribute) */
    return redistributeNos(raiz, n, neighbor, neighbor_index, k_prime_index, k_prime);
  }
}

/* Remove do n� n a chave e o ponteiro correspondentes, compactando os arrays. */
static void removeEntryFromNo(No *n, int key, void *pointer) {
  int i, j;

  // Remove a chave
  i = 0;
  while (i < n->num_chaves && n->chaves[i] != key) i++;
  for (j = i + 1; j < n->num_chaves; j++)
    n->chaves[j - 1] = n->chaves[j];

  // Remove o ponteiro (em folha: mesmo �ndice; em interno: procura ponteiro exato)
  if (n->folha) {
    for (j = i + 1; j < n->num_chaves + 1; j++)
      n->filhos[j - 1] = n->filhos[j];
  } else {
    i = 0;
    while (i <= n->num_chaves && n->filhos[i] != pointer) i++;
    for (j = i + 1; j <= n->num_chaves; j++)
      n->filhos[j - 1] = n->filhos[j];
  }

  n->num_chaves--;
  // Em folhas, n�o mexer em filhos[order-1] (prox); ser� ajustado em fus�o quando preciso
}

/* Se a raiz ficou com 0 chaves, reduz altura (ou zera �rvore). */
static No *adjustraiz(No *raiz) {
  if (raiz->num_chaves > 0) return raiz;

  No *new_raiz;
  if (!raiz->folha) {
    // Primeiro filho vira nova raiz
    new_raiz = (No *)raiz->filhos[0];
    new_raiz->parent = NULL;
  } else {
    // �rvore vazia
    new_raiz = NULL;
  }
  free(raiz->chaves);
  free(raiz->filhos);
  free(raiz);
  return new_raiz;
}

/* Retorna o �ndice do vizinho � esquerda; -1 se n � o filho mais � esquerda. */
static int getNeighborIndexFor(No *n) {
  No *parent = n->parent;
  int i;
  for (i = 0; i <= parent->num_chaves; i++)
    if (parent->filhos[i] == n) return i - 1;
  return -1; // n�o deveria ocorrer
}

/* Fus�o de n com neighbor; remove k_prime do pai. */
static No *coalesceNos(No *raiz, No *n, No *neighbor,
                           int neighbor_index, int k_prime) {
  int i, j, neighbor_inserts;
  No *tmp;

  /* Garante neighbor � esquerda; se n�o, troca */
  if (neighbor_index == -1) {
    tmp = n;
    n = neighbor;
    neighbor = tmp;
  }

  /* Em folha: concatenar chaves e ponteiros; ajustar encadeamento */
  if (n->folha) {
    for (i = 0, j = neighbor->num_chaves; i < n->num_chaves; i++, j++) {
      neighbor->chaves[j] = n->chaves[i];
      neighbor->filhos[j] = n->filhos[i];
    }
    neighbor->num_chaves += n->num_chaves;
    neighbor->filhos[order - 1] = n->filhos[order - 1]; // mant�m lista encadeada de folhas
  }
  /* Em interno: move k_prime e todos os pares de n para neighbor */
  else {
    // Insere chave separadora do pai
    neighbor->chaves[neighbor->num_chaves] = k_prime;
    neighbor->filhos[neighbor->num_chaves + 1] = n->filhos[0];
    if (neighbor->filhos[neighbor->num_chaves + 1])
      ((No *)neighbor->filhos[neighbor->num_chaves + 1])->parent = neighbor;
    neighbor->num_chaves++;

    // Copia as demais chaves/ponteiros de n
    for (i = 0; i < n->num_chaves; i++) {
      neighbor->chaves[neighbor->num_chaves] = n->chaves[i];
      neighbor->filhos[neighbor->num_chaves + 1] = n->filhos[i + 1];
      if (neighbor->filhos[neighbor->num_chaves + 1])
        ((No *)neighbor->filhos[neighbor->num_chaves + 1])->parent = neighbor;
      neighbor->num_chaves++;
    }
  }

  // Remove k_prime e o ponteiro 'n' do pai
  raiz = deleteEntry(raiz, n->parent, k_prime, (void *)n);

  // Libera n
  free(n->chaves);
  free(n->filhos);
  free(n);

  return raiz;
}

/* Empr�stimo de uma chave do vizinho (esq/dir) para sanar underflow. */
static No *redistributeNos(No *raiz, No *n, No *neighbor,int neighbor_index, int k_prime_index, int k_prime) {

  int i;

  /* Vizinho � esquerda */
  if (neighbor_index != -1) {
    if (!n->folha) {
      // Desloca em n para abrir espa�o
      for (i = n->num_chaves; i > 0; i--) {
        n->filhos[i + 1] = n->filhos[i];
        n->chaves[i] = n->chaves[i - 1];
      }
      n->filhos[1] = n->filhos[0];

      // Move ponteiro do vizinho e k_prime para n
      n->filhos[0] = neighbor->filhos[neighbor->num_chaves];
      if (n->filhos[0])
        ((No *)n->filhos[0])->parent = n;

      n->chaves[0] = k_prime;

      // Atualiza k_prime do pai
      n->parent->chaves[k_prime_index] = neighbor->chaves[neighbor->num_chaves - 1];

      neighbor->filhos[neighbor->num_chaves] = NULL;
      neighbor->chaves[neighbor->num_chaves - 1] = 0;
      neighbor->num_chaves--;
      n->num_chaves++;
    } else {
      // Folha: puxa a �ltima chave do vizinho
      for (i = n->num_chaves; i > 0; i--) {
        n->chaves[i] = n->chaves[i - 1];
        n->filhos[i] = n->filhos[i - 1];
      }
      n->chaves[0] = neighbor->chaves[neighbor->num_chaves - 1];
      n->filhos[0] = neighbor->filhos[neighbor->num_chaves - 1];
      neighbor->num_chaves--;
      n->num_chaves++;

      // Atualiza separadora no pai
      n->parent->chaves[k_prime_index] = n->chaves[0];
    }
  }
  /* Vizinho � direita */
  else {
    if (n->folha) {
      // Move a primeira chave do vizinho para o fim de n
      n->chaves[n->num_chaves] = neighbor->chaves[0];
      n->filhos[n->num_chaves] = neighbor->filhos[0];
      n->num_chaves++;

      // Compacta vizinho
      for (i = 0; i < neighbor->num_chaves - 1; i++) {
        neighbor->chaves[i] = neighbor->chaves[i + 1];
        neighbor->filhos[i] = neighbor->filhos[i + 1];
      }
      neighbor->num_chaves--;

      // Atualiza separadora no pai
      n->parent->chaves[k_prime_index] = neighbor->chaves[0];
    } else {
      // Move k_prime do pai para o fim de n
      n->chaves[n->num_chaves] = k_prime;
      n->filhos[n->num_chaves + 1] = neighbor->filhos[0];
      if (n->filhos[n->num_chaves + 1])
        ((No *)n->filhos[n->num_chaves + 1])->parent = n;
      n->num_chaves++;

      // Move primeira chave do vizinho para o pai
      n->parent->chaves[k_prime_index] = neighbor->chaves[0];

      // Compacta vizinho
      neighbor->filhos[0] = neighbor->filhos[1];
      for (i = 0; i < neighbor->num_chaves - 1; i++) {
        neighbor->chaves[i] = neighbor->chaves[i + 1];
        neighbor->filhos[i + 1] = neighbor->filhos[i + 2];
      }
      neighbor->num_chaves--;
    }
  }
  return raiz;
}



// ==============================
// PRETTY PRINT ROTACIONADA (I/L)
// ==============================
static void pp_indent(int level) {
    for (int i = 0; i < level * 4; ++i) putchar(' ');
}

static void printTreeRecRot(const No *n, int level) {
    if (!n) return;

    // se for interno, visita primeiro o filho mais � direita
    if (!n->folha && n->num_chaves >= 0)
        printTreeRecRot((const No*)n->filhos[n->num_chaves], level + 1);

    // da direita para a esquerda: imprime chave e sub�rvore � esquerda
    for (int i = n->num_chaves - 1; i >= 0; --i) {
        pp_indent(level);
        printf("%c %d\n", n->folha ? 'L' : 'I', n->chaves[i]);
        if (!n->folha)
            printTreeRecRot((const No*)n->filhos[i], level + 1);
    }
}

void printTreeRotated(const No *raiz) {
    printf("\nB+ TREE (ROTACIONADA 90� � ESQUERDA)\n");
    printf("MAIORES � ESQUERDA/TOPO, MENORES � DIREITA/BAIXO.\n\n");
    printTreeRecRot(raiz, 0);
}


int main() {
  No *raiz;
  char instruction;

  raiz = NULL;

  /*
  raiz = insert(raiz, 5, 33);
  raiz = insert(raiz, 15, 21);
  raiz = insert(raiz, 25, 31);
  raiz = insert(raiz, 35, 41);
  raiz = insert(raiz, 45, 10);
    */

    for(int i = 0 ; i < 100; i ++)
    {
        raiz = insert(raiz,i,i);
    }
    //printf("=====\n");
    //printTree(raiz);

    findAndPrint(raiz, 15, instruction = 'a');


    raiz = deleteKey(raiz, 25);
    //printTree(raiz);




    raiz = deleteKey(raiz, 49);
    //printf("=====\n");
   // printTree(raiz);
    findAndPrint(raiz, 88, instruction = 'a');

    printTreeRotated(raiz);


}
