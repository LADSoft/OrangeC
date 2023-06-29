/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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

#include <cstdio>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include "be.h"
#include "winmode.h"
#include "DotNetPELib.h"
#include "Utils.h"
#include <algorithm>
#include "memory.h"
#include "msilInit.h"
#include "MsilProcess.h"
#include "ioptimizer.h"

namespace occmsil
{
int funcindex = 1;
std::map<Optimizer::SimpleSymbol*, std::pair<Operand*, Operand*>> funcMap;
static Class* GetDelegateClass(char* name, Optimizer::LIST* syms, Optimizer::SimpleType* returntp, Operand*& ctor)
{
    Class* cls = peLib->AllocateClass(name, Qualifiers::Private | Qualifiers::Ansi | Qualifiers::Sealed, -1, -1);
    cls->Extends(multicastDelegate);
    mainContainer->Add(cls);
    MethodSignature* sig = peLib->AllocateMethodSignature(".ctor", MethodSignature::Managed | MethodSignature::InstanceFlag, cls);
    sig->ReturnType(peLib->AllocateType(Type::Void, 0));
    sig->AddParam(peLib->AllocateParam("object", peLib->AllocateType(Type::object, 0)));
    sig->AddParam(peLib->AllocateParam("method", peLib->AllocateType(Type::inative, 0)));
    ctor = peLib->AllocateOperand(peLib->AllocateMethodName(sig));
    Method* method = peLib->AllocateMethod(sig, Qualifiers::Public | Qualifiers::HideBySig | Qualifiers::SpecialName |
                                                    Qualifiers::RTSpecialName | Qualifiers::Managed | Qualifiers::Runtime);
    cls->Add(method);
    sig = peLib->AllocateMethodSignature("Invoke", MethodSignature::Managed | MethodSignature::InstanceFlag, cls);
    sig->ReturnType(GetType(returntp, true, true, true));
    char buf[256];
    Optimizer::LIST* list = syms;
    while (list)
    {
        Optimizer::SimpleType* type = ((Optimizer::SimpleSymbol*)list->data)->tp;
        if (type->type == Optimizer::st_void)
            break;
        sig->AddParam(peLib->AllocateParam("", GetType(type, true, true, true)));
        list = list->next;
    }
    method = peLib->AllocateMethod(sig, Qualifiers::Public | Qualifiers::HideBySig | Qualifiers::Virtual | Qualifiers::NewSlot |
                                            Qualifiers::Managed | Qualifiers::Runtime);
    cls->Add(method);
    sig = peLib->AllocateMethodSignature("BeginInvoke", MethodSignature::Managed | MethodSignature::InstanceFlag, cls);
    Type* iars;
    sig->ReturnType(iars = FindType("System.IAsyncResult", true));
    list = syms;
    while (list)
    {
        Optimizer::SimpleType* type = ((Optimizer::SimpleSymbol*)list->data)->tp;
        if (type->type == Optimizer::st_void)
            break;
        sig->AddParam(peLib->AllocateParam("", GetType(type, true, true, true)));
        list = list->next;
    }
    sig->AddParam(peLib->AllocateParam("callback", FindType("System.AsyncCallback", true)));
    sig->AddParam(peLib->AllocateParam("object", peLib->AllocateType(Type::object, 0)));
    method = peLib->AllocateMethod(sig, Qualifiers::Public | Qualifiers::HideBySig | Qualifiers::Virtual | Qualifiers::NewSlot |
                                            Qualifiers::Managed | Qualifiers::Runtime);
    cls->Add(method);
    sig = peLib->AllocateMethodSignature("EndInvoke", MethodSignature::Managed | MethodSignature::InstanceFlag, cls);
    sig->ReturnType(peLib->AllocateType(Type::Void, 0));
    sig->AddParam(peLib->AllocateParam("result", iars));
    method = peLib->AllocateMethod(sig, Qualifiers::Public | Qualifiers::HideBySig | Qualifiers::Virtual | Qualifiers::NewSlot |
                                            Qualifiers::Managed | Qualifiers::Runtime);
    cls->Add(method);
    return cls;
}
Operand* GetDelegateAllocator(Optimizer::SimpleType* tp, Operand*& ctor)
{
    auto pair = funcMap[tp->sp];
    Operand* rv = pair.first;
    ctor = pair.second;
    if (!rv)
    {
        char buf[256];
        sprintf(buf, "__funcname_%d", funcindex++);
        Class* cls = GetDelegateClass(buf, tp->sp->syms, tp->btp, ctor);
        MethodSignature* newsig = delegateAllocator;
        newsig = peLib->AllocateMethodSignature(newsig);
        newsig->GenericParent(delegateAllocator);
        newsig->Generic().push_back(peLib->AllocateType(cls));
        rv = peLib->AllocateOperand(peLib->AllocateMethodName(newsig));
        funcMap[tp->sp] = std::pair<Operand*, Operand*>(rv, ctor);
    }
    return rv;
}
Operand* GetDelegateInvoker(Optimizer::SimpleSymbol* sp)
{
    MethodSignature* sig = peLib->AllocateMethodSignature(delegateInvoker);
    Optimizer::LIST* lst = sp->syms;
    while (lst)
    {
        Optimizer::SimpleType* type = ((Optimizer::SimpleSymbol*)lst->data)->tp;
        if (type->type == Optimizer::st_void)
            break;
        sig->AddVarargParam(peLib->AllocateParam("", GetType(type, true, true, true)));
        lst = lst->next;
    }
    MethodSignature* oldsig = FindPInvokeWithVarargs(sig->Name(), sig->vbegin(), sig->vend(), sig->VarargParamCount());
    if (oldsig)
    {
        sig = oldsig;
    }
    else
    {
        sig->SignatureParent(delegateInvoker);
        pInvokeReferences.insert(std::pair<std::string, MethodSignature*>(sig->Name(), sig));
    }
    return peLib->AllocateOperand(peLib->AllocateMethodName(sig));
}
}  // namespace occmsil
