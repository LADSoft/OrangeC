int *aa;
#include <stdio.h>
#include <windows.h>
 PASCAL DllMain(
 	HINSTANCE hinstDLL,
	DWORD fdwReason,
	LPVOID lpvReserved
	)
 {
    return 0;
 }
int __export vv(int argc, char **argv)
{
    printf("hi");
}