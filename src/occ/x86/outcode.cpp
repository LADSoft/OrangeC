/* Software License Agreement
 *
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 *
 *     This file is part of the Orange C Compiler package.
 *
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the
 *     Orange C "Target Code" exception.
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
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "be.h"
#include "Label.h"
#include "Fixup.h"
#include "Instruction.h"
#include "Section.h"
#include "ObjIeee.h"
#include "ObjFile.h"
#include "ObjFactory.h"
#include "ObjSection.h"
#include "ObjDebugTag.h"
#include "ObjMemory.h"
#include "ObjLineNo.h"
#include "dbgtypes.h"
#include <map>
#include <set>
#include <deque>

#if 0
#    include <new>
void * operator new(std::size_t n) throw(std::bad_alloc)
{
    return (void *)Alloc(n);
}
void operator delete(void * p) throw()
{
    // noop
}
#endif
#define FULLVERSION

extern char realOutFile[260];
extern ARCH_ASM* chosenAssembler;
extern MULDIV* muldivlink;
extern ASMNAME oplst[];
extern enum e_sg oa_currentSeg;
extern DBGBLOCK* DbgBlocks[];
extern SYMBOL* theCurrentFunc;
extern int fastcallAlias;
extern FILE *outputFile, *browseFile;
extern char infile[];
extern int usingEsp;
extern AMODE* singleLabel, *doubleLabel, *zerolabel;
LIST* includedFiles;
InstructionParser* instructionParser;
Section* currentSection;

static char* segnames[] = {0,         "code",     "data",     "bss",        "string",     "const",
                           "tls",     "cstartup", "crundown", "tlsstartup", "tlsrundown", "codefix",
                           "datafix", "lines",    "types",    "symbols",    "browse"};

static int segAlignsDefault[] = {1, 2, 8, 8, 2, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
static int segFlags[] = {0,
                         ObjSection::rom,
                         ObjSection::ram,
                         ObjSection::ram,
                         ObjSection::rom,
                         ObjSection::rom,
                         ObjSection::rom,
                         ObjSection::rom,
                         ObjSection::rom,
                         ObjSection::rom,
                         ObjSection::rom,
                         ObjSection::rom,
                         ObjSection::rom,
                         ObjSection::rom,
                         ObjSection::rom,
                         ObjSection::rom,
                         ObjSection::rom,
                         ObjSection::rom,
                         ObjSection::rom};
static int virtualSegFlags = ObjSection::max | ObjSection::virt;

extern int segAligns[MAX_SEGS] = {};

static int virtualSegmentNumber;
static int lastIncludeNum;

static int sectionMap[MAX_SEGS];
static std::map<int, Label*> labelMap;

static std::vector<SYMBOL*> autotab;
static std::vector<Label*> strlabs;
static std::map<std::string, Label*> lblpubs;
static std::map<std::string, Label*> lbllabs;
static std::map<std::string, Label*> lblExterns;
static std::map<std::string, Label*> lblvirt;
static std::vector<Section*> virtuals;
static std::map<Section*, SYMBOL*> virtualSyms;

static Section* sections[MAX_SEGS];

static int autoCount;

struct symname
{
    bool operator()(const SYMBOL* left, const SYMBOL* right) const { return strcmp(left->decoratedName, right->decoratedName) < 0; }
};
static std::set<SYMBOL*, symname> globals;
static std::set<SYMBOL*, symname> externs;
static std::map<SYMBOL*, int> autos;
static std::vector<ObjSymbol*> autovector;
static std::map<SYMBOL*, ObjSymbol*> objExterns;
static std::map<SYMBOL*, ObjSymbol*> objGlobals;

static std::vector<SYMBOL*> impfuncs;
static std::vector<SYMBOL*> expfuncs;
static std::vector<std::string> includelibs;

static std::map<std::string, ObjSection*> objSectionsByName;
static std::map<int, ObjSection*> objSectionsByNumber;

static std::deque<BROWSEINFO*> browseInfo;
static BROWSEFILE* browseFiles;
static std::map<int, ObjSourceFile*> sourceFiles;

extern void adjustUsesESP();
extern int dbgblocknum = 0;

static int sectofs;

static std::deque<SYMBOL*> typedefs;

static Section dummySection("dummy", -1);

void omfInit(void)
{
    browseInfo.clear();
    browseFiles = nullptr;
    globals.clear();
    externs.clear();
    autos.clear();
    autotab.clear();
    autovector.clear();
    objExterns.clear();
    objGlobals.clear();
    lblExterns.clear();
    impfuncs.clear();
    expfuncs.clear();
    includelibs.clear();
    objSectionsByNumber.clear();
    objSectionsByName.clear();
    sourceFiles.clear();
    typedefs.clear();
}
void dbginit(void)
{
    dbgblocknum = 0;
    autoCount = 0;
}

void debug_outputtypedef(SYMBOL* sp) { typedefs.push_back(sp); }

void outcode_file_init(void)
{
    int i;
    instructionParser = InstructionParser::GetInstance();
    adjustUsesESP();
    lastIncludeNum = 0;
    virtualSegmentNumber = MAX_SEGS;
    labelMap.clear();
    lbllabs.clear();
    lblpubs.clear();
    lblvirt.clear();
    strlabs.clear();
    memset(sections, 0, sizeof(sections));
    virtuals.clear();
    virtualSyms.clear();
    currentSection = 0;
    memcpy(segAligns, segAlignsDefault, sizeof(segAligns));
    singleLabel = doubleLabel = zerolabel = 0;
}

/*-------------------------------------------------------------------------*/

