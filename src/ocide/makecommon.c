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

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <richedit.h>
#include <stdio.h>
#include <ctype.h>

#include "header.h"

#define TEMPFILE "$$$CC386.TMP"

extern HWND hwndFrame;
extern char szInstallPath[];
extern char szWorkAreaName[];
extern int making;
extern int profileDebugMode;
extern char currentProfileName[256];

int errcount, warncount;
char makeTempFile[MAX_PATH];

static HashTable* tables;

void ExpandSpecial(SETTING* setting, PROJECTITEM* pj, char* buf, char* src)
{
    if (setting)
    {
        if (pj && setting->ext)
        {
            char* p = strrchr(pj->realName, '.');
            if (!p || !MatchesExt(p, setting->ext))
            {
                buf[0] = 0;
                return;
            }
        }
        switch (setting->type)
        {
            case e_prependtext:
                while (src[0])
                {
                    if (isspace(*src))
                    {
                        *buf++ = ' ';
                        while (isspace(*src))
                            src++;
                    }
                    *buf++ = '"';
                    strcpy(buf, setting->aux);
                    buf += strlen(buf);
                    while (*src && !isspace(*src))
                    {
                        if (*src == '"')
                        {
                            src++;
                            while (*src && *src != '"')
                                *buf++ = *src++;
                            if (*src)
                                src++;
                        }
                        else
                        {
                            *buf++ = *src++;
                        }
                    }
                    *buf++ = '"';
                }
                *buf = 0;
                break;
            case e_separatedtext:
                while (isspace(*src))
                    src++;
                while (src[0])

                {
                    strcpy(buf, setting->aux);
                    buf += strlen(buf);
                    while (*src && !isspace(*src))
                    {
                        if (*src == '"')
                        {
                            src++;
                            while (*src && *src != '"')
                                *buf++ = *src++;
                            if (*src)
                                src++;
                        }
                        else
                        {
                            *buf++ = *src++;
                        }
                    }
                    while (isspace(*src))
                        src++;
                }
                *buf = 0;
                break;
            default:
                strcpy(buf, src);
                break;
        }
    }
    else
    {
        strcpy(buf, src);
    }
}
static void ExpandMacro(PROJECTITEM* proj, char** dest, char** src, PROJECTITEM** lcd)
{
    char* p = strchr(*src, ')');
    if ((*dest)[-1] == '$')  // get past the make instruction
    {
        (*dest)--;
        if (lcd)
        {
            memcpy(*dest, *src, p + 1 - *src);
            *dest += p + 1 - *src;
            *src = p + 1;
            return;
        }
    }
    (*src) += 2;
    if (p)
    {
        char buf[256];
        memcpy(buf, *src, p - *src);
        buf[p - *src] = 0;
        (*src) = p + 1;
        p = Lookup(buf, proj, lcd);
        if (p)
        {
            strcpy(*dest, p);
            *dest += strlen(p);
            free(p);
        }
    }
}
static void ExpandFile(PROJECTITEM* proj, char** dest, char** src, PROJECTITEM** lcd)
{
    char delim, *p, dest1[10000];
    FILE* fil = fopen(makeTempFile, "w");
    *src += 2;
    delim = *(*src)++;
    p = strchr((*src), delim);
    if (p)
    {
        *p = 0;
    }
    EvalMacros(proj, dest1, *src, lcd);
    if (p)
    {
        *p = delim;
        *src = p + 1;
    }
    else
    {
        *src += strlen(*src);
    }
    p = dest1;
    if (*p == '\n')
        p++;
    if (fil)
    {
        strcpy(*dest, makeTempFile);
        *dest += strlen(*dest);
        fputs(p, fil);
        fclose(fil);
        //        CopyFile("C:\orangec\src\ocide\temp.tmp",makeTempFile, FALSE);
    }
}
static void PutFilePath(PROJECTITEM* rel, char** dest, char* name, int escaped, int spacing)
{
    if (spacing)
    {
        *(*dest)++ = ' ';
    }
    if (escaped)
        *(*dest)++ = '"';
    strcpy(*dest, name);
    if (rel)
    {
        while (rel && rel->type != PJ_PROJ)
            rel = rel->parent;
        if (rel)
            strcpy(*dest, relpathmake(name, rel->realName));
        else
            strcpy(*dest, name);
    }
    else
    {
        strcpy(*dest, name);
    }
    (*dest) += strlen(*dest);
    if (escaped)
        *(*dest)++ = '"';
}
static int GetOneFilenamePath(PROJECTITEM* pj, char** dest, char* ext, int len, BOOL first, int escaped, int spacing,
                              PROJECTITEM** lcd)
{
    if (pj->type != PJ_PROJ && strlen(pj->realName) > len && !stricmp(pj->realName + strlen(pj->realName) - len, ext))
    {
        PutFilePath(lcd ? pj : NULL, dest, pj->realName, escaped, spacing);
        spacing = TRUE;
    }
    if (pj->outputExt[0] && !stricmp(pj->outputExt, ext))
    {
        PutFilePath(lcd ? pj : NULL, dest, pj->outputName, escaped, spacing);
        spacing = TRUE;
    }
    return spacing;
}
int GetFilenamePaths(PROJECTITEM* pj, char** dest, char* ext, int len, BOOL first, int escaped, int spacing, PROJECTITEM** lcd)
{
    do
    {
        if (pj->children)
            spacing = GetFilenamePaths(pj->children, dest, ext, len, FALSE, escaped, spacing, lcd);
        if (pj->type == PJ_PROJ)
        {
            PROJECTITEMLIST* list = pj->depends;
            while (list)
            {
                spacing = GetOneFilenamePath(list->item, dest, ext, len, FALSE, escaped, spacing, lcd);
                list = list->next;
            }
        }
        spacing = GetOneFilenamePath(pj, dest, ext, len, FALSE, escaped, spacing, lcd);
        pj = pj->next;
    } while (pj && !first);
    return spacing;
}
static void ExpandFileName(PROJECTITEM* pj, char** dest, char** src, PROJECTITEM** lcd)
{
    char buf[256], *p = buf;
    BOOL escaped = FALSE;
    if ((*src)[-1] == '"')
    {
        escaped = TRUE;
    }
    (*src) += 2;
    *p++ = '.';
    while (isalnum(**src) || **src == '_')
        *p++ = *(*src)++;
    if (escaped && *(*src) == '"')
    {
        (*dest)--;
        (*src)++;
    }
    else
    {
        escaped = FALSE;
    }
    *p = 0;
    GetFilenamePaths(pj, dest, buf, strlen(buf), TRUE, escaped, FALSE, lcd);
}
void EvalMacros(PROJECTITEM* pj, char* parsedCmd, char* rawCmd, PROJECTITEM** lcd)
{
    char* dest = parsedCmd;
    BOOL done = FALSE;
    while (!done)
    {
        char* ma = strstr(rawCmd, "$(");
        char* se = strstr(rawCmd, "%.");
        char* fi = NULL;
        if (!lcd)
            fi = strstr(rawCmd, "&&");
        if (ma && se)
        {
            if (se < ma)
                ma = NULL;
            else
                se = NULL;
        }
        if (ma && fi)
        {
            if (fi < ma)
                ma = NULL;
            else
                fi = NULL;
        }
        if (se && fi)
        {
            if (se < fi)
                fi = NULL;
            else
                se = NULL;
        }
        if (ma || se || fi)
        {
            char* p = (ma ? ma : se ? se : fi);
            memcpy(dest, rawCmd, p - rawCmd);
            dest[p - rawCmd] = 0;
            dest += p - rawCmd;
            rawCmd = p;
            if (fi)
            {
                ExpandFile(pj, &dest, &rawCmd, lcd);
            }
            else if (ma)
            {
                ExpandMacro(pj, &dest, &rawCmd, lcd);
            }
            else  // se
            {
                if (pj)
                    ExpandFileName(pj, &dest, &rawCmd, lcd);
                else
                    *dest++ = *rawCmd++;
            }
        }
        else
        {
            strcpy(dest, rawCmd);
            done = TRUE;
        }
    }
    // pack spacing and get rid of CR/LF
    dest = parsedCmd;
    while (*parsedCmd)
    {
        if (*parsedCmd == '"' && parsedCmd[1] == '"')
        {
            *dest++ = ' ';
            parsedCmd += 2;
        }
        else if (isspace(*parsedCmd) && *parsedCmd != '\n')
        {
            *dest++ = ' ';
            while (isspace(*parsedCmd))
                parsedCmd++;
        }
        else
        {
            *dest++ = *parsedCmd++;
        }
    }
    *dest = 0;
}
unsigned MakeHash(char* name)
{
    unsigned rv = 0;
    while (*name)
    {
        rv += (rv << 7) + (rv << 1) + *name++;
    }
    return rv % MAKEHASH_MAX;
}
PROJECTITEM* LCD(PROJECTITEM* proj, PROJECTITEM* lcd)
{
    PROJECTITEM* test;
    if (!lcd)
        return proj;
    if (!proj)
        return lcd;
    if (proj == lcd)
        return proj;
    test = proj->parent;
    while (test)
    {
        if (test == lcd)
        {
            return proj;
        }
        test = test->parent;
    }
    test = lcd->parent;
    while (test)
    {
        if (test == proj)
        {
            return lcd;
        }
        test = test->parent;
    }
    return NULL;
}
char* Lookup(char* id, PROJECTITEM* proj, PROJECTITEM** lcd)
{
    HashTable* search = tables;
    int hash = MakeHash(id);
    while (search)
    {
        NameValuePair* p = search->pairs[hash];
        while (p)
        {
            if (!strcmp(p->name, id))
            {
                PROJECTITEM* lcd2 = NULL;
                SETTING *value, *setting = PropSearchProtos(proj, id, &value);
                char buf1[10000];
                char* buf = malloc(10000);
                if (!p->assign)
                    lcd2 = search->pj;
                if (lcd)
                {
                    *lcd = LCD(lcd2, *lcd);
                }
                ExpandSpecial(setting, proj, buf1, p->value);
                EvalMacros(proj, buf, buf1, lcd);
                return buf;
            }
            p = p->next;
        }
        search = search->next;
    }
    return NULL;
}
void AddMakeSymbol(char* name, char* value, BOOL dup, BOOL assign)
{
    int hash = MakeHash(name);
    NameValuePair* p = calloc(1, sizeof(NameValuePair) + 32);
    if (p)
    {
        p->next = tables->pairs[hash];
        p->assign = assign;
        tables->pairs[hash] = p;
        if (dup)
        {
            p->value = strdup(value);
            p->name = strdup(name);
        }
        else
        {
            p->value = value;
            p->name = name;
        }
        p->alloced = dup;
    }
}
void RecursiveAddSymbols(SETTING* set, BOOL assign)
{
    while (set)
    {
        if (set->type >= e_values)
        {
            AddMakeSymbol(set->id, set->value, TRUE, assign);
        }
        else if (set->type == e_tree)
        {
            RecursiveAddSymbols(set->children, assign);
        }
        set = set->next;
    }
}
void AddAssigns(SETTING* set)
{
    while (set)
    {
        if (set->type == e_choose)
        {
            char* val = Lookup(set->id, NULL, NULL);
            if (val)
            {
                if (!strcmp(val, set->value))
                {
                    AddAssigns(set->children);
                }
                free(val);
            }
        }
        else if (set->type == e_assign)
        {
            AddMakeSymbol(set->id, set->value, FALSE, TRUE);
        }
        set = set->next;
    }
}
void AddRuleSymbols(PROJECTITEM* fi)
{
    struct _propsData data;
    PROFILE* pages[100];
    int i;
    memset(&data, 0, sizeof(data));
    data.prototype = pages;
    GetActiveRules(fi, &data);
    for (i = 0; i < data.protocount; i++)
    {
        RecursiveAddSymbols(GetSettings(data.prototype[i]), TRUE);
    }
}
void AddRuleCommands(PROJECTITEM* fi)
{
    struct _propsData data;
    PROFILE* pages[100];
    int i;
    memset(&data, 0, sizeof(data));
    data.prototype = pages;
    SelectRules(fi, &data);
    for (i = 0; i < data.protocount; i++)
    {
        SETTING* set = GetSettings(data.prototype[i]);
        if (set->depends)
            EvalDependentRules(set->depends, fi, &data);
    }
    for (i = 0; i < data.protocount; i++)
    {
        SETTING* set = GetSettings(data.prototype[i]);
        if (set->command)
        {
            AddAssigns(set->command->assignments);
            RecursiveAddSymbols(set->command->children, TRUE);
        }
    }
}
void CreateEnvironmentMacros(void)
{
    LPTCH x;
    LPTCH p = x = GetEnvironmentStrings();
    if (p)
    {
        while (*p)
        {
            char* name = p;
            char* value = strchr(p + 1, '=');
            if (*value)
            {
                *value = 0;
                AddMakeSymbol(name, value + 1, TRUE, FALSE);
                *value = '=';
            }
            p += strlen(p) + 1;
        }
        FreeEnvironmentStrings(x);
    }
}
void CreateBuiltinWorkspaceMacros(PROJECTITEM* wa)
{
    char buf[MAX_PATH], *p;
    strcpy(buf, szInstallPath);
    strcat(buf, "\\");
    p = buf + strlen(buf);
    AddMakeSymbol("WORKAREADIR", szWorkAreaName, FALSE, FALSE);
    AddMakeSymbol("LSINSTALLDIR", szInstallPath, FALSE, FALSE);
    strcpy(p, "include");
    AddMakeSymbol("LSINSTALLINCLUDE", buf, TRUE, FALSE);
    strcpy(p, "bin");
    AddMakeSymbol("LSINSTALLBIN", buf, TRUE, FALSE);
    strcpy(p, "lib");
    AddMakeSymbol("LSINSTALLLIB", buf, TRUE, FALSE);
    AddMakeSymbol("CURRENTPROFILE", currentProfileName, TRUE, FALSE);
    AddMakeSymbol("CURRENTRELEASETYPE", profileDebugMode ? "Debug" : "Release", FALSE, FALSE);
    CreateEnvironmentMacros();
}
void CreateBuiltinProjectMacros(PROJECTITEM* pj, BOOL rel)
{
    char buf[MAX_PATH], *p, *q;
    strcpy(buf, pj->realName);
    p = strrchr(buf, '\\');
    if (p)
        *p = 0;
    if (rel)
    {
        p = relpathmake(buf, pj->realName);
        strcpy(buf, p);
    }
    AddMakeSymbol("PROJECTDIR", buf, TRUE, FALSE);
    AddMakeSymbol("OUTPUTEXT", pj->outputExt, TRUE, FALSE);
    strcpy(buf, pj->realName);
    p = strrchr(buf, '.');
    if (p)
        *p = 0;
    p = strrchr(buf, '\\');
    if (p)
        p++;
    else
        p = buf;
    AddMakeSymbol("OUTPUTNAME", p, TRUE, FALSE);
    p = Lookup("OUTPUTEXE", pj, NULL);
    if (p)
    {
        q = strrchr(p, '\\');
        if (q)
            *q = 0;
        AddMakeSymbol("OUTPUTPATH", p, TRUE, FALSE);
        free(p);
    }
}
void CreateBuiltinFileMacros(PROJECTITEM* pj)
{
    char *p, buf[MAX_PATH], *q;
    AddMakeSymbol("INPUTFILE", pj->realName, TRUE, FALSE);
    strcpy(buf, pj->realName);
    p = strrchr(buf, '.');
    if (p)
    {
        AddMakeSymbol("INPUTEXT", p, TRUE, FALSE);
        *p = 0;
    }
    p = strrchr(buf, '\\');
    if (p)
    {
        AddMakeSymbol("INPUTNAME", p + 1, TRUE, FALSE);
        *p = 0;
        AddMakeSymbol("INPUTPATH", buf, TRUE, FALSE);
    }
    if (pj->outputExt[0])
    {
        strcpy(buf, pj->realName);
        p = strrchr(buf, '.');
        if (p)
            *p = 0;
        AddMakeSymbol("OUTPUTEXT", pj->outputExt, TRUE, FALSE);
        *p = 0;
        p = strrchr(buf, '\\');
        if (p)
            p++;
        else
            p = buf;
        AddMakeSymbol("OUTPUTNAME", p, TRUE, FALSE);
        p = Lookup("OUTPUTFILE", pj, NULL);
        if (p)
        {
            q = strrchr(p, '\\');
            if (q)
                *q = 0;
            AddMakeSymbol("OUTPUTPATH", p, TRUE, FALSE);
            free(p);
        }
    }
}
BOOL CreateFullPath(char* path)
{
    char buf[MAX_PATH], *p;
    strcpy(buf, path);
    p = strrchr(buf, '\\');
    if (p)
    {
        *p = 0;
        CreateFullPath(buf);
    }
    else
    {
        return FALSE;
    }
    CreateDirectory(path, NULL);
    return FALSE;
}
static void CreateOutputPath(PROJECTITEM* fi)
{
    char* p = Lookup("OUTPUTDIR", fi, NULL);
    if (p && (!CreateDirectory(p, NULL) && GetLastError() != ERROR_ALREADY_EXISTS))
    {
        CreateFullPath(p);
        free(p);
    }
}
void AddSymbolTable(PROJECTITEM* fi, BOOL rel)
{
    HashTable* newTable = calloc(1, sizeof(HashTable));
    if (newTable)
    {
        SETTING* settings;
        newTable->next = tables;
        newTable->pj = fi;
        tables = newTable;
        if (fi->type == PJ_WS)
            AddRuleSymbols(fi);
        settings = GetSettings(fi->profiles);
        if (settings)
            RecursiveAddSymbols(settings, FALSE);
        AddRuleCommands(fi);
        switch (fi->type)
        {
            case PJ_WS:
                CreateBuiltinWorkspaceMacros(fi);
                break;
            case PJ_PROJ:
                CreateBuiltinProjectMacros(fi, rel);
                CreateOutputPath(fi);
                break;
            case PJ_FILE:
                CreateBuiltinFileMacros(fi);
                break;
        }
    }
}
void AddRootTables(PROJECTITEM* pj, BOOL rel)
{
    if (pj->parent)
        AddRootTables(pj->parent, rel);
    AddSymbolTable(pj, rel);
}
void RemoveSymbols(HashTable* table)
{
    int i;
    for (i = 0; i < MAKEHASH_MAX; i++)
    {
        NameValuePair* p = table->pairs[i];
        while (p)
        {
            NameValuePair* next = p->next;
            if (p->alloced)
            {
                free(p->name);
                free(p->value);
            }
            free(p);
            p = next;
        }
    }
}
void RemoveSymbolTable(void)
{
    if (tables)
    {
        HashTable* oldTable = tables;
        tables = tables->next;
        RemoveSymbols(oldTable);
        free(oldTable);
    }
}
void ReleaseSymbolTables(void)
{
    while (tables)
        RemoveSymbolTable();
}
void SetIsMaking(int makeRunning)
{
    making = makeRunning;
    PostMessage(hwndFrame, WM_REDRAWTOOLBAR, 0, 0);
}
void CreateTempFileName(void)
{
    GetTempPath(sizeof(makeTempFile), makeTempFile);
    if (makeTempFile[strlen(makeTempFile) - 1] != '\\')
        strcat(makeTempFile, "\\");
    strcat(makeTempFile, TEMPFILE);
}
/*
 * Pull the next path off the path search list
 */
