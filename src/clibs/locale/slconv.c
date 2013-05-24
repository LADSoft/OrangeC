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
#include <stddef.h>
#include <locale.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <wchar.h>
#include <limits.h>
#include "libp.h"
#include "_locale.h"

struct lconv *  _RTL_FUNC localeconv( void )
{
    struct lconv *lc = &__getRtlData()->lconvbuf;
    char *stringbuf = __getRtlData()->lconvstringbuf;
    char *stringbufptr = stringbuf;

    lc->decimal_point = ".";
    lc->thousands_sep = "";
    lc->grouping = "";

    lc->int_curr_symbol = "";
    lc->currency_symbol = "";
    lc->mon_decimal_point = "";
    lc->mon_thousands_sep = "";
    lc->mon_grouping = "";
    lc->positive_sign = "";
    lc->negative_sign = "";
    lc->int_frac_digits = CHAR_MAX;
    lc->frac_digits = CHAR_MAX;
    lc->p_cs_precedes = CHAR_MAX;
    lc->p_sep_by_space = CHAR_MAX;
    lc->n_cs_precedes = CHAR_MAX;
    lc->n_sep_by_space = CHAR_MAX;
    lc->p_sign_posn = CHAR_MAX;
    lc->n_sign_posn = CHAR_MAX;
    lc->int_p_cs_precedes = CHAR_MAX;
    lc->int_p_sep_by_space = CHAR_MAX;
    lc->int_n_cs_precedes = CHAR_MAX;
    lc->int_n_sep_by_space = CHAR_MAX;
    lc->int_p_sign_posn = CHAR_MAX;
    lc->int_n_sign_posn = CHAR_MAX;
    return &lc;
}
char *          _RTL_FUNC setlocale( int __category, const char *__locale )
{	
    return 0;
}
