#include <stdio.h>
#include <string.h>

FILE *inasm, *innasm, *inoasm;

bool Getins(FILE *file, unsigned char *buf, int &count)
{
     count = 0;
     while (!feof(file))
     {
	int ch = fgetc(file);
	if (ch == -1)
		return false;
	buf[count++] = ch;
	if (count >=3 && buf[count-1] == 0x90 && buf[count-2] == 0x90 && buf[count-3] == 0x90)
	{
		int n = fgetc(file);
		if (n == 0x90)
		{
			count -= 2;
			return true;
		}
		else
		{
			ungetc(n, file);
			count -= 3;
			return true;
		}
	}        	
     }
     return !feof(file);
}
int Gather(unsigned char *nasm, int nasmc, char prefs[256])
{
	static char prefix[] = { 0x26, 0x2e, 0x36, 0x3e, 0x64, 0x65,0x66, 0x67, 0xf0, 0xf2, 0xf3,
				0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
				0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f};
	memset(prefs,0,256);
	int i;
	for (i=0; i < nasmc; i++)
	{
		int j;
		for (j=0; j < sizeof(prefix); j++)
			if (nasm[i] == prefix[j])
			{
				prefs[nasm[i]] = 1;
				break;
			}
		if (j == sizeof(prefix))
			return i;
	}
	
	return i;
}
bool CompareIns(unsigned char *nasm, int nasmc, unsigned char *oasm, int oasmc)
{
	char prefn[256], prefo[256];
	int noff = Gather(nasm, nasmc, prefn);
	int ooff = Gather(oasm, oasmc, prefo);
	if (nasmc - noff != oasmc - ooff)
		return false;
	for (int i=0; i < 256; i++)
		if (prefn[i] != prefo[i])
		{
			if (i != 0x36 && i != 0x3e || !prefn[i])
				return false;
		}

	for (int i=0; i < nasmc-noff; i++)
		if (nasm[i+noff] != oasm[i+ooff])
			return false;
	return true;

}
void DumpIns(unsigned char *buf, int len)
{
	for (int i=0; i < len; i++)
		printf( "%02X ",buf[i]);
}
int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("invalid command line");
		return 1;
	}
	char *name = argv[1];
        char asmfile[256], nasmfile[256], oasmfile[256];
	sprintf(asmfile, "%s.nas", name);	
	sprintf(nasmfile, "%s1.cmpx", name);	
	sprintf(oasmfile, "%s.o", name);
	inasm = fopen(asmfile, "r");
	innasm = fopen(nasmfile,"rb");
	inoasm = fopen(oasmfile, "rb");
	if (!inasm || !innasm || !inoasm)
	{
		printf("can't find file");
		return 1;
	}
	int instructioncount = 0;

	char asmline[256];
	fgets(asmline, 256, inasm);
	fgets(asmline, 256, inasm);
	unsigned char nasm[32],oasm[32];
	int nasmc, oasmc;
	if (Getins(innasm, nasm, nasmc) && Getins(inoasm, oasm, oasmc))
	{
		do
		{
			instructioncount++;
			if (!CompareIns(nasm, nasmc, oasm, oasmc))
			{
				while (asmline[strlen(asmline)-1] <= 32)
					asmline[strlen(asmline)-1] = 0;
				printf("%s: ",asmline);
				DumpIns(nasm, nasmc);
				printf(": ");
				DumpIns(oasm, oasmc);
				printf("\n");
			}
			fgets(asmline, 256, inasm);
			fgets(asmline, 256, inasm);
			fgets(asmline, 256, inasm);
			if (strstr(asmline, "[bits"))
			{
				fgets(asmline, 256, inasm);
				fgets(asmline, 256, inasm);
				fgets(asmline, 256, inasm);
				fgets(asmline, 256, inasm);
			}
			fgets(asmline, 256, inasm);
		} while (Getins(innasm, nasm, nasmc) && Getins(inoasm, oasm, oasmc));

	}
	printf("%d Instructions\n", instructioncount);
	return 0;
}