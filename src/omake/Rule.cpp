/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
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
           const std::string &File, int Lineno, bool SecondExpansion)
    : target(Target), prerequisites(Prerequisites), orderPrerequisites(OrderPrerequisites),
    commands(Commands), file(File), lineno(Lineno), secondExpansion(SecondExpansion), uptodate(false)
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
