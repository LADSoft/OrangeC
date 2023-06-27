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

#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <list>
#include "Variable.h"

class Command;
class RuleList;
class Parser
{
  public:
    Parser(const std::string& string, const std::string& file, int lineno, bool incrementLineno = true,
           Variable::Origin oOrigin = Variable::o_file);
    void SetAutoExport() { autoExport = true; }
    ~Parser() {}
    bool Parse();
    void SetIgnoreFirstGoal() { ignoreFirstGoal = true; }

  protected:
    bool AlwaysEval(const std::string& line);
    void UnTab(std::string& value);
    std::string GetLine(bool inCommand);
    bool ParseLine(const std::string& line);
    size_t UnfetteredChar(const std::string& line, char ch) const;
    std::string FirstWord(const std::string& line, size_t& n);
    std::string RemoveComment(const std::string& line);
    bool ParseAssign(const std::string& left, const std::string& right, bool dooverride, bool exportSpecific, RuleList* ruleList = nullptr);
    bool ParseRecursiveAssign(const std::string& left, const std::string& right, bool dooverride, bool exportSpecific, RuleList* ruleList = nullptr);
    bool ParsePlusAssign(const std::string& left, const std::string& right, bool dooverride, bool exportSpecific, RuleList* ruleList = nullptr);
    bool ParseQuestionAssign(const std::string& left, const std::string& right, bool dooverride, bool exportSpecific, RuleList* ruleList = nullptr);
    std::string ReplaceAllStems(const std::string& stem, const std::string value);
    bool ParseRule(const std::string& left, const std::string& line);
    bool ParseDefine(const std::string& line, bool dooverride);
    bool Parsevpath(const std::string& line);
    bool ParseSpecialTarget(const std::string& line);
    bool ParseCommand(const std::string& line);
    bool ParseSpecial(const std::string& line);
    bool ParseInclude(const std::string& line, bool ignoreMissing);
    bool ConditionalArgument(std::string& line);
    bool ParseCond(const std::string& line, bool eq);
    bool ParseDef(const std::string& line, bool def);
    bool ParseElse(const std::string& line);
    bool ParseEndif(const std::string& line);
    bool ParseExport(const std::string& line, bool exp);

  private:
    int lineno;
    std::string remaining;
    Command* lastCommand;
    std::list<bool> skips;
    bool secondaryExpansionEnabled;
    std::string file;
    bool incrementLineno;
    Variable::Origin origin;
    bool ignoreFirstGoal;
    bool autoExport;
};
#endif