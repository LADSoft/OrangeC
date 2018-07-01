/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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

extern COMPILER_PARAMS cparams;
extern int prm_targettype;
extern int prm_crtdll;
extern int prm_lscrtdll;
extern int prm_msvcrt;
extern int showBanner;
extern int verbosity;
extern char* prm_libpath;

char* winflags[] = {
    "/T:CON32 ", "/T:GUI32 ", "/T:DLL32 ", "/T:PM ", "/T:DOS32 ", "/T:BIN ", "/T:CON32;sdpmist32.bin ", "/T:CON32;shdld32.bin ",
};
char* winc0[] = {"c0xpe.o", "c0pe.o", "c0dpe.o", "c0pm.o", "c0wat.o", "", "c0xpe.o", "c0hx.o",
                 "c0xls.o", "c0ls.o", "c0dls.o", "c0om.o", "c0wat.o", "", "c0xpe.o", "c0hx.o"};

LIST *objlist, *asmlist, *liblist, *reslist, *rclist;
static char outputFileName[256];
static char *asm_params, *rc_params, *link_params;
#ifdef MICROSOFT
#    define system(x) winsystem(x)
extern int winsystem(const char*);
#endif
static BOOLEAN InsertOption(char* name)
{
    char** p;
    switch (name[0])
    {
        case 'a':
            p = &asm_params;
            break;
        case 'r':
            p = &rc_params;
            break;
        case 'l':
            p = &link_params;
            break;
        default:
            return FALSE;
    }
    int len = 0;
    if (*p)
        len = strlen(*p);
    *p = realloc(*p, 2 + len + strlen(name + 1));
    (*p)[len] = 0;
    strcat(*p, " ");
    strcat(*p, name + 1);
    return TRUE;
}
static void InsertFile(LIST** r, char* name, char* ext, BOOLEAN primary)
{
    LIST* lst;
    char buf[256], *newbuffer;
    strcpy(buf, name);
    if (primary && !outputFileName[0])
    {
        strcpy(outputFileName, name);
        StripExt(outputFileName);
        if (!cparams.prm_compileonly && !cparams.prm_assemble)
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
        if (!strcasecmp_internal(lst->data, buf))
            return;
        lst = lst->next;
    }
    newbuffer = (char*)malloc(strlen(buf) + 1);
    if (!newbuffer)
        return;
    strcpy(newbuffer, buf);

    /* Insert file */
    while (*r)
        r = &(*r)->next;
    *r = malloc(sizeof(LIST));
    if (!r)
        return;
    (*r)->next = 0;
    (*r)->data = newbuffer;
}

/*-------------------------------------------------------------------------*/

int InsertExternalFile(char* name, BOOLEAN primary)
{
    char buf[260], *p;
    if (name[0] == '$')
    {
        if (!InsertOption(name + 1))
            fatal("invalid parameter: /p%s", name + 1);
        return TRUE;
    }
    if (HasExt(name, ".asm") || HasExt(name, ".nas") || HasExt(name, ".s"))
    {
        InsertFile(&objlist, name, ".o", primary);
        InsertFile(&asmlist, name, 0, primary);
        return 1; /* compiler shouldn't process it*/
    }
    else if (HasExt(name, ".l") || HasExt(name, ".a") || HasExt(name, ".lib"))
    {
        InsertFile(&liblist, name, 0, primary);
        return 1;
    }
    else if (HasExt(name, ".rc"))
    {
        InsertFile(&reslist, name, ".res", primary);
        InsertFile(&rclist, name, 0, primary);
        return 1;
    }
    else if (HasExt(name, ".res"))
    {
        InsertFile(&reslist, name, 0, primary);
        return 1;
    }
    else if (HasExt(name, ".o"))
    {
        InsertFile(&objlist, name, 0, primary);
        return 1;
    }
    p = strrchr(name, '\\');
    if (!p)
        p = strrchr(name, '/');
    if (!p)
        p = name;
    else
        p++;
    strcpy(buf, p);
    InsertFile(&objlist, buf, ".o", primary);

    // compiling via assembly
    if (cparams.prm_asmfile && !cparams.prm_compileonly)
    {
        InsertFile(&asmlist, buf, ".asm", primary);
    }
    return 0; /* compiler should process it*/
}

