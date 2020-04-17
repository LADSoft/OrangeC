#include "iexpr.h"
#include "beinterf.h"
#include "Utils.h"

extern COMPILER_PARAMS cparams;
extern std::string prm_assemblerSpecifier;

extern ARCH_ASM msilAssemblerInterface[];
extern ARCH_ASM x86AssemblerInterface[];

ARCH_ASM* chosenAssembler;
ARCH_DEBUG* chosenDebugger;

enum asmTypes : int;

static ARCH_ASM *assemblerInterface;

ARCH_ASM *assemblerInterfaces[] = { x86AssemblerInterface, msilAssemblerInterface };

void SelectBackendData()
{
    switch (architecture)
    {
    case ARCHITECTURE_MSIL:
        assemblerInterface = msilAssemblerInterface;
        break;
    case ARCHITECTURE_X86:
        assemblerInterface = x86AssemblerInterface;
        break;
    default:
        return; // failure
    }
    char assembler[100], debugger[100];
    int i, rv;
    assembler[0] = debugger[0] = 0;
    if (cparams.prm_asmfile)
    {
        const char *p = prm_assemblerSpecifier.c_str();
        char* q = assembler;
        cparams.prm_compileonly = true;
        while (*p && !isspace(*p) && *p != ';')
            *q++ = *p++;
        *q = 0;
        if (*p == ';')
        {
            q = debugger;
            p++;
            while (*p && !isspace(*p))
                *q++ = *p++;
            *q = 0;
        }
    }
    if (assembler[0])
    {
        ARCH_ASM* a = assemblerInterface;
        while (a->name)
        {
            if (!strcmp(a->name, assembler))
            {
                chosenAssembler = a;
                break;
            }
            a++;
        }
        if (!a->name)
            Utils::fatal("Chosen assembler format '%s' not found", assembler);
    }
    else
    {
        chosenAssembler = assemblerInterface;
    }
    if (chosenAssembler->init)
	chosenAssembler->init(&cparams, chosenAssembler, chosenDebugger);
}