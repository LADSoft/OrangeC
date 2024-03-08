#include <windows.h>
#include <time.h>
#include <sys/timeb.h>

void _ftime(struct _timeb* buf)
{
    SYSTEMTIME timex;
    struct tm tms;
    __ll_tzset();
    GetLocalTime(&timex);
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
void ftime(struct timeb* buf) { _ftime((struct _timeb*)buf); }