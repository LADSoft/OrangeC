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

#include <stdio.h>

class gccocc
{
  public:
    int Run(int argc, char** argv);

  protected:
    void PutMultiple(FILE* fil, const char* switchName, std::string lst);
    void PutWarnings(FILE* fil);

  private:
    static const char* usageText;
    static const char* helpText;

    static CmdSwitchParser SwitchParser;
    static CmdSwitchBool prm_compileonly;
    static CmdSwitchString prm_directory_options;
    static CmdSwitchBool prm_debug;
    static CmdSwitchString prm_march;
    static CmdSwitchString prm_mtune;
    static CmdSwitchBool prm_verbose;
    static CmdSwitchCombineString prm_libs;
    static CmdSwitchCombineString prm_output;
    static CmdSwitchBool prm_cppmode;
    static CmdSwitchCombineString prm_define;
    static CmdSwitchCombineString prm_libpath;
    static CmdSwitchCombineString prm_cinclude;
    static CmdSwitchString prm_optimize;
    static CmdSwitchBool prm_assemble;
    static CmdSwitchCombineString prm_undefine;
    static CmdSwitchCombineString prmLdCommand;
    static CmdSwitchCombineString prm_warning_and_flags;
    static CmdSwitchBool prm_nostdinc;
    static CmdSwitchBool prm_nostdincpp;
    static CmdSwitchBool prmSyntaxOnly;
    static CmdSwitchBool prmCharIsUnsigned;
    static CmdSwitchCombineString prm_output_def_file;
    static CmdSwitchBool prm_export_all;
    static CmdSwitchBool prmLink;
    static CmdSwitchBool prmDll;
    static CmdSwitchBool prmStatic;
    static CmdSwitchBool prmShared;
    static CmdSwitchBool prmDumpVersion;
    static CmdSwitchBool prmDumpMachine;
    static CmdSwitchCombineString prmPrintFileName;
    static CmdSwitchCombineString prmPrintProgName;
    static CmdSwitchBool prmExceptions;
    static CmdSwitchString prm_std;
    static CmdSwitchBool prmInhibitWarnings;
    static CmdSwitchInt prmMaxErrors;
    static CmdSwitchBool prmPedantic;
    static CmdSwitchBool prmPedantic_errors;
    static CmdSwitchBool prmm32;
    static CmdSwitchBool prmm64;
    static CmdSwitchBool prmwindows;
    static CmdSwitchBool prmconsole;
    static CmdSwitchBool prmunicode;
    static CmdSwitchBool prmPipe;
    static CmdSwitchBool prmStrip;
    static CmdSwitchBool prmPthread;
    static CmdSwitchBool MakeStubsOption;
    static CmdSwitchBool MakeStubsUser;
    static CmdSwitchCombineString MakeStubsOutputFile;
    static CmdSwitchBool MakeStubsMissingHeaders;
    static CmdSwitchBool MakeStubsPhonyTargets;
    static CmdSwitchCombineString MakeStubsTargets;
    static CmdSwitchCombineString MakeStubsQuotedTargets;
    static CmdSwitchBool MakeStubsContinue;
    static CmdSwitchBool MakeStubsContinueUser;
    static CmdSwitchBool StackProtectorBasic;
    static CmdSwitchBool StackProtectorAll;
    static CmdSwitchBool StackProtectorStrong;
    static CmdSwitchBool StackProtectorExplicit;
};