void outcode_func_init(void)
{
    int i;
#define NOP 0x90
}

void omf_dump_browsefile(BROWSEFILE* brf) { browseFiles = brf; }
void omf_dump_browsedata(BROWSEINFO* bri) { browseInfo.push_back(bri); }
void omf_globaldef(SYMBOL* sp) { globals.insert(sp); }
void omf_put_extern(SYMBOL* sp, int code)
{
    externs.insert(sp);
    char buf[4096];
    buf[0] = 0;
    beDecorateSymName(buf, sp);
    std::string name = buf;
    Label* l = new Label(name, lblExterns.size(), 0);
    l->SetExtern(true);
    lblExterns[l->GetName()] = l;
}
void omf_put_impfunc(SYMBOL* sp, char* file) { impfuncs.push_back(sp); }
void omf_put_expfunc(SYMBOL* sp) { expfuncs.push_back(sp); }
void omf_put_includelib(char* name) { includelibs.push_back(name); }

Label* LookupLabel(const std::string& string)
{
    auto z1 = lbllabs.find(string);
    if (z1 != lbllabs.end())
        return z1->second;
    auto z2 = lblpubs.find(string);
    if (z2 != lblpubs.end())
        return z2->second;
    auto z3 = lblExterns.find(string);
    if (z3 != lblExterns.end())
        return z3->second;
    auto z4 = lblvirt.find(string);
    if (z4 != lblvirt.end())
        return z4->second;

    return nullptr;
}
void Release()
{
    for (auto v : lblExterns)
        delete v.second;
    for (auto v : lblpubs)
        delete v.second;
    for (auto v : lbllabs)
        delete v.second;
    for (auto v : lblvirt)
        delete v.second;
    for (auto v : sections)
        delete v;
    for (auto v : virtuals)
        delete v;
}

ObjSection* LookupSection(std::string& string)
{
    auto it = objSectionsByName.find(string);
    if (it != objSectionsByName.end())
        return it->second;
    return nullptr;
}
inline int GETSECT(Label* l, int sectofs)
{
    int n = l->GetSect();
    if (n < MAX_SEGS)
        return sectionMap[n];
    return n - sectofs;
}
static void DumpFileList(ObjFactory& f, ObjFile* fi)
{
    BROWSEFILE* bf = browseFiles;
    while (bf)
    {
        ObjSourceFile* sf = f.MakeSourceFile(ObjString(bf->name));
        sourceFiles[bf->filenum] = sf;
        fi->Add(sf);
        bf = bf->next;
    }
}

