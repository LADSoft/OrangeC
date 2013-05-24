#include <stdio.h>
#include "time.h"
#include "stdlib.h"
#include "string.h"

/* STDINC stuff compatible with our sys */

/* CCFILE is the new definition, FILE is the BCC definition */
typedef struct  {
        int             level;          /* fill/empty level of buffer */
        unsigned        flags;          /* File status flags          */
        char            fd;             /* File descriptor            */
        unsigned char   hold;           /* Ungetc char if no buffer   */
				char pad,pad1;									/* We need this to get BCC to define the
																				 * structure right */
        int             bsize;          /* Buffer size                */
        unsigned char   *buffer;   /* Data transfer buffer       */
        unsigned char   *curp;     /* Current active pointer     */
        unsigned        istemp;         /* Temporary file indicator   */
        short           token;          /* Used for validity checking */
}       CCFILE;                           /* This is the FILE object    */

#define _NFILE_ 100

extern FILE _streams[];

extern CCFILE *_PSTREAMS[_NFILE_];
extern char *_FILENAMES[_NFILE_];
extern int MAXFILES;

char inbuf[512];
char outbuf[512];
char errbuf[512];

#define FILTOK 0x444c

static CCFILE IN = {
	0,_F_READ | _F_LBUF,0,0,0,0,512,&inbuf,&inbuf,0,FILTOK
};
static CCFILE OUT = {
	0,_F_WRIT | _F_LBUF,1,0,0,0,512,&outbuf,&outbuf,0,FILTOK
};
static CCFILE ERR = {
	0,_F_WRIT | _F_LBUF,2,0,0,0,512,&errbuf,&errbuf,0,FILTOK
};

CCFILE *STDIN = &IN, *STDOUT = &OUT, *STDERR = &ERR;

/* REXIT is actually defined in the startup files and does not have to
 * be written explicitly
 */
void _REXIT(void)
{
	exit(0);
}
void _LL_INIT(void)
{
	table[numopen++] = &_streams[0];	
	table[numopen++] = &_streams[1];	
	table[numopen++] = &_streams[2];
	_PSTREAMS[MAXFILES++] = &IN;
	_PSTREAMS[MAXFILES++] = &OUT;
	_PSTREAMS[MAXFILES++] = &ERR;
}
int _LL_OPEN(char *__name, int flags)
{
	char buf[10],*p=buf;
	if (flags & _F_READ)
		*p++ = 'r';
	if (flags & _F_BIN)
		*p++ = 'b';
	if (flags & _F_WRIT)
		*p++ = '+';
	*p = 0;
	table[numopen] = fopen(__name,buf);
	if (table[numopen])
		return numopen++;
	return 0;
}
int _LL_CREAT(char *__name, int flags)
{
	char buf[10],*p=buf;
	if (flags & _F_WRIT)
		*p++ = 'w';
	if (flags & _F_BIN)
		*p++ = 'b';
	if (flags & _F_READ)
		*p++ = '+';
	*p = 0;
	table[numopen] = fopen(__name,buf);
	if (table[numopen])
		return numopen++;
	return 0;
}
int _LL_CLOSE(int __fd)
{
	int i;
	fclose(table[__fd]);
	for (i=__fd+1; i < numopen-1; i++)
		table[i] = table[i+1];
	numopen--;
	return 0;
}
size_t _LL_GETPOS(int __fd)
{
	return ftell(table[__fd]);
}
int _LL_FLAGS(int __oldflags)
{
	return __oldflags;
}
int _LL_SEEK(int __fd, size_t __pos, int __origin)
{
	fseek(table[__fd],__pos,__origin);
	return 0;
}
int _LL_RENAME(char *__old, char *__new)
{
	return rename(__old,__new);
}
int _LL_REMOVE(char *__name)
{
	return remove(__name);
}
int _LL_WRITE(int __fd,void *__buf,size_t __size)
{
	return !(fwrite(__buf,__size,1,table[__fd]) == __size);
}
int _LL_READ(int __fd,void *__buf,size_t __len)
{
	return fread(__buf,1,__len,table[__fd]);
}
void _LL_TRANSFER()
{
}
void *_LL_MALLOC(size_t size)
{
	return(malloc(size));
}
void _LL_FREE(void *block)
{
	free(block);
}
struct tm *_LL_GETTIME(struct tm *tm2)
{
	time_t t = time(0);
	*tm2 = *localtime(&t);
	return tm2;
}
_LL_TICKS()
{
	return 0;
}
_LL_SYSTEM(char *name)
{
	return 0;
}
_LL_GETENV(void)
{
	return 0;
}