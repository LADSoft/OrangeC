/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
 *
 */

#include "compiler.h"
#include "db.h"
#include "ccerr.h"
#include "declare.h"
#include "templatedecl.h"
#include "templateinst.h"
#include "templateutil.h"
#include "templatededuce.h"
#include "symtypes.h"
#include "Utils.h"
#include "PreProcessor.h"
#include "ioptimizer.h"
#include "memory.h"
#include "mangle.h"
#include "lex.h"
#include "help.h"
#include "namespace.h"
#include "symtab.h"
#include "types.h"

using namespace Parser;

namespace Parser
{
extern PreProcessor* preProcessor;
}  // namespace Parser

namespace CompletionCompiler
{

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
    const char* file;
    SYMBOL* sym;
    SYMBOL* parent;
} USING;

static USING* lusing;

Parser::SymbolTableFactory<SYMBOL> ccHashFactory;
Parser::SymbolTable<SYMBOL>* ccSymbols;

static int oldSkip[1000];
static int skipCount;
static LINEINCLUDES* lastFile;
static sqlite3_int64 main_id;
static bool skipThisFile;
static Optimizer::LIST* symList;
static LINEINCLUDES* mainFile;
const char* GetSymName(SYMBOL* sp, SYMBOL* parent)
{
    static char buf[4096];
    if (sp->sb->thisPtr)
    {
        return "_this";
    }
    else if (sp->sb->storage_class == StorageClass::namespace_)
    {
        mangleNameSpaces(buf, sp);
    }
    else if (sp->sb->storage_class == StorageClass::localstatic_ || sp->sb->storage_class == StorageClass::auto_ ||
             sp->sb->storage_class == StorageClass::parameter_)
    {
        sprintf(buf, "_%s", sp->name);
    }
    else if (sp->sb->decoratedName)
    {
        if (sp != parent && !sp->tp->IsFunction())
        {
            char buf1[2048];
            const char* p = strchr(sp->sb->decoratedName + 1, '@');
            if (!p)
                p = sp->name;

            sprintf(buf1, "@%s@%s", parent->name, sp->name);
            return litlate(buf1);
        }
        return sp->sb->decoratedName;
    }
    else
    {
        return sp->name;
    }
    return buf;
}
static int WriteStructMembers(SYMBOL* sym, SYMBOL* parent, sqlite3_int64 struct_id, sqlite3_int64 file_id, int order, bool base,
                              AccessLevel access)
{
    if (sym->tp->BaseType()->syms)
    {
        if (!sym->tp->IsFunction())
        {
            if (sym->sb->baseClasses)
            {
                for (auto bases : *sym->sb->baseClasses)
                {
                    order = WriteStructMembers(bases->cls, parent, struct_id, file_id, order, true,
                                               (AccessLevel)imin(bases->cls->sb->access, access));
                }
            }
        }
        for (auto st : *sym->tp->BaseType()->syms)
        {
            if (st->sb->storage_class == StorageClass::overloads_)
            {
                order = WriteStructMembers(st, parent, struct_id, file_id, order, base, access);
            }
            else
            {
                static char type_name[100000];
                memset(type_name, 0, sizeof(type_name));
                int indirectCount = 0;
                int rel_id = 0;
                Type* tp = st->tp;
                sqlite3_int64 id;
                int flags = (int)imin(st->sb->access, access) & 15;
                if (st->sb->storage_class == StorageClass::static_ || st->sb->storage_class == StorageClass::external_)
                    flags |= 16;
                if (ismemberdata(st))
                    flags |= 32;
                if (st->sb->storage_class == StorageClass::virtual_)
                    flags |= 64;
                if (st->tp->IsFunction())
                {
                    flags |= 128;
                    if (st->tp->BaseType()->syms->size() && ((SYMBOL*)st->tp->BaseType()->syms->front())->sb->thisPtr)
                        flags |= 2048;
                }
                if (base)
                    flags |= 256;
                if (st->sb->isConstructor)
                    flags |= 512;
                if (st->sb->isDestructor)
                    flags |= 1024;
                if (tp->IsFunction())
                    tp = tp->BaseType()->btp;
                while (tp->IsRef())
                    tp = tp->BaseType()->btp;
                while (tp->IsPtr())
                    tp = tp->BaseType()->btp, indirectCount++;
                if (tp->IsStructured() && tp->BaseType()->sp->tp->IsStructured())
                {
                    rel_id = tp->BaseType()->sp->tp->sp->sb->ccStructId;
                }
                type_name[0] = 0;
                if (tp->IsStructured())
                {
                    switch (tp->BaseType()->type)
                    {
                        case BasicType::struct_:
                            Utils::StrCat(type_name, "struct ");
                            break;
                        case BasicType::class_:
                            Utils::StrCat(type_name, "class ");
                            break;
                        case BasicType::union_:
                            Utils::StrCat(type_name, "union ");
                            break;
                        default:
                            break;
                    }
                }
                else if (tp->BaseType()->type == BasicType::enum_)
                {
                    Utils::StrCat(type_name, "enum ");
                }
                st->tp->BasicTypeToString(type_name + sizeof(type_name), type_name + strlen(type_name));

                ccWriteStructField(struct_id, GetSymName(st, parent), litlate(type_name), indirectCount, rel_id, file_id, main_id,
                                   flags, &order, &id);
            }
        }
    }
    return order;
}
static void DumpStructs(void)
{
    Optimizer::LIST* item = symList;
    while (item)
    {
        SYMBOL* sym = (SYMBOL*)item->data;
        if (sym->sb->storage_class != StorageClass::label_ && sym->tp && istype(sym) && sym->tp->IsStructured() &&
            (!sym->tp->btp || sym->tp->btp->type != BasicType::typedef_))  // DAL fix
        {
            sqlite3_int64 struct_id;
            if (ccWriteStructName(sym->sb->decoratedName, &struct_id))
                sym->tp->BaseType()->sp->sb->ccStructId = struct_id;
        }
        item = item->next;
    }
    item = symList;
    while (item)
    {
        SYMBOL* sym = (SYMBOL*)item->data;
        if (sym->sb->storage_class != StorageClass::label_ && sym->tp && istype(sym) && sym->tp->IsStructured() &&
            sym->sb->storage_class != StorageClass::typedef_ && sym->tp->syms)
        {
            sqlite3_int64 struct_id = sym->tp->BaseType()->sp->sb->ccStructId, file_id;
            if (ccWriteFileName(sym->sb->origdeclfile, &file_id))
            {
                int order = 1;
                WriteStructMembers(sym, sym, struct_id, file_id, order, false, sym->sb->access);
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
    if (sym->tp && sym->tp->IsFunction())
        type = ST_FUNCTION;
    else
        switch (sym->sb->storage_class)
        {
            case StorageClass::overloads_:
                return;
            case StorageClass::typedef_:
                type = ST_TYPEDEF;
                break;
            case StorageClass::namespace_:
            case StorageClass::type_:
                type = ST_TAG;
                break;
            case StorageClass::auto_:
            case StorageClass::register_:
                type = ST_AUTO;
                break;
            case StorageClass::parameter_:
                type = ST_PARAMETER;
                break;
            case StorageClass::localstatic_:
                type = ST_LOCALSTATIC;
                break;
            case StorageClass::static_:
                type = ST_STATIC;
                break;
            case StorageClass::global_:
                type = ST_GLOBAL;
                break;
            case StorageClass::external_:
                type = ST_EXTERN;
                break;
            case StorageClass::label_:
                type = ST_LABEL;
                break;
            default:
                type = ST_UNKNOWN;
                break;
        }
    /*
    if (sym->tp->IsConst())
        type |= ST_CONST;
    if (sym->tp->IsVolatile())
        type |= ST_VOLATILE;
        */
    ccWriteSymbolType(name, main_id, sym->sb->origdeclfile ? sym->sb->origdeclfile : (char*)"$$$", sym->sb->origdeclline,
                      sym->sb->ccEndLine, type);
}
static void DumpSymbol(SYMBOL* sym);
static void DumpNamespace(SYMBOL* sym)
{
    const char* symName = GetSymName(sym, sym);
    if (sym->sb->ccNamespaceData)
    {
        for (auto ns : *sym->sb->ccNamespaceData)
            while (ns)
            {
                ccWriteNameSpaceEntry(symName, main_id, ns->declfile, ns->startline, ns->endline);
            }
    }
}
static void DumpSymbol(SYMBOL* sym)
{
    DumpSymbolType(sym);
    if (sym->sb->storage_class != StorageClass::label_ && sym->tp &&
        (!istype(sym) || sym->sb->storage_class == StorageClass::typedef_) && sym->sb->storage_class != StorageClass::overloads_ &&
        sym->tp->type != BasicType::any_)
    {
        SYMBOL* declsym;
        char type_name[100000];
        int indirectCount = 0;
        const char* name = GetSymName(sym, sym);
        Type* tp = sym->tp;
        sqlite3_int64 id, struct_id = 0;
        if (strstr(name, "++"))
            name = " ";
        if (tp->IsFunction())
            tp = tp->BaseType()->btp;  // get rv
        type_name[0] = 0;
        if (sym->sb->storage_class == StorageClass::typedef_)
        {
            strcpy(type_name, "typedef ");
        }
        if (tp->IsStructured())
        {
            switch (tp->BaseType()->type)
            {
                case BasicType::struct_:
                    Utils::StrCat(type_name, "struct ");
                    break;
                case BasicType::class_:
                    Utils::StrCat(type_name, "class ");
                    break;
                case BasicType::union_:
                    Utils::StrCat(type_name, "union ");
                    break;
                default:
                    break;
            }
        }
        else if (tp->BaseType()->type == BasicType::enum_)
        {
            Utils::StrCat(type_name, "enum ");
        }
        (tp->type == BasicType::typedef_ ? tp->btp : tp)->BasicTypeToString(type_name + sizeof(type_name), type_name + strlen(type_name));
        type_name[40] = 0;
        while (tp->IsRef())
            tp = tp->BaseType()->btp;
        while (tp->IsPtr())
            tp = tp->BaseType()->btp, indirectCount++;
        if (tp->IsStructured())
        {
            struct_id = tp->BaseType()->sp->sb->ccStructId;
        }
        declsym = sym;
        if (sym->sb->parentClass)
        {
            // member vars are considered declared at the top of their class...
            declsym = sym->sb->parentClass;
        }
        if (sym->sb->storage_class == StorageClass::namespace_)
            DumpNamespace(sym);
        // use sym->sb->declline here to get real function addresses
        else if (ccWriteLineNumbers(name, litlate(type_name), sym->sb->declfile ? sym->sb->declfile : (char*)"$$$", indirectCount,
                                    struct_id, main_id, sym->sb->declline, sym->sb->ccEndLine, sym->sb->endLine,
                                    sym->tp->IsFunction(), &id))
        {
            if (sym->tp->IsFunction() && sym->tp->syms)
            {
                int order = 1;
                auto it = sym->tp->syms->begin();
                auto ite = sym->tp->syms->end();
                for (; it != ite && ((*it)->sb->storage_class == StorageClass::parameter_); ++it)
                {
                    SYMBOL* st = *it;
                    const char* argName = GetSymName(st, st);
                    if (strstr(argName, "++"))
                        argName = " ";
                    type_name[0] = 0;
                    st->tp->BasicTypeToString(type_name + sizeof(type_name), type_name);
                    type_name[40] = 0;
                    if (argName[0] == 'U' && strstr(argName, "++"))
                        argName = "{unnamed} ";  // the ide depends on the first char being '{'
                    ccWriteGlobalArg(id, main_id, argName, litlate(type_name), &order);
                }
            }
        }
    }
}
static void DumpSymbols(void)
{
    Optimizer::LIST* item = symList;
    int i;
    while (item)
    {
        SYMBOL* sym = (SYMBOL*)item->data;
        DumpSymbol(sym);
        item = item->next;
    }
    for (auto&& v : preProcessor->GetDefines())
    {
        ccWriteSymbolType(v->GetName().c_str(), main_id, (char*)"$$$", 1, 0, ST_DEFINE);
    }
}
static void DumpLines(void)
{
    int i;
    for (auto sym : *ccSymbols)
    {
        LINEINCLUDES* x = (LINEINCLUDES*)sym->sb;
        if (!x->dataSize)
        {
            x->lineTop = 0;
            x->dataSize = 1;
            x->data = (char*)"";  // is a null
        }
        ccWriteLineData(x->fileId, main_id, x->data, x->dataSize, x->lineTop);
    }
}
static void DumpUsing(void)
{
    while (lusing)
    {
        char name1[4096], name2[4096];
        Utils::StrCpy(name1, GetSymName(lusing->sym, lusing->sym));
        if (lusing->parent)
            Utils::StrCpy(name2, GetSymName(lusing->parent, lusing->parent));
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
    for (auto sym : *ccSymbols)
    {
        LINEINCLUDES* l = (LINEINCLUDES*)sym->sb;
        ccWriteFileTime(l->name, n, &l->fileId);
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
void ccInsertUsing(SYMBOL* ns, SYMBOL* parentns, const char* file, int line)
{
    if (!skipThisFile)
    {
        USING* susing = Allocate<USING>();
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
        Optimizer::LIST* newItem = Allocate<Optimizer::LIST>();
        if (sp->sb && sp->sb->decoratedName)
        {
            newItem->next = symList;
            newItem->data = sp;
            symList = newItem;
        }
    }
}
std::string ccNewFile(char* fileName, bool main)
{
    LINEINCLUDES* l;
    char *s = Utils::FullQualify(fileName), *q = s;
    while (*q)
    {
        *q = tolower(*q);
        q++;
    }
    if (main)
    {
        ccHashFactory.Reset();
        ccSymbols = ccHashFactory.CreateSymbolTable();
        ccReset();
    }
    // this only parses each file ONCE!!!!
    oldSkip[skipCount++] = skipThisFile;
    skipThisFile = !!ccSymbols->Lookup(s);
    if (!skipThisFile)
    {
        l = Allocate<LINEINCLUDES>();
        l->name = litlate(s);
        lastFile = l;
        l->fileId = 0;
        if (main)
            mainFile = l;
        else
        {
            // this is broken, the rest of the routines use
            // the line includes as sym->sb
            ccSymbols->Add((SYMBOL*)l);
        }
    }
    return "";
}
void ccEndFile(void) { skipThisFile = oldSkip[--skipCount]; }
void ccSetFileLine(char* filename, int lineno)
{
    if (!skipThisFile)
    {
        char* q;
        filename = Utils::FullQualify(filename);
        q = filename;
        while (*q)
        {
            *q = tolower(*q);
            q++;
        }
        if (strcmp(filename, lastFile->name) != 0)
        {
            lastFile = (LINEINCLUDES*)search(ccSymbols, filename);
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
}  // namespace CompletionCompiler