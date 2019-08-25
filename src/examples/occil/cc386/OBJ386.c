/*
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by
Matthew Brandt (mailto::mattb@walkingdog.net)

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "gen386.h"
#include "diag.h"
#include "lists.h"

#define DEBUG_VERSION 3.1
#define MAX_16BIT_ADDRESS 65536

enum omf_type
{
    THEADR = 0x80,  /* module header */
    COMENT = 0x88,  /* comment record */

    LINNUM = 0x95,  /* line number record */
    LNAMES = 0x96,  /* list of names */

    SEGDEF = 0x99,  /* segment definition */
    GRPDEF = 0x9A,  /* group definition */
    EXTDEF = 0x8C,  /* external definition */
    PUBDEF = 0x91,  /* public definition */
    COMDEF = 0xB0,  /* common definition */

        LEDATA16 = 0xA0,
    LEDATA = 0xA1,  /* logical enumerated data */
        FIXUPP = 0x9D,  /* fixups (relocations) */
    LIDATA16 = 0xA2,
    LIDATA = 0xA3,

    MODEND = 0x8A /* module end */
};

extern int prm_fardefault;
extern HASHREC **templateFuncs;
extern FILELIST *incfiles;
extern VIRTUAL_LIST *virtualFirst;
extern int prm_debug;
extern FILE *outputFile;
extern char version[];
extern char *infile;
extern EMIT_TAB segs[MAX_SEGS];
extern LIST *libincludes;
extern LINEBUF *linelist;
extern TABLE *gsyms;
extern LIST *localfuncs,  *localdata;
extern int prm_bss;
extern int outcode_base_address;

static int segxlattab[MAX_SEGS];
static int dgroupVal;
static int extSize, extIndex;
static char extbuf[1024];

char *segnames[20], *segclasses[20];
static char *segdefaultnames[] =
{
    0, "_TEXT", "_DATA", "_BSS", "_CONST", "_STRING", "_INIT_", "_EXIT_",
        "cppinit", "cppexit", "$$TYPES", "$$SYMBOLS", "$$BROWSE"
};
static char *segdefaultclasses[] =
{
    0, "CODE", "DATA", "BSS", "CONST", "STRING", "INITDATA", "EXITDATA",
        "CPPINIT", "CPPEXIT", "DEBTYP", "DEBSYM", "BROWSE"
};
// 29 = byte
// 69 = para
// a9 = dword
static unsigned char segattribs[] =
{
        0, 0x29, 0x69, 0x69, 0x69, 0xa9, 0x29, 0x29,
                0xa9, 0xa9, 0xa9, 0xa9, 0xa9
} ;
static char DgroupText[] = "DGROUP";

void obj_init()
{
        memcpy(segnames, segdefaultnames, sizeof(segdefaultnames));
        memcpy(segclasses, segdefaultclasses, sizeof(segdefaultclasses));
}
int put_ident(char *buf, int ident)
{
    char *p = buf;
    if (ident &0xc0000000)
        {
                ident &= 0x3fff;
        *p++ = 0xc0 + (ident >> 8);
                *p++ = ident & 0xff;
        }
        else
        {
            ident &= 0x7fff;
        *p++ = ident > 127 ? (ident >> 8) | 0x80: ident;
            if (ident > 127)
            *p++ = ident;
        }
    return p - buf;
}

//-------------------------------------------------------------------------

void emit_record(enum omf_type type, char *data, int len)
{
    int i, cs = 0;
    char buf[1100];
    buf[0] = type;
    *(short*)(buf + 1) = len + 1;
    memcpy(buf + 3, data, len);
    for (i = 0; i < len + 3; i++)
        {
        cs += buf[i];
        }
    buf[len + 3] =  - cs;
    owrite(buf, len + 4, 1, outputFile);
}

void omfFileName(void)
{
    char buf[256], *p, *p1;
    strcpy(buf + 1, fullqualify(infile));
    /*
    p = strrchr(buf + 1, '\\');
    p1 = strrchr(buf + 1, '/');
    p = (p > p1) ? p : p1;
    p1 = buf + 1;
    if (!p)
        p = strrchr(p1, ':');
    if (!p)
        p = p1;
    else
        p++;
    while (*p)
        *p1++ = *p++;
    *p1 = 0;
    */
    buf[0] = strlen(buf + 1);
    emit_record(THEADR, buf, buf[0] + 1);
}

//-------------------------------------------------------------------------

