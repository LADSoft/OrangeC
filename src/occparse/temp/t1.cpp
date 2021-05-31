#include <stdio.h>
int vv(
	int one,
	int two,
	int three,
	int four,
	int five,
	int six
)
{
	int aa = one + one + two + three + four + five + six
	 +  one + one + two + three + four + five + six
	 +  one + one + two + three + four + five + six
         +  one + one + two + three + four + five + six
	 +  one + one + two + three + four + five + six;
	return aa;
}
main()
{
	printf("%d\n", vv(1,2,3,4,5,6));
} 