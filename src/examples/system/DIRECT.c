/*
 * this example displays all the (normal) files in the current directory
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dir.h>

#define FIND_ALL 0xF7                  /* all but volume label */

/* files are listed here */
char *filelist[100];

/* a comparison routine for qsort */
int compare(const void *a, const void *b)
{
  return strcmp(*(char **)a,*(char **)b);
}
int main(int argc, char **argv)
{
  int i,count= 0;
  struct ffblk data;
  char filespec[260] = "*.*";

  /* if they specified a file to search for, copy it */
  if (argc > 1)
      strcpy(filespec, argv[1]);
      
  /* find the first file */
  if (findfirst(filespec,&data,FIND_ALL))
    printf("No files");
  else {
    /* we have one, put it in the list */
    filelist[count++] = strdup(data.ff_name);

    /* loop adding all remaining files to the list */
    while (!findnext(&data)) {
      filelist[count++] = strdup(data.ff_name);
      if (count > 99) {
        printf("too many files, truncating list");
        break;
      }
    }
    /* now sort the list */
    qsort(filelist,count,sizeof(char *),compare);

    /* and print it out */
    for (i=0; i < count; i++)
      printf("%s\n",filelist[i]);
  }
  return 0;
}
