#ifndef _APPNOTIFY_H
#define _APPNOTIFY_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-psm-app-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>

#undef APICONTRACT
#define APICONTRACT  DECLSPEC_IMPORT

typedef
VOID
(*PAPPSTATE_CHANGE_ROUTINE) (
     BOOLEAN ,
     PVOID 
    );

typedef struct _APPSTATE_REGISTRATION *PAPPSTATE_REGISTRATION;

#if (NTDDI_VERSION >= NTDDI_WINBLUE)
APICONTRACT ULONG NTAPI RegisterAppStateChangeNotification(PAPPSTATE_CHANGE_ROUTINE, PVOID, PAPPSTATE_REGISTRATION *);
APICONTRACT VOID NTAPI UnregisterAppStateChangeNotification(PAPPSTATE_REGISTRATION);
#endif /* NTDDI_VERSION >= NTDDI_WINBLUE */

#endif /* _APPNOTIFY_H */
