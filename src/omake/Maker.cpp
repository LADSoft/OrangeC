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
#include "Maker.h"
#include "Variable.h"
#include "Runner.h"
#include "Rule.h"
#include "Eval.h"
#include "Parser.h"
#include "CmdFiles.h"
#include "Utils.h"
#include <fstream>
#include <list>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <unordered_set>
std::unordered_map<std::string, Depends*> Depends::all;
std::string Maker::firstGoal;
std::unordered_map<std::string, std::string> Maker::filePaths;

Maker::Maker(bool Silent, bool DisplayOnly, bool IgnoreResults, bool Touch, OutputType Type, bool RebuildAll,
             bool KeepResponseFiles, std::string NewFiles, std::string OldFiles) :
    silent(Silent),
    displayOnly(DisplayOnly),
    ignoreResults(IgnoreResults),
    missingTarget(false),
    touch(Touch),
    rebuildAll(RebuildAll),
    keepResponseFiles(KeepResponseFiles),
    newFiles(NewFiles),
    oldFiles(OldFiles),
    outputType(Type),
    lowResolutionTime(false),
    dependsNesting(0)
{
    Variable* v = VariableContainer::Instance()->Lookup("SHELL");
    std::string shtest = v->GetValue();
    std::transform(shtest.begin(), shtest.end(), shtest.begin(), ::toupper);
    OS::SetSHEXE(shtest.find("SH.EXE") != std::string::npos);
}
Maker::~Maker() {}
void Maker::SetFirstGoal(const std::string& name)
{
    static std::unordered_set<std::string> InvalidCandidates
    {
        ".SUFFIXES",
        ".DEFAULT",
        ".EXPORT_ALL_VARIABLES",
        ".INTERMEDIATE",
        ".PRECIOUS",
        ".SECONDARY",
        ".SILENT",
        ".IGNORE",
        ".SECONDEXPANSION",
        ".PHONY",
        ".DELTE_ON_ERROR",
        ".LOW_RESOLUTION_TIME",
        ".NOTPARALLEL",
        ".ONESHELL",
        ".POSIX",
        ".RECURSIVE",
        ".MAIN",
        ".BEGIN",
        ".END",
        ".INCLUDES",
        ".INTERRUPT",
        ".LIBS",
        ".MAKEFILEDEPS",
        ".MFLAGS",
        ".MAKEFLAGS",
        ".NOTPARALLEL",
        ".ORDER",
        ".SHELL",
        ".WARN"
    };
    if (firstGoal.empty())
    {
        if(InvalidCandidates.find(name) == InvalidCandidates.end())
            firstGoal = name;
    }
}
bool Maker::CreateDependencyTree()
{
    if (goals.empty())
    {
        Variable* dg = VariableContainer::Instance()->Lookup(".DEFAULT_GOAL");
        std::string value;
        if (dg && !dg->GetValue().empty())
        {
            value = dg->GetValue();
            if (dg->GetFlavor() == Variable::f_recursive)
            {
                Eval r(value, false);
                value = r.Evaluate();
            }
        }
        else
        {
            value = firstGoal;
        }
        if (value.empty())
            return true;  // nothing to do
        AddGoal(value);
    }
    EnterSuffixTerminals();
    std::string intermediate;
    Variable* v = VariableContainer::Instance()->Lookup(".INTERMEDIATE");
    if (v)
    {
        intermediate = v->GetValue();
    }
    for (auto&& goal : goals)
    {
        Time tv1, tv2;
        dependsNesting = 0;
        if (goal != ".MAKEFLAGS" && goal != ".MFLAGS")
        {
            auto t = Dependencies(goal, "", tv1, true, "", -1);
            if (t)
            {
                depends.push_back(std::move(t));
            }
        }
    }
    v = VariableContainer::Instance()->Lookup(".INTERMEDIATE");
    if (v)
    {
        v->SetValue(intermediate);
    }
    return !missingTarget;
}
std::unique_ptr<Depends> Maker::Dependencies(const std::string& goal, const std::string& preferredPath, Time& timeval, bool err,
                                             std::string file, int line)
{
    if (++dependsNesting > 200)
    {
        Eval::error("depency nesting is too deep: possible recursive rule", "", -1);
        exit(1);
    }
    Time goalTime;
    Time dependsTime;
    std::unique_ptr<Depends> rv(Depends::Lookup(goal));
    bool intermediate = RuleContainer::Instance()->OnList(goal, ".INTERMEDIATE");
    bool secondary = RuleContainer::Instance()->OnList(goal, ".SECONDARY");
    bool precious = RuleContainer::Instance()->OnList(goal, ".PRECIOUS");
    lowResolutionTime = RuleContainer::Instance()->OnList(goal, ".LOW_RESOLUTION_TIME");
    if (!rv)
    {
        Time xx;
        RuleList* ruleList = RuleContainer::Instance()->Lookup(goal);
        bool remake = false;
        if (ruleList)
        {
            RuleList::iterator it;
            for (it = ruleList->begin(); it != ruleList->end(); ++it)
            {
                if ((*it)->HasCommands())
                {
                    break;
                }
            }
            if (it == ruleList->end())
            {
                Time xx;
                SearchImplicitRules(goal, preferredPath, true, xx);
            }
            std::string foundPath = GetFileTime(goal, preferredPath, goalTime);
            bool exists = !!goalTime;
            rv = std::make_unique<Depends>(goal, xx, intermediate && !precious && !secondary);
            Rule* executionRule = nullptr;
            std::string newerPrereqs;
            for (auto& rule : *ruleList)
            {
                std::string working = rule->GetPrerequisites();
                bool remakeThis = false;
                while (!working.empty())
                {
                    Time current;
                    std::string thisOne = Eval::ExtractFirst(working, " ");
                    auto dp = Dependencies(thisOne, foundPath, current, err && !rule->IsDontCare(), rule->File(), rule->Line());
                    auto dependentRuleList = RuleContainer::Instance()->Lookup(thisOne);
                    ruleList->CopyExports(dependentRuleList);
                    if (current > dependsTime)
                    {
                        dependsTime = current;
                    }
                    if (dp)
                    {
                        *rv += (dp);
                    }
                    if (current > goalTime || rebuildAll)
                    {
                        if (!newerPrereqs.empty())
                            newerPrereqs += " ";
                        newerPrereqs += thisOne;
                        remakeThis = true;
                    }
                }
                working = rule->GetOrderPrerequisites();
                while (!working.empty())
                {
                    Time current;
                    std::string thisOne = Eval::ExtractFirst(working, " ");
                    Time checkTime;
                    std::string foundPath = GetFileTime(thisOne, preferredPath, checkTime);
                    bool exists = !!goalTime;
                    if (!exists)
                    {
                        auto dp =
                            Dependencies(thisOne, preferredPath, current, err && !rule->IsDontCare(), rule->File(), rule->Line());
                        auto dependentRuleList = RuleContainer::Instance()->Lookup(thisOne);
                        ruleList->CopyExports(dependentRuleList);
                        if (dp)
                        {
                            dp->SetOrdered(true);
                            (*rv) += dp;
                        }
                    }
                }
                if (rule->HasCommands())
                    if ((ruleList->GetDoubleColon() && remakeThis) || !ruleList->GetDoubleColon())
                    {
                        if (executionRule)
                            Eval::warning("Conflicting command lists for goal '" + goal + "'", rule->File(), rule->Line());
                        else
                            executionRule = rule.get();
                    }
            }
            if (executionRule)
            {
                rv->SetRuleList(ruleList);
                rv->SetRule(executionRule);
                ruleList->SetNewerPrerequisites(newerPrereqs);
            }
            else
            {
                rv->SetRuleList(ruleList);
            }
        }
        else
        {
            if (SearchImplicitRules(goal, preferredPath, true, timeval))
            {
                rv = Dependencies(goal, preferredPath, timeval, err, file, line);
            }
            else if (err)
            {
                auto it = ignoreFailedTargets.find(goal);
                if (it == ignoreFailedTargets.end())
                {
                    Time time;
                    GetFileTime(goal, preferredPath, time);
                    if (!time)
                    {
                        if (!RuleContainer::Instance()->OnList(goal, ".PHONY"))
                        {
                            missingTarget = true;
                            Eval::error("No rule to make target '" + goal + "'", file, line);
                        }
                    }
                    else if (time > timeval)
                        timeval = time;
                }
            }
        }
    }
    if (rv && !rebuildAll)
    {
        if (goalTime >= dependsTime && !RuleContainer::Instance()->OnList(goal, ".PHONY"))
        {
            if (!rv->size())
            {
                rv = nullptr;
            }

            else
            {
                bool check = true;
                for (auto it = rv->begin(); check && it != rv->end(); ++it)
                {
                    check &= (*it)->IsSecondary();
                }
                if (check)
                {
                    rv = nullptr;
                }
            }
        }
    }
    if (rv)
    {
        rv->SetSecondary(secondary || intermediate);
    }
    if (dependsTime > goalTime)
    {
        if (dependsTime > timeval)
            timeval = dependsTime;
    }
    else
    {
        if (goalTime > timeval)
            timeval = goalTime;
    }
    dependsNesting--;
    return rv;
}
std::string Maker::GetFileTime(const std::string& goal, const std::string& preferredPath, Time& timeval)
{
    std::string rv;
    std::string internalGoal = goal;
    if (internalGoal.size() > 1 && internalGoal[0] == '"')
    {
        internalGoal = internalGoal.substr(1, internalGoal.size() - 2);
    }
    std::string vpath = preferredPath + std::string(" ./ ") + Eval::GetVPATH(internalGoal);
    std::string sep = std::string(" ") + CmdFiles::PATH_SEP;
    while (!vpath.empty())
    {
        std::string cur = Eval::ExtractFirst(vpath, sep);
        if (cur[cur.size() - 1] != '/' && cur[cur.size() - 1] != '\\')
            cur += '/';
        std::string name;
        if (internalGoal[0] != '/' && internalGoal[0] != '\\' && internalGoal[1] != ':')
            name = cur + internalGoal;
        else
            name = internalGoal;
        timeval = OS::GetFileTime(name);
        if (!!timeval)
        {
            rv = cur;  // return value is the path, with a slash on the end
            if (rv == "./")
                rv = "";
            timeval = OS::GetFileTime(name);
            if (rv != "")
                filePaths[internalGoal] = cur;
            break;
        }
    }
    if (RuleContainer::Instance()->ScanList(newFiles, goal) || RuleContainer::Instance()->ScanList(newFiles, rv + goal))
    {
        timeval = OS::GetCurrentTime();
    }
    else if (RuleContainer::Instance()->ScanList(oldFiles, goal) || RuleContainer::Instance()->ScanList(oldFiles, rv + goal))
    {
        timeval.Clear();
    }
    if (lowResolutionTime)
    {
        timeval.ms = 0;
    }
    return rv;
}
bool Maker::ExistsOrMentioned(const std::string& stem, RuleList* ruleList, const std::string& preferredPath, const std::string& dir,
                              bool implicit, bool outerMost)
{
    bool found = true;
    for (auto itr = ruleList->begin(); found && itr != ruleList->end(); ++itr)
    {
        std::string working = (*itr)->GetPrerequisites();
        while (!working.empty() && found)
        {
            std::string thisOne = dir + Eval::ExtractFirst(working, " ");
            thisOne = Eval::ReplaceStem(stem, thisOne);
            Time theTime;
            if (RuleContainer::Instance()->find(thisOne) != RuleContainer::Instance()->end())
            {
                // mentioned in makefile, the rule may be valid...
            }
            else
            {
                GetFileTime(thisOne, preferredPath, theTime);
                if (!theTime)
                {
                    if (implicit)
                    {
                        Time time;
                        // this rule may still match if we can recursively find
                        // other implicit rules that match the prerequisites
                        found = SearchImplicitRules(thisOne, preferredPath, false, time);
                    }
                    else
                    {
                        // not mentioned and doesn't exist, this rule doesn't match.
                        found = false;
                    }
                }
            }
        }
    }
    if (found)
    {
        // ok this is a match...
        EnterSpecificRule(ruleList, stem, preferredPath, dir, outerMost);
    }
    return found;
}
void Maker::EnterSpecificRule(RuleList* l, const std::string& stem, const std::string& preferredPath, const std::string& dir,
                              bool outerMost)
{
    Time theTime;
    std::string target = dir + Eval::ReplaceStem(stem, l->GetTarget());
    RuleList* ruleList = RuleContainer::Instance()->Lookup(target);
    if (!ruleList)
    {
        ruleList = new RuleList(target);
        *RuleContainer::Instance() += ruleList;
        ruleList->SetTargetPatternStem(stem);
    }
    std::string orderPrereq;
    std::string prereq;
    Command* commands = nullptr;
    for (auto& rule : *l)
    {
        if (!commands)
            commands = rule->GetCommands();
        std::string working = rule->GetPrerequisites();
        while (!working.empty())
        {
            std::string thisOne = dir + Eval::ExtractFirst(working, " ");
            thisOne = Eval::ReplaceStem(stem, thisOne);
            thisOne = GetFileTime(thisOne, preferredPath, theTime) + thisOne;
            if (!prereq.empty())
                prereq += " ";
            prereq += thisOne;
        }
        working = rule->GetOrderPrerequisites();
        while (!working.empty())
        {
            std::string thisOne = dir + Eval::ExtractFirst(working, " ");
            thisOne = Eval::ReplaceStem(stem, thisOne);
            thisOne = GetFileTime(thisOne, preferredPath, theTime) + thisOne;
            if (!orderPrereq.empty())
                orderPrereq += " ";
            orderPrereq += thisOne;
        }
    }
    Rule* rule = new Rule(target, prereq, orderPrereq, commands, "<implicitbuild>", 1);
    ruleList->InsertFirst(rule);
    if (!outerMost)
    {
        Parser p(".INTERMEDIATE: " + target, "<implicitbuild>", 1, false);
        p.Parse();
    }
}
void Maker::EnterDefaultRule(const std::string& goal, RuleList* dflt)
{
    if (!RuleContainer::Instance()->Lookup(goal))
    {
        Command* commands = nullptr;
        for (auto it = dflt->begin(); !commands && it != dflt->end(); ++it)
            commands = (*it)->GetCommands();
        RuleList* ruleList = new RuleList(goal);
        *RuleContainer::Instance() += ruleList;
        Rule* rule = new Rule(goal, "", "", commands, "<implicitbuild>", 1);
        ruleList->Add(rule);
    }
}
bool Maker::SearchImplicitRules(const std::string& goal, const std::string& preferredPath, bool outerMost, Time& timeval)
{
    std::list<RuleList*> matchedRules;
    size_t n = goal.find_last_of("/\\");
    std::string dir;
    std::string name;
    if (n != std::string::npos)
    {
        dir = goal.substr(0, n + 1);
        name = goal.substr(n + 1);
    }
    else
    {
        name = goal;
    }
    bool nonMatchAnything = false;
    for (auto it = RuleContainer::Instance()->ImplicitBegin(); it != RuleContainer::Instance()->ImplicitEnd(); ++it)
    {
        if ((*it)->GetTarget().find_first_of("/\\") != std::string::npos)
        {
            size_t start;
            n = Eval::MatchesPattern(goal, (*it)->GetTarget(), start);
            if (n == goal.size())
            {
                matchedRules.push_back((*it).get());
                if ((*it)->GetTarget() != "%")
                    nonMatchAnything = true;
            }
        }
        else
        {
            size_t start;
            n = Eval::MatchesPattern(name, (*it)->GetTarget(), start);
            if (n == name.size())
            {
                matchedRules.push_back((*it).get());
                if ((*it)->GetTarget() != "%")
                    nonMatchAnything = true;
            }
        }
    }
    bool nobuiltin = false;
    for (auto&& rl : matchedRules)
        for (auto&& r : *rl)
        {
            nobuiltin |= !r->GetBuiltin();
        }
    if (nobuiltin)
    {
        for (auto it = matchedRules.begin(); it != matchedRules.end();)
        {
            auto it1 = it;
            ++it;
            for (auto&& r : **it1)
            {
                if (r->GetBuiltin())
                {
                    matchedRules.erase(it1);
                    break;
                }
            }
        }
    }
    if (nonMatchAnything)
    {
        for (auto it = matchedRules.begin(); it != matchedRules.end();)
        {
            auto it1 = it;
            ++it;
            if ((*it1)->GetTarget() == "%")
                if (!(*it1)->GetDoubleColon())
                    matchedRules.erase(it1);
        }
    }
    for (auto it = matchedRules.begin(); it != matchedRules.end();)
    {
        auto it1 = it;
        ++it;
        if (!(*it1)->HasCommands())
            matchedRules.erase(it1);
    }
    for (auto rule : matchedRules)
    {

        std::string stem = Eval::FindStem(name, rule->GetTarget());
        if (ExistsOrMentioned(stem, rule, preferredPath, dir, false, outerMost))
            return true;
    }
    // no matches so far, dig into indirections...
    for (auto rule : matchedRules)
    {
        if (rule->GetTarget() != "%" || !rule->GetDoubleColon())
        {
            size_t n;
            if (rule->GetTarget() != "%" || (n = name.find_last_of(".")) == std::string::npos || n == name.size() - 1 ||
                name[n + 1] == '/' || name[n + 1] == '\\')
            {
                std::string stem = Eval::FindStem(name, rule->GetTarget());
                if (ExistsOrMentioned(stem, rule, preferredPath, dir, true, outerMost))
                    return true;
            }
        }
    }
    RuleList* dflt = RuleContainer::Instance()->Lookup(".DEFAULT");
    if (dflt)
    {
        // have a default, enter it
        EnterDefaultRule(goal, dflt);
        return true;
    }
    return false;
}
void Maker::EnterSuffixTerminals()
{
    RuleList* rl = RuleContainer::Instance()->Lookup(".SUFFIXES");
    if (rl)
    {
        for (auto& rule : *rl)
        {
            std::string value = rule->GetPrerequisites();
            while (!value.empty())
            {
                std::string target = "%" + Eval::ExtractFirst(value, " ");
                RuleList* ruleList = RuleContainer::Instance()->Lookup(target);
                if (!ruleList)
                {
                    ruleList = new RuleList(target);
                    Rule* rule = new Rule(target, "", "", nullptr, "<implicitbuild>", 1);
                    rule->SetBuiltin(true);
                    ruleList->Add(rule);
                    *RuleContainer::Instance() += ruleList;
                }
            }
        }
    }
}
void Maker::GetEnvironment(EnvironmentStrings& env)
{
    bool exportAll = false;
    RuleList* rl = RuleContainer::Instance()->Lookup(".EXPORT_ALL_VARIABLES");
    if (rl)
        exportAll = true;
    for (auto& var : *VariableContainer::Instance())
    {
        if (exportAll || var.second->GetExport())
        {
            EnvEntry a((var.first), var.second->GetValue());
            env.push_back(a);
        }
    }
}
void Maker::CallRunner(Runner&& runner, Depends* depend, EnvironmentStrings* env, bool keepGoing, std::promise<int> promise) {
    std::list<RuleList*> list;
    promise.set_value(runner.RunOne(&list, depend, env, keepGoing));
}
int Maker::RunCommands(bool keepGoing)
{
    if (RuleContainer::Instance()->Lookup(".NOTPARALLEL") || RuleContainer::Instance()->Lookup(".NO_PARALLEL"))
        OS::PushJobCount(1);

    bool stop = false;
    EnvironmentStrings env;
    GetEnvironment(env);
    Runner runner(silent, displayOnly, ignoreResults, touch, outputType, keepResponseFiles, firstGoal, filePaths);

    for (auto it = depends.begin(); it != depends.end(); ++it)
    {
        runner.CancelOne((*it).get());
    }

    OS::JobInit();
    std::list<std::future<int>> workingList;
    std::list<std::thread> workingThreads;
    int rv = 0;
    for (auto& i : depends)
    {
        std::promise<int> promise;
        workingList.push_back(promise.get_future());
        auto thrd = std::thread(CallRunner, runner, i.get(), &env, keepGoing, std::move(promise));
        workingThreads.push_back(std::move(thrd));
        if (MakeMain::jobs.GetValue() == 1)
        {
            int rv1 = workingList.back().get();
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
    OS::JobRundown();
    for (auto& d : depends)
    {
        runner.DeleteOne(d.get());
    }
    if (RuleContainer::Instance()->Lookup(".NOTPARALLEL") || RuleContainer::Instance()->Lookup(".NO_PARALLEL"))
        OS::PopJobCount();
    if (stop)
        return 2;
    else
        return rv;
}
void Maker::Clear()
{
    goals.clear();
    depends.clear();
    depends.clear();
    filePaths.clear();
}
std::string Maker::GetFullName(std::string name)
{
    std::string rv = name;
    auto it = filePaths.find(name);
    if (it != filePaths.end())
        rv = it->second + name;
    return rv;
}
