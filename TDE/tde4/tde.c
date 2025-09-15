#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define tam 300
#include <time.h>
typedef struct
{
    char nome[50];
    char uf[5];
    int qtdHabitantes;

} Cidade;

typedef struct
{
    Cidade cidades[tam];
    int posicoesLivres[tam];
    int qtdLivre;
    int qtdCidades;


} ListaCidade;

void inicializaLista(ListaCidade *l)
{

    l->qtdLivre =0;
    l->qtdCidades = 0;

}

void addCidade(ListaCidade *l, Cidade c)
{

    l->cidades[l->qtdCidades] = c;
    l->qtdCidades++;
}



//==============FUNCOES QUICKSORT===========

char *escolhePivo(ListaCidade *l,int inicioV, int fimV)
{
    int meio = (inicioV+fimV)/2;

    char *nomeInicio= l->cidades[inicioV].nome;
    char *nomeMeio = l->cidades[meio].nome;
    char *nomeFim = l->cidades[fimV].nome;


    char *pivo;

    if (strcmp(nomeInicio, nomeMeio) < 0)
    {
        if (strcmp(nomeMeio, nomeFim) < 0) pivo = nomeMeio;
        else if (strcmp(nomeInicio, nomeFim) < 0) pivo = nomeFim;
        else pivo = nomeInicio;
    }
    else
    {
        if (strcmp(nomeInicio, nomeFim) < 0) pivo = nomeInicio;
        else if (strcmp(nomeMeio, nomeFim) < 0) pivo = nomeFim;
        else pivo = nomeMeio;
    }


    return pivo;
}


int particiona(ListaCidade *l,int inicio, int fim)
{
    char *pivo = escolhePivo(l,inicio,fim);
    Cidade temp;

    while(inicio<fim)
    {
        while(inicio < fim && strcmp(l->cidades[inicio].nome,pivo) <= 0)
        {
            inicio++;
        }
        while(inicio < fim && strcmp(l->cidades[fim].nome,pivo) >= 0)
        {
            fim--;
        }

        temp = l->cidades[inicio];
        l->cidades[inicio] = l->cidades[fim];
        l->cidades[fim] = temp;
    }
return inicio;
}



void quickSort(ListaCidade *l,int inicio, int fim)
{
    int posicao;

    if(inicio < fim)
    {
       posicao = particiona(l,inicio,fim);

       quickSort(l,inicio,posicao-1);
       quickSort(l,posicao,fim);
    }

}


//============================================================

void lerArquivoTexto(ListaCidade *lista, int debug)
{


    FILE *arquivo = fopen(".//arquivos//cidades.txt","r");

    if(!arquivo)
    {
        printf("\nlerArquivoTexto: arquivo nao pode ser aberto\n");
        return;
    }

    if(debug) printf("\nlerArquivoTexto: arquivo aberto\n");

    Cidade c;

    while(fscanf(arquivo," %49[^;];%[^;];%d;",c.nome,c.uf,&c.qtdHabitantes)==3)
    {

        addCidade(lista,c);

        if (debug)
        {
            printf("Lida cidade: %s | UF: %s | Pop: %d\n", c.nome, c.uf, c.qtdHabitantes);

        }


    //adicionar quicksort

    }

    if(debug)
    {
        printf("\nQuantidade lida: %d", lista->qtdCidades);
    }
    fclose(arquivo);

}


void imprimirCidades(ListaCidade *l)
{
    printf("\nimprimirCidades: \n");
    for(int i=0; i < l->qtdCidades; i++)
    {
         if (strlen(l->cidades[i].nome) == 0)
            printf("[ERRO] Cidade %d com nome vazio!\n", i);

        printf("%s;%s;%d;\n",l->cidades[i].nome,l->cidades[i].uf,l->cidades[i].qtdHabitantes);
    }
}


void salvarEmArquivoBinario(ListaCidade *l)
{
    FILE *arquivo = fopen(".//arquivos//cidades.bin","wb");

    if(!arquivo)
    {
        printf("\nsalvarEmArquivoBinario: arquivo nao pode ser aberto\n");
        return;
    }

    fwrite(l->cidades,sizeof(Cidade),l->qtdCidades,arquivo);


    fclose(arquivo);
}