void omfTranslatorName(void)
{
    char buf[100];
    *(short*)buf = 0;
    sprintf(buf + 3, "CC386 Version %s (LADSoft)", version);
    buf[2] = strlen(buf + 3);
    emit_record(COMENT, buf, 3+buf[2]);
}

//-------------------------------------------------------------------------

void omfFileTime(char *file)
{
    /*
    unsigned time, date;
    char buf[256];
    int fd;
    *(short*)buf = 0xe900;
    if (file)
    {
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
        *(short*)(buf + 2) = time;
        *(short*)(buf + 4) = date;
        memcpy(buf + 7, file, strlen(file));
        buf[6] = strlen(file);
        emit_record(COMENT, buf, 7+buf[6]);
    }
    else
        emit_record(COMENT, buf, 2);
    */
}

//-------------------------------------------------------------------------

void omfDebugMarker(void)
{
    if (prm_debug)
    {
        char buf[3];
        buf[0] = 0x40;
        buf[1] = 0xe9;
        emit_record(COMENT, buf, 2);
    }
}

//-------------------------------------------------------------------------

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

//-------------------------------------------------------------------------

void omfLibMod(void)
{
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
            buf[0] = 0x40;
                        buf[1] = 0xa3;
            buf[2] = strlen(p);
            strcpy(buf + 3, p);
            emit_record(COMENT, buf, buf[2] + 3);
            libincludes = libincludes->link;
        }
    }
}

//-------------------------------------------------------------------------

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

//-------------------------------------------------------------------------

void omfComDefs(void)
{
    VIRTUAL_LIST *v = virtualFirst;
    char buf[512], name[512],  *p;
    while (v)
    {
        putsym(name, v->sp, v->sp->name);
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
            *((int*)p)++ = v->seg->curlast;
        }
        else
            *p++ = v->seg->curlast;
        emit_record(COMDEF, buf, p - buf);
        v = v->next;
    }
}

//-------------------------------------------------------------------------

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
        if (!prm_fardefault)
        {
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
        }
    emit_record(GRPDEF, buf, p - buf);
}

//-------------------------------------------------------------------------

void FlushExtBuf(void)
{
    if (extSize)
    {
        emit_record(EXTDEF, extbuf, extSize);
        extSize = 0;
    }
}

//-------------------------------------------------------------------------

void omfputext(SYM *sp)
{
    char buf[512];
    putsym(buf, sp, sp->name);
    sp->mainsym->value.i = extIndex++;
    if (strlen(buf) + extSize > 1024-2)
        FlushExtBuf();
    extbuf[extSize] = strlen(buf);
    strcpy(extbuf + extSize + 1, buf);
    extSize += extbuf[extSize] + 1;
    extbuf[extSize++] = 0;
}

//-------------------------------------------------------------------------

static void omfextdumphash(HASHREC **syms)
{
    int i;
    SYM *sp;
    for (i = 0; i < HASHTABLESIZE; i++)
    {
        if ((sp = (SYM*)syms[i]) != 0)
        {
            while (sp)
            {
                if (sp->storage_class == sc_externalfunc && sp->mainsym
                    ->extflag && !(sp->tp->cflags &DF_INTRINS))
                {
                    omfputext(sp);
                }
                if (sp->storage_class == sc_external && sp->mainsym->extflag)
                {
                    omfputext(sp);
                }
                if (sp->storage_class == sc_defunc)
                {
                    SYM *sp1 = sp->tp->lst.head;
                    while (sp1)
                    {
                        if (sp1->storage_class == sc_externalfunc && sp1
                            ->mainsym->extflag && !(sp1->tp->cflags &DF_INTRINS)
                            )
                        {
                            omfputext(sp1);
                        }
                        sp1 = sp1->next;
                    }
                }
                if (sp->storage_class == sc_namespace && !sp
                    ->value.classdata.parentns->anonymous)
                    omfextdumphash(sp->value.classdata.parentns->table);
                sp = sp->next;
            }
        }
    }
}

//-------------------------------------------------------------------------

