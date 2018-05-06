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

#include "Rule.h"
#include "Variable.h"
#include "Eval.h"
#include "os.h"
#include "Spawner.h"
#include <iostream>
CommandContainer *CommandContainer::instance = nullptr;
RuleContainer *RuleContainer::instance = nullptr;

CommandContainer *CommandContainer::Instance()
{
    if (!instance)
        instance = new CommandContainer;
    return instance;
}
void CommandContainer::Clear()
{
    for (auto cmd : commands)
        delete cmd;
    commands.clear();
}

Rule::Rule(const std::string &Target, const std::string &Prerequisites, 
           const std::string &OrderPrerequisites, Command *Commands, 
           const std::string &File, int Lineno, 
           bool DontCare, bool Ignore, bool Silent, bool Make, bool Precious,
           bool SecondExpansion)
    : target(Target), prerequisites(Prerequisites), orderPrerequisites(OrderPrerequisites),
    commands(Commands), file(File), lineno(Lineno), secondExpansion(SecondExpansion), uptodate(false),
    dontCare(DontCare), ignore(Ignore), silent(Silent), make(Make), precious(Precious)
{
}
void Rule::SecondaryEval(RuleList *ruleList)
{
    if (secondExpansion)
    {
        Eval a(prerequisites, true, ruleList, this);
        prerequisites = a.Evaluate();
        Eval b(orderPrerequisites, true, ruleList, this);
        orderPrerequisites = b.Evaluate();
    }
}
RuleList::RuleList(const std::string &Target)
    : target(Target), doubleColon(false), intermediate(false), keep(false),
         isBuilt(false)
{
}
RuleList::~RuleList()
{
    relatedPatternRules = "";
    for (auto rule : rules)
        delete rule;
    rules.clear();
    for (auto var : specificVariables)
        delete var.second;
    specificVariables.clear();
}
Variable *RuleList::Lookup(const std::string &name)
{
    auto it = specificVariables.find(&name);
    if (it != specificVariables.end())
        return it->second;
    else
        return nullptr;
}
bool RuleList::Touch(const Time &time)
{
    if (!IsImplicit())
        OS::SetFileTime(target, time);
    return true;
}
bool RuleList::HasCommands()
{
    for (auto rule : *this)
    {
        if (rule->HasCommands())
            return true;
    }
    return false;
}
bool RuleList::Add(Rule *rule, bool Double)
{
    if (rules.size() && Double != doubleColon)
        return false;
    rules.push_back(rule);
    return true;
}
void RuleList::InsertFirst( Rule * rule)
{
    rules.push_front(rule);
}
void RuleList::operator +=(Variable *variable)
{
    specificVariables[&variable->GetName()] = variable;
}
void RuleList::SecondaryEval()
{
    for (auto rule : rules)
        rule->SecondaryEval(this);
}
bool RuleList::IsUpToDate()
{
    bool rv = true;
    for (std::list<Rule *>::iterator it = rules.begin(); it != rules.end()&& rv; ++it)
        rv &= (*it)->IsUpToDate();
    return rv;
}
void RuleList::SetBuilt() 
{ 
    isBuilt = true; 
    std::string working = relatedPatternRules;
    while (working.size())
    {
        std::string temp = Eval::ExtractFirst(working, " ");
        RuleList *rl = RuleContainer::Instance()->Lookup(temp);
        if (rl)
            rl->SetBuilt();
    }
}
RuleContainer *RuleContainer::Instance()
{
    if (!instance)
        instance = new RuleContainer;
    return instance;
}
RuleList *RuleContainer::Lookup(const std::string &name)
{
    auto it = namedRules.find(&name);
    if (it != namedRules.end())
        return it->second;
    else
        return nullptr;

}
void RuleContainer::operator +=(RuleList *list)
{
    if (Eval::FindPercent(list->GetTarget()) != std::string::npos)
    {
        implicitRules.push_back(list);
    }
    else
    {
        namedRules[&list->GetName()] = list;
    }
}
void RuleContainer::operator -=(RuleList *list)
{
    if (Eval::FindPercent(list->GetTarget()) != std::string::npos)
    {
        // noop just in case
    }
    else
    {
        auto it = namedRules.find(&list->GetTarget());
        if (it != namedRules.end())
        {
            delete list;
            namedRules.erase(it);
        }
    }
}
void RuleContainer::SecondaryEval()
{
    for (auto rule : namedRules)
        rule.second->SecondaryEval();
    for (auto rule : implicitRules)
        rule->SecondaryEval();
}
void RuleContainer::Clear()
{
    for (auto rule : namedRules)
        delete rule.second;
    namedRules.clear();
    for (auto rule : implicitRules)
        delete rule;
    implicitRules.clear();
}
