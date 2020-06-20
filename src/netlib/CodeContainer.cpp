/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

#include "DotNetPELib.h"
#include "PEFile.h"
#include <typeinfo>
namespace DotNetPELib
{
bool CodeContainer::InAssemblyRef() const { return parent_->InAssemblyRef(); }
void CodeContainer::AddInstruction(Instruction* instruction)
{
    if (instruction)
        instructions_.push_back(instruction);
}

bool CodeContainer::ILSrcDump(PELib& peLib) const
{
    for (auto instruction : instructions_)
        instruction->ILSrcDump(peLib);
    return true;
}
void CodeContainer::ObjOut(PELib& peLib, int pass) const
{
    if (pass == 3 && instructions_.size())
    {
        peLib.Out() << std::endl << "$Ib";
        for (auto instruction : instructions_)
            instruction->ObjOut(peLib, pass);
        peLib.Out() << std::endl << "$Ie";
    }
}
void CodeContainer::ObjIn(PELib& peLib)
{
    if (instructions_.size())
    {
        // duplicate public..   ignore the second instance
        // peLib.ObjError(oe_syntax);
        while (peLib.ObjBegin() == 'i')
        {
            Instruction::ObjIn(peLib);
        }
    }
    else
    {
        while (peLib.ObjBegin() == 'i')
        {
            AddInstruction(Instruction::ObjIn(peLib));
        }
    }
    if (peLib.ObjEnd(false) != 'I')
        peLib.ObjError(oe_syntax);
}
Byte* CodeContainer::Compile(PELib& peLib, size_t& sz)
{
    Byte* rv = nullptr;
    CalculateOffsets();
    LoadLabels();
    Instruction* last = instructions_.size() ? instructions_.back() : nullptr;
    if (last)
    {
        sz = last->Offset() + last->InstructionSize();
        if (sz)
        {
            rv = new Byte[sz];
            int pos = 0;
            for (auto instruction : instructions_)
            {
                pos += instruction->Render(peLib, rv + pos, labels);
            }
        }
    }
    else
    {
        sz = 0;
    }
    labels.clear();
    return rv;
}
int CodeContainer::CompileSEH(std::vector<Instruction*> tags, int offset, std::vector<SEHData>& sehData)
{
    if (!tags[offset]->SEHBegin() || tags[offset]->SEHType() != Instruction::seh_try)
        return 1;
    SEHData current;
    memset(&current, 0, sizeof(current));
    current.tryOffset = tags[offset++]->Offset();
    while (offset < tags.size() && tags[offset]->SEHBegin())
        offset = CompileSEH(tags, offset, sehData);
    if (offset < tags.size() && tags[offset]->SEHType() == Instruction::seh_try)
    {
        current.tryLength = tags[offset]->Offset() - current.tryOffset;
        offset++;
    }
    else
    {
        // error
        return offset + 1;
    }
    do
    {
        if (!tags[offset]->SEHBegin())
            return offset;
        switch (tags[offset]->SEHType())
        {
            case Instruction::seh_try:
                return CompileSEH(tags, offset, sehData);
            case Instruction::seh_filter:
                // assumes there are no try catch block within a filter expression
                current.filterOffset = tags[offset]->Offset();
                offset += 2;
                current.flags = SEHData::Filter;
                break;
            case Instruction::seh_catch:
                if (tags[offset]->SEHCatchType()->GetClass()->InAssemblyRef())
                    current.classToken = tags[offset]->SEHCatchType()->GetClass()->PEIndex() + (tTypeRef << 24);
                else
                    current.classToken = tags[offset]->SEHCatchType()->GetClass()->PEIndex() + (tTypeDef << 24);
                current.flags = SEHData::Exception;
                break;
            case Instruction::seh_fault:
                current.flags = SEHData::Fault;
                break;
            case Instruction::seh_finally:
                current.flags = SEHData::Finally;
                break;
        }
        if (offset < tags.size())
        {
            current.handlerOffset = tags[offset]->Offset();
            offset++;
            while (offset < tags.size() && tags[offset]->SEHBegin())
                offset = CompileSEH(tags, offset, sehData);
            if (offset < tags.size())
            {
                current.handlerLength = tags[offset]->Offset() - current.handlerOffset;
                sehData.push_back(current);
            }
            offset++;
        }
    } while (offset < tags.size());
    return 1;
}
void CodeContainer::CompileSEH(PELib&, std::vector<SEHData>& sehData)
{
    std::vector<Instruction*> tags;
    for (auto instruction : instructions_)
    {
        if (instruction->OpCode() == Instruction::i_SEH)
        {
            tags.push_back(instruction);
        }
    }
    if (tags.size())
    {
        CompileSEH(tags, 0, sehData);
    }
}
void CodeContainer::BaseTypes(int& types) const
{
    if (!(types & DataContainer::baseIndexSystem))
    {
        for (auto instruction : instructions_)
        {
            Operand* op = instruction->GetOperand();
            if (op && op->OperandType() == Operand::t_value)
            {
                if (typeid(*op->GetValue()) == typeid(Value))
                {
                    Type* type = op->GetValue()->GetType();
                    if (typeid(*type) == typeid(BoxedType))
                    {
                        types |= DataContainer::baseIndexSystem;
                        break;
                    }
                }
            }
        }
    }
}
void CodeContainer::Optimize(PELib& peLib)
{
    LoadLabels();
    OptimizeLDC(peLib);
    OptimizeLDLOC(peLib);
    OptimizeLDARG(peLib);
    OptimizeBranch(peLib);
    labels.clear();
}
void CodeContainer::LoadLabels()
{
    for (auto instruction : instructions_)
    {
        if (instruction->OpCode() == Instruction::i_label)
        {
            if (labels[instruction->Label()] != 0)
            {
                throw PELibError(PELibError::DuplicateLabel, instruction->Label());
            }
            labels[instruction->Label()] = instruction;
        }
    }
}
void CodeContainer::OptimizeLDC(PELib& peLib)
{
    for (auto instruction : instructions_)
    {
        switch (instruction->OpCode())
        {
            Operand* operand;
            case Instruction::i_ldc_i4:
                //                case Instruction::i_ldc_i8:
                operand = instruction->GetOperand();
                if (operand->OperandType() == Operand::t_int)
                {
                    bool done = true;
                    longlong n;
                    static Instruction::iop ops[] = {Instruction::i_ldc_i4_M1, Instruction::i_ldc_i4_0, Instruction::i_ldc_i4_1,
                                                     Instruction::i_ldc_i4_2,  Instruction::i_ldc_i4_3, Instruction::i_ldc_i4_4,
                                                     Instruction::i_ldc_i4_5,  Instruction::i_ldc_i4_6, Instruction::i_ldc_i4_7,
                                                     Instruction::i_ldc_i4_8};
                    switch (n = operand->IntValue())
                    {
                        case -1:
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        case 6:
                        case 7:
                        case 8:
                            instruction->OpCode(ops[(int)n + 1]);
                            break;
                        default:
                            done = false;
                            if (n < 128 && n >= -128)
                            {
                                instruction->OpCode(Instruction::i_ldc_i4_s);
                            }
                            break;
                    }
                    if (done)
                    {
                        instruction->NullOperand(peLib);
                    }
                }
                break;
            default:
                break;
        }
    }
}
void CodeContainer::OptimizeLDLOC(PELib& peLib)
{
    for (auto instruction : instructions_)
    {
        switch (instruction->OpCode())
        {
            Value* v;
            Operand* operand;
            case Instruction::i_ldloc:
            case Instruction::i_ldloca:
            case Instruction::i_stloc:
                operand = instruction->GetOperand();
                v = operand->GetValue();
                if (v && typeid(*v) == typeid(Local))
                {
                    int index = ((Local*)v)->Index();
                    switch (instruction->OpCode())
                    {
                        case Instruction::i_ldloc:
                            if (index < 4)
                            {
                                static Instruction::iop ldlocs[] = {Instruction::i_ldloc_0, Instruction::i_ldloc_1,
                                                                    Instruction::i_ldloc_2, Instruction::i_ldloc_3};
                                instruction->OpCode(ldlocs[index]);
                                instruction->NullOperand(peLib);
                            }
                            else if (index < 128 && index >= -128)
                                instruction->OpCode(Instruction::i_ldloc_s);
                            break;
                        case Instruction::i_ldloca:
                            if (index < 128 && index >= -128)
                                instruction->OpCode(Instruction::i_ldloca_s);
                            break;
                        case Instruction::i_stloc:
                            if (index < 4)
                            {
                                static Instruction::iop stlocs[] = {Instruction::i_stloc_0, Instruction::i_stloc_1,
                                                                    Instruction::i_stloc_2, Instruction::i_stloc_3};
                                instruction->OpCode(stlocs[index]);
                                instruction->NullOperand(peLib);
                            }
                            else if (index < 128 && index >= -128)
                                instruction->OpCode(Instruction::i_stloc_s);
                            break;
                    }
                }
                break;
            default:
                break;
        }
    }
}
void CodeContainer::OptimizeLDARG(PELib& peLib)
{
    for (auto instruction : instructions_)
    {
        switch (instruction->OpCode())
        {
            Value* v;
            Operand* operand;
            case Instruction::i_ldarg:
            case Instruction::i_ldarga:
            case Instruction::i_starg:
                operand = instruction->GetOperand();
                v = operand->GetValue();
                if (v && typeid(*v) == typeid(Param))
                {
                    int index = ((Param*)v)->Index();
                    switch (instruction->OpCode())
                    {
                        case Instruction::i_ldarg:
                            if (index < 4)
                            {
                                static Instruction::iop ldargs[] = {Instruction::i_ldarg_0, Instruction::i_ldarg_1,
                                                                    Instruction::i_ldarg_2, Instruction::i_ldarg_3};
                                instruction->OpCode(ldargs[index]);
                                instruction->NullOperand(peLib);
                            }
                            else {
                                if (index < 128 && index >= -128)
                                {
                                    instruction->OpCode(Instruction::i_ldarg_s);
                                }
                                if (instruction->GetOperand()->OperandType() == Operand::t_value && instruction->GetOperand()->GetValue()->GetType() && instruction->GetOperand()->GetValue()->GetType()->GetBasicType() == Type::mvar)
                                {
                                    instruction->SetOperand(peLib.AllocateOperand(index, Operand::i32));
                                }
                            }
                            break;
                        case Instruction::i_ldarga:
                            if (index < 128 && index >= -128)
                            {
                                instruction->OpCode(Instruction::i_ldarga_s);
                            }
                            if (instruction->GetOperand()->OperandType() == Operand::t_value && instruction->GetOperand()->GetValue()->GetType() && instruction->GetOperand()->GetValue()->GetType()->GetBasicType() == Type::mvar)
                            {
                                instruction->SetOperand(peLib.AllocateOperand(index, Operand::i32));
                            }
                            break;
                        case Instruction::i_starg:
                            if (index < 128 && index >= -128)
                            {
                                instruction->OpCode(Instruction::i_starg_s);
                            }
                            if (instruction->GetOperand()->OperandType() == Operand::t_value && instruction->GetOperand()->GetValue()->GetType() && instruction->GetOperand()->GetValue()->GetType()->GetBasicType() == Type::mvar)
                            {
                                instruction->SetOperand(peLib.AllocateOperand(index, Operand::i32));
                            }
                            break;
                    }
                }
                break;
            default:
                break;
        }
    }
}
void CodeContainer::CalculateOffsets()
{
    int ofs = 0;
    for (auto instruction : instructions_)
    {
        instruction->Offset(ofs);
        ofs += instruction->InstructionSize();
    }
}
bool CodeContainer::ModifyBranches()
{
    bool rv = true;
    for (auto instruction : instructions_)
    {
        if (instruction->IsRel4())
        {
            int offset = instruction->Offset();
            auto it1 = labels.find(instruction->GetOperand()->StringValue());
            if (it1 != labels.end())
            {
                int loffset = it1->second->Offset();
                int diff = loffset - (offset + 5);
                if (diff < 128 && diff >= -128)
                {
                    rv = false;
                    instruction->Rel4To1();
                }
            }
        }
    }
    return rv;
}
void CodeContainer::ValidateInstructions()
{
    CalculateOffsets();
    for (auto instruction : instructions_)
    {
        if (instruction->IsBranch())
        {
            std::string branchLabel = instruction->GetOperand()->StringValue();
            if (labels[branchLabel] == 0)
            {
                throw PELibError(PELibError::MissingLabel, branchLabel);
            }
            else if (instruction->IsRel1())
            {
                int offset = instruction->Offset();
                auto it1 = labels.find(branchLabel);
                if (it1 != labels.end())
                {
                    int loffset = it1->second->Offset();
                    int diff = loffset - (offset + 2);
                    if (diff > 127 || diff < -128)
                    {
                        throw PELibError(PELibError::ShortBranchOutOfRange);
                    }
                }
            }
        }
        else
            switch (instruction->OpCode())
            {
                case Instruction::i_ldarg:
                case Instruction::i_ldarga:
                case Instruction::i_starg:
                    if (((Param*)instruction->GetOperand()->GetValue())->Index() > 65534)
                    {
                        throw PELibError(PELibError::IndexOutOfRange, ((Param*)instruction->GetOperand()->GetValue())->Name());
                    }
                    break;
                case Instruction::i_ldloc:
                case Instruction::i_ldloca:
                case Instruction::i_stloc:
                    if (((Local*)instruction->GetOperand()->GetValue())->Index() > 65534)
                    {
                        throw PELibError(PELibError::IndexOutOfRange, ((Local*)instruction->GetOperand()->GetValue())->Name());
                    }
                    break;
                case Instruction::i_ldarg_s:
                case Instruction::i_ldarga_s:
                case Instruction::i_starg_s:
                    if (((Param*)instruction->GetOperand()->GetValue())->Index() > 255)
                    {
                        throw PELibError(PELibError::IndexOutOfRange, ((Param*)instruction->GetOperand()->GetValue())->Name());
                    }
                    break;
                case Instruction::i_ldloc_s:
                case Instruction::i_ldloca_s:
                case Instruction::i_stloc_s:
                    if (((Local*)instruction->GetOperand()->GetValue())->Index() > 255)
                    {
                        throw PELibError(PELibError::IndexOutOfRange, ((Local*)instruction->GetOperand()->GetValue())->Name());
                    }
                    break;
                default:
                    break;
            }
    }
}
int CodeContainer::ValidateSEHTags(std::vector<Instruction*>& tags, int offset)
{
    int offset_in = offset;
    Instruction* start = tags[offset++];
    if (start->SEHType() != Instruction::seh_try)
        throw PELibError(PELibError::ExpectedSEHTry);
    if (!start->SEHBegin())
        throw PELibError(PELibError::ExpectedSEHTry);
    while (offset < tags.size() && tags[offset]->SEHBegin())
        offset = ValidateSEHTags(tags, offset);
    if (offset >= tags.size())
        throw PELibError(PELibError::OrphanedSEHTag);
    if (tags[offset]->SEHType() != Instruction::seh_try)
        throw PELibError(PELibError::MismatchedSEHTag);
    if (tags[offset]->SEHBegin())
        throw PELibError(PELibError::MismatchedSEHTag);
    offset++;
    if (!tags[offset]->SEHBegin() || tags[offset]->SEHType() == Instruction::seh_try)
        throw PELibError(PELibError::ExpectedSEHHandler);
    Instruction::iseh type;
    while (offset < tags.size())
    {
        if (!tags[offset]->SEHBegin())
            return offset;
        type = (Instruction::iseh)tags[offset]->SEHType();
        offset++;
        while (offset < tags.size() && tags[offset]->SEHBegin())
            offset = ValidateSEHTags(tags, offset);
        if (offset >= tags.size())
            throw PELibError(PELibError::OrphanedSEHTag);
        if ((Instruction::iseh)tags[offset]->SEHType() != type)
            throw PELibError(PELibError::MismatchedSEHTag);
        offset++;
    }
    return offset;
}
void CodeContainer::ValidateSEHFilters(std::vector<Instruction*>& tags)
{
    bool check = false;
    for (auto tag : tags)
    {
        if (!tag->SEHBegin() && tag->SEHType() == Instruction::seh_filter)
        {
            check = true;
        }
        if (check)
        {
            check = false;
            if (!tag->SEHBegin() || tag->SEHType() != Instruction::seh_filter_handler)
            {
                throw PELibError(PELibError::InvalidSEHFilter);
            }
        }
    }
}
void CodeContainer::ValidateSEHEpilogues()
{
    // we aren't checking the 'pop' here as it is up to the user when to handle that
    // but it will be caught in the normal process of level matching...
    Instruction *old = nullptr, *old1 = nullptr;
    for (auto instruction : instructions_)
    {
        if (instruction->OpCode() == Instruction::i_SEH)
        {
            if (!instruction->SEHBegin())
            {
                switch (instruction->SEHType())
                {
                    case Instruction::seh_try:
                        if (!old || (old->OpCode() != Instruction::i_leave && old->OpCode() != Instruction::i_leave_s))
                        {
                            throw PELibError(PELibError::InvalidSEHEpilogue);
                        }
                        break;
                    case Instruction::seh_catch:
                    case Instruction::seh_filter_handler:
                        if (!old || (old->OpCode() != Instruction::i_leave && old->OpCode() != Instruction::i_leave_s))
                        {
                            throw PELibError(PELibError::InvalidSEHEpilogue);
                        }
                        break;
                    case Instruction::seh_filter:
                        if (!old || old->OpCode() != Instruction::i_endfilter)
                        {
                            throw PELibError(PELibError::InvalidSEHEpilogue);
                        }
                        break;
                    case Instruction::seh_fault:
                        if (!old || old->OpCode() != Instruction::i_endfault)
                        {
                            throw PELibError(PELibError::InvalidSEHEpilogue);
                        }
                        break;
                    case Instruction::seh_finally:
                        if (!old || old->OpCode() != Instruction::i_endfinally)
                        {
                            throw PELibError(PELibError::InvalidSEHEpilogue);
                        }
                        break;
                }
            }
        }
        else if (instruction->OpCode() != Instruction::i_label && instruction->OpCode() != Instruction::i_comment)
        {
            old1 = old;
            old = instruction;
        }
    }
}
void CodeContainer::ValidateSEH()
{
    std::vector<Instruction*> tags;
    for (auto instruction : instructions_)
    {
        if (instruction->OpCode() == Instruction::i_SEH)
        {
            tags.push_back(instruction);
        }
    }
    if (tags.size())
    {
        hasSEH_ = true;
        ValidateSEHTags(tags, 0);
        ValidateSEHFilters(tags);
        ValidateSEHEpilogues();
    }
}
void CodeContainer::OptimizeBranch(PELib& peLib)
{
    bool done = false;
    ValidateSEH();
    while (!done)
    {
        CalculateOffsets();
        done = ModifyBranches();
    }
    ValidateInstructions();
}
}  // namespace DotNetPELib