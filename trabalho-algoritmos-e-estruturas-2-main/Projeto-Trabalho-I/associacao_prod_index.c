#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "sort.h"
#include "produto.h"
#include "associacao_prod_index.h"



int organiza_indice_associacao_produto()
{
    /* Tamanho fixo de bloco p/ construir níveis superiores (mesmo conceito dos seus outros índices). */
    const int tamanhoBloco = 100;

    /* ---------- 1) Carrega associacao.bin em memória ---------- */
    FILE *arquivoAssociacao = fopen("associacao.bin", "rb");
    if (!arquivoAssociacao) {
        printf("\n[ERRO] abrir associacao.bin");
        return -1;
    }

    /* vetor dinâmico de associações (pedido,produto) */
    Associacao *assocs = NULL;
    int totalAssocs = 0;
    int capacidadeAssocs = 0;

    for (;;) {
        if (totalAssocs == capacidadeAssocs) {
            int novaCapacidade = capacidadeAssocs ? capacidadeAssocs * 2 : 4096;
            Associacao *novoBuf = (Associacao*)realloc(assocs, (size_t)novaCapacidade * sizeof(Associacao));
            if (!novoBuf) {
                fclose(arquivoAssociacao);
                free(assocs);
                return -1;
            }
            assocs = novoBuf;
            capacidadeAssocs = novaCapacidade;
        }

        size_t lidos = fread(&assocs[totalAssocs], sizeof(Associacao), 1, arquivoAssociacao);
        if (lidos != 1) break;

        /* garante terminação das strings (defesa) */
        assocs[totalAssocs].id_pedido[19]  = '\0';
        assocs[totalAssocs].id_produto[19] = '\0';
        totalAssocs++;
    }
    fclose(arquivoAssociacao);

    /* Se não há nada, ainda assim criamos arquivos vazios p/ pipeline ficar estável. */
    if (totalAssocs == 0) {
        FILE *fVazio1 = fopen("AssociacaoPorProduto.bin","wb"); if (fVazio1) fclose(fVazio1);
        FILE *fVazio2 = fopen("IndiceAssocProdNivel-1.bin","wb"); if (fVazio2) fclose(fVazio2);
        return 1; /* nível mínimo */
    }

    /* ---------- 2) Ordena por id_produto (usa seu comparador cmp_assoc_by_prod) ---------- */
    /* IMPORTANTE: precisa existir em algum .c (ex.: sort.c):
       static int cmp_assoc_by_prod(const void *a, const void *b) { ... } */
    qsort(assocs, (size_t)totalAssocs, sizeof(Associacao), cmp_assoc_by_prod);

    /* ---------- 3) Grava arquivo ordenado por produto ---------- */
    FILE *arquivoOrdenado = fopen("AssociacaoPorProduto.bin","wb");
    if (!arquivoOrdenado) {
        free(assocs);
        return -1;
    }
    if (fwrite(assocs, sizeof(Associacao), (size_t)totalAssocs, arquivoOrdenado) != (size_t)totalAssocs) {
        fclose(arquivoOrdenado);
        free(assocs);
        return -1;
    }
    fclose(arquivoOrdenado);

    /* ---------- 4) Monta nível 1 (runs de mesmo id_produto) ---------- */
    IndexAssocProd *nivel1 = (IndexAssocProd*)malloc((size_t)totalAssocs * sizeof(IndexAssocProd));
    if (!nivel1) {
        free(assocs);
        return -1;
    }

    int i = 0;            /* cursor no vetor ordenado */
    int totalEntradasL1 = 0;
    while (i < totalAssocs) {
        int j = i + 1;
        /* avança até mudar o id_produto */
        while (j < totalAssocs && strcmp(assocs[j].id_produto, assocs[i].id_produto) == 0) {
            j++;
        }

        /* cria uma entrada de índice para esse “run” */
        strncpy(nivel1[totalEntradasL1].id_produto, assocs[i].id_produto, 19);
        nivel1[totalEntradasL1].id_produto[19] = '\0';
        nivel1[totalEntradasL1].start  = i;         /* início desse produto em AssociacaoPorProduto.bin */
        nivel1[totalEntradasL1].count  = j - i;     /* quantas associações desse produto */
        nivel1[totalEntradasL1].posicao = 0;        /* não usado em nível 1 */

        totalEntradasL1++;
        i = j;
    }

    free(assocs); /* não precisa mais do vetor de associações */

    FILE *arquivoIndiceL1 = fopen("IndiceAssocProdNivel-1.bin","wb");
    if (!arquivoIndiceL1) {
        free(nivel1);
        return -1;
    }
    if (fwrite(nivel1, sizeof(IndexAssocProd), (size_t)totalEntradasL1, arquivoIndiceL1) != (size_t)totalEntradasL1) {
        fclose(arquivoIndiceL1);
        free(nivel1);
        return -1;
    }
    fclose(arquivoIndiceL1);
    free(nivel1);

    /* ---------- 5) Construir níveis superiores (2..N) por amostragem em blocos ---------- */
    /* Ideia: cada nível “resume” o anterior pegando o último item de cada bloco fixo (clusterizado). */
    int nivelAtual = 1;
    int qtdEntradasNivelAnterior;

    /* mede quantas entradas tem o nível 1 */
    {
        FILE *f = fopen("IndiceAssocProdNivel-1.bin","rb");
        if (!f) return -1;
        if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return -1; }
        long bytes = ftell(f);
        fclose(f);
        if (bytes < 0) return -1;
        qtdEntradasNivelAnterior = (int)(bytes / (long)sizeof(IndexAssocProd));
    }

    /* gera nível 2, 3, ... enquanto “sobra” bastante item */
    while (qtdEntradasNivelAnterior > 10) {
        char caminhoNivelAnt[64], caminhoNivelNovo[64];
        sprintf(caminhoNivelAnt, "IndiceAssocProdNivel-%d.bin", nivelAtual);
        sprintf(caminhoNivelNovo, "IndiceAssocProdNivel-%d.bin", nivelAtual + 1);

        FILE *fAnt = fopen(caminhoNivelAnt, "rb");
        if (!fAnt) break;

        IndexAssocProd *bufferNivelAnt = (IndexAssocProd*)malloc((size_t)qtdEntradasNivelAnterior * sizeof(IndexAssocProd));
        if (!bufferNivelAnt) { fclose(fAnt); return -1; }
        if (fread(bufferNivelAnt, sizeof(IndexAssocProd), (size_t)qtdEntradasNivelAnterior, fAnt) != (size_t)qtdEntradasNivelAnterior) {
            free(bufferNivelAnt);
            fclose(fAnt);
            return -1;
        }
        fclose(fAnt);

        IndexAssocProd *saidaNivel = NULL;
        int qtdSaida = 0;
        int capSaida = 0;

        int inicioBlocoNoInferior = 0; /* guarda onde começa o bloco no nível imediatamente inferior */
        int numeroDoBloco = 1;

        while (1) {
            int indiceUltimoDoBloco = (numeroDoBloco * tamanhoBloco) - 1;
            if (indiceUltimoDoBloco >= qtdEntradasNivelAnterior) break;

            if (qtdSaida == capSaida) {
                int novaCap = capSaida ? capSaida * 2 : 1024;
                IndexAssocProd *novo = (IndexAssocProd*)realloc(saidaNivel, (size_t)novaCap * sizeof(IndexAssocProd));
                if (!novo) {
                    free(bufferNivelAnt);
                    free(saidaNivel);
                    return -1;
                }
                saidaNivel = novo; capSaida = novaCap;
            }

            /* “Amostra” a chave do fim do bloco e aponta para onde o bloco inicia no nível inferior */
            IndexAssocProd entradaResumida = {0};
            strncpy(entradaResumida.id_produto, bufferNivelAnt[indiceUltimoDoBloco].id_produto, 19);
            entradaResumida.id_produto[19] = '\0';
            entradaResumida.posicao = inicioBlocoNoInferior; /* bloco no nível inferior */
            entradaResumida.start   = 0;  /* só usados no nível 1 */
            entradaResumida.count   = 0;

            saidaNivel[qtdSaida++] = entradaResumida;

            /* próximo bloco começa imediatamente após este */
            inicioBlocoNoInferior += tamanhoBloco;
            numeroDoBloco++;
        }

        FILE *fNovo = fopen(caminhoNivelNovo, "wb");
        if (!fNovo) {
            free(bufferNivelAnt);
            free(saidaNivel);
            return -1;
        }
        if (fwrite(saidaNivel, sizeof(IndexAssocProd), (size_t)qtdSaida, fNovo) != (size_t)qtdSaida) {
            fclose(fNovo);
            free(bufferNivelAnt);
            free(saidaNivel);
            return -1;
        }
        fclose(fNovo);

        free(bufferNivelAnt);
        free(saidaNivel);

        nivelAtual++;
        qtdEntradasNivelAnterior = qtdSaida; /* itera “subindo” */
    }

    return nivelAtual; /* maior nível gerado */
}


