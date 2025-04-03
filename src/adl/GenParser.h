/* Software License Agreement
 * 
 *     Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#ifndef GenParser_h
#    define Genparser_h

#    include "Loader.h"
#    include <deque>
#    include <map>
#    include <string>
#    include <fstream>
#    include <iostream>

class Parser;

class GenParser
{
  public:
    GenParser(Parser& p) : parser(p), file(nullptr) {}
    ~GenParser()
    {
        if (file)
            delete file;
    }
    bool Generate();

  protected:
    void GatherVars(std::map<std::string, std::string>& values, std::string name);
    bool GenerateOperandHeader();
    bool GenerateHeader();
    bool GenerateCPPHeader();
    bool GenerateTokenTables();
    bool GenerateStateFuncs();
    bool GenerateTokenizer();
    void GenerateAddressData(TokenNode* value, std::string coding);
    void GenerateAddressFuncs(TokenNode* value, std::string coding);
    void GenerateAddressTokenTree(TokenNode* value);
    bool GenerateAddresses();
    void GenerateOperandTokenTree(TokenNode* value);
    bool GenerateOperands();
    void GenerateAddressTable(TokenNode* value);
    bool GenerateOpcodes();
    void GenerateCoding(const std::string coding, const std::string name);
    bool GenerateUtilityFuncs();
    bool GenerateAddressParser();
    bool GenerateOperandParser();
    bool GenerateCodingProcessor();
    bool GenerateDispatcher();
    bool GenerateCompilerStubs();
    bool GenerateAdlHeader();
    void GeneratedFile();
    std::string convertname(const std::string& name);

  private:
    std::fstream* file;
    Parser& parser;

    std::string className;
    std::string operandClassName;
    std::string tokenClassName;
    std::deque<BYTE*> tokenData;
    std::map<std::string, int> valueTags;
    std::map<std::string, int> registerTags;
    std::map<std::string, int> stateFuncTags;
    std::map<std::string, int> stateVarTags;
};
#endif
