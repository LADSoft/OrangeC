/* 
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

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

This program is derived from the cc68k complier by 
Matthew Brandt (mailto::mattb@walkingdog.net) 

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
#include <stdio.h>
#include <process.h>
#include "utype.h"
#include "lists.h"
#include "cmdline.h"
#include "winmode.h"
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <stdlib.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#define TEMPFILE "$$$CC386.TMP"

extern int prm_farkeyword;
extern int prm_targettype;
extern int prm_debug;
extern int prm_crtdll;
extern int prm_lscrtdll;
extern int prm_msvcrt;
extern char *prm_searchpath;
extern int prm_asmfile;
extern int prm_compileonly;

char *winflags[] = 
{
    "/NCI /32 /PE /CON ", "/NCI /32 /PE /WIN ", "/NCI /32 /PE /BDL ", 
    "/NCI /32 ", "/NCI /LE", "/NCI /32 /SYS", "/NCI /32 /PE /CON /STB:(dpmist32.bin)",
	"/NCI /32 /PE /STB:(HDLD32.BIN)",
};
char *winc0[] = 
{
    "c0Xwin.obj", "c0win.obj", "c0dwin.obj", "c0dos.obj", "c0dosw.obj", "", "c0xwin.obj", "c0doswhx.obj",
    "c0Xdll.obj", "c0dll.obj", "c0ddll.obj", "c0dos.obj", "c0dosw.obj", "", "c0xwin.obj", "c0doswhx.obj"
};

LIST *objlist,  *asmlist,  *liblist,  *reslist,  *rclist;
static char outputFileName[256];

static void InsertFile(LIST **r, char *name, char *ext)
{

    char buf[256],  *newbuffer;
    strcpy(buf, name);
    if (!outputFileName[0])
    {
        strcpy(outputFileName, name);
        StripExt(outputFileName);
        AddExt(outputFileName, ".EXE");
    }
    if (ext)
    {
        StripExt(buf);
        AddExt(buf, ext);
    }
    newbuffer = (char*)malloc(strlen(buf) + 1);
    if (!newbuffer)
        return ;
    strcpy(newbuffer, buf);

    /* Insert file */
    while (*r)
        r = &(*r)->link;
    *r = malloc(sizeof(LIST));
    if (!r)
        return ;
    (*r)->link = 0;
    (*r)->data = newbuffer;
}

//-------------------------------------------------------------------------

int InsertExternalFile(char *name)
{
    if (HasExt(name, ".ASM") || HasExt(name,".NAS"))
    {
        InsertFile(&objlist, name, ".OBJ");
        InsertFile(&asmlist, name, 0);
        return 1; // compiler shouldn't process it
    }
    else if (HasExt(name, ".LIB"))
    {
        InsertFile(&liblist, name, 0);
        return 1;
    }
    else if (HasExt(name, ".RC"))
    {
        InsertFile(&reslist, name, ".RES");
        InsertFile(&rclist, name, 0);
        return 1;
    }
    else if (HasExt(name, ".RES"))
    {
        InsertFile(&reslist, name, 0);
        return 1;
    }
    else if (HasExt(name, ".obj"))
    {
        InsertFile(&objlist, name, 0);
        return 1;
    }

    InsertFile(&objlist, name, ".OBJ");
	if (prm_asmfile && !prm_compileonly)
	{
    	InsertFile(&asmlist, name, ".ASM");
	}
    return 0; // compiler should process it
}

//-------------------------------------------------------------------------

void InsertOutputFile(char *name)
{
    strcpy(outputFileName, name);
}

//-------------------------------------------------------------------------

int RunExternalFiles(void)
{
    char args[1024];
    int rv;
    while (asmlist)
    {
        rv = spawnlp(P_WAIT, "nasm.exe", "nasm", "-f", "obj", asmlist->data, 0);
        if (rv)
            return rv;
        asmlist = asmlist->link;
    }
    if (prm_searchpath)
        sprintf(args, "-i%s", prm_searchpath);
    else
        args[0] = 0;
    while (rclist)
    {
        rv = spawnlp(P_WAIT, "xrc.exe", "xrc", "-r", args, rclist->data, 0);
        if (rv)
            return rv;
        rclist = rclist->link;
    }
	
    if (objlist)
    {
		char *c0;
        FILE *fil = fopen(TEMPFILE, "w");
        if (!fil)
		{
			printf("can't open temporary file");
            return 1;
		}
        strcpy(args, winflags[prm_targettype]);
        if (prm_debug)
            strcat(args, " /DEB");
		c0 = winc0[prm_targettype + prm_lscrtdll * 8];
		if (prm_debug)
		{
			if (prm_targettype == DOS)
				c0 = "c0dosd.obj";
			else if (prm_targettype == DOS32A)
				c0 = "c0doswd.obj";
		}
        fprintf(fil, "  %s", c0);
        while (objlist)
        {
            fprintf(fil, " %s", objlist->data);
            objlist = objlist->link;
        }
        fprintf(fil, "\n");
        fprintf(fil, "  %s\n  \n", outputFileName); // no libs
        while (liblist)
        {
            fprintf(fil, " %s", liblist->data);
            liblist = liblist->link;
        }
        if (prm_lscrtdll)
            fprintf(fil," climp lscrtl\n");
        else if (prm_crtdll)
            fprintf(fil, " climp crtdll\n");
		else if (prm_msvcrt)
		{
			fprintf(fil, " climp msvcrt\n");
		}
        else if (prm_targettype == DOS || prm_targettype == DOS32A || prm_targettype == RAW || prm_targettype == WHXDOS)
        {
            if (prm_farkeyword)
                fprintf(fil, " farmem");
            fprintf(fil, " cldos\n");
        }
        else
            fprintf(fil, " climp clwin\n");
        while (reslist)
        {
            fprintf(fil, " %s", reslist->data);
            reslist = reslist->link;
        }
        fprintf(fil, "\n  \n");
        fclose(fil);
        rv = spawnlp(P_WAIT, "valx.exe", "valx.exe", args, "@" TEMPFILE, 0);
        //      rv = spawnlp(P_WAIT,"mem.exe","mem.exe",0) ;

       unlink(TEMPFILE);
	   if (rv)
		   return rv;
	   if (prm_targettype == WHXDOS)
	   {
        	rv = spawnlp(P_WAIT, "patchpe.exe", "patchpe.exe", outputFileName, 0);
			if (rv)
			{
				printf("Could not spawn patchpe.exe\n");
			}
	   }
        if (rv)
            return rv;

    }
    return 0;
}
