/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2016, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, 
    with or without modification, are permitted provided that the following 
    conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
    THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
    PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
    OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
    PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
    OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

    contact information:
        email: TouchStone222@runbox.com <David Lindauer>
*/
#ifndef MAKEMAIN_H
#define MAKEMAIN_H
#include "CmdSwitch.h"
#include "Variable.h"
#include "Rule.h"
#include <string>
class MakeMain
{
public:
    MakeMain() : restarts(0) { }
    int Run (int argc, char **argv);
    
protected:
    void Dispatch(const char *data);
    const char * GetStr(const char *data);
    void SetVariable(const std::string &name, const std::string &value, Variable::Origin origin, bool toExport);
    void SetMakeFlags();
    void SetInternalVars();
    void SetupImplicit();
    void LoadEnvironment();
    void LoadCmdDefines();
    void ShowRule(RuleList *ruleList);
    void ShowDatabase();
    void SetTreePath(std::string &files);
private:
    int restarts;
    static CmdSwitchParser switchParser;
    static CmdSwitchCombineString specifiedFiles;
    static CmdSwitchBool displayOnly;
    static CmdSwitchBool touch;
    static CmdSwitchBool query;
    static CmdSwitchCombineString newFiles;
    static CmdSwitchBool keepGoing;
    static CmdSwitchBool ignoreErrors;	
    static CmdSwitchDefine defines;
    static CmdSwitchBool rebuild;
    static CmdSwitchCombineString dir;
    static CmdSwitchBool debug;
    static CmdSwitchBool environOverride;
    static CmdSwitchBool help;
    static CmdSwitchBool help2;
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
    static char *usageText;
    static char *builtinVars;
    static char *builtinRules;
    
    char **argvx;
    int argcx;
    std::string cwd;
};

#endif