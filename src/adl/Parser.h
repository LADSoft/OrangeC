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
#ifndef Parser_h
#define Parser_h

#include "xml.h"

typedef unsigned char BYTE;

#include <string>
#include <map>

class TokenNode;

struct Number
{
    Number() : id(0), relOfs(0) { }
    std::string name;
    std::deque<std::string> values;
    int id;
    int relOfs;
};
struct Register
{
    Register(std::string Name, std::string Class, std::map<std::string, std::string> &Values)
        : name(Name), cclass(Class), values(Values) { }
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
    std::deque<TokenNode *> tokens;
    int id;
};
class Opcode
{
public:
    std::string name;
    std::string cclass;
    std::map<std::string, std::string> values;	
    std::deque<Operand *> operands;
    TokenNode *tokenRoot;
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
} ;
class RegClass
{
public:
    RegClass(std::string Name, int bits) : name(Name) { regs = new BYTE((bits +7)/8); memset(regs, 0, (bits +7)/8); }
    ~RegClass() { if (regs) delete regs; }
    int id;
    BYTE *regs;
    std::string name;
};
class AddressClass
{
public:
    AddressClass(std::string Name) : name(Name) { }
    std::string name;
    int id;
} ;
class Parser: public xmlVisitor
{
public:
    Parser() { }
    virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
    virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);

    void DumpDB();
    std::string processorName;
    std::map<std::string, std::string> parameters;
    std::deque<Number *> numbers;
    std::map<std::string, std::string> stateVars;
    std::map<std::string, int> codings;
    std::deque<State *> states;
    std::deque<Register *> registers;
    std::deque<Address *> addresses;
    std::deque<Opcode *> opcodes;
    std::map<std::string, Opcode *> opcodeClasses;
    std::deque<Operand *> operands;
    std::deque<Prefix *> prefixes;

    bool CreateParseTree();
    
    std::map<std::string, RegClass *> registerClasses;
    std::map<std::string, AddressClass *> addressClasses;
    TokenNode *addressRoot;
protected:
    void EnterRegisterClasses(Register *reg);
    bool LoadRegisterClasses();
    void EnterAddressClasses(Address *address);
    bool LoadAddressClasses();
    bool LoadAddressTokens(std::string name, std::deque<TokenNode *> &nodes);
    void TagClasses(std::string &cclass, std::string coding, 
                    std::map<std::string, std::string> *values, 
                    std::deque<TokenNode *> &nodes);
    void EnterInTokenTree(TokenNode *root, std::deque<TokenNode *> &nodes);
    bool LoadAddresses();
    bool LoadOperandTokens(std::string name, std::string coding, 
                           std::map<std::string, std::string> *values, 
                           std::deque<TokenNode *> &nodes);
    bool LoadOperands();
private:
    class ProcessorParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    } ;
    class CodingParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingParamParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingNumberParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingNumberInstanceParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingStatevarsParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingStatevarsInstanceParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingStateParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingStateWhenParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingRegisterParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingRegisterInstanceParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingDoubleRegParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingAddressParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingOpcodeParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingOpcodeOperandParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
    class CodingPrefixParser : public xmlVisitor
    {
    public:
        virtual bool VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData);
        virtual bool VisitNode(const xmlNode &node, const xmlNode *child, void *userData);
    
    };
};
#endif
