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
 * keyword module
 */
#include "compiler.h"
#include "config.h"
#include "math.h"
#include <climits>
#include "Utils.h"
#include "PreProcessor.h"
#include <stack>
#include "lex.h"
#include "ccerr.h"
#include "config.h"
#include "declare.h"
#include "stmt.h"
#include "occparse.h"
#include "templatedecl.h"
#include "templateutil.h"
#include "templateinst.h"
#include "templatededuce.h"
#include "constopt.h"
#include "memory.h"
#include "ifloatconv.h"
#include "floatconv.h"
#include "browse.h"
#include "help.h"
#include "expr.h"
#include "namespace.h"
#include "symtab.h"
#include "ListFactory.h"
#include "types.h"
#include "stmt.h"

namespace Parser
{
#ifndef LONGLONG_MAX
#    define LONGLONG_MAX LONG_MAX
#endif

int eofLine;
const char* eofFile;
bool parsingPreprocessorConstant;

LexContext* context;

int charIndex;

LexList* currentLex;
Optimizer::LINEDATA nullLineData = {0, "", "", 0, 0};

static bool valid;
static unsigned long long llminus1;
static int nextFree;
static const unsigned char* linePointer;
static std::string currentLine;
static int lastBrowseIndex;
static unsigned char* bitIntBuffer;
static std::deque<ppDefine::TokenPos>::const_iterator tokenIterator;
struct ParseHold
{
    std::string currentLine;
    LexContext* context;
    int charIndex;
};

static std::stack<ParseHold> parseStack;
LexContext* contextHold;

KeywordData keywords[] = {
    {"!", 1, Keyword::not_, KW_ASSEMBLER, TT_UNARY | TT_OPERATOR},
    {"!=", 2, Keyword::neq_, 0, TT_RELATION | TT_EQUALITY},
    {"#", 1, Keyword::hash_, 0, TT_UNKNOWN},
    {"%", 1, Keyword::mod_, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"%=", 2, Keyword::asmod_, 0, TT_ASSIGN | TT_OPERATOR},
    {"&", 1, Keyword::and_, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"&&", 2, Keyword::land_, 0, TT_BINARY | TT_OPERATOR},
    {"&=", 2, Keyword::asand_, 0, TT_ASSIGN | TT_OPERATOR},
    {"(", 1, Keyword::openpa_, KW_ASSEMBLER, TT_PRIMARY},
    {")", 1, Keyword::closepa_, KW_ASSEMBLER, TT_PRIMARY},
    {"*", 1, Keyword::star_, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"*=", 2, Keyword::astimes_, 0, TT_ASSIGN | TT_OPERATOR},
    {"+", 1, Keyword::plus_, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"++", 2, Keyword::autoinc_, 0, TT_UNARY | TT_INCREMENT},
    {"+=", 2, Keyword::asplus_, 0, TT_ASSIGN | TT_OPERATOR},
    {",", 1, Keyword::comma_, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"-", 1, Keyword::minus_, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"--", 2, Keyword::autodec_, 0, TT_UNARY | TT_INCREMENT},
    {"-=", 2, Keyword::asminus_, 0, TT_ASSIGN | TT_OPERATOR},
    {"->", 2, Keyword::pointsto_, 0, TT_BINARY | TT_POINTER},
    {"->*", 3, Keyword::pointstar_, 0, TT_BINARY | TT_POINTER},
    {".", 1, Keyword::dot_, 0, TT_BINARY | TT_POINTER},
    {".*", 2, Keyword::dotstar_, 0, TT_BINARY | TT_POINTER},
    {"...", 3, Keyword::ellipse_},
    {"/", 1, Keyword::divide_, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"/=", 2, Keyword::asdivide_, 0, TT_ASSIGN | TT_OPERATOR},
    {":", 1, Keyword::colon_, KW_ASSEMBLER},
    {"::", 2, Keyword::classsel_, KW_CPLUSPLUS | KW_MSIL, TT_BINARY | TT_SELECTOR},
    {";", 1, Keyword::semicolon_, KW_ASSEMBLER, TT_CONTROL},
    {"<", 1, Keyword::lt_, 0, TT_RELATION | TT_INEQUALITY},
    {"<<", 2, Keyword::leftshift_, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"<<=", 3, Keyword::asleftshift_, 0, TT_ASSIGN | TT_OPERATOR},
    {"<=", 2, Keyword::leq_, 0, TT_RELATION | TT_INEQUALITY},
    {"=", 1, Keyword::assign_, 0, TT_ASSIGN | TT_OPERATOR | TT_BASE},
    {"==", 2, Keyword::eq_, 0, TT_RELATION | TT_EQUALITY},
    {">", 1, Keyword::gt_, 0, TT_RELATION | TT_INEQUALITY},
    {">=", 2, Keyword::geq_, 0, TT_RELATION | TT_INEQUALITY},
    {">>", 2, Keyword::rightshift_, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {">>=", 3, Keyword::asrightshift_, 0, TT_ASSIGN | TT_OPERATOR},
    {"?", 1, Keyword::hook_, 0, TT_BINARY | TT_OPERATOR},
    {"[", 1, Keyword::openbr_, 0, TT_BINARY | TT_POINTER},
    {"]", 1, Keyword::closebr_, 0, TT_BINARY | TT_POINTER},
    {"^", 1, Keyword::uparrow_, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"^=", 2, Keyword::asxor_, 0, TT_ASSIGN | TT_OPERATOR},
    {"_Alignas", 8, Keyword::alignas_, KW_C1X | KW_C2X, TT_CONTROL},
    {"_Alignof", 8, Keyword::alignof_, KW_C1X | KW_C2X, TT_UNARY | TT_OPERATOR},
    {"_Atomic", 7, Keyword::atomic_, 0, TT_POINTERQUAL | TT_TYPEQUAL | TT_BASETYPE},
    {"_BitInt", 7, Keyword::bitint_, 0, TT_BASETYPE | TT_INT | TT_BASE},
    {"_Bool", 5, Keyword::bool_, KW_C99 | KW_C1X | KW_C2X, TT_BASETYPE | TT_BOOL},
    {"_CR0", 4, Keyword::CR0_, KW_NONANSI | KW_386, TT_VAR},
    {"_CR1", 4, Keyword::CR1_, KW_NONANSI | KW_386, TT_VAR},
    {"_CR2", 4, Keyword::CR2_, KW_NONANSI | KW_386, TT_VAR},
    {"_CR3", 4, Keyword::CR3_, KW_NONANSI | KW_386, TT_VAR},
    {"_CR4", 4, Keyword::CR4_, KW_NONANSI | KW_386, TT_VAR},
    {"_CR5", 4, Keyword::CR5_, KW_NONANSI | KW_386, TT_VAR},
    {"_CR6", 4, Keyword::CR6_, KW_NONANSI | KW_386, TT_VAR},
    {"_CR7", 4, Keyword::CR7_, KW_NONANSI | KW_386, TT_VAR},
    {"_Complex", 8, Keyword::Complex_, 0, TT_BASETYPE | TT_COMPLEX},
    {"_DR0", 4, Keyword::DR0_, KW_NONANSI | KW_386, TT_VAR},
    {"_DR1", 4, Keyword::DR1_, KW_NONANSI | KW_386, TT_VAR},
    {"_DR2", 4, Keyword::DR2_, KW_NONANSI | KW_386, TT_VAR},
    {"_DR3", 4, Keyword::DR3_, KW_NONANSI | KW_386, TT_VAR},
    {"_DR4", 4, Keyword::DR4_, KW_NONANSI | KW_386, TT_VAR},
    {"_DR5", 4, Keyword::DR5_, KW_NONANSI | KW_386, TT_VAR},
    {"_DR6", 4, Keyword::DR6_, KW_NONANSI | KW_386, TT_VAR},
    {"_DR7", 4, Keyword::DR7_, KW_NONANSI | KW_386, TT_VAR},
    {"_EAX", 4, Keyword::D0_, KW_NONANSI | KW_386, TT_VAR},
    {"_EBP", 4, Keyword::D5_, KW_NONANSI | KW_386, TT_VAR},
    {"_EBX", 4, Keyword::D3_, KW_NONANSI | KW_386, TT_VAR},
    {"_ECX", 4, Keyword::D1_, KW_NONANSI | KW_386, TT_VAR},
    {"_EDI", 4, Keyword::D7_, KW_NONANSI | KW_386, TT_VAR},
    {"_EDX", 4, Keyword::D2_, KW_NONANSI | KW_386, TT_VAR},
    {"_ESI", 4, Keyword::D6_, KW_NONANSI | KW_386, TT_VAR},
    {"_ESP", 4, Keyword::D4_, KW_NONANSI | KW_386, TT_VAR},
    {"_FP0", 4, Keyword::F0_, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP1", 4, Keyword::F1_, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP2", 4, Keyword::F2_, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP3", 4, Keyword::F3_, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP4", 4, Keyword::F4_, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP5", 4, Keyword::F5_, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP6", 4, Keyword::F6_, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_FP7", 4, Keyword::F7_, KW_NONANSI | KW_68K | KW_386, TT_VAR},
    {"_Generic", 8, Keyword::generic_, 0, TT_VAR},
    {"_INF", 4, Keyword::INF_, 0, TT_VAR},
    {"_Imaginary", 10, Keyword::Imaginary_, 0, TT_BASETYPE | TT_COMPLEX},
    {"_NAN", 4, Keyword::NAN_, 0, TT_VAR},
    {"_Noreturn", 9, Keyword::noreturn_, KW_C1X | KW_C2X, TT_LINKAGE},
    {"_Pragma", 7, Keyword::Pragma_, KW_C99 | KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"_SS", 3, Keyword::AD_, KW_NONANSI | KW_386, TT_VAR},
    {"_Static_assert", 14, Keyword::static_assert_, 0, 0},
    {"_Thread_local", 13, Keyword::thread_local_, 0, TT_LINKAGE},
    {"_TR0", 4, Keyword::TR0_, KW_NONANSI | KW_386, TT_VAR},
    {"_TR1", 4, Keyword::TR1_, KW_NONANSI | KW_386, TT_VAR},
    {"_TR2", 4, Keyword::TR2_, KW_NONANSI | KW_386, TT_VAR},
    {"_TR3", 4, Keyword::TR3_, KW_NONANSI | KW_386, TT_VAR},
    {"_TR4", 4, Keyword::TR4_, KW_NONANSI | KW_386, TT_VAR},
    {"_TR5", 4, Keyword::TR5_, KW_NONANSI | KW_386, TT_VAR},
    {"_TR6", 4, Keyword::TR6_, KW_NONANSI | KW_386, TT_VAR},
    {"_TR7", 4, Keyword::TR7_, KW_NONANSI | KW_386, TT_VAR},
    {"__CS", 4, Keyword::A8_, KW_NONANSI | KW_386, TT_VAR},
    {"__DS", 4, Keyword::A9_, KW_NONANSI | KW_386, TT_VAR},
    {"__ES", 4, Keyword::AA_, KW_NONANSI | KW_386, TT_VAR},
    {"__FS", 4, Keyword::AB_, KW_NONANSI | KW_386, TT_VAR},
    {"__GS", 4, Keyword::AC_, KW_NONANSI | KW_386, TT_VAR},
    {"__I", 3, Keyword::I_, 0, TT_VAR},
    {"__alignof", 9, Keyword::alignof_, 0, TT_UNARY | TT_OPERATOR},
    {"__alignof__", 11, Keyword::alignof_, 0, TT_UNARY | TT_OPERATOR},
    {"__alloca", 8, Keyword::alloca_, KW_NONANSI | KW_ALL, TT_OPERATOR | TT_UNARY},
    {"__asm", 5, Keyword::asm_, KW_NONANSI | KW_ALL, TT_CONTROL},
    {"__atomic_flag_test_set", 22, Keyword::atomic_flag_test_set_, 0, TT_VAR},
    {"__atomic_flag_clear", 19, Keyword::atomic_flag_clear_, 0, TT_VAR},
    {"__builtin_constexpr", 19, Keyword::builtin_constexpr_, 0, (unsigned long)TT_DECLARE},
    {"__c11_atomic_init", 17, Keyword::c11_atomic_init_, 0, TT_VAR},
    {"__c11_atomic_thread_fence", 25, Keyword::c11_atomic_thread_fence_, 0, TT_VAR},
    {"__c11_atomic_signal_fence", 25, Keyword::c11_atomic_signal_fence_, 0, TT_VAR},
    {"__c11_atomic_is_lock_free", 25, Keyword::c11_atomic_is_lock_free_, 0, TT_VAR},
    {"__c11_atomic_store", 18, Keyword::c11_atomic_store_, 0, TT_VAR},
    {"__c11_atomic_load", 17, Keyword::c11_atomic_load_, 0, TT_VAR},
    {"__c11_atomic_exchange", 21, Keyword::c11_atomic_xchg_, 0, TT_VAR},
    {"__c11_atomic_compare_exchange_strong", 36, Keyword::c11_atomic_cmpxchg_strong_, 0, TT_VAR},
    {"__c11_atomic_compare_exchange_weak", 34, Keyword::c11_atomic_cmpxchg_weak_, 0, TT_VAR},
    {"__c11_atomic_fetch_add", 22, Keyword::c11_atomic_ftchadd_, 0, TT_VAR},
    {"__c11_atomic_fetch_sub", 22, Keyword::c11_atomic_ftchsub_, 0, TT_VAR},
    {"__c11_atomic_fetch_and", 22, Keyword::c11_atomic_ftchand_, 0, TT_VAR},
    {"__c11_atomic_fetch_xor", 22, Keyword::c11_atomic_ftchxor_, 0, TT_VAR},
    {"__c11_atomic_fetch_or", 21, Keyword::c11_atomic_ftchor_, 0, TT_VAR},
    {"__atomic_load_n", 15, Keyword::c11_atomic_load_, 0, TT_VAR},
    {"__atomic_store_n", 16, Keyword::c11_atomic_store_, 0, TT_VAR},
    {"__atomic_exchange_n", 19, Keyword::c11_atomic_xchg_, 0, TT_VAR},
    {"__atomic_compare_exchange_n", 27, Keyword::atomic_cmpxchg_n_, 0, TT_VAR},
    {"__atomic_add_fetch", 18, Keyword::atomic_addftch_, 0, TT_VAR},
    {"__atomic_sub_fetch", 18, Keyword::atomic_subftch_, 0, TT_VAR},
    {"__atomic_and_fetch", 18, Keyword::atomic_andftch_, 0, TT_VAR},
    {"__atomic_xor_fetch", 18, Keyword::atomic_xorftch_, 0, TT_VAR},
    {"__atomic_or_fetch", 17, Keyword::atomic_orftch_, 0, TT_VAR},
    {"__atomic_fetch_add", 22, Keyword::c11_atomic_ftchadd_, 0, TT_VAR},  // c11 keywords were added first so we use em'
    {"__atomic_fetch_sub", 22, Keyword::c11_atomic_ftchsub_, 0, TT_VAR},
    {"__atomic_fetch_and", 22, Keyword::c11_atomic_ftchand_, 0, TT_VAR},
    {"__atomic_fetch_xor", 22, Keyword::c11_atomic_ftchxor_, 0, TT_VAR},
    {"__atomic_fetch_or", 21, Keyword::c11_atomic_ftchor_, 0, TT_VAR},
    {"__attribute__", 13, Keyword::attribute_, 0, TT_VAR},
    {"__catch", 7, Keyword::seh_catch_, KW_MSIL, TT_CONTROL},
    {"__cdecl", 7, Keyword::cdecl_, 0, TT_LINKAGE},
    {"__char8_t", 9, Keyword::char8_t_, KW_C2X, TT_BASETYPE | TT_INT},
    {"__char16_t", 10, Keyword::char16_t_, KW_CPLUSPLUS | KW_C1X | KW_C2X, TT_BASETYPE | TT_INT},
    {"__char32_t", 10, Keyword::char32_t_, KW_CPLUSPLUS | KW_C1X | KW_C2X, TT_BASETYPE | TT_INT},
    {"__cpblk", 7, Keyword::cpblk_, KW_MSIL, TT_OPERATOR | TT_UNARY},
    {"__declspec", 10, Keyword::declspec_, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__entrypoint", 12, Keyword::entrypoint_, KW_MSIL, TT_LINKAGE},
    {"__export", 8, Keyword::dllexport_, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__fastcall", 10, Keyword::fastcall_, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__fastcall__", 12, Keyword::fastcall_, 0, TT_LINKAGE},
    {"seh_fault_", 7, Keyword::seh_fault_, KW_MSIL, TT_CONTROL},
    {"__finally", 9, Keyword::seh_finally_, KW_MSIL, TT_CONTROL},
    {"__func__", 8, Keyword::func_, KW_C99 | KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"__import", 8, Keyword::dllimport_, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__initblk", 9, Keyword::initblk_, KW_MSIL, TT_OPERATOR | TT_UNARY},
    {"__inline", 8, Keyword::inline_, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__int16", 7, Keyword::short_, KW_NONANSI | KW_386 | KW_MSIL, TT_BASETYPE | TT_INT},
    {"__int32", 7, Keyword::int_, KW_NONANSI | KW_386 | KW_MSIL, TT_BASETYPE | TT_INT | TT_BASE},
    {"__int64", 7, Keyword::int64_, KW_NONANSI | KW_386 | KW_MSIL, TT_BASETYPE | TT_INT},
    {"__int64__", 9, Keyword::int64_, 0, TT_BASETYPE | TT_INT},
    {"__int8", 6, Keyword::char_, KW_NONANSI | KW_386, TT_BASETYPE | TT_INT},
    {"__kill_dependency", 17, Keyword::atomic_kill_dependency_, 0, TT_VAR},
    {"__msil_rtl", 10, Keyword::msil_rtl_, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__object", 8, Keyword::object_, KW_MSIL, TT_BASETYPE},
    {"__offsetof", 10, Keyword::offsetof_, 0, TT_VAR},
    {"__pascal", 8, Keyword::pascal_, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__property", 10, Keyword::property_, KW_MSIL, TT_LINKAGE},
    {"__restrict", 10, Keyword::restrict_, KW_C99 | KW_CPLUSPLUS, TT_POINTERQUAL | TT_TYPEQUAL},
    {"__rtllinkage", 12, Keyword::rtllinkage_, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__stdcall", 9, Keyword::stdcall_, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__string", 8, Keyword::string_, KW_MSIL, TT_BASETYPE},
    {"__thread", 8, Keyword::thread_local_, 0, TT_LINKAGE},
    {"__try", 5, Keyword::seh_try_, KW_MSIL, TT_CONTROL},
    {"__typeid", 8, Keyword::typeid_, KW_NONANSI | KW_ALL, TT_VAR},
    {"__underlying_type", 17, Keyword::underlying_type_, KW_CPLUSPLUS, TT_BASETYPE},
    {"__unmanaged", 11, Keyword::unmanaged_, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"__uuid", 6, Keyword::uuid_, 0, TT_LINKAGE},
    {"__uuidof", 8, Keyword::uuidof_, 0, TT_VAR},
    {"__va_list__", 11, Keyword::va_list_, KW_NONANSI | KW_ALL, TT_TYPEQUAL | TT_POINTERQUAL},
    {"__va_typeof__", 13, Keyword::va_typeof_, KW_NONANSI | KW_ALL, TT_VAR},
    {"__volatile", 10, Keyword::volatile_, KW_NONANSI | KW_ALL, TT_VAR},
    {"_absolute", 9, Keyword::absolute_, KW_NONANSI | KW_ALL, TT_STORAGE_CLASS},
    {"_asm", 4, Keyword::asm_, KW_NONANSI | KW_ALL, TT_CONTROL},
    {"_cdecl", 6, Keyword::cdecl_, KW_NONANSI, TT_LINKAGE},
    {"_export", 7, Keyword::dllexport_, KW_NONANSI, TT_LINKAGE},
    //	{ "_far", 4,  Keyword::far_, KW_NONANSI, TT_TYPEQUAL | TT_POINTERQUAL },
    {"_fault", 6, Keyword::seh_fault_, KW_NONANSI, TT_LINKAGE},
    {"_genbyte", 8, Keyword::genword_, KW_NONANSI | KW_386, TT_UNARY | TT_OPERATOR},
    {"_genword", 8, Keyword::genword_, KW_NONANSI | KW_68K, TT_UNARY | TT_OPERATOR},
    {"_import", 7, Keyword::dllimport_, KW_NONANSI, TT_LINKAGE},
    {"_indirect", 9, Keyword::indirect_, KW_NONANSI | KW_ALL},
    {"_interrupt", 10, Keyword::interrupt_, KW_NONANSI, TT_LINKAGE},
    {"_intrinsic", 10, Keyword::intrinsic_, KW_NONANSI | KW_ALL, TT_TYPEQUAL},
    {"_loadds", 7, Keyword::loadds_, KW_NONANSI | KW_386, TT_TYPEQUAL},
    //	{ "_near", 5,  Keyword::near_, KW_NONANSI | KW_ALL, TT_TYPEQUAL | TT_POINTERQUAL},
    {"_pascal", 7, Keyword::pascal_, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"_seg", 4, Keyword::seg_, KW_NONANSI | KW_ALL, TT_TYPEQUAL | TT_POINTERQUAL},
    {"_stdcall", 8, Keyword::stdcall_, KW_NONANSI | KW_ALL, TT_LINKAGE},
    {"_trap", 5, Keyword::trap_, KW_NONANSI | KW_ALL, TT_OPERATOR | TT_UNARY},
    {"alignas", 7, Keyword::alignas_, KW_CPLUSPLUS | KW_C2X, TT_CONTROL},
    {"alignof", 7, Keyword::alignof_, KW_CPLUSPLUS | KW_C2X, TT_UNARY | TT_OPERATOR},
    {"and", 3, Keyword::land_, KW_CPLUSPLUS, TT_BINARY | TT_OPERATOR},
    {"and_eq", 6, Keyword::asand_, KW_CPLUSPLUS, TT_ASSIGN | TT_OPERATOR},
    {"asm", 3, Keyword::asm_, KW_NONANSI | KW_ALL, TT_CONTROL},
    {"auto", 4, Keyword::auto_, 0, TT_STORAGE_CLASS},
    {"bitand", 6, Keyword::and_, KW_CPLUSPLUS, TT_BINARY | TT_OPERATOR},
    {"bitor", 5, Keyword::or_, KW_CPLUSPLUS, TT_BINARY | TT_OPERATOR},
    {"bool", 4, Keyword::bool_, KW_CPLUSPLUS | KW_C2X, TT_BASETYPE | TT_BOOL},
    {"break", 5, Keyword::break_, 0, TT_CONTROL},
    {"case", 4, Keyword::case_, 0, TT_CONTROL | TT_SWITCH},
    {"catch", 5, Keyword::catch_, KW_CPLUSPLUS, TT_CONTROL},
    {"cdecl", 5, Keyword::cdecl_, 0, TT_LINKAGE},
    {"char", 4, Keyword::char_, 0, TT_BASETYPE | TT_INT},
    {"char8_t", 7, Keyword::char8_t_, KW_C2X, TT_BASETYPE | TT_INT},
    {"char16_t", 8, Keyword::char16_t_, KW_CPLUSPLUS, TT_BASETYPE | TT_INT},
    {"char32_t", 8, Keyword::char32_t_, KW_CPLUSPLUS, TT_BASETYPE | TT_INT},
    {"class", 5, Keyword::class_, KW_CPLUSPLUS, TT_BASETYPE | TT_STRUCT},
    {"compl", 5, Keyword::complx_, KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"const", 5, Keyword::const_, KW_ASSEMBLER, TT_POINTERQUAL | TT_TYPEQUAL},
    {"const_cast", 10, Keyword::const_cast_, KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"constexpr", 9, Keyword::constexpr_, KW_CPLUSPLUS | KW_C2X, (unsigned long)TT_DECLARE},
    {"continue", 8, Keyword::continue_, 0, TT_CONTROL},
    {"decltype", 8, Keyword::decltype_, KW_CPLUSPLUS, TT_OPERATOR},
    {"default", 7, Keyword::default_, 0, TT_CONTROL},
    {"delete", 6, Keyword::delete_, KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"do", 2, Keyword::do_, 0, TT_CONTROL},
    {"double", 6, Keyword::double_, 0, TT_BASETYPE | TT_FLOAT},
    {"dynamic_cast", 12, Keyword::dynamic_cast_, KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"else", 4, Keyword::else_, 0, TT_CONTROL},
    {"enum", 4, Keyword::enum_, 0, TT_BASETYPE | TT_ENUM},
    {"explicit", 8, Keyword::explicit_, KW_CPLUSPLUS, TT_STORAGE_CLASS},
    {"export", 6, Keyword::export_, KW_CPLUSPLUS, TT_UNKNOWN},
    {"extern", 6, Keyword::extern_, KW_ASSEMBLER, TT_STORAGE_CLASS},
    {"false", 5, Keyword::false_, KW_CPLUSPLUS | KW_C2X, TT_VAR},
    //	{ "far", 3,  Keyword::far_, KW_NONANSI | KW_ALL, TT_POINTERQUAL | TT_TYPEQUAL},
    {"float", 5, Keyword::float_, 0, TT_BASETYPE | TT_FLOAT},
    {"for", 3, Keyword::for_, 0, TT_CONTROL},
    {"friend", 6, Keyword::friend_, KW_CPLUSPLUS, TT_LINKAGE},
    {"goto", 4, Keyword::goto_, 0, TT_CONTROL},
    {"if", 2, Keyword::if_, 0, TT_CONTROL},
    {"inline", 6, Keyword::inline_, KW_C99 | KW_CPLUSPLUS, TT_LINKAGE},
    {"int", 3, Keyword::int_, 0, TT_BASETYPE | TT_INT | TT_BASE},
    {"long", 4, Keyword::long_, 0, TT_BASETYPE | TT_INT},
    {"mutable", 7, Keyword::mutable_, KW_CPLUSPLUS, TT_STORAGE_CLASS},
    {"namespace", 9, Keyword::namespace_, KW_CPLUSPLUS | KW_MSIL, 0},
    {"native", 6, Keyword::native_, KW_MSIL, TT_BASETYPE | TT_INT | TT_BASE},
    //	{ "near", 4,  Keyword::near_, KW_NONANSI | KW_ALL, TT_POINTERQUAL | TT_TYPEQUAL},
    {"new", 3, Keyword::new_, KW_CPLUSPLUS, TT_OPERATOR | TT_UNARY},
    {"noexcept", 8, Keyword::noexcept_, KW_CPLUSPLUS, TT_CONTROL},
    {"not", 3, Keyword::not_, KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"not_eq", 6, Keyword::neq_, KW_CPLUSPLUS, TT_RELATION | TT_EQUALITY},
    {"nullptr", 7, Keyword::nullptr_, KW_CPLUSPLUS | KW_C2X, TT_VAR},
    {"operator", 8, Keyword::operator_, KW_CPLUSPLUS, TT_OPERATOR},
    {"or", 2, Keyword::lor_, KW_CPLUSPLUS, TT_BINARY | TT_OPERATOR},
    {"or_eq", 5, Keyword::asor_, KW_CPLUSPLUS, TT_ASSIGN | TT_OPERATOR},
    {"private", 7, Keyword::private_, KW_CPLUSPLUS, TT_CLASS},
    {"protected", 9, Keyword::protected_, KW_CPLUSPLUS, TT_CLASS},
    {"public", 6, Keyword::public_, KW_CPLUSPLUS | KW_ASSEMBLER, TT_CLASS},
    {"register", 8, Keyword::register_, 0, TT_STORAGE_CLASS},
    {"reinterpret_cast", 16, Keyword::reinterpret_cast_, KW_CPLUSPLUS, TT_OPERATOR | TT_UNARY},
    {"restrict", 8, Keyword::restrict_, KW_C99, TT_POINTERQUAL | TT_TYPEQUAL},
    {"return", 6, Keyword::return_, 0, TT_CONTROL},
    {"short", 5, Keyword::short_, 0, TT_BASETYPE | TT_INT},
    {"signed", 6, Keyword::signed_, 0, TT_BASETYPE | TT_INT},
    {"sizeof", 6, Keyword::sizeof_, 0, TT_UNARY | TT_OPERATOR},
    {"static", 6, Keyword::static_, 0, TT_STORAGE_CLASS},
    {"static_assert", 13, Keyword::static_assert_, KW_CPLUSPLUS | KW_C2X, 0},
    {"static_cast", 11, Keyword::static_cast_, KW_CPLUSPLUS, TT_UNARY | TT_OPERATOR},
    {"struct", 6, Keyword::struct_, 0, TT_BASETYPE | TT_STRUCT},
    {"switch", 6, Keyword::switch_, 0, TT_CONTROL},
    {"template", 8, Keyword::template_, KW_CPLUSPLUS, TT_CONTROL},
    {"this", 4, Keyword::this_, KW_CPLUSPLUS, TT_VAR},
    {"thread_local", 12, Keyword::thread_local_, KW_CPLUSPLUS | KW_C2X, TT_LINKAGE},
    {"throw", 5, Keyword::throw_, KW_CPLUSPLUS, TT_OPERATOR | TT_UNARY},
    {"true", 4, Keyword::true_, KW_CPLUSPLUS | KW_C2X, TT_VAR},
    {"try", 3, Keyword::try_, KW_CPLUSPLUS, TT_CONTROL},
    {"typedef", 7, Keyword::typedef_, 0, TT_BASETYPE | TT_TYPEDEF | TT_STORAGE_CLASS},
    {"typeid", 6, Keyword::typeid_, KW_CPLUSPLUS, TT_UNKNOWN},
    {"typename", 8, Keyword::typename_, KW_CPLUSPLUS, TT_TYPENAME},
    {"typeof", 6, Keyword::typeof_, 0, TT_BASETYPE | TT_OPERATOR},
    {"typeof_unqual", 13, Keyword::typeof_unqual_, KW_C2X, TT_BASETYPE | TT_OPERATOR},
    {"union", 5, Keyword::union_, 0, TT_BASETYPE | TT_STRUCT},
    {"unsigned", 8, Keyword::unsigned_, 0, TT_BASETYPE | TT_INT | TT_BASE},
    {"using", 5, Keyword::using_, KW_CPLUSPLUS | KW_MSIL, TT_CONTROL},
    {"virtual", 7, Keyword::virtual_, KW_CPLUSPLUS, TT_STORAGE_CLASS},
    {"void", 4, Keyword::void_, 0, TT_BASETYPE | TT_VOID},
    {"volatile", 8, Keyword::volatile_, 0, TT_TYPEQUAL | TT_POINTERQUAL},
    {"wchar_t", 7, Keyword::wchar_t_, KW_CPLUSPLUS, TT_BASETYPE | TT_INT},
    {"while", 5, Keyword::while_, 0, TT_CONTROL},
    {"xor", 3, Keyword::uparrow_, KW_CPLUSPLUS, TT_BINARY | TT_OPERATOR},
    {"xor_eq", 6, Keyword::asxor_, KW_CPLUSPLUS, TT_ASSIGN | TT_OPERATOR},
    {"{", 1, Keyword::begin_, 0, TT_CONTROL | TT_BLOCK},
    {"|", 1, Keyword::or_, KW_ASSEMBLER, TT_BINARY | TT_OPERATOR},
    {"|=", 2, Keyword::asor_, 0, TT_ASSIGN | TT_OPERATOR},
    {"||", 2, Keyword::lor_, 0, TT_BINARY | TT_OPERATOR},
    {"}", 1, Keyword::end_, 0, TT_CONTROL | TT_BLOCK},
    {"~", 1, Keyword::complx_, KW_ASSEMBLER, TT_UNARY | TT_OPERATOR},
};

#define TABSIZE (sizeof(keywords) / sizeof(keywords[0]))
SymbolTableFactory<KeywordData> lexFactory;
SymbolTable<KeywordData>* kwSymbols;

static bool kwmatches(KeywordData* kw);
static LexContext* AllocateContext();

void lexini(void)
/*
 * create a keyword table
 */
{
    contextHold = nullptr;
    bool old = Optimizer::cparams.prm_extwarning;
    Optimizer::cparams.prm_extwarning = false;
    int i;
    lexFactory.Reset();
    kwSymbols = lexFactory.CreateSymbolTable();
    for (i = 0; i < TABSIZE; i++)
    {
        if (kwmatches(&keywords[i]))
            kwSymbols->Add(&keywords[i]);
    }
    llminus1 = 0;
    llminus1--;
    context = AllocateContext();
    nextFree = 0;
    currentLine = "";
    linePointer = (const unsigned char*)currentLine.c_str();
    while (parseStack.size())
        parseStack.pop();
    lastBrowseIndex = 0;
    Optimizer::cparams.prm_extwarning = old;
}

static LexContext* AllocateContext()
{
    if (!contextHold)
    {
        return Allocate<LexContext>();
    }
    else
    {
        auto rv = contextHold;
        contextHold = contextHold->next;
        *rv = {};
        return rv;
    }
}
static void FreeContext(LexContext* context)
{
    context->next = contextHold;
    contextHold = context;
}

/*-------------------------------------------------------------------------*/
bool KWTYPE(LexList* lex, unsigned types)
{
    int rv = 0;
    if (ISKW(lex))
    {
        if ((lex)->data->kw->key == Keyword::auto_)
        {
            if (Optimizer::cparams.prm_cplusplus)
            {
                // in C++ auto is a type
                rv = TT_BASETYPE;
            }
            else
            {
                // in C2x it is a storage class if another type is present
                // or a type if one isn't
                // and in earlier versions of C it is a storage_class
                lex = getsym();
                bool s;
                rv = TypeGenerator::StartOfType(lex, &s, false) ? TT_STORAGE_CLASS : TT_BASETYPE;
                lex = backupsym();
                if (rv == TT_BASETYPE)
                    RequiresDialect::Feature(Dialect::c2x, "auto as a type");
            }
        }
        else
        {
            rv = (lex)->data->kw->tokenTypes;
        }
    }
    return rv & types;
}

static bool kwmatches(KeywordData* kw)
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
KeywordData* searchkw(const unsigned char** p)
/*
 * see if the current symbol is a keyword
 */
{
    KeywordData* kw;
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
        kw = search(kwSymbols, (char*)buf);
        if (kw)
        {
            if (len == kw->len)
            {
                int count = 0;
                if (kw->matchFlags & (KW_C99 | KW_C1X | KW_C2X))
                {
                    if (kw->matchFlags & KW_C99)
                    {
                        if (RequiresDialect::Keyword(Dialect::c99, kw->name))
                            return nullptr;
                    }
                    else if (kw->matchFlags & KW_C1X)
                    {
                        if (RequiresDialect::Keyword(Dialect::c11, kw->name))
                            return nullptr;
                    }
                    else if (kw->matchFlags & KW_C2X)
                    {
                        if (RequiresDialect::Keyword(Dialect::c2x, kw->name))
                            return nullptr;
                    }
                    if (kw->matchFlags & KW_CPLUSPLUS)
                    {
                        if (RequiresDialect::Keyword(Dialect::cpp11, kw->name))
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
        KeywordData* found = nullptr;
        int len = 0;
        while (ispunct((unsigned char)*q1))
            *q++ = *q1++, len++;
        if (len)
        {
            buf[len] = 0;
            while (len && (found = search(kwSymbols, (char*)buf)) == nullptr)
            {
                buf[--len] = 0;
            }
            if (found)
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

int getChar(const unsigned char** source, LexType* tp)
{
    LexType v = LexType::l_achr_;
    const unsigned char* p = *source;
    if (*p == 'L')
    {
        v = LexType::l_wchr_;
        p++;
    }
    else if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c11)
    {
        if (*p == 'u')
        {
            if (p[1] == '8')
            {
                v = LexType::l_u8chr_;
                p += 2;
            }
            else
            {
                v = LexType::l_uchr_;
                p++;
            }
        }
        else if (*p == 'U')
        {
            v = LexType::l_Uchr_;
            p++;
        }
    }
    if (*p == '\'')
    {
        int i;
        do
            p++;
        while (*p == ppDefine::MACRO_PLACEHOLDER);
        i = getsch(v == LexType::l_Uchr_                            ? 8
                   : v == LexType::l_wchr_ || v == LexType::l_uchr_ ? 4
                   : v == LexType::l_u8chr_                         ? 1
                                                                    : 2,
                   &p);
        if (i == INT_MIN)
        {
            error(ERR_INVALID_CHAR_CONSTANT);
            i = '0';
        }
        if (*p != '\'')
        {
            if (v == LexType::l_uchr_ || v == LexType::l_Uchr_)
            {
                error(ERR_CHAR1632_CONSTANT_TOO_LONG);
            }
            else
            {
                int j;
                j = getsch(v == LexType::l_Uchr_ ? 8 : v == LexType::l_wchr_ || v == LexType::l_uchr_ ? 4 : 2, &p);
                if (j == INT_MIN)
                {
                    error(ERR_INVALID_CHAR_CONSTANT);
                    j = '0';
                }
                i = (i << 8) + j;
                if (*p != '\'')
                {
                    j = getsch(v == LexType::l_Uchr_ ? 8 : v == LexType::l_wchr_ || v == LexType::l_uchr_ ? 4 : 2, &p);
                    if (j == INT_MIN)
                    {
                        error(ERR_INVALID_CHAR_CONSTANT);
                        j = '0';
                    }
                    i = (i << 8) + j;
                    if (*p != '\'')
                    {
                        j = getsch(v == LexType::l_Uchr_ ? 8 : v == LexType::l_wchr_ || v == LexType::l_uchr_ ? 4 : 2, &p);
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
Optimizer::SLCHAR* getString(const unsigned char** source, LexType* tp)
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
    LexType v = LexType::l_astr_;
    if (*p == 'L')
    {
        v = LexType::l_wstr_;
        do
            p++;
        while (*p == ppDefine::MACRO_PLACEHOLDER);
    }
    else if (*p == '@' && (Optimizer::architecture == ARCHITECTURE_MSIL) && Optimizer::cparams.msilAllowExtensions)
    {
        v = LexType::l_msilstr_;
        do
            p++;
        while (*p == ppDefine::MACRO_PLACEHOLDER);
    }
    else if (Optimizer::cparams.prm_cplusplus || Optimizer::cparams.c_dialect >= Dialect::c11)
    {
        if (*p == 'u')
        {
            v = LexType::l_ustr_;
            do
                p++;
            while (*p == ppDefine::MACRO_PLACEHOLDER);
            if (*p == '8')
            {
                v = LexType::l_u8str_;
                do
                    p++;
                while (*p == ppDefine::MACRO_PLACEHOLDER);
            }
        }
        else if (*p == 'U')
        {
            v = LexType::l_Ustr_;
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
            char preamble[256];
            int pcount = 0, qcount;
            LCHAR* qpos = 0;
            int lineno = preProcessor->GetErrLineNo();
            unsigned char st[2];
            bool err = false;
            while (true)
            {
                if (!*p)
                {
                    if (!preProcessor->GetLine(currentLine))
                    {
                        errorint(ERR_EOF_RAW_STRING, lineno);
                        *source = p;
                        return nullptr;
                    }
                    p = (const unsigned char*)currentLine.c_str();
                    tokenIterator = preProcessor->TokenPositions().begin();
                }
                st[0] = *p;
                do
                    p++;
                while (*p == ppDefine::MACRO_PLACEHOLDER);
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
                    if (!*p)
                    {
                        if (!preProcessor->GetLine(currentLine))
                        {
                            errorint(ERR_EOF_RAW_STRING, lineno);
                            *source = p;
                            return nullptr;
                        }
                        p = (const unsigned char*)currentLine.c_str();
                        tokenIterator = preProcessor->TokenPositions().begin();
                        st[0] = '\n';
                    }
                    else
                    {
                        st[0] = *p;
                        do
                            p++;
                        while (*p == ppDefine::MACRO_PLACEHOLDER);
                    }
                    if (len < 3)
                    {
                        error(ERR_STRING_CONSTANT_TOO_LONG);
                    }
                    else
                    {
#ifdef TARGET_OS_WINDOWS
                        if (st[0] == '\n')
                        {
                            *dest++ = '\r';
                            len--;
                            count++;
                        }
#endif
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
                if (v == LexType::l_msilstr_)
                    i = *p++;
                else
                    i = getsch(v == LexType::l_Ustr_ || v == LexType::l_u8str_  ? 8
                               : v == LexType::l_wstr_ || v == LexType::l_ustr_ ? 4
                                                                                : 2,
                               &p);
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
                    if (v == LexType::l_u8str_ && i > 0x7f)
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
                        if (v == LexType::l_ustr_ && (i & 0xffff0000))
                            error(ERR_INVALID_CHAR_CONSTANT);
                        if (v == LexType::l_Ustr_ && i > 0x10ffff)
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
static long long getbase(int b, const unsigned char** ptr)
{
    long long i;
    const unsigned char* s = *ptr;
    int j;
    int errd = 0;
    i = 0;
    bool hasSep = **ptr == '\'';

    while (**ptr == '\'')
        (*ptr)++;
    while ((j = radix36(**ptr)) < b)
    {
        (*ptr)++;
        hasSep |= **ptr == '\'';
        while (**ptr == '\'')
            (*ptr)++;
        if (i > (llminus1 - j) / b)
            if (!errd)
            {
                error(ERR_CONSTTOOLARGE);
                errd++;
            }
        i = i * b + j;
    }
    if (hasSep)
    {
        RequiresDialect::Feature(Dialect::cpp14, "Digit Separators");
        RequiresDialect::Feature(Dialect::c2x, "Digit Separators");
    }
    return i;
}

static void getfloatingbase(int b, FPF* rval, const unsigned char** ptr)
{
    int j;
    FPF temp, temp1;
    rval->SetZero(0);
    bool hasSep = **ptr == '\'';
    while (**ptr == '\'')
        (*ptr)++;
    while ((j = radix36(**ptr)) < b)
    {
        (*ptr)++;
        hasSep |= **ptr == '\'';
        while (**ptr == '\'')
            (*ptr)++;
        temp = (unsigned long long)j;
        if (b == 10)
            rval->MultiplyPowTen(1);
        else
            rval->SetExp(rval->GetExp() + 4 * 1);
        temp1 = *rval + temp;
        *rval = temp1;
    }
    if (hasSep)
    {
        RequiresDialect::Feature(Dialect::cpp14, "Digit Separators");
        RequiresDialect::Feature(Dialect::c2x, "Digit Separators");
    }
}
/*
 *      getfrac - get fraction part of a floating number.
 */
static int getfrac(int radix, const unsigned char** ptr, FPF* rval)
{
    unsigned long long i = 0;
    int j, k = 0;
    FPF temp, temp1;
    int digits = 0;
    bool hasSep = **ptr == '\'';
    while (**ptr == '\'')
        (*ptr)++;
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
        hasSep |= **ptr == '\'';
        while (**ptr == '\'')
            (*ptr)++;
    }
    if (hasSep)
    {
        RequiresDialect::Feature(Dialect::cpp14, "Digit Separators");
        RequiresDialect::Feature(Dialect::c2x, "Digit Separators");
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
static int getexp(const unsigned char** ptr)
{
    bool neg = false;
    int ival;
    bool hasSep = **ptr == '\'';
    while (**ptr == '\'')
        (*ptr)++;
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
    if (hasSep)
    {
        RequiresDialect::Feature(Dialect::cpp14, "Digit Separators");
        RequiresDialect::Feature(Dialect::c2x, "Digit Separators");
    }
    return ival;
}

LexType getBitInt(const unsigned char* base, const unsigned char** ptr, int radix, long long* ival, unsigned char** bitintvalue)
{
    bool isunsigned = false;
    if (tolower(**ptr) == 'u')
    {
        if (tolower((*ptr)[1]) != 'w' || tolower((*ptr)[2]) != 'b')
            return LexType::none_;
        *ptr += 3;
        isunsigned = true;
    }
    else
    {
        if (tolower((*ptr)[0]) != 'w' || tolower((*ptr)[1]) != 'b')
            return LexType::none_;
        *ptr += 2;
    }
    const int n = Optimizer::chosenAssembler->arch->bitintmax / CHAR_BIT;
    if (!bitIntBuffer)
        bitIntBuffer = new unsigned char[n];
    memset(bitIntBuffer, 0, n);
    bool hasSep = **ptr == '\'';
    while (**ptr == '\'')
        (*ptr)++;
    int c;
    int bits = 0;
    int shift = -1;
    if (radix == 2)
    {
        shift = 1;
    }
    else if (radix == 8)
    {
        shift = 3;
    }
    else if (radix == 16)
    {
        shift = 4;
    }
    while ((c = radix36(*base++)) < radix)
    {
        bool hasSep = **ptr == '\'';
        while (**ptr == '\'')
            (*ptr)++;
        if (shift > 0)
        {
            unsigned char carry = c;
            for (int i = 0; i < bits / CHAR_BIT + 2; i++)
            {
                int d = (bitIntBuffer[i] << shift) + carry;
                bitIntBuffer[i] = d;
                carry = d >> 8;
            }
            bits += shift;
        }
        else
        {
            unsigned char carry = c;
            for (int i = 0; i < bits / CHAR_BIT + 2; i++)
            {
                int d = bitIntBuffer[i] * 10 + carry;
                bitIntBuffer[i] = d;
                carry = d >> 8;
            }
            bits += 4;  // conservative
        }
    }
    int i;
    for (i = n - 1; i >= 0; i--)
    {
        if (bitIntBuffer[i])
            break;
    }
    c = bitIntBuffer[i];
    i = i * 8;
    int j;
    for (j = 7; j >= 0; j--)
    {
        if (c & (1 << j))
        {
            break;
        }
    }
    i += j + 1;
    if (i <= 0)
        i++;
    if (!isunsigned)
        i++;
    *ival = i;  // bit count
    *bitintvalue = make_bitint(i, bitIntBuffer);
    if (hasSep)
    {
        RequiresDialect::Feature(Dialect::cpp14, "Digit Separators");
        RequiresDialect::Feature(Dialect::c2x, "Digit Separators");
    }
    return isunsigned ? LexType::ubitint_ : LexType::bitint_;
}

/*
 *      getnum - get a number from input.
 *
 *      getnum handles all of the numeric input. it accepts
 *      decimal, octal, hexidecimal, and floating point numbers.
 */
LexType getNumber(const unsigned char** ptr, const unsigned char** end, unsigned char* suffix, FPF* rval, long long* ival,
                  unsigned char** bitintvalue)
{
    int radix = 10;
    int floatradix = 0;
    int frac = 0;
    bool hasdot = false;
    LexType lastst;
    if (!isdigit((unsigned char)**ptr) && **ptr != '.')
        return (LexType)INT_MIN;
    if (**ptr == '.' && !isdigit((unsigned char)*(*ptr + 1)))
        return (LexType)INT_MIN;
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
            RequiresDialect::Feature(Dialect::c2x, "Binary Literals");
            RequiresDialect::Feature(Dialect::cpp14, "Binary Literals");
            (*ptr)++;
            radix = 2;
        }
        else if (isdigit(**ptr))
        {
            radix = 8;
        }
        else /* zero */
        {
            (*ptr)--;
        }
    }
    else if (Optimizer::cparams.prm_assemble && **ptr == '$')
    {
        radix = 16;
        (*ptr)++;
    }
    const unsigned char* base = *ptr;
    while (**ptr == '\'' || radix36(**ptr) < radix || (Optimizer::cparams.prm_assemble && radix36(**ptr) < 16))
    {
        (*ptr)++;
    }
    if ((lastst = getBitInt(base, ptr, radix, ival, bitintvalue)) != LexType::none_)
    {
        RequiresDialect::Feature(Dialect::c2x, "_Bitint Literals");
        return lastst;
    }
    if (**ptr == '.')
    {
        hasdot = true;
        if (radix == 8)
            radix = 10;
        (*ptr)++;
        while (**ptr == '\'' || radix36(**ptr) < radix)
        {
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
        (*ptr)++;
        if (**ptr == '-' || **ptr == '+')
        {
            (*ptr)++;
        }
        while (**ptr == '\'' || radix36(**ptr) < 10)
        {
            (*ptr)++;
        }
    }

    if (!floatradix && radix != 16 && Optimizer::cparams.prm_assemble)
    {
        const unsigned char* q = base;
        while (*q && radix36(*q) < 10)
            q++;
        if (*q)
        {
            radix = 16;
            while (*q && radix36(*q) < 16)
                q++;
            if (*q)
            {
                return (LexType)INT_MIN;
            }
            if (**ptr != 'H' && **ptr != 'h')
            {
                return (LexType)INT_MIN;
            }
            (*ptr)++;
        }
    }
    const unsigned char* p = base;
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
        lastst = LexType::i_;
        if (Utils::iequal((char*)suffix, "L"))
        {
            lastst = LexType::l_;
            suffix[0] = 0;
        }
        else if (Utils::iequal((char*)suffix, "U"))
        {
            lastst = LexType::ui_;
            suffix[0] = 0;
        }
        else if (Utils::iequal((char*)suffix, "UL") || Utils::iequal((char*)suffix, "LU"))
        {
            lastst = LexType::ul_;
            suffix[0] = 0;
        }
        else if (((Optimizer::cparams.c_dialect >= Dialect::c99 || Optimizer::cparams.prm_cplusplus) &&
                  Utils::iequal((char*)suffix, "LL")) ||
                 (!Optimizer::cparams.prm_ansi && Utils::iequal((char*)suffix, "i64")))
        {
            lastst = LexType::ll_;
            suffix[0] = 0;
        }
        else if (((Optimizer::cparams.c_dialect >= Dialect::c99 || Optimizer::cparams.prm_cplusplus) &&
                  (Utils::iequal((char*)suffix, "ULL") || Utils::iequal((char*)suffix, "LLU"))) ||
                 (!Optimizer::cparams.prm_ansi && Utils::iequal((char*)suffix, "ui64")))
        {
            lastst = LexType::ull_;
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
                lastst = LexType::ull_;
            }
        }

        if (lastst == LexType::i_) /* no qualifiers */
        {
            if (*ival > INT_MAX || radix != 10)
            {
                lastst = LexType::ui_;
                if ((unsigned long long)*ival > UINT_MAX)
                {
                    lastst = LexType::l_;
                    if (radix != 10 || *ival > LONG_MAX || *ival < LONG_MIN)
                    {
                        lastst = LexType::ul_;
                        if ((unsigned long long)*ival > ULONG_MAX)
                        {
                            lastst = LexType::ll_;
                            if (radix != 10 || *ival > LLONG_MAX || *ival < LLONG_MIN)
                            {
                                lastst = LexType::ull_;
                            }
                        }
                    }
                }
            }
            else if (*ival < INT_MIN)
            {
                lastst = LexType::l_;
                if (*ival < LONG_MIN)
                {
                    lastst = LexType::ll_;
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
            lastst = LexType::l_f_;
            Optimizer::CastToFloat(ISZ_FLOAT, rval);
            suffix[0] = 0;
        }
        else if (Utils::iequal((char*)suffix, "L"))
        {
            lastst = LexType::l_ld_;
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
            lastst = LexType::l_ld_;
            Optimizer::CastToFloat(ISZ_LDOUBLE, rval);
        }
        else
        {
            double d;
            lastst = LexType::l_d_;
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
LexList* SkipToNextLine(void)
{

    if (!context->next)
    {
        SkipToEol();
        context->cur = nullptr;
    }
    return getsym();
}
LexList* getGTSym(LexList* in)
{
    static LexList lex;
    static Lexeme data;
    const unsigned char pgreater[2] = {'>', 0}, *ppgreater = pgreater;
    KeywordData* kw;
    kw = searchkw(&ppgreater);
    lex = *in;
    lex.data = &data;
    *lex.data = *in->data;
    lex.data->type = LexType::l_kw_;
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
    Keyword err;
    while (isspace(*(*linePointer)))
        (*linePointer)++;
    if (**linePointer == '(')
    {
        (*linePointer)++;
        while (isspace(*(*linePointer)))
            (*linePointer)++;
        err = Keyword::none_;
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
        err = Keyword::closepa_;
    }
    else
    {
        err = Keyword::openpa_;
    }
    if (err != Keyword::none_)
        needkw(nullptr, (Keyword)err);
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
void InsertLineData(int lineno, int fileindex, const char* fname, char* line)
{
    if (!lines)
        lines = lineDataListFactory.CreateList();
    auto ld = Allocate<Optimizer::LINEDATA>();
    ld->file = fname;
    ld->line = litlate(line);
    ld->lineno = lineno;
    ld->fileindex = fileindex;
    lines->push_back(ld);
}
void FlushLineData(const char* file, int lineno)
{
    if (lines)
    {
        while (lines->size())
        {
            if (strcmp(file, lines->front()->file) != 0 || lines->front()->lineno < lineno)
                lines->pop_front();
            else
                break;
        }
    }
}
std::list<Statement*>* currentLineData(std::list<FunctionBlock*>& parent, LexList* lex, int offset)
{
    if (!lex || !lines)
        return nullptr;
    std::list<Statement*> rv;
    int lineno;
    const char* file;
    lineno = lex->data->linedata->lineno + offset + 1;
    file = lex->data->errfile;
    while (lines->size() && (strcmp(lines->front()->file, file) != 0 || lineno >= lines->front()->lineno))
    {
        rv.push_back(Statement::MakeStatement(lex, parent, StatementNode::line_));
        rv.back()->lineData = lines->front();
        lines->pop_front();
    }
    if (rv.size())
    {
        auto rva = stmtListFactory.CreateList();
        *rva = rv;
        return rva;
    }
    return nullptr;
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
LexList* getsym(void)
{
    static std::deque<std::pair<int, int>> annotations;
    static LexList* last;
    static const char* origLine = "";
    LexList* lex;
    KeywordData* kw;
    LexType tp;
    bool contin;
    FPF rval;
    long long ival;
    static unsigned char buf[16384];
    static int pos = 0;
    int cval;

    static int trailer;
    Optimizer::SLCHAR* strptr;

    if (context->cur)
    {
        LexList* rv;
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
    lex = Allocate<LexList>();
    lex->data = Allocate<Lexeme>();
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
            if (tp == LexType::l_achr_ && !Optimizer::cparams.prm_charisunsigned && !(cval & 0xffffff00))
                cval = (char)cval;
            if (tp == LexType::l_uchr_ && (cval & 0xffff0000))
                error(ERR_INVALID_CHAR_CONSTANT);
            lex->data->value.i = cval;
            if (!Optimizer::cparams.prm_cplusplus)
                lex->data->type = LexType::i_;
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
            lex->data->type = tp;
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
            unsigned char* bitintValue;
            LexType tp;
            lex->data->suffix = nullptr;
            if ((unsigned)(tp = getNumber(&linePointer, &end, suffix, &rval, &ival, &bitintValue)) != (unsigned)INT_MIN)
            {
                if (tp == LexType::bitint_ || tp == LexType::ubitint_)
                {
                    lex->data->value.b.bits = ival;
                    lex->data->value.b.value = bitintValue;
                }
                else
                {
                    if (tp < LexType::l_f_)
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
                }
                lex->data->type = tp;
            }
            else if ((kw = searchkw(&linePointer)) != nullptr)
            {
                if (kw->key == Keyword::Pragma_)
                {

                    CompilePragma(&linePointer);
                    contin = true;
                }
                else
                {
                    lex->data->type = LexType::l_kw_;
                    lex->data->kw = kw;
                }
            }
            else if (getId(&linePointer, buf + pos) != INT_MIN)
            {
                lex->data->value.s.a = (char*)buf + pos;
                lex->data->type = LexType::l_id_;
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
LexList* prevsym(LexList* lex)
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
LexList* backupsym(void)
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
LexList* SetAlternateLex(LexList* lexList)
{
    if (lexList)
    {
        LexContext* newContext = AllocateContext();
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
        auto c = context;
        context = context->next;
        currentLex = context->last;
        FreeContext(c);
        return nullptr;
    }
}
bool CompareLex(LexList* left, LexList* right)
{
    while (left && right)
    {
        if (left->data->type != right->data->type)
            break;
        switch (left->data->type)
        {
            case LexType::i_:
            case LexType::ui_:
            case LexType::l_:
            case LexType::ul_:
            case LexType::ll_:
            case LexType::ull_:
                if (left->data->value.i != right->data->value.i)
                    return false;
                break;
            case LexType::l_f_:
            case LexType::l_d_:
            case LexType::l_ld_:
                if (left->data->value.f != right->data->value.f)
                    return false;
                break;
            case LexType::l_I_:
                break;
            case LexType::l_kw_:
                if (left->data->kw != right->data->kw)
                    return false;
                break;
            case LexType::l_id_:
            case LexType::l_astr_:
            case LexType::l_u8str_:
            case LexType::l_msilstr_:
                if (strcmp(left->data->value.s.a, right->data->value.s.a))
                    return false;
                break;
            case LexType::l_wstr_:
            case LexType::l_ustr_:
            case LexType::l_Ustr_:
                int i;
                for (i = 0; left->data->value.s.w[i] && right->data->value.s.w[i]; i++)
                    if (left->data->value.s.w[i] != right->data->value.s.w[i])
                        break;
                if (left->data->value.s.w[i] || right->data->value.s.w[i])
                    return false;
                break;

            case LexType::l_achr_:
            case LexType::l_wchr_:
            case LexType::l_uchr_:
            case LexType::l_Uchr_:
                if (left->data->value.i != right->data->value.i)
                    return false;
            case LexType::l_qualifiedName_:
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
        parseStack.push(std::move(ParseHold{std::move(currentLine), context, n}));
        currentLine = val;
        linePointer = (const unsigned char*)currentLine.c_str();
        context = AllocateContext();
    }
    else if (parseStack.size())
    {
        FreeContext(context);
        currentLine = std::move(parseStack.top().currentLine);
        linePointer = (const unsigned char*)currentLine.c_str() + parseStack.top().charIndex;
        context = parseStack.top().context;
        parseStack.pop();
    }
}
long long ParseExpression(std::string& line)
{
    LexContext* oldContext = context;
    LexContext* newContext = AllocateContext();
    context = newContext;
    Type* tp = nullptr;
    EXPRESSION* exp = nullptr;
    SetAlternateParse(true, line);
    LexList* lex = getsym();
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
    FreeContext(newContext);
    return exp->v.i;
}
}  // namespace Parser