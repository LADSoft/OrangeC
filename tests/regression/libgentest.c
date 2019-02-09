#include <libgen.h>

main()
{
	char buf[256];
    printf("%d\n", !strcmp(dirname(NULL), "."));
    printf("%d\n", !strcmp(dirname(""), "."));

	strcpy(buf, "/usr/lib");
	printf("%d\n", !strcmp(dirname(buf), "/usr"));
	strcpy(buf, "\\usr\\lib");
	printf("%d\n", !strcmp(dirname(buf), "\\usr"));
    strcpy(buf, "/usr/");
	printf("%d\n", !strcmp(dirname(buf), "/"));
    strcpy(buf, "\\usr\\");
	printf("%d\n", !strcmp(dirname(buf), "\\"));
    strcpy(buf, "usr");
	printf("%d\n", !strcmp(dirname(buf), "."));
    strcpy(buf, "/");
	printf("%d\n", !strcmp(dirname(buf), "/"));
    strcpy(buf, "\\");
	printf("%d\n", !strcmp(dirname(buf), "\\"));
    strcpy(buf, ".");
	printf("%d\n", !strcmp(dirname(buf), "."));
    strcpy(buf, "..");
	printf("%d\n", !strcmp(dirname(buf), "."));

    printf("%d\n", !strcmp(basename(NULL), "."));
    printf("%d\n", !strcmp(basename(""), "."));

	strcpy(buf, "/usr/lib");
	printf("%d\n", !strcmp(basename(buf), "lib"));
	strcpy(buf, "\\usr\\lib");
	printf("%d\n", !strcmp(basename(buf), "lib"));
    strcpy(buf, "/usr/");
	printf("%d\n", !strcmp(basename(buf), "usr"));
    strcpy(buf, "\\usr\\");
	printf("%d\n", !strcmp(basename(buf), "usr"));
    strcpy(buf, "usr");
	printf("%d\n", !strcmp(basename(buf), "usr"));
    strcpy(buf, "/");
	printf("%d\n", !strcmp(basename(buf), "/"));
    strcpy(buf, "\\");
	printf("%d\n", !strcmp(basename(buf), "\\"));
    strcpy(buf, ".");
	printf("%d\n", !strcmp(basename(buf), "."));
    strcpy(buf, "..");
	printf("%d\n", !strcmp(basename(buf), ".."));


}