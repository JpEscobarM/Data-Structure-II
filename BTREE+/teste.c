// Deletion on a B+ Tree in C

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Default order
#define ORDER 3

typedef struct record {
  int value;
} record;

// Node
typedef struct node {
  void **pointers;
  int *keys;
  struct node *parent;
  bool is_leaf;
  int num_keys;
  struct node *next;
} node;

int order = ORDER;
node *queue = NULL;
bool verbose_output = false;

// Enqueue
void enqueue(node *new_node);

// Dequeue
node *dequeue(void);
int height(node *const root);
int pathToLeaves(node *const root, node *child);
void printLeaves(node *const root);
void printTree(node *const root);
void findAndPrint(node *const root, int key, bool verbose);
void findAndPrintRange(node *const root, int range1, int range2, bool verbose);
int findRange(node *const root, int key_start, int key_end, bool verbose,
        int returned_keys[], void *returned_pointers[]);
node *findLeaf(node *const root, int key, bool verbose);
record *find(node *root, int key, bool verbose, node **leaf_out);
int cut(int length);

record *makeRecord(int value);
node *makeNode(void);
node *makeLeaf(void);
int getLeftIndex(node *parent, node *left);
node *insertIntoLeaf(node *leaf, int key, record *pointer);
node *insertIntoLeafAfterSplitting(node *root, node *leaf, int key,
                   record *pointer);
node *insertIntoNode(node *root, node *parent,
           int left_index, int key, node *right);
node *insertIntoNodeAfterSplitting(node *root, node *parent,
                   int left_index,
                   int key, node *right);
node *insertIntoParent(node *root, node *left, int key, node *right);
node *insertIntoNewRoot(node *left, int key, node *right);
node *startNewTree(int key, record *pointer);
node *insert(node *root, int key, int value);
/*========================================================================  */




/* ==== PROTÓTIPOS - FUNÇÕES DE DELEÇÃO=== */
node *deleteKey(node *root, int key);
static node *deleteEntry(node *root, node *n, int key, void *pointer);
static node *adjustRoot(node *root);
static int getNeighborIndexFor(node *n);
static node *coalesceNodes(node *root, node *n, node *neighbor,
                           int neighbor_index, int k_prime);
static node *redistributeNodes(node *root, node *n, node *neighbor,
                               int neighbor_index, int k_prime_index, int k_prime);
static void removeEntryFromNode(node *n, int key, void *pointer);
/*========================================================================  */


// Enqueue
void enqueue(node *new_node) {
  node *c;
  if (queue == NULL) {
    queue = new_node;
    queue->next = NULL;
  } else {
    c = queue;
    while (c->next != NULL) {
      c = c->next;
    }
    c->next = new_node;
    new_node->next = NULL;
  }
}

// Dequeue
node *dequeue(void) {
  node *n = queue;
  queue = queue->next;
  n->next = NULL;
  return n;
}

// Print the leaves
void printLeaves(node *const root) {
  if (root == NULL) {
    printf("Empty tree.\n");
    return;
  }
  int i;
  node *c = root;
  while (!c->is_leaf)
    c = c->pointers[0];
  while (true) {
    for (i = 0; i < c->num_keys; i++) {
      if (verbose_output)
        printf("%p ", c->pointers[i]);
      printf("%d ", c->keys[i]);
    }
    if (verbose_output)
      printf("%p ", c->pointers[order - 1]);
    if (c->pointers[order - 1] != NULL) {
      printf(" | ");
      c = c->pointers[order - 1];
    } else
      break;
  }
  printf("\n");
}

// Calculate height
int height(node *const root) {
  int h = 0;
  node *c = root;
  while (!c->is_leaf) {
    c = c->pointers[0];
    h++;
  }
  return h;
}

// Get path to root
int pathToLeaves(node *const root, node *child) {
  int length = 0;
  node *c = child;
  while (c != root) {
    c = c->parent;
    length++;
  }
  return length;
}

