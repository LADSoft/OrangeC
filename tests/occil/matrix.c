#include <stdio.h>

void produtoMatriz(int A[][2], int B[][3], int C[][3],
                   int nLA, int nCA, 
                   int nLB, int nCB)
{
  int i, j, k;
  
  for(i=0;i<nLA;i++)
    for(j=0;j<nCB;j++)
      for(k=0;k<nLB;k++)
        C[i][j] += A[i][k] * B[k][j];
}

void zeraMatriz(int matriz[][3], int nL, int nC)
{
  int i, j;
  for(i=0;i<nL;i++)
    for(j=0;j<nC;j++)
      matriz[i][j] = 0;
}

void printMatriz(int matriz[][3], int nL, int nC)
{
  int i, j;
  for(i=0;i<nL;i++)
  {
    for(j=0;j<nC;j++)
      printf("%d ", matriz[i][j]);
    printf("\n");
  }   
}

int main()
{
  
  int matrizA[3][2] = {{1,4},{2,5},{3,6}};
  int matrizB[2][3] = {{7,8,9},{10,11,12}};
  int matrizC[3][3];
  
  zeraMatriz(matrizC,3,3);
  produtoMatriz(matrizA, matrizB, matrizC, 3, 2, 2, 3); 
  printMatriz(matrizC, 3, 3);

  return 0;
}
