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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dos.h>
#include <stdarg.h>
#include "be.h"
#include <time.h>
#include "objieee.h"
#include <assert.h>

#define DEBUG_VERSION 4.0
#define SECT_EQUAL 8
#define SECT_ROM 0x80
#define SECT_RAM 0x400

#define EEMBEDDED 0x80000000
#define EBYTE 1
#define EWORD 2
#define EDWORD 3
#define ESTRING 4
#define EBUF 5

#define embed(x) ( (x) | EEMBEDDED)
#define LDPERLINE 256
extern COMPILER_PARAMS cparams;
extern FILELIST *incfiles;
extern VIRTUAL_LIST *virtualFirst;
extern int prm_debug;
extern FILE *outputFile, *browseFile;
extern char version[];
extern char *infile;
extern EMIT_TAB segs[MAX_SEGS];
extern LIST *libincludes;
extern HASHREC **gsyms;
extern LIST *localfuncs,  *localdata;
extern int prm_bss;
extern int outcode_base_address;
extern LIST *includedFiles;

int dbgblocknum;

static int anyusedfloat = FALSE;

static int browsing;
static LIST *dbgTypeDefs;
static int firstVirtualSeg;
static int segxlattab[MAX_SEGS];
static int dgroupVal;
static int extSize, extIndex, pubIndex, localIndex, autoIndex, typeIndex;
static int checksum = 0;
static int options = 1; // signed addr
static int bitspermau = 8;
static int maus = 4;
static int bigendchar = 'L';

static LIST *includedLibraries;
static LIST *externNames ;
static LIST *globalNames ;
static LIST *exportNames ;
static LIST *importNames ;

static BROWSEINFO *browseInfo;
static BROWSEFILE *browseFiles;

char *segnames[] = 
{
    0, "code", "data", "bss", "string", "const", "tls", "cstartup", "crundown", 
        "cppinit", "cppexit", "codefix", "datafix", "lines", "types", "symbols",
        "browse"
};
char *segclasses[] = 
{
    0, "code", "data", "bss", "string", "const", "tls", "cstartup", "crundown", 
        "cppinit", "cppexit", "codefix", "datafix", "lines", "types", "symbols",
        "browse"
};
int segAligns[] =
{
    1, 2, 8, 8, 2, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
} ;
static char *segchars[] = 
{
    0, "R", "W", "W", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R"
};
static unsigned segcharsbin[] = 
{
    0, SECT_ROM, SECT_RAM, SECT_RAM, SECT_ROM, SECT_ROM, SECT_ROM, SECT_ROM, SECT_ROM, SECT_ROM, SECT_ROM, SECT_ROM, SECT_ROM, SECT_ROM, SECT_ROM, SECT_ROM, SECT_ROM
};
#define TYPE_HASH_SIZE 128
struct _type_hash {
    struct _type_hash *next;
    int id;
    char *name;
} *typeHashTab[TYPE_HASH_SIZE];

void dbginit(void) 
{ 
    dbgTypeDefs = NULL;
}
void omfInit(void)
{
    includedLibraries = 0;
    externNames = 0;
    globalNames = 0;
    exportNames = 0;
    importNames = 0;
    browseInfo = NULL;
    browseFiles = NULL;
}
static void PutRecord(BYTE *buf)
{
    int len = buf[1] + (buf[2] << 8);
    int i;
    for (i = 0; i < len; i++)
            checksum += buf[i];
    owrite(buf, 1, len, browsing ? browseFile : outputFile);
}
static void RenderMessageInternal(BYTE *buf, va_list arg)
{
    BYTE *p = buf + (buf[1]) + (buf[2] << 8);
    int n;
    while (n = va_arg(arg, int))
    {
        int len;
        if (n & EEMBEDDED)
        {
            *p++ = n & 0xff;
        }
        else switch (n)
        {
            case EBYTE:
                n = va_arg(arg, int);
                *p++ = n & 0xff;
                break;
            case EWORD:
                n = va_arg(arg, int);
                *p++ = n & 0xff;
                *p++ = (n >> 8) & 0xff;
                break;
            case EDWORD:
                n = va_arg(arg, int);
                *p++ = n & 0xff;
                *p++ = (n >> 8) & 0xff;
                *p++ = (n >> 16) & 0xff;
                *p++ = (n >> 24) & 0xff;
                break;
            case ESTRING:
            {
                int i;
                char *src = va_arg(arg, char *);
                int l = strlen(src);
                *p++ = l;
                for (i=0; i < l; i++)
                    *p++ = src[i];
                break;
            }
            case EBUF:
            {
                BYTE *buf = va_arg(arg, BYTE *);
                int n = va_arg(arg, int);
                memcpy(p, buf, n);
                p += n;
            }
                break;
            default:
                assert(0);
                break;
        }
        len = p - buf;
        buf[1] = len & 0xff;
        buf[2] = (len >> 8) & 0xff;
    }
    
}
static BYTE *StartMessage(enum eCommands msg, ...)
{
    static BYTE buf[4096];
    BYTE *p = buf;  
    va_list arg;
    *p++ = msg;
    *p++ = 3;
    *p++ = 0;
    va_start(arg, msg);
    RenderMessageInternal(buf, arg);
    va_end(arg);
    return buf;
}
static void ContinueMessage(BYTE *buf, ...)
{
    va_list arg;
    va_start(arg, buf);
    RenderMessageInternal(buf, arg);
    va_end(arg);
}
static void RenderMessage(enum eCommands msg, ...)
{
    static BYTE buf[4096];
    BYTE *p = buf;
    va_list arg;
    *p++ = msg;
    *p++ = 3;
    *p++ = 0;
    va_start(arg, msg);
    RenderMessageInternal(buf, arg);
    va_end(arg);
    PutRecord(buf);
}
void RenderComment(enum eCommentType tp, ...)
{
    static BYTE buf[4096];
    BYTE *p = buf;
    va_list arg;
    *p++ = ecCO;
    *p++ = 5;
    *p++ = 0;
    *p++ = tp & 0xff;
    *p++ = (tp >> 8) & 0xff;
    va_start(arg, tp);
    RenderMessageInternal(buf, arg);
    va_end(arg);
    PutRecord(buf);
}
/*-------------------------------------------------------------------------*/

