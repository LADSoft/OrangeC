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

#include "Instruction.h"
#include "InstructionParser.h"
#include "x64Operand.h"
#include "x64Parser.h"
#include "x64Instructions.h"
#include "AsmFile.h"
#include "Section.h"
#include "Fixup.h"
#include "AsmLexer.h"
#include <fstream>
#include <iostream>
#include "Errors.h"

const char* Lexer::preDataIntel =
    "%define __SECT__\n"
    "%imacro	pdata1	0+ .native\n"
    "	[db %1]\n"
    "%endmacro\n"
    "%imacro	.word	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%imacro	pdata2	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%imacro	pdata4	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%imacro	.long	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%imacro	pdata8	0+ .native\n"
    "	[dq %1]\n"
    "%endmacro\n"
    "%imacro	.byte	0+ .native\n"
    "	[db %1]\n"
    "%endmacro\n"
    "%imacro	db	0+ .native\n"
    "	[db %1]\n"
    "%endmacro\n"
    "%imacro	resb 0+ .native\n"
    "	[resb %1]\n"
    "%endmacro\n"
    "%imacro	dw	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%imacro	resw 0+ .native\n"
    "	[resw %1]\n"
    "%endmacro\n"
    "%imacro	dd	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%imacro	resd 0+ .native\n"
    "	[resd %1]\n"
    "%endmacro\n"
    "%imacro	dq	0+ .native\n"
    "	[dq %1]\n"
    "%endmacro\n"
    "%imacro	resq 0+ .native\n"
    "	[resq %1]\n"
    "%endmacro\n"
    "%imacro	dt	0+ .native\n"
    "	[dt %1]\n"
    "%endmacro\n"
    "%imacro	rest 0+ .native\n"
    "	[rest %1]\n"
    "%endmacro\n"
    "%imacro	section 0+ .nolist\n"
    "%define __SECT__ [section %1]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	segment 0+ .nolist\n"
    "%define __SECT__ [section %1]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	absolute 0+ .nolist\n"
    "%define __SECT__ [absolute %1]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	.code 0 .nolist\n"
    "%define __SECT__ [section code]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	.pdata 0 .nolist\n"
    "%define __SECT__ [section pdata]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	.bss 0 .nolist\n"
    "%define __SECT__ [section bss]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	align 0+ .nolist\n"
    "	[align %1]\n"
    "%endmacro\n"
    "%imacro	.align 0+ .nolist\n"
    "	[galign %1]\n"
    "%endmacro\n"
    "%imacro	import 0+ .nolist\n"
    "	[import %1]\n"
    "%endmacro\n"
    "%imacro	export 0+ .nolist\n"
    "	[export %1]\n"
    "%endmacro\n"
    "%imacro	global 0+ .nolist\n"
    "	[global %1]\n"
    "%endmacro\n"
    "%imacro	.globl 0+ .nolist\n"
    "	[global %1]\n"
    "%endmacro\n"
    "%imacro	extern 0+ .nolist\n"
    "	[extern %1]\n"
    "%endmacro\n"
    "%imacro equ 0+ .native\n"
    "	[equ	%1]\n"
    "%endmacro\n"
    "%imacro incbin 0+ .nolist\n"
    "	[incbin	%1]\n"
    "%endmacro\n"
    "%imacro times 0+ .native\n"
    "	[times %1]\n"
    "%endmacro\n"
    "%imacro struc 1 .nolist\n"
    "%push struc\n"
    "%define %$name %1\n"
    "[absolute 0]\n"
    "%{$name}:\n"
    "%endmacro\n"
    "%imacro endstruc 0 .nolist\n"
    "%{$name}_SIZE EQU $-%$name\n"
    "%pop\n"
    "__SECT__\n"
    "%endmacro\n";

