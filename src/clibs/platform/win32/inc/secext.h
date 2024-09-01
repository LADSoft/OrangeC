#ifndef _SECEXT_H
#define _SECEXT_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Security definitions not part of the SSPI interface */

#ifdef __cplusplus
extern "C" {
#endif

#if 0  /* see manual, page 42 */

#define SEC_WINNT_AUTH_IDENTITY_MARSHALLED  0x4
#define SEC_WINNT_AUTH_IDENTITY_ONLY  0x8

#define SECPKG_OPTIONS_TYPE_UNKNOWN 0
#define SECPKG_OPTIONS_TYPE_LSA  1
#define SECPKG_OPTIONS_TYPE_SSPI  2

#define SECPKG_OPTIONS_PERMANENT  0x00000001

#ifndef SEC_WINNT_AUTH_IDENTITY_VERSION
#define SEC_WINNT_AUTH_IDENTITY_VERSION  0x200

typedef struct _SEC_WINNT_AUTH_IDENTITY_EXW {
    unsigned long Version;
    unsigned long Length;
    unsigned short *User;
    unsigned long UserLength;
    unsigned short *Domain;
    unsigned long DomainLength;
    unsigned short *Password;
    unsigned long PasswordLength;
    unsigned long Flags;
    unsigned short *PackageList;
    unsigned long PackageListLength;
} SEC_WINNT_AUTH_IDENTITY_EXW, *PSEC_WINNT_AUTH_IDENTITY_EXW;

typedef struct _SEC_WINNT_AUTH_IDENTITY_EXA {
    unsigned long Version;
    unsigned long Length;
    unsigned char *User;
    unsigned long UserLength;
    unsigned char *Domain;
    unsigned long DomainLength;
    unsigned char *Password;
    unsigned long PasswordLength;
    unsigned long Flags;
    unsigned char *PackageList;
    unsigned long PackageListLength;
} SEC_WINNT_AUTH_IDENTITY_EXA, *PSEC_WINNT_AUTH_IDENTITY_EXA;

#ifdef UNICODE
#define SEC_WINNT_AUTH_IDENTITY_EX  SEC_WINNT_AUTH_IDENTITY_EXW
#else
#define SEC_WINNT_AUTH_IDENTITY_EX  SEC_WINNT_AUTH_IDENTITY_EXA
#endif

#endif /* SEC_WINNT_AUTH_IDENTITY_VERSION */

typedef struct _SECURITY_PACKAGE_OPTIONS {
    unsigned long Size;
    unsigned long Type;
    unsigned long Flags;
    unsigned long SignatureSize;
    void *Signature;
} SECURITY_PACKAGE_OPTIONS, *PSECURITY_PACKAGE_OPTIONS;

#endif /* 0 */

typedef enum {
    NameUnknown = 0,
    NameFullyQualifiedDN = 1,
    NameSamCompatible = 2,
    NameDisplay = 3,
    NameUniqueId = 6,
    NameCanonical = 7,
    NameUserPrincipal = 8,
    NameCanonicalEx = 9,
    NameServicePrincipal = 10,
    NameDnsDomain = 12,
} EXTENDED_NAME_FORMAT, *PEXTENDED_NAME_FORMAT;

/* SECURITY_STATUS SEC_ENTRY AddSecurityPackageA(SEC_CHAR*,SECURITY_PACKAGE_OPTIONS*); */
/* SECURITY_STATUS SEC_ENTRY AddSecurityPackageW(SEC_WCHAR*,SECURITY_PACKAGE_OPTIONS*); */
/* SECURITY_STATUS SEC_ENTRY DeleteSecurityPackageA(SEC_CHAR*); */
/* SECURITY_STATUS SEC_ENTRY DeleteSecurityPackageW(SEC_WCHAR*); */
BOOLEAN SEC_ENTRY GetUserNameExA(EXTENDED_NAME_FORMAT,LPSTR,PULONG);
BOOLEAN SEC_ENTRY GetUserNameExW(EXTENDED_NAME_FORMAT,LPWSTR,PULONG);
BOOLEAN SEC_ENTRY GetComputerObjectNameA(EXTENDED_NAME_FORMAT,LPSTR,PULONG);
BOOLEAN SEC_ENTRY GetComputerObjectNameW(EXTENDED_NAME_FORMAT,LPWSTR,PULONG);
BOOLEAN SEC_ENTRY TranslateNameA(LPCSTR,EXTENDED_NAME_FORMAT,EXTENDED_NAME_FORMAT,LPSTR,PULONG);
BOOLEAN SEC_ENTRY TranslateNameW(LPCWSTR,EXTENDED_NAME_FORMAT,EXTENDED_NAME_FORMAT,LPWSTR,PULONG);

#ifdef UNICODE
/* #define AddSecurityPackage  AddSecurityPackageW */
/* #define DeleteSecurityPackage  DeleteSecurityPackageW */
#define GetUserNameEx  GetUserNameExW
#define GetComputerObjectName  GetComputerObjectNameW
#define TranslateName  TranslateNameW
#else
/* #define AddSecurityPackage  AddSecurityPackageA */
/* #define DeleteSecurityPackage  DeleteSecurityPackageA */
#define GetUserNameEx  GetUserNameExA
#define GetComputerObjectName  GetComputerObjectNameA
#define TranslateName  TranslateNameA
#endif /* UNICODE */

#ifdef __cplusplus
}
#endif

#endif /* _SECEXT_H */
