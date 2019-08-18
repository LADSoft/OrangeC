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
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
#include "ccerr.h"
#include "cvinfo.h"
#include "gen386.h"   
#include "diag.h"

extern int packdata[], packlevel;
extern int global_flag;
extern int prm_debug, prm_cplusplus;
extern char version[];
extern char outfile[];
extern SYM *currentfunc;
extern EMIT_TAB segs[];
extern int startlab, retlab;
extern int packdata[], packlevel;
extern int outcode_base_address;
extern int lineno;
extern TABLE *gsyms;
extern long lc_maxauto;
extern int save_mask;

int dbgblocknum;
DBGBLOCK *DbgBlocks[2048];

int intrinsxlate[] = 
{
    T_VOID, T_BOOL08, 
    T_CHAR, T_UCHAR, 
    T_SHORT, T_USHORT, T_USHORT, 
    T_INT4, T_INT4, T_UINT4, T_INT4, T_UINT4, 
    T_INT8, T_UINT8, 
    T_REAL32, T_REAL64, T_REAL80,
    T_IMAGINARY32, T_IMAGINARY64, T_IMAGINARY80, 
    T_CPLX32, T_CPLX64, T_CPLX80, 
    T_VOID
};
int debug_outputtype(TYP *type);
static void emitsym(char *buf);
static void out_sym_header(void);

static DBGBLOCK *dbgblock,  *currentblock;
static int nexttype, funcSyms, blockSyms, symSize;
static int lastfuncoffset;

void dbginit(void)
{
    int v = CV_SIGNATURE_32BIT;
    nexttype = CV_FIRST_NONPRIM;
    if (prm_debug)
    {
        emit(typeseg, &v, 4);
        emit(symseg, &v, 4);
    }
    lastfuncoffset = 0;
}

//-------------------------------------------------------------------------

static int emittype(char *buf)
{
    int v = 0, len;
    len = *((short*)buf) + 2;
    if (len % 4)
    {
        *(int*)(buf + len) = 0;
        *(short*)buf += 4-len % 4;
        len += 4-len % 4;
    }
    emit(typeseg, buf, len);
}

//-------------------------------------------------------------------------

static void emitsym(char *buf)
{
    int v = 0, len;
    len = *((short*)buf) + 2;
    if (len % 4)
    {
        *(int*)(buf + len) = 0;
        *(short*)buf += 4-len % 4;
        len += 4-len % 4;
    }
    emit(symseg, buf, len);
}

//-------------------------------------------------------------------------

static int isintrins(TYP *type, int allowptr)
{
    int v;
    if (type->typeindex &&  *type->typeindex)
    {
        return  *type->typeindex; /* its in the table or an intrinsic that has
            been looked up */
    }
    if (allowptr && type->type == bt_pointer)
    {
        if (type->val_flag &VARARRAY)
            return 0;
        // will be handled separately
        if (type->btp == bt_pointer && (type->btp->val_flag &VARARRAY))
            return 0;
        if (v = isintrins(type, FALSE))
            if (v < CV_FIRST_NONPRIM && !(type->cflags &(DF_CONST | DF_VOL)))
                return v | (CV_TM_NPTR32 << CV_MSHIFT);
    }
    else if (allowptr && type->type == bt_farpointer)
    {
        if (v = isintrins(type, FALSE))
            if (v < CV_FIRST_NONPRIM && !(type->cflags &(DF_CONST | DF_VOL)))
                return v | (CV_TM_FPTR32 << CV_MSHIFT);
    }
    else if (allowptr && type->type == bt_segpointer)
    {
        v = CV_SP_SEGMENT << CV_SSHIFT;
    }
    else
    {
        if (type->type < sizeof(intrinsxlate) / sizeof(int) && type->type !=
            bt_enum)
            return intrinsxlate[type->type];
        // relies on the type table being ordered
    }

    return 0;
}

//-------------------------------------------------------------------------

int numeric_leaf(char *buf, unsigned int size)
{
    if (size < LF_NUMERIC)
    {
        *(short*)buf = size;
        return 2;
    }
    else
    {
        *(short*)buf = LF_LONG;
        *(int*)(buf + 2) = size;
        return 6;
    }
}