static char* parsepath(char* path, char* buffer)
{
    char* pos = path;

    /* Quit if hit a ';' */
    while (*pos)
    {
        if (*pos == ';')
        {
            pos++;
            break;
        }
        *buffer++ = *pos++;
    }
    *buffer = 0;

    /* Return a null pointer if no more data */
    if (*pos)
        return (pos);

    return (0);
}

/*
 * For each library:
 * Search local directory and all directories in the search path
 *  until it is found or run out of directories
 */
FILE* FindOnPath(char* string, char* searchPath)
{
    FILE* in;
    char* newpath = searchPath;
    if (string[1] == ':' || string[0] == '\\')
        return fopen(string, "rb");
    if (string[0] != '\\' && string[1] != ':')
    {
        char buffer[MAX_PATH];
        while (newpath)
        {
            /* Create a file name along this path */
            newpath = parsepath(newpath, buffer);
            if (buffer[strlen(buffer) - 1] != '\\')
                strcat(buffer, "\\");
            strcat(buffer, (char*)string);

            /* Check this path */
            in = fopen(buffer, "rb");
            if (in)
            {
                strcpy(string, buffer);
                return (in);
            }
        }
    }
    return (0);
}
void LoadPath(char* root, char* dest, char* item)
{
    char* p = Lookup(item, NULL, NULL);
    char buf[MAX_PATH];
    if (p && *p)
    {
        strcpy(buf, p);
        abspath(buf, root);
        strcat(dest, ";");
        strcat(dest, buf);
    }
    free(p);
}
char* GetNextFile(char* out, char* in)
{
    char* p;
    while (isspace(*in))
        in++;
    if (*in == '"')
    {
        p = strchr(++in, '"');
        if (!p)
        {
            p = in + strlen(in);
        }
        memcpy(out, in, p - in);
        out[p - in] = 0;
        if (*p)
            p++;
    }
    else
    {
        p = in;
        while (*p && !isspace(*p))
            p++;
        memcpy(out, in, p - in);
        out[p - in] = 0;
    }
    return p;
}
void countErrors(char* buf)
{
    char* p = buf;
    if (!strnicmp(p, "ERROR", 5) || stristr(p, "ERROR:"))
        errcount++;
    if (!strnicmp(p, "WARNING", 7) || stristr(p, "WARNING:"))
        warncount++;
    if (!strnicmp(p, "LINKER ERROR", 12))
        errcount++;
    if (!strnicmp(p, "FATAL ERROR", 11))
        errcount++;
    while ((p = stristr(p, "\nERROR")))
    {
        errcount++;
        p += 6;
    }
    p = buf;
    while ((p = stristr(p, "\nLINKER ERROR")))
    {
        errcount++;
        p += 13;
    }
    p = buf;
    while ((p = stristr(p, "\nFATAL ERROR")))
    {
        errcount++;
        p += 13;
    }
    p = buf;
    while ((p = stristr(p, "\nWARNING")))
    {
        warncount++;
        p += 8;
    }
}
int FileTime(FILETIME* timex, char* name)
{
    HANDLE fd;

    memset(timex, 0, sizeof(*timex));
    fd = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (fd == INVALID_HANDLE_VALUE)
        return 0;
    if (!GetFileTime(fd, 0, 0, timex))
    {
        CloseHandle(fd);
        return 0;
    }
    CloseHandle(fd);
    return 1;
}

