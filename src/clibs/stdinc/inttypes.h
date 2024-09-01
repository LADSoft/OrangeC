/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2024 David Lindauer, (LADSoft)
 *  
 *      This file is part of the Orange C Compiler package.
 *  
 *      The Orange C Compiler package is free software: you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation, either version 3 of the License, or
 *      (at your option) any later version.
 *  
 *      The Orange C Compiler package is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *  
 *      You should have received a copy of the GNU General Public License
 *      along with Orange C.  If not, see <http://www.gnu.org/licenses/>.
 *  
 *      contact information:
 *          email: TouchStone222@runbox.com <David Lindauer>
 *  
 */

#pragma GCC system_header

#ifndef __INTTYPES_H
#define __INTTYPES_H

#    include <stdint.h>

#    define PRId8 "hhd"
#    define PRIi8 "hhi"
#    define PRIo8 "hho"
#    define PRIu8 "hhU"
#    define PRIx8 "hhx"
#    define PRIX8 "hhX"

#    define PRId16 "hd"
#    define PRIi16 "hi"
#    define PRIo16 "ho"
#    define PRIu16 "hU"
#    define PRIx16 "hx"
#    define PRIX16 "hX"

#    define PRId32 "d"
#    define PRIi32 "i"
#    define PRIo32 "o"
#    define PRIu32 "u"
#    define PRIx32 "x"
#    define PRIX32 "X"

#    define PRId64 "lld"
#    define PRIi64 "lli"
#    define PRIo64 "llo"
#    define PRIu64 "llU"
#    define PRIx64 "llx"
#    define PRIX64 "llX"

#    define PRIdFAST8 "hhd"
#    define PRIiFAST8 "hhi"
#    define PRIoFAST8 "hho"
#    define PRIuFAST8 "hhU"
#    define PRIxFAST8 "hhx"
#    define PRIXFAST8 "hhX"

#    define PRIdFAST16 "hd"
#    define PRIiFAST16 "hi"
#    define PRIoFAST16 "ho"
#    define PRIuFAST16 "hU"
#    define PRIxFAST16 "hx"
#    define PRIXFAST16 "hX"

#    define PRIdFAST32 "d"
#    define PRIiFAST32 "i"
#    define PRIoFAST32 "o"
#    define PRIuFAST32 "u"
#    define PRIxFAST32 "x"
#    define PRIXFAST32 "X"

#    define PRIdFAST64 "lld"
#    define PRIiFAST64 "lli"
#    define PRIoFAST64 "llo"
#    define PRIuFAST64 "llU"
#    define PRIxFAST64 "llx"
#    define PRIXFAST64 "llX"

#    define PRIdLEAST8 "hhd"
#    define PRIiLEAST8 "hhi"
#    define PRIoLEAST8 "hho"
#    define PRIuLEAST8 "hhU"
#    define PRIxLEAST8 "hhx"
#    define PRIXLEAST8 "hhX"

#    define PRIdLEAST16 "hd"
#    define PRIiLEAST16 "hi"
#    define PRIoLEAST16 "ho"
#    define PRIuLEAST16 "hU"
#    define PRIxLEAST16 "hx"
#    define PRIXLEAST16 "hX"

#    define PRIdLEAST32 "d"
#    define PRIiLEAST32 "i"
#    define PRIoLEAST32 "o"
#    define PRIuLEAST32 "u"
#    define PRIxLEAST32 "x"
#    define PRIXLEAST32 "X"

#    define PRIdLEAST64 "lld"
#    define PRIiLEAST64 "lli"
#    define PRIoLEAST64 "llo"
#    define PRIuLEAST64 "llU"
#    define PRIxLEAST64 "llx"
#    define PRIXLEAST64 "llX"

#    define PRIdMAX8 "hhd"
#    define PRIiMAX8 "hhi"
#    define PRIoMAX8 "hho"
#    define PRIuMAX8 "hhU"
#    define PRIxMAX8 "hhx"
#    define PRIXMAX8 "hhX"

#    define PRIdMAX16 "hd"
#    define PRIiMAX16 "hi"
#    define PRIoMAX16 "ho"
#    define PRIuMAX16 "hU"
#    define PRIxMAX16 "hx"
#    define PRIXMAX16 "hX"

#    define PRIdMAX32 "d"
#    define PRIiMAX32 "i"
#    define PRIoMAX32 "o"
#    define PRIuMAX32 "u"
#    define PRIxMAX32 "x"
#    define PRIXMAX32 "X"

