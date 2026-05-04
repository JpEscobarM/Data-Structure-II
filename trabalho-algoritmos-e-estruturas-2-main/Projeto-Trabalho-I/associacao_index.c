#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "associacao_index.h"
#include "sort.h"

int organiza_indice_associacao(void)
{

    FILE *arquivoAssoc = fopen("associacao.bin", "rb");
    if (!arquivoAssoc) {
        printf("\n[ERRO] abrir associacao.bin");
        return -1;
    }

    printf("\nCriando Arquivo de Indice de Associacao...\n");

    /* Crescimento amortizado: carrega o arquivo inteiro em memória. */
    Associacao *vetorAssoc = NULL;
    int qtdAssoc = 0, capAssoc = 0;

    while (1) { //le O ARQUIVO de associacao e coloca na memoria
        if (qtdAssoc == capAssoc) {

            int novaCap = capAssoc ? capAssoc * 2 : 2048;
            Associacao *novo = (Associacao*)realloc(vetorAssoc, (size_t)novaCap * sizeof(Associacao)); //QUANDO A CAPACIDADE É MENOR QUE 0 ENTAO DOBRA O TAMANHO DO VETOR
            //COM O REALLOC PRA CONTINUAR LENDO

            if (!novo) { fclose(arquivoAssoc); free(vetorAssoc); return -1; }
            vetorAssoc = novo; capAssoc = novaCap;
        }

        size_t lidos = fread(&vetorAssoc[qtdAssoc], sizeof(Associacao), 1, arquivoAssoc);
        if (lidos != 1) break;


        vetorAssoc[qtdAssoc].id_pedido[19]  = '\0';
        vetorAssoc[qtdAssoc].id_produto[19] = '\0';
        qtdAssoc++;
    }
    fclose(arquivoAssoc);


    if (qtdAssoc == 0) {
        FILE *fidxVazio = fopen("IndiceAssociacaoNivel-1.bin", "wb"); if (fidxVazio) fclose(fidxVazio);
        FILE *fordVazio = fopen("AssociacaoOrdenada.bin",   "wb");    if (fordVazio) fclose(fordVazio);
        return 0;
    }


    qsort(vetorAssoc, (size_t)qtdAssoc, sizeof(Associacao), cmp_assoc_by_pedido);

    FILE *arquivoOrdenado = fopen("AssociacaoOrdenada.bin", "wb");
    if (!arquivoOrdenado) { free(vetorAssoc); return -1; }
    if (fwrite(vetorAssoc, sizeof(Associacao), (size_t)qtdAssoc, arquivoOrdenado) != (size_t)qtdAssoc) {
        fclose(arquivoOrdenado); free(vetorAssoc); return -1;
    }
    fclose(arquivoOrdenado);

    /* Constrói índice nivel-1: uma entrada por pedido, apontando para o “run” no arquivo ordenado. */
    IndexAssociacao *indice = (IndexAssociacao*)malloc((size_t)qtdAssoc * sizeof(IndexAssociacao));
    if (!indice) { free(vetorAssoc); return -1; }

    int i = 0, qtdIndice = 0;
    while (i < qtdAssoc) {
        int j = i + 1;
        /* Porquê: caminha até o fim do bloco de mesmo id_pedido (run). */
        while (j < qtdAssoc && strcmp(vetorAssoc[j].id_pedido, vetorAssoc[i].id_pedido) == 0) j++;

        /* Emite a entrada agregada desse run. */
        strncpy(indice[qtdIndice].id_pedido, vetorAssoc[i].id_pedido, 19);
        indice[qtdIndice].id_pedido[19] = '\0';
        indice[qtdIndice].start = i;         /* início do run no arquivo ordenado */
        indice[qtdIndice].count = j - i;     /* quantidade de associações no run  */
        qtdIndice++;

        i = j; /* pula para o próximo run */
    }

    /* Persiste o índice nivel-1. */
    FILE *arquivoIndice = fopen("IndiceAssociacaoNivel-1.bin", "wb");
    if (!arquivoIndice) { free(indice); free(vetorAssoc); return -1; }
    if (fwrite(indice, sizeof(IndexAssociacao), (size_t)qtdIndice, arquivoIndice) != (size_t)qtdIndice) {
        fclose(arquivoIndice); free(indice); free(vetorAssoc); return -1;
    }
    fclose(arquivoIndice);

    free(indice);
    free(vetorAssoc);
    return qtdIndice; /* Porquê: retorna quantas entradas únicas de pedido existem no índice. */
}

int pesquisa_por_id_associacao(const char id_pedido[20], int *start, int *count)
{
    if (start) *start = 0;
    if (count) *count = 0;

    FILE *file = fopen("IndiceAssociacaoNivel-1.bin", "rb");
    if (!file) return -2; /* arquivo ausente/erro de IO */


    if (fseek(file, 0, SEEK_END) != 0) { fclose(file); return -2; }
    long file_size = ftell(file); //PEGA O TAMANHO TOTAL DO ARQ



    if (file_size < 0)
    {
        fclose(file); return -2;
    }

    const long tam_elem = (long)sizeof(IndexAssociacao);  // TOTAL DE BYTES DE CADA ELEMENTO

    const long total_registros = file_size / tam_elem; //TOTAL DE ELEMENTOS NO ARQUIVO

    if (fseek(file, 0, SEEK_SET) != 0) { fclose(file); return -2; }

     /* busca binária no arquivo */
    long inicio = 0;
    long fim    = total_registros - 1;
    IndexAssociacao entrada;

    while (inicio <= fim) {
        long meio = inicio + (fim - inicio) / 2;

        if (fseek(file, meio * tam_elem, SEEK_SET) != 0) { fclose(file); return -2; }
        if (fread(&entrada, sizeof(entrada), 1, file) != 1) { fclose(file); return -2; }

        int cmp = strcmp(id_pedido, entrada.id_pedido);

        if (cmp == 0) {
            if (start) *start = entrada.start;   /* início do run no arquivo ordenado */
            if (count) *count = entrada.count;   /* quantidade de associações */
            fclose(file);
            return 0;
        } else if (cmp < 0) {
            fim = meio - 1;
        } else {
            inicio = meio + 1;
        }
    }


    fclose(file);
    return -1; /* não encontrado */
}

array pesquisa_produtos_associacao(char id_pedido[20]) {
    array arr = {0};
    Associacao assoc;
    FILE *arquivo_assoc;

    int inicio = 0, tamanho = 0, i = 0;

    if (pesquisa_por_id_associacao(id_pedido, &inicio, &tamanho) != 0)
    {
        fprintf(stderr, "Ocorreu um erro ao buscar o ID da associação\n.");
        return arr;
    }

    arquivo_assoc = fopen(ARQUIVO_ASSOCIACAO, "rb");
    if (!arquivo_assoc) {
        fprintf(stderr, "Ocorreu um erro ao abrir o arquivo de associações.\n");
        return arr;
    }

    arr.num_elementos = tamanho;
    arr.dados = malloc(arr.num_elementos * sizeof(Associacao));

    fseek(arquivo_assoc, inicio * sizeof(Associacao), SEEK_SET);
    if (fread(arr.dados, sizeof(Associacao), arr.num_elementos, arquivo_assoc) != arr.num_elementos)
    {
        fprintf(stderr, "Ocorreu um erro ao ler todos dados de associações.\n");
        free(arr.dados);
        arr.num_elementos = 0;
        arr.dados = NULL;
    } else {
        printf("%d produtos no pedido.\n", i);
    }

    fclose(arquivo_assoc);
    return arr;
}