#include <windows.h>
#include <time.h>
#include <sys/timeb.h>

void __ll_xtime(struct _timeb* buf)
{
    SYSTEMTIME timex;
    struct tm tms;

    GetSystemTime(&timex);
    tms.tm_sec = timex.wSecond;
    tms.tm_min = timex.wMinute;
    tms.tm_hour = timex.wHour;
    tms.tm_mday = timex.wDay;
    tms.tm_mon = timex.wMonth - 1;
    tms.tm_year = timex.wYear - 1900;
    buf->time = mktime(&tms);
    buf->millitm = timex.wMilliseconds;
    buf->timezone = _timezone / 60;
    buf->dstflag = _daylight;
}
