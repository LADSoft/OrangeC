// #include "..\stdinc\ctype.h"

void main(void)
{
	int i;
	printf("UPPER\n");
	for (i=0; i<256; i++)
		printf("%c %x %x\n",i,i,isupper(i));
	printf("LOWER\n");
	for (i=0; i<256; i++)
		printf("%c %x %x\n",i,i,islower(i));
	printf("ALPHA\n");
	for (i=0; i<256; i++)
		printf("%c %x %x\n",i,i,isalpha(i));
	printf("ALPHANUM\n");
	for (i=0; i<256; i++)
		printf("%c %x %x\n",i,i,isalnum(i));
	printf("HEX\n");
	for (i=0; i<256; i++)
		printf("%c %x %x\n",i,i,isxdigit(i));
	printf("DIGIT\n");
	for (i=0; i<256; i++)
		printf("%c %x %x\n",i,i,isdigit(i));
	printf("PUNCT\n");
	for (i=0; i<256; i++)
		printf("%c %x %x\n",i,i,ispunct(i));
	printf("SPACE\n");
	for (i=0; i<256; i++)
		printf("%c %x %x\n",i,i,isspace(i));
	printf("CONTROL\n");
	for (i=0; i<256; i++)
		printf("%c %x %x\n",i,i,iscntrl(i));
	printf("GRAPH\n");
	for (i=0; i<256; i++)
		printf("%c %x %x\n",i,i,isgraph(i));
	printf("ASCII\n");
	for (i=0; i<256; i++)
		printf("%c %x %x\n",i,i,isascii(i));
}