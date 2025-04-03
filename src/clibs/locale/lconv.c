/*  Software License Agreement
 *  
 *      Copyright(C) 1994-2025 David Lindauer, (LADSoft)
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

#include <stddef.h>
#include <locale.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <wchar.h>
#include "libp.h"
#include "_locale.h"

extern void* __locale_data[LC_LAST + 1];
static char* tostring(char* str, char** stringbufptr)
{
    char* rv = *stringbufptr;
    while (*str)
        *(*stringbufptr)++ = *str++;
    *(*stringbufptr)++ = 0;
    return rv;
}
struct lconv* _RTL_FUNC _llocaleconv(void)
{
    MONETARY_DATA* monetary_data = ((MONETARY_DATA*)(__locale_data[LC_MONETARY]));
    NUMERIC_DATA* numeric_data = ((NUMERIC_DATA*)(__locale_data[LC_NUMERIC]));
    struct lconv* lc = &__getRtlData()->lconvbuf;
    char* stringbuf = __getRtlData()->lconvstringbuf;
    char* stringbufptr = stringbuf;

    lc->decimal_point = tostring(numeric_data->decimal_point, &stringbufptr);
    lc->thousands_sep = tostring(numeric_data->thousands_sep, &stringbufptr);
    lc->grouping = tostring(numeric_data->grouping, &stringbufptr);

    lc->int_curr_symbol = tostring(monetary_data->int_curr_symbol, &stringbufptr);
    lc->currency_symbol = tostring(monetary_data->currency_symbol, &stringbufptr);
    lc->mon_decimal_point = tostring(monetary_data->mon_decimal_point, &stringbufptr);
    lc->mon_thousands_sep = tostring(monetary_data->mon_thousands_sep, &stringbufptr);
    lc->mon_grouping = tostring(monetary_data->mon_grouping, &stringbufptr);
    lc->positive_sign = tostring(monetary_data->positive_sign, &stringbufptr);
    lc->negative_sign = tostring(monetary_data->negative_sign, &stringbufptr);
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
