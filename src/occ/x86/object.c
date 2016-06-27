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
#include <time.h>
#include "be.h"
 
#define DEBUG_VERSION 3.1

#define MAX_16BIT_ADDRESS 65536

extern EMIT_TAB segs[MAX_SEGS];
extern LINEDATA *linelist;
extern int outcode_base_address;
extern LIST *includedFiles;
extern VIRTUAL_LIST *virtualFirst;

static int segxlattab[MAX_SEGS];
static int dgroupVal;
static int extSize, extIndex;
static char extbuf[1024];

static LIST *includedLibraries;
static LIST *externNames ;
static LIST *globalNames ;
static LIST *exportNames ;
static LIST *importNames ;

/* must match iexpr.h */
static char *segnames[] = 
{
    0, "_TEXT", "_DATA", "_BSS", "_STRING", "_CONST", "_TLS", "cstartup", "crundown", 
        "cppinit", "cppexit", "$$TYPES", "$$SYMBOLS", "$$BROWSE"
};
static char *segclasses[] = 
{
    0, "CODE", "DATA", "BSS", "STRING", "CONST", "TLS", "INITDATA", "EXITDATA", 
        "CPPINIT", "CPPEXIT", "DEBTYP", "DEBSYM", "BROWSE"
};
// 29 = byte
// 69 = para
// a9 = dword
static unsigned char segattribs[] =
{
    0, 0x29, 0x69, 0x69, 0x69, 0xa9, 0xa9, 0xa9, 0xa9,
        0xa9, 0xa9, 0xa9, 0xa9, 0xa9
} ;
static char DgroupText[] = "DGROUP";

void omfInit(void)
{
    includedLibraries = 0;
    externNames = 0;
    globalNames = 0;
    exportNames = 0;
    importNames = 0;
}
int put_ident(char *buf, int ident)
{
    char *p = buf;
    if (ident &0xc0000000)
        *p++ = 0xc0;
    ident &= 0x7fff;
    *p++ = ident > 127 ? (ident >> 8) | 0x80: ident;
    if (ident > 127)
        *p++ = ident;
    return p - buf;
}

/*-------------------------------------------------------------------------*/

