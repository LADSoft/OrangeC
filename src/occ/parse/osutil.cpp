/* Software License Agreement
 *
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
#include "../../util/Utils.h"
#    include "../../version.h"
#if defined(_MSC_VER) || defined(BORLAND) || defined(__ORANGEC__)
#    include <io.h>
#endif

#if defined(WIN32) || defined(MICROSOFT)
extern "C"
{
    char* __stdcall GetModuleFileNameA(void* handle, char* buf, int size);
}
#endif

extern COMPILER_PARAMS cparams;
extern ARCH_ASM* chosenAssembler;
extern int diagcount;
extern NAMESPACEVALUELIST* globalNameSpace;
extern char infile[];
extern PreProcessor* preProcessor;
extern int verbosity;

FILE* outputFile;
FILE* listFile;
LIST* clist = 0;
int showBanner = true;
int showVersion = false;

struct DefValue
{
    std::string name;
    bool undef;
};
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
CmdSwitchBool prm_makestubs(switchParser, 'M');
CmdSwitchBool prm_compileonly(switchParser, 'c');
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

CmdSwitchString prm_library(switchParser, 'l', ';');

CmdSwitchCombineString prm_include(switchParser, 'I', ';');
CmdSwitchCombineString prm_sysinclude(switchParser, 'z', ';');
CmdSwitchCombineString prm_libpath(switchParser, 'L', ';');
CmdSwitchString prm_pipe(switchParser, 'P', ';');


CmdSwitchBool prm_msil_noextensions(switchParser, 'd');
CmdSwitchString prm_msil_strongnamekeyfile(switchParser, 'K');
CmdSwitchString prm_msil_namespace(switchParser, 'N');
CmdSwitchString prm_msil_version(switchParser, 'V');

CmdSwitchString prm_Winmode(switchParser, 'W');

/* Print usage info */
/*
 * If no extension, add the one specified
 */
void AddExt(char* buffer, const char* ext)
{
    char* pos = strrchr(buffer, '.');
    if (!pos || (*(pos - 1) == '.') || (*(pos + 1) == '\\'))
        strcat(buffer, ext);
}

/*
 * Strip extension, if it has one
 */
void StripExt(char* buffer)
{
    char* pos = strrchr(buffer, '.');
    if (pos && (*(pos - 1) != '.'))
        *pos = 0;
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

/*-------------------------------------------------------------------------*/

int HasExt(char* buffer, const char* ext)
{
    int l = strlen(buffer), l1 = strlen(ext);
    if (l1 < l)
    {
        return Utils::iequal(buffer + l - l1, ext);
    }
    return 0;
}
/*
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

/*-------------------------------------------------------------------------*/
void library_setup(char select, char* string)
{
    (void)select;
    if (string[0] == 0)
    {
        cparams.prm_listfile = true;
    }
    else
    {
        char buf[260];
        strcpy(buf, string);
        StripExt(buf);
        AddExt(buf, ".l");
        InsertAnyFile(buf, 0, -1, false);
    }
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
        cparams.prm_optimize_for_speed = true;
        cparams.prm_optimize_for_size = false;
        cparams.prm_debug = false;
    }
    else if (*string == '-' && string[1] == '\0')
        cparams.prm_optimize_for_speed = cparams.prm_optimize_for_size = cparams.prm_optimize_float_access = false;
    else
    {
        cparams.prm_debug = false;
        if (*string == '0')
        {
            cparams.prm_optimize_for_speed = cparams.prm_optimize_for_size = 0;
        }
        else if (*string == 'f')
            cparams.prm_optimize_float_access = true;
        else if (*string == '2')
        {
            cparams.prm_optimize_for_speed = true;
            cparams.prm_optimize_for_size = false;
        }
        else if (*string == '1')
        {
            cparams.prm_optimize_for_speed = false;
            cparams.prm_optimize_for_size = true;
        }
    }
}
/*-------------------------------------------------------------------------*/

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
            /*                  cparams.prm_smartframes = bool ;*/
            /*                  break ;*/
        case 'u':
            cparams.prm_charisunsigned = v;
            break;
        case 'd':
            cparams.prm_diag = v;
            break;
        case 'r':
            cparams.prm_revbits = v;
            break;
        case 'b':
            cparams.prm_bss = v;
            break;
        case 'l':
            cparams.prm_lines = v;
            break;
        case 'm':
            cparams.prm_cmangle = v;
            break;
