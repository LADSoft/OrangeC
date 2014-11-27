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
#include "Eval.h"
#include "Rule.h"
#include "CmdFiles.h"
#include "Variable.h"
#include "Parser.h"
#include "Spawner.h"
#include "OS.h"
#include "Maker.h"
#include <ctype.h>
#include <strstream>
#include <iostream>
#include <fstream>
std::map<const std::string, Eval::StringFunc> Eval::builtins;
std::string Eval::VPath;
std::map<std::string, std::string> Eval::vpaths;
bool Eval::internalWarnings;
int Eval::lineno;
std::string Eval::file;
std::deque<RuleList *> Eval::ruleStack;
std::deque<Variable *> Eval::foreachVars;
std::set<std::string> Eval::macroset;
std::string Eval::GPath;
int Eval::errcount;

Eval::Eval(const std::string name, bool ExpandWildcards, RuleList *RuleList, Rule *Rule)
    : str(name), expandWildcards(ExpandWildcards), ruleList(RuleList), rule(Rule)
{
}
void Eval::Clear()
{
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

std::string Eval::GetVPATH(const std::string &goal)
{
    Variable *v = VariableContainer::Instance()->Lookup("VPATH");
    std::string rv;
    if (v)
    {
        if (v->GetFlavor() == Variable::f_recursive)
        {
            Eval r(v->GetValue(), false, NULL, NULL);
            rv = r.Evaluate();
        }
        else
        {
            rv = v->GetValue();
        }
    }		
    if (goal.size())
    {
        for (std::map<std::string, std::string>::iterator it = vpaths.begin(); it != vpaths.end(); ++it)
        {
            size_t start;
            size_t len;
            len = MatchesPattern(goal, it->first, start, 0);
            if (len == goal.size())
            {
                if (rv.size())
                    rv += " ";
                rv += it->second;
            }
        }
    }
    return rv;
}
void Eval::RemoveVPath(const std::string &path)
{
    std::map<std::string, std::string>::iterator it = vpaths.find(path);
    if (it != vpaths.end())
        vpaths.erase(it);
}
std::string Eval::ExtractFirst(std::string &value, const std::string &seps)
{
    StripLeadingSpaces(value);
    int n = value.size();
    for (int i =0; i < seps.size(); i++)
    {
        int m = value.find_first_of(seps[i]);
        if (m != std::string::npos && m < n)
            n = m;
    }
    std::string rv = value.substr(0, n);
    if (value.find_first_not_of(' ') == std::string::npos)
        value.replace(0,value.size(),"");
    else
        value.replace(0,n+1,"");
        
    return rv;
}
void Eval::StripLeadingSpaces(std::string &value)
{
    int n = value.find_first_not_of(' ');
    if (n != 0)
        value.replace(0, n, "");
}
int Eval::MacroSpan(const std::string line, int pos)
{
    std::deque<char> stack;
    int pos1 = pos;
    while (line[pos1] == '$')
        pos1++;
    if (line[pos1] != '(' && line[pos1] != '{')
    {
        if (line[pos1+1] == 'F' || line[pos1+1] == 'D')
            pos1++;
        return 1 + pos1 - pos;
    }
    
    for (;pos1 < line.size(); pos1++)
    {
        if (line[pos1] == '(')
            stack.push_front(')');
        else if (line[pos1] == '{')
            stack.push_front('}');
        else if (line[pos1] == stack.front())
        {
            stack.pop_front();
            if (stack.size() == 0)
                return pos1 - pos + 1;
        }
    }
    return std::string::npos;
}
std::string Eval::ParseMacroLine(const std::string &in)
{
    std::string rv;
    int n = 0;
    int m = in.find_first_of('$');
    while (m != std::string::npos)
    {
        rv += in.substr(n, m-n);
        if (m != in.size()-1 && in[m+1] == '$')
        {
            n = m + 1;
            m = MacroSpan(in, n);
            if (m == std::string::npos)
                m = 1;
            m = in.find_first_of('$', n + m);
        }
        else
        {
            n = MacroSpan(in, m + 1);
            if (n == 1 || n == 2)
            {
                rv += ExpandMacro(in.substr(m+1, n));
                n = m += 1 + n;
            }
            else if (n != std::string::npos)
            {
                rv += ExpandMacro(in.substr(m+2, n-2));
                n = m += n + 1;
            }
            m = in.find_first_of('$', n);
            
        }
    }
    if (n != std::string::npos)
        rv += in.substr(n, in.size());
    return rv;
}
Variable *Eval::LookupVariable(const std::string &name)
{
    Variable *v = NULL;
    for (std::deque<Variable *>::iterator it = foreachVars.begin(); it != foreachVars.end() && v == NULL; ++it)
    {
        if ((*it)->GetName() == name)
            v = (*it);
    }
    if (!v)
    {
        for (std::deque<RuleList *>::iterator it = ruleStack.begin(); it != ruleStack.end() && v == NULL; ++it)
            
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
bool Eval::AutomaticVar(const std::string &name, std::string &rv)
{
    bool found = false;
    if (ruleList && name.size () <= 2)
    {
        std::string extra;
        if (name[0] == '@') // target file name
        {
            rv = Maker::GetFullName(ruleList->GetTarget());
            found = true;
        }
        else if (name[0] == '%') // empty
        {
            rv = "";
            found = false;
        }
        else if (name[0] == '<') // first prereq of first rule
        {
            RuleList::iterator it = ruleList->begin();
            if (!rule || *it != rule)
            {
                extra = (*it)->GetPrerequisites();
                rv = ExtractFirst(extra, " ");
                rv = Maker::GetFullName(rv);
            }
            found = true;
        }
        else if (name[0] == '^') // all prereq or prereq of rules that have appeared
        {
            std::set<std::string> set;
            for (RuleList::iterator it = ruleList->begin(); it != ruleList->end(); ++it)
            {
                if ((*it) == rule)
                    break;
                extra = (*it)->GetPrerequisites();
                while (extra.size())
                {
                    std::string temp = ExtractFirst(extra, " ");
                    if (set.find(temp) == set.end())
                    {
                        set.insert(temp);
                        if (rv.size() != 0)
                            rv += " ";
                        rv += Maker::GetFullName(temp);
                    }
                }
            }
            found = true;
        }
        else if (name[0] == '+') // same with repetition
        {			
            for (RuleList::iterator it = ruleList->begin(); it != ruleList->end(); ++it)
            {
                if ((*it) == rule)
                    break;
                extra = (*it)->GetPrerequisites();
                while (extra.size())
                {
                    std::string temp = ExtractFirst(extra, " ");
                    if (rv.size() != 0)
                        rv += " ";
                    rv += Maker::GetFullName(temp);
                }
            }
            found = true;
        }
        else if (name[0] == '*') // implicit rule stem/target name - recognized suffix or target stem
        {
            rv = ruleList->GetTargetPatternStem();
            if (!rv.size())
            {
                rv = ruleList->GetTarget();
                size_t n = rv.find_last_of('.');
                if (n != std::string::npos)
                {
                    RuleList *rl = RuleContainer::Instance()->Lookup(".SUFFIXES");
                    if (rl)
                    {
                        std::string sfx = rv.substr(n);
                        bool found = false;
                        for (RuleList::iterator it = rl->begin(); !found && it != rl->end(); ++it)
                        {
                            std::string working = (*it)->GetPrerequisites();
                            while (working.size())
                            {
                                extra = ExtractFirst(working, " ");
                                if (extra == sfx)
                                {
                                    found = true;
                                    rv.replace(n, rv.size()-n, "");
                                }
                            }							
                        }
                    }
                }
            }
            found = true;
        }
        else if (name[0] == '?') // or names of prereqs newer than target
        {
            extra = ruleList->GetNewerPrerequisites();
            while (extra.size())
            {
                if (rv.size())
                    rv += " ";
                rv += Maker::GetFullName(ExtractFirst(extra, " "));
            }
            found = true;
        }
        else if (name[0] == '|') // or names of order-only prerequisites
        {
            std::set<std::string> set;
            for (RuleList::iterator it = ruleList->begin(); it != ruleList->end(); ++it)
            {
                if ((*it) == rule)
                    break;
                extra = (*it)->GetOrderPrerequisites();
                while (extra.size())
                {
                    std::string temp = ExtractFirst(extra, " ");
                    if (set.find(temp) == set.end())
                    {
                        set.insert(temp);
                        if (rv.size() != 0)
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
                while (extra.size())
                {
                    std::string temp = ExtractFirst(extra, " ");
                    size_t n = temp.find_last_of(CmdFiles::DIR_SEP);
                    if (rv.size())
                        rv += " ";
                    if (n != std::string::npos)
                    {
                        rv += temp.substr(0, n+1);
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
                while (extra.size())
                {
                    std::string temp = ExtractFirst(extra, " ");
                    size_t n = temp.find_last_of(CmdFiles::DIR_SEP);
                    if (rv.size())
                        rv += " ";
                    if (n != std::string::npos)
                    {
                        rv += temp.substr(n+1);
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
std::string Eval::ExpandMacro(const std::string &name)
{
    std::string rv;
    std::string extra;
    if (AutomaticVar(name, rv))
        return rv;
    if (name == ".VARIABLES")
    {
        for(VariableContainer::iterator it = VariableContainer::Instance()->begin();
                    it != VariableContainer::Instance()->end(); ++ it)
        {
            if (rv.size())
                rv += " ";
            rv += *(it->first);
        }	
    }
    else if (name[0] == '$')
    {
        size_t n = MacroSpan(name, 1);
        rv = name.substr(0, n+1);
        extra = std::string(name.substr(n+1));
        Eval a(rv, false, ruleList, rule);
        rv = a.Evaluate();
        Eval a1(rv, false, ruleList, rule);
        rv = a1.Evaluate();
    }
    else
    {
        std::string temp = name;
        std::string fw = ExtractFirst(temp, " ");
        std::map<const std::string, StringFunc>::iterator it = builtins.find(fw);
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
            if (index < args.size())
                rv = args[index];
        }
        else
        {
join:
            int m = name.find_first_of(':');
            if (m != std::string::npos)
            {
                extra = name.substr(m);
                rv = name.substr(0, m);
            }
            else
            {
                rv = name;
            }
            bool found = false;
            Variable *v = LookupVariable(rv);
            if (v)
            {
                rv = v->GetValue();
                if (v->GetFlavor() == Variable::f_recursive)
                {
                    if (macroset.find(v->GetName()) == macroset.end())
                    {
                        std::pair<std::set<std::string>::iterator, bool> p = macroset.insert(v->GetName());
                        rv = ParseMacroLine(rv);
                        macroset.erase(p.first);
                    }
                }
            }
            else {
                rv = "";
                if (internalWarnings)
                {
                    warning("'" + rv + "' is undefined.");
                }
            }
        }
    }
    if (extra.size())
    {
        int m = extra.find_first_not_of(' ');
        if (extra[m] != ':')
        {
            error("Invalid macro qualifier");
        }
        else
        {
            m++;
            int m1 = extra.find_first_of('=');
            if (m1 != std::string::npos)
            {
                std::string pat = extra.substr(m, m1 -m);
                std::string rep = extra.substr(m1 + 1);
                int n = pat.find_first_not_of(' ');
                if (pat[n] == '.')
                    pat.replace(0,n,"%");
                else if (n)
                    pat.replace(0,n,"");
                n = rep.find_first_not_of(' ');
                if (rep[n] == '.')
                    rep.replace(0,n,"%");
                else
                    rep.replace(0,n,"");
                rv = patsubst(pat + "," +  rep + "," + rv);					
            }
            else
            {
                error("Invalid macro qualifier");
            }
        }
    }
    return rv;
}
size_t Eval::FindPercent(const std::string &name, size_t pos)
{
    while (true)
    {
        pos = name.find_first_of('%', pos);
        if (pos == std::string::npos || pos == 0 || name[pos-1] != '\\' || pos > 1 && name[pos-2] == '\\') //  Priority of the '&&' operation is higher than that of the '||' operation
            return pos;
        pos++;
    }
}
std::string Eval::FindStem(const std::string &name, const std::string &pattern)
{
    int n = FindPercent(pattern);
    if (n != std::string::npos && name.size())
    {
        int m1 = pattern.find_first_not_of(' ');
        int m2 = pattern.find_last_not_of(' ');
        int n1 = name.find_first_not_of(' ');
        int n2 = name.find_last_not_of(' ');
        return name.substr(n1 - m1 + n, n2 - m2 + 1 - (n1 - m1));
    }
    return "";
}
std::string Eval::ReplaceQuotes(const std::string &value)
{
    int m = 0;
    std::string rv;
    while (m != std::string::npos)
    {
        int n = value.find_first_of(m);
        if (n != std::string::npos)
        {
            if (n > 0 && value[n-1] == '\\')
                if (n > 1 && value[n-2] == '\\')
                {
                    rv += value.substr(m, n-2);
                    m = n - 1;
                }
                else
                {
                    rv += value.substr(m, n-1);
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
std::string Eval::ReplaceStem(const std::string &stem, const std::string &pattern)
{
    int n = FindPercent(pattern);
    if (n != std::string::npos)
    {
        std::string rv = ReplaceQuotes(pattern.substr(0,n)) + stem + ReplaceQuotes(pattern.substr(n+1));
        return rv;
    }
    return pattern;
    
}
size_t Eval::MatchesPattern(const std::string &name, const std::string &pattern, size_t &start, size_t begin)
{
    size_t rv = std::string::npos;
    int m = FindPercent(pattern, begin);
    if (m != std::string::npos)
    {
        int m1 = m;
        int m2 = m;
        while (m1 && pattern[m1-1] != ' ')
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
                    if (name.substr(n2 - (m2 - m - 1), m2 - m - 1) == pattern.substr(m+1, m2 - m - 1))
                    {
                        start = m;
                        rv = n2;
                        break;
                    }
            }
            n1 = name.find_first_not_of(' ', n2);
        }
    }
    else
        if (name == pattern)
        {
            rv = name.size();
            start = 0;
        }
    return rv;
}
int Eval::FindBalancedComma(const std::string &right)
{
    int nest = 0;
    int n;
    for (n=0; n < right.size(); ++n)
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
bool Eval::TwoArgs(const std::string &line, std::string &left, std::string &right)
{
    bool rv = false;
    int n = FindBalancedComma(line);
    if (n != std::string::npos)
    {
        left = line.substr(0, n);
        right = line.substr(n+1);
        n = FindBalancedComma(right);
        if (n == std::string::npos)
            rv = true;
    }
    if (!rv)
        error("Expected two arguments");
    return rv;
}
bool Eval::ThreeArgs(const std::string &line, std::string &one, std::string &two, std::string &three)
{
    bool rv = false;
    int	n = FindBalancedComma(line);
    if (n != std::string::npos)
    {
        one = line.substr(0, n);
        two = line.substr(n+1);
        n = FindBalancedComma(two);
        if (n != std::string::npos)
        {
            three = two.substr( n+ 1);
            two.replace(n, two.size()-n, "");
            n = FindBalancedComma(three);
            if (n == std::string::npos)
                rv = true;
        }
    }
    if (!rv)
        error("Expected three arguments");
    return rv;
}
int Eval::GetNumber(const std::string &line)
{
    int rv = 0;
    int i;
    size_t n = line.find_first_not_of(' ');
    size_t m = line.find_last_not_of(' ');
    if (m == std::string::npos)
        m = line.size();
    else
        m++;
    for (i=n; i < m; i++)
        if (!isdigit(line[i]))
            break;
    if (i != m)
        error("Numeric value expected");
    else
    {
        std::strstream s;
        s << line;
        s >> rv;
    }
    return rv;
}
std::string Eval::subst(const std::string &arglist)
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
std::string Eval::patsubst(const std::string &arglist)
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
        size_t n =0;
        std::string rv;
        while (text.size())
        {
            std::string thisOne = ExtractFirst(text, " ");
            if (MatchesPattern(thisOne, pattern, start, 0) != std::string::npos)
            {
                std::string stem = FindStem(thisOne, pattern);
                thisOne = ReplaceStem(stem, replacement);
            }
            if (rv.size())
                rv += " ";
            rv += thisOne;
        }
        return rv;
    }
    return text;
}
std::string Eval::strip(const std::string &arglist)
{
    std::string rv;
    Eval e(arglist, false, ruleList, rule);
    std::string a = e.Evaluate();
    size_t m = a.find_first_not_of(' ');
    size_t n = a.find_first_of(' ', m);
    while (n != std::string::npos)
    {
        rv += a.substr(m, n-m);
        n = a.find_first_not_of(' ', n);
        m = n;
        if (n != std::string::npos)
            rv += " ";
        n = a.find_first_of(' ',n);
    }
    if (m != std::string::npos)
        rv += a.substr(m);
    return rv;
}
std::string Eval::findstring(const std::string &arglist)
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
std::string Eval::filter(const std::string &arglist)
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
        while (text.size())
        {
            std::string working = pattern;
            std::string fw = ExtractFirst(text, " ");
            while (working.size())
            {
                std::string p = ExtractFirst(working, " ");
                size_t xx = 0;
                if (MatchesPattern(fw, p,xx) == fw.size())
                {
                    if (rv.size())
                        rv += " ";
                    rv += fw;
                }
            }
        }
    }
    return rv;
}
std::string Eval::filterout(const std::string &arglist)
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
        while (text.size())
        {
            std::string working = pattern;
            std::string fw = ExtractFirst(text, " ");
            bool notfound = true;
            while (working.size())
            {
                std::string p = ExtractFirst(working, " ");
                size_t xx = 0;
                notfound &= (MatchesPattern(fw, p, xx) != fw.size());
            }
            if (notfound)
            {
                if (rv.size())
                    rv += " ";
                rv += fw;
            }
        }
    }
    return rv;
}
std::string Eval::sort(const std::string &arglist)
{
    std::set<std::string> sortList;
    Eval a(arglist, false, ruleList, rule);
    std::string working = a.Evaluate();
    while (working.size())
    {
        sortList.insert(ExtractFirst(working, " "));
    }
    std::string rv;
    for (std::set<std::string>::iterator it = sortList.begin(); it != sortList.end(); ++it)
    {
        if (rv.size())
            rv += " ";
        rv += *it;
    }
    return rv;
}
std::string Eval::word(const std::string &arglist)
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
        for (int i=0; i < n; i++)
        {
            if (text.size() == 0)
            {
                rv = "";
                break;
            }
            rv = ExtractFirst(text, " ");
        }
    }
    return rv;
}
std::string Eval::wordlist(const std::string &arglist)
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
        for (int i=1; i < sn; i++)
        {
            if (text.size() == 0)
            {
                break;
            }
            ExtractFirst(text, " ");
        }
        for (int i=sn; i <= en; i++)
        {
            if (text.size() == 0)
            {
                break;
            }
            if (rv.size())
                rv += " ";
            rv += ExtractFirst(text, " ");
        }
    }
    return rv;
}
std::string Eval::words(const std::string &arglist)
{
    std::string text;
    Eval t(arglist, false, ruleList, rule);
    text = t.Evaluate();
    size_t n = 0;
    size_t count = 0;
    n = text.find_first_not_of(' ');
    if (n != std::string::npos)
    {
        while (text.size())
        {
            count++;
            ExtractFirst(text, " ");
        }	
    }
    std::strstream s;
    s << count;
    std::string rv;
    s >> rv;
    return rv;
}
std::string Eval::firstword(const std::string &arglist)
{
    std::string text;
    Eval t(arglist, false, ruleList, rule);
    text = t.Evaluate();
    return ExtractFirst(text, " ");
}

std::string Eval::lastword(const std::string &arglist)
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
        rv = text.substr(m, n+1);
    }
    return rv;
}

std::string Eval::dir(const std::string &names)
{
    std::string working = names;
    Eval w(working, false, ruleList, rule);
    working = w.Evaluate();
    std::string rv;
    while (working.size())
    {
        std::string p = ExtractFirst(working, " ");
        size_t n = p.find_last_of(CmdFiles::DIR_SEP);
        if (rv.size())
            rv += " ";
        if (n != std::string::npos)
            rv += p.substr(0,n+1);
        else
            rv += std::string(".") + CmdFiles::DIR_SEP;
    }
    return rv;
}

std::string Eval::notdir(const std::string &names)
{
    std::string working = names;
    Eval w(working, false, ruleList, rule);
    working = w.Evaluate();
    std::string rv;
    while (working.size())
    {
        std::string p = ExtractFirst(working, " ");
        size_t n = p.find_last_of(CmdFiles::DIR_SEP);
        std::string intermed;
        if (n != std::string::npos)
            intermed = p.substr(n+1);
        else
            intermed = p;
        if (rv.size() && intermed.size())
            rv += " ";
        rv += intermed;
    }
    return rv;
}

std::string Eval::suffix(const std::string &names)
{
    std::string working = names;
    Eval w(working, false, ruleList, rule);
    working = w.Evaluate();
    std::string rv;
    while (working.size())
    {
        std::string p = ExtractFirst(working, " ");
        size_t n = p.find_last_of('.');
        if (n != std::string::npos && (n == p.size()-1 || p[n+1] != '\\'))
        {
            if (rv.size())
                rv += " ";
            rv += p.substr(n);
        }
    }
    return rv;
}

std::string Eval::basename(const std::string &names)
{
    std::string working = names;
    Eval w(working, false, ruleList, rule);
    working = w.Evaluate();
    std::string rv;
    while (working.size())
    {
        std::string p = ExtractFirst(working, " ");
        size_t n = p.find_last_of('.');
        if (rv.size())
            rv += " ";
        if (n != std::string::npos && (n == p.size()-1 || p[n+1] != '\\'))
        {
            rv += p.substr(0,n);
        }
        else
        {
            rv += p;
        }
    }
    return rv;
}

std::string Eval::addsuffix(const std::string &arglist)
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
            while (names.size())
            {
                if (rv.size())
                    rv += " ";
                rv += ExtractFirst(names, " ");
                rv += suffix;
            }
        }
    }
    return rv;
}

