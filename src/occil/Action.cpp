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
 *     contact information:b
 *         email: TouchStone222@runbox.com <David Lindauer>
 *
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "be.h"
#include "winmode.h"
#include "DotNetPELib.h"
#include "Utils.h"
#include <algorithm>
#include "memory.h"
#include "msilInit.h"
#include "MsilProcess.h"

namespace occmsil
{
    struct ActionData
    {
        ActionData(Type* type) :typeName(type), invoker(nullptr), ctor(nullptr) { }
        Type *typeName;
        Operand* invoker;
        Operand* ctor;
    };
    static std::map<std::string, ActionData*> actionMap;

    static const char *GetActionName(Optimizer::SimpleType* type)
    {
        static char buf[256];
        int count = 0;
        std::string name;
        if (type->btp->type == Optimizer::st_void)
        {
            name = "Action";
        }
        else
        {
            count++;
            name = "Func";
        }
        for (auto ptr = type->sp->syms; ptr != nullptr && ((Optimizer::SimpleSymbol*)ptr->data)->tp->type != Optimizer::st_void; ptr = ptr->next)
        {
            count++;
        }
        sprintf(buf, "System.%s", name.c_str());
        if (count != 0)
        {
            sprintf(buf + strlen(buf), "`%d", count);
        }
        return buf;
    }
    static std::string AdornWithTypes(std::string name, Optimizer::SimpleType* type)
    {
        return name;
    }
    static void InterjectType(Type *rv, Optimizer::SimpleType* type)
    {
        if (type->type != Optimizer::st_void)
        {
             
            Type* val;
            if (type->type == Optimizer::st_pointer)
                val = intPtr;
            else
                val = GetType(type, true);
            static_cast<Class*>(rv->GetClass())->Generic().push_back(val);
        }
    }
    static Type* AddGenerics(Type* rv, Optimizer::SimpleType* type)
    {
        if (rv->GetClass()->Name().find("`") != std::string::npos)
        {
            Class* cls = peLib->AllocateClass(static_cast<Class*>(rv->GetClass()));
            cls->GenericParent(static_cast<Class*>(rv->GetClass()));
            rv = peLib->AllocateType(cls);
            for (auto v = type->sp->syms; v; v = v->next)
            {
                InterjectType(rv, ((Optimizer::SimpleSymbol*)(v->data))->tp);
            }
            InterjectType(rv, type->btp);
        }
        return rv;
    }
    static Operand* FunctionOperand(Type* genericType, std::string objectName, bool ctor)
    {
        MethodSignature *sig;
        if (ctor)
        {
            sig = FindMethodSignature((objectName + "::.ctor").c_str());
        }
        else
        {
            sig = FindMethodSignature((objectName + "::Invoke").c_str());

        }
        if (genericType->GetClass()->Name().find("`") != std::string::npos)
        {
            sig = peLib->AllocateMethodSignature(sig);
            sig->SetContainer(genericType->GetClass());
        }
        return peLib->AllocateOperand(peLib->AllocateMethodName(sig));
    }
    Type* LookupActionType(Optimizer::SimpleType* type)
    {
        while (type->type == Optimizer::st_pointer)
            type = type->btp;
        std::string name;
        const char *p = GetActionName(type);
        std::string newName = AdornWithTypes(p, type);
        auto rv = actionMap[newName];
        if (rv == nullptr)
        {
            rv = new ActionData(FindType(p, false));
            if (!rv->typeName)
            {
                Utils::fatal("Could not find %s", p);
            }
            rv->typeName = AddGenerics(rv->typeName, type);
            actionMap[newName] = rv;
        }
        return rv->typeName;
    }
    Operand* LookupActionCtor(Optimizer::SimpleType* type)
    {
        while (type->type == Optimizer::st_pointer)
            type = type->btp;
        std::string name;
        const char *p = GetActionName(type);
        std::string newName = AdornWithTypes(p, type);
        auto rv = actionMap[newName];
        if (!rv)
        {
            LookupActionType(type);
            rv = actionMap[newName];
        }
        if (!rv->ctor)
        {
            rv->ctor = FunctionOperand(rv->typeName, newName, true);
        }
        return rv->ctor;
    }
    Operand* LookupActionInvoker(Optimizer::SimpleType* type)
    {
        while (type->type == Optimizer::st_pointer)
            type = type->btp;
        std::string name;
        const char *p = GetActionName(type);
        std::string newName = AdornWithTypes(p, type);
        auto rv = actionMap[newName];
        if (!rv)
        {
            LookupActionType(type);
            rv = actionMap[newName];
        }
        if (!rv->invoker)
        {
            rv->invoker = FunctionOperand(rv->typeName, newName, false);
        }
        return rv->invoker;
    }
}  // namespace occmsil