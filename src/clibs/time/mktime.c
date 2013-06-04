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
#include <time.h>

static char _monthdays[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

time_t _RTL_FUNC mktime(struct tm *timeptr)
{
	time_t days, x;
	int i;
	struct tm t = *timeptr;
	tzset();
	t.tm_year += 1900;
	if (t.tm_year < 1970 || t.tm_year > 2036)
		return (time_t)-1;
	if (!((t.tm_year - 1968)%4))
		_monthdays[1] = 29;
	else
		_monthdays[1] = 28;
	days = (t.tm_year - 1970)*365;
	days += (t.tm_year -1969)/4 ;

	if (t.tm_mon > 11)
		days += 365 + (t.tm_mon - 12) * 30;
	else
		for (i=0; i < t.tm_mon; i++)
			days += _monthdays[i];
	days += t.tm_mday-1;
	days *=24;
	days += t.tm_hour;
	days *=60;
	days += t.tm_min;
	days *=60;
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
	if (t.tm_yday < 0) {
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