//-------------------------------------------------------------------------

int debug_outputpointer(TYP *type)
{
    char buf[100];
    int val;
    if (!type->val_flag)
    {
        lfPointer *ptr = buf + 2;
        val = debug_outputtype(type->btp);
        if (CV_IS_PRIMITIVE(val) && CV_MODE(val) == CV_TM_DIRECT)
        {
            if (type->type == bt_pointer)
                val = CV_NEWMODE(val, CV_TM_NPTR32);
            else
                val = CV_NEWMODE(val, CV_TM_FPTR32);
        }
        else
        {
            memset(buf, 0, 100);
            ptr->u.leaf = LF_POINTER;
            if (type->type == bt_pointer)
                ptr->u.attr.ptrtype = CV_PTR_NEAR32;
            else
                ptr->u.attr.ptrtype = CV_PTR_FAR32;
            ptr->u.attr.ptrmode = CV_PTR_MODE_PTR;
            ptr->u.attr.isflat32 = TRUE;
            ptr->u.attr.isvolatile = !!(type->cflags &DF_VOL);
            ptr->u.attr.isconst = !!(type->cflags &DF_CONST);
            ptr->u.utype = val;
            *(short*)buf = sizeof(lfPointer); // we are cheating here
            emittype(buf);
            val = nexttype++;
        }
    }
    else
    {
        int indexes[100];
        int i = 0, len, size = 0;
        lfArray *ptr = buf + 2;
        if (type->val_flag &VARARRAY)
        {
            varjoin: ptr->leaf = LF_VARARRAY; 
                // LF_VARARRAY is NOT one of the original types
            type = type->btp;
            while (type->esize)
                type = type->btp;
			size = type->size;
        }
        else if (type->btp == bt_pointer && (type->btp->val_flag &VARARRAY))
        {
            type = type->btp;
            goto varjoin;
        }
        else
        {
            ptr->leaf = LF_ARRAY;
			size = type->size;
            type = type->btp;
        }
		// set the array size to 1 if it is unspecified, to allow the debugger
		// to show the first element of the array...
		if (size == 0)
			size = 1;
        val = debug_outputtype(type);
        ptr->elemtype = val;
        ptr->idxtype = T_INT4;
        len = numeric_leaf(ptr->data, size); 
            // size of base type for vararray
        len = sizeof(lfArray) - 1+len;
        *(short*)buf = len;
        emittype(buf);
        val = nexttype++;
    }
    return val;
}

//-------------------------------------------------------------------------

int debug_outputref(TYP *type)
{
    char buf[100];
    int val = debug_outputtype(type->btp);
    lfPointer *ptr = buf + 2;
    ptr->u.leaf = LF_POINTER;
    ptr->u.attr.ptrtype = CV_PTR_NEAR32;
    ptr->u.attr.ptrmode = CV_PTR_MODE_REF;
    ptr->u.attr.isflat32 = TRUE;
    ptr->u.attr.isvolatile = !!(type->cflags &DF_VOL);
    ptr->u.attr.isconst = !!(type->cflags &DF_CONST);
    ptr->u.utype = val;
    *(short*)buf[0] = sizeof(lfPointer);
    emittype(buf);
    val = nexttype++;
    return val;
}

//-------------------------------------------------------------------------

static int putname(char *buf, char *name)
{
    char internal[256];
    int len;
    unmangle(internal, name);
    len = strlen(internal);
    *buf++ = len++;
    strcpy(buf, internal);
    return len;
}

//-------------------------------------------------------------------------

