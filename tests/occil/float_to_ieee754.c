  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>

char *strrev_t(char *str)
{
	char *p1, *p2;

	if (!str || !*str)
		return str;
	for (p1 = str, p2 = str + strlen(str) - 1; p2 > p1; ++p1, --p2)
	{
		*p1 ^= *p2;
		*p2 ^= *p1;
		*p1 ^= *p2;
	}
	return str;
}

void floatToBinary(float f, char *str, int numeroDeBits)
{
	int i = 0;
	int strIndex = 0;
	union
	{
		float f;
		unsigned int i;
	}u;
	u.f = f;
	memset(str, '0', numeroDeBits);

	for (i = 0; i < numeroDeBits; i++)
	{
		str[strIndex++] = (u.i & (1 << 0)) ? '1' : '0';
		u.i >>= 1;
	}

  	str[strIndex] = '\0';
  
	str = strrev_t(str);
}

int main()
{
	float input = 0.0;
	const int numeroDeBits = 32;
	char *str = (char*)malloc(sizeof(char) * numeroDeBits);
	printf("Digite um numero float: ");
	scanf("%f", &input);
	floatToBinary(input, str, numeroDeBits);
	printf("%s\n", str);
	if(str != NULL)
    free(str);     
	return 0;
}
