#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int findCeil(float arr[], int r, int l, int h)
{
	int mid;
	while (l < h)
	{
		mid = l + ((h - l) >> 1);  
		(r > arr[mid]) ? (l = mid + 1) : (h = mid);
	}
	return (arr[l] >= r) ? l : -1;
}

unsigned int lfsr = 0xACE1u;
unsigned int bit, t = 0;

unsigned int mrand(unsigned int startNumber, unsigned int endNumber)
{
	if (startNumber == endNumber)
		return startNumber;
	int *p = (int*)malloc(sizeof(int));
	t = t ^ (int)p;
	bit = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5)) & 1;
	lfsr = ((bit << 15) | (lfsr >> 1) | t) % endNumber;
	while (lfsr < startNumber) {
		lfsr = lfsr + endNumber - startNumber;
	}
  free(p);
	return lfsr;
}

unsigned int lfsr113_Bits (void)
{
   static unsigned int z1 = 12345, z2 = 12345, z3 = 12345, z4 = 12345;
   unsigned int b;
   b  = ((z1 << 6) ^ z1) >> 13;
   z1 = ((z1 & 4294967294U) << 18) ^ b;
   b  = ((z2 << 2) ^ z2) >> 27; 
   z2 = ((z2 & 4294967288U) << 2) ^ b;
   b  = ((z3 << 13) ^ z3) >> 21;
   z3 = ((z3 & 4294967280U) << 7) ^ b;
   b  = ((z4 << 3) ^ z4) >> 12;
   z4 = ((z4 & 4294967168U) << 13) ^ b;
   srand(time(NULL));
   return (z1 ^ z2 ^ z3 ^ z4 ^ (mrand(1, 4294967168U)));
}

int myRand(int arr[], int freq[], int n)
{
	int i;
	float* prefix = malloc(sizeof(float) * n);
	prefix[0] = freq[0];
	for (i = 1; i < n; ++i)
		prefix[i] = prefix[i - 1] + freq[i];

  int r = (lfsr113_Bits() % (int)prefix[n - 1]) + 1;

	int indexc = findCeil(prefix, r, 0, n - 1);
	return arr[indexc];
}

#define SIZE 16 
int totalGerado = 0;
int* generated;
int RandomExist(int number)
{
	int i;
	for (i = 0; i < SIZE; i++)
		if (generated[i] == number)
			return 1;
	generated[totalGerado++] = number;
	return 0;
}

int listItens;
char** stringList;
int stringExist(char* str)
{
  int i = 0;
  for(;i<listItens;i++)
    if(strcmp(stringList[i], str) == 0)
      return 1;
  return 0;
}


int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

#define NUMERO_DE_JOGOS 25000
#define LISTALLOCATION NUMERO_DE_JOGOS * 2 

int main()
{
	int arr[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25 };
	int freq[] = 
  {
    849,    //1
    849,    //2
    840,    //3
    844,    //4
    824,    //5
    840,    //6
    806,    //7                          
    781,    //8
    831,    //9
    843,    //10
    872,    //11
    837,    //12
    865,    //13
    840,    //14
    842,    //15
    805,    //16
    827,    //17
    831,    //18
    833,    //19
    837,    //20
    835,    //21
    838,    //22
    837,    //23
    857,    //24
    847,    //25
  };
	int i, n = sizeof(arr) / sizeof(arr[0]);
	int num, k;
  int soma = 0;
  
  stringList = (char**)malloc(LISTALLOCATION);
  listItens=0;
  //strcpy(stringList[0], "Ola mundo!");
  //listItens++;
  
	for (int j = 0; j < NUMERO_DE_JOGOS; j++)
	{
    totalGerado = 0;
    generated = (int*)malloc(sizeof(int) * SIZE);
    char* tmp = (char*)malloc(sizeof(char) * 62);
  
		for (i = 0; i < SIZE; i++)
		{
			num = myRand(arr, freq, n);
			if (RandomExist(num) == 1)
			{
				i--;
				continue;
			}
			//printf("%d, ", num);
		}
    
    for(k=0;k<SIZE;k++)
      soma = soma + generated[k]; 
      
    if((soma / SIZE) == 11)
    {    
      qsort(generated, SIZE, sizeof(int), cmpfunc);
      
      sprintf(tmp, 
              "%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d,%02d",
              generated[0],generated[1],generated[2],generated[3],generated[4],generated[5],generated[6],
              generated[7],generated[8],generated[9],generated[10],generated[11],generated[12],generated[13],
              generated[14],generated[15]);
      
      if(stringExist(tmp))
      {
        j--;
      }
      else
      {
        stringList[listItens] = (char*)malloc(sizeof(char) * 62);
        strcpy(stringList[listItens], tmp);
        listItens++;
        
        printf("%s -- %d\n", tmp, (soma / SIZE));
      }
      
      //for(k = 0;k<SIZE;k++)
      //  printf("%02d, ", generated[k]);
      
  		//printf(" -- %d\n", (soma / SIZE));
    }
    else
      j--;
    
    soma = 0;
    free(generated);
    free(tmp);
	}

	return 0;
}