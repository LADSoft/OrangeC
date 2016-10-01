/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2011, David Lindauer, (LADSoft).
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
namespace DotNetPELib
{
    void CodeContainer::AddInstruction(Instruction *instruction)
    {
        if (instruction->GetOp() == Instruction::i_label)
        {
            if (labels[instruction->GetText()] != NULL)
            {
                throw PELibError(PELibError::DuplicateLabel, instruction->GetText());
            }
            labels[instruction->GetText()] = instruction;
        }
        instructions.push_back(instruction);
    }
    
    bool CodeContainer::ILSrcDump(PELib &peLib)
    {
        for (std::list<Instruction *>::iterator it = instructions.begin(); it != instructions.end(); ++it)
            (*it)->ILSrcDump(peLib);
        return true;
    }
    void CodeContainer::Optimize(PELib &peLib)
    {
        OptimizeLDC(peLib);
        OptimizeLDLOC(peLib);
        OptimizeLDARG(peLib);
        OptimizeBranch(peLib);
        labels.clear();
    }
    void CodeContainer::OptimizeLDC(PELib &peLib)
    {
        for (std::list<Instruction *>::iterator it = instructions.begin(); it != instructions.end(); ++it)
        {
            switch ((*it)->GetOp())
            {
                Operand *operand;
                case Instruction::i_ldc_i4:
                case Instruction::i_ldc_i8:
                    operand = (*it)->GetOperand();
                    if (operand->GetType() == Operand::t_int)
                    {
                        bool done = true;
                        longlong n;
                        static Instruction::iop ops[] = { Instruction::i_ldc_i4_M1, Instruction::i_ldc_i4_0,
                                    Instruction::i_ldc_i4_1,Instruction::i_ldc_i4_2,Instruction::i_ldc_i4_3,Instruction::i_ldc_i4_4,
                                    Instruction::i_ldc_i4_5, Instruction::i_ldc_i4_6, Instruction::i_ldc_i4_7, Instruction::i_ldc_i4_8 };
                        switch (n = operand->GetIntValue())
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
                                (*it)->SetOp(ops[(int)n + 1]);
                                break;
                            default:
                                done = false;
                                if (n < 128 && n >= -128)
                                {
                                    (*it)->SetOp(Instruction::i_ldc_i4_s);
                                }
                                break;

                        }
                        if (done)
                        {
                            (*it)->NullOperand(peLib);
                        }
                    }
            }
        }
    }
    void CodeContainer::OptimizeLDLOC(PELib &peLib)
    {
        for (std::list<Instruction *>::iterator it = instructions.begin(); it != instructions.end(); ++it)
        {
            switch ((*it)->GetOp())
            {
                Value *v;
                Operand *operand;
                case Instruction::i_ldloc:
                case Instruction::i_ldloca:
                case Instruction::i_stloc:
                    operand = (*it)->GetOperand();
                    v = operand->GetValue();
                    if (v && typeid(*v) == typeid(Local))
                    {
                        int index = ((Local *)v)->GetIndex();
                        switch ((*it)->GetOp())
                        {
                            case Instruction::i_ldloc:
                                if (index < 4)
                                {
                                    static Instruction::iop ldlocs[] = { Instruction::i_ldloc_0, Instruction::i_ldloc_1, Instruction::i_ldloc_2, Instruction::i_ldloc_3 };
                                    (*it)->SetOp(ldlocs[index]);
                                    (*it)->NullOperand(peLib);                            
                                }
                                else if (index < 128 && index >= -128)
                                    (*it)->SetOp(Instruction::i_ldloc_s);
                                break;
                            case Instruction::i_ldloca:
                                if (index < 128 && index >= -128)
                                    (*it)->SetOp(Instruction::i_ldloca_s);
                                break;
                            case Instruction::i_stloc:
                                if (index < 4)
                                {
                                    static Instruction::iop stlocs[] = { Instruction::i_stloc_0, Instruction::i_stloc_1, Instruction::i_stloc_2, Instruction::i_stloc_3 };
                                    (*it)->SetOp(stlocs[index]);
                                    (*it)->NullOperand(peLib);                            
                                }
                                else if (index < 128 && index >= -128)
                                    (*it)->SetOp(Instruction::i_stloc_s);
                                break;
                        }
                    }
                    break;
            }
        }
    }
    void CodeContainer::OptimizeLDARG(PELib &peLib)
    {
        for (std::list<Instruction *>::iterator it = instructions.begin(); it != instructions.end(); ++it)
        {
            switch ((*it)->GetOp())
            {
                Value *v;
                Operand *operand;
                case Instruction::i_ldarg:
                case Instruction::i_ldarga:
                case Instruction::i_starg:
                    operand = (*it)->GetOperand();
                    v = operand->GetValue();
                    if (v && typeid(*v) == typeid(Param))
                    {
                        int index = ((Param *)v)->GetIndex();
                        switch ((*it)->GetOp())
                        {
                            case Instruction::i_ldarg:
                                if (index < 4)
                                {
                                    static Instruction::iop ldargs[] = { Instruction::i_ldarg_0, Instruction::i_ldarg_1, Instruction::i_ldarg_2, Instruction::i_ldarg_3 };
                                    (*it)->SetOp(ldargs[index]);
                                    (*it)->NullOperand(peLib);                            
                                }
                                else if (index < 128 && index >= -128)
                                    (*it)->SetOp(Instruction::i_ldarg_s);
                                break;
                            case Instruction::i_ldarga:
                                if (index < 128 && index >= -128)
                                    (*it)->SetOp(Instruction::i_ldarga_s);
                                break;
                            case Instruction::i_starg:
                                if (index < 128 && index >= -128)
                                    (*it)->SetOp(Instruction::i_starg_s);
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
        for (std::list<Instruction *>::iterator it = instructions.begin(); it != instructions.end(); ++it)
        {
            (*it)->SetOffset(ofs);
            ofs += (*it)->GetInstructionSize();
        }

    }
    bool CodeContainer::ModifyBranches()
    {
        bool rv = true;
        for (std::list<Instruction *>::iterator it = instructions.begin(); it != instructions.end(); ++it)
        {
            if ((*it)->IsRel4())
            {
                int offset = (*it)->GetOffset();
                std::map<std::string, Instruction *>::iterator it1 = labels.find((*it)->GetOperand()->GetStringValue());
                if (it1 != labels.end())
                {
                    int loffset = it1->second->GetOffset();
                    int diff = loffset - (offset +5);
                    if (diff < 128 && diff >= - 128)
                    {
                        rv = false;
                        (*it)->Rel4To1();
                    }
                }
            }
        }        
        return rv;
    }
    void CodeContainer::ValidateInstructions()
    {
        CalculateOffsets();
        for (std::list<Instruction *>::iterator it = instructions.begin(); it != instructions.end(); ++it)
        {
            if ((*it)->IsBranch())
            {
                std::string branchLabel = (*it)->GetOperand()->GetStringValue();
                if (labels[branchLabel] == NULL)
                {
                    throw PELibError(PELibError::MissingLabel, branchLabel);
                }
                else if ((*it)->IsRel1())
                {
                    int offset = (*it)->GetOffset();
                    std::map<std::string, Instruction *>::iterator it1 = labels.find(branchLabel);
                    if (it1 != labels.end())
                    {
                        int loffset = it1->second->GetOffset();
                        int diff = loffset - (offset +2);
                        if (diff > 127 || diff < - 128)
                        {
                            throw PELibError(PELibError::ShortBranchOutOfRange);
                        }
                    }
                }
            }
            else switch ((*it)->GetOp())
            {
                case Instruction::i_ldarg:
                case Instruction::i_ldarga:
                case Instruction::i_starg:
                    if (((Param *)(*it)->GetOperand()->GetValue())->GetIndex() > 65534)
                    {
                        throw PELibError(PELibError::IndexOutOfRange, ((Param *)(*it)->GetOperand()->GetValue())->GetName());
                    }
                    break;
                case Instruction::i_ldloc:
                case Instruction::i_ldloca:
                case Instruction::i_stloc:
                    if (((Local *)(*it)->GetOperand()->GetValue())->GetIndex() > 65534)
                    {
                        throw PELibError(PELibError::IndexOutOfRange, ((Local *)(*it)->GetOperand()->GetValue())->GetName());
                    }
                    break;
                case Instruction::i_ldarg_s:
                case Instruction::i_ldarga_s:
                case Instruction::i_starg_s:
                    if (((Param *)(*it)->GetOperand()->GetValue())->GetIndex() > 255)
                    {
                        throw PELibError(PELibError::IndexOutOfRange, ((Param *)(*it)->GetOperand()->GetValue())->GetName());
                    }
                    break;
                case Instruction::i_ldloc_s:
                case Instruction::i_ldloca_s:
                case Instruction::i_stloc_s:
                    if (((Local *)(*it)->GetOperand()->GetValue())->GetIndex() > 255)
                    {
                        throw PELibError(PELibError::IndexOutOfRange, ((Local *)(*it)->GetOperand()->GetValue())->GetName());
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