std::string Eval::addprefix(const std::string &arglist)
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
            while (names.size())
            {
                if (rv.size())
                    rv += " ";
                rv += prefix;
                rv += ExtractFirst(names, " ");
            }
        }
    }
    return rv;
}

std::string Eval::wildcard(const std::string &arglist)
{
    CmdFiles files;
    std::string names = strip(arglist);
    return wildcardinternal(names);
}
std::string Eval::wildcardinternal(std::string &names)
{
    CmdFiles files;
    while (names.size())
        files.Add(ExtractFirst(names, " "));
    std::string rv;
    for (CmdFiles::FileNameIterator it = files.FileNameBegin(); it != files.FileNameEnd(); ++it)
    {
        if (rv.size())
            rv += " ";
        rv += *(*it);
    }
    return rv;
}

std::string Eval::join(const std::string &arglist)
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
        while (listone.size() || listtwo.size())
        {
            if (rv.size())
                rv += " ";
            rv += ExtractFirst(listone, " ");
            rv += ExtractFirst(listtwo, " ");
        }
    }
    // fixme - paths
    return rv;
}

std::string Eval::realpath(const std::string &arglist)
{
    Eval t(arglist, false);
    std::string text = t.Evaluate();
    std::string rv;
    while (text.size())
    {
        std::string thisOne = ExtractFirst(text, " ");
        if (thisOne[0] != CmdFiles::DIR_SEP[0] && thisOne[1] != ':') // windows specific
            thisOne = OS::GetWorkingDir() + CmdFiles::DIR_SEP + thisOne;
        if (rv.size())
            rv += " ";
        rv += thisOne;
    }
    return rv;
}

