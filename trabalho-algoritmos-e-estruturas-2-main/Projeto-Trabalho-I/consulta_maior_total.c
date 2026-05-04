#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "types.h"
#include "associacao_index.h"
#include "consulta_maior_total.h"
#include "pedido.h"
#include "produto.h"


static long preco_para_centavos(const char *s){
    if(!s||!*s) return -1;
    long in=0, fr=0; int sep=0, fd=0;
    for(const char* p=s; *p; ++p){
        if(*p=='.'||*p==','){ if(sep) return -1; sep=1; continue; }
        if(!isdigit((unsigned char)*p)) return -1;
        if(!sep) in=in*10+(*p-'0');
        else if(fd<2){ fr=fr*10+(*p-'0'); fd++; }
    }
    if(sep && fd==1) fr*=10;
    return in*100+fr;
}


int pedido_maior_total_por_associacao_index(int pedidoIndexNivel,
                                            int produtoIndexNivel,
                                            Pedido *pedidoOut,
                                            long *totalOutCentavos)
{
    if (pedidoOut) memset(pedidoOut, 0, sizeof(*pedidoOut));
    if (totalOutCentavos) *totalOutCentavos = 0;


    FILE *fIndexAssoc     = fopen("IndiceAssociacaoNivel-1.bin", "rb");
    FILE *fAssocOrdenada  = fopen("AssociacaoOrdenada.bin",     "rb");
    if (!fIndexAssoc || !fAssocOrdenada) {
        if (fIndexAssoc)    fclose(fIndexAssoc);
        if (fAssocOrdenada) fclose(fAssocOrdenada);
        return -1;
    }


    if (fseek(fIndexAssoc, 0, SEEK_END) != 0) { fclose(fIndexAssoc); fclose(fAssocOrdenada); return -1; }
    long bytesIdx = ftell(fIndexAssoc);
    if (bytesIdx < 0)      { fclose(fIndexAssoc); fclose(fAssocOrdenada); return -1; }
    long numEntradasIdx = bytesIdx / (long)sizeof(IndexAssociacao);
    if (fseek(fIndexAssoc, 0, SEEK_SET) != 0) { fclose(fIndexAssoc); fclose(fAssocOrdenada); return -1; }

    IndexAssociacao *vetorIdx = (IndexAssociacao*) malloc((size_t)numEntradasIdx * sizeof(IndexAssociacao));
    if (!vetorIdx) { fclose(fIndexAssoc); fclose(fAssocOrdenada); return -1; }
    if (fread(vetorIdx, sizeof(IndexAssociacao), (size_t)numEntradasIdx, fIndexAssoc) != (size_t)numEntradasIdx) {
        free(vetorIdx); fclose(fIndexAssoc); fclose(fAssocOrdenada); return -1;
    }
    fclose(fIndexAssoc);

    long melhorTotal = -1;
    int  melhorIdx   = -1;

    /* percorre cada grupo (todas as associações de um id_pedido) */
    for (long iGrupo = 0; iGrupo < numEntradasIdx; ++iGrupo) {
        const IndexAssociacao *grupo = &vetorIdx[iGrupo];
        long somaDoPedido = 0;

        if (fseek(fAssocOrdenada, (long)grupo->start * (long)sizeof(Associacao), SEEK_SET) != 0)
            continue;

        for (int iAssoc = 0; iAssoc < grupo->count; ++iAssoc) {
            Associacao assoc;
            if (fread(&assoc, sizeof(assoc), 1, fAssocOrdenada) != 1) break;
            assoc.id_produto[19] = '\0';

            int posIdxProduto = 0;
            int posProdutoDados = pesquisa_por_id_produto(assoc.id_produto, produtoIndexNivel, &posIdxProduto);
            if (posProdutoDados < 0) continue;

            FILE *fProduto = fopen("produto.bin", "rb");
            if (!fProduto) continue;

            Produto produto;
            if (fseek(fProduto, (long)posProdutoDados * (long)sizeof(Produto), SEEK_SET) == 0 &&
                fread(&produto, sizeof(produto), 1, fProduto) == 1)
            {
                produto.preco[9] = '\0';
                long cent = preco_para_centavos(produto.preco);
                if (cent > 0) somaDoPedido += cent;
            }
            fclose(fProduto);
        }

        if (somaDoPedido > melhorTotal) { melhorTotal = somaDoPedido; melhorIdx = (int)iGrupo; }
    }

    if (melhorIdx < 0) {
        free(vetorIdx);
        fclose(fAssocOrdenada);
        return -1;
    }

    /* recupera o pedido vencedor */
    int posIdxPedido = 0;
    int posPedidoDados = pesquisa_por_id_pedido(vetorIdx[melhorIdx].id_pedido, pedidoIndexNivel, &posIdxPedido);
    if (posPedidoDados >= 0) {
        FILE *fPedido = fopen("pedido.bin", "rb");
        if (fPedido) {
            Pedido pedido;
            if (fseek(fPedido, (long)posPedidoDados * (long)sizeof(Pedido), SEEK_SET) == 0 &&
                fread(&pedido, sizeof(pedido), 1, fPedido) == 1)
            {
                if (pedidoOut)         *pedidoOut = pedido;
                if (totalOutCentavos)  *totalOutCentavos = melhorTotal;
            }
            fclose(fPedido);
        }
    } else {
        if (totalOutCentavos) *totalOutCentavos = melhorTotal;
    }

    free(vetorIdx);
    fclose(fAssocOrdenada);
    return posPedidoDados >= 0 ? posPedidoDados : -1;
}
