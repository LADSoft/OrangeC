#include "parser.h"

//---Parser---

SyntaxNode* Parser::ConvertType(SyntaxNode* node, const SymType* type)
{
    if (node->GetSymType() == type) return node;
    if (node->GetSymType() == top_type_int && type == top_type_real)
        return new NodeIntToRealConv(node, top_type_real);
    return NULL;
}

void Parser::TryToConvertType(SyntaxNode*& first, SyntaxNode*& second)
{
    if (first->GetSymType() == second->GetSymType()) return;
    SyntaxNode* tmp = ConvertType(first, second->GetSymType());
    if (tmp != NULL)
    {
        first = tmp;
        return;
    }
    tmp = ConvertType(second, first->GetSymType());
    if (tmp != NULL) second = tmp;
}

void Parser::TryToConvertType(SyntaxNode*& expr, const SymType* type)
{
    if (expr->GetSymType() == type) return;
    SyntaxNode* tmp = ConvertType(expr, type);
    if (tmp == NULL) return;
    expr = tmp;
    return;
}

void Parser::ConvertTypeOrDie(SyntaxNode*& first, SyntaxNode*& second, Token tok_err)
{
    TryToConvertType(first, second);
    if (first->GetSymType() != second->GetSymType())
    {
        std::stringstream s;
        s << "incompatible types: ";
        first->GetSymType()->Print(s, 0);
        s << " and ";
        second->GetSymType()->Print(s, 0);
        Error(s.str(), tok_err);
    }
}

void Parser::ConvertTypeOrDie(SyntaxNode*& expr, const SymType* type, Token tok_err)
{
    TryToConvertType(expr, type);
    if (expr->GetSymType() != type)
    {
        std::stringstream s;
        s << "incompatible types: ";
        expr->GetSymType()->Print(s, 0);
        s << " and ";
        type->Print(s, 0);
        Error(s.str(), tok_err);
    }
}

void Parser::ConvertToBaseTypeOrDie(SyntaxNode*& first, SyntaxNode*& second, Token tok_err)
{
    CheckForBaseType(first, tok_err);
    CheckForBaseType(second, tok_err);
    ConvertTypeOrDie(first, second, tok_err);
}

void Parser::CheckForBaseType(SyntaxNode* expr, Token tok_err)
{
    if (expr->GetSymType() != top_type_int && expr->GetSymType() != top_type_real)
    {
        stringstream s;
        s << "can't do arithmetic operation with ";
        expr->GetSymType()->Print(s, 0);
        Error(s.str(), tok_err);
    }
}

void Parser::PrintSyntaxTree(ostream& o)
{
    body->Print(o, 0);
}

void Parser::PrintSymTable(ostream& o)
{
    if (body != NULL) sym_table_stack.back()->Print(o, 0);
}

void Parser::Generate(ostream& o)
{
    sym_table_stack.back()->GenerateDeclarations(asm_code);
    asm_code.AddMainFunctionLabel();
    asm_code.AddCmd(ASM_MOV, REG_ESP, REG_EBP);
    body->Generate(asm_code);
    asm_code.AddLabel(exit_label);
    asm_code.AddCmd(ASM_MOV, REG_EBP, REG_ESP);
    asm_code.AddCmd(ASM_MOV, 0, REG_EAX);
    asm_code.AddCmd(ASM_RET);
    asm_code.Print(o);
}

Parser::Parser(Scanner& scanner, bool optimize):
    optimization(optimize),
    body(NULL),
    scan(scanner),
    current_proc(NULL)
{
    scan.NextToken();
    top_sym_table.Add(top_type_int);
    top_sym_table.Add(top_type_real);
    sym_table_stack.push_back(&top_sym_table);
    sym_table_stack.push_back(new SymTable());
    exit_label = asm_code.GenLabel("exit");
    Parse();
}