#ifndef i386
            /*                case 'R':*/
            /*                    cparams.prm_linkreg = v;*/
            /*                    break;*/
#endif
        case 'S':
            cparams.prm_stackcheck = v;
            break;
        case 'O':
            cparams.prm_oldfor = v;
            break;
        case 'Z':
            cparams.prm_profiler = v;
            break;
        case 'i':
            cparams.prm_allowinline = v;
            break;
        case '-':
            v = false;
            break;
        case '+':
            v = true;
            break;
        default:
            if (chosenAssembler->parse_codegen)
            {
                switch (chosenAssembler->parse_codegen(v, string))
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
            else
                Utils::fatal("Invalid codegen parameter ");
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
            cparams.prm_extwarning = true;
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
void ParamTransfer()
/*
 * activation routine (callback) for boolean command line arguments
 */
{

    // booleans
    if (prm_c89.GetExists())
        cparams.prm_c99 = cparams.prm_c1x = !prm_c89.GetValue();
    if (prm_c99.GetExists())
    {
        cparams.prm_c99 = prm_c99.GetValue();
        cparams.prm_c1x = !prm_c99.GetValue();
    }
    if (prm_c11.GetExists())
    {
        cparams.prm_c99 = prm_c11.GetValue();
        cparams.prm_c1x = prm_c11.GetValue();
    }
    if (prm_makestubs.GetExists())
        cparams.prm_makestubs = prm_makestubs.GetValue();
    if (prm_ansi.GetExists())
        cparams.prm_ansi = prm_ansi.GetValue();
    if (prm_errfile.GetExists())
        cparams.prm_errfile = prm_errfile.GetValue();
    if (prm_cppfile.GetExists())
        cparams.prm_cppfile = prm_cppfile.GetValue();
    if (prm_quiet.GetExists())
        cparams.prm_quiet = prm_quiet.GetValue();
    if (prm_trigraph.GetExists())
        cparams.prm_trigraph = prm_trigraph.GetValue();
    if (prm_icdfile.GetExists())
        cparams.prm_icdfile = prm_icdfile.GetValue();
    if (prm_viaassembly.GetExists())
    {
        cparams.prm_assemble = true;
        cparams.prm_asmfile = false;
    }
    if (prm_debug.GetExists())
    {
        cparams.prm_debug = prm_debug.GetValue();
        if (cparams.prm_debug)
        {
            cparams.prm_optimize_for_speed = cparams.prm_optimize_for_size = 0;
        }
    }
    if (prm_compileonly.GetExists())
    {
        if (chosenAssembler->objext)
        {
            cparams.prm_compileonly = prm_compileonly.GetValue();
            cparams.prm_asmfile = false;
        }
    }
    if (prm_xcept.GetExists())
        cparams.prm_xcept = prm_xcept.GetValue();
    // non-bools
    if (prm_verbose.GetExists())
    {
        verbosity = 1 + prm_verbose.GetValue().size();
    }
    std::vector<std::string> checks = split(prm_optimize.GetValue());
    for (auto&& v : checks)
    {
        if (v.size() >= 1)
        {
            optimize_setup('O', v.c_str());
        }
        
    }
    checks = split(prm_codegen.GetValue());
    for (auto&& v : checks)
    {
        if (v.size() >= 1)
        {
            char ch = v[0];
            codegen_setup(ch, v.c_str() + 1);
        }

    }
    if (prm_stackalign.GetExists())
    {
        int n = prm_stackalign.GetValue();
        if (!n || (n % chosenAssembler->arch->stackalign))
            Utils::fatal("Invalid stack alignment parameter ");
        cparams.prm_stackalign = n;
    }
    if (prm_error.GetExists())
    {
        const char *string = prm_error.GetValue().c_str();
        if (*string == '+')
        {
            cparams.prm_extwarning = true;
            string++;
        }
        else if (*string == '-')
        {
            cparams.prm_warning = false;
            string++;
        }
        int n = atoi(string);
        if (n > 0)
            cparams.prm_maxerr = n;
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
        std::string hash = "$" + v;
        InsertAnyFile(hash.c_str(), 0, -1, false);
    }
    checks = split(prm_define.GetValue());
    for (auto&& v : checks)
    {
        defines.push_back(DefValue{ v.c_str(), 0 });
    }
    checks = split(prm_undefine.GetValue());
    for (auto&& v : checks)
    {
        defines.push_back(DefValue{ v.c_str(), 1 });
    }
    checks = split(prm_library.GetValue());
    if (!checks.size() && prm_library.GetExists())
        cparams.prm_listfile = true;
    for (auto&& v : checks)
    {
        char buf[260];
        strcpy(buf, v.c_str());
        StripExt(buf);
        AddExt(buf, ".l");
        InsertAnyFile(buf, 0, -1, false);
    }
    if (prm_libpath.GetExists())
    {
        checks = split(prm_libpath.GetValue());

        for (auto&& v : checks)
            if (chosenAssembler->parse_param)
                chosenAssembler->parse_param('L', v.c_str());
    }
    if (prm_Winmode.GetExists())
    {
        if (chosenAssembler->parse_param)
            chosenAssembler->parse_param('W', prm_Winmode.GetValue().c_str());
    }
    if (prm_pipe.GetExists() && prm_pipe.GetValue().size() != 0)
    {
        if (chosenAssembler->parse_param)
            chosenAssembler->parse_param('P', prm_pipe.GetValue().c_str());
    }
    if (prm_msil_noextensions.GetExists())
    {
        if (chosenAssembler->parse_param)
            chosenAssembler->parse_param('d', prm_msil_noextensions.GetValue() ? "+" : "-");
    }
    if (prm_msil_strongnamekeyfile.GetExists() && prm_msil_strongnamekeyfile.GetValue().size() != 0)
    {
        if (chosenAssembler->parse_param)
            chosenAssembler->parse_param('K', prm_msil_strongnamekeyfile.GetValue().c_str());
    }
    if (prm_msil_namespace.GetExists() && prm_msil_namespace.GetValue().size() != 0)
    {
        if (chosenAssembler->parse_param)
            chosenAssembler->parse_param('N', prm_msil_namespace.GetValue().c_str());

    }
    if (prm_msil_version.GetExists() && prm_msil_version.GetValue().size() != 0)
    {
        if (chosenAssembler->parse_param)
            chosenAssembler->parse_param('V', prm_msil_version.GetValue().c_str());
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
    ARCH_DEFINES* a = chosenAssembler->defines;
    sscanf(STRING_VERSION, "%d.%d.%d.%d", &major, &temp, &minor, &build);
    my_sprintf(buf, "%d", major * 100 + minor);
    preProcessor->Define("__ORANGEC__", buf);
    my_sprintf(buf, "%d", major);
    preProcessor->Define("__ORANGEC_MAJOR__", buf);
    my_sprintf(buf, "%d", minor);
    preProcessor->Define("__ORANGEC_MINOR__", buf);
    my_sprintf(buf, "%d", build);
    preProcessor->Define("__ORANGEC_PATCHLEVEL__", buf);
    sprintf(buf, "\"%s\"", STRING_VERSION);
    preProcessor->Define("__VERSION__", buf);
    preProcessor->Define("__CHAR_BIT__", "8");
    if (cparams.prm_cplusplus)
    {
        preProcessor->Define("__cplusplus", "201402");
        if (cparams.prm_xcept)
            preProcessor->Define("__RTTI__", "1");
    }
    preProcessor->Define("__STDC__", "1");

    if (cparams.prm_c99 || cparams.prm_c1x)
    {
        preProcessor->Define("__STDC_HOSTED__", chosenAssembler->hosted);  // hosted compiler, not embedded
    }
    if (cparams.prm_c1x)
    {
        preProcessor->Define("__STDC_VERSION__", "201112L");
    }
    else if (cparams.prm_c99)
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

void InsertOneFile(const char* filename, char* path, int drive, bool primary)
/*
 * Insert a file name onto the list of files to process
 */

{
    char a = 0;
    char *newbuffer, buffer[260], *p = buffer;
    bool inserted;
    LIST **r = &clist, *s;

    if (drive != -1)
    {
        *p++ = (char)(drive + 'A');
        *p++ = ':';
    }
    if (path)
    {
        strcpy(p, path);
        //        strcat(p, "\\");
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
    inserted = chosenAssembler->insert_noncompile_file && chosenAssembler->insert_noncompile_file(buffer, primary);
    if (a)
        buffer[0] = a;
    if (!inserted)
    {
        AddExt(buffer, ".c");
        newbuffer = (char*)malloc(strlen(buffer) + 1);
        if (!newbuffer)
            return;
        strcpy(newbuffer, buffer);

        while ((*r))
            r = &(*r)->next;
        (*r) = (LIST*)malloc(sizeof(LIST));
        s = (*r);
        if (!s)
            return;
        s->next = 0;
        s->data = newbuffer;
    }
}
void InsertAnyFile(const char* filename, char* path, int drive, bool primary)
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
            InsertOneFile(findbuf.name, dir[0] ? dir : 0, drv[0] ? tolower(drv[0]) - 'a' : -1, primary);
        } while (_findnext(n, &findbuf) != -1);
        _findclose(n);
    }
    else
    {
        InsertOneFile(filename, path, drive, primary);
    }
#else
    InsertOneFile(filename, path, drive, primary);
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
    StripExt(buf);
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
        StripExt(buf);
        AddExt(buf, ext);
    }
    else if (prm_output.GetExists())
    {
        AddExt(buf, ext);
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
        prm_include.Parse(string);
    }
    string = getenv("CPATH");
    if (string && string[0])
    {
        prm_include.Parse(string);
    }
}