const char* Lexer::preDataGas =
    "%define __SECT__\n"
    "%imacro	pdata1	0+ .native\n"
    "	[db %1]\n"
    "%endmacro\n"
    "%imacro	.word	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%imacro	pdata2	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%imacro	pdata4	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%imacro	.long	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%imacro	pdata8	0+ .native\n"
    "	[dq %1]\n"
    "%endmacro\n"
    "%imacro	.byte	0+ .native\n"
    "	[db %1]\n"
    "%endmacro\n"
    "%imacro	db	0+ .native\n"
    "	[db %1]\n"
    "%endmacro\n"
    "%imacro	resb 0+ .native\n"
    "	[resb %1]\n"
    "%endmacro\n"
    "%imacro	dw	0+ .native\n"
    "	[dw %1]\n"
    "%endmacro\n"
    "%imacro	resw 0+ .native\n"
    "	[resw %1]\n"
    "%endmacro\n"
    "%imacro	dd	0+ .native\n"
    "	[dd %1]\n"
    "%endmacro\n"
    "%imacro	resd 0+ .native\n"
    "	[resd %1]\n"
    "%endmacro\n"
    "%imacro	dq	0+ .native\n"
    "	[dq %1]\n"
    "%endmacro\n"
    "%imacro	resq 0+ .native\n"
    "	[resq %1]\n"
    "%endmacro\n"
    "%imacro	dt	0+ .native\n"
    "	[dt %1]\n"
    "%endmacro\n"
    "%imacro	rest 0+ .native\n"
    "	[rest %1]\n"
    "%endmacro\n"
    "%imacro	section 0+ .nolist\n"
    "%define __SECT__ [section %1]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	segment 0+ .nolist\n"
    "%define __SECT__ [section %1]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	absolute 0+ .nolist\n"
    "%define __SECT__ [absolute %1]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	.code 0 .nolist\n"
    "%define __SECT__ [section code]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	.pdata 0 .nolist\n"
    "%define __SECT__ [section pdata]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	.bss 0 .nolist\n"
    "%define __SECT__ [section bss]\n"
    "__SECT__\n"
    "%endmacro\n"
    "%imacro	align 0+ .nolist\n"
    "	[align %1]\n"
    "%endmacro\n"
    "%imacro	.align 0+ .nolist\n"
    "	[galign %1]\n"
    "%endmacro\n"
    "%imacro    .type 0+ nolist\n" // this macro simply is ignored...
    "%endmacro\n"
    "%imacro	import 0+ .nolist\n"
    "	[import %1]\n"
    "%endmacro\n"
    "%imacro	export 0+ .nolist\n"
    "	[export %1]\n"
    "%endmacro\n"
    "%imacro	global 0+ .nolist\n"
    "	[global %1]\n"
    "%endmacro\n"
    "%imacro	.globl 0+ .nolist\n"
    "	[global %1]\n"
    "%endmacro\n"
    "%imacro	extern 0+ .nolist\n"
    "	[extern %1]\n"
    "%endmacro\n"
    "%imacro equ 0+ .native\n"
    "	[equ	%1]\n"
    "%endmacro\n"
    "%imacro incbin 0+ .nolist\n"
    "	[incbin	%1]\n"
    "%endmacro\n"
    "%imacro times 0+ .native\n"
    "	[times %1]\n"
    "%endmacro\n"
    "%imacro struc 1 .nolist\n"
    "%push struc\n"
    "%define %$name %1\n"
    "[absolute 0]\n"
    "%{$name}:\n"
    "%endmacro\n"
    "%imacro endstruc 0 .nolist\n"
    "%{$name}_SIZE EQU $-%$name\n"
    "%pop\n"
    "__SECT__\n"
    "%endmacro\n";

int InstructionParser::processorMode = 32;

bool IsSymbolCharRoutine(const char* data, bool startOnly);