int pesquisa_por_id_associacao_produto(const char id_produto[20],
                                       int nivel,
                                       int *start,
                                       int *count)
{
    if (start) *start = 0;
    if (count) *count = 0;

    int posBloco = 0;

    /* desce níveis  N..2 usando ponteiros de bloco */
    for (int niv = nivel; niv > 1; --niv) {
        char nome[64];
        sprintf(nome, "IndiceAssocProdNivel-%d.bin", niv);

        FILE *f = fopen(nome, "rb");
        if (!f) return -2;

        if (fseek(f, (long)posBloco * (long)sizeof(IndexAssocProd), SEEK_SET) != 0) {
            fclose(f); return -2;
        }

        IndexAssocProd rec;
        while (fread(&rec, sizeof(rec), 1, f) == 1) {
            if (strcmp(id_produto, rec.id_produto) <= 0) break;
            posBloco++;
        }
        fclose(f);
        posBloco = rec.posicao; /* aponta bloco no nível abaixo */
    }

    /* nível 1: resolve [start,count] */
    {
        char nome[64];
        sprintf(nome, "IndiceAssocProdNivel-%d.bin", 1);
        FILE *f = fopen(nome, "rb");
        if (!f) return -2;

        if (fseek(f, (long)posBloco * (long)sizeof(IndexAssocProd), SEEK_SET) != 0) {
            fclose(f); return -2;
        }

        IndexAssocProd rec; int pos = posBloco;
        while (fread(&rec, sizeof(rec), 1, f) == 1) {
            pos++;
            if (strcmp(id_produto, rec.id_produto) <= 0) break;
        }

        if (strcmp(id_produto, rec.id_produto) == 0) {
            if (start) *start = rec.start;
            if (count) *count = rec.count;
            fclose(f);
            return 0;
        }

        /* fallback: olha uma posição anterior do bloco */
        int leituraPos = (pos > 1) ? (pos - 2) : 0;
        if (fseek(f, (long)leituraPos * (long)sizeof(IndexAssocProd), SEEK_SET) == 0 &&
            fread(&rec, sizeof(rec), 1, f) == 1 &&
            strcmp(id_produto, rec.id_produto) == 0)
        {
            if (start) *start = rec.start;
            if (count) *count = rec.count;
            fclose(f);
            return 0;
        }

        fclose(f);
        return -1;
    }
}

