#include <stdio.h>
#include <stdlib.h>
#include "huffman.h"

int main()
{
    TabelaFrequencia tabela;
    Lista listaFrequencia;
    No *arvore = NULL;
    char **dicionario;
    char *texto;

    //1 - organizar tabela de frequencia
   texto =  organiza_tabela_frequencia(&tabela);
    imprime_tabela_frequencia(&tabela);
    printf("\n");

    //2- montar fila de frequencia
    organiza_lista_encadeada_frequencia(&listaFrequencia,&tabela);
    imprimir_lista(&listaFrequencia);
    printf("\n");
    //3- montar arvore binaria de frequencia

    arvore = montar_arvore(&listaFrequencia);
    printf("\tArvore huffman\n");
    imprimir_arvore(arvore,0);

    //4- organizar dicionario
    organiza_dicionario(arvore,&dicionario);
    imprime_dicionario(dicionario);

    //5- Codificar a partir dados a partir do dicionario
    printf("\nTEXTO ORIGINAL:%s\n",texto);
    char *codificado = codificar(dicionario,texto);
    printf("\n\tTexto codificado: %s\n",codificado);

    return 0;
}
