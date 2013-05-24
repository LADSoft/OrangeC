/*
 * this example searches the current drive for all instances of a file or
 * list of files.  Wild cards may be used
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dir.h>

/* Give us a large stack since we will recurse */
int _stklen = 100000;

/*
 * cache argc and argv for use in the matching functions
 */
static char **arglist;
static int argcount;

/*
 * Match a wildcard filename spec against the current input file name
 */
static int matchone(char *name, int num)
{
    char *p = arglist[num];
    while (*p) {
        switch(*p) {
            case '*':
                while (*name && *name != '.')
                    name++;
                while (*p && *p != '.')
                    p++;
                break;
            case '?':
                if (*name && *name != '.')
                    name++;
                p++;
                break;
            default:
                if (toupper(*p++) != toupper(*name++))
                    return 0;
                break;
        }
    }
    return !*name;
}
/* 
 * Match the current input file name against all file specs in the argument
 * list
 */
static int matchall(char *name)
{
    int i;
    for (i=1; i < argcount; i++)
        if (matchone(name,i))
            return 1;
    return 0;
}
/*
 * Scan through a directory looking for normal files and directories.
 * Does NOT find hidden and system files
 */
static void scandir(char *path)
{
  int i;
  char pathbuf[256],dirbuf[256];
  struct ffblk data;

    /* Make the findfirst file name by concatenating *.* on our path */
 
    sprintf(dirbuf,"%s\\*.*",path);

  /* find the first file */
  if (findfirst(dirbuf,&data,_A_NORMAL | _A_SUBDIR))
    return;
  else {
        do {
            /* For each file that doesn't have a dot in front of it */
            if (data.ff_name[0] != '.') {

                /* Make the fully qualified path name */
                sprintf(pathbuf,"%s\\%s",path,data.ff_name);

                /* Check to see if it matches, and if so print the path */
                if (matchall(data.ff_name))
                    printf("%s\n",pathbuf);
    
                /* now if it is a subdirectory call ourselves recursively */
                if (data.ff_attrib & _A_SUBDIR)
                    scandir(pathbuf);
            }
            /* until there are no more files */
        } while (!findnext(&data));

  }
}
/*
 * Main line, just sets up the first path and initializes the program
 */
int main(int argc, char **argv)
{
    /* Get what drive we are on */
    int disk = getdisk();
    char buf[10];

    /* store our arguments for later */
    arglist = argv;
    argcount = argc;

    /* Exit with usage if they didn't give arguments */
    if (argcount < 2) {
        printf("Usage:  whereis <file list>");
        return 1;
    }

    /* Else constructed the path for the root directory ('/' will be added later) */
    buf[0] = disk + 'A';
    buf[1] = ':';
    buf[2] = 0;

    /* And scan through it */
    scandir(buf);

    return 0;
}