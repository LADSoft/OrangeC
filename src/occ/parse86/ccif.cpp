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

#include "compiler.h"
#include "db.h"
#include "symtypes.h"

extern HASHTABLE* defsyms;

typedef struct
{
    char* name;
    int lineTop;
    int dataSize;
    char* data;
    sqlite_int64 fileId;
} LINEINCLUDES;

typedef struct _using
{
    struct _using* next;
    int line;
    char* file;
    SYMBOL* sym;
    SYMBOL* parent;
} USING;

static USING* lusing;

HASHTABLE* ccHash;

static int oldSkip[1000];
static int skipCount;
static LINEINCLUDES* lastFile;
static sqlite3_int64 main_id;
static bool skipThisFile;
static LIST* symList;
static LINEINCLUDES* mainFile;
int equalnode(EXPRESSION* node1, EXPRESSION* node2)
/*
 *      equalnode will return 1 if the expressions pointed to by
 *      node1 and node2 are equivalent.
 */
{
    if (node1 == 0 || node2 == 0)
        return 0;
    if (node1->type != node2->type)
        return 0;
    if (natural_size(node1) != natural_size(node2))
        return 0;
    switch (node1->type)
    {
        case en_const:
        case en_pc:
        case en_global:
        case en_auto:
        case en_absolute:
        case en_threadlocal:
        case en_tempref:
            return node1->v.sp == node2->v.sp;
        default:
            return (!node1->left || equalnode(node1->left, node2->left)) &&
                   (!node1->right || equalnode(node1->right, node2->right));
        case en_labcon:
        case en_c_i:
        case en_c_l:
        case en_c_ul:
        case en_c_ui:
        case en_c_c:
        case en_c_u16:
        case en_c_u32:
        case en_c_bool:
        case en_c_uc:
        case en_c_ll:
        case en_c_ull:
        case en_c_wc:
        case en_nullptr:
            return node1->v.i == node2->v.i;
        case en_c_d:
        case en_c_f:
        case en_c_ld:
        case en_c_di:
        case en_c_fi:
        case en_c_ldi:
            return (node1->v.f == node2->v.f);
        case en_c_dc:
        case en_c_fc:
        case en_c_ldc:
            return (node1->v.c.r == node2->v.c.r) && (node1->v.c.i == node2->v.c.i);
    }
}
const char* GetSymName(SYMBOL* sp, SYMBOL* parent)
{
    static char buf[4096];
    if (sp->thisPtr)
    {
        return "_this";
    }
    else if (sp->storage_class == sc_namespace)
    {
        mangleNameSpaces(buf, sp);
    }
    else if (sp->storage_class == sc_localstatic || sp->storage_class == sc_auto || sp->storage_class == sc_parameter)
    {
        sprintf(buf, "_%s", sp->name);
    }
    else if (sp->decoratedName)
    {
        if (sp != parent && !isfunction(sp->tp))
        {
            char buf1[2048];
            const char* p = strchr(sp->decoratedName + 1, '@');
            if (!p)
                p = sp->name;

            sprintf(buf1, "@%s@%s", parent->name, sp->name);
            return litlate(buf1);
        }
        return sp->decoratedName;
    }
    else
    {
        return sp->name;
    }
    return buf;
}
static int WriteStructMembers(SYMBOL* sym, SYMBOL* parent, sqlite3_int64 struct_id, sqlite3_int64 file_id, int order, bool base,
                              enum e_ac access)
{
    if (basetype(sym->tp)->syms)
    {
        SYMLIST* hr = basetype(sym->tp)->syms->table[0];
        if (!isfunction(sym->tp))
        {
            BASECLASS* bases = sym->baseClasses;
            while (bases)
            {
                order =
                    WriteStructMembers(bases->cls, parent, struct_id, file_id, order, true, (e_ac)imin(bases->cls->access, access));
                bases = bases->next;
            }
        }
        while (hr)
        {
            SYMBOL* st = (SYMBOL*)hr->p;
            if (st->storage_class == sc_overloads)
            {
                order = WriteStructMembers(st, parent, struct_id, file_id, order, base, access);
            }
            else
            {
                static char type_name[100000];
                memset(type_name, 0, sizeof(type_name));
                int indirectCount = 0;
                int rel_id = 0;
                TYPE* tp = st->tp;
                sqlite3_int64 id;
                int flags = imin(st->access, access) & 15;
                if (st->storage_class == sc_static || st->storage_class == sc_external)
                    flags |= 16;
                if (ismemberdata(st))
                    flags |= 32;
                if (st->storage_class == sc_virtual)
                    flags |= 64;
                if (isfunction(st->tp))
                {
                    flags |= 128;
                    if (basetype(st->tp)->syms->table[0] && ((SYMBOL*)basetype(st->tp)->syms->table[0]->p)->thisPtr)
                        flags |= 2048;
                }
                if (base)
                    flags |= 256;
                if (st->isConstructor)
                    flags |= 512;
                if (st->isDestructor)
                    flags |= 1024;
                if (isfunction(tp))
                    tp = basetype(tp)->btp;
                while (isref(tp))
                    tp = basetype(tp)->btp;
                while (ispointer(tp))
                    tp = basetype(tp)->btp, indirectCount++;
                if (isstructured(tp) && isstructured(basetype(tp)->sp->tp))
                {
                    rel_id = basetype(tp)->sp->tp->sp->ccStructId;
                }
                type_name[0] = 0;
                if (isstructured(tp))
                {
                    switch (basetype(tp)->type)
                    {
                        case bt_struct:
                            strcat(type_name, "struct ");
                            break;
                        case bt_class:
                            strcat(type_name, "class ");
                            break;
                        case bt_union:
                            strcat(type_name, "union ");
                            break;
                        default:
                            break;
                    }
                }
                else if (basetype(tp)->type == bt_enum)
                {
                    strcat(type_name, "enum ");
                }
                typenum(type_name + strlen(type_name), st->tp);

                ccWriteStructField(struct_id, GetSymName(st, parent), litlate(type_name), indirectCount, rel_id, file_id, main_id,
                                   flags, &order, &id);
            }
            hr = hr->next;
        }
    }
    return order;
}
static void DumpStructs(void)
{
    LIST* item = symList;
    while (item)
    {
        SYMBOL* sym = (SYMBOL*)item->data;
        if (sym->storage_class != sc_label && istype(sym) && isstructured(sym->tp) && sym->tp->btp->type != bt_typedef)  // DAL fix
        {
            sqlite3_int64 struct_id;
            if (ccWriteStructName(sym->decoratedName, &struct_id))
                basetype(sym->tp)->sp->ccStructId = struct_id;
        }
        item = item->next;
    }
    item = symList;
    while (item)
    {
        SYMBOL* sym = (SYMBOL*)item->data;
        if (sym->storage_class != sc_label && istype(sym) && isstructured(sym->tp) && sym->storage_class != sc_typedef &&
            sym->tp->syms)
        {
            sqlite3_int64 struct_id = basetype(sym->tp)->sp->ccStructId, file_id;
            if (ccWriteFileName(sym->origdeclfile, &file_id))
            {
                int order = 1;
                WriteStructMembers(sym, sym, struct_id, file_id, order, false, sym->access);
            }
        }
        item = item->next;
    }
}
static void DumpSymbolType(SYMBOL* sym)
{
    int type;
    const char* name = GetSymName(sym, sym);
    if (strstr(name, "++"))
        name = " ";
    if (sym->tp && isfunction(sym->tp))
        type = ST_FUNCTION;
    else
        switch (sym->storage_class)
        {
            case sc_overloads:
                return;
            case sc_typedef:
                type = ST_TYPEDEF;
                break;
            case sc_namespace:
            case sc_type:
                type = ST_TAG;
                break;
            case sc_auto:
            case sc_register:
                type = ST_AUTO;
                break;
            case sc_parameter:
                type = ST_PARAMETER;
                break;
            case sc_localstatic:
                type = ST_LOCALSTATIC;
                break;
            case sc_static:
                type = ST_STATIC;
                break;
            case sc_global:
                type = ST_GLOBAL;
                break;
            case sc_external:
                type = ST_EXTERN;
                break;
            case sc_label:
                type = ST_LABEL;
                break;
            default:
                type = ST_UNKNOWN;
                break;
        }
    /*
    if (isconst(sym->tp))
        type |= ST_CONST;
    if (isvolatile(sym->tp))
        type |= ST_VOLATILE;
        */
    ccWriteSymbolType(name, main_id, sym->origdeclfile ? sym->origdeclfile : (char*)"$$$", sym->origdeclline, sym->ccEndLine, type);
}
static void DumpSymbol(SYMBOL* sym);
static void DumpNamespace(SYMBOL* sym)
{
    const char* symName = GetSymName(sym, sym);
    struct _ccNamespaceData* ns = sym->ccNamespaceData;
    while (ns)
    {
        ccWriteNameSpaceEntry(symName, main_id, ns->declfile, ns->startline, ns->endline);
        ns = ns->next;
    }
}
static void DumpSymbol(SYMBOL* sym)
{
    DumpSymbolType(sym);
    if (sym->storage_class != sc_label && (!istype(sym) || sym->storage_class == sc_typedef) &&
        sym->storage_class != sc_overloads && sym->tp->type != bt_any)
    {
        SYMBOL* declsym;
        char type_name[100000];
        int indirectCount = 0;
        const char* name = GetSymName(sym, sym);
        TYPE* tp = sym->tp;
        sqlite3_int64 id, struct_id = 0;
        if (strstr(name, "++"))
            name = " ";
        if (isfunction(tp))
            tp = basetype(tp)->btp;  // get rv
        type_name[0] = 0;
        if (sym->storage_class == sc_typedef)
        {
            strcpy(type_name, "typedef ");
        }
        if (isstructured(tp))
        {
            switch (basetype(tp)->type)
            {
                case bt_struct:
                    strcat(type_name, "struct ");
                    break;
                case bt_class:
                    strcat(type_name, "class ");
                    break;
                case bt_union:
                    strcat(type_name, "union ");
                    break;
                default:
                    break;
            }
        }
        else if (basetype(tp)->type == bt_enum)
        {
            strcat(type_name, "enum ");
        }
        typenum(type_name + strlen(type_name), tp->type == bt_typedef ? tp->btp : tp);
        type_name[40] = 0;
        while (isref(tp))
            tp = basetype(tp)->btp;
        while (ispointer(tp))
            tp = basetype(tp)->btp, indirectCount++;
        if (isstructured(tp))
        {
            struct_id = basetype(tp)->sp->ccStructId;
        }
        declsym = sym;
        if (sym->parentClass)
        {
            // member vars are considered declared at the top of their class...
            declsym = sym->parentClass;
        }
        if (sym->storage_class == sc_namespace)
            DumpNamespace(sym);
        // use sym->declline here to get real function addresses
        else if (ccWriteLineNumbers(name, litlate(type_name), sym->declfile ? sym->declfile : (char*)"$$$", indirectCount,
                                    struct_id, main_id, sym->declline, sym->ccEndLine, sym->endLine, isfunction(sym->tp), &id))
        {
            if (isfunction(sym->tp) && sym->tp->syms)
            {
                int order = 1;
                SYMLIST* hr = sym->tp->syms->table[0];
                while (hr && ((SYMBOL*)hr->p)->storage_class == sc_parameter)
                {
                    SYMBOL* st = (SYMBOL*)hr->p;
                    const char* argName = GetSymName(st, st);
                    if (strstr(argName, "++"))
                        argName = " ";
                    type_name[0] = 0;
                    typenum(type_name, st->tp);
                    type_name[40] = 0;
                    if (argName[0] == 'U' && strstr(argName, "++"))
                        argName = "{unnamed} ";  // the ide depends on the first char being '{'
                    ccWriteGlobalArg(id, main_id, argName, litlate(type_name), &order);
                    hr = hr->next;
                }
            }
        }
    }
}
static void DumpSymbols(void)
{
    LIST* item = symList;
    int i;
    while (item)
    {
        SYMBOL* sym = (SYMBOL*)item->data;
        DumpSymbol(sym);
        item = item->next;
    }
    for (i = 0; i < GLOBALHASHSIZE; i++)
    {
        SYMLIST* hr = defsyms->table[i];
        while (hr)
        {
            ccWriteSymbolType(((SYMBOL*)hr->p)->name, main_id, (char*)"$$$", 1, 0, ST_DEFINE);
            hr = hr->next;
        }
    }
}
static void DumpLines(void)
{
    int i;
    for (i = 0; i < ccHash->size; i++)
    {
        SYMLIST* hr = ccHash->table[i];
        while (hr)
        {
            LINEINCLUDES* x = (LINEINCLUDES*)hr->p;
            if (!x->dataSize)
            {
                x->lineTop = 0;
                x->dataSize = 1;
                x->data = (char*)"";  // is a null
            }
            ccWriteLineData(x->fileId, main_id, x->data, x->dataSize, x->lineTop);
            hr = hr->next;
        }
    }
}
static void DumpUsing(void)
{
    while (lusing)
    {
        char name1[4096], name2[4096];
        strcpy(name1, GetSymName(lusing->sym, lusing->sym));
        if (lusing->parent)
            strcpy(name2, GetSymName(lusing->parent, lusing->parent));
        else
            name2[0] = 0;
        ccWriteUsingRecord(name1, name2, lusing->file, lusing->line, main_id);
        lusing = lusing->next;
    }
}
static void DumpFiles(void)
{
    int i;
    time_t n = time(0);
    for (i = 0; i < ccHash->size; i++)
    {
        SYMLIST* hr = ccHash->table[i];
        while (hr)
        {
            LINEINCLUDES* l = (LINEINCLUDES*)hr->p;
            ccWriteFileTime(l->name, n, &l->fileId);
            hr = hr->next;
        }
    }
}
void ccDumpSymbols(void)
{
    time_t n = time(0);
    ccBegin();
    ccWriteFileTime(mainFile->name, n, &mainFile->fileId);
    main_id = mainFile->fileId;
    ccDBDeleteForFile(main_id);
    DumpFiles();
    DumpStructs();
    DumpSymbols();
    DumpLines();
    DumpUsing();
    ccEnd();
    symList = NULL;
    main_id = 0;
}
void ccInsertUsing(SYMBOL* ns, SYMBOL* parentns, char* file, int line)
{
    if (!skipThisFile)
    {
        USING* susing = (USING*)Alloc(sizeof(USING));
        susing->line = line;
        susing->file = file;
        susing->sym = ns;
        susing->parent = parentns;
        susing->next = lusing;
        lusing = susing;
    }
}
void ccSetSymbol(SYMBOL* sp)
{
    if (!skipThisFile)
    {
        LIST* newItem = (LIST*)Alloc(sizeof(LIST));
        if (sp->decoratedName)
        {
            newItem->next = symList;
            newItem->data = sp;
            symList = newItem;
        }
    }
}
void ccNewFile(char* fileName, bool main)
{
    LINEINCLUDES* l;
    char *s = fullqualify(fileName), *q = s;
    while (*q)
    {
        *q = tolower(*q);
        q++;
    }
    if (main)
    {
        ccHash = CreateHashTable(256);
        ccReset();
    }
    // this only parses each file ONCE!!!!
    oldSkip[skipCount++] = skipThisFile;
    skipThisFile = !!LookupName(s, ccHash);
    if (!skipThisFile)
    {
        IncGlobalFlag();
        l = (LINEINCLUDES*)Alloc(sizeof(LINEINCLUDES));
        l->name = litlate(s);
        lastFile = l;
        l->fileId = 0;
        if (main)
            mainFile = l;
        else
            insert((SYMBOL*)l, ccHash);
        DecGlobalFlag();
    }
}
void ccEndFile(void) { skipThisFile = oldSkip[--skipCount]; }
void ccSetFileLine(char* filename, int lineno)
{
    if (!skipThisFile)
    {
        char* q;
        filename = fullqualify(filename);
        q = filename;
        while (*q)
        {
            *q = tolower(*q);
            q++;
        }
        if (strcmp(filename, lastFile->name) != 0)
        {
            lastFile = (LINEINCLUDES*)search(filename, ccHash);
            if (!lastFile)
                lastFile = mainFile;
        }
        if (lineno >= lastFile->dataSize * 8)
        {
            int n = lastFile->dataSize;
            lastFile->dataSize = lastFile->dataSize ? lastFile->dataSize * 2 : 100;
            if (lastFile->dataSize < (lineno + 8) / 8)
                lastFile->dataSize = (lineno + 8) / 8;
            lastFile->data = (char*)realloc(lastFile->data, lastFile->dataSize);
            memset(lastFile->data + n, 0, lastFile->dataSize - n);
        }
        lastFile->lineTop = lineno;
        lastFile->data[lineno / 8] |= (1 << (lineno & 7));
    }
}