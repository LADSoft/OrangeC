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

#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <ctype.h>
#include <richedit.h>

#include "header.h"
#include "cvexefmt.h"
#include "dbgtype.h"

extern PROCESS* activeProcess;
extern enum DebugState uState;

unsigned bitmask[] = {
    1,        3,        7,         0xf,       0x1f,      0x3f,      0x7f,       0xff,       0x1ff,      0x3ff,      0x7ff,
    0xfff,    0x1fff,   0x3fff,    0x7fff,    0xffff,    0x1ffff,   0x3ffff,    0x7ffff,    0xfffff,    0x1fffff,   0x3fffff,
    0x7fffff, 0xffffff, 0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff, 0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff,

};

int ReadValue(int address, void* val, int size, VARINFO* var)
{
    DWORD len;
    if (address < 0x1000 && var->thread)
    {
        CONTEXT* regs = &var->thread->regs;
        // register
        switch (address)
        {
            case DB_REG_AL:
                *(char*)val = regs->Eax & 0xff;
                return 1;
            case DB_REG_CL:
                *(char*)val = regs->Ecx & 0xff;
                return 1;
            case DB_REG_DL:
                *(char*)val = regs->Edx & 0xff;
                return 1;
            case DB_REG_BL:
                *(char*)val = regs->Ebx & 0xff;
                return 1;
            case DB_REG_AH:
                *(char*)val = (regs->Eax >> 8) & 0xff;
                return 1;
            case DB_REG_CH:
                *(char*)val = (regs->Ecx >> 8) & 0xff;
                return 1;
            case DB_REG_DH:
                *(char*)val = (regs->Edx >> 8) & 0xff;
                return 1;
            case DB_REG_BH:
                *(char*)val = (regs->Ebx >> 8) & 0xff;
                return 1;
            case DB_REG_AX:
                *(short*)val = regs->Eax & 0xffff;
                return 2;
            case DB_REG_CX:
                *(short*)val = regs->Ecx & 0xffff;
                return 2;
            case DB_REG_DX:
                *(short*)val = regs->Edx & 0xffff;
                return 2;
            case DB_REG_BX:
                *(short*)val = regs->Ebx & 0xffff;
                return 2;
            case DB_REG_SP:
                *(short*)val = regs->Esp & 0xffff;
                return 2;
            case DB_REG_BP:
                *(short*)val = regs->Ebp & 0xffff;
                return 2;
            case DB_REG_SI:
                *(short*)val = regs->Esi & 0xffff;
                return 2;
            case DB_REG_DI:
                *(short*)val = regs->Edi & 0xffff;
                return 2;
            case DB_REG_EAX:
                *(int*)val = regs->Eax;
                return 4;
            case DB_REG_ECX:
                *(int*)val = regs->Ecx;
                return 4;
            case DB_REG_EDX:
                *(int*)val = regs->Edx;
                return 4;
            case DB_REG_EBX:
                *(int*)val = regs->Ebx;
                return 4;
            case DB_REG_ESP:
                *(int*)val = regs->Esp;
                return 4;
            case DB_REG_EBP:
                *(int*)val = regs->Ebp;
                return 4;
            case DB_REG_ESI:
                *(int*)val = regs->Esi;
                return 4;
            case DB_REG_EDI:
                *(int*)val = regs->Edi;
                return 4;
            default:
                return 0;
        }
    }
    else
    {
        ReadProcessMemory(activeProcess->hProcess, (LPVOID)address, (LPVOID)val, size, &len);
        return len;
    }
    return 0;
}

