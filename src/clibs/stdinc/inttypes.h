/*
    Software License Agreement (BSD License)
    
    Copyright (c) 1997-2008, David Lindauer, (LADSoft).
    All rights reserved.
    
    Redistribution and use of this software in source and binary forms, with or without modification, are
    permitted provided that the following conditions are met:
    
    * Redistributions of source code must retain the above
      copyright notice, this list of conditions and the
      following disclaimer.
    
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the
      following disclaimer in the documentation and/or other
      materials provided with the distribution.
    
    * Neither the name of LADSoft nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior
      written permission of LADSoft.
    
    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
    ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
    TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
    ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef __INTTYPES_H
#define __INTTYPES_H

#ifndef __STDINT_H
#include <stdint.h>
#endif

#if !defined(__cplusplus) || defined(__STDC_FORMAT_MACROS)

#define PRId8               "hhd"
#define PRIi8               "hhi"
#define PRIo8               "hho"
#define PRIu8               "hhU"
#define PRIx8               "hhx"
#define PRIX8               "hhX"

#define PRId16              "hd"
#define PRIi16              "hi"
#define PRIo16              "ho"
#define PRIu16              "hU"
#define PRIx16              "hx"
#define PRIX16              "hX"

#define PRId32              "d"
#define PRIi32              "i"
#define PRIo32              "o"
#define PRIu32              "u"
#define PRIx32              "x"
#define PRIX32              "X"

#define PRId64              "lld"
#define PRIi64              "lli"
#define PRIo64              "llo"
#define PRIu64              "llU"
#define PRIx64              "llx"
#define PRIX64              "llX"


#define PRIdFAST8           "hhd"
#define PRIiFAST8           "hhi"
#define PRIoFAST8           "hho"
#define PRIuFAST8           "hhU"
#define PRIxFAST8           "hhx"
#define PRIXFAST8           "hhX"

#define PRIdFAST16          "hd"
#define PRIiFAST16          "hi"
#define PRIoFAST16          "ho"
#define PRIuFAST16          "hU"
#define PRIxFAST16          "hx"
#define PRIXFAST16          "hX"

#define PRIdFAST32          "d"
#define PRIiFAST32          "i"
#define PRIoFAST32          "o"
#define PRIuFAST32          "u"
#define PRIxFAST32          "x"
#define PRIXFAST32          "X"

#define PRIdFAST64          "lld"
#define PRIiFAST64          "lli"
#define PRIoFAST64          "llo"
#define PRIuFAST64          "llU"
#define PRIxFAST64          "llx"
#define PRIXFAST64          "llX"

#define PRIdLEAST8          "hhd"
#define PRIiLEAST8          "hhi"
#define PRIoLEAST8          "hho"
#define PRIuLEAST8          "hhU"
#define PRIxLEAST8          "hhx"
#define PRIXLEAST8          "hhX"

#define PRIdLEAST16         "hd"
#define PRIiLEAST16         "hi"
#define PRIoLEAST16         "ho"
#define PRIuLEAST16         "hU"
#define PRIxLEAST16         "hx"
#define PRIXLEAST16         "hX"

#define PRIdLEAST32         "d"
#define PRIiLEAST32         "i"
#define PRIoLEAST32         "o"
#define PRIuLEAST32         "u"
#define PRIxLEAST32         "x"
#define PRIXLEAST32         "X"

#define PRIdLEAST64         "lld"
#define PRIiLEAST64         "lli"
#define PRIoLEAST64         "llo"
#define PRIuLEAST64         "llU"
#define PRIxLEAST64         "llx"
#define PRIXLEAST64         "llX"

#define PRIdMAX8            "hhd"
#define PRIiMAX8            "hhi"
#define PRIoMAX8            "hho"
#define PRIuMAX8            "hhU"
#define PRIxMAX8            "hhx"
#define PRIXMAX8            "hhX"

#define PRIdMAX16           "hd"
#define PRIiMAX16           "hi"
#define PRIoMAX16           "ho"
#define PRIuMAX16           "hU"
#define PRIxMAX16           "hx"
#define PRIXMAX16           "hX"

#define PRIdMAX32           "d"
#define PRIiMAX32           "i"
#define PRIoMAX32           "o"
#define PRIuMAX32           "u"
#define PRIxMAX32           "x"
#define PRIXMAX32           "X"

#define PRIdMAX64           "lld"
#define PRIiMAX64           "lli"
#define PRIoMAX64           "llo"
#define PRIuMAX64           "llU"
#define PRIxMAX64           "llx"
#define PRIXMAX64           "llX"

#define PRIdPTR8            "hhd"
#define PRIiPTR8            "hhi"
#define PRIoPTR8            "hho"
#define PRIuPTR8            "hhU"
#define PRIxPTR8            "hhx"
#define PRIXPTR8            "hhX"

#define PRIdPTR16           "hd"
#define PRIiPTR16           "hi"
#define PRIoPTR16           "ho"
#define PRIuPTR16           "hU"
#define PRIxPTR16           "hx"
#define PRIXPTR16           "hX"

#define PRIdPTR32           "d"
#define PRIiPTR32           "i"
#define PRIoPTR32           "o"
#define PRIuPTR32           "u"
#define PRIxPTR32           "x"
#define PRIXPTR32           "X"

#define PRIdPTR64           "lld"
#define PRIiPTR64           "lli"
#define PRIoPTR64           "llo"
#define PRIuPTR64           "llU"
#define PRIxPTR64           "llx"
#define PRIXPTR64           "llX"

#define SCNd8               "hhd"
#define SCNi8               "hhi"
#define SCNo8               "hho"
#define SCNu8               "hhU"
#define SCNx8               "hhx"

#define SCNd16              "hd"
#define SCNi16              "hi"
#define SCNo16              "ho"
#define SCNu16              "hU"
#define SCNx16              "hx"

#define SCNd32              "d"
#define SCNi32              "i"
#define SCNo32              "o"
#define SCNu32              "u"
#define SCNx32              "x"

#define SCNd64              "lld"
#define SCNi64              "lli"
#define SCNo64              "llo"
#define SCNu64              "llU"
#define SCNx64              "llx"

#define SCNdFAST8           "hhd"
#define SCNiFAST8           "hhi"
#define SCNoFAST8           "hho"
#define SCNuFAST8           "hhU"
#define SCNxFAST8           "hhx"

#define SCNdFAST16          "hd"
#define SCNiFAST16          "hi"
#define SCNoFAST16          "ho"
#define SCNuFAST16          "hU"
#define SCNxFAST16          "hx"

#define SCNdFAST32          "d"
#define SCNiFAST32          "i"
#define SCNoFAST32          "o"
#define SCNuFAST32          "u"
#define SCNxFAST32          "x"

#define SCNdFAST64          "lld"
#define SCNiFAST64          "lli"
#define SCNoFAST64          "llo"
#define SCNuFAST64          "llU"
#define SCNxFAST64          "llx"

#define SCNdLEAST8          "hhd"
#define SCNiLEAST8          "hhi"
#define SCNoLEAST8          "hho"
#define SCNuLEAST8          "hhU"
#define SCNxLEAST8          "hhx"

#define SCNdLEAST16         "hd"
#define SCNiLEAST16         "hi"
#define SCNoLEAST16         "ho"
#define SCNuLEAST16         "hU"
#define SCNxLEAST16         "hx"

#define SCNdLEAST32         "d"
#define SCNiLEAST32         "i"
#define SCNoLEAST32         "o"
#define SCNuLEAST32         "u"
#define SCNxLEAST32         "x"

#define SCNdLEAST64         "lld"
#define SCNiLEAST64         "lli"
#define SCNoLEAST64         "llo"
#define SCNuLEAST64         "llU"
#define SCNxLEAST64         "llx"

#define SCNdMAX8            "hhd"
#define SCNiMAX8            "hhi"
#define SCNoMAX8            "hho"
#define SCNuMAX8            "hhU"
#define SCNxMAX8            "hhx"

#define SCNdMAX16           "hd"
#define SCNiMAX16           "hi"
#define SCNoMAX16           "ho"
#define SCNuMAX16           "hU"
#define SCNxMAX16           "hx"

#define SCNdMAX32           "d"
#define SCNiMAX32           "i"
#define SCNoMAX32           "o"
#define SCNuMAX32           "u"
#define SCNxMAX32           "x"

#define SCNdMAX64           "lld"
#define SCNiMAX64           "lli"
#define SCNoMAX64           "llo"
#define SCNuMAX64           "llU"
#define SCNxMAX64           "llx"

#define SCNdPTR8            "hhd"
#define SCNiPTR8            "hhi"
#define SCNoPTR8            "hho"
#define SCNuPTR8            "hhU"
#define SCNxPTR8            "hhx"

#define SCNdPTR16           "hd"
#define SCNiPTR16           "hi"
#define SCNoPTR16           "ho"
#define SCNuPTR16           "hU"
#define SCNxPTR16           "hx"

#define SCNdPTR32           "d"
#define SCNiPTR32           "i"
#define SCNoPTR32           "o"
#define SCNuPTR32           "u"
#define SCNxPTR32           "x"

#define SCNdPTR64           "lld"
#define SCNiPTR64           "lli"
#define SCNoPTR64           "llo"
#define SCNuPTR64           "llU"
#define SCNxPTR64           "llx"

#endif

typedef struct {
        intmax_t     quot;
        intmax_t     rem;
} imaxdiv_t;

#ifdef __cplusplus
namespace __STD_NS__ {
extern "C" {
#endif

intmax_t _RTL_FUNC _IMPORT imaxabs(intmax_t __val);

imaxdiv_t _RTL_FUNC _IMPORT imaxdiv(intmax_t __numer, intmax_t __denom);

intmax_t _RTL_FUNC _IMPORT strtoimax(const char * restrict __nptr, 
                              char ** restrict __endptr, int __base);
uintmax_t _RTL_FUNC _IMPORT strtoumax(const char * restrict __nptr, 
                               char ** restrict __endptr, int __base);

intmax_t _RTL_FUNC _IMPORT wcstoimax(const wchar_t * restrict __nptr,
                   wchar_t ** restrict __endptr, int __base);
uintmax_t _RTL_FUNC _IMPORT wcstoumax(const wchar_t * restrict __nptr,
                    wchar_t ** restrict __endptr, int __base);

#ifdef __cplusplus
}
}
#endif

#endif
#if defined(__cplusplus) && !defined(__USING_CNAME__) && !defined(__INTTYPES_H_USING_LIST)
#define __INTTYPES_H_USING_LIST
    using __STD_NS_QUALIFIER imaxabs ;
    using __STD_NS_QUALIFIER imaxdiv ;
    using __STD_NS_QUALIFIER strtoimax;
    using __STD_NS_QUALIFIER strtoumax;
    using __STD_NS_QUALIFIER wcstoimax;
    using __STD_NS_QUALIFIER wcstoumax;
#endif
