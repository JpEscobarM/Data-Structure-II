#include <stdio.h>
#include "huffman.h"


//========== FUNCOES DE LEITURA
void ler_pedidos_tabela_frequencia(const char *nome_arquivo,TabelaFrequencia *t) {
    FILE *f = fopen(nome_arquivo, "rb");
    if (!f) { perror("Erro ao abrir pedido.bin"); return; }

    Pedido p;
    while (fread(&p, sizeof(Pedido), 1, f) == 1 ) {
        p.data[10]='\0'; p.id_pedido[19]='\0'; p.preco[9]='\0';

        adiciona_frequencia_pedido(p,t);
    }

    fclose(f);
}

void ler_produtos_tabela_frequencia(const char *nome_arquivo, TabelaFrequencia *t)
{
    FILE *f = fopen(nome_arquivo, "rb");
    if (!f) { perror("Erro ao abrir produto.bin"); return; }

    Produto p;
    while (fread(&p, sizeof(Produto), 1, f) == 1) {
        p.id_produto[19]='\0'; p.alias[20]='\0'; p.preco[9]='\0';
        p.genero[1]='\0'; p.cor[10]='\0'; p.material[10]='\0'; p.joia[10]='\0';
        adiciona_frequencia_produto(p,t);
    }

    fclose(f);
}

void inicializa_tabela_frequencia(TabelaFrequencia *t)
{
    t->size =256;
    for(int i = 0 ; i < t->size; i++)
    {
     t->table[i] = 0;

    }

}

/*Conta a frequencia de cada carcater usando unsigned int para ser de 0...255
*/
void conta_string(char *s, TabelaFrequencia *t)
{
    for (int i = 0; s[i] != '\0'; i++) {

        unsigned char c = (unsigned char) s[i];
        t->table[c]++;
    }
}

/*adiciona caracteres do pedido na tabela de frequencia*/
void adiciona_frequencia_pedido(Pedido p, TabelaFrequencia *t)
{
    conta_string(p.id_pedido,t);
    conta_string(p.data,t);
    conta_string(p.preco,t);
}
/*funcao que adiciona a contagem de strings do produto na tabela de frequencia*/
void adiciona_frequencia_produto(Produto p, TabelaFrequencia *t)
{
    conta_string(p.alias,t);
    conta_string(p.cor,t);
    conta_string(p.genero,t);
    conta_string(p.id_produto,t);
    conta_string(p.joia,t);
    conta_string(p.material,t);
    conta_string(p.preco,t);
}

void imprime_tabela_frequencia(TabelaFrequencia *t)
{
    for (int i = 0; i < t->size; i++) {
        if (t->table[i] != 0) {
            printf("'%c' (%d) apareceu %d vezes\n",i,i,t->table[i]);
        }
    }
}

/*funcao que organiza a tabela de frequencia lendo o arquivo de pedidos e arquivo de produtos*/
void organiza_tabela_frequencia(TabelaFrequencia *t)
{
     inicializa_tabela_frequencia(t);

    char *arquivoProduto ="produto.bin";
    char *arquivoPedido = "pedido.bin";

    ler_pedidos_tabela_frequencia(arquivoPedido,t);
    ler_produtos_tabela_frequencia(arquivoProduto,t);
}


/*==========================================
    FUNCOES LISTA ENCADEADA PARA CRIACAO DA AROVRE
==========================================*/

void criar_lista(Lista *lista){
    lista->inicio = NULL;
    lista->tam = 0;
}

//INSERE ORDENADO NA LISTA ENCADEADA COM BASE NA FREQUENCIA
void inserir_ordenado(Lista *lista, No *no){
    No *aux;
    // a lista está vazia?
    if(lista->inicio == NULL){
        lista->inicio = no;
    }
    // tem frequência menor que o início da lista
    else if(no->frequencia < lista->inicio->frequencia){
        no->proximo = lista->inicio;
        lista->inicio = no;
    }
    else{ // insere no meio ou no fim da lista
        aux = lista->inicio;
        while(aux->proximo && aux->proximo->frequencia <= no->frequencia)
            aux = aux->proximo;
        no->proximo = aux->proximo;
        aux->proximo = no;
    }
    lista->tam++; // incrementa o tamanho da lista
}
//FUNCAO QUE INICIALIZA LISTA ENCADEADA DE FREQUENCIA A PARTIR DA TABELA DE FREQUENCIA
void preencher_lista(TabelaFrequencia *t, Lista *lista){
    int i;
    No *novo;
    for(i = 0; i < t->size; i++){ // percorre toda a tabela
        if(t->table[i] > 0){ // se quantidade for maior que zero
            novo = malloc(sizeof(No));
            if(novo){
                novo->caracter = i;// posicao na tabela ja é o caracter
                novo->frequencia = t->table[i];
                novo->dir = NULL;
                novo->esq = NULL;
                novo->proximo = NULL;
                inserir_ordenado(lista, novo);
            }
            else{
                printf("\tERRO ao alocar memoria em preencher_lista!\n");
                break;
            }
        }
    }
}

void imprimir_lista(Lista *lista){
    No *aux = lista->inicio;
    printf("\n\tLista ordenada: Tamanho: %d\n", lista->tam);
    while(aux){
        printf("\tCaracter: %c Frequência: %d\n", aux->caracter, aux->frequencia);
        aux = aux->proximo;
    }
}


void organiza_lista_encadeada_frequencia(Lista *l,TabelaFrequencia *t)
{
    criar_lista(l);
    preencher_lista(t,l);

}
