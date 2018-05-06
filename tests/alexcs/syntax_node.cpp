#include "syntax_node.h"

static void Error(string msg, Token token)
{
    stringstream s;
    s << token.GetLine() << ':' << token.GetPos() << " ERROR at '" << token.GetName() << "': " << msg;
    throw( CompilerException(s.str()) );
}

//---NodeCallBase---

void NodeCallBase::PrintArgs(ostream& o, int offset) const
{
    for (std::vector<SyntaxNode*>::const_iterator it = args.begin(); it != args.end(); ++it)
    {
        (*it)->Print(o, offset + 1);
    }
}

void NodeCallBase::AddArg(SyntaxNode* arg)
{
    args.push_back(arg);
}

void NodeCallBase::Optimize()
{
    for (std::vector<SyntaxNode*>::iterator it = args.begin(); it != args.end(); ++it)
    {
        (*it)->TryToBecomeConst(*it);
    }
}

//---NodeCall---

NodeCall::NodeCall(SymProc* funct_):
    funct(funct_)
{
}

const SymType* NodeCall::GetCurrentArgType() const
{
    if (args.size() >= funct->GetArgsCount()) return NULL;
    return funct->GetArg(args.size())->GetVarType();
}

bool NodeCall::IsCurrentArfByRef() const
{
    if (args.size() >= funct->GetArgsCount()) return NULL;
    return funct->GetArg(args.size())->IsByRef();
}

void NodeCall::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "() " << funct->GetName() << " [";
    GetSymType()->Print(o , offset);
    o << "]\n";
    PrintArgs(o, offset);
}

const SymType* NodeCall::GetSymType() const
{
    return funct->GetResultType();
}

void NodeCall::GenerateValue(AsmCode& asm_code) const
{
    if (funct->IsDummyProc()) return;
    if (funct->GetResultType()->GetSize())
        asm_code.AddCmd(ASM_SUB, funct->GetResultType()->GetSize(), REG_ESP);
    for (int i = args.size() - 1 ; 0 <= i ; --i)
        if (funct->GetArg(i)->IsByRef())
            args[i]->GenerateLValue(asm_code);
        else
            args[i]->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_CALL, AsmMemory(funct->GetLabel()));
}

bool NodeCall::IsHaveSideEffect()
{
    for (int i = 0; i < args.size(); ++i)
    {
        if (funct->IsAffectToParam(i)) return true;
    }
    return funct->IsHaveSideEffect();
}

void NodeCall::GetAllAffectedVars(VarsContainer& res_cont)
{
    for (int i = 0; i < args.size(); ++i)
    {
        args[i]->GetAllAffectedVars(res_cont);
        if (funct->IsAffectToParam(i)) res_cont.insert(args[i]->GetAffectedVar());
    }
    return funct->GetAllAffectedVars(res_cont);
}

void NodeCall::GetAllDependences(VarsContainer& res_cont, bool with_self)
{
    for (int i = 0; i < args.size(); ++i)
    {
        if (funct->IsDependOnParam(i)) args[i]->GetAllDependences(res_cont);
    }
    funct->GetAllDependences(res_cont);
}

bool NodeCall::CanBeReplaced()
{
    for (int i = 0; i < args.size(); ++i)
    {
        if (!args[i]->CanBeReplaced()) return false;
    }
    return funct->CanBeReplaced();
}

//---NodeWriteCall---

NodeWriteCall::NodeWriteCall(bool new_line_):
    new_line(new_line_)
{
}

void NodeWriteCall::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "() " << (new_line ? "writeln" : "write" ) << " [untyped]\n";
    PrintArgs(o, offset);
}

void NodeWriteCall::GenerateValue(AsmCode& asm_code) const
{
    for (std::vector<SyntaxNode*>::const_iterator it = args.begin(); it != args.end(); ++it)
    {
        (*it)->GenerateValue(asm_code);
        const SymType* type = (*it)->GetSymType();
        if (type == top_type_int)
            asm_code.GenCallWriteForInt();
        else if (type == top_type_real)
            asm_code.GenCallWriteForReal();
        else
            asm_code.GenCallWriteForStr();
    }
    if (new_line) asm_code.GenWriteNewLine();
}

const SymType* NodeWriteCall::GetSymType() const
{
    return top_type_untyped;
}

bool NodeWriteCall::IsHaveSideEffect()
{
    return true;
}