SymType* Parser::ParseArrayType()
{
    std::vector<std::pair<int, int> > bounds;
    CheckTokOrDie(TOK_ARRAY);
    CheckTokOrDie(TOK_BRACKETS_SQUARE_LEFT);
    bool was_comma = true;
    while (was_comma)
    {
        int fst = GetIntConstValueOrDie();
        CheckTokOrDie(TOK_DOUBLE_DOT);
        int sec = GetIntConstValueOrDie();
        if (sec < fst) Error("invalid subrange type");
        bounds.push_back(std::pair<int, int>(fst, sec));
        if (was_comma = (scan.GetToken().GetValue() == TOK_COMMA)) scan.NextToken();
    }
    CheckTokOrDie(TOK_BRACKETS_SQUARE_RIGHT);
    CheckTokOrDie(TOK_OF);
    SymType* res = ParseType();
    for (std::vector<std::pair<int, int> >::reverse_iterator it = bounds.rbegin(); it != bounds.rend(); ++it)
        res = new SymTypeArray(res, it->first, it->second);
    return res;
}

SymType* Parser::ParseRecordType()
{
    CheckTokOrDie(TOK_RECORD);
    sym_table_stack.push_back(new SymTable);
    ParseVarDeclarations(false);
    SymType* res = new SymTypeRecord(sym_table_stack.back());
    sym_table_stack.pop_back();
    CheckTokOrDie(TOK_END);
    return res;
}

SymType* Parser::ParsePointerType()
{
    Error("pointers was not implemented");
    CheckTokOrDie(TOK_CAP);
    if (!scan.GetToken().IsVar()) Error("identifier expected");
    const Symbol* ref_type = FindSymbolOrDie(scan.GetToken(), SYM_TYPE, "type identifier expected");
    scan.NextToken();
    return new SymTypePointer((SymType*)ref_type);
}

SymType* Parser::ParseType()
{
    Token name = scan.GetToken();
    switch (name.GetValue()) {
        case TOK_ARRAY: {
            return ParseArrayType();
        } break;
        case TOK_RECORD: {
            return ParseRecordType();
        } break;
        case TOK_CAP: {
            return ParsePointerType();
        } break;
        default: {
            const Symbol* res = FindSymbolOrDie(scan.GetToken(), SYM_TYPE, "type identifier expected");
            scan.NextToken();
            return (SymType*)res;
        }
    }
}

void Parser::ParseConstDeclarations()
{
    while (scan.GetToken().IsVar())
    {
        Token name = scan.GetToken();
        if (FindSymbol(name) != NULL) Error("duplicate declaration");
        CheckNextTokOrDie(TOK_EQUAL);
        SymVarConst* sym = ParseConstExprOrDie(name);// ParseConstant(name);
        sym_table_stack.back()->Add(sym);
        CheckTokOrDie(TOK_SEMICOLON);
    }
}

void Parser::ParseVarDeclarations(bool is_global)
{
    while (scan.GetToken().IsVar())
    {
        ParseVarDeclarationFactory(is_global ? SYM_VAR_GLOBAL : SYM_VAR_LOCAL);
        CheckTokOrDie(TOK_SEMICOLON);
    }
}

void Parser::ParseVarDeclarationFactory(SymbolClass var_class_name)
{
    std::vector<Token> vars;
    bool was_comma = true;
    while (was_comma)
    {
        Token name = scan.GetToken();
        if (sym_table_stack.back()->Find(name) != NULL) Error("duplicate declaration");
        vars.push_back(name);
        if (was_comma = (scan.NextToken().GetValue() == TOK_COMMA))
            scan.NextToken();
    }
    CheckTokOrDie(TOK_COLON);
    SymType* type = ParseType();
    for (std::vector<Token>::iterator it = vars.begin(); it != vars.end(); ++it)
    {
        switch (var_class_name)
        {
            case SYM_VAR_PARAM:
                //TODO
            break;
            case SYM_VAR_GLOBAL:
                sym_table_stack.back()->Add(new SymVarGlobal(*it, type));
            break;
            case SYM_VAR_LOCAL:
                sym_table_stack.back()->Add(new SymVarLocal(*it, type, sym_table_stack.back()->GetLocalsSize()));
            break;
        }
    }
}