int WriteValue(int address, void* value, int size, CONTEXT* regs)
{
    DWORD len;
    if (address < 0x1000)
    {
        // register
        int val = *(int*)value;
        switch (address)
        {
            case DB_REG_AL:
                regs->Eax &= 0xffffff00;
                regs->Eax |= (unsigned char)val;
                return 1;
            case DB_REG_CL:
                regs->Ecx &= 0xffffff00;
                regs->Ecx |= (unsigned char)val;
                return 1;
            case DB_REG_DL:
                regs->Edx &= 0xffffff00;
                regs->Edx |= (unsigned char)val;
                return 1;
            case DB_REG_BL:
                regs->Ebx &= 0xffffff00;
                regs->Ebx |= (unsigned char)val;
                return 1;
            case DB_REG_AH:
                regs->Eax &= 0xffff00ff;
                regs->Eax |= ((unsigned char)val) << 8;
                return 1;
            case DB_REG_CH:
                regs->Ecx &= 0xffff00ff;
                regs->Ecx |= ((unsigned char)val) << 8;
                return 1;
            case DB_REG_DH:
                regs->Edx &= 0xffff00ff;
                regs->Edx |= ((unsigned char)val) << 8;
                return 1;
            case DB_REG_BH:
                regs->Ebx &= 0xffff00ff;
                regs->Ebx |= ((unsigned char)val) << 8;
                return 1;
            case DB_REG_AX:
                regs->Eax &= 0xffff0000;
                regs->Eax |= (unsigned short)val;
                return 2;
            case DB_REG_CX:
                regs->Ecx &= 0xffff0000;
                regs->Ecx |= (unsigned short)val;
                return 2;
            case DB_REG_DX:
                regs->Edx &= 0xffff0000;
                regs->Edx |= (unsigned short)val;
                return 2;
            case DB_REG_BX:
                regs->Ebx &= 0xffff0000;
                regs->Ebx |= (unsigned short)val;
                return 2;
            case DB_REG_SP:
                regs->Esp &= 0xffff0000;
                regs->Esp |= (unsigned short)val;
                return 2;
            case DB_REG_BP:
                regs->Ebp &= 0xffff0000;
                regs->Ebp |= (unsigned short)val;
                return 2;
            case DB_REG_SI:
                regs->Esi &= 0xffff0000;
                regs->Esi |= (unsigned short)val;
                return 2;
            case DB_REG_DI:
                regs->Edi &= 0xffff0000;
                regs->Edi |= (unsigned short)val;
                return 2;
            case DB_REG_EAX:
                regs->Eax = val;
                return 4;
            case DB_REG_ECX:
                regs->Ecx = val;
                return 4;
            case DB_REG_EDX:
                regs->Edx = val;
                return 4;
            case DB_REG_EBX:
                regs->Ebx = val;
                return 4;
            case DB_REG_ESP:
                regs->Esp = val;
                return 4;
            case DB_REG_EBP:
                regs->Ebp = val;
                return 4;
            case DB_REG_ESI:
                regs->Esi = val;
                return 4;
            case DB_REG_EDI:
                regs->Edi = val;
                return 4;
            default:
                return 0;
        }
    }
    else
    {
        WriteProcessMemory(activeProcess->hProcess, (LPVOID)address, (LPVOID)value, size, &len);
        return len;
    }
}

int GetEnumValue(DEBUG_INFO* dbg_info, VARINFO* info, char* text) { return LookupEnumValue(dbg_info, info->type, text); }
//-------------------------------------------------------------------------

int HintBasicValue(VARINFO* info, int* signedtype, char* data)
{
    int sz = -1;
    int type = info->type;
    int rv = type;
    if ((type >= ePChar && type <= ePULongLong) || (type >= ePFloat && type <= ePComplexLongDouble))
    {
        ReadValue(info->address, data, 4, info);
        *signedtype = FALSE;
        return type;
    }
    *signedtype = type != eBool && type != eUChar && type != eUShort && type != eWcharT && type != eChar16T && type != eUInt &&
                  type != eChar32T && type != eULong && type != eULongLong;
    switch (type)
    {
        case eBool:
            sz = 1;
            break;
        case eChar:
        case eUChar:
            rv = eInt;
            sz = 1;
            break;
        case eShort:
        case eUShort:
        case eWcharT:
        case eChar16T:
            rv = eInt;
            sz = 2;
            break;
        case eInt:
        case eUInt:
        case eLong:
        case eULong:
        case eChar32T:
            rv = eInt;
            sz = 4;
            break;
        case eLongLong:
        case eULongLong:
            rv = eLongLong;
            sz = 8;
            break;
        case eFloat:
        case eImaginary:
            sz = 4;
            break;
        case eDouble:
        case eImaginaryDouble:
            sz = 8;
            break;
        case eLongDouble:
        case eImaginaryLongDouble:
            sz = 10;
            break;
        case eComplex:
            sz = 8;
            break;
        case eComplexDouble:
            sz = 16;
            break;
        case eComplexLongDouble:
            sz = 20;
            break;
        default:
            sz = 4;
            break;
    }
    if (sz < 4)
        memset(data, 0, 4);
    ReadValue(info->address, data, sz, info);
    if (*signedtype)
    {
        if (sz == 1 && (data[0] & 0x80))
            data[1] = 0xff, data[2] = 0xff, data[3] = 0xff;
        else if (sz == 2 && (data[1] & 0x80))
            data[2] = 0xff, data[3] = 0xff;
    }
    return sz < 4 ? eInt : rv;
}

