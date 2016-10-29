/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <stdio.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include "be.h"
#include "winmode.h"
 
#define TEMPFILE "$$$OCC.TMP"

extern COMPILER_PARAMS cparams;
extern int prm_targettype;
extern int prm_crtdll;
extern int prm_lscrtdll;
extern int prm_msvcrt;
extern int showBanner;

char *winflags[] = 
{
    "/T:CON32 ", "/T:GUI32 ", "/T:DLL32 ", 
    "/T:PM ", "/T:DOS32 ", "/T:BIN ", "/T:CON32;sdpmist32.bin ",
    "/T:CON32;shdld32.bin ",
};
char *winc0[] = 
{
    "c0xpe.o", "c0pe.o", "c0dpe.o", "c0pm.o", "c0wat.o", "", "c0xpe.o", "c0hx.o",
    "c0xls.o", "c0ls.o", "c0dls.o", "c0om.o", "c0wat.o", "", "c0xpe.o", "c0hx.o"
};


LIST *objlist,  *asmlist,  *liblist,  *reslist,  *rclist;
static char outputFileName[256];

#ifdef MICROSOFT
#define system(x) winsystem(x)
extern int winsystem(const char *);
#endif
static void InsertFile(LIST **r, char *name, char *ext)
{

    char buf[256],  *newbuffer;
    LIST *lst;
    strcpy(buf, name);
    if (!outputFileName[0])
    {
        strcpy(outputFileName, name);
        StripExt(outputFileName);
        strcat(outputFileName, ".exe");
    }
    if (ext)
    {
        StripExt(buf);
        strcat(buf, ext);
    }
    lst = *r;
    while (lst)
    {
        if (!strcasecmp(lst->data, buf))
            return;
        lst = lst->next;
    }
    newbuffer = (char*)malloc(strlen(buf) + 1);
    if (!newbuffer)
        return ;
    strcpy(newbuffer, buf);

    /* Insert file */
    while (*r)
        r = &(*r)->next;
    *r = malloc(sizeof(LIST));
    if (!r)
        return ;
    (*r)->next = 0;
    (*r)->data = newbuffer;
}

/*-------------------------------------------------------------------------*/

int InsertExternalFile(char *name)
{
    char buf[260], *p;
    
    if (HasExt(name, ".asm") || HasExt(name,".nas"))
    {
        InsertFile(&objlist, name, ".o");
        InsertFile(&asmlist, name, 0);
        return 1; /* compiler shouldn't process it*/
    }
    else if (HasExt(name, ".l"))
    {
        InsertFile(&liblist, name, 0);
        return 1;
    }
    else if (HasExt(name, ".rc"))
    {
        InsertFile(&reslist, name, ".res");
        InsertFile(&rclist, name, 0);
        return 1;
    }
    else if (HasExt(name, ".res"))
    {
        InsertFile(&reslist, name, 0);
        return 1;
    }
    else if (HasExt(name, ".o"))
    {
        InsertFile(&objlist, name, 0);
        return 1;
    }
    p = strrchr(name, '\\');
    if (!p)
        p = name;
    else
        p++;
    strcpy(buf, p);
    InsertFile(&objlist, buf, ".o");
    
    // compiling via assembly
    if (cparams.prm_asmfile && !cparams.prm_compileonly)
    {
        InsertFile(&asmlist, buf, ".asm");
    }
    return 0; /* compiler should process it*/
}

/*-------------------------------------------------------------------------*/

void InsertOutputFileName(char *name)
{
    strcpy(outputFileName, name);
}

/*-------------------------------------------------------------------------*/

int RunExternalFiles(char *rootPath)
{
    char root[260];
    char args[1024], *c0;
    char spname[2048];
    char outName[260] ,*p;
    int rv;
    char temp[260];
    strcpy(root, rootPath);
    p = strrchr(root, '\\');
    if (!p)
        p = root;
    else
        p++;
    *p = 0;
    temp[0] = 0;
    strcpy(outName, outputFileName);
    if (objlist && outName[0] && outName[strlen(outName)-1] == '\\')
    {
        strcat(outName, objlist->data);
        StripExt(outName);
        strcat(outName, ".exe");
        strcpy(temp, outputFileName);
    }
//    p = strrchr(outName, '.');
//    if (p && p[1] != '\\')
//        *p = 0;
    while (asmlist)
    {
        sprintf(spname, "\"%soasm.exe\" %s \"%s\"", root, !showBanner ? "-!" : "", asmlist->data);
        rv = system(spname);
        if (rv)
            return rv;
        asmlist = asmlist->next;
    }
    if (beGetIncludePath)
        sprintf(args, "\"-i%s\"", beGetIncludePath);
    else
        args[0] = 0;
    while (rclist)
    {
        sprintf(spname, "\"%sorc.exe\" -r %s %s \"%s\"", root, !showBanner ? "-!" : "", args, rclist->data);
        rv = system(spname);
        if (rv)
            return rv;
        rclist = rclist->next;
    }
    if (objlist)
    {
        FILE *fil = fopen(TEMPFILE, "w");
        if (!fil)
            return 1;
        
        strcpy(args, winflags[prm_targettype]);
            
        c0 = winc0[prm_targettype + prm_lscrtdll * 8];
        if (cparams.prm_debug)
        {
//            strcat(args, " /DEB");
            if (prm_targettype == DOS)
                c0 = "c0pmd.o";
            else if (prm_targettype == DOS32A)
                c0 = "c0watd.o";
            strcat(args, " /v");
        }
        fprintf(fil, "  %s", c0);
        while (objlist)
        {
            fprintf(fil, " \"%s%s\"", temp, objlist->data);
            objlist = objlist->next;
        }
        fprintf(fil, "  \"/o%s\" ", outName);
        while (liblist)
        {
            fprintf(fil, " \"%s\"", liblist->data);
            liblist = liblist->next;
        }
        if (prm_msvcrt)
            fprintf(fil," climp.l msvcrt.l ");
        else if (prm_lscrtdll)
            fprintf(fil," climp.l lscrtl.l ");
        else if (prm_crtdll)
            fprintf(fil, " climp.l crtdll.l ");
        else if (prm_targettype == DOS || prm_targettype == DOS32A || prm_targettype == RAW || prm_targettype == WHXDOS)
        {
            if (cparams.prm_farkeyword)
                fprintf(fil, " farmem");
            fprintf(fil, " cldos.l ");
        }
        else
            fprintf(fil, " climp.l clwin.l ");
        while (reslist)
        {
            fprintf(fil, " \"%s\"", reslist->data);
            reslist = reslist->next;
        }
        fclose(fil);
        sprintf(spname, "\"%solink.exe\" %s /mx /c+ %s @"TEMPFILE, root, !showBanner ? "-!" : "", args);
        rv = system(spname);
        unlink(TEMPFILE);

        if (rv)
            return rv;
       if (prm_targettype == WHXDOS)
       {
               sprintf(spname, "\"%spatchpe\" %s", root, outputFileName);
            rv = system(spname);
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
