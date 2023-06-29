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

#include "Eval.h"
#include "Rule.h"
#include "CmdFiles.h"
#include "Variable.h"
#include "Parser.h"
#include "Spawner.h"
#include "os.h"
#include "Maker.h"
#include <cctype>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>
#include "Utils.h"

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#else
#    include <io.h>
#endif

std::string Eval::VPath;
std::unordered_map<std::string, std::string> Eval::vpaths;
bool Eval::internalWarnings;
int Eval::lineno;
std::string Eval::file;
std::list<RuleList*> Eval::ruleStack;
std::list<Variable*> Eval::foreachVars;
std::set<std::string> Eval::macroset;
std::string Eval::GPath;
int Eval::errcount;
std::vector<std::string> Eval::callArgs;
std::mutex Eval::evalLock;
std::unordered_map<std::string, Eval::StringFunc> Eval::builtins = {{"subst", &Eval::subst},
                                                                    {"patsubst", &Eval::patsubst},
                                                                    {"strip", &Eval::strip},
                                                                    {"findstring", &Eval::findstring},
                                                                    {"filter", &Eval::filter},
                                                                    {"filter-out", &Eval::filterout},
                                                                    {"sort", &Eval::sort},
                                                                    {"word", &Eval::word},
                                                                    {"wordlist", &Eval::wordlist},
                                                                    {"words", &Eval::words},
                                                                    {"firstword", &Eval::firstword},
                                                                    {"lastword", &Eval::lastword},
                                                                    {"dir", &Eval::dir},
                                                                    {"notdir", &Eval::notdir},
                                                                    {"suffix", &Eval::suffix},
                                                                    {"basename", &Eval::basename},
                                                                    {"addsuffix", &Eval::addsuffix},
                                                                    {"addprefix", &Eval::addprefix},
                                                                    {"wildcard", &Eval::wildcard},
                                                                    {"join", &Eval::join},
                                                                    {"realpath", &Eval::realpath},
                                                                    {"abspath", &Eval::abspath},
                                                                    {"if", &Eval::condIf},
                                                                    {"or", &Eval::condOr},
                                                                    {"and", &Eval::condAnd},
                                                                    {"foreach", &Eval::foreach},
                                                                    {"call", &Eval::call},
                                                                    {"value", &Eval::value},
                                                                    {"eval", &Eval::eval},
                                                                    {"origin", &Eval::origin},
                                                                    {"flavor", &Eval::flavor},
                                                                    {"shell", &Eval::shell},
                                                                    {"error", &Eval::errorx},
                                                                    {"warning", &Eval::warningx},
                                                                    {"info", &Eval::info},
                                                                    {"exists", &Eval::exists}};

Eval::Eval(const std::string name, bool ExpandWildcards, RuleList* RuleList, Rule* Rule) :
    str(name), expandWildcards(ExpandWildcards), ruleList(RuleList), rule(Rule)
{
}
void Eval::Clear()
{
    std::lock_guard<decltype(evalLock)> lk(evalLock);
    vpaths.clear();
    VPath = "";
    GPath = "";
    internalWarnings = false;
    ruleStack.clear();
    foreachVars.clear();
    macroset.clear();
    errcount = 0;
}

std::string Eval::Evaluate()
{
    std::string rv = ParseMacroLine(str);
    if (expandWildcards)
        rv = wildcardinternal(rv);
    return rv;
}

