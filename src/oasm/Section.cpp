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

#include "ObjFactory.h"
#include "ObjSection.h"
#include "ObjMemory.h"
#include "ObjSymbol.h"
#include "Section.h"
#include "Label.h"
#include "Fixup.h"
#include "AsmFile.h"

#include <stdexcept>
#include <climits>
#include <fstream>
#include <iostream>

bool Section::dontShowError;
std::unordered_map<ObjString, std::shared_ptr<Section>> Section::sections;

Section::~Section() {}
void Section::Parse(AsmFile* fil)
{
    while (!fil->AtEof() && fil->GetKeyword() != kw::closebr)
    {
        if (fil->GetKeyword() == kw::ALIGN)
        {
            fil->NextToken();
            if (fil->GetKeyword() != kw::assn)
                throw new std::runtime_error("Expected '='");
            fil->NextToken();
            if (!fil->IsNumber())
                throw new std::runtime_error("Expected alignment value");
            align = fil->GetValue();
        }
        else if (fil->GetKeyword() == kw::CLASS)
        {
            fil->NextToken();
            if (fil->GetKeyword() != kw::assn)
                throw new std::runtime_error("Expected '='");
            fil->NextToken();
            fil->GetId();
        }
        else if (fil->GetKeyword() == kw::VIRTUAL)
        {
            std::shared_ptr<Section> old = fil->GetCurrentSection();
            if (!old)
                throw new std::runtime_error("Virtual section must be enclosed in other section");
            align = old->align;
            memcpy(beValues, old->beValues, sizeof(beValues));
            isVirtual = true;
            fil->NextToken();
        }
        else if (fil->GetKeyword() == kw::STACK)
        {
            fil->NextToken();
        }
        else if (!fil->GetParser()->ParseSection(fil, this))
            throw new std::runtime_error("Invalid section qualifier");
    }
}
void Section::Optimize(std::shared_ptr<Section>& name)
{
    AsmExpr::SetSection(name);
    bool done = false;
    int pc = 0;
    for (int i = 0; i < instructions.size(); i++)
    {
        if (instructions[i]->IsLabel())
        {
            std::shared_ptr<Label> l = instructions[i]->GetLabel();
            if (l)
            {
                l->SetOffset(pc);
                labels[l->GetName()] = pc;
            }
        }
        pc += instructions[i]->GetSize();
    }
    while (!done)
    {
        done = true;
        pc = 0;
        for (int i = 0; i < instructions.size(); i++)
        {
            if (instructions[i]->IsLabel())
            {
                std::shared_ptr<Label> l = instructions[i]->GetLabel();
                if (l)
                {
                    l->SetOffset(pc);
                    labels[l->GetName()] = pc;
                }
            }
            else
            {
                int n = instructions[i]->GetSize();
                instructions[i]->SetOffset(pc);
                instructions[i]->Optimize(this, pc, false);
                int m = instructions[i]->GetSize();
                pc += m;
                if (n != m)
                {
                    done = false;
                }
            }
        }
    }
    pc = 0;
    for (int i = 0; i < instructions.size(); i++)
    {
        if (instructions[i]->IsLabel())
        {
            std::shared_ptr<Label> l = instructions[i]->GetLabel();
            if (l)
            {
                l->SetOffset(pc);
                labels[l->GetName()] = pc;
            }
        }
        instructions[i]->SetOffset(pc);
        instructions[i]->Optimize(this, pc, true);
        pc += instructions[i]->GetSize();
    }
}
void Section::MergeSections()
{
    for (auto&& s : subSections)
    {
        for (auto&& i : s.second->GetInstructions())
        {
            instructions.push_back(i);
            if (instructions.back()->IsLabel())
            {
                labels[instructions.back()->GetLabel()->GetName()] = pc;
            }
            else
            {
                instructions.back()->SetOffset(pc);
                pc += instructions.back()->GetSize();
            }
        }
        s.second->ClearInstructions();
    }
}

