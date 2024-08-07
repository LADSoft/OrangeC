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

#include "Loader.h"

#include <cstdlib>
#include <fstream>
#include <iostream>

void Parser::DumpDB()
{
    std::cout << "Name: " << processorName << std::endl;
    for (auto& m : parameters)
    {
        std::cout << "Param: " << m.first << " Value: " << m.second << std::endl;
    }
    for (auto x : numbers)
    {
        Number* n = (x);
        std::cout << "Number: " << n->name << std::endl;
        for (auto n : n->values)
        {
            std::cout << "\t" << n << std::endl;
        }
    }
    for (auto& m : stateVars)
    {
        std::cout << "State Var: " << m.first << " Init: " << m.second << std::endl;
    }
    for (auto x : states)
    {
        State* s = x;
        std::cout << "State: " << s->name << std::endl;
        for (auto& m : s->clauses)
        {
            std::cout << "\t if (" << m.first << "): '" << m.second << "'" << std::endl;
        }
    }
    for (auto x : registers)
    {
        Register* r = x;
        std::cout << "Register: " << r->name << " Classes: " << r->cclass << std::endl;
        for (auto& m : r->values)
        {
            std::cout << "\texpr: " << m.first << " Value: " << m.second << std::endl;
        }
    }
    for (auto x : addresses)
    {
        Address* r = x;
        std::cout << "Address: " << r->name << " Classes: " << r->cclass << std::endl;
        std::cout << "\tCoding: " << r->coding << std::endl;
        for (auto& m : r->values)
        {
            std::cout << "\texpr: " << m.first << " Value: " << m.second << std::endl;
        }
    }
    for (auto x : opcodes)
    {
        Opcode* r = x;
        std::cout << "Opcode: " << r->name << " Classes: " << r->cclass << std::endl;
        for (auto& m : r->values)
        {
            std::cout << "\texpr: " << m.first << " Value: " << m.second << std::endl;
        }
        for (auto o : r->operands)
        {
            std::cout << "Operand: " << o->name << " Coding: " << o->coding << std::endl;
            for (std::map<std::string, std::string>::iterator itm = o->values.begin(); itm != o->values.end(); ++itm)
            {
                std::cout << "\texpr: " << itm->first << " Value: " << itm->second << std::endl;
            }
        }
    }
    for (auto x : prefixes)
    {
        Prefix* p = x;
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
    Parser* parser;
    std::string name;
    std::string defaultClass;
    std::string extendedClass;
    std::map<std::string, std::string> values;
};
struct StateVar
{
    Parser* parser;
    std::string name;
    std::string init;
};
bool Parser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    // shouldn't be any attribs
    return true;
}
bool Parser::VisitNode(xmlNode& node, xmlNode* child, void* userData)
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
bool Parser::ProcessorParser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    Parser* parser = (Parser*)userData;
    if (attrib->GetName() == "Name")
    {
        parser->processorName = attrib->GetValue();
    }
    return true;
}
bool Parser::ProcessorParser::VisitNode(xmlNode& node, xmlNode* child, void* userData) 
{ 
    Parser* parser = (Parser*)userData;
    if (child->GetName() == "Param")
    {
        CodingParamParser p;
        Param x;
        x.name = "";
        x.value = "";
        child->Visit(p, &x);
        parser->parameters[x.name] = x.value;
    }
    return true; 
}
bool Parser::CodingParser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData) { return true; }
bool Parser::CodingParser::VisitNode(xmlNode& node, xmlNode* child, void* userData)
{
    Parser* parser = (Parser*)userData;
    if (child->GetName() == "Number")
    {
        CodingNumberParser p;
        Number* number = new Number;
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
    else if (child->GetName() == "State")
    {
        CodingStateParser p;
        State* s = new State;
        child->Visit(p, s);
        parser->states.push_back(s);
    }
    else if (child->GetName() == "Register")
    {
        CodingRegisterParser p;
        RegData r;
        r.parser = parser;
        child->Visit(p, &r);
    }
    else if (child->GetName() == "Address")
    {
        CodingAddressParser p;
        Address* a = new Address;
        child->Visit(p, a);
        parser->addresses.push_back(a);
    }
    else if (child->GetName() == "Opcode")
    {
        CodingOpcodeParser p;
        Opcode* o = new Opcode;
        child->Visit(p, o);
        o->id = parser->opcodes.size();
        parser->opcodes.push_back(o);
        if (o->name == "")
            parser->opcodeClasses[o->cclass] = o;
        for (auto x : o->operands)
        {
            x->id = parser->operands.size();
            parser->operands.push_back(x);
        }
    }
    else if (child->GetName() == "Prefix")
    {
        CodingPrefixParser p;
        Prefix* prefix = new Prefix;
        child->Visit(p, prefix);
        parser->prefixes.push_back(prefix);
    }
    return true;
}
bool Parser::CodingParamParser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    Param* x = (Param*)userData;
    if (attrib->GetName() == "Name")
    {
        x->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Value")
    {
        x->value = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingParamParser::VisitNode(xmlNode& node, xmlNode* child, void* userData) { return true; }
bool Parser::CodingNumberParser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    Number* number = (Number*)userData;
    if (attrib->GetName() == "Class")
    {
        number->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "relOfs")
    {
        number->relOfs = std::stoi(attrib->GetValue());
    }
    return true;
}
bool Parser::CodingNumberParser::VisitNode(xmlNode& node, xmlNode* child, void* userData)
{
    if (child->GetName() == "Instance")
    {
        CodingNumberInstanceParser p;
        child->Visit(p, userData);
    }
    return true;
}
bool Parser::CodingNumberInstanceParser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    Number* number = (Number*)userData;
    if (attrib->GetName() == "Value")
    {
        number->values.push_back(attrib->GetValue());
    }
    if (attrib->GetName() == "Cond")
    {
        number->values.back() += std::string("&") + attrib->GetValue();
    }
    return true;
}
bool Parser::CodingNumberInstanceParser::VisitNode(xmlNode& node, xmlNode* child, void* userData) { return true; }
bool Parser::CodingStatevarsParser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData) { return false; }
bool Parser::CodingStatevarsParser::VisitNode(xmlNode& node, xmlNode* child, void* userData)
{
    StateVar* r = (StateVar*)userData;
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
bool Parser::CodingStatevarsInstanceParser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    StateVar* r = (StateVar*)userData;
    if (attrib->GetName() == "Name")
    {
        r->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Init")
    {
        r->init = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingStatevarsInstanceParser::VisitNode(xmlNode& node, xmlNode* child, void* userData) { return true; }
bool Parser::CodingStateParser ::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    State* s = (State*)userData;
    if (attrib->GetName() == "Name")
    {
        s->name = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingStateParser ::VisitNode(xmlNode& node, xmlNode* child, void* userData)
{
    State* s = (State*)userData;
    if (child->GetName() == "Value")
    {
        CodingStateWhenParser p;
        Param r;
        child->Visit(p, &r);
        s->clauses[r.name] = r.value;
    }
    return true;
}
bool Parser::CodingStateWhenParser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    Param* p = (Param*)userData;
    if (attrib->GetName() == "Cond")
    {
        p->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Coding")
    {
        p->value = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingStateWhenParser::VisitNode(xmlNode& node, xmlNode* child, void* userData) { return true; }
bool Parser::CodingRegisterParser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    RegData* r = (RegData*)userData;
    if (attrib->GetName() == "Class")
    {
        r->defaultClass = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingRegisterParser::VisitNode(xmlNode& node, xmlNode* child, void* userData)
{
    RegData* r = (RegData*)userData;
    if (child->GetName() == "Instance")
    {
        CodingRegisterInstanceParser p;
        r->name = "";
        r->values.clear();
        r->extendedClass = "";
        child->Visit(p, r);
        Register* reg = new Register(r->name, r->defaultClass + "," + r->extendedClass, r->values);
        reg->id = r->parser->registers.size();
        r->parser->registers.push_back(reg);
    }
    return true;
}
bool Parser::CodingRegisterInstanceParser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    RegData* r = (RegData*)userData;
    if (attrib->GetName() == "Name")
    {
        r->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Class")
    {
        r->extendedClass = attrib->GetValue();
    }
    else
    {
        r->values[attrib->GetName()] = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingRegisterInstanceParser::VisitNode(xmlNode& node, xmlNode* child, void* userData) { return true; }
bool Parser::CodingAddressParser ::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    Address* r = (Address*)userData;
    if (attrib->GetName() == "Name")
    {
        r->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Class")
    {
        r->cclass = attrib->GetValue();
    }
    else if (attrib->GetName() == "Coding")
    {
        r->coding = attrib->GetValue();
    }
    else
    {
        r->values[attrib->GetName()] = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingAddressParser ::VisitNode(xmlNode& node, xmlNode* child, void* userData) { return true; }
bool Parser::CodingOpcodeParser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    Opcode* r = (Opcode*)userData;
    if (attrib->GetName() == "Name")
    {
        r->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Class")
    {
        r->cclass = attrib->GetValue();
    }
    else
    {
        r->values[attrib->GetName()] = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingOpcodeParser::VisitNode(xmlNode& node, xmlNode* child, void* userData)
{
    Opcode* opcodes = (Opcode*)userData;
    if (child->GetName() == "Operands")
    {
        CodingOpcodeOperandParser p;
        Operand* o = new Operand;
        child->Visit(p, o);
        opcodes->operands.push_back(o);
    }
    return true;
}
bool Parser::CodingOpcodeOperandParser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    Operand* operand = (Operand*)userData;
    if (attrib->GetName() == "Name")
    {
        operand->name = attrib->GetValue();
    }
    else if (attrib->GetName() == "Coding")
    {
        operand->coding = attrib->GetValue();
    }
    else
    {
        operand->values[attrib->GetName()] = attrib->GetValue();
    }
    return true;
}
bool Parser::CodingOpcodeOperandParser::VisitNode(xmlNode& node, xmlNode* child, void* userData) { return true; }
bool Parser::CodingPrefixParser::VisitAttrib(xmlNode& node, xmlAttrib* attrib, void* userData)
{
    Prefix* prefix = (Prefix*)userData;
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
bool Parser::CodingPrefixParser::VisitNode(xmlNode& node, xmlNode* child, void* userData) { return true; }