/*-------------------------------------------------------------------------*/

void dumperrs(FILE* file)
{
    if (cparams.prm_listfile)
    {
        fprintf(listFile, "******** Global Symbols ********\n");
        list_table(globalNameSpace->valueData->syms, 0);
        fprintf(listFile, "******** Global Tags ********\n");
        list_table(globalNameSpace->valueData->tags, 0);
    }
    if (diagcount && !TotalErrors())
        fprintf(file, "%d Diagnostics\n", diagcount);
    if (TotalErrors())
        fprintf(file, "%d Errors\n", TotalErrors());
}

/*-------------------------------------------------------------------------*/

void ctrlchandler(int aa)
{
    fprintf(stderr, "^C");
    extern void Cleanup();
    Cleanup();
    exit(1);
}

/*-------------------------------------------------------------------------*/

void internalError(int a)
{
    (void)a;
    extern void Cleanup();
    Cleanup();
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

    for (i = 1; i < argc; i++)
        if (argv[i][0] == '-' || argv[i][0] == '/')
        {
            if (argv[i][1] == '!' || !strcmp(argv[i], "--nologo"))
            {
                showBanner = false;
            }
            else if ((argv[i][1] == 'V' && argv[i][2] == 0) || !strcmp(argv[i], "--version"))
            {
                showVersion = true;
            }
        }

    if (showBanner || showVersion)
    {
        Utils::banner(chosenAssembler->progname);
    }
    if (showVersion)
    {
        fprintf(stderr, "Compile date: " __DATE__ ", time: " __TIME__ "\n");
        exit(0);
    }
    extern void Cleanup();
    Utils::SetCleanup(Cleanup);
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
    char *eargs[200];
    if (chosenAssembler->envname)
    {
        const char *env = getenv(chosenAssembler->envname);
        if (env && !switchParser.Parse(std::string(env), &ecnt, eargs))
            Utils::usage(argv[0], getUsageText());
    }
    CmdSwitchFile internalConfig(switchParser);
    if (chosenAssembler->cfgname)
    {
        std::string configName = Utils::QualifiedFile(buffer, ".cfg");
        std::fstream configTest(configName, std::ios::in);
        if (!configTest.fail())
        {
            configTest.close();
            if (!internalConfig.Parse(configName.c_str()))
                Utils::fatal("Corrupt configuration file");
        }
    }

    if (!switchParser.Parse(&argc, argv) || (argc == 1 && prm_file.GetCount() <= 1 && ecnt <= 1))
        Utils::usage(argv[0], getUsageText());

    ParamTransfer();
    /* tack the environment includes in */
    addinclude();

    /* Scan the command line for file names or response files */
    {
        int i;
        for (i = 1; i < argc; i++)
           InsertAnyFile(argv[i], 0, -1, true);
        for (i = 1; i < ecnt; i++)
            InsertAnyFile(eargs[i], 0, -1, true);
        int count = prm_file.GetCount();
        char **value = prm_file.GetValue();
        for (int i = 1; i < count; i++)
        {
            InsertAnyFile(argv[i], 0, -1, true);
        }   
    }

#ifndef PARSER_ONLY

    if (prm_output.GetExists())
    {
        if (chosenAssembler->insert_output_file)
            chosenAssembler->insert_output_file(prm_output.GetValue().c_str());
        if (!cparams.prm_compileonly)
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
        LIST* t = clist;
        while (t)
        {
            t->data = litlate(Utils::FullQualify((char*)t->data));
            t = t->next;
        }
    }
#endif

    /* Set up a ctrl-C handler so we can exit the prog with cleanup */
    signal(SIGINT, ctrlchandler);
    //    signal(SIGSEGV, internalError);
}