static int outputFields(SYM *field, int *count)
{
    char buffer[1024],  *p = buffer + 4;
    int val = 0;
    memset(buffer, 0, 1024);
    while (field)
    {
        if (field->tp->type != bt_defunc)
        {
            char qbuf[256];
            int len;
            lfMember *ptr = qbuf;
            memset(qbuf, 0, 256);
            ptr->leaf = LF_MEMBER;
            ptr->index = debug_outputtype(field->tp);
            len = numeric_leaf(ptr->offset, field->value.i);
            len += putname(ptr->offset + len, field->name) + sizeof(lfMember) -
                1;
            if (len % 4)
            {
                qbuf[len] = 0xf0 + 4-len % 4;
                len += 4-len % 4;
            }
            if (p - buffer + len < 1024-sizeof(lfIndex))
            {
                memcpy(p, qbuf, len);
                p += len;
                (*count)++;
            }
            else
            {
                lfIndex *ptr = p;
                val = outputFields(field, count);
                ptr->leaf = LF_INDEX;
                ptr->index = val;
                p += sizeof(lfIndex);
                break;
            }
        }
        field = field->next;
    }
    *(short*)(buffer + 2) = LF_FIELDLIST;
    *(short*)buffer = p - buffer - 2;
    emittype(buffer);
    return nexttype++;
}

//-------------------------------------------------------------------------

static int debug_outputnamesym(TYP *type, int idx)
{
    char buf[256];
    if (type->sp->name)
    {
        UDTSYM *us = buf;
        us->reclen = sizeof(UDTSYM) + putname(us->name, type->sp->name);
        us->rectyp = S_UDT;
        us->typind = idx;
        emitsym(buf);
    }
}

//-------------------------------------------------------------------------

static int debug_outputstruct(TYP *type)
{
    char buf[256];
    int thetype = nexttype++;
    int count = 0, len;
    int val, offset, typeofs, countofs;
    memset(buf, 0, 256);

    offset = segs[typeseg].last->filled + segs[typeseg].last->address;

    /* fill in the type now to prevent recursion */
    if (!type->typeindex)
    {
        global_flag++;
        type->typeindex = xalloc(sizeof(int));
        global_flag--;
    }
    *type->typeindex = thetype;

    if (type->type == bt_union)
    {
        lfUnion *ptr = buf + 2;
        ptr->leaf = LF_UNION;
        ptr->count = 0;
        ptr->property.packed = type->sp->structAlign < 2;
        ptr->field = 0;
        len = numeric_leaf(ptr->data, type->size);
        len += putname(ptr->data + len, type->sp->name);
        len += sizeof(lfUnion) - 1;
        typeofs = (char*) &ptr->field - (char*)ptr + 2;
        countofs = (char*) &ptr->count - (char*)ptr + 2;
    }
    else
    {
        lfClass *ptr = buf + 2;
        ptr->leaf = type->type == bt_class ? LF_CLASS : LF_STRUCTURE;
        ptr->count = 0;
        ptr->property.packed = type->sp->structAlign < 2;
        ptr->field = 0;
        len = numeric_leaf(ptr->data, type->size);
        len += putname(ptr->data + len, type->sp->name);
        len += sizeof(lfClass) - 1;
        typeofs = (char*) &ptr->field - (char*)ptr + 2;
        countofs = (char*) &ptr->count - (char*)ptr + 2;
    }

    *(short*)buf = len;
    emittype(buf);

    val = outputFields(type->lst.head, &count);

    /* have to go back after the fact and fill in the type and count */
    write_to_seg(typeseg, offset + typeofs, &val, sizeof(CV_typ_t));
    write_to_seg(typeseg, offset + countofs, &count, sizeof(unsigned short));

    debug_outputnamesym(type, thetype);

    return thetype;

}

//-------------------------------------------------------------------------

static int enumfields(SYM *field, int *count)
{
    char buffer[1024],  *p = buffer + 4;
    int val = 0;
    memset(buffer, 0, 1024);
    while (field)
    {
        char qbuf[256];
        int len;
        lfEnumerate *ptr = qbuf;
        memset(qbuf, 0, 256);
        ptr->leaf = LF_ENUMERATE;
        len = numeric_leaf(ptr->value, field->value.i);
        len += putname(ptr->value + len, field->name);
        len += sizeof(lfEnumerate) - 1;
        if (len % 4)
        {
            qbuf[len] = 0xf0 + 4-len % 4;
            len += 4-len % 4;
        }
        if (p - buffer + len < 1024-sizeof(lfIndex))
        {
            memcpy(p, qbuf, len);
            p += len;
            (*count)++;
        }
        else
        {
            lfIndex *ptr = p;
            val = enumfields(field, count);
            ptr->leaf = LF_INDEX;
            ptr->index = val;
            p += sizeof(lfIndex);
            break;
        }
        field = field->enumlist;
    }
    *(short*)(buffer + 2) = LF_FIELDLIST;
    *(short*)buffer = p - buffer - 2;
    emittype(buffer);
    return nexttype++;
}

