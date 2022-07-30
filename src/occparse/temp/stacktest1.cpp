#include <string>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stack>
#include <deque>

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
	for (int i=0; i < 1000000; i++)
        {
		std::stack<char *> aa;
	}
	Stop();
}