#include <stdio.h>

#define bool int
#define true 1
#define false 0

#define uint16_t unsigned short

uint16_t sum(uint16_t x, uint16_t y)
{
  int bit;
	uint16_t a = x;
	uint16_t b = y;
	uint16_t result = 0;
	bool math_carry = false;
	for (bit = 0; bit < 16; ++bit)
	{
		switch (math_carry)
		{
		case false:
		{
			bool bit0 = a & 1; a >>= 1; // Rotate-right A, get carry
			if (bit0)
				goto sum1_x;
		}

	sum0_x:
		{
			bool bit1 = b & 1; b >>= 1; // Rotate-right B, get carry
			if (bit1)
				goto sum1;
		}

	sum0:
		result >>= 1; // Clear math_carry, rotate-right result
		math_carry = false;
		continue;

	sum1_x:
		{
			bool bit1 = b & 1; b >>= 1; // Rotate-right B, get carry
			if (bit1)
				goto sum2;
		}

	sum1:
		result >>= 1;
		result |= 0x8000; // Set carry, rotate-right result
		math_carry = false;
		continue;

	sum2:
		result >>= 1; // Clear math_carry, rotate-right result
		math_carry = true;
		continue;

		case true:
		{
			bool bit0 = a & 1; a >>= 1; // Rotate-right A, get carry
			if (bit0)
				goto sum2_x;
		}
		goto sum1_x;
	sum2_x:
		{
			bool bit1 = b & 1; b >>= 1; // Rotate-right B, get carry
			if (bit1)
				goto sum3;
		}
		goto sum2;
	sum3:
		result >>= 1; result |= 0x8000; // Set carry, rotate-right result
		math_carry = true;
		continue;
		}
	}

	return result;
}

int main()
{
	unsigned short result = sum(2, 2);
	printf("%hu\r\n", result);
	return 0;
}