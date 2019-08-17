// source from the functions founded in this site: http://bisqwit.iki.fi/story/howto/bitmath

#include <stdio.h>

#define bool int
#define true 1
#define false 0
#define uint16_t unsigned short
#define uint32_t unsigned int

uint32_t xor_1(uint32_t v1, uint32_t v2)
{
	uint32_t a = v1;
	uint32_t b = v2;
	uint32_t nota = ~a; // Logical NOT
	uint32_t notb = ~b; // Logical NOT
	uint32_t result = (a & notb) | (b & nota);
	return result;
}

uint32_t xor_2(uint32_t v1, uint32_t v2)
{
	uint32_t a = v1;
	uint32_t b = v2;
	uint32_t result = (a | b) & ~(a & b);
	return result;
}

uint32_t xor_3(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
	uint32_t result = 0;
	uint32_t n;
	for (n = 0; n < 32; ++n)
	{
		// Test whether the bit is set in a but not b
		if (a & (1 << n))    // Test whether bit n is set in a
			if (!(b & (1 << n))) // Test whether bit n is not set in b
				result |= (1 << n); // Set bit N in target value

									// Test whether the bit is set in b but not a
		if (b & (1 << n))    // Test whether bit n is set in b
			if (!(a & (1 << n))) // Test whether bit n is not set in a
				result |= (1 << n); // Set bit N in target value
	}

	return result;
}

uint32_t xor_4(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
	uint32_t result = 0xFFFFFFFF;
	uint32_t mask;
  for (mask = 1; mask != 0; mask <<= 1)
	{
		// Deduct bit, if it's CLEAR in both operands
		if (!(a & mask)) // Test A and mask don't have common bits
			if (!(b & mask)) // Test B and mask don't have common bits
				result = result - mask;

		// Deduct bit, if it's SET in both operands
		if (a & mask) // Test A and mask have common bits
			if (b & mask) // Test B and mask have common bits
				result = result - mask;
	}

	return result;
}

uint32_t xor_5(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
	uint32_t result = 0;
	uint32_t mask = 1;
  int bit;
	for (bit = 0; bit<32; ++bit)
	{
		if (((a % 2) + (b % 2)) == 1) result = result + mask;
		a = a / 2; // Integer division, round towards zero
		b = b / 2; // Integer division, round towards zero
		mask = mask + mask; // Multiply mask by 2
	}
	return result;
}