void Parser::ParseTypeDeclarations()
{
    while (scan.GetToken().IsVar())
    {
        Token name = scan.GetToken();
        if (FindSymbol(name) != NULL) Error("duplicate declaration");
        CheckNextTokOrDie(TOK_EQUAL);
        SymType* type = ParseType();
        sym_table_stack.back()->Add(new SymTypeAlias(name, type));
        CheckTokOrDie(TOK_SEMICOLON);
    }
}

void Parser::ParseDeclarations(bool is_global)
{
    bool loop = true;
    while (loop)
    {
        switch (scan.GetToken().GetValue()) {
            case TOK_PROCEDURE:
            case TOK_FUNCTION:
                ParseFunctionDefinition();
            break;
            case TOK_CONST:
                scan.NextToken();
                ParseConstDeclarations();
            break;
            case TOK_VAR:
                scan.NextToken();
                if (!scan.GetToken().IsVar()) Error("identifier expected");
                ParseVarDeclarations(is_global);
            break;
            case TOK_TYPE:
                scan.NextToken();
                if (!scan.GetToken().IsVar()) Error("identifier expected");
                ParseTypeDeclarations();
            break;
            default:
                loop = false;
        }
    }
}

void Parser::ParseFunctionParameters(SymProc* funct)
{
    CheckTokOrDie(TOK_BRACKETS_LEFT);
    if (scan.GetToken().GetValue() == TOK_BRACKETS_RIGHT) Error("empty formal parameters list");
    bool was_semicolon = true;
    while (was_semicolon)
    {
        bool by_ref = false;
        if (by_ref = (scan.GetToken().GetValue() == TOK_VAR)) scan.NextToken();
        vector<Token> v;
        bool was_comma = true;
        while (was_comma)
        {
            if (!scan.GetToken().IsVar()) Error("identifier expected");
            v.push_back(scan.GetToken());
            if (was_comma = (scan.NextToken().GetValue() == TOK_COMMA)) scan.NextToken();
        }
        CheckTokOrDie(TOK_COLON);
        const SymType* type = (SymType*)FindSymbolOrDie(scan.GetToken(), SYM_TYPE, "type identifier expected");
        for (vector<Token>::iterator it = v.begin(); it != v.end(); ++it)
        {
            SymVarParam* param = new SymVarParam(*it, type, by_ref, sym_table_stack.back()->GetParamsSize() + 8);
            sym_table_stack.back()->Add(param);
            funct->AddParam(param);
        }
        if (was_semicolon = (scan.NextToken().GetValue() == TOK_SEMICOLON)) scan.NextToken();
    }
    CheckTokOrDie(TOK_BRACKETS_RIGHT);
}

void Parser::ParseFunctionBody(SymProc* funct)
{
    current_proc = funct;
    if (scan.GetToken().GetValue() != TOK_BEGIN) Error("'begin' expected");
    funct->AddBody(ParseStatement());
    current_proc = NULL;
}

