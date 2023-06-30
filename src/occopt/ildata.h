#pragma once
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

#include "ioptimizer.h"
#include "beinterfdefs.h"
#include "../occ/winmode.h"
#include <deque>
#include <map>
#include <set>

namespace Optimizer
{
enum FunctionFlags
{
    FF_USES_SETJMP = 1,
    FF_HAS_ASSEMBLY = 2
};
enum TempFlags
{
    TF_LOADTEMP = 1,
    TF_PUSHEDTOTEMP = 2,
};
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
    DT_VC1,
    DT_AUTOREF,
};
struct FunctionData
{
    Optimizer::SimpleSymbol* name;
    Optimizer::SimpleExpression* fltexp;
    std::vector<Optimizer::SimpleSymbol*> temporarySymbols;
    std::vector<Optimizer::SimpleSymbol*> variables;
    std::vector<IMODE*> imodeList;
    std::map<IMODE*, IMODE*> loadHash;
    TEMP_INFO** tempInfo;
    int fastcallAlias;
    int tempCount;
    int blockCount;
    int exitBlock;
    QUAD* instructionList;
    int setjmp_used : 1;
    int hasAssembly : 1;
};
struct BaseData
{
    BaseData() {}  // don't care about init, we just want the memory functions to compile
    enum
    {
        DF_GLOBAL = 1,
        DF_STATIC = 2,
        DF_LOCALSTATIC = 4,
        DF_EXPORT = 8
    };
    DataType type;
    union
    {
        struct
        {
            Optimizer::SimpleSymbol* sym;
            int i;
        } symbol;
        FunctionData* funcData;
        struct
        {
            int l1, l2;
        } diff;
        e_sg segtype;
        long long i;
        FPF f;
        struct
        {
            char* str;
            int i;
        } astring;
        struct
        {
            FPF r;
            FPF i;
        } c;
    };
};

static const char* magic = "LSIL";
static const int fileVersion = 1;

enum e_sbt
{
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

enum e_stt
{
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
extern bool pinning;
extern bool msilstrings;
extern bool delegateforfuncptr;
extern bool initializeScalars;
extern int gentype; /* Current DC type */
extern int curseg;  /* Current seg */
extern int outcol;  /* Curront col (roughly) */
extern FILE* icdFile;
extern std::vector<Optimizer::SimpleSymbol*> externals;
extern std::vector<Optimizer::SimpleSymbol*> globalCache;
extern std::vector<Optimizer::SimpleSymbol*> typeSymbols;
extern std::vector<Optimizer::SimpleSymbol*> typedefs;
extern std::vector<Optimizer::SimpleSymbol*> temporarySymbols;
extern std::vector<Optimizer::SimpleSymbol*> functionVariables;
extern std::vector<BROWSEINFO*> browseInfo;
extern std::vector<BROWSEFILE*> browseFiles;
extern std::set<Optimizer::SimpleSymbol*> externalSet;
extern std::set<Optimizer::SimpleSymbol*> definedFunctions;

extern std::list<std::string> inputFiles;
extern std::list<std::string> backendFiles;
extern std::string prm_libPath;
extern std::string prm_include;
extern std::list<std::string> libIncludes;
extern std::list<std::string> toolArgs;
extern const char* pinvoke_dll;
extern std::string prm_snkKeyFile;
extern std::list<std::string> prm_Using;
extern std::string prm_assemblyVersion;
extern std::string prm_namespace_and_class;
extern std::map<std::string, std::string> bePragma;
extern std::list<MsilProperty> msilProperties;
extern std::string prm_OutputDefFile;
extern std::string prm_OutputImportLibraryFile;

extern std::string prm_assemblerSpecifier;
extern std::string outputFileName;
extern std::string assemblerFileExtension;

extern Optimizer::SimpleExpression* fltexp;

extern int exitBlock;
extern COMPILER_PARAMS cparams;
extern int showBanner;
extern int nextLabel;
extern bool assembling;
extern int fastcallAlias;
extern bool setjmp_used;
extern bool functionHasAssembly;
extern std::string compilerName;
extern std::string intermediateName;
extern std::string backendName;

extern int dataAlign;
extern int bssAlign;
extern int constAlign;
extern int architecture;

extern int registersAssigned;

extern std::deque<BaseData*> baseData;

void InitIntermediate(void);
void AddFunction(void);
void gen_vtt(int dataOffset, Optimizer::SimpleSymbol* func, Optimizer::SimpleSymbol* name);
void gen_importThunk(Optimizer::SimpleSymbol* func);
void gen_vc1(Optimizer::SimpleSymbol* func);
void gen_strlab(Optimizer::SimpleSymbol* sym);
void put_label(int lab);
void put_string_label(int lab, int type);
void put_staticlabel(long label);
void genfloat(FPF* val);
void gendouble(FPF* val);
void genlongdouble(FPF* val);
void genbyte(long val);
void genbool(int val);
void genbit(Optimizer::SimpleSymbol* sym, int val);
void genshort(long val);
void genwchar_t(long val);
void genlong(long val);
void genlonglong(long long val);
void genint(int val);
void genuint16(int val);
void genuint32(int val);
void genenum(int val);
void genaddress(unsigned long long address);
void gensrref(Optimizer::SimpleSymbol* sym, int val, int type);
void genref(Optimizer::SimpleSymbol* sym, int offset);
void genpcref(Optimizer::SimpleSymbol* sym, int offset);
void genstorage(int nbytes);
void gen_autoref(Optimizer::SimpleSymbol* sym, int offset);
void gen_labref(int n);
void gen_labdifref(int n1, int n2);
int wchart_cmp(LCHAR* left, LCHAR* right, int len);
void cseg(void);
void dseg(void);
void tseg(void);
void bssseg(void);
void xconstseg(void);
void xstringseg(void);
void startupseg(void);
void rundownseg(void);
void tlsstartupseg(void);
void tlsrundownseg(void);
void gen_virtual(Optimizer::SimpleSymbol* sym, int data);
void gen_endvirtual(Optimizer::SimpleSymbol* sym);
void align(int size);
void gen_funcref(Optimizer::SimpleSymbol* sym);
void putstring(char* string, int len);
void nl(void);
void EnterExternal(Optimizer::SimpleSymbol* sym);
void EnterType(Optimizer::SimpleSymbol* sym);
}  // namespace Optimizer