void emit_record(enum omf_type type, char *data, int len)
{
    int i, cs = 0;
    char buf[1100];
    buf[0] = type;
    *(short*)(buf + 1) = len + 1;
    memcpy(buf + 3, data, len);
    for (i = 0; i < len + 3; i++)
        cs += buf[i];
    buf[len + 3] =  - cs;
    beWrite(buf, len + 4);
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
/*-------------------------------------------------------------------------*/

void omfFileName(void)
{
    char buf[256];
    strcpy(buf + 1, fullqualify(includedFiles->data));
    buf[0] = strlen(buf + 1);
    emit_record(THEADR, buf, buf[0] + 1);
}

/*-------------------------------------------------------------------------*/

void omfTranslatorName(void)
{
    char buf[100];
    *(short*)buf = 0;
    sprintf(buf + 3, "CC386 Version %s (LADSoft)", beCompilerVersion);
    buf[2] = strlen(buf + 3);
    emit_record(COMENT, buf, 3+buf[2]);
}

/*-------------------------------------------------------------------------*/

void omfFileTime(char *file)
{
    unsigned time, date;
    char buf[256];
    struct stat s ;
    struct tm *ltime;
    *(short*)buf = 0xe900;
    if (file)
    {
        if (stat(file, &s) == -1)
        {
            time = 0;
            date = 0;
        }
        else
        {
            ltime = localtime(&s.st_mtime);
            if (ltime)
            {
                time = (ltime->tm_hour << 11) + (ltime->tm_min << 5) + (ltime->tm_sec >> 1) ;
                date = ((ltime->tm_year - 80) << 9) + ((ltime->tm_mon + 1) << 5) + ltime->tm_mday ;
            }
            else
            {
                time = 0;
                date = 0;
            }
        }
        *(short*)(buf + 2) = time;
        *(short*)(buf + 4) = date;
        memcpy(buf + 7, file, strlen(file));
        buf[6] = strlen(file);
        emit_record(COMENT, buf, 7+buf[6]);
    }
    else
        emit_record(COMENT, buf, 2);
}

/*-------------------------------------------------------------------------*/

void omfDebugMarker(void)
{
    if (cparams.prm_debug)
    {
        char buf[3];
        buf[0] = 0x40;
        buf[1] = 0xe9;
        emit_record(COMENT, buf, 2);
    }
}

/*-------------------------------------------------------------------------*/

void omfLNames(void)
{
    char buf[1024],  *p = buf;
    int pos = 1, i;
    *p++ = 0;
    for (i = 1; i < MAX_SEGS; i++)
    {
        if (segs[i].curlast || i < 4)
        {
            segxlattab[i] = pos++;
            *p = strlen(segnames[i]);
            strcpy(p + 1, segnames[i]);
            p +=  *p + 1;
            *p = strlen(segclasses[i]);
            strcpy(p + 1, segclasses[i]);
            p +=  *p + 1;

        }
        else
            segxlattab[i] = 0;
    }
    dgroupVal = pos;
    *p = strlen(DgroupText);
    strcpy(p + 1, DgroupText);
    p +=  *p + 1;
    emit_record(LNAMES, buf, p - buf);
}

/*-------------------------------------------------------------------------*/

void omfLibMod(void)
{
    if (includedLibraries)
    {
        while (includedLibraries)
        {
            char buf[256],  *p;
            p = strrchr(includedLibraries->data, '\\');
            if (p)
                p++;
            else
                p = includedLibraries->data;
            buf[0] = 0x40;
            buf[1] = 0xa3;
            buf[2] = strlen(p);
            strcpy(buf + 3, p);
            emit_record(COMENT, buf, buf[2] + 2);
            includedLibraries = includedLibraries->next;
        }
    }
}

/*-------------------------------------------------------------------------*/

void omfSegs(void)
{
    char buf[100];
    int i;
    for (i = 1; i < MAX_SEGS; i++)
    {
        if (segxlattab[i])
        {
            buf[0] = segattribs[i];
            *(int*)(buf + 1) = segs[i].curlast;
            buf[5] = segxlattab[i] *2;
            buf[6] = segxlattab[i] *2+1;
            buf[7] = 1;
            emit_record(SEGDEF, buf, 8);
        }
    }
}

/*-------------------------------------------------------------------------*/

void omfComDefs(void)
{
    VIRTUAL_LIST *v = virtualFirst;
    char buf[512], name[512],  *p;
    while (v)
    {
        beDecorateSymName(name, v->sp);
        p = buf;
        *p = strlen(name) + 1;
        strcpy(p + 2, name);
        p[1] = '@';
        p += (*p) + 1;
        *p++ = 0;
        if (v->data)
            *p++ = segxlattab[dataseg];
        else
            *p++ = segxlattab[codeseg];
        if (v->seg->curlast > 128)
        {
            *p++ = 0x88;
            *(int*)p = v->seg->curlast;
            p += sizeof(int);
        }
        else
            *p++ = v->seg->curlast;
        emit_record(COMDEF, buf, p - buf);
        v = v->next;
    }
}

/*-------------------------------------------------------------------------*/

void omfGroups(void)
{
    char buf[100],  *p = buf + 1;
    buf[0] = dgroupVal * 2;
    if (segxlattab[2])
    {
        *p++ = 0xff;
        *p++ = segxlattab[2];
    }
    if (segxlattab[3])
    {
        *p++ = 0xff;
        *p++ = segxlattab[3];
    }
    if (segxlattab[4])
    {
        *p++ = 0xff;
        *p++ = segxlattab[4];
    }
    if (segxlattab[5])
    {
        *p++ = 0xff;
        *p++ = segxlattab[5];
    }
    emit_record(GRPDEF, buf, p - buf);
}

/*-------------------------------------------------------------------------*/

void FlushExtBuf(void)
{
    if (extSize)
    {
        emit_record(EXTDEF, extbuf, extSize);
        extSize = 0;
    }
}

/*-------------------------------------------------------------------------*/

void omfputext(SYMBOL *sp)
{
    char buf[512];
    beDecorateSymName(buf, sp);
    sp->value.i = extIndex++;
    if (strlen(buf) + extSize > 1024-2)
        FlushExtBuf();
    extbuf[extSize] = strlen(buf);
    strcpy(extbuf + extSize + 1, buf);
    extSize += extbuf[extSize] + 1;
    extbuf[extSize++] = 0;
}

/*-------------------------------------------------------------------------*/

void omfExtDefs(void)
{
    SYMBOL *sp;
    VIRTUAL_LIST *v = virtualFirst;
    LIST *lf = externNames;
    extIndex = 1;
    extSize = 0;
    while (v)
    {
        v->sp->value.i = extIndex++ + 0xc0000000;
        v = v->next;
    }
    while (lf)
    {
        sp = lf->data;
        omfputext(sp);
        lf = lf->next;
    }
    FlushExtBuf();
}

/*-------------------------------------------------------------------------*/

void omfputimport(SYMBOL *sp)
{
    char obuf[512];
    int l, l1;
    l = strlen(sp->decoratedName);
    *(short*)obuf = 0xa000;
    obuf[2] = 1; /* import*/
    obuf[3] = 0; /* import by name*/
    obuf[4] = l;
    strcpy(obuf + 5, sp->decoratedName);
    obuf[5+l] = l1 = strlen(sp->importfile);
    strcpy(obuf + 6+l, sp->importfile);
    obuf[6+l + l1] = l;
    strcpy(obuf + 7+l + l1, sp->decoratedName);

    emit_record(COMENT, obuf, 7+2 * l + l1);
}

/*-------------------------------------------------------------------------*/

void omfImports(void)
{
    SYMBOL *sp;
    LIST *lf = importNames;
    while (lf)
    {
        sp = lf->data;
        if (sp->linkage2 == lk_import && sp->importfile)
        {
            omfputimport(sp);
        }
        lf = lf->next;
    }
}

/*-------------------------------------------------------------------------*/

BOOLEAN isbaseconst(TYPE *tp)
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
    return isconst(tp1);
}
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
int omfgetseg(SYMBOL *sp)
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
                    DIAG("omfGetSeg - no static member initializer");
    */
            /* fall through*/

        case sc_static:
        case sc_global:
        case sc_external:
        case sc_localstatic:
        case sc_functions:
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
        /*
            if (sp->gennedvirtfunc)
                return sp->value.i;
            else
        */
                return codeseg;
                
        case sc_type:
        case sc_typedef:
