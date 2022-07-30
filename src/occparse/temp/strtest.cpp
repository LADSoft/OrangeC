#include <string>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int startTime, stopTime;

void Start()
{
	startTime = clock();
}
void Stop()
{
        stopTime = clock();
        printf("occparse timing: %d.%03d\n", (stopTime - startTime)/1000, (stopTime - startTime)% 1000); 
}
void GetLine (FILE* in, std::string&str)
{
	char buf[260];
	fgets(buf, 260, in);
	str += buf;
        str = str.substr(0, str.size());
}
int main()
{
	FILE *out = fopen("temp.txt", "w");

	char buf[257];
	for (int i=0; i < 100000; i++)
	{
		int r = random(256);
                memset(buf, 'a', r);
		buf[r] = 0;
		fprintf(out, "%s\n", buf);
	}
	fclose (out);
         
	FILE *in = fopen("temp.txt", "r");
	Start();
	while (!feof(in))
		fgets(buf, 81, in);
	Stop();	
	fclose(in);

	in = fopen("temp.txt", "r");
	std::string rv;
	Start();
	while (!feof(in))
	{
		rv = "";
		GetLine(in, rv);
	}
	Stop();	
	fclose(in);

}