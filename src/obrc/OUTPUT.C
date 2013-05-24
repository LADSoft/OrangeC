/* 
Browse Linker
Copyright 2003-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

You may contact the author at:
	mailto::camille@bluegrass.net
 */
#include <stdio.h>
#include "cmdline.h"
#include "browse.h"
#include "umem.h"
#include "dict.h"

#define BROWSE_VERSION 2
extern int dictpages;
extern HASHREC **symhash;

extern char **filelist;
extern int filelistcount;

static char blankbuf[512];

static int pad(FILE *fil, int ofs, int size)
{
    int len = size - (ofs % size);
    if (len == size)
        return ofs;
    fwrite(blankbuf, 1, len, fil);
    return ofs + len;
}

//-------------------------------------------------------------------------

static int putlinedata(FILE *fil, struct linedata *l)
{
    char buf[256];
    int len;
    memset(buf, 0, 256);
    len = 19+strlen(l->hint);
    if (len % 2)
        len += 2-(len % 2);
    *(short*)buf = len;
	*(short*)(buf+2) = l->type | (l->blocklevel == 0 ? 0x4000 : 0);
    *(int*)(buf + 4) = l->startline;
    *(int*)(buf + 8) = l->endline;
    *(int*)(buf + 12) = l->filenum;
    *(short*)(buf + 16) = l->charpos;
    buf[18] = strlen(l->hint);
    strcpy(buf + 19, l->hint);
    fwrite(buf, 1, len, fil);
    return len;
}
int lineTableCmp(const void *left, const void *right)
{
	struct symdata *lleft = *(struct symdata **)left;
	struct symdata *rright = *(struct symdata **)right;
	if (lleft->values->filenum < rright->values->filenum)
		return -1;
	if (lleft->values->filenum > rright->values->filenum)
		return 1;
	if (lleft->values->startline < rright->values->startline)
		return -1;
	if (lleft->values->startline > rright->values->startline)
		return 1;
	return 0;		
}
int WriteLineTable(FILE *fil, int ofs) 
{
	int i = 0, n = 0;
	int base = ofs;
	int count = 0;
	struct symdata **list;
    for (i = 0; i < HASH_TABLE_SIZE; i++)
    {
        struct symdata *p = symhash[i];
        while (p)
        {
			if (p->values->type == BRS_STARTFUNC)
				count++;
			p = p->link;
		} 
	}
	list = calloc(count, sizeof(struct symdata *));
	if (!list)
		fatal("Out of memory");
	count = 0;
    for (i = 0; i < HASH_TABLE_SIZE; i++)
    {
        struct symdata *p = symhash[i];
        while (p)
        {
			if (p->values->type == BRS_STARTFUNC)
				list[count++] = p;
			p = p->link;
		} 
	}
	qsort( list, count, sizeof(struct symdata *), lineTableCmp);
	ofs = ofs + (filelistcount + 1) * sizeof(int);
	for (i=0; i < filelistcount; i++)
	{
		fseek (fil, base + i * sizeof(int), SEEK_SET); // write the file position
		fwrite(&ofs, 1, 4, fil);
		fseek(fil, ofs, SEEK_SET);
		while (n < count && list[n]->values->filenum == i)
		{
			char v;
			fwrite(&list[n]->values->startline, 1, 4, fil);
			fwrite(&list[n]->values->funcendline, 1, 4, fil);
			v = strlen(list[n]->name);
			fputc(v, fil);
			fwrite(list[n]->name, 1, v, fil);
			ofs += 8 + v + 1;
			n++;
		}
	}
	fseek (fil, base + i * sizeof(int), SEEK_SET); // write the end of records position
	fwrite(&ofs, 1, 4, fil);
	free(list);
	fseek (fil, ofs, SEEK_SET); // write the end of records position
	return ofs;
}
void GenerateOutput(char *name)
{
    FILE *fil;
    char buf[32];
    int ofs = 32, i;
    if (!symhash)
        return ;
    fil = fopen(name, "wb");
    memset(buf, 0, 32);
    if (!fil)
        fatal("could not open output file %s", name);
    fprintf(fil, "$BRW");
	*(int *)(buf + 4) = BROWSE_VERSION;
    fwrite(buf, 28, 1, fil);
    fputc(filelistcount, fil);
    fputc(filelistcount >> 8, fil);
    ofs += 2;
    for (i = 0; i < filelistcount; i++)
    {
        fputc(strlen(filelist[i]), fil);
        fprintf(fil, "%s", filelist[i]);
        ofs += strlen(filelist[i]) + 1;
    }
    ofs = pad(fil, ofs, 16);
    for (i = 0; i < HASH_TABLE_SIZE; i++)
    {
        struct symdata *s = symhash[i];
        while (s)
        {
            struct linedata *l = s->values;
            s->fileoffs = ofs;
            while (l)
            {
                if (!l->external || !s->globalcount)
                    ofs += putlinedata(fil, l);
                l = l->link;
            } fputc(0, fil);
            fputc(0, fil);
            ofs += 2;
            ofs = pad(fil, ofs, 2);
            s = s->link;
        }
    }
	ofs = pad(fil, ofs, 4);
    *(int*)buf = ofs;
    fseek(fil, 12, SEEK_SET);
    fwrite(buf, 4, 1, fil);
	ofs = WriteLineTable(fil, ofs);

	// must come last because the dictionary size is calculated based on EOF...
    ofs = pad(fil, ofs, 512);
    *(int*)buf = ofs;
    fseek(fil, 4, SEEK_SET);
    fwrite(buf, 4, 1, fil);
    fseek(fil, ofs, SEEK_SET);
    CalculateDictionary();
    WriteDictionary(fil, ofs);
	
    fclose(fil);
}
