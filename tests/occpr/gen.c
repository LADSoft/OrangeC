#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

void onefile(char *dest, char *src)
{
	FILE *in = fopen(src,"r");
	FILE *out = fopen(dest, "w");
	if (in && out)
	{
		int base = random(4000);
		int step = random(4000);
		while (1)
		{
			char buf[8000];
			int n = fread(buf, 1, base + random(step), in);
			if (n == 0)
				break;
			fwrite(buf,1, n, out);
			fread(buf,1,random(4), in); // skip random chars			
		}
	}
	if (in)
		fclose(in);
	if (out)
		fclose(out);
}

int main(int argc, char *argv)
{
	time_t t = time(0);
	printf("seed: %x\n", t);
	srand(t);
	for (int i=1; i <= 2000; i++)
	{
		char buf[260];
		sprintf(buf,"bzip2_%d.c", i);
		onefile(buf, "bzip2.c");
	}
}