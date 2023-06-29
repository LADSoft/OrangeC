/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include <cstdio>
#include <malloc.h>
#include <cstring>
#include <climits>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include <cstdarg>
#include <map>
#include "config.h"
#include "ildata.h"
#include "memory.h"
#include "iblock.h"
#include "OptUtils.h"

namespace Optimizer
{
std::map<Optimizer::IMODE*, Optimizer::IMODE*> loadHash;
std::unordered_map<CASTTEMP*, IMODE*, OrangeC::Utils::fnv1a32_binary<sizeof(CASTTEMP)>, OrangeC::Utils::bin_eql<sizeof(CASTTEMP)>>
    castHash;
int tempCount;

LIST* immed_list[4091];
static unsigned long long shifts[sizeof(long long) * 8];

void constoptinit(void)
{
    int i;
    for (i = 0; i < sizeof(long long) * 8; i++)
        shifts[i] = ((unsigned long long)1) << i;
    memset(&immed_list, 0, sizeof(immed_list));
}

int sizeFromISZ(int isz)
{
    ARCH_SIZING* p = Optimizer::chosenAssembler->arch->type_sizes;
    switch (isz)
    {
        case ISZ_U16:
            return 2;
        case ISZ_U32:
            return 4;
        case ISZ_BIT:
            return 0;
        case ISZ_UCHAR:
        case -ISZ_UCHAR:
            return p->a_char;
        case ISZ_BOOLEAN:
            return p->a_bool;
        case ISZ_USHORT:
        case -ISZ_USHORT:
            return p->a_short;
        case ISZ_WCHAR:
            return p->a_wchar_t;
        case ISZ_ULONG:
        case -ISZ_ULONG:
            return p->a_long;
        case ISZ_ULONGLONG:
        case -ISZ_ULONGLONG:
            return p->a_longlong;
        case ISZ_UINT:
        case -ISZ_UINT:
        case ISZ_UNATIVE:
        case -ISZ_UNATIVE:
            return p->a_int;
            /*        case ISZ_ENUM:*/
            /*            return p->a_enum;*/
        case ISZ_ADDR:
        case ISZ_STRING:
        case ISZ_OBJECT:
            return p->a_addr;
        case ISZ_SEG:
            return p->a_farseg;
        case ISZ_FARPTR:
            return p->a_farptr;
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
            return p->a_float;
        case ISZ_DOUBLE:
        case ISZ_IDOUBLE:
            return p->a_double;
        case ISZ_LDOUBLE:
        case ISZ_ILDOUBLE:
            return p->a_longdouble;
        case ISZ_CFLOAT:
            return (p->a_float + p->a_fcomplexpad) * 2;
        case ISZ_CDOUBLE:
            return (p->a_double + p->a_rcomplexpad) * 2;
        case ISZ_CLDOUBLE:
            return (p->a_longdouble + p->a_lrcomplexpad) * 2;
        default:
            return 1;
    }
}
int alignFromISZ(int isz)
{
    ARCH_SIZING* p = Optimizer::chosenAssembler->arch->type_align;
    switch (isz)
    {
        case ISZ_U16:
            return 2;
        case ISZ_U32:
            return 4;
        case ISZ_BIT:
            return 0;
        case ISZ_UCHAR:
        case -ISZ_UCHAR:
            return p->a_char;
        case ISZ_BOOLEAN:
            return p->a_bool;
        case ISZ_USHORT:
        case -ISZ_USHORT:
            return p->a_short;
        case ISZ_WCHAR:
            return p->a_wchar_t;
        case ISZ_ULONG:
        case -ISZ_ULONG:
            return p->a_long;
        case ISZ_ULONGLONG:
        case -ISZ_ULONGLONG:
            return p->a_longlong;
        case ISZ_UINT:
        case -ISZ_UINT:
        case ISZ_UNATIVE:
        case -ISZ_UNATIVE:
            return p->a_int;
            /*        case ISZ_ENUM:*/
            /*            return p->a_enum;*/
        case ISZ_ADDR:
        case ISZ_STRING:
        case ISZ_OBJECT:
            return p->a_addr;
        case ISZ_SEG:
            return p->a_farseg;
        case ISZ_FARPTR:
            return p->a_farptr;
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
            return p->a_float;
        case ISZ_DOUBLE:
        case ISZ_IDOUBLE:
            return p->a_double;
        case ISZ_LDOUBLE:
        case ISZ_ILDOUBLE:
            return p->a_longdouble;
        case ISZ_CFLOAT:
            return (p->a_float + p->a_fcomplexpad) * 2;
        case ISZ_CDOUBLE:
            return (p->a_double + p->a_rcomplexpad) * 2;
        case ISZ_CLDOUBLE:
            return (p->a_longdouble + p->a_lrcomplexpad) * 2;
        default:
            return 1;
    }
}
int needsAtomicLockFromISZ(int isz)
{
    ARCH_SIZING* p = Optimizer::chosenAssembler->arch->type_needsLock;
    switch (isz)
    {
        case ISZ_U16:
            return 0;
        case ISZ_U32:
            return 0;
        case ISZ_BIT:
            return 0;
        case ISZ_UCHAR:
        case -ISZ_UCHAR:
            return p->a_char;
        case ISZ_BOOLEAN:
            return p->a_bool;
        case ISZ_USHORT:
        case -ISZ_USHORT:
            return p->a_short;
        case ISZ_WCHAR:
            return p->a_wchar_t;
        case ISZ_ULONG:
        case -ISZ_ULONG:
            return p->a_long;
        case ISZ_ULONGLONG:
        case -ISZ_ULONGLONG:
            return p->a_longlong;
        case ISZ_UINT:
        case -ISZ_UINT:
        case ISZ_UNATIVE:
        case -ISZ_UNATIVE:
            return p->a_int;
            /*        case ISZ_ENUM:*/
            /*            return p->a_enum;*/
        case ISZ_ADDR:
        case ISZ_STRING:
        case ISZ_OBJECT:
            return p->a_addr;
        case ISZ_SEG:
            return p->a_farseg;
        case ISZ_FARPTR:
            return p->a_farptr;
        case ISZ_FLOAT:
        case ISZ_IFLOAT:
            return p->a_float;
        case ISZ_DOUBLE:
        case ISZ_IDOUBLE:
            return p->a_double;
        case ISZ_LDOUBLE:
        case ISZ_ILDOUBLE:
            return p->a_longdouble;
        case ISZ_CFLOAT:
            return 1;
        case ISZ_CDOUBLE:
            return 1;
        case ISZ_CLDOUBLE:
            return 1;
        default:
            return 1;
    }
}

/*-------------------------------------------------------------------------*/
Optimizer::SimpleSymbol* varsp(Optimizer::SimpleExpression* node)
{
    if (!node)
        return 0;
    switch (node->type)
    {
        case Optimizer::se_auto:
        case Optimizer::se_pc:
        case Optimizer::se_global:
        case Optimizer::se_tempref:
        case Optimizer::se_threadlocal:
            return node->sp;
        default:
            return 0;
    }
}

Optimizer::IMODE* make_immed(int size, long long i)
/*
 *      make a node to reference an immediate value i.
 */

{
    int index = ((unsigned long long)i) % (sizeof(immed_list) / sizeof(immed_list[0]));
    LIST* a = immed_list[index];
    Optimizer::IMODE* ap;
    while (a)
    {
        ap = (Optimizer::IMODE*)a->data;
        if (ap->offset->i == i && size == ap->size)
            return ap;
        a = a->next;
    }
    ap = Allocate<Optimizer::IMODE>();
    ap->mode = i_immed;
    ap->offset = Allocate<Optimizer::SimpleExpression>();
    ap->offset->type = Optimizer::se_i;
    ap->offset->i = i;
    ap->size = size;
    a = Allocate<LIST>();
    a->data = ap;
    a->next = immed_list[index];
    immed_list[index] = a;
    return ap;
}

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* make_fimmed(int size, FPF f)
/*
 *      make a node to reference an immediate value i.
 */

{
    Optimizer::IMODE* ap = Allocate<Optimizer::IMODE>();
    ap->mode = i_immed;
    ap->offset = Allocate<Optimizer::SimpleExpression>();
    ap->offset->type = Optimizer::se_f;
    ap->offset->f = f;
    ap->offset->sizeFromType = size;
    ap->size = size;
    return ap;
}

/*-------------------------------------------------------------------------*/

Optimizer::IMODE* make_parmadj(long i)
/*
 *			make a direct immediate, e.g. for parmadj
 */
{
    Optimizer::IMODE* ap = Allocate<Optimizer::IMODE>();
    ap->mode = i_immed;
    ap->offset = Allocate<Optimizer::SimpleExpression>();
    ap->offset->type = Optimizer::se_i;
    ap->offset->i = i;
    return ap;
}

/*-------------------------------------------------------------------------*/

Optimizer::SimpleExpression* tempenode(void)
{
    Optimizer::SimpleSymbol* sym;
    char buf[256];
    sym = Allocate<Optimizer::SimpleSymbol>();

    sym->storage_class = scc_temp;
    my_sprintf(buf, "$$t%d", tempCount);
    sym->name = sym->outputName = litlate(buf);
    sym->i = tempCount++;
    Optimizer::SimpleExpression* rv = Allocate<Optimizer::SimpleExpression>();
    rv->type = Optimizer::se_tempref;
    rv->sp = sym;
    return rv;
}
Optimizer::IMODE* tempreg(int size, int mode)
/*
 * create a temporary register
 */
{
    Optimizer::IMODE* ap;
    ap = Allocate<Optimizer::IMODE>();
    ap->offset = tempenode();
    ap->offset->sp->tp = Allocate<Optimizer::SimpleType>();
    ap->offset->sp->tp->type = st_i;
    ap->offset->sp->tp->size = sizeFromISZ(size);
    ap->size = size;
    if (mode)
    {
        ap->mode = i_immed;
        ap->offset->sp->imaddress = ap;
    }
    else
    {
        ap->mode = i_direct;
        ap->offset->sp->imvalue = ap;
    }
    return ap;
}

Optimizer::IMODE* GetLoadTemp(Optimizer::IMODE* dest)
{
    auto it = loadHash.find(dest);
    if (it != loadHash.end())
        return it->second;
    return nullptr;
}
Optimizer::IMODE* LookupStoreTemp(Optimizer::IMODE* dest, Optimizer::IMODE* src) { return src; }
Optimizer::IMODE* LookupLoadTemp(Optimizer::IMODE* dest, Optimizer::IMODE* source)
{
    (void)dest;

    if (Optimizer::chosenAssembler->arch->denyopts & DO_UNIQUEIND)
    {
        return tempreg(source->size, 0);
    }
    if ((source->mode != i_immed) && (source->offset->type != Optimizer::se_tempref || source->mode == i_ind))
    {
        if (source->bits || (source->size >= ISZ_FLOAT && !Optimizer::chosenAssembler->arch->hasFloatRegs))
        {
            source = tempreg(source->size, false);
        }
        else
        {
            Optimizer::IMODE* found = GetLoadTemp(source);
            if (!found)
            {
                found = tempreg(source->size, false);
                found->offset->sp->loadTemp = true;
                found->vol = source->vol;
                found->restricted = source->restricted;
                loadHash[source] = found;
            }
            return found;
        }
    }
    return source;
}
Optimizer::IMODE* LookupImmedTemp(Optimizer::IMODE* dest, Optimizer::IMODE* source) { return source; }
Optimizer::IMODE* LookupCastTemp(Optimizer::IMODE* im, int size)
{
    if (im->mode != i_immed)
    {
        CASTTEMP ch;
        int hash;
        ch.im = im;
        ch.size = size;
        auto it = castHash.find(&ch);
        if (it != castHash.end())
            return it->second;
        auto ch1 = Allocate<CASTTEMP>();
        *ch1 = ch;
        return castHash[ch1] = tempreg(size, 0);
    }
    else
    {
        return tempreg(size, 0);
    }
}
Optimizer::SimpleExpression* simpleExpressionNode(enum Optimizer::se_type type, Optimizer::SimpleExpression* left,
                                                  Optimizer::SimpleExpression* right)
{
    Optimizer::SimpleExpression* rv = Allocate<Optimizer::SimpleExpression>();
    rv->type = type;
    rv->left = left;
    rv->right = right;
    return rv;
}
Optimizer::SimpleExpression* simpleIntNode(enum Optimizer::se_type type, unsigned long long i)
{
    Optimizer::SimpleExpression* rv = Allocate<Optimizer::SimpleExpression>();
    rv->type = type;
    rv->i = i;
    return rv;
}
Optimizer::IMODE* indnode(Optimizer::IMODE* ap1, int size)
/*
 * copy the address mode and change it to an indirect type
 *
 */
{
    Optimizer::IMODE* ap = nullptr;
    Optimizer::SimpleSymbol* sym;
    if (ap1->mode == i_ind)
    {
        ap = LookupLoadTemp(ap1, ap1);
        if (ap != ap1)
        {
            gen_icode(i_assn, ap, ap1, nullptr);
        }
        ap1 = ap;
    }
    if (ap1->bits)
    {
        Optimizer::IMODE* ap2 = Allocate<Optimizer::IMODE>();
        *ap2 = *ap1;

        if (ap1->mode == i_immed)
        {
            ap2->mode = i_direct;
        }
        else if (ap1->mode == i_direct)
        {
            ap2->mode = i_ind;
        }
        ap2->ptrsize = ap1->size;
        ap2->size = size;
        return ap2;
    }
    if (Optimizer::chosenAssembler->arch->denyopts & DO_UNIQUEIND)
    {
        Optimizer::IMODE* ap2 = tempreg(ap1->size, 0);
        gen_icode(i_assn, ap2, ap1, nullptr);
        ap1 = Allocate<Optimizer::IMODE>();
        *ap1 = *ap2;
        ap1->mode = i_ind;
        ap1->ptrsize = ap1->size;
        ap1->size = size;
        return ap1;
    }
    sym = varsp(ap1->offset);
    if (sym && ap1->mode == i_immed && sym->imvalue && sym->imvalue->size == size)
    {
        ap = sym->imvalue;
        ap->offset = ap1->offset;
    }
    else
    {
        Optimizer::IMODELIST* iml = nullptr;
        if (sym)
        {
            switch (sym->storage_class)
            {
                case scc_auto:
                case scc_parameter:
                case scc_global:
                case scc_localstatic:
                case scc_static:
                case scc_external: {
                    Optimizer::IMODE* im = LookupLoadTemp(ap1, ap1);
                    if (im != ap1)
                        gen_icode(i_assn, im, ap1, nullptr);
                    ap1 = im;
                    sym = im->offset->sp;
                }
                break;
                default:
                    break;
            }
            iml = sym->imind;
            while (iml)
            {
                if (iml->im->size == size)
                {
                    ap = iml->im;
                    break;
                }
                iml = iml->next;
            }
        }
        if (!iml)
        {
            if (sym)
            {
                Optimizer::SimpleExpression* node1 = ap1->offset;
                ap = Allocate<Optimizer::IMODE>();
                *ap = *ap1;
                ap->offset = node1;
                ap->retval = false;
            }
            else
            {
                ap = Allocate<Optimizer::IMODE>();
                *ap = *ap1;
                ap->retval = false;
            }
            ap->ptrsize = ap1->size;
            ap->size = size;
            if (ap1->mode == i_immed)
            {
                ap->mode = i_direct;
                if (sym)
                {
                    if (sym->imvalue && sym->imvalue->size == ap->size)
                        ap = sym->imvalue;
                    else if (sym && !sym->isstructured && sym->sizeFromType == ap->size)
                        sym->imvalue = ap;
                }
            }
            else
            {
                ap->mode = i_ind;
                if (sym)
                {
                    Optimizer::IMODELIST* iml;
                    iml = Allocate<Optimizer::IMODELIST>();
                    iml->next = sym->imind;
                    sym->imind = iml;
                    iml->im = ap;
                }
            }
        }
    }
    return ap;
}
int pwrof2(long long i)
/*
 *      return which power of two i is or -1.
 */
{
    if (Optimizer::chosenAssembler->arch->denyopts & DO_NOMULTOSHIFT)
        return -1;
    if (i > 1)
    {
        int top = sizeof(shifts) / sizeof(long long);
        int bottom = -1;
        while (top - bottom > 1)
        {
            int mid = (top + bottom) / 2;
            if (i < shifts[mid])
            {
                top = mid;
            }
            else
            {
                bottom = mid;
            }
        }
        if (bottom <= 0) /* ignore the oth power*/
            return -1;
        if (i == shifts[bottom])
            return bottom;
    }
    return -1;
}

/*-------------------------------------------------------------------------*/

long long mod_mask(int i)
/*
 *      make a mod mask for a power of two.
 */
{
    if (i >= sizeof(shifts) / sizeof(long long))
        return (long long)-1;
    return shifts[i] - 1;
}

static char* write_llong(char* dest, unsigned long long val)
{
    char obuf[256], *p = obuf + sizeof(obuf);
    *--p = 0;
    if (val)
    {
        while (val)
        {
            unsigned aa = val % 10;
            *--p = '0' + aa;
            val = val / 10;
        }
    }
    else
    {
        *--p = '0';
    }
    strcpy(dest, p);
    return dest + strlen(dest);
}
static char* write_int(char* dest, unsigned val)
{
    char obuf[256], *p = obuf + sizeof(obuf);
    *--p = 0;
    if (val)
    {
        while (val)
        {
            unsigned aa = val % 10;
            *--p = '0' + aa;
            val = val / 10;
        }
    }
    else
    {
        *--p = '0';
    }
    strcpy(dest, p);
    return dest + strlen(dest);
}
void my_sprintf(char* dest, const char* fmt, ...)
{
    va_list aa;
    va_start(aa, fmt);
    while (*fmt)
    {
        const char* q = strchr(fmt, '%');
        if (!q)
            q = fmt + strlen(fmt);
        memcpy(dest, fmt, q - fmt);
        dest += q - fmt;
        fmt += q - fmt;
        if (*fmt)
        {
            fmt++;
            switch (*fmt++)
            {
                unsigned long long val;
                unsigned val1;
                char* str;
                case 'l':
                    while (*fmt == 'd' || *fmt == 'l')
                        fmt++;
                    val = va_arg(aa, unsigned long long);
                    dest = write_llong(dest, val);
                    break;
                case 'd':
                case 'u':
                    val1 = va_arg(aa, unsigned);
                    dest = write_int(dest, val1);
                    break;
                case 'c':
                    val1 = va_arg(aa, unsigned);
                    *dest++ = val1;
                    break;
                case 's':
                    str = va_arg(aa, char*);
                    strcpy(dest, str);
                    dest += strlen(dest);
                    break;
                default:
                    fmt++;
                    break;
            }
        }
    }
    *dest = 0;
}

void cacheTempSymbol(Optimizer::SimpleSymbol* sym)
{
    if (sym->anonymous && sym->storage_class != scc_parameter)
    {
        if (sym->allocate && !sym->inAllocTable)
        {
            temporarySymbols.push_back(sym);
            sym->inAllocTable = true;
        }
    }
}
}  // namespace Optimizer