void omf_dump_browsedata(BROWSEINFO *bri)
{
    BROWSEINFO **b = &browseInfo;
    while (*b)
        b = &(*b)->next;
    *b = bri;
}
void omf_dump_browsefile(BROWSEFILE *brf)
{
    browseFiles = brf;
}

/*-------------------------------------------------------------------------*/
 
void omf_globaldef(SYMBOL *sp)
{
    LIST *newItem = beGlobalAlloc(sizeof(LIST));
    newItem->data = sp;
    newItem->next = globalNames;
    globalNames = newItem ;	
}

/*-------------------------------------------------------------------------*/

void omf_put_extern(SYMBOL *sp, int code)
{
    LIST *newItem = beGlobalAlloc(sizeof(LIST));
    (void)code;
    newItem->data = sp;
    newItem->next = externNames;
    externNames = newItem ;	
}
/*-------------------------------------------------------------------------*/

void omf_put_impfunc(SYMBOL *sp, char *file)
{
    LIST *newItem = beGlobalAlloc(sizeof(LIST));
    (void)file;
    newItem->data = sp;
    newItem->next = importNames;
    importNames = newItem ;	
}

/*-------------------------------------------------------------------------*/

void omf_put_expfunc(SYMBOL *sp)
{
    LIST *newItem = beGlobalAlloc(sizeof(LIST));
    newItem->data = sp;
    newItem->next = exportNames;
    exportNames = newItem ;	
}

/*-------------------------------------------------------------------------*/

void omf_put_includelib(char *name)
{
    LIST *newItem = beGlobalAlloc(sizeof(LIST));
    newItem->data = beGlobalAlloc(strlen(name) + 1);
    strcpy(newItem->data, name);
    newItem->next = includedLibraries;
    includedLibraries = newItem ;	
}
//-------------------------------------------------------------------------

static void emit_cs(void)
{
    RenderMessage(ecCS, EBYTE, checksum + ecCS, NULL);
    checksum = 0;
}

//-------------------------------------------------------------------------

static void link_header(char *name)
{
    char buf[256];
    time_t t = time(0);
    struct tm x = *localtime(&t);
    char fpspec = 'I';
    if (anyusedfloat)
        fpspec = 'F';
    strcpy(buf, "XOCC");
    buf[0] = fpspec;
    RenderMessage(ecMB, ESTRING, buf, ESTRING, name, NULL);
    RenderComment(eConfig, EDWORD, options, NULL);
    RenderMessage(ecAD, EWORD, bitspermau, EWORD, maus, embed(bigendchar), NULL);
    sprintf(buf,"%04d%02d%02d%02d%02d%02d", x.tm_year + 1900,
        x.tm_mon+1, x.tm_mday, x.tm_hour, x.tm_min,
        x.tm_sec);
    RenderMessage(ecDT, ESTRING, buf, NULL);
}

//-------------------------------------------------------------------------

static void link_trailer(void)
{
    RenderMessage(ecME, NULL);
} 
void link_FileTime(char *file, char *buf)
{
        unsigned int time, date;
        int fd;
        if (file)
        {
            struct stat ss;
            memset(&ss,0,sizeof(ss));
            stat(file, &ss);
            if (ss.st_mtime)
            {
                struct tm x = *localtime(&ss.st_mtime);
                sprintf(buf,"%04d%02d%02d%02d%02d%02d", x.tm_year + 1900, x.tm_mon +1, x.tm_mday, x.tm_hour, x.tm_min, x.tm_sec);
            }
            else
            {
                strcpy(buf,"19800101000000");
            }
            /*
            int year, month,day, hr, min, sec;
            if (_dos_open(file, 0, &fd))
            {
                time = 0;
                date = 0;
            }
            else
            {
                _dos_getftime(fd, &date, &time);
                _dos_close(fd);
            }
            year = (date >> 9) + 1980;
            month = (date >> 5) & 15;
            day = (date & 31);
            hr = (time >> 11) & 31;
            min = (time >>5) & 63;
            sec = (time & 31) * 2;
            sprintf(buf,"%04d%02d%02d%02d%02d%02d", year, month, day, hr, min, sec);
            */
        }
        else
        {
            strcpy(buf,"19800101000000");
        }
}

//-------------------------------------------------------------------------

void link_DebugMarker(void){}
void link_LibMod(void)
{
    #ifdef XXXXX
        if (libincludes)
        {
            while (libincludes)
            {
                char buf[256],  *p;
                p = strrchr(libincludes->data, '\\');
                if (p)
                    p++;
                else
                    p = libincludes->data;

                buf[0] = 0xa3;
                buf[1] = strlen(p);
                strcpy(buf + 2, p);
                emit_record_ieee(COMENT, buf, buf[1] + 2);
                libincludes = libincludes->link;
            }
        }
    #endif 
}
void link_Files(void)
{
    if (cparams.prm_browse || cparams.prm_debug)
    {
        BOOL start = TRUE;
        BROWSEFILE *files = browseFiles;
        while (files)
        {
            char buf1[256];
            char buf[256];
            link_FileTime(files->name, buf);
            RenderComment(eSourceFile, EDWORD, files->filenum, ESTRING, files->name, ESTRING, buf, NULL);
            if (start)
            {
                emit_cs();
                start = FALSE;
            }
            files = files->next;
        }
    }
    RenderComment(eMakePass, ESTRING, "ENDMAKE", NULL);
}