void Instruction::RepRemoveCancellations(AsmExprNode *exp, bool commit, int &count, Section *sect[], bool sign[], bool plus)
{
    if (exp->GetType() == AsmExprNode::LABEL)
    {
        AsmExprNode* num = AsmExpr::GetEqu(exp->label);
        if (!num)
        {
            auto s = AsmFile::GetLabelSection(exp->label);
            if (s)
            {
                if (commit)
                {
                    exp->SetType(AsmExprNode::IVAL);
                    exp->ival = s->Lookup(exp->label)->second;
                }
                else
                {
                    sect[count] = s;
                    sign[count++] = plus;
                }
            }
        }
        else
            RepRemoveCancellations(num, commit, count, sect, sign, plus);
    }
    else if (exp->GetType() == AsmExprNode::BASED)
    {
        if (!commit)
        {
            sect[count] = exp->GetSection();
            sign[count++] = plus;
        }
        else
        {
            exp->SetType(AsmExprNode::IVAL);
        }
    }
    if (exp->GetLeft())
    {
        RepRemoveCancellations(exp->GetLeft(), commit, count, sect, sign, plus);
    }
    if (exp->GetRight())
    {
        RepRemoveCancellations(exp->GetRight(), commit, count, sect, sign, exp->GetType() == AsmExprNode::SUB ? !plus : plus);
    }
}

