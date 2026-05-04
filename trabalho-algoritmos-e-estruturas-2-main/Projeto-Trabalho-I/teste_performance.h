#if !defined(__teste_performance_h__)
#define __teste_performance_h__

#include <time.h>

#define MEDIR_TEMPO(titulo, codigo)                         \
    do {                                                    \
        double tempo;                                       \
        struct timespec inicio, fim;                        \
        clock_gettime(1, &inicio);                          \
        codigo;                                             \
        clock_gettime(1, &fim);                             \
        tempo = (fim.tv_sec - inicio.tv_sec)                \
            + (fim.tv_nsec - inicio.tv_nsec) / 1e9;         \
        printf("%s: Levou %lf segundos\n", titulo, tempo);  \
    } while(0)


void teste_performance(int nivel_produto);

#endif // __teste_performance_h__