//-------------------------------------------------------------------------

void link_Segs(void)
{
    char buf[512];
    int i;
    VIRTUAL_LIST *v = virtualFirst;
    firstVirtualSeg = 1;
    for (i = 1; i < MAX_SEGS; i++)
    {
        if (segxlattab[i])
        {
            RenderMessage(ecST, EDWORD, segxlattab[i], EDWORD, segcharsbin[i], 
                          ESTRING, segnames[i], NULL);
            RenderMessage(ecSA, EDWORD, segxlattab[i], EDWORD, segAligns[i], NULL);
            RenderMessage(ecAS, embed('S'), EDWORD, segxlattab[i], embed('V'), EDWORD, segs[i].curlast, embed(0x1b), NULL);
            firstVirtualSeg++;
        }
    }
    i = firstVirtualSeg;
    while (v)
    {
        v->sp->value.i = i;
        if (v->data)
        {
            strcpy(buf,"vsd_");
        }
        else
        {
            strcpy(buf,"vsc_");
        }
        beDecorateSymName(buf + 4, v->sp);
        RenderMessage(ecST, EDWORD, i, EDWORD, (v->data ? segcharsbin[dataseg] : segcharsbin[codeseg]) | SECT_EQUAL,
                      ESTRING, buf, NULL);
        RenderMessage(ecSA, EDWORD, i, EDWORD, 4, NULL);
        RenderMessage(ecAS, embed('S'), EDWORD, i++, embed('V'), EDWORD, v->seg->curlast, embed(0x1b), NULL);
        v = v->next;
    }
}
#define ROTL(x, v) (((x) << v) | ((x) >> (32 - v)))
static int typehash(BYTE *name)
{
    int len = name[1] + (name[2] << 8);
    unsigned rv = 0;
    BYTE *pb = name;
    while (len--)
    {
        rv = ROTL(rv, 2) ^ (*pb++ | 2);
    }
    return rv % TYPE_HASH_SIZE;
}
static BYTE *copybuf(BYTE *buf)
{
    int l =buf[1] + (buf[2] <<8);
    BYTE *rv = malloc(l);
    memcpy(rv, buf, l);
    return rv;
}
static int lookuptype(char *buf)
{
    int n = typehash(buf);
    struct _type_hash *t = typeHashTab[n];
    while (t)
    {
        if (!strcmp(t->name, buf))
            return t->id;
        t = t->next;
    }
    t = Alloc(sizeof(struct _type_hash));
    t->id = typeIndex++;
    t->name = copybuf(buf);
    t->next = typeHashTab[n];
    typeHashTab[n] = t;
    return -t->id;
}
static int FinishType(BYTE *buf)
{
    int n = lookuptype(buf);
    if (n < 0)
    {
        // patch the type num then send it...
        n = - n;
        buf[4] = n & 0xff; 
        buf[5] = (n >> 8) & 0xff; 
        buf[6] = (n >> 16) & 0xff; 
        buf[7] = (n >> 24) & 0xff; 
        PutRecord(buf);
    }
    return n;
}    
int link_BasicType(TYPE *tp)
{
    static int basicTypes[] = 
    {
        35,34,40,48,41,46,49,45,0,42,47,50,43,51,44,52,72,73,74,80,81,82,88,89,90,32
    } ;
    static int pointedTypes[] =
    {
        0,0,40+16,48+16,41+16,46+16,49+16,45+16,0,42+16,47+16,50+16,43+16,51+16,44+16,52+16,72+16,73+16,74+16,80+16,81+16,82+16,88+16,89+16,90+16,33
    };
    int n = 0;
    TYPE *tp1 = basetype(tp);
    if( tp1->type <= bt_void)
    {
        n = basicTypes[tp1->type];
    }        
    else if (tp1->type == bt_pointer && !tp1->array && !tp1->vla && !tp1->bits)
    {
        TYPE *tp2 = basetype(tp1->btp);
        if (tp2->type <= bt_void)
        {
            n = pointedTypes[tp2->type];
        }
    }
    return n;
}
int link_puttype(TYPE *tp);
void dumpStructFields(int sel, int n, int sz, HASHREC *hr)
{
    BYTE *msg;
    char buf[512];
    int count = 0;
    SYMBOL *table[15];
    int table1[15];
    int last = 0;
    int i;
    while (hr && count < sizeof(table)/sizeof(table[0]))
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        table[count] = sp;
        table1[count++] = link_puttype(sp->tp);
        hr = hr->next;
    }
    if (hr)
        dumpStructFields(9, last = typeIndex++, sz, hr);
    msg = StartMessage(ecAT, embed('T'), EDWORD, n , EDWORD, sel, NULL);
    if (sel != 9)
        ContinueMessage(msg, EDWORD, sz, NULL);
    for (i=0; i < count; i++)
        ContinueMessage(msg, EDWORD, table1[i], ESTRING, table[i]->name, EDWORD, table[i]->offset, NULL);
    
    if (last != 0)
        ContinueMessage(msg, EDWORD, last, NULL);
    PutRecord(msg);
}
void dumpEnumFields(int sel, int n, int baseType, int sz, HASHREC *hr)
{
    BYTE *msg;
    char buf[512];
    int count = 0;
    SYMBOL * table[15];
    int table1[15];
    int last = 0;
    int i;
    while (hr && count < sizeof(table)/sizeof(table[0]))
    {
        SYMBOL *sp = (SYMBOL *)hr->p;
        table[count] = sp;
        table1[count++] = sp->value.i;
        hr = hr->next;
    }
    if (hr)
        dumpEnumFields(9, last = typeIndex++, baseType, sz, hr);
    msg = StartMessage(ecAT, embed('T'), EDWORD, n , EDWORD, sel, NULL);
    if (sel != 9)
        ContinueMessage(msg, EDWORD, sz, NULL);
        
    for (i=0; i < count; i++)
        ContinueMessage(msg, EDWORD, baseType, ESTRING, table[i]->name, EDWORD, table1[i], NULL);
    
    if (last != 0)
        ContinueMessage(msg, EDWORD, last, NULL);
    PutRecord(msg);
}
int dumpFunction(TYPE *tp)
{
    BYTE *msg;
    char *bptr;
    HASHREC *hr;
    int n;
    int m = link_puttype(basetype(tp)->btp);
    int v,i;
    int types[32];
    int count = 0;
    hr = basetype(tp)->syms->table[0];
    while (hr && count < sizeof(types)/sizeof(types[0]))
    {
        SYMBOL *s = hr->p;
        types[count++] = link_puttype(s->tp);
        hr = hr->next;
    }
    switch (basetype(tp)->sp->linkage)
    {
        default:
        case lk_cdecl:
            v = 1;
            break;
        case lk_stdcall:
            v = 2;
            break;
         case lk_pascal:
            v = 3;
            break;
    }
    msg = StartMessage(ecAT, embed('T'), EDWORD, 0, EDWORD, 2, EDWORD, m, EDWORD, v, NULL);
    for (i=0; i < count; i++)
    {
        ContinueMessage(msg, EDWORD, types[i], NULL);
        bptr += strlen(bptr);
    }
    return FinishType(msg);
}
void link_extendedtype(TYPE *tp1)
{
    TYPE *tp = basetype(tp1);
    if (tp->type == bt_pointer)
    {
        if (tp->vla)
        {
            int m = link_puttype(tp->btp);
            BYTE *msg = StartMessage(ecAT, embed('T'), EDWORD, 0, EDWORD, 7, EDWORD, tp->size, EDWORD, m, EDWORD, 0x2a, NULL);
            tp1->dbgindex = FinishType(msg);
        }
        else if (tp->array)
        {
            int m = link_puttype(tp->btp);
            BYTE *msg = StartMessage(ecAT, embed('T'), EDWORD, 0, EDWORD, 6, EDWORD, tp->size, EDWORD, m, EDWORD, 0x2a,
                                      EDWORD, 0, EDWORD, tp->size/basetype(tp)->btp->size, NULL);
            tp1->dbgindex = FinishType(msg);
        }
        else
        {
            int m = link_puttype(tp->btp);
            BYTE *msg = StartMessage(ecAT, embed('T'), EDWORD, 0, EDWORD, 1, EDWORD, tp->size, EDWORD, m, NULL);
            tp1->dbgindex = FinishType(msg);
        }
        
    }
    else if (isfunction(tp))
    {
        tp1->dbgindex = dumpFunction(tp);
    }
    else 
    {
        int n = tp1->dbgindex = typeIndex++;
        if (tp->hasbits)
        {
            int m = link_BasicType(tp);
            RenderMessage(ecAT, embed('T'), EDWORD, n, EDWORD, 3,
                                     EBYTE, tp->size, EDWORD, m, 
                                     EBYTE, tp->startbit, EBYTE, tp->bits, NULL);
        }
        else if (isstructured(tp))
        {
            int sel;
            if (tp->type == bt_union)
            {
                sel = 5;
            }
            else
            {
                sel = 4;
            }
            if (tp->syms)
                dumpStructFields(sel, n, tp->size, tp->syms->table[0]);
            else
                dumpStructFields(sel, n, tp->size, NULL);
            RenderMessage(ecNAME, embed('T'), EDWORD, n, ESTRING, tp->sp->name, NULL);
        }
        else if (tp->type == bt_ellipse)
        {
           // ellipse results in no debug info
        }
        else // enum
        {
            int m;
            if (tp->type == bt_enum)
                if (tp->btp)
                    m  = link_BasicType(tp->btp);
                else
                    m = 42;
            else
                m = link_BasicType(tp);
    
            if (tp->syms)
                dumpEnumFields(8, n, m, tp->size, tp->syms->table[0]);
            else
                dumpEnumFields(8, n, m, tp->size, NULL);
            RenderMessage(ecNAME, embed('T'), EDWORD, n, ESTRING, tp->sp->name, NULL);
        }
    }
}
int link_puttype(TYPE *tp)
{
    if (!tp->dbgindex)
    {
        if (tp->type == bt_typedef)
        {
            if (!tp->btp->dbgindex)
                link_puttype(tp->btp);
            if (tp->btp->dbgindex)
            {
                tp->dbgindex = typeIndex++;
                RenderMessage(ecAT, embed('T'), EDWORD, tp->dbgindex, EDWORD, 0xa, EDWORD, tp->btp->dbgindex, NULL);
                RenderMessage(ecNAME, embed('T'), EDWORD, tp->dbgindex, ESTRING, tp->sp->name, NULL);
            }
        }
        else
        {
            if (!tp->bits)
                tp->dbgindex = link_BasicType(tp);
            if (tp->dbgindex == 0)
            {
                link_extendedtype(tp);
            }
        }
    }
    return tp->dbgindex;
}
void debug_outputtypedef(SYMBOL *sp) 
{
    LIST *s = Alloc(sizeof(LIST));
    s->data = sp;
    s->next = dbgTypeDefs;
    dbgTypeDefs = s;
}
//-------------------------------------------------------------------------
void link_types()
{
    if (cparams.prm_debug)
    {
        LIST *lf = globalNames;
        VIRTUAL_LIST *v = virtualFirst;
        int i;
        while (lf)
        {
            SYMBOL *sp = lf->data;
            if (sp->storage_class == sc_global && sp->linkage != lk_inline && sp->tp->used)
            {
                link_puttype(sp->tp);
            }
            lf = lf->next;
        }
        lf = globalNames;
        while (lf)
        {
            SYMBOL *sp = lf->data;
            if (sp->storage_class == sc_static && sp->tp->used)
            {
                link_puttype(sp->tp);
            }
            lf = lf->next;
        }
        for (i = 1; i < MAX_SEGS; i++)
            if (segxlattab[i] && segs[i].curlast && segs[i].attriblist)
            {
                ATTRIBDATA *ad = segs[i].attriblist;
                while(ad)
                {
                    if (ad->type == e_ad_vardata && ad->v.sp->tp->used)
                    {
                        link_puttype(ad->v.sp->tp);
                    }
                    ad = ad->next;
                }
            }
        while (v)
        {
            if (v->seg->attriblist)
            {
                ATTRIBDATA *ad = v->seg->attriblist;
                while(ad)
                {
                    if (ad->type == e_ad_vardata && ad->v.sp->tp->used)
                    {
                        link_puttype(ad->v.sp->tp);
                    }
                    ad = ad->next;
                }
            }
        }
        while (dbgTypeDefs)
        {
            SYMBOL *sp = (SYMBOL *)dbgTypeDefs->data;
            sp->tp->dbgindex = link_puttype(sp->tp); 
            if (sp->tp->dbgindex < 1024 && sp->tp->used)
            {
                int n = sp->tp->dbgindex;
                sp->tp->dbgindex = typeIndex++;
                RenderMessage(ecAT, embed('T'), EDWORD, sp->tp->dbgindex, EDWORD, 0xa, EDWORD, n, NULL);
            }
            RenderMessage(ecNAME, embed('T'), EDWORD, sp->tp->dbgindex, ESTRING, sp->name, NULL);
            dbgTypeDefs = dbgTypeDefs->next;
        }
        emit_cs();
    }
}
//-------------------------------------------------------------------------

