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
#include "Parser.h"
#include "Rule.h"
#include "Variable.h"
#include "Eval.h"
#include "Spawner.h"
#include "Include.h"
#include "Maker.h"
#include "UTF8.h"
#include <ctype.h>
#include <fstream>
#include <iostream>
Parser::Parser(const std::string &string, const std::string &File, int Lineno, bool IncrementLineno, Variable::Origin oOrigin)
        : file(File), remaining(string), lineno(Lineno-1), incrementLineno(IncrementLineno), lastCommand(NULL),
               secondaryExpansionEnabled(false), origin(oOrigin), ignoreFirstGoal(false)
{
}
bool Parser::AlwaysEval(const std::string &line)
{
    size_t n;
    std::string firstWord = FirstWord(line, n);
    return firstWord == "ifeq" || firstWord == "ifneq" || firstWord == "else" || firstWord == "endif" ||
        firstWord == "ifdef" || firstWord == "ifndef";
}
bool Parser::Parse()
{
    bool rv = true;
    while (remaining.size())
    {
        std::string line = GetLine(false);
        if (!skips.size() || !skips.front() || AlwaysEval(line))
        {
            if (line.find_first_not_of(' ') != std::string::npos)
            {
                Eval::SetFile(file);
                Eval::SetLine(lineno);
                ParseLine(line);
            }
        }
    }
    if (skips.size())
    {
        Eval::SetFile(file);
        Eval::SetLine(lineno);
        Eval::error("File ended with conditional in progress");
        rv = false;
    }
    skips.clear();
    return rv;
}
void Parser::UnTab(std::string &value)
{
    size_t n = 0;
    while((n = value.find_first_of('\t', n+1)) != std::string::npos)
        value[n] = ' ';
}
std::string Parser::GetLine(bool inCommand)
{
    lineno++;
    std::string rv = Eval::ExtractFirst(remaining,"\n");
    if (rv[0] == '\t')
        inCommand = true;
        
    // concatenate lines
    while (rv[rv.size()-1] == '\\')
    {
        if (!remaining.size())
        {
            Eval::error("backslash-newline at end of input stream");
            break;
        }
        if (!inCommand)
        {
            rv.replace(rv.size()-1, 1,"");
            rv = rv + " ";
        }
        lineno++;
        rv += Eval::ExtractFirst(remaining, "\n");
    }
    // get rid of comments
    if (!inCommand)
    {
        rv = RemoveComment(rv);
    }
    UnTab(rv);
    return rv;
}
std::string Parser::RemoveComment(const std::string &line)
{
    int start = 0;
    int n;
    std::string rv = line;
    do
    {
        n = rv.find_first_of('#', start);
        if (n != std::string::npos)
        {
            if (!n || rv[n-1] != '\\')
            {
                rv.replace(n, rv.size()-n, "");
            }
            else
            {
                rv.replace(n,1,"");
                start = n+1;
            }
        }
    }
    while (n != std::string::npos);
    return rv;
}
void UnTab(std::string value)
{
    // get rid of tabs
    int start = 1; // first tab in line remains
    int n;
    do
    {
        n = value.find_first_of('\t', start);
        if (n != std::string::npos)
        {
            value[n] = ' ';
            start = n+1;
        }
    }
    while (n != std::string::npos);
}
size_t Parser::UnfetteredChar(const std::string &line, char ch) const
{
    bool charInWord = 0; //FIXME bool = 0 ?
    for (int i=0; i < line.size(); i++)
    {
        if (line[i] == ' ')
            charInWord = 0;
        else if (line[i] == '$')
        {
            i += Eval::MacroSpan(line, i+1);
            charInWord = 0;
        }
        else
        {
            charInWord++;
            if (ch == ':')
            {
                if (line[i] == ':' && (charInWord != 2 || !UTF8::IsAlpha(line.c_str() + i-1) || isspace(line[i+1])))
                    return i;
            }
            else
            {
                if (line[i] == ch)
                    return i;
            }
        }
    }
    return std::string::npos;
}
std::string Parser::FirstWord(const std::string &line, size_t &n)
{
    std::string rv;
    int s = line.find_first_not_of(' ');
    n = 0;
    if (s != std::string::npos)
    {
        int t = s;
        while (UTF8::IsAlnum(line.c_str() +t) || line[t] == '_' || line[t] == '-')
        {
            int v = UTF8::CharSpan(line.c_str() + t);
            for (int i = 0; i < v && t < line.size(); i++)
                t++;
        }
        rv = line.substr(s, t);
        n = t;
    }
    return rv;
}
bool Parser::ParseLine(const std::string &line)
{
    bool rv = false;
    if (line[0] == '\t')
    {
        rv = ParseCommand(line.substr(1));
    }
    else
    {
        size_t n;
        bool override = false;
        std::string firstWord = FirstWord(line, n);
        if (firstWord == "-include")
        {
            return ParseInclude(line.substr(n), true);
        }
        else if (firstWord == "include")
        {
            return ParseInclude(line.substr(n), false);
        }
        else if (firstWord == "ifeq")
        {
            return ParseCond(line.substr(n), true);
        }
        else if (firstWord == "ifneq")
        {
            return ParseCond(line.substr(n), false);
        }
        else if (firstWord == "ifdef")
        {
            return ParseDef(line.substr(n), true);
        }
        else if (firstWord == "ifndef")
        {
            return ParseDef(line.substr(n), false);
        }
        else if (firstWord == "else")
        {
            return ParseElse(line.substr(n));
        }
        else if (firstWord == "endif")
        {
            return ParseEndif(line.substr(n));
        }
        else if (firstWord == "export")
        {
            return ParseExport(line.substr(n), true);
        }
        else if (firstWord == "unexport")
        {
            return ParseExport(line.substr(n), false);
        }
        else if (firstWord == "define")
        {
            return ParseDefine(line.substr(n), false);
        }
        else if (firstWord == "vpath")
        {
            return Parsevpath(line.substr(n));
        }
        else if (firstWord == "endef")
        {
            Eval::error("endef without define");
            return false;
        }
        else if (firstWord == "override")
        {
            override = true;
            size_t x;
            firstWord = FirstWord(line.substr(n), x);
            if (firstWord == "define")
            {
                return ParseDefine(line.substr(x), true);
            }
        }
        else
        {
            n = 0;
        }
        std::string iline = line.substr(n);
        int eq = UnfetteredChar(iline,'=');
        int q = iline.find_first_of('?');
        int r = iline.find_first_of('+');
        int colon = UnfetteredChar(iline, ':');
        if (colon != std::string::npos && (eq == std::string::npos || colon < eq))
        {
            if (eq != std::string::npos && colon == eq - 1)
            {
                rv = ParseAssign(iline.substr(0, colon), iline.substr(eq + 1), override);
            }
            else
            {
                if (override)
                    Eval::warning("Expected variable assignment with override keyword");
                rv = ParseRule(iline.substr(0, colon), iline.substr(colon+1));
            }
        }
        else if (eq != std::string::npos)
        {
            if (q != std::string::npos && q == eq - 1)
            {
                rv = ParseQuestionAssign(iline.substr(0, q), iline.substr(eq + 1), override);
            }
            else if (r != std::string::npos && r == eq - 1)
            {
                rv = ParsePlusAssign(iline.substr(0,r), iline.substr(eq + 1), override);
            }
            else
            {
                rv = ParseRecursiveAssign(iline.substr(0, eq), iline.substr(eq + 1), override);
            }
        }
        else
        {
            Eval l(line, false);
            if (l.Evaluate().size())
            {
                if (override)
                    Eval::error("Expected variable assignment");
                else
                    Eval::error("Expected rule");
            }
        }
    }
    return rv;
}
bool Parser::ParseAssign(const std::string &left, const std::string &right, bool override, RuleList *ruleList)
{
    if (left == ".VARIABLES")
        return true;
    Eval l(left, false);
    std::string ls = l.Evaluate();
    ls = l.strip(ls);
    Eval r(right, false);
    std::string rs = r.Evaluate();
    Eval::StripLeadingSpaces(rs);
    Variable *v; 
    if (ruleList)
        v = ruleList->Lookup(ls);
    else
        v = VariableContainer::Instance()->Lookup(ls);
    if (v)
    {
        v->AssignValue(rs, origin, override);
    }
    else
    {
        v = new Variable(ls, rs, Variable::f_simple, ruleList ? Variable::o_automatic : origin);
        if (ruleList)
            *ruleList += v;
        else
            *VariableContainer::Instance() += v;
    }
    if (v && !ruleList && left == "MAKEFILES")
        v->SetExport(true);
    return true;	
}
bool Parser::ParseRecursiveAssign(const std::string &left, const std::string &right, bool override, RuleList *ruleList)
{
    if (left == ".VARIABLES")
        return true;
    Eval l(left, false);
    std::string ls = l.Evaluate();
    ls = l.strip(ls);
    std::string rs = right;
    Eval::StripLeadingSpaces(rs);
    Variable *v;
    if (ruleList)
        v = ruleList->Lookup(ls);
    else
        v = VariableContainer::Instance()->Lookup(ls);
    if (v)
    {
        v->AssignValue(rs, origin, override);
    }
    else
    {
        v = new Variable(ls, rs, Variable::f_recursive, ruleList ? Variable::o_automatic : origin);
        if (ruleList)
            *ruleList += v;
        else
            *VariableContainer::Instance() += v;
    }
    if (v && !ruleList && left == "MAKEFILES")
        v->SetExport(true);
    return true;
}
bool Parser::ParsePlusAssign(const std::string &left, const std::string &right, bool override, RuleList *ruleList)
{
    if (left == ".VARIABLES")
        return true;
    Eval l(left, false);
    std::string ls = l.Evaluate();
    ls = l.strip(ls);
    std::string rs = right;
    Eval::StripLeadingSpaces(rs);
    Variable *v;
    if (ruleList)
        v = ruleList->Lookup(ls);
    else
         v = VariableContainer::Instance()->Lookup(ls);
    if (v)
    {
        if (v->GetFlavor() == Variable::f_simple)
        {
            Eval r(rs, false);
            rs = r.Evaluate();
        }
        v->AppendValue(rs, override);
    }
    else
    {
        v = new Variable(ls, rs, Variable::f_recursive, ruleList ? Variable::o_automatic : origin);
        if (ruleList)
            *ruleList += v;
        else
            *VariableContainer::Instance() += v;
    }
    if (v && !ruleList && left == "MAKEFILES")
        v->SetExport(true);
    return true;
}
bool Parser::ParseQuestionAssign(const std::string &left, const std::string &right, bool override, RuleList *ruleList)
{
    if (left == ".VARIABLES")
        return true;
    Eval l(left, false);
    std::string ls = l.Evaluate();
    ls = l.strip(ls);
    Variable *v ;
    if (ruleList)
        v = ruleList->Lookup(ls);
    else
        v = VariableContainer::Instance()->Lookup(ls);
    if (!v)
    {
        std::string rs = right;
        Eval::StripLeadingSpaces(rs);
        v = new Variable(ls, rs, Variable::f_simple, ruleList ? Variable::o_automatic : origin);
        if (ruleList)
            *ruleList += v;
        else
            *VariableContainer::Instance() += v;
    }
    if (v && !ruleList && left == "MAKEFILES")
        v->SetExport(true);
    return true;
}
std::string Parser::ReplaceAllStems(const std::string &stem, const std::string value)
{
    bool done = false;
    int m = 0;
    int n = value.find('%');
    std::string rv;
    while (n != std::string::npos)
    {
        rv += value.substr(m, n - m);
        rv += stem;
        m = n + 1;
        n = value.find_first_of(' ', m);
        if (n == std::string::npos)
        {
            n = value.find_first_of('%', m);
        }
        else
        {
            n = value.find_first_of('%', n);
        }
    }
    rv += value.substr(m);
    return rv;
}
bool Parser::ParseRule(const std::string &left, const std::string &line)
{
    bool rv = true;
    std::string targetPattern;
    std::string prereqs;
    std::string orderPrereqs;
    std::string command;
    bool Double = false;
    std::string iline;
    bool hasCmd = false;
    if (line[0] == ':')
    {
        Double = true;
        iline = line.substr(1);
    }
    else
    {
        iline = line;
    }
    Eval l(left, true);
    std::string ls = l.Evaluate();
    if (ls.find(".SECONDEXPANSION") != std::string::npos)
    {
        secondaryExpansionEnabled = true;
        return true;
    }
    size_t n = UnfetteredChar(line,'=');
    size_t p = UnfetteredChar(line, ';');
    if (n != 0 && n != std::string::npos && (p == std::string::npos || n < p))
    {
        enum e_mode { asn, qasn, pasn, rasn } mode;
        if (line[n-1] == '?')
            mode=qasn;
        else if (line[n-1] == ':')
            mode=asn;
        else if (line[n-1] == '+')
            mode = pasn;
        else
            mode = rasn;
        std::string l, r;
        if (mode == rasn)
        {
            l = line.substr(0, n);
        }
        else
        {
            l = line.substr(0, n-1);
        }
        Eval l1(l, false);
        l = l1.strip(l);
        r = line.substr(n + 1);
        while (ls.size() && rv)
        {
            std::string cur = Eval::ExtractFirst(ls, std::string(" "));
            RuleList *ruleList = RuleContainer::Instance()->Lookup(cur);		
            if (!ruleList)
            {
                ruleList = new RuleList(cur);
                *RuleContainer::Instance() += ruleList;
            }
            switch(mode)
            {
                case rasn:
                    rv &= ParseRecursiveAssign(l, r, false, ruleList);
                    break;
                case pasn:
                    rv &= ParsePlusAssign(l, r, false, ruleList);
                    break;
                case qasn:
                    rv &= ParseQuestionAssign(l, r, false, ruleList);
                    break;
                case asn:
                    rv &= ParseAssign(l, r, false, ruleList);
                    break;
            }
        }
    }
    else
    {
        size_t m = ls.find_first_not_of(' ');
        if (ls[m] == '.')
        {
            RuleList *ruleList = RuleContainer::Instance()->Lookup(".SUFFIXES");
            bool found1 = false, found2 = false;
            n = ls.find_first_of('.', m+1);
            std::string one;
            std::string two;
            if (n == std::string::npos)
            {
                n = ls.find_first_of(' ', m);
                one = ls.substr(m,n );
                found2 = true;
            }
            else
            {
                one = ls.substr(m,n );
                m = n;
                n = ls.find_first_of(' ', m);
                two = ls.substr(m, n);
            }
            if (ruleList)
            {
                for(RuleList::iterator it = ruleList->begin(); it != ruleList->end() && (!found1 || !found2); ++it)
                {
                    std::string working = (*it)->GetPrerequisites();
                    while (working.size())
                    {
                        std::string a = Eval::ExtractFirst(working, " ");
                        if (a == one)
                            found1 = true;
                        if (a == two)
                            found2 = true;
                    }
                }
            }
            if (!found1 || !found2)
                goto join;
            // suffix rule, should not have any characters other than a possible double colon
            // in line
            if (line.size() && line[0] != ':')
            {
                m = line.find_first_not_of(' ');
                if (m != std::string::npos && line[m] != ';')
                    Eval::error("Suffix rule should have no prerequisites");
            }
            if (two.size() == 0)
            {
                ls = "%";
                prereqs = "%" + one;
            }
            else
            {
                ls = "%" + two;
                prereqs = "%" + one;
            }
            n = iline.find_first_of(';');
            if (n != std::string::npos)
            {
                command = iline.substr(n + 1);
            }			
        }
        else
        {
join:
            n = UnfetteredChar(line, ':');
            if (n != std::string::npos)
            {
                targetPattern = iline.substr(0, n);
                iline = line.substr(n + 1);
            }
            n = iline.find_first_of('|');
            if (n != std::string::npos)
            {
                prereqs = iline.substr(0, n);
                iline.replace(0, n+1, "");
                n = iline.find_first_of(';');
                if (n != std::string::npos)
                {
                    orderPrereqs = iline.substr(0, n);
                    command = iline.substr(n + 1);
                    hasCmd = true;
                }
                else
                {
                    orderPrereqs = iline;
                }
                
            }
            else
            {
                n = iline.find_first_of(';');
                if (n != std::string::npos)
                {
                    prereqs = iline.substr(0, n);
                    command = iline.substr(n + 1);
                    hasCmd = true;
                }
                else
                {
                    prereqs = iline;
                }
            }
        }
        lastCommand = new Command(file, lineno);
        *CommandContainer::Instance() += lastCommand;
        if (hasCmd)
            *lastCommand += command;
        Eval p(prereqs, true);
        std::string ps = p.Evaluate();
        Eval o(orderPrereqs, true);
        std::string os = o.Evaluate();
        
        std::string related;
        if (ls.find_first_of('%') != std::string::npos)
            related = ls;
        else
            if (!ignoreFirstGoal)
            {
                size_t n;
                std::string aa = ls;
                Maker::SetFirstGoal(Eval::ExtractFirst(aa, " "));
            }
        while (ls.size())
        {
            std::string cur = Eval::ExtractFirst(ls, std::string(" "));
            std::string stem;
            Rule *rule;
            std::string ps1;
            if ((cur == ".SECONDARY" || cur == ".IGNORE") && ps =="" && os == "")
                ps1 = "%";
            else
                ps1 = ps;
            if (ps.size() == 0 && cur == ".SUFFIXES")
            {
                // clears all rules...
                RuleList *ruleList = RuleContainer::Instance()->Lookup(cur);
                if (ruleList)
                    *RuleContainer::Instance() -= ruleList;
            }
            else
            { 
                if (targetPattern.size())
                {
                    size_t start;
                    if (Eval::MatchesPattern(cur, targetPattern, start))
                    {
                        stem = Eval::FindStem(cur, targetPattern);
                        std::string ps2 = ps1;
                        ps2 = ReplaceAllStems(stem, ps2);
                        std::string os1 = os;
                        os1 = ReplaceAllStems(stem, os1);
                        rule = new Rule(cur, ps2, os1, lastCommand, file, lineno, secondaryExpansionEnabled);	
                    }
                    else
                    {
                        Eval::error("'" + cur + "' does not match target pattern '" + targetPattern + "'");
                    }
                }
                else
                {
                    rule = new Rule(cur, ps1, os, lastCommand, file, lineno, secondaryExpansionEnabled);	
                }
                RuleList *ruleList = RuleContainer::Instance()->Lookup(cur);		
                if (!ruleList)
                {
                    ruleList = new RuleList(cur);
                    ruleList->SetRelated(related);
                    *RuleContainer::Instance() += ruleList;
                }
                ruleList->SetTargetPatternStem(stem);
                if (rule)
                    ruleList->Add(rule, Double);
            }
        }
    }
    return rv;
}
bool Parser::ParseDefine(const std::string &line, bool override)
{
    Eval l(line, false);
    std::string ls = l.Evaluate();
    
    std::string rs ;
    bool found = false;
    while (remaining.size())
    {
        std::string l = GetLine(false);
        size_t n;
        std::string fw = FirstWord(l, n);
        if (fw == "endef")
        {
            found = true;
            break;
        }
        rs = rs + l + std::string("\n");
    }
    if (!found)
    {
        Eval::error("Parse sequence ended with define in progress");
        return false;
    }
    size_t n = ls.find_first_not_of(' ');
    ls = ls.substr(n);
    Variable *v = VariableContainer::Instance()->Lookup(ls);
    if (v)
    {
        v->AssignValue(rs, origin, override);
    }
    else
    {
        v = new Variable(ls, rs, Variable::f_recursive, origin);
        *VariableContainer::Instance() += v;
    }
    return true;	
}
bool Parser::Parsevpath(const std::string &line)
{
    if (line.size() == 0 || line.find_first_not_of(' ') == std::string::npos)
    {
        Eval::RemoveAllVPaths();
    }
    else
    {
        std::string iline = line;
        std::string first = Eval::ExtractFirst(iline, " ");
        if (iline.size() == 0 || iline.find_first_not_of(' ') == std::string::npos)
        {
            Eval::RemoveVPath(first);
        }
        else
        {
            Eval t(iline, false);
            iline = t.Evaluate();
            Eval::AddVPath(first, iline);
        }
    }
    return true;
}
bool Parser::ParseSpecialTarget(const std::string &line)
{
    return true;
}
bool Parser::ParseCommand(const std::string &line)
{
    if (!lastCommand)
        Eval::error("Command without a rule");
    else
    {
        size_t n = line.find("&&");
        *lastCommand += line;
        if (n != std::string::npos && n != line.size()-2)
        {
            char match = line[n +2];
            bool found = false;
            while (remaining.size() && !found)
            {
                std::string iline = GetLine(false);
                *lastCommand += iline;
                found = iline.find(match) != std::string::npos;
            }
            if (!found)
            {
                Eval::error("End of file detected while processing temporary command file");
            }
        }
    }
    return true;
}
bool Parser::ParseSpecial(const std::string &line)
{
    return true;
}
bool Parser::ParseInclude(const std::string &line, bool ignoreMissing)
{
    return Include::Instance()->AddFileList(line, ignoreMissing);
}
bool Parser::ConditionalArgument(std::string &line)
{
    bool rv = true;
    size_t s = line.find_first_not_of(' ');
    size_t e = line.find_last_not_of(' ');
    if (line[s] == '\'' || line[s] == '"')
    {
        if (e == s || line[e] != line[s])
            
        {
            Eval::error("Invalid string constant in conditional");
            rv = false;
        }
        else
        {
            line = line.substr(s, e);
        }
    }
    else
    {
        Eval l(line, false);
        line = l.Evaluate();
    }
    return rv;
}
bool Parser::ParseCond(const std::string &line, bool eq)
{
    bool rv = true;
    if (skips.size() && skips.front())
        skips.push_front(true);
    else
    {
        int n = line.find_first_not_of(' ');
        std::string left;
        std::string right;
        if (line[n] == '(')
        {
            int m = line.find_last_not_of(' ');
            if (line[m] == ')')
            {
                if (!Eval::TwoArgs(line.substr(n+1, m-1), left, right))
                {
                    rv = false;
                }
            }
            else
            {
                Eval::error("Conditional syntax error");
                rv = false;
            }
        }
        else
        {
            int m = line.find_first_of(line[n], n+1);
            if (m == std::string::npos)
            {
                Eval::error("Conditional syntax error");
                rv = false;
            }
            else
            {
                left = line.substr(n+1, m-1);
                n = line.find_first_not_of(' ', m+ 1);
                if (line[n] == '"' || line[n] == '\'')
                {
                    m = line.find_first_of(line[n], n+1);
                    if (m == std::string::npos)
                    {
                        Eval::error("Conditional syntax error");
                        rv = false;
                    }
                    else
                    {
                        right = line.substr(n+1, m-1);
                        if (line.find_first_not_of(' ', m+1) != std::string::npos)
                        {
                            Eval::error("Conditional syntax error");
                            rv = false;
                        }
                    }
                }
                else
                {
                    Eval::error("Conditional syntax error");
                    rv = false;
                }
                
            }
        }
        if (rv)
        {
            bool b;	
            Eval l(left, false);
            left = l.Evaluate();
            Eval r(right, false);
            right = r.Evaluate();
            if (eq)
                b = left != right;
            else
                b = left == right;
            skips.push_front(b);
        }
        else
            skips.push_front(false);
    }
    return rv;
}
bool Parser::ParseDef(const std::string &line, bool def)
{
    if (skips.size() && skips.front())
        skips.push_front(true);
    else
    {
        Eval l(line, false);
        std::string ls = l.Evaluate();
        ls = l.strip(ls);
        Variable *v = VariableContainer::Instance()->Lookup(ls);
        bool b;	
        if (def)
            b = v == NULL;
        else
            b = v != NULL;
        skips.push_front(b);
    }
    return true;
}
bool Parser::ParseElse(const std::string &line)
{
    bool rv = true;
    if (!skips.size())
    {
        Eval::error("else without conditional");
        rv = false;
    }
    else
    {
        bool b = skips.front();
        skips.pop_front();
        if (skips.size() && skips.front())
            skips.push_front(true);
        else
            skips.push_front(!b);
    }
    return rv;
}
bool Parser::ParseEndif(const std::string &line)
{
    bool rv = true;
    if (!skips.size())
    {
        Eval::error("endif without conditional");
        rv = false;
    }
    else
    {
        skips.pop_front();
    }
    return rv;
}
bool Parser::ParseExport(const std::string &line, bool exp)
{
    Eval r(line, false);
    std::string working = r.Evaluate();
    while (working.size())
    {
        std::string temp = Eval::ExtractFirst(working, " ");
        Variable *v = VariableContainer::Instance()->Lookup(temp);
        if (v)
            v->SetExport(exp);
    }
    return true;
}
