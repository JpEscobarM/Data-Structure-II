#include "types.h"
#include "sort.h"
#include "pedido.h"

void adicionar_pedido(ListaPedido *lp, Pedido novoPedido)
{
    lp->lista = (Pedido*)realloc(lp->lista,(size_t)(lp->qtdPedido+1)*sizeof(Pedido));
    if (lp->lista == NULL) { printf("Nao foi possivel alocar memoria"); return; }
    lp->lista[lp->qtdPedido] = novoPedido;
    lp->qtdPedido++;
}

void adicionar_index_pedido(ListaIndexPedido *li, Pedido novoPedido, int posicao)
{
    IndexPedido ip;
    strcpy(ip.id_pedido,novoPedido.id_pedido);
    ip.posicao = posicao;
    ip.qtdExtensao=0;
    ip.excluido = 0;
    ip.elo = -1;
    ip.tamExtensao=10;

    li->listaIndex = (IndexPedido*)realloc(li->listaIndex,(size_t)(li->qtdIndex+1)*sizeof(IndexPedido));
    if (li->listaIndex == NULL) { printf("Nao foi possivel alocar memoria"); return; }
    li->listaIndex[li->qtdIndex] = ip;
    li->qtdIndex++;
}

void imprimir_lista_pedidos(ListaPedido *lp)
{
    for(int i=0;i<lp->qtdPedido;i++) printf("%\n[%.3d]ID_PEDIDO:%s",i,lp->lista[i].id_pedido);
}

void imprimir_lista_index_pedido(ListaIndexPedido *li)
{
    for(int i=0;i<li->qtdIndex;i++){
        printf("%\n[%.3d]ID_PEDIDO:%s ",i,li->listaIndex[i].id_pedido);
        printf("POSICAO:%d",li->listaIndex[i].posicao);
    }
}

void lerPedidos(const char *nome_arquivo,ListaIndexPedido *li) {
    FILE *f = fopen(nome_arquivo, "rb");
    if (!f) { perror("Erro ao abrir pedido.bin"); return; }

    Pedido p; int cursor = 0;
    while (fread(&p, sizeof(Pedido), 1, f) == 1 ) {
        p.data[10]='\0'; p.id_pedido[19]='\0'; p.preco[9]='\0';
        if(strcmp(p.id_pedido,"9999999999999999999") != 0)
            adicionar_index_pedido(li,p,cursor);
        cursor++;
    }
    fclose(f);
}

void lerAssociacoes(const char *nome_arquivo) {
    FILE *f = fopen(nome_arquivo, "rb");
    if (!f) { perror("Erro ao abrir associacao.bin"); return; }
    Associacao a; int count=0;
    while (fread(&a, sizeof(Associacao), 1, f)) { a.id_pedido[19]='\0'; a.id_produto[19]='\0'; }
    printf("\nTotal de associa��es lidas: %d\n\n", count);
    fclose(f);
}

int cria_indice_pedido(ListaIndexPedido *li, int *nivel,int *qtdBlocos, int tamanhoBloco)
{
    char nome[50]; sprintf(nome, "IndicePedidoNivel-%d.bin", *nivel);
    long cursor =0; size_t qtdEscrita = 0;

    if(*nivel == 1){
        FILE *arquivo = fopen(nome,"wb");
        if(!arquivo){ printf("\nNao foi possivel abrir arquivo de indice"); return -1; }
        qtdEscrita = fwrite(li->listaIndex,sizeof(IndexPedido),(size_t)li->qtdIndex,arquivo);
        if(qtdEscrita != (size_t)li->qtdIndex) printf("\nNem todos os indices foram gravados corretamente");
        fclose(arquivo);
        *qtdBlocos = 0;

        (*nivel)++;
        return li->qtdIndex;
    }

    sprintf(nome, "IndicePedidoNivel-%d.bin", (*nivel)-1);
    FILE *arquivoAnterior = fopen(nome,"rb");
    if(!arquivoAnterior){ printf("\nN�o foi possivel abrir arquivo anterior para criacao do indice"); return -1; }

    ListaIndexPedido temp; temp.listaIndex=NULL; temp.qtdIndex=0;
    IndexPedido indexTemp; Pedido pedidoTemp;

    cursor =0; int move = 1;
    while(1){
        if(fseek(arquivoAnterior,((move*tamanhoBloco)-1)*(long)sizeof(IndexPedido),SEEK_SET)!= 0 ) break;
        if(fread(&indexTemp,sizeof(indexTemp),1,arquivoAnterior) != 1 ) break;
        strcpy(pedidoTemp.id_pedido,indexTemp.id_pedido);
        adicionar_index_pedido(&temp,pedidoTemp,(int)cursor);
        cursor+=tamanhoBloco; move++;
    }

    *qtdBlocos = move;
    fclose(arquivoAnterior);

    sprintf(nome, "IndicePedidoNivel-%d.bin", *nivel);
    FILE *arquivoNovo = fopen(nome,"wb");
    if(!arquivoNovo){ printf("\nErro ao criar novo arquivo de �ndice"); return -1; }
    fwrite(temp.listaIndex,sizeof(IndexPedido),(size_t)temp.qtdIndex,arquivoNovo);
    fclose(arquivoNovo);

    (*nivel)++;

    return temp.qtdIndex;
}

