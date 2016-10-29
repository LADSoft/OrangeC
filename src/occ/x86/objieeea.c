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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dos.h>
#include <stdarg.h>
#include "be.h"
#include <time.h>

#define DEBUG_VERSION 4.0

#define LDPERLINE 32
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

static BROWSEINFO *browseInfo, *browseInfoTail;
static BROWSEFILE *browseFiles;

char *segnames[] = 
{
    0, "code", "data", "bss", "string", "const", "tls", "cstartup", "crundown", 
        "tlsstartup", "tlsrundown", "codefix", "datafix", 
        "lines", "types", "symbols", "browse"
};
char *segclasses[] = 
{
    0, "code", "data", "bss", "string", "const", "tls", "cstartup", "crundown", 
        "tlsstartup", "tlsrundown", "codefix", "datafix", 
        "lines", "types", "symbols", "browse"
};
int segAligns[] =
{
    1, 2, 8, 8, 2, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
} ;
static char *segchars[] = 
{
    0, "R", "W", "W", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R", "R"
};
#define TYPE_HASH_SIZE 128
struct _type_hash {
    struct _type_hash *next;
    int id;
    char *name;
} *typeHashTab[TYPE_HASH_SIZE];

int link_puttype(TYPE *tp);
void dbginit(void) 
{ 
    dbgTypeDefs = NULL;
    memset(typeHashTab, 0, sizeof(typeHashTab));
}
void omfInit(void)
{
    includedLibraries = 0;
    externNames = 0;
    globalNames = 0;
    exportNames = 0;
    importNames = 0;
    browseInfo = browseInfoTail = NULL;
    browseFiles = NULL;
}
static void emit_record_ieee(char *format, ...)
{
    char buffer[4096];
    int i, l;
    va_list ap;

    va_start(ap, format);
    vsprintf(buffer, format, ap);
    l = strlen(buffer);
    for (i = 0; i < l; i++)
        if (buffer[i] > 31)
            checksum += buffer[i];
    va_end(ap);
    oprintf(browsing ? browseFile : outputFile, "%s", buffer);
}
/*-------------------------------------------------------------------------*/

void omf_dump_browsedata(BROWSEINFO *bri)
{
    if (browseInfo)
        browseInfoTail = browseInfoTail->next = bri;
    else
        browseInfoTail = browseInfo = bri;
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

static void emit_cs(int toclear)
{
    if (toclear)
    {
        emit_record_ieee("CS.\r\n");
    }
    else
    {
        checksum += 'C';
        checksum += 'S';
        emit_record_ieee("CS%02X.\r\n", checksum &127);
    }
    checksum = 0;
}

//-------------------------------------------------------------------------

static void link_header(char *name)
{
    time_t t = time(0);
    struct tm x = *localtime(&t);
    char fpspec = 'I';
    if (anyusedfloat)
        fpspec = 'F';
    emit_record_ieee("MB%cCC386,%03X%s.\r\n", fpspec, strlen(name), name);
    emit_record_ieee("CO104,008%08lX.\r\n", options);
    emit_record_ieee("AD%x,%x,%c.\r\n", bitspermau, maus, bigendchar);
    emit_record_ieee("DT%04d%02d%02d%02d%02d%02d.\r\n", x.tm_year + 1900,
        x.tm_mon+1, x.tm_mday, x.tm_hour, x.tm_min,
        x.tm_sec);
}

//-------------------------------------------------------------------------

static void link_trailer(void)
{
    emit_record_ieee("ME.\r\n");
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
        BOOLEAN start = TRUE;
        BROWSEFILE *files = browseFiles;
        while (files)
        {
            char buf1[256];
            char buf[256];
            link_FileTime(files->name, buf);
            sprintf(buf1, "%d,%03X%s,%s",files->filenum, strlen(files->name), files->name, buf);
            emit_record_ieee("CO300,%03X%s.\r\n",strlen(buf1),buf1);
            
            if (start)
            {
                emit_cs(FALSE);
                start = FALSE;
            }
            files = files->next;
        }
    }
    emit_record_ieee("CO100,007ENDMAKE.\r\n");
}

