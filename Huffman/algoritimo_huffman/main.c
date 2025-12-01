#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

int main()
{
    TabelaFrequencia tabela;
    Lista listaFrequencia;

    //1 - organizar tabela de frequencia
    organiza_tabela_frequencia(&tabela);
   imprime_tabela_frequencia(&tabela);


    //2- montar fila de frequencia
    organiza_lista_encadeada_frequencia(&listaFrequencia,&tabela);
    imprimir_lista(&listaFrequencia);
    //3- montar arvore binaria de frequencia



    return 0;
}