uint16_t sum_1(uint16_t x, uint16_t y)
{
	uint16_t a = x;
	uint16_t b = y;
	uint16_t result = 0;
	bool math_carry = false;
	int bit;
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

uint16_t sum_2_nocarry(uint16_t x, uint16_t y)
{
	uint16_t a = x;
	uint16_t b = y;
	uint16_t result = 0;
	uint16_t mask = 1;
	while (mask != 0)
	{
		uint16_t nextmask = mask << 1;
		if (a & mask) // If A has that bit set?
		{
			// If result also has that bit set, set carry (next bit in result)
			if (result & mask) result ^= nextmask; // This could also be |= instead of ^= .
												   // Flip this bit in result
			result ^= mask;
		}
		if (b & mask) // If B has that bit set?
		{
			// If result also has that bit set, set carry (next bit in result)
			if (result & mask) result ^= nextmask; // This could also be |= instead of ^= .
												  
			result ^= mask;
		}
		mask = nextmask;
	}

	return result;
}

uint32_t sum_3_nocarry(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
	uint32_t carry = a & b;
	uint32_t result = a ^ b;
	while (carry != 0)
	{
		// If you need the mathematical carry from addition,
		// check the overflow from this shift.
		uint32_t shiftedcarry = carry << 1;
		carry = result & shiftedcarry;
		result = result ^ shiftedcarry;
	}
	return result;
}

uint16_t sum_4(uint16_t x, uint16_t y)
{
	uint16_t a = x;
	uint16_t b = y;
	uint32_t temp = a + b;

	bool   carry = temp >> 16;    // Bit-wise logical shift right
	uint16_t result = temp & 0xFFFF; // Bit-wise AND operation 
	return result;
}

uint16_t sum_5(uint16_t x, uint16_t y)
{
	uint16_t a = x;
	uint16_t b = y;
	uint16_t result = 0;
	uint16_t mask = 1;
	bool carry = false;
	while (mask != 0)
	{
		bool bit0 = a & mask;
		bool bit1 = b & mask;

		if (carry)
		{
			if (bit0 == bit1) result |= mask; carry = bit0 | bit1;
		}
		else
		{
			if (bit0 != bit1) result |= mask; carry = bit0 & bit1;
		}

		mask = mask << 1;
	}
	return result;
}

uint32_t logical_and_1(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
	uint32_t result;
  a = ~a; // Logical NOT
	b = ~b; // Logical NOT
	result = a | b; // Logical OR
	result = ~result; // Invert result
	return result;
}

uint32_t logical_and_2(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
	uint32_t result = 0;
	uint32_t n;
	for (n = 0; n < 32; ++n)
	{
		// Test whether the bit is set in both operands
		if (a & (1 << n)) // Test whether bit n is set in a
			if (b & (1 << n)) // Test whether bit n is set in a
				result |= (1 << n); // Set bit N in target value
	}
	return result;
}

uint32_t logical_and_3(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
	uint32_t result = 0;
	uint32_t mask;
  for (mask = 1; mask != 0; mask <<= 1)
	{
		// Test whether the bit is set in both operands
		if (a & mask) // Test A and mask have common bits
			if (b & mask) // Test B and mask have common bits
				result = result + mask;
	}
	return result;
}

uint32_t logical_and_4(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
	uint32_t result = 0;
	uint32_t mask = 1;
	int bit;
  for (bit = 0; bit<32; ++bit)
	{
		if ((a % 2) != 0 && (b % 2) != 0) result = result + mask;
		a = a / 2; // Integer division, round towards zero
		b = b / 2; // Integer division, round towards zero
		mask = mask + mask; // Multiply mask by 2
	}
	return result;
}

uint32_t logical_or_1(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
	uint32_t result;
  a = ~a; // Logical NOT
	b = ~b; // Logical NOT
	result = a & b; // Logical AND
	result = ~result; // Invert result
	return result;
}

uint32_t logical_or_2(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
	uint32_t result = 0;
	uint32_t n;
	for (n = 0; n < 32; ++n)
	{
		// Test whether the bit is set in either operand
		if (a & (1 << n)) // Test whether bit n is set in a
			result |= (1 << n); // Set bit N in target value
		if (b & (1 << n)) // Test whether bit n is set in a
			result |= (1 << n); // Set bit N in target value
	}
	return result;
}

uint32_t logical_or_3(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
	uint32_t result = 0xFFFFFFFF;
	uint32_t mask;
  for (mask = 1; mask != 0; mask <<= 1)
	{
		// Deduct bit, if it's CLEAR in both operands
		if (!(a & mask)) // Test A and mask don't have common bits
			if (!(b & mask)) // Test B and mask don't have common bits
				result = result - mask;
	}
	return result;
}

uint32_t logical_or_4(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
	uint32_t result = 0;
	uint32_t mask = 1;
	int bit;
  for (bit = 0; bit<32; ++bit)
	{
		if ((a % 2) != 0 || (b % 2) != 0) result = result + mask;
		a = a / 2; // Integer division, round towards zero
		b = b / 2; // Integer division, round towards zero
		mask = mask + mask; // Multiply mask by 2
	}
	return result;
}

uint32_t mul_1(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
	uint32_t result = 0;
	while (a != 0)
	{
		if (a & 1) // If the lowest order bit is set in A?
		{
			result = result + b;
		}
		a = a >> 1; // Note: This must set highest order bit ZERO. It must not copy the sign bit.
		b = b + b;  // Alternatively, left-shift by 1 bit
	}
	return result;
}

uint32_t mul_2(uint32_t x, uint32_t y)
{
	uint32_t a = x;
	uint32_t b = y;
  uint32_t result = 0;
	uint32_t stack[7];
  stack[0] = b; b = b + b; // b*1
	stack[1] = b; b = b + b; // b*2
	stack[2] = b; b = b + b; // b*4
	stack[3] = b; b = b + b; // b*8
	stack[4] = b; b = b + b; // b*16
	stack[5] = b; b = b + b; // b*32
	stack[6] = b;          // b*64

	// Repeat these lines for powers of two, according
	// to the upper limit of known value of A.
	if (a >= 64) { result = result + stack[6]; a = a - 64; }
	else { /* ignore stack[6] */ }
	if (a >= 32) { result = result + stack[5]; a = a - 32; }
	else { /* ignore stack[5] */ }
	if (a >= 16) { result = result + stack[4]; a = a - 16; }
	else { /* ignore stack[4] */ }
	if (a >= 8) { result = result + stack[3]; a = a - 8; }
	else { /* ignore stack[3] */ }
	if (a >= 4) { result = result + stack[2]; a = a - 4; }
	else { /* ignore stack[2] */ }
	if (a >= 2) { result = result + stack[1]; a = a - 2; }
	else { /* ignore stack[1] */ }
	if (a >= 1) { result = result + stack[0]; }
	else { /* ignore stack[0] */ }

	return result;
}

int main()
{
	int x = 90;
	int y = 18;

	printf("sum_1(2, 2) = %hu\n", sum_1(2, 2));
	printf("sum_2_nocarry(2, 2) = %hu\n", sum_2_nocarry(2, 2));
	printf("sum_3_nocarry(2, 2) = %d\n", sum_3_nocarry(2, 2));
	printf("sum_4(2, 2) = %hu\n", sum_4(2, 2));
	printf("sum_5(2, 2) = %hu\n", sum_5(2, 2));
	printf("xor_1(%d,%d) = %d == (%d ^ %d) = %d\n", x, y, xor_1(x, y), x, y, x ^ y);
	printf("xor_2(%d,%d) = %d == (%d ^ %d) = %d\n", x, y, xor_2(x, y), x, y, x ^ y);
	printf("xor_3(%d,%d) = %d == (%d ^ %d) = %d\n", x, y, xor_3(x, y), x, y, x ^ y);
	printf("xor_4(%d,%d) = %d == (%d ^ %d) = %d\n", x, y, xor_4(x, y), x, y, x ^ y);
	printf("xor_5(%d,%d) = %d == (%d ^ %d) = %d\n", x, y, xor_5(x, y), x, y, x ^ y);
	printf("logical_and_1(%d,%d) = %d == (%d & %d) = %d\n", x, y, logical_and_1(x, y), x, y, x & y);
	printf("logical_and_2(%d,%d) = %d == (%d & %d) = %d\n", x, y, logical_and_2(x, y), x, y, x & y);
	printf("logical_and_3(%d,%d) = %d == (%d & %d) = %d\n", x, y, logical_and_3(x, y), x, y, x & y);
	printf("logical_and_4(%d,%d) = %d == (%d & %d) = %d\n", x, y, logical_and_4(x, y), x, y, x & y);
	printf("logical_or_1(%d,%d) = %d == (%d | %d) = %d\n", x, y, logical_or_1(x, y), x, y, x | y);
	printf("logical_or_2(%d,%d) = %d == (%d | %d) = %d\n", x, y, logical_or_2(x, y), x, y, x | y);
	printf("logical_or_3(%d,%d) = %d == (%d | %d) = %d\n", x, y, logical_or_3(x, y), x, y, x | y);
	printf("logical_or_4(%d,%d) = %d == (%d | %d) = %d\n", x, y, logical_or_4(x, y), x, y, x | y);
	printf("mul_1(%d,%d) = %d == (%d * %d) = %d\n", x, y, mul_1(x, y), x, y, x * y);
	printf("mul_2(%d,%d) = %d == (%d * %d) = %d\n", x, y, mul_2(x, y), x, y, x * y);
	return 0;
}