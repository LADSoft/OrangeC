#ifndef _APPMGMT_H
#define _APPMGMT_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows MSI Application APIs */

#ifdef __cplusplus
extern "C" {
#endif

typedef enum _INSTALLSPECTYPE {
    APPNAME = 1,
    FILEEXT,
    PROGID,
    COMCLASS
} INSTALLSPECTYPE;

typedef union _INSTALLSPEC {
    struct {
        WCHAR *Name;
        GUID GPOId;
    } AppName;
    WCHAR *FileExt;
    WCHAR *ProgId;
    struct {
        GUID Clsid;
        DWORD ClsCtx;
    } COMClass;
} INSTALLSPEC;

typedef struct _INSTALLDATA {
    INSTALLSPECTYPE Type;
    INSTALLSPEC Spec;
} INSTALLDATA, *PINSTALLDATA;

typedef enum {
    ABSENT,
    ASSIGNED,
    PUBLISHED
} APPSTATE;

#define LOCALSTATE_ASSIGNED  0x1
#define LOCALSTATE_PUBLISHED  0x2
#define LOCALSTATE_UNINSTALL_UNMANAGED  0x4
#define LOCALSTATE_POLICYREMOVE_ORPHAN  0x8
#define LOCALSTATE_POLICYREMOVE_UNINSTALL  0x10
#define LOCALSTATE_ORPHANED  0x20
#define LOCALSTATE_UNINSTALLED  0x40

typedef struct _LOCALMANAGEDAPPLICATION {
    LPWSTR pszDeploymentName;
    LPWSTR pszPolicyName;
    LPWSTR pszProductId;
    DWORD dwState;
} LOCALMANAGEDAPPLICATION, *PLOCALMANAGEDAPPLICATION;

#define MANAGED_APPS_USERAPPLICATIONS  0x1
#define MANAGED_APPS_FROMCATEGORY  0x2
#define MANAGED_APPS_INFOLEVEL_DEFAULT  0x10000

#define MANAGED_APPTYPE_WINDOWSINSTALLER  0x1
#define MANAGED_APPTYPE_SETUPEXE  0x2
#define MANAGED_APPTYPE_UNSUPPORTED  0x3

typedef struct _MANAGEDAPPLICATION {
    LPWSTR pszPackageName;
    LPWSTR pszPublisher;
    DWORD dwVersionHi;
    DWORD dwVersionLo;
    DWORD dwRevision;
    GUID GpoId;
    LPWSTR pszPolicyName;
    GUID ProductId;
    LANGID Language;
    LPWSTR pszOwner;
    LPWSTR pszCompany;
    LPWSTR pszComments;
    LPWSTR pszContact;
    LPWSTR pszSupportUrl;
    DWORD dwPathType;
    BOOL bInstalled;
} MANAGEDAPPLICATION, *PMANAGEDAPPLICATION;

typedef struct _APPCATEGORYINFO {
    LCID Locale;
    LPWSTR pszDescription;
    GUID AppCategoryId;
} APPCATEGORYINFO;

typedef struct _APPCATEGORYINFOLIST {
    DWORD cCategory;
    APPCATEGORYINFO *pCategoryInfo;
} APPCATEGORYINFOLIST;

#ifndef WINAPI
#define WINAPI
#endif

DWORD WINAPI InstallApplication(PINSTALLDATA);
DWORD WINAPI UninstallApplication(WCHAR*,DWORD);
DWORD WINAPI CommandLineFromMsiDescriptor(WCHAR*,WCHAR*,DWORD*);
DWORD WINAPI GetManagedApplications(GUID*,DWORD,DWORD,LPDWORD,PMANAGEDAPPLICATION*);
DWORD WINAPI GetLocalManagedApplications(BOOL,LPDWORD,PLOCALMANAGEDAPPLICATION*);
void WINAPI GetLocalManagedApplicationData(WCHAR*,LPWSTR*,LPWSTR*);
DWORD WINAPI GetManagedApplicationCategories(DWORD,APPCATEGORYINFOLIST*);

#ifdef __cplusplus
}
#endif

#endif /* _APPMGMT_H */
