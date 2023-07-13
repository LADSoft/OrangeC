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
#include "Utils.h"
#include "ToolChain.h"
#include "CmdSwitch.h"
#include "gccocc.h"
#include "../exefmt/PEHeader.h"

#ifdef HAVE_UNISTD_H
#    include <unistd.h>
#    define CONSOLE_DEVICE "/dev/tty"
#else
#    include <io.h>
#    define CONSOLE_DEVICE "con:"
#endif

CmdSwitchParser gccocc::SwitchParser;
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
CmdSwitchCombineString gccocc::prmLdCommand(SwitchParser, 0, ';', {"Wl"});
CmdSwitchCombineString gccocc::prm_warning_and_flags(SwitchParser, 'W', ';');
CmdSwitchString gccocc::prm_std(SwitchParser,0, 0, {"std"});
CmdSwitchBool gccocc::prm_nostdinc(SwitchParser, 0, false, {"nostdinc"});
CmdSwitchBool gccocc::prm_nostdincpp(SwitchParser, 0, false, {"nostdinc++"});
CmdSwitchBool gccocc::prmSyntaxOnly(SwitchParser, 0, false, {"fsyntax-only"});
CmdSwitchBool gccocc::prmCharIsUnsigned(SwitchParser, 0, false, {"funsigned-char"});
CmdSwitchBool gccocc::prmExceptions(SwitchParser, 0, false, {"fexceptions"});
CmdSwitchCombineString gccocc::prm_output_def_file(SwitchParser, 0, 0, {"output-def"});
CmdSwitchBool gccocc::prm_export_all(SwitchParser, 0, false, {"export-all-symbols"});
CmdSwitchBool gccocc::prmLink(SwitchParser, 0, false, {"link"});
CmdSwitchBool gccocc::prmDll(SwitchParser, 0, false, {"dll", "mdll"});
CmdSwitchBool gccocc::prmStatic(SwitchParser, 0, false, {"static"});
CmdSwitchBool gccocc::prmShared(SwitchParser, 0, false, {"shared"});
CmdSwitchBool gccocc::prmDumpVersion(SwitchParser, 0, false, {"dumpversion"});
CmdSwitchBool gccocc::prmDumpMachine(SwitchParser, 0, false, {"dumpmachine"});
CmdSwitchString gccocc::prm_mtune(SwitchParser, 0, false, {"mtune"});
CmdSwitchString gccocc::prm_march(SwitchParser, 0, false, {"march"});
CmdSwitchCombineString gccocc::prmPrintFileName(SwitchParser, 0, 0, {"print-file-name"});
CmdSwitchCombineString gccocc::prmPrintProgName(SwitchParser, 0, 0, {"print-prog-name"});
CmdSwitchBool gccocc::prmInhibitWarnings(SwitchParser, 'w');
CmdSwitchInt  gccocc::prmMaxErrors(SwitchParser, 0, 24, 1,999, {"fmax-errors"});
CmdSwitchBool gccocc::prmPedantic(SwitchParser, 0, 0, {"pedantic", "pedantic-errors"});
CmdSwitchBool gccocc::prmm32(SwitchParser, 0, false, {"m32"});
CmdSwitchBool gccocc::prmm64(SwitchParser, 0, false, {"m64"});
CmdSwitchBool gccocc::prmwindows(SwitchParser, 0, false, {"mwindows"});
CmdSwitchBool gccocc::prmconsole(SwitchParser, 0, false, {"mconsole"});
CmdSwitchBool gccocc::prmunicode(SwitchParser, 0, false, {"municode"});
CmdSwitchBool gccocc::prmPipe(SwitchParser, 0, false, {"pipe"});
CmdSwitchBool gccocc::prmStrip(SwitchParser, 's');
CmdSwitchBool gccocc::prmPthread(SwitchParser, 0, false, {"pthread"});
CmdSwitchBool gccocc::MakeStubsOption(SwitchParser, 0, 0, {"M"});
CmdSwitchBool gccocc::MakeStubsUser(SwitchParser, 0, 0, {"MM"});
CmdSwitchCombineString gccocc::MakeStubsOutputFile(SwitchParser, 0, ';', {"MF"});
CmdSwitchBool gccocc::MakeStubsMissingHeaders(SwitchParser, 0, 0, {"MG"});
CmdSwitchBool gccocc::MakeStubsPhonyTargets(SwitchParser, 0, 0, {"MP"});
CmdSwitchCombineString gccocc::MakeStubsTargets(SwitchParser, 0, ';', {"MT"});
CmdSwitchCombineString gccocc::MakeStubsQuotedTargets(SwitchParser, 0, ';', {"MQ"});
CmdSwitchBool gccocc::MakeStubsContinue(SwitchParser, 0, 0, {"MD"});
CmdSwitchBool gccocc::MakeStubsContinueUser(SwitchParser, 0, 0, {"MMD"});