int produto_mais_vendido_via_indice(int nivelProduto, Produto *outProd, long *outQtd)
{
    if (outProd) memset(outProd, 0, sizeof(*outProd));
    if (outQtd)  *outQtd = 0;

    FILE *f = fopen("IndiceAssocProdNivel-1.bin","rb");
    if (!f) return -1;

    IndexAssocProd rec;
    long best = -1;
    char bestId[20] = {0};

    while (fread(&rec, sizeof(rec), 1, f) == 1) {
        if (rec.count > best) {
            best = rec.count;
            strncpy(bestId, rec.id_produto, 19);
            bestId[19] = '\0';
        }
    }
    fclose(f);

    if (best <= 0) return -1;

    int posIdx = 0;
    int pos = pesquisa_por_id_produto(bestId, nivelProduto, &posIdx);
    if (pos < 0) return -1;

    FILE *fp = fopen("produto.bin","rb");
    if (!fp) return -1;

    Produto p;
    if (fseek(fp, (long)pos * (long)sizeof(Produto), SEEK_SET) == 0 &&
        fread(&p, sizeof(p), 1, fp) == 1)
    {
        if (outProd) *outProd = p;
        if (outQtd)  *outQtd  = best;
        fclose(fp);
        return pos;
    }
    fclose(fp);
    return -1;
}
