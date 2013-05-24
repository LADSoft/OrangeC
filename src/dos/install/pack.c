#include <stdio.h>

void concat(FILE *out, FILE *in)
{
	while (!feof(in))
	{
		char buf[2048];
		int len = fread(buf, 1, 2048, in);
		if (len > 0)
			fwrite(buf, 1, len, out);
	}
}
int main()
{
	FILE *out = fopen("pinstall.exe", "rb+");
	FILE *in = fopen("C:\\orangec\\src\\doszip.zip", "rb");
	int offset;
	static char sig[4] = {'L', 'S', 0, 0 };
	if (!out)
	{
		printf("Cannot open destination file");
		exit(1);
	}
	if (!in)
	{
		printf("Cannot open source file");
		exit(1);
	}	
	fseek(out, 0, SEEK_END);
	offset = ftell(out);
	concat(out, in);
	fclose(in);
	fwrite(sig, 1, 4, out);
	fwrite(&offset, 1, 4, out);
	fclose (out);
}