void salvarEmBinarioOrdemAlfabetica(ListaCidade *l)
{
    quickSort(l,0,l->qtdCidades-1);
    FILE *arquivo = fopen(".//arquivos//cidadesOrdemAlfabetica.bin","wb");

    if(!arquivo)
    {
        printf("\nsalvarEmBinarioOrdenadoPorNome: arquivo nao pode ser aberto\n");
        return;
    }

    fwrite(l->cidades,sizeof(Cidade),l->qtdCidades,arquivo);


    fclose(arquivo);
}

int pesquisaNomeEmArquivoBin(char nome[50],int debug)
{
    FILE *arquivo = fopen(".//arquivos//cidades.bin", "rb");



    if(debug)
    {
        printf("Nome de entrada: %s\n",nome);
    }

    if (!arquivo)
    {
        printf("\npesquisaNomeEmArquivoBin: arquivo nao pode ser aberto\n");
        return -1;
    }

    Cidade c;
    int cursor = 1;

    while (1)
    {

        if (fseek(arquivo, (cursor - 1) * sizeof(Cidade), SEEK_SET) != 0)
            break;

        if (fread(&c, sizeof(Cidade), 1, arquivo) != 1)
            break;

        if (debug)
        {
            printf("Elemento %d = %s | UF: %s | Populacao: %d\n",cursor, c.nome, c.uf, c.qtdHabitantes);

        }

        if( strcasecmp(c.nome,nome) == 0)
        {
           printf("\n\tElemento %d = %s | UF: %s | Populacao: %d\n",cursor, c.nome, c.uf, c.qtdHabitantes);


            fclose(arquivo);
            return cursor;
        }

        cursor++;
    }

    fclose(arquivo);

    return -1;
}


void imprimirArquivoBinDesordenado()
{
    FILE *arquivo = fopen(".//arquivos//cidades.bin", "rb");


    if (!arquivo)
    {
        printf("\nimprimirArquivoBinDesordenado: arquivo nao pode ser aberto\n");
        return -1;
    }

    Cidade c;
    int cursor = 1;
    while (1)
    {

        if (fseek(arquivo, (cursor - 1) * sizeof(Cidade), SEEK_SET) != 0)
            break;

        if (fread(&c, sizeof(Cidade), 1, arquivo) != 1)
            break;

        printf("Elemento %d = %s | UF: %s | Populacao: %d\n",cursor, c.nome, c.uf, c.qtdHabitantes);

        cursor++;
    }

    fclose(arquivo);

    return -1;
}
void imprimirArquivoBinOrdenado()
{
    FILE *arquivo = fopen(".//arquivos//cidadesOrdemAlfabetica.bin", "rb");


    if (!arquivo)
    {
        printf("\nimprimirArquivoBinDesordenado: arquivo nao pode ser aberto\n");
        return -1;
    }

    Cidade c;
    int cursor = 1;
    while (1)
    {

        if (fseek(arquivo, (cursor - 1) * sizeof(Cidade), SEEK_SET) != 0)
            break;

        if (fread(&c, sizeof(Cidade), 1, arquivo) != 1)
            break;

        printf("Elemento %d = %s | UF: %s | Populacao: %d\n",cursor, c.nome, c.uf, c.qtdHabitantes);

        cursor++;
    }

    fclose(arquivo);

    return -1;
}

int excluirEmArquivoBinPorNome(ListaCidade *l,char nome[50],int debug)
{
    FILE *arquivo = fopen(".//arquivos//cidades.bin", "rb+");



    if(debug)
    {
        printf("Nome de entrada: %s\n",nome);
    }

    if (!arquivo)
    {
        printf("\npesquisaNomeEmArquivoBin: arquivo nao pode ser aberto\n");
        return -1;
    }

    Cidade c;
    int cursor = 1;

    while (1)
    {

        if (fseek(arquivo, (cursor - 1) * sizeof(Cidade), SEEK_SET) != 0)
            break;

        if (fread(&c, sizeof(Cidade), 1, arquivo) != 1)
            break;

        if (debug)
        {
            printf("Elemento %d = %s | UF: %s | Populacao: %d\n",cursor, c.nome, c.uf, c.qtdHabitantes);

        }

        if( strcasecmp(c.nome,nome) == 0)
        {
            //printf("Elemento %d = %s | UF: %s | Populacao: %d\n",cursor, c.nome, c.uf, c.qtdHabitantes);

            fseek(arquivo, (cursor - 1) * sizeof(Cidade), SEEK_SET);

            strcpy(c.nome,"APAGADO"); //APAGA A POSICAO
            strcpy(c.uf,"APGD");
            c.qtdHabitantes = -1;

            fwrite(&c,sizeof(Cidade),1,arquivo);

            l->posicoesLivres[l->qtdLivre] = cursor;
            l->qtdLivre++;

         //   printf("Elemento %d = %s | UF: %s | Populacao: %d\n",cursor, c.nome, c.uf, c.qtdHabitantes);

            fclose(arquivo);

            return cursor;
        }

        cursor++;
    }

    fclose(arquivo);

    return -1;
}


