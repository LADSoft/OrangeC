/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>
#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "ildata.h"
#include "../occ/winmode.h"
#include "../occ/be.h"
#include "SharedMemory.h"
#include <deque>
#include <functional>
#include <map>
#include <set>
#include "ildata.h"

namespace Optimizer
{
static std::list<std::string> textRegion;
static std::map<std::string, int> cachedText;
static size_t textOffset;
static std::map<IMODE*, int> cachedImodes;
static std::set<SimpleSymbol*> cachedAutos;
static std::set<SimpleSymbol*> cachedTemps;

static SharedMemory* sharedRegion;
// the next are intended not to be reset on each read, as there will be another file streamed next
static int outputPos;
static int outputSize;
static unsigned char* streamPointer;

static const int WRITE_INCREMENT = 256 * 1024;

static size_t TextName(const std::string& name)
{
    if (name[0] == 0)
        return 0;
    auto it = cachedText.find(name);
    if (it != cachedText.end())
        return it->second;
    auto rv = textOffset;
    textOffset += name.size();
    textRegion.push_back(name);
    cachedText[name] = rv;
    return rv;
}
inline static void resize(int size)
{
    if (outputPos + size > outputSize)
    {
        outputSize += WRITE_INCREMENT;
        sharedRegion->EnsureCommitted(outputSize);
        sharedRegion->CloseMapping();
        streamPointer = sharedRegion->GetMapping();
    }
}
inline static void StreamByte(int value)
{
    resize(1);
    streamPointer[outputPos++] = value;
}
inline static void StreamBlockType(int blockType, bool end) { StreamByte(blockType + (end ? 0x80 : 0x40)); }
template <class T>
inline static void StreamBlock(T blockType, std::function<void(void)> blockRenderer)
{
    StreamBlockType(blockType, false);
    blockRenderer();
    StreamBlockType(blockType, true);
}

inline static void StreamIndex(int value)
{
    if (value >= 0 && value < 0x8000)
    {
        StreamByte(value >> 8);
        StreamByte(value);
    }
    else
    {
        StreamByte((value >> 24) | 0x80);
        StreamByte(value >> 16);
        StreamByte(value >> 8);
        StreamByte(value >> 0);
    }
}
inline static void StreamTextIndex(const char* name)
{
    if (!name)
        StreamIndex(0);
    else
        StreamIndex(TextName(name));
}
inline static void StreamString(const std::string& value)
{
    StreamIndex(value.size());
    for (auto c : value)
        StreamByte(c);
}
static void StreamStringList(const std::list<std::string>& list)
{
    StreamIndex(list.size());
    for (auto&& s : list)
        StreamString(s);
}
static void StreamBuffer(const void* buf, int len)
{
    resize(len);
    memcpy(&streamPointer[outputPos], buf, len);
    outputPos += len;
}
inline static void StreamIntValue(const void* buf, int len)
{
    StreamBlock(STT_INT, [buf, len]() {
        for (int i = len - 1; i >= 0; i--)
            StreamByte(((unsigned char*)buf)[i]);
    });
}
static void StreamFloatValue(FPF& fv)
{
    StreamBlock(STT_FLOAT, [&fv]() {
        StreamIndex(fv.type);
        StreamIndex(fv.sign);
        StreamIntValue(&fv.exp, sizeof(int));
        for (int i = 0; i < INTERNAL_FPF_PRECISION; i++)
            StreamIndex(fv.mantissa[i]);
    });
}
static void StreamSymbol(SimpleSymbol* sym);
static void StreamSymbolTable(LIST* syms)
{
    int i = 0;
    for (auto l = syms; l && l->data; l = l->next, i++)
        ;
    StreamIndex(i);
    for (auto l = syms; l && l->data; l = l->next)
    {
        if (((SimpleSymbol*)l->data)->fileIndex == 0)
            StreamIndex(((SimpleSymbol*)l->data)->typeIndex);
        else
            StreamIndex(((SimpleSymbol*)l->data)->fileIndex);
    }
}
static void StreamBases(BaseList* bases)
{
    BaseList* baseClasses;
    int i = 0;
    for (auto l = bases; l; l = l->next, i++)
        ;
    StreamIndex(i);
    for (auto l = bases; l; l = l->next)
    {
        StreamBlock(STT_BASE, [l]() {
            StreamIndex(l->sym->fileIndex);
            StreamIndex(l->offset);
        });
    }
}
static void StreamType(SimpleType* type)
{
    StreamBlock(STT_TYPE, [type]() {
        if (!type)
        {
            StreamIndex(st_none);
        }
        else
        {
            StreamIndex(type->type);
            StreamIndex(type->size);
            StreamIntValue(&type->sizeFromType, sizeof(int));
            StreamIndex(type->bits);
            StreamIndex(type->startbit);
            if (type->sp && type->type != st_any)
            {
                //                if (type->sp->fileIndex == 0 && type->sp->typeIndex == 0)
                //                {
                //                    printf("hi");
                //                }

                if (type->sp->storage_class == scc_auto || type->sp->storage_class == scc_register)
                {
                    StreamIndex(type->sp->fileIndex | 0x20000000);
                }
                else if (type->sp->typeIndex)
                    StreamIndex(0x40000000 | type->sp->typeIndex);
                else
                    StreamIndex(type->sp->fileIndex);
            }
            else
            {
                StreamIndex(0);
            }
            StreamIndex(type->flags);
            StreamType(type->btp);
        }
    });
}
static void StreamSymbol(SimpleSymbol* sym)
{
    StreamBlock(STT_SYMBOL, [sym]() {
        if (!sym)
        {
            StreamIndex(scc_none);
        }
        else
        {
            StreamIndex(sym->storage_class);
            StreamTextIndex(sym->name);
            StreamTextIndex(sym->outputName);
            StreamTextIndex(sym->importfile);
            StreamTextIndex(sym->namespaceName);
            StreamTextIndex(sym->msil);
            StreamIndex(sym->i);
            StreamIndex(sym->regmode);
            StreamIntValue(&sym->offset, 4);
            StreamIndex(sym->label);
            StreamIndex(sym->templateLevel);
            StreamIntValue(&sym->flags, 8);
            StreamIntValue(&sym->sizeFromType, sizeof(int));
            StreamIndex(sym->align);
            StreamIndex(sym->size);
            if (sym->parentClass)
                StreamIndex(sym->parentClass->fileIndex);
            else
                StreamIndex(0);
            StreamType(sym->tp);
            StreamSymbolTable(sym->syms);
            StreamBases(sym->baseClasses);
        }
    });
}
static void StreamExpression(SimpleExpression* exp)
{
    StreamBlock(STT_EXPRESSION, [exp]() {
        if (!exp)
        {
            StreamIndex(Optimizer::se_none);
        }
        else
        {
            StreamIndex(exp->type);
            StreamIndex(exp->flags);
            StreamIntValue(&exp->sizeFromType, sizeof(int));
            switch (exp->type)
            {
                case Optimizer::se_i:
                case Optimizer::se_ui:
                    StreamIntValue(&exp->i, 8);
                    break;
                case Optimizer::se_f:
                case Optimizer::se_fi:
                    StreamFloatValue(exp->f);
                    break;
                case Optimizer::se_fc:
                    StreamFloatValue(exp->c.r);
                    StreamFloatValue(exp->c.i);
                    break;
                case Optimizer::se_auto:
                    if (exp->sp->storage_class == scc_localstatic)
                    {
                        StreamIndex(exp->sp->fileIndex | 0x40000000);
                        break;
                    }
                case Optimizer::se_const:
                case Optimizer::se_absolute:
                case Optimizer::se_global:
                case Optimizer::se_threadlocal:
                case Optimizer::se_pc:
                case Optimizer::se_structelem:
                    //                if (exp->sp->fileIndex == 0)
                    //                    printf("hi");
                    StreamIndex(exp->sp->fileIndex);
                    break;
                case Optimizer::se_labcon:
                    StreamIndex(exp->i);
                    break;
                case Optimizer::se_tempref:
                    StreamIndex(exp->sp->i);
                    break;
                case Optimizer::se_msil_array_access:
                    StreamType(exp->msilArrayTP);
                    break;
                case Optimizer::se_typeref:
                     StreamType(exp->tp);
                     break;
                case Optimizer::se_msil_array_init:
                    StreamType(exp->tp);
                    break;
                case Optimizer::se_string:
                    StreamIndex(exp->astring.len);
                    for (int i = 0; i < exp->astring.len; i++)
                        StreamByte(exp->astring.str[i]);
                    break;
            }
            StreamExpression(exp->left);
            if (exp->type == Optimizer::se_tempref)
            {
                SimpleSymbol* sp = ((SimpleSymbol*)exp->right);
                int n = 0;
                if (sp)
                {
                    n = sp->fileIndex;
                    if (sp->storage_class != scc_auto && sp->storage_class != scc_register && sp->storage_class != scc_parameter)
                        n |= 0x40000000;
                }
                StreamIndex(n);
            }
            else
            {
                StreamExpression(exp->right);
            }
            StreamExpression(exp->altData);
        }
    });
}
static void StreamBrowseFile(BROWSEFILE* bf)
{
    StreamBlock(STT_BROWSEFILE, [bf]() {
        StreamTextIndex(bf->name);
        StreamIndex(bf->filenum);
    });
}
static void StreamBrowseInfo(BROWSEINFO* bi)
{
    StreamBlock(STT_BROWSEINFO, [bi]() {
        StreamTextIndex(bi->name);
        StreamIndex(bi->filenum);
        StreamIndex(bi->type);
        StreamIndex(bi->lineno);
        StreamIndex(bi->charpos);
        StreamIndex(bi->flags);
    });
}

static void StreamAssemblyOperand(AMODE* im)
{
    if (!im)
    {
        StreamIndex(am_none);
    }
    else
    {
        StreamIndex(im->mode);
        StreamIndex(im->preg);
        StreamIndex(im->sreg);
        StreamIndex(im->tempflag);
        StreamIndex(im->scale);
        StreamIndex(im->length);
        StreamIndex(im->addrlen);
        StreamIndex(im->seg);
        StreamIntValue(&im->liveRegs, 8);
        StreamIndex(im->keepesp);
        StreamExpression(im->offset);
    }
}
static void StreamAssemblyInstruction(OCODE* oc)
{
    StreamIndex(oc->opcode);
    StreamIndex(oc->diag);
    StreamIndex(oc->noopt);
    StreamIndex(oc->size);
    StreamIndex(oc->blocknum);
    StreamAssemblyOperand(oc->oper1);
    StreamAssemblyOperand(oc->oper2);
    StreamAssemblyOperand(oc->oper3);
}
static void StreamOperand(IMODE* im)
{
    StreamBlock(STT_OPERAND, [im]() {
        StreamIndex(im->mode);
        StreamIndex(im->scale);
        StreamIndex(im->useindx);
        StreamIndex(im->size);
        StreamIndex(im->ptrsize);
        StreamIndex(im->startbit);
        StreamIndex(im->bits);
        StreamIndex(im->seg);
        StreamIndex(im->flags);
        StreamExpression(im->offset);
        StreamExpression(im->offset2);
        StreamExpression(im->offset3);
        if (im->mode == i_ind && im->offset && im->offset->type == Optimizer::se_tempref)
        {
            StreamType(im->offset->sp->tp);
        }
        StreamExpression(im->vararg);
    });
}

static void StreamInstruction(QUAD* q)
{
    StreamBlock(STT_INSTRUCTION, [q]() {
        StreamIndex(q->dc.opcode);
        if (q->dc.opcode == i_passthrough)
        {
            StreamAssemblyInstruction((OCODE*)q->dc.left);
        }
        else
        {
            int i;
            switch (q->dc.opcode)
            {
                case i_icon:
                    StreamIntValue(&q->dc.v.i, 8);
                    break;
                case i_imcon:
                case i_fcon:
                    StreamFloatValue(q->dc.v.f);
                    break;
                case i_cxcon:
                    StreamFloatValue(q->dc.v.c.r);
                    StreamFloatValue(q->dc.v.c.i);
                    break;
                case i_label:
                case i_expressiontag:
                    StreamIndex(q->dc.v.label);
                    break;
                case i_line:
                    i = 0;
                    for (auto v = (LINEDATA*)q->dc.left; v; v = v->next, i++)
                        ;
                    StreamIndex(i);
                    for (auto v = (LINEDATA*)q->dc.left; v; v = v->next)
                    {
                        StreamIndex(v->lineno);
                        StreamTextIndex(v->line);
                    }
                    break;
                case i_block:
                    StreamIndex(q->block->blocknum);
                    break;
                case i_blockend:
                    StreamIndex(q->block->blocknum);
                    break;
                case i_dbgblock:
                case i_dbgblockend:
                case i_livein:
                    break;
                case i_func:
                    StreamIndex(q->dc.v.label);
                    if (q->dc.left)
                        StreamIndex(cachedImodes[q->dc.left]);
                    else
                        StreamIndex(0);
                    break;
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
                    StreamIndex(q->dc.v.label);
                    // fallthrough
                default:
                    if (q->dc.left)
                        StreamIndex(cachedImodes[q->dc.left]);
                    else
                        StreamIndex(0);
                    if (q->dc.right)
                        StreamIndex(cachedImodes[q->dc.right]);
                    else
                        StreamIndex(0);
                    break;
            }
            if (q->ans)
                StreamIndex(cachedImodes[q->ans]);
            else
                StreamIndex(0);
            if (q->altsp)
            {
                //                if (q->altsp->fileIndex == 0)
                //                    printf("hi");
                if (q->altsp->storage_class == scc_auto ||
                    (q->altsp->storage_class == scc_parameter && q->altsp->fileIndex != q->altsp->typeIndex) ||
                    q->altsp->storage_class == scc_register)
                    StreamIndex(q->altsp->fileIndex | 0x20000000);
                else if (q->altsp->fileIndex == q->altsp->typeIndex)
                    StreamIndex(q->altsp->typeIndex | 0x40000000);
                else
                    StreamIndex(q->altsp->fileIndex);
            }
            else
            {
                StreamIndex(0);
            }
            StreamType(q->alttp);
            i = 0;
            for (auto v = (ArgList*)q->altargs; v; v = v->next, i++)
                ;
            StreamIndex(i);
            for (auto v = (ArgList*)q->altargs; v; v = v->next)
            {
                StreamType(v->tp);
                if (v->exp)
                {
                    StreamByte(1);
                    StreamExpression(v->exp);
                }
                else
                {
                    StreamByte(0);
                }
            }
            StreamIndex(q->ansColor);
            StreamIndex(q->leftColor);
            StreamIndex(q->rightColor);
            StreamIndex(q->scaleColor);
            StreamIndex(q->flags);
            StreamIndex(q->definition);
            StreamIndex(q->available);
            StreamIndex(q->sourceindx);
            StreamIndex(q->copy);
            StreamIndex(q->retcount);
            StreamIndex(q->sehMode);
            StreamIndex(q->fastcall);
            StreamIndex(q->oldmode);
            StreamIndex(q->novalue);
            StreamIndex(q->temps);
            StreamIndex(q->precolored);
            StreamIndex(q->moved);
            StreamIndex(q->livein);
            StreamIndex(q->liveRegs);
        }
    });
}
static void StreamSymbolList(const std::vector<SimpleSymbol*>& list)
{
    StreamIndex(list.size());
    for (auto s : list)
        StreamSymbol(s);
}

static void StreamHeader()
{
    StreamBuffer(magic, strlen(magic));
    StreamIntValue(&fileVersion, sizeof(fileVersion));
    StreamIndex(architecture);
}
static void StreamParams()
{
    StreamBlock(SBT_PARAMS, []() { StreamBuffer(&cparams, sizeof(cparams)); });
}
static void StreamXParams()
{
    StreamBlock(SBT_XPARAMS, []() {
        StreamString(compilerName);
        StreamString(intermediateName);
        StreamString(backendName);
        StreamIndex(showBanner);
        StreamIndex(verbosity);
        StreamIndex(assembling);
        StreamIndex(dataAlign);
        StreamIndex(bssAlign);
        StreamIndex(constAlign);
        StreamIndex(nextLabel);
        StreamIndex(pinning);
        StreamIndex(msilstrings);
        StreamIndex(delegateforfuncptr);
        StreamIndex(initializeScalars);
        StreamIndex(registersAssigned);
        StreamString(prm_assemblerSpecifier);
        StreamString(prm_libPath);
        StreamString(prm_include);
        StreamString(outputFileName);
        StreamString(assemblerFileExtension);
        StreamString(prm_OutputDefFile);
        StreamString(pinvoke_dll);
        StreamString(prm_snkKeyFile);
        StreamString(prm_assemblyVersion);
        StreamString(prm_namespace_and_class);
        StreamStringList(inputFiles);
        StreamStringList(backendFiles);
        StreamStringList(libIncludes);
        StreamStringList(toolArgs);
        StreamStringList(prm_Using);
        StreamIndex(bePragma.size());
        for (auto s : bePragma)
        {
            StreamString(s.first);
            StreamString(s.second);
        }
    });
}
static void StreamGlobals()
{
    StreamBlock(SBT_GLOBALSYMS, []() { StreamSymbolList(globalCache); });
}
static void StreamExternals()
{
    StreamBlock(SBT_EXTERNALS, []() { StreamSymbolList(externals); });
}
static void StreamTypes()
{
    StreamBlock(SBT_TYPES, []() {
        StreamSymbolList(typeSymbols);
        StreamSymbolList(typedefs);
    });
}
static void StreamMSILProperties()
{
    StreamBlock(SBT_MSILPROPS, []() {
        StreamIndex(msilProperties.size());
        for (auto&& s : msilProperties)
        {
            StreamIndex(s.prop->fileIndex);
            StreamIndex(s.getter->fileIndex);
            StreamIndex(s.setter->fileIndex);
        }
    });
}
static void StreamTypedefs()
{
    StreamBlock(SBT_TYPEDEFS, []() { StreamSymbolList(typedefs); });
}
static void StreamBrowse()
{
    StreamBlock(SBT_BROWSEFILES, []() {
        StreamIndex(browseFiles.size());
        for (auto s : browseFiles)
        {
            StreamBrowseFile(s);
        }
    });
    StreamBlock(SBT_BROWSEINFO, []() {
        StreamIndex(browseInfo.size());
        for (auto s : browseInfo)
        {
            StreamBrowseInfo(s);
        }
    });
}
static void StreamInstructions(QUAD* ins)
{
    int i = 0;
    for (QUAD* q = ins; q; q = q->fwd, i++)
        ;
    StreamIndex(i);
    for (QUAD* q = ins; q; q = q->fwd)
    {
        StreamInstruction(q);
    }
}
static void CacheOffset(FunctionData* fd, SimpleExpression* offset)
{
    if (offset)
    {
        if (offset->type == Optimizer::se_auto)
        {
            if (cachedAutos.find(offset->sp) == cachedAutos.end())
            {
                cachedAutos.insert(offset->sp);
                fd->temporarySymbols.push_back(offset->sp);
            }
        }
        else if (offset->type == Optimizer::se_tempref)
        {
            cachedTemps.insert(offset->sp);
        }
        else
        {
            CacheOffset(fd, offset->left);
            CacheOffset(fd, offset->right);
        }
    }
}
static void CacheImode(FunctionData* fd, IMODE* im)
{
    if (im)
    {
        auto it = cachedImodes.find(im);
        if (it == cachedImodes.end())
        {
            fd->imodeList.push_back(im);
            cachedImodes[im] = fd->imodeList.size();
        }
        CacheOffset(fd, im->offset);
        CacheOffset(fd, im->offset2);
        CacheOffset(fd, im->offset3);
    }
}
static void StreamIModes(FunctionData* fd)
{
    StreamBlock(SBT_IMODES, [fd]() {
        StreamIndex(fd->imodeList.size());
        for (auto v : fd->imodeList)
        {
            StreamOperand(v);
        }
    });
}
static void StreamTemps()
{
    int count = 0;
    for (auto t : cachedTemps)
    {
        if (t->loadTemp | t->pushedtotemp)
            count++;
    }
    StreamIndex(count);
    for (auto t : cachedTemps)
    {
        if (t->loadTemp | t->pushedtotemp)
        {
            StreamIndex(t->i);
            int val = 0;
            if (t->loadTemp)
                val |= TF_LOADTEMP;
            if (t->pushedtotemp)
                val |= TF_PUSHEDTOTEMP;
            StreamByte(val);
        }
    }
}
static void StreamLoadCache(std::map<IMODE*, IMODE*> hash)
{
    StreamIndex(hash.size());
    for (auto v : hash)
    {
        StreamIndex(cachedImodes[v.first]);
        StreamIndex(cachedImodes[v.second]);  // a tempreg number
    }
}
static void StreamFunc(FunctionData* fd)
{
    cachedAutos.clear();
    cachedTemps.clear();
    for (auto v : fd->variables)
        if (v->storage_class == scc_auto || v->storage_class == scc_parameter)
            cachedAutos.insert(v);
    for (auto v : fd->temporarySymbols)
        if (v->storage_class == scc_auto || v->storage_class == scc_parameter)
            cachedAutos.insert(v);

    cachedImodes.clear();
    fd->imodeList.clear();
    for (auto q = fd->instructionList; q; q = q->fwd)
    {
        switch (q->dc.opcode)
        {
            case i_icon:
            case i_imcon:
            case i_fcon:
            case i_cxcon:
            case i_label:
            case i_line:
            case i_passthrough:
                break;
            default:
                CacheImode(fd, q->dc.left);
                CacheImode(fd, q->dc.right);
                break;
        }
        CacheImode(fd, q->ans);
    }
    // have to do this here because inlining results in symbols being used across functions...
    int i = 1;
    for (auto s : fd->variables)
    {
        s->fileIndex = 2 * i++ + 1;
    }
    for (auto s : fd->temporarySymbols)
    {
        if (s->storage_class != scc_localstatic)
            s->fileIndex = 2 * i++ + 1;
    }
    StreamIndex(fd->name->fileIndex);
    StreamIndex((fd->setjmp_used ? FF_USES_SETJMP : 0) + (fd->hasAssembly ? FF_HAS_ASSEMBLY : 0));
    StreamIndex(fd->blockCount);
    StreamIndex(fd->tempCount);
    StreamIndex(fd->exitBlock);
    StreamIndex(fd->fastcallAlias);
    StreamSymbolList(fd->variables);
    StreamSymbolList(fd->temporarySymbols);
    StreamIModes(fd);
    StreamExpression(fd->fltexp);
    StreamInstructions(fd->instructionList);
    StreamTemps();
    StreamLoadCache(fd->loadHash);
}
static void StreamData()
{
    StreamBlock(SBT_DATA, []() {
        StreamIndex(baseData.size());
        for (auto data : baseData)
        {
            StreamIndex(data->type);
            StreamBlock(data->type, [data]() {
                switch (data->type)
                {
                    case DT_NONE:
                        break;
                    case DT_SEG:
                    case DT_SEGEXIT:
                        StreamIndex(data->i);
                        break;
                    case DT_DEFINITION:
                        StreamIndex(data->symbol.sym->fileIndex);
                        StreamIndex(data->symbol.i);
                        break;
                    case DT_LABELDEFINITION:
                        StreamIndex(data->i);
                        break;
                    case DT_RESERVE:
                        StreamIndex(data->i);
                        break;
                    case DT_SYM:
                        StreamIndex(data->symbol.sym->fileIndex);
                        StreamIndex(data->symbol.i);
                        break;
                    case DT_SRREF:
                        StreamIndex(data->symbol.sym->fileIndex);
                        StreamIndex(data->symbol.i);
                        break;
                    case DT_PCREF:
                        StreamIndex(data->symbol.sym->fileIndex);
                        break;
                    case DT_FUNCREF:
                        StreamIndex(data->symbol.sym->fileIndex);
                        StreamIndex(data->symbol.i);
                        break;
                    case DT_LABEL:
                        StreamIndex(data->i);
                        break;
                    case DT_LABDIFFREF:
                        StreamIndex(data->diff.l1);
                        StreamIndex(data->diff.l2);
                        break;
                    case DT_STRING:
                    {
                        bool instring = false;
                        StreamIndex(data->astring.i);
                        for (int i = 0; i < data->astring.i; i++)
                        {
                            StreamByte(data->astring.str[i]);
                        }
                    }
                    break;
                    case DT_BIT:
                        break;
                    case DT_BOOL:
                        StreamIntValue(&data->i, 1);
                        break;
                    case DT_BYTE:
                        StreamIntValue(&data->i, 1);
                        break;
                    case DT_USHORT:
                        StreamIntValue(&data->i, 2);
                        break;
                    case DT_UINT:
                        StreamIntValue(&data->i, 4);
                        break;
                    case DT_ULONG:
                        StreamIntValue(&data->i, 8);
                        break;
                    case DT_ULONGLONG:
                        StreamIntValue(&data->i, 8);
                        break;
                    case DT_16:
                        StreamIntValue(&data->i, 2);
                        break;
                    case DT_32:
                        StreamIntValue(&data->i, 4);
                        break;
                    case DT_ENUM:
                        StreamIntValue(&data->i, 4);
                        break;
                    case DT_FLOAT:
                        StreamFloatValue(data->f);
                        break;
                    case DT_DOUBLE:
                        StreamFloatValue(data->f);
                        break;
                    case DT_LDOUBLE:
                        StreamFloatValue(data->f);
                        break;
                    case DT_CFLOAT:
                        StreamFloatValue(data->c.r);
                        StreamFloatValue(data->c.i);
                        break;
                    case DT_CDOUBLE:
                        StreamFloatValue(data->c.r);
                        StreamFloatValue(data->c.i);
                        break;
                    case DT_CLONGDOUBLE:
                        StreamFloatValue(data->c.r);
                        StreamFloatValue(data->c.i);
                        break;
                    case DT_ADDRESS:
                        StreamIntValue(&data->i, 8);
                        break;
                    case DT_VIRTUAL:
                        StreamIndex(data->symbol.sym->fileIndex);
                        StreamIndex(data->symbol.i);
                        break;
                    case DT_ENDVIRTUAL:
                        StreamIndex(data->symbol.sym->fileIndex);
                        break;
                    case DT_ALIGN:
                        StreamIndex(data->i);
                        break;
                    case DT_VTT:
                        StreamIndex(data->symbol.sym->fileIndex);
                        StreamIntValue(&data->symbol.i, sizeof(data->symbol.i));
                        break;
                    case DT_IMPORTTHUNK:
                        StreamIndex(data->symbol.sym->fileIndex);
                        break;
                    case DT_VC1:
                        StreamIndex(data->symbol.sym->fileIndex);
                        break;
                    case DT_AUTOREF:
                        StreamIndex(data->symbol.sym->fileIndex);
                        StreamIndex(data->symbol.i);
                        break;
                    case DT_FUNC:
                        StreamFunc(data->funcData);
                        break;
                }
            });
        }
    });
}
void WriteText()
{
    StreamBlock(SBT_TEXT, []() {
        StreamIndex(textOffset);
        for (auto&& t : textRegion)
        {
            StreamIndex(t.size());
            for (auto c : t)
                StreamByte(c);
        }
    });
}
// in the following, the low bit being set signifies the data in the stream
// is an index rather than a pointer to a symbol (used in load resolution)
static void NumberGlobals()
{
    int i = 1;
    for (auto s : globalCache)
    {
        s->fileIndex = 2 * i++ + 1;
    }
    for (auto it = externals.begin(); it != externals.end(); ++it)
    {
        if (*it)
        {
            if ((*it)->fileIndex && !(*it)->dontinstantiate &&
                (*it)->initialized)  // && ((*it)->storage_class != scc_virtual || (*it)->hasInlineFunc))
            {
                *it = 0;
            }
            else
            {
                (*it)->fileIndex = 2 * i + 1;
            }
        }
        i++;
    }
}
static void NumberTypes()
{
    int i = 1;
    for (auto s : definedFunctions)
        if (!s->fileIndex)
            typeSymbols.push_back(s);
    for (auto s : typeSymbols)
    {
        s->typeIndex = 2 * i++ + 1;
        // static members
        if (s->storage_class != scc_external && s->storage_class != scc_global)
            s->fileIndex = s->typeIndex;
    }
    for (auto s : typedefs)
        s->typeIndex = s->fileIndex = 2 * i++ + 1;
}
int GetOutputSize() { return outputPos; }
void OutputIntermediate(SharedMemory* mem)
{
    sharedRegion = mem;
    streamPointer = sharedRegion->GetMapping();
    textRegion.clear();
    textOffset = 1;
    cachedText.clear();
    NumberGlobals();
    NumberTypes();
    StreamHeader();
    StreamParams();
    StreamXParams();
    StreamGlobals();
    StreamExternals();
    StreamTypes();
    StreamBrowse();
    StreamMSILProperties();
    StreamData();
    WriteText();
}
}  // namespace Optimizer