void Parser::ParseFunctionDefinition()
{
    SymProc* res = NULL;
    SymProc* prototype = NULL;
    TokenValue op = scan.GetToken().GetValue();
    if (op != TOK_PROCEDURE && op != TOK_FUNCTION) return;
    Token name = scan.NextToken();
    const Symbol* sym = FindSymbol(name);
    if (sym != NULL)
    {
        if (sym->GetClassName() & SYM_PROC && !((SymProc*)sym)->IsHaveBody())
            prototype = (SymProc*)sym;
        else
            Error("duplicate identifier");
    }
    res = (op == TOK_PROCEDURE) ? new SymProc(name) : new SymFunct(name);
    if (prototype == NULL) sym_table_stack.back()->Add(res);
    sym_table_stack.push_back(new SymTable);
    res->AddSymTable(sym_table_stack.back());
    scan.NextToken();
    if (scan.GetToken().GetValue() == TOK_BRACKETS_LEFT) ParseFunctionParameters(res);
    if (op == TOK_FUNCTION)
    {
        CheckTokOrDie(TOK_COLON);
        const SymType* type = (SymType*)FindSymbolOrDie(scan.GetToken(), SYM_TYPE, "type identifier expected");
        scan.NextToken();
        ((SymFunct*)res)->AddResultType(type);
    }
    CheckTokOrDie(TOK_SEMICOLON);
    if (prototype != NULL)
    {
        if (!prototype->ValidateParams(res)) Error("function prototype differs from preveous declaration", name);
        delete res;
        res = prototype;
        sym_table_stack.pop_back();
        sym_table_stack.push_back(res->GetSymTable());
    }
    if (scan.GetToken().GetValue() == TOK_FORWARD)
    {
        scan.NextToken();
        CheckTokOrDie(TOK_SEMICOLON);
    }
    else
    {
        res->ObtainLabels(asm_code);
        ParseDeclarations(false);
        ParseFunctionBody(res);
        CheckTokOrDie(TOK_SEMICOLON);
    }
    sym_table_stack.pop_back();
}

NodeStatement* Parser::ParseStatement()
{
    switch (scan.GetToken().GetValue())
    {
        case TOK_BEGIN:
            return ParseBlockStatement();
        case TOK_FOR:
        case TOK_WHILE:
        case TOK_REPEAT:
            return ParseLoopStatement();
        case TOK_IF:
            return ParseIfStatement();
        case TOK_BREAK:
        case TOK_CONTINUE:
            return ParseJumpStatement();
        case TOK_EXIT:
            return ParseExitStatement();
        case TOK_SEMICOLON:
            return new NodeStatement();
        default:
            return ParseAssignStatement();
    }
}

NodeStatement* Parser::ParseLoopStatement()
{
    StmtLoop* loop = NULL;
    switch (scan.GetToken().GetValue())
    {
        case TOK_REPEAT:
            loop = ParseUntilStatement();
        break;
        case TOK_FOR:
            loop = ParseForStatement();
        break;
        case TOK_WHILE:
            loop = ParseWhileStatement();
    }
    return loop;
}

StmtLoop* Parser::AddLoopBody(StmtLoop* loop)
{
    loop_stack.push_back(loop);
    NodeStatement* body = ParseStatement();
    if (body == NULL) Error("statement expected");
    loop_stack.pop_back();
    loop->AddBody(body);
    return loop;
}

NodeStatement* Parser::ParseBlockStatement()
{
    StmtBlock* block = new StmtBlock();
    CheckTokOrDie(TOK_BEGIN);
    while (scan.GetToken().GetValue() != TOK_END)
    {
        NodeStatement* stmt = ParseStatement();
        if (stmt != NULL) block->AddStatement(stmt);
        else Error("illeagl expression");
        if (scan.GetToken().GetValue() != TOK_SEMICOLON)
        {
            if (scan.GetToken().GetValue() != TOK_END) Error("';' expected");
        }
        else scan.NextToken();
    }
    scan.NextToken();
    return block;
}

StmtLoop* Parser::ParseForStatement()
{
    CheckTokOrDie(TOK_FOR);
    if (!scan.GetToken().IsVar()) Error("identifier expected");
    SymVar* index = (SymVar*)FindSymbolOrDie(scan.GetToken(), SYM_VAR, "identifier not found");
    if (index->GetVarType() != top_type_int) Error("integer variable expected");
    CheckNextTokOrDie(TOK_ASSIGN);
    SyntaxNode* first = GetIntExprOrDie();
    bool is_inc = (scan.GetToken().GetValue() == TOK_TO);
    if (!is_inc && scan.GetToken().GetValue() != TOK_DOWNTO) Error("'to' or 'downto' expected");
    scan.NextToken();
    SyntaxNode* second = GetIntExprOrDie();
    CheckTokOrDie(TOK_DO);
    return AddLoopBody(new StmtFor(index, first, second, is_inc));
}