int organiza_indice_pedido()
{
    int nivelIndice = 1, qtdBlocos = 0, tamanhoBloco = 100;
    printf("\nCriando Arquivo de Indice de Pedidos...\n");
    ListaIndexPedido li; li.listaIndex=NULL; li.qtdIndex=0;
    lerPedidos("pedido.bin",&li);
    qsort(li.listaIndex, (size_t)li.qtdIndex, sizeof(IndexPedido), comparaIndexPedido);

    int qtd = li.qtdIndex;
    while(qtd > 10) qtd = cria_indice_pedido(&li,&nivelIndice,&qtdBlocos,tamanhoBloco);
    return nivelIndice-1;
}

int pesquisa_por_id_pedido(char idPedido[20],int nivel,int *posicao)
{
    char nome[50]; sprintf(nome, "IndicePedidoNivel-%d.bin", nivel);


    IndexPedido indexTemp; int cursor =0;

    if(nivel > 1){

        FILE *arquivo = fopen(nome,"rb");
        if(!arquivo){ printf("\nErro ao abrir arquivo"); return -2; }
        if (fseek(arquivo,(*posicao) * (long)sizeof(IndexPedido), SEEK_SET) != 0){
            printf("Erro ao posicionar no bloco %d\n", *posicao); fclose(arquivo); return -2;
        }
        while (fread(&indexTemp,sizeof(indexTemp), 1, arquivo) == 1){
            if (strcmp(idPedido, indexTemp.id_pedido) <= 0) break;
            cursor++;
        }
        fclose(arquivo);
        *posicao = indexTemp.posicao;
        return pesquisa_por_id_pedido(idPedido,nivel-1,posicao);
    }else{

        FILE *arquivo = fopen(nome,"rb");
        if(!arquivo){ printf("\nErro ao abrir arquivo"); return -2; }

        int posicaoIndice;
        if (fseek(arquivo,(*posicao)* (long)sizeof(IndexPedido), SEEK_SET) != 0){
            printf("Erro ao posicionar no bloco %d\n", *posicao); fclose(arquivo); return -2;
        }

        while (fread(&indexTemp, sizeof(indexTemp), 1, arquivo) == 1){
            (*posicao)++;
            if (strcmp(idPedido, indexTemp.id_pedido) < 0) break;
            cursor++;
        }

        posicaoIndice = (*posicao > 0) ? (*posicao)-1 : 0;

        if(strcmp(idPedido,indexTemp.id_pedido) == 0 && indexTemp.excluido == 0 ){

            Pedido p; FILE *arquivoPedido = fopen("pedido.bin","rb");
            if (!arquivoPedido){ printf("\nErro ao abrir pedido.bin"); fclose(arquivo); return -2; }
            fseek(arquivoPedido,indexTemp.posicao*(long)sizeof(Pedido),SEEK_SET);
            fread(&p,sizeof(p),1,arquivoPedido);
            printf("\nPedido encontrado\nID: %s\nData: %s\nPreco: %s",p.id_pedido, p.data, p.preco);
            printf("\nPosicao no arquivo de dados %d\n",indexTemp.posicao);
            fclose(arquivoPedido);
            fclose(arquivo);
            return indexTemp.posicao;
        }
        else if(indexTemp.excluido == 1){

            Pedido p; FILE *arquivoPedido = fopen("pedido.bin","rb");
            if (!arquivoPedido){ printf("\nErro ao abrir pedido.bin"); fclose(arquivo); return -2; }
            fseek(arquivoPedido,indexTemp.posicao*(long)sizeof(Pedido),SEEK_SET);
            fread(&p,sizeof(p),1,arquivoPedido);
            printf("\nPedido encontrado\nID: %s\nData: %s\nPreco: %s",p.id_pedido, p.data, p.preco);
            printf("\nPosicao no arquivo de dados %d\n",indexTemp.posicao);
            fclose(arquivoPedido);
            fclose(arquivo);
            return indexTemp.posicao;
        }
        else if(indexTemp.elo != -1){

            fclose(arquivo);
            Pedido p; FILE *arquivoPedido = fopen("pedido.bin","rb");
            if (!arquivoPedido){ printf("\nErro ao abrir pedido.bin"); return -2; }

            int qtdLeitura =0,maxLeitura; *posicao = indexTemp.elo; maxLeitura = indexTemp.tamExtensao;
            if (fseek(arquivoPedido,(*posicao)* (long)sizeof(Pedido), SEEK_SET) != 0){
                printf("Erro ao posicionar no bloco %d\n",*posicao); fclose(arquivoPedido); return -2;
            }
            while ( qtdLeitura< indexTemp.qtdExtensao && fread(&p, sizeof(Pedido), 1, arquivoPedido) == 1){
                printf("\n[DEBUG] Lendo extensao pos=%d | id_pedido=%s", *posicao, p.id_pedido);
                if (strcmp(idPedido, p.id_pedido) == 0){

                    fclose(arquivoPedido);
                    return (*posicao);
                }
                qtdLeitura++; (*posicao)++;
            }
            fclose(arquivoPedido);
            printf("\nPedido n�o encontrado na extens�o (lidos %d de %d)\n", qtdLeitura, maxLeitura);
            return -1;
        } else {

            int leituraPos = (posicaoIndice > 0) ? (posicaoIndice - 1) : 0;
            fseek(arquivo, (leituraPos) * (long)sizeof(indexTemp), SEEK_SET);
            fread(&indexTemp, sizeof(indexTemp), 1, arquivo);


            if ( (strcmp(idPedido, indexTemp.id_pedido)) == 0 && (indexTemp.excluido != 1)) {

                fclose(arquivo);
                (*posicao) = leituraPos;
                return indexTemp.posicao;
            }


            if(indexTemp.elo != -1){

                fclose(arquivo);
                Pedido p; FILE *arquivoPedido = fopen("pedido.bin","rb");
                if (!arquivoPedido){ printf("\nErro ao abrir pedido.bin"); return -2; }

                int qtdLeitura =0,maxLeitura; *posicao = indexTemp.elo; maxLeitura = indexTemp.tamExtensao;
                if (fseek(arquivoPedido,(*posicao)* (long)sizeof(Pedido), SEEK_SET) != 0){
                    printf("Erro ao posicionar no bloco %d\n",*posicao); fclose(arquivoPedido); return -2;
                }
                while ( qtdLeitura< indexTemp.qtdExtensao && fread(&p, sizeof(Pedido), 1, arquivoPedido) == 1){

                    if (strcmp(idPedido, p.id_pedido) == 0){

                        fclose(arquivoPedido);
                        return (*posicao);
                    }
                    qtdLeitura++; (*posicao)++;
                }
                fclose(arquivoPedido);

            }


            (*posicao) = leituraPos;
            fclose(arquivo);
            return -1;
        }
    }
}