//-------------------------------------------------------------------------

int debug_outputenum(TYP *type)
{
    char buf[256];

    lfEnum *enu = buf + 2;
    int count = 0, val, len;
    memset(buf, 0, 256);
    val = enumfields(type->enumlist, &count);
    enu->leaf = LF_ENUM;
    enu->utype = T_INT4;
    enu->count = count;
    enu->field = val;
    putname(enu->Name, type->sp->name);
    len =  *enu->Name + 1+sizeof(lfEnum) - 1;
    *(short*)buf = len;
    emittype(buf);

    debug_outputnamesym(type, nexttype);

    return nexttype++;
}

//-------------------------------------------------------------------------

int debug_outputtypell(TYP *type)
{
    int val = 0;
    if (val = isintrins(type, TRUE))
    {
        if (!type->typeindex)
        {
            global_flag++;
            type->typeindex = xalloc(sizeof(int));
            global_flag--;
        }
        return  *type->typeindex = val;
    }
    switch (type->type)
    {
        case bt_pointer:
            val = debug_outputpointer(type);
            break;
        case bt_ref:
            val = debug_outputref(type);
            break;
        case bt_struct:
            val = debug_outputstruct(type);
            break;
        case bt_union:
            val = debug_outputstruct(type);
            break;
        case bt_class:
            val = debug_outputstruct(type);
            break;
        case bt_enum:
            val = debug_outputenum(type);
            break;
        case bt_func:
             /* function ptr */
            /* FIXME - should do something more elaborate than this but don't know what
             * and need to work on something else
             */
            val = T_VOID;
            break;
        default:
            DIAG("debug_outputtype: unhandled type");
            break;
    }
    if (!type->typeindex)
        type->typeindex = xalloc(sizeof(int));
    return  *type->typeindex = val;
}

//-------------------------------------------------------------------------

void debug_outputtypedef(SYM *sp)
{
    char buf[256];
    UDTSYM *us = buf;
    us->reclen = sizeof(UDTSYM) + putname(us->name, sp->name);
    us->rectyp = S_UDT;
    us->typind = debug_outputtype(sp->tp);
    emitsym(buf);
}

//-------------------------------------------------------------------------

int debug_outputtype(TYP *type)
{
    char qbuf[32];
    if (type->type == bt_ellipse)
    // nothing can access it
        return 0;
    if (type->bits !=  - 1)
    {
        int val = debug_outputtypell(type), len;
        lfBitfield *ptr = qbuf + 2;
        ptr->leaf = LF_BITFIELD;
        ptr->length = type->bits;
        ptr->position = type->startbit;
        ptr->type = val;
        len = sizeof(lfBitfield);
        qbuf[0] = len;
        qbuf[1] = 0;
        emittype(qbuf);
        return nexttype++;
    }
    else
        return debug_outputtypell(type);
}

//-------------------------------------------------------------------------

void debug_outfunc(SYM *sp)
{
    dbgblock = currentblock = 0;
    funcSyms = 0;
    symSize = 0;
    blockSyms = 0;
    dbgblocknum = 0;
}

//-------------------------------------------------------------------------

void debug_beginblock(TABLE lsyms)
{
    DBGBLOCK *p ;
	global_flag++;
	p = xalloc(sizeof(DBGBLOCK));
	global_flag--;
    if (!dbgblock)
    {
        dbgblock = p;
        currentblock = p;
    }
    else
    {
        DBGBLOCK *n = currentblock->child;
        if (!n)
            currentblock->child = p;
        else
        {
            while (n->next)
                n = n->next;
            n->next = p;
        }
        p->parent = currentblock;
        currentblock = p;
    }
    p->oldsyms = lsyms.head;
    DbgBlocks[dbgblocknum = blockSyms++] = currentblock;

}

