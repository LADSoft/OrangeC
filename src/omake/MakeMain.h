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

#ifndef MAKEMAIN_H
#define MAKEMAIN_H
#include "CmdSwitch.h"
#include "Variable.h"
#include "Rule.h"
#include "Maker.h"
#include <string>
class MakeMain
{
  public:
    MakeMain() : restarts(0), outputType(o_none), argcx(0) {}
    int Run(int argc, char** argv);

    template <class... Args>
    static void MakeMessage(const std::string& format, Args... args)
    {
        if (printDir.GetValue())
        {
            fprintf(stderr, "[omake(%d): ", makeLevel);
            fprintf(stderr, format.c_str(), args...);
            fprintf(stderr, "]\n");
        }
    }

    static CmdSwitchParser switchParser;
    static CmdSwitchCombineString specifiedFiles;
    static CmdSwitchBool displayOnly;
    static CmdSwitchBool touch;
    static CmdSwitchBool query;
    static CmdSwitchCombineString newFiles;
    static CmdSwitchBool keepGoing;
    static CmdSwitchBool ignoreErrors;
    static CmdSwitchDefine defines;
    static CmdSwitchDefine evals;
    static CmdSwitchBool rebuild;
    static CmdSwitchCombineString dir;
    static CmdSwitchBool debug;
    static CmdSwitchBool environOverride;
    static CmdSwitchBool help;
    static CmdSwitchBool ShowHelp;
    static CmdSwitchCombineString includes;
    static CmdSwitchCombineString oldFiles;
    static CmdSwitchBool showDatabase;
    static CmdSwitchBool noBuiltinRules;
    static CmdSwitchBool noBuiltinVars;
    static CmdSwitchBool silent;
    static CmdSwitchBool cancelKeep;
    static CmdSwitchBool printDir;
    static CmdSwitchBool warnUndef;
    static CmdSwitchBool treeBuild;
    static CmdSwitchBool keepResponseFiles;
    static CmdSwitchInt jobs;
    static CmdSwitchCombineString jobOutputMode;
    static CmdSwitchString jobServer;
    static const char* helpText;
    static const char* builtinVars;
    static const char* builtinRules;

  protected:
    void Dispatch(const char* data);
    const char* GetStr(const char* data);
    void SetVariable(const std::string& name, const std::string& value, Variable::Origin origin, bool toExport);
    void SetMakeFlags();
    void SetInternalVars();
    void SetupImplicit();
    bool LoadJobArgs();
    void LoadEnvironment();
    void LoadCmdDefines();
    void ShowRule(RuleList* ruleList);
    void ShowDatabase();
    void SetTreePath(std::string& files);
    void LoadEquates(int& argc, char** argv);
    void RunEquates();

  private:
    int restarts;
    OutputType outputType;

    static int makeLevel;

    std::unique_ptr<char*[]> argvx;
    int argcx;
    std::string cwd;
    std::deque<std::string> equates;
};

#endif