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

#ifndef RCFile_h
#define RCFile_h

#include "Lexer.h"
#include "Expression.h"
#include <string>

class PreProcessor;
class Resource;
class ResFile;

class RCFile
{
  public:
    RCFile(PreProcessor& PP, ResFile& ResFile, std::string& includes, int Language) :
        pp(PP), resFile(ResFile), lexer(PP), expr(lexer), language(Language), includePath(includes)
    {
    }
    virtual ~RCFile() {}

    void NextToken() { lexer.NextToken(); }
    const Token* GetToken() { return lexer.GetToken(); }
    bool IsKeyword();
    kw GetTokenId();
    bool IsNumber();
    unsigned GetNumber();
    bool IsString();
    std::wstring GetString();
    bool IsIdentifier();
    std::wstring GetId();
    void NeedEol();
    void SkipComma();
    void NeedBegin();
    void NeedEnd();
    int GetLanguage() { return language; }
    std::string GetFileName();
    bool AtEol() { return lexer.AtEol(); }
    bool AtEof() { return lexer.AtEof(); }
    std::string GetRestOfLine() { return lexer.GetRestOfLine(); }
    bool Read();
    ResFile& GetResFile() { return resFile; }
    std::string CvtString(const std::wstring& str);
    std::wstring CvtString(const std::string& str);

  protected:
    Resource* GetRes();
    void SkimStructOrEnum();
    void SkimTypedef();
    bool IsGenericResource();
    void SkimPrototype();

  private:
    PreProcessor& pp;
    Lexer lexer;
    Expression expr;
    int language;
    ResFile& resFile;
    std::string includePath;
};
#endif
