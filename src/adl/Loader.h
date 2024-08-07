/* Software License Agreement
 * 
 *     Copyright(C) 1994-2024 David Lindauer, (LADSoft)
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

#ifndef Parser_h
#define Parser_h

#include "xml.h"
#include <deque>
#include <string>
#include <map>
typedef unsigned char BYTE;

class TokenNode;

struct Number
{
    Number() : id(0), relOfs(0) {}
    std::string name;
    std::deque<std::string> values;
    int id;
    int relOfs;
};
struct Register
{
    Register(std::string Name, std::string Class, std::map<std::string, std::string>& Values) :
        name(Name),
        cclass(Class),
        values(Values)
    {
    }
    std::string name;
    std::string cclass;
    std::map<std::string, std::string> values;
    int id;
};
struct Address
{
    std::string name;
    std::string cclass;
    std::string coding;
    std::map<std::string, std::string> values;
};
class Operand
{
  public:
    std::string name;
    std::string coding;
    std::map<std::string, std::string> values;
    std::deque<TokenNode*> tokens;
    int id;
};
class Opcode
{
  public:
    std::string name;
    std::string cclass;
    std::map<std::string, std::string> values;
    std::deque<Operand*> operands;
    TokenNode* tokenRoot;
    int id;
};
class Prefix
{
  public:
    std::string name;
    std::string coding;
};
class State
{
  public:
    std::string name;
    std::map<std::string, std::string> clauses;
};
class RegClass
{
  public:
    RegClass(std::string Name, int bits) : name(Name)
    {
        regs = new BYTE[(bits + 7) / 8];
        memset(regs, 0, (bits + 7) / 8);
    }
    ~RegClass()
    {
        if (regs)
            delete []regs;
    }
    int id;
    BYTE* regs;
    std::string name;
};
class AddressClass
{
  public:
    AddressClass(std::string Name) : name(Name) {}
    std::string name;
    int id;
};
class Parser : public xmlVisitor
{
  public:
    Parser() {}
    virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
    virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);

    void DumpDB();
    std::string processorName;
    std::map<std::string, std::string> parameters;
    std::deque<Number*> numbers;
    std::map<std::string, std::string> stateVars;
    std::map<std::string, int> codings;
    std::deque<State*> states;
    std::deque<Register*> registers;
    std::deque<Address*> addresses;
    std::deque<Opcode*> opcodes;
    std::map<std::string, Opcode*> opcodeClasses;
    std::deque<Operand*> operands;
    std::deque<Prefix*> prefixes;

    bool CreateParseTree();

    std::map<std::string, RegClass*> registerClasses;
    std::map<std::string, AddressClass*> addressClasses;
    TokenNode* addressRoot;

  protected:
    void EnterRegisterClasses(Register* reg);
    bool LoadRegisterClasses();
    void EnterAddressClasses(Address* address);
    bool LoadAddressClasses();
    bool LoadAddressTokens(std::string name, std::deque<TokenNode*>& nodes);
    void TagClasses(std::string& cclass, std::string coding, std::map<std::string, std::string>* values,
                    std::deque<TokenNode*>& nodes);
    void EnterInTokenTree(TokenNode* root, std::deque<TokenNode*>& nodes);
    bool LoadAddresses();
    bool LoadOperandTokens(std::string name, std::string coding, std::map<std::string, std::string>* values,
                           std::deque<TokenNode*>& nodes);
    bool LoadOperands();

  private:
    class ProcessorParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingParamParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingNumberParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingNumberInstanceParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingStatevarsParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingStatevarsInstanceParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingStateParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingStateWhenParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingRegisterParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingRegisterInstanceParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingAddressParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingOpcodeParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingOpcodeOperandParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
    class CodingPrefixParser : public xmlVisitor
    {
      public:
        virtual bool VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData);
        virtual bool VisitNode(xmlNode& node, xmlNode* child, void* userData);
    };
};
#endif
