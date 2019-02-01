#include <stdio.h>
#include <stdlib.h>
#include "exefmt\PEHeader.h"

int main(int argc, char **argv)
{
    FILE *fil;
    char buf[1024];
    struct PEHeader * head;
    if (argc != 2)
    {
        printf("usage: PEPatch <file>");
        exit(1);
    }
    fil = fopen (argv[1], "rb+");
    if (!fil)
    {
        printf("can't find input file");
        exit(2);
        
    }
    fseek(fil, 0x3c, SEEK_SET);
    fread(buf, 4, 1, fil);
    fseek(fil, *(int*)buf, SEEK_SET);
    if (fread(buf, 1, sizeof(struct PEHeader), fil) != sizeof(struct PEHeader) ||
        buf[0] != 'P' || buf[1] != 'E')
    {
        printf("Invalid file");
        exit(2);
    }
    head = (struct PEHeader *)buf;
    head->dll_flags = 0x8140;
    head->os_major_version = 5;
    head->os_minor_version = 1;
    head->subsys_major_version = 5;
    head->subsys_minor_version = 1;
    fseek(fil, - sizeof(struct PEHeader), SEEK_CUR);
    fwrite(buf, sizeof(struct PEHeader), 1, fil);
    fclose(fil);
    printf("ok");
    return 0;
}