void Instruction::Optimize(Section* sect, int pc, bool last)
{
    unsigned char* pdata = data.get();
    if (pdata && size >= 3 && pdata[0] == 0x0f && pdata[1] == 0x0f && (pdata[2] == 0x9a || pdata[2] == 0xea))
    {
        if (fixups.size() != 1)
        {
            std::cout << "Far branch cannot be resolved" << std::endl;
            memmove(pdata, pdata + 2, size - 2);
            size -= 2;
        }
        else
        {
            Fixup* f = fixups[0].get();
            AsmExprNode* expr = AsmExpr::Eval(f->GetExpr(), pc);
            if (expr->IsAbsolute())
            {
                memmove(pdata, pdata + 1, size - 1);
                size -= 3;
                f->SetInsOffs(f->GetInsOffs() - 1);
                pdata[0] = 0x0e;  // push cs
                int n = pdata[f->GetInsOffs() - 1];
                if (n == 0xea)  // far jmp
                    n = 0xe9;   // jmp
                else
                    n = 0xe8;  // call
                pdata[f->GetInsOffs() - 1] = n;
                f->SetRel(true);
                f->SetRelOffs(f->GetSize());
                f->SetAdjustable(true);
            }
            else
            {
                memmove(pdata, pdata + 2, size - 2);
                size -= 2;
                f->SetInsOffs(f->GetInsOffs() - 2);
                AsmExprNode* n = new AsmExprNode(AsmExprNode::DIV, f->GetExpr(), new AsmExprNode(16));
                fixups.push_back(std::make_unique<Fixup>(n, 2, false));
                f = fixups.back().get();
                f->SetInsOffs(size - 2);
            }
            delete expr;
        }
    }
    if (type == CODE || type == DATA || type == RESERVE)
    {
        for (auto& fixup : fixups)
        {
            Section* sect[10];
            bool sign[10];
            int count = 0;
            bool canceled = false;
            AsmExprNode* expr = fixup->GetExpr();
            RepRemoveCancellations(expr, false, count, sect, sign, true);
            if (count == 2 && sect[0] == sect[1] && sign[0] == !sign[1])
            {
                canceled = true;
                RepRemoveCancellations(expr, true, count, sect, sign, true);
            }
            expr = AsmExpr::Eval(expr, pc);
            if (fixup->GetExpr()->IsAbsolute() || canceled || (fixup->IsRel() && expr->GetType() == AsmExprNode::IVAL))
            {
                int n = fixup->GetSize() * 8;
                int p = fixup->GetInsOffs();
                if (fixup->GetSize() < 4 || (expr->GetType() == AsmExprNode::IVAL && fixup->GetSize() == 4))
                {
                    int o;
                    bool error = false;
                    if (expr->GetType() == AsmExprNode::IVAL)
                        o = expr->ival;
                    else
                        o = (long long)expr->fval;
                    if (fixup->IsRel())
                    {
                        o -= size + pc;
                        if (fixup->IsAdjustable())
                        {
                            if (type == CODE)
                            {
                                if (p >= 1 && pdata[p - 1] == 0xe9)
                                {
                                    if (o <= 127 && o >= -128 - ((n / 8) - 1))
                                    {
                                        pdata[p - 1] = 0xeb;
                                        size -= fixup->GetSize() - 1;
                                        fixup->SetSize(1);
                                        fixup->SetRelOffs(1);
                                        n = 8;
                                    }
                                }
                                else if (p >= 2 && ((pdata[p - 1] & 0xf0) == 0x80) && pdata[p - 2] == 0x0f)
                                {
                                    if (o <= 127 && o >= -128 - (n / 8))
                                    {
                                        pdata[p - 2] = pdata[p - 1] - 0x80 + 0x70;
                                        size -= fixup->GetSize();
                                        fixup->SetInsOffs(p = fixup->GetInsOffs() - 1);
                                        fixup->SetSize(1);
                                        fixup->SetRelOffs(1);
                                        n = 8;
                                    }
                                }
                            }
                        }
                        int t = o >> (n - 1);
                        if (t != 0 && t != -1)
                        {
                            error = true;
                            if (!fixup->IsResolved())
                            {
                                if (last)
                                {
                                    Errors::IncrementCount();
                                    std::cout << "Error " << fixup->GetFileName().c_str() << "(" << fixup->GetErrorLine() << "):"
                                              << "Branch out of range" << std::endl;
                                }
                            }
                        }
                    }
                    else
                    {
                        if (n < 32)
                        {
                            int t = o >> (n);
                            if (t != 0 && t != -1)
                            {
                                error = true;
                                if (!fixup->IsResolved())
                                {
                                    if (last)
                                    {
                                        //                                        Errors::IncrementCount();
                                        std::cout << "Warning " << fixup->GetFileName() << "(" << fixup->GetErrorLine() << "):"
                                                  << "Value out of range" << std::endl;
                                    }
                                }
                            }
                        }
                    }
                    if (!error)
                        fixup->SetResolved();
                    if (bigEndian)
                    {
                        // never get here on an x86
                        for (int i = n - 8; i >= 0; i -= 8)
                            pdata[p + i / 8] = o >> i;
                    }
                    else
                    {
                        for (int i = 0; i < n; i += 8)
                            pdata[p + i / 8] = o >> i;
                    }
                }
                else if (expr->GetType() == AsmExprNode::IVAL || expr->GetType() == AsmExprNode::FVAL)
                {
                    FPF o;
                    fixup->SetResolved();
                    if (expr->GetType() == AsmExprNode::FVAL)
                        o = expr->fval;
                    else
                        o = expr->ival;
                    switch (n)
                    {
                        case 32:
                            o.ToFloat(pdata + p);
                            break;
                        case 64:
                            o.ToDouble(pdata + p);
                            break;
                        case 80:
                            o.ToLongDouble(pdata + p);
                            break;
                    }
                }
            }
            delete expr;
        }
    }
}
std::string InstructionParser::RewriteATTArg(const std::string& line)
{
    int i = line.find_first_not_of("\t\v \r\n");
    std::string seg;
    if (line[i] == '%')
    {
        seg = line.substr(i + 1);
        if (seg.size() > 2 && seg[2] == ':')
        {
            seg = seg.substr(0, 3);
            i += 4;
        }
        else
        {
            if (seg.size() >= 2 && seg.substr(0, 2) == "st")
            {
                if (seg == "st")
                    return "st0";
                else if (seg[2] == '(' && seg[4] == ')')
                    return "st" + seg.substr(3, 1);
            }
            return seg;
        }
    }
    if (line[i] == '$')
    {
        for (i = i + 1; i < line.size(); i++)
            if (!isspace(line[i]))
                break;
        if (i < line.size() && (line[i] == '-' || isdigit(line[i])))
        {
            return line.substr(i);
        }
        return "dword " + line.substr(i);
    }
    else
    {
        std::string name;
        if (IsSymbolCharRoutine(line.c_str() + i, true))
        {
            int j = i + 1;
            while (IsSymbolCharRoutine(line.c_str() + j, false)) j++;
            name = line.substr(i, j - i);
            i = j;
        }
        else
        {
            if (line[i] == '-' || isdigit(line[i]))
            {
                int j = i + 1;
                for (; j < line.size(); j++)
                    if (!isdigit(line[j]))
                        break;
                name = line.substr(i, j - i);
                i = j;
            }
        }
        if (i < line.size())
        {
            int start = i;
            if (line[i] != '(')
            {
                // error
                return line;
            }
            else
            {
                std::string primary, secondary, times;
                i++;
                for (; i < line.size(); i++)
                    if (!isspace(line[i]))
                        break;
                int j = line.size() - 1;
                while (j > i && isspace(line[j]))
                    j--;
                if (line[j] == ')')
                {
                    std::vector<std::string> splt;
                    Split(line.substr(i, j - i), splt);

                    switch (splt.size())
                    {
                    case 1:
                        if (splt[0] != "" && splt[0][0] == '%')
                        {
                            primary = splt[0].substr(1);
                            break;
                        }
                        // error
                        return line;
                    case 2:
                        if (splt[0] == "")
                        {
                            if (splt[1] == "1")
                            {
                                return "[" + seg + name + "]";
                            }
                        }
                        else if (splt[0][0] == '%')
                        {
                            primary = splt[0].substr(1);
                            if (splt[1] != "" && splt[1][0] == '%')
                            {
                                secondary = splt[1].substr(1);
                                break;
                            }
                        }
                        // error
                        return line;
                    case 3:
                        if (splt[0] != "")
                        {
                            if (splt[0][0] == '%')
                                primary = splt[0].substr(1);
                            else
                                return "[" + seg + name + line.substr(i) + "]";
                        }
                        if (splt[1] != "")
                        {
                            if (splt[1][0] == '%')
                                secondary = splt[1].substr(1);
                            else
                                return "[" + seg + name + line.substr(i) + "]";
                        }
                        if (splt[2] != "")
                        {
                            times = splt[2];
                        }
                        break;
                    default:
                        // error
                        return line;

                    }
                    if (secondary.size())
                    {
                        if (primary.size())
                            primary += "+";
                        primary += secondary;
                        if (times.size())
                        {
                            primary += "*" + times;
                        }
                    }
                    if (name.size())
                        primary += "+" + name;
                    primary = "[" + seg + primary + "]";
                    return primary;
                }
                else
                {
                    // error
                    return line;
                }
            }
        }
        else
        {
            return "[" + seg + name + "]";
        }
        return line;
    }
}
void InstructionParser::PreprendSize(std::string& val, int sz)
{
    const char *str = "";
    switch (sz)
    {
    case 1:
        str = "byte ";
        break;
    case 2:
        str = "word ";
        break;
    case 4:
    case -4:
        str = "dword ";
        break;
    case 8:
    case -8:
        str = "qword ";
        break;
    case -10:
        str = "tword ";
        break;
    }
    val = std::string(str) + val;
}

