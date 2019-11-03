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

 /*
  * C configuration when we are using ICODE code generator output
  */
#include <stdio.h>
#include <string.h>
#include "be.h"
#include "beinterf.h"
#include "Utils.h"
#include "winmode.h"
#include "CmdSwitch.h"
#include "ildata.h"

extern int architecture;
extern std::vector<SimpleSymbol*> temporarySymbols;
extern std::vector<SimpleSymbol*> functionVariables;
extern int tempCount;
extern int blockCount;
extern int exitBlock;
extern QUAD* intermed_head, *intermed_tail;
extern std::list<std::string> inputFiles;
extern FILE* icdFile;
extern std::deque<BaseData*> baseData;
extern int nextTemp;
extern int tempBottom;
extern BLOCK **blockArray;
extern ARCH_ASM* chosenAssembler;
extern SimpleExpression* objectArray_exp;

extern int usingEsp;

CmdSwitchParser SwitchParser;
CmdSwitchBool single(SwitchParser, 's', false, "single");

const char* usageText =
"[options] inputfile\n"
"\n"
"--single     don't open internal file list\n"
"\nTime: " __TIME__ "  Date: " __DATE__;

SimpleSymbol* currentFunction;
FILE* outputFile;
FILE* inputFile;

int dbgblocknum;

void regInit() { }
int usingEsp;
void flush_peep()
{

}
void Import()
{

}
void LoadFuncs()
{

}
void diag(const char*fmt, ...)
{

}

void ProcessFunction()
{

}

void ProcessData(BaseData* v)
{

}
void ProcessFunctions()
{
    for (auto v : baseData)
    {
        if (v->type == DT_FUNC)
        {
            //            temporarySymbols = v->funcData->temporarySymbols;
            //            functionVariables = v->funcData->variables;
            //            blockCount = v->funcData->blockCount;
            //            exitBlock = v->funcData->exitBlock;
            //            tempCount = v->funcData->tempCount;
            //            functionHasAssembly = v->funcData->hasAssembly;
            intermed_head = v->funcData->instructionList;
            intermed_tail = intermed_head;
            while (intermed_tail && intermed_tail->fwd)
                intermed_tail = intermed_tail->fwd;
            objectArray_exp = v->funcData->objectArray_exp;
            currentFunction = v->funcData->name;
            ProcessFunction();
        }
        else
        {
            ProcessData(v);
        }
    }
}

bool LoadFile(const char *name)
{
    char buf[260];
    strcpy(buf, name);
    Utils::StripExt(buf);
    Utils::AddExt(buf, ".icf");
    inputFile = fopen(name, "rb");
    if (!inputFile)
        return false;
    InitIntermediate();
    bool rv = InputIntermediate();
    fclose(inputFile);
//    flow_init();
//    BitInit();
//    SSAInit();
//    oinit();
    SelectBackendData();
    return rv;
}
void SaveFile(const char *name)
{
    outputFile = fopen(name, "rb");
    if (!outputFile)
        return;
//    OutputIntermediate();
    fclose(outputFile);
}
bool Matches(const char *arg, const char *cur)
{
    const char *l = strrchr(arg, '\\');
    if (!l)
        l = arg;
    const char *r = strrchr(cur, '\\');
    if (!r)
        r = cur;
    return Utils::iequal(l, r);
}
int main(int argc, char* argv[])
{
    Utils::banner(argv[0]);
    Utils::SetEnvironmentToPathParent("ORANGEC");

    if (!SwitchParser.Parse(&argc, argv) || argc != 1)
    {
        Utils::usage(argv[0], usageText);
    }
    if (!LoadFile(argv[1]))
        return 1;
//    regInit();
//    alloc_init();
    ProcessFunctions();
    SaveFile(argv[1]);
    if (!single.GetValue())
    {
        std::list<std::string> files = inputFiles;
        for (auto p : files)
        {
            if (!Matches(argv[1], p.c_str()))
            {
                if (!LoadFile(p.c_str()))
                    return 1;
                ProcessFunctions();
                SaveFile(p.c_str());
            }
        }
    }
    return 0;
}