#    define PRIdMAX64 "lld"
#    define PRIiMAX64 "lli"
#    define PRIoMAX64 "llo"
#    define PRIuMAX64 "llU"
#    define PRIxMAX64 "llx"
#    define PRIXMAX64 "llX"

#    define PRIdPTR8 "hhd"
#    define PRIiPTR8 "hhi"
#    define PRIoPTR8 "hho"
#    define PRIuPTR8 "hhU"
#    define PRIxPTR8 "hhx"
#    define PRIXPTR8 "hhX"

#    define PRIdPTR16 "hd"
#    define PRIiPTR16 "hi"
#    define PRIoPTR16 "ho"
#    define PRIuPTR16 "hU"
#    define PRIxPTR16 "hx"
#    define PRIXPTR16 "hX"

#    define PRIdPTR32 "d"
#    define PRIiPTR32 "i"
#    define PRIoPTR32 "o"
#    define PRIuPTR32 "u"
#    define PRIxPTR32 "x"
#    define PRIXPTR32 "X"

#    define PRIdPTR64 "lld"
#    define PRIiPTR64 "lli"
#    define PRIoPTR64 "llo"
#    define PRIuPTR64 "llU"
#    define PRIxPTR64 "llx"
#    define PRIXPTR64 "llX"

#    ifdef UINTPTR_MAX == INT32_MAX
#        define PRIdPTR PRIdPTR32
#        define PRIiPTR PRIiPTR32
#        define PRIoPTR PRIoPTR32
#        define PRIuPTR PRIuPTR32
#        define PRIxPTR PRIxPTR32
#        define PRIXPTR PRIXPTR32
#        define PRIdMAX PRIdMAX32
#        define PRIiMAX PRIiMAX32
#        define PRIoMAX PRIoMAX32
#        define PRIuMAX PRIuMAX32
#        define PRIxMAX PRIxMAX32
#        define PRIXMAX PRIXMAX32
#    else
#        define PRIdPTR PRIdPTR64
#        define PRIiPTR PRIiPTR64
#        define PRIoPTR PRIoPTR64
#        define PRIuPTR PRIuPTR64
#        define PRIxPTR PRIxPTR64
#        define PRIXPTR PRIXPTR64
#        define PRIdMAX PRIdMAX64
#        define PRIiMAX PRIiMAX64
#        define PRIoMAX PRIoMAX64
#        define PRIuMAX PRIuMAX64
#        define PRIxMAX PRIxMAX64
#        define PRIXMAX PRIXMAX64
#    endif

#    define SCNd8 "hhd"
#    define SCNi8 "hhi"
#    define SCNo8 "hho"
#    define SCNu8 "hhU"
#    define SCNx8 "hhx"

#    define SCNd16 "hd"
#    define SCNi16 "hi"
#    define SCNo16 "ho"
#    define SCNu16 "hU"
#    define SCNx16 "hx"

#    define SCNd32 "d"
#    define SCNi32 "i"
#    define SCNo32 "o"
#    define SCNu32 "u"
#    define SCNx32 "x"

#    define SCNd64 "lld"
#    define SCNi64 "lli"
#    define SCNo64 "llo"
#    define SCNu64 "llU"
#    define SCNx64 "llx"

#    define SCNdFAST8 "hhd"
#    define SCNiFAST8 "hhi"
#    define SCNoFAST8 "hho"
#    define SCNuFAST8 "hhU"
#    define SCNxFAST8 "hhx"

#    define SCNdFAST16 "hd"
#    define SCNiFAST16 "hi"
#    define SCNoFAST16 "ho"
#    define SCNuFAST16 "hU"
#    define SCNxFAST16 "hx"

#    define SCNdFAST32 "d"
#    define SCNiFAST32 "i"
#    define SCNoFAST32 "o"
#    define SCNuFAST32 "u"
#    define SCNxFAST32 "x"

#    define SCNdFAST64 "lld"
#    define SCNiFAST64 "lli"
#    define SCNoFAST64 "llo"
#    define SCNuFAST64 "llU"
#    define SCNxFAST64 "llx"

#    define SCNdLEAST8 "hhd"
#    define SCNiLEAST8 "hhi"
#    define SCNoLEAST8 "hho"
#    define SCNuLEAST8 "hhU"
#    define SCNxLEAST8 "hhx"

