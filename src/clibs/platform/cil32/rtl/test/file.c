#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

extern void __ll_uioinit(void);
extern int __ll_write(int, void *, size_t);
extern int __ll_read(int, void *, size_t);
int __ll_creat(char *name, int flags, int shflags);
int __ll_open(char *name, int flags, int shflags);
int __ll_close(int);
int __ll_uioflags(int);
int __ll_shflags(int);
int __ll_seek(int, size_t, int);
size_t __ll_getpos(int);
int __ll_dup(int);

int __uihandles[HANDLE_MAX];
main()
{
	__ll_uioinit();
	printf("%d\n",__ll_write(1,"hello",5));
	printf("%d\n",__ll_write(2,"goodbye", 6));
	char nn[256];
	int n = __ll_read(0,nn,10);
	printf("%d\n", n);
	__ll_write(1, nn, n);
	int t = __ll_creat("hidave", __ll_uioflags(O_RDWR), 0);
	printf("%d\n", __ll_write(t, "hellotheredave", 5+9));
	__ll_close(t);
        printf(":%d\n", t);
	printf("%d\n", __ll_write(t, "hellotheredave", 5+9));
	t = __ll_open("hidave", __ll_uioflags(O_RDWR), 0);
	printf("%d\n", __ll_write(t, "goodbye", 7));
	__ll_close(t);
        printf(":%d\n", t);
        t = __ll_open("hidave", __ll_uioflags(O_RDONLY),  __ll_shflags(0));
	printf("%d\n", __ll_write(t, "goodbye", 7));

	memset(nn, 0, sizeof(nn));
	printf("%d\n", __ll_read(t, nn, 4));
        printf("%s\n", nn);
	printf("%d\n", __ll_read(t, nn, 256));
        printf("%s\n", nn);
        printf("%d\n", __ll_getpos(t));
	__ll_seek(t, 1, SEEK_SET);
        printf("%d\n", __ll_getpos(t));
	__ll_seek(t, -1, SEEK_END);
        printf("%d\n", __ll_getpos(t));
	__ll_seek(t, 1, SEEK_SET);
        __ll_seek(t, 3, SEEK_CUR);
        printf("%d\n", __ll_getpos(t));
	__ll_seek(t, -2, SEEK_CUR);
        printf("%d\n", __ll_getpos(t));
	printf("%d\n", __ll_read(t, nn, 256));
        printf("%s\n", nn);
	__ll_seek(t, 4, SEEK_SET);
	int t1 = __ll_dup(t);
	__ll_close(t);
	printf("%d\n", __ll_read(t1, nn, 256));
        printf("%s\n", nn);
	printf("%d\n", __ll_read(t, nn, 256));
        printf(":%d\n", t);
	__ll_close(t1);
        printf(":%d\n", t1);
        t = __ll_open("hidave", __ll_uioflags(O_RDWR), 0);
        __ll_close(t);
        printf(":%d\n", t); 
}