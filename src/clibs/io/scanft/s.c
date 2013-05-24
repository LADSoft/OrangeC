#include <stdio.h>

void main()
{
  int nargs, nc, offset = 0;
  char s1[20], *s = "Friday August 014 1987";

  do {
    if ((nargs = sscanf(s+offset, "%s%n", s1, &nc)) != 1)
      break;
    printf("%d: %s (%d)\n", nargs, s1, nc);
    offset += nc;
  } while (*(s+offset));
}
