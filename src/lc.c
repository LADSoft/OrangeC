#include <windows.h>

#include <stdio.h>
void process(char *path)
{
    if (strstr(path, ".c") || strstr(path, ".asm") || strstr(path, ".h") || strstr(path, ".cpp") || strstr(path, ".nas"))
    {
        char newName[MAX_PATH], *p, *q = newName;
        char buf[1024];
        
        p = strrchr(path, '\\');
        if (p)
            p ++ ;
        else
            p = path;
        while (*p)
            *q++ = tolower(*p++);
        *q = 0;

        printf("%s\n", path);        
        sprintf(buf, "ren \"%s\" \"%s\"", path, newName);
        system(buf);
    }
}
void recurse(char *path)
{
    HANDLE hand;
    WIN32_FIND_DATA data;
    char root[MAX_PATH], search[MAX_PATH];
    strcpy(root, path);
    if (root[strlen(root)-1] != '\\')
        strcat(root, "\\");
    sprintf(search, "%s*.*", root);
    hand = FindFirstFile(search, &data);
    if (hand != INVALID_HANDLE_VALUE)
    {
        do
        {
            sprintf(search,"%s%s",root, data.cFileName);
            if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
//                if (data.cFileName[0] != '.' && stricmp(data.cFileName, "test"))
//                    recurse(search);
            }
            else
            {
                process(search);
            }
        } while (FindNextFile(hand, &data));
        FindClose(hand);
    }
          
                 
}
int main(int argc, char **argv)
{

    char buf[MAX_PATH];
    getcwd(buf, MAX_PATH);
    printf("%s\n", buf);
    recurse(buf);
}