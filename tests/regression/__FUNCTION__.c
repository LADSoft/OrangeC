#include <stdio.h>
#define AA printf("%s\n", __FUNCTION__)

void aa()
{
	AA;
}
void bb()
{
	AA;
}
void cc()
{
	AA;
}
int main()
{
	AA;
	aa();
	bb();
	cc();
}