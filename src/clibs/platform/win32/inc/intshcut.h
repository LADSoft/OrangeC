#ifndef _INTSHCUT_H
#define _INTSHCUT_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows Internet Shortcut interface definitions */

#include <isguids.h>

#ifdef __cplusplus
extern "C" {
#endif

#define INTSHCUTAPI DECLSPEC_IMPORT

#define E_FLAGS  MAKE_SCODE(SEVERITY_ERROR,FACILITY_ITF,0x1000)
#define IS_E_EXEC_FAILED  MAKE_SCODE(SEVERITY_ERROR,FACILITY_ITF,0x2002)
#define URL_E_INVALID_SYNTAX  MAKE_SCODE(SEVERITY_ERROR,FACILITY_ITF,0x1001)
#define URL_E_UNREGISTERED_PROTOCOL  MAKE_SCODE(SEVERITY_ERROR,FACILITY_ITF,0x1002)

typedef enum iurl_seturl_flags {
    IURL_SETURL_FL_GUESS_PROTOCOL = 0x0001,
    IURL_SETURL_FL_USE_DEFAULT_PROTOCOL = 0x0002,
} IURL_SETURL_FLAGS;

typedef enum iurl_invokecommand_flags {
    IURL_INVOKECOMMAND_FL_ALLOW_UI = 0x0001,
    IURL_INVOKECOMMAND_FL_USE_DEFAULT_VERB = 0x0002,
    IURL_INVOKECOMMAND_FL_DDEWAIT = 0x0004,
} IURL_INVOKECOMMAND_FLAGS;

typedef enum translateurl_in_flags {
    TRANSLATEURL_FL_GUESS_PROTOCOL = 0x0001,
    TRANSLATEURL_FL_USE_DEFAULT_PROTOCOL = 0x0002,
} TRANSLATEURL_IN_FLAGS;

typedef enum urlassociationdialog_in_flags {
    URLASSOCDLG_FL_USE_DEFAULT_NAME = 0x0001,
    URLASSOCDLG_FL_REGISTER_ASSOC = 0x0002
} URLASSOCIATIONDIALOG_IN_FLAGS;

typedef enum mimeassociationdialog_in_flags {
    MIMEASSOCDLG_FL_REGISTER_ASSOC = 0x0001
} MIMEASSOCIATIONDIALOG_IN_FLAGS;

typedef struct urlinvokecommandinfoA {
    DWORD dwcbSize;
    DWORD dwFlags;
    HWND hwndParent;
    PCSTR pcszVerb;
} URLINVOKECOMMANDINFOA, *PURLINVOKECOMMANDINFOA;
typedef const URLINVOKECOMMANDINFOA CURLINVOKECOMMANDINFOA;
typedef const URLINVOKECOMMANDINFOA *PCURLINVOKECOMMANDINFOA;

typedef struct urlinvokecommandinfoW {
    DWORD dwcbSize;
    DWORD dwFlags;
    HWND hwndParent;
    PCWSTR pcszVerb;
} URLINVOKECOMMANDINFOW, *PURLINVOKECOMMANDINFOW;
typedef const URLINVOKECOMMANDINFOW CURLINVOKECOMMANDINFOW;
typedef const URLINVOKECOMMANDINFOW *PCURLINVOKECOMMANDINFOW;

#undef INTERFACE
#define INTERFACE IUniformResourceLocatorA
DECLARE_INTERFACE_(IUniformResourceLocatorA,IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(SetURL)(THIS_ PCSTR,DWORD) PURE;
    STDMETHOD(GetURL)(THIS_ PSTR*) PURE;
    STDMETHOD(InvokeCommand)(THIS_ PURLINVOKECOMMANDINFOA) PURE;
};
typedef IUniformResourceLocatorA *PIUniformResourceLocatorA;
typedef const IUniformResourceLocatorA CIUniformResourceLocatorA;
typedef const IUniformResourceLocatorA *PCIUniformResourceLocatorA;

#undef INTERFACE
#define INTERFACE IUniformResourceLocatorW
DECLARE_INTERFACE_(IUniformResourceLocatorW,IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID,PVOID*) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(SetURL)(THIS_ PCWSTR,DWORD) PURE;
    STDMETHOD(GetURL)(THIS_ PWSTR*) PURE;
    STDMETHOD(InvokeCommand)(THIS_ PURLINVOKECOMMANDINFOW) PURE;
};
typedef IUniformResourceLocatorW *PIUniformResourceLocatorW;
typedef const IUniformResourceLocatorW CIUniformResourceLocatorW;
typedef const IUniformResourceLocatorW *PCIUniformResourceLocatorW;

INTSHCUTAPI BOOL WINAPI InetIsOffline(DWORD);
INTSHCUTAPI HRESULT WINAPI MIMEAssociationDialogA(HWND,DWORD,PCSTR,PCSTR,PSTR,UINT);
INTSHCUTAPI HRESULT WINAPI MIMEAssociationDialogW(HWND,DWORD,PCWSTR,PCWSTR,PWSTR,UINT);
INTSHCUTAPI HRESULT WINAPI TranslateURLA(PCSTR,DWORD,PSTR*);
INTSHCUTAPI HRESULT WINAPI TranslateURLW(PCWSTR,DWORD,PWSTR UNALIGNED*);
INTSHCUTAPI HRESULT WINAPI URLAssociationDialogA(HWND,DWORD,PCSTR,PCSTR,PSTR,UINT);
INTSHCUTAPI HRESULT WINAPI URLAssociationDialogW(HWND,DWORD,PCWSTR,PCWSTR,PWSTR,UINT);

#ifdef UNICODE
#define URLINVOKECOMMANDINFO URLINVOKECOMMANDINFOW
#define PURLINVOKECOMMANDINFO PURLINVOKECOMMANDINFOW
#define CURLINVOKECOMMANDINFO CURLINVOKECOMMANDINFOW
#define PCURLINVOKECOMMANDINFO PCURLINVOKECOMMANDINFOW
#define IUniformResourceLocator IUniformResourceLocatorW
#define IUniformResourceLocatorVtbl IUniformResourceLocatorWVtbl
#define PIUniformResourceLocator PIUniformResourceLocatorW;
#define CIUniformResourceLocator CIUniformResourceLocatorW;
#define PCIUniformResourceLocator PCIUniformResourceLocatorW;
#define TranslateURL TranslateURLW
#define MIMEAssociationDialog MIMEAssociationDialogW
#define URLAssociationDialog URLAssociationDialogW
#else
#define URLINVOKECOMMANDINFO URLINVOKECOMMANDINFOA
#define PURLINVOKECOMMANDINFO PURLINVOKECOMMANDINFOA
#define CURLINVOKECOMMANDINFO CURLINVOKECOMMANDINFOA
#define PCURLINVOKECOMMANDINFO PCURLINVOKECOMMANDINFOA
#define IUniformResourceLocator IUniformResourceLocatorA
#define IUniformResourceLocatorVtbl IUniformResourceLocatorAVtbl
#define PIUniformResourceLocator PIUniformResourceLocatorA;
#define CIUniformResourceLocator CIUniformResourceLocatorA;
#define PCIUniformResourceLocator PCIUniformResourceLocatorA;
#define TranslateURL TranslateURLA
#define MIMEAssociationDialog MIMEAssociationDialogA
#define URLAssociationDialog URLAssociationDialogA
#endif /* UNICODE */

#ifdef __cplusplus
}
#endif

#endif /* _INTSHCUT_H */
