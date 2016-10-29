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
#include "Variable.h"

bool Variable::environmentHasPriority = false;
VariableContainer *VariableContainer::instance = NULL;

Variable::Variable(const std::string &Name, const std::string &Value, Flavor oFlavor, Origin oOrigin)
    : name(Name), value(Value), flavor(oFlavor), origin(oOrigin), constant(false), permanent(false)
{
    exportFlag = origin == o_command_line || origin == o_environ || origin == o_environ_override;
}
void Variable::AppendValue(const std::string &Value, bool dooverride)
{
    if (dooverride || (origin != o_command_line && origin != o_environ_override))
    {
        if (!constant)
            value += Value;
    }
}
void Variable::AssignValue(const std::string &Value, Origin oOrigin, bool dooverride)
{
    if (dooverride || (origin != o_command_line && origin != o_environ_override))
    {
        if (!constant)
        {
            value = Value;
            origin = oOrigin;
        }
    }
}

VariableContainer *VariableContainer::Instance()
{
    if (!instance)
        instance = new VariableContainer;
    return instance;
}
Variable *VariableContainer::Lookup(const std::string &name)
{
    Variable *rv = NULL;
    if (name.find_first_of('%') != std::string::npos)
    {
        for (PatternIterator it = PatternBegin(); it != PatternEnd(); ++it)
        {
            if ((*it)->GetName() == name)
            {
                rv = *it;
                break;
            }
        }
    }
    else
    {
        std::map<const std::string *, Variable *, vlt>::iterator it = variables.find(&name);
        if (it != variables.end())
        {
            rv = it->second;
        }
    }
    return rv;
}
void VariableContainer::operator +(Variable *variable)
{
    if (variable->GetName().find_first_of('%') != std::string::npos)
    {
        patternVariables.push_back(variable);
    }
    else
    {
        variables[&variable->GetName()] = variable;
    }
}
void VariableContainer::Clear()
{
    for (PatternIterator it = PatternBegin(); it != PatternEnd(); ++it)
    {
        Variable *p = (*it);
        delete p;
    }
    patternVariables.clear();
    for (iterator it = begin(); it != end(); ++it)
    {
        Variable *p = it->second;
        delete p;
    }
    variables.clear();
}