//            if (sp->gennedvirtfunc)
//            {
                /* vtab & xt*/
//                return sp->value.i;
//            }
        default:
            diag("Unknown segment type in omfGetSeg");
            return codeseg;
    }
    /* also fix the value.i field of local funcs...*/
}

/*-------------------------------------------------------------------------*/

void omfputpub(SYMBOL *sp)
{
    int l = 0;
    char buf[512], obuf[512];

    int seg, group, pos;
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
    seg = omfgetseg(sp);
    beDecorateSymName(buf, sp);
    if (seg != 1 && !(seg &0xc0000000))
        group = 1;
    else
        group = 0;
    obuf[0] = group;
    l += put_ident(obuf + 1, seg) - 1;
    pos = (obuf[2+l] = strlen(buf)) + 3;
    strcpy(obuf + 3+l, buf);
    *(int*)(obuf + pos + l) = sp->offset;
    obuf[pos + 4+l] = 0;
    emit_record(PUBDEF, obuf, pos + 5+l);
}

/*-------------------------------------------------------------------------*/

void omfpubdumphash(HASHTABLE *syms)
{
    int i;
    while (syms)
    {
        for (i = 0; i < syms->size; i++)
        {
            HASHREC *hr;
            if ((hr = (HASHREC *)syms->table[i]) != NULL)
            {
                while (hr)
                {
                    SYMBOL *sp = (SYMBOL *)hr->p;
                    if (sp->storage_class == sc_global && sp->linkage != lk_inline)
                    {
                        omfputpub(sp);
                    }
                    if (sp->storage_class == sc_functions)
                    {
                        HASHREC *hr = sp->tp->syms->table[0];
                        while (hr)
                        {
                            SYMBOL *sp1 = (SYMBOL *)hr->p;
                            if (sp1->storage_class == sc_global && sp->linkage != lk_inline)
                            {
                                omfputpub(sp1);
                            }
                            hr = hr->next;
                        }
                    }
                    /*
                    if (sp->storage_class == sc_namespace && !sp
                        ->value.classdata.parentns->anonymous)
                        omfpubdumphash(sp->value.classdata.parentns->table);
    */
    
                    hr = hr->next;
                }
            }
        }
    }
}

