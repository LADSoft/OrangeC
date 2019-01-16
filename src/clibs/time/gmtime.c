/* Software License Agreement
 * 
 *     Copyright(C) 1994-2019 David Lindauer, (LADSoft)
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
 *     As a special exception, if other files instantiate templates or
 *     use macros or inline functions from this file, or you compile
 *     this file and link it with other works to produce a work based
 *     on this file, this file does not by itself cause the resulting
 *     work to be covered by the GNU General Public License. However
 *     the source code for this file must still be made available in
 *     accordance with section (3) of the GNU General Public License.
 *     
 *     This exception does not invalidate any other reasons why a work
 *     based on this file might be covered by the GNU General Public
 *     License.
 * 
 *     contact information:
 *         email: TouchStone222@runbox.com <David Lindauer>
 * 
 */

#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

static char _monthdays[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

struct tm *_RTL_FUNC gmtime(const time_t *time)
{
    struct tm *rv = &__getRtlData()->gmtime_buf;
	time_t t = *time;
	int temp1;
	
        t -= _daylight * 60 * 60;
	if (t & 0x80000000)
		return NULL;
	rv->tm_sec = t %60;
	t/=60;
	rv->tm_min = t %60;
	t /=60;
	rv->tm_hour = t %24;
	t /=24;
	rv->tm_yday = t;
	rv->tm_wday = (t +4 )%7;
	rv->tm_year = 70+(rv->tm_yday /365);
	rv->tm_yday = rv->tm_yday % 365;
	rv->tm_yday -= (rv->tm_year - 69)/4 ;
	if (rv->tm_yday <0) {
		rv->tm_year--;
		rv->tm_yday += 365 + ((rv->tm_year - 68) % 4 == 0);
	}
	if ((rv->tm_year - 68) % 4 == 0)
		_monthdays[1] = 29;
	else
		_monthdays[1] = 28;
	temp1 = rv->tm_yday;
	rv->tm_mon = -1;
	while (temp1 >=0)
		temp1-=_monthdays[++rv->tm_mon];
	rv->tm_mday = temp1 + _monthdays[rv->tm_mon]+1;
	rv->tm_isdst = 0;
   return rv;
}
