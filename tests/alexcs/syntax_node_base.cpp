#include "syntax_node_base.h"

//---SyntaxNodeBase---

void SyntaxNodeBase::Optimize()
{
}


bool SyntaxNodeBase::IsDependOnVars(std::set<SymVar*>& vars)
{
    std::set<SymVar*> t;
    GetAllDependences(t);
    for (set<SymVar*>::iterator it = vars.begin(); it != vars.end(); ++it)
        if (t.find(*it) != t.end()) return true;
    return false;
}

bool SyntaxNodeBase::IsAffectToVars(std::set<SymVar*>& vars)
{
    std::set<SymVar*> t;
    GetAllAffectedVars(t);
    for (set<SymVar*>::iterator it = vars.begin(); it != vars.end(); ++it)
        if (t.find(*it) != t.end()) return true;
    return false;
}

bool SyntaxNodeBase::IsAffectToVars()
{
    std::set<SymVar*> t;
    GetAllAffectedVars(t);
    return !t.empty();
}

void SyntaxNodeBase::Print(ostream& o, int offset) const
{
}

bool SyntaxNodeBase::IsHaveSideEffect()
{
    return false;
}

bool SyntaxNodeBase::IsDependOnVar(SymVar* var)
{
    std::set<SymVar*> t;
    GetAllDependences(t);
    return (t.find(var) != t.end());
}

bool SyntaxNodeBase::IsAffectToVar(SymVar* var)
{
    std::set<SymVar*> t;
    GetAllAffectedVars(t);
    return (t.find(var) != t.end());
}

void SyntaxNodeBase::GetAllAffectedVars(VarsContainer&)
{
}

void SyntaxNodeBase::GetAllDependences(VarsContainer&, bool with_self)
{
}

bool SyntaxNodeBase::CanBeReplaced()
{
    return true;
}

bool SyntaxNodeBase:: ContainJump()
{
    return false;
}

//---SyntaxNode---

const SymType* SyntaxNode::GetSymType() const
{
    return NULL;
}

bool SyntaxNode::IsLValue() const
{
    return false;
}

SymVar* SyntaxNode::GetAffectedVar() const
{
    return NULL;
}

void SyntaxNode::GenerateLValue(AsmCode& asm_code) const
{
}

void SyntaxNode::GenerateValue(AsmCode& asm_code) const
{
}

bool SyntaxNode::IsConst() const
{
    return false;
}

Token SyntaxNode::ComputeConstExpr() const
{
    if (GetSymType() == top_type_int) return Token(ComputeIntConstExpr());
    return Token(ComputeRealConstExpr());
}

int SyntaxNode::ComputeIntConstExpr() const
{
    return 0;
}

float SyntaxNode::ComputeRealConstExpr() const
{
    return 0;
}

bool SyntaxNode::TryToBecomeConst(SyntaxNode*& link)
{
    return false;
}
    


