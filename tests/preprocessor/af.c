#include <stdio.h>
#include <stdlib.h>

void _RTL_FUNC _IMPORT __assertfail(const char* __who, const char* __file, int __line, const char* __func, const char* __msg)
{
	printf("%s %s  Where: (%s, %d, %s)", __who, __msg, __file, __line, __func);
	abort();
}