int inserir_pedido(Pedido p, int nivel)
{
    int pesquisa = -1; int posicao = 0; IndexPedido indexTemp;

    printf("\n================ INSERCAO DE PEDIDO ================");
    printf("\nPedido a inserir: ID=%s | Data=%s | Preco=%s", p.id_pedido, p.data, p.preco);

    pesquisa = pesquisa_por_id_pedido(p.id_pedido, nivel, &posicao);


    if (pesquisa == -2){
            printf("\n[ERRO] Falha durante a pesquisa no indice.");
    return -1; }
    else if (pesquisa != -1){

        FILE *arquivoIndice = fopen("IndicePedidoNivel-1.bin", "rb+");
        FILE *arquivoDados  = fopen("pedido.bin", "rb+");
        if (!arquivoIndice || !arquivoDados){
            printf("\n[ERRO] Falha ao abrir arquivos.");
            if (arquivoIndice) fclose(arquivoIndice);
            if (arquivoDados)  fclose(arquivoDados);
            return -1;
        }

        fseek(arquivoIndice, posicao * (long)sizeof(IndexPedido), SEEK_SET);
        if (fread(&indexTemp, sizeof(IndexPedido), 1, arquivoIndice) != 1){
            printf("\n[ERRO] Falha ao ler indice na posicao %d.", posicao);
            fclose(arquivoIndice); fclose(arquivoDados); return -1;
        }

        if (indexTemp.excluido == 1){
            printf("\n[CASO] Reativando indice excluido...");

            if (indexTemp.elo != -1){
                printf("\n[VERIFICACAO] Checando ID na area de extensao...");
                FILE *arquivoPedido = fopen("pedido.bin", "rb");
                if (!arquivoPedido){ printf("\n[ERRO] Falha ao abrir pedido.bin."); fclose(arquivoIndice); fclose(arquivoDados); return -1; }

                Pedido pExt; int posAtual = indexTemp.elo; int encontrado = 0;
                fseek(arquivoPedido, posAtual * (long)sizeof(Pedido), SEEK_SET);
                for (int i = 0; i < indexTemp.qtdExtensao; i++){
                    if (fread(&pExt, sizeof(Pedido), 1, arquivoPedido) != 1) break;
                    if (strcmp(pExt.id_pedido, p.id_pedido) == 0){
                        encontrado = 1;
                        printf("\n[AVISO] ID %s ja existe na extensao (pos %d).", p.id_pedido, posAtual + i);
                        break;
                    }
                }
                fclose(arquivoPedido);
                if (encontrado){ fclose(arquivoIndice); fclose(arquivoDados); printf("\n[ABORTADO] Reativacao cancelada (ID duplicado)."); return 0; }
            }

            indexTemp.excluido = 0;
            strcpy(indexTemp.id_pedido, p.id_pedido);
            fseek(arquivoDados, indexTemp.posicao * (long)sizeof(Pedido), SEEK_SET);
            fwrite(&p, sizeof(Pedido), 1, arquivoDados);
            printf("\n[OK] Pedido regravado na posicao de dados %d.", indexTemp.posicao);

            fseek(arquivoIndice, posicao * (long)sizeof(IndexPedido), SEEK_SET);
            fwrite(&indexTemp, sizeof(IndexPedido), 1, arquivoIndice);
            printf("\n[OK] Indice atualizado e reativado.");
        }

        fflush(arquivoDados); fflush(arquivoIndice);
        fclose(arquivoDados); fclose(arquivoIndice);
        printf("\n[FINALIZADO] Insercao concluida.\n");
        return 1;
    } else {

        FILE *arquivoIndice = fopen("IndicePedidoNivel-1.bin", "rb+");
        FILE *arquivoDados  = fopen("pedido.bin", "rb+");
        if (!arquivoIndice || !arquivoDados){
            printf("\n[ERRO] Falha ao abrir arquivos.");
            if (arquivoIndice) fclose(arquivoIndice);
            if (arquivoDados)  fclose(arquivoDados);
            return -1;
        }

        fseek(arquivoIndice, posicao * (long)sizeof(IndexPedido), SEEK_SET);
        if (fread(&indexTemp, sizeof(IndexPedido), 1, arquivoIndice) != 1){
            printf("\n[ERRO] Falha ao ler indice na posicao %d.", posicao);
            fclose(arquivoIndice); fclose(arquivoDados); return -1;
        }

        if (indexTemp.elo != -1){

            long posInsercao = (long)indexTemp.elo + indexTemp.qtdExtensao;
            if (indexTemp.qtdExtensao < indexTemp.tamExtensao){
                fseek(arquivoDados, posInsercao * (long)sizeof(Pedido), SEEK_SET);
                fwrite(&p, sizeof(Pedido), 1, arquivoDados);
                indexTemp.qtdExtensao++;
                fseek(arquivoIndice, posicao * (long)sizeof(IndexPedido), SEEK_SET);
                fwrite(&indexTemp, sizeof(IndexPedido), 1, arquivoIndice);


            } else {
                printf("\n[ERRO] Extensao cheia (elo=%d, tam=%d).", indexTemp.elo, indexTemp.tamExtensao);
            }
        } else {

            Pedido vazio; strcpy(vazio.id_pedido, "9999999999999999999"); strcpy(vazio.data, "0000-00-00"); strcpy(vazio.preco, "0.00");

            fseek(arquivoDados, 0, SEEK_END);
            long inicioExtensao = ftell(arquivoDados) / (long)sizeof(Pedido);
            int tamanhoExtensao = (indexTemp.tamExtensao > 0) ? indexTemp.tamExtensao : 10;

            for (int i = 0; i < tamanhoExtensao; i++) fwrite(&vazio, sizeof(Pedido), 1, arquivoDados);

            fseek(arquivoDados, inicioExtensao * (long)sizeof(Pedido), SEEK_SET);
            fwrite(&p, sizeof(Pedido), 1, arquivoDados);

            indexTemp.elo = (int)inicioExtensao; indexTemp.qtdExtensao = 1; indexTemp.tamExtensao = tamanhoExtensao;
            fseek(arquivoIndice, posicao * (long)sizeof(IndexPedido), SEEK_SET);
            fwrite(&indexTemp, sizeof(IndexPedido), 1, arquivoIndice);


        }

        fflush(arquivoDados); fflush(arquivoIndice);
        fclose(arquivoDados); fclose(arquivoIndice);
        printf("\n[FINALIZADO]\n");
        return 1;
    }
}


