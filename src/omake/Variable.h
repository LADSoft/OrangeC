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

#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <map>
#include <list>
#include <memory>
#include <iostream>
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
    Variable(const std::string& name, const std::string& value, Flavor flavor, Origin origin);
    ~Variable() {}
    const std::string& GetName() const { return name; }
    const std::string& GetValue() const { return value; }
    void SetValue(const std::string& Value) { value = Value; }
    void AppendValue(const std::string& value, bool dooverride = false);
    void AssignValue(const std::string& value, Origin origin, bool dooverride = false);
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
    static std::shared_ptr<VariableContainer> Instance();
    ~VariableContainer() {}
    Variable* Lookup(const std::string& name);
    void operator+(Variable* variable);
    void operator+=(Variable* variable) { operator+(variable); }
    void Clear();

    typedef std::map<std::string, std::unique_ptr<Variable>>::iterator iterator;
    const iterator begin() { return variables.begin(); }
    const iterator end() { return variables.end(); }

    typedef std::list<std::unique_ptr<Variable>>::iterator PatternIterator;
    const PatternIterator PatternBegin() { return patternVariables.begin(); }
    const PatternIterator PatternEnd() { return patternVariables.end(); }

  private:
    std::map<std::string, std::unique_ptr<Variable>> variables;
    std::list<std::unique_ptr<Variable>> patternVariables;
    static std::shared_ptr<VariableContainer> instance;
};

#endif