//-------------------------------------------------------------------------

int CompareTimes(FILETIME* target, FILETIME* source)
{
    // gotta have a two second delay because windows writes the files
    // in different orders... this is the max delay based on FAT
    __int64 result = *(__int64*)source - *(__int64*)target;
    return result > 2000000;  // true if source is newer than target
}
int ParsePipeData(HANDLE handle, int window, HANDLE hProcess)
{
    static char buf[513];
    static int pos = 0;
    char* p;
    int rv = TRUE;
    while (TRUE)
    {
        DWORD read = 0;
        if (pos < 512)
        {
            DWORD avail = 0;
            DWORD xx;
            avail = 0;
            while (!avail)
            {
                rv = PeekNamedPipe(handle, 0, 0, 0, &avail, 0);
                if (!rv || !avail)
                {
                    GetExitCodeProcess(hProcess, &xx);
                    if (xx != STILL_ACTIVE)
                    {
                        break;
                    }
                    Sleep(10);
                }
            }
            if (avail)
            {
                rv = ReadFile(handle, buf + pos, 512 - pos, &read, 0);
            }
        }
        pos += read;
        buf[pos] = 0;
        while ((p = strchr(buf, '\n')))
        {
            char s = *++p;
            int wc = warncount, ec = errcount;
            *p = 0;
            if (window != ERR_NO_WINDOW)
            {
                countErrors(buf);
                if (ec != errcount)
                    SetInfoColor(window, 0x0000ff);  // red
                else if (wc != warncount)
                    SetInfoColor(window, 0xff0000);  // blue
                SendInfoMessage(window, buf);
                if (ec != errcount)
                    SetInfoColor(ERR_EXTENDED_BUILD_WINDOW, 0x0000ff);  // red
                else if (wc != warncount)
                    SetInfoColor(ERR_EXTENDED_BUILD_WINDOW, 0xff0000);  // blue
                SendInfoMessage(ERR_EXTENDED_BUILD_WINDOW, buf);
            }
            *p = s;
            strcpy(buf, p);
        }
        pos = strlen(buf);
        if (pos == 512 || (!rv && pos))
        {
            int wc = warncount, ec = errcount;
            buf[pos] = 0;
            if (window != ERR_NO_WINDOW)
            {
                countErrors(buf);
                if (ec != errcount)
                    SetInfoColor(window, 0x0000ff);  // red
                else if (wc != warncount)
                    SetInfoColor(window, 0xff0000);  // blue
                SendInfoMessage(window, buf);
                if (ec != errcount)
                    SetInfoColor(ERR_EXTENDED_BUILD_WINDOW, 0x0000ff);  // red
                else if (wc != warncount)
                    SetInfoColor(ERR_EXTENDED_BUILD_WINDOW, 0xff0000);  // blue
                SendInfoMessage(ERR_EXTENDED_BUILD_WINDOW, buf);
            }
            pos = 0;
        }
        if (!read || !rv)
            break;
    }
    return rv;
}

