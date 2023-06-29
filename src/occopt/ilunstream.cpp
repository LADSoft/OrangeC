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

#include <cstdint>
#include <cstdio>
#include <malloc.h>
#include <cstring>
#include <climits>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "ildata.h"
#include "ioptimizer.h"
#include "SharedMemory.h"
#include "../occ/winmode.h"
#include "../occ/be.h"
#include <deque>
#include <map>
#include <unordered_map>
#include <stdexcept>
#include "ildata.h"
#include "iblock.h"
#include "memory.h"
#include "OptUtils.h"

namespace Optimizer
{
static std::list<std::string> textRegion;
static std::unordered_map<std::string, int> cachedText;
static size_t textOffset;
static FunctionData *current, *lastFunction;
static std::map<int, std::string> texts;
static std::vector<Optimizer::SimpleSymbol*> temps;

static unsigned char* streamPointer;
static SharedMemory* shared;
static unsigned top;

static int
    inputPos;  // it is not intended we ever reset this, as multiple files could be streamed one after the other and we are going to
// read them in order
inline void dothrow()
{
    std::runtime_error e("");
    throw e;
}
inline static int UnstreamByte()
{
    if (inputPos >= top)
    {
        top += shared->ViewWindowSize();
        streamPointer = shared->GetMapping(inputPos);
    }
    return streamPointer[inputPos++];
}
inline static void UnstreamBlockType(int blockType, bool end)
{
    int n = UnstreamByte();
    if (n != blockType + (end ? 0x80 : 0x40))
    {
        printf("bt\n");
        dothrow();
    }
}

inline static int UnstreamIndex()
{
    int value = UnstreamByte() << 8;
    value |= UnstreamByte();
    if (value & 0x8000)
    {
        value &= 0x7fff;
        value <<= 16;
        value |= UnstreamByte() << 8;
        value |= UnstreamByte() << 0;
    }
    return value;
}
inline static int UnstreamTextIndex() { return UnstreamIndex(); }
inline static void UnstreamString(std::string& value)
{
    int v = UnstreamIndex();
    value.resize(v, 0);
    for (auto&& c : value)
        c = UnstreamByte();
}
static void UnstreamStringList(std::list<std::string>& list)
{
    int v = UnstreamIndex();
    for (int i = 0; i < v; i++)
    {
        list.push_back("");
        UnstreamString(list.back());
    }
}
static void UnstreamBuffer(void* buf, int len)
{
    if (inputPos + len >= top)
    {
        top += shared->ViewWindowSize();
        streamPointer = shared->GetMapping(inputPos);
    }
    memcpy(buf, &streamPointer[inputPos], len);
    inputPos += len;
}
inline static void UnstreamIntValue(void* buf, int len)
{
    UnstreamBlockType(STT_INT, false);
    for (int i = len - 1; i >= 0; i--)
        ((unsigned char*)buf)[i] = UnstreamByte();
    UnstreamBlockType(STT_INT, true);
}
static void UnstreamFloatValue(FPF& fv)
{
    UnstreamBlockType(STT_FLOAT, false);
    fv.type = UnstreamIndex();
    fv.sign = UnstreamIndex();
    UnstreamIntValue(&fv.exp, sizeof(int));
    for (int i = 0; i < INTERNAL_FPF_PRECISION; i++)
        fv.mantissa[i] = UnstreamIndex();
    UnstreamBlockType(STT_FLOAT, true);
}
static Optimizer::SimpleSymbol* UnstreamSymbol();
static LIST* UnstreamSymbolTable()
{
    LIST* syms = nullptr;
    int i = UnstreamIndex();
    LIST** p = &syms;
    for (; i > 0; i--)
    {
        *p = Allocate<LIST>();
        (*p)->data = (Optimizer::SimpleSymbol*)UnstreamIndex();
        p = &(*p)->next;
    }
    return syms;
}
static BaseList* UnstreamBases()
{
    BaseList* bases = nullptr;
    int i = UnstreamIndex();
    BaseList** p = &bases;
    for (; i > 0; i--)
    {
        *p = Allocate<BaseList>();
        UnstreamBlockType(STT_BASE, false);
        (*p)->sym = (Optimizer::SimpleSymbol*)UnstreamIndex();
        (*p)->offset = UnstreamIndex();
        UnstreamBlockType(STT_BASE, true);
        p = &(*p)->next;
    }
    return bases;
}
static Optimizer::SimpleType* UnstreamType()
{
    Optimizer::SimpleType* rv = nullptr;
    UnstreamBlockType(STT_TYPE, false);

    st_type type = (st_type)UnstreamIndex();
    if (type != st_none)
    {
        rv = Allocate<Optimizer::SimpleType>();
        rv->type = type;
        rv->size = UnstreamIndex();
        UnstreamIntValue(&rv->sizeFromType, sizeof(int));
        rv->bits = UnstreamIndex();
        rv->startbit = UnstreamIndex();
        rv->sp = (Optimizer::SimpleSymbol*)UnstreamIndex();
        rv->flags = UnstreamIndex();
        rv->btp = UnstreamType();
        rv->structuredAlias = UnstreamType();
    }
    UnstreamBlockType(STT_TYPE, true);
    return rv;
}
static Optimizer::SimpleSymbol* UnstreamSymbol()
{
    Optimizer::SimpleSymbol* rv = nullptr;
    UnstreamBlockType(STT_SYMBOL, false);
    int storage_class = UnstreamIndex();
    if (storage_class != scc_none)
    {
        rv = Allocate<Optimizer::SimpleSymbol>();
        rv->storage_class = (e_scc_type)storage_class;
        rv->name = (const char*)UnstreamTextIndex();
        rv->outputName = (const char*)UnstreamTextIndex();
        rv->importfile = (const char*)UnstreamTextIndex();
        rv->namespaceName = (const char*)UnstreamTextIndex();
        rv->msil = (const char*)UnstreamTextIndex();
        rv->i = UnstreamIndex();
        rv->regmode = UnstreamIndex();
        UnstreamIntValue(&rv->offset, 4);
        rv->label = UnstreamIndex();
        rv->templateLevel = UnstreamIndex();
        UnstreamIntValue(&rv->flags, 8);
        UnstreamIntValue(&rv->sizeFromType, sizeof(int));
        rv->align = UnstreamIndex();
        rv->size = UnstreamIndex();
        rv->parentClass = (Optimizer::SimpleSymbol*)UnstreamIndex();
        rv->tp = UnstreamType();
        rv->syms = UnstreamSymbolTable();
        rv->baseClasses = UnstreamBases();
    }
    UnstreamBlockType(STT_SYMBOL, true);
    return rv;
}
static Optimizer::SimpleSymbol* GetTempref(int n, int size, int sizeFromType)
{
    if (!temps[n])
    {
        Optimizer::SimpleSymbol* sym = temps[n] = Allocate<Optimizer::SimpleSymbol>();
        char buf[256];
        sym->storage_class = scc_temp;
        my_sprintf(buf, "$$t%d", n);
        sym->name = sym->outputName = litlate(buf);
        sym->tp = Allocate<Optimizer::SimpleType>();
        sym->tp->type = st_i;
        sym->tp->size = size;
        sym->tp->sizeFromType = sizeFromType;
        sym->i = n;
    }
    return temps[n];
}
static Optimizer::SimpleExpression* UnstreamExpression()
{
    Optimizer::SimpleExpression* rv = nullptr;
    UnstreamBlockType(STT_EXPRESSION, false);
    int type = UnstreamIndex();
    if (type != se_none)
    {
        rv = Allocate<Optimizer::SimpleExpression>();
        rv->type = (se_type)type;
        rv->flags = UnstreamIndex();
        UnstreamIntValue(&rv->sizeFromType, sizeof(int));
        switch (rv->type)
        {
            case se_i:
            case se_ui:
                UnstreamIntValue(&rv->i, 8);
                break;
            case se_f:
            case se_fi:
                UnstreamFloatValue(rv->f);
                break;
            case se_fc:
                UnstreamFloatValue(rv->c.r);
                UnstreamFloatValue(rv->c.i);
                break;
            case se_const:
            case se_absolute:
            case se_auto:
            case se_global:
            case se_threadlocal:
            case se_pc:
            case se_structelem:
                rv->sp = (Optimizer::SimpleSymbol*)UnstreamIndex();
                break;
            case se_labcon:
                rv->i = UnstreamIndex();
                break;
            case se_tempref: {
                int n = UnstreamIndex();
                int size = UnstreamIndex();
                int sizeFromType = UnstreamIndex();
                rv->sp = GetTempref(n, size, sizeFromType);
                break;
            }
            case se_msil_array_access:
                rv->msilArrayTP = UnstreamType();
                break;
            case se_msil_array_init:
                rv->tp = UnstreamType();
                break;
            case Optimizer::se_typeref:
                rv->tp = UnstreamType();
                break;
            case se_string: {
                std::string val;
                int count = UnstreamIndex();
                val.resize(count, 0);
                for (auto&& c : val)
                    c = UnstreamByte();
                rv->astring.str = (char*)Alloc(count);
                memcpy(rv->astring.str, val.c_str(), count);
                rv->astring.len = count;
                break;
            }
        }
        rv->left = UnstreamExpression();
        if (type == se_tempref)
        {
            rv->right = (Optimizer::SimpleExpression*)UnstreamIndex();
        }
        else
        {
            rv->right = UnstreamExpression();
        }
        rv->altData = UnstreamExpression();
    }
    UnstreamBlockType(STT_EXPRESSION, true);
    return rv;
}
static BROWSEFILE* UnstreamBrowseFile()
{
    BROWSEFILE* rv = Allocate<BROWSEFILE>();
    UnstreamBlockType(STT_BROWSEFILE, false);
    rv->name = (const char*)UnstreamTextIndex();
    rv->filenum = UnstreamIndex();
    UnstreamBlockType(STT_BROWSEFILE, true);
    return rv;
}
static BROWSEINFO* UnstreamBrowseInfo()
{
    BROWSEINFO* rv = Allocate<BROWSEINFO>();
    UnstreamBlockType(STT_BROWSEINFO, false);
    rv->name = (const char*)UnstreamTextIndex();
    rv->filenum = UnstreamIndex();
    rv->type = UnstreamIndex();
    rv->lineno = UnstreamIndex();
    rv->charpos = UnstreamIndex();
    rv->flags = UnstreamIndex();
    UnstreamBlockType(STT_BROWSEINFO, true);
    return rv;
}

static AMODE* UnstreamAssemblyOperand()
{
    AMODE* rv = nullptr;
    int mode = UnstreamIndex();
    if (mode != am_none)
    {
        rv = Allocate<AMODE>();
        rv->mode = (e_am)mode;
        rv->preg = UnstreamIndex();
        rv->sreg = UnstreamIndex();
        rv->tempflag = UnstreamIndex();
        rv->scale = UnstreamIndex();
        rv->length = UnstreamIndex();
        rv->addrlen = UnstreamIndex();
        rv->seg = UnstreamIndex();
        UnstreamIntValue(&rv->liveRegs, 8);
        rv->keepesp = UnstreamIndex();
        rv->offset = UnstreamExpression();
    }
    return rv;
}
static OCODE* UnstreamAssemblyInstruction()
{
    OCODE* rv = Allocate<OCODE>();
    rv->opcode = (e_opcode)UnstreamIndex();
    rv->diag = UnstreamIndex();
    rv->noopt = UnstreamIndex();
    rv->size = UnstreamIndex();
    rv->blocknum = UnstreamIndex();
    rv->oper1 = UnstreamAssemblyOperand();
    rv->oper2 = UnstreamAssemblyOperand();
    rv->oper3 = UnstreamAssemblyOperand();
    return rv;
}
static Optimizer::IMODE* UnstreamOperand()
{
    Optimizer::IMODE* rv = Allocate<Optimizer::IMODE>();
    UnstreamBlockType(STT_OPERAND, false);
    rv->mode = (i_adr)UnstreamIndex();
    rv->scale = UnstreamIndex();
    rv->useindx = UnstreamIndex();
    rv->size = UnstreamIndex();
    rv->ptrsize = UnstreamIndex();
    rv->startbit = UnstreamIndex();
    rv->bits = UnstreamIndex();
    rv->seg = UnstreamIndex();
    rv->flags = UnstreamIndex();
    rv->offset = UnstreamExpression();
    rv->offset2 = UnstreamExpression();
    rv->offset3 = UnstreamExpression();
    if (rv->mode == i_ind && rv->offset && rv->offset->type == se_tempref)
    {
        rv->offset->sp->tp = UnstreamType();
    }
    rv->vararg = UnstreamExpression();
    UnstreamBlockType(STT_OPERAND, true);
    return rv;
}

static Optimizer::QUAD* UnstreamInstruction(FunctionData& fd)
{
    Optimizer::QUAD* rv = Allocate<Optimizer::QUAD>();
    UnstreamBlockType(STT_INSTRUCTION, false);
    rv->dc.opcode = (i_ops)UnstreamIndex();
    rv->block = currentBlock;
    if (currentBlock)
    {
        currentBlock->tail->fwd = rv;
        rv->back = currentBlock->tail;

        if (rv->dc.opcode != i_block)
            currentBlock->tail = rv;
    }
    if (rv->dc.opcode == i_passthrough)
    {
        rv->dc.left = (Optimizer::IMODE*)UnstreamAssemblyInstruction();
    }
    else
    {
        int i;
        switch (rv->dc.opcode)
        {
            case i_icon:
                UnstreamIntValue(&rv->dc.v.i, 8);
                break;
            case i_imcon:
            case i_fcon:
                UnstreamFloatValue(rv->dc.v.f);
                break;
            case i_cxcon:
                UnstreamFloatValue(rv->dc.v.c.r);
                UnstreamFloatValue(rv->dc.v.c.i);
                break;
            case i_label:
            case i_expressiontag:
                rv->dc.v.label = UnstreamIndex();
                break;
            case i_line: {
                i = UnstreamIndex();
                if (i)
                {
                    auto lineData = Allocate<LINEDATA>();
                    lineData->fileindex = UnstreamIndex();
                    lineData->lineno = UnstreamIndex();
                    lineData->line = (const char*)UnstreamTextIndex();
                    rv->dc.left = (Optimizer::IMODE*)lineData;
                }
            }
            break;
            case i_block:
                currentBlock = Allocate<BLOCK>();
                rv->dc.v.label = currentBlock->blocknum = UnstreamIndex();
                currentBlock->head = currentBlock->tail = rv;
                rv->block = currentBlock;
                break;
            case i_blockend:
                rv->dc.v.label = currentBlock->blocknum = UnstreamIndex();
                break;
            case i_dbgblock:
            case i_dbgblockend:
            case i_livein:
                break;
            case i_func: {
                rv->dc.v.label = UnstreamIndex();
                int n = UnstreamIndex();
                if (n)
                    rv->dc.left = fd.imodeList[n - 1];
                break;
            }
            case i_jc:
            case i_jnc:
            case i_jbe:
            case i_ja:
            case i_je:
            case i_jne:
            case i_jge:
            case i_jg:
            case i_jle:
            case i_jl:
            case i_swbranch:
            case i_coswitch:
            case i_goto:
            case i_cmpblock:
                rv->dc.v.label = UnstreamIndex();
                // fallthrough
            default:
                int n = UnstreamIndex();
                if (n)
                    rv->dc.left = fd.imodeList[n - 1];
                n = UnstreamIndex();
                if (n)
                    rv->dc.right = fd.imodeList[n - 1];
                break;
        }
        int n = UnstreamIndex();
        if (n)
            rv->ans = fd.imodeList[n - 1];
        rv->altsp = (Optimizer::SimpleSymbol*)UnstreamIndex();
        rv->alttp = UnstreamType();
        i = UnstreamIndex();
        ArgList** p = (ArgList**)&rv->altargs;
        for (; i; i--)
        {
            *p = Allocate<ArgList>();
            (*p)->tp = UnstreamType();
            if (UnstreamByte())
                (*p)->exp = UnstreamExpression();
            p = &(*p)->next;
        }
        rv->ansColor = UnstreamIndex();
        rv->leftColor = UnstreamIndex();
        rv->rightColor = UnstreamIndex();
        rv->scaleColor = UnstreamIndex();
        rv->flags = UnstreamIndex();
        rv->definition = UnstreamIndex();
        rv->available = UnstreamIndex();
        rv->sourceindx = UnstreamIndex();
        rv->copy = UnstreamIndex();
        rv->retcount = UnstreamIndex();
        rv->sehMode = UnstreamIndex();
        rv->fastcall = UnstreamIndex();
        rv->oldmode = UnstreamIndex();
        rv->novalue = UnstreamIndex();
        rv->temps = UnstreamIndex();
        rv->precolored = UnstreamIndex();
        rv->moved = UnstreamIndex();
        rv->livein = UnstreamIndex();
        rv->liveRegs = UnstreamIndex();
        if (rv->alwayslive)
            rv->block->alwayslive = true;
    }
    UnstreamBlockType(STT_INSTRUCTION, true);
    return rv;
}
static void UnstreamSymbolList(std::vector<Optimizer::SimpleSymbol*>& list)
{
    int i = UnstreamIndex();
    for (; i; i--)
    {
        list.push_back(UnstreamSymbol());
    }
}

static void UnstreamHeader()
{
    char newmagic[sizeof(magic)];
    int vers;
    UnstreamBuffer(newmagic, strlen(magic));
    if (memcmp(newmagic, magic, strlen(magic)) != 0)
    {
        printf("magic\n");
        dothrow();
    }
    UnstreamIntValue(&vers, sizeof(vers));
    if (vers != fileVersion)
    {
        printf("fileversion\n");
        dothrow();
    }
    architecture = UnstreamIndex();
}
static void UnstreamParams()
{
    UnstreamBlockType(SBT_PARAMS, false);
    UnstreamBuffer(&cparams, sizeof(cparams));
    UnstreamBlockType(SBT_PARAMS, true);
}
static void UnstreamXParams()
{
    UnstreamBlockType(SBT_XPARAMS, false);
    UnstreamString(compilerName);
    UnstreamString(intermediateName);
    UnstreamString(backendName);
    showBanner = UnstreamIndex();
    assembling = UnstreamIndex();
    dataAlign = UnstreamIndex();
    bssAlign = UnstreamIndex();
    constAlign = UnstreamIndex();
    nextLabel = UnstreamIndex();
    pinning = !!UnstreamIndex();
    msilstrings = !!UnstreamIndex();
    delegateforfuncptr = !!UnstreamIndex();
    initializeScalars = !!UnstreamIndex();
    registersAssigned = UnstreamIndex();
    UnstreamString(prm_assemblerSpecifier);
    UnstreamString(prm_libPath);
    UnstreamString(prm_include);
    std::string temp;
    UnstreamString(outputFileName);
    UnstreamString(assemblerFileExtension);
    UnstreamString(prm_OutputDefFile);
    UnstreamString(temp);
    pinvoke_dll = litlate(temp.c_str());
    UnstreamString(prm_snkKeyFile);
    UnstreamString(prm_assemblyVersion);
    UnstreamString(prm_namespace_and_class);
    UnstreamStringList(inputFiles);
    UnstreamStringList(backendFiles);
    UnstreamStringList(libIncludes);
    UnstreamStringList(toolArgs);
    UnstreamStringList(prm_Using);
    int i = UnstreamIndex();
    for (; i; i--)
    {
        std::string key, val;
        UnstreamString(key);
        UnstreamString(val);
        bePragma[key] = val;
    }
    UnstreamBlockType(SBT_XPARAMS, true);
}
static void UnstreamGlobals()
{
    UnstreamBlockType(SBT_GLOBALSYMS, false);
    UnstreamSymbolList(globalCache);
    UnstreamBlockType(SBT_GLOBALSYMS, true);
}
static void UnstreamExternals()
{
    UnstreamBlockType(SBT_EXTERNALS, false);
    UnstreamSymbolList(externals);
    UnstreamBlockType(SBT_EXTERNALS, true);
}
static void UnstreamTypes()
{
    UnstreamBlockType(SBT_TYPES, false);
    UnstreamSymbolList(typeSymbols);
    UnstreamSymbolList(typedefs);
    UnstreamBlockType(SBT_TYPES, true);
}
static void UnstreamMSILProperties()
{
    UnstreamBlockType(SBT_MSILPROPS, false);
    int i = UnstreamIndex();
    for (; i; i--)
    {
        MsilProperty p;
        p.prop = (Optimizer::SimpleSymbol*)UnstreamIndex();
        p.getter = (Optimizer::SimpleSymbol*)UnstreamIndex();
        p.setter = (Optimizer::SimpleSymbol*)UnstreamIndex();
        msilProperties.push_back(p);
    }
    UnstreamBlockType(SBT_MSILPROPS, true);
}
static void UnstreamTypedefs()
{
    UnstreamBlockType(SBT_TYPEDEFS, false);
    UnstreamSymbolList(typedefs);
    UnstreamBlockType(SBT_TYPEDEFS, true);
}
static void UnstreamBrowse()
{
    UnstreamBlockType(SBT_BROWSEFILES, false);
    int i = UnstreamIndex();
    for (; i; i--)
    {
        browseFiles.push_back(UnstreamBrowseFile());
    }
    UnstreamBlockType(SBT_BROWSEFILES, true);

    UnstreamBlockType(SBT_BROWSEINFO, false);
    i = UnstreamIndex();
    for (; i; i--)
    {
        browseInfo.push_back(UnstreamBrowseInfo());
    }
    UnstreamBlockType(SBT_BROWSEINFO, true);
}
static Optimizer::QUAD* UnstreamInstructions(FunctionData& fd)
{
    Optimizer::QUAD *rv = nullptr, *last = nullptr;

    int i = UnstreamIndex();
    for (; i; i--)
    {
        Optimizer::QUAD* newQuad = UnstreamInstruction(fd);
        if (last)
            last->fwd = newQuad;
        else
            rv = newQuad;
        newQuad->back = last;
        last = newQuad;
    }
    return rv;
}
static void UnstreamIModes(FunctionData& fd)
{
    UnstreamBlockType(SBT_IMODES, false);
    int len = UnstreamIndex();
    for (int i = 0; i < len; i++)
    {
        fd.imodeList.push_back(UnstreamOperand());
    }
    UnstreamBlockType(SBT_IMODES, true);
}
static void UnstreamTemps()
{
    int i = UnstreamIndex();
    for (; i; i--)
    {
        int temp = UnstreamIndex();
        int val = UnstreamByte();
        if (temps[temp])
        {
            temps[temp]->loadTemp = !!(val & TF_LOADTEMP);
            temps[temp]->pushedtotemp = !!(val & TF_PUSHEDTOTEMP);
        }
        temps[temp]->tp = UnstreamType();
    }
}
static void UnstreamLoadCache(FunctionData* fd, std::map<Optimizer::IMODE*, Optimizer::IMODE*>& hash)
{
    int i = UnstreamIndex();
    for (; i; i--)
    {
        int n = UnstreamIndex();
        if (n)
        {
            Optimizer::IMODE* key = fd->imodeList[n - 1];
            n = UnstreamIndex();
            if (n)  // might fail in the backend...
            {
                Optimizer::IMODE* value = fd->imodeList[n - 1];
                hash[key] = value;
            }
        }
        else
        {
            n = UnstreamIndex();
        }
    }
}
static FunctionData* UnstreamFunc()
{
    currentBlock = nullptr;
    FunctionData* fd = new FunctionData;
    std::vector<Optimizer::SimpleSymbol*> temporarySymbols;
    std::vector<Optimizer::SimpleSymbol*> variables;
    Optimizer::QUAD* instructionList;

    fd->name = (Optimizer::SimpleSymbol*)UnstreamIndex();
    int flgs = UnstreamIndex();
    fd->setjmp_used = !!(flgs & FF_USES_SETJMP);
    fd->hasAssembly = !!(flgs & FF_HAS_ASSEMBLY);
    fd->blockCount = UnstreamIndex();
    fd->tempCount = UnstreamIndex();
    fd->exitBlock = UnstreamIndex();
    fd->fastcallAlias = UnstreamIndex();
    temps.clear();
    temps.resize(fd->tempCount);
    UnstreamSymbolList(fd->variables);
    UnstreamSymbolList(fd->temporarySymbols);
    UnstreamIModes(*fd);
    fd->fltexp = UnstreamExpression();
    fd->instructionList = UnstreamInstructions(*fd);
    UnstreamTemps();
    UnstreamLoadCache(fd, fd->loadHash);
    return fd;
}
static void UnstreamData()
{
    temps.clear();
    UnstreamBlockType(SBT_DATA, false);
    int len = UnstreamIndex();
    for (int i = 0; i < len; i++)
    {
        BaseData* data = Allocate<BaseData>();
        baseData.push_back(data);
        data->type = (DataType)UnstreamIndex();
        UnstreamBlockType(data->type, false);
        switch (data->type)
        {
            case DT_NONE:
                break;
            case DT_SEG:
            case DT_SEGEXIT:
                data->i = UnstreamIndex();
                break;
            case DT_DEFINITION:
                data->symbol.sym = (Optimizer::SimpleSymbol*)UnstreamIndex();
                data->symbol.i = UnstreamIndex();
                break;
            case DT_LABELDEFINITION:
                data->i = UnstreamIndex();
                break;
            case DT_RESERVE:
                data->i = UnstreamIndex();
                break;
            case DT_SYM:
                data->symbol.sym = (Optimizer::SimpleSymbol*)UnstreamIndex();
                data->symbol.i = UnstreamIndex();
                break;
            case DT_SRREF:
                data->symbol.sym = (Optimizer::SimpleSymbol*)UnstreamIndex();
                data->symbol.i = UnstreamIndex();
                break;
            case DT_PCREF:
                data->symbol.sym = (Optimizer::SimpleSymbol*)UnstreamIndex();
                break;
            case DT_FUNCREF:
                data->symbol.sym = (Optimizer::SimpleSymbol*)UnstreamIndex();
                data->symbol.i = UnstreamIndex();
                break;
            case DT_LABEL:
                data->i = UnstreamIndex();
                break;
            case DT_LABDIFFREF:
                data->diff.l1 = UnstreamIndex();
                data->diff.l2 = UnstreamIndex();
                break;
            case DT_STRING: {
                bool instring = false;
                data->astring.i = UnstreamIndex();
                data->astring.str = (char*)Alloc(data->astring.i + 1);
                for (int i = 0; i < data->astring.i; i++)
                {
                    data->astring.str[i] = UnstreamByte();
                }
            }
            break;
            case DT_BIT:
                break;
            case DT_BOOL:
                UnstreamIntValue(&data->i, 1);
                break;
            case DT_BYTE:
                UnstreamIntValue(&data->i, 1);
                break;
            case DT_USHORT:
                UnstreamIntValue(&data->i, 2);
                break;
            case DT_UINT:
                UnstreamIntValue(&data->i, 4);
                break;
            case DT_ULONG:
                UnstreamIntValue(&data->i, 8);
                break;
            case DT_ULONGLONG:
                UnstreamIntValue(&data->i, 8);
                break;
            case DT_16:
                UnstreamIntValue(&data->i, 2);
                break;
            case DT_32:
                UnstreamIntValue(&data->i, 4);
                break;
            case DT_ENUM:
                UnstreamIntValue(&data->i, 4);
                break;
            case DT_FLOAT:
                UnstreamFloatValue(data->f);
                break;
            case DT_DOUBLE:
                UnstreamFloatValue(data->f);
                break;
            case DT_LDOUBLE:
                UnstreamFloatValue(data->f);
                break;
            case DT_CFLOAT:
                UnstreamFloatValue(data->c.r);
                UnstreamFloatValue(data->c.i);
                break;
            case DT_CDOUBLE:
                UnstreamFloatValue(data->c.r);
                UnstreamFloatValue(data->c.i);
                break;
            case DT_CLONGDOUBLE:
                UnstreamFloatValue(data->c.r);
                UnstreamFloatValue(data->c.i);
                break;
            case DT_ADDRESS:
                UnstreamIntValue(&data->i, 8);
                break;
            case DT_VIRTUAL:
                data->symbol.sym = (Optimizer::SimpleSymbol*)UnstreamIndex();
                data->symbol.i = UnstreamIndex();
                break;
            case DT_ENDVIRTUAL:
                data->symbol.sym = (Optimizer::SimpleSymbol*)UnstreamIndex();
                break;
            case DT_ALIGN:
                data->i = UnstreamIndex();
                break;
            case DT_VTT:
                data->symbol.sym = (Optimizer::SimpleSymbol*)UnstreamIndex();
                UnstreamIntValue(&data->symbol.i, sizeof(data->symbol.i));
                break;
            case DT_IMPORTTHUNK:
                data->symbol.sym = (Optimizer::SimpleSymbol*)UnstreamIndex();
                break;
            case DT_VC1:
                data->symbol.sym = (Optimizer::SimpleSymbol*)UnstreamIndex();
                break;
            case DT_AUTOREF:
                data->symbol.sym = (Optimizer::SimpleSymbol*)UnstreamIndex();
                data->symbol.i = UnstreamIndex();
                break;
            case DT_FUNC:
                data->funcData = UnstreamFunc();
                break;
        }
        UnstreamBlockType(data->type, true);
    }
    UnstreamBlockType(SBT_DATA, true);
}
void ReadText(std::map<int, std::string>& texts)
{
    UnstreamBlockType(SBT_TEXT, false);
    textOffset = UnstreamIndex();
    for (int i = 1; i < textOffset;)
    {
        int len = UnstreamIndex();
        std::string val;
        val.resize(len, 0);

        for (auto&& c : val)
            c = UnstreamByte();
        texts[i] = val;
        i += len;
    }
    UnstreamBlockType(SBT_TEXT, true);
}
static Optimizer::SimpleSymbol* SymbolName(Optimizer::SimpleSymbol* selection, std::vector<Optimizer::SimpleSymbol*>* table)
{
    // symbol index was multiplied by two and the low bit was setst
    int index = ((int)(intptr_t)selection - 1) / 2;
    if (index > 0)
    {
        index--;
        if (table == &globalCache && index >= globalCache.size())
        {
            index -= globalCache.size();
            table = &externals;
        }
        else if (table == &typeSymbols && index >= typeSymbols.size())
        {
            index -= typeSymbols.size();
            table = &typedefs;
        }
        else if (current && table == &current->variables && index >= current->variables.size())
        {
            index -= current->variables.size();
            table = &current->temporarySymbols;
        }
        return (*table)[index];
    }
    return nullptr;
}
static void ResolveSymbol(Optimizer::SimpleSymbol*& sym, std::map<int, std::string>& texts,
                          std::vector<Optimizer::SimpleSymbol*>& table);
static void ResolveType(Optimizer::SimpleType* tp, std::map<int, std::string>& texts, std::vector<Optimizer::SimpleSymbol*>& table)
{
    bool ispointer = false;
    while (tp)
    {
        if ((int)(intptr_t)tp->sp & 1)
        {
            if ((int)(intptr_t)tp->sp & 0x20000000)
            {
                tp->sp = (Optimizer::SimpleSymbol*)((int)(intptr_t)tp->sp & 0x1fffffff);
                ResolveSymbol(tp->sp, texts, current->variables);
            }
            else
            {
                bool param = !!((int)(intptr_t)tp->sp & 0x40000000);
                tp->sp = (Optimizer::SimpleSymbol*)((int)(intptr_t)tp->sp & 0x3fffffff);
                ResolveSymbol(tp->sp, texts, tp->type == st_func && !param ? globalCache : typeSymbols);
            }
        }
        tp = tp->btp;
    }
}
static void ResolveExpression(Optimizer::SimpleExpression* exp, std::map<int, std::string>& texts)
{
    if (exp)
    {
        switch (exp->type)
        {
            case se_auto:
                if ((int)(intptr_t)exp->sp & 0x40000000)
                {
                    exp->sp = ((SimpleSymbol*)((int)(intptr_t)exp->sp & ~0x40000000));
                    ResolveSymbol(exp->sp, texts, globalCache);
                }
                else
                {
                    ResolveSymbol(exp->sp, texts, current->variables);
                }
                break;
            case se_const:
            case se_absolute:
            case se_global:
            case se_threadlocal:
            case se_pc:
                ResolveSymbol(exp->sp, texts, globalCache);
                break;
            case se_structelem:
                ResolveSymbol(exp->sp, texts, typeSymbols);
                break;
            case se_msil_array_access:
                exp->sp = SymbolName(exp->sp, &typeSymbols);
                break;
            case se_msil_array_init:
                ResolveType(exp->tp, texts, typeSymbols);
                break;
            case se_typeref:
                ResolveType(exp->tp, texts, typeSymbols);
                break;
        }
        ResolveExpression(exp->left, texts);
        if (exp->type == se_tempref)
        {
            if (exp->right)
            {
                Optimizer::SimpleSymbol* sym = (Optimizer::SimpleSymbol*)exp->right;
                int n = (int)(intptr_t)(exp->right);
                if (n & 0x40000000)
                {
                    sym = (Optimizer::SimpleSymbol*)((int)(intptr_t)exp->right & 0x3fffffff);
                    ResolveSymbol(sym, texts, globalCache);
                }
                else
                {
                    ResolveSymbol(sym, texts, current->variables);
                }
                exp->right = (Optimizer::SimpleExpression*)sym;
            }
        }
        else
        {
            ResolveExpression(exp->right, texts);
        }
    }
}
static void ResolveAssemblyInstruction(OCODE* c, std::map<int, std::string>& texts)
{
    if (c->oper1)
        ResolveExpression(c->oper1->offset, texts);
    if (c->oper2)
        ResolveExpression(c->oper2->offset, texts);
    if (c->oper3)
        ResolveExpression(c->oper3->offset, texts);
}
static void ResolveInstruction(Optimizer::QUAD* q, std::map<int, std::string>& texts)
{
    switch (q->dc.opcode)
    {
        case i_passthrough:
            ResolveAssemblyInstruction((OCODE*)q->dc.left, texts);
            break;
        case i_icon:
        case i_imcon:
        case i_fcon:
        case i_cxcon:
        case i_label:
            break;
        case i_line: {
            auto ld = (LINEDATA*)q->dc.left;
            ld->line = texts[(int)(intptr_t)ld->line].c_str();
            break;
        }
        default:
            break;
    }
    if (q->altsp)
    {
        if ((int)(intptr_t)q->altsp & 0x40000000)
        {
            q->altsp = (Optimizer::SimpleSymbol*)((int)(intptr_t)q->altsp & 0x3fffffff);
            ResolveSymbol(q->altsp, texts, typeSymbols);
        }
        else if ((int)(intptr_t)q->altsp & 0x20000000)
        {
            q->altsp = (Optimizer::SimpleSymbol*)((int)(intptr_t)q->altsp & 0x1fffffff);
            ResolveSymbol(q->altsp, texts, current->variables);
        }
        else
        {
            ResolveSymbol(q->altsp, texts, globalCache);
        }
    }
    if (q->alttp)
    {
        ResolveType(q->alttp, texts, typeSymbols);
    }
    for (auto a = q->altargs; a; a = a->next)
    {
        ResolveType(a->tp, texts, typeSymbols);
        if (a->exp)
            ResolveExpression(a->exp, texts);
    }
}
static void ResolveSymbol(std::vector<Optimizer::SimpleSymbol*> symbols, std::map<int, std::string>& texts,
                          std::vector<Optimizer::SimpleSymbol*>& table)
{
    for (auto&& v : symbols)
    {
        ResolveSymbol(v, texts, table);
    }
}
static void ResolveSymbol(Optimizer::SimpleSymbol*& sym, std::map<int, std::string>& texts,
                          std::vector<Optimizer::SimpleSymbol*>& table)
{
    if (sym != nullptr)
    {
        // low bit set means this is an index not a symbol
        if ((int)(intptr_t)sym & 1)
            sym = SymbolName(sym, &table);
        if (sym->visited)
            return;
        sym->visited = true;
        ResolveSymbol(sym->parentClass, texts, typeSymbols);
        for (auto l = sym->syms; l; l = l->next)
        {
            Optimizer::SimpleSymbol* s = (Optimizer::SimpleSymbol*)l->data;
            ResolveSymbol(s, texts, typeSymbols);
            l->data = (void*)s;
        }
        for (auto b = sym->baseClasses; b; b = b->next)
        {
            ResolveSymbol(b->sym, texts, typeSymbols);
        }
        ResolveType(sym->tp, texts, typeSymbols);

        sym->name = texts[(int)(intptr_t)sym->name].c_str();
        sym->outputName = texts[(int)(intptr_t)sym->outputName].c_str();
        sym->importfile = texts[(int)(intptr_t)sym->importfile].c_str();
        sym->namespaceName = texts[(int)(intptr_t)sym->namespaceName].c_str();
        if (sym->msil)
            sym->msil = texts[(int)(intptr_t)sym->msil].c_str();
    }
}
static void ResolveFunction(FunctionData* fd, std::map<int, std::string>& texts)
{
    current = fd;

    ResolveSymbol(fd->name, texts, globalCache);

    ResolveSymbol(fd->variables, texts, fd->variables);
    ResolveSymbol(fd->temporarySymbols, texts, fd->variables);
    for (auto v : fd->imodeList)
    {
        ResolveExpression(v->offset, texts);
        ResolveExpression(v->offset2, texts);
        ResolveExpression(v->offset3, texts);
        if (v->mode == i_ind && v->offset)
            ResolveType(v->offset->sp->tp, texts, typeSymbols);
        ResolveExpression(v->vararg, texts);
    }
    for (auto q = fd->instructionList; q; q = q->fwd)
        ResolveInstruction(q, texts);
    ResolveExpression(fd->fltexp, texts);
    lastFunction = current;
    current = nullptr;
}
static void ResolveNames(std::map<int, std::string>& texts)
{
    for (auto&& v : globalCache)
        ResolveSymbol(v, texts, globalCache);
    for (auto&& v : externals)
        ResolveSymbol(v, texts, externals);
    for (auto&& v : typedefs)
        ResolveSymbol(v, texts, typedefs);
    for (auto&& v : typeSymbols)
        ResolveSymbol(v, texts, typeSymbols);
    for (auto&& d : baseData)
    {
        switch (d->type)
        {
            case DT_FUNC:
                ResolveFunction(d->funcData, texts);
                break;
            case DT_AUTOREF:
                current = lastFunction;
                ResolveSymbol(d->symbol.sym, texts, current->variables);  // assumes the variables of the last generated function
                current = nullptr;
                break;

            case DT_DEFINITION:
            case DT_SYM:
            case DT_SRREF:
            case DT_PCREF:
            case DT_FUNCREF:
            case DT_VIRTUAL:
            case DT_ENDVIRTUAL:
            case DT_VTT:
            case DT_IMPORTTHUNK:
            case DT_VC1:
                ResolveSymbol(d->symbol.sym, texts, globalCache);
                break;
        }
    }
    for (auto b : browseFiles)
    {
        b->name = texts[(int)(intptr_t)b->name].c_str();
    }
    for (auto b : browseInfo)
    {
        b->name = texts[(int)(intptr_t)b->name].c_str();
    }
    for (auto&& v : msilProperties)
    {
        ResolveSymbol(v.prop, texts, globalCache);
        ResolveSymbol(v.getter, texts, globalCache);
        ResolveSymbol(v.setter, texts, globalCache);
    }
    /*
    for (auto&& t : temps)
    {
        if (t)
            ResolveType(t->tp, texts, typeSymbols);
    }
    */
}
bool InputIntermediate(SharedMemory* inputMem)
{
    FPF temp;  // force init
    shared = inputMem;
    currentBlock = nullptr;
    texts.clear();
    texts[0] = "";
    textOffset = 1;
    try
    {
        UnstreamHeader();
        UnstreamParams();
        UnstreamXParams();
        UnstreamGlobals();
        UnstreamExternals();
        UnstreamTypes();
        UnstreamBrowse();
        UnstreamMSILProperties();
        UnstreamData();
        ReadText(texts);
        ResolveNames(texts);
        return true;
    }
    catch (std::runtime_error e)
    {
        return false;
    }
}
void ReadMappingFile(SharedMemory* mem, FILE* fil)
{
    int pos = 0;
    fseek(fil, 0, SEEK_END);
    int end = ftell(fil);
    fseek(fil, 0, SEEK_SET);
    unsigned char* p = mem->GetMapping();
    mem->EnsureCommitted(end);
    while (end > 0)
    {

        int n = mem->ViewWindowSize();
        if (n > end)
            n = end;
        fread(p + pos, n, 1, fil);
        pos += n;
        end -= n;
        p = mem->GetMapping(pos);
    }
}
}  // namespace Optimizer