// Print the tree
void printTree(node *const root) {
  node *n = NULL;
  int i = 0;
  int rank = 0;
  int new_rank = 0;

  if (root == NULL) {
    printf("Empty tree.\n");
    return;
  }
  queue = NULL;
  enqueue(root);
  while (queue != NULL) {
    n = dequeue();
    if (n->parent != NULL && n == n->parent->pointers[0]) {
      new_rank = pathToLeaves(root, n);
      if (new_rank != rank) {
        rank = new_rank;
        printf("\n");
      }
    }
    if (verbose_output)
      printf("(%p)", n);
    for (i = 0; i < n->num_keys; i++) {
      if (verbose_output)
        printf("%p ", n->pointers[i]);
      printf("%d ", n->keys[i]);
    }
    if (!n->is_leaf)
      for (i = 0; i <= n->num_keys; i++)
        enqueue(n->pointers[i]);
    if (verbose_output) {
      if (n->is_leaf)
        printf("%p ", n->pointers[order - 1]);
      else
        printf("%p ", n->pointers[n->num_keys]);
    }
    printf("| ");
  }
  printf("\n");
}

// Find the node and print it
void findAndPrint(node *const root, int key, bool verbose) {
  node *leaf = NULL;
  record *r = find(root, key, verbose, NULL);
  if (r == NULL)
    printf("Record not found under key %d.\n", key);
  else
    printf("Record at %p -- key %d, value %d.\n",
         r, key, r->value);
}

// Find and print the range
void findAndPrintRange(node *const root, int key_start, int key_end,
             bool verbose) {
  int i;
  int array_size = key_end - key_start + 1;
  int returned_keys[array_size];
  void *returned_pointers[array_size];
  int num_found = findRange(root, key_start, key_end, verbose,
                returned_keys, returned_pointers);
  if (!num_found)
    printf("None found.\n");
  else {
    for (i = 0; i < num_found; i++)
      printf("Key: %d   Location: %p  Value: %d\n",
           returned_keys[i],
           returned_pointers[i],
           ((record *)
            returned_pointers[i])
             ->value);
  }
}

// Find the range
int findRange(node *const root, int key_start, int key_end, bool verbose,
        int returned_keys[], void *returned_pointers[]) {
  int i, num_found;
  num_found = 0;
  node *n = findLeaf(root, key_start, verbose);
  if (n == NULL)
    return 0;
  for (i = 0; i < n->num_keys && n->keys[i] < key_start; i++)
    ;
  if (i == n->num_keys)
    return 0;
  while (n != NULL) {
    for (; i < n->num_keys && n->keys[i] <= key_end; i++) {
      returned_keys[num_found] = n->keys[i];
      returned_pointers[num_found] = n->pointers[i];
      num_found++;
    }
    n = n->pointers[order - 1];
    i = 0;
  }
  return num_found;
}

// Find the leaf
node *findLeaf(node *const root, int key, bool verbose) {
  if (root == NULL) {
    if (verbose)
      printf("Empty tree.\n");
    return root;
  }
  int i = 0;
  node *c = root;
  while (!c->is_leaf) {
    if (verbose) {
      printf("[");
      for (i = 0; i < c->num_keys - 1; i++)
        printf("%d ", c->keys[i]);
      printf("%d] ", c->keys[i]);
    }
    i = 0;
    while (i < c->num_keys) {
      if (key >= c->keys[i])
        i++;
      else
        break;
    }
    if (verbose)
      printf("%d ->\n", i);
    c = (node *)c->pointers[i];
  }
  if (verbose) {
    printf("Leaf [");
    for (i = 0; i < c->num_keys - 1; i++)
      printf("%d ", c->keys[i]);
    printf("%d] ->\n", c->keys[i]);
  }
  return c;
}

record *find(node *root, int key, bool verbose, node **leaf_out) {
  if (root == NULL) {
    if (leaf_out != NULL) {
      *leaf_out = NULL;
    }
    return NULL;
  }

  int i = 0;
  node *leaf = NULL;

  leaf = findLeaf(root, key, verbose);