void NodeWriteCall::GetAllAffectedVars(VarsContainer& res_cont)
{
    for (std::vector<SyntaxNode*>::iterator it = args.begin(); it != args.end(); ++ it)
        (*it)->GetAllAffectedVars(res_cont);
}

void NodeWriteCall::GetAllDependences(VarsContainer& res_cont, bool with_self)
{
    for (std::vector<SyntaxNode*>::iterator it = args.begin(); it != args.end(); ++ it)
        (*it)->GetAllDependences(res_cont);
}

bool NodeWriteCall::CanBeReplaced()
{
    return false;
}

//---NodeBinaryOp---

void NodeBinaryOp::FinGenForIntRelationalOp(AsmCode& asm_code) const
{
    asm_code.AddCmd(ASM_CMP, REG_EBX, REG_EAX);
    switch (token.GetValue())
    {
        case TOK_GREATER:
            asm_code.AddCmd(ASM_SETG, REG_AL, SIZE_NONE);
        break;
        case TOK_GREATER_OR_EQUAL:
            asm_code.AddCmd(ASM_SETGE, REG_AL, SIZE_NONE);
        break;
        case TOK_LESS:
            asm_code.AddCmd(ASM_SETL, REG_AL, SIZE_NONE);
        break;
        case TOK_LESS_OR_EQUAL:
            asm_code.AddCmd(ASM_SETLE, REG_AL, SIZE_NONE);
        break;
        case TOK_EQUAL:
            asm_code.AddCmd(ASM_SETE, REG_AL, SIZE_NONE);
        break;
        case TOK_NOT_EQUAL:
            asm_code.AddCmd(ASM_SETNE, REG_AL, SIZE_NONE);
    }
    asm_code.AddCmd(ASM_MOVZB, REG_AL, REG_EAX);
}

void NodeBinaryOp::FinGenForRealRelationalOp(AsmCode& asm_code) const
{
    asm_code.AddCmd(ASM_FXCH, REG_ST1, SIZE_NONE);
    asm_code.AddCmd(ASM_FCOMPP, SIZE_NONE);
    asm_code.AddCmd(ASM_FNSTSW, REG_AX, SIZE_NONE);
    switch (token.GetValue())
    {
        case TOK_GREATER:
            asm_code.AddCmd(ASM_SAHF, SIZE_NONE);
            asm_code.AddCmd(ASM_SETA, REG_AL, SIZE_NONE);
        break;
        case TOK_GREATER_OR_EQUAL:
            asm_code.AddCmd(ASM_SAHF, SIZE_NONE);
            asm_code.AddCmd(ASM_SETAE, REG_AL, SIZE_NONE);
        break;
        case TOK_LESS:
            asm_code.AddCmd(ASM_SAHF, SIZE_NONE);
            asm_code.AddCmd(ASM_SETB, REG_AL, SIZE_NONE);
        break;
        case TOK_LESS_OR_EQUAL:
            asm_code.AddCmd(ASM_SAHF, SIZE_NONE);
            asm_code.AddCmd(ASM_SETBE, REG_AL, SIZE_NONE);
        break;
        case TOK_EQUAL:
            asm_code.AddCmd(ASM_SAHF, SIZE_NONE);
            asm_code.AddCmd(ASM_SETE, REG_AL, SIZE_NONE);
        break;
        case TOK_NOT_EQUAL:
            asm_code.AddCmd(ASM_SAHF, SIZE_NONE);
            asm_code.AddCmd(ASM_SETNE, REG_AL, SIZE_NONE);
    }
    asm_code.AddCmd(ASM_MOVZB, REG_AL, REG_EAX);
    asm_code.AddCmd(ASM_MOV, REG_EAX, AsmMemory(REG_ESP));
}

