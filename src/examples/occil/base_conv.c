#include <stdio.h>
#include <stdlib.h>

#define XOR_SWAP(a, b) ((a)^=(b),(b)^=(a),(a)^=(b))

void strReverser(char *p)
{
  char *q = p;
  while(q && *q) 
    ++q;
  for(--q; p < q; ++p, --q)
    XOR_SWAP(*p,*q);
}

char *converterBase(int valorEmDec, int base)
{
  char aux;
  char *result;
  int i=0;
  char map[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  
  if(valorEmDec == 0)
    return "0";
  else
    result = (char*)calloc(100, sizeof(char));
  
  while(valorEmDec > 0)
  {
    result[i++] = map[valorEmDec % base];
    valorEmDec /= base;
  }
  
  result[i++] = '\0';
  strReverser(result);
  
  return result;
}

int main()
{
  int valor;
  int base;
  
  do
  {
    printf("Digite um valor para converter: ");
    scanf("%d", &valor);
  }while(valor < 0);
  
  do
  {
    printf("Digite a base para qual você quer converter: ");
    scanf("%d", &base);
  }while(base < 2 || base > 36);
  
  printf("%s\n", converterBase(valor, base));  
  
  return 0;
}