void Section::Resolve(std::shared_ptr<Section>& name) { Optimize(name); }
void Section::InsertInstruction(std::shared_ptr<Instruction>& ins)
{
    if (subSection == 0)
    {
        instructions.push_back(ins);
        ins->SetOffset(pc);
        pc += ins->GetSize();
    }
    else
    {
        subSections[subSection]->InsertInstruction(ins);
    }
}
std::shared_ptr<Instruction> Section::InsertLabel(std::shared_ptr<Label>& label)
{
    if (subSection == 0)
        instructions.push_back(std::make_shared<Instruction>(label));
    else
        subSections[subSection]->GetInstructions().push_back(std::make_shared<Instruction>(label));
    labels[label->GetName()] = pc;
    return subSection == 0 ? instructions.back() : subSections[subSection]->GetInstructions().back();
}
void Section::pop_back()
{
    if (subSection == 0)
        instructions.pop_back();
    else
        subSections[subSection]->pop_back();
}
ObjSection* Section::CreateObject(ObjFactory& factory)
{
    objectSection = factory.MakeSection(name);
    objectSection->SetSize(factory.MakeExpression(0));
    objectSection->SetOffset(factory.MakeExpression(0));

    if (isVirtual)
        objectSection->SetQuals(objectSection->GetQuals() | ObjSection::max | ObjSection::virt);
    return objectSection;
}
ObjExpression* Section::ConvertExpression(std::shared_ptr<AsmExprNode>& node,
                                          std::function<std::shared_ptr<Label>(std::string&)> Lookup,
                                          std::function<ObjSection*(std::string&)> SectLookup, ObjFactory& factory)
{
    ObjExpression* xleft = nullptr;
    ObjExpression* xright = nullptr;
    if (node->GetLeft())
    {
        auto expr = node->GetLeft();
        xleft = ConvertExpression(expr, Lookup, SectLookup, factory);
    }
    if (node->GetRight())
    {
        auto expr = node->GetRight();
        xright = ConvertExpression(expr, Lookup, SectLookup, factory);
    }
    switch (node->GetType())
    {
        case AsmExprNode::IVAL:
            return factory.MakeExpression(node->ival);
        case AsmExprNode::FVAL:
            throw new std::runtime_error("Floating point in relocatable expression not allowed");
        case AsmExprNode::PC:
            return factory.MakeExpression(ObjExpression::ePC);
        case AsmExprNode::SECTBASE:
            return factory.MakeExpression(objectSection);
        case AsmExprNode::BASED: {
            ObjExpression* left = factory.MakeExpression(node->GetSection()->GetObjectSection());
            ObjExpression* right = factory.MakeExpression(node->ival);
            return factory.MakeExpression(ObjExpression::eAdd, left, right);
        }
        break;
        case AsmExprNode::LABEL: {
            std::shared_ptr<AsmExprNode> num = AsmExpr::GetEqu(node->label);
            if (num)
            {
                return ConvertExpression(num, Lookup, SectLookup, factory);
            }
            else
            {
                std::shared_ptr<Label> label = Lookup(node->label);
                if (label != nullptr)
                {

                    ObjExpression* t;
                    if (label->IsExtern())
                    {
                        t = factory.MakeExpression(label->GetObjSymbol());
                    }
                    else
                    {
                        auto offs = label->GetOffset();
                        ObjExpression* left = factory.MakeExpression(label->GetObjectSection());
                        ObjExpression* right = ConvertExpression(offs, std::move(Lookup), std::move(SectLookup), factory);
                        t = factory.MakeExpression(ObjExpression::eAdd, left, right);
                    }
                    return t;
                }
                else
                {
                    ObjSection* s = SectLookup(node->label);
                    if (s)
                    {
                        ObjExpression* left = factory.MakeExpression(s);
                        ObjExpression* right = factory.MakeExpression(16);
                        return factory.MakeExpression(ObjExpression::eDiv, left, right);
                    }
                    else
                    {
                        std::string name = node->label;
                        if (name.substr(0, 3) == "..@" && isdigit(name[3]))
                        {
                            int i;
                            for (i = 4; i < name.size() && isdigit(name[i]); i++)
                                ;
                            if (name[i] == '.')
                            {
                                name = std::string("%") + name.substr(i + 1);
                            }
                            else if (name[i] == '@')
                            {
                                name = std::string("%$") + name.substr(i + 1);
                            }
                        }
                        throw new std::runtime_error(std::string("Label '") + name + "' does not exist.");
                    }
                }
            }
        }
        case AsmExprNode::ADD:
            return factory.MakeExpression(ObjExpression::eAdd, xleft, xright);
        case AsmExprNode::SUB:
            return factory.MakeExpression(ObjExpression::eSub, xleft, xright);
        case AsmExprNode::NEG:
            return factory.MakeExpression(ObjExpression::eNeg, xleft, xright);
        case AsmExprNode::CMPL:
            return factory.MakeExpression(ObjExpression::eCmpl, xleft, xright);
        case AsmExprNode::MUL:
            return factory.MakeExpression(ObjExpression::eMul, xleft, xright);
        case AsmExprNode::DIV:
            return factory.MakeExpression(ObjExpression::eDiv, xleft, xright);
        case AsmExprNode::NOT:
        case AsmExprNode::SDIV:
        case AsmExprNode::SMOD:
        case AsmExprNode::OR:
        case AsmExprNode::XOR:
        case AsmExprNode::AND:
        case AsmExprNode::GT:
        case AsmExprNode::LT:
        case AsmExprNode::GE:
        case AsmExprNode::LE:
        case AsmExprNode::EQ:
        case AsmExprNode::NE:
        case AsmExprNode::MOD:
        case AsmExprNode::LSHIFT:
        case AsmExprNode::RSHIFT:
        case AsmExprNode::LAND:
        case AsmExprNode::LOR:
        case AsmExprNode::REG:
            throw new std::runtime_error("Operator not allowed in address expression");
    }
    return nullptr;
}
bool Section::SwapSectionIntoPlace(ObjExpression* t)
{
    ObjExpression* left = t->GetLeft();
    ObjExpression* right = t->GetRight();
    if (t->GetOperator() == ObjExpression::eSub || t->GetOperator() == ObjExpression::eDiv || (left && !right))
    {
        return SwapSectionIntoPlace(left);
    }
    else if (left && right)
    {
        bool n1 = SwapSectionIntoPlace(left);
        bool n2 = SwapSectionIntoPlace(right);
        if (n2 && !n1)
        {
            t->SetLeft(right);
            t->SetRight(left);
        }
        return n1 || n2;
    }
    else
    {
        return t->GetOperator() == ObjExpression::eSection;
    }
}
bool Section::MakeData(ObjFactory& factory, std::function<std::shared_ptr<Label>(std::string&)> Lookup,
                       std::function<ObjSection*(std::string&)> SectLookup,
                       std::function<void(ObjFactory&, Section*, std::shared_ptr<Instruction>&)> HandleAlt)
{
    bool rv = true;
    int pc = 0;
    int pos = 0;
    unsigned char buf[1024];
    Fixup f;
    int n;
    ObjSection* sect = objectSection;
    if (sect)
    {
        sect->SetAlignment(align);
        instructionPos = 0;
        while ((n = GetNext(f, buf + pos, sizeof(buf) - pos)) != 0)
        {
            if (n > 0)
            {
                pos += n;
                if (pos == sizeof(buf))
                {
                    ObjMemory* mem = factory.MakeData(buf, pos);
                    sect->Add(mem);
                    pos = 0;
                }
                pc += n;
            }
            else
            {
                if (pos || n == -2)
                {
                    ObjMemory* mem = factory.MakeData(buf, pos);
                    sect->Add(mem);
                    pos = 0;
                }
                if (n == -2)
                {
                    ObjMemory* mem = factory.MakeData(buf, 0);
                    sect->Add(mem);
                    while (instructionPos < instructions.size() && instructions[instructionPos]->GetType() == Instruction::ALT)
                        HandleAlt(factory, this, instructions[instructionPos++]);
                }
                else
                {
                    ObjExpression* t;
                    try
                    {
                        auto expr = f.GetExpr();
                        t = ConvertExpression(expr, Lookup, SectLookup, factory);
                        SwapSectionIntoPlace(t);
                    }
                    catch (std::runtime_error* e)
                    {
                        Errors::IncrementCount();
                        if (!dontShowError)
                            std::cout << "Error " << f.GetFileName().c_str() << "(" << f.GetErrorLine() << "):" << e->what()
                                      << std::endl;
                        delete e;
                        t = nullptr;
                        rv = false;
                    }
                    if (t && f.IsRel())
                    {
                        ObjExpression* left = factory.MakeExpression(f.GetRelOffs());
                        t = factory.MakeExpression(ObjExpression::eSub, t, left);
                        left = factory.MakeExpression(ObjExpression::ePC);
                        t = factory.MakeExpression(ObjExpression::eSub, t, left);
                    }
                    if (t)
                    {

                        ObjMemory* mem = factory.MakeFixup(t, f.GetSize());
                        if (mem)
                            sect->Add(mem);
                    }
                    pc += f.GetSize();
                }
            }
        }
        if (pos)
        {
            ObjMemory* mem = factory.MakeData(buf, pos);
            sect->Add(mem);
        }
    }
    return rv;
}
int Section::GetNext(Fixup& f, unsigned char* buf, int len)
{
    static int blen = 0;
    static char buf2[256];
    if (!blen)
    {

        while (instructionPos < instructions.size() &&
               (blen = instructions[instructionPos]->GetNext(f, (unsigned char*)&buf2[0])) == 0)
        {
            if (instructions[instructionPos]->GetType() == Instruction::ALT)
                return -2;
            instructionPos++;
        }
        if (instructionPos >= instructions.size())
            return 0;
    }
    if (blen == -1 || blen >= sizeof(buf2))
    {
        blen = 0;
        return -1;
    }
    if (blen <= len)
    {
        memcpy(buf, buf2, blen);
        int rv = blen;
        blen = 0;
        return rv;
    }
    else
    {
        memcpy(buf, buf2, len);
        memmove(buf2, buf2 + len, blen - len);
        blen -= len;
        return len;
    }
}