/*-------------------------------------------------------------------------*/

void omfPublics(void)
{
    LIST *lf = globalNames;
    while (lf)
    {
        SYMBOL *sp = lf->data;
        if (sp->storage_class == sc_global && sp->linkage != lk_inline)
        {
            omfputpub(sp);
        }
        lf = lf->next;
    }
}

/*-------------------------------------------------------------------------*/

void omfputexport(SYMBOL *sp)
{
    char buf[512], obuf[512];
    int l;
    beDecorateSymName(buf, sp);
    l = strlen(buf);
    *(short*)obuf = 0xa000;
    obuf[2] = 2; /* export*/
    obuf[3] = 0; /* export flags*/
    obuf[4] = l;
    strcpy(obuf + 5, buf);
    buf[5+l] = l;
    strcpy(obuf + 6+l, buf);

    emit_record(COMENT, obuf, 6+2 * l);
}

/*-------------------------------------------------------------------------*/

void omfExports(void)
{
    SYMBOL *sp;
    LIST *lf = exportNames;
    while (lf)
    {
        sp = lf->data;
        if (sp->storage_class == sc_global && sp->linkage2 == lk_export && 
            sp->linkage != lk_inline)
        {
            omfputexport(sp);
        }
        lf = lf->next;
    }
}

/*-------------------------------------------------------------------------*/

void omfPassSeperator(void)
{
    char buf[3];
    buf[0] = 0x40;
    buf[1] = 0xa2;
    buf[2] = 0x01;
    emit_record(COMENT, buf, 3);
}

/*-------------------------------------------------------------------------*/

int omfFixups(int seg, EMIT_LIST *rec, char *data, char *buf, int *len)
{
    int indent = 0, pos = 0;
    int fixeddat = FALSE;
    FIXUP *fixups = rec->fixups;
    while (fixups)
    {
        SYMBOL *sp;
        int rel = FALSE, iseg;
        int fixdat = 0, tdat;
        int offset = fixups->address - rec->address;
        int locat = (offset >> 8) + (offset << 8);
        int segrel = FALSE;
        switch (fixups->fmode)
        {
            case fm_relsymbol:
                rel = TRUE;
            case fm_symbol:
//				if (fixups->sym->name == 0)
//				{
//					printf("%p\n",fixups->sym);
//					fflush(stdout);
//				}
                if (fixups->sym->storage_class == sc_absolute)
                {
                    if (rel)
                        diag("Relative absolute in omfFixups");
                    else
                        *(int*)(rec->data + offset) += sp->offset;
                    continue;
                }
                iseg = omfgetseg(fixups->sym);
                {
                    SYMBOL *sp = fixups->sym;
/*						
                    if (sp->storage_class == sc_member && sp
                        ->value.classdata.gdeclare && sp
                        ->value.classdata.gdeclare->storage_class == sc_global)
                        sp = sp->value.classdata.gdeclare;
*/
                    if (sp->storage_class == sc_global || sp->storage_class ==
                        sc_static || sp->storage_class == sc_localstatic || sp->storage_class == sc_functions)
                    {
                        if (rel)
                        {
                            if (iseg == seg)
                            {
                                *(int*)(data + offset) = sp->offset -
                                    fixups->address - 4;
                            }
                            else
                            {
                                locat |= 0xa4;
                                fixdat = 0x54;
                                if (!(iseg &0xc0000000))
                                    tdat = segxlattab[iseg];
                                else
                                    tdat = iseg;
                                segrel = TRUE;
                                *(int*)(data + offset) = sp->offset;
                            }
                        }
                        else
                        {
                            locat |= 0xe4;
                            fixdat = 0x04;
                            if (!(iseg &0xc0000000))
                                tdat = segxlattab[iseg];
                            else
                                tdat = iseg;
                            /*							printf("%s:%x:%x:%p:%p\n",sp->name,sp->offset,sp->offset,sp,sp) ;*/
                            *(int*)(data + offset) += sp->offset;
                            segrel = TRUE;
                        }
                        /* segment relative */
                    }
                    else
                    {
                        if (rel)
                        {
                            locat |= 0xa4;
                            fixdat = 0x56;
                            tdat = sp->value.i;
                        }
                        else
                        {
                            locat |= 0xe4;
                            fixdat = 0x06;
                            tdat = sp->value.i;
                        }
                        /* extdef relative */
                    }
                }
                break;

            case fm_rellabel:
                rel = TRUE;
            case fm_label:
                iseg = LabelSeg(fixups->label);
                if (rel)
                {
                    if (iseg == seg)
                        *(int*)(data + offset) = LabelAddress(fixups->label) -
                            fixups->address - 4;
                    else
                    {
                        locat |= 0xa4;
                        fixdat = 0x54;
                        if (!(iseg &0xc0000000))
                            tdat = segxlattab[iseg];
                        else
                            tdat = iseg;
                    }
                }
                else
                {
                    *(int*)(data + offset) += LabelAddress(fixups->label);
                    locat |= 0xe4;
                    fixdat = 0x04;
                    if (!(iseg &0xc0000000))
                        tdat = segxlattab[iseg];
                    else
                        tdat = iseg;
                    segrel = TRUE;
                }
                /* segment relative */
                break;
        }
        if (fixdat)
        {
            int len = 5+((tdat &0x3fffffff) > 127) + !!(tdat &0xc0000000);
            if (!fixeddat && pos + len >= 1024)
            {
                fixeddat = TRUE;
                indent = 1024-pos;
                pos = 1024;
            }
            buf[pos++] = locat &0xff;
            buf[pos++] = locat >> 8;
            if ((fixdat &0x70) == 0x00)
            {
                if (segrel && (tdat == 2 || tdat == 3 || tdat == segxlattab[4] 
                    || tdat == segxlattab[5]))
                {
                    buf[pos++] = fixdat | 0x10; /* grouped, 1st group */
                    buf[pos++] = 1;
                }
                else
                    buf[pos++] = fixdat | 0x50;
            }
            else
                buf[pos++] = fixdat;
            pos += put_ident(buf + pos, tdat);
            if (tdat == 0)
            {
                diag("omfFixups - null index");
            }
        }
        fixups = fixups->next;
    }
    *len = pos;
    return indent;
}

