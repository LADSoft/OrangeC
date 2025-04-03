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

#include "Instruction.h"
#include "InstructionParser.h"
#include "x64Operand.h"
#include "x64Parser.h"
#include "AsmFile.h"
#include "Section.h"
#include "Fixup.h"
#include "AsmLexer.h"
#include <fstream>
#include <iostream>

std::set<std::string> InstructionParser::attPotentialExterns;

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

const char* Lexer::preDataGas = "";

InstructionParser* InstructionParser::GetInstance() { return static_cast<InstructionParser*>(new x64Parser()); }

void Instruction::Optimize(Section* sect, int pc, bool last)
{

    unsigned char* pdata = data.get();
    if (pdata && size >= 3 && pdata[0] == 0x0f && pdata[1] == 0x0f && (pdata[2] == 0x9a || pdata[2] == 0xea))
    {
        if (fixups.size() != 1)
        {
            std::cout << "Far branch cannot be resolved" << std::endl;
            memcpy(pdata, pdata + 2, size - 2);
            size -= 2;
        }
        else
        {
            std::shared_ptr<Fixup> f = fixups[0];
            std::shared_ptr<AsmExprNode> expr = AsmExpr::Eval(f->GetExpr(), pc);
            if (expr->IsAbsolute())
            {
                memcpy(pdata, pdata + 1, size - 1);
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
                memcpy(pdata, pdata + 2, size - 2);
                size -= 2;
                f->SetInsOffs(f->GetInsOffs() - 2);
                std::shared_ptr<AsmExprNode> n =
                    std::make_shared<AsmExprNode>(AsmExprNode::DIV, f->GetExpr(), std::make_shared<AsmExprNode>(16));
                fixups.push_back(std::make_shared<Fixup>(n, 2, false));
                f = fixups.back();
                f->SetInsOffs(size - 2);
            }
        }
    }
    if (type == CODE || type == DATA || type == RESERVE)
    {
        for (auto& fixup : fixups)
        {
            std::shared_ptr<AsmExprNode> expr = fixup->GetExpr();
            expr = AsmExpr::Eval(expr, pc);
            if (fixup->GetExpr()->IsAbsolute() || (fixup->IsRel() && expr->GetType() == AsmExprNode::IVAL))
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
                                        std::cout << "Warning " << fixup->GetFileName().c_str() << "(" << fixup->GetErrorLine()
                                                  << "):"
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
        }
    }
}
std::string InstructionParser::ParsePreInstruction(const std::string& op, bool doParse) { return op; }
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
    Section::NoShowError();
    if (sect->beValues[0] == 0)
        sect->beValues[0] = 32;  // 32 bit mode is the default
    Setprocessorbits(sect->beValues[0]);
}

bool x64Parser::ParseSection(AsmFile* fil, Section* sect) { return false; }
bool x64Parser::ParseDirective(AsmFile* fil, Section* sect) { return false; }