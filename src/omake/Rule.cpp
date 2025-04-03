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

#include "Rule.h"
#include "Variable.h"
#include "Eval.h"
#include "os.h"
#include "Spawner.h"
#include "CmdFiles.h"
#include <iostream>
CommandContainer* CommandContainer::instance = nullptr;
std::shared_ptr<RuleContainer> RuleContainer::instance = nullptr;

CommandContainer* CommandContainer::Instance()
{
    if (!instance)
        instance = new CommandContainer();
    return instance;
}

CommandContainer& CommandContainer::operator+=(std::shared_ptr<Command>& p)
{
    commands.push_back(p);
    return *this;
}

void CommandContainer::Clear() { commands.clear(); }

Rule::Rule(const std::string& Target, const std::string& Prerequisites, const std::string& OrderPrerequisites,
           std::shared_ptr<Command> Commands, const std::string& File, int Lineno, bool DontCare, bool Ignore, bool Silent,
           bool Make, bool Precious, bool SecondExpansion, bool prereq) :
    target(Target),
    prerequisites(Prerequisites),
    orderPrerequisites(OrderPrerequisites),
    commands(Commands),
    file(File),
    lineno(Lineno),
    secondExpansion(SecondExpansion),
    uptodate(false),
    dontCare(DontCare),
    ignore(Ignore),
    silent(Silent),
    make(Make),
    precious(Precious),
    hasPrereq(prereq)
{
}
void Rule::SecondaryEval(std::shared_ptr<RuleList> ruleList, std::shared_ptr<Rule> rule)
{
    if (secondExpansion)
    {
        Eval a(prerequisites, true, ruleList, rule);
        prerequisites = a.Evaluate();
        Eval b(orderPrerequisites, true, ruleList, rule);
        orderPrerequisites = b.Evaluate();
    }
}
RuleList::RuleList(const std::string& Target) :
    target(Target), doubleColon(false), intermediate(false), keep(false), isBuilt(false), onHold(60000)
{
}
RuleList::~RuleList() {}
Variable* RuleList::Lookup(const std::string& name)
{
    auto it = specificVariables.find(name);
    if (it != specificVariables.end())
        return it->second.get();
    else
        return nullptr;
}
void RuleList::CopyExports(std::shared_ptr<RuleList>& source)
{
    if (source)
    {
        for (auto&& a : source->specificVariables)
        {
            if (a.second->GetExport())
            {
                if (specificVariables.find(a.first) == specificVariables.end())
                    specificVariables[a.first] = a.second;
            }
        }
    }
}
bool RuleList::Touch(const Time& time)
{
    if (!IsImplicit())
        OS::SetFileTime(target, time);
    return true;
}
bool RuleList::HasCommands()
{
    for (auto& rule : *this)
    {
        if (rule->HasCommands())
            return true;
    }
    return false;
}
bool RuleList::Add(std::shared_ptr<Rule>& rule, bool Double)
{
    if (!rules.empty() && Double != doubleColon)
        return false;
    rules.push_back(rule);
    return true;
}
void RuleList::InsertFirst(std::shared_ptr<Rule>& rule) { rules.push_front(rule); }
void RuleList::operator+=(Variable* variable)
{
    std::unique_ptr<Variable> temp(variable);
    specificVariables[variable->GetName()] = std::move(temp);
}
void RuleList::SecondaryEval(std::shared_ptr<RuleList>& ruleList)
{
    for (auto& rule : rules)
        rule->SecondaryEval(ruleList, rule);
}
bool RuleList::IsUpToDate()
{
    bool rv = true;
    for (auto it = rules.begin(); it != rules.end() && rv; ++it)
        rv &= (*it)->IsUpToDate();
    return rv;
}
void RuleList::SetBuilt()
{
    isBuilt = true;
    std::string working = relatedPatternRules;
    while (!working.empty())
    {
        std::string temp = Eval::ExtractFirst(working, " ");
        std::shared_ptr<RuleList> rl = RuleContainer::Instance()->Lookup(temp);
        if (rl)
            rl->SetBuilt();
    }
}
std::shared_ptr<RuleContainer> RuleContainer::Instance()
{
    if (!instance)
        instance = std::shared_ptr<RuleContainer>(new RuleContainer);
    return instance;
}
std::shared_ptr<RuleList> RuleContainer::Lookup(const std::string& name)
{
    auto it = namedRules.find(name);
    if (it != namedRules.end())
        return it->second;
    else
        return nullptr;
}
void RuleContainer::operator+=(std::shared_ptr<RuleList>& list)
{
    std::shared_ptr<RuleList> temp(list);
    if (Eval::FindPercent(list->GetTarget()) != std::string::npos)
    {
        implicitRules.push_back(std::move(temp));
    }
    else
    {
        namedRules[list->GetName()] = std::move(temp);
    }
}
void RuleContainer::operator-=(std::shared_ptr<RuleList>& list)
{
    if (Eval::FindPercent(list->GetTarget()) != std::string::npos)
    {
        // noop just in case
    }
    else
    {
        auto it = namedRules.find(list->GetTarget());
        if (it != namedRules.end())
        {
            namedRules.erase(it);
        }
    }
}
void RuleContainer::SecondaryEval()
{
    for (auto& rule : namedRules)
        rule.second->SecondaryEval(rule.second);
    for (auto& ruleList : implicitRules)
        ruleList->SecondaryEval(ruleList);
}
void RuleContainer::Clear()
{
    namedRules.clear();
    implicitRules.clear();
}
bool RuleContainer::OnList(const std::string& goal, const char* what)
{
    bool rv = false;
    std::shared_ptr<RuleList> rl = Lookup(what);
    if (rl)
    {
        for (auto it = rl->begin(); !rv && it != rl->end(); ++it)
        {
            std::string value = (*it)->GetPrerequisites();
            rv = ScanList(value, goal);
        }
    }
    return rv;
}
bool RuleContainer::NoList(const char* what)
{
    bool rv = false;
    std::shared_ptr<RuleList> rl = Lookup(what);
    if (rl)
    {
        rv = true;
        for (auto it = rl->begin(); rv && it != rl->end(); ++it)
        {
            if (Eval::ExtractFirst((*it)->GetPrerequisites(), " ") != "")
            {
                rv = false;
            }
        }
    }
    return rv;
}
bool RuleContainer::ScanList(const std::string& v, const std::string& goal)
{
    std::string value = v;
    bool rv = false;
    while (!value.empty() && !rv)
    {
        size_t start;
        size_t span;

        std::string aa = Eval::ExtractFirst(value, " ");
        span = Eval::MatchesPattern(goal, aa, start, 0);
        if (span == goal.size())
        {
            rv = true;
            break;
        }
        if (aa[aa.size() - 1] != '/' && aa[aa.size() - 1] != '\\')
        {
            aa += "/";
            span = Eval::MatchesPattern(goal, aa, start, 0);
            if (span == goal.size())
            {
                rv = true;
                break;
            }
        }
    }
    return rv;
}
