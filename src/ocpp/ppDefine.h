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

#ifndef ppDefine_h
#define ppDefine_h

#include <string>
#include <vector>
#include <set>
#include <ctime>
#include <memory>
#include <stack>
#include "Utils.h"
#include "SymbolTable.h"
#include "Token.h"
#include "ppExpr.h"
#include "ppCtx.h"
#include "ppMacro.h"

typedef std::vector<std::string> DefinitionArgList;

class ppInclude;
class ppMacro;

class ppDefine
{
  public:
    class TokenPos
    {
      public:
        short origStart;
        short origEnd;
        short newStart;
        short newEnd;
    };
    class Definition : public Symbol
    {
      public:
        Definition(const std::string& Name, std::string& Value, DefinitionArgList* List, bool Permanent) :
            Symbol(Name),
            value(Value),
            argList(List),
            permanent(Permanent),
            varargs(false),
            caseInsensitive(false),
            preprocessing(false),
            undefined(false),
            elipses(false)
        {
        }
        Definition& operator=(const Definition&);
        Definition(const Definition&);
        virtual ~Definition() {}
        bool IsPreprocessing() const { return preprocessing; }
        void SetPreprocessing(bool flag) { preprocessing = flag; }
        bool HasVarArgs() const { return varargs; }
        void SetHasVarArgs() { varargs = true; }
        int GetArgCount()
        {
            if (!argList)
                return 0;
            return argList->size();
        }
        DefinitionArgList* GetArg(int count)
        {
            if (!argList)
                return nullptr;
            else if (count < argList->size())
                return &argList.get()[count];
            return nullptr;
        }
        DefinitionArgList* GetArgList() const { return argList.get(); }
        std::string& GetValue() { return value; }
        bool IsCaseInsensitive() { return caseInsensitive; }
        void SetCaseInsensitive(bool flag) { caseInsensitive = flag; }

      private:
        bool caseInsensitive;
        bool undefined;
        bool permanent;
        bool elipses;
        bool varargs;
        bool preprocessing;
        std::string value;
        std::unique_ptr<DefinitionArgList> argList;
    };

  public:
    ppDefine(bool UseExtensions, ppInclude* Include, bool C89, bool Asmpp);
    ~ppDefine() {}

    void SetParams(ppCtx* Ctx, ppMacro* Macro)
    {
        ctx = Ctx;
        macro = Macro;
    }
    bool Check(kw token, std::string& line);
    Definition* Define(const std::string& name, std::string& value, DefinitionArgList* args, bool permanent, bool varargs,
                       bool errors, bool caseInsensitive);
    void Undefine(const std::string& name);
    Definition* Lookup(const std::string& name);
    int Process(std::string& line, bool leavePlaceholder = false);
    void replaceDefined(std::string& line);
    void Assign(const std::string& name, int value, bool caseInsensitive)
    {
        std::string v = Utils::NumberToString(value);
        Define(name, v, nullptr, false, false, false, caseInsensitive);
    }
    SymbolTable& GetDefines() { return symtab; }

    void PushPopMacro(std::string name, bool push);
    enum
    {
        MACRO_REPLACE_SIZE = 65536,
        REPLACED_TOKENIZING = 1,
        /* left or right-hand size of a ## when an arg has been replaced by an empty string */
        TOKENIZING_PLACEHOLDER = 2,
        STRINGIZING_PLACEHOLDER = 3,
        REPLACED_ALREADY = 4,
        MACRO_PLACEHOLDER = 5
    };
    std::string defid(const std::string& macro, int& i, int& j);
    void DoDefine(std::string& line, bool caseInsensitive);
    void DoAssign(std::string& line, bool caseInsensitive);
    void DoUndefine(std::string& line);
    void SetDefaults();
    int LookupDefault(std::string& macro, int begin, int end, const std::string& name);
    void Stringize(std::string& macro);
    bool Tokenize(std::string& macro);
    int InsertReplacementString(std::string& macro, int end, int begin, std::string text, std::string etext);
    bool NotSlashed(const std::string& macro, int pos);
    bool ppNumber(const std::string& macro, int begin, int pos);
    bool ReplaceArgs(std::string& macro, const DefinitionArgList& oldargs, const DefinitionArgList& newArgs,
                     const DefinitionArgList& expandedargs, std::deque<Definition*>& definitions, const std::string varargs);
    void SetupAlreadyReplaced(std::string& macro);
    int ReplaceSegment(std::string& line, int begin, int end, int& pptr, bool eol, std::deque<Definition*>& definitions,
                       std::deque<TokenPos>* positions);
    void SyntaxError(const std::string& name);
    void ParseAsmSubstitutions(std::string& line);
    void ReplaceAsmMacros(std::string& line);

    const std::deque<TokenPos>& TokenPositions() const { return tokenPositions; }

  private:
    SymbolTable symtab;
    ppInclude* include;
    std::string date;
    std::string dateiso;
    std::string time;
    std::unordered_map<std::string, std::stack<Definition*>> macroStacks;
    std::deque<TokenPos> tokenPositions;
    static KeywordHash defTokens;
    bool c89;
    ppExpr expr;
    ppCtx* ctx;
    ppMacro* macro;
    bool asmpp;
    int counter_val;
    time_t source_date_epoch;
};
#endif