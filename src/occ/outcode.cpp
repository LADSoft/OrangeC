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
#include <cctype>
#include <cstdlib>
#include <cstring>
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
#include <unordered_map>
#include <set>
#include <deque>
#include "config.h"
#include "ildata.h"
#include "gen.h"
#include "occ.h"
#include "outasm.h"
#include "output.h"
#include "peep.h"
#include "ioptimizer.h"
#include "InstructionParser2.h"
#include "memory.h"
#include "outcode.h"

#define FULLVERSION

extern void adjustUsesESP();
namespace occx86
{

static Section* currentSection;

static const char* segnames[] = {0,          "code",     "data",    "bss",     "string", "const", "tls",     "cstartup", "crundown",
                                 "tstartup", "trundown", "codefix", "datafix", "lines",  "types", "symbols", "browse"};

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

int segAligns[Optimizer::MAX_SEGS] = {};
int dbgblocknum = 0;

static int virtualSegmentNumber;
static int lastIncludeNum;

static int sectionMap[Optimizer::MAX_SEGS];
static std::map<int, Label*> labelMap;

static std::vector<Optimizer::SimpleSymbol*> autotab;
static std::vector<Label*> strlabs;
static std::unordered_map<std::string, Label*> lblpubs;
static std::unordered_map<std::string, Label*> lbllabs;
static std::unordered_map<std::string, Label*> lblExterns;
static std::unordered_map<std::string, Label*> lblvirt;
static std::vector<Section*> virtuals;
static std::map<Section*, Optimizer::SimpleSymbol*> virtualSyms;

static Section* sections[Optimizer::MAX_SEGS];

static int autoCount;

struct symname
{
    bool operator()(const Optimizer::SimpleSymbol* left, const Optimizer::SimpleSymbol* right) const
    {
        return strcmp(left->outputName, right->outputName) < 0;
    }
};
static std::set<Optimizer::SimpleSymbol*, symname> globals;
static std::set<Optimizer::SimpleSymbol*, symname> locals;
static std::set<Optimizer::SimpleSymbol*, symname> externs;
static std::map<Optimizer::SimpleSymbol*, int> autos;
static std::vector<ObjSymbol*> autovector;
static std::map<Optimizer::SimpleSymbol*, ObjSymbol*> objExterns;
static std::map<Optimizer::SimpleSymbol*, ObjSymbol*> objGlobals;

static std::vector<Optimizer::SimpleSymbol*> impfuncs;
static std::vector<Optimizer::SimpleSymbol*> expfuncs;
static std::vector<std::string> includelibs;

static std::unordered_map<std::string, ObjSection*> objSectionsByName;
static std::map<int, ObjSection*> objSectionsByNumber;

static std::map<int, ObjSourceFile*> sourceFiles;

static int sectofs;

static Section dummySection("dummy", -1);

void omfInit(void)
{
    memset(sections, 0, sizeof(sections));
    currentSection = nullptr;
    globals.clear();
    locals.clear();
    externs.clear();
    autos.clear();
    autotab.clear();
    autovector.clear();
    objExterns.clear();
    objGlobals.clear();
    labelMap.clear();
    lblExterns.clear();
    impfuncs.clear();
    expfuncs.clear();
    includelibs.clear();
    strlabs.clear();
    lblpubs.clear();
    lbllabs.clear();
    lblExterns.clear();
    lblvirt.clear();
    objSectionsByNumber.clear();
    objSectionsByName.clear();
    sourceFiles.clear();
    virtuals.clear();
    virtualSyms.clear();
}
void dbginit(void)
{
    dbgblocknum = 0;
    autoCount = 0;
}

void outcode_file_init(void)
{
    int i;
    Parser::instructionParser = InstructionParser::GetInstance();
    lastIncludeNum = 0;
    virtualSegmentNumber = Optimizer::MAX_SEGS;
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

void omf_globaldef(Optimizer::SimpleSymbol* sym) { globals.insert(sym); }
void omf_localdef(Optimizer::SimpleSymbol* sym) { locals.insert(sym); }
void omf_put_extern(Optimizer::SimpleSymbol* sym, int code)
{
    externs.insert(sym);
    std::string name = sym->outputName;
    Label* l = new Label(name, lblExterns.size(), 0);
    l->SetExtern(true);
    lblExterns[l->GetName()] = l;
}
void omf_put_impfunc(Optimizer::SimpleSymbol* sym, const char* file) { impfuncs.push_back(sym); }
void omf_put_expfunc(Optimizer::SimpleSymbol* sym) { expfuncs.push_back(sym); }
void omf_put_includelib(const char* name) { includelibs.push_back(name); }

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
    if (n < Optimizer::MAX_SEGS)
        return sectionMap[n];
    return n - sectofs;
}
static void DumpFileList(ObjFactory& f, ObjFile* fi)
{
    for (auto bf : Optimizer::browseFiles)
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
    if (n < Optimizer::MAX_SEGS)
        n = sectionMap[n];
    else
        n -= sectofs;

    ObjSection* objSection = objSectionsByNumber[n];
    ATTRIBDATA* d = (ATTRIBDATA*)ins->GetAltData();
    if (d)
    {
        std::unique_ptr<ObjMemory::DebugTagContainer> dc = std::make_unique<ObjMemory::DebugTagContainer>();
        switch (d->type)
        {
            case e_ad_blockdata: {
                ObjDebugTag* tag = factory.MakeDebugTag(d->start);
                dc->push_back(tag);
                objSection->GetMemoryManager().Add(std::move(dc));
            }
            break;
            case e_ad_funcdata: {
                ObjDebugTag* tag = factory.MakeDebugTag(objGlobals[d->v.sp], d->start);
                dc->push_back(tag);
                objSection->GetMemoryManager().Add(std::move(dc));
            }
            break;
            case e_ad_vfuncdata: {
                Section* s = (Section*)d->v.section;
                n = sect->GetSect();
                if (n < Optimizer::MAX_SEGS)
                    n = sectionMap[n];
                else
                    n -= sectofs;
                ObjDebugTag* tag = factory.MakeDebugTag(objSectionsByNumber[n], d->start);
                dc->push_back(tag);
                objSection->GetMemoryManager().Add(std::move(dc));
            }
            break;
            case e_ad_linedata: {
                ObjLineNo* line = factory.MakeLineNo(sourceFiles[d->v.ld->fileindex], d->v.ld->lineno);
                ObjDebugTag* tag = factory.MakeDebugTag(line);
                dc->push_back(tag);
                objSection->GetMemoryManager().Add(std::move(dc));
            }
            break;
            case e_ad_vardata:
                if (autos.find(d->v.sp) != autos.end())
                {
                    ObjSymbol* autosp = autovector[autos[d->v.sp]];
                    ObjDebugTag* tag = factory.MakeDebugTag(autosp);
                    dc->push_back(tag);
                    objSection->GetMemoryManager().Add(std::move(dc));
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
        char buf[4096];
        dbgtypes types(factory, fi);
        if (Optimizer::cparams.prm_debug)
            DumpFileList(factory, fi);
        for (int i = 0; i < Optimizer::MAX_SEGS; ++i)
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
        sectofs = Optimizer::MAX_SEGS - sectofs;
        for (auto v : virtuals)
        {
            v->SetAlign(v->GetName()[2] == 'c' ? segAligns[Optimizer::codeseg] : segAligns[Optimizer::dataseg]);

            ObjSection* s = v->CreateObject(factory);
            if (s)
            {
                if (Optimizer::cparams.prm_debug)
                {
                    // destructor symbols aren't there so don't put the type...
                    auto tp = virtualSyms[v]->tp;
                    if (tp->type != Optimizer::st_func || tp->sp)
                        s->SetVirtualType(types.Put(tp));
                }
                bool cseg = s->GetName()[2] == 'c';
                s->SetQuals((cseg ? segFlags[Optimizer::codeseg] : segFlags[Optimizer::dataseg]) + virtualSegFlags);
                objSectionsByName[s->GetName()] = s;
                objSectionsByNumber[s->GetIndex()] = s;
                fi->Add(s);
            }
        }

        if (objSectionsByNumber.size())
        {
            Optimizer::SimpleSymbol s = {};
            for (auto l : strlabs)
            {
                std::string name = l->GetName();
                s.outputName = (char*)name.c_str();
                bool local = false;
                auto g = globals.find(&s);
                if (g != globals.end())
                {
                    ObjSymbol* s1;
                    strcpy(buf, (*g)->outputName);
                    if ((*g)->storage_class == Optimizer::scc_localstatic || (*g)->storage_class == Optimizer::scc_static)
                        s1 = factory.MakeLocalSymbol(buf);
                    else
                        s1 = factory.MakePublicSymbol(buf);
                    ObjExpression* left = factory.MakeExpression(objSectionsByNumber[GETSECT(l, sectofs)]);
                    ObjExpression* right = factory.MakeExpression(l->GetOffset()->ival);
                    ObjExpression* sum = factory.MakeExpression(ObjExpression::eAdd, left, right);
                    s1->SetOffset(sum);
                    if (Optimizer::cparams.prm_debug)
                        s1->SetBaseType(types.Put((*g)->tp));
                    fi->Add(s1);
                    l->SetObjSymbol(s1);
                    l->SetObjectSection(objSectionsByNumber[GETSECT(l, sectofs)]);
                    objGlobals[*g] = s1;
                }
                else
                {
                    auto l1 = locals.find(&s);
                    if (l1 != locals.end())
                    {
                        ObjSymbol* s1;
                        strcpy(buf, (*l1)->outputName);
                        s1 = factory.MakeLocalSymbol(buf);
                        ObjExpression* left = factory.MakeExpression(objSectionsByNumber[GETSECT(l, sectofs)]);
                        ObjExpression* right = factory.MakeExpression(l->GetOffset()->ival);
                        ObjExpression* sum = factory.MakeExpression(ObjExpression::eAdd, left, right);
                        s1->SetOffset(sum);
                        if (Optimizer::cparams.prm_debug)
                            s1->SetBaseType(types.Put((*l1)->tp));
                        fi->Add(s1);
                        l->SetObjSymbol(s1);
                        l->SetObjectSection(objSectionsByNumber[GETSECT(l, sectofs)]);
                        objGlobals[*l1] = s1;                    
                    }
                }
            }
            for (auto e : externs)
            {
                strcpy(buf, e->outputName);
                ObjSymbol* s1 = factory.MakeExternalSymbol(buf);
                fi->Add(s1);
                objExterns[e] = s1;
                auto it = lblExterns.find(e->outputName);
                if (it != lblExterns.end())
                    it->second->SetObjSymbol(s1);
            }
            for (auto e : autotab)
            {
                strcpy(buf, e->outputName);
                ObjSymbol* s1 = factory.MakeAutoSymbol(buf);
                if (Optimizer::cparams.prm_debug)
                    s1->SetBaseType(types.Put(e->tp));
                int resolved = 0;
                Optimizer::SimpleExpression* exp = Allocate<Optimizer::SimpleExpression>();
                exp->type = Optimizer::se_auto;
                exp->sp = e;
                s1->SetOffset(new ObjExpression(resolveoffset(exp, &resolved)));
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
            strcpy(buf, exp->outputName);
            ObjExportSymbol* p = factory.MakeExportSymbol(buf);
            p->SetExternalName(exp->outputName);
            fi->Add(p);
        }
        for (auto import : impfuncs)
        {
            strcpy(buf, import->outputName);
            ObjImportSymbol* p = factory.MakeImportSymbol(buf);
            p->SetExternalName(import->outputName);
            if (import->importfile)
                p->SetDllName(import->importfile);
            fi->Add(p);
        }
        for (int i = 0; i < Optimizer::MAX_SEGS; ++i)
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
        if (Optimizer::cparams.prm_debug)
        {
            for (auto v : Optimizer::typedefs)
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
            ObjIeee i(Parser::outFile);

            i.SetTranslatorName(ObjString("occ"));
            i.SetDebugInfoFlag(Optimizer::cparams.prm_debug && outputFile == Optimizer::outputFile);

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
    for (auto bi : Optimizer::browseInfo)
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
    DumpFile(f, fi, Optimizer::outputFile);

    if (Optimizer::cparams.prm_browse)
    {
        ObjIeeeIndexManager im1;
        ObjFactory f1(&im1);
        name = infile;
        fi = MakeBrowseFile(f1, name);
        DumpFile(f1, fi, Optimizer::browseFile);
    }
    Release();
}

void compile_start(char* name)
{
    Optimizer::LIST* newItem = beGlobalAllocate<Optimizer::LIST>();
    newItem->data = beGlobalAllocate<char>(strlen(name) + 1);
    strcpy((char*)newItem->data, name);

    oa_ini();
    o_peepini();
    omfInit();
}
/*-------------------------------------------------------------------------*/

void outcode_enterseg(int seg)
{
    if (!sections[seg])
    {
        sections[seg] = new Section(segnames[seg], seg);
        Parser::instructionParser->Setup(sections[seg]);
    }
    currentSection = sections[seg];
    if (oa_currentSeg == Optimizer::codeseg)
        AsmExpr::SetSection(&dummySection);
    else
        AsmExpr::SetSection(currentSection);
}

void InsertInstruction(Instruction* ins)
{
    if (oa_currentSeg == Optimizer::codeseg)
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
void outcode_gen_strlab(Optimizer::SimpleSymbol* sym)
{
    std::string name = sym->outputName;
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

Fixup* gen_symbol_fixup(Optimizer::SimpleSymbol* pub, int offset, bool PC)
{
    AsmExprNode* expr = new AsmExprNode(pub->outputName);
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
Fixup* gen_threadlocal_fixup(Optimizer::SimpleSymbol* tls, Optimizer::SimpleSymbol* base, int offset)
{
    AsmExprNode* expr = new AsmExprNode(base->outputName);
    AsmExprNode* expr1 = new AsmExprNode(tls->outputName);
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
    Optimizer::UBYTE buf[10];
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
            v->floatvalue.ToFloat(buf);
            emit(buf, 4);
        }
        else
        {
            v->floatvalue.ToDouble(buf);
            emit(buf, 8);
        }
        v = v->next;
    }
}

/*-------------------------------------------------------------------------*/

void outcode_genref(Optimizer::SimpleSymbol* sym, int offset)
{
    Fixup* f = gen_symbol_fixup(sym, offset, false);
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

void outcode_gensrref(Optimizer::SimpleSymbol* sym, int val)
{
    Fixup* f = gen_symbol_fixup(sym, 0, false);
    char buf[8] = {};
    buf[1] = val;

    emit(&buf, 6, f, 2);
}

/*-------------------------------------------------------------------------*/

void outcode_genstorage(int len) { emit(len); }

/*-------------------------------------------------------------------------*/

void outcode_genfloat(FPF* val)
{
    Optimizer::UBYTE buf[4];
    val->ToFloat(buf);
    emit(buf, 4);
}

/*-------------------------------------------------------------------------*/

void outcode_gendouble(FPF* val)
{
    Optimizer::UBYTE buf[8];
    val->ToDouble(buf);
    emit(buf, 8);
}

/*-------------------------------------------------------------------------*/

void outcode_genlongdouble(FPF* val)
{
    Optimizer::UBYTE buf[10];
    val->ToLongDouble(buf);
    emit(buf, 10);
}

/*-------------------------------------------------------------------------*/

void outcode_genstring(char* string, int len)
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

void outcode_genlonglong(long long val) { emit(&val, 8); }

/*-------------------------------------------------------------------------*/

void outcode_align(int size) { emit(-size); }

/*-------------------------------------------------------------------------*/

void outcode_put_label(int lab) { InsertLabel(lab); }

/*-------------------------------------------------------------------------*/

void outcode_start_virtual_seg(Optimizer::SimpleSymbol* sym, int data)
{
    char buf[4096];
    if (data)
        strcpy(buf, "vsd@");
    else
        strcpy(buf, "vsc@");
    strcpy(buf + 3 + (sym->outputName[0] != '@'), sym->outputName);
    Section* virtsect = new Section(buf, virtualSegmentNumber++);
    virtualSyms[virtsect] = sym;
    virtuals.push_back(virtsect);
    currentSection = virtsect;
    Parser::instructionParser->Setup(virtsect);
    AsmExpr::SetSection(virtsect);
    std::string name = sym->outputName;
    Label* l = new Label(name, lblvirt.size(), virtualSegmentNumber - 1);
    l->SetOffset(0);
    lblvirt[name] = l;
}

/*-------------------------------------------------------------------------*/

void outcode_end_virtual_seg(Optimizer::SimpleSymbol* sym) { outcode_enterseg(oa_currentSeg); }

/*-------------------------------------------------------------------------*/

void InsertAttrib(ATTRIBDATA* ad) { InsertInstruction(new Instruction(ad)); }
void InsertLine(Optimizer::LINEDATA* linedata)
{
    ATTRIBDATA* attrib = Allocate<ATTRIBDATA>();
    attrib->type = e_ad_linedata;
    attrib->v.ld = linedata;
    InsertAttrib(attrib);
}
void InsertVarStart(Optimizer::SimpleSymbol* sym)
{
    if (!strstr(sym->name, "++"))
    {
        ATTRIBDATA* attrib = Allocate<ATTRIBDATA>();
        attrib->type = e_ad_vardata;
        attrib->v.sp = sym;

        InsertAttrib(attrib);
        // this is done this way because VC++ has bug in optimized code...
        int n = autos.size();
        autos[sym] = n;

        autotab.push_back(sym);
    }
}
void InsertFunc(Optimizer::SimpleSymbol* sym, int start)
{
    if (oa_currentSeg == Optimizer::virtseg)
    {
        ATTRIBDATA* attrib = Allocate<ATTRIBDATA>();
        attrib->type = e_ad_vfuncdata;
        attrib->v.section = currentSection;
        attrib->start = !!start;
        InsertAttrib(attrib);
    }
    else
    {
        ATTRIBDATA* attrib = Allocate<ATTRIBDATA>();
        attrib->type = e_ad_funcdata;
        attrib->v.sp = sym;
        attrib->start = !!start;
        InsertAttrib(attrib);
    }
}
void InsertBlock(int start)
{
    ATTRIBDATA* attrib = Allocate<ATTRIBDATA>();
    attrib->type = e_ad_blockdata;
    attrib->start = !!start;
    InsertAttrib(attrib);
}

void AddFixup(Instruction* newIns, OCODE* ins, const std::list<Numeric*>& operands)
{
    if ((e_op)ins->opcode == op_dd)
    {
        int resolved = 1;
        int n = resolveoffset(ins->oper1->offset, &resolved);
        if (!resolved)
        {
            memcpy(newIns->GetBytes(), &n, 4);
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
                (((AsmExprNode*)operand->node)->GetType() == AsmExprNode::LABEL ||
                 ((AsmExprNode*)operand->node)->GetType() == AsmExprNode::SUB ||
                 ((AsmExprNode*)operand->node)->GetType() == AsmExprNode::ADD))
            {
                if (newIns->Lost() && operand->pos)
                    operand->pos -= 8;
                int n = operand->relOfs;
                if (n < 0)
                    n = -n;
                Fixup* f =
                    new Fixup((AsmExprNode*)operand->node, (operand->size + 7) / 8, operand->relOfs != 0, n, operand->relOfs > 0);
                f->SetInsOffs((operand->pos + 7) / 8);
                newIns->Add(f);
            }
        }
    }
}

void outcode_diag(OCODE* ins, const char* str)
{
    std::string instruction = Parser::instructionParser->FormatInstruction(ins);
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

        asmError err = Parser::instructionParser->GetInstruction(ins, newIns, operands);

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
        switch ((e_op)ins->opcode)
        {
            case op_label:
                InsertLabel(ins->oper1->offset->i);
                return;
            case op_line:
                if (Optimizer::cparams.prm_debug)
                {
                    auto ld = (Optimizer::LINEDATA*)ins->oper1;
                    if (ld)
                        InsertLine(ld);
                }
                break;
            case op_varstart:
                if (Optimizer::cparams.prm_debug)
                    InsertVarStart((Optimizer::SimpleSymbol*)ins->oper1);
                break;
            case op_blockstart:
                if (Optimizer::cparams.prm_debug)
                    InsertBlock(1);
                break;
            case op_blockend:
                if (Optimizer::cparams.prm_debug)
                    InsertBlock(0);
                break;
            case op_funcstart:
                if (Optimizer::cparams.prm_debug)
                    InsertFunc((Optimizer::SimpleSymbol*)ins->oper1, 1);
                break;
            case op_funcend:
                if (Optimizer::cparams.prm_debug)
                    InsertFunc((Optimizer::SimpleSymbol*)ins->oper1, 0);
                break;
            case op_genword:
                outcode_genbyte(ins->oper1->offset->i);
                break;
            case op_align: {
                Instruction* newIns = new Instruction(ins->oper1->offset->i);
                InsertInstruction(newIns);
                break;
            }
            case op_dd: {
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
    if (oa_currentSeg == Optimizer::codeseg)
    {
        // all this dancing around so that when compiling things into the main code segment, we don't spend an inordinate amound of
        // time in Resolve()...
        //
        dummySection.Resolve();
        for (auto& d : dummySection.GetInstructions())
        {
            currentSection->InsertInstruction(d.release());
        }
        dummySection.ClearInstructions();
    }
    else
    {
        currentSection->Resolve();
    }
}
}  // namespace occx86