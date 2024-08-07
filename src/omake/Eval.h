/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#ifndef EVAL_H
#define EVAL_H

#include <string>
#include <unordered_map>
#include <vector>
#include <list>
#include <set>
#include "os.h"
#include <mutex>
class RuleList;
class Rule;
class Variable;

#define SpaceThunk "\xff"

class Eval
{
  public:
    Eval(const std::string name, bool expandWildcards, std::shared_ptr<RuleList> ruleList = nullptr, std::shared_ptr<Rule> rule = nullptr);
    ~Eval() {}
    std::string Evaluate();
    void PushCallArg(std::string arg) { callArgs.push_back(arg); }
    static void Clear();
    static std::string GetVPATH(const std::string& name);
    static void AddVPath(const std::string& pattern, const std::string& dirs) { vpaths[pattern] = dirs; }
    static void RemoveVPath(const std::string& pattern);
    static void RemoveAllVPaths() { vpaths.clear(); }
    static void SetRuleStack(std::list<std::shared_ptr<RuleList>>& list)
    {
        std::lock_guard<decltype(evalLock)> lk(evalLock);
        ruleStack = list;
    }
    static void ClearRuleStack()
    {
        std::lock_guard<decltype(evalLock)> lk(evalLock);
        ruleStack.clear();
    }
    static void PushruleStack(std::shared_ptr<RuleList>& ruleList)
    {
        std::lock_guard<decltype(evalLock)> lk(evalLock);
        ruleStack.push_front(ruleList);
    }
    static void PopruleStack()
    {
        std::lock_guard<decltype(evalLock)> lk(evalLock);
        ruleStack.pop_front();
    }
    static void SetWarnings(bool flag) { internalWarnings = flag; }
    static bool GetWarnings() { return internalWarnings; }
    static void SetFile(const std::string& File) { file = File; }
    static void SetLine(int Lineno) { lineno = Lineno; }
    static std::string ExtractFirst(std::string& value, const std::string& seps);
    static void StripLeadingSpaces(std::string& value);
    void InternalWarning(const std::string& Warning)
    {
        if (internalWarnings)
            warning(Warning);
    }
    static std::string AdjustForSpaces(const std::string& in);
    static size_t MacroSpan(const std::string iline, size_t pos);
    std::string ParseMacroLine(const std::string& in);
    static Variable* LookupVariable(const std::string& name);
    bool AutomaticVar(const std::string& name, std::string& rv);
    std::string ExpandMacro(const std::string& name);
    static size_t FindPercent(const std::string& name, size_t pos = 0);
    static std::string FindStem(const std::string& name, const std::string& pattern);
    static std::string ReplaceQuotes(const std::string& value);
    static std::string ReplaceStem(const std::string& stem, const std::string& pattern);
    static size_t MatchesPattern(const std::string& name, const std::string& pattern, size_t& start, size_t begin = 0);
    static int FindBalancedComma(const std::string& right);
    static bool TwoArgs(const std::string& line, std::string& left, std::string& right);
    static bool ThreeArgs(const std::string& line, std::string& one, std::string& two, std::string& three);
    static int GetNumber(const std::string& line);
    // string functions
    std::string subst(const std::string& arglist);
    std::string patsubst(const std::string& arglist);
    std::string strip(const std::string& arglist);
    std::string findstring(const std::string& arglist);
    std::string filter(const std::string& arglist);
    std::string filterout(const std::string& arglist);
    std::string sort(const std::string& arglist);
    std::string word(const std::string& arglist);
    std::string wordlist(const std::string& arglist);
    std::string words(const std::string& arglist);
    std::string firstword(const std::string& arglist);
    std::string lastword(const std::string& arglist);
    // file functions
    std::string dir(const std::string& names);
    std::string notdir(const std::string& names);
    std::string suffix(const std::string& names);
    std::string basename(const std::string& names);
    std::string addsuffix(const std::string& arglist);
    std::string addprefix(const std::string& arglist);
    std::string wildcard(const std::string& arglist);
    std::string wildcardinternal(std::string& names);
    std::string join(const std::string& arglist);
    std::string realpath(const std::string& arglist);
    std::string abspath(const std::string& arglist);
    std::string condIf(const std::string& arglist);
    std::string condOr(const std::string& arglist);
    std::string condAnd(const std::string& arglist);
    std::string foreach (const std::string& arglist);
    std::string call(const std::string& arglist);
    std::string value(const std::string& arglist);
    std::string eval(const std::string& arglist);
    std::string origin(const std::string& arglist);
    std::string flavor(const std::string& arglist);
    std::string shell(const std::string& arglist);
    static std::string error(const std::string& arglist, const std::string fileOverride = "", int lineOverride = 0);
    std::string errorx(const std::string& arglist);
    static std::string warning(const std::string& arglist, const std::string fileOverride = "", int lineOverride = 0);
    std::string warningx(const std::string& arglist);
    std::string info(const std::string& arglist);
    std::string exists(const std::string& arglist);
    // internal
    static int GetErrCount() { return errcount; }

  private:
    typedef std::string (Eval::*StringFunc)(const std::string& arglist);
    static std::unordered_map<std::string, StringFunc> builtins;
    static std::string VPath;
    static std::unordered_map<std::string, std::string> vpaths;
    static bool internalWarnings;
    static int lineno;
    static std::string file;
    static std::list<std::shared_ptr<RuleList>> ruleStack;
    static std::list<Variable*> foreachVars;
    static std::set<std::string> macroset;
    static std::string GPath;
    static std::vector<std::string> callArgs;
    static std::mutex evalLock;
    std::string str;
    std::shared_ptr<Rule> rule;
    std::shared_ptr<RuleList> ruleList;
    bool expandWildcards;
    static int errcount;
};
#endif