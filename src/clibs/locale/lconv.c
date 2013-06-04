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
#include "libp.h"
#include "_locale.h"

extern void * __locale_data[LC_LAST+1];
static char *tostring(char *str, char **stringbufptr)
{
    char *rv = *stringbufptr;
    while (*str)
        *(*stringbufptr)++ = *str++;
    *(*stringbufptr)++ = 0;
    return rv;
}
struct lconv *  _RTL_FUNC _llocaleconv( void )
{
    MONETARY_DATA *monetary_data =((MONETARY_DATA *)(__locale_data[LC_MONETARY])) ;
    NUMERIC_DATA *numeric_data = ((NUMERIC_DATA *)(__locale_data[LC_NUMERIC])) ;
    struct lconv *lc = &__getRtlData()->lconvbuf;
    char *stringbuf = __getRtlData()->lconvstringbuf;
    char *stringbufptr = stringbuf;

    lc->decimal_point = tostring(numeric_data->decimal_point,&stringbufptr);
    lc->thousands_sep = tostring(numeric_data->thousands_sep,&stringbufptr);
    lc->grouping = tostring(numeric_data->grouping,&stringbufptr);

    lc->int_curr_symbol = tostring(monetary_data->int_curr_symbol,&stringbufptr);
    lc->currency_symbol = tostring(monetary_data->currency_symbol,&stringbufptr);
    lc->mon_decimal_point = tostring(monetary_data->mon_decimal_point,&stringbufptr);
    lc->mon_thousands_sep = tostring(monetary_data->mon_thousands_sep,&stringbufptr);
    lc->mon_grouping = tostring(monetary_data->mon_grouping,&stringbufptr);
    lc->positive_sign = tostring(monetary_data->positive_sign,&stringbufptr);
    lc->negative_sign = tostring(monetary_data->negative_sign,&stringbufptr);
    lc->int_frac_digits = monetary_data->int_frac_digits;
    lc->frac_digits = monetary_data->frac_digits;
    lc->p_cs_precedes = monetary_data->p_cs_precedes;
    lc->p_sep_by_space = monetary_data->p_sep_by_space;
    lc->n_cs_precedes = monetary_data->n_cs_precedes;
    lc->n_sep_by_space = monetary_data->n_sep_by_space;
    lc->p_sign_posn = monetary_data->p_sign_posn;
    lc->n_sign_posn = monetary_data->n_sign_posn;
    lc->int_p_cs_precedes = monetary_data->int_p_cs_precedes;
    lc->int_p_sep_by_space = monetary_data->int_p_sep_by_space;
    lc->int_n_cs_precedes = monetary_data->int_n_cs_precedes;
    lc->int_n_sep_by_space = monetary_data->int_n_sep_by_space;
    lc->int_p_sign_posn = monetary_data->int_p_sign_posn;
    lc->int_n_sign_posn = monetary_data->int_n_sign_posn;    
    return lc;
}