/*-------------------------------------------------------------------------*/

void omfComDefData(void)
{
    VIRTUAL_LIST *v = virtualFirst;
    char buf[1500],  *p;
    while (v)
    {
        EMIT_LIST *rec = v->seg->first;
        while (rec)
        {
            int indent;
            int len;
            char fbuf[4096];
            p = buf;
            p += put_ident(p, v->sp->value.i);
            if (rec->address < MAX_16BIT_ADDRESS)
                *((short *)(p))++ = rec->address;
            else
                *((int*)(p))++ = rec->address;
            memcpy(p, rec->data, rec->filled);
            indent = omfFixups(virtseg, rec, p, fbuf, &len);
            if (rec->address < MAX_16BIT_ADDRESS)
                emit_record(LEDATA16, buf, rec->filled + p - buf);
            else
                emit_record(LEDATA, buf, rec->filled + p - buf);
            if (len)
                if (len < 1024)
                    emit_record(FIXUPP, fbuf, len);
                else
            {
                emit_record(FIXUPP, fbuf, 1024-indent);
                emit_record(FIXUPP, fbuf + 1024, len - 1024);
            }
            rec = rec->next;
        }
        v = v->next;
    }
}

/*-------------------------------------------------------------------------*/

void omfData(void)
{
    char buf[1100];
    int i;
    for (i = 1; i < MAX_SEGS; i++)
    if (segxlattab[i] && segs[i].curlast)
    {
        if (i == bssxseg)
        {
            int n = segs[i].curlast;
            char *p = buf;
            *p++ = segxlattab[i];
            *((short *)p)++ = 0; // offset 0
            if (n >= 65535)
            {
                *((short *)(p))++ = 1; // repeat once
                *((short *)(p))++ = 2; // two blocks
                *((short *)(p))++ = n/65535; // repeat n/65536
                *((short *)(p))++ = 1;  //  one inner block
                *((short *)(p))++ = 65535;  // repeat 65536
                *((short *)(p))++ = 0;	// data block
                *p++ = 1;				// one byte
                *p++ = 0;				// a zero
            }
            *((short *)(p))++ = n%65535; // inner repeat
            *((short *)(p))++ = 0;	// data block
            *p++ = 1;				// one byte
            *p++ = 0;				// a zero
            emit_record(LIDATA16, buf, p - buf);
        }
        else
        {
            EMIT_LIST *rec = segs[i].first;
            while (rec)
            {
                int indent;
                int len;
                char fbuf[4096];
                buf[0] = segxlattab[i];
                if (rec->address < MAX_16BIT_ADDRESS)
                {
                    *((short *)(buf + 1)) = rec->address;
                    memcpy(buf + 3, rec->data, rec->filled);
                    indent = omfFixups(i, rec, buf + 3, fbuf, &len);
                    emit_record(LEDATA16, buf, rec->filled + 3);
                }
                else
                {
                    *((int*)(buf + 1)) = rec->address;
                    memcpy(buf + 5, rec->data, rec->filled);
                    indent = omfFixups(i, rec, buf + 5, fbuf, &len);
                    emit_record(LEDATA, buf, rec->filled + 5);
                }
                if (len)
                    if (len < 1024)
                        emit_record(FIXUPP, fbuf, len);
                    else
                {
                    emit_record(FIXUPP, fbuf, 1024-indent);
                    emit_record(FIXUPP, fbuf + 1024, len - 1024);
                }
                rec = rec->next;
            }
        }
    }
}