StmtLoop* Parser::ParseWhileStatement()
{
    CheckTokOrDie(TOK_WHILE);
    SyntaxNode* cond = GetIntExprOrDie();
    CheckTokOrDie(TOK_DO);
    return AddLoopBody(new StmtWhile(cond));
}

StmtLoop* Parser::ParseUntilStatement()
{
    CheckTokOrDie(TOK_REPEAT);
    StmtUntil* until = new StmtUntil(NULL);
    loop_stack.push_back(until);
    StmtBlock* body = new StmtBlock();
    while (scan.GetToken().GetValue() != TOK_UNTIL)
    {
        body->AddStatement(ParseStatement());
        if (scan.GetToken().GetValue() != TOK_SEMICOLON)
        {
            if (scan.GetToken().GetValue() != TOK_UNTIL) Error("';' expected");
        }
        else scan.NextToken();
    }
    scan.NextToken();
    until->AddBody(body);
    until->AddCondition(GetIntExprOrDie());
    loop_stack.pop_back();
    return until;
}

NodeStatement* Parser::ParseIfStatement()
{
    CheckTokOrDie(TOK_IF);
    SyntaxNode* cond = GetIntExprOrDie();
    CheckTokOrDie(TOK_THEN);
    NodeStatement* then_branch = ParseStatement();
    NodeStatement* else_branch = NULL;
    if (then_branch == NULL) Error("statement expected");
    if (scan.GetToken().GetValue() == TOK_ELSE)
    {
        scan.NextToken();
        else_branch = ParseStatement();
        if (else_branch == NULL) Error("statement expected");
    }
    return new StmtIf(cond, then_branch, else_branch);
}

NodeStatement* Parser::ParseAssignStatement()
{
    SyntaxNode* left = ParseRelationalExpr();
    if (left == NULL) return NULL;
    if (scan.GetToken().GetValue() != TOK_ASSIGN)
        return new StmtExpression(left);
    Token op = scan.GetToken();
    scan.NextToken();
    SyntaxNode* right = ParseRelationalExpr();
    if (right == NULL) Error("expression expected");
    ConvertTypeOrDie(right, left->GetSymType(), op);
    if (!(left->IsLValue())) Error("l-value expected", op);
    return new StmtAssign(left, right);
}

NodeStatement* Parser::ParseJumpStatement()
{
    if (loop_stack.empty())
    {
        stringstream s;
        s << scan.GetToken().GetName();
        s << " outside loop";
        Error(s.str());
    }
    Token tok = scan.GetToken();
    scan.NextToken();
    return new StmtJump(tok, loop_stack.back());
    return NULL;
}

NodeStatement* Parser::ParseExitStatement()
{
    CheckTokOrDie(TOK_EXIT);
    AsmStrImmediate label = (current_proc == NULL) ? exit_label : current_proc->GetExitLabel();
    return new StmtExit(label);
}

const Symbol* Parser::FindSymbol(Symbol* sym)
{
    const Symbol* res = NULL;
    for (std::vector<SymTable*>::reverse_iterator it = sym_table_stack.rbegin();
         it != sym_table_stack.rend() && res == NULL; ++it)
    {
        res = (*it)->Find(sym);
    }
    return res;
}

const Symbol* Parser::FindSymbolOrDie(Symbol* sym, SymbolClass type, string msg)
{
    const Symbol* res = FindSymbol(sym);
    if (res == NULL) Error("identifier not found");
    if (!(res->GetClassName() & type)) Error(msg);
    return res;
}

