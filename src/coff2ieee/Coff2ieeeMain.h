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

#ifndef COFF2IEEEMAIN_H
#define COFF2IEEEMAIN_H

#include "CmdSwitch.h"
#include "CoffHeader.h"
#include <string>
#include <vector>

class ObjFile;
class ObjFactory;
class PEObject;

class Coff2ieeeMain
{
  public:
    Coff2ieeeMain() : mode(unknown) {}
    ~Coff2ieeeMain();

    int Run(int argc, char** argv);

  protected:
    std::string GetOutputName(char* infile) const;
    bool GetMode(char* infile);

  private:
    enum
    {
        unknown = 0,
        obj = 1,
        lib = 2
    } mode;
    static CmdSwitchParser SwitchParser;
    static CmdSwitchBool ShowHelp;
    static CmdSwitchCombineString outputFileSwitch;
    static const char* usageText;
    static const char* helpText;

    std::string outputName;
};
#endif