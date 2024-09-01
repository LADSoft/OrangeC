#ifndef _OOBENOTIFICATION_H
#define _OOBENOTIFICATION_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* API Set api-ms-win-oobe-notification-l1 */

#if (NTDDI_VERSION >= NTDDI_WINBLUE)

typedef VOID (CALLBACK *OOBE_COMPLETED_CALLBACK)(PVOID);

BOOL WINAPI OOBEComplete(PBOOL);
BOOL WINAPI RegisterWaitUntilOOBECompleted(OOBE_COMPLETED_CALLBACK, PVOID, PVOID *);
BOOL WINAPI UnregisterWaitUntilOOBECompleted(PVOID);

#endif /* NTDDI_VERSION >= NTDDI_WINBLUE */

#endif /* _OOBENOTIFICATION_H */
