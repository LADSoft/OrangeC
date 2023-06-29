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

#include "MakeMain.h"
#include "Runner.h"
#include "Depends.h"
#include "Eval.h"
#include "Utils.h"
#include "Variable.h"
#include <fstream>
#include <list>
#include <cstdlib>
#include <iostream>

void Runner::DeleteOne(Depends* depend)
{
    for (auto& d : *depend)
    {
        DeleteOne(d.get());
    }
    if (depend->ShouldDelete())
        OS::RemoveFile(depend->GetGoal());
}
void Runner::CallRunner(Runner* runner, std::list<RuleList*>* list, Depends* depend, EnvironmentStrings* env, bool keepGoing, std::promise<int> promise) {
    auto retval = runner->RunOne(list, depend, env, keepGoing);
    promise.set_value(retval);
}
int Runner::RunOne(std::list<RuleList*>* ruleStack_in, Depends* depend, EnvironmentStrings* env, bool keepGoing)
{
    RuleList* rl = depend->GetRuleList();
    if (rl->IsBuilt())
    {
        rl->Wait();
        return 0;
    }
    rl->SetBuilt();
    auto ruleStack(*ruleStack_in);
    ruleStack.push_back(rl);
    std::list<std::future<int>> workingList;
    std::list<std::thread> workingThreads;
    int rv = 0;
    bool stop = false;
    for (auto& i : *depend)
    {
        std::promise<int> promise;
        workingList.push_back(promise.get_future());
        auto thrd = std::thread(CallRunner, this, &ruleStack, i.get(), env, keepGoing, std::move(promise));
        workingThreads.push_back(std::move(thrd));
        if (MakeMain::jobs.GetValue() == 1)
        {
            int rv1 = workingList.back().get();
            if (rv <= 0 && rv1 != 0)
                rv = rv1;
            if (rv > 0)
            {
                if (!keepGoing)
                {
                    stop = true;
                    Spawner::Stop();
                    OS::TerminateAll();
                    break;
                }
            }
        }
    }
    if (MakeMain::jobs.GetValue() != 1)
    {
        for (auto&& w : workingList)
        {
            int rv1 = w.get();
            if (rv <= 0 && rv1 != 0)
                rv = rv1;
            if (rv > 0)
            {
                stop = true;
                if (!keepGoing)
                {
                    Spawner::Stop();
                    OS::TerminateAll();
                    break;
                }
            }
        }
    }
    for (auto&& w : workingThreads)
        w.join();
    if (stop)
    {
        rl->Release();
        return 1;
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

        Eval::SetRuleStack(ruleStack);
        Spawner sp(*env, ig, sil, oneShell, posix, displayOnly && !make, keepResponseFiles);
        sp.Run(*depend->GetRule()->GetCommands(), outputType, rl, nullptr);
        Eval::ClearRuleStack();
        rv = sp.RetVal();
        if (rv)
        {
            std::string b = Utils::NumberToString(rv);
            if (RuleContainer::Instance()->Lookup(".DELETE_ON_ERROR"))
            {
                OS::RemoveFile(depend->GetGoal());
                std::cout << std::endl;
                Eval::error("commands returned error code " + b + " '" + depend->GetGoal() + "' removed",
                    depend->GetRule()->GetCommands()->GetFile(), depend->GetRule()->GetCommands()->GetLine());
            }
            else
            {
                std::cout << std::endl;
                Eval::error("commands returned error code " + b, depend->GetRule()->GetCommands()->GetFile(),
                    depend->GetRule()->GetCommands()->GetLine());
            }
        }
    }
    else
    {
        rv = 0;
    }
    rl->Release();
    return rv;
}
void Runner::CancelOne(Depends* depend)
{
    for (auto& d : *depend)
    {
        CancelOne(d.get());
    }
    std::string path = filePaths[depend->GetGoal()];
    if (!path.empty())
    {
        Variable* v = VariableContainer::Instance()->Lookup("GPATH");
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