void NodeBinaryOp::GenerateForInt(AsmCode& asm_code) const
{
    asm_code.AddCmd(ASM_POP, REG_EBX);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    switch (token.GetValue())
    {
        case TOK_PLUS:
            asm_code.AddCmd(ASM_ADD, REG_EBX, REG_EAX);
        break;
        case TOK_MINUS:
            asm_code.AddCmd(ASM_SUB, REG_EBX, REG_EAX);
        break;
        case TOK_MULT:
            asm_code.AddCmd(ASM_IMUL, REG_EBX);
        break;
        case TOK_DIV:
            asm_code.AddCmd(ASM_XOR, REG_EDX, REG_EDX);
            asm_code.AddCmd(ASM_IDIV, REG_EBX);
        break;
        case TOK_MOD:
            asm_code.AddCmd(ASM_XOR, REG_EDX, REG_EDX);
            asm_code.AddCmd(ASM_IDIV, REG_EBX);
            asm_code.AddCmd(ASM_PUSH, REG_EDX);
            return;
        case TOK_AND:
            asm_code.AddCmd(ASM_AND, REG_EBX, REG_EAX);
        break;
        case TOK_OR:
            asm_code.AddCmd(ASM_OR, REG_EBX, REG_EAX);
        break;
        case TOK_XOR:
            asm_code.AddCmd(ASM_XOR, REG_EBX, REG_EAX);
        break;
        case TOK_SHR:
            asm_code.AddCmd(ASM_SAR, REG_EAX, REG_EBX);
            asm_code.AddCmd(ASM_PUSH, REG_EBX);
            return;
        break;
        case TOK_SHL:
            asm_code.AddCmd(ASM_SAL, REG_EAX, REG_EBX);
            asm_code.AddCmd(ASM_PUSH, REG_EBX);
            return;
        break;
        case TOK_NOT:
            asm_code.AddCmd(ASM_NOT, REG_EBX, REG_EAX);
        break;
        default:
            FinGenForIntRelationalOp(asm_code);
    }
    asm_code.AddCmd(ASM_PUSH, REG_EAX);
}

void NodeBinaryOp::GenerateForReal(AsmCode& asm_code) const
{
    asm_code.AddCmd(ASM_FLD, AsmMemory(REG_ESP, 4), SIZE_SHORT);
    asm_code.AddCmd(ASM_FLD, AsmMemory(REG_ESP), SIZE_SHORT);
    asm_code.AddCmd(ASM_ADD, 4, REG_ESP);
    switch (token.GetValue())
    {
        case TOK_PLUS:
            asm_code.AddCmd(ASM_FADDP, REG_ST, REG_ST1, SIZE_NONE);
        break;
        case TOK_MINUS:
            asm_code.AddCmd(ASM_FSUBRP, REG_ST, REG_ST1, SIZE_NONE);
        break;
        case TOK_MULT:
            asm_code.AddCmd(ASM_FMULP, REG_ST, REG_ST1, SIZE_NONE);
        break;
        case TOK_DIVISION:
            asm_code.AddCmd(ASM_FDIVRP, REG_ST, REG_ST1, SIZE_NONE);
        break;
        default:
            FinGenForRealRelationalOp(asm_code);
            return;
    }
    asm_code.AddCmd(ASM_FSTP, AsmMemory(REG_ESP), SIZE_SHORT);
}

NodeBinaryOp::NodeBinaryOp(const Token& name, SyntaxNode* left_, SyntaxNode* right_):
    token(name),
    left(left_),
    right(right_)
{
}

void NodeBinaryOp::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << token.GetName() << " [";
    GetSymType()->Print(o , offset);
    o << "]\n";
    left->Print(o, offset + 1);
    right->Print(o, offset + 1);
}

const SymType* NodeBinaryOp::GetSymType() const
{
    if (token.IsRelationalOp()) return top_type_int;
    return left->GetSymType();
}

void NodeBinaryOp::GenerateValue(AsmCode& asm_code) const
{
    left->GenerateValue(asm_code);
    right->GenerateValue(asm_code);
    if (left->GetSymType() == top_type_int) GenerateForInt(asm_code);
    else GenerateForReal(asm_code);
}

bool NodeBinaryOp::IsConst() const
{
    return left->IsConst() && right->IsConst();
}

