#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct no {
    int num;
    short altura;    // 16 bits
    struct no *esq;
    struct no *dir;
} No;

short maior(short a, short b)
{

    //DATA A ALTURA DE DUAS SUBARVORES, RETORNA O MAIOR
    return (a >b) ? a : b;
}

No* criarNo(int n){
    No *novo = (No*)malloc(sizeof(No));
    if(!novo){
        printf("\nProblema ao criar novo no");
        exit(1);
    }
    novo->num = n;
    novo->esq = novo->dir = NULL;
    novo->altura = 0;
    return novo;
}

short alturaDoNo(No *raiz){
    if (raiz == NULL) return -1;
    short esq = alturaDoNo(raiz->esq);
    short dir = alturaDoNo(raiz->dir);
    return maior(esq, dir) + 1;
}

short fatorDeBalanceamento(No *raiz){
    if (!raiz) return 0;
    return alturaDoNo(raiz->esq) - alturaDoNo(raiz->dir);
}


No* rotacaoEsquerda(No *r){
    No *y = r->dir;
    No *f = y->esq;

    y->esq = r;
    r->dir = f;

    r->altura = 1 + maior(alturaDoNo(r->esq), alturaDoNo(r->dir));
    y->altura = 1 + maior(alturaDoNo(y->esq), alturaDoNo(y->dir));
    return y;
}

No* rotacaoDireita(No *r){
    No *y = r->esq;
    No *f = y->dir;

    y->dir = r;
    r->esq = f;

    r->altura = 1 + maior(alturaDoNo(r->esq), alturaDoNo(r->dir));
    y->altura = 1 + maior(alturaDoNo(y->esq), alturaDoNo(y->dir));
    return y;
}

No* rotacaoEsquerdaDireita(No *r){
    r->esq = rotacaoEsquerda(r->esq);
    return rotacaoDireita(r);
}
No* rotacaoDireitaEsquerda(No *r){
    r->dir = rotacaoDireita(r->dir);
    return rotacaoEsquerda(r);
}

No* balancear(No *raiz){
    short fb = fatorDeBalanceamento(raiz);

    if (fb < -1 && fatorDeBalanceamento(raiz->dir) <= 0)
        raiz = rotacaoEsquerda(raiz);
    else if (fb > 1 && fatorDeBalanceamento(raiz->esq) >= 0)
        raiz = rotacaoDireita(raiz);
    else if (fb > 1 && fatorDeBalanceamento(raiz->esq) < 0)
        raiz = rotacaoEsquerdaDireita(raiz);
    else if (fb < -1 && fatorDeBalanceamento(raiz->dir) > 0)
        raiz = rotacaoDireitaEsquerda(raiz);

    return raiz;
}

No* inserir(No *raiz, int x){
    if (!raiz) return criarNo(x);

    if (x < raiz->num)      raiz->esq = inserir(raiz->esq, x);
    else if (x > raiz->num) raiz->dir = inserir(raiz->dir, x);
    else {
        printf("\nInsercao nao realizada! O elemento %d ja existe!\n", x);
        return raiz;
    }

    raiz->altura = 1 + maior(alturaDoNo(raiz->esq), alturaDoNo(raiz->dir));
    return balancear(raiz);
}


No* remover(No *raiz, int chave){
    if (!raiz){
        printf("Valor nao encontrado!\n");
        return NULL;
    }

    if (chave < raiz->num){
        raiz->esq = remover(raiz->esq, chave);
    } else if (chave > raiz->num){
        raiz->dir = remover(raiz->dir, chave);
    } else {
        // achou
        if (!raiz->esq && !raiz->dir){
            free(raiz);
            printf("Elemento folha removido: %d !\n", chave);
            return NULL;
        } else if (raiz->esq && raiz->dir){
            // predecessor
            No *aux = raiz->esq;
            while (aux->dir) aux = aux->dir;
            raiz->num = aux->num;
            raiz->esq = remover(raiz->esq, aux->num);
        } else {
            No *aux = raiz->esq ? raiz->esq : raiz->dir;
            free(raiz);
            printf("Elemento com 1 filho removido: %d !\n", chave);
            return aux;
        }
    }

    raiz->altura = 1 + maior(alturaDoNo(raiz->esq), alturaDoNo(raiz->dir));
    return balancear(raiz);
}


void imprimir(No *raiz, int nivel){
    if (raiz){
        imprimir(raiz->dir, nivel + 1);
        printf("\n\n");
        for (int i = 0; i < nivel; i++) printf("\t");
        printf("%d", raiz->num);
        imprimir(raiz->esq, nivel + 1);
    }
}


int main(){
    int opcao, valor;
    No *raiz = NULL;

    do{
        printf("\n\n\t0 - Sair\n\t1 - Inserir\n\t2 - Remover\n\t3 - Imprimir\n\n");
        scanf("%d", &opcao);

        switch(opcao){
        case 0:
            printf("Saindo!!!");
            break;
        case 1:
            printf("\tDigite o valor a ser inserido: ");
            scanf("%d", &valor);
            raiz = inserir(raiz, valor);
            break;
        case 2:
            printf("\tDigite o valor a ser removido: ");
            scanf("%d", &valor);
            raiz = remover(raiz, valor);
            break;
        case 3:
            imprimir(raiz, 1);
            break;
        default:
            printf("\nOpcao invalida!!!\n");
        }
    } while(opcao != 0);

    return 0;
}