//-------------------------------------------------------------------------

void HintEnum(DEBUG_INFO* dbg_info, VARINFO* info, char* buf, int toenum, int onevalue)
{
    char name[256];
    int v;
    int signedtype;
    name[0] = 0;
    //	info->size = DeclType(dbg_info, info);
    HintBasicValue(info, &signedtype, &v);
    LookupEnumName(dbg_info, info->type, name, v);
    if (toenum)
        sprintf(buf, "ENUM: %s(%u)", name, v);
    else if (onevalue)
        sprintf(buf, "%s", name);
    else
        sprintf(buf, "%s(%u)", name, v);
}

//-------------------------------------------------------------------------

int HintBf(VARINFO* info, int* signedtype)
{
    char data[20];
    int v = 0;
    HintBasicValue(info, signedtype, &data[0]);
    v = *(int*)data;
    if (*signedtype)
    {
        v <<= 32 - info->bitstart - info->bitlength;
        v >>= 32 - info->bitlength;
    }
    else
    {
        v >>= info->bitstart;
        v &= bitmask[info->bitlength - 1];
    }
    return v;
}

//-------------------------------------------------------------------------

void GetStringValue(VARINFO* info, char* buf, int len, int address)
{
    int i;
    char buf2[5000], *p;
    memset(buf2, 0, sizeof(buf2));
    if (info->type == eChar && ReadValue(address, buf2, len, info))
    {
        int j;
        buf += strlen(buf);
        *buf++ = '\"';
        p = buf2;
        for (i = 0; i < len && *p; i++)
            if (*p >= 0x20 && *p < 0x7f)
                *buf++ = *p++;
            else
            {
                char temp[5];
                int l;
                *buf++ = '\\';
                itoa((unsigned char)*p++, temp, 8);
                l = strlen(temp);
                for (j = 0; j < (3 - l); j++)
                    *buf++ = '0';
                for (j = 0; j < l; j++)
                    *buf++ = temp[j];
            }
        *buf++ = '"';
        *buf++ = 0;
    }
}

//-------------------------------------------------------------------------

