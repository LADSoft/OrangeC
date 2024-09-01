#ifndef _FEATURESTAGINGAPI_H
#define _FEATURESTAGINGAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-core-featurestaging-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>

typedef enum FEATURE_CHANGE_TIME {
    FEATURE_CHANGE_TIME_READ = 0,
    FEATURE_CHANGE_TIME_MODULE_RELOAD = 1,
    FEATURE_CHANGE_TIME_SESSION = 2,
    FEATURE_CHANGE_TIME_REBOOT = 3
} FEATURE_CHANGE_TIME;

typedef enum FEATURE_ENABLED_STATE {
    FEATURE_ENABLED_STATE_DEFAULT = 0,
    FEATURE_ENABLED_STATE_DISABLED = 1,
    FEATURE_ENABLED_STATE_ENABLED = 2
} FEATURE_ENABLED_STATE;

typedef struct FEATURE_ERROR {
    HRESULT hr;
    UINT16 lineNumber;
    PCSTR file;
    PCSTR process;
    PCSTR module;
    UINT32 callerReturnAddressOffset;
    PCSTR callerModule;
    PCSTR message;
    UINT16 originLineNumber;
    PCSTR originFile;
    PCSTR originModule;
    UINT32 originCallerReturnAddressOffset;
    PCSTR originCallerModule;
    PCSTR originName;
} FEATURE_ERROR;

DECLARE_HANDLE(FEATURE_STATE_CHANGE_SUBSCRIPTION);
typedef void WINAPI FEATURE_STATE_CHANGE_CALLBACK(void *context);
typedef FEATURE_STATE_CHANGE_CALLBACK *PFEATURE_STATE_CHANGE_CALLBACK;

EXTERN_C FEATURE_ENABLED_STATE GetFeatureEnabledState(UINT32, FEATURE_CHANGE_TIME);
EXTERN_C void RecordFeatureUsage(UINT32, UINT32, UINT32, PCSTR);
EXTERN_C void RecordFeatureError(UINT32, const FEATURE_ERROR *);
EXTERN_C void SubscribeFeatureStateChangeNotification(FEATURE_STATE_CHANGE_SUBSCRIPTION *, PFEATURE_STATE_CHANGE_CALLBACK, void *);
EXTERN_C void UnsubscribeFeatureStateChangeNotification(FEATURE_STATE_CHANGE_SUBSCRIPTION);

#endif /* _FEATURESTAGINGAPI_H */
