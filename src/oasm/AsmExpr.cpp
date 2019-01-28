/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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

#include "AsmExpr.h"
#include "ppDefine.h"
#include "ppExpr.h"
#include "Errors.h"
#include "Section.h"
#include "AsmLexer.h"
#include <exception>
#include <fstream>
#include <limits.h>
#include <stdexcept>

KeywordHash AsmExpr::hash;
bool AsmExpr::initted;
std::string AsmExpr::currentLabel;
Section* AsmExpr::section;
std::map<std::string, AsmExprNode*> AsmExpr::equs;

enum
{
    dollars = 300,
    dollarsdollars,
    sdivide,
    smod,
    SEG,
    WRT
};
void AsmExpr::ReInit()
{
    currentLabel = "";
    section = nullptr;
    equs.clear();
}
void AsmExpr::InitHash()
{
    if (!initted)
    {
        initted = true;
        hash["("] = openpa;
        hash[")"] = closepa;
        hash["+"] = ::plus;
        hash["-"] = ::minus;
        hash["!"] = lnot;
        hash["~"] = bcompl;
        hash["*"] = star;
        hash["/"] = divide;
        hash["/-"] = sdivide;
        hash["%"] = mod;
        hash["%-"] = smod;
        hash["<<"] = leftshift;
        hash[">>"] = rightshift;
        hash[">"] = gt;
        hash["<"] = lt;
        hash[">="] = geq;
        hash["<="] = leq;
        hash["=="] = eq;
        hash["!="] = ne;
        hash["|"] = bor;
        hash["&"] = band;
        hash["^"] = bxor;
        hash["||"] = lor;
        hash["&&"] = land;
        hash["$"] = dollars;
        hash["$$"] = dollarsdollars;
        hash["seg"] = SEG;
        hash["wrt"] = WRT;
    }
}
AsmExprNode* AsmExpr::Build(std::string& line)
{
    tokenizer = new Tokenizer(line, &hash);
    token = tokenizer->Next();
    if (!token)
        return 0;
    AsmExprNode* rv = logicalor();
    if (!token->IsEnd())
        line = token->GetChars() + tokenizer->GetString();
    else
        line = "";
    delete tokenizer;
    return rv;
}
AsmExprNode* AsmExpr::ConvertToBased(AsmExprNode* n, int pc)
{
    if (section)
    {
        if (n->GetLeft())
            n->SetLeft(ConvertToBased(n->GetLeft(), pc));
        if (n->GetRight())
            n->SetRight(ConvertToBased(n->GetRight(), pc));
        if (n->GetType() == AsmExprNode::PC)
        {
            delete n;
            return new AsmExprNode(section, pc);
        }
        else if (n->GetType() == AsmExprNode::SECTBASE)
        {
            delete n;
            return new AsmExprNode(section, 0);
        }
    }
    return n;
}
AsmExprNode* AsmExpr::Eval(AsmExprNode* n, int pc)
{
    AsmExprNode* rv = nullptr;
    FPF fv;
    AsmExprNode *xleft = 0, *xright = 0;
    if (n->GetLeft())
        xleft = Eval(n->GetLeft(), pc);
    if (n->GetRight())
        xright = Eval(n->GetRight(), pc);
    switch (n->GetType())
    {
        case AsmExprNode::IVAL:
        case AsmExprNode::FVAL:
        default:
            rv = new AsmExprNode(*n);
            break;
        case AsmExprNode::LABEL:
        {
            AsmExprNode* num = GetEqu(n->label);
            if (num)
            {
                rv = Eval(num, pc);
            }
            else if (pc != -1)
            {
                auto it = section->Lookup(n->label);
                if (it != section->GetLabels().end())
                {
                    rv = new AsmExprNode(it->second);
                }
                else
                {
                    rv = new AsmExprNode(*n);
                }
            }
            else
            {
                rv = new AsmExprNode(*n);
            }
            break;
        }
        case AsmExprNode::PC:
            if (pc == -1)
                rv = new AsmExprNode(*n);
            else
                rv = new AsmExprNode(pc);
            break;
        case AsmExprNode::SECTBASE:
            rv = new AsmExprNode(0);
            break;
        case AsmExprNode::BASED:
            rv = new AsmExprNode(n->GetSection(), n->ival);
            break;
        case AsmExprNode::ADD:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = new AsmExprNode(xleft->ival + xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    fv += xright->fval;
                    rv = new AsmExprNode(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    fv += xleft->fval;
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->FVAL + xright->FVAL;
                    rv = new AsmExprNode(fv);
                    break;
                default:
                    rv = new AsmExprNode(*n);
                    break;
            }
            break;
        case AsmExprNode::SUB:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = new AsmExprNode(xleft->ival - xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    fv -= xright->fval;
                    rv = new AsmExprNode(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    fv = xleft->fval - fv;
                    rv = new AsmExprNode(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->fval - xright->fval;
                    rv = new AsmExprNode(fv);
                    break;
                default:
                    rv = new AsmExprNode(*n);
                    break;
            }
            break;
        case AsmExprNode::MUL:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = new AsmExprNode(xleft->ival * xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    fv *= xright->fval;
                    rv = new AsmExprNode(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    fv *= xleft->fval;
                    rv = new AsmExprNode(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->fval * xright->fval;
                    rv = new AsmExprNode(fv);
                    break;
                default:
                    rv = new AsmExprNode(*n);
                    break;
            }
            break;
        case AsmExprNode::DIV:
        case AsmExprNode::SDIV:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    if (xright->ival == 0)
                        rv = new AsmExprNode(INT_MAX);
                    else if (n->GetType() == AsmExprNode::SDIV)
                        rv = new AsmExprNode(xleft->ival / xright->ival);
                    else
                        rv = new AsmExprNode((unsigned)xleft->ival / (unsigned)xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    fv /= xright->fval;
                    rv = new AsmExprNode(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    fv = xleft->fval / fv;
                    rv = new AsmExprNode(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->fval / xright->fval;
                    rv = new AsmExprNode(fv);
                    break;
                default:
                    rv = new AsmExprNode(*n);
                    break;
            }
            break;
        case AsmExprNode::GT:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = new AsmExprNode(xleft->ival > xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    rv = new AsmExprNode(fv > xright->fval);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    rv = new AsmExprNode(xleft->fval > fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    rv = new AsmExprNode(xleft->fval > xright->fval);
                    break;
                default:
                    rv = new AsmExprNode(*n);
                    break;
            }
            break;
        case AsmExprNode::LT:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = new AsmExprNode(xleft->ival < xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    rv = new AsmExprNode(fv < xright->fval);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    rv = new AsmExprNode(xleft->fval < fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    rv = new AsmExprNode(xleft->fval < xright->fval);
                    break;
                default:
                    rv = new AsmExprNode(*n);
                    break;
            }
            break;
        case AsmExprNode::GE:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = new AsmExprNode(xleft->ival >= xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    rv = new AsmExprNode(fv >= xright->fval);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    rv = new AsmExprNode(xleft->fval >= fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    rv = new AsmExprNode(xleft->fval >= xright->fval);
                    break;
                default:
                    rv = new AsmExprNode(*n);
                    break;
            }
            break;
        case AsmExprNode::LE:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = new AsmExprNode(xleft->ival <= xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    rv = new AsmExprNode(fv <= xright->fval);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    rv = new AsmExprNode(xleft->fval <= fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    rv = new AsmExprNode(xleft->fval <= xright->fval);
                    break;
                default:
                    rv = new AsmExprNode(*n);
                    break;
            }
            break;
        case AsmExprNode::EQ:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = new AsmExprNode(xleft->ival == xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    rv = new AsmExprNode(fv == xright->fval);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    rv = new AsmExprNode(xleft->fval == fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    rv = new AsmExprNode(xleft->fval == xright->fval);
                    break;
                default:
                    rv = new AsmExprNode(*n);
                    break;
            }
            break;
        case AsmExprNode::NE:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = new AsmExprNode(xleft->ival != xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    rv = new AsmExprNode(fv != xright->fval);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    rv = new AsmExprNode(xleft->fval != fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    rv = new AsmExprNode(xleft->fval != xright->fval);
                    break;
                default:
                    rv = new AsmExprNode(*n);
                    break;
            }
            break;
        case AsmExprNode::NEG:
            if (xleft->GetType() == AsmExprNode::IVAL)
            {
                rv = new AsmExprNode(-xleft->ival);
            }
            else if (xleft->GetType() == AsmExprNode::FVAL)
            {
                xleft->fval.Negate();
                rv = xleft;
                xleft = nullptr;
            }
            else
            {
                rv = new AsmExprNode(*n);
            }
            break;
        case AsmExprNode::CMPL:
            if (xleft->GetType() == AsmExprNode::IVAL)
            {
                rv = new AsmExprNode(~xleft->ival);
            }
            else
            {
                rv = new AsmExprNode(*n);
            }
            break;
        case AsmExprNode::MOD:
        case AsmExprNode::SMOD:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                if (xright->ival == 0)
                    rv = new AsmExprNode(INT_MAX);
                else if (n->GetType() == AsmExprNode::SMOD)
                {
                    rv = new AsmExprNode(xleft->ival % xright->ival);
                }
                else
                {
                    rv = new AsmExprNode((unsigned)xleft->ival % (unsigned)xright->ival);
                }
            }
            else
            {
                rv = new AsmExprNode(*n);
            }
            break;
        case AsmExprNode::LSHIFT:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = new AsmExprNode(xleft->ival << xright->ival);
            }
            else
            {
                rv = new AsmExprNode(*n);
            }
            break;
        case AsmExprNode::RSHIFT:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = new AsmExprNode((unsigned)xleft->ival >> xright->ival);
            }
            else
            {
                rv = new AsmExprNode(*n);
            }
            break;
        case AsmExprNode::OR:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = new AsmExprNode(xleft->ival | xright->ival);
            }
            else
            {
                rv = new AsmExprNode(*n);
            }
            break;
        case AsmExprNode::XOR:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = new AsmExprNode(xleft->ival ^ xright->ival);
            }
            else
            {
                rv = new AsmExprNode(*n);
            }
            break;
        case AsmExprNode::AND:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = new AsmExprNode(xleft->ival & xright->ival);
            }
            else
            {
                rv = new AsmExprNode(*n);
            }
            break;
        case AsmExprNode::LAND:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = new AsmExprNode(xleft->ival && xright->ival);
            }
            else
            {
                rv = new AsmExprNode(*n);
            }
            break;
        case AsmExprNode::LOR:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = new AsmExprNode(xleft->ival || xright->ival);
            }
            else
            {
                rv = new AsmExprNode(*n);
            }
            break;
    }
    if (rv->GetLeft())
    {
        rv->SetLeft(xleft);
        rv->SetRight(xright);
    }
    else
    {
        if (xleft)
            delete xleft;
        if (xright)
            delete xright;
    }
    return rv;
}
bool AsmExprNode::IsAbsoluteInternal(int& n)
{
    bool rv = true;
    ;
    if (type == LABEL)
    {
        AsmExprNode* num = AsmExpr::GetEqu(label);
        if (!num)
        {
            auto it = AsmExpr::GetSection()->Lookup(label);
            if (it == AsmExpr::GetSection()->GetLabels().end())
            {
                rv = false;
            }
            else
            {
                n++;
            }
        }
        else
            rv = num->IsAbsoluteInternal(n);
    }
    else if (type == PC)
    {
        n++;
    }
    else if (type == SECTBASE)
    {
        n++;
    }
    else if (type == BASED)
    {
        rv = false;
    }
    else if (!left && !right && type != IVAL && type != FVAL)
    {
        rv = false;
    }
    else
    {
        int n1 = 0, n2 = 0;
        if (left)
            rv &= left->IsAbsoluteInternal(n);
        if (right)
            rv &= right->IsAbsoluteInternal(n2);
        if (type == ADD)
            n += n1 + n2;
        else if (type == SUB)
            n += n1 - n2;
        else if (type == NEG)
            n -= n1;
        else if (n1 != 0 || n2 != 0)
            rv = false;
    }
    return rv;
}
bool AsmExprNode::IsAbsolute()
{
    int n = 0;
    bool rv = IsAbsoluteInternal(n);
    return rv && n == 0;
}
AsmExprNode* AsmExpr::primary()
{
    AsmExprNode* rv = 0;
    if (token->GetKeyword() == dollars)
    {
        rv = new AsmExprNode(AsmExprNode::PC);
        token = tokenizer->Next();
    }
    else if (token->GetKeyword() == dollarsdollars)
    {
        rv = new AsmExprNode(AsmExprNode::SECTBASE);
        token = tokenizer->Next();
    }
    else if (token->GetKeyword() == openpa)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            rv = logicalor();
            if (token->GetKeyword() == closepa)
                token = tokenizer->Next();
            else
                throw new std::runtime_error("Expected ')'");
        }
    }
    else if (token->IsIdentifier())
    {
        std::string line = token->GetChars();
        if (line[0] == '%')
            line.erase(0, 1);
        if (line[0] == '.' && line.substr(0, 3) != "..@")
            line = currentLabel + line;
        rv = new AsmExprNode(line);
        token = tokenizer->Next();
    }
    else if (token->IsNumeric())
    {
        if (token->IsFloating())
        {
            rv = new AsmExprNode(*(token->GetFloat()));
        }
        else
        {
            rv = new AsmExprNode(token->GetInteger());
        }
        token = tokenizer->Next();
    }
    else if (token->IsCharacter())
    {
        rv = new AsmExprNode(token->GetInteger());
        token = tokenizer->Next();
    }
    else if (token->IsString())
    {
        int num = 0;
        std::wstring aa = token->GetString();
        for (int i = 0; i < 4 && i < aa.size(); i++)
        {
            num += aa[i] << (i * 8);
        }
        rv = new AsmExprNode(num);
        token = tokenizer->Next();
    }
    else
    {
        throw new std::runtime_error("Constant value expected");
        rv = new AsmExprNode(0);
    }
    return rv;
}
AsmExprNode* AsmExpr::unary()
{
    if (!token->IsEnd())
    {
        int kw = token->GetKeyword();
        if (kw == ::plus || kw == ::minus || kw == lnot || kw == bcompl || kw == SEG)
        {
            token = tokenizer->Next();
            if (!token->IsEnd())
            {
                AsmExprNode* val1 = unary();
                switch (kw)
                {
                    case SEG:
                        val1 = new AsmExprNode(AsmExprNode::DIV, val1, new AsmExprNode(16));
                        break;
                    case ::minus:
                        val1 = new AsmExprNode(AsmExprNode::NEG, val1);
                        break;
                    case ::plus:
                        break;
                    case lnot:
                        val1 = new AsmExprNode(AsmExprNode::NOT, val1);
                        break;
                    case bcompl:
                        val1 = new AsmExprNode(AsmExprNode::CMPL, val1);
                        break;
                }
                return val1;
            }
        }
        else
        {
            return primary();
        }
    }
    throw new std::runtime_error("Syntax error in constant expression");
    return 0;
}
AsmExprNode* AsmExpr::multiply()
{
    AsmExprNode* val1 = unary();
    int kw = token->GetKeyword();
    while (kw == star || kw == divide || kw == mod || kw == sdivide || kw == smod)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            AsmExprNode* val2 = unary();
            switch (kw)
            {
                case star:
                    val1 = new AsmExprNode(AsmExprNode::MUL, val1, val2);
                    break;
                case divide:
                    val1 = new AsmExprNode(AsmExprNode::DIV, val1, val2);
                    break;
                case sdivide:
                    val1 = new AsmExprNode(AsmExprNode::SDIV, val1, val2);
                    break;
                case mod:
                    val1 = new AsmExprNode(AsmExprNode::MOD, val1, val2);
                    break;
                case smod:
                    val1 = new AsmExprNode(AsmExprNode::SMOD, val1, val2);
                    break;
            }
        }
        kw = token->GetKeyword();
    }
    return val1;
}
AsmExprNode* AsmExpr::add()
{
    AsmExprNode* val1 = multiply();
    int kw = token->GetKeyword();
    while (kw == ::plus || kw == ::minus || kw == WRT)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            AsmExprNode* val2 = multiply();
            switch (kw)
            {
                case ::plus:
                    val1 = new AsmExprNode(AsmExprNode::ADD, val1, val2);
                    break;
                case WRT:
                case ::minus:
                    val1 = new AsmExprNode(AsmExprNode::SUB, val1, val2);
                    break;
            }
        }
        kw = token->GetKeyword();
    }
    return val1;
}
AsmExprNode* AsmExpr::shift()
{
    AsmExprNode* val1 = add();
    int kw = token->GetKeyword();
    while (kw == leftshift || kw == rightshift)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            AsmExprNode* val2 = add();
            switch (kw)
            {
                case leftshift:
                    val1 = new AsmExprNode(AsmExprNode::LSHIFT, val1, val2);
                    break;
                case rightshift:
                    val1 = new AsmExprNode(AsmExprNode::RSHIFT, val1, val2);
                    break;
            }
        }
        kw = token->GetKeyword();
    }
    return val1;
}
AsmExprNode* AsmExpr::relation()
{
    AsmExprNode* val1 = shift();
    int kw = token->GetKeyword();
    while (kw == gt || kw == lt || kw == geq || kw == leq)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            AsmExprNode* val2 = shift();
            switch (kw)
            {
                case gt:
                    val1 = new AsmExprNode(AsmExprNode::GT, val1, val2);
                    break;
                case lt:
                    val1 = new AsmExprNode(AsmExprNode::LT, val1, val2);
                    break;
                case geq:
                    val1 = new AsmExprNode(AsmExprNode::GE, val1, val2);
                    break;
                case leq:
                    val1 = new AsmExprNode(AsmExprNode::LE, val1, val2);
                    break;
            }
        }
        kw = token->GetKeyword();
    }
    return val1;
}
AsmExprNode* AsmExpr::equal()
{
    AsmExprNode* val1 = relation();
    int kw = token->GetKeyword();
    while (kw == eq || kw == ne)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            AsmExprNode* val2 = shift();
            switch (kw)
            {
                case eq:
                    val1 = new AsmExprNode(AsmExprNode::EQ, val1, val2);
                    break;
                case ne:
                    val1 = new AsmExprNode(AsmExprNode::NE, val1, val2);
                    break;
            }
        }
        kw = token->GetKeyword();
    }
    return val1;
}
AsmExprNode* AsmExpr::and_()
{
    AsmExprNode* val1 = equal();
    while (!token->IsEnd() && token->GetKeyword() == band)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            AsmExprNode* val2 = equal();
            val1 = new AsmExprNode(AsmExprNode::AND, val1, val2);
        }
    }
    return val1;
}
AsmExprNode* AsmExpr::xor_()
{
    AsmExprNode* val1 = and_();
    while (!token->IsEnd() && token->GetKeyword() == bxor)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            AsmExprNode* val2 = and_();
            val1 = new AsmExprNode(AsmExprNode::XOR, val1, val2);
        }
    }
    return val1;
}
AsmExprNode* AsmExpr::or_()
{
    AsmExprNode* val1 = xor_();
    while (!token->IsEnd() && token->GetKeyword() == bor)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            AsmExprNode* val2 = xor_();
            val1 = new AsmExprNode(AsmExprNode::OR, val1, val2);
        }
    }
    return val1;
}
AsmExprNode* AsmExpr::logicaland()
{
    AsmExprNode* val1 = or_();
    while (!token->IsEnd() && token->GetKeyword() == land)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            AsmExprNode* val2 = or_();
            val1 = new AsmExprNode(AsmExprNode::LAND, val1, val2);
        }
    }
    return val1;
}
AsmExprNode* AsmExpr::logicalor()
{
    AsmExprNode* val1 = logicaland();
    while (!token->IsEnd() && token->GetKeyword() == lor)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            AsmExprNode* val2 = logicaland();
            val1 = new AsmExprNode(AsmExprNode::LOR, val1, val2);
        }
    }
    return val1;
}
