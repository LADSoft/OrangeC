/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
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

#include "Runner.h"
#include "Depends.h"
#include "Eval.h"
#include "utils.h"
#include "Variable.h"
#include <fstream>
#include <list>
#include <stdlib.h>
#include <iostream>

void Runner::DeleteOne(Depends *depend)
{
    for (auto d : *depend)
    {
        DeleteOne(d);
    }
    if (depend->ShouldDelete())
        OS::RemoveFile(depend->GetGoal());	
}
int Runner::RunOne(Depends *depend, EnvironmentStrings &env, bool keepGoing)
{
    int rv = 0;
    RuleList *rl = depend->GetRuleList();
    Eval::PushruleStack(rl);
    if (!rl->GetSpawner())
    {
        bool stop = false;
        bool cantbuild = false;
        for (auto d : *depend)
        {
            int rv1;
            if ((rv1 = RunOne(d, env, keepGoing)))
            {
                if (rv <= 0 && rv1 != 0)
                    rv = rv1;
                if (rv > 0)
                {
                    stop = true;
                    if (!keepGoing)
                    {
                        Eval::PopruleStack();
                        return rv;
                    }
                }
            }
        }
        if (stop)
        {
            Eval::PopruleStack();
            return 1;
        }
        if (rv < 0)
        {
            Eval::PopruleStack();
            return -1;
        }
        if (cantbuild || rl->IsBuilt())
        {
            Eval::PopruleStack();
            return 0;
        }
        if (touch)
        {
            rl->Touch(OS::GetCurrentTime());
        }
        bool sil = silent;
        bool ig = ignoreResults;
        bool precious = false;
        bool make = RuleContainer::Instance()->OnList(depend->GetGoal(), ".RECURSIVE");
        bool oneShell = RuleContainer::Instance()->Lookup(".ONESHELL") != nullptr;
        bool posix = RuleContainer::Instance()->Lookup(".POSIX") != nullptr;
        if (!sil)
            sil = RuleContainer::Instance()->OnList(depend->GetGoal(), ".SILENT") || RuleContainer::Instance()->NoList(".SILENT");
        if (!ig)
            ig = RuleContainer::Instance()->OnList(depend->GetGoal(), ".IGNORE") || RuleContainer::Instance()->NoList(".IGNORE");
        if (depend->GetRule())
        {
            ig |= depend->GetRule()->IsIgnore();
            sil |= depend->GetRule()->IsSilent();
            precious = depend->GetRule()->IsPrecious();
            make |= depend->GetRule()->IsMake();
            if (precious)
                depend->Precious();
        }
        if (depend->GetRule() && depend->GetRule()->GetCommands())
        {
            Spawner *sp = new Spawner(env, ig, sil, oneShell, posix, displayOnly && !make, keepResponseFiles);
            rl->SetSpawner(sp);
            sp->Run(*depend->GetRule()->GetCommands(), outputType, rl, nullptr);
            rv = -1;
        }
        else
        {
            rl->SetSpawner((Spawner *)-1);
            rl->SetBuilt();
            rv = 0;
        }        
    }
    else if (rl->GetSpawner() == (Spawner *)-1)
    {
        rv = 0;
        rl->SetBuilt();
    }
    else if (!rl->GetSpawner()->IsDone())
    {
        rv = -1;
    }
    else
    {
        rl->SetBuilt();
        rv = rl->GetSpawner()->RetVal();
        if (rv)
        {
            std::string b = Utils::NumberToString(rv);
            if (RuleContainer::Instance()->Lookup(".DELETE_ON_ERROR"))
            {
                OS::RemoveFile(depend->GetGoal());
                std::cout << std::endl;
                Eval::error("commands returned error code " + b + " '" + depend->GetGoal()  + "' removed", depend->GetRule()->GetCommands()->GetFile(), depend->GetRule()->GetCommands()->GetLine());
            }
            else
            {
                std::cout << std::endl;
                Eval::error("commands returned error code " + b, depend->GetRule()->GetCommands()->GetFile(), depend->GetRule()->GetCommands()->GetLine());
            }
        }
        delete rl->GetSpawner();
        rl->SetSpawner((Spawner *)-1);
    }
    Eval::PopruleStack();
    return rv;	
}
void Runner::CancelOne(Depends *depend)
{
    for (auto d : *depend)
    {
        CancelOne(d);
    }
    std::string path = filePaths[depend->GetGoal()];
    if (path.size() != 0)
    {
        Variable *v = VariableContainer::Instance()->Lookup("GPATH");
        if (v)
        {
            std::string t = v->GetValue();
            if (v->GetFlavor() == Variable::f_recursive)
            {
                Eval e(t, false);
                t = e.Evaluate();
            }
            if (!RuleContainer::Instance()->ScanList(t, path))
                    filePaths[depend->GetGoal()] = "";		
        }
//		else
//			filePaths[depend->GetGoal()] = "";		
    }
}
