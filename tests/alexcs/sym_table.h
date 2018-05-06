#ifndef SYN_TABLE
#define SYN_TABLE

#include <string.h>
#include "scanner.h"
#include "statement_base.h"
#include <map>
#include <set>
#include <vector>
#include <algorithm>

enum SymbolClass{
    SYM = 0,
    SYM_FUNCT = 1,
    SYM_PROC = 2,
    SYM_TYPE = 4,
    SYM_TYPE_SCALAR = 8,
    SYM_TYPE_INTEGER = 16,
    SYM_TYPE_REAL = 32,
    SYM_TYPE_UNTYPED = 32768,
    SYM_TYPE_ARRAY = 64,
    SYM_TYPE_RECORD = 16384,
    SYM_TYPE_ALIAS = 128,
    SYM_TYPE_POINTER = 256,
    SYM_VAR = 512,
    SYM_VAR_CONST = 1024,
    SYM_VAR_PARAM = 2048,
    SYM_VAR_GLOBAL = 4096,
    SYM_VAR_LOCAL = 8192
};

class SymTable;
class SymType;
class SymVarParam;
class SymVarLocal;

extern SymType* top_type_int;
extern SymType* top_type_real;
extern SymType* top_type_untyped;
extern SymType* top_type_str;

class Symbol{
protected:
    Token token;
public:
    Symbol(Token token_);
    Symbol(const Symbol& sym);
    const char* GetName() const;
    Token GetToken() const;
    virtual SymbolClass GetClassName() const;
    virtual void PrintVerbose(ostream& o, int offset) const;
    virtual void Print(ostream& o, int offset = 0) const;
};

class SymType: public Symbol{
protected:
    SymType();
public:
    SymType(Token token);
    virtual SymbolClass GetClassName() const;
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetActualType() const;
    virtual unsigned GetSize() const;
};

class SymProc: public Symbol{
protected:
    bool have_side_effect;
    bool known_side_effect;
    bool searching;
    bool dummy_proc;
    vector<SymVarParam*> params;
    SymTable* sym_table;
    NodeStatement* body;
    AsmStrImmediate label;
    AsmStrImmediate exit_label;
    virtual void PrintPrototype(ostream& o, int offset) const;
public:
    bool IsAffectToParam(int index);
    bool IsDependOnParam(int index);
    SymProc(Token token_, SymTable* syn_table_);
    SymProc(Token name);
    ~SymProc();
    void AddSymTable(SymTable* syn_table_);
    void AddParam(SymVarParam* param);
    int GetArgsCount() const;
    const SymVarParam* GetArg(int arg_num) const;
    SymTable* GetSymTable() const;
    NodeStatement* GetBody() const;
    void AddBody(NodeStatement* body_);
    void GenerateDeclaration(AsmCode& asm_code);
    AsmStrImmediate GetLabel() const;
    AsmStrImmediate GetExitLabel() const;
    void ObtainLabels(AsmCode& asm_code);
    bool IsHaveBody() const;
    bool ValidateParams(SymProc* src);
    bool IsHaveSideEffect();
    bool IsAffectToVar(SymVar* var);
    bool IsDependOnVar(SymVar* var);
    void GetAllAffectedVars(VarsContainer& res_cont);
    void GetAllDependences(VarsContainer& res_cont);
    void Optimize();
    virtual bool IsDummyProc();
    virtual SymbolClass GetClassName() const;
    virtual const SymType* GetResultType() const;
    virtual void PrintVerbose(ostream& o, int offset) const;
    virtual void Print(ostream& o, int offset = 0) const;
    virtual bool CanBeReplaced();
};

class SymFunct: public SymProc{
protected:
    const SymType* result_type;
public:
    SymFunct(Token token_, SymTable* syn_table, const SymType* result_type_);
    SymFunct(Token name);
    void AddResultType(const SymType* result_type_);
    virtual SymbolClass GetClassName() const;
    virtual const SymType* GetResultType() const;
    virtual void Print(ostream& o, int offset = 0) const;
    virtual bool IsDummyProc();
};

class SymVar: public Symbol{
protected:
    const SymType* type;
public:
    SymVar(Token token, const SymType* type_);
    virtual SymbolClass GetClassName() const;
    virtual void Print(ostream& o, int ofefset = 0) const;
    virtual void PrintVerbose(ostream& o, int offset) const;
    const SymType* GetVarType() const;
    void PrintAsNode(ostream& o, int offset = 0) const;
    virtual void GenerateLValue(AsmCode& asm_code) const;
    virtual void GenerateValue(AsmCode& asm_code) const;
};

//---SymType descendants---