int NodeBinaryOp::ComputeIntConstExpr() const
{
    if (left->GetSymType() == top_type_int)
    {
        int a = left->ComputeIntConstExpr();
        int b = right->ComputeIntConstExpr();
        switch (token.GetValue())
        {
            case TOK_PLUS:
                return a + b;
            break;
            case TOK_MINUS:
                return a - b;
            break;
            case TOK_MULT:
                return a * b;
            break;
            case TOK_DIV:
                return a / b;
            break;
            case TOK_MOD:
                return a % b;
            case TOK_AND:
                return a & b;
            break;
            case TOK_OR:
                return a | b;
            break;
            case TOK_XOR:
                return a ^ b;
            break;
            case TOK_SHR:
                return a >> b;
            break;
            case TOK_SHL:
                return a << b;
            break;
            case TOK_GREATER:
                return a > b;
            break;
            case TOK_GREATER_OR_EQUAL:
                return a >= b;
            break;
            case TOK_LESS:
                return a < b;
            break;
            case TOK_LESS_OR_EQUAL:
                return a <= b;
            break;
            case TOK_EQUAL:
                return a == b;
            break;
            case TOK_NOT_EQUAL:
                return a != b;
        }
    }
    else
    {
        float a = left->ComputeRealConstExpr();
        float b = right->ComputeRealConstExpr();
        switch (token.GetValue())
        {
            case TOK_GREATER:
                return a > b;
            break;
            case TOK_GREATER_OR_EQUAL:
                return a >= b;
            break;
            case TOK_LESS:
                return a < b;
            break;
            case TOK_LESS_OR_EQUAL:
                return a <= b;
            break;
            case TOK_EQUAL:
                return a == b;
            break;
            case TOK_NOT_EQUAL:
                return a != b;
        }
    }
}

float NodeBinaryOp::ComputeRealConstExpr() const
{
    float a = left->ComputeRealConstExpr();
    float b = right->ComputeRealConstExpr();
    switch (token.GetValue())
    {
        case TOK_PLUS:
            return a + b;
        break;
        case TOK_MINUS:
            return a - b;
        break;
        case TOK_MULT:
            return a * b;
        break;
        case TOK_DIVISION:
            return a / b;
        break;
    }
}

bool NodeBinaryOp::TryToBecomeConst(SyntaxNode*& link)
{
    left->TryToBecomeConst(left);
    right->TryToBecomeConst(right);
    bool lc = left->IsConst();
    bool rc = right->IsConst();
    if (!lc && !rc) return false;
    if (!lc || !rc) return true;
    Token tok_val(ComputeConstExpr());
    SymVarConst* sym = new SymVarConst(tok_val, tok_val, GetSymType());
    link = new NodeVar(sym);
    delete this;
    return true;
}

bool NodeBinaryOp::IsHaveSideEffect()
{
    return left->IsHaveSideEffect() || right->IsHaveSideEffect();
}

void NodeBinaryOp::GetAllAffectedVars(VarsContainer& res_cont)
{
    left->GetAllAffectedVars(res_cont);
    right->GetAllAffectedVars(res_cont);
}

void NodeBinaryOp::GetAllDependences(VarsContainer& res_cont, bool with_self)
{
    left->GetAllDependences(res_cont);
    right->GetAllDependences(res_cont);
}

void NodeBinaryOp::Optimize()
{
    left->Optimize();
    right->Optimize();
}

//---NodeUnaryOp---

void NodeUnaryOp::GenerateForInt(AsmCode& asm_code) const
{
    asm_code.AddCmd(ASM_POP, REG_EAX);
    switch (token.GetValue())
    {
        case TOK_NOT:
            asm_code.AddCmd(ASM_TEST, REG_EAX, REG_EAX);
            asm_code.AddCmd(ASM_SETE, REG_AL, SIZE_NONE);
            asm_code.AddCmd(ASM_MOVZB, REG_AL, REG_EAX);
        break;
        case TOK_PLUS:
        break;
        case TOK_MINUS:
            asm_code.AddCmd(ASM_NEG, REG_EAX);
        break;
    }
    asm_code.AddCmd(ASM_PUSH, REG_EAX);
}

void NodeUnaryOp::GenerateForReal(AsmCode& asm_code) const
{
    if (token.GetValue() != TOK_MINUS) return;
    asm_code.AddCmd(ASM_FLD, AsmMemory(REG_ESP), SIZE_SHORT);
    asm_code.AddCmd(ASM_FCH, SIZE_SHORT);
    asm_code.AddCmd(ASM_FSTP, AsmMemory(REG_ESP), SIZE_SHORT);
}

NodeUnaryOp::NodeUnaryOp(const Token& name, SyntaxNode* child_):
    token(name),
    child(child_)
{
}

void NodeUnaryOp::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << token.GetName() << " [";
    GetSymType()->Print(o , offset);
    o << "]\n";
    child->Print(o, offset + 1);
}