std::string InstructionParser::ParsePreInstruction(const std::string& op, bool doParse) 
{ 
    std::string parse = op;
    while (true)
    {
        int npos = parse.find_first_not_of("\t\v \n\r");
        if (npos != std::string::npos)
            parse = parse.substr(npos);
        if (!parse.size() || parse[0] != '{')
            break;
        npos = parse.find_first_of("}");
        if (npos == std::string::npos)
            break;
        Errors::Warning("Unknown qualifier " +  parse.substr(0, npos+1));

        parse = parse.substr(npos + 1);
    }
    return parse; 
}

std::string InstructionParser::RewriteATT(int& op, const std::string& line, int& size1, int& size2)
{
    switch (op)
    {
    case op_call:
    case op_jmp:
    {
        int npos = line.find_first_not_of("\t\v \r\n");
        if (npos != std::string::npos)
            if (line[npos] == '*')
                return "[" + line.substr(npos + 1) + "]";
        return line;
    }
    case 10000://lcall
        op = op_call;
        return "far [" + line + "]";
    case 10001://ljmp
        op = op_jmp;
        return "far [" + line + "]";
    case op_ja:
    case op_jae:
    case op_jb:
    case op_jbe:
    case op_jc:
    case op_jcxz:
    case op_jecxz:
    case op_je:
    case op_jg:
    case op_jge:
    case op_jl:
    case op_jle:
    case op_jna:
    case op_jnae:
    case op_jnb:
    case op_jnbe:
    case op_jnc:
    case op_jne:
    case op_jng:
    case op_jnge:
    case op_jnl:
    case op_jnle:
    case op_jno:
    case op_jnp:
    case op_jns:
    case op_jnz:
    case op_jo:
    case op_jp:
    case op_jpe:
    case op_jpo:
    case op_js:
    case op_jz:
        return line;
    }
    std::vector<std::string> splt, splt2;
    // split arguments out into multiple strings
    Split(line, splt);
    // process each arg
    for (auto s : splt)
    {
        splt2.push_back(RewriteATTArg(s));
    }
    if (size1 == 4) // 'l'
    {
        if (op >= op_f2xm1 && op <= op_fyl2xp1)
        {
            size1 = -8;
        }
    }
    if (size2 && splt2[0].find_first_of("[") != std::string::npos)
    {
        PreprendSize(splt2[0], size2);
    }
    else if (size1)
    {
        // the other combination has a destination which MUST BE a reg so we don't consider it.
        if (splt2[1].find_first_of("[") != std::string::npos)
            if (splt[0].find_first_of("%") == std::string::npos)
                PreprendSize(splt2[1], size1);
    }

    if (op >= op_f2xm1 && op <= op_fyl2xp1 && splt.size() == 2)
    {
        // as swaps the sense of some of the floating point instructions
        // and gcc depends on this...
        if (splt2[1].size() > 2 && splt2[1].substr(0, 2) == "st" && splt2[1] != "st0")
        {
            switch (op)
            {
            case op_fsub:
                op = op_fsubr;
                break;
            case op_fsubp:
                op = op_fsubrp;
                break;
            case op_fsubr:
                op = op_fsub;
                break;
            case op_fsubrp:
                op = op_fsubp;
                break;
            case op_fdiv:
                op = op_fdivr;
                break;
            case op_fdivp:
                op = op_fdivrp;
                break;
            case op_fdivr:
                op = op_fdiv;
                break;
            case op_fdivrp:
                op = op_fdivp;
                break;
            }
        }
    }
    std::string rv;
    // reverse
    for (int i = splt2.size() - 1; i >= 0; i--)
    {
        if (rv.size())
            rv += ",";
        rv += splt2[i];
    }
    return rv;
}

