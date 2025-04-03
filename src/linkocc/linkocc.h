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

/*
      /BASE:{address[,size]|@filename,key}
*      /DEBUG[:{FASTLINK|FULL|NONE}]
      /DEF:filename
      /DLL
      /ENTRY:symbol
      /EXPORT:symbol
      /FILEALIGN:#
      /HEAP:reserve[,commit]
      /IMPLIB:filename
*      /LIBPATH:dir
      /MACHINE:{ARM|ARM64|ARM64EC|ARM64X|EBC|X64|X86}
      /MANIFESTFILE:filename
      /MAP[:filename]
      /NODEFAULTLIB[:library]
      /OPT:{ICF[=iterations]|LBR|NOICF|NOLBR|NOREF|REF}
*      /OUT:filename
      /PROFILE
      /RELEASE
      /SAFESEH[:NO]
      /STACK:reserve[,commit]
      /STUB:filename
      /SUBSYSTEM:{BOOT_APPLICATION|CONSOLE|EFI_APPLICATION|
                  EFI_BOOT_SERVICE_DRIVER|EFI_ROM|EFI_RUNTIME_DRIVER|
                  NATIVE|POSIX|WINDOWS|WINDOWSCE}[,#[.##]]
*      /VERBOSE[:{CLR|ICF|INCR|LIB|REF|SAFESEH|UNUSEDDELAYLOAD|UNUSEDLIBS}]
      /VERSION:#[.#]
      /WX[:NO]
      /WX[:nnnn[,nnnn...]]
      */

class linkocc
{
  public:
    int Run(int argc, char** argv);

  protected:
    std::string SanitizeExtension(std::string fileName, std::string ext);

  private:
    static const char* usageText;
    static const char* helpText;

    static CmdSwitchParser SwitchParser;
    static CmdSwitchString prm_debug;
    static CmdSwitchBool prm_verbose;
    static CmdSwitchString prm_output;
    static CmdSwitchCombineString prm_libpath;
    static CmdSwitchString prm_base;
    static CmdSwitchString prm_def;
    static CmdSwitchBool prm_dll;
    static CmdSwitchString prm_entry;
    static CmdSwitchString prm_export;
    static CmdSwitchString prm_filealign;
    static CmdSwitchString prm_implib;
    static CmdSwitchString prm_machine;
    static CmdSwitchString prm_manifestfile;
    static CmdSwitchString prm_map;
    static CmdSwitchString prm_nodefaultlib;
    static CmdSwitchString prm_opt;
    static CmdSwitchBool prm_profile;
    static CmdSwitchBool prm_release;
    static CmdSwitchString prm_safeseh;
    static CmdSwitchString prm_stack;
    static CmdSwitchString prm_stub;
    static CmdSwitchString prm_subsystem;
    static CmdSwitchString prm_version;
    static CmdSwitchString prm_wx;
};