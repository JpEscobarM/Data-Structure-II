#ifndef CONSULTA_MAIOR_TOTAL_H_INCLUDED
#define CONSULTA_MAIOR_TOTAL_H_INCLUDED


#include "types.h"

/* Usa o índice de associação (IndiceAssociacaoNivel-1.bin + AssociacaoOrdenada.bin)
   para retornar o pedido com maior total de produtos somados.
   Retorna posicao do pedido em pedido.bin, ou -1 se vazio/erro. */
int pedido_maior_total_por_associacao_index(int nivelPedido,
                                            int nivelProduto,
                                            Pedido *outPedido,
                                            long *outTotalCentavos);


#endif // CONSULTA_MAIOR_TOTAL_H_INCLUDED