class SymTypeScalar: public SymType{
public:
    SymTypeScalar(Token name);
    virtual SymbolClass GetClassName() const;
};

class SymTypeInteger: public SymTypeScalar{
public:
    SymTypeInteger(Token name);
    virtual SymbolClass GetClassName() const;
};

class SymTypeReal: public SymTypeScalar{
public:
    SymTypeReal(Token name);
    virtual SymbolClass GetClassName() const;
};

class SymTypeUntyped: public SymTypeScalar{
public:
    SymTypeUntyped();
    virtual SymbolClass GetClassName() const;
    virtual unsigned GetSize() const;
};

class SymTypeArray: public SymType{
private:
    SymType* elem_type;
    int low;
    int high;
public:
    SymTypeArray(SymType* elem_type_, int low_, int high_);
    int GetLow();
    int GetHigh();
    const SymType* GetElemType();
    virtual SymbolClass GetClassName() const;
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void Print(ostream& o, int offset = 0);
    virtual void PrintVerbose(ostream& o, int offset) const;
    virtual unsigned GetSize() const;
};

class SymTypeRecord: public SymType{
private:
    SymTable* sym_table;
public:
    SymTypeRecord(SymTable* sym_table_);
    const SymVarLocal* FindField(Token& field_name);
    virtual SymbolClass GetClassName() const;
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void PrintVerbose(ostream& o, int offset) const;
    virtual unsigned GetSize() const;
};

class SymTypeAlias: public SymType{
private:
    SymType* target;
public:
    SymTypeAlias(Token name, SymType* ratget_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void PrintVerbose(ostream& o, int offset) const;
    virtual SymbolClass GetClassName() const;
    virtual const SymType* GetActualType() const;
    virtual unsigned GetSize() const;
};

class SymTypePointer: public SymType{
private:
    SymType* ref_type;
public:
    SymTypePointer(SymType* ref_type_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual SymbolClass GetClassName() const;
};

//---SymVar descendants---

class SymVarConst: public SymVar{
private:
    Token value;
public:
    SymVarConst(Token name, Token value, const SymType* type);
    Token GetValueTok() const;
    virtual SymbolClass GetClassName() const;
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void PrintVerbose(ostream& o, int offset = 0) const;
    virtual void GenerateLValue(AsmCode& asm_code) const;
    virtual void GenerateValue(AsmCode& asm_code) const;
};

class SymVarParam: public SymVar{
protected:
    bool by_ref;
    int offset;
    void GenAdrInStack(AsmCode& asm_code) const;
    void GenValueInStack(AsmCode& asm_code) const;
    void GenValueByRef(AsmCode& asm_code) const;
public:
    SymVarParam(Token name, const SymType* type, bool by_ref_, int offset_);
    bool IsByRef() const;
    virtual SymbolClass GetClassName() const;
    virtual void GenerateLValue(AsmCode& asm_code) const;
    virtual void GenerateValue(AsmCode& asm_code) const;
};

class SymVarGlobal: public SymVar{
private:
    AsmStrImmediate label;
public:
    SymVarGlobal(Token name, const SymType* type);
    void SetLabel(AsmStrImmediate& new_label);
    AsmStrImmediate GetLabel() const;
    virtual SymbolClass GetClassName() const;
    void GenerateDeclaration(AsmCode& asm_code);
    virtual void GenerateLValue(AsmCode& asm_code) const;
    virtual void GenerateValue(AsmCode& asm_code) const;
};

class SymVarLocal: public SymVar{
private:
    unsigned offset;
public:
    SymVarLocal(Token name, const SymType* type, unsigned offset_);
    virtual SymbolClass GetClassName() const;
    virtual void GenerateLValue(AsmCode& asm_code) const;
    virtual void GenerateValue(AsmCode& asm_code) const;
    unsigned GetOffset() const;
    void SetOffset(unsigned offset_);
};

//---SymTable---

class SymTable{
private:
    struct SymbLessComp{
        bool operator () (Symbol* a, Symbol* b) const
        {
            return strcmp(a->GetName(), b->GetName()) < 0;
        }
    };
    std::set<Symbol*, SymbLessComp> table;
    std::vector<SymProc*> proc_decl_order;
    unsigned params_size;
    unsigned locals_size;
public:
    SymTable();
    void Add(Symbol* sym);
    const Symbol* Find(Symbol* sym) const;
    const Symbol* Find(const Token& tok) const;
    void Print(ostream& o, int offset = 0) const;
    bool IsEmpty() const;
    unsigned GetSize() const;
    unsigned GetLocalsSize() const;
    unsigned GetParamsSize() const;
    void GenerateDeclarations(AsmCode& asm_code) const;
    void Optimize();
};

#endif
