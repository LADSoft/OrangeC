#include "..\stdinc\stdio.h"
#include <time.h>
#include <stdlib.h>
#include <string.h>

extern FILE *STDOUT,*STDIN;
FILE * FOPEN(char *,char *);
FILE * FREOPEN(char *,char *,FILE *);

void main(void)
{
	char buf[1000];
	int i;
	FILE *fil;
	_LL_INIT();

	FPUTS("hello\n",STDOUT);
	FPUTS("hello ",STDOUT);
	FPUTS("hello\n",STDOUT);
	FPUTS("hello\n",STDOUT);

	FGETS(buf,10,STDIN);
	PRINTF("%s",buf);

	SCANF("%d",&i);
	FPRINTF(STDOUT,"HELLO DAVE %d\n",i);

	fil = FOPEN("z1","wb");
	for (i=0; i < 523; i++)
		buf[i] = i & 0xff;
	i = FWRITE(buf,523,1,fil);
	PRINTF("%d\n",i);
	FCLOSE(fil);

	fil = FOPEN("z2","w");
	i = FWRITE(buf,523,1,fil);
	FCLOSE(fil);

	fil = FOPEN("Z2","r");
	i = FREAD(buf,1,1000,fil);
	PRINTF("%d %d %d\n",i,feof(fil),ferror(fil));
	FCLOSE(fil);

	fil = FOPEN("Z2","rb");
	i = FREAD(buf,1,1000,fil);
	PRINTF("%d %d %d\n",i,feof(fil),ferror(fil));

	fil = FREOPEN("Z1","rb+",fil);

	i = 500;
	FSEEK(fil,i,SEEK_SET);
	FGETPOS(fil,&i);
	PRINTF("%d %d\n",i,FTELL(fil));

	i = FREAD(buf,1,1000,fil);
	PRINTF("%d %d %d\n",i,feof(fil),ferror(fil));
	FGETPOS(fil,&i);
	PRINTF("%d %d\n",i,FTELL(fil));

	i = 50;
	FSEEK(fil,i,SEEK_SET);
	FGETPOS(fil,&i);
	PRINTF("%d %d\n",i,FTELL(fil));

	i = FREAD(buf,20,1,fil);
	FGETPOS(fil,&i);
	PRINTF("%d %d\n",i,FTELL(fil));

	i = 50;
	FSEEK(fil,i,SEEK_SET);
	FGETPOS(fil,&i);
	PRINTF("%d %d\n",i,FTELL(fil));

	i = FWRITE(buf,20,1,fil);
	FGETPOS(fil,&i);
	PRINTF("%d %d\n",i,FTELL(fil));
	FCLOSE(fil);

	fil = FOPEN("z1","rb");
	UNGETC('c',fil);
	UNGETC('a',fil);
	PRINTF("%d %d %d %d\n",FGETC(fil),FGETC(fil),FGETC(fil),FGETC(fil));
	UNGETC('c',fil);
	PRINTF("%d %d %d %d\n",FGETC(fil),FGETC(fil),FGETC(fil),FGETC(fil));
	FCLOSE(fil);

	for(i=0; i<20; i++)
		PRINTF("%s",TMPNAM());

	fil = TMPFILE();
	FPRINTF(fil,"hi dave");
	FCLOSE(fil);

	CLOSEALL();
}