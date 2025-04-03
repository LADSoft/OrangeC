/* Software License Agreement
 *
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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
#include <climits>
#include <stdexcept>

KeywordHash AsmExpr::hash = {
    {"(", kw::openpa},      {")", kw::closepa},
    {"+", kw::plus},        {"-", kw::minus},
    {"!", kw::lnot},        {"~", kw::bcompl},
    {"*", kw::star},        {"/", kw::divide},
    {"/-", kw::sdivide},    {"%", kw::mod},
    {"%-", kw::smod},       {"<<", kw::leftshift},
    {">>", kw::rightshift}, {">", kw::gt},
    {"<", kw::lt},          {">=", kw::geq},
    {"<=", kw::leq},        {"==", kw::eq},
    {"!=", kw::ne},         {"|", kw::bor},
    {"&", kw::band},        {"^", kw::bxor},
    {"||", kw::lor},        {"&&", kw::land},
    {"$", kw::dollars},     {"$$", kw::dollarsdollars},
    {"seg", kw::SEG},       {"wrt", kw::WRT},

};

std::string AsmExpr::currentLabel;
std::shared_ptr<Section> AsmExpr::section;
std::unordered_map<std::string, std::shared_ptr<AsmExprNode>> AsmExpr::equs;

void AsmExpr::ReInit()
{
    currentLabel = "";
    section = nullptr;
    equs.clear();
}
std::shared_ptr<AsmExprNode> AsmExpr::Build(std::string& line)
{
    tokenizer = std::make_unique<Tokenizer>(line, &hash);
    token = tokenizer->Next();
    if (!token)
        return 0;
    std::shared_ptr<AsmExprNode> rv = logicalor();
    if (!token->IsEnd())
        line = token->GetChars() + tokenizer->GetString();
    else
        line = "";

    tokenizer.release();
    return rv;
}
std::shared_ptr<AsmExprNode> AsmExpr::ConvertToBased(std::shared_ptr<AsmExprNode>& n, int pc)
{
    if (section)
    {
        if (n->GetLeft())
        {
            auto expr = n->GetLeft();
            n->SetLeft(ConvertToBased(expr, pc));
        }
        if (n->GetRight())
        {
            auto expr = n->GetRight();
            n->SetRight(ConvertToBased(expr, pc));
        }
        if (n->GetType() == AsmExprNode::PC)
        {
            return std::make_shared<AsmExprNode>(section, pc);
        }
        else if (n->GetType() == AsmExprNode::SECTBASE)
        {
            return std::make_shared<AsmExprNode>(section, 0);
        }
    }
    return n;
}
std::shared_ptr<AsmExprNode> AsmExpr::Eval(std::shared_ptr<AsmExprNode> n, int pc)
{
    std::shared_ptr<AsmExprNode> rv = nullptr;
    FPF fv;
    std::shared_ptr<AsmExprNode> xleft, xright;
    if (n->GetLeft())
        xleft = Eval(n->GetLeft(), pc);
    if (n->GetRight())
        xright = Eval(n->GetRight(), pc);
    switch (n->GetType())
    {
        case AsmExprNode::IVAL:
        case AsmExprNode::FVAL:
        default:
            rv = n;
            break;
        case AsmExprNode::LABEL: {
            std::shared_ptr<AsmExprNode> num = GetEqu(n->label);
            if (num)
            {
                rv = Eval(num, pc);
            }
            else if (pc != -1)
            {
                auto it = section->Lookup(n->label);
                if (it != section->GetLabels().end())
                {
                    rv = std::make_shared<AsmExprNode>(it->second);
                }
                else
                {
                    rv = n;
                }
            }
            else
            {
                rv = n;
            }
            break;
        }
        case AsmExprNode::PC:
            if (pc == -1)
                rv = n;
            else
                rv = std::make_shared<AsmExprNode>(pc);
            break;
        case AsmExprNode::SECTBASE:
            rv = std::make_shared<AsmExprNode>(0);
            break;
        case AsmExprNode::BASED: {
            auto sect = n->GetSection();
            rv = std::make_shared<AsmExprNode>(sect, n->ival);
        }
        break;
        case AsmExprNode::ADD:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->ival + xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    fv += xright->fval;
                    rv = std::make_shared<AsmExprNode>(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    fv += xleft->fval;
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->fval + xright->fval;
                    rv = std::make_shared<AsmExprNode>(fv);
                    break;
                default:
                    rv = n;
                    break;
            }
            break;
        case AsmExprNode::SUB:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->ival - xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    fv -= xright->fval;
                    rv = std::make_shared<AsmExprNode>(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    fv = xleft->fval - fv;
                    rv = std::make_shared<AsmExprNode>(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->fval - xright->fval;
                    rv = std::make_shared<AsmExprNode>(fv);
                    break;
                default:
                    rv = n;
                    break;
            }
            break;
        case AsmExprNode::MUL:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->ival * xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    fv *= xright->fval;
                    rv = std::make_shared<AsmExprNode>(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    fv *= xleft->fval;
                    rv = std::make_shared<AsmExprNode>(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->fval * xright->fval;
                    rv = std::make_shared<AsmExprNode>(fv);
                    break;
                default:
                    rv = n;
                    break;
            }
            break;
        case AsmExprNode::DIV:
        case AsmExprNode::SDIV:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    if (xright->ival == 0)
                        rv = std::make_shared<AsmExprNode>(INT_MAX);
                    else if (n->GetType() == AsmExprNode::SDIV)
                        rv = std::make_shared<AsmExprNode>(xleft->ival / xright->ival);
                    else
                        rv = std::make_shared<AsmExprNode>((unsigned)xleft->ival / (unsigned)xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    fv /= xright->fval;
                    rv = std::make_shared<AsmExprNode>(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    fv = xleft->fval / fv;
                    rv = std::make_shared<AsmExprNode>(fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->fval / xright->fval;
                    rv = std::make_shared<AsmExprNode>(fv);
                    break;
                default:
                    rv = n;
                    break;
            }
            break;
        case AsmExprNode::GT:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->ival > xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    rv = std::make_shared<AsmExprNode>(fv > xright->fval);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    rv = std::make_shared<AsmExprNode>(xleft->fval > fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->fval > xright->fval);
                    break;
                default:
                    rv = n;
                    break;
            }
            break;
        case AsmExprNode::LT:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->ival < xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    rv = std::make_shared<AsmExprNode>(fv < xright->fval);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    rv = std::make_shared<AsmExprNode>(xleft->fval < fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->fval < xright->fval);
                    break;
                default:
                    rv = n;
                    break;
            }
            break;
        case AsmExprNode::GE:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->ival >= xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    rv = std::make_shared<AsmExprNode>(fv >= xright->fval);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    rv = std::make_shared<AsmExprNode>(xleft->fval >= fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->fval >= xright->fval);
                    break;
                default:
                    rv = n;
                    break;
            }
            break;
        case AsmExprNode::LE:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->ival <= xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    rv = std::make_shared<AsmExprNode>(fv <= xright->fval);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    rv = std::make_shared<AsmExprNode>(xleft->fval <= fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->fval <= xright->fval);
                    break;
                default:
                    rv = n;
                    break;
            }
            break;
        case AsmExprNode::EQ:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->ival == xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    rv = std::make_shared<AsmExprNode>(fv == xright->fval);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    rv = std::make_shared<AsmExprNode>(xleft->fval == fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->fval == xright->fval);
                    break;
                default:
                    rv = n;
                    break;
            }
            break;
        case AsmExprNode::NE:
            switch ((xleft->GetType() << 8) + xright->GetType())
            {
                case (AsmExprNode::IVAL << 8) + AsmExprNode::IVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->ival != xright->ival);
                    break;
                case (AsmExprNode::IVAL << 8) + AsmExprNode::FVAL:
                    fv = xleft->ival;
                    rv = std::make_shared<AsmExprNode>(fv != xright->fval);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::IVAL:
                    fv = xright->ival;
                    rv = std::make_shared<AsmExprNode>(xleft->fval != fv);
                    break;
                case (AsmExprNode::FVAL << 8) + AsmExprNode::FVAL:
                    rv = std::make_shared<AsmExprNode>(xleft->fval != xright->fval);
                    break;
                default:
                    rv = n;
                    break;
            }
            break;
        case AsmExprNode::NEG:
            if (xleft->GetType() == AsmExprNode::IVAL)
            {
                rv = std::make_shared<AsmExprNode>(-xleft->ival);
            }
            else if (xleft->GetType() == AsmExprNode::FVAL)
            {
                rv = std::make_shared<AsmExprNode>(xleft->fval);
                rv->fval.Negate();
            }
            else
            {
                rv = n;
            }
            break;
        case AsmExprNode::CMPL:
            if (xleft->GetType() == AsmExprNode::IVAL)
            {
                rv = std::make_shared<AsmExprNode>(~xleft->ival);
            }
            else
            {
                rv = n;
            }
            break;
        case AsmExprNode::MOD:
        case AsmExprNode::SMOD:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                if (xright->ival == 0)
                    rv = std::make_shared<AsmExprNode>(INT_MAX);
                else if (n->GetType() == AsmExprNode::SMOD)
                {
                    rv = std::make_shared<AsmExprNode>(xleft->ival % xright->ival);
                }
                else
                {
                    rv = std::make_shared<AsmExprNode>((unsigned)xleft->ival % (unsigned)xright->ival);
                }
            }
            else
            {
                rv = n;
            }
            break;
        case AsmExprNode::LSHIFT:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = std::make_shared<AsmExprNode>(xleft->ival << xright->ival);
            }
            else
            {
                rv = n;
            }
            break;
        case AsmExprNode::RSHIFT:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = std::make_shared<AsmExprNode>((unsigned)xleft->ival >> xright->ival);
            }
            else
            {
                rv = n;
            }
            break;
        case AsmExprNode::OR:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = std::make_shared<AsmExprNode>(xleft->ival | xright->ival);
            }
            else
            {
                rv = n;
            }
            break;
        case AsmExprNode::XOR:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = std::make_shared<AsmExprNode>(xleft->ival ^ xright->ival);
            }
            else
            {
                rv = n;
            }
            break;
        case AsmExprNode::AND:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = std::make_shared<AsmExprNode>(xleft->ival & xright->ival);
            }
            else
            {
                rv = n;
            }
            break;
        case AsmExprNode::LAND:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = std::make_shared<AsmExprNode>(xleft->ival && xright->ival);
            }
            else
            {
                rv = n;
            }
            break;
        case AsmExprNode::LOR:
            if (xleft->GetType() == AsmExprNode::IVAL && xright->GetType() == AsmExprNode::IVAL)
            {
                rv = std::make_shared<AsmExprNode>(xleft->ival || xright->ival);
            }
            else
            {
                rv = n;
            }
            break;
    }
    if (rv && rv->GetLeft())
    {
        rv->SetLeft(xleft);
        rv->SetRight(xright);
    }
    return rv;
}
bool AsmExprNode::IsAbsoluteInternal(int& n)
{
    bool rv = true;
    ;
    if (type == LABEL)
    {
        std::shared_ptr<AsmExprNode> num = AsmExpr::GetEqu(label);
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
std::shared_ptr<AsmExprNode> AsmExpr::primary()
{
    std::shared_ptr<AsmExprNode> rv = 0;
    if (token->GetKeyword() == kw::dollars)
    {
        rv = std::make_shared<AsmExprNode>(AsmExprNode::PC);
        token = tokenizer->Next();
    }
    else if (token->GetKeyword() == kw::dollarsdollars)
    {
        rv = std::make_shared<AsmExprNode>(AsmExprNode::SECTBASE);
        token = tokenizer->Next();
    }
    else if (token->GetKeyword() == kw::openpa)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            rv = logicalor();
            if (token->GetKeyword() == kw::closepa)
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
        rv = std::make_shared<AsmExprNode>(line);
        token = tokenizer->Next();
    }
    else if (token->IsNumeric())
    {
        if (token->IsFloating())
        {
            rv = std::make_shared<AsmExprNode>(*(token->GetFloat()));
        }
        else
        {
            rv = std::make_shared<AsmExprNode>(token->GetInteger());
        }
        token = tokenizer->Next();
    }
    else if (token->IsCharacter())
    {
        rv = std::make_shared<AsmExprNode>(token->GetInteger());
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
        rv = std::make_shared<AsmExprNode>(num);
        token = tokenizer->Next();
    }
    else
    {
        throw new std::runtime_error("Constant value expected");
    }
    return rv;
}
std::shared_ptr<AsmExprNode> AsmExpr::unary()
{
    if (!token->IsEnd())
    {
        kw keyWord = token->GetKeyword();
        if (keyWord == kw::plus || keyWord == kw::minus || keyWord == kw::lnot || keyWord == kw::bcompl || keyWord == kw::SEG)
        {
            token = tokenizer->Next();
            if (!token->IsEnd())
            {
                std::shared_ptr<AsmExprNode> val1 = unary();
                switch (keyWord)
                {
                    case kw::SEG:
                        val1 = std::make_shared<AsmExprNode>(AsmExprNode::DIV, val1, std::make_shared<AsmExprNode>(16));
                        break;
                    case kw::minus:
                        val1 = std::make_shared<AsmExprNode>(AsmExprNode::NEG, val1);
                        break;
                    case kw::plus:
                        break;
                    case kw::lnot:
                        val1 = std::make_shared<AsmExprNode>(AsmExprNode::NOT, val1);
                        break;
                    case kw::bcompl:
                        val1 = std::make_shared<AsmExprNode>(AsmExprNode::CMPL, val1);
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
std::shared_ptr<AsmExprNode> AsmExpr::multiply()
{
    std::shared_ptr<AsmExprNode> val1 = unary();
    kw keyWord = token->GetKeyword();
    while (keyWord == kw::star || keyWord == kw::divide || keyWord == kw::mod || keyWord == kw::sdivide || keyWord == kw::smod)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            std::shared_ptr<AsmExprNode> val2 = unary();
            switch (keyWord)
            {
                case kw::star:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::MUL, val1, val2);
                    break;
                case kw::divide:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::DIV, val1, val2);
                    break;
                case kw::sdivide:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::SDIV, val1, val2);
                    break;
                case kw::mod:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::MOD, val1, val2);
                    break;
                case kw::smod:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::SMOD, val1, val2);
                    break;
            }
        }
        keyWord = token->GetKeyword();
    }
    return val1;
}
std::shared_ptr<AsmExprNode> AsmExpr::add()
{
    std::shared_ptr<AsmExprNode> val1 = multiply();
    kw keyWord = token->GetKeyword();
    while (keyWord == kw::plus || keyWord == kw::minus || keyWord == kw::WRT)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            std::shared_ptr<AsmExprNode> val2 = multiply();
            switch (keyWord)
            {
                case kw::plus:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::ADD, val1, val2);
                    break;
                case kw::WRT:
                case kw::minus:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::SUB, val1, val2);
                    break;
            }
        }
        keyWord = token->GetKeyword();
    }
    return val1;
}
std::shared_ptr<AsmExprNode> AsmExpr::shift()
{
    std::shared_ptr<AsmExprNode> val1 = add();
    kw keyWord = token->GetKeyword();
    while (keyWord == kw::leftshift || keyWord == kw::rightshift)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            std::shared_ptr<AsmExprNode> val2 = add();
            switch (keyWord)
            {
                case kw::leftshift:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::LSHIFT, val1, val2);
                    break;
                case kw::rightshift:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::RSHIFT, val1, val2);
                    break;
            }
        }
        keyWord = token->GetKeyword();
    }
    return val1;
}
std::shared_ptr<AsmExprNode> AsmExpr::relation()
{
    std::shared_ptr<AsmExprNode> val1 = shift();
    kw keyWord = token->GetKeyword();
    while (keyWord == kw::gt || keyWord == kw::lt || keyWord == kw::geq || keyWord == kw::leq)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            std::shared_ptr<AsmExprNode> val2 = shift();
            switch (keyWord)
            {
                case kw::gt:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::GT, val1, val2);
                    break;
                case kw::lt:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::LT, val1, val2);
                    break;
                case kw::geq:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::GE, val1, val2);
                    break;
                case kw::leq:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::LE, val1, val2);
                    break;
            }
        }
        keyWord = token->GetKeyword();
    }
    return val1;
}
std::shared_ptr<AsmExprNode> AsmExpr::equal()
{
    std::shared_ptr<AsmExprNode> val1 = relation();
    kw keyWord = token->GetKeyword();
    while (keyWord == kw::eq || keyWord == kw::ne)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            std::shared_ptr<AsmExprNode> val2 = shift();
            switch (keyWord)
            {
                case kw::eq:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::EQ, val1, val2);
                    break;
                case kw::ne:
                    val1 = std::make_shared<AsmExprNode>(AsmExprNode::NE, val1, val2);
                    break;
            }
        }
        keyWord = token->GetKeyword();
    }
    return val1;
}
std::shared_ptr<AsmExprNode> AsmExpr::and_()
{
    std::shared_ptr<AsmExprNode> val1 = equal();
    while (!token->IsEnd() && token->GetKeyword() == kw::band)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            std::shared_ptr<AsmExprNode> val2 = equal();
            val1 = std::make_shared<AsmExprNode>(AsmExprNode::AND, val1, val2);
        }
    }
    return val1;
}
std::shared_ptr<AsmExprNode> AsmExpr::xor_()
{
    std::shared_ptr<AsmExprNode> val1 = and_();
    while (!token->IsEnd() && token->GetKeyword() == kw::bxor)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            std::shared_ptr<AsmExprNode> val2 = and_();
            val1 = std::make_shared<AsmExprNode>(AsmExprNode::XOR, val1, val2);
        }
    }
    return val1;
}
std::shared_ptr<AsmExprNode> AsmExpr::or_()
{
    std::shared_ptr<AsmExprNode> val1 = xor_();
    while (!token->IsEnd() && token->GetKeyword() == kw::bor)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            std::shared_ptr<AsmExprNode> val2 = xor_();
            val1 = std::make_shared<AsmExprNode>(AsmExprNode::OR, val1, val2);
        }
    }
    return val1;
}
std::shared_ptr<AsmExprNode> AsmExpr::logicaland()
{
    std::shared_ptr<AsmExprNode> val1 = or_();
    while (!token->IsEnd() && token->GetKeyword() == kw::land)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            std::shared_ptr<AsmExprNode> val2 = or_();
            val1 = std::make_shared<AsmExprNode>(AsmExprNode::LAND, val1, val2);
        }
    }
    return val1;
}
std::shared_ptr<AsmExprNode> AsmExpr::logicalor()
{
    std::shared_ptr<AsmExprNode> val1 = logicaland();
    while (!token->IsEnd() && token->GetKeyword() == kw::lor)
    {
        token = tokenizer->Next();
        if (!token->IsEnd())
        {
            std::shared_ptr<AsmExprNode> val2 = logicaland();
            val1 = std::make_shared<AsmExprNode>(AsmExprNode::LOR, val1, val2);
        }
    }
    return val1;
}
