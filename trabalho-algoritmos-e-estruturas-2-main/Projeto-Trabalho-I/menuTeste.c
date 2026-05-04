#include "menuTeste.h"
#include "associacao.h"
#include "huffman.h"
#include "produto_hash.h"
#include "teste_performance.h"

#include <stdio.h>
#include <string.h>

void format_centavos(long c, char out[32]) {
    long a = c < 0 ? -c : c, r = a % 100, i = a / 100;
    sprintf(out, "%s%ld.%02ld", (c < 0) ? "-" : "", i, r);
}

void tempo_passou(const char *label, struct timespec inicio, struct timespec fim) {
    double tempo = (fim.tv_sec - inicio.tv_sec) + (fim.tv_nsec - inicio.tv_nsec) / 1e9;
    printf("%s: Levou %lf segundos\n", label, tempo);
}

void menuTeste() {
    int nivelPedido, nivelProduto, nivelAssocProd;

    MEDIR_TEMPO("Organizando indice pedido", {
        nivelPedido  = organiza_indice_pedido();
    });
    
    MEDIR_TEMPO("Organizando indice produto", {
        nivelProduto = organiza_indice_produto();
    });

    MEDIR_TEMPO("Organizando indice produto (Indice hash em memória)", {
        organiza_indice_produto_hash();
    });

    MEDIR_TEMPO("Organizando indice associacao", {
        organiza_indice_associacao();
    });
    
    MEDIR_TEMPO("Organizando indice preco", {
        organiza_indice_preco();
    });

    MEDIR_TEMPO("Organizando indice associacao produto", {
        nivelAssocProd = organiza_indice_associacao_produto();
    });

    int opPrincipal = 1, opSecundario = 1;
    char idPesquisa[20];
    Pedido p;
    Produto prod;
    int posicao = 0;

    while (opPrincipal != 0) {
        printf("\n=========================================");
        printf("\n\tMENU PRINCIPAL");
        printf("\n=========================================");
        printf("\n1 - Gerenciar Pedidos");
        printf("\n2 - Gerenciar Produtos");
        printf("\n3 - Pedido com maior total? (via associacao de ID_PEDIDO)");
        printf("\n4 - Produto mais vendido? (via indice de associacao de ID_PRODUTO)");
        printf("\n5 - Produtos associados ao pedido");
        printf("\n6 - Huffman (compactar/descompactar)");
        printf("\n7 - Produtos de determinado valor");
        printf("\n8 - Teste de performance dos indices");
        printf("\n0 - Sair");
        printf("\n-----------------------------------------");
        printf("\nEscolha uma opcao: ");
        scanf("%d", &opPrincipal);
        scanf("%*c");

        switch (opPrincipal) {
        case 1: {
            opSecundario = 1;
            while (opSecundario != 0) {
                printf("\n\n======= MENU DE PEDIDOS =======");
                printf("\n1 - Inserir Pedido");
                printf("\n2 - Excluir Pedido");
                printf("\n3 - Pesquisar Pedido");
                printf("\n0 - Voltar ao menu principal");
                printf("\n-------------------------------");
                printf("\nEscolha: ");
                scanf("%d", &opSecundario);
                scanf("%*c");

                switch (opSecundario) {
                case 1:
                    printf("\nDigite o ID do pedido: ");
                    scanf("%19s", p.id_pedido);
                    printf("Digite a data (AAAA-MM-DD): ");
                    scanf("%10s", p.data);
                    printf("Digite o preco: ");
                    scanf("%9s", p.preco);
                    printf("\nResumo do pedido digitado:");
                    printf("\nID: %s\nData: %s\nPreco: %s\n", p.id_pedido, p.data, p.preco);
                    inserir_pedido(p, nivelPedido);
                    break;
                case 2:
                    printf("\nDigite o ID do pedido a excluir: ");
                    scanf("%19s", idPesquisa);
                    excluir_pedido(idPesquisa, nivelPedido);
                    break;
                case 3:
                    posicao = 0;
                    printf("\nDigite o ID do pedido a pesquisar: ");
                    scanf("%19s", idPesquisa);
                    printf("\n[INFO] Pesquisando pedido ID=%s...\n", idPesquisa);
                    {
                        int resultado = pesquisa_por_id_pedido(idPesquisa, nivelPedido, &posicao);
                        if (resultado >= 0)       printf("\n[OK] Pedido encontrado na posicao %d.\n", resultado);
                        else if (resultado == -1) printf("\n[ERRO] Pedido nao encontrado.\n");
                        else                      printf("\n[ERRO] Falha na pesquisa.\n");
                    }
                    break;
                case 0: printf("\nRetornando ao menu principal...\n"); break;
                default: printf("\nOpcao invalida.\n");
                }
            }
            break;
        }
        case 2: {
            opSecundario = 1;
            while (opSecundario != 0) {
                printf("\n\n======= MENU DE PRODUTOS =======");
                printf("\n1 - Inserir Produto");
                printf("\n2 - Excluir Produto");
                printf("\n3 - Pesquisar Produto");
                printf("\n4 - Pesquisar Produto (Hash)");
                printf("\n0 - Voltar ao menu principal");
                printf("\n-------------------------------");
                printf("\nEscolha: ");
                scanf("%d", &opSecundario);
                scanf("%*c");

                switch (opSecundario) {
                case 1:
                    printf("\nDigite o ID do produto: ");
                    scanf("%19s", prod.id_produto);
                    printf("Digite o alias (nome curto): ");
                    scanf("%19s", prod.alias);
                    printf("Digite o preco: ");
                    scanf("%9s", prod.preco);
                    printf("\nResumo do produto digitado:");
                    printf("\nID: %s\nAlias: %s\nPreco: %s\n", prod.id_produto, prod.alias, prod.preco);
                    inserir_produto(prod, nivelProduto);
                    break;

                case 2:
                    printf("\nDigite o ID do produto a excluir: ");
                    scanf("%19s", idPesquisa);
                    excluir_produto(idPesquisa, nivelProduto);
                    break;

                case 3:
                    posicao = 0;
                    printf("\nDigite o ID do produto a pesquisar: ");
                    scanf("%19s", idPesquisa);
                    printf("\n[INFO] Pesquisando produto ID=%s...\n", idPesquisa);
                    {
                        int resultado = pesquisa_por_id_produto(idPesquisa, nivelProduto, &posicao);
                        if (resultado >= 0)       printf("\n[OK] Produto encontrado na posicao %d.\n", resultado);
                        else if (resultado == -1) printf("\n[ERRO] Produto nao encontrado.\n");
                        else                       printf("\n[ERRO] Falha na pesquisa.\n");
                    }
                    break;

                case 4:
                    Produto produto;

                    printf("\nDigite o ID do produto a pesquisar: ");
                    scanf("%19s", idPesquisa);
                    printf("\n[INFO] Pesquisando produto ID=%s...\n", idPesquisa);

                    produto = busca_produto_hash(idPesquisa);

                    if (is_null(produto.id_produto)) {
                        printf("Não foi possível encontrar o produto com o Id [%s]\n", produto.id_produto);
                    }
                    else {
                        printf("Encontrado produto %s, %s com valor %s\n", produto.alias, produto.cor, produto.preco);
                    }

                    break;    
                break;
                    

                case 0: printf("\nRetornando ao menu principal...\n"); break;
                default: printf("\nOpcao invalida.\n");
                }
            }
            break;
        }
        case 3: { // Pedido com maior total (via associacao por id_pedido)
            Pedido pedMax; long totalCentavos;
            int pos = pedido_maior_total_por_associacao_index(nivelPedido, nivelProduto,
                                                              &pedMax, &totalCentavos);
            if (pos >= 0) {
                char buf[32]; format_centavos(totalCentavos, buf);
                printf("\n[OK] Maior pedido: ID=%s  Data=%s  Total=%s  (pos=%d)\n",
                       pedMax.id_pedido, pedMax.data, buf, pos);
            } else {
                puts("\n[ERRO] Nao foi possivel calcular o maior total.");
            }
            break;
        }
        case 4: { // Produto mais vendido (via indice por id_produto)
            Produto pmax; long qtd = 0;
            if (nivelAssocProd < 1) {
                nivelAssocProd = organiza_indice_associacao_produto(100);
            }
            int pos = produto_mais_vendido_via_indice(nivelProduto, &pmax, &qtd);
            if (pos >= 0) {
                printf("\n[OK] Produto mais vendido:");
                printf("\nID=%s  Alias=%s  Qtd=%ld  (pos=%d)\n",
                       pmax.id_produto, pmax.alias, qtd, pos);
            } else {
                puts("\n[ERRO] Nao foi possivel obter o produto mais vendido.");
            }
            break;
        }
        case 5: {
            int i;
            Associacao *associacoes;
            Produto produto;
            array lista_associacoes;

            printf("\nDigite o ID do pedido a buscar: ");
            scanf("%19s", idPesquisa);

            lista_associacoes = pesquisa_produtos_associacao(idPesquisa);

            if (!lista_associacoes.num_elementos) {
                fprintf(stderr, "Nenhum produto vinculado ao pedido %s encontrado.\n", idPesquisa);
                continue;
            }

            associacoes = (Associacao*) lista_associacoes.dados;
            for (i = 0; i < lista_associacoes.num_elementos; i++) {
                produto = busca_por_produto(associacoes[i].id_produto, nivelProduto);

                if (!produto.id_produto) {
                    fprintf(stderr,
                        "Ocorreu um erro ao buscar o produto nº%d (%s)\n, ignorando-o.\n",
                        i,
                        associacoes[i].id_produto);
                    continue;
                }

                printf(" - %2d) %ld (%s) (%s)\n", i + 1,
                    associacoes[i].id_pedido,
                    produto.material,
                    produto.preco);
            }

            printf("%d produtos no pedido.\n", lista_associacoes.num_elementos);
            free(lista_associacoes.dados);
            break;
        }
        case 6: {  // <<< SUBMENU HUFFMAN
            int opHuff = 1;
            while (opHuff != 0) {
                printf("\n\n======= MENU HUFFMAN =======");
                printf("\n1 - Compactar produto.bin");
                printf("\n2 - Compactar pedido.bin");
                printf("\n3 - Descompactar (usando arvore de produto.bin)");
                printf("\n4 - Descompactar (usando arvore de pedido.bin)");
                printf("\n0 - Voltar ao menu principal");
                printf("\n---------------------------");
                printf("\nEscolha: ");
                scanf("%d", &opHuff);
                scanf("%*c");

                switch (opHuff) {
                case 1:
                    huffman_compactar_arquivo("produto.bin");
                    break;
                case 2:
                    huffman_compactar_arquivo("pedido.bin");
                    break;
                case 3:
                    huffman_descompactar_arquivo("produto.bin");
                    break;
                case 4:
                    huffman_descompactar_arquivo("pedido.bin");
                    break;
                case 0:
                    printf("\nRetornando ao menu principal...\n");
                    break;
                default:
                    printf("\nOpcao invalida.\n");
                }
            }
            break;
        }
        case 7: {
            float preco_minimo, preco_maximo;
            printf("Insira o valor mínimo do produto: ");
            scanf("%f", &preco_minimo);
            putchar('\n');
            printf("Insira o valor maximo do produto: ");
            scanf("%f", &preco_maximo);

            mostra_produtos_em_faixa(preco_minimo, preco_maximo, nivelProduto);
            break;
        }
        case 8: {
            teste_performance(nivelProduto);
            break;
        }
        case 0:
            printf("\nEncerrando o programa...\n");
            break;

        default:
            printf("\nOpcao invalida.\n");
        }
    }
    return;
}
