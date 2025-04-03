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

class libocc
{
  public:
    int Run(int argc, char** argv);

  protected:
    std::string SanitizeExtension(std::string fileName, std::string ext);

  private:
    static const char* usageText;
    static const char* helpText;

    static CmdSwitchParser SwitchParser;
    static CmdSwitchBool Verbose;
    static CmdSwitchCombineString Extract;
    static CmdSwitchCombineString Remove;
    static CmdSwitchString LibPath;
    static CmdSwitchString List;
    static CmdSwitchString Machine;
    static CmdSwitchString DllName;
    static CmdSwitchString NoDefaultLib;
    static CmdSwitchString OutFile;
    static CmdSwitchString Subsytem;
    static CmdSwitchString WarningsAsErrors;
};