/* Software License Agreement
 * 
 *     Copyright(C) 1994-2018 David Lindauer, (LADSoft)
 * 
 *     This file is part of the Orange C Compiler package.
 * 
 *     The Orange C Compiler package is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version, with the addition of the 
 *     Orange C "Target Code" exception.
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

#include "GenParser.h"
#include "Loader.h"
#include "TokenNode.h"
#include "Utils.h"
#include <ctype.h>
#include <stdlib>
#include <fstream>
#include <sstream>
#include <assert.h>
bool GenParser::Generate()
{
    bool rv = false;
    className = parser.processorName + "Parser";
    operandClassName = parser.processorName + "Operand";
    tokenClassName = parser.processorName + "Token";
    std::string headerName = operandClassName + ".h";
    file = new std::fstream(headerName.c_str(), std::ios::out);
    if (file->is_open())
    {
        if (GenerateOperandHeader())
        {
            delete file;
            std::string fileName = className + ".h";
            file = new std::fstream(fileName.c_str(), std::ios::out);
            if (file->is_open())
            {
                if (GenerateHeader())
                {
                    delete file;
                    std::string fileName = className + ".cpp";
                    file = new std::fstream(fileName.c_str(), std::ios::out);
                    if (GenerateCPPHeader())
                        if (GenerateStateFuncs())	
                            if (GenerateTokenTables())
                                if (GenerateTokenizer())
                                    if (GenerateAddresses())
                                        if (GenerateOperands())
                                            if (GenerateOpcodes())
                                                if (GenerateUtilityFuncs())
                                                    if (GenerateAddressParser())
                                                        if (GenerateOperandParser())
                                                            if (GenerateCodingProcessor())
                                                                if (GenerateDispatcher())
                                                                {
                                                                    rv = true;
                                                                }
                }
            }
        }
    }
    return rv;
}
void GenParser::GatherVars(std::map<std::string, std::string> &values, std::string name)
{
    for (std::map<std::string, std::string>::iterator it1 = values.begin();
         it1 != values.end(); ++it1)
    {
        std::map<std::string, int>::iterator it2 = valueTags.find(it1->first);
        if (it2 == valueTags.end())
        {
            valueTags[it1->first] = valueTags.size();
        }
    }
    while (name.size())
    {
        int n = name.find_first_of("'");
        if (n == std::string::npos)
            name = "";
        else
        {
            int n1 = name.find_first_of("'", n+1);
            if (n1 == std::string::npos)
            {
                name = "";
            }
            else
            {
                int n2 = name.find_first_of(":", n+1);
                if (n2 != std::string::npos && n2 < n1)
                {
                    std::string var = name.substr(n+1, n2 -n -1);
                    std::map<std::string, int>::iterator it2 = valueTags.find(var);
                    if (it2 == valueTags.end())
                    {
                        valueTags[var] = valueTags.size();
                    }					
                }
                if (n1+1 == name.size())
                    name = "";
                else
                    name = name.substr(n1+1);
            }
        }
    }
}
bool GenParser::GenerateOperandHeader()
{
    for (std::deque<Address *>::iterator it = parser.addresses.begin();
         it != parser.addresses.end(); ++it)
    {
        GatherVars((*it)->values, (*it)->name);
    }
    for (std::deque<Operand *>::iterator it = parser.operands.begin();
         it != parser.operands.end(); ++it)
    {
        GatherVars((*it)->values, (*it)->name);
    }
    for (std::deque<Opcode *>::iterator it = parser.opcodes.begin();
         it != parser.opcodes.end(); ++it)
    {
        GatherVars((*it)->values, "");
    }
    for (std::deque<TokenNode *>::iterator it = TokenNode::tokenList.begin();
         it != TokenNode::tokenList.end(); ++it)
    {
        if ((*it)->name != "")
        {
            std::map<std::string, int>::iterator it2 = valueTags.find((*it)->name);
            if (it2 == valueTags.end())
            {
                valueTags[(*it)->name] = valueTags.size();
            }
        }
    }
    (*file) << "#ifndef " << operandClassName << "_h" << std::endl;
    (*file) << "#define " << operandClassName << "_h" << std::endl;
    (*file) << std::endl;
    
    (*file) << "class " << className << ";" << std::endl;	
    (*file) << "class Coding;" << std::endl;	
    (*file) << "static const int OPARRAY_SIZE = " << valueTags.size() << ";" << std::endl;
    (*file) << "class " << operandClassName << std::endl;
    (*file) << "{ " << std::endl;
    (*file) << "public:" << std::endl;
    (*file) << "\t" << operandClassName << "() : opcode(-1), addressCoding(-1), operandCoding(-1) { Clear(); }" << std::endl;
    (*file) << "\tvoid Clear() { memset(values, 0, sizeof(values)); }" << std::endl;
    (*file) << "\tint opcode;" << std::endl;
    (*file) << "\tint addressCoding;" << std::endl;
    (*file) << "\tint operandCoding;" << std::endl;
    (*file) << "\tCoding *values[OPARRAY_SIZE];" << std::endl;
    (*file) << "}; " << std::endl;
    (*file) << std::endl;	
    
    (*file) << "struct " << tokenClassName << std::endl;
    (*file) << "{" << std::endl;
    (*file) << "\tenum" << std::endl;
    (*file) << "\t{" << std::endl;
    (*file) << "\t\tEOT, TOKEN, REGISTER, REGISTERCLASS, ADDRESSCLASS, NUMBER, EMPTY" << std::endl;
    (*file) << "\t} type;" << std::endl;
    (*file) << "\tunsigned char id;" << std::endl;
    (*file) << "\tunsigned char eos;" << std::endl;
    (*file) << "\tunsigned char level;" << std::endl;
    (*file) << "\tunsigned char *addrClass;" << std::endl;
    (*file) << "\tvoid (" << className << "::*tokenFunc)(" << operandClassName << "& operand, int tokenPos);" << std::endl;
    (*file) << "\t" << tokenClassName <<" *next;" << std::endl;
    (*file) << "};" << std::endl;
    (*file) << std::endl;
    (*file) << "#endif" << std::endl;
    return true;
}
bool GenParser::GenerateHeader()
{
    (*file) << "#ifndef " << className << "_h" << std::endl;
    (*file) << "#define " << className << "_h" << std::endl;
    (*file) << std::endl;

    (*file) << "class Section;" << std::endl;
    (*file) << "class AsmFile;" << std::endl;
    (*file) << std::endl;
    (*file) << "class " << className << " : public InstructionParser " << std::endl;
    (*file) << "{ " << std::endl;
    (*file) << "public:" << std::endl;
    (*file) << "\t" << className << "() { Init(); }" << std::endl;
    int i = 0;
    for (std::map<std::string, std::string>::iterator it= parser.stateVars.begin();
         it != parser.stateVars.end(); ++it, ++i)
    {
        stateVarTags[it->first] = i;
        (*file) << "\tvoid Set" << it->first << "(int v) { stateVars[" << i << "] = v; }" << std::endl;
        (*file) << "\tint Get" << it->first << "() { return stateVars[" << i << "]; }" << std::endl;
    }
    (*file) << "\tvirtual void Setup(Section *sect);" << std::endl;
    (*file) << "\tvirtual void Init();" << std::endl;
    (*file) << "\tvirtual bool ParseSection(AsmFile *fil, Section *sect);" << std::endl;
    int be = 0;
    std::string bes = parser.parameters["Endian"];
    if (bes.size())
        be = atoi(bes.c_str());
    (*file) << "\tvirtual bool IsBigEndian() { return " << be << "; }" << std::endl;
    (*file) << std::endl;
    (*file) << "protected:" << std::endl;
    (*file) << "\tbool ParseAddresses("<<operandClassName<<" &operand, int addrClass, int &tokenPos);" << std::endl;
    (*file) << "\tbool ParseOperands2(" << tokenClassName << " *tokenList, "<<operandClassName<<" &operand, int tokenPos, int level);" << std::endl;
    (*file) << "\tbool ParseOperands(" << tokenClassName << " *tokenList, "<<operandClassName<<" &operand);" << std::endl;
    (*file) << "\tbool ProcessCoding(CodingHelper &base, "<<operandClassName<<" &operand, Coding *coding);" << std::endl;
    (*file) << "\tbool MatchesToken(int token, int tokenPos);" << std::endl;
    (*file) << "\tbool MatchesRegister(int reg, int tokenPos);" << std::endl;
    (*file) << "\tbool MatchesRegisterClass(int cclass, int tokenPos);" << std::endl;
    (*file) << "\tvirtual bool DispatchOpcode(int opcode);" << std::endl;
    (*file) << std::endl;	
    (*file) << "\ttypedef bool ("<<className<<"::*DispatchType)(" << operandClassName << " &);" << std::endl;
    (*file) << "\tstatic DispatchType DispatchTable[];" << std::endl;
    (*file) << std::endl;
    (*file) << "\ttypedef Coding * (" << className << "::*StateFuncDispatchType)();" << std::endl;
    (*file) << "\tstatic StateFuncDispatchType stateFuncs[];" << std::endl;
    i = 1;
    for (std::deque<State *>::iterator it = parser.states.begin();
         it != parser.states.end(); ++it, ++i)
    {
        stateFuncTags[(*it)->name] = stateFuncTags.size();
        (*file) << "\tCoding *StateFunc" << i << "();" << std::endl;
    }
    (*file) << std::endl;
    (*file) << "private:" << std::endl;
    (*file) << "\tint stateVars[" << parser.stateVars.size() << "];" << std::endl;
    (*file) << std::endl;
    i = 1;
    (*file) << "\tstatic Coding stateCoding_eot[];" << std::endl;
    for (std::deque<State *>::iterator it = parser.states.begin();
         it != parser.states.end(); ++it, ++i)
    {
        int j = 1;
        for (std::map<std::string, std::string>::iterator it1 = (*it)->clauses.begin();
             it1 != (*it)->clauses.end(); ++it1, ++j)
             
        {
            (*file) << "\tstatic Coding stateCoding" << i << "_" << j << "[];" << std::endl;
        }
    }
    (*file) << std::endl;
    (*file) << "\ttypedef bool (" << className << "::*NumberDispatchType)(int tokenPos);" << std::endl;
    (*file) << "\tstatic NumberDispatchType numberFuncs[];" << std::endl;
    for (std::deque<Number *>::iterator it = parser.numbers.begin();
         it != parser.numbers.end(); ++ it)
    {
        (*file) << "\tbool Number" << (*it)->id + 1 << "(int tokenPos);" << std::endl;
    }
    (*file) << std::endl;
    int nextToken = 1;
    int n = (parser.addressClasses.size() + 7)/8;
    for (std::deque<TokenNode *>::iterator it = TokenNode::tokenList.begin() ;
             it != TokenNode::tokenList.end(); ++it)
    {
        if ((*it)->used && (*it)->bytes)
        {
            int x = nextToken;
            int i = 1;
            for (std::deque<BYTE *>::iterator it1 = tokenData.begin(); it1 != tokenData.end(); it1++, i++)
            {
                if (!memcmp(*it1, (*it)->bytes, n))
                {
                    x = i;
                    break;
                }
            }
            if (x == nextToken)
            {
                (*file) << "\tstatic unsigned char RegClassData" << x << "[];" <<std::endl;
                nextToken++;
                tokenData.push_back((*it)->bytes);
            }
            (*it)->tokenId = x;
        }
    }
    (*file) << std::endl;
    for (std::map<std::string, RegClass *>::iterator it = parser.registerClasses.begin();
         it != parser.registerClasses.end(); ++it)
    {
        (*file) << "\tstatic unsigned char registerData" << it->second->id << "[];" << std::endl;
    }
    (*file) << "\tstatic unsigned char *registerDataIndirect[];" << std::endl;
    (*file) << std::endl;
    for (std::deque<Register *>::iterator it = parser.registers.begin();
         it != parser.registers.end(); ++it)
    {
        for (std::map<std::string, std::string>:: iterator it1 = (*it)->values.begin();
             it1 != (*it)->values.end(); ++it1)
        {
            std::map<std::string, int>::iterator it2 = registerTags.find(it1->first);
            if (it2 == registerTags.end())
            {
                registerTags[it1->first] = registerTags.size();
            }
        }
    }
    (*file) << "\tstatic int registerValues[][" << registerTags.size() << "];" << std::endl;
    (*file) << std::endl;
    for (std::deque<TokenNode *>::iterator it = TokenNode::tokenList.begin() ;
             it != TokenNode::tokenList.end(); ++it)
    {
        if ((*it)->used)
        {
            std::map<std::string, std::string> *vals = NULL;
            if ((*it)->values || (*it)->coding != -1 || (*it)->type == TokenNode::tk_regclass || (*it)->type == TokenNode::tk_number)
            {
                if ((*it)->values)
                {
                    for (std::map<std::string, std::string>::iterator it1 =
                         (*it)->values->begin(); it1 != (*it)->values->end(); ++it1)
                    {
                        int n = valueTags[it1->first];
                        (*file) << "\tstatic Coding tokenCoding" << (*it)->id << "_" << n << "[];" << std::endl;
                    }
                }
                (*file) << "\tvoid TokenFunc" << (*it)->id << "(" << operandClassName << " &operand, int tokenPos);" << std::endl;
            }
            if ((*it)->branches.size())
            {
                (*file) << "\tstatic " << tokenClassName << " tokenBranches" << (*it)->id << "[];" << std::endl;
            }
        }
    }
    (*file) << std::endl;	
    for (std::deque<Opcode *>::iterator it = parser.opcodes.begin();
         it != parser.opcodes.end(); ++it)
    {
        for (std::map<std::string, std::string> :: iterator it1 = (*it)->values.begin();
             it1 != (*it)->values.end(); ++ it1)
        {
            int n = valueTags[it1->first];
            (*file) << "\tstatic Coding OpcodeCodings" << (*it)->id << "_" << n << "[];" << std::endl;
        }
        (*file) << "\tbool Opcode" << (*it)->id << "(" << operandClassName << " &operand);" << std::endl;
    }
    (*file) << std::endl;	
    for (int i=0; i < parser.codings.size(); i++)
    {
        (*file) << "\tstatic Coding Coding" << i + 1 << "[];" << std::endl;
    }
    (*file) << "\tstatic Coding *Codings[];" << std::endl;
    (*file) << std::endl;	
    if (parser.prefixes.size())
    {
        i = 1;
        for (std::deque<Prefix *>::iterator it = parser.prefixes.begin();
             it != parser.prefixes.end(); ++it, ++i)
        {
            (*file) << "\tstatic Coding prefixCoding" << i << "[];" << std::endl;
        }
        (*file) << "\tstatic Coding *prefixCodings" << "[];" << std::endl;
        (*file) << std::endl;	
    }
    (*file) << "}; " << std::endl;
    (*file) << std::endl;
    (*file) << "#endif" << std::endl;
    return true;
}
bool GenParser::GenerateCPPHeader()
{
    (*file) << "#include \"InstructionParser.h\"" << std::endl;
    (*file) << "#include \"" << operandClassName << ".h" << "\"" << std::endl;
    (*file) << "#include \"" << className << ".h" << "\"" << std::endl;
    (*file) << std::endl;
    (*file) << "InstructionParser *InstructionParser::GetInstance()" << std::endl;
    (*file) << "{" << std::endl;
    (*file) << "\treturn static_cast<InstructionParser *>(new " << className << "());" << std::endl;
    (*file) << "}" << std::endl;
    return true;
}
bool GenParser::GenerateStateFuncs()
{
    int i = 1;
    (*file) << "Coding " << className << "::stateCoding_eot[] = { { Coding::eot } };" << std::endl;
    for (std::deque<State *>::iterator it = parser.states.begin();
         it != parser.states.end(); ++it, ++i)
    {
        int j = 1;
        for (std::map<std::string, std::string>::iterator it1 = (*it)->clauses.begin();
             it1 != (*it)->clauses.end(); ++it1, ++j)
             
        {
            (*file) << "Coding " << className << "::stateCoding" << i << "_" << j << "[] = {" << std::endl;
            GenerateCoding(it1->second);
            (*file) << "};" << std::endl;
        }
    }
    i = 1;
    for (std::deque<State *>::iterator it = parser.states.begin();
         it != parser.states.end(); ++it, ++i)
    {
        (*file) << "Coding *" << className << "::StateFunc" << i << "()" << std::endl;
        (*file) << "{" << std::endl;
        int j = 1;
        for (std::map<std::string, std::string>::iterator it1 = (*it)->clauses.begin();
             it1 != (*it)->clauses.end(); ++it1, ++j)
             
        {
            std::string sel = it1->first;
            int npos = sel.find_first_of("'");
            while (npos != std::string::npos)
            {
                int n1 = sel.find_first_of("'", npos+1);
                if (n1 == std::string::npos)
                {
                    std::cout << "Error { " << it1->first << "} Invalid when clause in state" << std::endl;
                    break;
                }
                std::string val = sel.substr(npos + 1, n1 - npos - 1);
                std::map<std::string, int>:: iterator it2 = stateVarTags.find(val);
                if (it2 == stateVarTags.end())
                {
                    std::cout << "Error { " << it1->first << " } Unknown state var '" << val << "'" << std::endl;
                    break;
                }
                else
                {
                    std::stringstream t ;
                    t << "stateVars[" << it2->second << "]";
                    val = "";
                    t >> val;
                    sel.replace(npos, n1 - npos + 1, val);
                }
                npos = sel.find_first_of("'");
            }
            (*file) << "\tif (" << sel << ")" << std::endl;
            (*file) << "\t{" << std::endl;
            (*file) << "\t\treturn stateCoding" << i << "_" << j << ";" << std::endl;
            (*file) << "\t}" << std::endl;
            
        }
        (*file) << "\treturn stateCoding_eot;" << std::endl;
        (*file) << "}" << std::endl;
    }
    (*file) << className << "::StateFuncDispatchType " << className << "::stateFuncs[] = {" << std::endl;
    i = 1;
    for (std::deque<State *>::iterator it = parser.states.begin();
         it != parser.states.end(); ++it, ++i)
    {
        (*file) << "\t&" << className << "::StateFunc" << i << "," << std::endl;
    }
    (*file) << "};" << std::endl;
    for (std::deque<Number *>::iterator it = parser.numbers.begin();
         it != parser.numbers.end(); ++ it)
    {
        (*file) << "bool " << className << "::Number" << (*it)->id + 1 << "(int tokenPos)" << std::endl;
        (*file) << "{" << std::endl;
        (*file) << "\tbool rv = false;" << std::endl;
        for (std::deque<std::string>::iterator it1 = (*it)->values.begin();
             it1 != (*it)->values.end(); ++ it1)
        {
            std::string temp = (*it1);
            std::string sel;
            int npos = temp.find("&");
            if (npos != std::string::npos)
            {
                sel = temp.substr(npos);
                temp = temp.substr(0, npos);
                sel.replace(0,1,"");
                std::string err = sel;
                int npos = sel.find_first_of("'");
                while (npos != std::string::npos)
                {
                    int n1 = sel.find_first_of("'", npos+1);
                    if (n1 == std::string::npos)
                    {
                        std::cout << "Error { " << err << "} Invalid when clause in number" << std::endl;
                        break;
                    }
                    std::string val = sel.substr(npos + 1, n1 - npos - 1);
                    std::map<std::string, int>:: iterator it2 = stateVarTags.find(val);
                    if (it2 == stateVarTags.end())
                    {
                        std::cout << "Error { " << err << " } Unknown state var '" << val << "'" << std::endl;
                        break;
                    }
                    else
                    {
                        std::stringstream t ;
                        t << "stateVars[" << it2->second << "]";
                        val = "";
                        t >> val;
                        sel.replace(npos, n1 - npos + 1, val);
                    }
                    npos = sel.find_first_of("'");
                }
            }
            bool xsigned = false;
            int bits = 0;
            bool label = false;
            if (temp[0] == '-')
            {
                xsigned = true;
                temp.replace(0,1,"");
            }
            else if (temp[0] == '+')
            {
                temp.replace(0,1,"");
            }
            if (isdigit(temp[0]))
            {
                int oldv, newv;
                oldv = strtol(temp.c_str(), NULL, 0);
                npos = temp.find_first_of(';');
                if (npos != std::string::npos && npos != temp.size()-1)
                {
                    newv= strtol(temp.c_str() + npos + 1, NULL, 0);
                }
                else
                {
                    newv = oldv;
                }
                if (sel != "")
                    (*file) << "\tif (!rv && " << sel << ")" << std::endl;
                else
                    (*file) << "\tif (!rv)" << std::endl;
                (*file) << "\t\trv = SetNumber(tokenPos, " << oldv << ", " << newv << ");" << std::endl;
            }
            else
            {
                if (temp[0] == '$')
                {
                    label = true;
                    temp.erase(0,1);
                }
                temp.erase(0,1); // # symbol
                if (temp.size())
                {
                    if (temp[0] == ':')
                    {
                        bits = strtol(temp.c_str() + 1, NULL, 0);
                    }
                }
                    
                if (sel != "")
                    (*file) << "\tif (!rv && " << sel << ")" << std::endl;
                else
                    (*file) << "\tif (!rv)" << std::endl;
                (*file) << "\t{" << std::endl;
                /*
                if (label)
                {
                    (*file) << "\t\trv = ParseLabel(" << (*it)->relOfs << ", " << (xsigned ? 1 : 0) << ", " << bits << ");" << std::endl;
                }
                else
                */
                {
                    (*file) << "\t\trv = ParseNumber(" << (*it)->relOfs << ", " << (xsigned ? 1 : 0) << ", " << bits << ", " << (label ? 0 : 1) << ", tokenPos);" << std::endl;
                }
                (*file) << "\t}" << std::endl;
            }
        }
        (*file) << "\treturn rv;" << std::endl;
        (*file) << "}" << std::endl;
    }
    (*file) << className << "::NumberDispatchType " << className << "::numberFuncs[] = {" << std::endl;
    for (std::deque<Number *>::iterator it = parser.numbers.begin();
         it != parser.numbers.end(); ++ it)
    {
        (*file) << "\t&" << className << "::Number" << (*it)->id + 1 << "," << std::endl;
    }
    (*file) << "};" << std::endl;
    (*file) << std::endl;
    return true;
}
bool GenParser::GenerateTokenizer()
{
    (*file) << "void "<<className<<"::Init()" << std::endl;
    (*file) << "{" << std::endl;
    (*file) << "\tmemset(&stateVars, 0, sizeof(stateVars));" << std::endl;
    int i = 0;
    for (std::map<std::string, std::string>::iterator it = parser.stateVars.begin();
         it != parser.stateVars.end(); ++it, ++i)
    {
        if (it->second == "")
            it->second = "0";
        (*file) << "\tstateVars[" << i << "] = " << it->second << ";" << std::endl;
    }
    for (std::map<std::string, int>::iterator it = TokenNode::tokenTable.begin();
         it != TokenNode::tokenTable.end(); ++it)
    {
        if (it->first != "empty")
            (*file) << "\t" << "tokenTable[\"" << it->first << "\"] = " << it->second + TokenNode::TOKEN_BASE << ";" << std::endl;
    }
    for (std::deque<Register *>::iterator it = parser.registers.begin();
         it != parser.registers.end(); ++it)
    {
          (*file) << "\t" << "tokenTable[\"" << (*it)->name << "\"] = " << (*it)->id + TokenNode::REGISTER_BASE << ";" << std::endl;
    }
    i = 0;
    for (std::deque<Opcode *>::iterator it = parser.opcodes.begin(); it != parser.opcodes.end(); ++it, ++i)
    {
        if ((*it)->name != "")
        {
            (*file) << "\t" << "opcodeTable[\"" << (*it)->name << "\"] = " << i << ";" << std::endl;
        }
    }
    if (parser.prefixes.size())
    {
        i = 0;
        for (std::deque<Prefix *>::iterator it = parser.prefixes.begin(); it != parser.prefixes.end(); ++it, ++i)
        {
            (*file) << "\t" << "prefixTable[\"" << (*it)->name << "\"] = " << i << ";" << std::endl;
        }
        (*file) << "}" << std::endl;
        (*file) << std::endl;
    }
    return true;
}
bool GenParser::GenerateTokenTables()
{
    int n = (parser.addressClasses.size() + 7) / 8;
    int i = 1;
    for (std::deque<BYTE *>::iterator it = tokenData.begin() ;
             it != tokenData.end(); ++it, ++i)
    {
        (*file) << "unsigned char " << className << "::RegClassData" << i << "[] = {" ;
        for (int j=0; j < n; j++)
        {
            (*file) << (int)((*it)[j]) << ", " ;
        }
        (*file) << "};" << std::endl;
    }
    (*file) << std::endl;
    n = (parser.registers.size() + 7)/ 8;
    for (std::map<std::string, RegClass *>::iterator it = parser.registerClasses.begin();
         it != parser.registerClasses.end(); ++it)
    {
        (*file) << "unsigned char " << className << "::registerData" << it->second->id << "[] = {";
        for (int j=0; j < n; j++)
        {
            (*file) << (int)(it->second->regs[j]) << ", " ;
        }
        (*file) << "};" << std::endl;		
    }
    (*file) << std::endl;
    (*file) << "unsigned char *" << className << "::registerDataIndirect[] = {" << std::endl;
    for (int i=1; i <= parser.registerClasses.size(); i++)
    {
        (*file) << "\tregisterData" << i << "," << std::endl;
    }
    (*file) << "};" << std::endl;
    (*file) << std::endl;
    int *vals = new int[registerTags.size() * parser.registers.size()];
    memset(vals, 0xff, (registerTags.size() * parser.registers.size()) * sizeof(int));
    for (std::deque<Register *>::iterator it = parser.registers.begin();
         it != parser.registers.end(); ++it)
    {
        for (std::map<std::string, std::string>:: iterator it1 = (*it)->values.begin();
             it1 != (*it)->values.end(); ++it1)
        {
            int n = registerTags[it1->first];
            vals[(*it)->id * registerTags.size() + n] = strtol(it1->second.c_str(), NULL, 0);
        }
    }
    (*file) << "int " << className << "::registerValues[][" << registerTags.size() << "] = {" << std::endl;
    for (int k=0; k < parser.registers.size(); k++)
    {
        (*file) << "\t{ ";
        for (int m=0; m < registerTags.size(); m++)
            (*file) << vals[k * registerTags.size() + m] <<", ";
        (*file) << "}," << std::endl;			
    }
    (*file) << "};" << std::endl;
    delete vals;
    (*file) << std::endl;
    return true;
}
void GenParser::GenerateAddressData(TokenNode *value, std::string coding)
{
    switch(value->type)
    {
        case TokenNode::tk_reg:
            (*file) << "\t{" << tokenClassName << "::REGISTER, " << value->reg->id;
            break;
        case TokenNode::tk_regclass:
            (*file) << "\t{" << tokenClassName << "::REGISTERCLASS, " << value->regClass->id-1;
            break;
        case TokenNode::tk_addrclass:
            (*file) << "\t{" << tokenClassName << "::ADDRESSCLASS, " << value->addrClass->id-1;
            break;
        case TokenNode::tk_string:
        {
            if (*value->str == "empty")
            {
                (*file) << "\t{" << tokenClassName << "::EMPTY, " << 0;
            }
            else
            {
                (*file) << "\t{" << tokenClassName << "::TOKEN, " << value->token;
            }
        }
            break;
        case TokenNode::tk_number:
            (*file) << "\t{" << tokenClassName << "::NUMBER, " << value->number->id;
            break;
        default:
            assert(0);
    }
    (*file) << ", " << value->eos;
    (*file) << ", " << value->optionLevel;
    if (value->bytes)
    {	
        (*file) << ", " << className << "::RegClassData" << value->tokenId << ", ";
    }
    else
    {
        (*file) << ", NULL,";
    }
    if (value->values || value->coding != -1 || value->type == TokenNode::tk_regclass || value->type == TokenNode::tk_number)
    {
        (*file) << "&" << className << "::TokenFunc" << value->id << ", ";
    }
    else
    {
        (*file) << " NULL, ";
    }
    if (value->branches.size())
    {
        (*file) << className << "::tokenBranches" << value->id ;
    }
    (*file) << " }," << std::endl;
}
void GenParser::GenerateAddressFuncs(TokenNode *value, std::string coding)
{
    if (value->values || value->coding != -1 || value->type == TokenNode::tk_regclass || value->type == TokenNode::tk_number)
    {
        if (value->values)
        {
            for (std::map<std::string, std::string>::iterator it1 =
                 value->values->begin(); it1 != value->values->end(); ++it1)
            {
                int n = valueTags[it1->first];
                (*file) << "Coding " << className << "::tokenCoding" << value->id << "_" << n << "[] = {" << std::endl;
                GenerateCoding(it1->second);
                (*file) << "};" << std::endl;
            }
        }
        (*file) << "void " << className <<"::TokenFunc" << value->id << "(" << operandClassName << " &operand, int tokenPos)" << std::endl;
        (*file) << "{" << std::endl;
        if (value->coding != -1)
        {
            (*file) << "\toperand." << coding << " = " << value->coding-1 << ";" << std::endl;
        }
        if (value->type == TokenNode::tk_regclass)
        {
            int n = valueTags[value->name];
            (*file) << "\toperand.values[" << n << "] = new Coding[2];" << std::endl;
            (*file) << "\tCleanupValues.push_back(operand.values[" << n << "]);" << std::endl;
            (*file) << "\toperand.values[" << n << "]->type = Coding::reg;" << std::endl;
            (*file) << "\toperand.values[" << n << "]->val = inputTokens[tokenPos]->val->ival;" << std::endl;
            (*file) << "\toperand.values[" << n << "]->bits = 0;" << std::endl;
            (*file) << "\toperand.values[" << n << "]->field = 0;" << std::endl;
            (*file) << "\toperand.values[" << n << "]->math = 0;" << std::endl;
            (*file) << "\toperand.values[" << n << "]->mathval = 0;" << std::endl;
            (*file) << "\toperand.values[" << n << "][1].type = Coding::eot;" << std::endl;
        }
        else if (value->type == TokenNode::tk_number)
        {
            int n = valueTags[value->name];
            (*file) << "\toperand.values[" << n << "] = new Coding[2];" << std::endl;
            (*file) << "\tCleanupValues.push_back(operand.values[" << n << "]);" << std::endl;
            (*file) << "\toperand.values[" << n << "]->type = Coding::number;" << std::endl;
            (*file) << "\toperand.values[" << n << "]->val = operands.size();" << std::endl;
            (*file) << "\toperand.values[" << n << "]->bits = 0;" << std::endl;
            (*file) << "\toperand.values[" << n << "]->field = 0;" << std::endl;
            (*file) << "\toperand.values[" << n << "]->math = 0;" << std::endl;
            (*file) << "\toperand.values[" << n << "]->mathval = 0;" << std::endl;
            (*file) << "\toperand.values[" << n << "][1].type = Coding::eot;" << std::endl;
            (*file) << "\toperands.push_back(numeric);" << std::endl;
        }
        if (value->values)
        {
            for (std::map<std::string, std::string>::iterator it1 =
                 value->values->begin(); it1 != value->values->end(); ++it1)
            {
                int n = valueTags[it1->first];
                (*file) << "\toperand.values[" << n << "] = tokenCoding" << value->id << "_" << n << ";" << std::endl;			
            }
        }
        (*file) << "}" << std::endl;
    }
}
void GenParser::GenerateAddressTokenTree(TokenNode *value)
{
    for (std::deque<TokenNode *>::iterator it = value->branches.begin();
         it != value->branches.end(); it++)
    {
        GenerateAddressTokenTree(*it);
    }
    for (std::deque<TokenNode *>::iterator it = value->branches.begin();
         it != value->branches.end(); it++)
    {
        GenerateAddressFuncs(*it, "addressCoding");
    }
    if (value->branches.size())
    {
        (*file) << tokenClassName << " " << className << "::tokenBranches" << value->id << "[] = {" << std::endl;
        for (std::deque<TokenNode *>::iterator it = value->branches.begin();
             it != value->branches.end(); it++)
        {
            if ((*it)->type == TokenNode::tk_reg)
            {
                GenerateAddressData((*it), "addressCoding");
            }
        }
        for (std::deque<TokenNode *>::iterator it = value->branches.begin();
             it != value->branches.end(); it++)
        {
            if ((*it)->type != TokenNode::tk_reg)
            {
                GenerateAddressData((*it), "addressCoding");
            }
        }
        (*file) << "\t{" << tokenClassName << "::EOT }" << std::endl;
        (*file) << "};" << std::endl;
    }
         
}
bool GenParser::GenerateAddresses()
{
    GenerateAddressTokenTree(parser.addressRoot);
    return true;
}
void GenParser::GenerateOperandTokenTree(TokenNode *value)
{
    for (std::deque<TokenNode *>::iterator it = value->branches.begin();
         it != value->branches.end(); it++)
    {
        GenerateOperandTokenTree(*it);
    }
    for (std::deque<TokenNode *>::iterator it = value->branches.begin();
         it != value->branches.end(); it++)
    {
        GenerateAddressFuncs(*it, "operandCoding");
    }
    if (value->branches.size())
    {
        (*file) << tokenClassName << " " << className << "::tokenBranches" << value->id << "[] = {" << std::endl;
        for (std::deque<TokenNode *>::iterator it = value->branches.begin();
             it != value->branches.end(); it++)
        {
            GenerateAddressData((*it), "operandCoding");
        }
        (*file) << "\t{" << tokenClassName << "::EOT }" << std::endl;
        (*file) << "};" << std::endl;
    }
         
}
bool GenParser::GenerateOperands()
{
    for (std::deque<Opcode *>::iterator it = parser.opcodes.begin();
         it != parser.opcodes.end(); ++it)
    {
        GenerateOperandTokenTree((*it)->tokenRoot);
    }
    return true;
}
bool GenParser::GenerateOpcodes()
{
    for (std::deque<Opcode *>::iterator it = parser.opcodes.begin();
         it != parser.opcodes.end(); ++it)
    {
        for (std::map<std::string, std::string> :: iterator it1 = (*it)->values.begin();
             it1 != (*it)->values.end(); ++ it1)
        {
            int n = valueTags[it1->first];
            (*file) << "Coding " << className << "::OpcodeCodings" << (*it)->id << "_" << n << "[] = {" << std::endl;
            GenerateCoding(it1->second);
            (*file) << "};" << std::endl;
        }
        (*file) << "bool "<<className<<"::Opcode"<<(*it)->id<<"(" << operandClassName << " &operand)" << std::endl;
        (*file) << "{" << std::endl;
        for (std::map<std::string, std::string> :: iterator it1 = (*it)->values.begin();
             it1 != (*it)->values.end(); ++ it1)
        {
            int n = valueTags[it1->first];
            (*file) << "\toperand.values[" << n << "] = OpcodeCodings" << (*it)->id << "_" << n << ";" << std::endl;			
        }
        if ((*it)->tokenRoot->branches.size())
        {
            (*file) << "\tbool rv = ParseOperands(tokenBranches" << (*it)->tokenRoot->id << ", operand);" << std::endl;
        }
        if ((*it)->name != "" && (*it)->cclass != "")
        {
            Opcode *op = parser.opcodeClasses[(*it)->cclass];
            if (op)
            {
                if ((*it)->operands.size())
                {
                    (*file) << "\tif (!rv)" << std::endl;
                }
                else
                {
                    (*file) << "\tbool rv;" << std::endl;
                }
                    
                (*file) << "\t{" << std::endl;
                (*file) << "\t\trv = Opcode" << op->id << "(operand);" << std::endl;
                (*file) << "\t}" << std::endl;
            }
            else
            {
                std::cout << "Undefined opcode class '" << (*it)->cclass <<
                               "' in definition of opcode '" << (*it)->name << "'";
            }
        }
        else
        {
            if (!(*it)->operands.size())
                (*file) << "\tbool rv = true;" << std::endl;
        }
        (*file) << "\treturn rv;" << std::endl;
        (*file) << "}" << std::endl;
    }	
    (*file) << className<<"::DispatchType "<<className<<"::DispatchTable["<<parser.opcodes.size()<<"] = {" << std::endl;
    for (std::deque<Opcode *>::iterator it = parser.opcodes.begin();
         it != parser.opcodes.end(); ++it)
    {
        if ((*it)->name == "")
            (*file) << "\tNULL," << std::endl;
        else
            (*file) << "\t&" << className << "::Opcode" << (*it)->id << "," << std::endl;
    }	
    (*file) << "};" << std::endl;
    const std::string **codingNames = new const std::string *[parser.codings.size()];
    for (std::map<std::string,int>::iterator it = parser.codings.begin();
         it != parser.codings.end(); ++it)
    {
        codingNames[it->second-1] = &it->first;
    }
    for (int i=0; i < parser.codings.size(); i++)
    {
        (*file) << "Coding " << className << "::Coding" << i + 1 << "[] = {" << std::endl;
        GenerateCoding(*codingNames[i]);
        (*file) << "};" << std::endl;
    }
    (*file) <<"Coding * "<<className<<"::Codings["<<parser.codings.size()<<"] = {" << std::endl;
    for (int i=0; i < parser.codings.size(); i++)
            (*file) << "\t" << className << "::Coding" << i+1 << "," << std::endl;
    (*file) << "};" << std::endl;
    if (parser.prefixes.size())
    {
        int i = 1;
        for (std::deque<Prefix *>::iterator it = parser.prefixes.begin();
             it != parser.prefixes.end(); ++it, ++i)
        {
            (*file) << "Coding " << className << "::prefixCoding" << i << "[] = {" << std::endl;
            GenerateCoding((*it)->coding);
            (*file) << "};" << std::endl;
        }
        (*file) << "Coding *" << className << "::prefixCodings" << "[] = {" << std::endl;
        i = 1;
        for (std::deque<Prefix *>::iterator it = parser.prefixes.begin();
             it != parser.prefixes.end(); ++it, ++i)
        {
            (*file) << "\t" << className << "::prefixCoding" << i << "," << std::endl;
        }
        (*file) << "};";
        (*file) << std::endl;
    }
    delete codingNames;
    return true;
}
void GenParser::GenerateCoding(const std::string coding) 
{
    std::string temp = coding;
    while (temp.size())
    {
        int npos = temp.find_first_not_of(" ,\t\v\r\n");
        if (npos == std::string::npos)
        {
            temp = "";
        }
        else
        {
            if (npos != 0)
                temp = temp.substr(npos);
            if (isdigit(temp[0]))
            {
                int n = 0;
                char *pch;
                int bits = -1;
                int val = strtol(temp.c_str(), &pch, 0);
                n = pch - temp.c_str();
                if (temp[n] == ':')
                {
                    bits = strtol(temp.c_str() + n + 1, &pch, 0);
                    if (pch - temp.c_str() == temp.size())
                        temp = "";
                    else
                        temp = temp.substr(pch - temp.c_str());
                }
                else
                {
                    if (n == temp.size())
                        temp = "";
                    else
                        temp = temp.substr(n);
                }
                if (bits != -1)
                {
                    (*file) << "\t{ (Coding::Type)(Coding::bitSpecified | Coding::valSpecified), " << val << ", " << bits << "}," << std::endl;
                }
                else
                {
                    (*file) << "\t{ Coding::valSpecified, " << val << "}," << std::endl;
                }
            }
            else if (temp[0] == '\'')
            {
                int n = 1;
                char *pch;
                while (isalnum(temp[n]))
                    n++;
                std::string name = temp.substr(1, n-1);
                std::map<std::string, int>::iterator itst = stateFuncTags.find(name);
                if (itst != stateFuncTags.end())
                {
                    if (n == temp.size() || temp[n] != '\'')
                    {
                        std::cout << "Error { " << coding << " } missing close quote " << std::endl;
                        if (n == temp.size())
                            temp = "";
                        else
                            temp = temp.substr(n);
                    }
                    else
                    {
                        if (n+1 == temp.size())
                            temp = "";
                        else
                            temp = temp.substr(n+1);
                        (*file) << "\t{ Coding::stateFunc, " << itst->second << " }," << std::endl;
                    }
                }
                else
                {
                    std::map<std::string, int>::iterator itst = stateVarTags.find(name);
                    if (itst != stateVarTags.end())
                    {
                        if (n == temp.size() || temp[n] != '\'')
                        {
                            std::cout << "Error { " << coding << " } missing close quote " << std::endl;
                            if (n == temp.size())
                                temp = "";
                            else
                                temp = temp.substr(n);
                        }
                        else
                        {
                            if (n+1 == temp.size())
                                temp = "";
                            else
                                temp = temp.substr(n+1);
                            (*file) << "\t{ Coding::stateVar, " << itst->second << " }," << std::endl;
                        }
                    }
                    else
                    {
                        std::string field;
                        if (temp[n] == '.')
                        {
                            int m = ++n;
                            while (isalnum(temp[n]))
                                n++;
                            field = temp.substr(m, n-m);
                        }
                        if (n == temp.size() || temp[n] != '\'')
                        {
                            std::cout << "Error { " << coding << " } missing close quote " << std::endl;
                            if (n == temp.size())
                                temp = "";
                            else
                                temp = temp.substr(n);
                        }
                        else
                        {
                            n++;
                            int sel = 0;
                            switch (temp[n])
                            {
                                case '!':
                                case '~':
                                case '+':
                                case '-':
                                case '&':
                                case '|':
                                case '^':
                                    sel = temp[n];
                                    n++;
                                    break;
                                case '>':
                                    if (temp[n+1] == '>')
                                    {
                                        sel = temp[n];
                                        n+=2;
                                    }
                                    break;
                                case '<':
                                    if (temp[n+1] == '<')
                                    {
                                        sel = temp[n];
                                        n+=2;
                                    }
                                    break;
                                default:
                                    break;
                            }
                            int selval = 0;
                            if (sel != 0 && sel != '!' && sel != '~')
                            {
                                char *pch;
                                selval = strtol(temp.c_str() + n, &pch, 0);
                                n = pch - temp.c_str();
                            }
                            int n1 = 0;
                            char *pch;
                            int bits = -1;
                            if (temp[n] == ':')
                            {
                                bits = strtol(temp.c_str() + n + 1, &pch, 0);
                                if (pch - temp.c_str() == temp.size())
                                    temp = "";
                                else
                                    temp = temp.substr(pch - temp.c_str());
                            }
                            else
                            {
                                if (n == temp.size())
                                    temp = "";
                                else
                                    temp = temp.substr(n);
                            }
                            int p=0,f=0;
                            std::map<std::string, int> :: iterator itp = valueTags.find(name);
                            if (itp == valueTags.end())
                            {
                                std::cout << "Error in coding: unknown variable '" << name << "'" << temp.substr(0, npos) << std::endl;
                            }
                            else
                            {
                                p = itp->second;
                            }
                            if (field != "")
                            {
                                std::map<std::string, int> :: iterator itf = registerTags.find(field);
                                if (itf == registerTags.end())
                                {
                                    std::cout << "Error in coding: unknown variable field '" << field << "'" << temp.substr(0, npos) << std::endl;
                                }
                                else
                                {
                                    f = itf->second;
                                }
                            }
                            if (bits == -1 && field.size() == 0)
                            {
                                (*file) << "\t{ Coding::indirect, " << p << ", -1, 0";
                            }
                            else if (bits == -1 && field.size())
                            {
                                (*file) << "\t{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified), " << p << ", " << bits << ", " << f;
                            }
                            else if (bits != -1 && field.size() == 0)
                            {
                                (*file) << "\t{ (Coding::Type)(Coding::indirect | Coding::bitSpecified), " << p << ", " << bits << ", 0";
                            }
                            else
                            {
                                (*file) << "\t{ (Coding::Type)(Coding::indirect | Coding::fieldSpecified | Coding::bitSpecified), " << p << ", " << bits << ", " << f;
                            }
                            if (sel)
                            {
                                (*file) << ", " << sel << ", " << selval;
                            }
                            (*file) << " }," << std::endl;
                        }
                    }
                }
            }
            else
            {
                if (temp.substr(0, 6) == "native")
                {
                    (*file) << "\t{ Coding::native " << "}," << std::endl;
                    if (temp.size() == 6)
                        temp = "";
                    else
                        temp = temp.substr(6);
                }
                else if (temp.substr(0, 7) == "illegal")
                {
                    (*file) << "\t{ Coding::illegal " << "}," << std::endl;
                    if (temp.size() == 7)
                        temp = "";
                    else
                        temp = temp.substr(7);
                }
                else
                {
                    npos = temp.find_first_of(", \t\r\v\n");
                    std::cout << "Error { " << coding << " } unknown token sequence " << temp.substr(0, npos) << std::endl;
                    if (npos == std::string::npos)
                        temp = "";
                    else
                        temp = temp.substr(npos);
                }
            }	
        }
    }
    (*file) << "\t{ Coding::eot " << "}," << std::endl;
}
bool GenParser::GenerateUtilityFuncs()
{
    (*file) << "bool " << className << "::MatchesToken(int token, int tokenPos)" << std::endl;
    (*file) << "{" << std::endl;
    (*file) << "\treturn tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::TOKEN && inputTokens[tokenPos]->val->ival == token;" << std::endl;
    (*file) << "}" << std::endl;
    (*file) << "\tbool " << className << "::MatchesRegister(int reg, int tokenPos)" << std::endl;
    (*file) << "{" << std::endl;
    (*file) << "\treturn tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::REGISTER && inputTokens[tokenPos]->val->ival == reg;" << std::endl;
    (*file) << "}" << std::endl;
    (*file) << "\tbool " << className << "::MatchesRegisterClass(int cclass, int tokenPos)" << std::endl;
    (*file) << "{" << std::endl;
    (*file) << "\treturn tokenPos < inputTokens.size() && inputTokens[tokenPos]->type == InputToken::REGISTER && (registerDataIndirect[cclass][inputTokens[tokenPos]->val->ival >> 3] & (1 << (inputTokens[tokenPos]->val->ival & 7)));" << std::endl;
    (*file) << "}" << std::endl;
    return true;
}
bool GenParser::GenerateAddressParser()
{
    (*file) << "bool " << className << "::ParseAddresses("<<operandClassName<<" &operand, int addrClass, int &tokenPos)" << std::endl;
    (*file) << "{" << std::endl;
    (*file) << "\tint level = 0;" << std::endl;
    (*file) << "\tbool rv = false;" << std::endl;
    (*file) << "\t" << tokenClassName << " *t = tokenBranches1;" << std::endl;
    (*file) << "\twhile (t->type != " << tokenClassName << "::EOT)" << std::endl;
    (*file) << "\t{" << std::endl;
    (*file) << "\t\tbool matches = false;" << std::endl;
    (*file) << "\t\tif (t->addrClass[addrClass >> 3] & (1 << (addrClass & 7)))" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tswitch (t->type)" << std::endl;
    (*file) << "\t\t\t{" << std::endl;
    (*file) << "\t\t\t\tcase " << tokenClassName << "::EMPTY:" << std::endl;
    (*file) << "\t\t\t\t\tmatches = true;" << std::endl;
    (*file) << "\t\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\t\tcase " << tokenClassName << "::TOKEN:" << std::endl;
    (*file) << "\t\t\t\t\tmatches = MatchesToken(t->id, tokenPos);" << std::endl;
    (*file) << "\t\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\t\tcase " << tokenClassName << "::REGISTER:" << std::endl;
    (*file) << "\t\t\t\t\tmatches = MatchesRegister(t->id, tokenPos);" << std::endl;
    (*file) << "\t\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\t\tcase " << tokenClassName << "::REGISTERCLASS:" << std::endl;
    (*file) << "\t\t\t\t\tmatches = MatchesRegisterClass(t->id, tokenPos);" << std::endl;
    (*file) << "\t\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\t\tcase " << tokenClassName << "::NUMBER:" << std::endl;
    (*file) << "\t\t\t\t\tif (tokenPos < inputTokens.size())" << std::endl;
    (*file) << "\t\t\t\t\t\tmatches = (this->*numberFuncs[t->id])(tokenPos);" << std::endl;
    (*file) << "\t\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\t\tdefault:" << std::endl;
    (*file) << "\t\t\t\t\tmatches = false;" << std::endl;
    (*file) << "\t\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\t}" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\tif (!matches)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tif (t->level > level)" << std::endl;
    (*file) << "\t\t\t{" << std::endl;
    (*file) << "\t\t\t\tt = t->next;" << std::endl;
    (*file) << "\t\t\t\tcontinue;" << std::endl;
    (*file) << "\t\t\t}" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse" << std::endl;
    (*file) << "\t\t\tlevel = t->level;" << std::endl;
    (*file) << "\t\tif (matches)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tif (t->tokenFunc)" << std::endl;
    (*file) << "\t\t\t\t(this->*(t->tokenFunc))(operand, tokenPos);" << std::endl;
    (*file) << "\t\t\tif (t->eos && (tokenPos == inputTokens.size()-1 || !t->next))" << std::endl;
    (*file) << "\t\t\t{" << std::endl;
    (*file) << "\t\t\t\trv = true;" << std::endl;
    (*file) << "\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\t}" << std::endl;
    (*file) << "\t\t\tt = t->next;" << std::endl;
    (*file) << "\t\t\ttokenPos++;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tt++;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t}" << std::endl;
    (*file) << "\treturn rv;" << std::endl;
    (*file) << "}" << std::endl;
    (*file) << std::endl;
    return true;
}
bool GenParser::GenerateOperandParser()
{
    (*file) << "bool " << className << "::ParseOperands2(" << tokenClassName << " *tokenList, "<<operandClassName<<" &operand, int tokenPos, int level)" << std::endl;
    (*file) << "{" << std::endl;
    (*file) << "\tbool rv = false;" << std::endl;
    (*file) << "\t" << tokenClassName << " *t = tokenList;" << std::endl;
    (*file) << "\twhile (t && t->type != " << tokenClassName << "::EOT)" << std::endl;
    (*file) << "\t{" << std::endl;
    (*file) << "\t\tbool matches = false;" << std::endl;
    (*file) << "\t\tint last = tokenPos;" << std::endl;
    (*file) << "\t\tswitch (t->type)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tcase " << tokenClassName << "::EMPTY:" << std::endl;
    (*file) << "\t\t\t\tmatches = true;" << std::endl;
    (*file) << "\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\tcase " << tokenClassName << "::TOKEN:" << std::endl;
    (*file) << "\t\t\t\tmatches = MatchesToken(t->id, tokenPos);" << std::endl;
    (*file) << "\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\tcase " << tokenClassName << "::REGISTER:" << std::endl;
    (*file) << "\t\t\t\tmatches = MatchesRegister(t->id, tokenPos);" << std::endl;
    (*file) << "\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\tcase " << tokenClassName << "::REGISTERCLASS:" << std::endl;
    (*file) << "\t\t\t\tmatches = MatchesRegisterClass(t->id,tokenPos);" << std::endl;
    (*file) << "\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\tcase " << tokenClassName << "::ADDRESSCLASS:" << std::endl;
    (*file) << "\t\t\t\tmatches = ParseAddresses(operand, t->id, tokenPos);" << std::endl;
    (*file) << "\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\tcase " << tokenClassName << "::NUMBER:" << std::endl;
    (*file) << "\t\t\t\tif (tokenPos < inputTokens.size())" << std::endl;
    (*file) << "\t\t\t\t\tmatches = (this->*numberFuncs[t->id])(tokenPos);" << std::endl;
    (*file) << "\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\tdefault:" << std::endl;
    (*file) << "\t\t\t\tmatches = false;" << std::endl;
    (*file) << "\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\tif (!matches)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tif (t->level > level)" << std::endl;
    (*file) << "\t\t\t{" << std::endl;
    (*file) << "\t\t\t\tif (ParseOperands2(t->next, operand, tokenPos, t->level))" << std::endl;
    (*file) << "\t\t\t\t{" << std::endl;
    (*file) << "\t\t\t\t\trv = true;" << std::endl;
    (*file) << "\t\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\t\t}" << std::endl;
    (*file) << "\t\t\t}" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tif (t->tokenFunc)" << std::endl;
    (*file) << "\t\t\t\t(this->*(t->tokenFunc))(operand, last);" << std::endl;
    (*file) << "\t\t\tif (t->eos && (!t->next || tokenPos == inputTokens.size()-1))" << std::endl;
    (*file) << "\t\t\t{" << std::endl;
    (*file) << "\t\t\t\tif (tokenPos >= (int)(inputTokens.size()-1))" << std::endl;
    (*file) << "\t\t\t\t\teol = true;" << std::endl;
    (*file) << "\t\t\t\trv = true;" << std::endl;
    (*file) << "\t\t\t\tbreak;" << std::endl;
    (*file) << "\t\t\t}" << std::endl;
    (*file) << "\t\t\tif (tokenPos < inputTokens.size() && ParseOperands2(t->next, operand, tokenPos+1, t->level))" << std::endl;	
    (*file) << "\t\t\t{" << std::endl;	
    (*file) << "\t\t\t\trv = true;" << std::endl;	
    (*file) << "\t\t\t\tbreak;" << std::endl;	
    (*file) << "\t\t\t}" << std::endl;	
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\ttokenPos = last;" << std::endl;
    (*file) << "\t\tt++;" << std::endl;
    (*file) << "\t}" << std::endl;
    (*file) << "\treturn rv;" << std::endl;
    (*file) << "}" << std::endl;
    (*file) << std::endl;
    (*file) << "bool " << className << "::ParseOperands(" << tokenClassName << " *tokenList, "<<operandClassName<<" &operand)" << std::endl;
    (*file) << "{" << std::endl;
    (*file) << "\treturn ParseOperands2(tokenList, operand, 0, 0);" << std::endl;
    (*file) << "}" << std::endl;
    (*file) << std::endl;	
    return true;
}
bool GenParser::GenerateCodingProcessor()
{
    (*file) << "bool " << className << "::ProcessCoding(CodingHelper &base, "<<operandClassName<<" &operand, Coding *coding)" << std::endl;
    (*file) << "{" << std::endl;
    (*file) << "\tCodingHelper current;" << std::endl;
    (*file) << std::endl;
    (*file) << "\twhile (coding->type != Coding::eot)" << std::endl;
    (*file) << "\t{" << std::endl;
    (*file) << "\t\tcurrent = base;" << std::endl;
    (*file) << "\t\tif (coding->type & Coding::bitSpecified)" << std::endl;
    (*file) << "\t\t\tcurrent.bits = coding->bits;" << std::endl;
    (*file) << "\t\tif (coding->type & Coding::fieldSpecified)" << std::endl;
    (*file) << "\t\t\tcurrent.field = coding->field;" << std::endl;
    (*file) << "\t\tif (coding->math)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tcurrent.math = coding->math;" << std::endl;
    (*file) << "\t\t\tcurrent.mathval = coding->mathval;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\tif (coding->type & Coding::valSpecified)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tint n = current.DoMath(coding->val);" << std::endl;
    (*file) << "\t\t\tbits.Add(n, current.bits);" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse if (coding->type & Coding::reg)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tint n = coding->val;" << std::endl;
    (*file) << "\t\t\tif (current.field != -1)" << std::endl;
    (*file) << "\t\t\t\tn = registerValues[n][current.field];" << std::endl;
    (*file) << "\t\t\tn = current.DoMath(n);" << std::endl;
    (*file) << "\t\t\tbits.Add(n, current.bits);" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse if (coding->type & Coding::stateFunc)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tCoding *c = (this->*stateFuncs[coding->val])();" << std::endl;
    (*file) << "\t\t\tif (!ProcessCoding(current, operand,c))" << std::endl;
    (*file) << "\t\t\t\treturn false;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse if (coding->type & Coding::stateVar)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tint n = current.DoMath(stateVars[coding->val]);" << std::endl;
    (*file) << "\t\t\tbits.Add(n, current.bits);" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse if (coding->type & Coding::number)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tint n = coding->val;" << std::endl;
    (*file) << "\t\t\tstd::deque<Numeric *>::iterator it = operands.begin();" << std::endl;
    (*file) << "\t\t\tfor (int i=0; i < n; i++)" << std::endl;
    (*file) << "\t\t\t{" << std::endl;
    (*file) << "\t\t\t\t++it;" << std::endl;
    (*file) << "\t\t\t}" << std::endl;
    (*file) << "\t\t\t(*it)->used = true;" << std::endl;
    (*file) << "\t\t\t(*it)->pos = this->bits.GetBits();" << std::endl;
    (*file) << "\t\t\tbits.Add((*it)->node->ival, current.bits);" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse if (coding->type & Coding::native)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tif (operand.addressCoding == -1)" << std::endl;
    (*file) << "\t\t\t\treturn false;" << std::endl;
    (*file) << "\t\t\tif (!ProcessCoding(current, operand, Codings[operand.addressCoding]))" << std::endl;
    (*file) << "\t\t\t\treturn false;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse if (coding->type & Coding::indirect)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tif (!operand.values[coding->val])" << std::endl;
    (*file) << "\t\t\t\treturn false;" << std::endl;
    (*file) << "\t\t\tif (!ProcessCoding(current, operand, operand.values[coding->val]))" << std::endl;
    (*file) << "\t\t\t\treturn false;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse if (coding->type & Coding::illegal)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\treturn false;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\treturn false;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\tcoding++;" << std::endl;
    (*file) << "\t}" << std::endl;
    (*file) << "\treturn true;" << std::endl;
    (*file) << "}" << std::endl;
    (*file) << std::endl;
    return true;
}
bool GenParser::GenerateDispatcher()
{
    (*file) << "bool "<<className<<"::DispatchOpcode(int opcode)" << std::endl;
    (*file) << "{" << std::endl;
    (*file) << "\tbool rv;" << std::endl;
    (*file) << "\tif (opcode == -1)" << std::endl;
    (*file) << "\t{" << std::endl;
    (*file) << "\t\trv = true;" << std::endl;
    (*file) << "\t\t" << operandClassName << " operand;" << std::endl;
    (*file) << "\t\tCodingHelper base;" << std::endl;
    (*file) << "\t\tfor (std::deque<int>::iterator it = prefixes.begin(); rv && it !=prefixes.end(); ++it)" << std::endl;
    (*file) << "\t\t\trv &= ProcessCoding(base, operand, prefixCodings[*it]);" << std::endl;
    (*file) << "\t}" << std::endl;
    (*file) << "\telse" << std::endl;
    (*file) << "\t{" << std::endl;
    (*file) << "\t\t" << operandClassName << " operand;" << std::endl;
    (*file) << "\t\toperand.opcode = opcode;" << std::endl;
    (*file) << "\t\trv = (this->*DispatchTable[opcode])(operand);" << std::endl;
    (*file) << "\t\tif (rv)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tCodingHelper base;" << std::endl;
    if (parser.prefixes.size())
    {
        (*file) << "\t\t\tfor (std::deque<int>::iterator it = prefixes.begin(); rv && it !=prefixes.end(); ++it)" << std::endl;
        (*file) << "\t\t\t\trv &= ProcessCoding(base, operand, prefixCodings[*it]);" << std::endl;
        (*file) << "\t\t\tif (rv)" << std::endl;
    }
    (*file) << "\t\t\t{" << std::endl;
    (*file) << "\t\t\t\tif (operand.operandCoding != -1)" << std::endl;
    (*file) << "\t\t\t\t\trv = ProcessCoding(base, operand, Codings[operand.operandCoding]);" << std::endl;
    (*file) << "\t\t\t\telse if (operand.addressCoding != -1)" << std::endl;
    (*file) << "\t\t\t\t\trv = ProcessCoding(base, operand, Codings[operand.addressCoding]);" << std::endl;
    (*file) << "\t\t\t\telse rv = false;" << std::endl;
    (*file) << "\t\t\t}" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t}" << std::endl;
    (*file) << "\treturn rv;" << std::endl;
    (*file) << "}" << std::endl;
    return true;
}
