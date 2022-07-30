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
int main()
{
	Start();
	for (int i=0; i < 200000000/500; i++)
             calloc(500,1);
	Stop();
}