void HandleDebugInfo(ObjFactory& factory, Section* sect, Instruction* ins)
{
    int n = sect->GetSect();
    if (n < MAX_SEGS)
        n = sectionMap[n];
    else
        n -= sectofs;

    ObjSection* objSection = objSectionsByNumber[n];
    ATTRIBDATA* d = (ATTRIBDATA*)ins->GetAltData();
    if (d)
    {
        ObjMemory::DebugTagContainer* dc = new ObjMemory::DebugTagContainer;
        switch (d->type)
        {
            case e_ad_blockdata:
            {
                ObjDebugTag* tag = factory.MakeDebugTag(d->start);
                dc->push_back(tag);
                objSection->GetMemoryManager().Add(dc);
            }
            break;
            case e_ad_funcdata:
            {
                ObjDebugTag* tag = factory.MakeDebugTag(objGlobals[d->v.sp], d->start);
                dc->push_back(tag);
                objSection->GetMemoryManager().Add(dc);
            }
            break;
            case e_ad_vfuncdata:
            {
                Section* s = (Section*)d->v.section;
                n = sect->GetSect();
                if (n < MAX_SEGS)
                    n = sectionMap[n];
                else
                    n -= sectofs;
                ObjDebugTag* tag = factory.MakeDebugTag(objSectionsByNumber[n], d->start);
                dc->push_back(tag);
                objSection->GetMemoryManager().Add(dc);
            }
            break;
            case e_ad_linedata:
            {
                ObjLineNo* line = factory.MakeLineNo(sourceFiles[d->v.ld->fileindex], d->v.ld->lineno);
                ObjDebugTag* tag = factory.MakeDebugTag(line);
                dc->push_back(tag);
                objSection->GetMemoryManager().Add(dc);
            }
            break;
            case e_ad_vardata:
                if (autos.find(d->v.sp) != autos.end())
                {
                    ObjSymbol* autosp = autovector[autos[d->v.sp]];
                    ObjDebugTag* tag = factory.MakeDebugTag(autosp);
                    dc->push_back(tag);
                    objSection->GetMemoryManager().Add(dc);
                }
                break;
        }
    }
}
ObjFile* MakeFile(ObjFactory& factory, std::string& name)
{
    bool rv = true;
    sectofs = 0;
    ObjFile* fi = factory.MakeFile(name);
    if (fi)
    {
        dbgtypes types(factory, fi);
        if (cparams.prm_debug)
            DumpFileList(factory, fi);
        for (int i = 0; i < MAX_SEGS; ++i)
        {
            sectionMap[i] = 0;
            if (sections[i])
            {
                sections[i]->SetAlign(segAligns[i]);

                sections[i]->Resolve();

                ObjSection* s = sections[i]->CreateObject(factory);
                if (s)
                {
                    s->SetQuals(segFlags[i]);
                    objSectionsByName[s->GetName()] = s;
                    objSectionsByNumber[s->GetIndex()] = s;
                    fi->Add(s);
                }
                sectionMap[i] = sectofs++;
            }
        }
        sectofs = MAX_SEGS - sectofs;
        for (auto v : virtuals)
        {
            v->SetAlign(v->GetName()[2] == 'c' ? segAligns[codeseg] : segAligns[dataseg]);

            ObjSection* s = v->CreateObject(factory);
            if (s)
            {
                if (cparams.prm_debug)
                    s->SetVirtualType(types.Put(virtualSyms[v]->tp));
                bool cseg = s->GetName()[2] == 'c';
                s->SetQuals((cseg ? segFlags[codeseg] : segFlags[dataseg]) + virtualSegFlags);
                objSectionsByName[s->GetName()] = s;
                objSectionsByNumber[s->GetIndex()] = s;
                fi->Add(s);
            }
        }

        if (objSectionsByNumber.size())
        {
            SYMBOL s = {};

            for (auto l : strlabs)
            {
                std::string name = l->GetName();
                s.decoratedName = (char*)name.c_str();
                auto g = globals.find(&s);
                if (g != globals.end())
                {
                    ObjSymbol* s1;
                    if ((*g)->storage_class == sc_localstatic || (*g)->storage_class == sc_static)
                        s1 = factory.MakeLocalSymbol(l->GetName());
                    else
                        s1 = factory.MakePublicSymbol(l->GetName());
                    ObjExpression* left = factory.MakeExpression(objSectionsByNumber[GETSECT(l, sectofs)]);
                    ObjExpression* right = factory.MakeExpression(l->GetOffset()->ival);
                    ObjExpression* sum = factory.MakeExpression(ObjExpression::eAdd, left, right);
                    s1->SetOffset(sum);
                    if (cparams.prm_debug)
                        s1->SetBaseType(types.Put((*g)->tp));
                    fi->Add(s1);
                    l->SetObjSymbol(s1);
                    l->SetObjectSection(objSectionsByNumber[GETSECT(l, sectofs)]);
                    objGlobals[*g] = s1;
                }
            }
            for (auto e : externs)
            {
                ObjSymbol* s1 = factory.MakeExternalSymbol(e->decoratedName);
                fi->Add(s1);
                objExterns[e] = s1;
                auto it = lblExterns.find(e->decoratedName);
                if (it != lblExterns.end())
                    it->second->SetObjSymbol(s1);
            }
            for (auto e : autotab)
            {
                ObjSymbol* s1 = factory.MakeAutoSymbol(e->decoratedName);
                if (cparams.prm_debug)
                    s1->SetBaseType(types.Put(e->tp));
                s1->SetOffset(new ObjExpression(e->offset));
                fi->Add(s1);
                autovector.push_back(s1);
            }
            for (auto e : labelMap)
            {
                Label* l = e.second;
                l->SetObjectSection(objSectionsByNumber[GETSECT(l, sectofs)]);
            }
            for (auto e : lblvirt)
            {
                Label* l = e.second;
                auto o = objSectionsByNumber[GETSECT(l, sectofs)];
                l->SetObjectSection(o);
            }
        }

        for (auto exp : expfuncs)
        {
            ObjExportSymbol* p = factory.MakeExportSymbol(exp->decoratedName);
            p->SetExternalName(exp->decoratedName);
            fi->Add(p);
        }
        for (auto import : impfuncs)
        {
            ObjImportSymbol* p = factory.MakeImportSymbol(import->decoratedName);
            p->SetExternalName(import->decoratedName);
            p->SetDllName(import->importfile);
            fi->Add(p);
        }
        for (int i = 0; i < MAX_SEGS; ++i)
        {
            if (sections[i])
            {
                if (!sections[i]->MakeData(factory, LookupLabel, LookupSection, HandleDebugInfo))
                    rv = false;
            }
        }
        for (auto v : virtuals)
        {
            if (!v->MakeData(factory, LookupLabel, LookupSection, HandleDebugInfo))
                rv = false;
        }
        if (cparams.prm_debug)
        {
            for (auto v : typedefs)
            {
                types.OutputTypedef(v);
            }
        }
    }
    if (!rv)
    {
        fi = nullptr;
    }
    return fi;
}