std::string Eval::abspath(const std::string &arglist)
{
    return realpath(arglist);
}

std::string Eval::condIf(const std::string &arglist)
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
        std::string then = arglist.substr(n+1);
        std::string els;
        n = then.find_first_of(',');
        if (n != std::string::npos)
        {
            els = then.substr( n + 1);
            then.replace(n, then.size()-n, "");
            if (els.find_first_of(',') != std::string::npos)
                error("Error in 'if' expression");
        }
        n = ifst.find_first_not_of(' ');
        if (n == std::string::npos)
            ifst = "";
        else 
        {
            if (n)
                ifst.replace(0,n,"");
            n = ifst.find_last_not_of(' ');
            ifst.replace(n+1,ifst.size()-n-1,"");
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
            if (els.size())
            {
                Eval e(els, false, ruleList, rule);
                rv = e.Evaluate();
            }
        }
    }
    return rv;
}

std::string Eval::condOr(const std::string &arglist)
{
    std::string left;
    std::string right = arglist;
    size_t n = right.find_first_of(',');
    while (n != std::string::npos)
    {
        left = right.substr(0,n);
        right.replace(0, n+1,"");
        Eval l(left,false, ruleList, rule);
        left = l.Evaluate();
        if (left.size())
            return left;
        n = arglist.find_first_of(',');
    }
    Eval r(right, false, ruleList, rule);
    return r.Evaluate();
}

