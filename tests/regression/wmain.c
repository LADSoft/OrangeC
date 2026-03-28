#include <stdio.h>
#include <stdlib.h>
int func(int argc, wchar_t ** argv, wchar_t **env)
{
	printf("%d\n", argc);
        wchar_t *p = wcsrchr(argv[0], L'\\');
        if (p)
            p++;
        else
            p = argv[0];
        printf("%ls\n", p);
	for (int i=1 ; i < argc; i++)
		printf("%ls\n", argv[i]);
	printf("----------------\n");
}
int wmain(int argc, wchar_t **argv, wchar_t **env)
{
	func(argc,argv,env);
	func(__wargc, __wargv, _wenviron);
	return 50;
}
