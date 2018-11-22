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
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <iostream>

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
    for (auto &x : values)
    {
        std::map<std::string, int>::iterator itm = valueTags.find(x.first);
        if (itm == valueTags.end())
        {
            valueTags[x.first] = valueTags.size();
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
                    std::map<std::string, int>::iterator itm = valueTags.find(var);
                    if (itm == valueTags.end())
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
    for (auto x : parser.addresses)
    {
        GatherVars(x->values, x->name);
    }
    for (auto x : parser.operands)
    {
        GatherVars(x->values, x->name);
    }
    for (auto x : parser.opcodes)
    {
        GatherVars(x->values, "");
    }
    for (auto x : TokenNode::tokenList)
    {
        if (x->name != "")
        {
            std::map<std::string, int>::iterator itm = valueTags.find(x->name);
            if (itm == valueTags.end())
            {
                valueTags[x->name] = valueTags.size();
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
    for (auto &x : parser.stateVars)
    {
        stateVarTags[x.first] = i;
        (*file) << "\tvoid Set" << x.first << "(int v) { stateVars[" << i << "] = v; }" << std::endl;
        (*file) << "\tint Get" << x.first << "() { return stateVars[" << i << "]; }" << std::endl;

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
    (*file) << "\tbool ProcessCoding("<<operandClassName<<" &operand, Coding *coding);" << std::endl;
    (*file) << "\tbool ProcessCoding(" << operandClassName << " &operand, Coding *coding, int field, int bits, int* arr, char* bitcounts, char *func, int &index);" << std::endl;
    (*file) << "\tbool MatchesToken(int token, int tokenPos);" << std::endl;
    (*file) << "\tbool MatchesRegister(int reg, int tokenPos);" << std::endl;
    (*file) << "\tbool MatchesRegisterClass(int cclass, int tokenPos);" << std::endl;
    (*file) << "\tint DoMath(char op, int left, int right);" << std::endl;
    (*file) << "\tvirtual bool DispatchOpcode(int opcode);" << std::endl;
    (*file) << std::endl;	
    (*file) << "\ttypedef bool ("<<className<<"::*DispatchType)(" << operandClassName << " &);" << std::endl;
    (*file) << "\tstatic DispatchType DispatchTable[];" << std::endl;
    (*file) << std::endl;
    (*file) << "\ttypedef Coding * (" << className << "::*StateFuncDispatchType)();" << std::endl;
    (*file) << "\tstatic StateFuncDispatchType stateFuncs[];" << std::endl;
    i = 1;
    for (auto x : parser.states)
    {
        stateFuncTags[x->name] = stateFuncTags.size();
        (*file) << "\tCoding *StateFunc" << i << "();" << std::endl;
        ++i;
    }
    (*file) << std::endl;
    (*file) << "private:" << std::endl;
    (*file) << "\tint stateVars[" << parser.stateVars.size() << "];" << std::endl;
    (*file) << std::endl;
    i = 1;
    (*file) << "\tstatic Coding stateCoding_eot[];" << std::endl;
    for (auto x : parser.states)
    {
        int j = 1;
        for (auto& m : x->clauses)
        {
            (*file) << "\tstatic Coding stateCoding" << i << "_" << j << "[];" << std::endl;
            ++j;
        }
        ++i;
    }
    (*file) << std::endl;
    (*file) << "\ttypedef bool (" << className << "::*NumberDispatchType)(int tokenPos);" << std::endl;
    (*file) << "\tstatic NumberDispatchType numberFuncs[];" << std::endl;
    for (auto x : parser.numbers)
    {
        (*file) << "\tbool Number" << x->id + 1 << "(int tokenPos);" << std::endl;
    }
    (*file) << std::endl;
    int nextToken = 1;
    int n = (parser.addressClasses.size() + 7)/8;
    for (auto t : TokenNode::tokenList)
    {
        if (t->used && t->bytes)
        {
            int x = nextToken;
            int i = 1;
            for (auto x1 : tokenData)
            {
                if (!memcmp(x1, t->bytes, n))
                {
                    x = i;
                    break;
                }
                ++i;
            }
            if (x == nextToken)
            {
                (*file) << "\tstatic unsigned char RegClassData" << x << "[];" <<std::endl;
                nextToken++;
                tokenData.push_back(t->bytes);
            }
            t->tokenId = x;
        }
    }
    (*file) << std::endl;
    for (auto& m : parser.registerClasses)
    {
        (*file) << "\tstatic unsigned char registerData" << m.second->id << "[];" << std::endl;
    }
    (*file) << "\tstatic unsigned char *registerDataIndirect[];" << std::endl;
    (*file) << std::endl;
    for (auto x : parser.registers)
    {
        for (auto& m : x->values)
        {
            std::map<std::string, int>::iterator itm = registerTags.find(m.first);
            if (itm == registerTags.end())
            {
                registerTags[m.first] = registerTags.size();
            }
        }
    }
    (*file) << "\tstatic int registerValues[][" << registerTags.size() << "];" << std::endl;
    (*file) << std::endl;
    for (auto x : TokenNode::tokenList)
    {
        if (x->used)
        {
            std::map<std::string, std::string> *vals = NULL;
            if (x->values || x->coding != -1 || x->type == TokenNode::tk_regclass || x->type == TokenNode::tk_number)
            {
                if (x->values)
                {
                    for (auto& m : *x->values)
                    {
                        int n = valueTags[m.first];
                        (*file) << "\tstatic Coding tokenCoding" << x->id << "_" << n << "[];" << std::endl;
                    }
                }
                (*file) << "\tvoid TokenFunc" << x->id << "(" << operandClassName << " &operand, int tokenPos);" << std::endl;
            }
            if (x->branches.size())
            {
                (*file) << "\tstatic " << tokenClassName << " tokenBranches" << x->id << "[];" << std::endl;
            }
        }
    }
    (*file) << std::endl;	
    for (auto x : parser.opcodes)
    {
        for (auto& m : x->values)
        {
            int n = valueTags[m.first];
            (*file) << "\tstatic Coding OpcodeCodings" << x->id << "_" << n << "[];" << std::endl;
        }
        (*file) << "\tbool Opcode" << x->id << "(" << operandClassName << " &operand);" << std::endl;
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
        for (auto x : parser.prefixes)
        {
            (*file) << "\tstatic Coding prefixCoding" << i << "[];" << std::endl;
            i++;
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
    for (auto x : parser.states)
    {
        int j = 1;
        for (auto& m : x->clauses)
             
        {
            (*file) << "Coding " << className << "::stateCoding" << i << "_" << j << "[] = {" << std::endl;
            GenerateCoding(m.second);
            (*file) << "};" << std::endl;
            ++j;
        }
        i++;
    }
    i = 1;
    for (auto x : parser.states)
    {
        (*file) << "Coding *" << className << "::StateFunc" << i << "()" << std::endl;
        (*file) << "{" << std::endl;
        int j = 1;
        for (auto& m : x->clauses)
        {
            std::string sel = m.first;
            int npos = sel.find_first_of("'");
            while (npos != std::string::npos)
            {
                int n1 = sel.find_first_of("'", npos+1);
                if (n1 == std::string::npos)
                {
                    std::cout << "Error { " << m.first << "} Invalid when clause in state" << std::endl;
                    break;
                }
                std::string val = sel.substr(npos + 1, n1 - npos - 1);
                std::map<std::string, int>:: iterator itm = stateVarTags.find(val);
                if (itm == stateVarTags.end())
                {
                    std::cout << "Error { " << m.first << " } Unknown state var '" << val << "'" << std::endl;
                    break;
                }
                else
                {
                    std::stringstream t ;
                    t << "stateVars[" << itm->second << "]";
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
            ++j;
        }
        (*file) << "\treturn stateCoding_eot;" << std::endl;
        (*file) << "}" << std::endl;
        ++i;
    }
    (*file) << className << "::StateFuncDispatchType " << className << "::stateFuncs[] = {" << std::endl;
    i = 1;
    for (auto x : parser.states)
    {
        (*file) << "\t&" << className << "::StateFunc" << i << "," << std::endl;
        ++i;
    }
    (*file) << "};" << std::endl;
    for (auto x : parser.numbers)
    {
        (*file) << "bool " << className << "::Number" << x->id + 1 << "(int tokenPos)" << std::endl;
        (*file) << "{" << std::endl;
        (*file) << "\tbool rv = false;" << std::endl;
        for (auto &x1: x->values)
        {
            std::string temp = x1;
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
                    std::map<std::string, int>:: iterator itm = stateVarTags.find(val);
                    if (itm == stateVarTags.end())
                    {
                        std::cout << "Error { " << err << " } Unknown state var '" << val << "'" << std::endl;
                        break;
                    }
                    else
                    {
                        std::stringstream t ;
                        t << "stateVars[" << itm->second << "]";
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
                    (*file) << "\t\trv = ParseLabel(" << x->relOfs << ", " << (xsigned ? 1 : 0) << ", " << bits << ");" << std::endl;
                }
                else
                */
                {
                    (*file) << "\t\trv = ParseNumber(" << x->relOfs << ", " << (xsigned ? 1 : 0) << ", " << bits << ", " << (label ? 0 : 1) << ", tokenPos);" << std::endl;
                }
                (*file) << "\t}" << std::endl;
            }
        }
        (*file) << "\treturn rv;" << std::endl;
        (*file) << "}" << std::endl;
    }
    (*file) << className << "::NumberDispatchType " << className << "::numberFuncs[] = {" << std::endl;
    for (auto x : parser.numbers)
    {
        (*file) << "\t&" << className << "::Number" << x->id + 1 << "," << std::endl;
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
    for (auto& m :  parser.stateVars)
    {
        if (m.second == "")
            m.second = "0";
        (*file) << "\tstateVars[" << i << "] = " << m.second << ";" << std::endl;
        ++i;
    }
    for (auto& m : TokenNode::tokenTable)
    {
        if (m.first != "empty")
            (*file) << "\t" << "tokenTable[\"" << m.first << "\"] = " << m.second + TokenNode::TOKEN_BASE << ";" << std::endl;
    }
    for (auto x : parser.registers)
    {
          (*file) << "\t" << "tokenTable[\"" << x->name << "\"] = " << x->id + TokenNode::REGISTER_BASE << ";" << std::endl;
    }
    i = 0;
    for (auto x : parser.opcodes)
    {
        if (x->name != "")
        {
            (*file) << "\t" << "opcodeTable[\"" << x->name << "\"] = " << i << ";" << std::endl;
        }
        i++;
    }
    if (parser.prefixes.size())
    {
        i = 0;
        for (auto x : parser.prefixes)
        {
            (*file) << "\t" << "prefixTable[\"" << x->name << "\"] = " << i << ";" << std::endl;
            i++;
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
    for (auto x : tokenData)
    {
        (*file) << "unsigned char " << className << "::RegClassData" << i << "[] = {" ;
        for (int j=0; j < n; j++)
        {
            (*file) << (int)((x)[j]) << ", " ;
        }
        (*file) << "};" << std::endl;
        i++;
    }
    (*file) << std::endl;
    n = (parser.registers.size() + 7)/ 8;
    for (auto& m : parser.registerClasses)
    {
        (*file) << "unsigned char " << className << "::registerData" << m.second->id << "[] = {";
        for (int j=0; j < n; j++)
        {
            (*file) << (int)(m.second->regs[j]) << ", " ;
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
    for (auto x : parser.registers)
    {
        for (auto& m : x->values)
        {
            int n = registerTags[m.first];
            vals[x->id * registerTags.size() + n] = strtol(m.second.c_str(), NULL, 0);
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
            for (auto& m : *value->values)
            {
                int n = valueTags[m.first];
                (*file) << "Coding " << className << "::tokenCoding" << value->id << "_" << n << "[] = {" << std::endl;
                GenerateCoding(m.second);
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
            (*file) << "\toperand.values[" << n << "]->unary = 0;" << std::endl;
            (*file) << "\toperand.values[" << n << "]->binary = 0;" << std::endl;
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
            (*file) << "\toperand.values[" << n << "]->unary = 0;" << std::endl;
            (*file) << "\toperand.values[" << n << "]->binary = 0;" << std::endl;
            (*file) << "\toperand.values[" << n << "][1].type = Coding::eot;" << std::endl;
            (*file) << "\toperands.push_back(numeric);" << std::endl;
        }
        if (value->values)
        {
            for (auto& m : *value->values)
            {
                int n = valueTags[m.first];
                (*file) << "\toperand.values[" << n << "] = tokenCoding" << value->id << "_" << n << ";" << std::endl;			
            }
        }
        (*file) << "}" << std::endl;
    }
}
void GenParser::GenerateAddressTokenTree(TokenNode *value)
{
    for (auto x : value->branches)
    {
        GenerateAddressTokenTree(x);
    }
    for (auto x : value->branches)
    {
        GenerateAddressFuncs(x, "addressCoding");
    }
    if (value->branches.size())
    {
        (*file) << tokenClassName << " " << className << "::tokenBranches" << value->id << "[] = {" << std::endl;
        for (auto x : value->branches)
        {
            if (x->type == TokenNode::tk_reg)
            {
                GenerateAddressData((x), "addressCoding");
            }
        }
        for (auto x : value->branches)
        {
            if (x->type != TokenNode::tk_reg)
            {
                GenerateAddressData((x), "addressCoding");
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
    for (auto x : value->branches)
    {
        GenerateOperandTokenTree(x);
    }
    for (auto x : value->branches)
    {
        GenerateAddressFuncs(x, "operandCoding");
    }
    if (value->branches.size())
    {
        (*file) << tokenClassName << " " << className << "::tokenBranches" << value->id << "[] = {" << std::endl;
        for (auto x : value->branches)
        {
            GenerateAddressData((x), "operandCoding");
        }
        (*file) << "\t{" << tokenClassName << "::EOT }" << std::endl;
        (*file) << "};" << std::endl;
    }
         
}
bool GenParser::GenerateOperands()
{
    for (auto x : parser.opcodes)
    {
        GenerateOperandTokenTree(x->tokenRoot);
    }
    return true;
}
bool GenParser::GenerateOpcodes()
{
    for (auto x : parser.opcodes)
    {
        for (auto& m : x->values)
        {
            int n = valueTags[m.first];
            (*file) << "Coding " << className << "::OpcodeCodings" << x->id << "_" << n << "[] = {" << std::endl;
            GenerateCoding(m.second);
            (*file) << "};" << std::endl;
        }
        (*file) << "bool "<<className<<"::Opcode"<<x->id<<"(" << operandClassName << " &operand)" << std::endl;
        (*file) << "{" << std::endl;
        for (auto& m : x->values)
        {
            int n = valueTags[m.first];
            (*file) << "\toperand.values[" << n << "] = OpcodeCodings" << x->id << "_" << n << ";" << std::endl;			
        }
        if (x->tokenRoot->branches.size())
        {
            (*file) << "\tbool rv = ParseOperands(tokenBranches" << x->tokenRoot->id << ", operand);" << std::endl;
        }
        if (x->name != "" && x->cclass != "")
        {
            Opcode *op = parser.opcodeClasses[x->cclass];
            if (op)
            {
                if (x->operands.size())
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
                std::cout << "Undefined opcode class '" << x->cclass <<
                               "' in definition of opcode '" << x->name << "'";
            }
        }
        else
        {
            if (!x->operands.size())
                (*file) << "\tbool rv = true;" << std::endl;
        }
        (*file) << "\treturn rv;" << std::endl;
        (*file) << "}" << std::endl;
    }	
    (*file) << className<<"::DispatchType "<<className<<"::DispatchTable["<<parser.opcodes.size()<<"] = {" << std::endl;
    for (auto x : parser.opcodes)
    {
        if (x->name == "")
            (*file) << "\tNULL," << std::endl;
        else
            (*file) << "\t&" << className << "::Opcode" << x->id << "," << std::endl;
    }	
    (*file) << "};" << std::endl;
    const std::string **codingNames = new const std::string *[parser.codings.size()];
    for (auto& m : parser.codings)
    {
        codingNames[m.second-1] = &m.first;
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
        for (auto x : parser.prefixes)
        {
            (*file) << "Coding " << className << "::prefixCoding" << i << "[] = {" << std::endl;
            GenerateCoding(x->coding);
            (*file) << "};" << std::endl;
            i++;
        }
        (*file) << "Coding *" << className << "::prefixCodings" << "[] = {" << std::endl;
        i = 1;
        for (auto x : parser.prefixes)
        {
            (*file) << "\t" << className << "::prefixCoding" << i << "," << std::endl;
            i++;
        }
        (*file) << "};";
        (*file) << std::endl;
    }
    delete codingNames;
    return true;
}
void GenParser::GenerateCoding(const std::string coding) 
{
    bool needsMore = false;
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
            char unary = 0;
            char binary = 0;
            int value = 0;
            int field =- 1;
            std::string fieldName;
            bool hasVal = false;
            bool native = false;
            bool illegal = false;
            bool stateFunc = false;
            bool stateVar = false;
            bool optional = false;
            int bits = -1;
            needsMore = false;
            if (npos != 0)
                temp = temp.substr(npos);
            switch (temp[0])
            {
                case '!':
                case '~':
                    unary = temp[0];
                    temp = temp.substr(1);
                    break;
                case '*':
                    optional = true;
                    temp = temp.substr(1);
                    break;
            }
            npos = temp.find_first_not_of(" ,\t\v\r\n");
            if (npos == std::string::npos)
            {
                temp = "";
                needsMore = true;
            }
            else
            {
                if (npos != 0)
                    temp = temp.substr(npos);

                if (isdigit(temp[0]))
                {
                    char *pch;
                    value = strtol(temp.c_str(), &pch, 0);
                    hasVal = true;
                    temp = temp.substr(pch - temp.c_str());
                }
                else if (temp[0] == '\'')
                {
                    int n = 1;
                    char *pch;
                    while (isalnum(temp[n]))
                        n++;
                    std::string name = temp.substr(1, n - 1);
                    temp = temp.substr(n);
                    std::map<std::string, int>::iterator itst = stateFuncTags.find(name);
                    if (itst != stateFuncTags.end())
                    {
                        value = itst->second;
                        stateFunc = true;
                    }
                    else
                    {
                        std::map<std::string, int>::iterator itst = stateVarTags.find(name);
                        if (itst != stateVarTags.end())
                        {
                            value = itst->second;
                            stateVar = true;
                        }
                        else
                        {
                            if (temp[0] == '.')
                            {
                                int m = 1, n = 1;
                                while (isalnum(temp[n]))
                                    n++;
                                fieldName = temp.substr(m, n - m);
                                temp = temp.substr(n);
                            }
                            std::map<std::string, int> ::iterator itp = valueTags.find(name);
                            if (itp == valueTags.end())
                            {
                                std::cout << "Error in coding: unknown variable '" << name << "'" << temp.substr(0, npos) << std::endl;
                            }
                            else
                            {
                                value = itp->second;
                            }
                            if (fieldName != "")
                            {
                                std::map<std::string, int> ::iterator itf = registerTags.find(fieldName);
                                if (itf == registerTags.end())
                                {
                                    std::cout << "Error in coding: unknown variable field '" << fieldName << "'" << temp.substr(0, npos) << std::endl;
                                }
                                else
                                {
                                    field = itf->second;
                                }
                            }
                        }
                    }
                    if (temp.size() == 0 || temp[0] != '\'')
                    {
                        std::cout << "Error { " << coding << " } missing close quote " << std::endl;
                        break;
                    }
                    else
                    {
                        temp = temp.substr(1);
                    }
                }
                else if (temp.size() >= 6 && temp.substr(0, 6) == "native")
                {
                    native = true;
                    temp = temp.substr(6);
                }
                else if (temp.size() >= 7 && temp.substr(0, 7) == "illegal")
                {
                    illegal = true;
                    temp = temp.substr(7);
                }
                npos = temp.find_first_not_of(" ,\t\v\r\n");
                if (npos == std::string::npos)
                {
                    temp = "";
                }
                else
                {
                    if (npos != 0)
                        temp = temp.substr(npos);
                    switch (temp[0])
                    {
                    case '+':
                    case '-':
                    case '&':
                    case '|':
                    case '^':
                        binary = temp[0];
                        temp = temp.substr(1);
                        break;
                    case '>':
                        if (temp[1] == '>')
                        {
                            binary = '>';
                            temp = temp.substr(2);
                        }
                        break;
                    case '<':
                        if (temp[1] == '>')
                        {
                            binary = '<';
                            temp = temp.substr(2);
                        }
                        break;
                    }
                    if (binary)
                        needsMore = true;
                    npos = temp.find_first_not_of(" ,\t\v\r\n");
                    if (npos == std::string::npos)
                    {
                        temp = "";
                    }
                    else
                    {
                        if (npos != 0)
                            temp = temp.substr(npos);
                        if (!needsMore && temp[0] == ':')
                        {
                            char *pch;
                            bits = strtol(temp.c_str()+1, &pch, 0);
                            temp = temp.substr(pch - temp.c_str());
                        }
                    }
                }

                if (native)
                {
                    (*file) << "\t{ Coding::native " << "}," << std::endl;
                }
                else if (illegal)
                {
                    (*file) << "\t{ Coding::illegal " << "}," << std::endl;
                }
                else if (stateVar)
                {
                    (*file) << "\t{ Coding::stateVar, " << value << " }," << std::endl;
                }
                else if (stateFunc)
                {
                    (*file) << "\t{ Coding::stateFunc, " << value << " }," << std::endl;
                }
                else
                {
                    (*file) << "\t{ (Coding::Type)(";
                    if (optional)
                        (*file) << "Coding::optional | ";
                    if (bits != -1)
                        (*file) << "Coding::bitSpecified | ";
                    if (hasVal)
                    {
                        (*file) << "Coding::valSpecified), ";
                    }
                    else if (field == -1)
                    {
                        (*file) << "Coding::indirect), ";
                    }
                    else
                    {
                        (*file) << "Coding::indirect | Coding::fieldSpecified), ";
                    }
                    (*file) << value << ", " << bits << ", " << field << ", ";
                    if (unary)
                        (*file) << "'" << unary << "', ";
                    else
                        (*file << "0, ");
                    if (binary)
                        (*file) << "'" << binary << "'";
                    else
                        (*file) << "0";
                    (*file) << " }, " << std::endl;
                }
            }
        }
    }
    (*file) << "\t{ Coding::eot " << "}," << std::endl;
    if (needsMore)
    {
        std::cout << "Error { " << coding << " } premature end of coding sequence" << std::endl;
    }
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
    (*file) << "bool " << className << "::ProcessCoding("<<operandClassName<<" &operand, Coding *coding, int field, int bits, int* arr, char* bitcounts, char *func, int &index)" << std::endl;
    (*file) << "{" << std::endl;
    (*file) << "\tint acc = 0, binary = 0;" << std::endl;
    (*file) << "\twhile (coding->type != Coding::eot)" << std::endl;
    (*file) << "\t{" << std::endl;
    (*file) << "\t\tif (coding->type & Coding::bitSpecified)" << std::endl;
    (*file) << "\t\t\tbits = coding->bits;" << std::endl;
    (*file) << "\t\tif (coding->type & Coding::valSpecified)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tfunc[index] = coding->binary;" << std::endl;
    (*file) << "\t\t\tbitcounts[index] = bits;" << std::endl;
    (*file) << "\t\t\tarr[index++] = coding->val;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse if (coding->type & Coding::reg)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tint n = coding->val;" << std::endl;
    (*file) << "\t\t\tif (field != -1)" << std::endl;
    (*file) << "\t\t\t\tn = registerValues[n][field];" << std::endl;
    (*file) << "\t\t\tfunc[index] = coding->binary;" << std::endl;
    (*file) << "\t\t\tbitcounts[index] = bits;" << std::endl;
    (*file) << "\t\t\tarr[index++] = n;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse if (coding->type & Coding::stateFunc)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tCoding *c = (this->*stateFuncs[coding->val])();" << std::endl;
    (*file) << "\t\t\tint index1 = index;" << std::endl;
    (*file) << "\t\t\tif (!ProcessCoding(operand, c, coding->field, bits, arr, bitcounts, func, index))" << std::endl;
    (*file) << "\t\t\t\treturn false;" << std::endl;
    (*file) << "\t\t\tif (index != index1 && coding->binary)" << std::endl;
    (*file) << "\t\t\t\tfunc[index - 1] = coding->binary;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse if (coding->type & Coding::stateVar)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tfunc[index] = coding->binary;" << std::endl;
    (*file) << "\t\t\tbitcounts[index] = bits;" << std::endl;
    (*file) << "\t\t\tarr[index++] = stateVars[coding->val];" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse if (coding->type & Coding::number)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tint n = coding->val;" << std::endl;
    (*file) << "\t\t\tauto it = operands.begin();" << std::endl;
    (*file) << "\t\t\tfor (int i = 0; i < n; i++)" << std::endl;
    (*file) << "\t\t\t{" << std::endl;
    (*file) << "\t\t\t\t++it;" << std::endl;
    (*file) << "\t\t\t}" << std::endl;
    (*file) << "\t\t\t(*it)->used = true;" << std::endl;
    (*file) << "\t\t\t(*it)->pos = this->bits.GetBits();" << std::endl;
    (*file) << "\t\t\tfunc[index] = coding->binary;" << std::endl;
    (*file) << "\t\t\tbitcounts[index] = bits;" << std::endl;
    (*file) << "\t\t\tarr[index++] = (*it)->node->ival;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse if (coding->type & Coding::native)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tif (operand.addressCoding == -1)" << std::endl;
    (*file) << "\t\t\t\treturn false;" << std::endl;
    (*file) << "\t\t\tint index1 = index;" << std::endl;
    (*file) << "\t\t\tif (!ProcessCoding(operand, Codings[operand.addressCoding], coding->field, bits, arr, bitcounts, func, index))" << std::endl;
    (*file) << "\t\t\t\treturn false;" << std::endl;
    (*file) << "\t\t\tif (index != index1 && coding->binary)" << std::endl;
    (*file) << "\t\t\t\tfunc[index - 1] = coding->binary;" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t\telse if (coding->type & Coding::indirect)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tif (!operand.values[coding->val])" << std::endl;
    (*file) << "\t\t\t{" << std::endl;
    (*file) << "\t\t\t\tif (!(coding->type & Coding::optional))" << std::endl;
    (*file) << "\t\t\t\t{" << std::endl;
    (*file) << "\t\t\t\t\treturn false;" << std::endl;
    (*file) << "\t\t\t\t}" << std::endl;
    (*file) << "\t\t\t}" << std::endl;
    (*file) << "\t\t\telse" << std::endl;
    (*file) << "\t\t\t{" << std::endl;
    (*file) << "\t\t\t\tint index1 = index;" << std::endl;
    (*file) << "\t\t\t\tif (!ProcessCoding(operand, operand.values[coding->val], coding->field, bits, arr, bitcounts, func, index))" << std::endl;
    (*file) << "\t\t\t\t\treturn false;" << std::endl;
    (*file) << "\t\t\t\tif (index != index1 && coding->binary)" << std::endl;
    (*file) << "\t\t\t\t\tfunc[index - 1] = coding->binary;" << std::endl;
    (*file) << "\t\t\t}" << std::endl;

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


    (*file) << "bool " << className << "::ProcessCoding(" << operandClassName << " &operand, Coding *coding)" << std::endl;
    (*file) << "{" << std::endl;
    (*file) << "\tint arr[1000];" << std::endl;
    (*file) << "\tchar bitcount[1000];" << std::endl;
    (*file) << "\tchar func[1000];" << std::endl;
    (*file) << "\tint index = 0;" << std::endl;
    (*file) << "\tif (coding->type == Coding::eot)" << std::endl;
    (*file) << "\t\treturn true;" << std::endl;
    (*file) << "\tint defaultBits = 8;" << std::endl;
    (*file) << "\tbool rv = ProcessCoding(operand, coding, -1, defaultBits, arr, bitcount, func, index);" << std::endl;
    (*file) << "\tif (rv)" << std::endl;
    (*file) << "\t{" << std::endl;
    (*file) << "\t\tfor (int i = 0; i < index; i++)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    (*file) << "\t\t\tif (func[i])" << std::endl;
    (*file) << "\t\t\t\tarr[i + 1] = DoMath(func[i], arr[i], arr[i + 1]);" << std::endl;
    (*file) << "\t\t\telse" << std::endl;
    (*file) << "\t\t\t\tbits.Add(arr[i], bitcount[i]);" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t}" << std::endl;
    (*file) << "\treturn rv;" << std::endl;
    (*file) << "}" << std::endl;

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
    (*file) << "\t\tfor (auto& a : prefixes)" << std::endl;
    (*file) << "\t\t\trv &= ProcessCoding(operand, prefixCodings[a]);" << std::endl;
    (*file) << "\t}" << std::endl;
    (*file) << "\telse" << std::endl;
    (*file) << "\t{" << std::endl;
    (*file) << "\t\t" << operandClassName << " operand;" << std::endl;
    (*file) << "\t\toperand.opcode = opcode;" << std::endl;
    (*file) << "\t\trv = (this->*DispatchTable[opcode])(operand);" << std::endl;
    (*file) << "\t\tif (rv)" << std::endl;
    (*file) << "\t\t{" << std::endl;
    if (parser.prefixes.size())
    {
        (*file) << "\t\t\tfor (auto& a : prefixes)" << std::endl;
        (*file) << "\t\t\t\trv &= ProcessCoding(operand, prefixCodings[a]);" << std::endl;
        (*file) << "\t\t\tif (rv)" << std::endl;
    }
    (*file) << "\t\t\t{" << std::endl;
    (*file) << "\t\t\t\tif (operand.operandCoding != -1)" << std::endl;
    (*file) << "\t\t\t\t\trv = ProcessCoding(operand, Codings[operand.operandCoding]);" << std::endl;
    (*file) << "\t\t\t\telse if (operand.addressCoding != -1)" << std::endl;
    (*file) << "\t\t\t\t\trv = ProcessCoding(operand, Codings[operand.addressCoding]);" << std::endl;
    (*file) << "\t\t\t\telse rv = false;" << std::endl;
    (*file) << "\t\t\t}" << std::endl;
    (*file) << "\t\t}" << std::endl;
    (*file) << "\t}" << std::endl;
    (*file) << "\treturn rv;" << std::endl;
    (*file) << "}" << std::endl;
    return true;
}
