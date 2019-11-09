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
#pragma once
#include "iexpr.h"
#include "beinterf.h"
#include "../occ/Winmode.h"
#include <deque>
#include <map>

enum DataType
{
    DT_NONE,
    DT_SEG,
    DT_SEGEXIT,
    DT_DEFINITION,
    DT_LABELDEFINITION,
    DT_RESERVE,
    DT_SYM,
    DT_SRREF,
    DT_PCREF,
    DT_FUNCREF,
    DT_LABEL,
    DT_LABDIFFREF,
    DT_FUNC,
    DT_STRING,
    DT_BIT,
    DT_BOOL,
    DT_BYTE,
    DT_USHORT,
    DT_UINT,
    DT_ULONG,
    DT_ULONGLONG,
    DT_16,
    DT_32,
    DT_ENUM,
    DT_FLOAT,
    DT_DOUBLE,
    DT_LDOUBLE,
    DT_CFLOAT,
    DT_CDOUBLE,
    DT_CLONGDOUBLE,
    DT_ADDRESS,
    DT_VIRTUAL,
    DT_ENDVIRTUAL,
    DT_ALIGN,
    DT_VTT,
    DT_IMPORTTHUNK,
    DT_VC1
};
struct FunctionData
{
    SimpleSymbol* name;
    SimpleExpression* objectArray_exp;
    std::vector<SimpleSymbol*> temporarySymbols;
    std::vector<SimpleSymbol*> variables;
    std::vector<IMODE*> imodeList;
    std::unordered_map<IMODE*, IMODE*> loadHash;
    TEMP_INFO **tempInfo;
    int tempCount;
    int blockCount;
    int exitBlock;
    QUAD *instructionList;
    int setjmp_used : 1;
    int hasAssembly : 1;
};
struct BaseData
{
    enum
    {
        DF_GLOBAL=1,
        DF_STATIC=2,
        DF_LOCALSTATIC=4,
        DF_EXPORT=8
    };
    DataType type;
    union
    {
        struct
        {
            SimpleSymbol* sym;
            int i;
        } symbol;
        FunctionData *funcData;
        struct
        {
            int l1, l2;
        } diff;
        e_sg segtype;
        long long i;
        FPF f;
        struct
        {
            char *str;
            int i;
        } astring;
        struct
        {
            FPF r;
            FPF i;
        } c;
    };
};


static const char *magic = "LSIL";
static const int fileVersion = 1;


enum e_sbt {
    SBT_PARAMS,
    SBT_XPARAMS,
    SBT_TEXT,
    SBT_STRUCTURES,
    SBT_GLOBALSYMS,
    SBT_EXTERNALS,
    SBT_TYPES,
    SBT_MSILPROPS,
    SBT_TYPEDEFS,
    SBT_BROWSEFILES,
    SBT_BROWSEINFO,
    SBT_IMODES,
    SBT_DATA,
};

enum e_stt {
    STT_INT,
    STT_FLOAT,
    STT_TYPE,
    STT_SYMBOL,
    STT_STRING,
    STT_EXPRESSION,
    STT_BASE,
    STT_BROWSEFILE,
    STT_BROWSEINFO,
    STT_OPERAND,
    STT_INSTRUCTION,

};
