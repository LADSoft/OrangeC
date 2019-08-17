/* 
CC386 C Compiler
Copyright 1994-2011 David Lindauer.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

This program is derived from the cc68k complier by 
Matthew Brandt (mailto::mattb@walkingdog.net) 

You may contact the author of this derivative at:

mailto::camille@bluegrass.net
 */
#include <stdio.h>
#include <string.h>
#include "lists.h"
#include "expr.h"
#include "c.h"
extern int prm_68020, prm_asmfile, prm_compileonly, prm_smartframes;
extern long lc_maxauto;
extern int packdata[], packlevel;

char PROGNAME[] = "CC386";
char ENVNAME[] = "CC386";
char SOURCEXT[] = ".ASM";
char GLBDEFINE[] = "_i386_";
char OBJEXT[] = ".OBJ";

#ifdef BRIEFHELP
    char *usage_text = "[+e/+i/f+l+v/w+A/C/D/E/I/O/Q/T/X] file list";
#else 
    char *usage_text = "[options] files"
        //"      [C++ mode is enabled, use .CPP extension]"
        "\n""/9     - C99 mode                     /c     - compile only\n"
        "+e     - dump errors to file          +i     - dump preprocessed file\n"
        "/fname - specify parameter file       +l     - dump listing file\n"
        "/oname - specify output file name     /s     - compile via assembly\n"
		"+v     - enable debug symbols         /w-xxx - disable a warning\n"
        "+A     - disable extensions           /Dxxx  - define something\n"
		"/E[+]nn- max number of errors         /Ipath - specify include path\n"
		"+Q     - quiet mode                   -R     - disable RTTI\n"
		"/S     - make ASM source file         /T     - translate trigraphs\n"
		"/Wxx   - set executable type\n"
    "Codegen parameters: (/C[+][-][params])\n"
        "  +d   - show diagnostics             -b     - no BSS\n"
		"  +f   - generate FASM code           -l     - no C source in ASM file\n"
		"  -m   - no leading underscores       +s     - maintain 16-byte stack align\n"
		"  +I   - microsoft exports            +M     - generate MASM code\n"
		"  +N[X]- generate NASM code [GENERIC] +O     - old style for scoping\n"
		"  +R   - enable far keyword           +T     - generate TASM code\n"
		"  +U   - assume DS != SS              +Z-    - add profiler calls\n"
//                   +S     - add stack checking code\n
        "Optimizer parameters (/O[+][-][params])\n"
        "  +a   - peepopt inline ASM           -p     - no peephole opts\n"
//        "  +a   - peepopt inline ASM           +i     - inline intrinsics\n"
//        "  -m   - don't merge constants        -p     - no peephole opts\n"
#ifdef MSDOS
		"DOS Hosted Version "
#else
		"WIN32 Hosted Version "
#endif
        "Time: " __TIME__ "  Date: " __DATE__;

#endif 
int stdmemberptrsize = 8;
int prm_farkeyword = FALSE;
int prm_fardefault = FALSE;
int prm_masm = FALSE;
int prm_nodos = FALSE;
int prm_flat = TRUE; /* not supporting TASM small model any more */
int prm_fasm = 0;
int prm_nasm = 0;
int prm_microlink = 0;
int prm_browse = 0;
int prm_stackalign = 0;
int cf_maxaddress = 20;
int cf_maxdata = 4;
int cf_freeaddress = 1;
int cf_freedata = 3;
int cf_freefloat = 3;
int stackadd = 3;
int stackmod =  - 4;
int strucadd = 3;
int strucmod =  - 4;
int stddefalignment = 4;
int stdenumsize = 4;
int stdretblocksize = 8;
int stdinttype = bt_int;
int stdunstype = bt_unsigned;
int stdintsize = 4;
int stdshortsize = 2;
int stdwchar_tsize = 2;
int stdlongsize = 4;
int stdlonglongsize = 8;
int stdfloatsize = 4;
int stddoublesize = 8;
int stdldoublesize = 10;
int stdaddrsize = 4;
int regdsize = 4;
int regasize = 4;
int regfsize = 10;
int prm_dsnotss = FALSE;

