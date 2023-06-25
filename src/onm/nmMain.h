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

#pragma once

#include "ObjTypes.h"
#include "CmdSwitch.h"
#include <string>
#include <vector>

class ObjFile;
class ObjFactory;
class PEObject;

class nmMain
{
  public:
    nmMain() {}

    int Run(int argc, char** argv);

  protected:
  private:
    static CmdSwitchParser SwitchParser;
    static CmdSwitchBool ShowHelp;
    static CmdSwitchBool filePerLine;
    static CmdSwitchBool filePerLine2;
    static CmdSwitchBool allSymbols;
    static CmdSwitchBool demangle;
    static CmdSwitchBool externalSymbols;
    static CmdSwitchBool help;
    static CmdSwitchBool lineNumbers;
    static CmdSwitchBool sortByAddress;
    static CmdSwitchBool sortByAddress2;
    static CmdSwitchBool noSort;
    static CmdSwitchBool reverseSort;
    static CmdSwitchBool archiveIndex;
    static CmdSwitchString radix;
    static CmdSwitchBool undefinedSymbols;
    static CmdSwitchBool reservedP;
    static CmdSwitchBool reservedS;
    static CmdSwitchBool reservedX;
    static const char* usageText;
    static const char* helpText;
};
