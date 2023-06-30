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

#ifndef LINKERMAIN_H
#define LINKERMAIN_H

class CmdFiles;

class LinkerMain
{
  public:
    LinkerMain() {}
    virtual ~LinkerMain() {}
    int Run(int argc, char** argv);

  protected:
    const ObjString& GetOutputFile(CmdFiles& files);
    const ObjString& GetMapFile(CmdFiles& files);
    void AddFile(LinkManager& linker, std::string& name);
    void AddFiles(LinkManager& linker, CmdFiles& files);
    void SetDefines(LinkManager& linker);
    std::string SpecFileContents(const std::string& specFile);
    void RewriteArgs(int argc, char** argv);
    bool DoPrintFileName(LinkManager& linker);
    void ParseSpecifiedLibFiles(CmdFiles& files, LinkManager& manager);

  private:
    static CmdSwitchParser SwitchParser;

    static CmdSwitchBool CaseSensitive;
    static CmdSwitchBool ShowHelp;
    static CmdSwitchCombo Map;
    static CmdSwitchBool DebugInfo;
    static CmdSwitchBool DebugInfo2;
    static CmdSwitchFile File;
    static CmdSwitchCombineString Specification;
    static CmdSwitchDefine Defines;
    static CmdSwitchCombineString LibPath;
    static CmdSwitchOutput OutputFile;
    static CmdSwitchCombineString LinkOnly;
    static CmdSwitchBool RelFile;
    static CmdSwitchBool Verbosity;
    static CmdSwitchCombineString OutputDefFile;
    static CmdSwitchCombineString OutputImportLibrary;
    static CmdSwitchCombineString PrintFileName;
    static SwitchConfig TargetConfig;
    static const char* usageText;
    static const char* helpText;
};
#endif