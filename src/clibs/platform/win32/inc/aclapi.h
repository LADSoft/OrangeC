#ifndef _ACLAPI_H
#define _ACLAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows Access Control API definitions */

#include <windows.h>
#include <accctrl.h>

typedef VOID (CALLBACK *FN_PROGRESS)(LPWSTR, DWORD, PPROG_INVOKE_SETTING, PVOID, BOOL);

WINADVAPI DWORD WINAPI SetEntriesInAclA(ULONG,PEXPLICIT_ACCESS_A,PACL,PACL*);
WINADVAPI DWORD WINAPI SetEntriesInAclW(ULONG,PEXPLICIT_ACCESS_W,PACL,PACL*);
WINADVAPI DWORD WINAPI GetExplicitEntriesFromAclA(PACL,PULONG,PEXPLICIT_ACCESS_A*);
WINADVAPI DWORD WINAPI GetExplicitEntriesFromAclW(PACL,PULONG,PEXPLICIT_ACCESS_W*);
WINADVAPI DWORD WINAPI GetEffectiveRightsFromAclA(PACL,PTRUSTEE_A,PACCESS_MASK);
WINADVAPI DWORD WINAPI GetEffectiveRightsFromAclW(PACL,PTRUSTEE_W,PACCESS_MASK);
WINADVAPI DWORD WINAPI GetAuditedPermissionsFromAclA(PACL,PTRUSTEE_A,PACCESS_MASK,PACCESS_MASK);
WINADVAPI DWORD WINAPI GetAuditedPermissionsFromAclW(PACL,PTRUSTEE_W,PACCESS_MASK,PACCESS_MASK);
WINADVAPI DWORD WINAPI GetNamedSecurityInfoA(LPCSTR,SE_OBJECT_TYPE,SECURITY_INFORMATION,PSID*,PSID*,PACL*,PACL*,PSECURITY_DESCRIPTOR*);
WINADVAPI DWORD WINAPI GetNamedSecurityInfoW(LPCWSTR,SE_OBJECT_TYPE,SECURITY_INFORMATION,PSID*,PSID*,PACL*,PACL*,PSECURITY_DESCRIPTOR*);
WINADVAPI DWORD WINAPI GetSecurityInfo(HANDLE,SE_OBJECT_TYPE,SECURITY_INFORMATION,PSID*,PSID*,PACL*,PACL*,PSECURITY_DESCRIPTOR*);
WINADVAPI DWORD WINAPI SetNamedSecurityInfoA(LPSTR,SE_OBJECT_TYPE,SECURITY_INFORMATION,PSID,PSID,PACL,PACL);
WINADVAPI DWORD WINAPI SetNamedSecurityInfoW(LPWSTR,SE_OBJECT_TYPE,SECURITY_INFORMATION,PSID,PSID,PACL,PACL);
WINADVAPI DWORD WINAPI SetSecurityInfo(HANDLE,SE_OBJECT_TYPE,SECURITY_INFORMATION,PSID,PSID,PACL,PACL);
WINADVAPI DWORD WINAPI GetInheritanceSourceA(LPSTR,SE_OBJECT_TYPE,SECURITY_INFORMATION,BOOL,GUID**,DWORD,PACL,PFN_OBJECT_MGR_FUNCTS,PGENERIC_MAPPING,PINHERITED_FROMA);
WINADVAPI DWORD WINAPI GetInheritanceSourceW(LPWSTR,SE_OBJECT_TYPE,SECURITY_INFORMATION,BOOL,GUID**,DWORD,PACL,PFN_OBJECT_MGR_FUNCTS,PGENERIC_MAPPING,PINHERITED_FROMW);
WINADVAPI DWORD WINAPI FreeInheritedFromArray(PINHERITED_FROMW,USHORT,PFN_OBJECT_MGR_FUNCTS);
WINADVAPI DWORD WINAPI TreeResetNamedSecurityInfoA(LPSTR,SE_OBJECT_TYPE,SECURITY_INFORMATION,PSID,PSID,PACL,PACL,BOOL,FN_PROGRESS,PROG_INVOKE_SETTING,PVOID);
WINADVAPI DWORD WINAPI TreeResetNamedSecurityInfoW(LPWSTR,SE_OBJECT_TYPE,SECURITY_INFORMATION,PSID,PSID,PACL,PACL,BOOL,FN_PROGRESS,PROG_INVOKE_SETTING,PVOID);
#if (NTDDI_VERSION >= NTDDI_VISTA)
WINADVAPI DWORD WINAPI TreeSetNamedSecurityInfoA(LPSTR,SE_OBJECT_TYPE,SECURITY_INFORMATION,PSID,PSID,PACL,PACL,DWORD,FN_PROGRESS,PROG_INVOKE_SETTING,PVOID);
WINADVAPI DWORD WINAPI TreeSetNamedSecurityInfoW(LPWSTR,SE_OBJECT_TYPE,SECURITY_INFORMATION,PSID,PSID,PACL,PACL,DWORD,FN_PROGRESS,PROG_INVOKE_SETTING,PVOID);
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