const SymType* NodeUnaryOp::GetSymType() const
{
    return child->GetSymType();
}

void NodeUnaryOp::GenerateValue(AsmCode& asm_code) const
{
    child->GenerateValue(asm_code);
    if (GetSymType() == top_type_int) GenerateForInt(asm_code);
    else GenerateForReal(asm_code);
}

bool NodeUnaryOp::IsConst() const
{
    return child->IsConst();
}

int NodeUnaryOp::ComputeIntConstExpr() const
{
    int a = child->ComputeIntConstExpr();
    switch (token.GetValue())
    {
        case TOK_NOT:
            return !a;
        break;
        case TOK_PLUS:
            return a;
        break;
        case TOK_MINUS:
            return -a;
        break;
    }
}

float NodeUnaryOp::ComputeRealConstExpr() const
{
    float a = child->ComputeRealConstExpr();
    if (token.GetValue() == TOK_PLUS) return a;
    return -a;
}

bool NodeUnaryOp::TryToBecomeConst(SyntaxNode*& link)
{
    if (!child->IsConst()) return false;
    Token tok_val(child->ComputeConstExpr());
    SymVarConst* sym = new SymVarConst(tok_val, tok_val, GetSymType());
    link = new NodeVar(sym);
    delete this;
    return true;
}

bool NodeUnaryOp::IsHaveSideEffect()
{
    return child->IsHaveSideEffect();
}

void NodeUnaryOp::GetAllAffectedVars(VarsContainer& res_cont)
{
    return child->GetAllAffectedVars(res_cont);
}

void NodeUnaryOp::GetAllDependences(VarsContainer& res_cont, bool with_self)
{
    return child->GetAllDependences(res_cont);
}

void NodeUnaryOp::Optimize()
{
    child->Optimize();
}

//---NodeIntToRealConv---

NodeIntToRealConv::NodeIntToRealConv(SyntaxNode* child_, SymType* real_type_):
    NodeUnaryOp(Token(), child_),
    real_type(real_type_)
{
}

void NodeIntToRealConv::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "IntToReal [";
    real_type->Print(o, 0);
    o << "]\n";
    child->Print(o, offset + 1);
}

const SymType* NodeIntToRealConv::GetSymType() const
{
    return real_type;
}

void NodeIntToRealConv::GenerateValue(AsmCode& asm_code) const
{
    child->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_FILD, AsmMemory(REG_ESP), SIZE_SHORT);
    asm_code.AddCmd(ASM_FSTP, AsmMemory(REG_ESP), SIZE_SHORT);
}

float NodeIntToRealConv::ComputeRealConstExpr() const
{
    return child->ComputeRealConstExpr();
}

//---NodeVar---

NodeVar::NodeVar(SymVar* var_):
    var(var_)
{
}

const SymVar* NodeVar::GetVar()
{
    return var;
}

const SymType* NodeVar::GetSymType() const
{
    return var->GetVarType()->GetActualType();
}

void NodeVar::Print(ostream& o, int offset) const
{
    var->PrintAsNode(o, offset);
}

bool NodeVar::IsLValue() const
{
    return !(var->GetClassName() & SYM_VAR_CONST);
}

SymVar* NodeVar::GetAffectedVar() const
{
    return var;
}

void NodeVar::GenerateLValue(AsmCode& asm_code) const
{
    var->GenerateLValue(asm_code);
}

void NodeVar::GenerateValue(AsmCode& asm_code) const
{
    var->GenerateValue(asm_code);
}

int NodeVar::ComputeIntConstExpr() const
{
    return ((SymVarConst*)var)->GetValueTok().GetIntValue();
}

float NodeVar::ComputeRealConstExpr() const
{
    return ((SymVarConst*)var)->GetValueTok().GetRealValue();
}

bool NodeVar::IsConst() const
{
    return var->GetClassName() & SYM_VAR_CONST;
}

bool NodeVar::IsHaveSideEffect()
{
    return false;
}

void NodeVar::GetAllAffectedVars(VarsContainer& res_cont)
{
}

void NodeVar::GetAllDependences(VarsContainer& res_cont, bool with_self)
{
    if (with_self) res_cont.insert(var);
}

//---NodeArrayAccess----

NodeArrayAccess::NodeArrayAccess(SyntaxNode* arr_, SyntaxNode* index_):
    arr(arr_),
    index(index_)
{
}

