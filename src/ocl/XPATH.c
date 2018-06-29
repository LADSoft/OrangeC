#include "afx.h"

int last_char(char* str)
{
    int len;

    assert(str != NULL);
    len = strlen(str);
    if (len > 0)
        return str[len - 1];
    return 0;
}

char* add2path(char* path, char* add, int delim)
{
    static char toend[2];
    int c;

    assert(path != NULL);
    assert(add != NULL);
    c = last_char(path);
    if (c && c != delim)
    {
        toend[0] = delim;
        toend[1] = 0;
        strcat(path, toend);
    }
    return strcat(path, add);
}