const Symbol* Parser::FindSymbolOrDie(Token tok, SymbolClass type, string msg)
{
    Symbol sym(tok);
    return FindSymbolOrDie(&sym, type, msg);
}

const Symbol* Parser::FindSymbol(const Token& tok)
{
    Symbol sym(tok);
    return FindSymbol(&sym);
}

void Parser::Parse()
{
    ParseDeclarations(true);
    if (scan.GetToken().GetValue() != TOK_BEGIN) Error("'begin' expected");
    body = (StmtBlock*)ParseStatement();
    if (scan.GetToken().GetValue() != TOK_DOT) Error("'.' expected");
    if (optimization)
    {
        sym_table_stack.back()->Optimize();
        body->Optimize();
    }
}

void Parser::CheckTokOrDie(TokenValue tok_val)
{
    if (scan.GetToken().GetValue() != tok_val)
    {
        stringstream str;
        str << "'" <<  TOKEN_TO_STR[tok_val] << "' expected";
        Error(str.str());
    }
    scan.NextToken();
}

void Parser::CheckNextTokOrDie(TokenValue tok_val)
{
    scan.NextToken();
    CheckTokOrDie(tok_val);
}

SyntaxNode* Parser::GetIntExprOrDie()
{
    Token err_tok = scan.GetToken();
    SyntaxNode* res = ParseRelationalExpr();
    if (res == NULL) Error("expression expected");
    if (res->GetSymType() != top_type_int) Error("integer expression expected", err_tok);
    return res;
}

int Parser::GetIntConstValueOrDie()
{
    Token tok = GetConstTokOrDie();
    if (tok.GetType() != INT_CONST) Error("integer const expected");
    return tok.GetIntValue();
}

Token Parser::GetConstTokOrDie()
{
    bool ch_sgn = false;
    if (scan.GetToken().GetValue() == TOK_PLUS) scan.NextToken();
    else if (scan.GetToken().GetValue() == TOK_MINUS)
    {
        scan.NextToken();
        ch_sgn = true;
    }
    Token res = scan.GetToken();
    if (!res.IsConstVar()) Error("constant expected");
    if (res.IsVar())
    {
        SymVarConst* sym = (SymVarConst*)FindSymbolOrDie(res, SYM_VAR_CONST, "constant expected");
        res = sym->GetValueTok();
    }
    scan.NextToken();
    if (ch_sgn) res.ChangeSign();
    return res;
}

SyntaxNode* Parser::ParseFunctionCall(SymProc* funct_name)
{
    NodeCall* funct = new NodeCall(funct_name);
    if (scan.NextToken().GetValue() == TOK_BRACKETS_LEFT)
    {
        scan.NextToken();
        while (scan.GetToken().GetValue() != TOK_BRACKETS_RIGHT)
        {
            Token err_pos_tok = scan.GetToken();
            SyntaxNode* arg = ParseRelationalExpr();
            if (arg == NULL) Error("illegal expression");
            if (scan.GetToken().GetValue() == TOK_COMMA)
                scan.NextToken();
            else if (scan.GetToken().GetValue() != TOK_BRACKETS_RIGHT)
                Error(", expected");
            if (funct->GetCurrentArgType() == NULL) Error("too many actual parameters", err_pos_tok);
            ConvertTypeOrDie(arg, funct->GetCurrentArgType(), err_pos_tok);
            if (funct->IsCurrentArfByRef() && !arg->IsLValue()) Error("lvalue expected", err_pos_tok);
            funct->AddArg(arg);
        }
        scan.NextToken();
    }
    if (funct->GetCurrentArgType() != NULL) Error("not enough actual parameters");
    return funct;
}

