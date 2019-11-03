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

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <limits.h>
#include "iexpr.h"
#include "beinterf.h"
#include "ildata.h"
#include "iexpr.h"
#include "../occ/Winmode.h"
#include "../occ/be.h"
#include <deque>
#include <functional>
#include <map>
#include <set>
extern std::vector<SimpleSymbol*> externals;
extern std::vector<SimpleSymbol*> globalCache;
extern std::vector<SimpleSymbol*> typeSymbols;
extern std::vector<SimpleSymbol*> typedefs;
extern std::vector<BROWSEINFO*> browseInfo;
extern std::vector<BROWSEFILE*> browseFiles;

extern std::deque<BaseData*> baseData;
extern std::list<MsilProperty> msilProperties;

extern std::string prm_libPath;
extern std::string prm_include;
extern const char* pinvoke_dll;
extern std::string prm_snkKeyFile;
extern std::string prm_assemblyVersion;
extern std::string prm_namespace_and_class;
extern std::string prm_OutputDefFile;
extern std::string compilerName;
extern std::string intermediateName;
extern std::string backendName;

extern int exitBlock;
extern int nextLabel;
extern bool assembling;
extern int fastcallAlias;

extern int showBanner;
extern int verbosity;
extern int dataAlign;
extern int bssAlign;
extern int constAlign;
extern int architecture;

extern std::list<std::string> inputFiles;
extern std::list<std::string> backendFiles;
extern std::list<std::string> libIncludes;
extern std::list<std::string> toolArgs;
extern std::list<std::string> prm_Using;
extern std::map<std::string, std::string> bePragma;

extern std::string outputFileName;
extern std::string prm_assemblerSpecifier;

extern FILE* outputFile;

static std::list<std::string> textRegion;
static std::map <std::string, int> cachedText;
static size_t textOffset;
static std::map<IMODE*, int> cachedImodes;
static char outBuf[8192];
static int outLevel;
static std::set<SimpleSymbol*> cachedAutos;

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
inline static void StreamByte(int value)
{
    outBuf[outLevel++] = value;
    if (outLevel >= sizeof(outBuf))
    {
        owrite(outBuf, 1, outLevel, outputFile);
        outLevel = 0;
    }
}
inline static void StreamBlockType(int blockType, bool end)
{
    StreamByte(blockType + (end ? 0x80 : 0x40));

}
template <class T>
inline static void StreamBlock(T blockType, std::function<void(void)> blockRenderer)
{
    StreamBlockType(blockType, false);
    blockRenderer();
    StreamBlockType(blockType, true);
}

