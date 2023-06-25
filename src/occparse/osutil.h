#pragma once
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

#include "CmdSwitch.h"

#include <deque>

/* Mode values for ARGLIST */
#define ARG_CONCATSTRING 1
#define ARG_NOCONCATSTRING 2
#define ARG_BOOL 3
#define ARG_SWITCH 4
#define ARG_SWITCHSTRING 5
#define ARG_COMBINESTRING 6

/* Valid arg separators */
#define ARG_SEPSWITCH '/'
#define ARG_SEPfalse '-'
#define ARG_SEPtrue '+'

/* Return values for dispatch routine */
#define ARG_NEXTCHAR 1
#define ARG_NEXTARG 2
#define ARG_NEXTNOCAT 3
#define ARG_NOMATCH 4
#define ARG_NOARG 5
namespace Parser
{
typedef struct
{
    char id;
    Optimizer::UBYTE mode;
    void (*routine)(char, char*);
} CMDLIST;

typedef struct _symlist_
{
    _symlist_* next;
    struct sym* p;
} SYMLIST;

struct DefValue
{
    std::string name;
    bool undef;
};
enum e_lk : int;

enum e_lk getDefaultLinkage();

extern Optimizer::LIST* clist;
extern int showVersion;
extern std::string bePostFile;
extern int cplusplusversion;

extern std::deque<DefValue> defines;

extern CmdSwitchParser switchParser;
extern CmdSwitchBool prm_c89;
extern CmdSwitchBool prm_c99;
extern CmdSwitchBool prm_c11;
extern CmdSwitchBool prm_c2x;
extern CmdSwitchBool prm_ansi;
extern CmdSwitchBool prm_errfile;
extern CmdSwitchBool prm_cppfile;
extern CmdSwitchBool prm_quiet;
extern CmdSwitchBool prm_icdfile;
extern CmdSwitchBool prm_trigraph;
extern CmdSwitchBool prm_debug;
extern CmdSwitchBool prm_debug2;
extern CmdSwitchBool prm_makestubs;
extern CmdSwitchBool prm_compileonly;
extern CmdSwitchString prm_assemble;
extern CmdSwitchBool prm_xcept;
extern CmdSwitchBool prm_viaassembly;
extern CmdSwitchBool displayTiming;
extern CmdSwitchInt prm_stackaligns;
extern CmdSwitchString prm_error;
extern CmdSwitchString prm_define;
extern CmdSwitchString prm_undefine;
extern CmdSwitchFile prm_file;
extern CmdSwitchString prm_codegen;
extern CmdSwitchString prm_optimize;
extern CmdSwitchString prm_verbose;
extern CmdSwitchString prm_warning;
extern CmdSwitchCombineString prm_output;
extern CmdSwitchCombineString prm_tool;

extern CmdSwitchCombineString prm_library;
extern CmdSwitchCombineString prm_language;
extern CmdSwitchString prm_std;
extern CmdSwitchCombineString prm_cinclude;
extern CmdSwitchCombineString prm_Csysinclude;
extern CmdSwitchCombineString prm_CPPsysinclude;
extern CmdSwitchCombineString prm_libpath;
extern CmdSwitchString prm_pipe;
extern CmdSwitchCombineString prm_output_def_file;
extern CmdSwitchBool prm_export_all;

extern CmdSwitchBool prm_msil_noextensions;
extern CmdSwitchString prm_msil_strongnamekeyfile;
extern CmdSwitchString prm_msil_namespace;
extern CmdSwitchString prm_msil_version;
extern CmdSwitchCombineString prm_architecture;

extern CmdSwitchString prm_Winmode;
extern CmdSwitchCombineString OutputDefFile;
extern CmdSwitchBool ExportAll;

extern CmdSwitchString AssemblerExtension;

extern CmdSwitchBool MakeStubsOption;
extern CmdSwitchBool MakeStubsUser;
extern CmdSwitchCombineString MakeStubsOutputFile;
extern CmdSwitchBool MakeStubsMissingHeaders;
extern CmdSwitchBool MakeStubsPhonyTargets;
extern CmdSwitchCombineString MakeStubsTargets;
extern CmdSwitchCombineString MakeStubsQuotedTargets;
extern CmdSwitchBool MakeStubsContinue;
extern CmdSwitchBool MakeStubsContinueUser;

extern const char* getUsageText(void);
extern const char* getHelpText(void);

void EXEPath(char* buffer, char* filename);
void optimize_setup(char select, const char* string);
void codegen_setup(char select, const char* string);
void warning_setup(char select, const char* string);
void ParamTransfer(void);
void setglbdefs(void);
int insert_noncompile_file(const char* buf);
void InsertOneFile(const char* filename, char* path, int drive);
void InsertAnyFile(const char* filename, char* path, int drive);
void setfile(char* buf, const char* orgbuf, const char* ext);
void outputfile(char* buf, const char* orgbuf, const char* ext);
void addinclude(void);
void dumperrs(FILE* file);
void internalError(int a);
int ccinit(int argc, char* argv[]);

}  // namespace Parser