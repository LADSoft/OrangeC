#include <stdlib.h>
div_t DIV(int,int);

void main(void)
{
	div_t a;
	printf("%d\n",LABS(-4));
	printf("%d\n",LABS(10));
	printf("%d\n\n",LABS(0));
	printf("%d\n",RAND());
	printf("%d\n",RAND());
	printf("%d\n",RAND());
	printf("%d\n\n",RAND());
	SRAND(0x44);
	printf("%d\n",RAND());
	printf("%d\n",RAND());
	printf("%d\n",RAND());
	printf("%d\n\n",RAND());
	SRAND(0x44);
	printf("%d\n",RAND());
	printf("%d\n",RAND());
	printf("%d\n",RAND());
	printf("%d\n\n",RAND());
	a = DIV(50,7);
	printf("%d %d\n",a.quot,a.rem);
}