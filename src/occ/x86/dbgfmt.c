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
#include <string.h>
#include "be.h"
#include "cvinfo.h"
 
extern EMIT_TAB segs[];
extern int startlab, retlab;
extern int outcode_base_address;

extern HASHTABLE *globalSyms;

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
int debug_outputtype(TYPE *type);
static void emitsym(char *buf);

static DBGBLOCK *dbgblock,  *currentblock;
static int nexttype, funcSyms, blockSyms, symSize;
static int lastfuncoffset;

void dbginit(void)
{
    int v = CV_SIGNATURE_32BIT;
    nexttype = CV_FIRST_NONPRIM;
    if (cparams.prm_debug)
    {
        emit(typeseg, &v, 4);
        emit(symseg, &v, 4);
    }
    lastfuncoffset = 0;
}

/*-------------------------------------------------------------------------*/

static void emittype(char *buf)
{
    int len;
    len = *((short*)buf) + 2;
    if (len % 4)
    {
        *(int*)(buf + len) = 0;
        *(short*)buf += 4-len % 4;
        len += 4-len % 4;
    }
    emit(typeseg, buf, len);
}

/*-------------------------------------------------------------------------*/

static void emitsym(char *buf)
{
    int len;
    len = *((short*)buf) + 2;
    if (len % 4)
    {
        *(int*)(buf + len) = 0;
        *(short*)buf += 4-len % 4;
        len += 4-len % 4;
    }
    emit(symseg, buf, len);
}

/*-------------------------------------------------------------------------*/

static int isintrins(TYPE *type, int allowptr)
{
    int v;
    type = basetype(type);
    if (type->typeindex &&  *type->typeindex)
    {
        return  *type->typeindex; /* its in the table or an intrinsic that has
            been looked up */
    }
    if (allowptr && type->type == bt_pointer)
    {
        if (type->vla)
            return 0;
        /* will be handled separately*/
        if (type->btp->type == bt_pointer && (type->btp->vla))
            return 0;
        v = isintrins(type, FALSE);
        if (v)
            if (v < CV_FIRST_NONPRIM && !isconst(type) && !isvolatile(type))
                return v | (CV_TM_NPTR32 << CV_MSHIFT);
    }
    else if (allowptr && type->type == bt_far)
    {
        v = isintrins(type, FALSE);
        if (v)
            if (v < CV_FIRST_NONPRIM && !isconst(type) && !isvolatile(type))
                return v | (CV_TM_FPTR32 << CV_MSHIFT);
    }
    else if (allowptr && type->type == bt_seg)
    {
/*        v = CV_SP_SEGMENT << CV_SSHIFT;*/
    }
    else
    {
        if (type->type < sizeof(intrinsxlate) / sizeof(int) && type->type !=
            bt_enum)
            return intrinsxlate[type->type];
        /* relies on the type table being ordered*/
    }

    return 0;
}

/*-------------------------------------------------------------------------*/

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

/*-------------------------------------------------------------------------*/

int debug_outputpointer(TYPE *type)
{
    char buf[100];
    int val;
    if (!type->array)
    {
        lfPointer *ptr = (lfPointer *)(buf + 2);
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
            ptr->u.attr.isvolatile = isvolatile(type);
            ptr->u.attr.isconst = isconst(type);
            ptr->u.utype = val;
            *(short*)buf = sizeof(lfPointer); /* we are cheating here*/
            emittype(buf);
            val = nexttype++;
        }
    }
    else
    {
        int len, size;
        lfArray *ptr = (lfArray *)(buf + 2);
        if (type->vla)
        {
            varjoin: ptr->leaf = LF_VARARRAY; 
                /* LF_VARARRAY is NOT one of the original types*/
            type = type->btp;
            while (type->esize)
                type = type->btp;
            size = type->size;
        }
        else if (type->btp->type == bt_pointer && (type->btp->vla))
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
        val = debug_outputtype(type);
        ptr->elemtype = val;
        ptr->idxtype = T_INT4;
        len = numeric_leaf(ptr->data, size); 
            /* size of base type for vararray*/
        len = sizeof(lfArray) - 1+len;
        *(short*)buf = len;
        emittype(buf);
        val = nexttype++;
    }
    return val;
}

/*-------------------------------------------------------------------------*/

