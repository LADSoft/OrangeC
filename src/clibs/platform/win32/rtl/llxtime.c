#include <windows.h>
#include <time.h>
#include <sys/timeb.h>

#pragma startup setfreq 1

static LARGE_INTEGER frequency;
static void setfreq()
{
    QueryPerformanceFrequency(&frequency);
}

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
int __ll_xtimehp(struct timespec* buf, int res)
{
    if (res)
    {
        long v = 1000000000/frequency.QuadPart;
        if (v == 0) v = 1;
        buf->tv_sec = 0;
        buf->tv_nsec = v;
    }
    else
    {
        LARGE_INTEGER count;
        QueryPerformanceCounter(&count);
        buf->tv_sec = count.QuadPart / frequency.QuadPart;
        long v = (count.QuadPart % frequency.QuadPart) * (1000000000/frequency.QuadPart);
        buf->tv_nsec = v;        
    }
    return 0;
}
int __ll_xtimeprocess(struct timespec* buf, int res)
{
    if (res)
    {
        buf->tv_sec = 0;
        buf->tv_nsec = 100;
    }
    else
    {
        FILETIME creat, exit, kernel, user;
        GetProcessTimes(GetCurrentProcess(), &creat, &exit, &kernel, &user);
        long long t = (((long long)user.dwHighDateTime) << 32) + user.dwLowDateTime;
        buf->tv_sec = t / (1000000000/100);
        buf->tv_nsec = t % (1000000000/100) * 100;
    }
    return 0;
}
int __ll_xtimethread(struct timespec* buf, int res)
{
    if (res)
    {
        buf->tv_sec = 0;
        buf->tv_nsec = 100;
    }
    else
    {
        FILETIME creat, exit, kernel, user;
        GetThreadTimes(GetCurrentThread(), &creat, &exit, &kernel, &user);
        long long t = (((long long)user.dwHighDateTime) << 32) + user.dwLowDateTime;
        buf->tv_sec = t / (1000000000/100);
        buf->tv_nsec = t % (1000000000/100) * 100;
    }
    return 0;
}