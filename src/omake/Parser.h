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
    Parser(const std::string &string, const std::string &file, int lineno, bool incrementLineno = true, Variable::Origin oOrigin = Variable::o_file);
	void SetAutoExport() { autoExport = true; }
    ~Parser() { }
    bool Parse();
    void SetIgnoreFirstGoal() { ignoreFirstGoal = true; }
protected:
    bool AlwaysEval(const std::string &line);
    void UnTab(std::string &value);
    std::string GetLine(bool inCommand);
    bool ParseLine(const std::string &line);
    size_t UnfetteredChar(const std::string &line, char ch) const;
    std::string FirstWord(const std::string &line, size_t &n);
    std::string RemoveComment(const std::string &line);
    bool ParseAssign(const std::string &left, const std::string &right, bool dooverride, RuleList *ruleList = nullptr);
    bool ParseRecursiveAssign(const std::string &left, const std::string &right, bool dooverride, RuleList *ruleList = nullptr);
    bool ParsePlusAssign(const std::string &left, const std::string &right, bool dooverride, RuleList *ruleList = nullptr);
    bool ParseQuestionAssign(const std::string &left, const std::string &right, bool dooverride, RuleList *ruleList = nullptr);
    std::string ReplaceAllStems(const std::string &stem, const std::string value);
    bool ParseRule(const std::string &left, const std::string &line);
    bool ParseDefine(const std::string &line, bool dooverride);
    bool Parsevpath(const std::string &line);
    bool ParseSpecialTarget(const std::string &line);
    bool ParseCommand(const std::string &line);
    bool ParseSpecial(const std::string &line);
    bool ParseInclude(const std::string &line, bool ignoreMissing);
    bool ConditionalArgument(std::string &line);
    bool ParseCond(const std::string &line, bool eq);
    bool ParseDef(const std::string &line, bool def);
    bool ParseElse(const std::string &line);
    bool ParseEndif(const std::string &line);
    bool ParseExport(const std::string &line, bool exp);
private:
    int lineno;
    std::string remaining;
    Command *lastCommand;
    std::list<bool> skips;
    bool secondaryExpansionEnabled;
    std::string file;
    bool incrementLineno;
    Variable::Origin origin;
    bool ignoreFirstGoal;
	bool autoExport;
};
#endif