int debug_outputref(TYPE *type)
{
    char buf[100];
    int val = debug_outputtype(type->btp);
    lfPointer *ptr = (lfPointer *)(buf + 2);
    ptr->u.leaf = LF_POINTER;
    ptr->u.attr.ptrtype = CV_PTR_NEAR32;
    ptr->u.attr.ptrmode = CV_PTR_MODE_REF;
    ptr->u.attr.isflat32 = TRUE;
    ptr->u.attr.isvolatile = isvolatile(type);
    ptr->u.attr.isconst = isconst(type);
    ptr->u.utype = val;
    *(short*)buf[0] = sizeof(lfPointer);
    emittype(buf);
    val = nexttype++;
    return val;
}

/*-------------------------------------------------------------------------*/

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

/*-------------------------------------------------------------------------*/

static int outputFields(HASHREC *h, int *count)
{
    char buffer[1024],  *p = buffer + 4;
    int val ;
    memset(buffer, 0, 1024);
    while (h)
    {
        SYMBOL *field = (SYMBOL *)h->p;
        if (field->tp->type != bt_defunc)
        {
            char qbuf[256];
            int len;
            lfMember *ptr = (lfMember *)qbuf;
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
                lfIndex *ptr = (lfIndex *)p;
                val = outputFields(h, count);
                ptr->leaf = LF_INDEX;
                ptr->index = val;
                p += sizeof(lfIndex);
                break;
            }
        }
        h = h->next;
    }
    *(short*)(buffer + 2) = LF_FIELDLIST;
    *(short*)buffer = p - buffer - 2;
    emittype(buffer);
    return nexttype++;
}

/*-------------------------------------------------------------------------*/

static void debug_outputnamesym(TYPE *type, int idx)
{
    char buf[256];
    if (type->sp->name)
    {
        UDTSYM *us = (UDTSYM *)buf;
        us->reclen = sizeof(UDTSYM) + putname(us->name, type->sp->name);
        us->rectyp = S_UDT;
        us->typind = idx;
        emitsym(buf);
    }
}

/*-------------------------------------------------------------------------*/

static int debug_outputstruct(TYPE *type)
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
        type->typeindex = beGlobalAlloc(sizeof(int));
    }
    *type->typeindex = thetype;

    if (type->type == bt_union)
    {
        lfUnion *ptr = (lfUnion *)(buf + 2);
        ptr->leaf = LF_UNION;
        ptr->count = 0;
        ptr->property.packed = type->alignment < 2;
        ptr->field = 0;
        len = numeric_leaf(ptr->data, type->size);
        len += putname(ptr->data + len, type->sp->name);
        len += sizeof(lfUnion) - 1;
        typeofs = (char*) &ptr->field - (char*)ptr + 2;
        countofs = (char*) &ptr->count - (char*)ptr + 2;
    }
    else
    {
        lfClass *ptr = (lfClass *)(buf + 2);
        ptr->leaf = type->type == bt_class ? LF_CLASS : LF_STRUCTURE;
        ptr->count = 0;
        ptr->property.packed = type->alignment < 2;
        ptr->field = 0;
        len = numeric_leaf(ptr->data, type->size);
        len += putname(ptr->data + len, type->sp->name);
        len += sizeof(lfClass) - 1;
        typeofs = (char*) &ptr->field - (char*)ptr + 2;
        countofs = (char*) &ptr->count - (char*)ptr + 2;
    }

    *(short*)buf = len;
    emittype(buf);

    val = outputFields(type->syms->table[0], &count);

    /* have to go back after the fact and fill in the type and count */
    write_to_seg(typeseg, offset + typeofs, &val, sizeof(CV_typ_t));
    write_to_seg(typeseg, offset + countofs, &count, sizeof(unsigned short));

    debug_outputnamesym(type, thetype);

    return thetype;

}

/*-------------------------------------------------------------------------*/

static int enumfields(HASHREC *h, int *count)
{
    char buffer[1024],  *p = buffer + 4;
    int val ;
    memset(buffer, 0, 1024);
    while (h)
    {
        char qbuf[256];
        int len;
        SYMBOL *field = (SYMBOL *)h->p;
        lfEnumerate *ptr = (lfEnumerate *)qbuf;
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
            lfIndex *ptr = (lfIndex *)p;
            val = enumfields(h, count);
            ptr->leaf = LF_INDEX;
            ptr->index = val;
            p += sizeof(lfIndex);
            break;
        }
        h = h->next;
    }
    *(short*)(buffer + 2) = LF_FIELDLIST;
    *(short*)buffer = p - buffer - 2;
    emittype(buffer);
    return nexttype++;
}

