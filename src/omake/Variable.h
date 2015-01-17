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
#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <map>
#include <list>

class Variable
{
public:
    enum Flavor
    {
        f_undefined,
        f_recursive,
        f_simple
    };
    enum Origin
    {
        o_default,
        o_environ,
        o_environ_override,
        o_file,
        o_command_line,
        o_override,
        o_automatic,
    };
    Variable(const std::string &name, const std::string &value, Flavor flavor, Origin origin);
    ~Variable() { }
    const std::string &GetName() const  { return name; }
    const std::string &GetValue() const { return value; }
    void SetValue(const std::string &Value) { value = Value; }
    void AppendValue(const std::string &value, bool override = false);
    void AssignValue(const std::string &value, Origin origin, bool override = false);
    void SetExport(bool flag) { exportFlag = flag; }
    bool GetExport() const { return exportFlag; }
    Flavor GetFlavor() const { return flavor; }
    Origin GetOrigin() const { return origin; }
    void SetConstant(bool flag) { constant = flag; }
    bool GetConstant() const { return constant; }
    void SetPermanent(bool flag) { permanent = flag; }
    bool GetPermanent() const { return permanent; }
    bool IsPatternedName() const { return name.find_first_of('%') != std::string::npos; }
    static void SetEnvironmentHasPriority(bool flag) { environmentHasPriority = flag; }
    static bool GetEnvironmentHasPriority() { return environmentHasPriority; }
private:
    Flavor flavor;
    Origin origin;
    std::string name;
    std::string value;
    bool constant;
    bool permanent;
    bool exportFlag;
    static bool environmentHasPriority;
};
class Rule;
class VariableContainer
{
public:
    static VariableContainer *Instance();
    ~VariableContainer() { Clear(); }
    Variable *Lookup(const std::string &name);
    void operator +(Variable *variable);
    void operator +=(Variable *variable) { operator +(variable); }
    void Clear();

    struct vlt
    {
        bool operator () (const std::string *one, const std::string *two) const
        {
            return *one < *two;
        }
    } ;
    typedef std::map<const std::string *, Variable *, vlt>::iterator iterator;
    const iterator begin() { return variables.begin(); }
    const iterator end() { return variables.end(); }

    typedef std::list<Variable *>::iterator PatternIterator;
    const PatternIterator PatternBegin() { return patternVariables.begin(); }
    const PatternIterator PatternEnd() { return patternVariables.end(); }
protected:	
    VariableContainer() { }
    
private:
    std::map<const std::string *, Variable *, vlt> variables;
    std::list<Variable *> patternVariables;
    static VariableContainer *instance;
};

#endif