//-------------------------------------------------------------------------

static void getSymSize(DBGBLOCK *blk)
{
    SYM *s = blk->syms;
    while (s && s != blk->oldsyms)
    {
        int l;
        char internal[256];
        funcSyms++;
        unmangle(internal, s->name);
        l = strlen(internal);
        if (s->inreg)
        {
            l += sizeof(REGSYM);
        }
        else if (s->funcparm | s->storage_class == sc_auto || s->storage_class 
            == sc_autoreg)
        {
            l += sizeof(BPRELSYM32);
        }
        else
        {
            l += sizeof(DATASYM32);
        }
        if (l % 4)
            l += 4-l % 4;
        symSize += l;
        s = s->next;
    }
    if (blk->child)
        getSymSize(blk->child);
    if (blk->next)
        getSymSize(blk->next);
}

//-------------------------------------------------------------------------

static void dumpblocks(DBGBLOCK *blk, char *buf, int *ofs, int parent, int
    baseoffs)
{
    SYM *p = blk->syms;
    SYMTYPE *ss;
    BLOCKSYM32 *bss;
    int startofs =  *ofs;

    if (blk != dbgblock)
    {
        bss = buf +  *ofs;
        bss->reclen = sizeof(BLOCKSYM32) + 1-2;
        *ofs += bss->reclen + 2;
        bss->rectyp = S_BLOCK32;
        bss->pParent = parent;
        bss->off = blk->startofs;
        bss->len = blk->endofs - blk->startofs;
        bss->seg = codeseg;
        bss->name[0] = 0;
    }

    while (p && p != blk->oldsyms)
    {
        int l;
        char internal[256];
        unmangle(internal, p->name);
        l = strlen(internal);
        if (p->inreg)
        {
            REGSYM *rs = buf +  *ofs;
            int size;
            l += sizeof(REGSYM);
            if (l % 4)
                l += 4-l % 4;
            *ofs += l;
            rs->reclen = l - 2;
            rs->rectyp = S_REGISTER;
            rs->typind = debug_outputtype(p->tp);
            size = ( - p->value.i) / 256;
            rs->reg = ( - p->value.i) &255;
            if (rs->reg > 15)
            // ESI,EDI,
                rs->reg -= 12;
            rs->reg += (size == 4 ? 2 : size - 1) *8+CV_REG_AL;
            rs->name[0] = strlen(internal);
            memcpy(rs->name + 1, internal, rs->name[0]);
        }
        else if (p->funcparm | p->storage_class == sc_auto || p->storage_class 
            == sc_autoreg)
        {
            BPRELSYM32 *bs = buf +  *ofs;
            l += sizeof(BPRELSYM32);
            if (l % 4)
                l += 4-l % 4;
            *ofs += l;
            bs->reclen = l - 2;
            bs->rectyp = S_BPREL32;
            bs->off = p->value.i;
            bs->typind = debug_outputtype(p->tp);
            bs->name[0] = strlen(internal);
            memcpy(bs->name + 1, internal, bs->name[0]);
        }
        else
        {
            DATASYM32 *ds = buf +  *ofs;
			int n;
            l += sizeof(DATASYM32);
            if (l % 4)
                l += 4-l % 4;
            *ofs += l;
            ds->reclen = l - 2;
            ds->rectyp = S_LDATA32;
            ds->off = p->offset;
            ds->seg = n = omfgetseg(p);
            if (n &0xc0000000)
                ds->seg = codeseg;
            ds->typind = debug_outputtype(p->tp);
            ds->name[0] = strlen(internal);
            memcpy(ds->name + 1, internal, ds->name[0]);
        }
        p = p->next;
    }

    if (blk->child)
    {
        if (blk != dbgblock)
            dumpblocks(blk->child, buf, ofs, startofs + baseoffs, baseoffs);
        else
            dumpblocks(blk->child, buf, ofs, parent, baseoffs);
    }
    if (blk != dbgblock)
    {
        bss->pEnd =  *ofs + baseoffs;

        ss = buf +  *ofs;
        ss->reclen = 4-2;
        ss->rectyp = S_END;
        *ofs += 4;
    }

    if (blk->next)
        dumpblocks(blk->next, buf, ofs, parent, baseoffs);
}