void link_putext(SYMBOL *sp)
{

    char buf[512];
    beDecorateSymName(buf, sp);
    sp->value.i = extIndex++;
    RenderMessage(ecNAME, embed('X'), EDWORD, (int)sp->value.i, ESTRING, buf, NULL);
    #ifdef XXXXX
        if (prm_debug)
            if (r->datatype &TY_TYPE)
                emit_record_ieee("ATX%X,T%lX.\r\n", r->id, r->datatype &~TY_TYPE);
            else
                if (r->datatype &TY_NAME)
                    emit_record_ieee("ATX%X,I%lX.\r\n", r->id, r->datatype &~TY_NAME)
                        ;
                else
                    emit_record_ieee("ATX%X,%lX.\r\n", r->id, r->datatype);
        q = q->link;
    }
#endif 
}

//-------------------------------------------------------------------------

void link_ExtDefs(void)
{
    SYMBOL *sp;
    VIRTUAL_LIST *v = virtualFirst;
    LIST *lf = externNames;
    extIndex = 1;
//    while (v)
//    {
//        v->sp->value.i = extIndex++ + 0xc000;
//        v = v->next;
//    }
    while (lf)
    {
        sp = lf->data;
        link_putext(sp);
        lf = lf->next;
    }

}

//-------------------------------------------------------------------------