inline static void StreamInt(int value)
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
        StreamInt(0);
    else
        StreamInt(TextName(name));
}
inline static void StreamString(const std::string&value)
{
    StreamInt(value.size());
    for (auto c : value)
        StreamByte(c);
}
static void StreamStringList(const std::list<std::string> & list)
{
    StreamInt(list.size());
    for (auto&& s : list)
        StreamString(s);
}
static void StreamBuffer(const void *buf, int len)
{
    owrite(outBuf, outLevel, 1, outputFile);
    outLevel = 0;
    owrite((char *)buf, len, 1, outputFile);
}
inline static void StreamIntValue(const void *buf, int len)
{
    StreamBlock(STT_INT, [buf, len]() {
        for (int i = len - 1; i >= 0; i--)
            StreamByte(((unsigned char *)buf)[i]);
    });
}
static void StreamFloatValue(FPF& fv)
{
    StreamBlock(STT_FLOAT, [&fv]() {
        StreamInt(fv.type);
        StreamInt(fv.sign);
        StreamInt(fv.exp);
        for (int i = 0; i < INTERNAL_FPF_PRECISION; i++)
            StreamInt(fv.mantissa[i]);
    });

}
static void StreamSymbol(SimpleSymbol* sym);
static void StreamSymbolTable(LIST *syms)
{
    int i = 0;
    for (auto l = syms; l; l = l->next, i++);
    StreamInt(i);
    for (auto l = syms; l; l = l->next)
    {
        StreamInt(((SimpleSymbol*)l->data)->fileIndex);
    }
}
static void StreamBases(BaseList *bases)
{
    BaseList* baseClasses;
    int i = 0;
    for (auto l = bases; l; l = l->next, i++);
    StreamInt(i);
    for (auto l = bases; l; l = l->next)
    {
        StreamBlock(STT_BASE, [l]() {
            StreamInt(l->sym->fileIndex);
            StreamInt(l->offset);
        });
    }

}
static void StreamType(SimpleType* type)
{
    StreamBlock(STT_TYPE, [type]() {
        if (!type)
        {
            StreamInt(st_none);
        }
        else
        {
            StreamInt(type->type);
            StreamInt(type->size);
            StreamInt(type->sizeFromType);
            StreamInt(type->bits);
            StreamInt(type->startbit);
            if (type->sp)
                StreamInt(type->sp->fileIndex);
            else
                StreamInt(0);
            StreamInt(type->flags);
            StreamType(type->btp);
        }
    });
}
static void StreamSymbol(SimpleSymbol* sym)
{
    StreamBlock(STT_SYMBOL, [sym]() {
        if (!sym)
        {
            StreamInt(scc_none);
        }
        else
        {
            StreamInt(sym->storage_class);
            StreamTextIndex(sym->name);
            StreamTextIndex(sym->outputName);
            StreamTextIndex(sym->importfile);
            StreamTextIndex(sym->namespaceName);
            StreamTextIndex(sym->msil);
            StreamInt(sym->i);
            StreamInt(sym->offset);
            StreamInt(sym->label);
            StreamInt(sym->templateLevel);
            StreamInt(sym->flags >> 32);
            StreamInt(sym->flags & 0xffffffff);
            StreamInt(sym->sizeFromType);
            StreamInt(sym->align);
            StreamInt(sym->size);
            if (sym->parentClass)
                StreamInt(sym->parentClass->fileIndex);
            else
                StreamInt(0);
            if (sym->paramSubstitute)
                StreamInt(sym->paramSubstitute->fileIndex);
            else
                StreamInt(0);
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
            StreamInt(se_none);
        }
        else
        {
            StreamInt(exp->type);
            StreamInt(exp->flags);
            switch (exp->type)
            {
            case se_i:
            case se_ui:
                StreamIntValue(&exp->i, 8);
                break;
            case se_f:
            case se_fi:
                StreamFloatValue(exp->f);
                break;
            case se_fc:
                StreamFloatValue(exp->c.r);
                StreamFloatValue(exp->c.i);
                break;
            case se_const:
            case se_absolute:
            case se_auto:
            case se_global:
            case se_threadlocal:
            case se_pc:
            case se_structelem:
                StreamInt(exp->sp->fileIndex);
                break;
            case se_labcon:
                StreamInt(exp->i);
                break;
            case se_tempref:
                StreamInt(exp->sp->i);
                break;
            case se_msil_array_access:
                StreamType(exp->msilArrayTP);
                break;
            case se_msil_array_init:
                StreamType(exp->tp);
                break;
            case se_string:
                StreamInt(exp->astring.len);
                for (int i = 0; i < exp->astring.len; i++)
                    StreamByte(exp->astring.str[i]);
                break;
            }
            StreamExpression(exp->left);
            StreamExpression(exp->right);
            StreamExpression(exp->altData);
        }
    });
}
static void StreamBrowseFile(BROWSEFILE *bf)
{
    StreamBlock(STT_BROWSEFILE, [bf]() {
        StreamTextIndex(bf->name);
        StreamInt(bf->filenum);
    });
}
static void StreamBrowseInfo(BROWSEINFO *bi)
{
    StreamBlock(STT_BROWSEINFO, [bi]() {
        StreamTextIndex(bi->name);
        StreamInt(bi->filenum);
        StreamInt(bi->type);
        StreamInt(bi->lineno);
        StreamInt(bi->charpos);
        StreamInt(bi->flags);
    });

}

static void StreamAssemblyOperand(AMODE *im)
{
    if (!im)
    {
        StreamInt(am_none);
    }
    else
    {
        StreamInt(im->mode);
        StreamInt(im->preg);
        StreamInt(im->sreg);
        StreamInt(im->tempflag);
        StreamInt(im->scale);
        StreamInt(im->length);
        StreamInt(im->addrlen);
        StreamInt(im->seg);
        StreamIntValue(&im->liveRegs, 8);
        StreamInt(im->keepesp);
        StreamExpression(im->offset);

    }
}
static void StreamAssemblyInstruction(OCODE *oc)
{
    StreamInt(oc->opcode);
    StreamInt(oc->diag);
    StreamInt(oc->noopt);
    StreamInt(oc->size);
    StreamInt(oc->blocknum);
    StreamAssemblyOperand(oc->oper1);
    StreamAssemblyOperand(oc->oper2);
    StreamAssemblyOperand(oc->oper3);
}
static void StreamOperand(IMODE* im)
{
    StreamBlock(STT_OPERAND, [im]() {
        StreamInt(im->mode);
        StreamInt(im->scale);
        StreamInt(im->useindx);
        StreamInt(im->size);
        StreamInt(im->ptrsize);
        StreamInt(im->startbit);
        StreamInt(im->bits);
        StreamInt(im->seg);
        StreamInt(im->flags);
        StreamExpression(im->offset);
        StreamExpression(im->offset2);
        StreamExpression(im->offset3);
        StreamExpression(im->vararg);
    });
}