std::string Eval::GetVPATH(const std::string& goal)
{
    Variable* v = VariableContainer::Instance()->Lookup("VPATH");
    std::string rv;
    if (v)
    {
        if (v->GetFlavor() == Variable::f_recursive)
        {
            Eval r(v->GetValue(), false, nullptr, nullptr);
            rv = r.Evaluate();
        }
        else
        {
            rv = v->GetValue();
        }
    }
    if (!goal.empty())
    {
        for (auto&& path : vpaths)
        {
            size_t start;
            size_t len;
            len = MatchesPattern(goal, path.first, start, 0);
            if (len == goal.size())
            {
                if (!rv.empty())
                    rv += " ";
                rv += path.second;
            }
        }
    }
    return rv;
}
void Eval::RemoveVPath(const std::string& path)
{

    auto it = vpaths.find(path);
    if (it != vpaths.end())
        vpaths.erase(it);
}
std::string Eval::ExtractFirst(std::string& value, const std::string& seps)
{
    StripLeadingSpaces(value);
    int n = value.size();
    if (seps == " " && (value[0] == '"' || value[0] == '\''))
    {
        int m = value.find_first_of(value[0], 1);
        if (m != std::string::npos)
            n = m + 1;
    }
    else
    {
        int m;
        if (seps == " ")
            m = value.find_first_of(" \t\n");
        else
            m = value.find_first_of(seps);
        if (m != std::string::npos)
            n = m;
    }
    std::string rv = value.substr(0, n);
    if (value.find_first_not_of(" \t", n) == std::string::npos)
        value.replace(0, value.size(), "");
    else if ((value[0] == '"' || value[0] == '\'') && seps == " ")
        value.replace(0, n, "");
    else
        value.replace(0, n + 1, "");

    return rv;
}
void Eval::StripLeadingSpaces(std::string& value)
{
    int n = value.find_first_not_of(' ');
    if (n != 0)
        value.replace(0, n, "");
}
size_t Eval::MacroSpan(const std::string line, size_t pos)
{
    std::list<char> stack;
    size_t pos1 = pos;
    while (line[pos1] == '$')
        pos1++;
    if (line[pos1] != '(' && line[pos1] != '{')
    {
        if (line[pos1 + 1] == 'F' || line[pos1 + 1] == 'D')
            pos1++;
        return 1 + pos1 - pos;
    }

    for (; pos1 < line.size(); pos1++)
    {
        if (line[pos1] == '(')
            stack.push_front(')');
        else if (line[pos1] == '{')
            stack.push_front('}');
        else if (line[pos1] == stack.front())
        {
            stack.pop_front();
            if (stack.empty())
                return pos1 - pos + 1;
        }
    }
    return std::string::npos;
}
std::string Eval::ParseMacroLine(const std::string& in)
{
    std::string rv;
    int n = 0;
    int m = in.find_first_of('$');
    while (m != std::string::npos)
    {
        rv += in.substr(n, m - n);
        if (m != in.size() - 1 && in[m + 1] == '$')
        {
            n = m + 1;
            m = MacroSpan(in, n);
            if (m == std::string::npos)
                m = 1;
            if (in[n + 1] == '(' && m >= 3)
            {
                rv += "$(";
                std::string temp = in.substr(n + 2, m - 3);
                for (size_t q = 0; q < temp.size() - 1; q++)
                {
                    if (temp[q] == '$')
                    {
                        std::string temp1 = temp.substr(q + 1, 1), temp2;
                        if (AutomaticVar(temp1, temp2))
                        {
                            temp = temp.substr(0, q) + temp2 + (q < temp.size() - 2 ? temp.substr(q + 2) : "");
                            q += temp2.size() - 2;
                        }
                    }
                }
                rv += temp;
                rv += ")";
                n = m + n;
                m = 0;
            }
            m = in.find_first_of('$', n + m);
        }
        else
        {
            n = MacroSpan(in, m + 1);
            if (n == 1 || n == 2)
            {
                rv += ExpandMacro(in.substr(m + 1, n));
                n = m + 1 + n;
            }
            else if (n != std::string::npos)
            {
                rv += ExpandMacro(in.substr(m + 2, n - 2));
                n = m + n + 1;
            }
            m = in.find_first_of('$', n);
        }
    }
    if (n != std::string::npos)
        rv += in.substr(n, in.size());
    return rv;
}
Variable* Eval::LookupVariable(const std::string& name)
{
    Variable* v = nullptr;
    std::lock_guard<decltype(evalLock)> lk(evalLock);

    for (auto it = foreachVars.begin(); it != foreachVars.end() && v == nullptr; ++it)
    {
        if ((*it)->GetName() == name)
            v = (*it);
    }
    if (!v)
    {
        for (auto it = ruleStack.begin(); it != ruleStack.end() && v == nullptr; ++it)

        {
            v = (*it)->Lookup(name);
        }
    }
    if (!v)
    {
        v = VariableContainer::Instance()->Lookup(name);
    }
    return v;
}
bool Eval::AutomaticVar(const std::string& name, std::string& rv)
{
    bool found = false;
    std::set<std::string> set;  // has to be at this scope to make openwatcom happy
    if (ruleList && name.size() <= 2)
    {
        std::string extra;
        if (name[0] == '@')  // target file name
        {
            //            rv = Maker::GetFullName(ruleList->GetTarget());
            rv = ruleList->GetTarget();
            found = true;
        }
        else if (name[0] == '%')  // empty
        {
            rv = "";
            found = false;
        }
        else if (name[0] == '<')  // first prereq of first rule
        {
            auto it = ruleList->begin();
            while (it != ruleList->end() && (*it)->GetCommands()->size() == 0)
                ++it;
            if (it != ruleList->end())
            {
                if (!rule || (*it).get() != rule)
                {
                    extra = (*it)->GetPrerequisites();
                    rv = ExtractFirst(extra, " ");
                    //                    rv = Maker::GetFullName(rv);
                }
                found = true;
            }
        }
        else if (name[0] == '^')  // all prereq or prereq of rules that have appeared
        {
            for (auto& item : *ruleList)
            {
                if (item.get() == rule)
                    break;
                extra = item->GetPrerequisites();
                while (!extra.empty())
                {
                    std::string temp = ExtractFirst(extra, " ");
                    if (set.find(temp) == set.end())
                    {
                        set.insert(temp);
                        if (!rv.empty())
                            rv += " ";
                        rv += Maker::GetFullName(temp);
                    }
                }
            }
            found = true;
        }
        else if (name[0] == '+')  // same with repetition
        {
            for (auto& item : *ruleList)
            {
                if (item.get() == rule)
                    break;
                extra = item->GetPrerequisites();
                while (!extra.empty())
                {
                    std::string temp = ExtractFirst(extra, " ");
                    if (!rv.empty())
                        rv += " ";
                    rv += Maker::GetFullName(temp);
                }
            }
            found = true;
        }
        else if (name[0] == '*')  // implicit rule stem/target name - recognized suffix or target stem
        {
            rv = ruleList->GetTargetPatternStem();
            if (rv.empty())
            {
                rv = ruleList->GetTarget();
                size_t n = rv.find_last_of('.');
                if (n != std::string::npos)
                {
                    RuleList* rl = RuleContainer::Instance()->Lookup(".SUFFIXES");
                    if (rl)
                    {
                        std::string sfx = rv.substr(n);
                        bool found = false;
                        for (auto it = rl->begin(); !found && it != rl->end(); ++it)
                        {
                            std::string working = (*it)->GetPrerequisites();
                            while (!working.empty())
                            {
                                extra = ExtractFirst(working, " ");
                                if (extra == sfx)
                                {
                                    found = true;
                                    rv.replace(n, rv.size() - n, "");
                                }
                            }
                        }
                    }
                }
            }
            found = true;
        }
        else if (name[0] == '?')  // or names of prereqs newer than target
        {
            extra = ruleList->GetNewerPrerequisites();
            while (!extra.empty())
            {
                if (!rv.empty())
                    rv += " ";
                rv += Maker::GetFullName(ExtractFirst(extra, " "));
            }
            found = true;
        }
        else if (name[0] == '|')  // or names of order-only prerequisites
        {
            std::set<std::string> set;
            for (auto& item : *ruleList)
            {
                if (item.get() == rule)
                    break;
                extra = item->GetOrderPrerequisites();
                while (!extra.empty())
                {
                    std::string temp = ExtractFirst(extra, " ");
                    if (set.find(temp) == set.end())
                    {
                        set.insert(temp);
                        if (!rv.empty())
                            rv += " ";
                        rv += Maker::GetFullName(temp);
                    }
                }
            }
            found = true;
        }
        if (found && name.size() == 2)
        {
            if (name[1] == 'D')
            {
                extra = rv;
                rv = "";
                while (!extra.empty())
                {
                    std::string temp = ExtractFirst(extra, " ");
                    size_t n = temp.find_last_of("/\\");
                    if (!rv.empty())
                        rv += " ";
                    if (n != std::string::npos)
                    {
                        rv += temp.substr(0, n + 1);
                    }
                    else
                    {
                        rv += temp;
                    }
                }
            }
            else if (name[1] == 'F')
            {
                extra = rv;
                rv = "";
                while (!extra.empty())
                {
                    std::string temp = ExtractFirst(extra, " ");
                    size_t n = temp.find_last_of("/\\");
                    if (!rv.empty())
                        rv += " ";
                    if (n != std::string::npos)
                    {
                        rv += temp.substr(n + 1);
                    }
                    else
                    {
                        rv += temp;
                    }
                }
            }
            else
            {
                found = false;
                rv = "";
            }
        }
    }
    return found;
}
std::string Eval::ExpandMacro(const std::string& name)
{
    std::string rv;
    std::string extra;

    if (AutomaticVar(name, rv))
        return rv;
    if (name == ".VARIABLES")
    {
        for (auto& var : *VariableContainer::Instance())
        {
            if (!rv.empty())
                rv += " ";
            rv += var.first;
        }
    }
    else if (name[0] == '$')
    {
        size_t n = MacroSpan(name, 1);
        rv = name.substr(0, n + 1);
        extra = std::string(name.substr(n + 1));
        Eval a(rv, false, ruleList, rule);
        rv = a.Evaluate();
    }
    else
    {
        std::string temp = name;
        std::string fw = ExtractFirst(temp, " ");
        auto it = builtins.find(fw);
        if (it != builtins.end())
        {
            size_t z = name.find_first_not_of(' ', fw.size());
            std::string args;
            if (z != std::string::npos)
                args = name.substr(z);
            else
                goto join;
            rv = (this->*(it->second))(args);
        }
        else if (isdigit(name[0]))
        {
            int index = GetNumber(name);
            if (index < callArgs.size())
                rv = callArgs[index];
        }
        else
        {
        join:
            int m = name.find_first_of(':');
            if (m != std::string::npos)
            {
                extra = name.substr(m);
                rv = name.substr(0, m);
                Eval a(extra, false, ruleList, rule);
                extra = a.Evaluate();
            }
            else
            {
                rv = name;
            }
            bool found = false;
            Variable* v = LookupVariable(rv);
            if (v)
            {
                rv = v->GetValue();
                if (v->GetFlavor() == Variable::f_recursive)
                {
                    if (macroset.find(v->GetName()) == macroset.end())
                    {
                        auto p = macroset.insert(v->GetName());
                        rv = ParseMacroLine(rv);
                        macroset.erase(p.first);
                    }
                }
            }
            else
            {
                rv = "";
                if (internalWarnings)
                {
                    warning("'" + fw + "' is undefined.");
                }
            }
        }
    }
    if (!extra.empty())
    {
        int m = extra.find_first_not_of(' ');
        if (m < 0 || extra[m] != ':')
        {
            error("Invalid macro qualifier");
        }
        else
        {
            m++;
            int m1 = extra.find_first_of('=');
            if (m1 != std::string::npos)
            {
                std::string pat = extra.substr(m, m1 - m);
                std::string rep = extra.substr(m1 + 1);
                int n = pat.find_first_not_of(' ');
                if (pat[n] == '.')
                    pat.replace(0, n, "%");
                else if (n)
                    pat.replace(0, n, "");
                n = rep.find_first_not_of(' ');
                if (n == std::string::npos)
                {
                    pat = extra.substr(m, m1 - m);
                    while ((n = rv.find(pat)) != std::string::npos)
                    {
                        rv.replace(n, pat.size(), "");
                    }
                }
                else
                {
                    if (rep[n] == '.')
                        rep.replace(0, n, "%");
                    else
                        rep.replace(0, n, "");
                    rv = patsubst(pat + "," + rep + "," + rv);
                }
            }
            else
            {
                error("Invalid macro qualifier");
            }
        }
    }
    //   std::replace(rv.begin(), rv.end(), '\t', ' ');
    //    std::replace(rv.begin(), rv.end(), '\n', ' ');
    return rv;
}
size_t Eval::FindPercent(const std::string& name, size_t pos)
{
    pos = name.find_first_of('%', pos);
    return pos;
}
std::string Eval::FindStem(const std::string& name, const std::string& pattern)
{
    int n = FindPercent(pattern);
    if (n != std::string::npos && !name.empty())
    {
        int m1 = pattern.find_first_not_of(' ');
        int m2 = pattern.find_last_not_of(' ');
        int n1 = name.find_first_not_of(' ');
        int n2 = name.find_last_not_of(' ');
        return name.substr(n1 - m1 + n, n2 - m2 + 1 - (n1 - m1));
    }
    return "";
}
std::string Eval::ReplaceQuotes(const std::string& value)
{
    int m = 0;
    std::string rv;
    while (m != std::string::npos)
    {
        int n = value.find_first_of(m);
        if (n != std::string::npos)
        {
            if (n > 0 && value[n - 1] == '\\')
                if (n > 1 && value[n - 2] == '\\')
                {
                    rv += value.substr(m, n - 2);
                    m = n - 1;
                }
                else
                {
                    rv += value.substr(m, n - 1);
                    m = n;
                }
            else
            {
                rv += value.substr(m, n);
                m = n;
            }
        }
        else
        {
            rv += value.substr(m);
            m = n;
        }
    }
    return rv;
}
std::string Eval::ReplaceStem(const std::string& stem, const std::string& pattern)
{
    int n = FindPercent(pattern);
    if (n != std::string::npos)
    {
        std::string rv = ReplaceQuotes(pattern.substr(0, n)) + stem + ReplaceQuotes(pattern.substr(n + 1));
        return rv;
    }
    return pattern;
}
size_t Eval::MatchesPattern(const std::string& name, const std::string& pattern, size_t& start, size_t begin)
{
    size_t rv = std::string::npos;
    size_t m = FindPercent(pattern, begin);
    if (m != std::string::npos)
    {
        size_t m1 = m;
        size_t m2 = m;
        while (m1 && pattern[m1 - 1] != ' ')
            m1--;
        while (m2 < pattern.size() && pattern[m2] != ' ')
            m2++;
        int n1 = name.find_first_not_of(' ');
        while (n1 != std::string::npos)
        {
            int n2 = name.find_first_of(' ', n1);
            if (n2 == std::string::npos)
                n2 = name.size();
            if (name.substr(n1, m - m1) == pattern.substr(m1, m - m1))
            {
                if (n2 > (m2 - m - 1))
                    if (name.substr(n2 - (m2 - m - 1), m2 - m - 1) == pattern.substr(m + 1, m2 - m - 1))
                    {
                        start = m;
                        rv = n2;
                        break;
                    }
            }
            n1 = name.find_first_not_of(' ', n2);
        }
    }
    else if (name == pattern)
    {
        rv = name.size();
        start = 0;
    }
    return rv;
}
int Eval::FindBalancedComma(const std::string& right)
{
    int nest = 0;
    size_t n;
    for (n = 0; n < right.size(); ++n)
    {
        if (right[n] == '(')
            nest++;
        else if (right[n] == ')' && nest)
            nest--;
        else if (!nest && right[n] == ',')
            break;
    }
    if (n == right.size())
        n = std::string::npos;
    return n;
}
bool Eval::TwoArgs(const std::string& line, std::string& left, std::string& right)
{
    bool rv = false;
    int n = FindBalancedComma(line);
    if (n != std::string::npos)
    {
        left = line.substr(0, n);
        right = line.substr(n + 1);
        n = FindBalancedComma(right);
        if (n == std::string::npos)
            rv = true;
    }
    if (!rv)
        error("Expected two arguments");
    return rv;
}
bool Eval::ThreeArgs(const std::string& line, std::string& one, std::string& two, std::string& three)
{
    bool rv = false;
    int n = FindBalancedComma(line);
    if (n != std::string::npos)
    {
        one = line.substr(0, n);
        two = line.substr(n + 1);
        n = FindBalancedComma(two);
        if (n != std::string::npos)
        {
            three = two.substr(n + 1);
            two.replace(n, two.size() - n, "");
            n = FindBalancedComma(three);
            if (n == std::string::npos)
                rv = true;
        }
    }
    if (!rv)
        error("Expected three arguments");
    return rv;
}
int Eval::GetNumber(const std::string& line)
{
    int rv = 0;
    size_t i;
    size_t n = line.find_first_not_of(' ');
    size_t m = line.find_last_not_of(' ');
    if (m == std::string::npos)
        m = line.size();
    else
        m++;
    for (i = n; i < m; i++)
        if (!isdigit(line[i]))
            break;
    if (i != m)
        error("Numeric value expected");
    else
    {
        rv = Utils::StringToNumber(line.substr(n, m - n));
    }
    return rv;
}
std::string Eval::subst(const std::string& arglist)
{
    std::string from;
    std::string to;
    std::string text;
    if (ThreeArgs(arglist, from, to, text))
    {
        Eval f(from, false, ruleList, rule);
        from = f.Evaluate();
        Eval t(to, false, ruleList, rule);
        to = t.Evaluate();
        Eval x(text, false, ruleList, rule);
        text = x.Evaluate();
        int m = text.find(from);
        while (m != std::string::npos)
        {
            text.replace(m, from.size(), to);
            m += to.size();
            m = text.find(from, m);
        }
    }
    return text;
}
std::string Eval::patsubst(const std::string& arglist)
{
    std::string pattern;
    std::string replacement;
    std::string text;
    if (ThreeArgs(arglist, pattern, replacement, text))
    {
        Eval o(pattern, false, ruleList, rule);
        pattern = o.Evaluate();
        Eval t(replacement, false, ruleList, rule);
        replacement = t.Evaluate();
        Eval r(text, false, ruleList, rule);
        text = r.Evaluate();
        size_t start;
        size_t n = 0;
        std::string rv;
        while (!text.empty())
        {
            std::string thisOne = ExtractFirst(text, " ");
            if (MatchesPattern(thisOne, pattern, start, 0) != std::string::npos)
            {
                std::string stem = FindStem(thisOne, pattern);
                thisOne = ReplaceStem(stem, replacement);
            }
            if (!rv.empty())
                rv += " ";
            rv += thisOne;
        }
        return rv;
    }
    return text;
}
std::string Eval::strip(const std::string& arglist)
{
    std::string rv;
    Eval e(arglist, false, ruleList, rule);
    std::string a = e.Evaluate();
    size_t m = a.find_first_not_of("\t ");
    size_t n = a.find_first_of("\t ", m);
    while (n != std::string::npos)
    {
        rv += a.substr(m, n - m);
        n = a.find_first_not_of("\t ", n);
        m = n;
        if (n != std::string::npos)
            rv += " ";
        n = a.find_first_of("\t ", n);
    }
    if (m != std::string::npos)
        rv += a.substr(m);
    return rv;
}
std::string Eval::findstring(const std::string& arglist)
{
    std::string find;
    std::string in;
    std::string rv;
    if (TwoArgs(arglist, find, in))
    {
        Eval f(find, false, ruleList, rule);
        find = f.Evaluate();
        Eval i(in, false, ruleList, rule);
        in = i.Evaluate();
        if (in.find(find) != std::string::npos)
            rv = find;
    }
    return rv;
}
std::string Eval::filter(const std::string& arglist)
{
    std::string pattern;
    std::string text;
    std::string rv;
    if (TwoArgs(arglist, pattern, text))
    {
        Eval p(pattern, false, ruleList, rule);
        pattern = p.Evaluate();
        Eval t(text, false, ruleList, rule);
        text = t.Evaluate();
        while (!text.empty())
        {
            std::string working = pattern;
            std::string fw = ExtractFirst(text, " ");
            while (!working.empty())
            {
                std::string p = ExtractFirst(working, " ");
                size_t xx = 0;
                if (MatchesPattern(fw, p, xx) == fw.size())
                {
                    if (!rv.empty())
                        rv += " ";
                    rv += fw;
                }
            }
        }
    }
    return rv;
}
std::string Eval::filterout(const std::string& arglist)
{
    std::string pattern;
    std::string text;
    std::string rv;
    if (TwoArgs(arglist, pattern, text))
    {
        Eval p(pattern, false, ruleList, rule);
        pattern = p.Evaluate();
        Eval t(text, false, ruleList, rule);
        text = t.Evaluate();
        while (!text.empty())
        {
            std::string working = pattern;
            std::string fw = ExtractFirst(text, " ");
            bool notfound = true;
            while (!working.empty())
            {
                std::string p = ExtractFirst(working, " ");
                size_t xx = 0;
                notfound &= (MatchesPattern(fw, p, xx) != fw.size());
            }
            if (notfound)
            {
                if (!rv.empty())
                    rv += " ";
                rv += fw;
            }
        }
    }
    return rv;
}
std::string Eval::sort(const std::string& arglist)
{
    std::set<std::string> sortList;
    Eval a(arglist, false, ruleList, rule);
    std::string working = a.Evaluate();
    while (!working.empty())
    {
        sortList.insert(ExtractFirst(working, " "));
    }
    std::string rv;
    for (auto&& strng : sortList)
    {
        if (!rv.empty())
            rv += " ";
        rv += strng;
    }
    return rv;
}
std::string Eval::word(const std::string& arglist)
{
    std::string count;
    std::string text;
    std::string rv;
    if (TwoArgs(arglist, count, text))
    {
        Eval c(count, false, ruleList, rule);
        count = c.Evaluate();
        Eval t(text, false, ruleList, rule);
        text = t.Evaluate();
        int n = GetNumber(count);
        for (int i = 0; i < n; i++)
        {
            if (text.empty())
            {
                rv = "";
                break;
            }
            rv = ExtractFirst(text, " ");
        }
    }
    return rv;
}
std::string Eval::wordlist(const std::string& arglist)
{
    std::string start;
    std::string end;
    std::string text;
    std::string rv;
    if (ThreeArgs(arglist, start, end, text))
    {
        Eval s(start, false, ruleList, rule);
        start = s.Evaluate();
        Eval e(end, false, ruleList, rule);
        end = e.Evaluate();
        Eval t(text, false, ruleList, rule);
        text = t.Evaluate();
        int sn = GetNumber(start);
        int en = GetNumber(end);
        for (int i = 1; i < sn; i++)
        {
            if (text.empty())
            {
                break;
            }
            ExtractFirst(text, " ");
        }
        for (int i = sn; i <= en; i++)
        {
            if (text.empty())
            {
                break;
            }
            if (!rv.empty())
                rv += " ";
            rv += ExtractFirst(text, " ");
        }
    }
    return rv;
}
std::string Eval::words(const std::string& arglist)
{
    std::string text;
    Eval t(arglist, false, ruleList, rule);
    text = t.Evaluate();
    size_t n = 0;
    size_t count = 0;
    n = text.find_first_not_of(' ');
    if (n != std::string::npos)
    {
        while (!text.empty())
        {
            count++;
            ExtractFirst(text, " ");
        }
    }
    std::string rv = Utils::NumberToString(count);
    return rv;
}
std::string Eval::firstword(const std::string& arglist)
{
    std::string text;
    Eval t(arglist, false, ruleList, rule);
    text = t.Evaluate();
    return ExtractFirst(text, " ");
}

