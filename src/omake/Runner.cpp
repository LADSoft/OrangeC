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
#include "BasicLogging.h"
void Runner::DeleteOne(Depends* depend)
{
    for (auto& d : *depend)
    {
        DeleteOne(d.get());
    }
    if (depend->ShouldDelete())
        OS::RemoveFile(depend->GetGoal());
}
void Runner::CallRunner(Runner* runner, std::list<std::shared_ptr<RuleList>>* list, Depends* depend, EnvironmentStrings* env,
                        bool keepGoing, std::promise<int> promise)
{
    auto retval = runner->RunOne(list, depend, env, keepGoing);
    promise.set_value(retval);
    OrangeC::Utils::BasicLogger::debug("CallRunner returning from a runner: ", depend->GetGoal());
}
struct future_holding_struct
{
    std::string command;
    std::future<int> return_value;
    future_holding_struct(const std::string& command, std::future<int>&& return_value) :
        command(command), return_value(std::future(std::move(return_value)))
    {
    }
    int get() { return return_value.get(); }
    bool valid() { return return_value.valid(); }
    template <class Rep, class Period>
    std::future_status wait_for(const std::chrono::duration<Rep, Period>& timeout_duration) const
    {
        return return_value.wait_for(timeout_duration);
    }
    future_holding_struct(future_holding_struct&& other) noexcept
    {
        this->command = std::move(other.command);
        this->return_value = std::move(other.return_value);
    }
    future_holding_struct& operator=(future_holding_struct&& holder) noexcept
    {
        this->command.swap(holder.command);
        this->return_value = std::move(holder.return_value);
        return *this;
    }
};
struct holding_struct
{
    std::string command;
    std::thread workingThread;
    holding_struct(std::string goal, std::thread&& workingThread) : command(goal)
    {
        this->workingThread = std::thread(std::move(workingThread));
    }
    void join() { workingThread.join(); }
    ~holding_struct()
    {
        if (workingThread.joinable())
        {
            workingThread.join();
        }
    }
    holding_struct(holding_struct&& other) noexcept
    {
        this->command = std::move(other.command);
        this->workingThread = std::move(other.workingThread);
    }
    holding_struct& operator=(holding_struct&& holder) noexcept
    {
        this->command.swap(holder.command);
        this->workingThread.swap(holder.workingThread);
        return *this;
    }
};
int Runner::RunOne(std::list<std::shared_ptr<RuleList>>* ruleStack_in, Depends* depend, EnvironmentStrings* env, bool keepGoing)
{
    std::shared_ptr<RuleList> rl = depend->GetRuleList();
    if (rl->IsBuilt())
    {
        rl->Wait();
        return 0;
    }
    rl->SetBuilt();
    auto ruleStack(*ruleStack_in);
    ruleStack.push_back(rl);
    std::vector<future_holding_struct> workingList;
    std::vector<holding_struct> workingThreads;
    int rv = 0;
    bool stop = false;
    for (auto& i : *depend)
    {
        std::promise<int> promise;
        workingList.push_back(future_holding_struct(depend->GetGoal(), promise.get_future()));
        OrangeC::Utils::BasicLogger::debug("RunOne CallRunner Creating a runner: ", i->GetGoal());

        auto thrd = std::thread(CallRunner, this, &ruleStack, i.get(), env, keepGoing, std::move(promise));
        workingThreads.emplace_back(i->GetGoal(), std::move(thrd));
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
        for (auto it = workingList.begin(); it != workingList.end(); it++)
        {
            OrangeC::Utils::BasicLogger::extremedebug("Waiting for a return value from futures in the list");
            size_t distance = std::distance(it, workingList.end());
            OrangeC::Utils::BasicLogger::extremedebug(
                "Waiting on iterator at distance from end: ", std::to_string(distance),
                " for future get. Distance from start: ", std::distance(workingList.begin(), it));

            auto& w = *it;
            using namespace std::chrono_literals;
            while (w.wait_for(1s) != std::future_status::ready)
            {
                OrangeC::Utils::BasicLogger::extremedebug("Waiting on the future for depends: ", w.command,
                                                          " current jobs: ", OS::GetCurrentJobs());
            }
            int rv1 = w.get();
            OrangeC::Utils::BasicLogger::extremedebug("Finished the future get on iterator: ", std::to_string(distance));

            if (rv <= 0 && rv1 != 0)
                rv = rv1;
            if (rv > 0)
            {
                stop = true;
                if (!keepGoing)
                {
                    OrangeC::Utils::BasicLogger::extremedebug("Terminating all subprograms");
                    Spawner::Stop();
                    OS::TerminateAll();
                    break;
                }
            }
        }
    }
    OrangeC::Utils::BasicLogger::extremedebug("Joining workingThreads");
    for (auto&& w : workingThreads)
    {
        OrangeC::Utils::BasicLogger::extremedebug("Joining working thread for depend: ", w.command);
        w.join();
        OrangeC::Utils::BasicLogger::extremedebug("Joined working thread for depend: ", w.command);
    }
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
        auto commands = depend->GetRule()->GetCommands();
        sp.Run(commands, outputType, rl, nullptr);
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
