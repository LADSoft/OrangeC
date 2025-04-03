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

#include <windows.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "libp.h"

#undef _daylight
#undef _timezone
#undef _tzname

static int _daylight = 0;
static long _timezone = 0;

static char tz1[36] = "EST", tz2[36] = "EDT";
static char* _tzname[2] = {tz1, tz2};

int* _RTL_FUNC __getDaylight(void)
{
    __ll_tzset();
    return &_daylight;
}
long* _RTL_FUNC __getTimezone(void)
{
    __ll_tzset();
    return &_timezone;
}
char** _RTL_FUNC __getTzName(void)
{
    __ll_tzset();
    return &_tzname[0];
}
static int parsetz(char* str)
{
    int temp;
    if (strlen(str) < 4 || (str[3] != '+' && str[3] != '-' && !isdigit(str[3])))
        return 0;
    memcpy(tz1, str, 3);
    str += 3;
    temp = strtol(str, &str, 10);
    if (str[0] && strlen(str) < 3)
        return 0;
    _timezone = temp * 60 * 60;
    if (str[0])
    {
        memcpy(tz2, str, 3);
        _daylight = 1;
        _tzname[1] = tz2;
        str += 3;
        if (str[0] && str[0] != ',')
        {
            temp = strtol(str, &str, 10);
            if (str[0] && str[0] != ',')
                return 0;
            _daylight = temp;
        }
        if (str[0] == ',')
        {
            str++;
            // fixme: now parse DST start/end fields
        }
    }
    else
    {
        _daylight = 0;
        _tzname[1] = 0;
    }
    return 1;
}
void __ll_tzset(void)
{
    TIME_ZONE_INFORMATION tzinfo;
    int temp;
    int i;
    char* s = getenv("TZ");
    __ll_enter_critical();

    if (s)
        if (parsetz(s))
        {
            __ll_exit_critical();
            return;
        }

    temp = GetTimeZoneInformation(&tzinfo);
    if (temp != TIME_ZONE_ID_INVALID)
    {
        _daylight = temp == TIME_ZONE_ID_DAYLIGHT;
        _timezone = tzinfo.Bias * 60;
        //      if (_daylight)
        //        _timezone -=tzinfo.DaylightBias ;
        //      else
        //        _timezone -=tzinfo.StandardBias ;

        if (_daylight)
        {
            _tzname[1] = tz2;
        }
        else
        {
            _tzname[1] = 0;
        }
        for (i = 0; i < 32; i++)
        {
            tz1[i] = tzinfo.StandardName[i];
            tz2[i] = tzinfo.DaylightName[i];
        }
    }
    __ll_exit_critical();
}