void omfExtDefs(void)
{
    int i;
    SYM *sp;
    VIRTUAL_LIST *v = virtualFirst;
    LIST *lf = localfuncs;
    extIndex = 1;
    extSize = 0;
    while (v)
    {
        v->sp->value.i = extIndex++ + 0xc0000000;
        v = v->next;
    }
    omfextdumphash(gsyms);

    while (lf)
    {
        sp = lf->data;
        if (sp->storage_class == sc_externalfunc && sp->mainsym->extflag && !
            (sp->tp->cflags &DF_INTRINS))
        {
            omfputext(sp);
        }
        lf = lf->link;
    }
    lf = localdata;
    while (lf)
    {
        sp = lf->data;
        if (sp->mainsym->extflag)
            omfputext(sp);
        lf = lf->link;
    }
    for (i = 0; i < HASHTABLESIZE; i++)
    {
        struct _templatelist *tl;
        if ((tl = (struct templatelist*)templateFuncs[i]) != 0)
        {
            while (tl)
            {
                if (!tl->sp->value.classdata.templatedata->hasbody && tl
                    ->finalsp)
                {
                    omfputext(tl->finalsp);
                } tl = tl->next;
            }
        }
    }
    FlushExtBuf();
}

//-------------------------------------------------------------------------

void omfputimport(SYM *sp)
{
    char buf[512], obuf[512];
    int l, l1;
    putsym(buf, sp, sp->name);
    l = strlen(buf);
    *(short*)obuf = 0xa000;
    obuf[2] = 1; // import
    obuf[3] = 0; // import by name
    obuf[4] = l;
    strcpy(obuf + 5, buf);
    obuf[5+l] = l1 = strlen(sp->importfile);
    strcpy(obuf + 6+l, sp->importfile);
    obuf[6+l + l1] = l;
    strcpy(obuf + 7+l + l1, buf);

    emit_record(COMENT, obuf, 7+2 * l + l1);
}

//-------------------------------------------------------------------------

void omfImports(void)
{
    SYM *sp;
    int i;
    LIST *lf = localfuncs;
    while (lf)
    {
        sp = lf->data;
        if (sp->mainsym->importable && sp->mainsym->importfile && sp->mainsym->extflag)
        {
            omfputimport(sp);
        }
        lf = lf->link;
    }
    lf = localdata;
    while (lf)
    {
        sp = lf->data;
        if (sp->mainsym->importable && sp->mainsym->importfile && sp->mainsym->extflag)
        {
            omfputimport(sp);
        }
        lf = lf->link;
    }
    for (i = 0; i < HASHTABLESIZE; i++)
    {
        if ((sp = (SYM*)((SYM **)gsyms)[i]) != 0)
        {
            while (sp)
            {
                if (sp->mainsym->importable && sp->mainsym->importfile && sp->mainsym->extflag)
                {
                    omfputimport(sp);
                }
                sp = sp->next;
            }
        }
    }
}

//-------------------------------------------------------------------------

int omfgetseg(SYM *sp)
{
    if (!sp->tp)
        return dataseg;
    switch (sp->storage_class)
    {
        case sc_member:
            if (sp->value.classdata.gdeclare)
                sp = sp->value.classdata.gdeclare;
            else
                if (sp->tp->type != bt_func && sp->tp->type != bt_ifunc)
                    DIAG("omfGetSeg - no static member initializer");
            // fall through

        case sc_static:
        case sc_global:
            if (sp->tp->type == bt_func || sp->tp->type == bt_ifunc)
                if (sp->mainsym->gennedvirtfunc)
                    return sp->mainsym->value.i;
                else
                    return codeseg;
            /* fall through */
        case sc_external:
            if ((sp->tp->cflags &DF_CONST) && !(sp->tp->cflags &DF_VOL))
                return constseg;
            else
                if (sp->init || !prm_bss)
                    return dataseg;
                else
                    return bssxseg;
        case sc_externalfunc:
        case sc_label:
            if (sp->mainsym->gennedvirtfunc)
                return sp->mainsym->value.i;
            else
                return codeseg;
        case sc_type:
            if (sp->mainsym->gennedvirtfunc)
            {
                // vtab & xt
                return sp->mainsym->value.i;
            }
        default:
            DIAG("Unknown segment type in omfGetSeg");
            DIAG(sp->name);
            return codeseg;
    }
    // also fix the value.i field of local funcs...
}

//-------------------------------------------------------------------------

void omfputpub(SYM *sp)
{
    int l = 0;
    char buf[512], obuf[512];

    int seg, group, pos;
    if (sp->gennedvirtfunc)
        return ;
    if (sp->value.classdata.templatedata)
        return ;
    if (sp->value.classdata.cppflags &PF_INLINE)
        return ;
    if ((sp->tp->cflags &DF_CONST) && scalarnonfloat(sp->tp) && sp
        ->storage_class == sc_static)
        return ;
    seg = omfgetseg(sp);
    putsym(buf, sp, sp->name);
    if (seg != 1 && !(seg &0xc0000000))
        group = 1;
    else
        group = 0;
    obuf[0] = group;
    l += put_ident(obuf + 1, seg) - 1;
    pos = (obuf[2+l] = strlen(buf)) + 3;
    strcpy(obuf + 3+l, buf);
    *(int*)(obuf + pos + l) = sp->mainsym->offset;
    obuf[pos + 4+l] = 0;
    emit_record(PUBDEF, obuf, pos + 5+l);
}

