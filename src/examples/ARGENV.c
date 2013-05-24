/*
 * this example displays the command line arguments and the environment
 * The 'wildargs' object file is included in the build for this file,
 * which will expand wild card file names
 *
 */
#include <stdio.h>

/*
 * note that the environment list as an argument to main() 
 * is an extension to C, it is *not* something standard but is 
 * generally present in MSDOS C compilers.
 *
 * for portablity, use the 'getenv()' function to get environment variable
 * values
 */
int main(int argc, char *argv[], char *env[])
{
  int i ;
  char **ptr;

  printf("Argument list:\n");
  for (i=0; i < argc; i++)
    printf("\t%d: \042%s\042\n",i,argv[i]);

  ptr = env;
  i = 0;
  printf("Environment:\n");
  while (*ptr)
    printf("\t%d: \042%s\042\n",i++,*ptr++);
}