std::string Eval::lastword(const std::string& arglist)
{
    std::string text;
    Eval t(arglist, false, ruleList, rule);
    text = t.Evaluate();
    size_t n = text.find_last_not_of(' ');
    std::string rv;
    if (n != std::string::npos)
    {
        size_t m = n;
        while (m > 0 && text[m] != ' ')
            m--;
        rv = text.substr(m, n + 1);
    }
    return rv;
}

std::string Eval::dir(const std::string& names)
{
    std::string working = names;
    Eval w(working, false, ruleList, rule);
    working = w.Evaluate();
    std::string rv;
    while (!working.empty())
    {
        std::string p = ExtractFirst(working, " ");
        size_t n = p.find_last_of("/\\");
        if (!rv.empty())
            rv += " ";
        if (n != std::string::npos)
            rv += p.substr(0, n + 1);
        else
            rv += std::string("./");
    }
    return rv;
}

std::string Eval::notdir(const std::string& names)
{
    std::string working = names;
    Eval w(working, false, ruleList, rule);
    working = w.Evaluate();
    std::string rv;
    while (!working.empty())
    {
        std::string p = ExtractFirst(working, " ");
        size_t n = p.find_last_of("/\\");
        std::string intermed;
        if (n != std::string::npos)
            intermed = p.substr(n + 1);
        else
            intermed = p;
        if (!rv.empty() && !intermed.empty())
            rv += " ";
        rv += intermed;
    }
    return rv;
}

