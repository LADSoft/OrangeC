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

#include "Parser.h"
#include "Rule.h"
#include "Variable.h"
#include "Eval.h"
#include "Spawner.h"
#include "Include.h"
#include "Maker.h"
#include "UTF8.h"
#include <cctype>
#include <fstream>
#include <iostream>
#include <algorithm>

Parser::Parser(const std::string& string, const std::string& File, int Lineno, bool IncrementLineno, Variable::Origin oOrigin) :
    file(File),
    remaining(string),
    lineno(Lineno - 1),
    incrementLineno(IncrementLineno),
    lastCommand(nullptr),
    autoExport(false),
    secondaryExpansionEnabled(false),
    origin(oOrigin),
    ignoreFirstGoal(false)
{
}
bool Parser::AlwaysEval(const std::string& line)
{
    size_t n;
    std::string line1 = line;
    std::replace(line1.begin(), line1.end(), '\t', ' ');
    std::string firstWord = FirstWord(line1, n);
    return firstWord == "ifeq" || firstWord == "ifneq" || firstWord == "else" || firstWord == "endif" || firstWord == "ifdef" ||
           firstWord == "ifndef";
}
bool Parser::Parse()
{
    bool rv = true;
    while (!remaining.empty())
    {
        std::string line = GetLine(false);
        if (skips.empty() || !skips.front() || AlwaysEval(line))
        {
            if (line.find_first_not_of(" \t") != std::string::npos)
            {
                Eval::SetFile(file);
                Eval::SetLine(lineno);
                ParseLine(line);
            }
        }
    }
    if (!skips.empty())
    {
        Eval::SetFile(file);
        Eval::SetLine(lineno);
        Eval::error("File ended with conditional in progress");
        rv = false;
    }
    skips.clear();
    return rv;
}
void Parser::UnTab(std::string& value)
{
    size_t n = 0;
    while ((n = value.find_first_of('\t', n + 1)) != std::string::npos)
        value[n] = ' ';
}
std::string Parser::GetLine(bool inCommand)
{
    lineno++;
    std::string rv = Eval::ExtractFirst(remaining, "\n");
    if (rv[0] == '\t' && lastCommand)
        inCommand = true;

    // concatenate lines
    if (!rv.empty())
    {
        while (rv[rv.size() - 1] == '\\')
        {
            if (rv.size() > 1 && rv[rv.size() - 2] == '\\')
            {
                break;
            }
            if (remaining.empty())
            {
                Eval::error("backslash-newline at end of input stream");
                break;
            }
            rv.replace(rv.size() - 1, 1, " ");
            lineno++;
            std::string next = Eval::ExtractFirst(remaining, "\n");
            rv += next;
            if (next.empty())
                break;
        }
    }
    // get rid of comments
    if (!inCommand)
    {
        rv = RemoveComment(rv);
    }
    UnTab(rv);
    if (rv.find_first_not_of("\t ") == std::string::npos)
        rv = "";
    return rv;
}
std::string Parser::RemoveComment(const std::string& line)
{
    int start = 0;
    int n;
    std::string rv = line;
    do
    {
        n = rv.find_first_of('#', start);
        if (n != std::string::npos)
        {
            if (!n || rv[n - 1] != '\\')
            {
                rv.replace(n, rv.size() - n, "");
            }
            else
            {
                rv.replace(n, 1, "");
                start = n + 1;
            }
        }
    } while (n != std::string::npos);
    return rv;
}
void UnTab(std::string value)
{
    // get rid of tabs
    int start = 1;  // first tab in line remains
    int n;
    do
    {
        n = value.find_first_of('\t', start);
        if (n != std::string::npos)
        {
            value[n] = ' ';
            start = n + 1;
        }
    } while (n != std::string::npos);
}
size_t Parser::UnfetteredChar(const std::string& line, char ch) const
{
    int charInWord = 0;
    bool instr = false;
    for (size_t i = 0; i < line.size(); i++)
    {
        if (instr)
        {
            if (line[i] == '"')
            {
                charInWord = 0;
                instr = false;
            }
        }
        else if (line[i] == '"')
        {
            instr = true;
        }
        else if (line[i] == ' ')
        {
            charInWord = 0;
        }
        else if (line[i] == '$')
        {
            i += Eval::MacroSpan(line, i + 1);
            charInWord = 0;
        }
        else
        {
            charInWord++;
            if (ch == ':')
            {
                if (line[i] == ':' && (charInWord != 2 || !UTF8::IsAlpha(line[i - 1]) || isspace(line[i + 1])))
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
std::string Parser::FirstWord(const std::string& line, size_t& n)
{
    std::string rv;
    int s = line.find_first_not_of(' ');
    n = 0;
    if (s != std::string::npos)
    {
        size_t t = s;
        while (UTF8::IsAlnum(line[t]) || line[t] == '_' || line[t] == '-')
        {
            size_t v = UTF8::CharSpan(line.c_str() + t);
            for (size_t i = 0; i < v && t < line.size(); i++)
                t++;
        }
        rv = line.substr(s, t - s);
        n = t;
    }
    return rv;
}
bool Parser::ParseLine(const std::string& line)
{
    bool rv = false;
    if (line[0] == '\t' && lastCommand)
    {
        rv = ParseCommand(line.substr(1));
    }
    else
    {
        size_t n = 0;
        bool dooverride = false;
        std::string line1 = line;
        std::replace(line1.begin(), line1.end(), '\t', ' ');
        std::string firstWord = FirstWord(line1, n);
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
            dooverride = true;
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
        int eq = UnfetteredChar(iline, '=');
        int q = iline.find_first_of('?');
        int r = iline.find_first_of('+');
        int colon = UnfetteredChar(iline, ':');
        if (colon != std::string::npos && (eq == std::string::npos || colon < eq))
        {
            if (eq != std::string::npos && colon == eq - 1)
            {
                rv = ParseAssign(iline.substr(0, colon), iline.substr(eq + 1), dooverride, false);
            }
            else
            {
                if (dooverride)
                    Eval::warning("Expected variable assignment with override keyword");
                rv = ParseRule(iline.substr(0, colon), iline.substr(colon + 1));
            }
        }
        else if (eq != std::string::npos)
        {
            if (q != std::string::npos && q == eq - 1)
            {
                rv = ParseQuestionAssign(iline.substr(0, q), iline.substr(eq + 1), dooverride, false);
            }
            else if (r != std::string::npos && r == eq - 1)
            {
                rv = ParsePlusAssign(iline.substr(0, r), iline.substr(eq + 1), dooverride, false);
            }
            else
            {
                rv = ParseRecursiveAssign(iline.substr(0, eq), iline.substr(eq + 1), dooverride, false);
            }
        }
        else
        {
            Eval l(line, false);
            if (!l.Evaluate().empty())
            {
                if (dooverride)
                    Eval::error("Expected variable assignment");
                else
                    Eval::error("Expected rule");
            }
        }
    }
    return rv;
}
bool Parser::ParseAssign(const std::string& left, const std::string& right, bool dooverride, bool exportSpecific, RuleList* ruleList)
{
    if (left == ".VARIABLES")
        return true;
    Eval l(left, false);
    std::string ls = l.Evaluate();
    ls = l.strip(ls);
    Eval r(right, false);
    std::string rs = r.Evaluate();
    Eval::StripLeadingSpaces(rs);
    Variable* v;
    if (ruleList)
        v = ruleList->Lookup(ls);
    else
        v = VariableContainer::Instance()->Lookup(ls);
    if (v)
    {
        v->AssignValue(rs, origin, dooverride);
    }
    else
    {
        v = new Variable(ls, rs, Variable::f_simple, ruleList ? Variable::o_automatic : origin);
        if (ruleList)
            *ruleList += v;
        else
            *VariableContainer::Instance() += v;
    }
    if (v && !ruleList && (left == "MAKEFILES" || autoExport))
        v->SetExport(true);
    else if (v && ruleList)
        v->SetExport(exportSpecific);
    return true;
}
bool Parser::ParseRecursiveAssign(const std::string& left, const std::string& right, bool dooverride, bool exportSpecific, RuleList* ruleList)
{
    if (left == ".VARIABLES")
        return true;
    Eval l(left, false);
    std::string ls = l.Evaluate();
    ls = l.strip(ls);
    std::string rs = right;
    Eval::StripLeadingSpaces(rs);
    Variable* v;
    if (ruleList)
        v = ruleList->Lookup(ls);
    else
        v = VariableContainer::Instance()->Lookup(ls);
    if (v)
    {
        v->AssignValue(rs, origin, dooverride);
    }
    else
    {
        v = new Variable(ls, rs, Variable::f_recursive, ruleList ? Variable::o_automatic : origin);
        if (ruleList)
            *ruleList += v;
        else
            *VariableContainer::Instance() += v;
    }
    if (v && !ruleList && (left == "MAKEFILES" || autoExport))
        v->SetExport(true);
    else if (v && ruleList)
        v->SetExport(exportSpecific);
    return true;
}
bool Parser::ParsePlusAssign(const std::string& left, const std::string& right, bool dooverride, bool exportSpecific, RuleList* ruleList)
{
    if (left == ".VARIABLES")
        return true;
    Eval l(left, false);
    std::string ls = l.Evaluate();
    ls = l.strip(ls);
    std::string rs = right;
    //    Eval::StripLeadingSpaces(rs);
    Variable* v;
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
        v->AppendValue(rs, dooverride);
    }
    else
    {
        v = new Variable(ls, rs, Variable::f_recursive, ruleList ? Variable::o_automatic : origin);
        if (ruleList)
            *ruleList += v;
        else
            *VariableContainer::Instance() += v;
    }
    if (v && !ruleList && (left == "MAKEFILES" || autoExport))
        v->SetExport(true);
    else if (v && ruleList)
        v->SetExport(exportSpecific);
    return true;
}
bool Parser::ParseQuestionAssign(const std::string& left, const std::string& right, bool dooverride, bool exportSpecific, RuleList* ruleList)
{
    if (left == ".VARIABLES")
        return true;
    Eval l(left, false);
    std::string ls = l.Evaluate();
    ls = l.strip(ls);
    Variable* v;
    if (ruleList)
        v = ruleList->Lookup(ls);
    else
        v = VariableContainer::Instance()->Lookup(ls);
    if (!v)
    {
        std::string rs = right;
        //        Eval::StripLeadingSpaces(rs);
        v = new Variable(ls, rs, Variable::f_simple, ruleList ? Variable::o_automatic : origin);
        if (ruleList)
            *ruleList += v;
        else
            *VariableContainer::Instance() += v;
    }
    if (v && !ruleList && (left == "MAKEFILES" || autoExport))
        v->SetExport(true);
    else if (v && ruleList)
        v->SetExport(exportSpecific);
    return true;
}
std::string Parser::ReplaceAllStems(const std::string& stem, const std::string value)
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
bool Parser::ParseRule(const std::string& left, const std::string& line)
{
    bool rv = true;
    std::string targetPattern;
    std::string prereqs;
    std::string orderPrereqs;
    std::string command;
    bool Double = false;
    bool dontCare = false;
    bool ignore = false;
    bool silent = false;
    bool make = false;
    bool notMain = false;
    bool precious = false;
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
    size_t n = UnfetteredChar(line, '=');
    size_t p = UnfetteredChar(line, ';');
    if (n != 0 && n != std::string::npos && (p == std::string::npos || n < p))
    {
        size_t q = 0;
        std::string first = FirstWord(line, q);
        bool private_ = first != "export";
        enum e_mode
        {
            asn,
            qasn,
            pasn,
            rasn
        } mode;
        if (line[n - 1] == '?')
            mode = qasn;
        else if (line[n - 1] == ':')
            mode = asn;
        else if (line[n - 1] == '+')
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
            l = line.substr(0, n - 1);
        }
        if (first == "private" || first == "export")
            l = l.substr(q);
        Eval l1(l, false);
        l = l1.strip(l);
        r = line.substr(n + 1);
        while (!ls.empty() && rv)
        {
            std::string cur = Eval::ExtractFirst(ls, std::string(" "));
            RuleList* ruleList = RuleContainer::Instance()->Lookup(cur);
            if (!ruleList)
            {
                ruleList = new RuleList(cur);
                *RuleContainer::Instance() += ruleList;
            }
            switch (mode)
            {
                case rasn:
                    rv &= ParseRecursiveAssign(l, r, false, !private_, ruleList);
                    break;
                case pasn:
                    rv &= ParsePlusAssign(l, r, false, !private_, ruleList);
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
        if (m != std::string::npos && ls[m] == '.')
        {
            RuleList* ruleList = RuleContainer::Instance()->Lookup(".SUFFIXES");
            bool found1 = false, found2 = false;
            n = ls.find_first_of('.', m + 1);
            std::string one;
            std::string two;
            if (n == std::string::npos)
            {
                n = ls.find_first_of(' ', m);
                one = ls.substr(m, n);
                found2 = true;
            }
            else
            {
                one = ls.substr(m, n);
                m = n;
                n = ls.find_first_of(' ', m);
                two = ls.substr(m, n);
            }
            if (ruleList)
            {
                for (auto it = ruleList->begin(); it != ruleList->end() && (!found1 || !found2); ++it)
                {
                    std::string working = (*it)->GetPrerequisites();
                    while (!working.empty())
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
            // in line - if it does treat it as a normal rule
            if (!line.empty() && line[0] != ':')
            {
                m = line.find_first_not_of(' ');
                if (m != std::string::npos && line[m] != ';')
                    goto join;
            }
            if (two.empty())
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
                std::replace(targetPattern.begin(), targetPattern.end(), '\n', ' ');
            }
            std::string iparsed;
            precious = Double;
            while (!iline.empty())
            {
                std::string cur = Eval::ExtractFirst(iline, std::string(" "));
                if (cur[0] != '.')
                    iparsed += cur + " ";
                else if (cur == ".DONTCARE" || cur == ".OPTIONAL")
                    dontCare = true;
                else if (cur == ".SILENT")
                    silent = true;
                else if (cur == ".IGNORE")
                    ignore = true;
                else if (cur == ".NOTMAIN")
                    notMain = true;
                else if (cur == ".MAKE")
                    make = true;
                else if (cur == ".PRECIOUS")
                    precious = true;
                else if (cur == ".EXEC" || cur == ".EXPORT" || cur == ".EXPORTSAME" || cur == ".INVISIBLE" || cur == ".JOIN" ||
                         cur == ".NOEXPORT" || cur == ".USE" || cur == ".WAIT")
                    Eval::warning(std::string("Target Attribute '") + cur + "' ignored");
                else
                    iparsed += cur + " ";
            }
            iline = iparsed;
            n = iline.find_first_of('|');
            if (n != std::string::npos)
            {
                prereqs = iline.substr(0, n);
                iline.replace(0, n + 1, "");
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
        lastCommand = new Command(file, lineno + 1);
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
        else if (!ignoreFirstGoal && !notMain)
        {
            std::string aa = ls;
            Maker::SetFirstGoal(Eval::ExtractFirst(aa, " "));
        }
        while (!ls.empty())
        {
            std::string cur = Eval::ExtractFirst(ls, std::string(" "));
            std::string stem;
            Rule* rule = NULL;
            std::string ps1;
            if ((cur == ".SECONDARY" || cur == ".IGNORE") && ps == "" && os == "")
                ps1 = "%";
            else
                ps1 = ps;
            if (ps.empty() && cur == ".SUFFIXES")
            {
                // clears all rules...
                RuleList* ruleList = RuleContainer::Instance()->Lookup(cur);
                if (ruleList)
                    *RuleContainer::Instance() -= ruleList;
            }
            else
            {
                if (!targetPattern.empty())
                {
                    size_t start;
                    if (Eval::MatchesPattern(cur, targetPattern, start) != std::string::npos)
                    {
                        stem = Eval::FindStem(cur, targetPattern);
                        std::string ps2 = ps1;
                        ps2 = ReplaceAllStems(stem, ps2);
                        std::string os1 = os;
                        os1 = ReplaceAllStems(stem, os1);
                        rule = new Rule(cur, ps2, os1, lastCommand, file, lineno, dontCare, ignore, silent, make, precious,
                                        secondaryExpansionEnabled);
                    }
                    else
                    {
                        Eval::error("'" + cur + "' does not match target pattern '" + targetPattern + "'");
                    }
                }
                else
                {
                    rule = new Rule(cur, ps1, os, lastCommand, file, lineno, dontCare, ignore, silent, make, precious,
                                    secondaryExpansionEnabled);
                }
                RuleList* ruleList = RuleContainer::Instance()->Lookup(cur);
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
bool Parser::ParseDefine(const std::string& line, bool dooverride)
{
    Eval l(line, false);
    std::string ls = l.Evaluate();

    std::string rs;
    bool found = false;
    while (!remaining.empty())
    {
        std::string l = GetLine(false);
        size_t n;
        std::string fw = FirstWord(l, n);
        if (fw == "endef")
        {
            found = true;
            break;
        }
        rs = rs + l + "\n";
    }
    if (!found)
    {
        Eval::error("Parse sequence ended with define in progress");
        return false;
    }
    size_t n = ls.find_first_not_of(' ');
    ls = ls.substr(n);
    while ((!ls.empty() && ls[ls.size() - 1] == '=') || isspace(ls[ls.size() - 1]))
        ls = ls.substr(0, ls.size() - 1);
    Variable* v = VariableContainer::Instance()->Lookup(ls);
    if (v)
    {
        v->AssignValue(rs, origin, dooverride);
    }
    else
    {
        v = new Variable(ls, rs, Variable::f_recursive, origin);
        *VariableContainer::Instance() += v;
    }
    return true;
}
bool Parser::Parsevpath(const std::string& line)
{
    if (line.empty() || line.find_first_not_of(' ') == std::string::npos)
    {
        Eval::RemoveAllVPaths();
    }
    else
    {
        std::string iline = line;
        std::string first = Eval::ExtractFirst(iline, " ");
        if (iline.empty() || iline.find_first_not_of(' ') == std::string::npos)
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
bool Parser::ParseSpecialTarget(const std::string& line) { return true; }
bool Parser::ParseCommand(const std::string& line)
{
    if (!lastCommand)
        Eval::error("Command without a rule");
    else
    {
        size_t n = line.find("&&");
        *lastCommand += line;
        if (n != std::string::npos && n != line.size() - 2)
        {
            // disable the temporary command files for /bin/sh
            Variable* v = VariableContainer::Instance()->Lookup("SHELL");
            if (v)
            {
                std::string shell = v->GetValue();
                if (shell != "/bin/sh")
                {
                    char match = line[n + 2];
                    bool found = false;
                    while (!remaining.empty() && !found)
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
        }
    }
    return true;
}
bool Parser::ParseSpecial(const std::string& line) { return true; }
bool Parser::ParseInclude(const std::string& line, bool ignoreMissing)
{
    return Include::Instance()->AddFileList(line, ignoreMissing);
}
bool Parser::ConditionalArgument(std::string& line)
{
    bool rv = true;
    size_t s = line.find_first_not_of(' ');
    size_t e = line.find_last_not_of(' ');
    if (s >= 0 && (line[s] == '\'' || line[s] == '"'))
    {
        if (e < 0 || e == s || line[e] != line[s])

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
bool Parser::ParseCond(const std::string& line, bool eq)
{
    bool rv = true;
    if (!skips.empty() && skips.front())
        skips.push_front(true);
    else
    {
        int n = line.find_first_not_of(' ');
        std::string left;
        std::string right;
        if (line[n] == '(')
        {
            int m = line.find_last_not_of(' ');
            if (m >= 0 && line[m] == ')')
            {
                if (!Eval::TwoArgs(line.substr(n + 1, m - 1 - n), left, right))
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
            int m = line.find_first_of(line[n], n + 1);
            if (m == std::string::npos)
            {
                Eval::error("Conditional syntax error");
                rv = false;
            }
            else
            {
                left = line.substr(n + 1, m - 1);
                n = line.find_first_not_of(' ', m + 1);
                if (n >= 0 && (line[n] == '"' || line[n] == '\''))
                {
                    m = line.find_first_of(line[n], n + 1);
                    if (m == std::string::npos)
                    {
                        Eval::error("Conditional syntax error");
                        rv = false;
                    }
                    else
                    {
                        right = line.substr(n + 1, m - 1);
                        if (line.find_first_not_of(' ', m + 1) != std::string::npos)
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
bool Parser::ParseDef(const std::string& line, bool def)
{
    if (!skips.empty() && skips.front())
        skips.push_front(true);
    else
    {
        Eval l(line, false);
        std::string ls = l.Evaluate();
        ls = l.strip(ls);
        Variable* v = VariableContainer::Instance()->Lookup(ls);
        bool b;
        if (def)
            b = v == nullptr;
        else
            b = v != nullptr;
        skips.push_front(b);
    }
    return true;
}
bool Parser::ParseElse(const std::string& line)
{
    bool rv = true;
    if (skips.empty())
    {
        Eval::error("else without conditional");
        rv = false;
    }
    else
    {
        bool b = skips.front();
        skips.pop_front();
        if (!skips.empty() && skips.front())
            skips.push_front(true);
        else
            skips.push_front(!b);
    }
    return rv;
}
bool Parser::ParseEndif(const std::string& line)
{
    bool rv = true;
    if (skips.empty())
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
bool Parser::ParseExport(const std::string& line, bool exp)
{
    Eval r(line, false);
    std::string working = r.Evaluate();
    while (!working.empty())
    {
        std::string temp = Eval::ExtractFirst(working, " ");
        Variable* v = VariableContainer::Instance()->Lookup(temp);
        if (v)
            v->SetExport(exp);
    }
    return true;
}