int x64Parser::DoMath(char op, int left, int right)
{
    switch (op)
    {
        case '!':
            return -left;
        case '~':
            return ~left;
        case '+':
            return left + right;
        case '-':
            return left - right;
        case '>':
            return left >> right;
        case '<':
            return left << right;
        case '&':
            return left & right;
        case '|':
            return left | right;
        case '^':
            return left ^ right;
        default:
            return left;
    }
}
void x64Parser::Setup(Section* sect)
{
    if (sect->beValues[0] == 0)
        sect->beValues[0] = processorMode;  // 32 bit mode is the default
    Setprocessorbits(sect->beValues[0]);
}

bool x64Parser::ParseSection(AsmFile* fil, Section* sect)
{
    bool rv = false;
    if (fil->GetKeyword() == kw::USE16)
    {
        sect->beValues[0] = 16;
        fil->NextToken();
        rv = true;
    }
    else if (fil->GetKeyword() == kw::USE32)
    {
        sect->beValues[0] = 32;
        fil->NextToken();
        rv = true;
    }
    else if (fil->GetKeyword() == kw::USE64)
    {
        sect->beValues[0] = 64;
        fil->NextToken();
        rv = true;
    }
    return rv;
}
bool x64Parser::ParseDirective(AsmFile* fil, Section* sect)
{
    bool rv = false;
    if (fil->GetKeyword() == kw::BITS)
    {
        fil->NextToken();
        if (fil->IsNumber())
        {
            int n = static_cast<const NumericToken*>(fil->GetToken())->GetInteger();
            if (n == 16 || n == 32 || n == 64)
            {
                sect->beValues[0] = n;
                Setprocessorbits(sect->beValues[0]);
                rv = true;
            }
            fil->NextToken();
        }
    }
    return rv;
}