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

#ifndef RULE_H
#define RULE_H

#include "Spawner.h"
#include "semaphores.h"
#include <string>
#include <list>
#include <map>
#include <memory>

class CommandContainer
{
  public:
    static std::shared_ptr<CommandContainer> Instance();
    ~CommandContainer() {}

    CommandContainer& operator+=(Command* p);
    void Clear();

  protected:
    CommandContainer() {}

  private:
    std::list<std::unique_ptr<Command>> commands;

    static std::shared_ptr<CommandContainer> instance;
};
class Variable;
class RuleList;
class Rule
{
  public:
    Rule(const std::string& targets, const std::string& Prerequisites, const std::string& OrderPrerequisites, Command* Commands,
         const std::string& file, int lineno, bool dontCare = false, bool ignore = false, bool silent = false, bool make = false,
         bool precious = false, bool secondExpansion = false);

    Command* GetCommands() { return commands; }
    std::string& GetPrerequisites() { return prerequisites; }
    std::string& GetOrderPrerequisites() { return orderPrerequisites; }
    void SetBuiltin(bool flag) { builtin = flag; }
    bool GetBuiltin() const { return builtin; }
    bool IsUpToDate() const { return uptodate; }
    void SetUpToDate(bool flag) { uptodate = flag; }
    void SecondaryEval(RuleList* ruleList);
    bool HasCommands() { return commands && commands->size() != 0; }
    bool IsDontCare() const { return dontCare; }
    bool IsIgnore() const { return ignore; }
    bool IsSilent() const { return silent; }
    bool IsMake() const { return make; }
    bool IsPrecious() const { return precious; }
    std::string File() const { return file; }
    int Line() const { return lineno; }
  private:
    std::string target;
    std::string prerequisites;
    std::string orderPrerequisites;
    Command* commands;
    const std::string file;
    int lineno;
    bool uptodate;
    bool secondExpansion;
    bool dontCare;
    bool ignore;
    bool silent;
    bool make;
    bool precious;
    bool builtin = false;
};
class RuleList
{
  public:
    RuleList(const std::string& target);
    ~RuleList();
    bool Touch(const Time& time);
    std::string& GetTarget() { return target; }
    void SetTarget(const std::string& targ) { target = targ; }
    Variable* Lookup(const std::string& name);
    bool Add(Rule*, bool Double = false);
    void InsertFirst(Rule*);
    void operator+=(Variable*);
    void SetNewerPrerequisites(const std::string& val) { newerPrerequisites = val; }
    std::string GetNewerPrerequisites() { return newerPrerequisites; }
    void SetTargetPatternStem(const std::string stem) { targetPatternStem = stem; }
    std::string GetTargetPatternStem() const { return targetPatternStem; }
    const std::string& GetName() const { return target; }
    void SetIntermediate(bool flag) { intermediate = flag; }
    bool GetIntermediate() const { return intermediate; }
    void SetKeep(bool flag) { keep = flag; }
    bool GetKeep() const { return keep; }
    bool GetDoubleColon() const { return doubleColon; }
    bool IsImplicit() const { return target.find_first_of('%') != std::string::npos; }
    bool HasCommands();
    void SetRelated(const std::string& related) { relatedPatternRules = related; }
    typedef std::list<std::unique_ptr<Rule>>::iterator iterator;
    iterator begin() { return rules.begin(); }
    iterator end() { return rules.end(); }
    typedef std::map<std::string, std::shared_ptr<Variable>>::iterator VariableIterator;
    const VariableIterator VariableBegin() { return specificVariables.begin(); }
    const VariableIterator VariableEnd() { return specificVariables.end(); }
    void SecondaryEval();
    bool IsUpToDate();
    bool IsBuilt() { return isBuilt; }
    void SetBuilt();
    void Wait() { onHold.Wait(); }
    void Release() { onHold.Post(60000); }
    void CopyExports(RuleList* source);

  private:
    Semaphore onHold;
    std::string targetPatternStem;
    std::string target;
    std::string relatedPatternRules;
    std::list<std::unique_ptr<Rule>> rules;
    std::map<std::string, std::shared_ptr<Variable>> specificVariables;
    std::string newerPrerequisites;
    bool doubleColon;
    bool intermediate;
    bool keep;
    bool isBuilt;
};
class RuleContainer
{
  public:
    static std::shared_ptr<RuleContainer> Instance();
    ~RuleContainer() {}
    RuleList* Lookup(const std::string& name);
    void operator+=(RuleList*);
    void operator-=(RuleList*);
    typedef std::map<std::string, std::unique_ptr<RuleList>>::iterator iterator;
    const iterator begin() { return namedRules.begin(); }
    const iterator end() { return namedRules.end(); }

    iterator find(const std::string& str) { return namedRules.find(str); }
    typedef std::list<std::unique_ptr<RuleList>>::iterator ImplicitIterator;
    const ImplicitIterator ImplicitBegin() { return implicitRules.begin(); }
    const ImplicitIterator ImplicitEnd() { return implicitRules.end(); }
    void Clear();
    void SecondaryEval();
    bool OnList(const std::string& goal, const char* what);
    bool NoList(const char* what);
    bool ScanList(const std::string& v, const std::string& goal);
  protected:
    RuleContainer() {}

  private:
    std::map<std::string, std::unique_ptr<RuleList>> namedRules;
    std::list<std::unique_ptr<RuleList>> implicitRules;
    static std::shared_ptr<RuleContainer> instance;
};
#endif