/*-------------------------------------------------------------------------*/

int debug_outputenum(TYPE *type)
{
    char buf[256];

    lfEnum *enu = (lfEnum *)(buf + 2);
    int count = 0, val, len;
    memset(buf, 0, 256);
    val = enumfields(type->syms->table[0], &count);
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

/*-------------------------------------------------------------------------*/

int debug_outputtypell(TYPE *type)
{
    int val = isintrins(type, TRUE);
    if (val)
    {
        if (!type->typeindex)
        {
            type->typeindex = beGlobalAlloc(sizeof(int));
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
            diag("debug_outputtype: unhandled type");
            break;
    }
    if (!type->typeindex)
        type->typeindex = beLocalAlloc(sizeof(int));
    return  *type->typeindex = val;
}

/*-------------------------------------------------------------------------*/

void debug_outputtypedef(SYMBOL *sp)
{
    char buf[256];
    UDTSYM *us = (UDTSYM *)buf;
    us->reclen = sizeof(UDTSYM) + putname(us->name, sp->name);
    us->rectyp = S_UDT;
    us->typind = debug_outputtype(sp->tp);
    emitsym(buf);
}

/*-------------------------------------------------------------------------*/

int debug_outputtype(TYPE *type)
{
    char qbuf[32];
    if (type->type == bt_ellipse)
    /* nothing can access it*/
        return 0;
    if (type->bits !=  - 1)
    {
        int val = debug_outputtypell(type), len;
        lfBitfield *ptr = (lfBitfield *)(qbuf + 2);
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

/*-------------------------------------------------------------------------*/

void debug_outfunc(SYMBOL *sp)
{
    (void)sp;
    dbgblock = 0;
    funcSyms = 0;
    symSize = 0;
    blockSyms = 0;
    dbgblocknum = 0;
}

/*-------------------------------------------------------------------------*/

void debug_beginblock(HASHTABLE *lsyms, int startlab)
{
    DBGBLOCK *p = beGlobalAlloc(sizeof(DBGBLOCK));
    (void)lsyms;
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
//    p->oldsyms = lsyms.head;
    DbgBlocks[dbgblocknum = blockSyms++] = currentblock;
    currentblock->startlab= startlab;
}

/*-------------------------------------------------------------------------*/

static void getSymSize(DBGBLOCK *blk)
{
    HASHREC *h = blk->syms;
    while (h)
    {
        SYMBOL *s = (SYMBOL *)h->p;
        if (s->storage_class != sc_parameter)
        {
            int l;
            char internal[256];
            funcSyms++;
            unmangle(internal, s->name);
            l = strlen(internal);
            if (s->regmode)
            {
                l += sizeof(REGSYM);
            }
            else if (s->storage_class == sc_parameter || 
                     s->storage_class == sc_auto || 
                     s->storage_class == sc_register)
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
        }
        h = h->next;
    }
    if (blk->child)
        getSymSize(blk->child);
    if (blk->next)
        getSymSize(blk->next);
}

/*-------------------------------------------------------------------------*/

static void dumpblocks(DBGBLOCK *blk, char *buf, int *ofs, int parent, int
    baseoffs)
{
    HASHREC *h = blk->syms;
    SYMTYPE *ss;
    BLOCKSYM32 *bss;
    int startofs =  *ofs;

    if (blk != dbgblock)
    {
        bss = (BLOCKSYM32 *)(buf +  *ofs);
        bss->reclen = sizeof(BLOCKSYM32) + 1-2;
        *ofs += bss->reclen + 2;
        bss->rectyp = S_BLOCK32;
        bss->pParent = parent;
        bss->off = LabelAddress(blk->startlab);
        bss->len = LabelAddress(blk->endlab) - bss->off;
        bss->seg = codeseg;
        bss->name[0] = 0;
    }

    while (h)
    {
        SYMBOL *p = (SYMBOL *)h->p;
        if (p->storage_class != sc_parameter)
        {
            int l;
            char internal[256];
            unmangle(internal, p->name);
            l = strlen(internal);
            if (p->regmode)
            {
                REGSYM *rs = (REGSYM *)(buf +  *ofs);
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
                /* ESI,EDI,*/
                    rs->reg -= 12;
                rs->reg += (size == 4 ? 2 : size - 1) *8+CV_REG_AL;
                rs->name[0] = strlen(internal);
                memcpy(rs->name + 1, internal, rs->name[0]);
            }
            else if (p->storage_class == sc_parameter || p->storage_class == sc_auto || p->storage_class 
                == sc_register)
            {
                BPRELSYM32 *bs = (BPRELSYM32 *)(buf +  *ofs);
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
                DATASYM32 *ds = (DATASYM32 *)(buf +  *ofs);
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
        }
        h = h->next;
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

        ss = (SYMTYPE *)(buf +  *ofs);
        ss->reclen = 4-2;
        ss->rectyp = S_END;
        *ofs += 4;
    }

    if (blk->next)
        dumpblocks(blk->next, buf, ofs, parent, baseoffs);
}

/*-------------------------------------------------------------------------*/

void dumptypes(DBGBLOCK *blk)
{
    HASHREC *h = blk->syms;
    while (h)
    {
        SYMBOL *p = (SYMBOL *)h->p;
        debug_outputtype(p->tp);
        h = h->next;
    }
    if (blk->child)
        dumptypes(blk->child);
    if (blk->next)
        dumptypes(blk->next);
}

/*-------------------------------------------------------------------------*/

void debug_endblock(HASHTABLE *lsyms, int endlab)
{
    DBGBLOCK *p = currentblock;
    HASHREC *h;
    int size, offset, procsize, l, bufofs = 0, retsize, retstyle;
    char *buf;
    PROCSYM32 *ps ;
    RETURNSYM *rets;
    SYMTYPE *ss;
    ENDARGSYM *es;
    BPRELSYM32 *bs;
    REGSYM *rs;
    char internal[256];
    int proctype;
    if (lsyms)
    {
        p->syms = lsyms->table[0];
        currentblock->endlab = endlab;
    }
    if (currentblock != dbgblock)
    {
        currentblock = currentblock->parent;
        return ;
    }
    else
        if (lsyms)
            return;
    getSymSize(dbgblock);
    h= beGetCurrentFunc->tp->syms->table[0];
    while (h)
    {
        SYMBOL *s = (SYMBOL *)h->p;
        if (s->storage_class != sc_type && s->tp->type != bt_void)
        {
            debug_outputtype(s->tp);
            funcSyms++;
            unmangle(internal, s->name);
            l = strlen(internal);
            if (s->regmode)
                l += sizeof(REGSYM);
            else
                l += sizeof(BPRELSYM32);
            if (l % 4)
                l += 4-l % 4;
            symSize += l;
        }
        h = h->next;
    }
    dumptypes(dbgblock);
    proctype = debug_outputtype(beGetCurrentFunc->tp->btp);

    /* if we get here it is time to dump the function and its blocks */
    unmangle(internal, beGetCurrentFunc->name);
    procsize = sizeof(PROCSYM32) + strlen(internal);
    if (procsize % 4)
        procsize += (4-procsize % 4);
    retsize = sizeof(RETURNSYM);
    if (beGetCurrentFunc->tp->btp->type == bt_void)
        retstyle = CV_GENERIC_VOID;
    else if (beGetCurrentFunc->calleenearret)
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
    size = procsize + retsize + symSize + sizeof(S_ENDARG) + (blockSyms - 1)*(
        (sizeof(BLOCKSYM32) + 1) + sizeof(SYMTYPE) - CV_ZEROLEN) + sizeof
        (SYMTYPE) - 1;
    buf = (char *)beLocalAlloc(size);
    ps = (PROCSYM32 *)buf;

    bufofs += (ps->reclen = procsize - 2) + 2;
    /* fill in the procedure data last */
    offset = segs[symseg].last->filled + segs[symseg].last->address;
    if (lastfuncoffset)
    {
        write_to_seg(symseg, lastfuncoffset, &offset, 4);
    }
    lastfuncoffset = offset + (int) &ps->pNext - (int)ps;
    ps->rectyp = beGetCurrentFunc->storage_class == sc_static ? S_LPROC32 :
        S_GPROC32;
    ps->pParent = 0;
    ps->pEnd = offset + size - (sizeof(SYMTYPE) - CV_ZEROLEN); 
        /* Will be filled in later*/
    ps->pNext = 0; /* will be filled in later*/
    ps->len = outcode_base_address - beGetCurrentFunc->offset;
    ps->DbgStart = LabelAddress(startlab) - beGetCurrentFunc->offset;
    ps->DbgEnd = LabelAddress(retlab) - beGetCurrentFunc->offset;
    ps->off = beGetCurrentFunc->offset;
    ps->seg = codeseg; /* should not be relocated*/
    ps->typind = proctype;
    /* ps->flags = 0 ; // technically we should track FPO here, but I'm not using it*/
    ps->name[0] = strlen(internal);
    memcpy(ps->name + 1, internal, ps->name[0]);

    rets = (RETURNSYM *)(buf + bufofs);
    rets->reclen = retsize - 2;
    rets->rectyp = S_RETURN;
    rets->flags.cstyle = !(beGetCurrentFunc->linkage == lk_pascal);
    rets->flags.rsclean = beGetCurrentFunc->linkage == lk_pascal ||
                            beGetCurrentFunc->linkage == lk_stdcall;
    rets->style = retsize > 8 ? CV_GENERIC_REG : CV_GENERIC_VOID;
    if (rets->style == CV_GENERIC_REG)
    {
        TYPE *tp = beGetCurrentFunc->tp->btp;
        int sz = tp->size;
        if (sz < 0)
            sz =  - sz;
        buf[bufofs + sizeof(RETURNSYM)] = 1;
        if (tp->type == bt_float || tp->type == bt_double || tp->type ==
            bt_long_double || tp->type == bt_float_complex || tp->type == bt_double_complex
            || tp->type == bt_long_double_complex)
            buf[bufofs + sizeof(RETURNSYM) + 1] = CV_REG_ST0;
        else
            buf[bufofs + sizeof(RETURNSYM) + 1] = CV_REG_AL + 8 *(sz == 4 ? 2 :
                sz - 1);
    }

    bufofs += retsize;
    h = beGetCurrentFunc->tp->syms->table[0];
    while (h)
    {
        SYMBOL *s = (SYMBOL *)h->p;
        if (s->storage_class != sc_type && s->tp->type != bt_void)
        {
            unmangle(internal, s->name);
            if (s->regmode)
            {
                int size;
                rs = (REGSYM *)(buf + bufofs);
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
                /* ESI,EDI,*/
                    rs->reg -= 12;
                rs->reg += (size == 4 ? 2 : size - 1) *8+CV_REG_AL;
                rs->name[0] = strlen(internal);
                memcpy(rs->name + 1, internal, rs->name[0]);
            }
            else
            {
                bs = (BPRELSYM32 *)(buf + bufofs);
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
        h = h->next;
    }
    es = (ENDARGSYM *)(bufofs + buf);
    bufofs += sizeof(ENDARGSYM);
    es->reclen = sizeof(ENDARGSYM) - 2;
    es->rectyp = S_ENDARG;

    dumpblocks(dbgblock, buf, &bufofs, offset, offset);

    ss = (SYMTYPE *)(buf + bufofs);
    ss->reclen = 4-2;
    ss->rectyp = S_END;

    bufofs += 4; /* should equal SIZE now if we did everything right */
    if (bufofs != size)
    {
        char bf[256];
        sprintf(bf, "Debug output - function size mismatch %d %d", size, bufofs)
            ;
        diag(bf);
    }


    emit(symseg, buf, size);
    h = beGetCurrentFunc->tp->syms->table[0];
    while (h)
    {
        SYMBOL *s = (SYMBOL *)h->p;
        if (s->storage_class == sc_type)
            debug_outputtypedef(s);
        h = h->next;
    }
}

/*-------------------------------------------------------------------------*/

void debug_outdata(SYMBOL *sp, int BSS)
{
    char buf[256];
    DATASYM32 *p = (DATASYM32 *)buf;
    char internal[256];
    int offset;
    int n;
    (void)BSS;
    unmangle(internal, sp->name);
    p->reclen = sizeof(DATASYM32) + (p->name[0] = strlen(internal)) - 2;
    p->rectyp = sp->storage_class == sc_static || sp->storage_class == sc_localstatic
        ? S_LDATA32 : S_GDATA32;
    p->off = sp->offset;
    p->seg = n = omfgetseg(sp);
    if (n&0xc0000000)
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

/*-------------------------------------------------------------------------*/