//-------------------------------------------------------------------------

void dumptypes(DBGBLOCK *blk)
{
    SYM *p = blk->syms;
    while (p && p != blk->oldsyms)
    {
        debug_outputtype(p->tp);
        p = p->next;
    }
    if (blk->child)
        dumptypes(blk->child);
    if (blk->next)
        dumptypes(blk->next);
}

//-------------------------------------------------------------------------

void debug_endblock(TABLE lsyms, TABLE oldsyms)
{
    DBGBLOCK *p = currentblock;
    SYM *s;
    int size = 0, offset, procsize, l, bufofs = 0, retsize, retstyle;
    char *buf;
    PROCSYM32 *ps,  *funcsym;
    RETURNSYM *rets;
    BLOCKSYM32 *bss;
    SYMTYPE *ss;
    ENDARGSYM *es;
    BPRELSYM32 *bs;
    REGSYM *rs;
    char internal[256];
    int proctype;
	int regthunksize;
	REGREL32 *rr;
	int i;
    p->syms = s = lsyms.head;
    if (currentblock != dbgblock)
    {
        currentblock = currentblock->parent;
        return ;
    }
    getSymSize(dbgblock);
    s = currentfunc->tp->lst.head;
    while (s && s != (SYM*) - 1)
    {
        if (s->storage_class != sc_type)
        {
            debug_outputtype(s->tp);
            funcSyms++;
            unmangle(internal, s->name);
            l = strlen(internal);
            if (s->inreg)
                l += sizeof(REGSYM);
            else
                l += sizeof(BPRELSYM32);
            if (l % 4)
                l += 4-l % 4;
            symSize += l;
        }
        s = s->next;
    }
	regthunksize = sizeof(REGREL32)+1;
	if (regthunksize % 4)
		regthunksize += 4 - regthunksize % 4;
    dumptypes(dbgblock);
    proctype = debug_outputtype(currentfunc->tp->btp);

    /* if we get here it is time to dump the function and its blocks */
    unmangle(internal, currentfunc->name);
    procsize = sizeof(PROCSYM32) + strlen(internal);
    if (procsize % 4)
        procsize += (4-procsize % 4);
    retsize = sizeof(RETURNSYM);
    if (currentfunc->tp->btp->type == bt_void)
        retstyle = CV_GENERIC_VOID;
    else if (currentfunc->calleenearret)
    {
        retstyle = CV_GENERIC_ICAN;
    }
    else
    {
        retstyle = CV_GENERIC_REG;
        retsize += 2;
    }
    if (retsize % 4)
        retsize += 4-retsize % 4;
    size = procsize + retsize + regthunksize * 3 + symSize + sizeof(S_ENDARG) + 
		(blockSyms - 1)*((sizeof(BLOCKSYM32) + 1) + sizeof(SYMTYPE) - CV_ZEROLEN) + 
		sizeof(SYMTYPE) - 1;
    funcsym = ps = buf = xalloc(size);

    bufofs += (ps->reclen = procsize - 2) + 2;
    /* fill in the procedure data last */
    offset = segs[symseg].last->filled + segs[symseg].last->address;
    if (lastfuncoffset)
    {
        write_to_seg(symseg, lastfuncoffset, &offset, 4);
    }
    lastfuncoffset = offset + (int) &ps->pNext - (int)ps;
    ps->rectyp = currentfunc->storage_class == sc_static ? S_LPROC32 :
        S_GPROC32;
    ps->pParent = 0;
    ps->pEnd = offset + size - (sizeof(SYMTYPE) - CV_ZEROLEN); 
        // Will be filled in later
    ps->pNext = 0; // will be filled in later
    ps->len = outcode_base_address - currentfunc->offset;
    ps->DbgStart = LabelAddress(startlab) - currentfunc->offset;
    ps->DbgEnd = LabelAddress(retlab) - currentfunc->offset;
    ps->off = currentfunc->offset;
    ps->seg = codeseg; // should not be relocated
    ps->typind = proctype;
    // ps->flags = 0 ; // technically we should track FPO here, but I'm not using it
    ps->name[0] = strlen(internal);
    memcpy(ps->name + 1, internal, ps->name[0]);

    rets = buf + bufofs;
    rets->reclen = retsize - 2;
    rets->rectyp = S_RETURN;
    rets->flags.cstyle = !(currentfunc->pascaldefn);
    rets->flags.rsclean = currentfunc->pascaldefn | currentfunc->isstdcall;
    rets->style = retsize > 8 ? CV_GENERIC_REG : CV_GENERIC_VOID;
    if (rets->style == CV_GENERIC_REG)
    {
        TYP *tp = currentfunc->tp->btp;
        int sz = tp->size;
        if (sz < 0)
            sz =  - sz;
        buf[bufofs + sizeof(RETURNSYM)] = 1;
        if (tp->type == bt_float || tp->type == bt_double || tp->type ==
            bt_longdouble || tp->type == bt_fcomplex || tp->type == bt_rcomplex
            || tp->type == bt_lrcomplex)
            buf[bufofs + sizeof(RETURNSYM) + 1] = CV_REG_ST0;
        else
            buf[bufofs + sizeof(RETURNSYM) + 1] = CV_REG_AL + 8 *(sz == 4 ? 2 :
                sz - 1);
    }
    bufofs += retsize;

	i = 4;
	rr = buf + bufofs;
	rr->name[0] = 1;
	rr->name[1] = '&';
	rr->rectyp = S_REGREL32;
	rr->reclen = regthunksize - 2;
	rr->reg = CV_REG_EBX;
	if (save_mask & 0x8)
	{
		rr->off = -lc_maxauto - i;
		i += 4;
	}
	else
	{
		rr->off = 0;
	}
	rr->typind = 0;
	bufofs += regthunksize;
	rr = buf + bufofs;
	rr->name[0] = 1;
	rr->name[1] = '&';
	rr->rectyp = S_REGREL32;
	rr->reclen = regthunksize - 2;
	rr->reg = CV_REG_ESI;
	if (save_mask & 0x400)
	{
		rr->off = -lc_maxauto - i;
		i += 4;
	}
	else
	{
		rr->off = 0;
	}
	rr->typind = 0;
	bufofs += regthunksize;
	rr = buf + bufofs;
	rr->name[0] = 1;
	rr->name[1] = '&';
	rr->rectyp = S_REGREL32;
	rr->reclen = regthunksize - 2;
	rr->reg = CV_REG_EDI;
	if (save_mask & 0x800)
	{
		rr->off = -lc_maxauto - i;
		i += 4;
	}
	else
	{
		rr->off = 0;
	}
	rr->typind = 0;
	bufofs += regthunksize;

    s = currentfunc->tp->lst.head;
    while (s && s != (SYM*) - 1)
    {
        if (s->storage_class != sc_type)
        {
            unmangle(internal, s->name);
            if (s->inreg)
            {
                int size;
                rs = buf + bufofs;
                retsize = sizeof(REGSYM) + strlen(internal);
                if (retsize % 4)
                    retsize += 4-retsize % 4;
                bufofs += retsize;
                rs->reclen = retsize - 2;
                rs->rectyp = S_REGISTER;
                rs->typind = debug_outputtype(s->tp);
                size = ( - s->value.i) / 256;
                rs->reg = ( - s->value.i) &255;
                if (rs->reg > 15)
                // ESI,EDI,
                    rs->reg -= 12;
                rs->reg += (size == 4 ? 2 : size - 1) *8+CV_REG_AL;
                rs->name[0] = strlen(internal);
                memcpy(rs->name + 1, internal, rs->name[0]);
            }
            else
            {
                bs = buf + bufofs;
                retsize = sizeof(BPRELSYM32) + strlen(internal);
                if (retsize % 4)
                    retsize += 4-retsize % 4;
                bufofs += retsize;
                bs->reclen = retsize - 2;
                bs->rectyp = S_BPREL32;
                bs->off = s->value.i;
                bs->typind = debug_outputtype(s->tp);
                bs->name[0] = strlen(internal);
                memcpy(bs->name + 1, internal, bs->name[0]);
            }
        }
        s = s->next;
    }
    es = bufofs + buf;
    bufofs += sizeof(ENDARGSYM);
    es->reclen = sizeof(ENDARGSYM) - 2;
    es->rectyp = S_ENDARG;

    dumpblocks(dbgblock, buf, &bufofs, offset, offset);

    ss = buf + bufofs;
    ss->reclen = 4-2;
    ss->rectyp = S_END;

    bufofs += 4; /* should equal SIZE now if we did everything right */
    if (bufofs != size)
    {
        char bf[256];
        sprintf(bf, "Debug output - function size mismatch %d %d", size, bufofs)
            ;
        DIAG(bf);
    }


    emit(symseg, buf, size);
    s = currentfunc->tp->lst.head;
    while (s && s != (SYM*) - 1)
    {
        if (s->storage_class == sc_type && s->istypedef)
            debug_outputtypedef(s);
        s = s->next;
    }
}