SyntaxNode* Parser::ParseWriteFunctCall()
{
    bool new_line = (scan.GetToken().GetValue() == TOK_WRITELN);
    if (new_line) scan.NextToken();
    else CheckTokOrDie(TOK_WRITE);
    NodeWriteCall* write = new NodeWriteCall(new_line);
    if (scan.GetToken().GetValue() == TOK_BRACKETS_LEFT)
    {
        scan.NextToken();
        while (scan.GetToken().GetValue() != TOK_BRACKETS_RIGHT)
        {
            Token err_tok = scan.GetToken();
            SyntaxNode* arg = ParseRelationalExpr();
            if (arg == NULL) Error("illegal expression");
            if (scan.GetToken().GetValue() == TOK_COMMA)
                scan.NextToken();
            else if (scan.GetToken().GetValue() != TOK_BRACKETS_RIGHT)
                Error(", expected");
            const SymType* type = arg->GetSymType();
            if (type != top_type_int && type != top_type_real && type != top_type_str )
                Error("cant write variables of this type ", err_tok);
            write->AddArg(arg);
        }
        scan.NextToken();
    }
    return write;
}

SymVarConst* Parser::ParseConstExprOrDie(Token const_name)
{
    SyntaxNode* expr = ParseRelationalExpr();
    if (!expr->IsConst()) Error("constant expression expected");
    Token val = expr->ComputeConstExpr();
    SymVarConst* res = new SymVarConst(const_name, val, expr->GetSymType());
    delete expr;
    return res;
}

SymVarConst* Parser::ParseConstant(Token const_name)
{
    Token value = GetConstTokOrDie();
    SymType* type = NULL;
    if (value.GetType() == INT_CONST) type = top_type_int;
    else if (value.GetType() == REAL_CONST) type = top_type_real;
    else type = top_type_str;
    return new SymVarConst(const_name, value, type);
}

SyntaxNode* Parser::ParseRecordAccess(SyntaxNode* record)
{
    CheckTokOrDie(TOK_DOT);
    Token field = scan.GetToken();
    if (field.GetType() != IDENTIFIER) Error("identifier expected after '.'");
    scan.NextToken();
    if (!(record->GetSymType()->GetClassName() & SYM_TYPE_RECORD))
        Error("illegal qualifier", field);
    return new NodeRecordAccess(record, field);
}

SyntaxNode* Parser::ParseArrayAccess(SyntaxNode* array)
{
    Token err_tok = scan.GetToken();
    CheckTokOrDie(TOK_BRACKETS_SQUARE_LEFT);
    while (true)
    {
        SyntaxNode* index = GetIntExprOrDie();
        if (!(array->GetSymType()->GetClassName() & SYM_TYPE_ARRAY))
            Error("array expected before '[' token", err_tok);
        array = new NodeArrayAccess(array, index);
        if (scan.GetToken().GetValue() == TOK_COMMA) scan.NextToken();
        else if (scan.GetToken().GetValue() == TOK_BRACKETS_SQUARE_RIGHT)
        {
            if (scan.NextToken().GetValue() == TOK_BRACKETS_SQUARE_LEFT)
                scan.NextToken();
            else
                break;
        }
        else Error("illegal expression");
    }
    return array;
}

SyntaxNode* Parser::ParseFactor()
{
    SyntaxNode* left = NULL;
    TokenValue tok_val = scan.GetToken().GetValue();
    if (scan.GetToken().IsConst())
    {
        return new NodeVar(ParseConstant(scan.GetToken()));
    }
    else if (tok_val == TOK_BRACKETS_LEFT)
    {
        scan.NextToken();
        left = ParseRelationalExpr();
        if (left == NULL) Error("illegal expression");
        CheckTokOrDie(TOK_BRACKETS_RIGHT);
    }
    else if (tok_val == TOK_WRITE || tok_val == TOK_WRITELN) left = ParseWriteFunctCall();
    else {
        if (scan.GetToken().GetType() != IDENTIFIER) return NULL;
        const Symbol* sym = FindSymbolOrDie(scan.GetToken(), SymbolClass(SYM_VAR | SYM_PROC), "identifier not found");
        if (sym->GetClassName() & SYM_VAR)
        {
            left = new NodeVar((SymVar*)sym);
            scan.NextToken();
            if (sym->GetClassName() & SYM_VAR_CONST) return left;
        }
        else if (sym->GetClassName() & SYM_PROC)
        {
            left = ParseFunctionCall((SymProc*)sym);
        }
        else Error("identifier expected");
    }
    while (scan.GetToken().IsFactorOp())
    {
        if (scan.GetToken().GetValue() == TOK_DOT)
            left = ParseRecordAccess(left);
        else
            left = ParseArrayAccess(left);
    }
    return left;
}

