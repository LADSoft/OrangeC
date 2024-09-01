#ifndef _MMISCAPI2_H
#define _MMISCAPI2_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-mm-misc-l2-1-0 */

/* #include <apiset.h> */
#include <apisetcconv.h>

#include <mmsyscom.h>

typedef void (CALLBACK TIMECALLBACK)(UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR);
typedef TIMECALLBACK *LPTIMECALLBACK;

#define TIME_ONESHOT    0x0000
#define TIME_PERIODIC   0x0001

#define TIME_CALLBACK_FUNCTION      0x0000
#define TIME_CALLBACK_EVENT_SET     0x0010
#define TIME_CALLBACK_EVENT_PULSE   0x0020

#if WINVER >= 0x0501
#define TIME_KILL_SYNCHRONOUS   0x0100
#endif /* WINVER >= 0x0501 */

WINMMAPI MMRESULT WINAPI timeSetEvent(UINT, UINT, LPTIMECALLBACK, DWORD_PTR, UINT);
WINMMAPI MMRESULT WINAPI timeKillEvent(UINT);

#endif /* _MMISCAPI2_H */
