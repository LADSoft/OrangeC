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
#ifndef EVAL_H
#define EVAL_H

#include <string>
#include <map>
#include <vector>
#include <deque>
#include <set>

class RuleList;
class Rule;
class Variable;
class Eval
{
public:
    Eval(const std::string name, bool expandWildcards, RuleList *ruleList = NULL, Rule *rule = NULL);
    ~Eval() { }
    std::string Evaluate();
    void PushCallArg(std::string arg)
    {
        args.push_back(arg);
    }
    static void Clear();
    static std::string GetVPATH(const std::string &name);
    static void AddVPath(const std::string &pattern, const std::string &dirs) { vpaths[pattern] = dirs; }
    static void RemoveVPath(const std::string &pattern);
    static void RemoveAllVPaths() { vpaths.clear(); }
    static void PushruleStack( RuleList *ruleList) { ruleStack.push_front(ruleList); }
    static void PopruleStack() { ruleStack.pop_front(); }
    static void SetWarnings(bool flag) { internalWarnings = flag; }
    static bool GetWarnings() { return internalWarnings; }
    static void SetFile(const std::string &File) { file = File; }
    static void SetLine(int Lineno) { lineno = Lineno; }
    static std::string ExtractFirst(std::string &value, const std::string &seps);	
    static void StripLeadingSpaces(std::string &value);
    void InternalWarning(const std::string &Warning)
    {
        if (internalWarnings)
            warning(Warning);
    }
    static int MacroSpan(const std::string iline, int pos);
    std::string ParseMacroLine(const std::string &in);
    static Variable *LookupVariable(const std::string &name);
    bool AutomaticVar(const std::string &name, std::string &rv);
    std::string ExpandMacro(const std::string &name);
    static size_t FindPercent(const std::string &name, size_t pos = 0);
    static std::string FindStem(const std::string &name, const std::string &pattern);
    static std::string ReplaceQuotes(const std::string &value);
    static std::string ReplaceStem(const std::string &stem, const std::string &pattern);
    static size_t MatchesPattern(const std::string &name, const std::string &pattern, size_t &start, size_t begin = 0);
    static int FindBalancedComma(const std::string &right);
    static bool TwoArgs(const std::string &line, std::string &left, std::string &right);
    static bool ThreeArgs(const std::string &line, std::string &one, std::string &two, std::string &three);
    static int GetNumber(const std::string &line);
    // string functions
    std::string subst(const std::string &arglist);
    std::string patsubst(const std::string &arglist);
    std::string strip(const std::string &arglist);
    std::string findstring(const std::string &arglist);
    std::string filter(const std::string &arglist);
    std::string filterout(const std::string &arglist);
    std::string sort(const std::string &arglist);
    std::string word(const std::string &arglist);
    std::string wordlist(const std::string &arglist);
    std::string words(const std::string &arglist);
    std::string firstword(const std::string &arglist);
    std::string lastword(const std::string &arglist);
    // file functions
    std::string dir(const std::string &names);
    std::string notdir(const std::string &names);
    std::string suffix(const std::string &names);
    std::string basename(const std::string &names);
    std::string addsuffix(const std::string &arglist);
    std::string addprefix(const std::string &arglist);
    std::string wildcard(const std::string &arglist);
    std::string wildcardinternal(std::string &names);
    std::string join(const std::string &arglist);
    std::string realpath(const std::string &arglist);
    std::string abspath(const std::string &arglist);
    std::string condIf(const std::string &arglist);
    std::string condOr(const std::string &arglist);
    std::string condAnd(const std::string &arglist);
    std::string foreach(const std::string &arglist);
    std::string call(const std::string &arglist);
    std::string value(const std::string &arglist);
    std::string eval(const std::string &arglist);
    std::string origin(const std::string &arglist);
    std::string flavor(const std::string &arglist);
    std::string shell(const std::string &arglist);
    static std::string error(const std::string &arglist);
    std::string errorx(const std::string &arglist);
    static std::string warning(const std::string &arglist);
    std::string warningx(const std::string &arglist);
    std::string info(const std::string &arglist);
    std::string exists(const std::string &arglist);
    // internal
    static void Init();
    static int GetErrCount() { return errcount; }
private:
    typedef std::string (Eval::*StringFunc)(const std::string &arglist);
    static std::map<const std::string, StringFunc> builtins;
    static std::string VPath;
    static std::map<std::string, std::string> vpaths;
    static bool internalWarnings;
    static int lineno;
    static std::string file;
    static std::deque<RuleList *> ruleStack;
    static std::deque<Variable *> foreachVars;
    static std::set<std::string> macroset;
    static std::string GPath;
    std::vector<std::string> args;
    std::string str;
    Rule *rule;
    RuleList *ruleList;
    bool expandWildcards;
    static int errcount;
};
#endif