void imprimirCidade(Cidade c)
{
    if(c.qtdHabitantes == -1)
    {
        printf("Registro apagado\n");
        return;
    }

    printf("Nome: %s|", c.nome);
    printf("UF: %s|", c.uf);
    printf("Populacao: %d\n", c.qtdHabitantes);

}

void salvarNaLista(ListaCidade *l)
{

    if (l->qtdCidades >= tam) {
    printf("[ERRO] Lista cheia. Não é possível adicionar mais cidades.\n");
    return;
}

    Cidade c;
    printf("Digite o nome da cidade: ");
    scanf(" %[^\n]", c.nome);


    printf("Digite a UF: ");
    scanf(" %[^\n]", c.uf);


    printf("Digite a quantidade de habitantes: ");
    scanf("%d", &c.qtdHabitantes);
    getchar(); // remove o '\n' deixado pelo scanf

    addCidade(l,c);
    imprimirCidade(c);
}

int inserirEmArquivoBin(ListaCidade *lista, int debug)
{
    FILE *arquivo = fopen(".//arquivos//cidades.bin","rb+");
    int cursor;

    Cidade c;
    printf("Digite o nome da cidade: ");
    scanf(" %[^\n]", c.nome);


    printf("Digite a UF: ");
    scanf(" %[^\n]", c.uf);


    printf("Digite a quantidade de habitantes: ");
    scanf("%d", &c.qtdHabitantes);
    getchar(); // remove o '\n' deixado pelo scanf

    imprimirCidade(c);


    if(!arquivo)
    {
        printf("inserirEmArquivoBin: Arquivo nao pode ser aberto\n");
        return -1;
    }

    if(debug)
    {
        printf("\tNome de entrada: %s | UF %s | Habitantes %d\n", c.nome, c.uf, c.qtdHabitantes);
    }


    Cidade search;

    if(lista->qtdLivre > 0)
    {
        cursor = lista->posicoesLivres[lista->qtdLivre-1];

        fseek(arquivo,(cursor -1)*sizeof(Cidade),SEEK_SET);//POSICIONA O CURSOR

        fread(&search,sizeof(Cidade),1,arquivo); //LE

        if(search.qtdHabitantes != -1) //CONFIRMA SE REALMENTE A POSICAO É VAZIA
        {
            printf("inserirEmArquivoBin: Erro de posicao invalida");
            return -1;
        }

        fseek(arquivo,(cursor-1)*sizeof(Cidade),SEEK_SET); // VOLTA NA POSICAO CORRETA ANTES DA LEITURA

        fwrite(&c,sizeof(Cidade),1,arquivo);//ESCREVE NA POSICAO CERTA

        lista->qtdLivre--;

        fclose(arquivo);
        return cursor;
    }
    else
    {

        fseek(arquivo,0,SEEK_END);

        fwrite(&c,sizeof(Cidade),1,arquivo);

        fclose(arquivo);

    }


    fclose(arquivo);
    return -1;
}



int pesquisaBinariaEmArquivoOrdenado(char nome[50],int *qtdComparacoes,int debug)
{
    FILE *arquivo = fopen(".//arquivos//cidadesOrdemAlfabetica.bin", "rb");



    if(debug)
    {
        printf("Nome de entrada: %s\n",nome);
    }

    if (!arquivo)
    {
        printf("\npesquisaBinariaEmArquivoOrdenado: arquivo nao pode ser aberto\n");
        return -1;
    }

    Cidade c;

    fseek(arquivo, 0, SEEK_END);
    long tamanhoArquivo = ftell(arquivo);
    int totalRegistros = tamanhoArquivo / sizeof(Cidade);
    int fim = totalRegistros;
    int inicio = 0;
    int meio;

    while (inicio <= fim)
    {

        meio  = (inicio+fim)/2;

       if(fseek(arquivo, meio * sizeof(Cidade), SEEK_SET) != 0)
        break;

        if (fread(&c, sizeof(Cidade), 1, arquivo) != 1)
            break;

      (*qtdComparacoes)++;

       int cmp = strcasecmp(c.nome,nome);


        if(debug)
        {
            printf("\nCOMPARACOES: %d", *qtdComparacoes);
            printf("\tDebug: %s | UF %s | Habitantes %d\n", c.nome, c.uf, c.qtdHabitantes);
        }

        if(cmp == 0 )
        {
            printf("\n\tELEMENTO ENCONTRADO = Posicao: %d | Nome de entrada: %s | UF %s | Habitantes %d\n",meio+1, c.nome, c.uf, c.qtdHabitantes);
            fclose(arquivo);
            return meio;
        }
        else if(cmp < 0)
        {
            inicio = meio+1;
        }
        else
        {
            fim = meio -1;
        }

    }

    fclose(arquivo);

    return -1;
}