//-------------------------------------------------------------------------

void debug_outdata(SYM *sp, int BSS)
{
    char buf[256];
    DATASYM32 *p = buf;
    char internal[256];
    int offset;
	int n;
    unmangle(internal, sp->name);
    p->reclen = sizeof(DATASYM32) + (p->name[0] = strlen(internal)) - 2;
    p->rectyp = sp->storage_class == sc_static ? S_LDATA32 : S_GDATA32;
    p->off = sp->offset;
    p->seg = n = omfgetseg(sp);
    if (n &0xc0000000)
        p->seg = codeseg;
    /* complex types should already be in the table, but not all 
     * inherent types have been resolved to thier CV version yet
     */
    p->typind = debug_outputtype(sp->tp);
    strcpy(p->name + 1, internal);
    offset = segs[symseg].last->filled + segs[symseg].last->address;
    if (lastfuncoffset)
    {
        write_to_seg(symseg, lastfuncoffset, &offset, 4);
        lastfuncoffset = 0;
    }
    emitsym(buf);
}

//-------------------------------------------------------------------------

static void dumphashtable(HASHREC **syms)
{
    int i;
    SYM *sp;
    for (i = 0; i < HASHTABLESIZE; i++)
    {
        if ((sp = (SYM*)syms[i]) != 0)
        {
            while (sp)
            {
                if (sp->storage_class == sc_type && sp->istypedef && sp->tp
                    ->typeindex &&  *sp->tp->typeindex)
                    debug_outputtypedef(sp);
                if (sp->storage_class == sc_namespace && !sp
                    ->value.classdata.parentns->anonymous)
                    dumphashtable(sp->value.classdata.parentns->table);
                sp = sp->next;
            }
        }
    }
}