static void StreamInstruction(QUAD *q)
{
    StreamBlock(STT_BROWSEFILE, [q]() {
        StreamInt(q->dc.opcode);
        if (q->dc.opcode == i_passthrough)
        {
            StreamAssemblyInstruction((OCODE *)q->dc.left);
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
                StreamInt(q->dc.v.label);
                break;
            case i_line:
                i = 0;
                for (auto v = (LINEDATA*)q->dc.left; v; v = v->next, i++);
                StreamInt(i);
                for (auto v = (LINEDATA*)q->dc.left; v; v = v->next)
                {
                    StreamInt(v->lineno);
                    StreamTextIndex(v->line);
                }
                break;
            case i_block:
            case i_blockend:
                StreamInt(q->block->blocknum);
                break;
            case i_dbgblock:
            case i_dbgblockend:
            case i_livein:
                break;
            case i_func:
                StreamInt(q->dc.v.label);
                if (q->dc.left)
                    StreamInt(cachedImodes[q->dc.left]);
                else
                    StreamInt(0);
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
                StreamInt(q->dc.v.label);
                // fallthrough
            default:
                if (q->dc.left)
                    StreamInt(cachedImodes[q->dc.left]);
                else
                    StreamInt(0);
                if (q->dc.right)
                    StreamInt(cachedImodes[q->dc.right]);
                else
                    StreamInt(0);
                break;
            }
            if (q->ans)
                StreamInt(cachedImodes[q->ans]);
            else
                StreamInt(0);
            if (q->altsp)
                StreamInt(q->altsp->fileIndex);
            else
                StreamInt(0);
            StreamType(q->alttp);
            i = 0;
            for (auto v = (ArgList*)q->altargs; v; v = v->next, i++);
            StreamInt(i);
            for (auto v = (ArgList*)q->altargs; v; v = v->next)
            {
                StreamType(v->tp);
                StreamExpression(v->exp);
            }
            StreamInt(q->ansColor);
            StreamInt(q->leftColor);
            StreamInt(q->rightColor);
            StreamInt(q->scaleColor);
            StreamInt(q->flags);
            StreamInt(q->definition);
            StreamInt(q->available);
            StreamInt(q->sourceindx);
            StreamInt(q->copy);
            StreamInt(q->retcount);
            StreamInt(q->sehMode);
            StreamInt(q->fastcall);
            StreamInt(q->oldmode);
            StreamInt(q->novalue);
            StreamInt(q->temps);
            StreamInt(q->precolored);
            StreamInt(q->moved);
            StreamInt(q->livein);
        }
    });
}
static void StreamSymbolList(const std::vector<SimpleSymbol*>& list)
{
    StreamInt(list.size());
    for (auto s : list)
        StreamSymbol(s);
}

