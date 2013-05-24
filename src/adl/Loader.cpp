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
#include "Parser.h"

#include <stdlib.h>
#include <fstream>

void Parser::DumpDB()
{
    std::cout << "Name: " << processorName << std::endl;
    for (std::map<std::string, std::string>::iterator it = parameters.begin();
         it != parameters.end(); ++it)
    {
        std::cout << "Param: " << it->first << " Value: " << it->second << std::endl;
    }
    for (std::deque<Number *>::iterator it = numbers.begin(); it != numbers.end();
         ++it)
    {
        Number *n = (*it);
        std::cout << "Number: " << n->name << std::endl;
        for (std::deque<std::string>::iterator it1 = n->values.begin();
             it1 != n->values.end(); ++it1)
        {
            std::cout << "\t" << (*it1) << std::endl;
        }
    }
    for (std::map<std::string, std::string>::iterator it = stateVars.begin();
         it != stateVars.end(); ++it)
    {
        std::cout << "State Var: " << it->first << " Init: " << it->second << std::endl;
    }
    for (std::deque<State *>::iterator it = states.begin(); it != states.end(); ++it)
    {
        State *s = *it;
        std::cout << "State: " << s->name << std::endl;
        for (std::map<std::string, std::string>::iterator it1 = s->clauses.begin();
             it1 != s->clauses.end(); ++it1)
        {
            std::cout << "\t if (" <<it1->first << "): '" << it1->second << "'" << std::endl;
        }
    }
    for (std::deque<Register *>::iterator it = registers.begin(); it != registers.end(); ++it)
    {
        Register *r = *it;
        std::cout << "Register: " << r->name << " Classes: " << r->cclass << std::endl;
        for (std::map<std::string, std::string>::iterator it1 = r->values.begin();
             it1 != r->values.end(); ++it1)
        {
            std::cout << "\tExpr: " << it1->first << " Value: " << it1->second << std::endl;
        }
    }
    for (std::deque<Address *>::iterator it = addresses.begin(); it != addresses.end(); ++it)
    {
        Address *r = *it;
        std::cout << "Address: " << r->name << " Classes: " << r->cclass << std::endl;
        std::cout << "\tCoding: " << r->coding << std::endl;
        for (std::map<std::string, std::string>::iterator it1 = r->values.begin();
             it1 != r->values.end(); ++it1)
        {
            std::cout << "\tExpr: " << it1->first << " Value: " << it1->second << std::endl;
        }
    }
    for (std::deque<Opcode *>::iterator it = opcodes.begin(); it != opcodes.end(); ++it)
    {
        Opcode *r = *it;
        std::cout << "Opcode: " << r->name << " Classes: " << r->cclass << std::endl;
        for (std::map<std::string, std::string>::iterator it1 = r->values.begin();
             it1 != r->values.end(); ++it1)
        {
            std::cout << "\tExpr: " << it1->first << " Value: " << it1->second << std::endl;
        }
        for (std::deque<Operand *>::iterator it1 = r->operands.begin();
             it1 != r->operands.end(); ++ it1)
        {
            Operand *o = *it1;
            std::cout << "Operand: " << o->name << " Coding: " << o->coding << std::endl;
            for (std::map<std::string, std::string>::iterator it2 = o->values.begin();
                 it2 != o->values.end(); ++it2)
            {
                std::cout << "\tExpr: " << it2->first << " Value: " << it2->second << std::endl;
            }
        }
    }
    for (std::deque<Prefix *>::iterator it = prefixes.begin(); it != prefixes.end(); ++it)
    {
        Prefix *p = *it;
        std::cout << "Prefix: " << p->name << " Coding: " << p->coding << std::endl;
    }
}

