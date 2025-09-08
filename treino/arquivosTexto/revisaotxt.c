#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define tam 100


typedef struct
{
    char nome[50];
    int idade;
    char sexo;

}Pessoa;

typedef struct
{

      Pessoa pessoas[tam];

      int qtdPessoa;

}ListaPessoa;

void percorrePorChar(char *v)
{
    int i=0;

    while(v[i] != '\0')
    {

        printf("%c\n",v[i]);
        i++;
    }
}


Pessoa lerPessoa()
{
    Pessoa nova;

    printf("Nome: ");
    fgets(nova.nome,50,stdin);
    nova.nome[strcspn(nova.nome,"\n")]='\0';

    printf("Sexo M ou F: ");
    scanf("%c",&nova.sexo);


    printf("Idade: ");
    scanf("%d",&nova.idade);



    return nova;
}


int cadastrarPessoa(ListaPessoa *l)
{
    if(l->qtdPessoa == tam)
    {
        printf("\nVetor cheio\n");
        return 0;
    }

    Pessoa p = lerPessoa();

    l->pessoas[l->qtdPessoa] = p;

    l->qtdPessoa++;

    return 1;
}



int addPessoa(ListaPessoa *l,Pessoa p)
{
    if(l->qtdPessoa == tam)
    {
        printf("\nVetor cheio\n");
        return 0;
    }

    l->pessoas[l->qtdPessoa] = p;

    l->qtdPessoa++;

    return 1;
}


void inicializa(ListaPessoa *listaP)
{

    listaP->qtdPessoa = 0;

}


void imprimirVetor(ListaPessoa *l)
{
    for(int i = 0 ;  i < l->qtdPessoa ; i++ )
    {
        printf("\nNome: %s ", l->pessoas[i].nome);
        printf("sexo: %c idade: %d\n", l->pessoas[i].sexo, l->pessoas[i].idade);
    }
}

void salvarEmArquivo(ListaPessoa *l)
{
    FILE *arquivo = fopen("lista.txt","w");

    if(!arquivo)
    {
        return;
    }

    for(int i = 0; i < l->qtdPessoa; i++)
    {
        fprintf(arquivo,"%s;%c;%d;\n",l->pessoas[i].nome,l->pessoas[i].sexo,l->pessoas[i].idade);
    }

    fclose(arquivo);
}



void LerEmArquivo(ListaPessoa *l,int debug)
{
    FILE *arquivo = fopen("lista.txt","r");
    if (!arquivo)
    {
         return;
    }


    Pessoa p;

    while (fscanf(arquivo, "%49[^;];%c;%d;\n", p.nome, &p.sexo, &p.idade) == 3) {

       if(debug)
       {
           printf("Nome: %s | Sexo: %c | Idade: %d\n", p.nome, p.sexo, p.idade);
       }
        addPessoa(l,p);

    }



    fclose(arquivo);
}



void injetarPessoasGenericas(ListaPessoa *l) {
    for (int i = 0; i < 100; i++) {
        Pessoa p;

        sprintf(p.nome, "Pessoa_%d", i + 1);


        p.sexo = (i % 2 == 0) ? 'M' : 'F';


        p.idade = 18 + (i % 50);

        addPessoa(l, p);
    }

    salvarEmArquivo(l);
    printf("Foram injetadas 100 pessoas genericas e salvas em arquivo.\n");
}


int main() {
    ListaPessoa *lista = malloc(sizeof(ListaPessoa));

    inicializa(lista);

    LerEmArquivo(lista,0);

    int op = 1;

    while (op) {
    printf("\n--- MENU ---\n");
    printf("1 - Cadastrar pessoa\n");
    printf("2 - Listar pessoas\n");
    printf("3 - Salvar em arquivo\n");
    printf("4 - Injetar %d pessoas genericas\n",tam);
    printf("0 - Sair\n");
    printf("Escolha: ");
    scanf("%d", &op);
    scanf("%*c");

    switch (op) {
        case 1:
            if (cadastrarPessoa(lista)) {
                printf("Cadastro realizado com sucesso!\n");
            }
            break;

        case 2:
            imprimirVetor(lista);
            break;

        case 3:
            salvarEmArquivo(lista);
            printf("Lista salva em arquivo.\n");
            break;

        case 4:
            injetarPessoasGenericas(lista);
            break;

        case 0:
            printf("Encerrando...\n");
            break;

        default:
            printf("Opcao invalida!\n");
    }
}

    free(lista);
    return 0;
}