void NodeArrayAccess::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << "[] [";
    GetSymType()->Print(o, offset);
    o << "]\n";
    arr->Print(o, offset+1);
    index->Print(o, offset+1);
}

const SymType* NodeArrayAccess::GetSymType() const
{
    return ((SymTypeArray*)arr->GetSymType())->GetElemType();
}

bool NodeArrayAccess::IsLValue() const
{
    return true;
}

SymVar* NodeArrayAccess::GetAffectedVar() const
{
    return arr->GetAffectedVar();
}

void NodeArrayAccess::ComputeIndexToEax(AsmCode& asm_code) const
{
    arr->GenerateLValue(asm_code);
    index->GenerateValue(asm_code);
    asm_code.AddCmd(ASM_MOV, GetSymType()->GetSize(), REG_EBX);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_SUB, ((SymTypeArray*)arr->GetSymType())->GetLow(), REG_EAX);
    asm_code.AddCmd(ASM_XOR, REG_EDX, REG_EDX);
    asm_code.AddCmd(ASM_MUL, REG_EBX);
    asm_code.AddCmd(ASM_POP, REG_EBX);
    asm_code.AddCmd(ASM_ADD, REG_EBX, REG_EAX);
}

void NodeArrayAccess::GenerateLValue(AsmCode& asm_code) const
{
    ComputeIndexToEax(asm_code);
    asm_code.AddCmd(ASM_PUSH, REG_EAX);
}

void NodeArrayAccess::GenerateValue(AsmCode& asm_code) const
{
    ComputeIndexToEax(asm_code);
    if (GetSymType()->GetSize() == 4)
        asm_code.AddCmd(ASM_PUSH, AsmMemory(REG_EAX));
    else
    {
        asm_code.AddCmd(ASM_PUSH, REG_EAX);
        asm_code.PushMemory(GetSymType()->GetSize());
    }
}

bool NodeArrayAccess::IsHaveSideEffect()
{
    return index->IsHaveSideEffect();
}

void NodeArrayAccess::GetAllAffectedVars(VarsContainer& res_cont)
{
    index->GetAllAffectedVars(res_cont);
    arr->GetAllAffectedVars(res_cont);
}

void NodeArrayAccess::GetAllDependences(VarsContainer& res_cont, bool with_self)
{
    index->GetAllDependences(res_cont);
    arr->GetAllDependences(res_cont, with_self);
}

void NodeArrayAccess::Optimize()
{
    index->Optimize();
    index->TryToBecomeConst(index);
}

//---NodeRecordAccess---

NodeRecordAccess::NodeRecordAccess(SyntaxNode* record_, Token field_):
    record(record_)
{
    const SymVarLocal* var = ((SymTypeRecord*)record_->GetSymType())->FindField(field_);
    if (var == NULL) Error("unknown record field identifier", field_);
    field = var;
}

void NodeRecordAccess::Print(ostream& o, int offset) const
{
    PrintSpaces(o, offset) << ". [";
    GetSymType()->Print(o , offset);
    o << "]\n";
    record->Print(o, offset + 1);
    field->PrintAsNode(o, offset + 1);
}

const SymType* NodeRecordAccess::GetSymType() const
{
    return field->GetVarType();
}

bool NodeRecordAccess::IsLValue() const
{
    return true;
}

SymVar* NodeRecordAccess::GetAffectedVar() const
{
    return record->GetAffectedVar();
}

void NodeRecordAccess::GenerateLValue(AsmCode& asm_code) const
{
    record->GenerateLValue(asm_code);
    asm_code.AddCmd(ASM_POP, REG_EAX);
    asm_code.AddCmd(ASM_LEA, AsmMemory(REG_EAX, field->GetOffset()), REG_EAX);
    asm_code.AddCmd(ASM_PUSH, REG_EAX);
}

void NodeRecordAccess::GenerateValue(AsmCode& asm_code) const
{
    GenerateLValue(asm_code);
    asm_code.PushMemory(field->GetVarType()->GetSize());
}

bool NodeRecordAccess::IsHaveSideEffect()
{
    return false;
}

void NodeRecordAccess::GetAllAffectedVars(VarsContainer& res_cont)
{
}

void NodeRecordAccess::GetAllDependences(VarsContainer& res_cont, bool with_self)
{
    if (with_self) res_cont.insert(GetAffectedVar());
}