WINADVAPI DWORD WINAPI BuildSecurityDescriptorA(PTRUSTEE_A,PTRUSTEE_A,ULONG,PEXPLICIT_ACCESS_A,ULONG,PEXPLICIT_ACCESS_A,PSECURITY_DESCRIPTOR,PULONG,PSECURITY_DESCRIPTOR*);
WINADVAPI DWORD WINAPI BuildSecurityDescriptorW(PTRUSTEE_W,PTRUSTEE_W,ULONG,PEXPLICIT_ACCESS_W,ULONG,PEXPLICIT_ACCESS_W,PSECURITY_DESCRIPTOR,PULONG,PSECURITY_DESCRIPTOR*);
WINADVAPI DWORD WINAPI LookupSecurityDescriptorPartsA(PTRUSTEE_A*,PTRUSTEE_A*,PULONG,PEXPLICIT_ACCESS_A*,PULONG,PEXPLICIT_ACCESS_A*,PSECURITY_DESCRIPTOR);
WINADVAPI DWORD WINAPI LookupSecurityDescriptorPartsW(PTRUSTEE_W*,PTRUSTEE_W*,PULONG,PEXPLICIT_ACCESS_W*,PULONG,PEXPLICIT_ACCESS_W*,PSECURITY_DESCRIPTOR);
WINADVAPI VOID WINAPI BuildExplicitAccessWithNameA(PEXPLICIT_ACCESS_A,LPSTR,DWORD,ACCESS_MODE,DWORD);
WINADVAPI VOID WINAPI BuildExplicitAccessWithNameW(PEXPLICIT_ACCESS_W,LPWSTR,DWORD,ACCESS_MODE,DWORD);
WINADVAPI VOID WINAPI BuildImpersonateExplicitAccessWithNameA(PEXPLICIT_ACCESS_A,LPSTR,PTRUSTEE_A,DWORD,ACCESS_MODE,DWORD);
WINADVAPI VOID WINAPI BuildImpersonateExplicitAccessWithNameW(PEXPLICIT_ACCESS_W,LPWSTR,PTRUSTEE_W,DWORD,ACCESS_MODE,DWORD);
WINADVAPI VOID WINAPI BuildTrusteeWithNameA(PTRUSTEE_A,LPSTR);
WINADVAPI VOID WINAPI BuildTrusteeWithNameW(PTRUSTEE_W,LPWSTR);
WINADVAPI VOID WINAPI BuildImpersonateTrusteeA(PTRUSTEE_A,PTRUSTEE_A);
WINADVAPI VOID WINAPI BuildImpersonateTrusteeW(PTRUSTEE_W,PTRUSTEE_W);
WINADVAPI VOID WINAPI BuildTrusteeWithSidA(PTRUSTEE_A,PSID);
WINADVAPI VOID WINAPI BuildTrusteeWithSidW(PTRUSTEE_W,PSID);
WINADVAPI VOID WINAPI BuildTrusteeWithObjectsAndSidA(PTRUSTEE_A,POBJECTS_AND_SID,GUID*,GUID*,PSID);
WINADVAPI VOID WINAPI BuildTrusteeWithObjectsAndSidW(PTRUSTEE_W,POBJECTS_AND_SID,GUID*,GUID*,PSID);
WINADVAPI VOID WINAPI BuildTrusteeWithObjectsAndNameA(PTRUSTEE_A,POBJECTS_AND_NAME_A,SE_OBJECT_TYPE,LPSTR,LPSTR,LPSTR);
WINADVAPI VOID WINAPI BuildTrusteeWithObjectsAndNameW(PTRUSTEE_W,POBJECTS_AND_NAME_W,SE_OBJECT_TYPE,LPWSTR,LPWSTR,LPWSTR);
WINADVAPI LPSTR WINAPI GetTrusteeNameA(PTRUSTEE_A);
WINADVAPI LPWSTR WINAPI GetTrusteeNameW(PTRUSTEE_W);
WINADVAPI TRUSTEE_TYPE WINAPI GetTrusteeTypeA(PTRUSTEE_A);
WINADVAPI TRUSTEE_TYPE WINAPI GetTrusteeTypeW(PTRUSTEE_W);
WINADVAPI TRUSTEE_FORM WINAPI GetTrusteeFormA(PTRUSTEE_A);
WINADVAPI TRUSTEE_FORM WINAPI GetTrusteeFormW(PTRUSTEE_W);
WINADVAPI MULTIPLE_TRUSTEE_OPERATION WINAPI GetMultipleTrusteeOperationA(PTRUSTEE_A);
WINADVAPI MULTIPLE_TRUSTEE_OPERATION WINAPI GetMultipleTrusteeOperationW(PTRUSTEE_W);
WINADVAPI PTRUSTEE_A WINAPI GetMultipleTrusteeA(PTRUSTEE_A);
WINADVAPI PTRUSTEE_W WINAPI GetMultipleTrusteeW(PTRUSTEE_W);

