#ifndef SYNTAX_NODE_BASE
#define SYNTAX_NODE_BASE

#include "scanner.h"
#include "generator.h"
#include <ostream>
#include <set>

class SymType;
class SymVar;

extern SymType* top_type_int;
extern SymType* top_type_real;
extern SymType* top_type_untyped;
extern SymType* top_type_str;

typedef std::set<SymVar*> VarsContainer;
typedef std::map<SymVar*, std::set<SymVar*> > DependencyGraph;
typedef std::set<SymVar*> DependedVerts;

class SyntaxNodeBase{
public:
    bool IsDependOnVars(std::set<SymVar*>& vars);
    bool IsAffectToVars(std::set<SymVar*>& vars);
    bool IsAffectToVars();
    bool IsAffectToVar(SymVar* var);
    bool IsDependOnVar(SymVar* var);    
    virtual bool IsHaveSideEffect();    
    virtual void Print(ostream& o, int offset = 0) const;
    virtual void GetAllAffectedVars(VarsContainer&);
    virtual void GetAllDependences(VarsContainer&, bool with_self = true);
    virtual bool CanBeReplaced();
    virtual bool ContainJump();
    virtual void Optimize();
};

class SyntaxNode: public SyntaxNodeBase{
public:
    virtual const SymType* GetSymType() const;
    virtual bool IsLValue() const;
    virtual SymVar* GetAffectedVar() const;
    virtual void GenerateLValue(AsmCode& asm_code) const;    
    virtual void GenerateValue(AsmCode& asm_code) const;
    virtual bool IsConst() const;
    virtual Token ComputeConstExpr() const;
    virtual int ComputeIntConstExpr() const;
    virtual float ComputeRealConstExpr() const;
    virtual bool TryToBecomeConst(SyntaxNode*& link);
};

#endif
