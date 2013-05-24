/*
 * turn a unix text file into an MSDOS text file
 */
#include <stdio.h>

int main(int argc, char **argv)
{
  FILE *in;
  FILE *out;
  if (argc != 3) {
    printf("usage: lf infile outfile");
    return 1;
  }
  if ((in = fopen(argv[1],"rb")) == 0) {
    printf("cannot open %s for read",argv[1]);
    return 1;
  }
  out = fopen(argv[2],"wb");
  while (!feof(in)) {
    char a = fgetc(in);
    if (a == '\r')
      continue;
    if (a == '\n')
      fputc('\r',out);
    fputc(a,out);
  }
  fclose(in);
  fclose(out);
  return 0;
}