/*
    Software License Agreement (BSD License)

    Copyright (c) 2016, David Lindauer, (LADSoft).
    All rights reserved.

    Redistribution and use of this software in source and binary forms,
    with or without modification, are permitted provided that the following
    conditions are met:

    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.

    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.

    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#include "DotNetPELib.h"
#include <typeinfo>
namespace DotNetPELib
{
    void CodeContainer::AddInstruction(Instruction *instruction)
    {
        instructions_.push_back(instruction);
    }

    bool CodeContainer::ILSrcDump(PELib &peLib) const
    {
        for (auto instruction : instructions_)
            instruction->ILSrcDump(peLib);
        return true;
    }
    Byte * CodeContainer::Compile(PELib &peLib, size_t &sz)
    {
        Byte *rv = nullptr;
        CalculateOffsets();
        LoadLabels();
        Instruction *last = instructions_.back();
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
    void CodeContainer::BaseTypes(int &types) const
    {
        if (!(types & DataContainer::baseIndexSystem))
        {
            for (auto instruction : instructions_)
            {
                Operand *op = instruction->GetOperand();
                if (op && op->OperandType() == Operand::t_value)
                {
                    if (typeid(*op->GetValue()) == typeid(Value))
                    {
                        Type * type = op->GetValue()->GetType();
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
    void CodeContainer::Optimize(PELib &peLib)
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
    void CodeContainer::OptimizeLDC(PELib &peLib)
    {
        for (auto instruction : instructions_)
        {
            switch (instruction->OpCode())
            {
                Operand *operand;
            case Instruction::i_ldc_i4:
                //                case Instruction::i_ldc_i8:
                operand = instruction->GetOperand();
                if (operand->OperandType() == Operand::t_int)
                {
                    bool done = true;
                    longlong n;
                    static Instruction::iop ops[] = { Instruction::i_ldc_i4_M1, Instruction::i_ldc_i4_0,
                                Instruction::i_ldc_i4_1,Instruction::i_ldc_i4_2,Instruction::i_ldc_i4_3,Instruction::i_ldc_i4_4,
                                Instruction::i_ldc_i4_5, Instruction::i_ldc_i4_6, Instruction::i_ldc_i4_7, Instruction::i_ldc_i4_8 };
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
            }
        }
    }
    void CodeContainer::OptimizeLDLOC(PELib &peLib)
    {
        for (auto instruction : instructions_)
        {
            switch (instruction->OpCode())
            {
                Value *v;
                Operand *operand;
            case Instruction::i_ldloc:
            case Instruction::i_ldloca:
            case Instruction::i_stloc:
                operand = instruction->GetOperand();
                v = operand->GetValue();
                if (v && typeid(*v) == typeid(Local))
                {
                    int index = ((Local *)v)->Index();
                    switch (instruction->OpCode())
                    {
                    case Instruction::i_ldloc:
                        if (index < 4)
                        {
                            static Instruction::iop ldlocs[] = { Instruction::i_ldloc_0, Instruction::i_ldloc_1, Instruction::i_ldloc_2, Instruction::i_ldloc_3 };
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
                            static Instruction::iop stlocs[] = { Instruction::i_stloc_0, Instruction::i_stloc_1, Instruction::i_stloc_2, Instruction::i_stloc_3 };
                            instruction->OpCode(stlocs[index]);
                            instruction->NullOperand(peLib);
                        }
                        else if (index < 128 && index >= -128)
                            instruction->OpCode(Instruction::i_stloc_s);
                        break;
                    }
                }
                break;
            }
        }
    }
    void CodeContainer::OptimizeLDARG(PELib &peLib)
    {
        for (auto instruction : instructions_)
        {
            switch (instruction->OpCode())
            {
                Value *v;
                Operand *operand;
            case Instruction::i_ldarg:
            case Instruction::i_ldarga:
            case Instruction::i_starg:
                operand = instruction->GetOperand();
                v = operand->GetValue();
                if (v && typeid(*v) == typeid(Param))
                {
                    int index = ((Param *)v)->Index();
                    switch (instruction->OpCode())
                    {
                    case Instruction::i_ldarg:
                        if (index < 4)
                        {
                            static Instruction::iop ldargs[] = { Instruction::i_ldarg_0, Instruction::i_ldarg_1, Instruction::i_ldarg_2, Instruction::i_ldarg_3 };
                            instruction->OpCode(ldargs[index]);
                            instruction->NullOperand(peLib);
                        }
                        else if (index < 128 && index >= -128)
                            instruction->OpCode(Instruction::i_ldarg_s);
                        break;
                    case Instruction::i_ldarga:
                        if (index < 128 && index >= -128)
                            instruction->OpCode(Instruction::i_ldarga_s);
                        break;
                    case Instruction::i_starg:
                        if (index < 128 && index >= -128)
                            instruction->OpCode(Instruction::i_starg_s);
                        break;
                    }
                }
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
            else switch (instruction->OpCode())
            {
            case Instruction::i_ldarg:
            case Instruction::i_ldarga:
            case Instruction::i_starg:
                if (((Param *)instruction->GetOperand()->GetValue())->Index() > 65534)
                {
                    throw PELibError(PELibError::IndexOutOfRange, ((Param *)instruction->GetOperand()->GetValue())->Name());
                }
                break;
            case Instruction::i_ldloc:
            case Instruction::i_ldloca:
            case Instruction::i_stloc:
                if (((Local *)instruction->GetOperand()->GetValue())->Index() > 65534)
                {
                    throw PELibError(PELibError::IndexOutOfRange, ((Local *)instruction->GetOperand()->GetValue())->Name());
                }
                break;
            case Instruction::i_ldarg_s:
            case Instruction::i_ldarga_s:
            case Instruction::i_starg_s:
                if (((Param *)instruction->GetOperand()->GetValue())->Index() > 255)
                {
                    throw PELibError(PELibError::IndexOutOfRange, ((Param *)instruction->GetOperand()->GetValue())->Name());
                }
                break;
            case Instruction::i_ldloc_s:
            case Instruction::i_ldloca_s:
            case Instruction::i_stloc_s:
                if (((Local *)instruction->GetOperand()->GetValue())->Index() > 255)
                {
                    throw PELibError(PELibError::IndexOutOfRange, ((Local *)instruction->GetOperand()->GetValue())->Name());
                }
                break;
            }
        }
    }
    void CodeContainer::OptimizeBranch(PELib &peLib)
    {
        bool done = false;
        while (!done)
        {
            CalculateOffsets();
            done = ModifyBranches();
        }
        ValidateInstructions();
    }
}