std::string Eval::suffix(const std::string& names)
{
    std::string working = names;
    Eval w(working, false, ruleList, rule);
    working = w.Evaluate();
    std::string rv;
    while (!working.empty())
    {
        std::string p = ExtractFirst(working, " ");
        size_t n = p.find_last_of('.');
        if (n != std::string::npos && (n == p.size() - 1 || (p[n + 1] != '\\' && p[n + 1] != '/')))
        {
            if (!rv.empty())
                rv += " ";
            rv += p.substr(n);
        }
    }
    return rv;
}

std::string Eval::basename(const std::string& names)
{
    std::string working = names;
    Eval w(working, false, ruleList, rule);
    working = w.Evaluate();
    std::string rv;
    while (!working.empty())
    {
        std::string p = ExtractFirst(working, " ");
        size_t n = p.find_last_of('.');
        if (!rv.empty())
            rv += " ";
        if (n != std::string::npos && (n == p.size() - 1 || (p[n + 1] != '\\' && p[n + 1] != '/')))
        {
            rv += p.substr(0, n);
        }
        else
        {
            rv += p;
        }
    }
    return rv;
}

std::string Eval::addsuffix(const std::string& arglist)
{
    std::string suffix;
    std::string names;
    std::string rv;
    if (TwoArgs(arglist, suffix, names))
    {
        Eval s(suffix, false, ruleList, rule);
        suffix = s.Evaluate();
        Eval n(names, false, ruleList, rule);
        names = n.Evaluate();
        if (names.find_last_not_of(' ') != std::string::npos)
        {
            while (!names.empty())
            {
                if (!rv.empty())
                    rv += " ";
                rv += ExtractFirst(names, " ");
                rv += suffix;
            }
        }
    }
    return rv;
}

