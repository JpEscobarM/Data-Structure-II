#if !defined(__util_h__)
#define __util_h__

#define NOT_FOUND (-1)

#define ARQUIVO_PRODUTOS ("produto.bin")
#define ARQUIVO_PEDIDOS ("pedido.bin")
#define ARQUIVO_ASSOCIACAO ("associacao.bin")
typedef unsigned long chave_t;
typedef unsigned long valor_t;

typedef struct array_t {
    int num_elementos;
    void *dados;
} array;

unsigned long str_para_long(const char *str);

array cria_array(int num_elementos, void *dados);

// realiza o hash de uma string
chave_t hash_str(const char *str);

int is_null(const char id[20]);

#endif // __util_h__