static void DumpFile(ObjFactory& f, ObjFile* fi, FILE* outputFile)
{
    if (fi)
    {

        std::time_t x = std::time(0);
        fi->SetFileTime(*std::localtime(&x));

        fi->ResolveSymbols(&f);

        if (outputFile != nullptr)
        {
            ObjIeee i(realOutFile);

            i.SetTranslatorName(ObjString("occ"));
            i.SetDebugInfoFlag(cparams.prm_debug && outputFile == ::outputFile);

            i.Write(outputFile, fi, &f);
        }
    }
}
ObjFile* MakeBrowseFile(ObjFactory& factory, std::string name)
{
    bool rv = true;
    int sectofs = 0;
    ObjFile* fi = factory.MakeFile(name);
    DumpFileList(factory, fi);
    for (auto bi : browseInfo)
    {
        ObjLineNo* lineno = factory.MakeLineNo(sourceFiles[bi->filenum], bi->lineno);
        ObjBrowseInfo* bd =
            factory.MakeBrowseInfo((ObjBrowseInfo::eType)bi->type, (ObjBrowseInfo::eQual)bi->flags, lineno, bi->charpos, bi->name);
        fi->Add(bd);
    }
    return fi;
}
void output_obj_file(void)
{
    ObjIeeeIndexManager im;
    ObjFactory f(&im);
    std::string name = infile;
    ObjFile* fi = MakeFile(f, name);
    DumpFile(f, fi, outputFile);

    if (cparams.prm_browse)
    {
        ObjIeeeIndexManager im1;
        ObjFactory f1(&im1);
        name = infile;
        fi = MakeBrowseFile(f1, name);
        DumpFile(f1, fi, browseFile);
    }
    Release();
}

void compile_start(char* name)
{
    LIST* newItem = (LIST*)(LIST *)beGlobalAlloc(sizeof(LIST));
    newItem->data = beGlobalAlloc(strlen(name) + 1);
    strcpy((char*)newItem->data, name);

    includedFiles = newItem;
    oa_ini();
    o_peepini();
    omfInit();
}
void include_start(char* name, int num)
{
    if (num > lastIncludeNum)
    {
        LIST* newItem = (LIST*)(LIST *)beGlobalAlloc(sizeof(LIST));
        newItem->data = beGlobalAlloc(strlen(name) + 1);
        strcpy((char*)newItem->data, name);

        if (!includedFiles)
            includedFiles = newItem;
        else
        {
            LIST* l = includedFiles;
            while (l->next)
                l = l->next;
            l->next = newItem;
        }
        lastIncludeNum = num;
    }
}
/*-------------------------------------------------------------------------*/

void outcode_enterseg(int seg)
{
    if (!sections[seg])
    {
        sections[seg] = new Section(segnames[seg], seg);
        instructionParser->Setup(sections[seg]);
    }
    currentSection = sections[seg];
    if (oa_currentSeg == codeseg)
        AsmExpr::SetSection(&dummySection);
    else
        AsmExpr::SetSection(currentSection);
}