void link_putimport(SYMBOL *sp)
{
    char buf[512], buf1[512];
    int l;
    beDecorateSymName(buf, sp);
    l = strlen(buf);
    RenderComment(eImport, embed('N'), ESTRING, buf, ESTRING, buf, ESTRING, sp->importfile, NULL);
}

//-------------------------------------------------------------------------

void link_Imports(void)
{
    SYMBOL *sp;
    LIST *lf = importNames;
    while (lf)
    {
        sp = lf->data;
        if (sp->linkage2 == lk_import && sp->importfile)
        {
            link_putimport(sp);
        }
        lf = lf->next;
    }
}

//-------------------------------------------------------------------------
BOOL isbasevolatile(TYPE *tp)
{
    TYPE *tp1 = tp;
    while (TRUE)
    {
        TYPE *tp2 = basetype(tp1);
        if (tp2->type == bt_pointer)
            tp1 = tp2->btp;
        else
            break;
        
    }
    return isvolatile(tp1);
}

int link_getseg(SYMBOL *sp)
{
    if (!sp->tp)
        return dataseg;
    switch (sp->storage_class)
    {
    /*
        case sc_member:
            if (sp->value.classdata.gdeclare)
                sp = sp->value.classdata.gdeclare;
            else
                if (sp->tp->type != bt_func && sp->tp->type != bt_ifunc)
                    diag("link_getseg - no static member initializer");
    */
            /* fall through*/

        case sc_member:
        case sc_virtual:
        case sc_static:
        case sc_global:
        case sc_external:
        case sc_localstatic:
        case sc_overloads:
            if (sp->linkage == lk_inline)
                return sp->value.i | 0xc0000000;
            if (isfunction(sp->tp))
/*				
                if (sp->gennedvirtfunc)
                    return sp->value.i;
                else
*/
                    return codeseg;
            /* fall through */
            if (IsConstWithArr(sp->tp) && !isbasevolatile(sp->tp))
                return constseg;
            else if (sp->linkage3 == lk_threadlocal)
                return tlsseg;
            else
                if (sp->init || !cparams.prm_bss)
                    return dataseg;
                else
                    return bssxseg;
        case sc_label:
            if (sp->linkage == lk_inline)
                return sp->value.i | 0xc0000000;
            else
                return codeseg;
                
        case sc_type:
        case sc_typedef:
//            if (sp->gennedvirtfunc)
//            {
                /* vtab & xt*/
//                return sp->value.i;
//            }
        default:
            diag("Unknown segment type in link_getseg");
            return codeseg;
    }
    /* also fix the value.i field of local funcs...*/
}

