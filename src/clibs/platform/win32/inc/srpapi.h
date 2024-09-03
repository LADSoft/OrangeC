#ifndef _SRPAPI_H
#define _SRPAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet ext-ms-win-security-srp-l1 */

/* #include <apiset.h> */
#include <apisetcconv.h>

#include <minwindef.h>
#include <minwinbase.h>

#define HR_PROC_NOT_FOUND  HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND)

#if (_WIN32_WINNT >= _WIN32_WINNT_WINTHRESHOLD)

typedef struct HTHREAD_NETWORK_CONTEXT {
    DWORD ThreadId;
    HANDLE ThreadContext;
} HTHREAD_NETWORK_CONTEXT;

STDAPI SrpCreateThreadNetworkContext(PCWSTR, HTHREAD_NETWORK_CONTEXT *);
STDAPI SrpCloseThreadNetworkContext(HTHREAD_NETWORK_CONTEXT *);
STDAPI SrpSetTokenEnterpriseId(HANDLE, PCWSTR);
STDAPI SrpGetEnterpriseIds(HANDLE, PULONG, PCWSTR *, PULONG);
STDAPI SrpEnablePermissiveModeFileEncryption(PCWSTR);
STDAPI SrpDisablePermissiveModeFileEncryption(VOID);

#endif /* _WIN32_WINNT >= _WIN32_WINNT_WINTHRESHOLD */

typedef enum {
    ENTERPRISE_POLICY_NONE = 0x0,
    ENTERPRISE_POLICY_ALLOWED = 0x1,
    ENTERPRISE_POLICY_ENLIGHTENED = 0x2,
    ENTERPRISE_POLICY_EXEMPT = 0x4
} ENTERPRISE_DATA_POLICIES;

DEFINE_ENUM_FLAG_OPERATORS(ENTERPRISE_DATA_POLICIES);
STDAPI SrpGetEnterprisePolicy(HANDLE, ENTERPRISE_DATA_POLICIES *);
NTSTATUS SrpIsTokenService(HANDLE, BOOLEAN *);

#if (_WIN32_WINNT >= _WIN32_WINNT_WINTHRESHOLD)
#include <appmodel.h>
STDAPI SrpDoesPolicyAllowAppExecution(const PACKAGE_ID *, BOOL *);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WINTHRESHOLD */

#endif /* _SRPAPI_H */


#ifndef ext_ms_win_security_srp_l1_1_1_query_routines
#define ext_ms_win_security_srp_l1_1_1_query_routines

#if (_WIN32_WINNT >= _WIN32_WINNT_WINTHRESHOLD)

BOOLEAN __stdcall IsSrpCreateThreadNetworkContextPresent(VOID);
BOOLEAN __stdcall IsSrpCloseThreadNetworkContextPresent(VOID);
BOOLEAN __stdcall IsSrpSetTokenEnterpriseIdPresent(VOID);
BOOLEAN __stdcall IsSrpGetEnterpriseIdsPresent(VOID);
BOOLEAN __stdcall IsSrpEnablePermissiveModeFileEncryptionPresent(VOID);
BOOLEAN __stdcall IsSrpDisablePermissiveModeFileEncryptionPresent(VOID);

#endif /* _WIN32_WINNT >= _WIN32_WINNT_WINTHRESHOLD */

BOOLEAN __stdcall IsSrpGetEnterprisePolicyPresent(VOID);
BOOLEAN __stdcall IsSrpIsTokenServicePresent(VOID);

#if (_WIN32_WINNT >= _WIN32_WINNT_WINTHRESHOLD)
BOOLEAN __stdcall IsSrpDoesPolicyAllowAppExecutionPresent(VOID);
#endif /* _WIN32_WINNT >= _WIN32_WINNT_WINTHRESHOLD */

#endif /* ext_ms_win_security_srp_l1_1_1_query_routines */