SyntaxNode* Parser::ParseUnaryExpr()
{
    std::vector<Token> un;
    while (scan.GetToken().IsUnaryOp())
    {
        un.push_back(scan.GetToken());
        scan.NextToken();
    }
    SyntaxNode* res = ParseFactor();
    if (un.size() != 0 && res == NULL) Error("illegal expression");
    if (res != NULL)
    {
        for (std::vector<Token>::reverse_iterator it = un.rbegin(); it != un.rend(); ++it)
        {
            if (it->IsBitwiseOp()) ConvertTypeOrDie(res, top_type_int, *it);
            else CheckForBaseType(res, *it);
            res = new NodeUnaryOp(*it, res);
        }
    }
    return res;
}

SyntaxNode* Parser::ParseMultiplyingExpr()
{
    SyntaxNode* left = ParseUnaryExpr();
    if (left == NULL) return NULL;
    Token op = scan.GetToken();
    while (op.IsMultOp())
    {
        scan.NextToken();
        SyntaxNode* right = ParseUnaryExpr();
        if (right == NULL) Error("illegal expression");
        if (op.GetValue() == TOK_DIVISION)
        {
            ConvertTypeOrDie(left, top_type_real, op);
            ConvertTypeOrDie(right, top_type_real, op);
        }
        else if (op.IsBitwiseOp())
        {
            ConvertTypeOrDie(left, top_type_int, op);
            ConvertTypeOrDie(right, top_type_int, op);
        }
        else
            ConvertToBaseTypeOrDie(left, right, op);
        left = new NodeBinaryOp(op, left, right);
        op = scan.GetToken();
    }
    return left;
}

SyntaxNode* Parser::ParseAddingExpr()
{
    SyntaxNode* left = ParseMultiplyingExpr();
    if (left == NULL) return NULL;
    Token op = scan.GetToken();
    while (op.IsAddingOp())
    {
        scan.NextToken();
        SyntaxNode* right = ParseMultiplyingExpr();
        if (right == NULL) Error("expression expected");
        if (op.IsBitwiseOp())
        {
            ConvertTypeOrDie(left, top_type_int, op);
            ConvertTypeOrDie(right, top_type_int, op);
        }
        else
            ConvertToBaseTypeOrDie(left, right, op);
        left = new NodeBinaryOp(op, left, right);
        op = scan.GetToken();
    }
    return left;
}

SyntaxNode* Parser::ParseRelationalExpr()
{
    SyntaxNode* left = ParseAddingExpr();
    if (left == NULL) return NULL;
    Token op = scan.GetToken();
    while (op.IsRelationalOp())
    {
        scan.NextToken();
        SyntaxNode* right = ParseAddingExpr();
        if (right == NULL) Error("expression expected");
        ConvertToBaseTypeOrDie(left, right, op);
        left = new NodeBinaryOp(op, left, right);
        op = scan.GetToken();
    }
    return left;
}

void Parser::Error(string msg)
{
    Error(msg, scan.GetToken());
}

void Parser::Error(string msg, Token err_pos_tok)
{
    stringstream s;
    s << err_pos_tok.GetLine() << ':' << err_pos_tok.GetPos()
      << " ERROR at '" << err_pos_tok.GetName() << "': " << msg;
    throw( CompilerException(s.str()) );
}