const char* gccocc::helpText =
    "[options] files...\n"
    "\n"
    "   -c              compile only\n"
    "   -g              compile for debug\n"
    "   -i              reserved for compatibility\n"
    "   -lxxx           specify library\n"
    "   -o xxx          set output file\n"
    "   -s              ignored\n"
    "   -v              ignored\n"
    "   -D xxx          define something\n"
    "   -E              preprocess\n"
    "   -I xxx          specify include directory\n"
    "   -L xxx          specify library directory\n"
    "   -M              basic dependency generation\n"
    "   -MM             basic dependency generation, user files only\n"
    "   -MF file        basic dependency generation, specify output file\n"
    "   -MG             missing headers as dependencies\n"
    "   -MP             dependency generation, add phony targets\n"
    "   -MT target      dependency generation, add target\n"
    "   -MQ target      dependency generation, add target, quote special characters\n"
    "   -MD             basic dependency generation and continue\n"
    "   -MMD            basic dependency generation and continue, user files only\n"
    "   -Ox             specify optimize level\n"
    "   -S              generate assembler code\n"
    "   -Uxxx           undefine something\n"
    "   -w              don't show warnings\n"
    "   -Wno            don't show warnings\n"
    "   -Wall           show all warnings\n"
    "   -Wextra         show all warnings\n"
    "   -Werror         display warnings as errors\n"
    "   -Wfatal-errors  stop after first error\n"
    "   -Wxxxxxxxxxx    ignored\n"
    "   -V, --version   show version information\n"
    "   --nologo        no logo\n"	
    "   /?, --help      this text\n"
    "Other options\n"
    "   -dll            generate a DLL\n"
    "   -dumpmachine    dump machine string\n"
    "   -dumpversion    dump compiler version\n"
    "   -export-all-symbols force export of symbols\n"
    "   -fexceptions    ignored\n"
    "   -fmax-errors=n  show a maximum of 'n' errors\n"
    "   -fsyntax-only   compile, don't generate output\n"
    "   -funsigned-char treat char as unsigned\n"
    "   -link           reserved for compitiblity\n"
    "   -m32            ignored\n"
    "   -m64            ignored\n"
    "   -mconsole       generate a console program\n"
    "   -mdll           generate a DLL\n"
    "   -mwindows       generate a gui program\n"
    "   -municode       use the unicode version of the headers\n"
    "   -nostdinc       disable c language system include files\n"
    "   -nostdinc++     disable c++ language system include files\n"
    "   -output-def xxxx  create a .def file\n"  
    "   -pedantic       ignored\n"
    "   -pedantic-errors ignored\n"
    "   -pipe           ignored\n"
    "   -print-file-name=xxx  print the full path of a library\n"
    "   -print-prog-name=xxx  print the full path of one of the executables\n"
    "   -pthread        ignored\n"
    "   -shared         generate a DLL instead of an executable\n"
    "   -static         generate a static library instead of an executable\n"
    "   -march=xxx      ignored\n"
    "   -mtune=xxx      ignored\n"
    "   --Wl,dll        make a dll\n"
    "   --Wl,--out-implib=xxx generate an import library when creating a dll\n"
    "   --Wl,--output-def=xxx generate a def file when creating a dll\n"
    "   --Wl,substem=xxx set the windows subsystem\n"
    "   other -Wl,xxx=yyy switches are ignored\n"
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
void gccocc::PutWarnings(FILE* fil)
{
    if (prmMaxErrors.GetExists())
    {
        fprintf(fil,  " /E%d", prmMaxErrors.GetValue());
    }
    if (prmInhibitWarnings.GetValue())
    {
        fputs(" /w", fil);
    }
    else if (prm_warning_and_flags.GetExists())
    {
        auto warnings = Utils::split(prm_warning_and_flags.GetValue());
        if (!warnings.size())
            fputs(" /w+", fil);
        else
        {
            for (auto w : warnings)
            {
                if (w == "no")
                   fputs(" /w", fil);
                else if (w == "error")
                   fputs(" /wx", fil);
                else if (w == "all" || w == "extra")
                   fputs(" /w+", fil);
                else if (w == "fatal-errors")
                {
                   fputs(" /E1", fil);
                }
                else
                {
                    // anything else is ignored
                }                   
            }
        }
    }
}
int gccocc::Run(int argc, char** argv) 
{
    #define XPE_SUBSYS_DLL -1
    int subsystem = PE_SUBSYS_CONSOLE; 
    auto files = ToolChain::StandardToolStartup(SwitchParser, argc, argv, usageText, helpText, [this]() { return prm_cppmode.GetValue();});
    if (files.size() < 2 && !prmDumpVersion.GetExists() && !prmDumpMachine.GetExists() && !prmPrintFileName.GetExists() &&
        !prmPrintProgName.GetExists())
        ToolChain::Usage(usageText);
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
        fputs(" -i -o" CONSOLE_DEVICE, fil);
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
    if (prmDumpVersion.GetValue())
        fputs(" -dumpversion", fil);
    if (prmDumpMachine.GetValue())
        fputs(" -dumpmachine", fil);
    if (prm_export_all.GetValue())
        fputs(" -export-all-symbols", fil);
    if (prm_std.GetExists())
    {
        fputs(" -std=", fil);
        fputs(prm_std.GetValue().c_str(), fil);
    }
    if (prm_optimize.GetExists())
    {
        char ch = '1';
        if (!prm_optimize.GetValue().empty())
            ch = prm_optimize.GetValue()[0];
        switch (ch)
        {
             default:
                 Utils::Fatal("Unknown optimizer flag");
                 break;
             case '1':
             case '2':
             case '3':
                 fputs(" -O2", fil);
                 break;
             case 's':
                 fputs(" -O1", fil);
                 break;
        }
    }
    else
    {
         fputs(" -O-", fil);
    }
    if (prmShared.GetValue())
        subsystem = XPE_SUBSYS_DLL;
    if (prmStatic.GetValue())
        subsystem = PE_SUBSYS_CONSOLE;
    if (prmconsole.GetValue())
        subsystem = PE_SUBSYS_CONSOLE;
    if (prmwindows.GetValue())
        subsystem = PE_SUBSYS_WINDOWS;
    if (prmDll.GetValue())
        subsystem = XPE_SUBSYS_DLL;
    auto ldstrings = prmLdCommand.GetValue();
    for (auto&& ld : Utils::split(ldstrings))
    {
        auto eq = ld.find_first_of('=');
        std::string key, val;
        if (eq == std::string::npos)
        {
            key = ld;
        }
        else
        {
            key = ld.substr(0, eq);
            val = ld.substr(eq + 1);
        }
        if (key == "--output-def")
            prm_output_def_file.SetValue(val);
        else if (key == "--out-implib")
            fprintf(fil, " --out-implib %s", val.c_str());
        else if (key == "--dll")
            subsystem = XPE_SUBSYS_DLL;
        else if (key == "--subsystem" && val.size())
        {
            if (isdigit(val[0]))
                 subsystem = atoi(val.c_str());
            else if (val == "windows")
                 subsystem = PE_SUBSYS_WINDOWS;
            else if (val == "console")
                 subsystem = PE_SUBSYS_CONSOLE;
            else if (val == "native")
                 subsystem = PE_SUBSYS_NATIVE;
        }
        else
        {
            // ignore
        }
    }
    switch (subsystem)
    {
         case XPE_SUBSYS_DLL:
             fputs(" -Wd", fil);
             break;
         case PE_SUBSYS_WINDOWS:
             fputs(" -Wg", fil);
             break;
         case PE_SUBSYS_CONSOLE:
         default:
             fputs(" -Wc", fil);
             break;
    }
    if (MakeStubsOption.GetValue())
        fputs(" -M", fil);
    if (MakeStubsUser.GetValue())
        fputs(" -MM", fil);
    if (MakeStubsOutputFile.GetExists())
        fprintf(fil, " -MF %s", MakeStubsOutputFile.GetValue().c_str());
    if (MakeStubsMissingHeaders.GetValue())
        fputs(" -MG", fil);
    if (MakeStubsPhonyTargets.GetValue())
        fputs(" -MP", fil);
    if (MakeStubsTargets.GetExists())
    {
        auto strings = Utils::split(MakeStubsTargets.GetValue());
        for (auto a : strings)
            fprintf(fil, " -MT %s", a.c_str());
    }
    if (MakeStubsQuotedTargets.GetExists())
    {
        auto strings = Utils::split(MakeStubsQuotedTargets.GetValue());
        for (auto a : strings)
            fprintf(fil, " -MQ %s", a.c_str());
    }
    if (MakeStubsContinue.GetValue())
        fputs(" -MD", fil);
    if (MakeStubsContinueUser.GetValue())
        fputs(" -MMD", fil);
    PutWarnings(fil);
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
    if (prmunicode.GetValue())
        fputs(" -DUNICODE", fil);
    if (prm_cinclude.GetExists())
        PutMultiple(fil, "I", prm_cinclude.GetValue());
    if (prm_libpath.GetExists())
        PutMultiple(fil, "L", prm_libpath.GetValue());
    for (int i=1; i < files.size(); i++)
        fprintf(fil, " \"%s\"", files[i].c_str());
    auto libs = Utils::split(prm_libs.GetValue(),';');
    for (int i=0; i < libs.size(); i++)
    {
        if (libs[i].find(".") == std::string::npos)
           libs[i] += ".lib";
        fprintf(fil, " \"%s\"", libs[i].c_str());
     
    } 
    fclose(fil);
    auto rv = ToolChain::ToolInvoke("occ.exe", nullptr, " -! @%s", tempName.c_str());
    unlink(tempName.c_str());
    return rv;
}