static void StreamHeader()
{
    StreamBuffer(magic, strlen(magic));
    StreamIntValue(&fileVersion, sizeof(fileVersion));
    StreamInt(architecture);
}
static void StreamParams()
{
    StreamBlock(SBT_PARAMS, []() {
        StreamBuffer(&cparams, sizeof(cparams));
    });
}
static void StreamXParams()
{
    StreamBlock(SBT_XPARAMS, []() {
        StreamString(compilerName);
        StreamString(intermediateName);
        StreamString(backendName);
        StreamInt(showBanner);
        StreamInt(verbosity);
        StreamInt(assembling);
        StreamInt(dataAlign);
        StreamInt(bssAlign);
        StreamInt(constAlign);
        StreamString(prm_assemblerSpecifier);
        StreamString(prm_libPath);
        StreamString(prm_include);
        StreamString(outputFileName);
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
        StreamInt(bePragma.size());
        for (auto s : bePragma)
        {
            StreamString(s.first);
            StreamString(s.second);
        }
    });
}
static void StreamGlobals()
{
    StreamBlock(SBT_GLOBALSYMS, []() {
        StreamSymbolList(globalCache);
    });
}
static void StreamExternals()
{
    StreamBlock(SBT_EXTERNALS, []() {
        StreamSymbolList(externals);
    });
}
static void StreamTypes()
{
    StreamBlock(SBT_TYPES, []() {
        StreamSymbolList(typeSymbols);
    });
}
static void StreamMSILProperties()
{
    StreamBlock(SBT_MSILPROPS, []() {
        StreamInt(msilProperties.size());
        for (auto&&s : msilProperties)
        {
            StreamInt(s.prop->fileIndex);
            StreamInt(s.getter->fileIndex);
            StreamInt(s.setter->fileIndex);
        }
    });
}
static void StreamTypedefs()
{
    StreamBlock(SBT_TYPEDEFS, []() {
        StreamSymbolList(typedefs);
    });

}
static void StreamBrowse()
{
    StreamBlock(SBT_BROWSEFILES, []() {
        StreamInt(browseFiles.size());
        for (auto s : browseFiles)
        {
            StreamBrowseFile(s);
        }
    });
    StreamBlock(SBT_BROWSEINFO, []() {
        StreamInt(browseInfo.size());
        for (auto s : browseInfo)
        {
            StreamBrowseInfo(s);
        }
    });

}
static void StreamInstructions(QUAD *ins)
{
    int i=0;
    for (QUAD *q = ins; q; q = q->fwd, i++);
    StreamInt(i);
    for (QUAD *q = ins; q; q = q->fwd)
        StreamInstruction(q);

}
static void CacheOffset(FunctionData* fd, SimpleExpression*offset)
{
    if (offset)
    {
        if (offset->type == se_auto)
        {
            if (cachedAutos.find(offset->sp) == cachedAutos.end())
            {
                cachedAutos.insert(offset->sp);
                fd->temporarySymbols.push_back(offset->sp);
            }
        }
        else
        {
            CacheOffset(fd, offset->left);
            CacheOffset(fd, offset->right);
        }
    }
}
static void CacheImode(FunctionData* fd, IMODE *im)
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
        StreamInt(fd->imodeList.size());
        for (auto v : fd->imodeList)
        {
            StreamOperand(v);
        }
    });
}
static void StreamFunc(FunctionData *fd)
{
    cachedAutos.clear();
    for (auto v : fd->variables)
        if (v->storage_class == scc_auto)
            cachedAutos.insert(v);
    for (auto v : fd->temporarySymbols)
        if (v->storage_class == scc_auto)
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
        s->fileIndex = 2 * i++ + 1;
    for (auto s : fd->temporarySymbols)
        s->fileIndex = 2 * i++ + 1;


    StreamInt(fd->name->fileIndex);
    StreamInt((fd->setjmp_used ? 1 : 0) + (fd->hasAssembly ? 2 : 0));
    StreamInt(fd->blockCount);
    StreamInt(fd->tempCount);
    StreamInt(fd->exitBlock);
    StreamInt(fd->nextLabel);
    StreamSymbolList(fd->variables);
    StreamSymbolList(fd->temporarySymbols);
    StreamIModes(fd);
    StreamExpression(fd->objectArray_exp);
    StreamInstructions(fd->instructionList);
}
static void StreamData()
{
    StreamBlock(SBT_DATA, []() {
        StreamInt(baseData.size());
        for (auto data : baseData)
        {
            StreamInt(data->type);
            StreamBlock(data->type, [data]() {
                switch (data->type)
                {
                case DT_NONE:
                    break;
                case DT_SEG:
                case DT_SEGEXIT:
                    StreamInt(data->i);
                    break;
                case DT_DEFINITION:
                    StreamInt(data->symbol.sym->fileIndex);
                    StreamInt(data->symbol.i);
                    break;
                case DT_LABELDEFINITION:
                    StreamInt(data->i);
                    break;
                case DT_RESERVE:
                    StreamInt(data->i);
                    break;
                case DT_SYM:
                    StreamInt(data->symbol.sym->fileIndex);
                    break;
                case DT_SRREF:
                    StreamInt(data->symbol.sym->fileIndex);
                    StreamInt(data->symbol.i);
                    break;
                case DT_PCREF:
                    StreamInt(data->symbol.sym->fileIndex);
                    break;
                case DT_FUNCREF:
                    StreamInt(data->symbol.sym->fileIndex);
                    StreamInt(data->symbol.i);
                    break;
                case DT_LABEL:
                    StreamInt(data->i);
                    break;
                case DT_LABDIFFREF:
                    StreamInt(data->diff.l1);
                    StreamInt(data->diff.l2);
                    break;
                case DT_STRING:
                {
                    bool instring = false;
                    StreamInt(data->astring.i);
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
                    StreamInt(data->symbol.sym->fileIndex);
                    StreamInt(data->symbol.i);
                    break;
                case DT_ENDVIRTUAL:
                    StreamInt(data->symbol.sym->fileIndex);
                    break;
                case DT_ALIGN:
                    StreamInt(data->i);
                    break;
                case DT_VTT:
                    StreamInt(data->symbol.sym->fileIndex);
                    StreamInt(data->symbol.i);
                    break;
                case DT_IMPORTTHUNK:
                    StreamInt(data->symbol.sym->fileIndex);
                    break;
                case DT_VC1:
                    StreamInt(data->symbol.sym->fileIndex);
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
        StreamInt(textOffset);
        for (auto&& t : textRegion)
        {
            StreamInt(t.size());
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
            if ((*it)->fileIndex)
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
    for (auto s : typeSymbols)
        s->fileIndex = 2 * i++ + 1;
    i = 1;
    for (auto s : typedefs)
        s->fileIndex = 2 * i++ + 1;

}
void OutputIntermediate()
{
    textRegion.clear();
    textOffset = 1;
    outLevel = 0;
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
    owrite(outBuf, 1, outLevel, outputFile);
}