void HintValue(DEBUG_INFO* dbg_info, VARINFO* info, char* buf)
{
restart:
    if (info->outofscope)
        strcpy(buf, "out of scope");
    else if (info->constant)
    {
        switch (info->type)
        {
            case eBool:
            case eChar:
            case eUChar:
            case eShort:
            case eUShort:
            case eWcharT:
            case eChar16T:
            case eInt:
            case eUInt:
            case eLong:
            case eULong:
            case eChar32T:
            case eLongLong:
            case eULongLong:
                sprintf(buf, "%lld(%llx)", info->ival, info->ival);
                break;
            case eFloat:
            case eImaginary:
            case eDouble:
            case eImaginaryDouble:
            case eLongDouble:
            case eImaginaryLongDouble:
                sprintf(buf, "%f", (double)info->fval);
                break;
            case eComplex:
            case eComplexDouble:
            case eComplexLongDouble:
                sprintf(buf, "%f + %f * I", (double)info->fval, (double)info->fvali);
                break;
            default:
                sprintf(buf, "0x%x", info->address);
                break;
        }
    }
    else if (info->structure)
    {
        sprintf(buf, "STRUCTURE: 0x%x", info->address);
    }
    else if (info->unionx)
    {
        sprintf(buf, "UNION: 0x%x", info->address);
    }
    else if (info->pointer)
    {
        int val;
        if (info->lref || info->rref)
        {
            val = info->derefaddress;
            if (val == -1)
                if (!ReadValue(info->address, &val, 4, info))
                {
                    sprintf(buf, "REFERENCE: <UNKNOWN>");
                    return;
                }
            if (info->type <= eReservedTop && info->type > eVoid)
            {
                info->address = val;
                info->lref = info->rref = info->pointer = FALSE;

                goto restart;
            }
            else
            {
                if (info->lref)
                    sprintf(buf, "LVALUE REFERENCE: 0x%x", val);
                else
                    sprintf(buf, "RVALUE REFERENCE: 0x%x", val);
            }
        }
        else if (info->derefaddress != -1)
        {
            sprintf(buf, "POINTER: 0x%x ", info->derefaddress);
            GetStringValue(info, buf + strlen(buf), 32, info->derefaddress);
        }
        else if (ReadValue(info->address, &val, 4, info))
        {
            sprintf(buf, "POINTER: 0x%x ", val);
            GetStringValue(info, buf + strlen(buf), 32, val);
        }
        else
            sprintf(buf, "POINTER: <UNKNOWN>");
    }
    else if (info->enumx)
        HintEnum(dbg_info, info, buf, TRUE, FALSE);
    else if (info->bitfield)
    {
        int signedtype;
        int v = HintBf(info, &signedtype);
        if (signedtype)
            sprintf(buf, "%d(0x%x)", v, v);
        else
            sprintf(buf, "%u(0x%x)", v, v);
    }
    else if (info->array)
    {
        sprintf(buf, "ARRAY: 0x%x ", info->address);
        GetStringValue(info, buf + strlen(buf), 32, info->address);
    }
    else
    {
        int signedtype;
        char buf1[20];
        LLONG_TYPE v;
        switch (HintBasicValue(info, &signedtype, buf1))
        {
            case eLongLong:
                v = *(LLONG_TYPE*)buf1;
                if (signedtype)
                    sprintf(buf, "%lld(0x%llx)", v, v);
                else
                    sprintf(buf, "%llu(0x%llx)", v, v);
                break;
            default:
                sprintf(buf, "unknown type");
                break;
            case eInt:
                v = *(int*)buf1;
                if (signedtype)
                    sprintf(buf, "%d(0x%x)", (int)v, (int)v);
                else
                    sprintf(buf, "%u(0x%x)", (int)v, (int)v);
                break;
            case eBool:
                if (buf1[0])
                    sprintf(buf, "True");
                else
                    sprintf(buf, "False");
                break;
            case eFloat:
            case eImaginary:
                sprintf(buf, "%f", (double)*(float*)buf1);
                break;
            case eLongDouble:
            case eImaginaryLongDouble:
                *(double*)buf1 = *(long double*)buf1;
                break;
            case eDouble:
            case eImaginaryDouble:
                sprintf(buf, "%f", *(double*)buf1);
                break;
            case eComplex:
                sprintf(buf, "%f + %f * I", (double)*(float*)buf1, (double)*(float*)(buf1 + 4));
                break;
            case eComplexDouble:
                sprintf(buf, "%f + %f * I", *(double*)buf1, *(double*)(buf1 + 8));
                break;
            case eComplexLongDouble:
                sprintf(buf, "%f + %f * I", (double)*(long double*)buf1, (double)*(long double*)(buf1 + 10));
                break;
        }
    }
}

