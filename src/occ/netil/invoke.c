/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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


LIST *objlist,  *reslist,  *rclist;
static char outputFileName[260];

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
        strcat(outputFileName, prm_targettype == DLL ? ".dll" : ".exe");
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
    
    if (HasExt(name, ".rc"))
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
    else if (HasExt(name, ".ilo"))
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
    InsertFile(&objlist, buf, ".ilo");
    
    return 0; /* compiler should process it*/
}

/*-------------------------------------------------------------------------*/

void InsertOutputFileName(char *name)
{
    strcpy(outputFileName, name);
}

/*-------------------------------------------------------------------------*/
static LIST *objPosition;
void GetOutputFileName(char *name, char *path, BOOLEAN obj)
{
    if (obj)
    {
        if (!objPosition)
            objPosition = objlist;
        if (!objPosition)
            fatal("Cannot get object file name");
        strcpy(name, objPosition->data);
        strcpy(path, objPosition->data);
    }
    else
    {
        path[0] = 0;
        strcpy(name, outputFileName);
        if (objlist && name[0] && name[strlen(name) - 1] == '\\')
        {
            strcat(name, objlist->data);
            StripExt(name);
            strcat(name, ".exe");
            strcpy(path, outputFileName);
        }
    }
}
void NextOutputFileName()
{
    if (objPosition)
        objPosition = objPosition->next;
}
int RunExternalFiles(char *rootPath)
{
    char root[260];
    char args[1024], *c0;
    char spname[2048];
    char outName[260] ,*p;
    int rv;
    char temp[260];
    return;
    strcpy(root, rootPath);
    p = strrchr(root, '\\');
    if (!p)
        p = root;
    else
        p++;
    *p = 0;
    GetOutputFileName(outName, temp, FALSE);
    StripExt(outName);
    AddExt(outName, ".il");
//    if (beGetIncludePath)
//        sprintf(args, "\"-i%s\"", beGetIncludePath);
//    else
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
        char resources[10000];
        if (!fil)
            return 1;
        resources[0] = 0;
        
        if (cparams.prm_debug)
        {
            strcat(args, " /DEBUG");
        }
        if (prm_targettype == DLL)
        {
            strcat(args, " /DLL");
        }
        while (reslist)
        {
            sprintf(resources + strlen(resources), " /Resource:\"%s\"", reslist->data);
            reslist = reslist->next;
        }
        //while (objlist)
        {
            char filname[260];
            sprintf(filname, "\"%s%s\"", temp, outName);
            sprintf(spname, "ilasm.exe /QUIET %s %s %s", args, filname, resources);
            rv = system(spname);
            unlink(TEMPFILE);

            if (rv)
            {
                printf("assembly failed");
                return rv;
            }
            objlist = objlist->next;
        }
    }
    return 0;
}