extern TYP stdchar;
TYP stdfloat = 
{
    bt_float, 0, UF_DEFINED | UF_USED,  - 1,  - 1, 4
};
TYP stdimaginary = 
{
    bt_fimaginary, 0, UF_DEFINED | UF_USED,  - 1,  - 1, 4
};
TYP stdcomplex = 
{
    bt_fcomplex, 0, UF_DEFINED | UF_USED,  - 1,  - 1, 8
};
TYP stddouble = 
{
    bt_double, 0, 0,  - 1,  - 1, 8
};
TYP stdrimaginary = 
{
    bt_rimaginary, 0, 0,  - 1,  - 1, 8
};
TYP stdrcomplex = 
{
    bt_rcomplex, 0, 0,  - 1,  - 1, 16
};
TYP stdlonglong = 
{
    bt_longlong, 0, 0,  - 1,  - 1, 8
};
TYP stdunsigned = 
{
    bt_unsigned, 0, 0,  - 1,  - 1, 4
};
TYP stdunsignedlonglong = 
{
    bt_unsignedlonglong, 0, 0,  - 1,  - 1, 8
};
TYP stdenum = 
{
    bt_enum, 1, UF_DEFINED,  - 1,  - 1, 4, 0, 0, "stdenum"
};
TYP stdconst = 
{
    bt_int, 1, UF_DEFINED,  - 1,  - 1, 4, 0, 0, "stdconst"
};
TYP stdstring = 
{
    bt_pointer, 0, 0,  - 1,  - 1, 4, 0, &stdchar
};
static TYP stdwchar =
{
    bt_unsignedshort, 1, UF_DEFINED,  - 1,  - 1, 2
} ;
TYP stdwstring = 
{
    bt_pointer, 0, 0,  - 1,  - 1, 4, 0, &stdwchar
};
TYP stdint = 
{
    bt_int, 0, UF_DEFINED | UF_USED,  - 1,  - 1, 4
};
TYP stdlongdouble = 
{
    bt_longdouble, 0, 0,  - 1,  - 1, 10
};
TYP stdlrimaginary = 
{
    bt_lrimaginary, 0, 0,  - 1,  - 1, 10
};
TYP stdlrcomplex = 
{
    bt_lrcomplex, 0, 0,  - 1,  - 1, 20
};
TYP stduns = 
{
    bt_unsigned, 0, 0,  - 1,  - 1, 4
};
KEYWORDS prockeywords[] = 
{
    {
        0, "_absolute", kw__abs, 4
    }
    , 
    {
        0, "_pascal", kw__pascal, 4
    }
    , 
    {
        0, "_cdecl", kw__cdecl, 0
    }
    , 
    {
        0, "_stdcall", kw__stdcall, 4
    }
    , 
    {
        0, "__cdecl", kw__cdecl, 0
    }
    , 
    {
        0, "__stdcall", kw__stdcall, 4
    }
    , 
    {
        0, "_interrupt", kw__interrupt, 4
    }
    , 
    {
        0, "_genbyte", kw__genword, 4
    }
    , 
    {
        0, "_intrinsic", kw__intrinsic, 4
    }
    , 
    {
        0, "_trap", kw__trap, 4
    }
    , 
    {
        0, "_loadds", kw__loadds, 4
    }
    , 
    {
        0, "_far", kw__far, 4
    }
    , 
    {
        0, "_indirect", kw__indirect, 4
    }
    , 
    {
        0, "_export", kw__export, 4
    }
    , 
    {
        0, "__export", kw__export, 4
    }
    , 
    {
        0, "__import", kw__import, 4
    }
    , 
    {
        0, "_import", kw__import, 4
    }
    , 
    {
        0, "far", kw__far, 4
    }
    , 
    {
        0, "near", kw__near, 4
    }
    , 
    {
        0, "_fault", kw__fault, 4
    }
    , 
    {
        0, "_seg", kw__seg, 4
    }
    , 
    {
        0, "_near", kw__near, 4
    }
    , 
    {
        0, "_EAX", kw_D0, 4
    }
    , 
    {
        0, "_ECX", kw_D1, 4
    }
    , 
    {
        0, "_EDX", kw_D2, 4
    }
    , 
    {
        0, "_EBX", kw_D3, 4
    }
    , 
    {
        0, "_ESP", kw_D4, 4
    }
    , 
    {
        0, "_EBP", kw_D5, 4
    }
    , 
    {
        0, "_ESI", kw_D6, 4
    }
    , 
    {
        0, "_EDI", kw_D7, 4
    }
    , 
    {
        0, "_CS", kw_A8, 4
    }
    , 
    {
        0, "_DS", kw_A9, 4
    }
    , 
    {
        0, "_ES", kw_AA, 4
    }
    , 
    {
        0, "_FS", kw_AB, 4
    }
    , 
    {
        0, "_GS", kw_AC, 4
    }
    , 
    {
        0, "_SS", kw_AD, 4
    }
    , 
    {
        0, "_CR0", kw_cr0, 4
    }
    , 
    {
        0, "_CR1", kw_cr1, 4
    }
    , 
    {
        0, "_CR2", kw_cr2, 4
    }
    , 
    {
        0, "_CR3", kw_cr3, 4
    }
    , 
    {
        0, "_CR4", kw_cr4, 4
    }
    , 
    {
        0, "_CR5", kw_cr5, 4
    }
    , 
    {
        0, "_CR6", kw_cr6, 4
    }
    , 
    {
        0, "_CR7", kw_cr7, 4
    }
    , 
    {
        0, "_DR0", kw_dr0, 4
    }
    , 
    {
        0, "_DR1", kw_dr1, 4
    }
    , 
    {
        0, "_DR2", kw_dr2, 4
    }
    , 
    {
        0, "_DR3", kw_dr3, 4
    }
    , 
    {
        0, "_DR4", kw_dr4, 4
    }
    , 
    {
        0, "_DR5", kw_dr5, 4
    }
    , 
    {
        0, "_DR6", kw_dr6, 4
    }
    , 
    {
        0, "_DR7", kw_dr7, 4
    }
    , 
    {
        0, "_TR0", kw_tr0, 4
    }
    , 
    {
        0, "_TR1", kw_tr1, 4
    }
    , 
    {
        0, "_TR2", kw_tr2, 4
    }
    , 
    {
        0, "_TR3", kw_tr3, 4
    }
    , 
    {
        0, "_TR4", kw_tr4, 4
    }
    , 
    {
        0, "_TR5", kw_tr5, 4
    }
    , 
    {
        0, "_TR6", kw_tr6, 4
    }
    , 
    {
        0, "_TR7", kw_tr7, 4
    }
    , 
    {
        0, 0, 0
    }
};

