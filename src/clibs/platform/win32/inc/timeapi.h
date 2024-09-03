#ifndef _TIMEAPI_H
#define _TIMEAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-mm-time-l1-1-0 */

/* #include <apiset.h> */
#include <apisetcconv.h>

#include <mmsyscom.h>

#ifndef MMNOTIMER  

#define TIMERR_NOERROR  (0)
#define TIMERR_NOCANDO  (TIMERR_BASE+1)
#define TIMERR_STRUCT   (TIMERR_BASE+33)

typedef struct timecaps_tag {
    UINT wPeriodMin;
    UINT wPeriodMax;
} TIMECAPS, *PTIMECAPS, *NPTIMECAPS, *LPTIMECAPS;

WINMMAPI MMRESULT WINAPI timeGetSystemTime(LPMMTIME, UINT);
WINMMAPI DWORD WINAPI timeGetTime(void);
WINMMAPI MMRESULT WINAPI timeGetDevCaps(LPTIMECAPS, UINT);
WINMMAPI MMRESULT WINAPI timeBeginPeriod(UINT);
WINMMAPI MMRESULT WINAPI timeEndPeriod(UINT);

#endif  /* ifndef MMNOTIMER */  

#endif /* _TIMEAPI_H */
