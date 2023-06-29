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

/*
 * keyword module
 */
#include "compiler.h"
#include "math.h"
#include <climits>
#include "Utils.h"
#include "PreProcessor.h"
#include <stack>
#include "lex.h"
#include "ccerr.h"
#include "config.h"
#include "stmt.h"
#include "occparse.h"
#include "template.h"
#include "constopt.h"
#include "memory.h"
#include "ifloatconv.h"
#include "browse.h"
#include "help.h"
#include "expr.h"
#include "template.h"
#include "declare.h"
#include "symtab.h"
#include "ListFactory.h"

namespace Parser
{
#ifndef LONGLONG_MAX
#    define LONGLONG_MAX LONG_MAX
#endif

int eofLine;
const char* eofFile;
bool parsingPreprocessorConstant;

LEXCONTEXT* context;

int charIndex;

LEXLIST* currentLex;
Optimizer::LINEDATA nullLineData = { 0, "", "", 0, 0 };

static bool valid;
static unsigned long long llminus1;
static int nextFree;
static const unsigned char* linePointer;
static std::string currentLine;
static int lastBrowseIndex;
struct ParseHold
{
    std::string currentLine;
    int charIndex;
};

static std::stack<ParseHold> parseStack;

KEYWORD keywords[] = {
    {"!", 1, notx, KW_ASSEMBLER, TT_UNARY | TT_OPERATOR},
    {"!=", 2, neq, 0, TT_RELATION | TT_EQUALITY},
    {"#", 1, hash, 0, TT_UNKNOWN},
    {"%", 1, mod, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"%=", 2, asmod, 0, TT_ASSIGN | TT_OPERATOR},
    {"&", 1, andx, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"&&", 2, land, 0, TT_BINARY | TT_OPERATOR},
    {"&=", 2, asand, 0, TT_ASSIGN | TT_OPERATOR},
    {"(", 1, openpa, KW_ASSEMBLER, TT_PRIMARY},
    {")", 1, closepa, KW_ASSEMBLER, TT_PRIMARY},
    {"*", 1, star, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"*=", 2, astimes, 0, TT_ASSIGN | TT_OPERATOR},
    {"+", 1, plus, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"++", 2, autoinc, 0, TT_UNARY | TT_INCREMENT},
    {"+=", 2, asplus, 0, TT_ASSIGN | TT_OPERATOR},
    {",", 1, comma, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"-", 1, minus, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"--", 2, autodec, 0, TT_UNARY | TT_INCREMENT},
    {"-=", 2, asminus, 0, TT_ASSIGN | TT_OPERATOR},
    {"->", 2, pointsto, 0, TT_BINARY | TT_POINTER},
    {"->*", 3, pointstar, 0, TT_BINARY | TT_POINTER},
    {".", 1, dot, 0, TT_BINARY | TT_POINTER},
    {".*", 2, dotstar, 0, TT_BINARY | TT_POINTER},
    {"...", 3, ellipse},
    {"/", 1, divide, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"/=", 2, asdivide, 0, TT_ASSIGN | TT_OPERATOR},
    {":", 1, colon, KW_ASSEMBLER},
    {"::", 2, classsel, KW_CPLUSPLUS | KW_MSIL, TT_BINARY | TT_SELECTOR},
    {";", 1, semicolon, KW_ASSEMBLER, TT_CONTROL},
    {"<", 1, lt, 0, TT_RELATION | TT_INEQUALITY},
    {"<<", 2, leftshift, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"<<=", 3, asleftshift, 0, TT_ASSIGN | TT_OPERATOR},
    {"<=", 2, leq, 0, TT_RELATION | TT_INEQUALITY},
    {"=", 1, assign, 0, TT_ASSIGN | TT_OPERATOR | TT_BASE},
    {"==", 2, eq, 0, TT_RELATION | TT_EQUALITY},
    {">", 1, gt, 0, TT_RELATION | TT_INEQUALITY},
    {">=", 2, geq, 0, TT_RELATION | TT_INEQUALITY},
    {">>", 2, rightshift, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {">>=", 3, asrightshift, 0, TT_ASSIGN | TT_OPERATOR},
    {"?", 1, hook, 0, TT_BINARY | TT_OPERATOR},
    {"[", 1, openbr, 0, TT_BINARY | TT_POINTER},
    {"]", 1, closebr, 0, TT_BINARY | TT_POINTER},
    {"^", 1, uparrow, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"^=", 2, asxor, 0, TT_ASSIGN | TT_OPERATOR},
    /*
    { "_A0", 3,  kw_A0, KW_NONANSI | KW_68K, TT_VAR },
    { "_A1", 3,  kw_A1, KW_NONANSI | KW_68K, TT_VAR  },
    { "_A2", 3,  kw_A2, KW_NONANSI | KW_68K, TT_VAR  },
    { "_A3", 3,  kw_A3, KW_NONANSI | KW_68K, TT_VAR  },
    { "_A4", 3,  kw_A4, KW_NONANSI | KW_68K, TT_VAR  },
    { "_A5", 3,  kw_A5, KW_NONANSI | KW_68K, TT_VAR  },
    { "_A6", 3,  kw_A6, KW_NONANSI | KW_68K, TT_VAR  },
    { "_A7", 3,  kw_A7, KW_NONANSI | KW_68K, TT_VAR  },
    */
    {"_Alignas", 8, kw_alignas, KW_C1X, TT_CONTROL},
    {"_Alignof", 8, kw_alignof, KW_C1X, TT_UNARY | TT_OPERATOR},
    {"_Atomic", 7, kw_atomic, KW_C1X | KW_CPLUSPLUS | KW_C2X, TT_POINTERQUAL | TT_TYPEQUAL | TT_BASETYPE},
    {"_Bool", 5, kw_bool, KW_C99 | KW_C1X, TT_BASETYPE | TT_BOOL},
    {"_CR0", 4, kw_cr0, KW_NONANSI | KW_386, TT_VAR},
    {"_CR1", 4, kw_cr1, KW_NONANSI | KW_386, TT_VAR},
    {"_CR2", 4, kw_cr2, KW_NONANSI | KW_386, TT_VAR},
    {"_CR3", 4, kw_cr3, KW_NONANSI | KW_386, TT_VAR},
    {"_CR4", 4, kw_cr4, KW_NONANSI | KW_386, TT_VAR},
    {"_CR5", 4, kw_cr5, KW_NONANSI | KW_386, TT_VAR},
    {"_CR6", 4, kw_cr6, KW_NONANSI | KW_386, TT_VAR},
    {"_CR7", 4, kw_cr7, KW_NONANSI | KW_386, TT_VAR},
    {"_Complex", 8, kw__Complex, 0, TT_BASETYPE | TT_COMPLEX},
    /*
    { "_D0", 3,  kw_D0, KW_NONANSI | KW_68K, TT_VAR  },
    { "_D1", 3,  kw_D1, KW_NONANSI | KW_68K, TT_VAR  },
    { "_D2", 3,  kw_D2, KW_NONANSI | KW_68K, TT_VAR  },
    { "_D3", 3,  kw_D3, KW_NONANSI | KW_68K, TT_VAR  },
    { "_D4", 3,  kw_D4, KW_NONANSI | KW_68K, TT_VAR  },
    { "_D5", 3,  kw_D5, KW_NONANSI | KW_68K, TT_VAR  },
    { "_D6", 3,  kw_D6, KW_NONANSI | KW_68K, TT_VAR  },
    { "_D7", 3,  kw_D7, KW_NONANSI | KW_68K, TT_VAR  },
    */
    {"_DR0", 4, kw_dr0, KW_NONANSI | KW_386, TT_VAR},
    {"_DR1", 4, kw_dr1, KW_NONANSI | KW_386, TT_VAR},
    {"_DR2", 4, kw_dr2, KW_NONANSI | KW_386, TT_VAR},
    {"_DR3", 4, kw_dr3, KW_NONANSI | KW_386, TT_VAR},
    {"_DR4", 4, kw_dr4, KW_NONANSI | KW_386, TT_VAR},
    {"_DR5", 4, kw_dr5, KW_NONANSI | KW_386, TT_VAR},
    {"_DR6", 4, kw_dr6, KW_NONANSI | KW_386, TT_VAR},
    {"_DR7", 4, kw_dr7, KW_NONANSI | KW_386, TT_VAR},
    {"_EAX", 4, kw_D0, KW_NONANSI | KW_386, TT_VAR},
    {"_EBP", 4, kw_D5, KW_NONANSI | KW_386, TT_VAR},
    {"_EBX", 4, kw_D3, KW_NONANSI | KW_386, TT_VAR},
    {"_ECX", 4, kw_D1, KW_NONANSI | KW_386, TT_VAR},
    {"_EDI", 4, kw_D7, KW_NONANSI | KW_386, TT_VAR},
    {"_EDX", 4, kw_D2, KW_NONANSI | KW_386, TT_VAR},
    {"_ESI", 4, kw_D6, KW_NONANSI | KW_386, TT_VAR},
    {"_ESP", 4, kw_D4, KW_NONANSI | KW_386, TT_VAR},
    {"_FP0", 4, kw_F0, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP1", 4, kw_F1, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP2", 4, kw_F2, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP3", 4, kw_F3, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP4", 4, kw_F4, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP5", 4, kw_F5, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP6", 4, kw_F6, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP7", 4, kw_F7, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_Generic", 8, kw_generic, KW_C1X, TT_VAR},
    {"_INF", 4, kw__INF, 0, TT_VAR},
    {"_Imaginary", 10, kw__Imaginary, 0, TT_BASETYPE | TT_COMPLEX},
    {"_NAN", 4, kw__NAN, 0, TT_VAR},
    {"_Noreturn", 9, kw_noreturn, KW_C1X, TT_LINKAGE},
    {"_Pragma", 7, kw__Pragma, KW_C99 | KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"_SS", 3, kw_AD, KW_NONANSI | KW_386, TT_VAR},
    {"_Static_assert", 14, kw_static_assert, KW_C1X, 0},
    {"_Thread_local", 13, kw_thread_local, KW_C1X, TT_LINKAGE},
    {"_TR0", 4, kw_tr0, KW_NONANSI | KW_386, TT_VAR},
    {"_TR1", 4, kw_tr1, KW_NONANSI | KW_386, TT_VAR},
    {"_TR2", 4, kw_tr2, KW_NONANSI | KW_386, TT_VAR},
    {"_TR3", 4, kw_tr3, KW_NONANSI | KW_386, TT_VAR},
    {"_TR4", 4, kw_tr4, KW_NONANSI | KW_386, TT_VAR},
    {"_TR5", 4, kw_tr5, KW_NONANSI | KW_386, TT_VAR},
    {"_TR6", 4, kw_tr6, KW_NONANSI | KW_386, TT_VAR},
    {"_TR7", 4, kw_tr7, KW_NONANSI | KW_386, TT_VAR},
    {"__CS", 4, kw_A8, KW_NONANSI | KW_386, TT_VAR},
    {"__DS", 4, kw_A9, KW_NONANSI | KW_386, TT_VAR},
    {"__ES", 4, kw_AA, KW_NONANSI | KW_386, TT_VAR},
    {"__FS", 4, kw_AB, KW_NONANSI | KW_386, TT_VAR},
    {"__GS", 4, kw_AC, KW_NONANSI | KW_386, TT_VAR},
    {"__I", 3, kw___I, 0, TT_VAR},
    {"__alignof", 9, kw_alignof, 0, TT_UNARY | TT_OPERATOR},
    {"__alignof__", 11, kw_alignof, 0, TT_UNARY | TT_OPERATOR},
    {"__alloca", 8, kw_alloca, KW_NONANSI | KW_ALL, TT_OPERATOR | TT_UNARY},
    {"__asm", 5, kw_asm, KW_NONANSI | KW_ALL, TT_CONTROL},
    {"__atomic_flag_test_set", 22, kw_atomic_flag_test_set, 0, TT_VAR},
    {"__atomic_flag_clear", 19, kw_atomic_flag_clear, 0, TT_VAR},
    {"__c11_atomic_init", 17, kw_c11_atomic_init, 0, TT_VAR},
    {"__c11_atomic_thread_fence", 25, kw_c11_atomic_thread_fence, 0, TT_VAR},
    {"__c11_atomic_signal_fence", 25, kw_c11_atomic_signal_fence, 0, TT_VAR},
    {"__c11_atomic_is_lock_free", 25, kw_c11_atomic_is_lock_free, 0, TT_VAR},
    {"__c11_atomic_store", 18, kw_c11_atomic_store, 0, TT_VAR},
    {"__c11_atomic_load", 17, kw_c11_atomic_load, 0, TT_VAR},
    {"__c11_atomic_exchange", 21, kw_c11_atomic_xchg, 0, TT_VAR},
    {"__c11_atomic_compare_exchange_strong", 36, kw_c11_atomic_cmpxchg_strong, 0, TT_VAR},
    {"__c11_atomic_compare_exchange_weak", 34, kw_c11_atomic_cmpxchg_weak, 0, TT_VAR},
    {"__c11_atomic_fetch_add", 22, kw_c11_atomic_ftchadd, 0, TT_VAR},
    {"__c11_atomic_fetch_sub", 22, kw_c11_atomic_ftchsub, 0, TT_VAR},
    {"__c11_atomic_fetch_and", 22, kw_c11_atomic_ftchand, 0, TT_VAR},
    {"__c11_atomic_fetch_xor", 22, kw_c11_atomic_ftchxor, 0, TT_VAR},
    {"__c11_atomic_fetch_or", 21, kw_c11_atomic_ftchor, 0, TT_VAR},
    {"__atomic_load_n", 15, kw_c11_atomic_load, 0, TT_VAR},
    {"__atomic_store_n", 16, kw_c11_atomic_store, 0, TT_VAR},
    {"__atomic_exchange_n", 19, kw_c11_atomic_xchg, 0, TT_VAR},
    {"__atomic_compare_exchange_n", 27, kw_atomic_cmpxchg_n, 0, TT_VAR},
    {"__atomic_add_fetch", 18, kw_atomic_addftch, 0, TT_VAR},
    {"__atomic_sub_fetch", 18, kw_atomic_subftch, 0, TT_VAR},
    {"__atomic_and_fetch", 18, kw_atomic_andftch, 0, TT_VAR},
    {"__atomic_xor_fetch", 18, kw_atomic_xorftch, 0, TT_VAR},
    {"__atomic_or_fetch", 17, kw_atomic_orftch, 0, TT_VAR},
    {"__atomic_fetch_add", 22, kw_c11_atomic_ftchadd, 0, TT_VAR},  // c11 keywords were added first so we use em'
    {"__atomic_fetch_sub", 22, kw_c11_atomic_ftchsub, 0, TT_VAR},
    {"__atomic_fetch_and", 22, kw_c11_atomic_ftchand, 0, TT_VAR},
    {"__atomic_fetch_xor", 22, kw_c11_atomic_ftchxor, 0, TT_VAR},
    {"__atomic_fetch_or", 21, kw_c11_atomic_ftchor, 0, TT_VAR},
    {"__attribute__", 13, kw__attribute, 0, TT_VAR},
    {"__catch", 7, kw___catch, KW_MSIL, TT_CONTROL},
    {"__cdecl", 7, kw__cdecl, 0, TT_LINKAGE},
    {"__char16_t", 10, kw_char16_t, KW_CPLUSPLUS | KW_C1X, TT_BASETYPE | TT_INT},
    {"__char32_t", 10, kw_char32_t, KW_CPLUSPLUS | KW_C1X, TT_BASETYPE | TT_INT},
    {"__cpblk", 7, kw__cpblk, KW_MSIL, TT_OPERATOR | TT_UNARY},
    {"__declspec", 10, kw__declspec, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__entrypoint", 12, kw__entrypoint, KW_MSIL, TT_LINKAGE},
    {"__export", 8, kw__export, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__fastcall", 10, kw__fastcall, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__fastcall__", 12, kw__fastcall, 0, TT_LINKAGE},
    {"__fault", 7, kw___fault, KW_MSIL, TT_CONTROL},
    {"__finally", 9, kw___finally, KW_MSIL, TT_CONTROL},
    {"__func__", 8, kw___func__, KW_C99 | KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"__import", 8, kw__import, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__initblk", 9, kw__initblk, KW_MSIL, TT_OPERATOR | TT_UNARY},
    {"__inline", 8, kw_inline, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__int16", 7, kw_short, KW_NONANSI | KW_386 | KW_MSIL, TT_BASETYPE | TT_INT},
    {"__int32", 7, kw_int, KW_NONANSI | KW_386 | KW_MSIL, TT_BASETYPE | TT_INT | TT_BASE},
    {"__int64", 7, kw___int64, KW_NONANSI | KW_386 | KW_MSIL, TT_BASETYPE | TT_INT},
    {"__int64__", 9, kw___int64, 0, TT_BASETYPE | TT_INT},
    {"__int8", 6, kw_char, KW_NONANSI | KW_386, TT_BASETYPE | TT_INT},
    {"__kill_dependency", 17, kw_atomic_kill_dependency, 0, TT_VAR},
    {"__msil_rtl", 10, kw__msil_rtl, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__object", 8, kw___object, KW_MSIL, TT_BASETYPE},
    {"__offsetof", 10, kw___offsetof, 0, TT_VAR},
    {"__pascal", 8, kw__pascal, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__property", 10, kw__property, KW_MSIL, TT_LINKAGE},
    {"__rtllinkage", 12, kw__rtllinkage, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__stdcall", 9, kw__stdcall, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__string", 8, kw___string, KW_MSIL, TT_BASETYPE},
    {"__try", 5, kw___try, KW_MSIL, TT_CONTROL},
    {"__typeid", 8, kw___typeid, KW_NONANSI | KW_ALL, TT_VAR},
    {"__underlying_type", 17, kw___underlying_type, KW_CPLUSPLUS, TT_BASETYPE},
    {"__unmanaged", 11, kw__unmanaged, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__uuid", 6, kw__uuid, 0, TT_LINKAGE},
    {"__uuidof", 8, kw__uuidof, 0, TT_VAR},
    {"__va_list__", 11, kw___va_list__, KW_NONANSI | KW_ALL, TT_TYPEQUAL | TT_POINTERQUAL},
    {"__va_typeof__", 13, kw___va_typeof__, KW_NONANSI | KW_ALL, TT_VAR},
    {"__volatile", 10, kw__volatile, KW_NONANSI | KW_ALL, TT_VAR},
    {"_absolute", 9, kw__absolute, KW_NONANSI | KW_ALL, TT_STORAGE_CLASS},
    {"_asm", 4, kw_asm, KW_NONANSI | KW_ALL, TT_CONTROL},
    {"_cdecl", 6, kw__cdecl, KW_NONANSI, TT_LINKAGE},
    {"_export", 7, kw__export, KW_NONANSI, TT_LINKAGE},
    //	{ "_far", 4,  kw__far, KW_NONANSI, TT_TYPEQUAL | TT_POINTERQUAL },
    {"_fault", 6, kw__fault, KW_NONANSI, TT_LINKAGE},
    {"_genbyte", 8, kw__genword, KW_NONANSI | KW_386, TT_UNARY | TT_OPERATOR},
    {"_genword", 8, kw__genword, KW_NONANSI | KW_68K, TT_UNARY | TT_OPERATOR},
    {"_import", 7, kw__import, KW_NONANSI, TT_LINKAGE},
    {"_indirect", 9, kw__indirect, KW_NONANSI | KW_ALL},
    {"_interrupt", 10, kw__interrupt, KW_NONANSI, TT_LINKAGE},
    {"_intrinsic", 10, kw__intrinsic, KW_NONANSI | KW_ALL, TT_TYPEQUAL},
    {"_loadds", 7, kw__loadds, KW_NONANSI | KW_386, TT_TYPEQUAL},
    //	{ "_near", 5,  kw__near, KW_NONANSI | KW_ALL, TT_TYPEQUAL | TT_POINTERQUAL},
    {"_pascal", 7, kw__pascal, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"_seg", 4, kw__seg, KW_NONANSI | KW_ALL, TT_TYPEQUAL | TT_POINTERQUAL},
    {"_stdcall", 8, kw__stdcall, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"_trap", 5, kw__trap, KW_NONANSI | KW_ALL, TT_OPERATOR | TT_UNARY},
    {"alignas", 7, kw_alignas, KW_CPLUSPLUS | KW_C2X, TT_CONTROL},
    {"alignof", 7, kw_alignof, KW_CPLUSPLUS | KW_C2X, TT_UNARY | TT_OPERATOR},
    {"and", 3, land, KW_CPLUSPLUS, TT_BINARY | TT_OPERATOR},
    {"and_eq", 6, asand, KW_CPLUSPLUS, TT_ASSIGN | TT_OPERATOR},
    {"asm", 3, kw_asm, KW_NONANSI | KW_ALL, TT_CONTROL},
    {"auto", 4, kw_auto, 0, TT_STORAGE_CLASS},
    {"bitand", 6, andx, KW_CPLUSPLUS, TT_BINARY | TT_OPERATOR},
    {"bitor", 5, orx, KW_CPLUSPLUS, TT_BINARY | TT_OPERATOR},
    {"bool", 4, kw_bool, KW_CPLUSPLUS | KW_C2X, TT_BASETYPE | TT_BOOL},
    {"break", 5, kw_break, 0, TT_CONTROL},
    {"case", 4, kw_case, 0, TT_CONTROL | TT_SWITCH},
    {"catch", 5, kw_catch, KW_CPLUSPLUS, TT_CONTROL},
    {"cdecl", 5, kw__cdecl, 0, TT_LINKAGE},
    {"char", 4, kw_char, 0, TT_BASETYPE | TT_INT},
    {"char16_t", 8, kw_char16_t, KW_CPLUSPLUS, TT_BASETYPE | TT_INT},
    {"char32_t", 8, kw_char32_t, KW_CPLUSPLUS, TT_BASETYPE | TT_INT},
    {"class", 5, kw_class, KW_CPLUSPLUS, TT_BASETYPE | TT_STRUCT},
    {"compl", 5, complx, KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"const", 5, kw_const, KW_ASSEMBLER, TT_POINTERQUAL | TT_TYPEQUAL},
    {"const_cast", 10, kw_const_cast, KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"constexpr", 9, kw_constexpr, KW_CPLUSPLUS, (unsigned long)TT_DECLARE},
    {"continue", 8, kw_continue, 0, TT_CONTROL},
    {"decltype", 8, kw_decltype, KW_CPLUSPLUS, TT_OPERATOR},
    {"default", 7, kw_default, 0, TT_CONTROL},
    {"delete", 6, kw_delete, KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"do", 2, kw_do, 0, TT_CONTROL},
    {"double", 6, kw_double, 0, TT_BASETYPE | TT_FLOAT},
    {"dynamic_cast", 12, kw_dynamic_cast, KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"else", 4, kw_else, 0, TT_CONTROL},
    {"enum", 4, kw_enum, 0, TT_BASETYPE | TT_ENUM},
    {"explicit", 8, kw_explicit, KW_CPLUSPLUS, TT_STORAGE_CLASS},
    {"export", 6, kw_export, KW_CPLUSPLUS, TT_UNKNOWN},
    {"extern", 6, kw_extern, KW_ASSEMBLER, TT_STORAGE_CLASS},
    {"false", 5, kw_false, KW_CPLUSPLUS | KW_C2X, TT_VAR},
    //	{ "far", 3,  kw__far, KW_NONANSI | KW_ALL, TT_POINTERQUAL | TT_TYPEQUAL},
    {"float", 5, kw_float, 0, TT_BASETYPE | TT_FLOAT},
    {"for", 3, kw_for, 0, TT_CONTROL},
    {"friend", 6, kw_friend, KW_CPLUSPLUS, TT_LINKAGE},
    {"goto", 4, kw_goto, 0, TT_CONTROL},
    {"if", 2, kw_if, 0, TT_CONTROL},
    {"inline", 6, kw_inline, KW_C99 | KW_CPLUSPLUS, TT_LINKAGE},
    {"int", 3, kw_int, 0, TT_BASETYPE | TT_INT | TT_BASE},
    {"long", 4, kw_long, 0, TT_BASETYPE | TT_INT},
    {"mutable", 7, kw_mutable, KW_CPLUSPLUS, TT_STORAGE_CLASS},
    {"namespace", 9, kw_namespace, KW_CPLUSPLUS | KW_MSIL, 0},
    {"native", 6, kw_native, KW_MSIL, TT_BASETYPE | TT_INT | TT_BASE},
    //	{ "near", 4,  kw__near, KW_NONANSI | KW_ALL, TT_POINTERQUAL | TT_TYPEQUAL},
    {"new", 3, kw_new, KW_CPLUSPLUS, TT_OPERATOR | TT_UNARY},
    {"noexcept", 8, kw_noexcept, KW_CPLUSPLUS, TT_CONTROL},
    {"not", 3, notx, KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"not_eq", 6, neq, KW_CPLUSPLUS, TT_RELATION | TT_EQUALITY},
    {"nullptr", 7, kw_nullptr, KW_CPLUSPLUS | KW_C2X, TT_VAR},
    {"operator", 8, kw_operator, KW_CPLUSPLUS, TT_OPERATOR},
    {"or", 2, lor, KW_CPLUSPLUS, TT_BINARY | TT_OPERATOR},
    {"or_eq", 5, asor, KW_CPLUSPLUS, TT_ASSIGN | TT_OPERATOR},
    {"private", 7, kw_private, KW_CPLUSPLUS, TT_CLASS},
    {"protected", 9, kw_protected, KW_CPLUSPLUS, TT_CLASS},
    {"public", 6, kw_public, KW_CPLUSPLUS | KW_ASSEMBLER, TT_CLASS},
    {"register", 8, kw_register, 0, TT_STORAGE_CLASS},
    {"reinterpret_cast", 16, kw_reinterpret_cast, KW_CPLUSPLUS, TT_OPERATOR | TT_UNARY},
    {"restrict", 8, kw_restrict, KW_C99, TT_POINTERQUAL | TT_TYPEQUAL},
    {"return", 6, kw_return, 0, TT_CONTROL},
    {"short", 5, kw_short, 0, TT_BASETYPE | TT_INT},
    {"signed", 6, kw_signed, 0, TT_BASETYPE | TT_INT},
    {"sizeof", 6, kw_sizeof, 0, TT_UNARY | TT_OPERATOR},
    {"static", 6, kw_static, 0, TT_STORAGE_CLASS},
    {"static_assert", 13, kw_static_assert, KW_CPLUSPLUS, 0},
    {"static_cast", 11, kw_static_cast, KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"struct", 6, kw_struct, 0, TT_BASETYPE | TT_STRUCT},
    {"switch", 6, kw_switch, 0, TT_CONTROL},
    {"template", 8, kw_template, KW_CPLUSPLUS, TT_CONTROL},
    {"this", 4, kw_this, KW_CPLUSPLUS, TT_VAR},
    {"thread_local", 12, kw_thread_local, KW_CPLUSPLUS, TT_LINKAGE},
    {"throw", 5, kw_throw, KW_CPLUSPLUS, TT_OPERATOR | TT_UNARY},
    {"true", 4, kw_true, KW_CPLUSPLUS | KW_C2X, TT_VAR},
    {"try", 3, kw_try, KW_CPLUSPLUS, TT_CONTROL},
    {"typedef", 7, kw_typedef, 0, TT_BASETYPE | TT_TYPEDEF | TT_STORAGE_CLASS},
    {"typeid", 6, kw_typeid, KW_CPLUSPLUS, TT_UNKNOWN},
    {"typename", 8, kw_typename, KW_CPLUSPLUS, TT_TYPENAME},
    {"typeof", 6, kw_typeof, 0, TT_BASETYPE | TT_OPERATOR},
    {"union", 5, kw_union, 0, TT_BASETYPE | TT_STRUCT},
    {"unsigned", 8, kw_unsigned, 0, TT_BASETYPE | TT_INT | TT_BASE},
    {"using", 5, kw_using, KW_CPLUSPLUS | KW_MSIL, TT_CONTROL},
    {"virtual", 7, kw_virtual, KW_CPLUSPLUS, TT_STORAGE_CLASS},
    {"void", 4, kw_void, 0, TT_BASETYPE | TT_VOID},
    {"volatile", 8, kw_volatile, 0, TT_TYPEQUAL | TT_POINTERQUAL},
    {"wchar_t", 7, kw_wchar_t, KW_CPLUSPLUS, TT_BASETYPE | TT_INT},
    {"while", 5, kw_while, 0, TT_CONTROL},
    {"xor", 3, uparrow, KW_CPLUSPLUS, TT_BINARY | TT_OPERATOR},
    {"xor_eq", 6, asxor, KW_CPLUSPLUS, TT_ASSIGN | TT_OPERATOR},
    {"{", 1, begin, 0, TT_CONTROL | TT_BLOCK},
    {"|", 1, orx, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"|=", 2, asor, 0, TT_ASSIGN | TT_OPERATOR},
    {"||", 2, lor, 0, TT_BINARY | TT_OPERATOR},
    {"}", 1, end, 0, TT_CONTROL | TT_BLOCK},
    {"~", 1, complx, KW_ASSEMBLER, TT_UNARY | TT_OPERATOR},
};

#define TABSIZE (sizeof(keywords) / sizeof(keywords[0]))
#ifdef KW_HASH
SymbolTableFactory<KEYWORD> lexFactory;
SymbolTable<KEYWORD>* kwSymbols;
#endif

static bool kwmatches(KEYWORD* kw);
void lexini(void)
/*
 * create a keyword hash table
 */
{
    bool old = Optimizer::cparams.prm_extwarning;
    Optimizer::cparams.prm_extwarning = false;
#ifdef KW_HASH
    int i;
    lexFactory.Reset();
    kwSymbols = lexFactory.CreateSymbolTable();
    for (i = 0; i < TABSIZE; i++)
    {
        if (kwmatches(&keywords[i]))
            kwSymbols->Add(&keywords[i]);
    }
#endif
    llminus1 = 0;
    llminus1--;
    context = Allocate<LEXCONTEXT>();
    nextFree = 0;
    currentLine = "";
    linePointer = (const unsigned char*)currentLine.c_str();
    while (parseStack.size())
        parseStack.pop();
    lastBrowseIndex = 0;
    Optimizer::cparams.prm_extwarning = old;
}

/*-------------------------------------------------------------------------*/

#ifndef KW_HASH
static KEYWORD* binarySearch(char* name)
{
    int top = TABSIZE;
    int bottom = -1;
    int v;
    KEYWORD* kw;
    while (top - bottom > 1)
    {
        int mid = (top + bottom) / 2;
        kw = &keywords[mid];
        v = strncmp(name, kw->name, kw->len);
        if (v < 0)
        {
            top = mid;
        }
        else
        {
            bottom = mid;
        }
    }
    if (bottom == -1)
        return 0;
    kw = &keywords[bottom];
    v = strncmp(name, kw->name, kw->len);
    if (v)
        return 0;
    return &keywords[bottom];
}
#endif
static bool kwmatches(KEYWORD* kw)
{
    if (Optimizer::cparams.prm_assemble)
        return !!(kw->matchFlags & KW_ASSEMBLER);
    else if (!kw->matchFlags || kw->matchFlags == KW_ASSEMBLER)
        return true;
    else if (((kw->matchFlags & KW_CPLUSPLUS) && Optimizer::cparams.prm_cplusplus) ||
             (kw->matchFlags & (KW_C99 | KW_C1X | KW_C2X)) ||
             ((kw->matchFlags & KW_MSIL) && (Optimizer::architecture == ARCHITECTURE_MSIL) &&
              Optimizer::cparams.msilAllowExtensions) ||
             ((kw->matchFlags & (KW_NONANSI | KW_INLINEASM)) && !Optimizer::cparams.prm_ansi))
    {
        if (kw->matchFlags & KW_NONANSI)
        {
            /* fill in here for processor specific */
        }
        return true;
    }
    return false;
}
KEYWORD* searchkw(const unsigned char** p)
/*
 * see if the current symbol is a keyword
 */
{
    KEYWORD* kw;
    unsigned char buf[1000], *q = buf;
    const unsigned char* q1 = *p;
    if (isstartchar(*q1))
    {
        int len = 0;
        while (issymchar(*q1))
        {
            len++;
            *q++ = *q1++;
        }
        *q = 0;
#ifdef KW_HASH
        kw = search(kwSymbols, (char*) buf);
        if (kw)
#else
        kw = (KEYWORD*)binarySearch(buf);
        if (kw && kwmatches(kw))
#endif
        {
            if (len == kw->len)
            {
                int count = 0;
                if (kw->matchFlags & (KW_C99 | KW_C1X | KW_C2X))
                {
                    if (Optimizer::cparams.prm_c99 && (kw->matchFlags & KW_C99))
                        count++;
                    if (Optimizer::cparams.prm_c1x && (kw->matchFlags & KW_C1X))
                        count++;
                    if (Optimizer::cparams.prm_c2x && (kw->matchFlags & KW_C2X))
                        count++;
                    if (Optimizer::cparams.prm_cplusplus && (kw->matchFlags & KW_CPLUSPLUS))
                        count++;
                    if (!count)
                    {
                        errorstr(ERR_C99_KEYWORD, (char*)buf);
                        return nullptr;
                    }
                }
                *p = *p + len;
                return kw;
            }
        }
    }
    else
    {
        KEYWORD* found = nullptr;
#ifdef KW_HASH
        int len = 0;
        while (ispunct((unsigned char)*q1))
            *q++ = *q1++, len++;
        if (len)
        {
            buf[len] = 0;
            while (len && (found = search(kwSymbols, (char*) buf)) == nullptr)
            {
                buf[--len] = 0;
            }
            if (found)
#else
        *q++ = *q1++;
        *q = 0;
        kw = (KEYWORD*)binarySearch(buf);
        if (kw)
        {
            KEYWORD* list = kw;
            found = kw;
            while (list < &keywords[0] + sizeof(keywords) / sizeof(keywords[0]) && buf[0] == list->name[0])
            {
                if (!strncmp(list->name, *p, list->len))
                {
                    found = list;
                }
                list++;
            }
            if (kwmatches(found))
#endif
            {
                *p = *p + found->len;
                return found;
            }
        }
    }
    return nullptr;
}
int getsch(int bytes, const unsigned char** source) /* return an in-quote character */
{
    int i = *(*source)++, j;
    if (**source == '\n')
        return INT_MIN;
    if (i != '\\')
    {
        return (char)i;
    }
    i = *(*source); /* get an escaped character */
    if (isdigit(i) && i < '8')
    {
        for (i = 0, j = 0; j < 3; ++j)
        {
            if (*(*source) <= '7' && *(*source) >= '0')
                i = (i << 3) + *(*source) - '0';
            else
                break;
            (*source)++;
        }
        return i;
    }
    (*source)++;
    switch (i)
    {
        case 'a':
            return '\a';
        case 'b':
            return '\b';
        case 'f':
            return '\f';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 'v':
            return '\v';
        case 't':
            return '\t';
        case '\'':
            return '\'';
        case '\"':
            return '\"';
        case '\\':
            return '\\';
        case '?':
            return '?';
        case 'U':
            bytes = 4;
        case 'u':
            if (i == 'u')
                bytes = 2;
        case 'x': {
            int n = 0;
            while (isxdigit(*(*source)))
            {
                int ch = *(*source)++;
                if (ch >= 0x60)
                    ch &= 0xdf;
                ch -= 0x30;

                if (ch > 10)
                    ch -= 7;
                if (ch > 15)
                    ch -= 32;
                n *= 16;
                n += ch;
            }
            /* hexadecimal escape sequences are only terminated by a non hex char */
            /* we sign extend or truncate */
            if (bytes == 1)
            {
                if (Optimizer::cparams.prm_charisunsigned)
                    n = (int)(Optimizer::UBYTE)n;
                else
                    n = (int)(char)n;
            }
            if (bytes == 2 && i == 'x')
                n = (int)(LCHAR)n;
            if (i != 'x')
            {
                if (n <= 0x20 || (n >= 0x7f && n <= 0x9f) || (n >= 0xd800 && n <= 0xdfff))
                    pperror(ERR_INVCONST, 0);
            }
            return n;
        }
        default:
            return (char)i;
    }
}

int getChar(const unsigned char** source, enum e_lexType* tp)
{
    enum e_lexType v = l_achr;
    const unsigned char* p = *source;
    if (*p == 'L')
    {
        v = l_wchr;
        p++;
    }
    else if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.prm_c1x)
    {
        if (*p == 'u')
        {
            v = l_uchr;
            p++;
        }
        else if (*p == 'U')
        {
            v = l_Uchr;
            p++;
        }
    }
    if (*p == '\'')
    {
        int i;
        do
            p++;
        while (*p == ppDefine::MACRO_PLACEHOLDER);
        i = getsch(v == l_Uchr ? 8 : v == l_wchr || v == l_uchr ? 4 : 2, &p);
        if (i == INT_MIN)
        {
            error(ERR_INVALID_CHAR_CONSTANT);
            i = '0';
        }
        if (*p != '\'')
        {
            if (v == l_uchr || v == l_Uchr)
            {
                error(ERR_CHAR1632_CONSTANT_TOO_LONG);
            }
            else
            {
                int j;
                j = getsch(v == l_Uchr ? 8 : v == l_wchr || v == l_uchr ? 4 : 2, &p);
                if (j == INT_MIN)
                {
                    error(ERR_INVALID_CHAR_CONSTANT);
                    j = '0';
                }
                i = (i << 8) + j;
                if (*p != '\'')
                {
                    j = getsch(v == l_Uchr ? 8 : v == l_wchr || v == l_uchr ? 4 : 2, &p);
                    if (j == INT_MIN)
                    {
                        error(ERR_INVALID_CHAR_CONSTANT);
                        j = '0';
                    }
                    i = (i << 8) + j;
                    if (*p != '\'')
                    {
                        j = getsch(v == l_Uchr ? 8 : v == l_wchr || v == l_uchr ? 4 : 2, &p);
                        if (j == INT_MIN)
                        {
                            error(ERR_INVALID_CHAR_CONSTANT);
                            j = '0';
                        }
                        i = (i << 8) + j;
                        if (*p != '\'')
                        {
                            error(ERR_UNTERM_CHAR_CONSTANT);
                        }
                        else
                            do
                                p++;
                            while (*p == ppDefine::MACRO_PLACEHOLDER);
                    }
                    else
                        do
                            p++;
                        while (*p == ppDefine::MACRO_PLACEHOLDER);
                }
                else
                    do
                        p++;
                    while (*p == ppDefine::MACRO_PLACEHOLDER);
            }
        }
        else
            do
                p++;
            while (*p == ppDefine::MACRO_PLACEHOLDER);
        *tp = v;
        *source = p;
        return i;
    }
    return INT_MIN;
}
int nextch()
{
    if (!*linePointer)
    {
        if (!preProcessor->GetLine(currentLine))
            return -1;
        linePointer = (const unsigned char*)currentLine.c_str();
    }
    return *linePointer++;
}
Optimizer::SLCHAR* getString(const unsigned char** source, enum e_lexType* tp)
{
    // the static declaration speeds it up by about 5% on windows platforms.
    static LCHAR data[32768];
    LCHAR* dest = data;
    bool raw = false;
    bool found = false;
    bool msil = false;
    const unsigned char* p = *source;
    int len = sizeof(data) / sizeof(data[0]);
    int count = 0;
    int errored = 0;
    enum e_lexType v = l_astr;
    if (*p == 'L')
    {
        v = l_wstr;
        do
            p++;
        while (*p == ppDefine::MACRO_PLACEHOLDER);
    }
    else if (*p == '@' && (Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions)
    {
        v = l_msilstr;
        do
            p++;
        while (*p == ppDefine::MACRO_PLACEHOLDER);
    }
    else if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.prm_c1x)
    {
        if (*p == 'u')
        {
            v = l_ustr;
            do
                p++;
            while (*p == ppDefine::MACRO_PLACEHOLDER);
            if (*p == '8')
            {
                v = l_u8str;
                do
                    p++;
                while (*p == ppDefine::MACRO_PLACEHOLDER);
            }
        }
        else if (*p == 'U')
        {
            v = l_Ustr;
            do
                p++;
            while (*p == ppDefine::MACRO_PLACEHOLDER);
        }
    }
    if (Optimizer::cparams.prm_cplusplus && *p == 'R')
    {
        raw = true;
        do
            p++;
        while (*p == ppDefine::MACRO_PLACEHOLDER);
    }
    if (*p == '"')
    {
        do
            p++;
        while (*p == ppDefine::MACRO_PLACEHOLDER);
        if (raw)
        {
            // fixme utf8 raw strings...
            char preamble[16];
            int pcount = 0, qcount;
            LCHAR* qpos = 0;
            int lineno = preProcessor->GetErrLineNo();
            unsigned char st[2];
            bool err = false;
            while (true)
            {
                if (*p)
                {
                    st[0] = *p;
                    do
                        p++;
                    while (*p == ppDefine::MACRO_PLACEHOLDER);
                }
                else if ((st[0] = nextch()))
                {
                    errorint(ERR_EOF_RAW_STRING, lineno);
                    *source = p;
                    return nullptr;
                }
                if (err)
                {
                    if (st[0] == '"')
                    {
                        Optimizer::SLCHAR* rv;
                        int i;
                        *source = p;
                        rv = Allocate<Optimizer::SLCHAR>();
                        rv->str = Allocate<LCHAR>(1);
                        rv->str[0] = 0;
                        rv->count = 1;
                        return rv;
                    }
                }
                else if (st[0] == '(')
                {
                    break;
                }
                else if (st[0] == '\n' || st[0] == ' ' || st[0] == '\v' || st[0] == '\t' || st[0] == '\f' || st[0] == ')' ||
                         count >= 16)
                {
                    error(ERR_RAW_STRING_INVALID_CHAR);
                    err = true;
                }
                else
                {
                    preamble[pcount++] = st[0];
                }
            }
            if (st[0] != '(')
                error(ERR_RAW_STRING_INVALID_CHAR);
            else
                while (true)
                {
                    if (*p)
                    {
                        st[0] = *p;
                        do
                            p++;
                        while (*p == ppDefine::MACRO_PLACEHOLDER);
                    }
                    else if ((st[0] = nextch()))
                    {
                        errorint(ERR_EOF_RAW_STRING, lineno);
                        *source = p;
                        return nullptr;
                    }
                    if (len == 1)
                    {
                        error(ERR_STRING_CONSTANT_TOO_LONG);
                    }
                    else
                    {
                        *dest++ = st[0];
                        len--;
                        count++;
                        if (st[0] == ')')
                        {
                            qcount = 0;
                            qpos = dest;
                        }
                        else if (qpos)
                        {
                            if (qcount == pcount)
                            {
                                if (st[0] == '"')
                                {
                                    count -= 2 + pcount;
                                    dest -= 2 + pcount;
                                    break;
                                }
                            }
                            if (st[0] != preamble[qcount])
                                qpos = 0;
                            else
                                qcount++;
                        }
                    }
                }
            *dest = 0;
            found = true;
            while (isspace(*p) || *p == ppDefine::MACRO_PLACEHOLDER)
                p++;
            *source = p;
        }
        else
        {
            while (*p && *p != '"')
            {
                while (*p == ppDefine::MACRO_PLACEHOLDER)
                    p++;
                if (!*p || *p == '"')
                    continue;
                int i;
                if (v == l_msilstr)
                    i = *p++;
                else
                    i = getsch(v == l_Ustr || v == l_u8str ? 8 : v == l_wstr || v == l_ustr ? 4 : 2, &p);
                if (i == INT_MIN)
                {
                    if (!errored)
                    {
                        errored++;
                        error(ERR_INVALID_STRING_CONSTANT);
                    }
                }
                if (len == 1)
                    error(ERR_STRING_CONSTANT_TOO_LONG);
                else
                {
                    if (v == l_u8str && i > 0x7f)
                    {
                        if (i > 0x10ffff)
                            error(ERR_INVALID_CHAR_CONSTANT);
                        if (i < (1 << (6 + 5)))
                        {
                            *dest++ = 0xC0 + (i >> 6);
                            *dest++ = 0x80 + (i & 0x3f);
                            len -= 2;
                            count += 2;
                        }
                        else if (i < (1 << (6 + 6 + 4)))
                        {
                            *dest++ = 0xe0 + (i >> 12);
                            *dest++ = 0x80 + ((i >> 6) & 0x3f);
                            *dest++ = 0x80 + (i & 0x3f);
                            len -= 3;
                            count += 3;
                        }
                        else
                        {
                            // fixme (i>>18)? - I think this is correct because of the way UTF-8 conversions work
                            *dest++ = 0xf0 + ((i >> 18 & 0x7));
                            *dest++ = 0x80 + ((i >> 12) & 0x3f);
                            *dest++ = 0x80 + ((i >> 6) & 0x3f);
                            *dest++ = 0x80 + (i & 0x3f);
                            len -= 4;
                            count += 4;
                        }
                    }
                    else
                    {
                        if (v == l_ustr && (i & 0xffff0000))
                            error(ERR_INVALID_CHAR_CONSTANT);
                        if (v == l_Ustr && i > 0x10ffff)
                            error(ERR_INVALID_CHAR_CONSTANT);
                        *dest++ = i;
                        len--;
                        count++;
                    }
                }
            }
            *dest = 0;
            if (*p != '"')
                error(ERR_UNTERM_STRING_CONSTANT);
            else
                do
                    p++;
                while (*p == ppDefine::MACRO_PLACEHOLDER);
            found = true;
            while (*p == ppDefine::MACRO_PLACEHOLDER)
                p++;
            *source = p;
        }
    }
    *tp = v;
    if (found)
    {
        Optimizer::SLCHAR* rv;
        int i;
        rv = Allocate<Optimizer::SLCHAR>();
        rv->str = Allocate<LCHAR>(count);
        for (i = 0; i < count; i++)
            rv->str[i] = data[i];
        rv->count = count;
        return rv;
    }
    return nullptr;
}
static int radix36(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'z')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'Z')
        return c - 'A' + 10;
    return INT_MAX;
}
static long long getbase(int b, char** ptr)
{
    long long i;
    char* s = *ptr;
    int j;
    int errd = 0;
    i = 0;
    while ((j = radix36(**ptr)) < b)
    {
        (*ptr)++;
        if (i > (llminus1 - j) / b)
            if (!errd)
            {
                error(ERR_CONSTTOOLARGE);
                errd++;
            }
        i = i * b + j;
    }
    return i;
}

static void getfloatingbase(int b, FPF* rval, char** ptr)
{
    int j;
    FPF temp, temp1;
    rval->SetZero(0);
    while ((j = radix36(**ptr)) < b)
    {
        (*ptr)++;
        temp = (unsigned long long)j;
        if (b == 10)
            rval->MultiplyPowTen(1);
        else
            rval->SetExp(rval->GetExp() + 4 * 1);
        temp1 = *rval + temp;
        *rval = temp1;
    }
}
/*
 *      getfrac - get fraction part of a floating number.
 */
static int getfrac(int radix, char** ptr, FPF* rval)
{
    unsigned long long i = 0;
    int j, k = 0;
    FPF temp, temp1;
    int digits = 0;
    while ((j = radix36(**ptr)) < radix)
    {
        i = radix * i + j;
        if (++k == sizeof(i) * 16 / CHAR_BIT)  // number of digits that can fit in an int
        {
            temp = (unsigned long long)i;
            if (radix == 10)
                rval->MultiplyPowTen(k);
            else
                rval->SetExp(rval->GetExp() + 4 * k);
            temp1 = *rval + temp;
            *rval = temp1;
            digits += k;
            k = 0;
            i = 0;
        }
        (*ptr)++;
    }
    temp = (unsigned long long)i;
    if (radix == 10)
        rval->MultiplyPowTen(k);
    else
        rval->SetExp(rval->GetExp() + 4 * k);
    temp1 = *rval + temp;
    *rval = temp1;
    digits += k;
    return radix == 10 ? -digits : -digits * 4;
}

/*
 *      getexp - get exponent part of floating number.
 */
static int getexp(char** ptr)
{
    bool neg = false;
    int ival;
    if (**ptr == '-')
    {
        neg = true;
        (*ptr)++;
    }
    else
    {
        if (**ptr == '+')
            (*ptr)++;
    }
    ival = getbase(10, ptr);
    if (neg)
        ival = -ival;
    return ival;
}

/*
 *      getnum - get a number from input.
 *
 *      getnum handles all of the numeric input. it accepts
 *      decimal, octal, hexidecimal, and floating point numbers.
 */
e_lexType getNumber(const unsigned char** ptr, const unsigned char** end, unsigned char* suffix, FPF* rval, long long* ival)
{
    char buf[200], *p = buf;
    int radix = 10;
    int floatradix = 0;
    int frac = 0;
    bool hasdot = false;
    enum e_lexType lastst;
    if (!isdigit((unsigned char)**ptr) && **ptr != '.')
        return (e_lexType)INT_MIN;
    if (**ptr == '.' && !isdigit((unsigned char)*(*ptr + 1)))
        return (e_lexType)INT_MIN;
    if (**ptr == '0')
    {
        (*ptr)++;
        if (**ptr == 'x' || **ptr == 'X')
        {
            (*ptr)++;
            radix = 16;
        }
        else if ((Optimizer::cparams.prm_cplusplus || !Optimizer::cparams.prm_ansi) && (**ptr == 'b' || **ptr == 'B'))
        {
            (*ptr)++;
            radix = 2;
        }
        else
        {
            radix = 8;
        }
    }
    else if (Optimizer::cparams.prm_assemble && **ptr == '$')
    {
        radix = 16;
        (*ptr)++;
    }
    while (((Optimizer::cparams.prm_cplusplus || Optimizer::cparams.prm_c2x) && **ptr == '\'') || radix36(**ptr) < radix ||
           (Optimizer::cparams.prm_assemble && radix36(**ptr) < 16))
    {
        if (**ptr != '\'')
            *p++ = **ptr;
        (*ptr)++;
    }
    if (**ptr == '.')
    {
        hasdot = true;
        if (radix == 8)
            radix = 10;
        *p++ = **ptr;
        (*ptr)++;
        while (radix36(**ptr) < radix)
        {
            *p++ = **ptr;
            (*ptr)++;
        }
    }
    if ((**ptr == 'e' || **ptr == 'E') && radix != 16)
        radix = floatradix = 10;
    else if ((**ptr == 'p' || **ptr == 'P') && radix == 16)
    {
        floatradix = 2;
    }
    else if (radix == 16 && hasdot)
    {
        error(ERR_INVFPCONST);
    }

    if (floatradix)
    {
        *p++ = **ptr;
        (*ptr)++;
        if (**ptr == '-' || **ptr == '+')
        {
            *p++ = **ptr;
            (*ptr)++;
        }
        while (radix36(**ptr) < 10)
        {
            *p++ = **ptr;
            (*ptr)++;
        }
    }

    *p = 0;
    if (!floatradix && radix != 16 && Optimizer::cparams.prm_assemble)
    {
        char* q = buf;
        while (*q && radix36(*q) < 10)
            q++;
        if (*q)
        {
            radix = 16;
            while (*q && radix36(*q) < 16)
                q++;
            if (*q)
            {
                return (e_lexType)INT_MIN;
            }
            if (**ptr != 'H' && **ptr != 'h')
            {
                return (e_lexType)INT_MIN;
            }
            (*ptr)++;
        }
    }
    p = buf;
    if (floatradix || hasdot)
    {
        getfloatingbase(radix, rval, &p);
    }
    else if (radix36(*p) < radix)
    {
        *ival = getbase(radix, &p);
    }
    else
    {
        *ival = 0;
    }
    if (*p == '.')
    {
        p++;
        frac = getfrac(radix, &p, rval);  // rval needs to be adjusted down by ival to make it fractional
        *ival = 0;
    }
    if (*p == 'e' || *p == 'E' || *p == 'p' || *p == 'P')
    {
        p++;
        *ival = getexp(&p);  // total exponent takes into account that rval is an integer that needs to be divided down
    }
    *end = *ptr;
    *suffix = 0;
    if (isstartchar(**ptr))
    {
        unsigned char* ufd = suffix;
        while (issymchar(**ptr))
            *ufd++ = *(*ptr)++;
        *ufd = 0;
    }
    if (!floatradix && !hasdot)
    {
        lastst = l_i;
        if (Utils::iequal((char*)suffix, "L"))
        {
            lastst = l_l;
            suffix[0] = 0;
        }
        else if (Utils::iequal((char*)suffix, "U"))
        {
            lastst = l_ui;
            suffix[0] = 0;
        }
        else if (Utils::iequal((char*)suffix, "UL") || Utils::iequal((char*)suffix, "LU"))
        {
            lastst = l_ul;
            suffix[0] = 0;
        }
        else if (((Optimizer::cparams.prm_c99 || Optimizer::cparams.prm_cplusplus) && Utils::iequal((char*)suffix, "LL")) ||
                 (!Optimizer::cparams.prm_ansi && Utils::iequal((char*)suffix, "i64")))
        {
            lastst = l_ll;
            suffix[0] = 0;
        }
        else if (((Optimizer::cparams.prm_c99 || Optimizer::cparams.prm_cplusplus) &&
                  (Utils::iequal((char*)suffix, "ULL") || Utils::iequal((char*)suffix, "LLU"))) ||
                 (!Optimizer::cparams.prm_ansi && Utils::iequal((char*)suffix, "ui64")))
        {
            lastst = l_ull;
            suffix[0] = 0;
        }
        else if (suffix[0])
        {
            if (!Optimizer::cparams.prm_cplusplus)
            {
                suffix[0] = 0;
                error(ERR_INVCONST);
            }
            else
            {
                lastst = l_ull;
            }
        }
        if (lastst == l_i) /* no qualifiers */
        {
            if (*ival > INT_MAX)
            {
                lastst = l_ui;
                if (radix == 10 || (unsigned long long)*ival > UINT_MAX)
                {
                    lastst = l_l;
                    if (*ival > LONG_MAX)
                    {
                        lastst = l_ul;
                        if (radix == 10 || (unsigned long long)*ival > ULONG_MAX)
                        {
                            if (radix == 10 || *ival > ULLONG_MAX)
                            {
                                lastst = l_ll;
                            }
                            else
                                lastst = l_ull;
                        }
                    }
                }
            }
        }
    }
    else
    {
        /* floating point too large goes to infinity... */
        *ival += frac;
        if (floatradix == 2)
        {
            rval->SetExp(rval->GetExp() + *ival);
        }
        else
        {
            rval->MultiplyPowTen(*ival);
        }
        if (Utils::iequal((char*)suffix, "F"))
        {
            float f;
            lastst = l_f;
            Optimizer::CastToFloat(ISZ_FLOAT, rval);
            suffix[0] = 0;
        }
        else if (Utils::iequal((char*)suffix, "L"))
        {
            lastst = l_ld;
            Optimizer::CastToFloat(ISZ_LDOUBLE, rval);
            suffix[0] = 0;
        }
        else if (suffix[0])
        {
            if (!Optimizer::cparams.prm_cplusplus)
            {
                suffix[0] = 0;
                error(ERR_INVCONST);
            }
            lastst = l_ld;
            Optimizer::CastToFloat(ISZ_LDOUBLE, rval);
        }
        else
        {
            double d;
            lastst = l_d;
            Optimizer::CastToFloat(ISZ_DOUBLE, rval);
        }
    }
    return lastst;
}
int getId(const unsigned char** ptr, unsigned char* dest)
{
    if (!isstartchar(**ptr))
        return INT_MIN;
    while (issymchar(**ptr) || **ptr == '\\')
    {
        if (**ptr == '\\')
        {
            unsigned long long n = 0;
            int i;
            if ((*ptr)[1] == 'u')
            {
                for (i = 0; i < 4; i++)
                {
                    int c = (*ptr)[i + 2];
                    if (!isxdigit(c))
                        break;
                    else
                    {
                        c -= '0';
                        if (c >= 10)
                            c -= 7;
                        n = n * 16 + c;
                    }
                }
                if (i < 4)
                    break;
            }
            else if ((*ptr)[1] == 'U')
            {
                for (i = 0; i < 8; i++)
                {
                    int c = (*ptr)[i + 2];
                    if (!isxdigit(c))
                        break;
                    else
                    {
                        c -= '0';
                        if (c >= 10)
                            c -= 7;
                        n = n * 16 + c;
                    }
                }
                if (i < 8)
                    break;
            }
            else
                break;
            if (n <= 0x20 || (n >= 0x7f && n <= 0x9f) || (n >= 0xd800 && n <= 0xdfff))
                pperror(ERR_INVCONST, 0);
        }
        *dest++ = *(*ptr)++;
    }
    *dest = 0;
    return 0;
}
LEXLIST* SkipToNextLine(void)
{

    if (!context->next)
    {
        SkipToEol();
        context->cur = nullptr;
    }
    return getsym();
}
LEXLIST* getGTSym(LEXLIST* in)
{
    static LEXLIST lex;
    static LEXEME data;
    const unsigned char pgreater[2] = {'>', 0}, *ppgreater = pgreater;
    KEYWORD* kw;
    kw = searchkw(&ppgreater);
    lex = *in;
    lex.data = &data;
    *lex.data = *in->data;
    lex.data->type = l_kw;
    lex.data->kw = kw;
    return &lex;
}
void SkipToEol() { linePointer = (const unsigned char*)currentLine.c_str() + currentLine.size(); }
bool AtEol()
{
    const unsigned char* p = linePointer;
    while (isspace(*p))
        p++;
    return *p == 0;
}
static void ReplaceStringInString(std::string& string, const std::string& val, const std::string& replace)
{
    size_t n = string.find(val);
    while (n != std::string::npos)
    {
        string.replace(n, val.size(), replace);
        n = string.find(val);
    }
}
void CompilePragma(const unsigned char** linePointer)
{
    int err;
    while (isspace(*(*linePointer)))
        (*linePointer)++;
    if (**linePointer == '(')
    {
        (*linePointer)++;
        while (isspace(*(*linePointer)))
            (*linePointer)++;
        err = -1;
        if (**linePointer == '"')
        {
            (*linePointer)++;
            const char* p = (const char*)*linePointer;
            while (**linePointer)
                if (**linePointer == '\\' && (*(*linePointer + 1) == '\\' || *(*linePointer + 1) == '"'))
                    (*linePointer) += 2;
                else if (**linePointer != '"')
                    (*linePointer)++;
                else
                    break;

            std::string toCompile(p, *linePointer - (const unsigned char*)p);
            ReplaceStringInString(toCompile, "\\\"", "\"");
            ReplaceStringInString(toCompile, "\\\\", "\\");
            preProcessor->CompilePragma(toCompile);
            if (**linePointer)
                (*linePointer)++;
        }
        while (isspace(*(*linePointer)))
            (*linePointer)++;
        if (**linePointer == ')')
        {
            (*linePointer)++;
            return;
        }
        err = closepa;
    }
    else
    {
        err = openpa;
    }
    if (err > 0)
        needkw(nullptr, (e_kw)err);
    else
        error(ERR_NEEDSTRING);
}
void DumpAnnotatedLine(FILE* fil, const std::string& line, const std::deque<std::pair<int, int>> positions)
{
    int pos = 0;
    // print the line
    for (int i = 0; i < line.size(); i++)
    {
        if (line[i] != ppDefine::MACRO_PLACEHOLDER)
            fputc(line[i], fil);
    }
    fputc('\n', fil);
    // print the annotations
    int lastStart = -1;
    int lastEnd = -1;
    for (auto&& position : positions)
    {
        int start = position.first;
        int end = position.second;
        if (start == lastStart && end == lastEnd)
            continue;
        lastStart = start;
        lastEnd = end;
        while (pos < start)
        {
            if (line[pos++] != ppDefine::MACRO_PLACEHOLDER)
            {
                fputc(' ', fil);
            }
        }
        int count = 0;
        for (int i = start; i < end; i++)
        {
            if (line[i] != ppDefine::MACRO_PLACEHOLDER)
                count++;
        }
        pos += end - start;
        if (count > 0)
        {
            fputc('^', fil);
            if (count != 1)
            {
                for (int i = 0; i < count - 2; i++)
                    fputc('-', fil);
                fputc('^', fil);
            }
        }
    }
    fputc('\n', fil);
}
static void DumpPreprocessedLine()
{
    if (cppFile)
    {
        const unsigned char* p = linePointer;
        /* this isn't quite kosher, because, for example
         * #define FFLUSH -2
         * int a = -FFLUSH;
         * won't compile correctly from a file that has been generated from here
         */
        fprintf(cppFile, "#line %d \"%s\"\n", preProcessor->GetErrLineNo(), preProcessor->GetErrFile().c_str());
        while (*p)
        {
            if (*p != ppDefine::MACRO_PLACEHOLDER)
                fputc(*p++, cppFile);
            else
                p++;
        }
        fputc('\n', cppFile);
    }
}
LEXLIST* getsym(void)
{
    static std::deque<std::pair<int, int>> annotations;
    static LEXLIST* last;
    static const char* origLine = "";
    LEXLIST* lex;
    KEYWORD* kw;
    enum e_lexType tp;
    bool contin;
    FPF rval;
    long long ival;
    static unsigned char buf[16384];
    static int pos = 0;
    int cval;

    static int trailer;
    static std::deque<ppDefine::TokenPos>::const_iterator tokenIterator;
    Optimizer::SLCHAR* strptr;

    if (context->cur)
    {
        LEXLIST* rv;
        rv = context->cur;
        if (context->last == rv->prev)
            TemplateRegisterDeferred(context->last);
        context->last = rv;
        context->cur = context->cur->next;
        if (rv->data->linedata && rv->data->linedata != &nullLineData)
        {
            if (!lines)
                lines = lineDataListFactory.CreateList();
            while (lines->size() > 1)
                lines->pop_back();
            if (lines->size() == 1)
            {
                lines->front() = rv->data->linedata;
            }
            else
            {
                lines->push_back(rv->data->linedata);
            }
        }
        currentLex = rv;
        return rv;
    }
    else if (context->next)
    {
        return nullptr;
    }
    lex = Allocate<LEXLIST>();
    lex->data = Allocate<LEXEME>();
    lex->data->linedata = nullptr;
    lex->prev = context->last;
    context->last = lex;
    context->last->data->linedata = &nullLineData;

    lex->next = nullptr;
    if (lex->prev)
        lex->prev->next = lex;
    if (++nextFree >= MAX_LOOKBACK)
        nextFree = 0;
    lex->data->registered = false;
    if (!parsingPreprocessorConstant)
        TemplateRegisterDeferred(last);
    last = nullptr;
    bool fetched = false;
    do
    {
        contin = false;
        do
        {
            if (*linePointer == 0)
            {
#ifdef TESTANNOTATE
                printf("%s\n", currentLine.c_str());
                DumpAnnotatedLine(stdout, std::string(origLine), annotations);
                annotations.clear();
                origLine = "";
#endif
                if (parseStack.size() || !preProcessor->GetLine(currentLine))
                {
                    if (lex->prev)
                        lex->prev->next = nullptr;
                    return nullptr;
                }
                linePointer = (const unsigned char*)currentLine.c_str();
                if (cppFile)
                    DumpPreprocessedLine();
                InsertLineData(preProcessor->GetRealLineNo(), preProcessor->GetFileIndex(), preProcessor->GetRealFile().c_str(),
                               (char*)preProcessor->GetOrigLine().c_str());
                fetched = true;
                valid = true;
            }
            while (isspace(*linePointer) || *linePointer == ppDefine::MACRO_PLACEHOLDER)
                linePointer++;
            if (*linePointer != 0)
            {
                if (lines && lines->size())
                    origLine = lines->front()->line;
                if (fetched)
                {
                    trailer = 0;
                    tokenIterator = preProcessor->TokenPositions().begin();
                }
            }
        } while (*linePointer == 0);
        charIndex = lex->data->charindex = linePointer - (const unsigned char*)currentLine.c_str();
        eofLine = lex->data->errline = preProcessor->GetErrLineNo();
        eofFile = lex->data->errfile = preProcessor->GetErrFile().c_str();
        int fileIndex = preProcessor->GetFileIndex();
        if (fileIndex != lastBrowseIndex)
        {
            browse_startfile(preProcessor->GetRealFile().c_str(), fileIndex);
            lastBrowseIndex = fileIndex;
        }

        int start = linePointer - (const unsigned char*)currentLine.c_str();
        if ((cval = getChar(&linePointer, &tp)) != INT_MIN)
        {
            if (tp == l_achr && !Optimizer::cparams.prm_charisunsigned && !(cval & 0xffffff00))
                cval = (e_lexType)(char)cval;
            if (tp == l_uchr && (cval & 0xffff0000))
                error(ERR_INVALID_CHAR_CONSTANT);
            lex->data->value.i = cval;
            if (!Optimizer::cparams.prm_cplusplus)
                lex->data->type = l_i;
            else
                lex->data->type = tp;
            lex->data->suffix = nullptr;
            if (isstartchar(*linePointer))
            {
                char suffix[256], *p = suffix;
                while (issymchar(*linePointer))
                    *p++ = *linePointer++;
                *p = 0;
                if (!Optimizer::cparams.prm_cplusplus)
                    error(ERR_INVCONST);
                else
                    lex->data->suffix = litlate(suffix);
            }
        }
        else if ((strptr = getString(&linePointer, &tp)) != nullptr)
        {
            lex->data->value.s.w = (LCHAR*)strptr;
            lex->data->type = tp == l_u8str ? l_astr : tp;
            lex->data->suffix = nullptr;
            if (isstartchar(*linePointer) && !isspace(*(linePointer - 1)))
            {
                char suffix[256], *p = suffix;
                while (issymchar(*linePointer))
                    *p++ = *linePointer++;
                *p = 0;
                if (!Optimizer::cparams.prm_cplusplus)
                    error(ERR_INVCONST);
                else
                    lex->data->suffix = litlate(suffix);
            }
        }
        else if (*linePointer != 0)
        {
            unsigned char suffix[256];
            const unsigned char* start = linePointer;
            const unsigned char* end = linePointer;
            enum e_lexType tp;
            lex->data->suffix = nullptr;
            if ((unsigned)(tp = getNumber(&linePointer, &end, suffix, &rval, &ival)) != (unsigned)INT_MIN)
            {
                if (tp < l_f)
                {
                    lex->data->value.i = ival;
                }
                else
                {
                    lex->data->value.f = Allocate<FPF>();
                    *lex->data->value.f = rval;
                }
                if (suffix[0])
                {
                    lex->data->suffix = litlate((char*)suffix);
                    memcpy(suffix, start, end - start);
                    suffix[end - start] = 0;
                    lex->data->litaslit = litlate((char*)suffix);
                }
                lex->data->type = tp;
            }
            else if ((kw = searchkw(&linePointer)) != nullptr)
            {
                if (kw->key == kw__Pragma)
                {

                    CompilePragma(&linePointer);
                    contin = true;
                }
                else
                {
                    lex->data->type = l_kw;
                    lex->data->kw = kw;
                }
            }
            else if (getId(&linePointer, buf + pos) != INT_MIN)
            {
                lex->data->value.s.a = (char*)buf + pos;
                lex->data->type = l_id;
                pos += strlen((char*)buf + pos) + 1;
                if (pos >= sizeof(buf) - 512)
                    pos = 0;
            }
            else
            {
                contin = true;
                errorint(ERR_UNKNOWN_CHAR, *linePointer++);
            }
        }
        else
        {
            contin = true;
        }
        if (!contin)
        {
            int end = linePointer - (const unsigned char*)currentLine.c_str();
            if (valid && tokenIterator != preProcessor->TokenPositions().end())
            {
                auto p = *tokenIterator;
                int oldend = end;
                if (start <= p.newStart)
                {
                    start -= trailer;
                    end -= trailer;
                }
                else
                {
                    trailer = p.newEnd - p.origEnd;
                    start = p.origStart;
                    end = p.origEnd;
                }
                if (oldend >= p.newEnd)
                    ++tokenIterator;
            }
            else
            {
                start -= trailer;
                end -= trailer;
            }
            lex->data->charindex = start;
            lex->data->charindexend = end;
#ifdef TESTANNOTATE
            //            printf("%d %d\n", start, end);
            annotations.push_back(std::pair<int, int>(start, end));
#endif
        }
    } while (contin);
    if (lines && lines->size())
    {
        lex->data->linedata = lines->front();
    }
    else
    {
        lex->data->linedata = &nullLineData;
    }
    currentLex = lex;
    return last = lex;
}
LEXLIST* prevsym(LEXLIST* lex)
{
    if (lex)
    {
        if (lex->next)
        {
            context->cur = lex->next;
        }
        else
        {
            context->cur = nullptr;
        }
    }
    return lex;
}
LEXLIST* backupsym(void)
{
    if (context->cur)
    {
        context->cur = context->cur->prev;
        if (!context->cur)
            context->cur = context->last;
    }
    else
    {
        context->cur = context->last;
    }
    return context->cur->prev;
}
LEXLIST* SetAlternateLex(LEXLIST* lexList)
{
    if (lexList)
    {
        LEXCONTEXT* newContext = Allocate<LEXCONTEXT>();
        newContext->next = context;
        context = newContext;
        context->cur = lexList->next;
        context->last = lexList;
        TemplateRegisterDeferred(lexList);
        currentLex = lexList;
        return lexList;
    }
    else
    {
        context = context->next;
        currentLex = context->last;
        return nullptr;
    }
}
bool CompareLex(LEXLIST* left, LEXLIST* right)
{
    while (left && right)
    {
        if (left->data->type != right->data->type)
            break;
        switch (left->data->type)
        {
            case l_i:
            case l_ui:
            case l_l:
            case l_ul:
            case l_ll:
            case l_ull:
                if (left->data->value.i != right->data->value.i)
                    return false;
                break;
            case l_f:
            case l_d:
            case l_ld:
                if (left->data->value.f != right->data->value.f)
                    return false;
                break;
            case l_I:
                break;
            case l_kw:
                if (left->data->kw != right->data->kw)
                    return false;
                break;
            case l_id:
            case l_astr:
            case l_u8str:
            case l_msilstr:
                if (strcmp(left->data->value.s.a, right->data->value.s.a))
                    return false;
                break;
            case l_wstr:
            case l_ustr:
            case l_Ustr:
                int i;
                for (i = 0; left->data->value.s.w[i] && right->data->value.s.w[i]; i++)
                    if (left->data->value.s.w[i] != right->data->value.s.w[i])
                        break;
                if (left->data->value.s.w[i] || right->data->value.s.w[i])
                    return false;
                break;

            case l_achr:
            case l_wchr:
            case l_uchr:
            case l_Uchr:
                if (left->data->value.i != right->data->value.i)
                    return false;
            case l_qualifiedname:
            default:
                return false;
        }
        left = left->next;
        right = right->next;
    }
    return !left && !right;
}
void SetAlternateParse(bool set, const std::string& val)
{
    valid = false;
    if (set)
    {
        int n = (int)(linePointer - (unsigned char*)currentLine.c_str());
        parseStack.push(std::move(ParseHold{std::move(currentLine), n}));
        currentLine = val;
        linePointer = (const unsigned char*)currentLine.c_str();
    }
    else if (parseStack.size())
    {
        currentLine = std::move(parseStack.top().currentLine);
        linePointer = (const unsigned char*)currentLine.c_str() + parseStack.top().charIndex;
        parseStack.pop();
    }
}
long long ParseExpression(std::string& line)
{
    LEXCONTEXT* oldContext = context;
    LEXCONTEXT* newContext = Allocate<LEXCONTEXT>();
    context = newContext;
    TYPE* tp = nullptr;
    EXPRESSION* exp = nullptr;
    SetAlternateParse(true, line);
    LEXLIST* lex = getsym();
    parsingPreprocessorConstant = true;
    dontRegisterTemplate++;
    lex = expression_no_comma(lex, nullptr, nullptr, &tp, &exp, nullptr, 0);
    dontRegisterTemplate--;
    if (tp)
    {
        if (Optimizer::architecture == ARCHITECTURE_MSIL)
        {
            RemoveSizeofOperators(exp);
        }
        optimize_for_constants(&exp);
    }
    parsingPreprocessorConstant = false;
    if (!tp || !exp || !isintconst(exp))
    {
        error(ERR_CONSTANT_VALUE_EXPECTED);
    }
    SetAlternateParse(false, "");
    context = oldContext;
    return exp->v.i;
}
}  // namespace Parser