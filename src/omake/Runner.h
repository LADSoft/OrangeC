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

#ifndef RUNNER_H
#define RUNNER_H

#include <string>
#include <map>
#include <list>
#include <set>
#include "Spawner.h"
#include "Maker.h"

class Depends;
class Variable;
class Runner
{
  public:
    Runner(bool Silent, bool DisplayOnly, bool IgnoreResults, bool Touch, OutputType Type, bool KeepResponseFiles,
           std::string& FirstGoal, std::map<std::string, std::string>& FilePaths) :
        silent(Silent),
        displayOnly(DisplayOnly),
        ignoreResults(IgnoreResults),
        touch(Touch),
        keepResponseFiles(KeepResponseFiles),
        firstGoal(FirstGoal),
        filePaths(FilePaths),
        outputType(Type)
    {
    }
    void DeleteOne(Depends* depend);
    int RunOne(Depends* depend, EnvironmentStrings& env, bool keepGoing);
    void CancelOne(Depends* depend);

  private:
    OutputType outputType;
    bool silent;
    bool displayOnly;
    bool ignoreResults;
    bool touch;
    bool keepResponseFiles;
    std::string& firstGoal;
    std::map<std::string, std::string>& filePaths;
};
#endif