//-------------------------------------------------------------------------

int Execute(char* cmd, char* wdp, int window)
{
    char path[260], *p;
    DWORD retcode;
    HANDLE stdoutWr, stdinRd;
    HANDLE stdoutRd, stdinWr;
    static char buf[1000];

    //        HANDLE oldhand = GetStdHandle(STD_OUTPUT_HANDLE) ;
    //        HANDLE oldhande = GetStdHandle(STD_ERROR_HANDLE) ;
    //        HANDLE oldhandi = GetStdHandle(STD_INPUT_HANDLE) ;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    SECURITY_ATTRIBUTES security;

    // set up the working dir for the tool
    strcpy(path, wdp);
    p = strrchr(path, '\\');
    if (p && p[-1] != ':')
    {
        *p = 0;
    }

    memset(&security, 0, sizeof(security));
    security.nLength = sizeof(security);
    security.bInheritHandle = TRUE;
    CreatePipe(&stdoutRd, &stdoutWr, &security, 0);

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    CreatePipe(&stdinRd, &stdinWr, &security, 0);
    DuplicateHandle(GetCurrentProcess(), stdinWr, GetCurrentProcess(), &stdinWr, 0, FALSE,
                    DUPLICATE_SAME_ACCESS | DUPLICATE_CLOSE_SOURCE);

    si.hStdInput = INVALID_HANDLE_VALUE;
    si.hStdOutput = stdoutWr;
    si.hStdError = stdoutWr;

    retcode = CreateProcess(0, cmd, 0, 0, TRUE, CREATE_SUSPENDED, 0, path, &si, &pi);

    CloseHandle(stdoutWr);
    CloseHandle(stdinRd);

    if (retcode)
    {
        int prio = GetThreadPriority(GetCurrentThread());
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
        SetPriorityClass(pi.hProcess, NORMAL_PRIORITY_CLASS);
        SetThreadPriority(pi.hThread, THREAD_PRIORITY_HIGHEST);
        ResumeThread(pi.hThread);
        while (ParsePipeData(stdoutRd, window, pi.hProcess))
            ;
        WaitForSingleObject(pi.hProcess, INFINITE);
        while (ParsePipeData(stdoutRd, window, pi.hProcess))
            ;
        SetThreadPriority(GetCurrentThread(), prio);
    }
    else if (window != ERR_NO_WINDOW)
    {
        sprintf(buf, "\r\n%d: Can't spawn %s\r\n", GetLastError(), cmd);
        SendInfoMessage(window, buf);
    }

    CloseHandle(stdoutRd);
    CloseHandle(stdinWr);

    if (!retcode)
        return 0x55555555;
    else
    {
        GetExitCodeProcess(pi.hProcess, &retcode);
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    return retcode;
}

//-------------------------------------------------------------------------

void MakeMessage(char* title, char* name)
{
    char buf[512], buf1[512], *p = name;
    if (!title)
        title = "%s";
    name = strrchr(name, '\\');
    if (name)
        name++;
    else
        name = p;
    sprintf(buf1, title, name);
    sprintf(buf, ";============ %s ============\r\n", buf1);
    SendInfoMessage(ERR_BUILD_WINDOW, buf);
    SendInfoMessage(ERR_EXTENDED_BUILD_WINDOW, buf);
}
void ErrWarnCounts()
{
    char buf[256];
    sprintf(buf, "\r\nCompile done.  Errors: %d,  Warnings: %d\r\n", errcount, warncount);
    if (0 != errcount)
        SetInfoColor(ERR_BUILD_WINDOW, 0x0000ff);  // red
    else if (0 != warncount)
        SetInfoColor(ERR_BUILD_WINDOW, 0xff0000);  // blue
    SendInfoMessage(ERR_BUILD_WINDOW, buf);
    if (0 != errcount)
        SetInfoColor(ERR_EXTENDED_BUILD_WINDOW, 0x0000ff);  // red
    else if (0 != warncount)
        SetInfoColor(ERR_EXTENDED_BUILD_WINDOW, 0xff0000);  // blue
    SendInfoMessage(ERR_EXTENDED_BUILD_WINDOW, buf);
}
/*---------------------------------------------------------------------------------------------------*/
void ResetErrorCounts(void)
{
    errcount = 0;
    warncount = 0;
}