//-------------------------------------------------------------------------

void omfpubdumphash(HASHREC **syms)
{
    int i;
    SYM *sp;
    for (i = 0; i < HASHTABLESIZE; i++)
    {
        if ((sp = (SYM*)syms[i]) != 0)
        {
            while (sp)
            {
                if (sp->storage_class == sc_global && !(sp
                    ->value.classdata.cppflags &PF_INLINE))
                {
                    omfputpub(sp);
                }
                if (sp->storage_class == sc_defunc)
                {
                    SYM *sp1 = sp->tp->lst.head;
                    while (sp1)
                    {
                        if (sp1->storage_class == sc_global && !(sp
                            ->value.classdata.cppflags &PF_INLINE))
                        {
                            omfputpub(sp1);
                        }
                        sp1 = sp1->next;
                    }
                }
                if (sp->storage_class == sc_namespace && !sp
                    ->value.classdata.parentns->anonymous)
                    omfpubdumphash(sp->value.classdata.parentns->table);


                sp = sp->next;
            }
        }
    }
}

//-------------------------------------------------------------------------

void omfPublics(void)
{
    SYM *sp;
    LIST *lf = localfuncs;
    int i;
    omfpubdumphash(gsyms);
    while (lf)
    {
        SYM *sp = lf->data;
        if (sp->storage_class == sc_global && !(sp->value.classdata.cppflags
            &PF_INLINE))
        {
            omfputpub(sp);
        }
        lf = lf->link;
    }
    for (i = 0; i < HASHTABLESIZE; i++)
    {
        struct _templatelist *tl;
        if ((tl = (struct templatelist*)templateFuncs[i]) != 0)
        {
            while (tl)
            {
                if (tl->sp->value.classdata.templatedata->hasbody && tl
                    ->finalsp)
                {
                    omfputpub(tl->finalsp);
                } tl = tl->next;
            }
        }
    }
}

//-------------------------------------------------------------------------

void omfputexport(SYM *sp)
{
    char buf[512], obuf[512];
    int l;
    putsym(buf, sp, sp->name);
    l = strlen(buf);
    *(short*)obuf = 0xa000;
    obuf[2] = 2; // export
    obuf[3] = 0; // export flags
    obuf[4] = l;
    strcpy(obuf + 5, buf);
    buf[5+l] = l;
    strcpy(obuf + 6+l, buf);

    emit_record(COMENT, obuf, 6+2 * l);
}

//-------------------------------------------------------------------------

void omfExports(void)
{
    SYM *sp;
    int i;
    LIST *lf = localfuncs;
    while (lf)
    {
        sp = lf->data;
        if (sp->storage_class == sc_global && sp->mainsym->exportable && !(sp->value.classdata.cppflags & PF_INLINE))
        {
            omfputexport(sp);
        }
        lf = lf->link;
    }
    for (i = 0; i < HASHTABLESIZE; i++)
    {
        if ((sp = (SYM*)((SYM **)gsyms)[i]) != 0)
        {
            while (sp)
            {
                if (sp->storage_class == sc_global && sp->mainsym->exportable)
                {
                    omfputexport(sp);
                }
                if (sp->storage_class == sc_defunc) {
                    SYM *sp1 = sp->tp->lst.head ;
                    while (sp1) {
                        if (sp1->storage_class == sc_global && sp1->mainsym->exportable &&
                                !(sp1->value.classdata.cppflags & PF_INLINE))
                        {
                            omfputexport(sp1);
                        }
                        sp1 = sp1->next;
                    }
                }
                sp = sp->next;
            }
        }
    }
}

//-------------------------------------------------------------------------

void omfPassSeperator(void)
{
    char buf[3];
    buf[0] = 0x40;
    buf[1] = 0xa2;
    buf[2] = 0x01;
    emit_record(COMENT, buf, 3);
}

//-------------------------------------------------------------------------

