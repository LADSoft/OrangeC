/* (null) */

#include "Variable.h"

bool Variable::environmentHasPriority = false;
VariableContainer *VariableContainer::instance = nullptr;

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
    Variable *rv = nullptr;
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
        auto it = variables.find(&name);
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
    for (auto var : *this)
        delete var.second;
    variables.clear();
}