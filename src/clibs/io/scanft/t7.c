#include <stdio.h>

#define MAXLINE 80

int main()
{
  char buf[MAXLINE +1];
  int nlines;

  for (nlines = 0; gets(buf) != NULL; ++nlines) {
    int temp, offset = 0;
    char s[MAXLINE +1];

    printf("%d: ",nlines+1);
    while (sscanf(buf+offset,"%s%n",s,&temp) == 1) {
      printf("%s ",s);
      offset += temp;
    }
    putchar('\n');
  }
  return 0;
}