int omfFixups(int seg, EMIT_LIST *rec, char *data, char *buf, int *len)
{
    int indent = 0, pos = 0;
    int fixeddat = FALSE;
    FIXUP *fixups = rec->fixups;
    while (fixups)
    {
        SYM *sp;
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
                if (fixups->sym->storage_class == sc_abs)
                {
                    if (rel)
                        DIAG("Relative absolute in omfFixups");
                    else
                        *(int*)(rec->data + offset) += sp->mainsym->offset;
                    continue;
                }
                iseg = omfgetseg(fixups->sym);
                {
                    SYM *sp = fixups->sym;
                    if (sp->mainsym)
                        sp = sp->mainsym;
                    if (sp->storage_class == sc_member && sp
                        ->value.classdata.gdeclare && sp
                        ->value.classdata.gdeclare->storage_class == sc_global)
                        sp = sp->value.classdata.gdeclare;
                    if (sp->storage_class == sc_global || sp->storage_class ==
                        sc_static || (sp->storage_class == sc_external || sp
                        ->storage_class == sc_externalfunc) && !sp->mainsym
                        ->extflag)
                    {
                        if (rel)
                        {
                            if (iseg == seg)
                            {
                                *(int*)(data + offset) = sp->mainsym->offset -
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
                                *(int*)(data + offset) = sp->mainsym->offset;
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
                            //                                                  printf("%s:%x:%x:%p:%p\n",sp->name,sp->offset,sp->mainsym->offset,sp,sp->mainsym) ;
                            *(int*)(data + offset) += sp->mainsym->offset;
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
                DIAG("omfFixups - null index");
        }
        fixups = fixups->next;
    }
    *len = pos;
    return indent;
}

//-------------------------------------------------------------------------

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
            char fbuf[2048];
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

//-------------------------------------------------------------------------

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
                                *((short *)(p))++ = 0;  // data block
                                *p++ = 1;                               // one byte
                                *p++ = 0;                               // a zero
                        }
                        *((short *)(p))++ = n%65535; // inner repeat
                        *((short *)(p))++ = 0;  // data block
                        *p++ = 1;                               // one byte
                        *p++ = 0;                               // a zero
            emit_record(LIDATA16, buf, p - buf);
        }
        else
        {
            EMIT_LIST *rec = segs[i].first;
            while (rec)
            {
                int indent;
                int len;
                char fbuf[2048];
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

//-------------------------------------------------------------------------

void omfSourceFile(char *file, int num)
{
    unsigned time=0, date=0;
    char buf[256];
    int fd;
    *(short*)buf = 0xe800;
    buf[2] = num;
    strcpy(buf + 4, fullqualify(file));
    buf[3] = strlen(buf + 4);
    /*
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
    */
    *(short*)(buf + 4+buf[3]) = time;
    *(short*)(buf + 6+buf[3]) = date;
    emit_record(COMENT, buf, 8+buf[3]);
}

//-------------------------------------------------------------------------

void omfLineNumbers(int file)
{
    LINEBUF *l = linelist;
    char buf[1100],  *p;
    int lastnum =  - 1;
    while (l)
    {
        p = buf;
        *p++ = 0;
        *p++ = segxlattab[codeseg];
        while (l && p - buf < 1024-6)
        {
            if (l->file == file)
            {
                //            if (l->address != lastnum) {
                *(short*)p = l->lineno;
                *(int*)(p + 2) = l->address;
                p += 6;
                lastnum = l->address;
                //            }
            }
            l = l->next;
        }
        if (p - buf > 2)
        {
            emit_record(LINNUM, buf, p - buf);
        }
    }
}

//-------------------------------------------------------------------------

void omfEmitLineInfo(void)
{
    int i, q;
    FILELIST *l;
    omfSourceFile(infile, 1);
    omfLineNumbers(0);
    for (i = 1, q = 2, l = incfiles; l; i++, l = l->link)
    if (l->hascode)
    {
        omfSourceFile(l->data, q++);
        omfLineNumbers(i);
    }
}

//-------------------------------------------------------------------------

void omfModEnd(void)
{
    char buf = 0;
    emit_record(MODEND, &buf, 1);
}

//-------------------------------------------------------------------------

void output_obj_file(void)
{
    LIST *l = incfiles;
    extIndex = extSize = 0;
    omfFileName();
    omfTranslatorName();
    omfFileTime(fullqualify(infile));
    while (l)
    {
        omfFileTime(l->data);
        l = l->link;
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
    if (prm_debug)
        omfEmitLineInfo();
    omfComDefData();
    omfData();
    omfModEnd();
}