char *registers[] = 
{
    "EAX", "ECX", "EDX", "EBX", "ESP", "EBP", "ESI", "EDI", "", "", "", "", "",
        "", "", "", "ESP", "EBP", "ESI", "EDI"
};

int confcodegen(char s, int bool)
{
    switch (s)
    {
        case 'I':
            prm_microlink = bool;
            break;
        case 'M':
            prm_masm = bool;
            prm_asmfile = bool;
            prm_compileonly = bool;
            break;
		case 'f':
			prm_fasm = bool;
			/* fallthrough */
        case 'N':
            prm_nasm = bool;
            prm_asmfile = bool;
            prm_compileonly = bool;
            break;
		case 'U':
			prm_dsnotss = bool;
			break;
        case 'T':
            prm_asmfile = bool;
            prm_compileonly = bool;
            break;
        case 'X':
            prm_nodos = bool;
            break;
        case 'F':
            prm_flat = bool;
            break;
        case 'R':
            prm_farkeyword = bool;
            break;
		case 'P':
            prm_farkeyword = bool;
			prm_fardefault = bool;
			break;
		case 's':
			prm_stackalign = bool;
			prm_smartframes = FALSE;
			if (bool)
			{
				stackadd = 15;
				stackmod = -16;
			}
			else
			{
				stackadd = 3;
				stackmod = -4;
			}
			break;
        case '?':
            prm_browse = bool;
            break;
        default:
            return 0;
    }
    return 1;
}

//-------------------------------------------------------------------------

void confsetup(void)
{
    cf_freeaddress = 2;
}

//-------------------------------------------------------------------------

int alignment(int type, TYP *tp)
{
    switch (tp->type)
    {
        case bt_char:
        case bt_unsignedchar:
        case bt_bool:
            return 1;
        case bt_short:
        case bt_unsignedshort:
            return 2;
        case bt_long:
        case bt_unsignedlong:
            return 4;
        case bt_longlong:
        case bt_unsignedlonglong:
            return 8;
        case bt_int:
        case bt_unsigned:
            return 4;
        case bt_enum:
            return 4;
        case bt_pointer:
			if (tp->val_flag)
				return alignment(type, tp->btp);
			else
				return 4;
        case bt_matchall:
            return 4;
        case bt_segpointer:
            return 2;
        case bt_farpointer:
            return 4;
        case bt_memberptr:
        case bt_float:
            return 4;
        case bt_double:
            return 8;
        case bt_longdouble:
            return 8;
        case bt_fcomplex:
            return 8;
        case bt_rcomplex:
            return 16;
        case bt_lrcomplex:
            return 16;
        case bt_class:
        case bt_struct:
        case bt_union:
            return tp->sp->structAlign ? tp->sp->structAlign: 1;
        default:
            return 1;
    }
}

//-------------------------------------------------------------------------

int getalign(int sc, TYP *tp)
{
    int align = alignment(sc, tp);
    if (sc != sc_auto)
        if (packdata[packlevel] < align)
            align = packdata[packlevel];
    return align;
}

//-------------------------------------------------------------------------

long getautoval(long val)
{
    return val;
}

//-------------------------------------------------------------------------

int funcvaluesize(int size)
{
    return 0;
}
