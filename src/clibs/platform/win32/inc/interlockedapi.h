#ifndef _INTERLOCKAPI_H
#define _INTERLOCKAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-interlocked-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>
#include <minwindef.h>

#ifndef NOWINBASEINTERLOCK

#ifndef _NTOS_

#if defined(_SLIST_HEADER_) && !defined(_NTOSP_)

WINBASEAPI VOID WINAPI InitializeSListHead(PSLIST_HEADER);
WINBASEAPI PSLIST_ENTRY WINAPI InterlockedPopEntrySList(PSLIST_HEADER);
WINBASEAPI PSLIST_ENTRY WINAPI InterlockedPushEntrySList(PSLIST_HEADER, PSLIST_ENTRY);

#if (NTDDI_VERSION >= NTDDI_WIN8)
#define InterlockedPushListSList  InterlockedPushListSListEx
WINBASEAPI PSLIST_ENTRY WINAPI InterlockedPushListSListEx(PSLIST_HEADER, PSLIST_ENTRY, PSLIST_ENTRY, ULONG);
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

WINBASEAPI PSLIST_ENTRY WINAPI InterlockedFlushSList(PSLIST_HEADER);
WINBASEAPI USHORT WINAPI QueryDepthSList(PSLIST_HEADER);

#endif /* SLIST_HEADER_ */

#endif /* _NTOS_ */

#endif /* NOWINBASEINTERLOCK */

#endif /* _INTERLOCKAPI_H */
