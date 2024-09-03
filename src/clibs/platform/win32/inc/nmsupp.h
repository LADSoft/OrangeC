#ifndef _NMSUPP_H
#define _NMSUPP_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Microsoft Network Monitor support definitions */

#include <pshpack1.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
#ifndef try
#define try  __try
#endif
#ifndef except
#define except  __except
#endif
#endif /* __cplusplus */

#define WINDOWS_VERSION_UNKNOWN  0
#define WINDOWS_VERSION_WIN32S  1
#define WINDOWS_VERSION_WIN32C  2
#define WINDOWS_VERSION_WIN32  3

#define FRAME_MASK_ETHERNET  ((BYTE) ~0x01)
#define FRAME_MASK_TOKENRING  ((BYTE) ~0x80)
#define FRAME_MASK_FDDI  ((BYTE) ~0x01)

#define HANDLE_TYPE_PASSWORD  MAKE_IDENTIFIER('P','W','D','$')

typedef enum _ACCESSRIGHTS {
    AccessRightsNoAccess,
    AccessRightsMonitoring,
    AccessRightsUserAccess,
    AccessRightsAllAccess
} ACCESSRIGHTS, *PACCESSRIGHTS;

typedef LPVOID HPASSWORD;
typedef LPVOID HOBJECTHEAP;
typedef VOID (WINAPI *OBJECTPROC)(HOBJECTHEAP,LPVOID);

#include <poppack.h>

typedef struct _TIMER *HTIMER;
typedef VOID (WINAPI *BHTIMERPROC)(LPVOID);

extern HTIMER WINAPI BhSetTimer(BHTIMERPROC,LPVOID,DWORD);
extern VOID WINAPI BhKillTimer(HTIMER);
extern LPVOID WINAPI BhAllocSystemMemory(DWORD);
extern LPVOID WINAPI BhFreeSystemMemory(LPVOID);
extern LPVOID WINAPI BhGetNetworkRequestAddress(DWORD);
extern DWORD WINAPI BhGetLastError(VOID);
extern DWORD WINAPI BhSetLastError(DWORD);
extern HOBJECTHEAP WINAPI CreateObjectHeap(DWORD,OBJECTPROC);
extern HOBJECTHEAP WINAPI DestroyObjectHeap(HOBJECTHEAP);
extern LPVOID WINAPI AllocObject(HOBJECTHEAP);
extern LPVOID WINAPI FreeObject(HOBJECTHEAP,LPVOID);
extern DWORD WINAPI GrowObjectHeap(HOBJECTHEAP,DWORD);
extern DWORD WINAPI GetObjectHeapSize(HOBJECTHEAP);
extern VOID WINAPI PurgeObjectHeap(HOBJECTHEAP);
extern LPVOID WINAPI AllocMemory(SIZE_T);
extern LPVOID WINAPI ReallocMemory(LPVOID,SIZE_T);
extern VOID WINAPI FreeMemory(LPVOID);
extern VOID WINAPI TestMemory(LPVOID);
extern SIZE_T WINAPI MemorySize(LPVOID);
extern HANDLE WINAPI MemoryHandle(LPBYTE);
extern HPASSWORD WINAPI CreatePassword(LPSTR);
extern VOID WINAPI DestroyPassword(HPASSWORD);
extern ACCESSRIGHTS WINAPI ValidatePassword(HPASSWORD);
extern LPEXPRESSION WINAPI InitializeExpression(LPEXPRESSION);
extern LPPATTERNMATCH WINAPI InitializePattern(LPPATTERNMATCH,LPVOID,DWORD,DWORD);
extern LPEXPRESSION WINAPI AndExpression(LPEXPRESSION,LPPATTERNMATCH);
extern LPEXPRESSION WINAPI OrExpression(LPEXPRESSION,LPPATTERNMATCH);
extern LPPATTERNMATCH WINAPI NegatePattern(LPPATTERNMATCH);
extern LPADDRESSTABLE WINAPI AdjustOperatorPrecedence(LPADDRESSTABLE);
extern LPADDRESS WINAPI NormalizeAddress(LPADDRESS);
extern LPADDRESSTABLE WINAPI NormalizeAddressTable(LPADDRESSTABLE);
extern HANDLE WINAPI BhOpenService(LPSTR);
extern VOID WINAPI BhCloseService(HANDLE);
extern DWORD WINAPI BhStartService(HANDLE);
extern DWORD WINAPI BhStopService(HANDLE);
extern DWORD WINAPI BhGetWindowsVersion(VOID);
extern BOOL WINAPI IsDaytona(VOID);
extern VOID _cdecl dprintf(LPSTR, ...);

#ifdef __cplusplus
}
#endif

#endif /* _NMSUPP_H */