std::string Eval::condAnd(const std::string &arglist)
{
    std::string left;
    std::string right = arglist;
    size_t n = right.find_first_of(',');
    while (n != std::string::npos)
    {
        left = right.substr(0,n);
        right.replace(0, n+1, "");
        Eval l(left,false, ruleList, rule);
        left = l.Evaluate();
        if (!left.size())
            return left;
        n = right.find_first_of(',');
    }
    Eval r(right, false, ruleList, rule);
    return r.Evaluate();
}

std::string Eval::foreach(const std::string &arglist)
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
            Variable  * v = new Variable(var, list, Variable::f_simple, Variable::o_file);
            foreachVars.push_front(v);
            while (list.size())
            {
                std::string value = ExtractFirst(list, " ");
                v->SetValue(value);
                Eval t(next , false, ruleList, rule);
                if (rv.size())
                    rv += " ";
                rv += t.Evaluate();
            }
            foreachVars.pop_front();
            delete v;
        }
    }
    return rv;
}

std::string Eval::call(const std::string &arglist)
{
    std::string sub = arglist;
    std::string args;
    std::string rv;
    size_t n = sub.find_first_of(',');
    if (n == std::string::npos)
    {
        std::map<const std::string, StringFunc>::iterator it = builtins.find(sub);
        if (it != builtins.end())
        {
            rv = (this->*(it->second))("");
        }
        else
        {
            Eval l(sub, false, ruleList, rule);
            rv = l.Evaluate();
        }
    }
    else
    {
        sub = sub.substr(0, n);
        args = arglist.substr(n+1);
        std::map<const std::string, StringFunc>::iterator it = builtins.find(sub);
        if (it != builtins.end())
        {
            rv = (this->*(it->second))(args);
        }
        else
        {
            Eval l(sub, false, ruleList, rule);
            l.PushCallArg(sub);
            n = args.find_first_of(',');
            while (n != std::string::npos)
            {
                std::string left = args.substr(0, n);
                args.replace(0, n+1,"");
                Eval l1(left, false, ruleList, rule);
                left = l1.Evaluate();
                l.PushCallArg(left);
                n = args.find_first_of(',');
            }
            if (args.size())
                l.PushCallArg(args);
            rv = l.Evaluate();
        }
    }
    return rv;
}
std::string Eval::value(const std::string &arglist)
{
    std::string rv;
    Eval a(arglist, false, ruleList, rule);
    Variable *v = LookupVariable(a.Evaluate());
    if (v)
    {
        rv = v->GetValue();
    }
    return rv;
}

