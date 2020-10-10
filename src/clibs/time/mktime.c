/* Software License Agreement
 *
 *     Copyright(C) 1994-2020 David Lindauer, (LADSoft)
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

static char _monthdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

time_t _RTL_FUNC mktime(struct tm* timeptr)
{
    time_t days, x;
    int i;
    struct tm t = *timeptr;
    tzset();
    t.tm_year += 1900;
    if (t.tm_year < 1970 || t.tm_year > 2036)
        return (time_t)-1;
    if (!((t.tm_year - 1968) % 4))
        _monthdays[1] = 29;
    else
        _monthdays[1] = 28;
    days = (t.tm_year - 1970) * 365;
    days += (t.tm_year - 1969) / 4;

    if (t.tm_mon > 11)
        days += 365 + (t.tm_mon - 12) * 30;
    else
        for (i = 0; i < t.tm_mon; i++)
            days += _monthdays[i];
    days += t.tm_mday - 1;
    days *= 24;
    days += t.tm_hour;
    days *= 60;
    days += t.tm_min;
    days *= 60;
    days += t.tm_sec;
    if (days < 0)
        return (time_t)-1;
    days += _timezone;
    if (days > 0x7fffffff)
        return (time_t)-1;

    if (t.tm_isdst < 0)
        t.tm_isdst = _daylight;
    x = days - _timezone;
    t.tm_sec = x % 60;
    x /= 60;
    t.tm_min = x % 60;
    x /= 60;
    t.tm_hour = x % 24;
    x /= 24;
    t.tm_yday = x;
    t.tm_wday = (x + 4) % 7;
    t.tm_year = 70 + (t.tm_yday / 365);
    t.tm_yday %= 365;
    t.tm_yday -= (t.tm_year - 69) / 4;
    if (t.tm_yday < 0)
    {
        t.tm_year--;
        t.tm_yday += 365 + ((t.tm_year - 68) % 4 == 0);
    }
    if ((t.tm_year - 68) % 4 == 0)
        _monthdays[1] = 29;
    else
        _monthdays[1] = 28;
    i = t.tm_yday;
    t.tm_mon = -1;
    while (i >= 0)
        i -= _monthdays[++t.tm_mon];
    t.tm_mday = i + _monthdays[t.tm_mon] + 1;
    *timeptr = t;
    return days;
}