#    define SCNdLEAST16 "hd"
#    define SCNiLEAST16 "hi"
#    define SCNoLEAST16 "ho"
#    define SCNuLEAST16 "hU"
#    define SCNxLEAST16 "hx"

#    define SCNdLEAST32 "d"
#    define SCNiLEAST32 "i"
#    define SCNoLEAST32 "o"
#    define SCNuLEAST32 "u"
#    define SCNxLEAST32 "x"

#    define SCNdLEAST64 "lld"
#    define SCNiLEAST64 "lli"
#    define SCNoLEAST64 "llo"
#    define SCNuLEAST64 "llU"
#    define SCNxLEAST64 "llx"

#    define SCNdMAX8 "hhd"
#    define SCNiMAX8 "hhi"
#    define SCNoMAX8 "hho"
#    define SCNuMAX8 "hhU"
#    define SCNxMAX8 "hhx"

#    define SCNdMAX16 "hd"
#    define SCNiMAX16 "hi"
#    define SCNoMAX16 "ho"
#    define SCNuMAX16 "hU"
#    define SCNxMAX16 "hx"

#    define SCNdMAX32 "d"
#    define SCNiMAX32 "i"
#    define SCNoMAX32 "o"
#    define SCNuMAX32 "u"
#    define SCNxMAX32 "x"

#    define SCNdMAX64 "lld"
#    define SCNiMAX64 "lli"
#    define SCNoMAX64 "llo"
#    define SCNuMAX64 "llU"
#    define SCNxMAX64 "llx"

#    define SCNdPTR8 "hhd"
#    define SCNiPTR8 "hhi"
#    define SCNoPTR8 "hho"
#    define SCNuPTR8 "hhU"
#    define SCNxPTR8 "hhx"

#    define SCNdPTR16 "hd"
#    define SCNiPTR16 "hi"
#    define SCNoPTR16 "ho"
#    define SCNuPTR16 "hU"
#    define SCNxPTR16 "hx"

#    define SCNdPTR32 "d"
#    define SCNiPTR32 "i"
#    define SCNoPTR32 "o"
#    define SCNuPTR32 "u"
#    define SCNxPTR32 "x"

#    define SCNdPTR64 "lld"
#    define SCNiPTR64 "lli"
#    define SCNoPTR64 "llo"
#    define SCNuPTR64 "llU"
#    define SCNxPTR64 "llx"

#    ifdef UINTPTR_MAX == INT32_MAX
#        define SCNdPTR SCNdPTR32
#        define SCNiPTR SCNiPTR32
#        define SCNoPTR SCNoPTR32
#        define SCNuPTR SCNuPTR32
#        define SCNxPTR SCNxPTR32
#        define SCNdMAX SCNdMAX32
#        define SCNiMAX SCNiMAX32   
#        define SCNoMAX SCNoMAX32
#        define SCNuMAX SCNuMAX32
#        define SCNxMAX SCNxMAX32
#    else
#        define SCNdPTR SCNdPTR64
#        define SCNiPTR SCNiPTR64
#        define SCNoPTR SCNoPTR64
#        define SCNuPTR SCNuPTR64
#        define SCNxPTR SCNxPTR64
#        define SCNdMAX SCNdMAX64
#        define SCNiMAX SCNiMAX64
#        define SCNoMAX SCNoMAX64
#        define SCNuMAX SCNuMAX64
#        define SCNxMAX SCNxMAX64
#    endif

#ifndef RC_INVOKED
typedef struct
{
    intmax_t quot;
    intmax_t rem;
} imaxdiv_t;

#    ifdef __cplusplus
extern "C"
{
#    endif

    intmax_t _RTL_FUNC _IMPORT imaxabs(intmax_t __val);

    imaxdiv_t _RTL_FUNC _IMPORT imaxdiv(intmax_t __numer, intmax_t __denom);

    intmax_t _RTL_FUNC _IMPORT strtoimax(const char* ZSTR restrict __nptr, char* ZSTR* restrict __endptr, int __base);
    uintmax_t _RTL_FUNC _IMPORT strtoumax(const char* ZSTR restrict __nptr, char* ZSTR* restrict __endptr, int __base);

    intmax_t _RTL_FUNC _IMPORT wcstoimax(const wchar_t* restrict __nptr, wchar_t** restrict __endptr, int __base);
    uintmax_t _RTL_FUNC _IMPORT wcstoumax(const wchar_t* restrict __nptr, wchar_t** restrict __endptr, int __base);

#    ifdef __cplusplus
}
#    endif
#endif

#endif
