/* Software License Agreement
 * 
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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
    NetLinkMain() : 
        mainContainer(nullptr),
        mainNameSpace(nullptr), 
        mainClass(nullptr), 
        hasEntryPoint(false),
        peLib(nullptr),
        mainSym(nullptr),
        currentMethod(nullptr)
    { }

    int Run(int argc, char **argv);
protected:

    const std::string &GetAssemblyName(CmdFiles &files);
    const std::string &GetOutputFile(CmdFiles &files);
    bool LoadImage(CmdFiles &files);
    bool Validate();
    bool CreateExecutable(CmdFiles &files);

    DotNetPELib::MethodSignature *LookupSignature(const char * name);
    DotNetPELib::Field *LookupField(const char *name);
    DotNetPELib::MethodSignature *LookupManagedSignature(const char *name);
    DotNetPELib::Field *LookupManagedField(const char *name);
    void MainLocals(void);
    void MainInit(void);
    void dumpInitializerCalls(std::list<DotNetPELib::MethodSignature *>&);
    void dumpCallToMain(void);
    void dumpGlobalFuncs();
    bool AddRTLThunks();

    // pelib traverse callback
    virtual bool EnterNamespace(const DotNetPELib::Namespace *nmspc) override;
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
    std::list<DotNetPELib::MethodSignature *> initializers, startups;
    std::list<DotNetPELib::MethodSignature *> destructors, rundowns;
    std::list<DotNetPELib::MethodSignature *> stringinitializers;

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
    static CmdSwitchBool CManaged;
    static CmdSwitchBool NoDefaultlibs;
    static CmdSwitchBool WeedPInvokes;

    static const char *usageText;
};
#endif
