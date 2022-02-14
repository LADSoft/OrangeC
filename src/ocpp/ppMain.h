/* Software License Agreement
 *
 *     Copyright(C) 1994-2022 David Lindauer, (LADSoft)
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

#ifndef ppMain_h
#define ppMain_h

#include "CmdSwitch.h"
#include "MakeStubs.h"

class ppMain
{
  public:
    int Run(int argc, char* argv[]);

  private:
    static CmdSwitchParser SwitchParser;
    static CmdSwitchBool assembly;
    static CmdSwitchBool disableExtensions;
    static CmdSwitchBool c99Mode;
    static CmdSwitchBool c11Mode;
    static CmdSwitchBool trigraphs;
    static CmdSwitchDefine defines;
    static CmdSwitchDefine undefines;
    static CmdSwitchString includePath;
    static CmdSwitchString CPPsysIncludePath;
    static CmdSwitchString CsysIncludePath;
    static CmdSwitchString errorMax;
    static CmdSwitchFile File;
    static CmdSwitchString outputPath;

    static CmdSwitchBool MakeStubs;
    static CmdSwitchBool MakeStubsUser;
    static CmdSwitchCombineString MakeStubsOutputFile;
    static CmdSwitchBool MakeStubsMissingHeaders;
    static CmdSwitchBool MakeStubsPhonyTargets;
    static CmdSwitchCombineString MakeStubsTargets;
    static CmdSwitchCombineString MakeStubsQuotedTargets;
    static CmdSwitchBool MakeStubsContinue;
    static CmdSwitchBool MakeStubsContinueUser;

    static const char* usageText;
};

#endif