/* Software License Agreement
 * 
 *     Copyright(C) 1994-2023 David Lindauer, (LADSoft)
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
 */

#ifndef INCLUDE_H
#define INCLUDE_H

#include "Maker.h"
#include <string>
#include <list>
#include <set>
#include <stack>

class Include
{
  public:
    static std::shared_ptr<Include> Instance();
    ~Include() {}
    void Clear();

    bool AddFileList(const std::string& name, bool ignoreOk = false, bool MakeFile = false);
    bool MakeMakefiles(bool Silent, OutputType outputType, bool& didSomething);

    typedef std::list<std::string>::iterator iterator;
    iterator begin() { return files.begin(); }
    iterator end() { return files.end(); }

  protected:
    Include() {}
    bool Parse(const std::string& name, bool ignoreOk, bool MakeFile = false);

  private:
    std::list<std::string> files;
    std::set<std::string> ignoredFiles;

    std::stack<std::string> currentPath;
    static std::shared_ptr<Include> instance;
};
#endif