//-------------------------------------------------------------------------

void link_Segs(void)
{
    char buf[4096];
    int i;
    VIRTUAL_LIST *v = virtualFirst;
    firstVirtualSeg = 1;
    for (i = 1; i < MAX_SEGS; i++)
    {
        if (segxlattab[i])
        {
            emit_record_ieee("ST%X,%s,%03X%s.\r\n", segxlattab[i], segchars[i],
                strlen(segnames[i]), segnames[i]);
            emit_record_ieee("SA%X,%x.\r\n", segxlattab[i], segAligns[i]);
            emit_record_ieee("ASS%X,%lX.\r\n", segxlattab[i], segs[i].curlast);
            firstVirtualSeg++;
        }
    }
    while (v)
    {
        i = v->sp->value.i;
        if (v->data)
        {
            strcpy(buf,"vsd");
        }
        else
        {
            strcpy(buf,"vsc");
        }
        beDecorateSymName(buf + 3, v->sp);
        emit_record_ieee("ST%X,%s,M,V,%03X%s.\r\n", i, v->data ? segchars[dataseg]:
            segchars[codeseg], strlen(buf), buf);
        emit_record_ieee("SA%X,%x.\r\n", i, 4);
        emit_record_ieee("ASS%X,%lX.\r\n", i++, v->seg->curlast);
        v = v->next;
    }
}
static int typehash(char *name)
{
    unsigned i = 0;
    while (*name)
        i = ((i << 7) + (i << 1) +i )  ^ *name++;
    return i % TYPE_HASH_SIZE;
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
    t->name = litlate(buf);
    t->next = typeHashTab[n];
    typeHashTab[n] = t;
    return -t->id;
}
int emit_type_ieee(char *format, ...)
{
    int n;
    char buf[512];
    va_list a;
    va_start(a, format);
    vsprintf(buf, format, a);
    va_end(a);
    n = lookuptype(buf);
    if (n < 0)
    {
        n = -n;
        emit_record_ieee("ATT%X,%s.\r\n", n, buf);
    }
    return n;
}
void emit_type_name(int n, char *nm)
{
    if (nm[0] == '_')
        nm++; 
    emit_record_ieee("NT%X,%03X%s.\r\n", n, strlen(nm), nm);
}
int link_BasicType(TYPE *tp)
{
    static int basicTypes[] = 
    {
        35,34,40,40,48,41,46,49,45,0,42,47,50,43,51,44,52,72,73,74,80,81,82,88,89,90,32
    } ;
    static int pointedTypes[] =
    {
        0,0,40+16,40+16,48+16,41+16,46+16,49+16,45+16,0,42+16,47+16,50+16,43+16,51+16,44+16,52+16,72+16,73+16,74+16,80+16,81+16,82+16,88+16,89+16,90+16,33
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
void dumpStructFields(int sel, int n, int sz, SYMBOL *parent, BASECLASS *bc, HASHREC *hr)
{
    char buf[512];
    int count = 0;
    SYMBOL *table[15];
    int table1[15];
    char table2[15];
    int last = 0;
    int i;
    memset(table2, 1, sizeof(table2));
    if (bc)
    {
        TYPE tpl = { 0 };
        tpl.type = bt_pointer;
        tpl.size = getSize(bt_pointer);
        while (bc && count < sizeof(table)/sizeof(table[0]))
        {
            SYMBOL *sp = (SYMBOL *)bc->cls;
            table[count] = sp;
            // we are setting sp->offset here for use later in this function
            if (bc->isvirtual)
            {
                VBASEENTRY *vbase = parent->vbaseEntries;
                while (vbase)
                {
                    if (vbase->cls == bc->cls || sameTemplate(vbase->cls->tp, bc->cls->tp))
                    {
                        sp->offset = vbase->pointerOffset;  
                        break;
                    }
                    vbase = vbase->next;
                }
                tpl.btp = sp->tp;
                table2[count] = 0;
                table1[count++] = link_puttype(&tpl);
            }
            else
            {
                sp->offset = bc->offset;
                table1[count++] = link_puttype(sp->tp);
            }
            bc = bc->next;
        }
    }
    else
    {
        while (hr && count < sizeof(table)/sizeof(table[0]))
        {
            SYMBOL *sp = (SYMBOL *)hr->p;
            if (!istype(sp) && sp->tp->type != bt_aggregate)
            {
                table[count] = sp;
                table1[count++] = link_puttype(sp->tp);
            }
            hr = hr->next;
        }
    }
    if (bc || hr)
        dumpStructFields(9, last = typeIndex++, sz, parent, bc, hr);
    emit_record_ieee("ATT%X,T%X", n, sel);
    if (sel != 9)
        emit_record_ieee(",%X",sz);
    for (i=0; i < count; i++)
        emit_record_ieee(",T%X,%03X%s,%X", table1[i], strlen(table[i]->name), table[i]->name, table2[i] ? table[i]->offset : -1);
    
    if (last != 0)
        emit_record_ieee(",T%X", last);
    emit_record_ieee(".\r\n");
}
void dumpEnumFields(int sel, int n, int baseType, int sz, HASHREC *hr)
{
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
    emit_record_ieee("ATT%X,T%X", n, sel);
    if (sel != 9)
        emit_record_ieee(",%X", sz);
        
    for (i=0; i < count; i++)
        emit_record_ieee(",T%X,%03X%s,%X", baseType, strlen(table[i]->name), table[i]->name, table1[i]);
    
    if (last != 0)
        emit_record_ieee(",T%X", last);
    emit_record_ieee(".\r\n");
}
int dumpFunction(TYPE *tp)
{
    char buf[4096], *bptr;
    HASHREC *hr;
    int n;
    int m = link_puttype(basetype(tp)->btp);
    int v,i;
    int types[32];
    int count = 0;
    hr = basetype(tp)->syms->table[0];
    while (hr && count < sizeof(types)/sizeof(types[0]))
    {
        SYMBOL *s = (SYMBOL *)hr->p;
        types[count++] = link_puttype(s->tp);
        hr = hr->next;
    }
    switch(basetype(tp)->sp->storage_class)
    {
        case sc_virtual:
        case sc_member:
        case sc_mutable:
            v = 4;  // has a this pointer
            break;
        default:
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
            break;
    }
    if (isstructured(basetype(tp)->btp))
        v |= 32; // structured return value
    sprintf(buf,"T2,T%X,%X",m, v);
    bptr = buf + strlen(buf);
    for (i=0; i < count; i++)
    {
        sprintf(bptr,",T%X", types[i]);
        bptr += strlen(bptr);
    }
    return emit_type_ieee(buf);
}
void link_extendedtype(TYPE *tp1)
{
    TYPE *tp = basetype(tp1);
    if (tp->type == bt_pointer)
    {
        if (tp->vla)
        {
            int m = link_puttype(tp->btp);
            
            tp1->dbgindex = emit_type_ieee("T7,%X,T%X,T2A",tp->size,m);
        }
        else if (tp->array)
        {
            int m = link_puttype(tp->btp);
            tp1->dbgindex = emit_type_ieee("T6,%X,T%X,T2A,%d,%d",tp->size,m,0,tp->size/basetype(tp)->btp->size);
        }
        else
        {
            int m = link_puttype(tp->btp);
            tp1->dbgindex = emit_type_ieee("T1,%X,T%X",tp->size,m);
        }
        
    }
    else if (tp->type == bt_lref)
    {
        int m = link_puttype(tp->btp);
        tp1->dbgindex = emit_type_ieee("TB,%X,T%X",tp->size,m);
    }
    else if (tp->type == bt_rref)
    {
        int m = link_puttype(tp->btp);
        tp1->dbgindex = emit_type_ieee("TC,%X,T%X",tp->size,m);
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
            emit_record_ieee("ATT%X,T3,%X,T%X,%d,%d.\r\n",n, tp->size, m, tp->startbit, tp->bits);
            
        }
        else if (isstructured(tp))
        {
            int sel;
            tp = basetype(tp)->sp->tp; // find instantiated version in case of C++ struct
            if (tp->type == bt_union)
            {
                sel = 5;
            }
            else
            {
                sel = 4;
            }
            if (tp->syms)
                dumpStructFields(sel, n, tp->size, tp->sp, tp->sp->baseClasses, tp->syms->table[0]);
            else
                dumpStructFields(sel, n, tp->size, tp->sp, tp->sp->baseClasses, NULL);
            emit_type_name(n, tp->sp->decoratedName);
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
            emit_type_name(n, tp->sp->decoratedName);
        }
    }
}
int link_puttype(TYPE *tp)
{
        
    if (!tp->dbgindex)
    {
        if (tp->type == bt_any)
        {
            tp->dbgindex = 42;
        }
        else if (tp->type == bt_typedef)
        {
            if (!tp->btp->dbgindex)
                link_puttype(tp->btp);
            if (tp->btp->dbgindex)
            {
                tp->dbgindex = typeIndex++;
                emit_record_ieee("ATT%X,TA,T%X.\r\n", tp->dbgindex, tp->btp->dbgindex);
                emit_type_name(tp->dbgindex, tp->sp->decoratedName);
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
            if ((sp->storage_class == sc_global ||sp->storage_class == sc_localstatic) && !sp->isInline && sp->tp->used)
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
            if (!v->data && basetype(v->sp->tp)->sp)
            {
                link_puttype(v->sp->tp);
            }
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
            v = v->next;
        }
        while (dbgTypeDefs)
        {
            SYMBOL *sp = (SYMBOL *)dbgTypeDefs->data;
            sp->tp->dbgindex = link_puttype(sp->tp); 
            /*
            if (sp->tp->dbgindex < 1024 && sp->tp->used)
            {
                int n = sp->tp->dbgindex;
                sp->tp->dbgindex = typeIndex++;
               emit_record_ieee("ATT%X,TA,T%X.\r\n", sp->tp->dbgindex, n);
            }
            emit_type_name(sp->tp->dbgindex, sp->decoratedName);
            emit_record_ieee("NT%X,%03X%s.\r\n", sp->tp->dbgindex, strlen(sp->name), sp->name);
            */
            dbgTypeDefs = dbgTypeDefs->next;
        }
        emit_cs(FALSE);
    }
}
void link_virtualtypes(void)
{
    if (cparams.prm_debug)
    {
        VIRTUAL_LIST *v = virtualFirst;
        while (v)
        {
            int i = v->sp->value.i;
            if (basetype(v->sp->tp)->sp)
                emit_record_ieee("ATR%X,T%lX.\r\n", i, v->sp->tp->dbgindex);
            v = v->next;
        }
    }
}
//-------------------------------------------------------------------------

void link_putext(SYMBOL *sp)
{

    char buf[4096];
    beDecorateSymName(buf, sp);
    sp->value.i = extIndex++;
    emit_record_ieee("NX%X,%03X%s.\r\n", (int)sp->value.i, strlen(buf), buf);
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
    sprintf(buf1,"N,%03X%s,%03X%s,%03X%s", l, buf, l, buf, strlen(sp->importfile), sp->importfile);
    emit_record_ieee("CO201,%03X%s.\r\n", strlen(buf1), buf1);
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
BOOLEAN isbasevolatile(TYPE *tp)
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
    // the exception is for the win32 address thunks
    if (sp->mainsym && (sp->linkage != lk_virtual || sp->mainsym->storage_class != sc_external))
        sp = sp->mainsym;
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
            if (sp->linkage == lk_virtual)// && cparams.prm_cplusplus)
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
            if (sp->linkage == lk_virtual)
                return sp->value.i | 0xc0000000;
            else
                return codeseg;
        case sc_constant:
            return constseg;                
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
    char buf[4096], obuf[4096];
    int index;
    if (!cparams.prm_cplusplus && sp->linkage == lk_virtual)
    {
        return ;
    }
    if (isconst(sp->tp) && isint(sp->tp) && sp
        ->storage_class == sc_static)
        return ;
    if (sp->tp->type == bt_ellipse)
        return;
    if (sel == 'I')
        index = pubIndex++;
    else if (sel == 'N')
        index = localIndex++;
    else
        index = autoIndex++;
    sp->value.i = index;
    if (sp->thisPtr)
    {
        strcpy(buf, "_this");
    }
    else if (sel == 'A')
    {
        sprintf(buf, "_%s", sp->name);   
    }
    else if (sp->storage_class == sc_localstatic && sp->parent)
    {
        sprintf(buf, "@%s@%s", sp->parent->name, sp->name);
    }
    else
    {
        beDecorateSymName(buf, sp);
    }
    emit_record_ieee("N%c%X,%03X%s.\r\n", sel, index, strlen(buf), buf);
    if (sel == 'A')
    {
        emit_record_ieee("AS%c%X,%X.\r\n", sel, index, sp->offset); // taken as BP relative...
    }
    else
    {
        int seg = link_getseg(sp);
        if (seg & 0xc0000000)
            seg &= 0xffffff;
        else
            seg = segxlattab[seg];
        emit_record_ieee("AS%c%X,R%X,%X,+.\r\n", sel, index, seg, sp->offset);
    }
    if (cparams.prm_debug)
       emit_record_ieee("AT%c%X,T%lX.\r\n", sel, index, sp->tp->dbgindex);
}

//-------------------------------------------------------------------------

void link_Publics(void)
{
    VIRTUAL_LIST *v = virtualFirst;
    LIST *lf = globalNames;
    while (lf)
    {
        SYMBOL *sp = lf->data;
        if ((sp->storage_class == sc_global || sp->storage_class == sc_localstatic || (sp->storage_class == sc_constant && !sp->parent) ||
            (cparams.prm_debug && sp->parentClass && !isfunction(sp->tp)) ||
             ((sp->storage_class == sc_member || sp->storage_class == sc_virtual) && isfunction(sp->tp)&& sp->inlineFunc.stmt)))
        {
            if (sp->storage_class == sc_localstatic) // local function variable
                link_putpub(sp, 'N');
            else
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
            v = v->next;
        }
        emit_cs(FALSE);
    }    
}
//-------------------------------------------------------------------------

void link_putexport(SYMBOL *sp)
{
    char buf[512], buf1[512];
    int l;
    beDecorateSymName(buf, sp);
    l = strlen(buf);
    sprintf(buf1,"N,%03X%s,%03X%s", l, buf, l, buf);
    emit_record_ieee("CO200,%03X%s.\r\n", strlen(buf1), buf1);
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
            !sp->isInline)
        {
            link_putexport(sp);
        }
        lf = lf->next;
    }
}