void excluir_pedido(char id[20], int nivel)
{
    printf("\n================ EXCLUSAO DE PEDIDO ================");
    printf("\n[INFO] Pedido a excluir: ID=%s", id);

    int posicao = 0;
    int pesquisa = pesquisa_por_id_pedido(id, nivel, &posicao);
    if (pesquisa == -2){ printf("\n[ERRO] Nao foi possivel realizar a pesquisa pelo id\n"); return; }
    if (pesquisa == -1){ printf("\n[ERRO] Pedido nao encontrado para exclusao.\n"); return; }

    char nome[50]; sprintf(nome, "IndicePedidoNivel-%d.bin", 1);
    FILE *arquivoIndice = fopen(nome, "rb+");
    FILE *arquivoDados  = fopen("pedido.bin", "rb+");
    if (!arquivoIndice || !arquivoDados){
        printf("\n[ERRO] Falha ao abrir arquivos para exclusao.\n");
        if (arquivoIndice) fclose(arquivoIndice);
        if (arquivoDados)  fclose(arquivoDados);
        return;
    }

    IndexPedido indexTemp;
    fseek(arquivoIndice, posicao * (long)sizeof(IndexPedido), SEEK_SET);
    fread(&indexTemp, sizeof(IndexPedido), 1, arquivoIndice);

    if (indexTemp.excluido == 1) {
        printf("\n[AVISO] Pedido %s ja esta marcado como excluido (posicao %d).", id, posicao);
        if (indexTemp.elo != -1) {
            FILE *arquivoPedidos = fopen("pedido.bin", "rb+");
            Pedido pExt; int posAtual = indexTemp.elo; int encontrado = 0;
            for (int i = 0; i < indexTemp.qtdExtensao; i++) {
                fseek(arquivoPedidos, (posAtual + i) * (long)sizeof(Pedido), SEEK_SET);
                fread(&pExt, sizeof(Pedido), 1, arquivoPedidos);
                if (strcmp(pExt.id_pedido, id) == 0) {
                    strcpy(pExt.id_pedido, "9999999999999999999");
                    strcpy(pExt.data, "0000-00-00");
                    strcpy(pExt.preco, "0.00");
                    fseek(arquivoPedidos, (posAtual + i) * (long)sizeof(Pedido), SEEK_SET);
                    fwrite(&pExt, sizeof(Pedido), 1, arquivoPedidos);
                    fflush(arquivoPedidos);
                    printf("\n[OK] Pedido ID=%s excluido da area de extensao (posicao %d).", id, posAtual + i);
                    encontrado = 1; break;
                }
            }
            fclose(arquivoPedidos);
            if (!encontrado) printf("\n[ERRO] Pedido nao encontrado na area de extensao.");
        }
        printf("\n[FINALIZADO]\n");
        fclose(arquivoIndice); fclose(arquivoDados); return;
    }

    if (indexTemp.excluido == 0 && strcmp(indexTemp.id_pedido, id) == 0) {
        indexTemp.excluido = 1;
        fseek(arquivoIndice, posicao * (long)sizeof(IndexPedido), SEEK_SET);
        fwrite(&indexTemp, sizeof(IndexPedido), 1, arquivoIndice);
        fflush(arquivoIndice);


        Pedido p;
        fseek(arquivoDados, indexTemp.posicao * (long)sizeof(Pedido), SEEK_SET);
        fread(&p, sizeof(Pedido), 1, arquivoDados);
        strcpy(p.id_pedido, "9999999999999999999");
        strcpy(p.data, "0000-00-00");
        strcpy(p.preco, "0.00");
        fseek(arquivoDados, indexTemp.posicao * (long)sizeof(Pedido), SEEK_SET);
        fwrite(&p, sizeof(Pedido), 1, arquivoDados);
        fflush(arquivoDados);

    }
    else if (indexTemp.excluido == 0 && strcmp(indexTemp.id_pedido, id) != 0) {

        if (indexTemp.elo != -1) {
            FILE *arquivoPedidos = fopen("pedido.bin", "rb+");
            if (!arquivoPedidos){ printf("\n[ERRO] Falha ao abrir pedido.bin"); fclose(arquivoIndice); fclose(arquivoDados); return; }
            Pedido pExt; int posAtual = indexTemp.elo; int encontrado = 0;
            for (int i = 0; i < indexTemp.tamExtensao; i++) {
                fseek(arquivoPedidos, (posAtual + i) * (long)sizeof(Pedido), SEEK_SET);
                fread(&pExt, sizeof(Pedido), 1, arquivoPedidos);
                if (strcmp(pExt.id_pedido, id) == 0) {

                    strcpy(pExt.id_pedido, "9999999999999999999");
                    strcpy(pExt.data, "0000-00-00");
                    strcpy(pExt.preco, "0.00");
                    fseek(arquivoPedidos, (posAtual + i) * (long)sizeof(Pedido), SEEK_SET);
                    fwrite(&pExt, sizeof(Pedido), 1, arquivoPedidos);
                    fflush(arquivoPedidos);
                    printf("\n[OK] Pedido ID=%s excluido na posicao %d.", id, posAtual + i);
                    fseek(arquivoPedidos, (posAtual + i) * (long)sizeof(Pedido), SEEK_SET);
                    fread(&pExt, sizeof(Pedido), 1, arquivoPedidos);
                    printf("\n[DEBUG] Pos %d agora contem ID=%s", posAtual + i, pExt.id_pedido);
                    encontrado = 1; break;
                }
            }
            fclose(arquivoPedidos);

            if (encontrado) {
                fseek(arquivoIndice, posicao * (long)sizeof(IndexPedido), SEEK_SET);
                fwrite(&indexTemp, sizeof(IndexPedido), 1, arquivoIndice);
                fflush(arquivoIndice);

            }
        }
    }

    if (pesquisa && indexTemp.elo != -1) {
        Pedido pExt; int posAtual = indexTemp.elo; int encontrado = 0;
        for (int i = 0; i < indexTemp.qtdExtensao; i++) {
            fseek(arquivoDados, (posAtual + i) * (long)sizeof(Pedido), SEEK_SET);
            fread(&pExt, sizeof(Pedido), 1, arquivoDados);
            if (strcmp(pExt.id_pedido, id) == 0) {

                strcpy(pExt.id_pedido, "9999999999999999999");
                strcpy(pExt.data, "0000-00-00");
                strcpy(pExt.preco, "0.00");
                fseek(arquivoDados, (posAtual + i) * (long)sizeof(Pedido), SEEK_SET);
                fwrite(&pExt, sizeof(Pedido), 1, arquivoDados);
                fflush(arquivoDados);

                fseek(arquivoDados, (posAtual + i) * (long)sizeof(Pedido), SEEK_SET);
                fread(&pExt, sizeof(Pedido), 1, arquivoDados);
                printf("\n[DEBUG] Pos %d agora contem ID=%s", posAtual + i, pExt.id_pedido);
                encontrado = 1; break;
            }
        }


        FILE *arquivoPedidos = fopen("pedido.bin", "rb+");
        if (!arquivoDados){ printf("\n[ERRO] Falha ao abrir pedido.bin para sobrescrever."); fclose(arquivoIndice); fclose(arquivoDados); return; }

        fseek(arquivoDados, pesquisa * (long)sizeof(Pedido), SEEK_SET);
        fread(&pExt, sizeof(Pedido), 1, arquivoPedidos);

        strcpy(pExt.id_pedido, "9999999999999999999");
        strcpy(pExt.data, "0000-00-00");
        strcpy(pExt.preco, "0.00");

        fseek(arquivoPedidos, pesquisa * (long)sizeof(Pedido), SEEK_SET);
        fwrite(&pExt, sizeof(Pedido), 1, arquivoPedidos);
        fflush(arquivoPedidos);


        fclose(arquivoPedidos);
        fflush(arquivoIndice); fflush(arquivoDados);
        fclose(arquivoIndice); fclose(arquivoDados);

        return;
    }

    fflush(arquivoIndice); fflush(arquivoDados);
    fclose(arquivoIndice); fclose(arquivoDados);
    printf("\n[FINALIZADO] Exclusao concluida com sucesso.\n");
}
