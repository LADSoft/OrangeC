#include <setjmp.h>

jmp_buf jmpbuf;
void goto_s1(int a)
{
	int a1=5;
zz1:
	goto zz1;
}
void goto_s2(int a)
{
	int a1=5;
	goto zz1;
zz1:
}
void goto_s3(int a)
{
	if (a)
	{
zz:
	}
	else
	{
		goto zz;
	}
}
void goto_s4(int a)
{
	if (a)
	{
		if (a+1)
		{
zz:
		}
	}
	else
	{
		goto zz;
	}
}
void goto_s5(int a)
{
	if (a)
	{
zz:
	}
	else
	{
		if (a+1)
		{
			goto zz;
		}
	}
}
void goto_s6(int a)
{
	switch (a)
	{
		case 1:
		{
			int aa = 4;
		}
		break;
		case 2:
zz:
			break;
	}
	goto zz;
}
void goto_s7(int a)
{
        switch (a)
        {
		case 2:
			goto zz;
		case 1:
		{
			int aa = 4;
		}
		break;
		case 3:
zz:
			break;
	}
}
void goto_s8(int a)
{
	int qq = 23;
while (qq--)
{
	if (a)
	{
zz:
	}
	else
	{
		goto zz;
	}
}
}
void goto_s9(int a)
{
	int qq = 23;
zz1:
	if (a)
	{
	}
	else
	{
		if (qq)
			goto zz;
	}


	if (a)
{
zz:

		goto zz1;
}
	if (a + qq)
	{
		int nn = 3;
		goto zz;
	}
}
void goto_s10(int a)
{
	int r =44, s = 2;
zz:
	if (a == r)
    {
        if (r + s)
        {
    		if (a)
            {
                if (r)
                    if (s)
            			goto b;
            }
            /*
    		else
    			printf("hi");
    		if (r + s)
    		{
    				goto zz;
    		}
            */
        }
    }
	else
	{
    			b:
                if (r+s)
            		goto zz;
	}
}
void goto_f1(int a)
{
	if (a)
	{
		int t = 5;
zz:
	}
	else
	{
		goto zz;
	}
}
void goto_f2(int a)
{
	if (a)
	{
		if (a+1)
		{
		int t = 5;
zz:
		}
	}
	else
	{
		goto zz;
	}
}
void goto_f3(int a)
{
	if (a)
	{
		int t = 5;
zz:
	}
	else
	{
		if (a+1)
		{
			goto zz;
		}
	}
}