/*-------------------------------------------------------------------------*/

void omfSourceFile(char *file, int num)
{
    unsigned short time, date;
    char buf[256];
    struct stat s ;
    struct tm *ltime;
    *(short*)buf = 0xe800;
    buf[2] = num;
    strcpy(buf + 4, fullqualify(file));
    buf[3] = strlen(buf + 4);
    if (stat(file, &s) == -1)
    {
        time = 0;
        date = 0;
    }
    else
    {
        ltime = localtime(&s.st_mtime);
        time = (ltime->tm_hour << 11) + (ltime->tm_min << 5) + (ltime->tm_sec >> 1) ;
        date = ((ltime->tm_year - 80) << 9) + ((ltime->tm_mon + 1) << 5) + ltime->tm_mday ;
    }
    *(short*)(buf + 4+buf[3]) = time;
    *(short*)(buf + 6+buf[3]) = date;
    emit_record(COMENT, buf, 8+buf[3]);
}

/*-------------------------------------------------------------------------*/

void omfLineNumbers(char *file)
{
    LINEDATA *l = linelist;
    char buf[1100],  *p;
    int lastnum =  - 1;
    while (l)
    {
        p = buf;
        *p++ = 0;
        *p++ = segxlattab[codeseg];
        while (l && p - buf < 1024-6)
        {
            if (!stricmp(l->file, file))
            {
                if (l->address != lastnum) {
                    while (l->next && l->next->file == file 
                           && l->next->address == l->address)
                        l = l->next;
                    *(short*)p = l->lineno;
                    *(int*)(p + 2) = l->address;
                    p += 6;
                    lastnum = l->address;
                }
            }
            l = l->next;
        }
        if (p - buf > 2)
        {
            emit_record(LINNUM, buf, p - buf);
        }
    }
}

/*-------------------------------------------------------------------------*/

void omfEmitLineInfo(void)
{
    int q;
    LIST *l;
//    omfSourceFile((char *)includedFiles->data, 1);
//    omfLineNumbers(0);
    for (q = 1, l = includedFiles; l; l = l->next)
/*    if (l->hascode) // fixme*/
    {
        omfSourceFile((char *)l->data, q++);
        omfLineNumbers(l->data);
    }
}

/*-------------------------------------------------------------------------*/

void omfModEnd(void)
{
    char buf = 0;
    emit_record(MODEND, &buf, 1);
}

/*-------------------------------------------------------------------------*/

void output_obj_file(void)
{
    LIST *l = includedFiles;
    extIndex = extSize = 0;
    omfFileName();
    omfTranslatorName();
    while (l)
    {
        omfFileTime(fullqualify(l->data));
        l = l->next;
    }
    omfFileTime(0);
    omfDebugMarker();
    omfLibMod();
    omfLNames();
    omfSegs();
    omfComDefs();
    omfGroups();
    omfExtDefs();
    omfImports();
    omfPublics();
    omfExports();
    omfPassSeperator();
    if (cparams.prm_debug)
        omfEmitLineInfo();
    omfComDefData();
    omfData();
    omfModEnd();
}
