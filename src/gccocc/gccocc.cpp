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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
/* these here are the only 2 function using win32-specific APIs */
#include <process.h>
#include "Utils.h"
#include "CmdSwitch.h"
#include "gccocc.h"

CmdSwitchParser gccocc::SwitchParser;
CmdSwitchBool gccocc::ShowHelp(SwitchParser, '?', false, {"help"});
CmdSwitchBool gccocc::prm_compileonly(SwitchParser, 'c');
CmdSwitchString gccocc::prm_directory_options(SwitchParser, 'i'); // ignored
CmdSwitchBool gccocc::prm_debug(SwitchParser, 'g');
CmdSwitchBool gccocc::prm_verbose(SwitchParser, 'v');
CmdSwitchString gccocc::prm_libs(SwitchParser, 'l', ';');
CmdSwitchString gccocc::prm_output(SwitchParser, 'o');
CmdSwitchBool gccocc::prm_cppmode(SwitchParser, 'E');
CmdSwitchCombineString gccocc::prm_define(SwitchParser, 'D', ';');
CmdSwitchCombineString gccocc::prm_libpath(SwitchParser, 'L', ';');
CmdSwitchCombineString gccocc::prm_cinclude(SwitchParser, 'I', ';');
CmdSwitchString gccocc::prm_optimize(SwitchParser, 'O');
CmdSwitchBool gccocc::prm_assemble(SwitchParser, 'S');
CmdSwitchCombineString gccocc::prm_undefine(SwitchParser, 'U', ';');
CmdSwitchString gccocc::prm_warning_and_flags(SwitchParser, 'W');
CmdSwitchBool gccocc::prm_nostdinc(SwitchParser, 0, false, {"nostdinc"});
CmdSwitchBool gccocc::prm_nostdincpp(SwitchParser, 0, false, {"nostdinc++"});
CmdSwitchBool gccocc::prmSyntaxOnly(SwitchParser, 0, false, {"fsyntax-only"});  // doesn't do anything yet
CmdSwitchBool gccocc::prmCharIsUnsigned(SwitchParser, 0, false, {"funsigned-char"});
CmdSwitchCombineString gccocc::prm_output_def_file(SwitchParser, 0, 0, {"output-def"});
CmdSwitchBool gccocc::prm_export_all(SwitchParser, 0, false, {"export-all-symbols"});
CmdSwitchBool gccocc::prmLink(SwitchParser, 0, 0, {"link"});
CmdSwitchBool gccocc::prmDll(SwitchParser, 0, 0, {"dll"});
CmdSwitchBool gccocc::prmStatic(SwitchParser, 0, 0, {"static"});
CmdSwitchBool gccocc::prmShared(SwitchParser, 0, 0, {"shared"});
CmdSwitchBool gccocc::prmDumpVersion(SwitchParser, 0, 0, {"dumpversion"});
CmdSwitchBool gccocc::prmDumpMachine(SwitchParser, 0, 0, {"dumpmachine"});
CmdSwitchCombineString gccocc::prmPrintFileName(SwitchParser, 0, 0, {"print-file-name"});
CmdSwitchCombineString gccocc::prmPrintProgName(SwitchParser, 0, 0, {"print-prog-name"});


const char* gccocc::helpText =
    "[options] files...\n"
    "\n"
    "   -c              compile only\n"
    "   -g              compile for debug\n"
    "   -i              reserved for compatibility\n"
    "   -lxxx           specify library\n"
    "   -o xxx          set output file\n"
    "   -v              verbose\n"
    "   -D xxx          define something\n"
    "   -E              compile as c++\n"
    "   -I xxx          specify include directory\n"
    "   -L xxx          specify library directory\n"
    "   -Ox             specify optimize level\n"
    "   -S              generate assembler code\n"
    "   -Uxxx           undefine something\n"
    "   -W              show warnings\n"
    "   -Wno            no warnings\n"
    "   -Wa,xxx         specify an assembler flag\n"
    "   -Wl,xxx         specify a linker flag\n"
    "   -V, --version   show version information\n"
    "   --nologo        no logo\n"
    "   /?, --help      this text\n"
    "Other options\n"
    "   -dll            generate a DLL\n"
    "   -dumpmachine    dump machine string\n"
    "   -dumpversion    dump compiler version\n"
    "   -export-all-symbols force export of symbols\n"
    "   -fsyntax-only   compile, don't generate output\n"
    "   -funsigned-char treat char as unsigned\n"
    "   -link           reserved for compitiblity\n"
    "   -nostdinc       disable c language system include files\n"
    "   -nostdinc++     disable c++ language system include files\n"
    "   -output-def xxxx  create a .def file\n"  
    "   -print-file-name=xxx  print the full path of a library\n"
    "   -print-prog-name=xxx  print the full path of one of the executables\n"
    "   -shared         generate a DLL\n"
    "   -static         generate an executable\n"
    "\n"
    "\nTime: " __TIME__ "  Date: " __DATE__;
const char* gccocc::usageText = "[options] files...";

int main(int argc, char** argv)
{
    gccocc gcc;
    return gcc.Run(argc, argv);
}