//-------------------------------------------------------------------------

void link_PassSeperator(void)
{
    emit_record_ieee("CO101,006ENDSYM.\r\n");
}

//-------------------------------------------------------------------------

void link_Fixups(char *buf, FIXUP *fixup, EMIT_LIST *rec, int curseg, int offs)
{
    int iseg, xseg;
    int rel = FALSE;
    switch (fixup->fmode)
    {
        case fm_relsymbol:
            rel = TRUE;
        case fm_symbol:
        {
            SYMBOL *sp = fixup->sym;
            if (sp->storage_class == sc_absolute)
            {
                if (rel)
                {
                    strcpy(buf, "0");
                    diag("Relative absolute in link_Fixups");
                }
                else
                    sprintf(buf, "%X", fixup->sym->offset);
                return ;
            }
            iseg = link_getseg(sp);
            if (iseg & 0xc0000000)
                xseg = iseg;
            else
                xseg = segxlattab[iseg];
            if (sp->wasUsing && sp->mainsym)
                sp = sp->mainsym;
            if (!sp->dontinstantiate && ((isfunction(sp->tp) && sp->inlineFunc.stmt) || (!isfunction(sp->tp) && sp->linkage == lk_virtual) ||
                sp->storage_class == sc_global || sp->storage_class ==
                sc_static || sp->storage_class == sc_localstatic || sp->storage_class == sc_overloads))
            {
                int n = sp->linkage == lk_virtual ? 0 : sp->offset;
                if ((xseg & 0xffffff) == 0)
                    iseg = link_getseg(fixup->sym);
                if (rel)
                {
                    sprintf(buf, "R%X,%X,+,4,-,P,-", xseg & ~0xc0000000, n + offs);
                }
                else
                {
                    sprintf(buf, "R%X,%X,+", xseg & ~0xc0000000, n + offs);
                }
                /* segment relative */
            }
            else
            {
                if (rel)
                {
                    sprintf(buf, "X%X,4,-,P,-", (int)sp->value.i);
                }
                else
                {
                    sprintf(buf, "X%X,%X,+", (int)sp->value.i, offs);
                }
                /* extdef relative */
            }
            break;
        }
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
                    sprintf(buf, "R%X,%X,+", xseg, sp->offset + offs);
                }
                else
                {
                    sprintf(buf, "X%X", (int)sp->value.i);
                }
                sp = fixup->base;
                sprintf(buf+strlen(buf), ",X%X,-", (int)sp->value.i);
            }
            break;
        case fm_rellabel:
            rel = TRUE;
        case fm_label:
            xseg = iseg = LabelSeg(fixup->label);
            if (iseg < MAX_SEGS)
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
                    sprintf(buf, "R%X,%X,+,4,-,P,-", xseg, LabelAddress(fixup
                        ->label) + offs);
                }
            }
            else
            {
                sprintf(buf, "R%X,%X,+", xseg, LabelAddress(fixup
                    ->label) + offs);
            }
            /* segment relative */
            break;
    }
}

