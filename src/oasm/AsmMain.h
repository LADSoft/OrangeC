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

#ifndef AsmMain_h
#define AsmMain_h

#include "CmdSwitch.h"

class PreProcessor;

class AsmMain
{
  public:
    AsmMain() {}
    virtual ~AsmMain() {}

    int Run(int argc, char* argv[]);

  protected:
    void CheckAssign(std::string& line, PreProcessor& pp);

  private:
    static CmdSwitchParser SwitchParser;
    static CmdSwitchBool ShowHelp;
    static CmdSwitchBool CaseInsensitive;
    static CmdSwitchCombo CreateListFile;
    static CmdSwitchFile File;
    static CmdSwitchBool PreprocessOnly;
    static CmdSwitchOutput OutputFile;
    static CmdSwitchDefine Defines;
    static CmdSwitchCombineString includePath;
    static CmdSwitchBool BinaryOutput;
    static CmdSwitchBool Intel;
    static CmdSwitchBool GAS;
    static CmdSwitchInt ProcessorMode;
    static CmdSwitchBool WarningsAsErrors;
    static CmdSwitchBool NoGasDirectiveWarning;

    static const char* usageText;
    static const char* helpText;
};
#endif