void SimpleTypeName(char* name, int type)
{
    char* rv;
    switch (type)
    {
        case eBool:
            rv = "bool";
            break;
        case eChar:
            rv = "char";
            break;
        case eUChar:
            rv = "unsigned char";
            break;
        case eShort:
            rv = "short";
            break;
        case eUShort:
            rv = "unsigned short";
            break;
        case eWcharT:
            rv = "wchar_t";
            break;
        case eChar16T:
            rv = "char16_t";
            break;
        case eInt:
            rv = "int";
            break;
        case eUInt:
            rv = "unsigned int";
            break;
        case eLong:
            rv = "long";
            break;
        case eULong:
            rv = "unsigned long";
            break;
        case eChar32T:
            rv = "char32_t";
            break;
        case eLongLong:
            rv = "long long";
            break;
        case eULongLong:
            rv = "unsigned long long";
            break;
        case eFloat:
            rv = "float";
            break;
        case eImaginary:
            rv = "imaginary float";
            break;
        case eDouble:
            rv = "double";
            break;
        case eImaginaryDouble:
            rv = "imaginary double";
            break;
        case eLongDouble:
            rv = "long double";
            break;
        case eImaginaryLongDouble:
            rv = "imaginary long double";
            break;
        case eComplex:
            rv = "complex float";
            break;
        case eComplexDouble:
            rv = "complex double";
            break;
        case eComplexLongDouble:
            rv = "complex long double";
            break;
        case ePChar:
            rv = "char *";
            break;
        case ePUChar:
            rv = "unsigned char *";
            break;
        case ePShort:
            rv = "short *";
            break;
        case ePUShort:
            rv = "unsigned short *";
            break;
        case ePWcharT:
            rv = "wchar_t *";
            break;
        case ePChar16T:
            rv = "char16_t *";
            break;
        case ePInt:
            rv = "int *";
            break;
        case ePUInt:
            rv = "unsigned int *";
            break;
        case ePLong:
            rv = "long *";
            break;
        case ePULong:
            rv = "unsigned long *";
            break;
        case ePChar32T:
            rv = "char32_t *";
            break;
        case ePLongLong:
            rv = "long long *";
            break;
        case ePULongLong:
            rv = "unsigned long long *";
            break;
        case ePFloat:
            rv = "float *";
            break;
        case ePImaginary:
            rv = "imaginary float *";
            break;
        case ePDouble:
            rv = "double *";
            break;
        case ePImaginaryDouble:
            rv = "imaginary double *";
            break;
        case ePLongDouble:
            rv = "long double *";
            break;
        case ePImaginaryLongDouble:
            rv = "imaginary long double *";
            break;
        case ePComplex:
            rv = "complex float *";
            break;
        case ePComplexDouble:
            rv = "complex double *";
            break;
        case ePComplexLongDouble:
            rv = "complex long double *";
            break;
        default:
            rv = "Unknown Type";
            break;
    }
    strcpy(name, rv);
}
//-------------------------------------------------------------------------

char* SymTypeName(char* buf, DEBUG_INFO* info, VARINFO* v)
{
    char* p = buf;
    p[0] = 0;
    if (!v)
        return buf;
    if (v->udt && v->structtag[0] != 0)
    {
        strncpy(buf, v->structtag, 256);
        buf[255] = 0;
        unmanglename(buf);
        return buf;
    }
    if (v->constant)
    {
        if (v->type == ePVoid || v->type == eVoid)
            strcpy(buf, "void * ");
        else
            strcpy(buf, "constant ");
        return buf;
    }
    else if (v->pointer)
    {
        if (v->type < eReservedTop)
        {
            SimpleTypeName(buf, v->type);
            if (v->lref)
                strcat(buf, "&");
            else if (v->rref)
                strcat(buf, "&&");
            else
                strcat(buf, "* ");
        }
        else
        {
            int replace = !v->subtype;
            if (!v->subtype)
                ExpandPointerInfo(info, v);
            SymTypeName(buf + strlen(buf), info, v->subtype);
            if (v->lref)
                strcat(buf, "&");
            else if (v->rref)
                strcat(buf, "&&");
            else
                strcat(buf, "* ");
            if (replace)
            {
                FreeVarInfo(v->subtype);
                v->subtype = 0;
            }
        }
    }
    else if (v->type < eReservedTop)
    {
        SimpleTypeName(buf, v->type);
    }
    else if (v->unionx)
    {
        sprintf(buf, "union ");
        p = buf + strlen(buf);
        strncpy(p, v->structtag, 256);
        p[255] = 0;
        unmanglename(p);
    }
    else if (v->structure)
    {
        sprintf(buf, "struct ");
        p = buf + strlen(buf);
        strncpy(p, v->structtag, 256);
        p[255] = 0;
        unmanglename(p);
    }
    else if (v->enumx)
    {
        sprintf(buf, "enum ");
        p = buf + strlen(buf);
        strncpy(p, v->structtag, 256);
        p[255] = 0;
        unmanglename(p);
    }
    if (v->array)
    {
        strcat(buf, "[] ");
    }
    if (v->bitfield)
    {
        p = buf + strlen(buf);
        sprintf(p, ":%d", v->bitlength);
    }
    return buf;
}
