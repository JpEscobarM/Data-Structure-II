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

}Cidade;

typedef struct
{
    Cidade cidades[tam];
    int posicoesLivres[tam];
    int qtdLivre;
    int qtdCidades;

}ListaCidade;


typedef struct
{
    char nome[50];
    long posicao;

}IndiceNome;

typedef struct
{
    IndiceNome indices[tam];
    int qtdIndices;

}ListaIndiceNome;

void inicializarLista(ListaCidade *l)
{
    l->qtdLivre = 0;
    l->qtdCidades = 0;
}

void adicionaCidade(ListaCidade *l, Cidade c)
{

    l->cidades[l->qtdCidades] = c;
    l->qtdCidades++;
}



void inicializarIndicePorNome(ListaIndiceNome *indice)
{
    indice->qtdIndices = 0 ;
}

void adicionaIndice(ListaIndiceNome *lista,IndiceNome indice)
{
    lista->indices[lista->qtdIndices] = indice;
    lista->qtdIndices++;

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


//=================================


void imprimirCidades(ListaCidade *l)
{
    printf("\nimprimirCidades: \n");
    for(int i=0; i < l->qtdCidades; i++)
    {
        printf("[%d] = %s;%s;%d;\n",i,l->cidades[i].nome,l->cidades[i].uf,l->cidades[i].qtdHabitantes);
    }
}

void imprimirIndices(ListaIndiceNome *l)
{

    if(l->qtdIndices == 0 )
    {
        printf("\nArquivo de indices vazio\n");
        return;
    }
    printf("\nimprimirIndice: \n");
    for(int i=0; i < l->qtdIndices; i++)
    {
        printf("Chave[%s] | Posicao[%d]\n",l->indices[i].nome,l->indices[i].posicao);
    }
}

void lerArquivoBinario(ListaCidade *l)
{
   FILE *arquivo = fopen(".//arquivos//cidadesOrdemAlfabetica.bin","rb");
  if(!arquivo)
    {
        printf("\nlerArquivoBinario: arquivo nao pode ser aberto\n");
        return;
    }

    Cidade c;
    int cursor =1;

    while(1)
    {
      if (fseek(arquivo, (cursor - 1) * sizeof(Cidade), SEEK_SET) != 0)
            break;

      if (fread(&c, sizeof(Cidade), 1, arquivo) != 1)
            break;

     adicionaCidade(l,c);
     cursor++;
    }

    fclose(arquivo);
}

void lerArquivoIndice(ListaIndiceNome *lista)
{
   FILE *arquivo =  fopen(".//arquivos//indicePorNomeCidades.bin","rb");

    if(!arquivo)
    {
        printf("\nlerArquivoIndice: arquivo nao pode ser aberto\n");
        return;
    }

    IndiceNome idc;
    int cursor =1;

    while(1)
    {
      if (fseek(arquivo, (cursor - 1) * sizeof(IndiceNome), SEEK_SET) != 0)
            break;

      if (fread(&idc, sizeof(IndiceNome), 1, arquivo) != 1)
            break;

     adicionaIndice(lista,idc);
     cursor++;
    }

    fclose(arquivo);

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

void imprimirIndice(IndiceNome i)
{
    printf("Chave[%s] | Posicao %d",i.nome,i.posicao);
}

ListaIndiceNome *organizarIndice(ListaIndiceNome *indice, ListaCidade *listaCidades)
{
    free(indice);

    indice = malloc(sizeof(ListaIndiceNome));
    inicializarIndicePorNome(indice);

    FILE *arquivoCidades = fopen(".//arquivos//cidadesOrdemAlfabetica.bin", "rb");

    int tamanhoBloco = 4096/sizeof(Cidade);
    int qtdRestante = listaCidades->qtdCidades;

    long cursor = 0 ;


    IndiceNome idc;
    Cidade c;

    while(1)
    {

     if(fseek(arquivoCidades,cursor*sizeof(Cidade),SEEK_SET) != 0)
        break;

     if (fread(&c, sizeof(Cidade), 1, arquivoCidades) != 1)
        break;


     strcpy(idc.nome,c.nome);

     idc.posicao = cursor;

     adicionaIndice(indice,idc);


     qtdRestante = qtdRestante - tamanhoBloco;
     if(qtdRestante < 0)
     {
         break;
     }
     cursor = cursor+tamanhoBloco;

    }




    fclose(arquivoCidades);
    return indice;
}

void salvarArquivoIndice(ListaIndiceNome *lista)
{
   FILE *arquivoIndice = fopen(".//arquivos//indicePorNomeCidades.bin","wb");

    if (!arquivoIndice) {
        printf("Erro ao abrir arquivo de índice para escrita!\n");
        return;
    }

    size_t num_escritos = fwrite(lista->indices,sizeof(IndiceNome),lista->qtdIndices,arquivoIndice);

     if (num_escritos != lista->qtdIndices) {
        printf("Aviso: nem todos os índices foram gravados corretamente.\n");
    }
    else
    {
        printf("\nQuantidade de indices criados: %zu",  num_escritos);
    }

    fclose(arquivoIndice);
}

void pesquisarPorNome(char nome[50],ListaIndiceNome *listIndices)
{

    FILE *arquivoCidades = fopen(".//arquivos//cidadesOrdemAlfabetica.bin", "rb");
    Cidade c;
    c.qtdHabitantes = -1;
    long cursor;
    long fimBloco;
    int i =0;

     // se o nome é < nome na posicao indice[i], entao deve ser procurado no campo a partir do indice[i-1]
    while(i < listIndices->qtdIndices && strcasecmp(nome, listIndices->indices[i].nome) > 0 )
    {
        i++;
    }


    // se passou do último índice
    if (i == listIndices->qtdIndices)
    {
        cursor   = listIndices->indices[i-1].posicao;
        fseek(arquivoCidades,0,SEEK_END);
        fimBloco = ftell(arquivoCidades) / sizeof(Cidade);
    }
    else if (i == 0)
    {
        cursor   = listIndices->indices[0].posicao;
        fimBloco = listIndices->indices[1].posicao;
    }
    else
    {
        cursor   = listIndices->indices[i-1].posicao;
        fimBloco = listIndices->indices[i].posicao;
    }

    while( cursor <= fimBloco )
    {
        if(fseek(arquivoCidades,cursor*sizeof(Cidade),SEEK_SET) != 0) break;

        if(fread(&c,sizeof(Cidade),1,arquivoCidades)!= 1 ) break;

        if(strcasecmp(nome,c.nome) == 0 )
        {
             printf("\nCidade na posicao %d do arquivo", cursor);
              fclose(arquivoCidades);
             return;
        }

        cursor++;
    }


    printf("\nCidade nao encontrada no arquivo\n");
    fclose(arquivoCidades);


}

int main()
{

    ListaCidade *lista = malloc(sizeof(ListaCidade));
    ListaIndiceNome *indice = malloc(sizeof(ListaIndiceNome));

    char nome[50];


     int op = 1 ;


    inicializarLista(lista);
    inicializarIndicePorNome(indice);


    Cidade c;

    printf("Tamanho em bytes de cada cidade: %d",sizeof(c));

    FILE *arquivo = fopen(".//arquivos//cidadesOrdemAlfabetica.bin", "rb");
    fseek(arquivo, 0, SEEK_END);
    printf("\nTamanho maximo do arquivo cidadesOrdemAlfabetica.bin em bytes %d\n\n", ftell(arquivo));


    printf("valor classico de paginas usado -> 4096 bytes (4 KB).\nLogo = 4kb / 60 bytes (tamanho de cada cidade) = 68 cidades por bloco\n");

    fclose(arquivo);
    /*

    printf("\nLENDO ARQUIVO DE CIDADES:\n");
    lerArquivoBinario(lista);
    imprimirCidades(lista);

    printf("\nLENDO ARQUIVO DE INDICE:\n");

    lerArquivoIndice(indice);
    imprimirIndice(indice);

*/

    lerArquivoBinario(lista);
    lerArquivoIndice(indice);


    while(op != 0 )
    {   printf("\n\tOPCOES\n");
        printf("0 = sair\n");
        printf("1 = imprimir lista de cidades\n");
        printf("2 = imprimir indices\n");
        printf("3 = reorganizar indices\n");
        printf("4 = salvar indices\n");
        printf("5 = Pesquisa por nome");
        printf("\n");
        scanf("%d",&op);
        scanf("%*c");

        switch (op)
        {
        case 1:

            imprimirCidades(lista);

            break;
        case 2:
            imprimirIndices(indice);
            break;
        case 3:
            indice=organizarIndice(indice,lista);
            break;
        case 4:
            salvarArquivoIndice(indice);
            break;

          case 5:
                printf("\nDigite o nome da cidade: ");
                scanf("%49[^\n]", nome);
                printf("%s",nome);
                pesquisarPorNome(nome,indice);
            break;

        }
    }

    return 0;
}
