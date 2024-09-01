#ifndef _SDDL_H
#define _SDDL_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Security Descriptor Definition Language (SDDL) definitions */

#ifdef __cplusplus
extern "C" {
#endif

#define SDDL_REVISION_1  1
#define SDDL_REVISION  SDDL_REVISION_1

#define SDDL_OWNER  TEXT("O")
#define SDDL_GROUP  TEXT("G")
#define SDDL_DACL  TEXT("D")
#define SDDL_SACL  TEXT("S")

#define SDDL_PROTECTED  TEXT("P")
#define SDDL_AUTO_INHERIT_REQ  TEXT("AR")
#define SDDL_AUTO_INHERITED  TEXT("AI")

#define SDDL_ACCESS_ALLOWED  TEXT("A")
#define SDDL_ACCESS_DENIED  TEXT("D")
#define SDDL_OBJECT_ACCESS_ALLOWED  TEXT("OA")
#define SDDL_OBJECT_ACCESS_DENIED  TEXT("OD")
#define SDDL_AUDIT  TEXT("AU")
#define SDDL_ALARM  TEXT("AL")
#define SDDL_OBJECT_AUDIT  TEXT("OU")
#define SDDL_OBJECT_ALARM  TEXT("OL")

#define SDDL_CONTAINER_INHERIT  TEXT("CI")
#define SDDL_OBJECT_INHERIT  TEXT("OI")
#define SDDL_NO_PROPAGATE  TEXT("NP")
#define SDDL_INHERIT_ONLY  TEXT("IO")
#define SDDL_INHERITED  TEXT("ID")
#define SDDL_AUDIT_SUCCESS  TEXT("SA")
#define SDDL_AUDIT_FAILURE  TEXT("FA")

#define SDDL_READ_PROPERTY  TEXT("RP")
#define SDDL_WRITE_PROPERTY  TEXT("WP")
#define SDDL_CREATE_CHILD  TEXT("CC")
#define SDDL_DELETE_CHILD  TEXT("DC")
#define SDDL_LIST_CHILDREN  TEXT("LC")
#define SDDL_SELF_WRITE  TEXT("SW")
#define SDDL_LIST_OBJECT  TEXT("LO")
#define SDDL_DELETE_TREE  TEXT("DT")
#define SDDL_CONTROL_ACCESS  TEXT("CR")
#define SDDL_READ_CONTROL  TEXT("RC")
#define SDDL_WRITE_DAC  TEXT("WD")
#define SDDL_WRITE_OWNER  TEXT("WO")
#define SDDL_STANDARD_DELETE  TEXT("SD")
#define SDDL_GENERIC_ALL  TEXT("GA")
#define SDDL_GENERIC_READ  TEXT("GR")
#define SDDL_GENERIC_WRITE  TEXT("GW")
#define SDDL_GENERIC_EXECUTE  TEXT("GX")
#define SDDL_FILE_ALL  TEXT("FA")
#define SDDL_FILE_READ  TEXT("FR")
#define SDDL_FILE_WRITE  TEXT("FW")
#define SDDL_FILE_EXECUTE  TEXT("FX")
#define SDDL_KEY_ALL  TEXT("KA")
#define SDDL_KEY_READ  TEXT("KR")
#define SDDL_KEY_WRITE  TEXT("KW")
#define SDDL_KEY_EXECUTE  TEXT("KX")

#define SDDL_DOMAIN_ADMINISTRATORS  TEXT("DA")
#define SDDL_DOMAIN_GUESTS  TEXT("DG")
#define SDDL_DOMAIN_USERS  TEXT("DU")
#define SDDL_ENTERPRISE_DOMAIN_CONTROLLERS  TEXT("ED")
#define SDDL_DOMAIN_DOMAIN_CONTROLLERS  TEXT("DD")
#define SDDL_DOMAIN_COMPUTERS  TEXT("DC")
#define SDDL_BUILTIN_ADMINISTRATORS  TEXT("BA")
#define SDDL_BUILTIN_GUESTS  TEXT("BG")
#define SDDL_BUILTIN_USERS  TEXT("BU")
#define SDDL_LOCAL_ADMIN  TEXT("LA")
#define SDDL_LOCAL_GUEST  TEXT("LG")
#define SDDL_ACCOUNT_OPERATORS  TEXT("AO")
#define SDDL_BACKUP_OPERATORS  TEXT("BO")
#define SDDL_PRINTER_OPERATORS  TEXT("PO")
#define SDDL_SERVER_OPERATORS  TEXT("SO")
#define SDDL_AUTHENTICATED_USERS  TEXT("AU")
#define SDDL_PERSONAL_SELF  TEXT("PS")
#define SDDL_CREATOR_OWNER  TEXT("CO")
#define SDDL_CREATOR_GROUP  TEXT("CG")
#define SDDL_LOCAL_SYSTEM  TEXT("SY")
#define SDDL_POWER_USERS  TEXT("PU")
#define SDDL_EVERYONE  TEXT("WD")
#define SDDL_REPLICATOR  TEXT("RE")
#define SDDL_INTERACTIVE  TEXT("IU")
#define SDDL_NETWORK  TEXT("NU")
#define SDDL_SERVICE  TEXT("SU")
#define SDDL_RESTRICTED_CODE  TEXT("RC")
#define SDDL_SCHEMA_ADMINISTRATORS  TEXT("SA")
#define SDDL_CERT_SERV_ADMINISTRATORS  TEXT("CA")
#define SDDL_RAS_SERVERS  TEXT("RS")
#define SDDL_ENTERPRISE_ADMINS  TEXT("EA")
#define SDDL_GROUP_POLICY_ADMINS  TEXT("PA")
#define SDDL_ALIAS_PREW2KCOMPACC  TEXT("RU")

#define SDDL_SEPERATORC  TEXT(';')
#define SDDL_DELIMINATORC  TEXT(':')
#define SDDL_ACE_BEGINC  TEXT('(')
#define SDDL_ACE_ENDC  TEXT(')')

#if(_WIN32_WINNT >= 0x0500)
WINADVAPI BOOL WINAPI ConvertSidToStringSidA(PSID,LPSTR*);
WINADVAPI BOOL WINAPI ConvertSidToStringSidW(PSID,LPWSTR*);
WINADVAPI BOOL WINAPI ConvertStringSidToSidA(LPCSTR,PSID*);
WINADVAPI BOOL WINAPI ConvertStringSidToSidW(LPCWSTR,PSID*);
WINADVAPI BOOL WINAPI ConvertStringSecurityDescriptorToSecurityDescriptorA(LPCSTR,DWORD,PSECURITY_DESCRIPTOR*,PULONG);
WINADVAPI BOOL WINAPI ConvertStringSecurityDescriptorToSecurityDescriptorW(LPCWSTR,DWORD,PSECURITY_DESCRIPTOR*,PULONG);
WINADVAPI BOOL WINAPI ConvertSecurityDescriptorToStringSecurityDescriptorA(PSECURITY_DESCRIPTOR,DWORD,SECURITY_INFORMATION,LPSTR*,PULONG);
WINADVAPI BOOL WINAPI ConvertSecurityDescriptorToStringSecurityDescriptorW(PSECURITY_DESCRIPTOR,DWORD,SECURITY_INFORMATION,LPWSTR*,PULONG);

#ifdef UNICODE
#define ConvertSidToStringSid  ConvertSidToStringSidW
#define ConvertStringSidToSid  ConvertStringSidToSidW
#define ConvertStringSecurityDescriptorToSecurityDescriptor  ConvertStringSecurityDescriptorToSecurityDescriptorW
#define ConvertSecurityDescriptorToStringSecurityDescriptor  ConvertSecurityDescriptorToStringSecurityDescriptorW
#else
#define ConvertSidToStringSid  ConvertSidToStringSidA
#define ConvertStringSidToSid  ConvertStringSidToSidA
#define ConvertStringSecurityDescriptorToSecurityDescriptor  ConvertStringSecurityDescriptorToSecurityDescriptorA
#define ConvertSecurityDescriptorToStringSecurityDescriptor  ConvertSecurityDescriptorToStringSecurityDescriptorA
#endif /* UNICODE */
#endif /* _WIN32_WINNT >=  0x0500 */

#ifdef __cplusplus
}
#endif

#endif /* _SDDL_H */
