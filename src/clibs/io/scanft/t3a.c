#include <stdio.h>

int main()
{
  char *s = "hello there 12345.67589";
  char s1[6], s2[6], s3[6];
  int n1, n2, nargs, nc;
  float f;

  nargs = sscanf(s,"%20s%3s%s%3d%*5f%d%n",s1,s2,s3,&n1/*,&f*/,&n2,&nc);
  printf("%d: %s,%s,%s,%d,%f,%d (%d)\n",nargs,s1,s2,s3,n1,f,n2,nc);
  return 0;
}