void InsertInstruction(Instruction* ins)
{
    if (oa_currentSeg == codeseg)
        dummySection.InsertInstruction(ins);
    else
        currentSection->InsertInstruction(ins);
}
static Label* GetLabel(int lbl)
{
    Label* l = labelMap[lbl];
    if (!l)
    {
        char buf[256];
        sprintf(buf, "L_%d", lbl);
        std::string name = buf;
        l = new Label(name, labelMap.size(), currentSection->GetSect());
        labelMap[lbl] = l;
        lbllabs[l->GetName()] = l;
    }
    return l;
}
void outcode_gen_strlab(SYMBOL* sp)
{
    char buf[4096];
    buf[0] = 0;
    beDecorateSymName(buf, sp);
    std::string name = buf;
    Label* l = new Label(name, strlabs.size(), currentSection->GetSect());
    strlabs.push_back(l);
    lblpubs[name] = l;
    InsertInstruction(new Instruction(l));
}
void InsertLabel(int lbl)
{
    Label* l = GetLabel(lbl);
    l->SetSect(currentSection->GetSect());
    Instruction* newIns = new Instruction(l);
    InsertInstruction(newIns);
}

void emit(void* data, int len)
{
    Instruction* newIns = new Instruction((unsigned char*)data, len, true);
    InsertInstruction(newIns);
}
void emit(void* data, int len, Fixup* fixup, int fixofs)
{
    Instruction* newIns = new Instruction((unsigned char*)data, len, true);
    newIns->Add(fixup);
    fixup->SetInsOffs(fixofs);
    InsertInstruction(newIns);
}
void emit(Label* label)
{
    Instruction* newIns = new Instruction(label);
    InsertInstruction(newIns);
}

void emit(int size)
{
    // size < 0 = align > 0 = reserve
    if (size > 0)
    {
        Instruction* newIns = new Instruction(size, 1);
        InsertInstruction(newIns);
    }
    else
    {
        if (size < 0)
        {
            size = -size;
            Instruction* newIns = new Instruction(size);
            InsertInstruction(newIns);
        }
    }
}
/*-------------------------------------------------------------------------*/

Fixup* gen_symbol_fixup(SYMBOL* pub, int offset, bool PC)
{
    char buf[4096];
    buf[0] = 0;
    beDecorateSymName(buf, pub);
    AsmExprNode* expr = new AsmExprNode(buf);
    if (offset)
    {
        AsmExprNode* expr1 = new AsmExprNode(offset);
        expr = new AsmExprNode(AsmExprNode::ADD, expr, expr1);
    }
    Fixup* f = new Fixup(expr, 4, !!PC, PC ? 4 : 0);
    return f;
}
Fixup* gen_label_fixup(int lab, int offset, bool PC)
{
    Label* l = GetLabel(lab);
    AsmExprNode* expr = new AsmExprNode(l->GetName());
    if (offset)
    {
        AsmExprNode* expr1 = new AsmExprNode(offset);
        expr = new AsmExprNode(AsmExprNode::ADD, expr, expr1);
    }
    Fixup* f = new Fixup(expr, 4, !!PC, PC ? 4 : 0);
    return f;
}
Fixup* gen_threadlocal_fixup(SYMBOL* tls, SYMBOL* base, int offset)
{
    char buf[4096];
    buf[0] = 0;
    beDecorateSymName(buf, base);
    AsmExprNode* expr = new AsmExprNode(buf);
    buf[0] = 0;
    beDecorateSymName(buf, tls);
    AsmExprNode* expr1 = new AsmExprNode(buf);
    expr = new AsmExprNode(AsmExprNode::SUB, expr1, expr);
    if (offset)
    {
        expr1 = new AsmExprNode(offset);
        expr = new AsmExprNode(AsmExprNode::ADD, expr, expr1);
    }
    Fixup* f = new Fixup(expr, 4, false);
    return f;
}
Fixup* gen_diff_fixup(int lab1, int lab2)
{
    Label* l = GetLabel(lab1);
    AsmExprNode* expr = new AsmExprNode(l->GetName());
    l = GetLabel(lab2);
    AsmExprNode* expr1 = new AsmExprNode(l->GetName());
    expr = new AsmExprNode(AsmExprNode::SUB, expr, expr1);
    Fixup* f = new Fixup(expr, 4, false);
    return f;
}
void outcode_dump_muldivval(void)
{
    MULDIV* v = muldivlink;
    UBYTE buf[10];
    while (v)
    {
        oa_align(8);
        if (v->label)
            InsertLabel(v->label);
        if (v->size == 0)
        {
            *(int*)buf = v->value;
            emit(buf, 4);
        }
        else if (v->size == ISZ_FLOAT || v->size == ISZ_IFLOAT || v->size == ISZ_CFLOAT)
        {
            FPFToFloat(buf, &v->floatvalue);
            emit(buf, 4);
        }
        else
        {
            FPFToDouble(buf, &v->floatvalue);
            emit(buf, 8);
        }
        v = v->next;
    }
}

