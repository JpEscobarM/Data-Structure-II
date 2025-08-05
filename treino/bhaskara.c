#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(){


    int a, b, c, delta;

    printf("Informe os valores para A, B e C:\n");
    printf("A: ");
    scanf("%d",&a);
    printf("B: ");
    scanf("%d",&b);
    printf("C: ");
    scanf("%d",&c);
    printf("Equacao : %dx^2 + (%d) + (%d)\n",a,b,c);

    delta = b * b - 4 * a * c;


    if( b == 0 || c == 0 )
    {

        printf("\nEquação incompleta.\n");
    }
    else
    {
        if( delta < 0 )
        {

            printf("Delta %d\t nao possui raizes reais.\n", delta);

        }
        else
        {
            if(delta == 0 )
            {
                printf("Delta %d, possui raizes iguais\n",delta);

            }
            else
            {
                printf("Delta %d, possui raizes diferentes\n",delta);
                printf("Bhaskara: (-b +- sqrt(delta)) / (2 * a)\n");
                printf("Bhaskara: (-(%d) +- sqrt(delta)) / (2 * a)\n",b);
                printf("Bhaskara: (-(%d) +- %.1f) / (2 * a)\n",b,sqrt(delta));
                printf("Bhaskara: (-(%d) +- %.1f) / (2 * %d)\n",b,sqrt(delta),a);
                printf("Bhaskara: (%d +- %.1f) / (2 * %d)\n", -1*b,sqrt(delta),a);
                printf("Bhaskara: (%d +- %.1f) / %d\n", -1*b,sqrt(delta),2*a);
                 printf("X I = %.1f\n", ((-1*b)+sqrt(delta))/(2*a));
                  printf("X II = %.1f\n", ((-1*b)-sqrt(delta))/(2*a));

            }
        }
    }

return 0;
}