//-------------------------------------------------------------------------

void link_putpub(SYMBOL *sp, char sel)
{
    int l = 0;
    char buf[512], obuf[512];
    int index;
    /*
    if (sp->gennedvirtfunc)
        return ;
    if (sp->value.classdata.templatedata)
        return ;
        */
    if (sp->linkage == lk_inline)
        return ;
    if (isconst(sp->tp) && isint(sp->tp) && sp
        ->storage_class == sc_static)
        return ;
    if (sp->tp->type == bt_ellipse)
        return ;
    if (sel == 'I')
        index = pubIndex++;
    else if (sel == 'N')
        index = localIndex++;
    else
        index = autoIndex++;
    sp->value.i = index;
    beDecorateSymName(buf, sp);
    RenderMessage(ecNAME, embed(sel), EDWORD, index, ESTRING, buf, NULL);
    if (sel == 'A')
    {
        RenderMessage(ecAS, embed(sel), EDWORD, index, embed('V'), EDWORD, sp->offset, embed(0x1b), NULL);
    }
    else
    {
        int seg = link_getseg(sp);
        if (seg & 0xc0000000)
            seg &= 0xffffff;
        else
            seg = segxlattab[seg];
        RenderMessage(ecAS, embed(sel), EDWORD, index, embed('R'), EDWORD, seg, embed('V'), EDWORD, sp->offset, embed('+'), embed(0x1b), NULL);
    }
    if (cparams.prm_debug)
    {
        RenderMessage(ecAT, embed(sel), EDWORD, index, EDWORD, sp->tp->dbgindex, NULL);
    }
}

//-------------------------------------------------------------------------

void link_Publics(void)
{
    LIST *lf = globalNames;
    while (lf)
    {
        SYMBOL *sp = lf->data;
        if (sp->storage_class == sc_global && sp->linkage != lk_inline)
        {
            link_putpub(sp, 'I');
        }
        lf = lf->next;
    }
    if (cparams.prm_debug)
    {
        lf = globalNames;
        while (lf)
        {
            SYMBOL *sp = lf->data;
            if (sp->storage_class == sc_static)
            {
                link_putpub(sp, 'N');
            }
            lf = lf->next;
        }
    }
}
void link_Autos(void)
{
    if (cparams.prm_debug)
    {
        VIRTUAL_LIST *v = virtualFirst;
        int i;
        for (i = 1; i < MAX_SEGS; i++)
            if (segxlattab[i] && segs[i].curlast && segs[i].attriblist)
            {
                ATTRIBDATA *ad = segs[i].attriblist;
                while(ad)
                {
                    if (ad->type == e_ad_vardata)
                    {
                        link_putpub(ad->v.sp, 'A');
                    }
                    ad = ad->next;
                }
            }
        while (v)
        {
            if (v->seg->attriblist)
            {
                ATTRIBDATA *ad = v->seg->attriblist;
                while(ad)
                {
                    if (ad->type == e_ad_vardata)
                    {
                        link_putpub(ad->v.sp, 'A');
                    }
                    ad = ad->next;
                }
            }
        }
        emit_cs();
    }    
}
//-------------------------------------------------------------------------

void link_putexport(SYMBOL *sp)
{
    char buf[512], buf1[512];
    int l;
    beDecorateSymName(buf, sp);
    l = strlen(buf);
    RenderComment(eExport, embed('N'), ESTRING, buf, ESTRING, buf, NULL);
}

//-------------------------------------------------------------------------

void link_Exports(void)
{
    SYMBOL *sp;
    LIST *lf = exportNames;
    while (lf)
    {
        sp = lf->data;
        if (sp->storage_class == sc_global && sp->linkage2 == lk_export && 
            sp->linkage != lk_inline)
        {
            link_putexport(sp);
        }
        lf = lf->next;
    }
}

//-------------------------------------------------------------------------

void link_PassSeperator(void)
{
    RenderComment(eLinkPass, ESTRING, "ENDSYM", NULL);
}

//-------------------------------------------------------------------------