/*-------------------------------------------------------------------------*/

void outcode_genref(SYMBOL* sp, int offset)
{
    Fixup* f = gen_symbol_fixup(sp, offset, false);
    int i = 0;
    emit(&i, 4, f, 0);
}

/*-------------------------------------------------------------------------*/

void outcode_gen_labref(int n)
{
    Fixup* f = gen_label_fixup(n, 0, false);
    int offset = 0;
    emit(&offset, 4, f, 0);
}

/* the labels will already be resolved well enough by this point */
void outcode_gen_labdifref(int n1, int n2)
{
    Fixup* f = gen_diff_fixup(n1, n2);
    int offset = 0;
    emit(&offset, 4, f, 0);
}

/*-------------------------------------------------------------------------*/

void outcode_gensrref(SYMBOL* sp, int val)
{
    Fixup* f = gen_symbol_fixup(sp, 0, false);
    char buf[8] = {};
    buf[1] = val;

    emit(&buf, 6, f, 2);
}

/*-------------------------------------------------------------------------*/

void outcode_genstorage(int len) { emit(len); }

/*-------------------------------------------------------------------------*/

void outcode_genfloat(FPFC* val)
{
    UBYTE buf[4];
    FPFToFloat(buf, val);
    emit(buf, 4);
}

/*-------------------------------------------------------------------------*/

void outcode_gendouble(FPFC* val)
{
    UBYTE buf[8];
    FPFToDouble(buf, val);
    emit(buf, 8);
}

/*-------------------------------------------------------------------------*/

void outcode_genlongdouble(FPFC* val)
{
    UBYTE buf[10];
    FPFToLongDouble(buf, val);
    emit(buf, 10);
}

/*-------------------------------------------------------------------------*/

void outcode_genstring(LCHAR* string, int len)
{
    int i;
    for (i = 0; i < len; i++)
        outcode_genbyte(string[i]);
}

/*-------------------------------------------------------------------------*/

void outcode_genbyte(int val)
{
    char v = (char)val;
    emit(&v, 1);
}

/*-------------------------------------------------------------------------*/

void outcode_genword(int val)
{
    short v = (short)val;
    emit(&v, 2);
}

/*-------------------------------------------------------------------------*/

void outcode_genlong(int val) { emit(&val, 4); }

/*-------------------------------------------------------------------------*/

void outcode_genlonglong(LLONG_TYPE val) { emit(&val, 8); }

/*-------------------------------------------------------------------------*/

void outcode_align(int size) { emit(-size); }

/*-------------------------------------------------------------------------*/

void outcode_put_label(int lab) { InsertLabel(lab); }

/*-------------------------------------------------------------------------*/

void outcode_start_virtual_seg(SYMBOL* sp, int data)
{
    char buf[4096];
    if (data)
        strcpy(buf, "vsd@");
    else
        strcpy(buf, "vsc@");
    beDecorateSymName(buf + 3 + (sp->decoratedName[0] != '@'), sp);
    Section* virtsect = new Section(buf, virtualSegmentNumber++);
    virtualSyms[virtsect] = sp;
    virtuals.push_back(virtsect);
    currentSection = virtsect;
    instructionParser->Setup(virtsect);
    AsmExpr::SetSection(virtsect);
    std::string name = sp->decoratedName;
    Label* l = new Label(name, lblvirt.size(), virtualSegmentNumber - 1);
    l->SetOffset(0);
    lblvirt[name] = l;
}

/*-------------------------------------------------------------------------*/

void outcode_end_virtual_seg(SYMBOL* sp) { outcode_enterseg(oa_currentSeg); }

/*-------------------------------------------------------------------------*/

