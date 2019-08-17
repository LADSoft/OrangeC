/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 * 
 *     The Orange C Compiler package is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <stdio.h>
#include <process.h>
#include <string.h>
#include <stdlib.h>
#include "be.h"
#include "winmode.h"
#include "Utils.h"
 
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
        if (Utils::iequal((char *)lst->data, buf))
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
    *r = (LIST *)malloc(sizeof(LIST));
    if (!r)
        return ;
    (*r)->next = 0;
    (*r)->data = newbuffer;
}

/*-------------------------------------------------------------------------*/

int InsertExternalFile(char *name, bool)
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
void GetOutputFileName(char *name, char *path, bool obj)
{
    if (obj)
    {
        char *p;
        if (!objPosition)
            objPosition = objlist;
        if (!objPosition)
            fatal("Cannot get object file name");
        strcpy(name, outputFileName);
        p = strrchr(name, '\\');
        if (!p)
            p = name;
        else
            p++;
        strcpy(p, (char *)objPosition->data);
        strcpy(path, name);
    }
    else
    {
        path[0] = 0;
        strcpy(name, outputFileName);
        if (objlist && name[0] && name[strlen(name) - 1] == '\\')
        {
            strcat(name, (char *)objlist->data);
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
    return 0;
    strcpy(root, rootPath);
    p = strrchr(root, '\\');
    if (!p)
        p = root;
    else
        p++;
    *p = 0;
    GetOutputFileName(outName, temp, false);
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
+            unlink(TEMPFILE);

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
