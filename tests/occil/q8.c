#include <Stdio.h>
#include <Stdlib.h>
#include <Windows.h>
//__using__ "user32";
/*
int vv(int m,int n)
{
    int zz[m][n];

}
*/
int qq(int m, int n)
{
    for (int i=0; i < 10; i++)
    {
        printf("%d\n", i);
        char *a = malloc(i);
        int zz[m + i][n + i];
        for (int k=0; k < m +i; k++)
            for (int j=0; j < n+i; j++)
                zz[k][j] = i + j + k;
        for (int k=0; k < m +i; k++)
            for (int j=0; j < n+i; j++)
                printf("%d ", zz[k][j]);
        printf("\n");


    }
    return 0;
}
main()
{
    MessageBox(0, "hi", "hi", 0);
    return qq(5,3);
}
    