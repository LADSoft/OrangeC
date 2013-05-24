/*
 * example shows how to get the full path of the current
 * working directory
 */
#include <stdio.h>
#include <dir.h>

int main()
{
    char buf[MAXPATH];
    getcwd(buf,MAXPATH);
    printf("%s",buf);
}