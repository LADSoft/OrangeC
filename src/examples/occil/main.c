#include <stdio.h>

// ^
int xor(int a, int b)
{
    int x;
    int c = 0;
    for (x = 0; x <= 31; ++x)
    {
        c += c;
        if (a < 0)
        {
            if (b >= 0)
            {
                c += 1;
            }
        }
        else if (b < 0)
        {
            c += 1;
        }
        a += a;
        b += b;
    }
    return c;
}

// &
int and(int a, int b)
{
    int x;
    int c = 0;
    for (x = 0; x <= 31; ++x)
    {
        c += c;
        if (a < 0)
        {
            if (b < 0)
            {
                c += 1;
            }
        }
        a += a;
        b += b;
    }
    return c;
}

// | ( or )
int or(int a, int b)
{
    int x;
    int c = 0;
    for (x = 0; x <= 31; ++x)
    {
        c += c;
        if (a < 0)
        {
            c += 1;
        }
        else if (b < 0)
        {
            c += 1;
        }
        a += a;
        b += b;
    }
    return c;
}

int powtab[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, -32768 };

int shiftRight(int a, int shift)
{
    if (shift >= 15)
    {
        if (a < 0)
        {
            a = -1;
        }
        else
        {
            a = 0;
        }
    }
    else if (shift > 0)
    {
        if (a < 0)
        {
            a += -32768;
            a /= powtab[shift];
            a -= powtab[15 - shift];
        }
        else
        {
            a /= powtab[shift];
        }
    }
    return a;
}

unsigned int uShiftRight(int a, int shift)
{
    if (shift > 15)
    {
        a = 0;
    }
    else if (shift > 0)
    {
        if (a < 0)
        {
            a += -32768;
            a /= powtab[shift];
            a += powtab[15 - shift];
        }
        else
        {
            a /= powtab[shift];
        }
    }
    return a;
}

// signed
int shiftLeft(int a, int shift)
{
    if (shift > 15)
    {
        a = 0;
    }
    else
    {
        a *= powtab[shift];
    }
    return a;
}

int somar(int x, int y)
{
    int carry = 0;
    while (y != 0)
    {
        carry = and(x, y); 
        x = xor(x,y);         
        y = shiftLeft(carry, 1);   
    }
    return x;
}

void incrementar(int *variable, int qnt)
{
    *variable = somar(*variable, qnt);
}

int negar(int x)
{
    int retVal;
    retVal = somar(~x, 1);
    return retVal;
}

int subtrair(int x, int y)
{
    int retSub = 0;
    retSub = somar(x, negar(y));
    return retSub;
}

int dividir_por_dois(int numero)
{
    int resultado;
    resultado = shiftRight(numero,1);
    return resultado;
}

int multiplicar_por_dois(int numero)
{
    int resultado;
    resultado = shiftLeft(numero, 1);
    return resultado;
}

int ehPar(int x)
{
    int resultado;
	resultado = !(x & 1);
    return resultado;
}

int ehImpar(int x)
{
    int resultado;
    resultado = !(ehPar(x));
    return resultado;
}

int multiplicar(int x, int y)
{
    int produto = 0;
    
    if (x < 0 && y < 0)
        return multiplicar(negar(x), negar(y));

    if (x >= 0 && y < 0)
        return multiplicar(y, x);

    while (y > 0)
    {
        if (ehPar(y))
        {
            x = multiplicar_por_dois(x);
            y = dividir_por_dois(y);
        }
        else
        {
            produto = somar(produto, x);
            y = somar(y, -1);
        }
    }
    return produto;
}

int dividir(int dividendo, int divisor)
{
    int temp = 1;
    int quociente = 0;

    while (divisor <= dividendo)
    {
        divisor = shiftLeft(divisor, 1);
		temp = shiftLeft(temp,1);
    }

    while (temp > 1)
    {
        divisor = shiftRight(divisor, 1);
		temp = shiftRight(temp, 1);

        if (dividendo >= divisor)
        {
            dividendo = subtrair(dividendo, divisor);
            quociente = somar(quociente, temp);
        }
    }
    return quociente;
}

int main()
{
    int x = 90;
    int y = 18;
    int i = 0;
    int incremento = 20;

    printf("XOR LOGICO: %d^%d=%d\n", x, y, x^y);
    printf("XOR MATEMA: %d^%d=%d\n", x, y, xor(x, y));
    printf("RSHIFT LOGICO: %d\n", x >> 1);
    printf("RSHIFT MATEMA: %d\n", shiftRight(x, 1));
    printf("URSHIFT MATEM: %d\n", uShiftRight(x, 1));
    printf("LSHIFT LOGICO: %d\n", x << 1);
    printf("LSHIFT MATEMA: %d\n", shiftLeft(x, 1));
    printf("OR LOGICO: %d\n", x | y);
    printf("OR MATEMA: %d\n", or(x, y));
    printf("AND LOGICO: %d\n", x & y);
    printf("AND MATEMA: %d\n", and(x, y));

    printf("Somar............: 20+20=%d\n", somar(20, 20));
    printf("Subtrair.........: 30-10=%d\n", subtrair(30, 10));
    printf("Negar............:   ~10=%d\n", negar(10));
    printf("Multiplicar......: 70*27=%d\n", multiplicar(70, 27));
    printf("Dividir(inteiros): 36/06=%d\n", dividir(36, 6));
    printf("Multiplicar por 2: 70*02=%d\n", multiplicar_por_dois(70));
    printf("Dividir por 2....: 40/02=%d\n\n", dividir_por_dois(40));
    incrementar(&incremento, 1);
    printf("Incrementar: 20+01=%d\n", incremento);
    printf("7 eh impar.: %s\n", ehImpar(7) ? "true" : "false");
    printf("2 eh impar.: %s\n", ehImpar(2) ? "true" : "false");
    printf("8 eh par...: %s\n", ehPar(8) ? "true" : "false");
    printf("7 eh par...: %s\n", ehPar(7) ? "true" : "false");

    return 0;
}