/*-------------------------------------------------------------------------*/

void InsertOutputFileName(char* name) { strcpy(outputFileName, name); }

/*-------------------------------------------------------------------------*/

int RunExternalFiles(char* rootPath)
{
    char root[260];
    char args[1024], *c0;
    char spname[2048];
    char outName[260], *p;
    int rv;
    char temp[260];
    int i;
    char verbosityString[20];

    memset(verbosityString, 0, sizeof(verbosityString));
    if (verbosity > 1)
    {
        verbosityString[0] = '-';
        memset(verbosityString + 1, 'y', verbosity > sizeof(verbosityString) - 2 ? sizeof(verbosityString) - 2 : verbosity);
    }
    strcpy(root, rootPath);
    p = strrchr(root, '\\');
    if (!p)
        p = strrchr(root, '/');
    if (!p)
        p = root;
    else
        p++;
    *p = 0;
    temp[0] = 0;
    strcpy(outName, outputFileName);
    if (objlist && outName[0] && outName[strlen(outName) - 1] == '\\')
    {
        strcat(outName, objlist->data);
        StripExt(outName);
        strcat(outName, ".exe");
        strcpy(temp, outputFileName);
    }
    //    p = strrchr(outName, '.');
    //    if (p && p[1] != '\\')
    //        *p = 0;
    BOOLEAN first = FALSE;
    while (asmlist)
    {
        if (cparams.prm_compileonly && outputFileName[0] && !first)
            sprintf(spname, "\"%soasm.exe\" \"-o%s\" %s %s \"%s\"", root, outputFileName, asm_params ? asm_params : "",
                    !showBanner ? "-!" : "", asmlist->data);
        else
            sprintf(spname, "\"%soasm.exe\" %s %s \"%s\"", root, asm_params ? asm_params : "", !showBanner ? "-!" : "",
                    asmlist->data);
        first = TRUE;
        if (verbosity)
            printf("%s\n", spname);
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
        if (cparams.prm_compileonly && outputFileName[0] && !first)
            sprintf(spname, "\"%sorc.exe\" \"-o%s\" -r %s %s %s \"%s\"", root, outputFileName, rc_params ? rc_params : "",
                    !showBanner ? "-!" : "", args, rclist->data);
        else
            sprintf(spname, "\"%sorc.exe\" -r %s %s %s \"%s\"", root, rc_params ? rc_params : "", !showBanner ? "-!" : "", args,
                    rclist->data);
        first = TRUE;
        if (verbosity)
            printf("%s\n", spname);
        rv = system(spname);
        if (rv)
            return rv;
        rclist = rclist->next;
    }
    if (!cparams.prm_compileonly && objlist)
    {
        char tempFile[260];
        tmpnam(tempFile);
        FILE* fil = fopen(tempFile, "w");
        if (!fil)
            return 1;
        if (prm_libpath)
        {
            fprintf(fil, "\"/L%s\" ", prm_libpath);
        }

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
            fprintf(fil, " climp.l msvcrt.l ");
        else if (prm_lscrtdll)
            fprintf(fil, " climp.l lscrtl.l ");
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
        sprintf(spname, "\"%solink.exe\" %s %s /c+ %s %s @%s", root, link_params ? link_params : "", !showBanner ? "-!" : "", args,
                verbosityString, tempFile);
        if (verbosity)
        {
            FILE* fil = fopen(tempFile, "r");
            printf("%s\n", spname);
            if (fil)
            {
                char buffer[8192];
                int len;
                printf("with %s=\n", tempFile);
                while ((len = fread(buffer, 1, 8192, fil)) > 0)
                    fwrite(buffer, 1, len, stdout);
                fclose(fil);
            }
        }
        rv = system(spname);
        unlink(tempFile);

        if (rv)
            return rv;
        if (prm_targettype == WHXDOS)
        {
            sprintf(spname, "\"%spatchpe\" %s", root, outputFileName);
            if (verbosity)
                printf("%s\n", spname);
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
