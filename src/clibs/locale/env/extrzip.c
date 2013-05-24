#include <stdio.h>

unsigned char buf[65536];

int main()
{
    FILE *fil = fopen("env.zip","rb");
    int size,zsize,i,j;
    if (!fil) {
        printf("file not found");
        return 1;
    }
    fread(buf,0x1e,1,fil);
    size = *(int *)(buf + 0x16);
    zsize = *(int *)(buf + 0x12);
    fread(buf,buf[0x1a],1,fil);
    fread(buf+4,zsize,1,fil);
    *(int *)buf = size ;
    fclose(fil);
    fil = fopen("env.h","w");
    fprintf(fil,"unsigned char __envlocale[] = {\n");
    for (i=0; i < zsize + 4; i+=16) {
        fprintf(fil,"\t\"");
        for (j=0; j < 16; j++) {
            if (i + j >= zsize + 4)
                break ;
            fprintf(fil,"\\x%02X",buf[i+j]);
            
        }
        fprintf(fil,"\"\n");
    }
    fprintf(fil,"};\n");
}