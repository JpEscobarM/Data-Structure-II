#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define tam 300

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


int main()
{

    char nomePesquisa[50];
    int posicao;

    ListaCidade *lista = malloc(sizeof(ListaCidade));

    inicializaLista(lista);

    lerArquivoTexto(lista,0);

    salvarEmArquivoBinario(lista);



    int op =99;

    while (op != 0)
    {

        printf("\nEscolha uma opcao:\n");
        printf("1-Imprimir lista atual:\n");
        printf("2-Pesquisar em arquivo bin:\n");
        printf("3-Excluir por nome\n");
        printf("4-Inserir no arquivo binario\n");
        scanf("%d",&op);
        scanf("%*c");

        switch(op)
        {

        case 1:

            imprimirCidades(lista);

            break;

        case 2:

            printf("Digite o nome:\n");
            scanf(" %50[^\n]",nomePesquisa);
            posicao = pesquisaNomeEmArquivoBin(nomePesquisa,1);

            if(posicao == -1)
            {

             printf("\n\tElemento nao encontrado no arquivo.\n");
            }

            break;

            case 3:

            printf("Digite o nome:\n");
            scanf(" %50[^\n]",nomePesquisa);
            posicao = excluirEmArquivoBinPorNome(lista,nomePesquisa,0);

            if(posicao == -1)
            {

             printf("Elemento não encontrado no arquivo.\n");

            }
            else
            {
                printf("Elemento na posicao %d apagado com sucesso\n",posicao);
            }

            break;
            case 4:


                inserirEmArquivoBin(lista,0);

                break;

        case 0:

            op=0;

            break;

        default:

            printf("valor invalido\n");

        }

    }



    return 0;
}
