#include <limits.h>
#include <float.h>

double d1 = { 4 };
double d2 = {4.0};
double d3 = {DBL_MAX};

float f1 = {4 };
float f2 = {4.0};
float f3 = {DBL_MAX};
float f4 = {d1};

unsigned long long ull1 = { 4 };
unsigned long long ull2 = { -1 };
unsigned long long ull3 = {400000};
unsigned long long ull4 = {ULLONG_MAX};

unsigned int ui1 = { 4 };
unsigned int ui2 = {-1};
unsigned int ui3 = {400000};
unsigned int ui4 = {ULLONG_MAX};
unsigned int ui5 = {ull1};

int i1 = {4 };
int i2 = {4000};
int i3 = {400000};
int i4 = {ui1};

short s1 = {4 };
short s2 = {4000};
short s3 = {400000};
short s4 = { i1} ;

unsigned char uc1 = { 4 };
unsigned char uc2 = {-1};
unsigned char uc3 = {128};
unsigned char uc4 = {s3 };

char c1 = {4 };
char c2 = {-1};
char c3 = {128};
char c4 = {4000};
char c5 = {s3};
char c6 = {uc1};

unsigned int ui6 = {c6};

struct aa
{
	int r;
	char s;
	float t;
};

aa st1 = {
4,
4,
4
};

aa st2 = {
ull1,
4,
4
};
aa st3 = {
4,
i1,
4
};
aa st4 = {
4,
4,
d1
};