#ifdef UNICODE
#define SetEntriesInAcl SetEntriesInAclW
#define GetExplicitEntriesFromAcl GetExplicitEntriesFromAclW
#define GetEffectiveRightsFromAcl GetEffectiveRightsFromAclW
#define GetAuditedPermissionsFromAcl GetAuditedPermissionsFromAclW
#define GetNamedSecurityInfo GetNamedSecurityInfoW
#define SetNamedSecurityInfo SetNamedSecurityInfoW
#define GetInheritanceSource  GetInheritanceSourceW
#define TreeResetNamedSecurityInfo  TreeResetNamedSecurityInfoW
#define TreeSetNamedSecurityInfo  TreeSetNamedSecurityInfoW
#define BuildSecurityDescriptor BuildSecurityDescriptorW
#define LookupSecurityDescriptorParts LookupSecurityDescriptorPartsW
#define BuildExplicitAccessWithName BuildExplicitAccessWithNameW
#define BuildImpersonateExplicitAccessWithName BuildImpersonateExplicitAccessWithNameW
#define BuildTrusteeWithName BuildTrusteeWithNameW
#define BuildImpersonateTrustee BuildImpersonateTrusteeW
#define BuildTrusteeWithSid BuildTrusteeWithSidW
#define BuildTrusteeWithObjectsAndSid BuildTrusteeWithObjectsAndSidW
#define BuildTrusteeWithObjectsAndName BuildTrusteeWithObjectsAndNameW
#define GetTrusteeName GetTrusteeNameW
#define GetTrusteeType GetTrusteeTypeW
#define GetTrusteeForm GetTrusteeFormW
#define GetMultipleTrusteeOperation GetMultipleTrusteeOperationW
#define GetMultipleTrustee GetMultipleTrusteeW
#else /* !UNICODE */
#define SetEntriesInAcl SetEntriesInAclA
#define GetExplicitEntriesFromAcl GetExplicitEntriesFromAclA
#define GetEffectiveRightsFromAcl GetEffectiveRightsFromAclA
#define GetAuditedPermissionsFromAcl GetAuditedPermissionsFromAclA
#define GetNamedSecurityInfo GetNamedSecurityInfoA
#define SetNamedSecurityInfo SetNamedSecurityInfoA
#define GetInheritanceSource  GetInheritanceSourceA
#define TreeResetNamedSecurityInfo  TreeResetNamedSecurityInfoA
#define TreeSetNamedSecurityInfo  TreeSetNamedSecurityInfoA
#define BuildSecurityDescriptor BuildSecurityDescriptorA
#define LookupSecurityDescriptorParts LookupSecurityDescriptorPartsA
#define BuildExplicitAccessWithName BuildExplicitAccessWithNameA
#define BuildImpersonateExplicitAccessWithName BuildImpersonateExplicitAccessWithNameA
#define BuildTrusteeWithName BuildTrusteeWithNameA
#define BuildImpersonateTrustee BuildImpersonateTrusteeA
#define BuildTrusteeWithSid BuildTrusteeWithSidA
#define BuildTrusteeWithObjectsAndSid BuildTrusteeWithObjectsAndSidA
#define BuildTrusteeWithObjectsAndName BuildTrusteeWithObjectsAndNameA
#define GetTrusteeName GetTrusteeNameA
#define GetTrusteeType GetTrusteeTypeA
#define GetTrusteeForm GetTrusteeFormA
#define GetMultipleTrusteeOperation GetMultipleTrusteeOperationA
#define GetMultipleTrustee GetMultipleTrusteeA
#endif /* !UNICODE */

#define AccProvInit(err)

#endif /* _ACLAPI_H */