struct Param
{
    std::string name;
    std::string value;
};
struct RegData
{
    Parser *parser;
    std::string name;
    std::string defaultClass;
    std::string extendedClass;
    std::map<std::string, std::string> values;
} ;
struct StateVar
{
    Parser *parser;
    std::string name;
    std::string init;
};
bool Parser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    // shouldn't be any attribs
    return true;
}
bool Parser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    if (child->GetName() == "Processor")
    {
        ProcessorParser p;
        child->Visit(p, userData);
    }
    else if (child->GetName() == "Coding")
    {
        CodingParser p;
        child->Visit(p, userData);
    }
    return true;
}
bool Parser::ProcessorParser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    Parser *parser = (Parser *)userData;
    if (attrib->GetName() == "Name" )
    {
        parser->processorName = attrib->GetValue();
    }
    return true;
}
bool Parser::ProcessorParser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    return true;
}
bool Parser::CodingParser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    return true;
}
bool Parser::CodingParser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    Parser *parser = (Parser *)userData;
    if (child->GetName() == "Param")
    {
        CodingParamParser p;
        Param x;
        x.name = "";
        x.value= "";
        child->Visit(p, &x);
        parser->parameters[x.name] = x.value;
    }
    else if (child->GetName() == "Number")
    {
        CodingNumberParser p;
        Number *number = new Number;
        child->Visit(p, number);
        number->id = parser->numbers.size();
        parser->numbers.push_back(number);
    }
    else if (child->GetName() == "StateVars")
    {
    
        CodingStatevarsParser p;
        StateVar r;
        r.parser = parser;
        child->Visit(p, &r);
    }
    else if (child->GetName()== "State")
    {
        CodingStateParser p;
        State *s = new State;
        child->Visit(p, s);
        parser->states.push_back(s);
    }
    else if (child->GetName()== "Register")
    {
        CodingRegisterParser p;
        RegData r;
        r.parser = parser;
        child->Visit(p, &r);
    }
    else if (child->GetName() == "Address")
    {
        CodingAddressParser p;
        Address *a = new Address;
        child->Visit(p, a);
        parser->addresses.push_back(a);
    }
    else if (child->GetName() == "Opcode")
    {
        CodingOpcodeParser p;
        Opcode *o = new Opcode;
        child->Visit(p, o);
        o->id = parser->opcodes.size();
        parser->opcodes.push_back(o);
        if (o->name == "")
            parser->opcodeClasses[o->cclass] = o;
        for (std::deque<Operand *>::iterator it = o->operands.begin();
             it != o->operands.end(); ++it)
        {
            (*it)->id = parser->operands.size();
            parser->operands.push_back(*it);
        }
    }
    else if (child->GetName() == "Prefix")
    {
        CodingPrefixParser p;
        Prefix *prefix = new Prefix;
        child->Visit(p, prefix);
        parser->prefixes.push_back(prefix);
    }
    return true;
}
bool Parser::CodingParamParser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    Param *x = (Param *)userData;
    if (attrib->GetName() == "Name" )
    {
        x->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Value" )
    {
        x->value = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingParamParser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    return true;
}
bool Parser::CodingNumberParser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    Number *number = (Number *)userData;
    if (attrib->GetName() == "Name" )
    {
        number->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "relOfs")
    {
        number->relOfs = atoi(attrib->GetValue().c_str());
    }
    return true;
}
bool Parser::CodingNumberParser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    if (child->GetName() == "Instance")
    {
        CodingNumberInstanceParser p;
        child->Visit(p, userData);
    }
    return true;
}
bool Parser::CodingNumberInstanceParser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    Number *number = (Number *)userData;
    if (attrib->GetName() == "Value" )
    {
        number->values.push_back(attrib->GetValue());
    }
    if (attrib->GetName() == "Cond")
    {
        number->values.back() += std::string("&") + attrib->GetValue();
    }
    return true;
}
bool Parser::CodingNumberInstanceParser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    return true;
}
bool Parser::CodingStatevarsParser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    return false;
}
bool Parser::CodingStatevarsParser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    StateVar *r = (StateVar *)userData;
    if (child->GetName() == "Instance")
    {
        CodingStatevarsInstanceParser p;
        r->name = "";
        r->init = "";
        child->Visit(p, r);
        r->parser->stateVars[r->name] = r->init;
    }
    return true;
}
bool Parser::CodingStatevarsInstanceParser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    StateVar *r = (StateVar *)userData;
    if (attrib->GetName() == "Name" )
    {
        r->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Init" )
    {
        r->init = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingStatevarsInstanceParser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    return true;
}
bool Parser::CodingStateParser ::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    State *s = (State *)userData;
    if (attrib->GetName() == "Name" )
    {
        s->name = attrib->GetValue();
    }	
    return true;
}
bool Parser::CodingStateParser ::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    State *s = (State *)userData;
    if (child->GetName()=="When")
    {
        CodingStateWhenParser p;
        Param r;
        child->Visit(p, &r);
        s->clauses[r.name] = r.value;
    }
    return true;
}
bool Parser::CodingStateWhenParser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    Param *p = (Param *)userData;
    if (attrib->GetName() == "Cond" )
    {
        p->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Value" )
    {
        p->value = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingStateWhenParser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    return true;
}
bool Parser::CodingRegisterParser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    RegData *r = (RegData *)userData;
    if (attrib->GetName() == "Class" )
    {
        r->defaultClass = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingRegisterParser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    RegData * r = (RegData *)userData;
    if (child->GetName() == "Instance")
    {
        CodingRegisterInstanceParser p;
        r->name = "";
        r->values.clear();
        r->extendedClass="";
        child->Visit(p, r);
        Register *reg = new Register(r->name, r->defaultClass+","+r->extendedClass, r->values);
        reg->id = r->parser->registers.size();
        r->parser->registers.push_back(reg);
    }
    return true;
}
bool Parser::CodingRegisterInstanceParser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    RegData * r = (RegData *)userData;
    if (attrib->GetName() == "Name" )
    {
        r->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Class" )
    {
        r->extendedClass = attrib->GetValue();
    }
    else
    {
        r->values[attrib->GetName()] = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingRegisterInstanceParser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    return true;
}
bool Parser::CodingDoubleRegParser ::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    if (attrib->GetName() == "Name" )
    {
    }
    return true;
}
bool Parser::CodingDoubleRegParser ::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    return true;
}
bool Parser::CodingAddressParser ::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    Address *r = (Address *)userData;
    if (attrib->GetName() == "Name" )
    {
        r->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Class" )
    {
        r->cclass = attrib->GetValue();
    }
    else if (attrib->GetName() == "Coding" )
    {
        r->coding = attrib->GetValue();
    }
    else
    {
        r->values[attrib->GetName()] = attrib->GetValue();
    }
    return true;

}
bool Parser::CodingAddressParser ::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    return true;
}
bool Parser::CodingOpcodeParser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    Opcode *r = (Opcode *) userData;
    if (attrib->GetName() == "Name" )
    {
        r->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Class" )
    {
        r->cclass = attrib->GetValue();
    }
    else
    {
        r->values[attrib->GetName()] = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingOpcodeParser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    Opcode *opcodes = (Opcode *)userData;
    if (child->GetName() == "Operands")
    {
        CodingOpcodeOperandParser p;
        Operand *o = new Operand;
        child->Visit(p, o);
        opcodes->operands.push_back(o);
    }
}
bool Parser::CodingOpcodeOperandParser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    Operand *operand = (Operand *)userData;
    if (attrib->GetName() == "Name" )
    {
        operand->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Coding" )
    {
        operand->coding = attrib->GetValue();
    }
    else
    {
        operand->values[attrib->GetName()] = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingOpcodeOperandParser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    return true;
}
bool Parser::CodingPrefixParser::VisitAttrib(const xmlNode &node, const xmlAttrib *attrib, void *userData)
{
    Prefix *prefix = (Prefix *)userData;
    if (attrib->GetName() == "Name")
    {
        prefix->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Coding")
    {
        prefix->coding = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingPrefixParser::VisitNode(const xmlNode &node, const xmlNode *child, void *userData)
{
    return true;
}