//-------------------------------------------------------------------------

static long putlr(FIXUP *p, EMIT_LIST *s, int curseg, int offs)
{
    char buf[512];
    link_Fixups(buf, p, s, curseg, offs);
    if (strstr(buf, "R0"))
        diag("putlr: invalid segment");
    if (strstr(buf, "X0"))
        diag("putlr: invalid extern");
    emit_record_ieee("LR(%s,%X).\r\n", buf, 4);
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
            emit_record_ieee("LD");
            for (i = 0; i < LDPERLINE; i++)
                emit_record_ieee("%02X", buf[count++]);
            start += LDPERLINE;
            emit_record_ieee(".\r\n");
        }
        else
        {
            for (i=0; i < end - start; i++)
            {
                if (buf[count + i] != buf[count])
                    break;
            }
            emit_record_ieee("LE(%08X,%02X).\r\n", i, buf[count]);
            count += i;
            start += i;
        }
    }
    if (start == end)
        return (start);
    emit_record_ieee("LD");
    while (start < end)
    {
        emit_record_ieee("%02X", buf[count++]);
        start++;
    }
    emit_record_ieee(".\r\n");
    return (start);
}

static void putattribs(EMIT_TAB *seg, int addr)
{
    while (seg->attriblist && seg->attriblist->address <= addr)
    {
        if (cparams.prm_debug)
        {
            char buf1[256];
            SYMBOL *sp = seg->attriblist->v.sp;
            switch(seg->attriblist->type)
            {
                case e_ad_funcdata:
                    if (sp->linkage == lk_virtual)
                        sprintf(buf1, "R%X", (int)sp->value.i);
                    
                     else if ((sp->storage_class == sc_global || (sp->storage_class == sc_constant && !sp->parent) ||
                         ((sp->storage_class == sc_member || sp->storage_class == sc_virtual) && isfunction(sp->tp)&& sp->inlineFunc.stmt)))
                        sprintf(buf1, "I%X", (int)sp->value.i);
                    else
                        sprintf(buf1, "N%X", (int)sp->value.i);
                    if (seg->attriblist->start)
                        emit_record_ieee("CO404,%03X%s.\r\n", strlen(buf1), buf1);
                    else
                        emit_record_ieee("CO405,%03X%s.\r\n", strlen(buf1), buf1);
                    break;
                case e_ad_blockdata:
                    if (seg->attriblist->start)
                    {
                        emit_record_ieee("CO402,000.\r\n");
                    }
                    else
                    {
                        emit_record_ieee("CO403,000.\r\n");
                    }
                    break;
                case e_ad_vardata:
                    if (sp->tp->type != bt_ellipse)
                    {
                        sprintf(buf1, "A%X", (int)sp->value.i);
                        emit_record_ieee("CO400,%03X%s.\r\n", strlen(buf1), buf1);
                    }
                    break;
                case e_ad_linedata:
                    sprintf(buf1, "%d,%d", seg->attriblist->v.ld->fileindex, seg->attriblist->v.ld->lineno);
                    emit_record_ieee("CO401,%03X%s.\r\n", strlen(buf1), buf1);
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
            emit_record_ieee("SB%X.\r\n", segxlattab[i]);
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
            emit_cs(FALSE);
        }
    while (v)
    {
        EMIT_LIST *rec = v->seg->first;
        i = v->sp->value.i;
        emit_record_ieee("SB%X.\r\n", i);
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
                putld(j, alen + j, rec->data + j);
                if (alen == len1)
                    putattribs(v->seg, rec->address + j);
                j += alen;
                if (f && alen == len)
                {
                    j += putlr(f, rec, i, (*(unsigned *)(rec->data + j)));
                    f = f->next;
                }
            }
            rec = rec->next;
        }
        putattribs(v->seg, INT_MAX);
        emit_cs(FALSE);
        i++;
        v = v->next;
    }
}

