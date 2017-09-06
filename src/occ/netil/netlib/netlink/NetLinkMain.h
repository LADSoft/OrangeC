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
#ifndef NetLinkMain_H
#define NetLinkMain_H
#include "CmdFiles.h"
#include "CmdSwitch.h"
#include "Utils.h"
#include <string>
#include "DotNetPELib.h"
class NetLinkMain : public DotNetPELib::Callback
{
public:
    NetLinkMain() : mainNameSpace(nullptr), mainClass(nullptr), hasEntryPoint(false) { }

    int Run(int argc, char **argv);
protected:

    const std::string &GetAssemblyName(CmdFiles &files);
    const std::string &GetOutputFile(CmdFiles &files);
    bool LoadImage(CmdFiles &files);
    bool NetLinkMain::Validate();
    bool CreateExecutable(CmdFiles &files);

    DotNetPELib::MethodSignature *LookupSignature(char * name);
    DotNetPELib::Field *LookupField(char *name);
    void MainLocals(void);
    void MainInit(void);
    void dumpInitializerCalls(std::list<DotNetPELib::MethodSignature *>&);
    void dumpCallToMain(void);
    void dumpGlobalFuncs();
    bool AddRTLThunks();

    // pelib traverse callback
    virtual bool NetLinkMain::EnterNamespace(const DotNetPELib::Namespace *nmspc) override;
    virtual bool EnterClass(const DotNetPELib::Class *method) override;
    virtual bool EnterMethod(const DotNetPELib::Method *) override;
private:

    DotNetPELib::PELib *peLib;
    std::vector<DotNetPELib::Local *> localList;
    DotNetPELib::MethodSignature *mainSym;
    DotNetPELib::Method *currentMethod;
    const DotNetPELib::Namespace *mainNameSpace;
    const DotNetPELib::Class *mainClass;
    DotNetPELib::DataContainer *mainContainer;
    std::list<DotNetPELib::MethodSignature *> initializers;
    std::list<DotNetPELib::MethodSignature *> destructors;

    std::string namespaceAndClass;
    bool hasEntryPoint;

    static CmdSwitchParser SwitchParser;
    static CmdSwitchString StrongName;
    static CmdSwitchBool LibraryFile;
    static CmdSwitchBool AssemblyFile;
    static CmdSwitchBool GUIApp;
    static CmdSwitchBool Managed;
    static CmdSwitchOutput AssemblyName;
    static CmdSwitchString AssemblyVersion;
    static CmdSwitchFile File;

    static char *usageText;
};
#endif
