#include <stdio.h>
#include <stdlib.h>

int main()
{
    int m = 0;
    double **a = 0;    
    int i = 0, j = 0, k = 0;	
    double factor = 0;	
    double temp = 0;	
    int count = 0;	

    printf("dimensao => ");
    scanf("%d", &m);

    a = (double **) calloc(m, sizeof(double *));
    for(i = 0; i < m; i++)
    {
        a[i] = (double *) calloc(m, sizeof(double));
    }

    printf("\n\nEntre com o conteudo da matriz\n\n");
    for(i = 0; i < m; i++)
    {
        for(j = 0; j < m; j++)
        {
            printf("A[%d ; %d] => ", i+1, j+1);
            scanf("%lf", &a[i][j]);
        }
    }

    // mostra a matriz
    printf("\nMatriz digitada:\n");
    for(i = 0; i < m; i++)
    {
        for(j = 0; j < m; j++)
        {
            printf("%8.3f ", a[i][j]);
        }
        printf("\n");
    }

    // faz a transformação em um triangulo...
    for(i = 0; i < m - 1; i++)
    {
        if(a[i][i] == 0)
        {
            for(k = i; k < m; k++)
            {
                if(a[k][i] != 0)
                {
                    for(j = 0; j < m; j++)
                    {
                        temp = a[i][j];
                        a[i][j] = a[k][j];
                        a[k][j] = temp;
                    }
                    k = m;
                }
            }
            count++;
        }

        if(a[i][i] != 0)
        {
            for(k = i + 1; k < m; k++)
            {
                factor = -1.0 * a[k][i] /  a[i][i];
                for(j = i; j < m; j++)
                {
                    a[k][j] = a[k][j] + (factor * a[i][j]);
                }
            }
        }
    }

    temp = 1.0;
    // Calcula o determinante
    for(i = 0; i < m; i++)
        temp *= a[i][i];

    printf("\nDeterminante:\n");
    if(count % 2 == 0)
        printf("%8.3f \n", temp);
    else
        printf("%8.3f \n", -1.0 * temp);

    return 0;
}