void link_BrowseInfo(void)
{
    if (browseInfo && cparams.prm_browse)
    {
        emit_record_ieee("CO102,007ENDDATA.\r\n");
        while (browseInfo)
        {
            char buf1[4096];
            sprintf(buf1, "%x,%x,%d,%d,%d,%03X%s",
                             browseInfo->type, 
                             browseInfo->flags, 
                             browseInfo->filenum,
                             browseInfo->lineno,
                             browseInfo->charpos, strlen(browseInfo->name), browseInfo->name);   
            emit_record_ieee("CO500,%03X%s.\r\n", strlen(buf1), buf1);
            browseInfo = browseInfo->next;
        }
    }
}


//-------------------------------------------------------------------------


//-------------------------------------------------------------------------

void output_obj_file(void)
{
    char name[260];
    FILELIST *l = incfiles;
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
    emit_cs(FALSE);
    link_Segs();
    emit_cs(FALSE);
    link_types();
    link_virtualtypes();
    emit_cs(FALSE);
    link_ExtDefs();
    link_Publics();
    link_Autos();
    link_Exports();
    link_Imports();
    emit_cs(FALSE);
    link_PassSeperator();
    link_Data();
    emit_cs(FALSE);
    link_trailer();
    
    if (cparams.prm_browse)
    {
        browsing = TRUE;
        extIndex = pubIndex = localIndex = autoIndex =1;
        typeIndex = 1024;
        checksum = 0;
        link_header(fullqualify(includedFiles->data));
        link_Files();
        emit_cs(FALSE);
        link_BrowseInfo();
        emit_cs(FALSE);
        link_trailer();
        browsing = FALSE;
    }
}