int main()
{

    char nomePesquisa[50];
    int posicao;
    int comparacoes = 0;
    ListaCidade *lista = malloc(sizeof(ListaCidade));

    inicializaLista(lista);

    lerArquivoTexto(lista,0);

    salvarEmArquivoBinario(lista);



    int op =99;
    while (op != 0)
    {
  printf("\nEscolha uma opcao:\n");

    // ==== Operações com cidades.bin ====
    printf("1 - Pesquisar por nome no arquivo binario [cidades.bin]\n");
    printf("2 - Excluir por nome no arquivo binario [cidades.bin]\n");
    printf("3 - Inserir nova cidade no arquivo binario [cidades.bin]\n");
    printf("4 - Imprimir todas as cidades do arquivo binario [cidades.bin]\n");

    // ==== Operações com lista em memória ====
    printf("5 - Salvar nova cidade na lista atual [EM MEMORIA]\n");
    printf("6 - Imprimir lista atual em memória\n");
    printf("7 - Ordenar lista com QuickSort [EM MEMORIA]\n");

    // ==== Operações com cidadesOrdemAlfabetica.bin ====
    printf("8 - Salvar lista atual no arquivo [cidadesOrdemAlfabetica.bin]\n");
    printf("9 - Imprimir cidades do arquivo [cidadesOrdemAlfabetica.bin]\n");
    printf("10 - Pesquisa binária no arquivo [cidadesOrdemAlfabetica.bin]\n");
    printf("0 - Sair\n");

    scanf("%d", &op);
    scanf("%*c"); // limpa \n do buffer

    switch (op)
    {
        // cidades.bin
        case 1:
            printf("Digite o nome:\n");
            scanf(" %50[^\n]", nomePesquisa);
            posicao = pesquisaNomeEmArquivoBin(nomePesquisa, 1);
            if (posicao == -1)
                printf("\n\tElemento nao encontrado no arquivo.\n");
            break;

        case 2:
            printf("Digite o nome:\n");
            scanf(" %50[^\n]", nomePesquisa);
            posicao = excluirEmArquivoBinPorNome(lista, nomePesquisa, 0);
            if (posicao == -1)
                printf("Elemento não encontrado no arquivo.\n");
            else
                printf("Elemento na posicao %d apagado com sucesso\n", posicao);
            break;

        case 3:
            inserirEmArquivoBin(lista, 0);
            break;

        case 4:
            imprimirArquivoBinDesordenado();
            break;


        case 5:
            salvarNaLista(lista);
            break;

        case 6:
            imprimirCidades(lista);
            break;

        case 7:
            quickSort(lista, 0, lista->qtdCidades - 1);
            break;

        case 8:
            salvarEmBinarioOrdemAlfabetica(lista);
            break;
        case 9:
            imprimirArquivoBinOrdenado();
            break;
        case 10:
            comparacoes = 0;
            printf("Digite o nome para pesquisa binária:\n");
            scanf(" %50[^\n]", nomePesquisa);


            clock_t inicio = clock();

            posicao = pesquisaBinariaEmArquivoOrdenado(nomePesquisa, &comparacoes, 1);


            clock_t fim = clock();


            double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

            if (posicao == -1)
                printf("\n\tElemento NAO encontrado.\n");

            printf("Total de comparacoes: %d\n", comparacoes);
            printf("Tempo de execução: %.6f segundos\n", tempo);
            break;

        case 0:
            printf("Saindo...\n");
            break;

        default:
            printf("Valor invalido\n");
            break;
    }
}

    return 0;
}
