#ifndef _RTLSUPPORTAPI_H
#define _RTLSUPPORTAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-rtlsupport-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>

/* #include <winapifamily.h> */

#if (NTDDI_VERSION > NTDDI_WINXP)
NTSYSAPI USHORT NTAPI RtlCaptureStackBackTrace(ULONG, ULONG, PVOID *, PULONG);
#endif /* NTDDI_VERSION > NTDDI_WINXP */

#if (NTDDI_VERSION > NTDDI_WIN2K)
NTSYSAPI VOID NTAPI RtlCaptureContext(PCONTEXT);
#endif /* NTDDI_VERSION > NTDDI_WIN2K */

NTSYSAPI VOID NTAPI RtlUnwind(PVOID, PVOID, PEXCEPTION_RECORD, PVOID);
NTSYSAPI VOID NTAPI RtlRaiseException(PEXCEPTION_RECORD);
NTSYSAPI PVOID NTAPI RtlPcToFileHeader(PVOID, PVOID *);

#if (NTDDI_VERSION >= NTDDI_WIN2K)
NTSYSAPI SIZE_T NTAPI RtlCompareMemory(const VOID *, const VOID *, SIZE_T);
#endif /* NTDDI_VERSION >= NTDDI_WIN2K */

#endif /* _RTLSUPPORTAPI_H */
