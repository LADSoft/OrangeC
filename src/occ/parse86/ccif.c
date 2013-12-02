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
#include "compiler.h"
#include "db.h"

typedef struct
{
    char *name;
    int lineTop;
    int dataSize;
    char *data;
    sqlite_int64 fileId;
} LINEINCLUDES ;
HASHTABLE *ccHash;
static LINEINCLUDES *lastFile;
static sqlite3_int64 main_id;

static LIST *symList;
static void DumpStructs(void)
{
    LIST *item = symList;
    while (item)
    {
        SYMBOL *sym = item->data;
        if (istype(sym) && isstructured(sym->tp) && sym->storage_class != sc_typedef)
        {
            sqlite3_int64 struct_id;
            if (ccWriteStructName(sym->name, &struct_id))
                sym->ccStructId = struct_id;
        }
        item = item->next;
    }
    item = symList;
    while (item)
    {
        SYMBOL *sym = item->data;
        if (istype(sym) && isstructured(sym->tp) && sym->storage_class != sc_typedef && sym->tp->syms)
        {
            sqlite3_int64 struct_id = sym->ccStructId, file_id;
            if (ccWriteFileName(sym->declfile, &file_id))
            {
                int order = 1;
                HASHREC *hr = sym->tp->syms->table[0];
                while (hr)
                {
                    char type_name[10000];
                    SYMBOL *st = (SYMBOL *)hr->p;
                    int indirectCount = 0;
                    int rel_id = 0;
                    TYPE *tp = st->tp;
                    sqlite3_int64 id;
                    while (ispointer(tp))
                        tp = basetype(tp)->btp, indirectCount++;
                    if (isstructured(tp))
                    {
                        rel_id = basetype(tp)->sp->ccStructId;
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
                    typenum(type_name+strlen(type_name), st->tp);
                    ccWriteStructField(struct_id, st->name, litlate(type_name), 
                                       indirectCount, rel_id,
                                       file_id, main_id, &order, &id);
                    hr = hr->next;
                }
            }
        }
        item = item->next;
    }
}
static void DumpSymbols(void)
{
    LIST *item = symList;
    while (item)
    {
        SYMBOL *sym = item->data;
        if ((!istype(sym) || sym->storage_class == sc_typedef) && sym->storage_class != sc_overloads && sym->tp->type != bt_any)
        {
            char type_name[10000];
            int indirectCount = 0;
            char *name = sym->name;
            TYPE *tp = sym->tp;
            sqlite3_int64 id, struct_id = 0;
            if (strstr(name, "++"))
                name = " ";
            if (isfunction(tp))
                tp = basetype(tp)->btp; // get rv
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
            typenum(type_name+strlen(type_name), tp->type == bt_typedef ? tp->btp : tp);
            while (ispointer(tp))
                tp = basetype(tp)->btp, indirectCount++;
            if (isstructured(tp))
            {
                struct_id = basetype(tp)->sp->ccStructId;
            }
            if (ccWriteLineNumbers( name, litlate(type_name), sym->declfile ? sym->declfile : "$$$", 
                                   indirectCount, struct_id, main_id, 
                               sym->declline, sym->ccEndLine, isfunction(sym->tp), &id))
            {
                if (isfunction(sym->tp) && sym->tp->syms)
                {
                    int order = 1;
                    HASHREC *hr = sym->tp->syms->table[0];
                    while (hr && ((SYMBOL *)hr->p)->storage_class == sc_parameter)
                    {
                        char type_name[10000];
                        SYMBOL *st = (SYMBOL *)hr->p;
                        char *argName = st->name;
                        type_name[0] = 0;
                        typenum(type_name, st->tp);
                        if (argName[0] == 'U' && strstr(argName, "++"))
                            argName = "{unnamed} "; // the ide depends on the first char being '{'
                        ccWriteGlobalArg(id, main_id, argName, litlate(type_name), &order);
                        hr = hr->next;
                    }
                }
            }
        }
        item = item->next;
    }
}
static void DumpLines(void)
{
    int i;
    for (i=0;i < ccHash->size; i++)
    {
        HASHREC *hr = ccHash->table[i];
        while(hr)
        {
            LINEINCLUDES *x = (LINEINCLUDES *)hr->p;
            if (!x->dataSize)
            {
                x->lineTop = 0;
                x->dataSize = 1;
                x->data = ""; // is a null
            }
            ccWriteLineData(x->fileId, main_id, x->data, x->dataSize, x->lineTop);
            hr = hr->next;
        }
    }
}
void ccDumpSymbols(void)
{
    ccBegin();
    ccDBDeleteForFile(main_id);
    DumpStructs();
    DumpSymbols();
    DumpLines();
    ccEnd();
    symList = NULL;
    main_id = 0;
}
void ccSetSymbol(SYMBOL *sp)
{
    LIST *newItem = Alloc(sizeof(LIST));
    newItem->next = symList;
    newItem->data = sp;
    symList = newItem;
}
void ccNewFile(char *fileName, BOOL main)
{
    LINEINCLUDES *l = Alloc(sizeof(LINEINCLUDES));
    sqlite3_int64 id;
    time_t n = time(0);
    l->name = litlate(fullqualify(fileName));
    if (main)
    {
        ccHash = CreateHashTable(32);
        ccReset();
    }
    insert((SYMBOL *)l, ccHash);
    lastFile = l;
    if (ccWriteFileTime(l->name, n, &id))
    {
        if (main)
        {
            main_id = id;
        }
    }
    l->fileId = id;    
}
void ccSetFileLine(char *filename, int lineno)
{
    filename = fullqualify(filename);
    if (strcmp(filename, lastFile->name))
    {
        lastFile = (LINEINCLUDES *)search(filename, ccHash);
    }
    if (lineno >= lastFile->dataSize * 8)
    {
        int n = lastFile->dataSize;
        lastFile->dataSize = lastFile->dataSize? lastFile->dataSize * 2 : 100;
        lastFile->data = realloc(lastFile->data, lastFile->dataSize);
        memset(lastFile->data +n, 0, lastFile->dataSize - n);
    }
    lastFile->lineTop = lineno;
    lastFile->data[lineno/8] |= (1 << (lineno & 7));
}