void link_Fixups(BYTE  *msg, FIXUP *fixup, EMIT_LIST *rec, int curseg, int offs)
{
    int iseg, xseg;
    int rel = FALSE;
    switch (fixup->fmode)
    {
        case fm_relsymbol:
            rel = TRUE;
        case fm_symbol:
            if (fixup->sym->storage_class == sc_absolute)
            {
                if (rel)
                {
                    ContinueMessage(msg, EDWORD, 0, NULL);
                    diag("Relative absolute in link_Fixups");
                }
                else
                    ContinueMessage(msg, EDWORD, fixup->sym->offset, NULL);
                return ;
            }
            iseg = link_getseg(fixup->sym);
            if (iseg & 0xc0000000)
                xseg = iseg;
            else
                xseg = segxlattab[iseg];
            {
                SYMBOL *sp = fixup->sym;
                    /*
                if (sp->mainsym)
                    sp = sp->mainsym;
                if (sp->storage_class == sc_member && sp
                    ->value.classdata.gdeclare && sp->value.classdata.gdeclare
                    ->storage_class == sc_global)
                    sp = sp->value.classdata.gdeclare;
                    */
                if (sp->storage_class == sc_global || sp->storage_class ==
                    sc_static || sp->storage_class == sc_localstatic || sp->storage_class == sc_overloads)
                {
                    if (rel)
                    {
                    /*
                        if (iseg == curseg)
                        {
                            sprintf(buf, "%X", sp->offset - fixup
                                ->address - 4);
                        }
                        else
                    */
                        {
                            ContinueMessage(msg, embed('R'), EDWORD, xseg, embed('V'), EDWORD, sp->offset + offs,
                                            embed('+'), embed('V'), EDWORD, 4, embed('-'), embed('P'), embed('-'), NULL);
                        }
                    }
                    else
                    {
                        ContinueMessage(msg, embed('R'), EDWORD, xseg, embed('V'), EDWORD, sp->offset + offs,
                                        embed('+'), NULL);
                    }
                    /* segment relative */
                }
                else
                {
                    if (rel)
                    {
                        ContinueMessage(msg, embed('X'), EDWORD, (int)sp->value.i,
                                        embed('V'), EDWORD, 4, embed('-'), embed('P'), embed('-'), NULL);
                    }
                    else
                    {
                        ContinueMessage(msg, embed('X'), EDWORD, (int)sp->value.i, embed('V'), EDWORD, offs, embed('+'), NULL);
                    }
                    /* extdef relative */
                }
            }
            break;
        case fm_threadlocal:
            iseg = link_getseg(fixup->sym);
            if (iseg & 0xc0000000)
                xseg = iseg;
            else
                xseg = segxlattab[iseg];
            {
                SYMBOL *sp = fixup->sym;
                if (sp->storage_class == sc_global || sp->storage_class ==
                    sc_static || sp->storage_class == sc_localstatic || sp->storage_class == sc_overloads)
                {
                    ContinueMessage(msg, embed('R'), EDWORD, xseg, embed('V'), EDWORD, sp->offset + offs,
                                    embed('+'), NULL);
                }
                else
                {
                    ContinueMessage(msg, embed('X'), EDWORD, (int)sp->value.i, NULL);
                }
                sp = fixup->base;
                ContinueMessage(msg, embed('X'), EDWORD, (int)sp->value.i, embed('-'), NULL);
            }
            break;
        case fm_rellabel:
            rel = TRUE;
        case fm_label:
            iseg = LabelSeg(fixup->label);
            xseg = segxlattab[iseg];
            if (rel)
            {
            /*
                if (iseg == curseg)
                    sprintf(buf, "%X", LabelAddress(fixup->label) 
                        - fixup->address - 4);
                else
            */
                {
                    ContinueMessage(msg, embed('R'), EDWORD, xseg, 
                                    embed('V'), EDWORD, LabelAddress(fixup->label) + offs,
                                    embed('+'), embed('V'), EDWORD, 4, embed('-'), embed('P'), embed('-'), NULL);
                }
            }
            else
            {
                ContinueMessage(msg, embed('R'), EDWORD, segxlattab[iseg], 
                                embed('V'), EDWORD, LabelAddress(fixup->label) + offs,
                                embed('+'), NULL);
            }
            /* segment relative */
            break;
    }
}

//-------------------------------------------------------------------------

static long putlr(FIXUP *p, EMIT_LIST *s, int curseg, int offs)
{
    BYTE *msg = StartMessage(ecLR, NULL);
    link_Fixups(msg, p, s, curseg, offs);
    ContinueMessage(msg, EBYTE, 0x1b, NULL);
    ContinueMessage(msg, EBYTE, 4, NULL);
    PutRecord(msg);
    return (long)(4);
}

//-------------------------------------------------------------------------

static long putld(long start, long end, unsigned char *buf)
{
    long count = 0;
    int i;
    if (start == end)
        return (start);
    while (end - start >= LDPERLINE)
    {
        for (i=0; i < 16; i++)
            if (buf[count + i] != buf[count])
                break;
        if (i < 16)
        {
            RenderMessage(ecLD, EBUF, buf + count, LDPERLINE, NULL);
            count += LDPERLINE; 
            start += LDPERLINE;
        }
        else
        {
            for (i=0; i < end - start; i++)
            {
                if (buf[count + i] != buf[count])
                    break;
            }
            RenderMessage(ecLE, EDWORD, i, EBYTE, buf[count], NULL);
            count += i;
            start += i;
        }
    }
    if (start == end)
        return (start);
    while (start < end)
    {
        for (i=0; i < 16 && i < end - start; i++)
            if (buf[count + i] != buf[count])
                break;
        if (i < 16)
        {
            RenderMessage(ecLD, EBUF, buf + count, end-start, NULL);
            start += end - start;
        }
        else
        {
            for (i=0; i < end - start; i++)
            {
                if (buf[count + i] != buf[count])
                    break;
            }
            RenderMessage(ecLE, EDWORD, i, EBYTE, buf[count], NULL);
            start += i;
        }
    }
    return (start);
}

