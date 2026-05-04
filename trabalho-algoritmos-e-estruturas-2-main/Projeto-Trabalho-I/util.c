#include "./util.h"

unsigned long str_para_long(const char *str) {
    unsigned long resultado = 0;

    while ((*str) != '\0') {
        resultado = resultado * 10 + (*str - '0');
        str++;
    }

    return resultado;
}

array cria_array(int num_elementos, void *dados) {
    array arr;

    arr.num_elementos = num_elementos;
    arr.dados = dados;

    return arr;
}

chave_t hash_str(const char* str) {
    int acc = 1;

    for (; *str != '\0'; str++) { 
        // faz loucuras com os bytes pra tentar aleatorizar o hash
        acc += (*str) * 567889;
        acc ^= (*str) - 74198;
        acc ^= (*str) - 33213;
        acc ^= (*str) - 43222;
    }

    return acc;
}

int is_null(const char id[20]) {
    int i;

    for (i = 0; i < 20; i++) {
        if (id[i] != '0') return 0;
    }

    return 1;
}