int resolveoffset(EXPRESSION* n, int* resolved)
{
    int rv = 0;
    if (n)
    {
        switch (n->type)
        {
            case en_sub:
                rv += resolveoffset(n->left, resolved);
                rv -= resolveoffset(n->right, resolved);
                break;
            case en_add:
                // case en_addstruc:
                rv += resolveoffset(n->left, resolved);
                rv += resolveoffset(n->right, resolved);
                break;
            case en_c_ll:
            case en_c_ull:
            case en_c_i:
            case en_c_c:
            case en_c_uc:
            case en_c_u16:
            case en_c_u32:
            case en_c_l:
            case en_c_ul:
            case en_c_ui:
            case en_c_wc:
            case en_c_s:
            case en_c_us:
            case en_absolute:
            case en_c_bool:
                rv += n->v.i;
                break;
            case en_auto:
            {
                int m = n->v.sp->offset;

                if (!usingEsp && m> 0)
                    m += 4;

                if (n->v.sp->storage_class == sc_parameter && fastcallAlias)
                {
                    if (!isstructured(basetype(theCurrentFunc->tp)->btp) || n->v.sp->offset != chosenAssembler->arch->retblocksize)
                    {

                        m-= fastcallAlias * chosenAssembler->arch->parmwidth;
                        if (m >= chosenAssembler->arch->retblocksize)
                        {
                            rv += m;
                        }
                    }
                    else
                    {
                        rv += m;
                    }
                }
                else
                {
                    rv += m;
                }
            }
            break;
            case en_labcon:
            case en_global:
            case en_pc:
            case en_threadlocal:
                *resolved = 0;
                break;
            default:
                diag("Unexpected node type in resolveoffset");
                break;
        }
    }
    return rv;
}
AsmExprNode* MakeFixup(EXPRESSION* offset)
{
    int resolved = 1;
    int n = resolveoffset(offset, &resolved);
    if (!resolved)
    {
        AsmExprNode* rv;
        if (offset->type == en_sub && offset->left->type == en_threadlocal)
        {
            EXPRESSION* node = GetSymRef(offset->left);
            EXPRESSION* node1 = GetSymRef(offset->right);
            std::string name = node->v.sp->decoratedName;
            AsmExprNode* left = new AsmExprNode(name);
            name = node1->v.sp->decoratedName;
            AsmExprNode* right = new AsmExprNode(name);
            rv = new AsmExprNode(AsmExprNode::SUB, left, right);
        }
        else
        {
            EXPRESSION* node = GetSymRef(offset);
            std::string name;
            if (node->type == en_labcon)
            {
                char buf[256];
                sprintf(buf, "L_%d", node->v.i);
                name = buf;
            }
            else
            {
                name = node->v.sp->decoratedName;
            }
            rv = new AsmExprNode(name);
        }
        if (n)
        {
            rv = new AsmExprNode(AsmExprNode::ADD, rv, new AsmExprNode(n));
        }
        return rv;
    }
    else
    {
        return new AsmExprNode(n);
    }
}
void InsertAttrib(ATTRIBDATA* ad) { InsertInstruction(new Instruction(ad)); }
void InsertLine(LINEDATA* linedata)
{
    ATTRIBDATA* attrib = (ATTRIBDATA*)Alloc(sizeof(ATTRIBDATA));
    attrib->type = e_ad_linedata;
    attrib->v.ld = linedata;
    InsertAttrib(attrib);
}
void InsertVarStart(SYMBOL* sp)
{
    if (!strstr(sp->name, "++"))
    {
        ATTRIBDATA* attrib = (ATTRIBDATA*)Alloc(sizeof(ATTRIBDATA));
        attrib->type = e_ad_vardata;
        attrib->v.sp = sp;

        InsertAttrib(attrib);
        autos[sp] = autos.size();
        autotab.push_back(sp);
    }
}
void InsertFunc(SYMBOL* sp, int start)
{
    if (oa_currentSeg == virtseg)
    {
        ATTRIBDATA* attrib = (ATTRIBDATA*)Alloc(sizeof(ATTRIBDATA));
        attrib->type = e_ad_vfuncdata;
        attrib->v.section = currentSection;
        attrib->start = !!start;
        InsertAttrib(attrib);
    }
    else
    {
        ATTRIBDATA* attrib = (ATTRIBDATA*)Alloc(sizeof(ATTRIBDATA));
        attrib->type = e_ad_funcdata;
        attrib->v.sp = sp;
        attrib->start = !!start;
        InsertAttrib(attrib);
    }
}
void InsertBlock(int start)
{
    ATTRIBDATA* attrib = (ATTRIBDATA*)Alloc(sizeof(ATTRIBDATA));
    attrib->type = e_ad_blockdata;
    attrib->start = !!start;
    InsertAttrib(attrib);
}

