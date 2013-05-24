#include <stdio.h>
void main()
{
  int count;
  float quant;
  char units[21], item[21];
  do {
    count = fscanf(stdin, "%f%20s of %20s", &quant, units, item);
    fscanf(stdin,"%*[^\n]");
    printf("%f, %s, %s\n%d\n", quant, units, item, count);
  } while (count != EOF);
}
