#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ARRAY_SIZE 128000

int main(int argc, char **argv)
{
	int casas = argc == 2 ? atoi(argv[1]) : 1000;
	int x[ARRAY_SIZE];
	int xc;
	memset(x, 0, ARRAY_SIZE);

	printf("Numero de casas: %d\n", casas);

	xc = 0;
	xc++;
	x[xc] += casas;
	while (x[xc] != 0)
	{
		xc--;
		x[xc]++;
		xc += 8;
		x[xc] += 10;
		xc -= 7;
		x[xc]--;
	}
	xc++;
	x[xc] += 5;
	while (x[xc] != 0)
	{
		xc--;
		x[xc] += 9;
		xc++;
		x[xc]--;
	}
	x[xc]++;
	xc += 6;
	x[xc]++;
	while (x[xc] != 0)
	{
		xc -= 2;
		x[xc] += 3;
		while (x[xc] != 0)
		{
			xc += 2;
			while (x[xc] != 0)
			{
				x[xc]--;
				xc--;
			}
			xc--;
			while (x[xc] != 0)
			{
				xc++;
			}
			xc--;
			x[xc]--;
		}
		xc += 2;
		while (x[xc] != 0)
		{
			xc++;
			x[xc]++;
			xc++;
		}
		xc--;
		while (x[xc] != 0)
		{
			xc--;
		}
		xc++;
	}
	xc++;
	while (x[xc] != 0)
	{
		while (x[xc] != 0)
		{
			x[xc]--;
			xc += 4;
			x[xc]++;
			xc -= 4;
		}
		xc += 3;
		x[xc] += 3;
		xc++;
		x[xc]--;
	}
	xc--;
	while (x[xc] != 0)
	{
		xc -= 4;
	}
	xc -= 8;
	x[xc]++;
	while (x[xc] != 0)
	{
		x[xc]--;
		xc += 12;
		while (x[xc] != 0)
		{
			xc--;
			x[xc]++;
			while (x[xc] != 0)
			{
				x[xc]--;
				xc += 4;
				x[xc]++;
				xc -= 4;
			}
			xc++;
			xc += 4;
		}
		xc -= 4;
		while (x[xc] != 0)
		{
			xc += 5;
			while (x[xc] != 0)
			{
				xc -= 4;
				x[xc]++;
				xc += 4;
				x[xc]--;
			}
			xc -= 5;
			x[xc]--;
			while (x[xc] != 0)
			{
				xc -= 2;
				x[xc] += 10;
				xc += 2;
				x[xc]--;
			}
			xc += 3;
			while (x[xc] != 0)
			{
				xc -= 2;
				while (x[xc] != 0)
				{
					xc--;
					x[xc]++;
					xc -= 2;
					x[xc]++;
					xc += 3;
					x[xc]--;
				}
				xc--;
				while (x[xc] != 0)
				{
					xc++;
					x[xc]++;
					xc--;
					x[xc]--;
				}
				xc--;
				x[xc] += 2;
				xc -= 2;
				x[xc]++;
				xc += 6;
				x[xc]--;
			}
			xc -= 2;
			while (x[xc] != 0)
			{
				x[xc]--;
			}
			xc -= 2;
			x[xc]--;
			xc--;
			while (x[xc] != 0)
			{
				x[xc]--;
				xc += 2;
				x[xc]++;
				xc--;
				x[xc]--;
				while (x[xc] != 0)
				{
					xc += 3;
				}
				xc++;
				while (x[xc] != 0)
				{
					while (x[xc] != 0)
					{
						xc--;
						x[xc]++;
						xc++;
						x[xc]--;
					}
					xc++;
					x[xc]++;
					xc += 2;
				}
				xc -= 5;
			}
			xc++;
			while (x[xc] != 0)
			{
				x[xc]--;
			}
			xc++;
			x[xc]++;
			xc -= 3;
			x[xc]--;
			while (x[xc] != 0)
			{
				xc += 2;
				x[xc]++;
				xc -= 2;
				x[xc]--;
			}
			xc--;
		}
		xc -= 4;
		x[xc]++;
		xc += 7;
		xc++;
		while (x[xc] != 0)
		{
			x[xc]--;
		}
		xc++;
		while (x[xc] != 0)
		{
			xc -= 3;
			x[xc]++;
			xc += 3;
			x[xc]--;
		}
		xc -= 2;
		x[xc] += 10;
		xc--;
		while (x[xc] != 0)
		{
			x[xc]--;
			xc += 2;
			x[xc]++;
			xc--;
			x[xc]--;
			while (x[xc] != 0)
			{
				xc += 3;
			}
			xc++;
			while (x[xc] != 0)
			{
				while (x[xc] != 0)
				{
					xc--;
					x[xc]++;
					xc++;
					x[xc]--;
				}
				xc++;
				x[xc]++;
				xc += 2;
			}
			xc -= 5;
		}
		xc++;
		while (x[xc] != 0)
		{
			x[xc]--;
		}
		xc++;
		x[xc]++;
		xc++;
		while (x[xc] != 0)
		{
			xc -= 2;
			x[xc]++;
			xc--;
			x[xc]++;
			xc += 3;
			x[xc]--;
		}
		xc -= 3;
		xc--;
		x[xc]++;
		xc--;
		x[xc]++;
		xc += 2;
		while (x[xc] != 0)
		{
			x[xc]--;
			while (x[xc] != 0)
			{
				x[xc]--;
				while (x[xc] != 0)
				{
					x[xc]--;
					while (x[xc] != 0)
					{
						x[xc]--;
						while (x[xc] != 0)
						{
							x[xc]--;
							while (x[xc] != 0)
							{
								x[xc]--;
								while (x[xc] != 0)
								{
									x[xc]--;
									while (x[xc] != 0)
									{
										x[xc]--;
										while (x[xc] != 0)
										{
											x[xc]--;
											xc--;
											x[xc]--;
											xc++;
											while (x[xc] != 0)
											{
												x[xc]--;
												xc--;
												x[xc]++;
												xc--;
												x[xc]--;
												xc += 2;
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		xc--;
		while (x[xc] != 0)
		{
			x[xc] += 5;
			while (x[xc] != 0)
			{
				xc -= 3;
				x[xc] += 8;
				xc--;
				x[xc] += 8;
				xc += 4;
				x[xc]--;
			}
			xc--;
			xc -= 3;
			x[xc]++;
			xc--;
			x[xc]--;
			xc += 4;
			while (x[xc] != 0)
			{
				xc++;
				x[xc]++;
				xc -= 3;
				x[xc] += 9;
				xc--;
				x[xc]--;
				xc += 3;
				x[xc]--;
			}
			xc -= 5;
			while (x[xc] != 0)
			{
				xc += 2;
				x[xc]++;
				xc -= 2;
				x[xc]--;
			}
			x[xc]++;
			xc--;
			while (x[xc] != 0)
			{
				x[xc]--;
				xc++;
				x[xc]--;
				xc--;
			}
			xc++;
			while (x[xc] != 0)
			{
				xc += 2;
				putchar(x[xc]);
				xc -= 4;
				while (x[xc] != 0)
				{
					x[xc]++;
					putchar(x[xc]);
					while (x[xc] != 0)
					{
						x[xc]--;
					}
				}
				xc += 2;
				x[xc]--;
			}
			xc++;
			while (x[xc] != 0)
			{
				xc += 2;
				putchar(x[xc]);
				xc -= 2;
				x[xc]--;
			}
			xc++;
			while (x[xc] != 0)
			{
				x[xc]--;
			}
			xc++;
			while (x[xc] != 0)
			{
				x[xc]--;
			}
			xc += 3;
			while (x[xc] != 0)
			{
				xc += 2;
				while (x[xc] != 0)
				{
					xc -= 8;
					x[xc]++;
					xc += 8;
					x[xc]--;
				}
				xc -= 2;
				x[xc]--;
			}
		}
		xc += 2;
		while (x[xc] != 0)
		{
			x[xc]--;
		}
		xc -= 3;
		while (x[xc] != 0)
		{
			x[xc]--;
		}
		xc -= 8;
	}
	x[xc] += 10;
	putchar(x[xc]);
	printf("\n");
  return 0;
}
