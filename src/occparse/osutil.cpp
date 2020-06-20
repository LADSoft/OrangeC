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

#include "compiler.h"
#include <signal.h>
#include <setjmp.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <string>
#include <deque>
#include "PreProcessor.h"
#include "Utils.h"
#include "CmdFiles.h"
#include "CmdSwitch.h"
#include <vector>
#include <sstream>
#include <iostream>
#include "Utils.h"
#include "ccerr.h"
#include "config.h"
#include "occparse.h"
#include "ildata.h"
#include "using.h"
#include "configx86.h"
#include "OptUtils.h"
#include "configmsil.h"
#include "initbackend.h"

namespace Optimizer
{
extern bool doBackendInit;
}
namespace Parser
{

#include "../version.h"
#if defined(_MSC_VER) || defined(BORLAND) || defined(__ORANGEC__)
#    include <io.h>
#endif

#if defined(WIN32) || defined(MICROSOFT)
extern "C"
{
    char* __stdcall GetModuleFileNameA(void* handle, char* buf, int size);
}
#endif

Optimizer::LIST* clist = 0;
int showVersion = false;
std::string bePostFile;

std::deque<DefValue> defines;

CmdSwitchParser switchParser;
CmdSwitchBool prm_c89(switchParser, '8');
CmdSwitchBool prm_c99(switchParser, '9');
CmdSwitchBool prm_c11(switchParser, '1');
CmdSwitchBool prm_ansi(switchParser, 'A');
CmdSwitchBool prm_errfile(switchParser, 'e');
CmdSwitchBool prm_cppfile(switchParser, 'i');
CmdSwitchBool prm_quiet(switchParser, 'Q');
CmdSwitchBool prm_icdfile(switchParser, 'Y');
CmdSwitchBool prm_trigraph(switchParser, 'T');
CmdSwitchBool prm_debug(switchParser, 'v');
CmdSwitchBool prm_debug2(switchParser, 'g');
CmdSwitchBool prm_makestubs(switchParser, 'M');
CmdSwitchBool prm_compileonly(switchParser, 'c');
CmdSwitchString prm_assemble(switchParser, 'S');
CmdSwitchBool prm_xcept(switchParser, 'x');
CmdSwitchBool prm_viaassembly(switchParser, '#');
CmdSwitchInt prm_stackalign(switchParser, 's', 16, 0, 2048);
CmdSwitchString prm_error(switchParser, 'E');
CmdSwitchString prm_define(switchParser, 'D', ';');
CmdSwitchString prm_undefine(switchParser, 'U', ';');
CmdSwitchFile prm_file(switchParser, '@');
CmdSwitchString prm_codegen(switchParser, 'C', ';');
CmdSwitchString prm_optimize(switchParser, 'O', ';');
CmdSwitchString prm_verbose(switchParser, 'y');
CmdSwitchString prm_warning(switchParser, 'w', ';');
CmdSwitchCombineString prm_output(switchParser, 'o');
CmdSwitchCombineString prm_tool(switchParser, 'p', ';');

CmdSwitchCombineString prm_library(switchParser, 'l', ';');

CmdSwitchCombineString prm_cinclude(switchParser, 'I', ';');
CmdSwitchCombineString prm_sysinclude(switchParser, 'z', ';');
CmdSwitchCombineString prm_libpath(switchParser, 'L', ';');
CmdSwitchString prm_pipe(switchParser, 'P', ';');
CmdSwitchCombineString prm_output_def_file(switchParser, 0, 0, "output-def");
CmdSwitchBool prm_export_all(switchParser, 0, false, "export-all-symbols");

CmdSwitchBool prm_msil_noextensions(switchParser, 'd');
CmdSwitchString prm_msil_strongnamekeyfile(switchParser, 'K');
CmdSwitchString prm_msil_namespace(switchParser, 'N');
CmdSwitchString prm_msil_version(switchParser, 'V');
CmdSwitchCombineString prm_architecture(switchParser, 0, 0, "architecture");

CmdSwitchString prm_Winmode(switchParser, 'W');
CmdSwitchCombineString OutputDefFile(switchParser, 0, 0, "output-def");
CmdSwitchBool ExportAll(switchParser, 0, false, "export-all-symbols");

CmdSwitchString AssemblerExtension(switchParser, 'a');

static std::string firstFile;
enum e_lk getDefaultLinkage()
{
    switch (Optimizer::architecture)
    {
        case ARCHITECTURE_X86:
            return Optimizer::cparams.prm_lscrtdll ? lk_import : lk_none;
        default:
            return lk_none;
    }
}

/*
 * Return path of EXE file
 */
void EXEPath(char* buffer, char* filename)
{
    char* temp;
    strcpy(buffer, filename);
    if ((temp = strrchr(buffer, '\\')) != 0)
        *(temp + 1) = 0;
    else
        buffer[0] = 0;
}

/*-------------------------------------------------------------------------*/ /*
                                                                               * Pull the next path off the path search list
                                                                               */
static const char* parsepath(const char* path, char* buffer)
{
    const char* pos = path;

    /* Quit if hit a ';' */
    while (*pos)
    {
        if (*pos == ';')
        {
            pos++;
            break;
        }
        *buffer++ = *pos++;
    }
    *buffer = 0;

    /* Return a null pointer if no more data */
    if (*pos)
        return (pos);

    return (0);
}

static std::vector<std::string> split(std::string strToSplit, char delimeter = ';')
{
    std::stringstream ss(strToSplit);
    std::string item;
    std::vector<std::string> splittedStrings;
    while (std::getline(ss, item, delimeter))
    {
        splittedStrings.push_back(item);
    }
    return splittedStrings;
}
void optimize_setup(char select, const char* string)
{
    (void)select;
    if (!*string || (*string == '+' && string[1] == '\0'))
    {
        Optimizer::cparams.prm_optimize_for_speed = true;
        Optimizer::cparams.prm_optimize_for_size = false;
        Optimizer::cparams.prm_debug = false;
    }
    else if (*string == '-' && string[1] == '\0')
        Optimizer::cparams.prm_optimize_for_speed = Optimizer::cparams.prm_optimize_for_size =
            Optimizer::cparams.prm_optimize_float_access = false;
    else
    {
        Optimizer::cparams.prm_debug = false;
        if (*string == '0')
        {
            Optimizer::cparams.prm_optimize_for_speed = Optimizer::cparams.prm_optimize_for_size = 0;
        }
        else if (*string == 'f')
            Optimizer::cparams.prm_optimize_float_access = true;
        else if (*string == '2')
        {
            Optimizer::cparams.prm_optimize_for_speed = true;
            Optimizer::cparams.prm_optimize_for_size = false;
        }
        else if (*string == '1')
        {
            Optimizer::cparams.prm_optimize_for_speed = false;
            Optimizer::cparams.prm_optimize_for_size = true;
        }
    }
}
/*-------------------------------------------------------------------------*/
static int parseCodegen(bool v, const char* string)
{
    switch (Optimizer::architecture)
    {
        case ARCHITECTURE_X86:
            return Optimizer::parse_codegen(v, string);
        case ARCHITECTURE_MSIL:
            return Optimizer::parse_msil_codegen(v, string);
        default:
            break;
    }
    return 0;
}

void codegen_setup(char select, const char* string)
/*
 * activation for code-gen type command line arguments
 */
{
    char v = true;
    (void)select;
    while (*string)
    {
        switch (*string)
        {
                /*               case 'f':*/
                /*                  Optimizer::cparams.prm_smartframes = bool ;*/
                /*                  break ;*/
            case 'u':
                Optimizer::cparams.prm_charisunsigned = v;
                break;
            case 'd':
                Optimizer::cparams.prm_diag = v;
                break;
            case 'r':
                Optimizer::cparams.prm_revbits = v;
                break;
            case 'b':
                Optimizer::cparams.prm_bss = v;
                break;
            case 'l':
                Optimizer::cparams.prm_lines = v;
                break;
            case 'm':
                Optimizer::cparams.prm_cmangle = v;
                break;
#ifndef i386
                /*                case 'R':*/
                /*                    Optimizer::cparams.prm_linkreg = v;*/
                /*                    break;*/
#endif
            case 'S':
                Optimizer::cparams.prm_stackcheck = v;
                break;
            case 'O':
                Optimizer::cparams.prm_oldfor = v;
                break;
            case 'Z':
                Optimizer::cparams.prm_profiler = v;
                break;
            case 'i':
                Optimizer::cparams.prm_allowinline = v;
                break;
            case '-':
                v = false;
                break;
            case '+':
                v = true;
                break;
            default:
                switch (parseCodegen(v, string))
                {
                    case 1:
                        break;
                    case 2:
                        return;
                    case 0:
                    default:
                        Utils::fatal("Invalid codegen parameter ");
                        break;
                }
        }
        string++;
    }
}
void warning_setup(char select, const char* string)
{
    if (string[0] == 0)
        AllWarningsDisable();
    else
        switch (string[0])
        {
            case '+':
                Optimizer::cparams.prm_extwarning = true;
                DisableTrivialWarnings();
                break;
            case 'd':
                DisableWarning(atoi(string + 1));
                break;
            case 'o':
                WarningOnlyOnce(atoi(string + 1));
                break;
            case 'x':
                AllWarningsAsError();
                break;
            case 'e':
                if (!strcmp(string, "error"))
                    AllWarningsAsError();
                else
                    WarningAsError(atoi(string + 1));
                break;
            default:
                EnableWarning(atoi(string));
                break;
        }
}
static bool validatenamespaceAndClass(const char* str)
{
    if (!isalpha(str[0]))
        return false;
    while (*str && *str != '.')
    {
        if (!isalnum(*str) && *str != '_')
            return false;
        str++;
    }
    if (!*str++)
        return false;
    if (!isalpha(str[0]))
        return false;
    while (*str)
    {
        if (!isalnum(*str) && *str != '_')
            return false;
        str++;
    }
    return true;
}
void ParamTransfer()
/*
 * activation routine (callback) for boolean command line arguments
 */
{

    // booleans
    if (prm_c89.GetExists())
        Optimizer::cparams.prm_c99 = Optimizer::cparams.prm_c1x = !prm_c89.GetValue();
    if (prm_c99.GetExists())
    {
        Optimizer::cparams.prm_c99 = prm_c99.GetValue();
        Optimizer::cparams.prm_c1x = !prm_c99.GetValue();
    }
    if (prm_c11.GetExists())
    {
        Optimizer::cparams.prm_c99 = prm_c11.GetValue();
        Optimizer::cparams.prm_c1x = prm_c11.GetValue();
    }
    if (prm_makestubs.GetExists())
        Optimizer::cparams.prm_makestubs = prm_makestubs.GetValue();
    if (prm_ansi.GetExists())
        Optimizer::cparams.prm_ansi = prm_ansi.GetValue();
    if (prm_errfile.GetExists())
        Optimizer::cparams.prm_errfile = prm_errfile.GetValue();
    if (prm_cppfile.GetExists())
        Optimizer::cparams.prm_cppfile = prm_cppfile.GetValue();
    if (prm_quiet.GetExists())
        Optimizer::cparams.prm_quiet = prm_quiet.GetValue();
    if (prm_trigraph.GetExists())
        Optimizer::cparams.prm_trigraph = prm_trigraph.GetValue();
    if (prm_icdfile.GetExists())
        Optimizer::cparams.prm_icdfile = prm_icdfile.GetValue();
    if (prm_viaassembly.GetExists())
    {
        Optimizer::cparams.prm_assemble = true;
        Optimizer::cparams.prm_asmfile = false;
    }
    if (prm_debug.GetExists() || prm_debug2.GetExists())
    {
        Optimizer::cparams.prm_debug = prm_debug.GetValue() || prm_debug2.GetValue();
        if (Optimizer::cparams.prm_debug)
        {
            Optimizer::cparams.prm_optimize_for_speed = Optimizer::cparams.prm_optimize_for_size = 0;
        }
    }
    if (prm_compileonly.GetExists())
    {
        if (Optimizer::chosenAssembler->objext)
        {
            Optimizer::cparams.prm_compileonly = prm_compileonly.GetValue();
            Optimizer::cparams.prm_asmfile = false;
        }
    }
    if (prm_xcept.GetExists())
        Optimizer::cparams.prm_xcept = prm_xcept.GetValue();
    // non-bools
    if (prm_verbose.GetExists())
    {
        Optimizer::verbosity = 1 + prm_verbose.GetValue().size();
    }
    std::vector<std::string> checks = split(prm_optimize.GetValue());
    for (auto&& v : checks)
    {
        if (v.size() >= 1)
        {
            optimize_setup('O', v.c_str());
        }
    }
    if (prm_assemble.GetExists())
    {
        Optimizer::cparams.prm_asmfile = true;
        Optimizer::prm_assemblerSpecifier = prm_assemble.GetValue();
    }
    if (prm_codegen.GetExists())
    {
        codegen_setup('C', prm_codegen.GetValue().c_str());
    }
    if (prm_stackalign.GetExists())
    {
        int n = prm_stackalign.GetValue();
        if (!n || (n % Optimizer::chosenAssembler->arch->stackalign))
            Utils::fatal("Invalid stack alignment parameter ");
        Optimizer::cparams.prm_stackalign = n;
    }
    if (prm_error.GetExists())
    {
        const char* string = prm_error.GetValue().c_str();
        if (*string == '+')
        {
            Optimizer::cparams.prm_extwarning = true;
            string++;
        }
        else if (*string == '-')
        {
            Optimizer::cparams.prm_warning = false;
            string++;
        }
        int n = atoi(string);
        if (n > 0)
            Optimizer::cparams.prm_maxerr = n;
        DisableTrivialWarnings();
    }
    checks = split(prm_warning.GetValue());
    for (auto&& v : checks)
    {
        warning_setup('w', v.c_str());
    }
    checks = split(prm_tool.GetValue());
    for (auto&& v : checks)
    {
        Optimizer::toolArgs.push_back(v);
    }
    checks = split(prm_define.GetValue());
    for (auto&& v : checks)
    {
        defines.push_back(DefValue{v.c_str(), 0});
    }
    checks = split(prm_undefine.GetValue());
    for (auto&& v : checks)
    {
        defines.push_back(DefValue{v.c_str(), 1});
    }
    checks = split(prm_library.GetValue());
    for (auto&& v : checks)
    {
        char buf[260];
        strcpy(buf, v.c_str());
        Utils::StripExt(buf);
        Utils::AddExt(buf, ".l");
        InsertAnyFile(buf, 0, -1);
    }
    if (prm_libpath.GetExists())
    {
        switch (Optimizer::architecture)
        {
            case ARCHITECTURE_MSIL:
            {
                auto v = split(prm_libpath.GetValue());
                for (auto&& s : v)
                {
                    Optimizer::prm_Using.push_back(s);
#ifndef PARSER_ONLY
                    occmsil::_add_global_using(s.c_str());
#endif
                }
                break;
            }
            case ARCHITECTURE_X86:
                Optimizer::prm_libPath = prm_libpath.GetValue();
                break;
        }
    }
    if (prm_Winmode.GetExists())
    {
        switch (Optimizer::architecture)
        {
            case ARCHITECTURE_MSIL:
                Optimizer::msilWinmodeSetup(prm_Winmode.GetValue().c_str());
                break;
            case ARCHITECTURE_X86:
                Optimizer::WinmodeSetup(prm_Winmode.GetValue().c_str());
                break;
        }
    }
    if (prm_pipe.GetExists())
    {
        Optimizer::cparams.prm_bepeep = false;
        if (prm_pipe.GetValue().size() != 0)
        {
            const char* string = prm_pipe.GetValue().c_str();
            Optimizer::cparams.replacePInvoke = string[0] == '+';
        }
        else
        {
            Optimizer::cparams.replacePInvoke = true;
        }
    }
    if (prm_msil_noextensions.GetExists())
    {
        Optimizer::cparams.msilAllowExtensions = !prm_msil_noextensions.GetValue();
    }
    if (prm_msil_strongnamekeyfile.GetExists() && prm_msil_strongnamekeyfile.GetValue().size() != 0)
    {
        Optimizer::prm_snkKeyFile = prm_msil_strongnamekeyfile.GetValue();
    }
    if (prm_msil_namespace.GetExists() && prm_msil_namespace.GetValue().size() != 0)
    {
        if (!validatenamespaceAndClass(prm_msil_namespace.GetValue().c_str()))
            Utils::fatal("namesplace/class info in wrong format");

        Optimizer::prm_namespace_and_class = prm_msil_namespace.GetValue();
    }
    if (prm_msil_version.GetExists() && prm_msil_version.GetValue().size() != 0)
    {
        Optimizer::prm_assemblyVersion = prm_msil_version.GetValue();
    }
    if (prm_output_def_file.GetExists())
    {
        Optimizer::prm_OutputDefFile = prm_output_def_file.GetValue();
    }
    if (AssemblerExtension.GetExists())
    {
        Optimizer::assemblerFileExtension = "." + AssemblerExtension.GetValue();
    }
    else
    {
        Optimizer::assemblerFileExtension = Optimizer::chosenAssembler->asmext;
    }
    if (Optimizer::cparams.prm_debug || Optimizer::cparams.prm_stackalign || !Optimizer::cparams.prm_optimize_for_speed)
    {
        Optimizer::cparams.prm_useesp = false;
    }
}

/*-------------------------------------------------------------------------*/

void setglbdefs(void)
/*
 * function declares any global defines from the command line and also
 * declares a couple of other macros so we can tell what the compiler is
 * doing
 */
{
    char buf[256];
    int major, temp, minor, build;
    Optimizer::ARCH_DEFINES* a = Optimizer::chosenAssembler->defines;
    sscanf(STRING_VERSION, "%d.%d.%d.%d", &major, &temp, &minor, &build);
    Optimizer::my_sprintf(buf, "%d", major * 100 + minor);
    preProcessor->Define("__ORANGEC__", buf);
    Optimizer::my_sprintf(buf, "%d", major);
    preProcessor->Define("__ORANGEC_MAJOR__", buf);
    Optimizer::my_sprintf(buf, "%d", minor);
    preProcessor->Define("__ORANGEC_MINOR__", buf);
    Optimizer::my_sprintf(buf, "%d", build);
    preProcessor->Define("__ORANGEC_PATCHLEVEL__", buf);
    sprintf(buf, "\"%s\"", STRING_VERSION);
    preProcessor->Define("__VERSION__", buf);
    preProcessor->Define("__CHAR_BIT__", "8");
    if (Optimizer::cparams.prm_cplusplus)
    {
        preProcessor->Define("__cplusplus", "201402");
        if (Optimizer::cparams.prm_xcept)
            preProcessor->Define("__RTTI__", "1");
    }
    preProcessor->Define("__STDC__", "1");

    if (Optimizer::cparams.prm_c99 || Optimizer::cparams.prm_c1x)
    {
        preProcessor->Define("__STDC_HOSTED__", Optimizer::chosenAssembler->hosted);  // hosted compiler, not embedded
    }
    if (Optimizer::cparams.prm_c1x)
    {
        preProcessor->Define("__STDC_VERSION__", "201112L");
    }
    else if (Optimizer::cparams.prm_c99)
    {
        preProcessor->Define("__STDC_VERSION__", "199901L");
    }
    /*   preProcessor->Define("__STDC_IEC_599__","1");*/
    /*   preProcessor->Define("__STDC_IEC_599_COMPLEX__","1");*/
    /*   preProcessor->Define("__STDC_ISO_10646__","199712L");*/
    /*    preProcessor->Define(preProcessor->Define, "");*/
    if (a)
    {
        while (a->define)
        {
            if (a->respect)
            {
                preProcessor->Define(a->define, a->value);
            }
            a++;
        }
    }
    for (auto d : defines)
    {
        size_t n = d.name.find_first_of("=");
        std::string name, val;
        if (n != std::string::npos)
        {
            name = d.name.substr(0, n);
            if (n != d.name.size() - 1)
                val = d.name.substr(n + 1);
        }
        else
        {
            name = d.name;
            val = "1";
        }
        if (d.undef)
            preProcessor->Undefine(name);
        else
            preProcessor->Define(name.c_str(), val);
    }
}

/*-------------------------------------------------------------------------*/

int insert_noncompile_file(const char* buf)
{
    const char* ext = Optimizer::chosenAssembler->beext;
    if (ext)
    {
        auto splt = split(ext);
        for (auto&& str : splt)
        {
            if (Utils::HasExt(buf, str.c_str()))
            {
                Optimizer::backendFiles.push_back(buf);
                return 1;
            }
        }
    }
    return 0;
}

void InsertOneFile(const char* filename, char* path, int drive)
/*
 * Insert a file name onto the list of files to process
 */

{
    char a = 0;
    char *newbuffer, buffer[260], *p = buffer;
    bool inserted;
    Optimizer::LIST **r = &clist, *s;

    if (drive != -1)
    {
        *p++ = (char)(drive + 'A');
        *p++ = ':';
    }
    if (path)
    {
        strcpy(p, path);
    }
    else
        *p = 0;
    /* Allocate buffer and make .C if no extension */
    strcat(buffer, filename);
    if (buffer[0] == '-')
    {
        a = buffer[0];
        buffer[0] = 'a';
    }
    if (firstFile.empty())
    {
        char temp[260];
        char* p = strrchr(buffer, '/');
        char* q = strrchr(buffer, '\\');
        if (q > p)
            p = q;
        if (!p)
            p = buffer;
        else
            p++;
        strcpy(temp, p);
        Utils::StripExt(temp);
        if (!Optimizer::cparams.prm_compileonly && !Optimizer::cparams.prm_assemble)
            Utils::AddExt(temp, ".exe");
        firstFile = temp;
    }
    inserted = insert_noncompile_file(buffer);
    if (a)
        buffer[0] = a;
    if (!inserted)
    {
        Utils::AddExt(buffer, ".c");
        newbuffer = (char*)malloc(strlen(buffer) + 1);
        if (!newbuffer)
            return;
        strcpy(newbuffer, buffer);

        while ((*r))
            r = &(*r)->next;
        (*r) = (Optimizer::LIST*)malloc(sizeof(Optimizer::LIST));
        s = (*r);
        if (!s)
            return;
        s->next = 0;
        s->data = newbuffer;
    }
}
void InsertAnyFile(const char* filename, char* path, int drive)
{
    char drv[256], dir[256], name[256], ext[256];
#if defined(_MSC_VER) || defined(BORLAND) || defined(__ORANGEC__)
    struct _finddata_t findbuf;
    size_t n;
    _splitpath(filename, drv, dir, name, ext);
    n = _findfirst(filename, &findbuf);
    if (n != -1)
    {
        do
        {
            InsertOneFile(findbuf.name, dir[0] ? dir : 0, drv[0] ? tolower(drv[0]) - 'a' : -1);
        } while (_findnext(n, &findbuf) != -1);
        _findclose(n);
    }
    else
    {
        InsertOneFile(filename, path, drive);
    }
#else
    InsertOneFile(filename, path, drive);
#endif
}
/*-------------------------------------------------------------------------*/

void dumperrs(FILE* file);
void setfile(char* buf, const char* orgbuf, const char* ext)
/*
 * Get rid of a file path an add an extension to the file name
 */
{
    const char* p = strrchr(orgbuf, '\\');
    const char* p1 = strrchr(orgbuf, '/');
    if (p1 > p)
        p = p1;
    else if (!p)
        p = p1;
    if (!p)
        p = orgbuf;
    else
        p++;
    strcpy(buf, p);
    Utils::StripExt(buf);
    strcat(buf, ext);
}

/*-------------------------------------------------------------------------*/

void outputfile(char* buf, const char* orgbuf, const char* ext)
{

    if (buf[strlen(buf) - 1] == '\\')
    {
        const char* p = strrchr(orgbuf, '\\');
        if (p)
            p++;
        else
            p = orgbuf;
        strcat(buf, p);
        Utils::StripExt(buf);
        Utils::AddExt(buf, ext);
    }
    else if (prm_output.GetExists())
    {
        Utils::AddExt(buf, ext);
    }
    else
    {
        setfile(buf, orgbuf, ext);
    }
}

/*-------------------------------------------------------------------------*/

void addinclude(void)
/*
 * Look up the INCLUDE environment variable and append it to the
 * search path
 */
{
    char* string = getenv("CCINCL");
    if (string && string[0])
    {
        prm_cinclude.Parse(string);
    }
    string = getenv("CPATH");
    if (string && string[0])
    {
        prm_cinclude.Parse(string);
    }
}

/*-------------------------------------------------------------------------*/

void dumperrs(FILE* file)
{
    if (diagcount && !TotalErrors())
        fprintf(file, "%d Diagnostics\n", diagcount);
    if (TotalErrors())
        fprintf(file, "%d Errors\n", TotalErrors());
}

/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/

void internalError(int a)
{
    (void)a;
    fprintf(stderr, "Internal Error - Aborting compile");
    exit(1);
}

/*-------------------------------------------------------------------------*/
void ccinit(int argc, char* argv[])
{
    char buffer[260];
    char* p;
    int rv;
    int i;

    Optimizer::doBackendInit = true;

    for (i = 1; i < argc; i++)
        if (argv[i][0] == '-' || argv[i][0] == '/')
        {
            if (argv[i][1] == '!' || !strcmp(argv[i], "--nologo"))
            {
                Optimizer::showBanner = false;
            }
            else if ((argv[i][1] == 'V' && argv[i][2] == 0) || !strcmp(argv[i], "--version"))
            {
                showVersion = true;
            }
        }

    if (Optimizer::showBanner || showVersion)
    {
        Utils::banner(argv[0]);
    }
    if (showVersion)
    {
        fprintf(stderr, "Compile date: " __DATE__ ", time: " __TIME__ "\n");
        exit(0);
    }

#if defined(WIN32) || defined(MICROSOFT)
    GetModuleFileNameA(nullptr, buffer, sizeof(buffer));
#else
    strcpy(buffer, argv[0]);
#endif

    if (!getenv("ORANGEC"))
    {
        char* p = strrchr(buffer, '\\');
        if (p)
        {
            *p = 0;
            char* q = strrchr(buffer, '\\');
            if (q)
            {
                *q = 0;
                char* buf1 = (char*)calloc(1, strlen("ORANGEC") + strlen(buffer) + 2);
                strcpy(buf1, "ORANGEC");
                strcat(buf1, "=");
                strcat(buf1, buffer);
                putenv(buf1);
                *q = '\\';
            }
            *p = '\\';
        }
    }
    DisableTrivialWarnings();
    /* parse the environment and command line */
    int ecnt = 0;
    char* eargs[200];
    bool need_usage = false;
    if (!switchParser.Parse(&argc, argv) || (argc == 1 && prm_file.GetCount() <= 1 && ecnt <= 1))
    {
        need_usage = true;
    }
    /* initialize back end */
    if (prm_assemble.GetExists())
    {
        Optimizer::cparams.prm_asmfile = true;
        Optimizer::prm_assemblerSpecifier = prm_assemble.GetValue();
    }
    if (prm_architecture.GetExists())
    {
        auto splt = split(prm_architecture.GetValue(), ';');
        static std::map<std::string, int> architectures = {
            {"x86", ARCHITECTURE_X86},
            {"msil", ARCHITECTURE_MSIL},
        };
        if (architectures.find(splt[0]) != architectures.end())
        {
            Optimizer::architecture = architectures[splt[0]];
        }
        else
        {
            Utils::fatal("invalid Optimizer::architecture");
        }
        if (splt.size() > 1)
        {
            bePostFile = splt[1];
        }
    }
    else
    {
        // default to x86
        Optimizer::architecture = ARCHITECTURE_X86;
    }
    if (!init_backend())
        Utils::fatal("Could not initialize back end");

    if (need_usage)
    {
        Utils::usage(argv[0], getUsageText());
    }

    if (Optimizer::chosenAssembler->envname)
    {
        const char* env = getenv(Optimizer::chosenAssembler->envname);
        if (env && !switchParser.Parse(std::string(env), &ecnt, eargs))
            Utils::usage(argv[0], getUsageText());
    }

    CmdSwitchFile internalConfig(switchParser);
    if (Optimizer::chosenAssembler->cfgname)
    {
        char temp[260];
        strcpy(temp, buffer);
        char *p1 = strrchr(temp, '/'), *p2 = strrchr(temp, '\\');
        if (p2 > p1)
            p1 = p2;
        else if (!p1)
            p1 = p2;
        if (p1)
            p1++;
        else
            p1 = temp;
        strcpy(p1, Optimizer::chosenAssembler->progname);
        std::string configName = Utils::QualifiedFile(temp, ".cfg");
        std::fstream configTest(configName, std::ios::in);
        if (!configTest.fail())
        {
            configTest.close();
            if (!internalConfig.Parse(configName.c_str()))
                Utils::fatal("Corrupt configuration file");
        }
    }

    ParamTransfer();
    /* tack the environment includes in */
    addinclude();

    /* Scan the command line for file names or response files */
    {
        int i;
        for (i = 1; i < argc; i++)
            InsertAnyFile(argv[i], 0, -1);
        for (i = 1; i < ecnt; i++)
            InsertAnyFile(eargs[i], 0, -1);
        int count = prm_file.GetCount();
        char** value = prm_file.GetValue();
        for (int i = 1; i < count; i++)
        {
            InsertAnyFile(value[i], 0, -1);
        }
    }

#ifndef PARSER_ONLY

    if (prm_output.GetExists())
    {
        Optimizer::outputFileName = prm_output.GetValue();
        if (!Optimizer::cparams.prm_compileonly)
        {
            prm_output.SetExists(false);
        }
        else
        {
            if (clist && clist->next && prm_output.GetValue()[prm_output.GetValue().size() - 1] != '\\')
                Utils::fatal("Cannot specify output file for multiple input files\n");
        }
    }
#else
    {
        Optimizer::LIST* t = clist;
        while (t)
        {
            t->data = strdup(Utils::FullQualify((char*)t->data));
            t = t->next;
        }
    }
#endif

    /* Set up a ctrl-C handler so we can exit the prog with cleanup */
    //    signal(SIGINT, ctrlchandler);
    //    signal(SIGSEGV, internalError);
}
}  // namespace Parser