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

#include <stddef.h>
#include <locale.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <wchar.h>
#include <limits.h>
#include "libp.h"
#include "_locale.h"

struct lconv* _RTL_FUNC localeconv(void)
{
    struct lconv* lc = &__getRtlData()->lconvbuf;

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
    return lc;
}
char* _RTL_FUNC setlocale(int __category, const char* __locale) { return 0; }
