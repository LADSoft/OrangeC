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

#include "ildata.h"
#include <set>
#include "beinterfdefs.h"
#include "config.h"
#include "iblock.h"
#include "OptUtils.h"
#include "optmain.h"
#include "memory.h"
#include "ifloatconv.h"
#include "ilocal.h"
#include "output.h"
#ifdef BORLAND
#    include "malloc.h"
#endif
namespace Parser
{
bool IsCompiler();
}

namespace Optimizer
{
bool pinning;
bool msilstrings;
bool delegateforfuncptr;
bool initializeScalars;
int gentype; /* Current DC type */
int curseg;  /* Current seg */
int outcol;  /* Curront col (roughly) */
FILE* icdFile;
std::vector<Optimizer::SimpleSymbol*> externals;
std::vector<Optimizer::SimpleSymbol*> globalCache;
std::vector<Optimizer::SimpleSymbol*> typeSymbols;
std::vector<Optimizer::SimpleSymbol*> typedefs;
std::vector<Optimizer::SimpleSymbol*> temporarySymbols;
std::vector<Optimizer::SimpleSymbol*> functionVariables;
std::vector<BROWSEINFO*> browseInfo;
std::vector<BROWSEFILE*> browseFiles;
std::set<Optimizer::SimpleSymbol*> externalSet;
std::set<Optimizer::SimpleSymbol*> typeSet;
std::set<Optimizer::SimpleSymbol*> definedFunctions;

std::list<std::string> inputFiles;
std::list<std::string> backendFiles;
std::string prm_libPath;
std::string prm_include;
std::list<std::string> libIncludes;
std::list<std::string> toolArgs;
const char* pinvoke_dll = "msvcrt.dll";
std::string prm_snkKeyFile;
std::list<std::string> prm_Using;
std::string prm_assemblyVersion = "0.0.0.0";
std::string prm_namespace_and_class;
std::map<std::string, std::string> bePragma;
std::list<MsilProperty> msilProperties;
std::string prm_OutputDefFile;
std::string prm_OutputImportLibraryFile;

std::string prm_assemblerSpecifier;
std::string outputFileName;
std::string assemblerFileExtension;

Optimizer::SimpleExpression* fltexp;

int exitBlock;
COMPILER_PARAMS cparams;
int showBanner = true;
int nextLabel;
bool assembling;
int fastcallAlias;
bool setjmp_used;
bool functionHasAssembly;
std::string compilerName;
std::string intermediateName;
std::string backendName;

int dataAlign;
int bssAlign;
int constAlign;
int architecture;

int registersAssigned;

std::deque<BaseData*> baseData;
static bool virtualMode;

void InitIntermediate()
{
    curseg = noseg;
    dataAlign = bssAlign = constAlign = 1;
    if (architecture != ARCHITECTURE_MSIL || (cparams.prm_compileonly && !cparams.prm_asmfile))
    {
        for (auto b : baseData)
            if (b->type == DT_FUNC)
                delete b->funcData;
        baseData.clear();
        externals.clear();
        globalCache.clear();
        browseInfo.clear();
        browseFiles.clear();
        //    inputFiles.clear();
        typedefs.clear();
        bePragma.clear();
        msilProperties.clear();
        typeSymbols.clear();
        typeSet	.clear();
        definedFunctions.clear();
    }
}

static BaseData* AddData(DataType dt)
{
    if (Parser::IsCompiler())
    {
        BaseData* rv = Allocate<BaseData>();
        rv->type = dt;
        baseData.push_back(rv);
        return rv;
    }
    else
    {
        static BaseData* rv = (BaseData*)malloc(sizeof(BaseData));
        return rv;
    }
}

void AddFunction()
{
    auto val = AddData(DT_FUNC);
    if (Parser::IsCompiler())
    {
        FunctionData* data = new FunctionData;
        data->name = currentFunction;
        data->fltexp = fltexp;
        data->temporarySymbols = temporarySymbols;
        data->variables = functionVariables;
        data->loadHash = loadHash;
        data->instructionList = intermed_head;
        data->setjmp_used = setjmp_used;
        data->hasAssembly = functionHasAssembly;
        data->exitBlock = exitBlock;
        data->tempCount = tempCount;
        data->blockCount = blockCount;
        data->fastcallAlias = fastcallAlias;
        val->funcData = data;
    }
}

void gen_vtt(int dataOffset, Optimizer::SimpleSymbol* func, Optimizer::SimpleSymbol* name)
{
    auto val = AddData(DT_VTT);
    val->symbol.i = dataOffset;
    val->symbol.sym = func;
    name->generated = true;
    globalCache.push_back(val->symbol.sym);
}
void gen_importThunk(Optimizer::SimpleSymbol* func)
{
    auto val = AddData(DT_IMPORTTHUNK);
    val->symbol.sym = func;
    globalCache.push_back(val->symbol.sym);

}
void gen_vc1(Optimizer::SimpleSymbol* func)
{
    auto val = AddData(DT_VC1);
    val->symbol.sym = func;
    globalCache.push_back(val->symbol.sym);
    func->generated = true;
    Optimizer::EnterExternal(func);
}
/*-------------------------------------------------------------------------*/

void gen_strlab(Optimizer::SimpleSymbol* sym)
/*
 *      generate a named label.
 */
{
    auto val = AddData(DT_DEFINITION);
    val->symbol.sym = sym;
    sym->generated = true;

    if (sym->storage_class == scc_global || (sym->storage_class == scc_constant && !sym->inFunc))
        val->symbol.i |= BaseData::DF_GLOBAL;
    else if (sym->storage_class == scc_static)
        val->symbol.i |= BaseData::DF_STATIC;
    else if (sym->storage_class == scc_localstatic)
        val->symbol.i |= BaseData::DF_LOCALSTATIC;
    if (sym->isexport)
        val->symbol.i |= BaseData::DF_EXPORT;
    globalCache.push_back(val->symbol.sym);
}

/*-------------------------------------------------------------------------*/

void put_label(int lab)
/*
 *      icdFile a compiler generated label.
 */
{
    auto val = AddData(DT_LABELDEFINITION);
    val->i = lab;
}
void put_string_label(int lab, int type)
/*
 *      icdFile a compiler generated label.
 */
{
    put_label(lab);
}

/*-------------------------------------------------------------------------*/

void put_staticlabel(long label) { put_label(label); }

/*-------------------------------------------------------------------------*/

void genfloat(FPF* val)
/*
 * Output a float value
 */
{
    CastToFloat(ISZ_FLOAT, val);
    auto f = AddData(DT_FLOAT);
    f->f = *val;
}

/*-------------------------------------------------------------------------*/

void gendouble(FPF* val)
/*
 * Output a double value
 */
{
    CastToFloat(ISZ_DOUBLE, val);
    auto f = AddData(DT_DOUBLE);
    f->f = *val;
}

/*-------------------------------------------------------------------------*/

void genlongdouble(FPF* val)
/*
 * Output a double value
 */
{
    CastToFloat(ISZ_LDOUBLE, val);
    auto f = AddData(DT_LDOUBLE);
    f->f = *val;
}

/*-------------------------------------------------------------------------*/

void genbyte(long val)
/*
 * Output a byte value
 */
{
    auto i = AddData(DT_BYTE);
    i->i = val;
}
void genbool(int val)
{
    auto i = AddData(DT_BOOL);
    i->i = val;
}
/* val not really used and will always be zero*/
void genbit(Optimizer::SimpleSymbol* sym, int val)
{
    auto i = AddData(DT_BIT);
    i->i = val;
}
/*-------------------------------------------------------------------------*/

void genshort(long val)
/*
 * Output a word value
 */
{
    auto i = AddData(DT_USHORT);
    i->i = val;
}

void genwchar_t(long val)
/*
 * Output a word value
 */
{
    genshort(val);
}
/*-------------------------------------------------------------------------*/

void genlong(long val)
/*
 * Output a long value
 */
{
    auto i = AddData(DT_ULONG);
    i->i = val;
}

/*-------------------------------------------------------------------------*/

void genlonglong(long long val)
/*
 * Output a long value
 */
{
    auto i = AddData(DT_ULONGLONG);
    i->i = val;
}
void genint(int val)
{
    auto i = AddData(DT_UINT);
    i->i = val;
}
void genuint16(int val)
{
    auto i = AddData(DT_16);
    i->i = val;
}
void genuint32(int val)
{
    auto i = AddData(DT_32);
    i->i = val;
}
void genenum(int val)
{
    auto i = AddData(DT_ENUM);
    i->i = val;
}
/*-------------------------------------------------------------------------*/

void genaddress(unsigned long long address)
/*
 * Output a long value
 */
{
    auto i = AddData(DT_ADDRESS);
    i->i = address;
}

/*-------------------------------------------------------------------------*/

void gensrref(Optimizer::SimpleSymbol* sym, int val, int type)
/*
 * Output a startup/rundown reference
 */
{
    auto v = AddData(DT_SRREF);
    v->symbol.sym = sym;
    v->symbol.i = val;
    globalCache.push_back(v->symbol.sym);
    Optimizer::EnterExternal(sym);
}

/*-------------------------------------------------------------------------*/
void genref(Optimizer::SimpleSymbol* sym, int offset)
/*
 * Output a reference to the data area (also gens fixups )
 */
{
    auto v = AddData(DT_SYM);
    v->symbol.sym = sym;
    v->symbol.i = offset;
    globalCache.push_back(v->symbol.sym);
    Optimizer::EnterExternal(sym);
}

/*-------------------------------------------------------------------------*/

void genpcref(Optimizer::SimpleSymbol* sym, int offset)
/*
 * Output a reference to the code area (also gens fixups )
 */
{
    auto v = AddData(DT_PCREF);
    v->symbol.sym = sym;
    globalCache.push_back(v->symbol.sym);
    Optimizer::EnterExternal(sym);
}

/*-------------------------------------------------------------------------*/

void genstorage(int nbytes)
/*
 * Output bytes of storage
 */
{
    auto v = AddData(DT_RESERVE);
    v->i = nbytes;
}

void gen_autoref(Optimizer::SimpleSymbol* sym, int offset)
{
    if (sym == nullptr)  // this is likely a bug but was preexisting...
    {
        genint(offset);
    }
    else
    {
        auto v = AddData(DT_AUTOREF);
        v->symbol.sym = sym;
        v->symbol.i = offset;
    }
}

/*-------------------------------------------------------------------------*/

void gen_labref(int n)
/*
 * Generate a reference to a label
 */
{
    if (n < 0)
        diag("gen_labref: uncompensatedlabel");
    auto v = AddData(DT_LABEL);
    v->i = n;
}

void gen_labdifref(int n1, int n2)
{
    auto v = AddData(DT_LABDIFFREF);
    v->diff.l1 = n1;
    v->diff.l2 = n2;
}

int wchart_cmp(LCHAR* left, LCHAR* right, int len)
{
    while (len--)
    {
        if (*left != *right)
        {
            if (*left < *right)
                return -1;
            else
                return 1;
        }
        left++, right++;
    }
    return 0;
}

static void exitseg(void)
{
    if (curseg != noseg)
    {
        auto v = AddData(DT_SEGEXIT);
        v->i = curseg;
        curseg = noseg;
    }
}
static void enterseg(enum e_sg seg)
{
    if (curseg != seg)
    {
        exitseg();
        auto v = AddData(DT_SEG);
        v->i = seg;
        curseg = seg;
    }
}
/*
 * Switch to cseg
 */
void cseg(void) { enterseg(codeseg); }

/*
 * Switch to dseg
 */
void dseg(void) { enterseg(dataseg); }

void tseg(void) { enterseg(tlsseg); }
/*
 * Switch to bssseg
 */
void bssseg(void) { enterseg(bssxseg); }
/*
 * Switch to const seg
 */
void xconstseg(void) { enterseg(constseg); }
/*
 * Switch to string seg
 */
void xstringseg(void) { enterseg(stringseg); }

/*
 * Switch to startupseg
 */
void startupseg(void) { enterseg(startupxseg); }

/*
 * Switch to rundownseg
 */
void rundownseg(void) { enterseg(rundownxseg); }
void tlsstartupseg(void) { enterseg(tlssuseg); }
void tlsrundownseg(void) { enterseg(tlsrdseg); }
void gen_virtual(Optimizer::SimpleSymbol* sym, int data)
{
    exitseg();
    auto v = AddData(DT_VIRTUAL);
    v->symbol.i = data;
    v->symbol.sym = sym;
    virtualMode = data;
    //    globalCache.push_back(v->symbol.sym);
}

/*-------------------------------------------------------------------------*/

void gen_endvirtual(Optimizer::SimpleSymbol* sym)
{
    auto v = AddData(DT_ENDVIRTUAL);

    v->symbol.sym = sym;
    if (virtualMode)
        dseg();
    else
        cseg();
    globalCache.push_back(v->symbol.sym);
}
void align(int size)
{
    auto v = AddData(DT_ALIGN);
    v->i = size;
}
void gen_funcref(Optimizer::SimpleSymbol* sym)
{
    auto v = AddData(DT_FUNCREF);
    v->symbol.sym = sym;
    if (sym->storage_class == scc_global ||
        ((sym->storage_class == scc_member || sym->storage_class == scc_virtual) && sym->hasInlineFunc))
        v->symbol.i |= BaseData::DF_GLOBAL;
    else
        v->symbol.i |= BaseData::DF_STATIC;
    if (sym->isexport)
        v->symbol.i |= BaseData::DF_EXPORT;
}

void putstring(char* string, int len)
{
    auto v = AddData(DT_STRING);
    v->astring.str = (char*)Alloc(len + 1);
    v->astring.i = len;
    memcpy(v->astring.str, string, len);
}
void nl(void)
/*
 * New line
 */
{
    {
        // if (outcol > 0)
        {
            oputc('\n', icdFile);
            outcol = 0;
            gentype = nogen;
        }
    }
}
void EnterExternal(Optimizer::SimpleSymbol* sym)
{
    if (Optimizer::externalSet.find(sym) == Optimizer::externalSet.end())
    {
        Optimizer::externals.push_back(sym);
        Optimizer::externalSet.insert(sym);
    }
}
void EnterType(Optimizer::SimpleSymbol* sym)
{
    if (Optimizer::typeSet.find(sym) == Optimizer::typeSet.end())
    {
        Optimizer::typeSymbols.push_back(sym);
        Optimizer::typeSet.insert(sym);
    }
}
}  // namespace Optimizer