//-------------------------------------------------------------------------

void debug_dumpglobaltypedefs(void)
{

    dumphashtable(gsyms);
}

//-------------------------------------------------------------------------

static void out_sym_header(void)
{
    char buf[256], buf1[256];
    CFLAGSYM *p = buf;
    OBJNAMESYM *q = buf;
    sprintf(buf1, "CC386 Version %s (LADSoft)", version);
    memset(buf, 0, 256);
    p->reclen = sizeof(CFLAGSYM) + (p->ver[0] = strlen(buf1));
    p->rectyp = S_COMPILE;
    p->machine = CV_CFL_80386;
    p->flags.language = prm_cplusplus ? CV_CFL_CXX : CV_CFL_C;
    p->flags.pcode = FALSE;
    p->flags.floatprec = TRUE;
    p->flags.floatpkg = CV_CFL_NDP;
    p->flags.ambdata = CV_CFL_DNEAR;
    p->flags.ambcode = CV_CFL_CNEAR;
    p->flags.mode32 = TRUE;
    strcpy(p->ver + 1, buf1);
    emitsym(buf);

    memset(buf, 0, 256);
    q->reclen = sizeof(OBJNAMESYM) + (q->name[0] = strlen(outfile));
    q->rectyp = S_OBJNAME;
    q->signature = 0;
    strcpy(q->name + 1, outfile);
    emitsym(buf);
}
