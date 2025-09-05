#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define tam 10


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

    printf("Sexo M ou F: ");
    scanf("%c",&nova.sexo);


    printf("Idade: ");
    scanf("%d",&nova.idade);
    scanf("%*c");


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




void inicializa(ListaPessoa *listaP)
{

    listaP->qtdPessoa = 0;

}


void imprimirVetor(ListaPessoa *l)
{
    for(int i = 0 ;  i < l->qtdPessoa ; i++ )
    {
        printf("\nNome: %s", l->pessoas[i].nome);
        printf("sexo: %c idade: %d\n", l->pessoas[i].sexo, l->pessoas[i].idade);
    }
}

int main(){


    ListaPessoa *lista = malloc(sizeof(ListaPessoa));

    inicializa(lista);

    if(cadastrarPessoa(lista))
    {
        printf("cadastro realizado\n");
    }

    imprimirVetor(lista);


return 0;
}
