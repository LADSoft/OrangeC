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
#include <search.h>

namespace DotNetPELib
{    
    Method::Method(MethodSignature *Prototype, Qualifiers flags, bool entry) 
        :CodeContainer(flags), prototype(Prototype), maxStack(100), 
        invokeMode(CIL), pInvokeType(Stdcall), entryPoint(entry)
    {
        if (!(flags.flags & Qualifiers::Static))
            prototype->SetInstanceFlag();
    }
    void Method::AddLocal(Local *local)
    {
        local->SetIndex(varList.size());
        varList.push_back(local);
    }

    bool Method::ILSrcDump(PELib &peLib)
    {
        peLib.Out() << ".method";
        flags.ILSrcDumpBeforeFlags(peLib);
        if (invokeMode == PInvoke)
        {
            peLib.Out() << " pinvokeimpl(\"" << pInvokeName << "\" " 
                        << (pInvokeType == Cdecl ? "cdecl) " : "stdcall) ");
            
        }
        else
        {
            peLib.Out() << " ";
        }
        prototype->ILSrcDump(peLib, invokeMode != PInvoke, false, invokeMode == PInvoke);
        flags.ILSrcDumpAfterFlags(peLib);
        if (invokeMode != PInvoke)
        {
            peLib.Out() << "{" << std::endl;
            if ((prototype->GetFlags() & MethodSignature::Vararg) && (prototype->GetFlags() & MethodSignature::Managed))
            {
                // allow C# to use ...
                peLib.Out() << "\t.param\t[" << prototype->GetParamCount() << "]" << std::endl;
                peLib.Out() << "\t.custom instance void [mscorlib]System.ParamArrayAttribute::.ctor() = ( 01 00 00 00 )" << std::endl; 
            }
            if (varList.size())
            {
                peLib.Out() << "\t.locals (" << std::endl;
                for (std::list<Local *>::iterator it = varList.begin(); it != varList.end();)
                {

                    peLib.Out() << "\t\t[" << (*it)->GetIndex() << "]\t";
                    if ((*it)->GetType()->GetBasicType() == Type::cls)
                    {
                        if ((*it)->GetType()->GetClass()->GetFlags().flags & Qualifiers::Value)
                            peLib.Out() << "valuetype ";
                        else
                            peLib.Out() << "class ";
                    }
                    (*it)->GetType()->ILSrcDump(peLib);
                    peLib.Out() << " ";
                    (*it)->ILSrcDump(peLib);
                    ++it ;
                    if (it != varList.end())
                        peLib.Out() << "," << std::endl;
                    else
                        peLib.Out() << std::endl;
                }
                peLib.Out() << "\t)" << std::endl;
            }
            if (entryPoint)
                peLib.Out() << "\t.entrypoint" << std::endl;
            peLib.Out() << "\t.maxstack " << maxStack << std::endl << std::endl;
            CodeContainer::ILSrcDump(peLib);
            peLib.Out() << "}" << std::endl;
        }
        else
        {
            peLib.Out() << "{}" << std::endl;
        }
        return true;
    }
    void Method::Optimize(PELib &peLib)
    {
        CalculateMaxStack();
        OptimizeLocals(peLib );
        CodeContainer::Optimize(peLib);
    }
    void Method::CalculateMaxStack()
    {
        std::map<std::string, int> labels;
        maxStack = 0;
        int n = 0;
        bool lastBranch = false;
        for (std::list<Instruction *>::iterator it = instructions.begin(); it != instructions.end(); ++it)
        {
            int m = (*it)->GetStackUsage();
            if (m == -127)
                n = 0;
            else
                n += m;
            if (n > maxStack)
                maxStack = n;
            if (n <0 )
            {
                throw PELibError(PELibError::StackUnderflow);
            }
            if ((*it)->IsBranch())
            {
                lastBranch = true;
                std::map<std::string, int>::iterator it1 = labels.find((*it)->GetOperand()->GetStringValue());
                if (it1 != labels.end())
                {
                    if (it1->second != n)
                    {
                        throw PELibError(PELibError::MismatchedStack, (*it)->GetOperand()->GetStringValue());
                    }
                }
                else
                {
                    labels[(*it)->GetOperand()->GetStringValue()] = n;
                }

            }
            else if ((*it)->GetOp() == Instruction::i_switch)
            {
                if ((*it)->GetSwitches())
                {
                    for (std::list<std::string>::iterator its = (*it)->GetSwitches()->begin(); its != (*it)->GetSwitches()->end(); ++its)
                    {
                        std::map<std::string, int>::iterator it1 = labels.find(*its);
                        if (it1 != labels.end())
                        {
                            if (it1->second != n)
                            {
                                throw PELibError(PELibError::MismatchedStack, *its);
                            }
                        }
                        else
                        {
                            labels[*its] = n;
                        }
                    }
                }
            }
            else if ((*it)->GetOp() == Instruction::i_label)
            {
                if (lastBranch)
                {
                    std::map<std::string, int>::iterator it1 = labels.find((*it)->GetText());
                    if (it1 != labels.end())
                    {
                        n = it1->second;
                    }
                    else
                    {
                        n = 0;
                    }
                }
                else
                {
                    std::map<std::string, int>::iterator it1 = labels.find((*it)->GetText());
                    if (it1 != labels.end())
                    {
                        if (it1->second != n)
                        {
                            throw PELibError(PELibError::MismatchedStack, (*it)->GetText());
                        }
                    }
                    else
                    {
                        labels[(*it)->GetText()] = n;
                    }
                }

            }
            else if ((*it)->GetOp() == Instruction::i_comment)
            {
                // placeholder
            }
            else
            {
                lastBranch = false;
            }
        }
        if (n != 0)
        {
            if (n != 1 || prototype->GetReturnType()->IsVoid())
                throw PELibError(PELibError::StackNotEmpty, " at end of function");
        }
    }
    static int localCompare(const void *left, const void *right)
    {
        Local **l = (Local **)left;
        Local **r = (Local **)right;
        // sort from highest to lowest
        if ((*l)->Uses() > (*r)->Uses())
            return -1;
        return (*l)->Uses() < (*r)->Uses();
    }
    void Method::OptimizeLocals(PELib &peLib)
    {
        for (std::list<Instruction *>::iterator it = instructions.begin(); it != instructions.end(); ++it)
        {
            Operand *op = (*it)->GetOperand();
            if (op)
            {
                Value *v = op->GetValue();
                if (v && typeid(*v) == typeid(Local))
                {
                    ((Local *)v)->IncrementUses();
                }
            }
        }
        int n = varList.size();
        Local **vars = new Local *[n];
        int i = 0;
        for (std::list<Local *>::iterator it = varList.begin(); it != varList.end(); ++it, ++i)
        {
            vars[i] = *it;
        }
        varList.clear();
        qsort(vars, n, sizeof(Local *), localCompare);
        for (int i=0; i < n; i++)
        {
            vars[i]->SetIndex(i);
            varList.push_back(vars[i]);
        }
        delete [] vars;
    }
}