void AddFixup(Instruction* newIns, OCODE* ins, const std::list<Numeric*>& operands)
{
    if (ins->opcode == op_dd)
    {
        int resolved = 1;
        int n = resolveoffset(ins->oper1->offset, &resolved);
        if (!resolved)
        {
            memcpy(newIns->GetData(), &n, 4);
            AsmExprNode* expr = MakeFixup(ins->oper1->offset);
            Fixup* f = new Fixup(expr, 4, false);
            newIns->Add(f);
        }
    }
    else
    {
        for (auto operand : operands)
        {
            if (operand->used && operand->size &&
                (operand->node->GetType() == AsmExprNode::LABEL || operand->node->GetType() == AsmExprNode::SUB ||
                 operand->node->GetType() == AsmExprNode::ADD))
            {
                if (newIns->Lost() && operand->pos)
                    operand->pos -= 8;
                int n = operand->relOfs;
                if (n < 0)
                    n = -n;
                Fixup* f = new Fixup(operand->node, (operand->size + 7) / 8, operand->relOfs != 0, n, operand->relOfs > 0);
                f->SetInsOffs((operand->pos + 7) / 8);
                newIns->Add(f);
            }
        }
    }
}

void outcode_diag(OCODE* ins, char* str)
{
    std::string instruction = instructionParser->FormatInstruction(ins);
    std::string name("Error compiling assembly instruction \"" + instruction + "\": " + str);
    diag(name.c_str());
}
/*-------------------------------------------------------------------------*/

void outcode_AssembleIns(OCODE* ins)
{
    if (ins->opcode >= op_aaa)
    {
        Instruction* newIns = nullptr;
        std::list<Numeric*> operands;

        asmError err = instructionParser->GetInstruction(ins, newIns, operands);

        switch (err)
        {

            case AERR_NONE:
                AddFixup(newIns, ins, operands);
                InsertInstruction(newIns);
                ins->ins = newIns;
                break;
            case AERR_SYNTAX:
                outcode_diag(ins, "Syntax error while parsing instruction");
                break;
            case AERR_OPERAND:
                outcode_diag(ins, "Unknown operand");
                break;
            case AERR_BADCOMBINATIONOFOPERANDS:
                outcode_diag(ins, "Bad combination of operands");
                break;
            case AERR_UNKNOWNOPCODE:
                outcode_diag(ins, "Unrecognized opcode");
                break;
            case AERR_INVALIDINSTRUCTIONUSE:
                outcode_diag(ins, "Invalid use of instruction");
                break;
            default:
                outcode_diag(ins, "unknown assembler error");
                break;
        }
    }
    else
    {
        switch (ins->opcode)
        {
            case op_label:
                InsertLabel((int)ins->oper1);
                return;
            case op_line:
                if (cparams.prm_debug)
                {
                    LINEDATA* ld = (LINEDATA*)ins->oper1;
                    while (ld->next)
                        ld = ld->next;
                    InsertLine(ld);
                }
                break;
            case op_varstart:
                if (cparams.prm_debug)
                    InsertVarStart((SYMBOL*)ins->oper1);
                break;
            case op_blockstart:
                if (cparams.prm_debug)
                    InsertBlock(1);
                break;
            case op_blockend:
                if (cparams.prm_debug)
                    InsertBlock(0);
                break;
            case op_funcstart:
                if (cparams.prm_debug)
                    InsertFunc((SYMBOL*)ins->oper1, 1);
                break;
            case op_funcend:
                if (cparams.prm_debug)
                    InsertFunc((SYMBOL*)ins->oper1, 0);
                break;
            case op_genword:
                outcode_genbyte(ins->oper1->offset->v.i);
                break;
            case op_align:
            {
                Instruction* newIns = new Instruction(ins->oper1->offset->v.i);
                InsertInstruction(newIns);
                break;
            }
            case op_dd:
            {
                int i = 0;
                Instruction* newIns = new Instruction(&i, 4, true);
                const std::list<Numeric*> operands;
                AddFixup(newIns, ins, operands);
                InsertInstruction(newIns);
                // reserve 4
                break;
            }
            default:
                break;
        }
    }
}

/*-------------------------------------------------------------------------*/

void outcode_gen(OCODE* peeplist)
{
    OCODE* head = peeplist;
    outcode_func_init();
    while (head)
    {
        outcode_AssembleIns(head);

        head = head->fwd;
    }
    if (oa_currentSeg == codeseg)
    {
        // all this dancing around so that when compiling things into the main code segment, we don't spend an inordinate amound of
        // time in Resolve()...
        //
        dummySection.Resolve();
        for (auto d : dummySection.GetInstructions())
        {
            currentSection->InsertInstruction(d);
        }
        dummySection.ClearInstructions();
    }
    else
    {
        currentSection->Resolve();
    }
}
