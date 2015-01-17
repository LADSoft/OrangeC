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
#include "Maker.h"
#include "Variable.h"
#include "Rule.h"
#include "Eval.h"
#include "Parser.h"
#include "CmdFiles.h"
#include "utils.h"
#include <fstream>
#include <list>
#include <stdlib.h>
#include <iostream>
std::map<std::string, Maker::Depends *> Maker::Depends::all;
std::string Maker::firstGoal;
std::map<std::string, std::string> Maker::filePaths;

Maker::Depends::~Depends()
{
    for (std::list<Depends *>::iterator it = subgoals.begin(); it != subgoals.end(); ++it)
    {
        Depends *x = *it;
        delete x;
    }
    subgoals.clear();
}

Maker::Maker(bool Silent, bool DisplayOnly, bool IgnoreResults, bool Touch, bool RebuildAll,
             std::string NewFiles, std::string OldFiles) : silent(Silent),
    displayOnly(DisplayOnly), ignoreResults(IgnoreResults), missingTarget(false), touch(Touch),
    rebuildAll(RebuildAll), newFiles(NewFiles), oldFiles(OldFiles)
    {
    }
Maker::~Maker()
{
    Clear();
}
void Maker::SetFirstGoal(const std::string &name) 
{ 
    if (firstGoal.size() == 0) 
    {
        if (name != ".SUFFIXES" && name != ".DEFAULT" && name != ".EXPORT_ALL_VARIABLES")
            if (name != ".INTERMEDIATE" && name != ".PRECIOUS" && name != ".SECONDARY")
                if (name != ".SILENT" && name != ".IGNORE" && name != ".SECONDEXPANSION")
                    if (name != ".PHONY" && name != ".DELETE_ON_ERROR")
                        firstGoal = name;
    }
}
bool Maker::CreateDependencyTree()
{
    if (goals.size() == 0)
    {
        Variable *dg = VariableContainer::Instance()->Lookup(".DEFAULT_GOAL");
        std::string value;
        if (dg && dg->GetValue().size() != 0)
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
        if (value.size() == 0)
            return true; // nothing to do
        AddGoal(value);
    }
    EnterSuffixTerminals();
    std::string intermediate;
    Variable *v = VariableContainer::Instance()->Lookup(".INTERMEDIATE");
    if (v)
    {
        intermediate = v->GetValue();
    }
    for (std::list<std::string>::iterator it = goals.begin(); it != goals.end(); ++it)
    {
        Time tv1, tv2;
        Depends *t = Dependencies(*it, "", tv1);
        if (t || OnList(*it,".PHONY"))
        {
            depends.push_back(t);
        }
        else if (t)
            delete t;
    }
    v = VariableContainer::Instance()->Lookup(".INTERMEDIATE");
    if (v)
    {
        v->SetValue(intermediate);
    }
    return !missingTarget;
}
Maker::Depends *Maker::Dependencies(const std::string &goal, const std::string &preferredPath, Time &timeval)
{
    Time goalTime;
    Time dependsTime;
    Depends *rv = Depends::Lookup(goal);
    bool intermediate = OnList(goal, ".INTERMEDIATE");
    bool secondary = OnList(goal, ".SECONDARY");
    bool precious = OnList(goal, ".PRECIOUS");
    if (!rv)
    {
        Time xx;
        RuleList *ruleList = RuleContainer::Instance()->Lookup(goal);
        bool remake = false;
        if (ruleList)
        {
            RuleList::iterator it;
            for (it = ruleList->begin(); it != ruleList->end(); ++it)
            {
                if ((*it)->HasCommands())
                    break;
            }
            if (it == ruleList->end())
            {
                Time xx;
                SearchImplicitRules(goal, preferredPath, true, false, xx);
            }
            std::string foundPath = GetFileTime(goal ,preferredPath, goalTime);
            bool exists = ! !goalTime;
            rv = new Depends(goal, xx, intermediate && !precious && !secondary);
            Rule *executionRule = NULL;
            std::string newerPrereqs;
            for (RuleList::iterator it = ruleList->begin(); it != ruleList->end(); ++it)
            {
                std::string working = (*it)->GetPrerequisites();
                bool remakeThis = false;
                while (working.size())
                {
                    Time current;
                    std::string thisOne = Eval::ExtractFirst(working, " ");
                    Depends *dp = Dependencies(thisOne, foundPath, current);
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
                        if (newerPrereqs.size())
                            newerPrereqs += " ";
                        newerPrereqs += thisOne;
                        remakeThis = true;
                    }
                }
                working = (*it)->GetOrderPrerequisites();
                while (working.size())
                {
                    Time current;
                    std::string thisOne = Eval::ExtractFirst(working, " ");
                    Depends *dp = Dependencies(thisOne, preferredPath, current);
                    if (dp)
                    {
                        dp->SetOrdered(true);
                        (*rv) += dp;
                    }
                }
                if ((*it)->HasCommands())
                    if ((ruleList->GetDoubleColon() && remakeThis) || !ruleList->GetDoubleColon())
                    {
                        if (executionRule)
                            Eval::warning("Conflicting command lists for goal '" + goal + "'");
                        else
                            executionRule = *it;
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
            if (SearchImplicitRules(goal, preferredPath, true, true, timeval))
                rv = Dependencies(goal, preferredPath, timeval);
        }
    }
    if (rv && !rebuildAll)
    {
        if (goalTime > dependsTime)
        {
            if (!rv->size())
            {
                    delete rv;
                    rv = NULL;
            }
            else
            {
                bool check = true;
                for (Depends::iterator it = rv->begin(); check && it != rv->end(); ++it)
                {
                    check &= (*it)->IsSecondary();
                }
                if (check)
                {
                    delete rv;
                    rv = NULL;
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
    return rv;
}
std::string Maker::GetFileTime(const std::string &goal, const std::string &preferredPath, Time &timeval)
{
    std::string rv;
    if (goal.find_first_of(CmdFiles::DIR_SEP) != std::string::npos)
    {
        std::fstream fil(goal.c_str(), std::ios::in);
        if (fil != NULL)
        {
            fil.close();
            timeval = OS::GetFileTime(goal);
        }
    }
    else
    {
        std::string vpath = preferredPath + std::string(" .\\ ") + Eval::GetVPATH(goal);
        std::string sep = std::string(" ") + CmdFiles::PATH_SEP;
        while (vpath.size())
        {
            std::string cur = Eval::ExtractFirst(vpath, sep);
            if (cur[cur.size() -1] != CmdFiles::DIR_SEP[0])
                cur += CmdFiles::DIR_SEP;
            std::string name ;
            if (goal[0] != CmdFiles::DIR_SEP[0] && goal[1] != ':')
                name = cur + goal;
            else
                name = goal;
            if (cur != ".\\")
                filePaths[goal] = cur; // tentatively enter this as the goal path
                                    // this will collide if there are multiple paths and no file exists
                                    // and choose the last one
            std::fstream fil(name.c_str(), std::ios::in);
            if (fil != NULL)
            {
                fil.close();
                rv = cur; // return value is the path, with a slash on the end
                timeval = OS::GetFileTime(name);
                filePaths[goal] = cur;
                break;
            }
        }
    }
    if (ScanList(newFiles, goal) || ScanList(newFiles, rv + goal))
    {
        timeval = OS::GetCurrentTime();
    }
    else if (ScanList(oldFiles, goal) || ScanList(oldFiles, rv + goal))
    {
        timeval.Clear();
    }
    return rv;
}
bool Maker::ExistsOrMentioned(const std::string &stem, RuleList *ruleList, const std::string &preferredPath, const std::string &dir, bool implicit, bool outerMost)
{
    bool found = true;
    for (RuleList::iterator itr = ruleList->begin(); found && itr != ruleList->end(); ++ itr)
    {
        std::string working = (*itr)->GetPrerequisites();
        while (working.size() && found)
        {
            std::string thisOne = Eval::ExtractFirst(working, " ");
            if (thisOne.find_first_of(CmdFiles::DIR_SEP) == std::string::npos)
                thisOne = dir + thisOne;
            thisOne = Eval::ReplaceStem(stem, thisOne);
            Time theTime;
            if (RuleContainer::Instance()->find(&thisOne) != RuleContainer::Instance()->end())
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
                        found = SearchImplicitRules(thisOne, preferredPath, false, false, time);
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
        EnterSpecificRule(ruleList, stem, preferredPath, outerMost);
    }
    return found;
}
void Maker::EnterSpecificRule(RuleList *l, const std::string &stem, 
                              const std::string &preferredPath, bool outerMost)
{
    std::string target = Eval::ReplaceStem(stem, l->GetTarget());
    RuleList *ruleList = RuleContainer::Instance()->Lookup(target);
    if (!ruleList)
    {
        ruleList = new RuleList(target);
//        if (ruleList)
        {
            *RuleContainer::Instance() += ruleList;
            ruleList->SetTargetPatternStem(stem);
        }
    }
    std::string orderPrereq;
    std::string prereq;
    Command *commands = NULL;
    for (RuleList::iterator it = l->begin(); it != l->end(); ++it)
    {
        if (!commands)
            commands = (*it)->GetCommands();
        std::string working = (*it)->GetPrerequisites();
        while (working.size())
        {
            std::string temp = Eval::ExtractFirst(working, " ");
            temp = Eval::ReplaceStem(stem, temp);
            if (prereq.size())
                prereq += " ";
            prereq += temp;
        }
        working = (*it)->GetOrderPrerequisites();
        while (working.size())
        {
            std::string temp = Eval::ExtractFirst(working, " ");
            temp = Eval::ReplaceStem(stem, temp);
            if (orderPrereq.size())
                orderPrereq += " ";
            orderPrereq += temp;
        }
    }
    Rule *rule = new Rule(target, prereq, orderPrereq, commands, "<implicitbuild>", 1);
//    if (rule)
        ruleList->InsertFirst(rule);
    if (!outerMost)
    {
        Parser p(".INTERMEDIATE: " + target, "<implicitbuild>", 1, false);
        p.Parse();
    }
//	Time tv;
    // for vpaths...
//	GetFileTime(target, preferredPath, tv);
}
void Maker::EnterDefaultRule(const std::string &goal, RuleList *dflt)
{
    if (!RuleContainer::Instance()->Lookup(goal))
    {
        Command *commands = NULL;
        for (RuleList::iterator it = dflt->begin(); !commands && it != dflt->end(); ++it)
            commands = (*it)->GetCommands();
        RuleList *ruleList = new RuleList(goal);
//        if (ruleList)
        {
            *RuleContainer::Instance() += ruleList;
            Rule *rule = new Rule(goal, "", "", commands, "<implicitbuild>", 1);
//            if (rule)
                ruleList->Add(rule);
        }
    }
}
bool Maker::SearchImplicitRules(const std::string &goal, const std::string &preferredPath, bool outerMost, bool err, Time &timeval)
{
    std::list<RuleList *>matchedRules;
    size_t n = goal.find_last_of(CmdFiles::DIR_SEP);
    std::string dir;
    std::string name;
    if (n != std::string::npos)
    {
        dir = goal.substr(0, n+1);
        name = goal.substr(n+1);
    }
    else
    {
        name = goal;
    }
    bool nonMatchAnything = false;
    for (RuleContainer::ImplicitIterator it = RuleContainer::Instance()->ImplicitBegin();
             it != RuleContainer::Instance()->ImplicitEnd(); ++it)
    {
        if ((*it)->GetTarget().find_first_of(CmdFiles::DIR_SEP) != std::string::npos)
        {
            size_t start;
            n = Eval::MatchesPattern(goal, (*it)->GetTarget(), start);
            if (n == goal.size())
            {
                matchedRules.push_back(*it);
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
                matchedRules.push_back(*it);
                if ((*it)->GetTarget() != "%")
                    nonMatchAnything = true;
            }
        }
    }
    if (nonMatchAnything)
    {
        for (std::list<RuleList *>::iterator it = matchedRules.begin();
             it != matchedRules.end();)
        {
            std::list<RuleList *>::iterator it1 = it;
            ++it;
            if ((*it1)->GetTarget() == "%")
                if (!(*it1)->GetDoubleColon())
                    matchedRules.erase(it1);
        }		
    }
    for (std::list<RuleList *>::iterator it = matchedRules.begin();
         it != matchedRules.end();)
    {
        std::list<RuleList *>::iterator it1 = it;
        ++it;
        if (!(*it1)->HasCommands())
            matchedRules.erase(it1);
    }
    for (std::list<RuleList *>::iterator it = matchedRules.begin();
         it != matchedRules.end(); ++it)
    {
        
        std::string stem = Eval::FindStem(goal, (*it)->GetTarget());
        if (ExistsOrMentioned(stem, *it, preferredPath, dir, false, outerMost))
            return true;
    }
    // no matches so far, dig into indirections...
    for (std::list<RuleList *>::iterator it = matchedRules.begin();
         it != matchedRules.end(); ++it)
    {
        if ((*it)->GetTarget() != "%" || !(*it)->GetDoubleColon())
        {
            std::string stem = Eval::FindStem(goal, (*it)->GetTarget());		
            if (ExistsOrMentioned(stem, *it, preferredPath, dir, true, outerMost))
                return true;
        }
    }
    RuleList *dflt = RuleContainer::Instance()->Lookup(".DEFAULT");
    if (dflt)
    {
        // have a default, enter it
        EnterDefaultRule(goal, dflt);
        return true;
    }
    if (outerMost && err)
    {
        std::set<std::string>::iterator it = ignoreFailedTargets.find(goal);
        if (it == ignoreFailedTargets.end())
        {
            Time time;
            GetFileTime(goal, preferredPath, time);
            if (!time)
            {
                missingTarget = true;
                Eval::error("No rule to make target '" + goal + "'");
            }
            else if (time > timeval)
                timeval = time;
        }
    }
    return false;
}
bool Maker::ScanList(const std::string &v, const std::string &goal)
{
    std::string value = v;
    bool rv = false;
    while (value.size() && !rv)
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
        if (aa[aa.size()-1] != CmdFiles::DIR_SEP[0])
        {
            aa += CmdFiles::DIR_SEP;
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
bool Maker::OnList(const std::string &goal, char *what)
{
    bool rv = false;
    RuleList *rl = RuleContainer::Instance()->Lookup(what);
    if (rl)
    {
        for (RuleList::iterator it = rl->begin(); !rv && it != rl->end(); ++it)
        {
            std::string value = (*it)->GetPrerequisites();	
            rv = ScanList(value, goal);
        }
    }
    return rv;
}
void Maker::EnterSuffixTerminals()
{
    RuleList *rl = RuleContainer::Instance()->Lookup(".SUFFIXES");
    if (rl)
    {
        for (RuleList::iterator it = rl->begin(); it != rl->end(); ++it)
        {
            std::string value = (*it)->GetPrerequisites();
            while (value.size())
            {
                std::string target = "%" + Eval::ExtractFirst(value, " ");
                RuleList *ruleList = RuleContainer::Instance()->Lookup(target);
                if (!ruleList)
                {
                    ruleList = new RuleList(target);
                    //if (ruleList)
                    {
                        Rule *rule = new Rule(target, "", "", NULL, "<implicitbuild>", 1);
                        //if (rule)
                            ruleList->Add(rule);
                    }
                }
            }
        }
    }
}
void Maker::GetEnvironment(EnvironmentStrings &env)
{
    bool exportAll = false;
    RuleList *rl = RuleContainer::Instance()->Lookup(".EXPORT_ALL_VARIABLES");
    if (rl)
        exportAll = true;
    for (VariableContainer::iterator it = VariableContainer::Instance()->begin();
         it != VariableContainer::Instance()->end(); ++it)
    {
        if (exportAll || it->second->GetExport())
        {
            EnvEntry a(*(it->first), it->second->GetValue());
            env.push_back(a);
        }
    }
}
void Maker::DeleteOne(Depends *depend)
{
    for (Depends::iterator it = depend->begin(); it != depend->end(); ++it)
    {
        DeleteOne(*it);
    }
    if (depend->ShouldDelete())
        OS::RemoveFile(depend->GetGoal());	
}
int Maker::RunOne(Depends *depend, EnvironmentStrings &env, bool keepGoing)
{
    int rv = 0;
    if (depend->GetRuleList()->IsBuilt())
        return 0;
    RuleList *rl = depend->GetRuleList();
    Eval::PushruleStack(rl);
    bool stop = false;
    bool cantbuild = false;
    for (Depends::iterator it = depend->begin(); it != depend->end(); ++it)
    {
//		if (!(*it)->GetOrdered())
        {
            if ((rv = RunOne(*it, env, keepGoing)))
            {
                stop = true;
                if (!keepGoing)
                {
                    Eval::PopruleStack();
                    return rv;
                }
            }			
        }
//		else 
//			cantbuild |= !(*it)->GetRuleList()->IsBuilt();
    }
    if (stop)
    {
        Eval::PopruleStack();
        return -1;
    }
    if (cantbuild || rl->IsBuilt())
    {
        Eval::PopruleStack();
        return 0;
    }
    rl->SetBuilt();
    if (touch)
    {
        rl->Touch(OS::GetCurrentTime());
    }
    bool sil = silent;
    bool ig = ignoreResults;
    if (!sil)
        sil = OnList(depend->GetGoal(), ".SILENT");
    if (!ig)
        ig = OnList(depend->GetGoal(), ".IGNORE");
    Spawner sp(env, ig, sil, displayOnly);
    if (depend->GetRule() && depend->GetRule()->GetCommands())
        rv = sp.Run(*depend->GetRule()->GetCommands(), rl, NULL);
    if (rv)
    {
        std::string b = Utils::NumberToString(rv);
        if (RuleContainer::Instance()->Lookup(".DELETE_ON_ERROR"))
        {
            OS::RemoveFile(depend->GetGoal());
            std::cout << std::endl;
            Eval::error("commands returned error code " + b + " '" + depend->GetGoal()  + "' removed");
        }
        else
        {
            std::cout << std::endl;
            Eval::error("commands returned error code " + b);
        }
    }
    Eval::PopruleStack();
    return rv;	
}
void Maker::CancelOne(Depends *depend)
{
    for (Depends::iterator it = depend->begin(); it != depend->end(); ++it)
    {
        CancelOne(*it);
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
            if (!ScanList(t, path))
                    filePaths[depend->GetGoal()] = "";		
        }
//		else
//			filePaths[depend->GetGoal()] = "";		
    }
}
int Maker::RunCommands(bool keepGoing)
{
    int rv =0 ;
    bool stop = false;
    EnvironmentStrings env;
    GetEnvironment(env);
    int count;
    for (std::list<Depends *>::iterator it = depends.begin(); (rv == 0 || keepGoing) && it != depends.end(); ++it)
    {
        CancelOne(*it);
    }
    for (std::list<Depends *>::iterator it = depends.begin(); (rv == 0 || keepGoing) && it != depends.end(); ++it)
    {
        rv = RunOne(*it, env, keepGoing);
        if (rv)
            stop = true;
    }
    for (std::list<Depends *>::iterator it = depends.begin(); it != depends.end(); ++it)
    {
        DeleteOne(*it);
    }
    if (stop)
        return 2;
    else
        return rv;
}
void Maker::Clear() 
{ 
    goals.clear(); 
    for (std::list<Depends *>::iterator it = depends.begin(); it != depends.end(); ++it)
    {
        Depends *d = *it;
        delete d;
    }
    depends.clear();
    filePaths.clear();
}
std::string Maker::GetFullName(std::string name)
{
    std::string rv = name;
    std::map<std::string, std::string>::iterator it = filePaths.find(name);
    if (it != filePaths.end())
        rv = it->second + name;
    return rv;
}