std::string Eval::addprefix(const std::string& arglist)
{
    std::string prefix;
    std::string names;
    std::string rv;
    if (TwoArgs(arglist, prefix, names))
    {
        Eval p(prefix, false, ruleList, rule);
        prefix = p.Evaluate();
        Eval n(names, false, ruleList, rule);
        names = n.Evaluate();
        if (names.find_last_not_of(' ') != std::string::npos)
        {
            while (!names.empty())
            {
                if (!rv.empty())
                    rv += " ";
                rv += prefix;
                rv += ExtractFirst(names, " ");
            }
        }
    }
    return rv;
}

std::string Eval::wildcard(const std::string& arglist)
{
    std::string names = strip(arglist);
    std::string rv;
    names = wildcardinternal(names);
    while (!names.empty())
    {
        std::string current = ExtractFirst(names, " ");
        if (access(current.c_str(), 0) == 0)
        {
            if (!rv.empty())
                rv += " ";
            rv += current;
        }
    }

    return rv;
}
std::string Eval::wildcardinternal(std::string& names)
{
    CmdFiles files;
    while (!names.empty())
    {
        std::string current = ExtractFirst(names, " ");
        files.Add(current);
    }
    std::string rv;
    for (auto it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
    {
        if (!rv.empty())
            rv += " ";
        rv += (*it);
    }
    return rv;
}

std::string Eval::join(const std::string& arglist)
{
    std::string listone;
    std::string listtwo;
    std::string rv;
    if (TwoArgs(arglist, listone, listtwo))
    {
        Eval l1(listone, false, ruleList, rule);
        listone = strip(l1.Evaluate());
        Eval l2(listtwo, false, ruleList, rule);
        listtwo = strip(l2.Evaluate());
        while (!listone.empty() || listtwo.empty())
        {
            if (!rv.empty())
                rv += " ";
            rv += ExtractFirst(listone, " ");
            rv += ExtractFirst(listtwo, " ");
        }
    }
    // fixme - paths
    return rv;
}

std::string Eval::realpath(const std::string& arglist)
{
    Eval t(arglist, false);
    std::string text = t.Evaluate();
    std::string rv;
    while (!text.empty())
    {
        std::string thisOne = ExtractFirst(text, " ");
        if (thisOne[0] != '\\' && thisOne[1] != ':' && thisOne[0] != '/')  // windows specific
            thisOne = OS::GetWorkingDir() + '/' + thisOne;
        if (!rv.empty())
            rv += " ";
        rv += thisOne;
    }
    return rv;
}

std::string Eval::abspath(const std::string& arglist) { return realpath(arglist); }

std::string Eval::condIf(const std::string& arglist)
{
    size_t n = arglist.find_first_of(',');
    std::string rv;
    if (n == std::string::npos)
    {
        error("Error in 'if' expression");
    }
    else
    {
        std::string ifst = arglist.substr(0, n);
        std::string then = arglist.substr(n + 1);
        std::string els;
        n = then.find_first_of(',');
        if (n != std::string::npos)
        {
            els = then.substr(n + 1);
            then.replace(n, then.size() - n, "");
            if (els.find_first_of(',') != std::string::npos)
                error("Error in 'if' expression");
        }
        n = ifst.find_first_not_of(' ');
        if (n == std::string::npos)
            ifst = "";
        else
        {
            if (n)
                ifst.replace(0, n, "");
            n = ifst.find_last_not_of(' ');
            ifst.replace(n + 1, ifst.size() - n - 1, "");
        }
        Eval i(ifst, false, ruleList, rule);
        ifst = i.Evaluate();
        if (ifst != "")
        {
            Eval t(then, false, ruleList, rule);
            rv = t.Evaluate();
        }
        else
        {
            if (!els.empty())
            {
                Eval e(els, false, ruleList, rule);
                rv = e.Evaluate();
            }
        }
    }
    return rv;
}

std::string Eval::condOr(const std::string& arglist)
{
    std::string left;
    std::string right = arglist;
    size_t n = right.find_first_of(',');
    while (n != std::string::npos)
    {
        left = right.substr(0, n);
        right.replace(0, n + 1, "");
        Eval l(left, false, ruleList, rule);
        left = l.Evaluate();
        if (!left.empty())
            return left;
        n = arglist.find_first_of(',');
    }
    Eval r(right, false, ruleList, rule);
    return r.Evaluate();
}

std::string Eval::condAnd(const std::string& arglist)
{
    std::string left;
    std::string right = arglist;
    size_t n = right.find_first_of(',');
    while (n != std::string::npos)
    {
        left = right.substr(0, n);
        right.replace(0, n + 1, "");
        Eval l(left, false, ruleList, rule);
        left = l.Evaluate();
        if (left.empty())
            return left;
        n = right.find_first_of(',');
    }
    Eval r(right, false, ruleList, rule);
    return r.Evaluate();
}

std::string Eval::foreach (const std::string& arglist)
{
    std::string var;
    std::string list;
    std::string next;
    std::string rv;
    if (ThreeArgs(arglist, var, list, next))
    {
        Eval v1(var, false, ruleList, rule);
        var = v1.Evaluate();
        Eval l(list, false, ruleList, rule);
        list = l.Evaluate();
        if (list.find_first_not_of(' ') != std::string::npos)
        {
            std::unique_ptr<Variable> v = std::make_unique<Variable>(var, list, Variable::f_simple, Variable::o_file);
            foreachVars.push_front(v.get());
            while (!list.empty())
            {
                std::string value = ExtractFirst(list, " ");
                v->SetValue(value);
                Eval t(next, false, ruleList, rule);
                if (!rv.empty())
                    rv += " ";
                rv += t.Evaluate();
            }
            foreachVars.pop_front();
        }
    }
    return rv;
}

std::string Eval::call(const std::string& arglist)
{
    std::string sub = arglist;
    std::string args;
    std::string rv;
    size_t n = sub.find_first_of(',');
    if (n == std::string::npos)
    {
        auto it = builtins.find(sub);
        if (it != builtins.end())
        {
            rv = (this->*(it->second))("");
        }
        else
        {
            sub = "$(" + sub + ")";
            Eval l(sub, false, ruleList, rule);
            rv = l.Evaluate();
        }
    }
    else
    {
        sub = sub.substr(0, n);
        args = arglist.substr(n + 1);
        auto it = builtins.find(sub);
        if (it != builtins.end())
        {
            rv = (this->*(it->second))(args);
        }
        else
        {
            auto oldArgs = callArgs;
            callArgs.clear();
            sub = "$(" + sub + ")";
            Eval l(sub, false, ruleList, rule);
            l.PushCallArg(sub);
            n = args.find_first_of(',');
            while (n != std::string::npos)
            {
                std::string left = args.substr(0, n);
                args.replace(0, n + 1, "");
                Eval l1(left, false, ruleList, rule);
                left = l1.Evaluate();
                l.PushCallArg(left);
                n = args.find_first_of(',');
            }
            if (!args.empty())
            {
                Eval l1(args, false, ruleList, rule);
                args = l1.Evaluate();
                l.PushCallArg(args);
            }
            rv = l.Evaluate();
            callArgs = oldArgs;
        }
    }
    return rv;
}
std::string Eval::value(const std::string& arglist)
{
    std::string rv;
    Eval a(arglist, false, ruleList, rule);
    Variable* v = LookupVariable(a.Evaluate());
    if (v)
    {
        rv = v->GetValue();
    }
    return rv;
}

std::string Eval::eval(const std::string& arglist)
{
    Eval l(arglist, false, ruleList, rule);
    Parser p(l.Evaluate(), file, lineno, false);
    p.Parse();
    return "";
}

std::string Eval::origin(const std::string& arglist)
{
    std::string rv;
    Eval a(arglist, false, ruleList, rule);
    Variable* v = LookupVariable(a.Evaluate());
    if (v)
    {
        switch (v->GetOrigin())
        {
            case Variable::o_default:
                rv = "default";
                break;
            case Variable::o_environ:
                rv = "environment";
                break;
            case Variable::o_environ_override:
                rv = "environment override";
                break;
            case Variable::o_file:
                rv = "file";
                break;
            case Variable::o_command_line:
                rv = "command line";
                break;
            case Variable::o_override:
                rv = "override";
                break;
            case Variable::o_automatic:
                rv = "automatic";
                break;
        }
    }
    else
    {
        rv = "undefined";
    }
    return rv;
}

std::string Eval::flavor(const std::string& arglist)
{
    std::string rv;
    Eval a(arglist, false, ruleList, rule);
    Variable* v = LookupVariable(a.Evaluate());
    if (v)
        switch (v->GetFlavor())
        {
            case Variable::f_recursive:
                rv = "recursive";
                break;
            case Variable::f_simple:
                rv = "simple";
                break;
            case Variable::f_undefined:
                rv = "undefined";
                break;
        }
    else
        rv = "undefined";
    return rv;
}

std::string Eval::shell(const std::string& arglist)
{
    Eval a(arglist, false, ruleList, rule);
    EnvironmentStrings empty;
    Spawner sp(empty, true, true, false, false, false, false);
    return sp.shell(a.Evaluate());
}

std::string Eval::error(const std::string& arglist, const std::string fileOverride, int lineOverride)
{
    std::ostringstream os;
    if (!fileOverride.empty())
    {
        os << "Error " << fileOverride << "(" << lineOverride << "): " << arglist << std::endl;
    }
    else if (lineOverride != -1)
    {
        os << "Error " << file << "(" << lineno << "): " << arglist << std::endl;
    }
    else
    {
        os << "Error: " << arglist << std::endl;
    }
    OS::WriteToConsole(os.str());
    errcount++;
    return "";
}
std::string Eval::errorx(const std::string& arglist)
{
    Eval a(arglist, false, nullptr, nullptr);
    std::ostringstream os;
    std::cout << "Error " << file << "(" << lineno << "): " << a.Evaluate() << std::endl;
    OS::WriteToConsole(os.str());
    errcount++;
    return "";
}
std::string Eval::warning(const std::string& arglist, const std::string fileOverride, int lineOverride)
{
    std::ostringstream os;
    if (!fileOverride.empty())
    {
        os << "Warning " << fileOverride << "(" << lineOverride << "): " << arglist << std::endl;
    }
    else if (lineOverride != -1)
    {
        os << "Warning " << file << "(" << lineno << "): " << arglist << std::endl;
    }
    else
    {
        os << "Warning: " << arglist << std::endl;
    }
    OS::WriteToConsole(os.str());
    return "";
}
std::string Eval::warningx(const std::string& arglist)
{
    Eval a(arglist, false, nullptr, nullptr);
    std::ostringstream os;
    os << "Warning " << file << "(" << lineno << "): " << a.Evaluate() << std::endl;
    OS::WriteToConsole(os.str());
    return "";
}

std::string Eval::info(const std::string& arglist)
{
    Eval a(arglist, false, ruleList, rule);
    std::ostringstream os;
    os << a.Evaluate() << std::endl;
    OS::WriteToConsole(os.str());
    return "";
}
std::string Eval::exists(const std::string& arglist)
{
    Eval a(arglist, false, ruleList, rule);
    std::string fileName = a.Evaluate();
    std::fstream aa(fileName, std::ios::in);
    if (aa.is_open())
        return "1";
    return "0";
}