void gccocc::PutMultiple(FILE* fil, const char* switchName, std::string lst)
{
    auto splt = Utils::split(lst,';');
    for (int i=0; i < splt.size(); i++)
        fprintf(fil, " -%s%s\n", switchName, splt[i].c_str());
}
int gccocc::Run(int argc, char** argv) 
{
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");
    CmdSwitchFile internalConfig(SwitchParser);
    std::string configName = Utils::QualifiedFile(argv[0], ".cfg");
    std::fstream configTest(configName, std::ios::in);
    if (!configTest.fail())
    {
        configTest.close();
        if (!internalConfig.Parse(configName.c_str()))
            Utils::fatal("Corrupt configuration file");
    }
    if (!SwitchParser.Parse(&argc, argv) || argc < 2)
    {
        if (argc < 2 && !prmDumpVersion.GetExists() && !prmDumpMachine.GetExists() && !prmPrintFileName.GetExists() && !prmPrintProgName.GetExists() && !ShowHelp.GetExists())
            Utils::usage(argv[0], usageText);
    }	
    if (ShowHelp.GetExists())
        Utils::usage(argv[0], helpText);
    std::string tempName;
    FILE* fil = Utils::TempName(tempName);
    if (prm_compileonly.GetValue())
        fputs(" -c", fil);
    if (prm_assemble.GetValue())
        fputs(" -S", fil);
    if (prm_debug.GetValue())
        fputs(" -g", fil);
    if (prm_verbose.GetExists())
        fputs(" -yyyy", fil);
    if (prm_output.GetExists())
        fprintf(fil, " \"-o%s\"", prm_output.GetValue().c_str());
    if (prm_cppmode.GetValue())
        fputs(" -x c++", fil);
    if (prm_nostdinc.GetValue())
        fputs(" -nostdinc", fil);
    if (prm_nostdincpp.GetValue())
        fputs(" -nostdinc++", fil);
    if (prmSyntaxOnly.GetValue())
        fputs(" -fsyntax-only", fil);
    if (prmCharIsUnsigned.GetValue())
        fputs(" -funsigned-char", fil);
    if (prmLink.GetValue())
        fputs(" -link", fil);
    if (prmDll.GetValue())
        fputs(" -dll", fil);
    if (prmShared.GetValue())
        fputs(" -shared", fil);
    if (prmDumpVersion.GetValue())
        fputs(" -dumpversion", fil);
    if (prmDumpMachine.GetValue())
        fputs(" -dumpmachine", fil);
    if (prm_export_all.GetValue())
        fputs(" -export-all-symbols", fil);
    if (prm_optimize.GetExists())
        fprintf(fil, " -O%s", prm_optimize.GetValue().c_str());
    if (prm_warning_and_flags.GetExists())
    {
        if (prm_warning_and_flags.GetValue() == "")
            fputs(" /w+", fil);
        else if (prm_warning_and_flags.GetValue() == "no")
            fputs(" /E-", fil);
        else if (prm_warning_and_flags.GetValue().size() >= 3)
        {
            if (prm_warning_and_flags.GetValue()[1] == ',')
            {
                if (prm_warning_and_flags.GetValue()[0] == 'l')
                   fprintf(fil, " \"-pl%s\"", prm_warning_and_flags.GetValue().substr(2).c_str());
                else if (prm_warning_and_flags.GetValue()[0] == 'a')
                   fprintf(fil, " \"-pa%s\"", prm_warning_and_flags.GetValue().substr(2).c_str());
                else
                   Utils::fatal("unknown warning flags");
            }
            else
               Utils::fatal("unknown warning flags");
        }
        else
            Utils::fatal("unknown warning flags");
    }
    if (prm_output_def_file.GetExists())
        fprintf(fil, " -output-def %s", prm_output_def_file.GetValue().c_str());
    if (prmPrintFileName.GetExists())
        fprintf(fil, " -print-file-name=%s", prmPrintFileName.GetValue().c_str());
    if (prmPrintProgName.GetExists())
        fprintf(fil, " -print-prog-name=%s", prmPrintProgName.GetValue().c_str());
    if (prm_undefine.GetExists())
        PutMultiple(fil, "U", prm_undefine.GetValue());
    if (prm_define.GetExists())
        PutMultiple(fil, "D", prm_define.GetValue());
    if (prm_cinclude.GetExists())
        PutMultiple(fil, "I", prm_cinclude.GetValue());
    if (prm_libpath.GetExists())
        PutMultiple(fil, "L", prm_libpath.GetValue());
    for (int i=1; i < argc; i++)
       fprintf(fil, " \"%s\"", argv[i]);
    auto libs = Utils::split(prm_libs.GetValue(),';');
    for (int i=0; i < libs.size(); i++)
    {
        if (libs[i].find(".") == std::string::npos)
           libs[i] += ".lib";
        fprintf(fil, " \"%s\"", libs[i].c_str());
     
    } 
    fclose(fil);
    auto rv = Utils::ToolInvoke("occ.exe", nullptr, " -! @%s", tempName.c_str());
    unlink(tempName.c_str());
    return rv;
}