std::string Eval::eval(const std::string &arglist)
{
    Eval l(arglist, false, ruleList, rule);
    Parser p(l.Evaluate(), file, lineno, false);
    p.Parse();
    return "";
}

std::string Eval::origin(const std::string &arglist)
{
    std::string rv;
    Eval a(arglist, false, ruleList, rule);
    Variable *v = LookupVariable(a.Evaluate());
    if (v)
    {
        switch(v->GetOrigin())
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

std::string Eval::flavor(const std::string &arglist)
{
    std::string rv;
    Eval a(arglist, false, ruleList, rule);
    Variable *v = LookupVariable(a.Evaluate());
    if (v)
        switch (v->GetFlavor())
        {
            case Variable::f_recursive:
                rv = "recursive";
                break;
            case Variable::f_simple:
                rv = "simple";
                break;
        }
    else
        rv = "undefined";
    return rv;
}

std::string Eval::shell(const std::string &arglist)
{
    Eval a(arglist, false, ruleList, rule);
    EnvironmentStrings empty;
    Spawner sp (empty, true, true, false);
    return sp.shell(a.Evaluate());
}

std::string Eval::error(const std::string &arglist)
{
    std::cout << "Error " << file << "(" << lineno << "): " << arglist << std::endl;
    errcount++;
    return "";
}
std::string Eval::errorx(const std::string &arglist)
{
    Eval a(arglist, false, NULL, NULL);
    std::cout << "Error " << file << "(" << lineno << "): " << a.Evaluate() << std::endl;
    errcount++;
    return "";
}
std::string Eval::warning(const std::string &arglist)
{
    std::cout << "Warning " << file << "(" << lineno << "): " << arglist << std::endl;
    return "";
}
std::string Eval::warningx(const std::string &arglist)
{
    Eval a(arglist, false, NULL, NULL);
    std::cout << "Warning " << file << "(" << lineno << "): " << a.Evaluate() << std::endl;
    return "";
}

std::string Eval::info(const std::string &arglist)
{
    Eval a(arglist, false, ruleList, rule);
    std::cout << a.Evaluate() << std::endl;
    return "";
}
std::string Eval::exists(const std::string &arglist)
{
    Eval a(arglist, false, ruleList, rule);
    std::string fileName = a.Evaluate();
    std::fstream aa(fileName.c_str(), std::ios::in);
    if (aa.is_open())
        return "1";
    else
        return "0";
}
void Eval::Init()
{
    builtins["subst"] = &Eval::subst;
    builtins["patsubst"] = &Eval::patsubst;
    builtins["strip"] = &Eval::strip;
    builtins["findstring"] = &Eval::findstring;
    builtins["filter"] = &Eval::filter;
    builtins["filter-out"] = &Eval::filterout;
    builtins["sort"] = &Eval::sort;
    builtins["word"] = &Eval::word;
    builtins["wordlist"] = &Eval::wordlist;
    builtins["words"] = &Eval::words;
    builtins["firstword"] = &Eval::firstword;
    builtins["lastword"] = &Eval::lastword;
    builtins["dir"] = &Eval::dir;
    builtins["notdir"] = &Eval::notdir;
    builtins["suffix"] = &Eval::suffix;
    builtins["basename"] = &Eval::basename;
    builtins["addsuffix"] = &Eval::addsuffix;
    builtins["addprefix"] = &Eval::addprefix;
    builtins["wildcard"] = &Eval::wildcard;
    builtins["join"] = &Eval::join;
    builtins["realpath"] = &Eval::realpath;
    builtins["abspath"] = &Eval::abspath;
    builtins["if"] = &Eval::condIf;
    builtins["or"] = &Eval::condOr;
    builtins["and"] = &Eval::condAnd;
    builtins["foreach"] = &Eval::foreach;
    builtins["call"] = &Eval::call;
    builtins["value"] = &Eval::value;
    builtins["eval"] = &Eval::eval;
    builtins["origin"] = &Eval::origin;
    builtins["flavor"] = &Eval::flavor;
    builtins["shell"] = &Eval::shell;
    builtins["error"] = &Eval::errorx;
    builtins["warning"] = &Eval::warningx;
    builtins["info"] = &Eval::info;
    builtins["exists"] = &Eval::exists;
}

