#ifndef SYNTAX_NODE
#define SYNTAX_NODE

#include "syntax_node_base.h"
#include "scanner.h"
#include "sym_table.h"
#include "exception.h"
#include <vector>

class NodeCallBase: public SyntaxNode{
protected:
    std::vector<SyntaxNode*> args;
    void PrintArgs(ostream& o, int offset = 0) const;
public:
    void AddArg(SyntaxNode* arg);
    virtual void Optimize();
};

class NodeCall: public NodeCallBase{
private:
    SymProc* funct;
public: 
    NodeCall(SymProc* funct_);
    const SymType* GetCurrentArgType() const;
    bool IsCurrentArfByRef() const;
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;
    virtual void GenerateValue(AsmCode& asm_code) const;
    virtual bool IsHaveSideEffect();    
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual void GetAllDependences(VarsContainer& res_cont, bool with_self);
    virtual bool CanBeReplaced();
};

class NodeWriteCall: public NodeCallBase{
private:
    bool new_line;
public:
    NodeWriteCall(bool new_line_ = false);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void GenerateValue(AsmCode& asm_code) const;
    virtual const SymType* GetSymType() const;
    virtual bool IsHaveSideEffect();    
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual bool CanBeReplaced();
    virtual void GetAllDependences(VarsContainer& res_cont, bool with_self = true);
};

class NodeBinaryOp: public SyntaxNode{
private:
    Token token;
    SyntaxNode* left;
    SyntaxNode* right;
    void FinGenForIntRelationalOp(AsmCode& asm_code) const;
    void FinGenForRealRelationalOp(AsmCode& asm_code) const;
    void GenerateForInt(AsmCode& asm_code) const;
    void GenerateForReal(AsmCode& asm_code) const;
public:
    NodeBinaryOp(const Token& name, SyntaxNode* left_, SyntaxNode* right_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;
    virtual void GenerateValue(AsmCode& asm_code) const;
    virtual bool IsConst() const;
    virtual int ComputeIntConstExpr() const;
    virtual float ComputeRealConstExpr() const;
    virtual bool TryToBecomeConst(SyntaxNode*& link);
    virtual bool IsHaveSideEffect();    
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual void GetAllDependences(VarsContainer& res_cont, bool with_self = true);
    virtual void Optimize();
};

class NodeUnaryOp: public SyntaxNode{
protected:
    Token token;
    SyntaxNode* child;
    void GenerateForInt(AsmCode& asm_code) const;
    void GenerateForReal(AsmCode& asm_code) const;
public:
    NodeUnaryOp(const Token& name, SyntaxNode* child_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;
    void GenerateValue(AsmCode& asm_code) const;
    virtual bool IsConst() const;
    virtual int ComputeIntConstExpr() const;
    virtual float ComputeRealConstExpr() const;
    virtual bool TryToBecomeConst(SyntaxNode*& link);
    virtual bool IsHaveSideEffect();    
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual void GetAllDependences(VarsContainer& res_cont, bool with_self = true);
    virtual void Optimize();
};

class NodeIntToRealConv: public NodeUnaryOp{
private:
    SymType* real_type;
public:
    NodeIntToRealConv(SyntaxNode* child_, SymType* real_type_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;
    virtual void GenerateValue(AsmCode& asm_code) const;
    virtual float ComputeRealConstExpr() const;
};

class NodeVar: public SyntaxNode{
private:
    SymVar* var;
public:
    NodeVar(SymVar* var_);
    const SymVar* GetVar();
    virtual const SymType* GetSymType() const;
    virtual void Print(ostream& o, int offset = 0) const;
    virtual bool IsLValue() const;
    virtual SymVar* GetAffectedVar() const;
    virtual void GenerateLValue(AsmCode& asm_code) const;
    virtual void GenerateValue(AsmCode& asm_code) const;
    virtual int ComputeIntConstExpr() const;
    virtual float ComputeRealConstExpr() const;
    virtual bool IsConst() const;
    virtual bool IsHaveSideEffect();    
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual void GetAllDependences(VarsContainer& res_cont, bool with_self = true);
};

class NodeArrayAccess: public SyntaxNode{
private:
    SyntaxNode* arr;
    SyntaxNode* index;
    void ComputeIndexToEax(AsmCode& asm_code) const;
public:
    NodeArrayAccess(SyntaxNode* arr_, SyntaxNode* index_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;    
    virtual bool IsLValue() const;
    virtual SymVar* GetAffectedVar() const;
    virtual void GenerateLValue(AsmCode& asm_code) const;
    virtual void GenerateValue(AsmCode& asm_code) const; 
    virtual bool IsHaveSideEffect();    
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual void GetAllDependences(VarsContainer& res_cont, bool with_self = true);
    virtual void Optimize();
};

class NodeRecordAccess: public SyntaxNode{
private:
    SyntaxNode* record;
    const SymVarLocal* field;
public:
    NodeRecordAccess(SyntaxNode* record_, Token field_);
    virtual void Print(ostream& o, int offset = 0) const;
    virtual const SymType* GetSymType() const;    
    virtual bool IsLValue() const;
    virtual SymVar* GetAffectedVar() const;
    virtual void GenerateLValue(AsmCode& asm_code) const;
    virtual void GenerateValue(AsmCode& asm_code) const; 
    virtual bool IsHaveSideEffect();    
    virtual void GetAllAffectedVars(VarsContainer& res_cont);
    virtual void GetAllDependences(VarsContainer& res_cont, bool with_self = true);
};

#endif