  for (i = 0; i < leaf->num_keys; i++)
    if (leaf->keys[i] == key)
      break;
  if (leaf_out != NULL) {
    *leaf_out = leaf;
  }
  if (i == leaf->num_keys)
    return NULL;
  else
    return (record *)leaf->pointers[i];
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

node *makeNode(void) {
  node *new_node;
  new_node = malloc(sizeof(node));
  if (new_node == NULL) {
    perror("Node creation.");
    exit(EXIT_FAILURE);
  }
  new_node->keys = malloc((order - 1) * sizeof(int));
  if (new_node->keys == NULL) {
    perror("New node keys array.");
    exit(EXIT_FAILURE);
  }
  new_node->pointers = malloc(order * sizeof(void *));
  if (new_node->pointers == NULL) {
    perror("New node pointers array.");
    exit(EXIT_FAILURE);
  }
  new_node->is_leaf = false;
  new_node->num_keys = 0;
  new_node->parent = NULL;
  new_node->next = NULL;
  return new_node;
}

node *makeLeaf(void) {
  node *leaf = makeNode();
  leaf->is_leaf = true;
  return leaf;
}

int getLeftIndex(node *parent, node *left) {
  int left_index = 0;
  while (left_index <= parent->num_keys &&
       parent->pointers[left_index] != left)
    left_index++;
  return left_index;
}

node *insertIntoLeaf(node *leaf, int key, record *pointer) {
  int i, insertion_point;

  insertion_point = 0;
  while (insertion_point < leaf->num_keys && leaf->keys[insertion_point] < key)
    insertion_point++;

  for (i = leaf->num_keys; i > insertion_point; i--) {
    leaf->keys[i] = leaf->keys[i - 1];
    leaf->pointers[i] = leaf->pointers[i - 1];
  }
  leaf->keys[insertion_point] = key;
  leaf->pointers[insertion_point] = pointer;
  leaf->num_keys++;
  return leaf;
}

node *insertIntoLeafAfterSplitting(node *root, node *leaf, int key, record *pointer) {
  node *new_leaf;
  int *temp_keys;
  void **temp_pointers;
  int insertion_index, split, new_key, i, j;

  new_leaf = makeLeaf();

  temp_keys = malloc(order * sizeof(int));
  if (temp_keys == NULL) {
    perror("Temporary keys array.");
    exit(EXIT_FAILURE);
  }

  temp_pointers = malloc(order * sizeof(void *));
  if (temp_pointers == NULL) {
    perror("Temporary pointers array.");
    exit(EXIT_FAILURE);
  }

  insertion_index = 0;
  while (insertion_index < order - 1 && leaf->keys[insertion_index] < key)
    insertion_index++;

  for (i = 0, j = 0; i < leaf->num_keys; i++, j++) {
    if (j == insertion_index)
      j++;
    temp_keys[j] = leaf->keys[i];
    temp_pointers[j] = leaf->pointers[i];
  }

  temp_keys[insertion_index] = key;
  temp_pointers[insertion_index] = pointer;

  leaf->num_keys = 0;

  split = cut(order - 1);

  for (i = 0; i < split; i++) {
    leaf->pointers[i] = temp_pointers[i];
    leaf->keys[i] = temp_keys[i];
    leaf->num_keys++;
  }

  for (i = split, j = 0; i < order; i++, j++) {
    new_leaf->pointers[j] = temp_pointers[i];
    new_leaf->keys[j] = temp_keys[i];
    new_leaf->num_keys++;
  }

  free(temp_pointers);
  free(temp_keys);

  new_leaf->pointers[order - 1] = leaf->pointers[order - 1];
  leaf->pointers[order - 1] = new_leaf;

  for (i = leaf->num_keys; i < order - 1; i++)
    leaf->pointers[i] = NULL;
  for (i = new_leaf->num_keys; i < order - 1; i++)
    new_leaf->pointers[i] = NULL;

  new_leaf->parent = leaf->parent;
  new_key = new_leaf->keys[0];

  return insertIntoParent(root, leaf, new_key, new_leaf);
}

node *insertIntoNode(node *root, node *n,
           int left_index, int key, node *right) {
  int i;

  for (i = n->num_keys; i > left_index; i--) {
    n->pointers[i + 1] = n->pointers[i];
    n->keys[i] = n->keys[i - 1];
  }
  n->pointers[left_index + 1] = right;
  n->keys[left_index] = key;
  n->num_keys++;
  return root;
}

node *insertIntoNodeAfterSplitting(node *root, node *old_node, int left_index,
                   int key, node *right) {
  int i, j, split, k_prime;
  node *new_node, *child;
  int *temp_keys;
  node **temp_pointers;

  temp_pointers = malloc((order + 1) * sizeof(node *));
  if (temp_pointers == NULL) {
    exit(EXIT_FAILURE);
  }
  temp_keys = malloc(order * sizeof(int));
  if (temp_keys == NULL) {
    exit(EXIT_FAILURE);
  }

  for (i = 0, j = 0; i < old_node->num_keys + 1; i++, j++) {
    if (j == left_index + 1)
      j++;
    temp_pointers[j] = old_node->pointers[i];
  }

  for (i = 0, j = 0; i < old_node->num_keys; i++, j++) {
    if (j == left_index)
      j++;
    temp_keys[j] = old_node->keys[i];
  }

  temp_pointers[left_index + 1] = right;
  temp_keys[left_index] = key;

  split = cut(order);
  new_node = makeNode();
  old_node->num_keys = 0;
  for (i = 0; i < split - 1; i++) {
    old_node->pointers[i] = temp_pointers[i];
    old_node->keys[i] = temp_keys[i];
    old_node->num_keys++;
  }
  old_node->pointers[i] = temp_pointers[i];
  k_prime = temp_keys[split - 1];
  for (++i, j = 0; i < order; i++, j++) {
    new_node->pointers[j] = temp_pointers[i];
    new_node->keys[j] = temp_keys[i];
    new_node->num_keys++;
  }
  new_node->pointers[j] = temp_pointers[i];
  free(temp_pointers);
  free(temp_keys);
  new_node->parent = old_node->parent;
  for (i = 0; i <= new_node->num_keys; i++) {
    child = new_node->pointers[i];
    child->parent = new_node;
  }

  return insertIntoParent(root, old_node, k_prime, new_node);
}

node *insertIntoParent(node *root, node *left, int key, node *right) {
  int left_index;
  node *parent;

  parent = left->parent;

  if (parent == NULL)
    return insertIntoNewRoot(left, key, right);

  left_index = getLeftIndex(parent, left);

  if (parent->num_keys < order - 1)
    return insertIntoNode(root, parent, left_index, key, right);

  return insertIntoNodeAfterSplitting(root, parent, left_index, key, right);
}

node *insertIntoNewRoot(node *left, int key, node *right) {
  node *root = makeNode();
  root->keys[0] = key;
  root->pointers[0] = left;
  root->pointers[1] = right;
  root->num_keys++;
  root->parent = NULL;
  left->parent = root;
  right->parent = root;
  return root;
}

node *startNewTree(int key, record *pointer) {
  node *root = makeLeaf();
  root->keys[0] = key;
  root->pointers[0] = pointer;
  root->pointers[order - 1] = NULL;
  root->parent = NULL;
  root->num_keys++;
  return root;
}

node *insert(node *root, int key, int value) {
  record *record_pointer = NULL;
  node *leaf = NULL;

  record_pointer = find(root, key, false, NULL);
  if (record_pointer != NULL) {
    record_pointer->value = value;
    return root;
  }

  record_pointer = makeRecord(value);

  if (root == NULL)
    return startNewTree(key, record_pointer);

  leaf = findLeaf(root, key, false);

  if (leaf->num_keys < order - 1) {
    leaf = insertIntoLeaf(leaf, key, record_pointer);
    return root;
  }

  return insertIntoLeafAfterSplitting(root, leaf, key, record_pointer);
}









/* ==== IMPLEMENTAÇÃO DE DELEÇÃO ==== */

/* Remove chave 'key' da árvore; se não existir, retorna root inalterada. */
node *deleteKey(node *root, int key) {
  node *leaf = NULL;
  record *r = find(root, key, false, &leaf);
  if (r == NULL || leaf == NULL) return root; // não existe

  // Em folha: 'pointer' é o próprio record armazenado
  return deleteEntry(root, leaf, key, (void *)r);
}

/* Remove (key,pointer) do nó n e resolve underflow (empréstimo/fusão). */
static node *deleteEntry(node *root, node *n, int key, void *pointer) {
  int min_keys, neighbor_index, k_prime_index, k_prime;
  node *neighbor;
  /* 1) Remove a entrada do nó */
  removeEntryFromNode(n, key, pointer);

  /* 2) Caso base: se n é raiz, pode reduzir altura ou ficar vazia */
  if (n == root)
    return adjustRoot(root);

  /* 3) Verifica se n ainda satisfaz o mínimo de chaves */
  if (n->is_leaf)
    min_keys = cut(order - 1) - 1;     // ceil((order-1)/2) - 1
  else
    min_keys = cut(order) - 1;         // ceil(order/2) - 1

  if (n->num_keys >= min_keys)
    return root; // OK, nada a fazer

  /* 4) Caso underflow: tenta emprestar do vizinho; senão, funde */
  neighbor_index = getNeighborIndexFor(n);
  k_prime_index = (neighbor_index == -1) ? 0 : neighbor_index;
  neighbor = (neighbor_index == -1)
               ? (node *)n->parent->pointers[1]
               : (node *)n->parent->pointers[neighbor_index];
  k_prime = n->parent->keys[k_prime_index];

  /* Capacidade após fusão para decidir coalesce x redistribute */
  int capacity = n->is_leaf ? order : (order - 1);

  if (neighbor->num_keys + n->num_keys < capacity) {
    /* Fusão (coalesce) com vizinho */
    return coalesceNodes(root, n, neighbor, neighbor_index, k_prime);
  } else {
    /* Empréstimo (redistribute) */
    return redistributeNodes(root, n, neighbor, neighbor_index, k_prime_index, k_prime);
  }
}

/* Remove do nó n a chave e o ponteiro correspondentes, compactando os arrays. */
static void removeEntryFromNode(node *n, int key, void *pointer) {
  int i, j;

  // Remove a chave
  i = 0;
  while (i < n->num_keys && n->keys[i] != key) i++;
  for (j = i + 1; j < n->num_keys; j++)
    n->keys[j - 1] = n->keys[j];

  // Remove o ponteiro (em folha: mesmo índice; em interno: procura ponteiro exato)
  if (n->is_leaf) {
    for (j = i + 1; j < n->num_keys + 1; j++)
      n->pointers[j - 1] = n->pointers[j];
  } else {
    i = 0;
    while (i <= n->num_keys && n->pointers[i] != pointer) i++;
    for (j = i + 1; j <= n->num_keys; j++)
      n->pointers[j - 1] = n->pointers[j];
  }

  n->num_keys--;
  // Em folhas, não mexer em pointers[order-1] (next); será ajustado em fusão quando preciso
}

/* Se a raiz ficou com 0 chaves, reduz altura (ou zera árvore). */
static node *adjustRoot(node *root) {
  if (root->num_keys > 0) return root;

  node *new_root;
  if (!root->is_leaf) {
    // Primeiro filho vira nova raiz
    new_root = (node *)root->pointers[0];
    new_root->parent = NULL;
  } else {
    // Árvore vazia
    new_root = NULL;
  }
  free(root->keys);
  free(root->pointers);
  free(root);
  return new_root;
}

/* Retorna o índice do vizinho à esquerda; -1 se n é o filho mais à esquerda. */
static int getNeighborIndexFor(node *n) {
  node *parent = n->parent;
  int i;
  for (i = 0; i <= parent->num_keys; i++)
    if (parent->pointers[i] == n) return i - 1;
  return -1; // não deveria ocorrer
}

/* Fusão de n com neighbor; remove k_prime do pai. */
static node *coalesceNodes(node *root, node *n, node *neighbor,
                           int neighbor_index, int k_prime) {
  int i, j, neighbor_inserts;
  node *tmp;

  /* Garante neighbor à esquerda; se não, troca */
  if (neighbor_index == -1) {
    tmp = n;
    n = neighbor;
    neighbor = tmp;
  }

  /* Em folha: concatenar chaves e ponteiros; ajustar encadeamento */
  if (n->is_leaf) {
    for (i = 0, j = neighbor->num_keys; i < n->num_keys; i++, j++) {
      neighbor->keys[j] = n->keys[i];
      neighbor->pointers[j] = n->pointers[i];
    }
    neighbor->num_keys += n->num_keys;
    neighbor->pointers[order - 1] = n->pointers[order - 1]; // mantém lista encadeada de folhas
  }
  /* Em interno: move k_prime e todos os pares de n para neighbor */
  else {
    // Insere chave separadora do pai
    neighbor->keys[neighbor->num_keys] = k_prime;
    neighbor->pointers[neighbor->num_keys + 1] = n->pointers[0];
    if (neighbor->pointers[neighbor->num_keys + 1])
      ((node *)neighbor->pointers[neighbor->num_keys + 1])->parent = neighbor;
    neighbor->num_keys++;

    // Copia as demais chaves/ponteiros de n
    for (i = 0; i < n->num_keys; i++) {
      neighbor->keys[neighbor->num_keys] = n->keys[i];
      neighbor->pointers[neighbor->num_keys + 1] = n->pointers[i + 1];
      if (neighbor->pointers[neighbor->num_keys + 1])
        ((node *)neighbor->pointers[neighbor->num_keys + 1])->parent = neighbor;
      neighbor->num_keys++;
    }
  }

  // Remove k_prime e o ponteiro 'n' do pai
  root = deleteEntry(root, n->parent, k_prime, (void *)n);

  // Libera n
  free(n->keys);
  free(n->pointers);
  free(n);

  return root;
}

/* Empréstimo de uma chave do vizinho (esq/dir) para sanar underflow. */
static node *redistributeNodes(node *root, node *n, node *neighbor,
                               int neighbor_index, int k_prime_index, int k_prime) {
  int i;

  /* Vizinho à esquerda */
  if (neighbor_index != -1) {
    if (!n->is_leaf) {
      // Desloca em n para abrir espaço
      for (i = n->num_keys; i > 0; i--) {
        n->pointers[i + 1] = n->pointers[i];
        n->keys[i] = n->keys[i - 1];
      }
      n->pointers[1] = n->pointers[0];

      // Move ponteiro do vizinho e k_prime para n
      n->pointers[0] = neighbor->pointers[neighbor->num_keys];
      if (n->pointers[0])
        ((node *)n->pointers[0])->parent = n;

      n->keys[0] = k_prime;

      // Atualiza k_prime do pai
      n->parent->keys[k_prime_index] = neighbor->keys[neighbor->num_keys - 1];

      neighbor->pointers[neighbor->num_keys] = NULL;
      neighbor->keys[neighbor->num_keys - 1] = 0;
      neighbor->num_keys--;
      n->num_keys++;
    } else {
      // Folha: puxa a última chave do vizinho
      for (i = n->num_keys; i > 0; i--) {
        n->keys[i] = n->keys[i - 1];
        n->pointers[i] = n->pointers[i - 1];
      }
      n->keys[0] = neighbor->keys[neighbor->num_keys - 1];
      n->pointers[0] = neighbor->pointers[neighbor->num_keys - 1];
      neighbor->num_keys--;
      n->num_keys++;

      // Atualiza separadora no pai
      n->parent->keys[k_prime_index] = n->keys[0];
    }
  }
  /* Vizinho à direita */
  else {
    if (n->is_leaf) {
      // Move a primeira chave do vizinho para o fim de n
      n->keys[n->num_keys] = neighbor->keys[0];
      n->pointers[n->num_keys] = neighbor->pointers[0];
      n->num_keys++;

      // Compacta vizinho
      for (i = 0; i < neighbor->num_keys - 1; i++) {
        neighbor->keys[i] = neighbor->keys[i + 1];
        neighbor->pointers[i] = neighbor->pointers[i + 1];
      }
      neighbor->num_keys--;

      // Atualiza separadora no pai
      n->parent->keys[k_prime_index] = neighbor->keys[0];
    } else {
      // Move k_prime do pai para o fim de n
      n->keys[n->num_keys] = k_prime;
      n->pointers[n->num_keys + 1] = neighbor->pointers[0];
      if (n->pointers[n->num_keys + 1])
        ((node *)n->pointers[n->num_keys + 1])->parent = n;
      n->num_keys++;

      // Move primeira chave do vizinho para o pai
      n->parent->keys[k_prime_index] = neighbor->keys[0];

      // Compacta vizinho
      neighbor->pointers[0] = neighbor->pointers[1];
      for (i = 0; i < neighbor->num_keys - 1; i++) {
        neighbor->keys[i] = neighbor->keys[i + 1];
        neighbor->pointers[i + 1] = neighbor->pointers[i + 2];
      }
      neighbor->num_keys--;
    }
  }
  return root;
}



int main() {
  node *root;
  char instruction;

  root = NULL;

  root = insert(root, 5, 33);
  root = insert(root, 15, 21);
  root = insert(root, 25, 31);
  root = insert(root, 35, 41);
  root = insert(root, 45, 10);

     printf("=====\n");
  printTree(root);

  findAndPrint(root, 15, instruction = 'a');


    root = deleteKey(root, 25);
    printTree(root);

     root = insert(root, 49, 10);

     printf("=====\n");
     printTree(root);
      root = insert(root, 25, 31);
       root = deleteKey(root, 45);
       printf("=====\n");
     printTree(root);

     root = deleteKey(root, 49);
      printf("=====\n");
     printTree(root);
     findAndPrint(root, 49, instruction = 'a');




}
