#include <stdio.h>

void main()
{
  int nargs, nc;
  char s1[20];

  do {
    if ((nargs = scanf("%s %n", s1, &nc)) != 1)
      break;
    printf("%d: %s (%d)\n", nargs, s1, nc);
  } while (*s1);
}
