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
#ifndef RULE_H
#define RULE_H

#include <string>
#include <list>
#include <map>
#include "Spawner.h"

class CommandContainer
{
public:
    static CommandContainer *Instance() ;
    ~CommandContainer() { Clear(); }
    
    CommandContainer &operator += (Command *p) { commands.push_back(p); return *this; }
    void Clear();	
protected:
    CommandContainer()  { }
private:
    std::list<Command *> commands;
    
    static CommandContainer *instance;
} ;
class Variable;
class RuleList;
class Rule
{
public:
    Rule(const std::string &targets, const std::string &Prerequisites, const std::string &OrderPrerequisites, Command *Commands, const std::string &file, int lineno, bool secondExpansion = false);
    
    Command *GetCommands() { return commands; }
    std::string &GetPrerequisites() { return prerequisites; }
    std::string &GetOrderPrerequisites() { return orderPrerequisites; }
    bool IsUpToDate() const { return uptodate; }
    void SetUpToDate(bool flag) { uptodate = flag; }
    void SecondaryEval(RuleList *ruleList);
    bool HasCommands() { return commands && commands->size() != 0; }
private:
    std::string target;
    std::string prerequisites;
    std::string orderPrerequisites;
    Command *commands;
    const std::string &file;
    int lineno;
    bool uptodate;
    bool secondExpansion;
};
class RuleList
{
public:
    RuleList(const std::string &target);
    ~RuleList();
    bool Touch(const Time &time);
    std::string &GetTarget() { return target; }
    void SetTarget(const std::string &targ) { target = targ; }
    Variable *Lookup(const std::string &name);
    bool Add(Rule *, bool Double = false);
    void InsertFirst( Rule *);
    void operator +=(Variable *);
    void SetNewerPrerequisites(const std::string &val) { newerPrerequisites = val; }
    std::string GetNewerPrerequisites() { return newerPrerequisites; }
    void SetTargetPatternStem(const std::string stem) { targetPatternStem = stem; }
    std::string GetTargetPatternStem() const { return targetPatternStem; }
    const std::string &GetName() const { return target; }
    void SetIntermediate(bool flag) { intermediate = flag; }
    bool GetIntermediate() const { return intermediate; }
    void SetKeep(bool flag) { keep = flag; }
    bool GetKeep() const { return keep; }
    bool GetDoubleColon() const { return doubleColon; }
    bool IsImplicit() const { return target.find_first_of('%') != std::string::npos; }
    bool HasCommands();
    void SetRelated(const std::string &related) { relatedPatternRules = related; }
    typedef std::list<Rule *>::iterator iterator;
    iterator begin() { return rules.begin(); }
    iterator end() { return rules.end(); }
    struct rllt
    {
        bool operator () (const std::string *one, const std::string *two) const
        {
            return *one < *two;
        }
    } ;
    typedef std::map<const std::string *, Variable *, rllt>::iterator VariableIterator;
    const VariableIterator VariableBegin() { return specificVariables.begin(); }
    const VariableIterator VariableEnd() { return specificVariables.end(); }
    void SecondaryEval();
    bool IsUpToDate();
    bool IsBuilt() { return isBuilt; }
    void SetBuilt();
private:
    std::string targetPatternStem;
    std::string target;
    std::string relatedPatternRules;
    std::list<Rule *> rules;
    std::map<const std::string *, Variable *, rllt> specificVariables;
    std::string newerPrerequisites;
    bool doubleColon;
    bool intermediate;
    bool keep;
    bool isBuilt;
};
class RuleContainer
{
public:
    static RuleContainer *Instance();
    ~RuleContainer() { Clear(); }
    RuleList *Lookup(const std::string &name);
    void operator +=(RuleList *);
    void operator -=(RuleList *);
    struct rllt
    {
        bool operator () (const std::string *one, const std::string *two) const
        {
            return *one < *two;
        }
    } ;
    typedef std::map<const std::string *, RuleList *, rllt>::iterator iterator;
    const iterator begin() { return namedRules.begin(); }
    const iterator end() { return namedRules.end(); }

    iterator find(const std::string * str)
    {
        return namedRules.find(str);
    }
    typedef std::list<RuleList *>::iterator ImplicitIterator;
    const ImplicitIterator ImplicitBegin() { return implicitRules.begin(); }
    const ImplicitIterator ImplicitEnd() { return implicitRules.end(); }
    void Clear();
    void SecondaryEval();	
protected:
    RuleContainer() { }
private:
    std::map<const std::string *, RuleList *, rllt> namedRules;
    std::list<RuleList *> implicitRules;
    static RuleContainer *instance;
};
#endif