static void putattribs(EMIT_TAB *seg, int addr)
{
    while (seg->attriblist && seg->attriblist->address <= addr)
    {
        if (cparams.prm_debug)
        {
            char buf1[256];
            switch(seg->attriblist->type)
            {
                BYTE *msg;
                case e_ad_funcdata:
                    if (seg->attriblist->start)
                        if (seg->attriblist->v.sp->storage_class == sc_global)
                            RenderComment(eFunctionStart, embed('I'), EDWORD, (int)seg->attriblist->v.sp->value.i, NULL);
                        else
                            RenderComment(eFunctionStart, embed('N'), EDWORD, (int)seg->attriblist->v.sp->value.i, NULL);
                    else
                        if (seg->attriblist->v.sp->storage_class == sc_global)
                            RenderComment(eFunctionEnd, embed('I'), EDWORD, (int)seg->attriblist->v.sp->value.i, NULL);
                        else
                            RenderComment(eFunctionEnd, embed('N'), EDWORD, (int)seg->attriblist->v.sp->value.i, NULL);
                    break;
                case e_ad_blockdata:
                    if (seg->attriblist->start)
                    {
                        RenderComment(eBlockStart, ESTRING, "", NULL);
                    }
                    else
                    {
                        RenderComment(eBlockEnd, ESTRING, "", NULL);
                    }
                    break;
                case e_ad_vardata:
                    if (seg->attriblist->v.sp->tp->type != bt_ellipse)
                        RenderComment(eVar, embed('A'), EDWORD, (int)seg->attriblist->v.sp->value.i, NULL);
                    break;
                case e_ad_linedata:
                    RenderComment(eLineNo, EDWORD, seg->attriblist->v.ld->fileindex, 
                                  EDWORD, seg->attriblist->v.ld->lineno,
                                  NULL);
                    break;
            }
        }
        seg->attriblist = seg->attriblist->next;
    }
}
//-------------------------------------------------------------------------

void link_Data(void)
{
    char buf[1100];
    int i;
    VIRTUAL_LIST *v = virtualFirst;
    for (i = 1; i < MAX_SEGS; i++)
        if (segxlattab[i] && segs[i].curlast)
        {
            EMIT_LIST *rec = segs[i].first;
            RenderMessage(ecSB, EDWORD, segxlattab[i], NULL);
            while (rec)
            {
                int j = 0;
                int len, len1;
                long size = segs[i].curlast;
                FIXUP *f = rec->fixups;
                while (j < rec->filled)
                {
                    int mode;
                    int alen;
                    if (f)
                        len = f->address - rec->address - j;
                    else
                        len = rec->filled - j;
                    if (segs[i].attriblist)
                        len1 = segs[i].attriblist->address - rec->address - j;
                    else
                        len1 = rec->filled - j;
                        
                    if (len1 == len)
                    {
                        alen = len;
                    }
                    else if (len1 < len)
                    {
                        alen = len1;
                    }
                    else
                    {
                        alen = len;
                    }
                   
                    putld(j, alen + j, rec->data + j);
                    if (alen == len1)
                    {
                        putattribs(&segs[i], rec->address + j);
                    }
                    j += alen;
                    if (f && alen == len)
                    {
                        j += putlr(f, rec, i, (*(unsigned *)(rec->data + j)));
                        f = f->next;
                    }
                }
                putattribs(&segs[i], INT_MAX);
                rec = rec->next;
            }
            emit_cs();
        }
    i = firstVirtualSeg;
    while (v)
    {
        EMIT_LIST *rec = v->seg->first;
        RenderMessage(ecSB, EDWORD, i, NULL);
        while (rec)
        {
            int j = 0;
            int len, len1;
            long size = v->seg->curlast;
            FIXUP *f = rec->fixups;
            while (j < rec->filled)
            {
                int mode;
                int alen;
                if (f)
                    len = f->address - rec->address - j;
                else
                    len = rec->filled - j;
                if (v->seg->attriblist)
                    len1 = v->seg->attriblist->address - rec->address - j;
                else
                    len1 = rec->filled - j;
                if (len1 == len)
                {
                    alen = len;
                }
                else if (len1 < len)
                {
                    alen = len1;
                }
                else
                {
                    alen = len;
                }
                putld(j, len + j, rec->data + j);
                if (alen == len1)
                    putattribs(v->seg, rec->address + j);
                j += len;
                if (f && alen == len)
                {
                    j += putlr(f, rec, i, (*(unsigned *)(rec->data + j)));
                    f = f->next;
                }
            }
            rec = rec->next;
        }
        putattribs(v->seg, INT_MAX);
        emit_cs();
        i++;
        v = v->next;
    }
}

void link_BrowseInfo(void)
{
    if (browseInfo && cparams.prm_browse)
    {
        RenderComment(eBrowsePass, ESTRING, "ENDDATA", NULL);
        while (browseInfo)
        {
            char buf1[256];
            RenderComment(eBrowseInfo, EDWORD, browseInfo->type,
                             EDWORD, browseInfo->flags, 
                             EDWORD, browseInfo->filenum,
                             EDWORD, browseInfo->lineno,
                             EWORD, browseInfo->charpos, 
                             ESTRING, browseInfo->name);   
            browseInfo = browseInfo->next;
        }
    }
}


//-------------------------------------------------------------------------


//-------------------------------------------------------------------------

void output_obj_file(void)
{
    char name[260];
    LIST *l = incfiles;
    int i, pos = 1;
    extIndex = pubIndex = localIndex = autoIndex =1;
    typeIndex = 1024;
    checksum = 0;
    for (i = 1; i < MAX_SEGS; i++)
    {
        if (segs[i].curlast)
        {
            segxlattab[i] = pos++;
        }
        else
            segxlattab[i] = 0;
    }
    link_header(fullqualify(includedFiles->data));
    link_Files();
    emit_cs();
    link_Segs();
    emit_cs();
    link_types();
    link_ExtDefs();
    link_Publics();
    link_Autos();
    link_Exports();
    link_Imports();
    emit_cs();
    link_PassSeperator();
    link_Data();
    emit_cs();
    link_trailer();
    
    if (cparams.prm_browse)
    {
        browsing = TRUE;
        extIndex = pubIndex = localIndex = autoIndex =1;
        typeIndex = 1024;
        checksum = 0;
        link_header(fullqualify(includedFiles->data));
        link_Files();
        emit_cs();
        link_BrowseInfo();
        emit_cs();
        link_trailer();
        browsing = FALSE;
    }
}
