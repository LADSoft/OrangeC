#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__  500
#endif

#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__  100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of < rpcndr.h >
#endif

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /* COM_NO_WINDOWS_H */

#ifndef _URLMON_H
#define _URLMON_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IPersistMoniker_FWD_DEFINED__
#define __IPersistMoniker_FWD_DEFINED__
typedef interface IPersistMoniker IPersistMoniker;
#endif
#ifndef __IBindProtocol_FWD_DEFINED__
#define __IBindProtocol_FWD_DEFINED__
typedef interface IBindProtocol IBindProtocol;
#endif
#ifndef __IBinding_FWD_DEFINED__
#define __IBinding_FWD_DEFINED__
typedef interface IBinding IBinding;
#endif
#ifndef __IBindStatusCallback_FWD_DEFINED__
#define __IBindStatusCallback_FWD_DEFINED__
typedef interface IBindStatusCallback IBindStatusCallback;
#endif
#ifndef __IAuthenticate_FWD_DEFINED__
#define __IAuthenticate_FWD_DEFINED__
typedef interface IAuthenticate IAuthenticate;
#endif
#ifndef __IHttpNegotiate_FWD_DEFINED__
#define __IHttpNegotiate_FWD_DEFINED__
typedef interface IHttpNegotiate IHttpNegotiate;
#endif
#ifndef __IWindowForBindingUI_FWD_DEFINED__
#define __IWindowForBindingUI_FWD_DEFINED__
typedef interface IWindowForBindingUI IWindowForBindingUI;
#endif
#ifndef __ICodeInstall_FWD_DEFINED__
#define __ICodeInstall_FWD_DEFINED__
typedef interface ICodeInstall ICodeInstall;
#endif
#ifndef __IUri_FWD_DEFINED__
#define __IUri_FWD_DEFINED__
typedef interface IUri IUri;
#endif
#ifndef __IUriContainer_FWD_DEFINED__
#define __IUriContainer_FWD_DEFINED__
typedef interface IUriContainer IUriContainer;
#endif
#ifndef __IUriBuilder_FWD_DEFINED__
#define __IUriBuilder_FWD_DEFINED__
typedef interface IUriBuilder IUriBuilder;
#endif
#ifndef __IUriBuilderFactory_FWD_DEFINED__
#define __IUriBuilderFactory_FWD_DEFINED__
typedef interface IUriBuilderFactory IUriBuilderFactory;
#endif
#ifndef __IWinInetInfo_FWD_DEFINED__
#define __IWinInetInfo_FWD_DEFINED__
typedef interface IWinInetInfo IWinInetInfo;
#endif
#ifndef __IHttpSecurity_FWD_DEFINED__
#define __IHttpSecurity_FWD_DEFINED__
typedef interface IHttpSecurity IHttpSecurity;
#endif
#ifndef __IWinInetHttpInfo_FWD_DEFINED__
#define __IWinInetHttpInfo_FWD_DEFINED__
typedef interface IWinInetHttpInfo IWinInetHttpInfo;
#endif
#ifndef __IWinInetCacheHints_FWD_DEFINED__
#define __IWinInetCacheHints_FWD_DEFINED__
typedef interface IWinInetCacheHints IWinInetCacheHints;
#endif
#ifndef __IWinInetCacheHints2_FWD_DEFINED__
#define __IWinInetCacheHints2_FWD_DEFINED__
typedef interface IWinInetCacheHints2 IWinInetCacheHints2;
#endif
#ifndef __IBindHost_FWD_DEFINED__
#define __IBindHost_FWD_DEFINED__
typedef interface IBindHost IBindHost;
#endif
#ifndef __IInternet_FWD_DEFINED__
#define __IInternet_FWD_DEFINED__
typedef interface IInternet IInternet;
#endif
#ifndef __IInternetBindInfo_FWD_DEFINED__
#define __IInternetBindInfo_FWD_DEFINED__
typedef interface IInternetBindInfo IInternetBindInfo;
#endif
#ifndef __IInternetProtocolRoot_FWD_DEFINED__
#define __IInternetProtocolRoot_FWD_DEFINED__
typedef interface IInternetProtocolRoot IInternetProtocolRoot;
#endif
#ifndef __IInternetProtocol_FWD_DEFINED__
#define __IInternetProtocol_FWD_DEFINED__
typedef interface IInternetProtocol IInternetProtocol;
#endif
#ifndef __IInternetProtocolEx_FWD_DEFINED__
#define __IInternetProtocolEx_FWD_DEFINED__
typedef interface IInternetProtocolEx IInternetProtocolEx;
#endif
#ifndef __IInternetProtocolSink_FWD_DEFINED__
#define __IInternetProtocolSink_FWD_DEFINED__
typedef interface IInternetProtocolSink IInternetProtocolSink;
#endif
#ifndef __IInternetProtocolSinkStackable_FWD_DEFINED__
#define __IInternetProtocolSinkStackable_FWD_DEFINED__
typedef interface IInternetProtocolSinkStackable IInternetProtocolSinkStackable;
#endif
#ifndef __IInternetSession_FWD_DEFINED__
#define __IInternetSession_FWD_DEFINED__
typedef interface IInternetSession IInternetSession;
#endif
#ifndef __IInternetThreadSwitch_FWD_DEFINED__
#define __IInternetThreadSwitch_FWD_DEFINED__
typedef interface IInternetThreadSwitch IInternetThreadSwitch;
#endif
#ifndef __IInternetPriority_FWD_DEFINED__
#define __IInternetPriority_FWD_DEFINED__
typedef interface IInternetPriority IInternetPriority;
#endif
#ifndef __IInternetProtocolInfo_FWD_DEFINED__
#define __IInternetProtocolInfo_FWD_DEFINED__
typedef interface IInternetProtocolInfo IInternetProtocolInfo;
#endif
#ifndef __IInternetSecurityMgrSite_FWD_DEFINED__
#define __IInternetSecurityMgrSite_FWD_DEFINED__
typedef interface IInternetSecurityMgrSite IInternetSecurityMgrSite;
#endif
#ifndef __IInternetSecurityManager_FWD_DEFINED__
#define __IInternetSecurityManager_FWD_DEFINED__
typedef interface IInternetSecurityManager IInternetSecurityManager;
#endif
#ifndef __IInternetSecurityManagerEx_FWD_DEFINED__
#define __IInternetSecurityManagerEx_FWD_DEFINED__
typedef interface IInternetSecurityManagerEx IInternetSecurityManagerEx;
#endif
#ifndef __IInternetSecurityManagerEx2_FWD_DEFINED__
#define __IInternetSecurityManagerEx2_FWD_DEFINED__
typedef interface IInternetSecurityManagerEx2 IInternetSecurityManagerEx2;
#endif
#ifndef __IZoneIdentifier_FWD_DEFINED__
#define __IZoneIdentifier_FWD_DEFINED__
typedef interface IZoneIdentifier IZoneIdentifier;
#endif
#ifndef __IInternetHostSecurityManager_FWD_DEFINED__
#define __IInternetHostSecurityManager_FWD_DEFINED__
typedef interface IInternetHostSecurityManager IInternetHostSecurityManager;
#endif
#ifndef __IInternetZoneManager_FWD_DEFINED__
#define __IInternetZoneManager_FWD_DEFINED__
typedef interface IInternetZoneManager IInternetZoneManager;
#endif
#ifndef __IInternetZoneManagerEx_FWD_DEFINED__
#define __IInternetZoneManagerEx_FWD_DEFINED__
typedef interface IInternetZoneManagerEx IInternetZoneManagerEx;
#endif
#ifndef __IInternetZoneManagerEx2_FWD_DEFINED__
#define __IInternetZoneManagerEx2_FWD_DEFINED__
typedef interface IInternetZoneManagerEx2 IInternetZoneManagerEx2;
#endif
#ifndef __ISoftDistExt_FWD_DEFINED__
#define __ISoftDistExt_FWD_DEFINED__
typedef interface ISoftDistExt ISoftDistExt;
#endif
#ifndef __ICatalogFileInfo_FWD_DEFINED__
#define __ICatalogFileInfo_FWD_DEFINED__
typedef interface ICatalogFileInfo ICatalogFileInfo;
#endif
#ifndef __IDataFilter_FWD_DEFINED__
#define __IDataFilter_FWD_DEFINED__
typedef interface IDataFilter IDataFilter;
#endif
#ifndef __IEncodingFilterFactory_FWD_DEFINED__
#define __IEncodingFilterFactory_FWD_DEFINED__
typedef interface IEncodingFilterFactory IEncodingFilterFactory;
#endif

#include "objidl.h"
#include "oleidl.h"
#include "servprov.h"
#include "msxml.h"

#ifdef __cplusplus
extern "C" {
#endif

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *);

#define BINDF_DONTUSECACHE  BINDF_GETNEWESTVERSION
#define BINDF_DONTPUTINCACHE  BINDF_NOWRITECACHE
#define BINDF_NOCOPYDATA  BINDF_PULLDATA
#define PI_DOCFILECLSIDLOOKUP  PI_CLSIDLOOKUP

EXTERN_C const IID IID_IAsyncMoniker;
EXTERN_C const IID CLSID_StdURLMoniker;
EXTERN_C const IID CLSID_HttpProtocol;
EXTERN_C const IID CLSID_FtpProtocol;
EXTERN_C const IID CLSID_GopherProtocol;
EXTERN_C const IID CLSID_HttpSProtocol;
EXTERN_C const IID CLSID_FileProtocol;
EXTERN_C const IID CLSID_MkProtocol;
EXTERN_C const IID CLSID_StdURLProtocol;
EXTERN_C const IID CLSID_UrlMkBindCtx;
EXTERN_C const IID CLSID_StdEncodingFilterFac;
EXTERN_C const IID CLSID_DeCompMimeFilter;
EXTERN_C const IID CLSID_CdlProtocol;
EXTERN_C const IID CLSID_ClassInstallFilter;
EXTERN_C const IID IID_IAsyncBindCtx;

#define SZ_URLCONTEXT  OLESTR("URL Context")
#define SZ_ASYNC_CALLEE  OLESTR("AsyncCallee")
#define MKSYS_URLMONIKER  6

STDAPI CreateURLMoniker(LPMONIKER,LPCWSTR,LPMONIKER*);
STDAPI GetClassURL(LPCWSTR,CLSID*);
STDAPI CreateAsyncBindCtx(DWORD,IBindStatusCallback*,IEnumFORMATETC*,IBindCtx**);
STDAPI CreateAsyncBindCtxEx(IBindCtx*,DWORD,IBindStatusCallback*,IEnumFORMATETC*,IBindCtx**,DWORD);
#if (_WIN32_IE >= _WIN32_IE_IE70)
STDAPI CreateURLMonikerEx2(LPMONIKER, IUri*, LPMONIKER*, DWORD);
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */
STDAPI MkParseDisplayNameEx(IBindCtx*,LPCWSTR,ULONG*,LPMONIKER*);
STDAPI RegisterBindStatusCallback(LPBC,IBindStatusCallback*,IBindStatusCallback**,DWORD);
STDAPI RevokeBindStatusCallback(LPBC,IBindStatusCallback*);
STDAPI GetClassFileOrMime(LPBC,LPCWSTR,LPVOID,DWORD,LPCWSTR,DWORD,CLSID*);
STDAPI IsValidURL(LPBC,LPCWSTR,DWORD);
STDAPI CoGetClassObjectFromURL(REFCLSID,LPCWSTR,DWORD,DWORD,LPCWSTR,LPBINDCTX,DWORD,LPVOID,REFIID,LPVOID*);
STDAPI FaultInIEFeature(HWND,uCLSSPEC*,QUERYCONTEXT*,DWORD);
STDAPI GetComponentIDFromCLSSPEC(uCLSSPEC*,LPSTR*);

#define FIEF_FLAG_FORCE_JITUI  0x1
#define FIEF_FLAG_PEEK  0x2
#define FIEF_FLAG_SKIP_INSTALLED_VERSION_CHECK  0x4

STDAPI IsAsyncMoniker(IMoniker*);
STDAPI CreateURLBinding(LPCWSTR,IBindCtx*,IBinding**);
STDAPI RegisterMediaTypes(UINT,const LPCSTR*,CLIPFORMAT*);
STDAPI FindMediaType(LPCSTR,CLIPFORMAT*);
STDAPI CreateFormatEnumerator(UINT,FORMATETC*,IEnumFORMATETC**);
STDAPI RegisterFormatEnumerator(LPBC,IEnumFORMATETC*,DWORD);
STDAPI RevokeFormatEnumerator(LPBC,IEnumFORMATETC*);
STDAPI RegisterMediaTypeClass(LPBC,UINT,const LPCSTR*,CLSID*,DWORD);
STDAPI FindMediaTypeClass(LPBC,LPCSTR,CLSID*,DWORD);
STDAPI UrlMkSetSessionOption(DWORD,LPVOID,DWORD,DWORD);
STDAPI UrlMkGetSessionOption(DWORD,LPVOID,DWORD,DWORD*,DWORD);
STDAPI FindMimeFromData(LPBC,LPCWSTR,LPVOID,DWORD,LPCWSTR,DWORD,LPWSTR*,DWORD);

#define FMFD_DEFAULT  0x00000000
#define FMFD_URLASFILENAME  0x00000001
#if (_WIN32_IE >= _WIN32_IE_IE60SP2)
#define FMFD_ENABLEMIMESNIFFING  0x00000002
#define FMFD_IGNOREMIMETEXTPLAIN  0x00000004
#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */
#define FMFD_SERVERMIME  0x00000008 
#define UAS_EXACTLEGACY  0x00001000 

STDAPI ObtainUserAgentString(DWORD dwOption,LPSTR pszUAOut,DWORD*);

#define URLMON_OPTION_USERAGENT  0x10000001
#define URLMON_OPTION_USERAGENT_REFRESH 0x10000002
#define URLMON_OPTION_URL_ENCODING  0x10000004
#if (_WIN32_IE >= _WIN32_IE_IE60SP2)
#define URLMON_OPTION_USE_BINDSTRINGCREDS 0x10000008
#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */
#if (_WIN32_IE >= _WIN32_IE_IE70)
#define URLMON_OPTION_USE_BROWSERAPPSDOCUMENTS 0x10000010
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */

#define CF_NULL  0

#define CFSTR_MIME_NULL  NULL
#define CFSTR_MIME_TEXT  (TEXT("text/plain"))
#define CFSTR_MIME_RICHTEXT  (TEXT("text/richtext"))
#define CFSTR_MIME_X_BITMAP  (TEXT("image/x-xbitmap"))
#define CFSTR_MIME_POSTSCRIPT  (TEXT("application/postscript"))
#define CFSTR_MIME_AIFF  (TEXT("audio/aiff"))
#define CFSTR_MIME_BASICAUDIO  (TEXT("audio/basic"))
#define CFSTR_MIME_WAV  (TEXT("audio/wav"))
#define CFSTR_MIME_X_WAV  (TEXT("audio/x-wav"))
#define CFSTR_MIME_GIF  (TEXT("image/gif"))
#define CFSTR_MIME_PJPEG  (TEXT("image/pjpeg"))
#define CFSTR_MIME_JPEG  (TEXT("image/jpeg"))
#define CFSTR_MIME_TIFF  (TEXT("image/tiff"))
#define CFSTR_MIME_X_PNG  (TEXT("image/x-png"))
#define CFSTR_MIME_BMP  (TEXT("image/bmp"))
#define CFSTR_MIME_X_ART  (TEXT("image/x-jg"))
#define CFSTR_MIME_X_EMF  (TEXT("image/x-emf"))
#define CFSTR_MIME_X_WMF  (TEXT("image/x-wmf"))
#define CFSTR_MIME_AVI  (TEXT("video/avi"))
#define CFSTR_MIME_MPEG  (TEXT("video/mpeg"))
#define CFSTR_MIME_FRACTALS  (TEXT("application/fractals"))
#define CFSTR_MIME_RAWDATA  (TEXT("application/octet-stream"))
#define CFSTR_MIME_RAWDATASTRM  (TEXT("application/octet-stream"))
#define CFSTR_MIME_PDF  (TEXT("application/pdf"))
#define CFSTR_MIME_X_AIFF  (TEXT("audio/x-aiff"))
#define CFSTR_MIME_X_REALAUDIO  (TEXT("audio/x-pn-realaudio"))
#define CFSTR_MIME_XBM  (TEXT("image/xbm"))
#define CFSTR_MIME_QUICKTIME  (TEXT("video/quicktime"))
#define CFSTR_MIME_X_MSVIDEO  (TEXT("video/x-msvideo"))
#define CFSTR_MIME_X_SGI_MOVIE  (TEXT("video/x-sgi-movie"))
#define CFSTR_MIME_HTML  (TEXT("text/html"))

#define MK_S_ASYNCHRONOUS  _HRESULT_TYPEDEF_(0x000401E8L)

#ifndef S_ASYNCHRONOUS
#define S_ASYNCHRONOUS  MK_S_ASYNCHRONOUS
#endif

#ifndef E_PENDING
#define E_PENDING  _HRESULT_TYPEDEF_(0x8000000AL)
#endif

#define INET_E_INVALID_URL  _HRESULT_TYPEDEF_(0x800C0002L)
#define INET_E_NO_SESSION  _HRESULT_TYPEDEF_(0x800C0003L)
#define INET_E_CANNOT_CONNECT  _HRESULT_TYPEDEF_(0x800C0004L)
#define INET_E_RESOURCE_NOT_FOUND  _HRESULT_TYPEDEF_(0x800C0005L)
#define INET_E_OBJECT_NOT_FOUND  _HRESULT_TYPEDEF_(0x800C0006L)
#define INET_E_DATA_NOT_AVAILABLE  _HRESULT_TYPEDEF_(0x800C0007L)
#define INET_E_DOWNLOAD_FAILURE  _HRESULT_TYPEDEF_(0x800C0008L)
#define INET_E_AUTHENTICATION_REQUIRED  _HRESULT_TYPEDEF_(0x800C0009L)
#define INET_E_NO_VALID_MEDIA  _HRESULT_TYPEDEF_(0x800C000AL)
#define INET_E_CONNECTION_TIMEOUT  _HRESULT_TYPEDEF_(0x800C000BL)
#define INET_E_INVALID_REQUEST  _HRESULT_TYPEDEF_(0x800C000CL)
#define INET_E_UNKNOWN_PROTOCOL  _HRESULT_TYPEDEF_(0x800C000DL)
#define INET_E_SECURITY_PROBLEM  _HRESULT_TYPEDEF_(0x800C000EL)
#define INET_E_CANNOT_LOAD_DATA  _HRESULT_TYPEDEF_(0x800C000FL)
#define INET_E_CANNOT_INSTANTIATE_OBJECT  _HRESULT_TYPEDEF_(0x800C0010L)
#define INET_E_REDIRECT_FAILED  _HRESULT_TYPEDEF_(0x800C0014L)
#define INET_E_REDIRECT_TO_DIR  _HRESULT_TYPEDEF_(0x800C0015L)
#define INET_E_CANNOT_LOCK_REQUEST  _HRESULT_TYPEDEF_(0x800C0016L)
#define INET_E_USE_EXTEND_BINDING  _HRESULT_TYPEDEF_(0x800C0017L)
#define INET_E_ERROR_FIRST  _HRESULT_TYPEDEF_(0x800C0002L)
#define INET_E_CODE_DOWNLOAD_DECLINED  _HRESULT_TYPEDEF_(0x800C0100L)
#define INET_E_RESULT_DISPATCHED  _HRESULT_TYPEDEF_(0x800C0200L)
#define INET_E_CANNOT_REPLACE_SFP_FILE  _HRESULT_TYPEDEF_(0x800C0300L)
#if (_WIN32_IE >= _WIN32_IE_IE60SP2)
#define INET_E_CODE_INSTALL_SUPPRESSED  _HRESULT_TYPEDEF_(0x800C0400L)
#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */
#define INET_E_CODE_INSTALL_BLOCKED_BY_HASH_POLICY  _HRESULT_TYPEDEF_(0x800C0500L)
#define INET_E_DOWNLOAD_BLOCKED_BY_INPRIVATE  _HRESULT_TYPEDEF_(0x800C0501L)
#define INET_E_ERROR_LAST  INET_E_DOWNLOAD_BLOCKED_BY_INPRIVATE


#ifndef _LPPERSISTMONIKER_DEFINED
#define _LPPERSISTMONIKER_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_v0_0_s_ifspec;

#ifndef __IPersistMoniker_INTERFACE_DEFINED__
#define __IPersistMoniker_INTERFACE_DEFINED__

typedef IPersistMoniker *LPPERSISTMONIKER;
EXTERN_C const IID IID_IPersistMoniker;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9c9-baf9-11ce-8c82-00aa004ba90b")
IPersistMoniker:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetClassID(CLSID*) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsDirty(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Load(BOOL,IMoniker*,LPBC,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE Save(IMoniker*,LPBC,BOOL) = 0;
    virtual HRESULT STDMETHODCALLTYPE SaveCompleted(IMoniker*,LPBC) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurMoniker(IMoniker**) = 0;
};
#else
typedef struct IPersistMonikerVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IPersistMoniker*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IPersistMoniker*);
    ULONG(STDMETHODCALLTYPE *Release)(IPersistMoniker*);
    HRESULT(STDMETHODCALLTYPE *GetClassID)(IPersistMoniker*,CLSID*);
    HRESULT(STDMETHODCALLTYPE *IsDirty)(IPersistMoniker*);
    HRESULT(STDMETHODCALLTYPE *Load)(IPersistMoniker*,BOOL,IMoniker*,LPBC,DWORD);
    HRESULT(STDMETHODCALLTYPE *Save)(IPersistMoniker*,IMoniker*,LPBC,BOOL);
    HRESULT(STDMETHODCALLTYPE *SaveCompleted)(IPersistMoniker*,IMoniker*,LPBC);
    HRESULT(STDMETHODCALLTYPE *GetCurMoniker)(IPersistMoniker*,IMoniker**);
    END_INTERFACE
} IPersistMonikerVtbl;

interface IPersistMoniker {
    CONST_VTBL struct IPersistMonikerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPersistMoniker_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IPersistMoniker_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IPersistMoniker_Release(This)  (This)->lpVtbl->Release(This)
#define IPersistMoniker_GetClassID(This,pClassID)  (This)->lpVtbl->GetClassID(This,pClassID)
#define IPersistMoniker_IsDirty(This)  (This)->lpVtbl->IsDirty(This)
#define IPersistMoniker_Load(This,fFullyAvailable,pimkName,pibc,grfMode)  (This)->lpVtbl->Load(This,fFullyAvailable,pimkName,pibc,grfMode)
#define IPersistMoniker_Save(This,pimkName,pbc,fRemember)  (This)->lpVtbl->Save(This,pimkName,pbc,fRemember)
#define IPersistMoniker_SaveCompleted(This,pimkName,pibc)  (This)->lpVtbl->SaveCompleted(This,pimkName,pibc)
#define IPersistMoniker_GetCurMoniker(This,ppimkName)  (This)->lpVtbl->GetCurMoniker(This,ppimkName)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IPersistMoniker_GetClassID_Proxy(IPersistMoniker*,CLSID*);
void __RPC_STUB IPersistMoniker_GetClassID_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IPersistMoniker_IsDirty_Proxy(IPersistMoniker*);
void __RPC_STUB IPersistMoniker_IsDirty_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IPersistMoniker_Load_Proxy(IPersistMoniker*,BOOL,IMoniker*,LPBC,DWORD);
void __RPC_STUB IPersistMoniker_Load_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IPersistMoniker_Save_Proxy(IPersistMoniker*,IMoniker*,LPBC,BOOL);
void __RPC_STUB IPersistMoniker_Save_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IPersistMoniker_SaveCompleted_Proxy(IPersistMoniker*,IMoniker*,LPBC);
void __RPC_STUB IPersistMoniker_SaveCompleted_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IPersistMoniker_GetCurMoniker_Proxy(IPersistMoniker*,IMoniker**);
void __RPC_STUB IPersistMoniker_GetCurMoniker_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IPersistMoniker_INTERFACE_DEFINED__ */
#endif


#ifndef _LPBINDPROTOCOL_DEFINED
#define _LPBINDPROTOCOL_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0168_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0168_v0_0_s_ifspec;

#ifndef __IBindProtocol_INTERFACE_DEFINED__
#define __IBindProtocol_INTERFACE_DEFINED__

typedef IBindProtocol *LPBINDPROTOCOL;
EXTERN_C const IID IID_IBindProtocol;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9cd-baf9-11ce-8c82-00aa004ba90b")
IBindProtocol:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateBinding(LPCWSTR,IBindCtx*,IBinding**) = 0;
};
#else
typedef struct IBindProtocolVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IBindProtocol*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IBindProtocol*);
    ULONG(STDMETHODCALLTYPE *Release)(IBindProtocol*);
    HRESULT(STDMETHODCALLTYPE *CreateBinding)(IBindProtocol*,LPCWSTR,IBindCtx*,IBinding**);
    END_INTERFACE
} IBindProtocolVtbl;

interface IBindProtocol {
    CONST_VTBL struct IBindProtocolVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBindProtocol_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IBindProtocol_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IBindProtocol_Release(This)  (This)->lpVtbl->Release(This)
#define IBindProtocol_CreateBinding(This,szUrl,pbc,ppb)  (This)->lpVtbl->CreateBinding(This,szUrl,pbc,ppb)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IBindProtocol_CreateBinding_Proxy(IBindProtocol*,LPCWSTR,IBindCtx*,IBinding**);
void __RPC_STUB IBindProtocol_CreateBinding_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IBindProtocol_INTERFACE_DEFINED__ */
#endif


#ifndef _LPBINDING_DEFINED
#define _LPBINDING_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0169_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0169_v0_0_s_ifspec;

#ifndef __IBinding_INTERFACE_DEFINED__
#define __IBinding_INTERFACE_DEFINED__

typedef IBinding *LPBINDING;
EXTERN_C const IID IID_IBinding;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9c0-baf9-11ce-8c82-00aa004ba90b")
IBinding:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Abort(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Suspend(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Resume(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPriority(LONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPriority(LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBindResult(CLSID*,DWORD*,LPOLESTR*,DWORD*) = 0;
};
#else
typedef struct IBindingVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IBinding*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IBinding*);
    ULONG(STDMETHODCALLTYPE *Release)(IBinding*);
    HRESULT(STDMETHODCALLTYPE *Abort)(IBinding*);
    HRESULT(STDMETHODCALLTYPE *Suspend)(IBinding*);
    HRESULT(STDMETHODCALLTYPE *Resume)(IBinding*);
    HRESULT(STDMETHODCALLTYPE *SetPriority)(IBinding*,LONG);
    HRESULT(STDMETHODCALLTYPE *GetPriority)(IBinding*,LONG*);
    HRESULT(STDMETHODCALLTYPE *GetBindResult)(IBinding*,CLSID*,DWORD*,LPOLESTR*,DWORD*);
    END_INTERFACE
} IBindingVtbl;

interface IBinding {
    CONST_VTBL struct IBindingVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBinding_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IBinding_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IBinding_Release(This)  (This)->lpVtbl->Release(This)
#define IBinding_Abort(This)  (This)->lpVtbl->Abort(This)
#define IBinding_Suspend(This)  (This)->lpVtbl->Suspend(This)
#define IBinding_Resume(This)  (This)->lpVtbl->Resume(This)
#define IBinding_SetPriority(This,nPriority)  (This)->lpVtbl->SetPriority(This,nPriority)
#define IBinding_GetPriority(This,pnPriority)  (This)->lpVtbl->GetPriority(This,pnPriority)
#define IBinding_GetBindResult(This,pclsidProtocol,pdwResult,pszResult,pdwReserved)  (This)->lpVtbl->GetBindResult(This,pclsidProtocol,pdwResult,pszResult,pdwReserved)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IBinding_Abort_Proxy(IBinding*);
void __RPC_STUB IBinding_Abort_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBinding_Suspend_Proxy(IBinding*);
void __RPC_STUB IBinding_Suspend_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBinding_Resume_Proxy(IBinding*);
void __RPC_STUB IBinding_Resume_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBinding_SetPriority_Proxy(IBinding*,LONG);
void __RPC_STUB IBinding_SetPriority_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBinding_GetPriority_Proxy(IBinding*,LONG*);
void __RPC_STUB IBinding_GetPriority_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBinding_RemoteGetBindResult_Proxy(IBinding*,CLSID*,DWORD*,LPOLESTR*,DWORD);
void __RPC_STUB IBinding_RemoteGetBindResult_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IBinding_INTERFACE_DEFINED__ */
#endif

#ifndef _LPBINDSTATUSCALLBACK_DEFINED
#define _LPBINDSTATUSCALLBACK_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0170_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0170_v0_0_s_ifspec;

#ifndef __IBindStatusCallback_INTERFACE_DEFINED__
#define __IBindStatusCallback_INTERFACE_DEFINED__

typedef IBindStatusCallback *LPBINDSTATUSCALLBACK;

typedef enum __MIDL_IBindStatusCallback_0001 {
    BINDVERB_GET = 0,
    BINDVERB_POST = 0x1,
    BINDVERB_PUT = 0x2,
    BINDVERB_CUSTOM = 0x3
} BINDVERB;

typedef enum __MIDL_IBindStatusCallback_0002 {
    BINDINFOF_URLENCODESTGMEDDATA = 0x1,
    BINDINFOF_URLENCODEDEXTRAINFO = 0x2
} BINDINFOF;

typedef enum __MIDL_IBindStatusCallback_0003 {
    BINDF_ASYNCHRONOUS = 0x1,
    BINDF_ASYNCSTORAGE = 0x2,
    BINDF_NOPROGRESSIVERENDERING = 0x4,
    BINDF_OFFLINEOPERATION = 0x8,
    BINDF_GETNEWESTVERSION = 0x10,
    BINDF_NOWRITECACHE = 0x20,
    BINDF_NEEDFILE = 0x40,
    BINDF_PULLDATA = 0x80,
    BINDF_IGNORESECURITYPROBLEM = 0x100,
    BINDF_RESYNCHRONIZE = 0x200,
    BINDF_HYPERLINK = 0x400,
    BINDF_NO_UI = 0x800,
    BINDF_SILENTOPERATION = 0x1000,
    BINDF_PRAGMA_NO_CACHE = 0x2000,
    BINDF_GETCLASSOBJECT = 0x4000,
    BINDF_RESERVED_1 = 0x8000,
    BINDF_FREE_THREADED = 0x10000,
    BINDF_DIRECT_READ = 0x20000,
    BINDF_FORMS_SUBMIT = 0x40000,
    BINDF_GETFROMCACHE_IF_NET_FAIL = 0x80000,
    BINDF_FROMURLMON = 0x100000,
    BINDF_FWD_BACK = 0x200000,
    BINDF_RESERVED_2 = 0x400000,
    BINDF_RESERVED_3 = 0x800000
} BINDF;

typedef enum __MIDL_IBindStatusCallback_0004 {
    URL_ENCODING_NONE = 0,
    URL_ENCODING_ENABLE_UTF8 = 0x10000000,
    URL_ENCODING_DISABLE_UTF8 = 0x20000000
} URL_ENCODING;

typedef struct _tagBINDINFO {
    ULONG cbSize;
    LPWSTR szExtraInfo;
    STGMEDIUM stgmedData;
    DWORD grfBindInfoF;
    DWORD dwBindVerb;
    LPWSTR szCustomVerb;
    DWORD cbstgmedData;
    DWORD dwOptions;
    DWORD dwOptionsFlags;
    DWORD dwCodePage;
    SECURITY_ATTRIBUTES securityAttributes;
    IID iid;
    IUnknown *pUnk;
    DWORD dwReserved;
} BINDINFO;

typedef struct _REMSECURITY_ATTRIBUTES {
    DWORD nLength;
    DWORD lpSecurityDescriptor;
    BOOL bInheritHandle;
} REMSECURITY_ATTRIBUTES,*PREMSECURITY_ATTRIBUTES,*LPREMSECURITY_ATTRIBUTES;

typedef struct _tagRemBINDINFO {
    ULONG cbSize;
    LPWSTR szExtraInfo;
    DWORD grfBindInfoF;
    DWORD dwBindVerb;
    LPWSTR szCustomVerb;
    DWORD cbstgmedData;
    DWORD dwOptions;
    DWORD dwOptionsFlags;
    DWORD dwCodePage;
    REMSECURITY_ATTRIBUTES securityAttributes;
    IID iid;
    IUnknown *pUnk;
    DWORD dwReserved;
} RemBINDINFO;

typedef struct tagRemFORMATETC {
    DWORD cfFormat;
    DWORD ptd;
    DWORD dwAspect;
    LONG lindex;
    DWORD tymed;
} RemFORMATETC,*LPREMFORMATETC;

typedef enum __MIDL_IBindStatusCallback_0005 {
    BINDINFO_OPTIONS_WININETFLAG = 0x10000,
    BINDINFO_OPTIONS_ENABLE_UTF8 = 0x20000,
    BINDINFO_OPTIONS_DISABLE_UTF8 = 0x40000,
    BINDINFO_OPTIONS_USE_IE_ENCODING = 0x80000,
    BINDINFO_OPTIONS_BINDTOOBJECT = 0x100000
} BINDINFO_OPTIONS;

typedef enum __MIDL_IBindStatusCallback_0006 {
    BSCF_FIRSTDATANOTIFICATION = 0x1,
    BSCF_INTERMEDIATEDATANOTIFICATION = 0x2,
    BSCF_LASTDATANOTIFICATION = 0x4,
    BSCF_DATAFULLYAVAILABLE = 0x8,
    BSCF_AVAILABLEDATASIZEUNKNOWN = 0x10
} BSCF;

typedef enum tagBINDSTATUS {
    BINDSTATUS_FINDINGRESOURCE = 1,
    BINDSTATUS_CONNECTING = BINDSTATUS_FINDINGRESOURCE+1,
    BINDSTATUS_REDIRECTING = BINDSTATUS_CONNECTING+1,
    BINDSTATUS_BEGINDOWNLOADDATA = BINDSTATUS_REDIRECTING+1,
    BINDSTATUS_DOWNLOADINGDATA = BINDSTATUS_BEGINDOWNLOADDATA+1,
    BINDSTATUS_ENDDOWNLOADDATA = BINDSTATUS_DOWNLOADINGDATA+1,
    BINDSTATUS_BEGINDOWNLOADCOMPONENTS = BINDSTATUS_ENDDOWNLOADDATA+1,
    BINDSTATUS_INSTALLINGCOMPONENTS = BINDSTATUS_BEGINDOWNLOADCOMPONENTS+1,
    BINDSTATUS_ENDDOWNLOADCOMPONENTS = BINDSTATUS_INSTALLINGCOMPONENTS+1,
    BINDSTATUS_USINGCACHEDCOPY = BINDSTATUS_ENDDOWNLOADCOMPONENTS+1,
    BINDSTATUS_SENDINGREQUEST = BINDSTATUS_USINGCACHEDCOPY+1,
    BINDSTATUS_CLASSIDAVAILABLE = BINDSTATUS_SENDINGREQUEST+1,
    BINDSTATUS_MIMETYPEAVAILABLE = BINDSTATUS_CLASSIDAVAILABLE+1,
    BINDSTATUS_CACHEFILENAMEAVAILABLE = BINDSTATUS_MIMETYPEAVAILABLE+1,
    BINDSTATUS_BEGINSYNCOPERATION = BINDSTATUS_CACHEFILENAMEAVAILABLE+1,
    BINDSTATUS_ENDSYNCOPERATION = BINDSTATUS_BEGINSYNCOPERATION+1,
    BINDSTATUS_BEGINUPLOADDATA = BINDSTATUS_ENDSYNCOPERATION+1,
    BINDSTATUS_UPLOADINGDATA = BINDSTATUS_BEGINUPLOADDATA+1,
    BINDSTATUS_ENDUPLOADDATA = BINDSTATUS_UPLOADINGDATA+1,
    BINDSTATUS_PROTOCOLCLASSID = BINDSTATUS_ENDUPLOADDATA+1,
    BINDSTATUS_ENCODING = BINDSTATUS_PROTOCOLCLASSID+1,
    BINDSTATUS_VERIFIEDMIMETYPEAVAILABLE = BINDSTATUS_ENCODING+1,
    BINDSTATUS_CLASSINSTALLLOCATION = BINDSTATUS_VERIFIEDMIMETYPEAVAILABLE+1,
    BINDSTATUS_DECODING = BINDSTATUS_CLASSINSTALLLOCATION+1,
    BINDSTATUS_LOADINGMIMEHANDLER = BINDSTATUS_DECODING+1,
    BINDSTATUS_CONTENTDISPOSITIONATTACH = BINDSTATUS_LOADINGMIMEHANDLER+1,
    BINDSTATUS_FILTERREPORTMIMETYPE = BINDSTATUS_CONTENTDISPOSITIONATTACH+1,
    BINDSTATUS_CLSIDCANINSTANTIATE = BINDSTATUS_FILTERREPORTMIMETYPE+1,
    BINDSTATUS_IUNKNOWNAVAILABLE = BINDSTATUS_CLSIDCANINSTANTIATE+1,
    BINDSTATUS_DIRECTBIND = BINDSTATUS_IUNKNOWNAVAILABLE+1,
    BINDSTATUS_RAWMIMETYPE = BINDSTATUS_DIRECTBIND+1,
    BINDSTATUS_PROXYDETECTING = BINDSTATUS_RAWMIMETYPE+1,
    BINDSTATUS_ACCEPTRANGES = BINDSTATUS_PROXYDETECTING+1
} BINDSTATUS;

EXTERN_C const IID IID_IBindStatusCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9c1-baf9-11ce-8c82-00aa004ba90b")
IBindStatusCallback:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnStartBinding(DWORD,IBinding*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPriority(LONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnLowResource(DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnProgress(ULONG,ULONG,ULONG,LPCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnStopBinding(HRESULT,LPCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD*,BINDINFO*) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnDataAvailable(DWORD,DWORD,FORMATETC*,STGMEDIUM*) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnObjectAvailable(REFIID,IUnknown*) = 0;
};
#else
typedef struct IBindStatusCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IBindStatusCallback*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IBindStatusCallback*);
    ULONG(STDMETHODCALLTYPE *Release)(IBindStatusCallback*);
    HRESULT(STDMETHODCALLTYPE *OnStartBinding)(IBindStatusCallback*,DWORD,IBinding*);
    HRESULT(STDMETHODCALLTYPE *GetPriority)(IBindStatusCallback*,LONG*);
    HRESULT(STDMETHODCALLTYPE *OnLowResource)(IBindStatusCallback*,DWORD);
    HRESULT(STDMETHODCALLTYPE *OnProgress)(IBindStatusCallback*,ULONG,ULONG,ULONG,LPCWSTR);
    HRESULT(STDMETHODCALLTYPE *OnStopBinding)(IBindStatusCallback*,HRESULT,LPCWSTR);
    HRESULT(STDMETHODCALLTYPE *GetBindInfo)(IBindStatusCallback*,DWORD*,BINDINFO*);
    HRESULT(STDMETHODCALLTYPE *OnDataAvailable)(IBindStatusCallback*,DWORD,DWORD,FORMATETC*,STGMEDIUM*);
    HRESULT(STDMETHODCALLTYPE *OnObjectAvailable)(IBindStatusCallback*,REFIID,IUnknown*);
    END_INTERFACE
} IBindStatusCallbackVtbl;

interface IBindStatusCallback {
    CONST_VTBL struct IBindStatusCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBindStatusCallback_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IBindStatusCallback_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IBindStatusCallback_Release(This)  (This)->lpVtbl->Release(This)
#define IBindStatusCallback_OnStartBinding(This,dwReserved,pib)  (This)->lpVtbl->OnStartBinding(This,dwReserved,pib)
#define IBindStatusCallback_GetPriority(This,pnPriority)  (This)->lpVtbl->GetPriority(This,pnPriority)
#define IBindStatusCallback_OnLowResource(This,reserved)  (This)->lpVtbl->OnLowResource(This,reserved)
#define IBindStatusCallback_OnProgress(This,ulProgress,ulProgressMax,ulStatusCode,szStatusText)  (This)->lpVtbl->OnProgress(This,ulProgress,ulProgressMax,ulStatusCode,szStatusText)
#define IBindStatusCallback_OnStopBinding(This,hresult,szError)  (This)->lpVtbl->OnStopBinding(This,hresult,szError)
#define IBindStatusCallback_GetBindInfo(This,grfBINDF,pbindinfo)  (This)->lpVtbl->GetBindInfo(This,grfBINDF,pbindinfo)
#define IBindStatusCallback_OnDataAvailable(This,grfBSCF,dwSize,pformatetc,pstgmed)  (This)->lpVtbl->OnDataAvailable(This,grfBSCF,dwSize,pformatetc,pstgmed)
#define IBindStatusCallback_OnObjectAvailable(This,riid,punk)  (This)->lpVtbl->OnObjectAvailable(This,riid,punk)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnStartBinding_Proxy(IBindStatusCallback*,DWORD,IBinding*);
void __RPC_STUB IBindStatusCallback_OnStartBinding_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBindStatusCallback_GetPriority_Proxy(IBindStatusCallback*,LONG*);
void __RPC_STUB IBindStatusCallback_GetPriority_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnLowResource_Proxy(IBindStatusCallback*,DWORD);
void __RPC_STUB IBindStatusCallback_OnLowResource_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnProgress_Proxy(IBindStatusCallback*,ULONG,ULONG,ULONG,LPCWSTR);
void __RPC_STUB IBindStatusCallback_OnProgress_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnStopBinding_Proxy(IBindStatusCallback*,HRESULT,LPCWSTR);
void __RPC_STUB IBindStatusCallback_OnStopBinding_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBindStatusCallback_RemoteGetBindInfo_Proxy(IBindStatusCallback*,DWORD*,RemBINDINFO*,RemSTGMEDIUM*);
void __RPC_STUB IBindStatusCallback_RemoteGetBindInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBindStatusCallback_RemoteOnDataAvailable_Proxy(IBindStatusCallback*,DWORD,DWORD,RemFORMATETC*,RemSTGMEDIUM*);
void __RPC_STUB IBindStatusCallback_RemoteOnDataAvailable_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnObjectAvailable_Proxy(IBindStatusCallback *,REFIID,IUnknown*);
void __RPC_STUB IBindStatusCallback_OnObjectAvailable_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IBindStatusCallback_INTERFACE_DEFINED__ */
#endif


#ifndef _LPAUTHENTICATION_DEFINED
#define _LPAUTHENTICATION_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0171_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0171_v0_0_s_ifspec;

#ifndef __IAuthenticate_INTERFACE_DEFINED__
#define __IAuthenticate_INTERFACE_DEFINED__

typedef IAuthenticate *LPAUTHENTICATION;
EXTERN_C const IID IID_IAuthenticate;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9d0-baf9-11ce-8c82-00aa004ba90b")
IAuthenticate:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Authenticate(HWND*,LPWSTR * pszUsername,LPWSTR * pszPassword) = 0;
};
#else
typedef struct IAuthenticateVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IAuthenticate*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IAuthenticate*);
    ULONG(STDMETHODCALLTYPE *Release)(IAuthenticate*);
    HRESULT(STDMETHODCALLTYPE *Authenticate)(IAuthenticate*,HWND*,LPWSTR*,LPWSTR*);
    END_INTERFACE
} IAuthenticateVtbl;

interface IAuthenticate {
    CONST_VTBL struct IAuthenticateVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAuthenticate_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAuthenticate_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAuthenticate_Release(This)  (This)->lpVtbl->Release(This)
#define IAuthenticate_Authenticate(This,phwnd,pszUsername,pszPassword)  (This)->lpVtbl->Authenticate(This,phwnd,pszUsername,pszPassword)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IAuthenticate_Authenticate_Proxy(IAuthenticate*,HWND*,LPWSTR*,LPWSTR*);
void __RPC_STUB IAuthenticate_Authenticate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IAuthenticate_INTERFACE_DEFINED__ */
#endif


#ifndef _LPHTTPNEGOTIATE_DEFINED
#define _LPHTTPNEGOTIATE_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0172_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0172_v0_0_s_ifspec;

#ifndef __IHttpNegotiate_INTERFACE_DEFINED__
#define __IHttpNegotiate_INTERFACE_DEFINED__

typedef IHttpNegotiate *LPHTTPNEGOTIATE;
EXTERN_C const IID IID_IHttpNegotiate;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9d2-baf9-11ce-8c82-00aa004ba90b")
IHttpNegotiate:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE BeginningTransaction(LPCWSTR,LPCWSTR,DWORD,LPWSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnResponse(DWORD,LPCWSTR,LPCWSTR,LPWSTR*) = 0;
};
#else
typedef struct IHttpNegotiateVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IHttpNegotiate*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IHttpNegotiate*);
    ULONG(STDMETHODCALLTYPE *Release)(IHttpNegotiate*);
    HRESULT(STDMETHODCALLTYPE *BeginningTransaction)(IHttpNegotiate*,LPCWSTR,LPCWSTR,DWORD,LPWSTR*);
    HRESULT(STDMETHODCALLTYPE *OnResponse)(IHttpNegotiate*,DWORD,LPCWSTR,LPCWSTR,LPWSTR*);
    END_INTERFACE
} IHttpNegotiateVtbl;

interface IHttpNegotiate {
    CONST_VTBL struct IHttpNegotiateVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IHttpNegotiate_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IHttpNegotiate_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IHttpNegotiate_Release(This)  (This)->lpVtbl->Release(This)
#define IHttpNegotiate_BeginningTransaction(This,szURL,szHeaders,dwReserved,pszAdditionalHeaders)  (This)->lpVtbl->BeginningTransaction(This,szURL,szHeaders,dwReserved,pszAdditionalHeaders)
#define IHttpNegotiate_OnResponse(This,dwResponseCode,szResponseHeaders,szRequestHeaders,pszAdditionalRequestHeaders)  (This)->lpVtbl->OnResponse(This,dwResponseCode,szResponseHeaders,szRequestHeaders,pszAdditionalRequestHeaders)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IHttpNegotiate_BeginningTransaction_Proxy(IHttpNegotiate*,LPCWSTR,LPCWSTR,DWORD,LPWSTR*);
void __RPC_STUB IHttpNegotiate_BeginningTransaction_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IHttpNegotiate_OnResponse_Proxy(IHttpNegotiate*,DWORD,LPCWSTR,LPCWSTR,LPWSTR*);
void __RPC_STUB IHttpNegotiate_OnResponse_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IHttpNegotiate_INTERFACE_DEFINED__ */
#endif


#ifndef _LPWINDOWFORBINDINGUI_DEFINED
#define _LPWINDOWFORBINDINGUI_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0173_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0173_v0_0_s_ifspec;

#ifndef __IWindowForBindingUI_INTERFACE_DEFINED__
#define __IWindowForBindingUI_INTERFACE_DEFINED__

typedef IWindowForBindingUI *LPWINDOWFORBINDINGUI;
EXTERN_C const IID IID_IWindowForBindingUI;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9d5-bafa-11ce-8c82-00aa004ba90b")
IWindowForBindingUI:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetWindow(REFGUID,HWND*) = 0;
};
#else
typedef struct IWindowForBindingUIVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IWindowForBindingUI*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWindowForBindingUI*);
    ULONG(STDMETHODCALLTYPE *Release)(IWindowForBindingUI*);
    HRESULT(STDMETHODCALLTYPE *GetWindow)(IWindowForBindingUI*,REFGUID,HWND*);
    END_INTERFACE
} IWindowForBindingUIVtbl;

interface IWindowForBindingUI {
    CONST_VTBL struct IWindowForBindingUIVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWindowForBindingUI_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWindowForBindingUI_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IWindowForBindingUI_Release(This)  (This)->lpVtbl->Release(This)
#define IWindowForBindingUI_GetWindow(This,rguidReason,phwnd)  (This)->lpVtbl->GetWindow(This,rguidReason,phwnd)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWindowForBindingUI_GetWindow_Proxy(IWindowForBindingUI*,REFGUID,HWND*);
void __RPC_STUB IWindowForBindingUI_GetWindow_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IWindowForBindingUI_INTERFACE_DEFINED__ */
#endif


#ifndef _LPCODEINSTALL_DEFINED
#define _LPCODEINSTALL_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0174_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0174_v0_0_s_ifspec;

#ifndef __ICodeInstall_INTERFACE_DEFINED__
#define __ICodeInstall_INTERFACE_DEFINED__

typedef ICodeInstall *LPCODEINSTALL;

typedef enum __MIDL_ICodeInstall_0001 {
    CIP_DISK_FULL = 0,
    CIP_ACCESS_DENIED = CIP_DISK_FULL+1,
    CIP_NEWER_VERSION_EXISTS = CIP_ACCESS_DENIED+1,
    CIP_OLDER_VERSION_EXISTS = CIP_NEWER_VERSION_EXISTS+1,
    CIP_NAME_CONFLICT = CIP_OLDER_VERSION_EXISTS+1,
    CIP_TRUST_VERIFICATION_COMPONENT_MISSING = CIP_NAME_CONFLICT+1,
    CIP_EXE_SELF_REGISTERATION_TIMEOUT = CIP_TRUST_VERIFICATION_COMPONENT_MISSING+1,
    CIP_UNSAFE_TO_ABORT = CIP_EXE_SELF_REGISTERATION_TIMEOUT+1,
    CIP_NEED_REBOOT = CIP_UNSAFE_TO_ABORT+1
} CIP_STATUS;

EXTERN_C const IID IID_ICodeInstall;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9d1-baf9-11ce-8c82-00aa004ba90b")
ICodeInstall:public IWindowForBindingUI
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnCodeInstallProblem(ULONG,LPCWSTR,LPCWSTR,DWORD) = 0;
};
#else
typedef struct ICodeInstallVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICodeInstall*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICodeInstall*);
    ULONG(STDMETHODCALLTYPE *Release)(ICodeInstall*);
    HRESULT(STDMETHODCALLTYPE *GetWindow)(ICodeInstall*,REFGUID,HWND*);
    HRESULT(STDMETHODCALLTYPE *OnCodeInstallProblem)(ICodeInstall*,ULONG,LPCWSTR,LPCWSTR,DWORD);
    END_INTERFACE
} ICodeInstallVtbl;

interface ICodeInstall {
    CONST_VTBL struct ICodeInstallVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICodeInstall_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICodeInstall_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICodeInstall_Release(This)  (This)->lpVtbl->Release(This)
#define ICodeInstall_GetWindow(This,rguidReason,phwnd)  (This)->lpVtbl->GetWindow(This,rguidReason,phwnd)
#define ICodeInstall_OnCodeInstallProblem(This,ulStatusCode,szDestination,szSource,dwReserved)  (This)->lpVtbl->OnCodeInstallProblem(This,ulStatusCode,szDestination,szSource,dwReserved)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE ICodeInstall_OnCodeInstallProblem_Proxy(ICodeInstall*,ULONG,LPCWSTR,LPCWSTR,DWORD);
void __RPC_STUB ICodeInstall_OnCodeInstallProblem_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __ICodeInstall_INTERFACE_DEFINED__ */
#endif

#if (_WIN32_IE >= _WIN32_IE_IE70)
#ifndef _LPUri_DEFINED
#define _LPUri_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0014_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0014_v0_0_s_ifspec;

#ifndef __IUri_INTERFACE_DEFINED__
#define __IUri_INTERFACE_DEFINED__

typedef enum __MIDL_IUri_0001 {
    Uri_PROPERTY_ABSOLUTE_URI = 0,
    Uri_PROPERTY_STRING_START = Uri_PROPERTY_ABSOLUTE_URI,
    Uri_PROPERTY_AUTHORITY = 1,
    Uri_PROPERTY_DISPLAY_URI = 2,
    Uri_PROPERTY_DOMAIN = 3,
    Uri_PROPERTY_EXTENSION = 4,
    Uri_PROPERTY_FRAGMENT = 5,
    Uri_PROPERTY_HOST = 6,
    Uri_PROPERTY_PASSWORD = 7,
    Uri_PROPERTY_PATH = 8,
    Uri_PROPERTY_PATH_AND_QUERY = 9,
    Uri_PROPERTY_QUERY = 10,
    Uri_PROPERTY_RAW_URI = 11,
    Uri_PROPERTY_SCHEME_NAME = 12,
    Uri_PROPERTY_USER_INFO = 13,
    Uri_PROPERTY_USER_NAME = 14,
    Uri_PROPERTY_STRING_LAST = Uri_PROPERTY_USER_NAME,
    Uri_PROPERTY_HOST_TYPE = 15,
    Uri_PROPERTY_DWORD_START = Uri_PROPERTY_HOST_TYPE,
    Uri_PROPERTY_PORT = 16,
    Uri_PROPERTY_SCHEME = 17,
    Uri_PROPERTY_ZONE = 18,
    Uri_PROPERTY_DWORD_LAST = Uri_PROPERTY_ZONE
} Uri_PROPERTY;

typedef enum __MIDL_IUri_0002 {
    Uri_HOST_UNKNOWN = 0,
    Uri_HOST_DNS = (Uri_HOST_UNKNOWN + 1),
    Uri_HOST_IPV4 = (Uri_HOST_DNS + 1),
    Uri_HOST_IPV6 = (Uri_HOST_IPV4 + 1),
    Uri_HOST_IDN = (Uri_HOST_IPV6 + 1)
} Uri_HOST_TYPE;


EXTERN_C const IID IID_IUri;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A39EE748-6A27-4817-A6F2-13914BEF5890")IUri:public IUnknown
{
  public:
    virtual HRESULT STDMETHODCALLTYPE GetPropertyBSTR(Uri_PROPERTY uriProp, BSTR * pbstrProperty, DWORD dwFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyLength(Uri_PROPERTY uriProp, DWORD *pcchProperty, DWORD dwFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyDWORD(Uri_PROPERTY uriProp, DWORD *pdwProperty, DWORD dwFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE HasProperty(Uri_PROPERTY uriProp, BOOL *pfHasProperty) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAbsoluteUri(BSTR *pbstrAbsoluteUri) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAuthority(BSTR *pbstrAuthority) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDisplayUri(BSTR *pbstrDisplayString) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDomain(BSTR *pbstrDomain) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetExtension(BSTR *pbstrExtension) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFragment(BSTR *pbstrFragment) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHost(BSTR *pbstrHost) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPassword(BSTR *pbstrPassword) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPath(BSTR *pbstrPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPathAndQuery(BSTR *pbstrPathAndQuery) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetQuery(BSTR *pbstrQuery) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRawUri(BSTR *pbstrRawUri) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSchemeName(BSTR *pbstrSchemeName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUserInfo(BSTR *pbstrUserInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUserName(BSTR *pbstrUserName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHostType(DWORD *pdwHostType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPort(DWORD *pdwPort) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetScheme(DWORD *pdwScheme) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetZone(DWORD *pdwZone) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperties(LPDWORD pdwFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsEqual(IUri *pUri, BOOL *pfEqual) = 0;
};
#else /* C style interface */
typedef struct IUriVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IUri * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IUri * This);
    ULONG (STDMETHODCALLTYPE *Release)(IUri * This);
    HRESULT (STDMETHODCALLTYPE *GetPropertyBSTR)(IUri * This, Uri_PROPERTY uriProp, BSTR * pbstrProperty, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *GetPropertyLength)(IUri * This, Uri_PROPERTY uriProp, DWORD * pcchProperty, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *GetPropertyDWORD)(IUri * This, Uri_PROPERTY uriProp, DWORD * pdwProperty, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *HasProperty)(IUri * This, Uri_PROPERTY uriProp, BOOL * pfHasProperty);
    HRESULT (STDMETHODCALLTYPE *GetAbsoluteUri)(IUri * This, BSTR * pbstrAbsoluteUri);
    HRESULT (STDMETHODCALLTYPE *GetAuthority)(IUri * This, BSTR * pbstrAuthority);
    HRESULT (STDMETHODCALLTYPE *GetDisplayUri)(IUri * This, BSTR * pbstrDisplayString);
    HRESULT (STDMETHODCALLTYPE *GetDomain)(IUri * This, BSTR * pbstrDomain);
    HRESULT (STDMETHODCALLTYPE *GetExtension)(IUri * This, BSTR * pbstrExtension);
    HRESULT (STDMETHODCALLTYPE *GetFragment)(IUri * This, BSTR * pbstrFragment);
    HRESULT (STDMETHODCALLTYPE *GetHost)(IUri * This, BSTR * pbstrHost);
    HRESULT (STDMETHODCALLTYPE *GetPassword)(IUri * This, BSTR * pbstrPassword);
    HRESULT (STDMETHODCALLTYPE *GetPath)(IUri * This, BSTR * pbstrPath);
    HRESULT (STDMETHODCALLTYPE *GetPathAndQuery)(IUri * This, BSTR * pbstrPathAndQuery);
    HRESULT (STDMETHODCALLTYPE *GetQuery)(IUri * This, BSTR * pbstrQuery);
    HRESULT (STDMETHODCALLTYPE *GetRawUri)(IUri * This, BSTR * pbstrRawUri);
    HRESULT (STDMETHODCALLTYPE *GetSchemeName)(IUri * This, BSTR * pbstrSchemeName);
    HRESULT (STDMETHODCALLTYPE *GetUserInfo)(IUri * This, BSTR * pbstrUserInfo);
    HRESULT (STDMETHODCALLTYPE *GetUserName)(IUri * This, BSTR * pbstrUserName);
    HRESULT (STDMETHODCALLTYPE *GetHostType)(IUri * This, DWORD * pdwHostType);
    HRESULT (STDMETHODCALLTYPE *GetPort)(IUri * This, DWORD * pdwPort);
    HRESULT (STDMETHODCALLTYPE *GetScheme)(IUri * This, DWORD * pdwScheme);
    HRESULT (STDMETHODCALLTYPE *GetZone)(IUri * This, DWORD * pdwZone);
    HRESULT (STDMETHODCALLTYPE *GetProperties)(IUri * This, LPDWORD pdwFlags);
    HRESULT (STDMETHODCALLTYPE *IsEqual)(IUri * This, IUri * pUri, BOOL * pfEqual);
    END_INTERFACE
} IUriVtbl;

interface IUri {
    CONST_VTBL struct IUriVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUri_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IUri_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IUri_Release(This)  ((This)->lpVtbl->Release(This))
#define IUri_GetPropertyBSTR(This,uriProp,pbstrProperty,dwFlags)  ((This)->lpVtbl->GetPropertyBSTR(This,uriProp,pbstrProperty,dwFlags))
#define IUri_GetPropertyLength(This,uriProp,pcchProperty,dwFlags)  ((This)->lpVtbl->GetPropertyLength(This,uriProp,pcchProperty,dwFlags))
#define IUri_GetPropertyDWORD(This,uriProp,pdwProperty,dwFlags)  ((This)->lpVtbl->GetPropertyDWORD(This,uriProp,pdwProperty,dwFlags))
#define IUri_HasProperty(This,uriProp,pfHasProperty)  ((This)->lpVtbl->HasProperty(This,uriProp,pfHasProperty))
#define IUri_GetAbsoluteUri(This,pbstrAbsoluteUri)  ((This)->lpVtbl->GetAbsoluteUri(This,pbstrAbsoluteUri))
#define IUri_GetAuthority(This,pbstrAuthority)  ((This)->lpVtbl->GetAuthority(This,pbstrAuthority))
#define IUri_GetDisplayUri(This,pbstrDisplayString)  ((This)->lpVtbl->GetDisplayUri(This,pbstrDisplayString))
#define IUri_GetDomain(This,pbstrDomain)  ((This)->lpVtbl->GetDomain(This,pbstrDomain))
#define IUri_GetExtension(This,pbstrExtension)  ((This)->lpVtbl->GetExtension(This,pbstrExtension))
#define IUri_GetFragment(This,pbstrFragment)  ((This)->lpVtbl->GetFragment(This,pbstrFragment))
#define IUri_GetHost(This,pbstrHost)  ((This)->lpVtbl->GetHost(This,pbstrHost))
#define IUri_GetPassword(This,pbstrPassword)  ((This)->lpVtbl->GetPassword(This,pbstrPassword))
#define IUri_GetPath(This,pbstrPath)  ((This)->lpVtbl->GetPath(This,pbstrPath))
#define IUri_GetPathAndQuery(This,pbstrPathAndQuery)  ((This)->lpVtbl->GetPathAndQuery(This,pbstrPathAndQuery))
#define IUri_GetQuery(This,pbstrQuery)  ((This)->lpVtbl->GetQuery(This,pbstrQuery))
#define IUri_GetRawUri(This,pbstrRawUri)  ((This)->lpVtbl->GetRawUri(This,pbstrRawUri))
#define IUri_GetSchemeName(This,pbstrSchemeName)  ((This)->lpVtbl->GetSchemeName(This,pbstrSchemeName))
#define IUri_GetUserInfo(This,pbstrUserInfo)  ((This)->lpVtbl->GetUserInfo(This,pbstrUserInfo))
#define IUri_GetUserName(This,pbstrUserName)  ((This)->lpVtbl->GetUserName(This,pbstrUserName))
#define IUri_GetHostType(This,pdwHostType)  ((This)->lpVtbl->GetHostType(This,pdwHostType))
#define IUri_GetPort(This,pdwPort)  ((This)->lpVtbl->GetPort(This,pdwPort))
#define IUri_GetScheme(This,pdwScheme)  ((This)->lpVtbl->GetScheme(This,pdwScheme))
#define IUri_GetZone(This,pdwZone)  ((This)->lpVtbl->GetZone(This,pdwZone))
#define IUri_GetProperties(This,pdwFlags)  ((This)->lpVtbl->GetProperties(This,pdwFlags))
#define IUri_IsEqual(This,pUri,pfEqual)  ((This)->lpVtbl->IsEqual(This,pUri,pfEqual))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IUri_INTERFACE_DEFINED__ */

STDAPI CreateUri(LPCWSTR, DWORD, DWORD_PTR, IUri **);
STDAPI CreateUriWithFragment(LPCWSTR, LPCWSTR, DWORD, DWORD_PTR, IUri **);
STDAPI CreateUriFromMultiByteString(LPCSTR, DWORD, DWORD, DWORD, DWORD_PTR, IUri **);

#define Uri_HAS_ABSOLUTE_URI    (1 << Uri_PROPERTY_ABSOLUTE_URI)
#define Uri_HAS_AUTHORITY       (1 << Uri_PROPERTY_AUTHORITY)
#define Uri_HAS_DISPLAY_URI     (1 << Uri_PROPERTY_DISPLAY_URI)
#define Uri_HAS_DOMAIN          (1 << Uri_PROPERTY_DOMAIN)
#define Uri_HAS_EXTENSION       (1 << Uri_PROPERTY_EXTENSION)
#define Uri_HAS_FRAGMENT        (1 << Uri_PROPERTY_FRAGMENT)
#define Uri_HAS_HOST            (1 << Uri_PROPERTY_HOST)
#define Uri_HAS_PASSWORD        (1 << Uri_PROPERTY_PASSWORD)
#define Uri_HAS_PATH            (1 << Uri_PROPERTY_PATH)
#define Uri_HAS_QUERY           (1 << Uri_PROPERTY_QUERY)
#define Uri_HAS_RAW_URI         (1 << Uri_PROPERTY_RAW_URI)
#define Uri_HAS_SCHEME_NAME     (1 << Uri_PROPERTY_SCHEME_NAME)
#define Uri_HAS_USER_NAME       (1 << Uri_PROPERTY_USER_NAME)
#define Uri_HAS_PATH_AND_QUERY  (1 << Uri_PROPERTY_PATH_AND_QUERY)
#define Uri_HAS_USER_INFO       (1 << Uri_PROPERTY_USER_INFO)
#define Uri_HAS_HOST_TYPE       (1 << Uri_PROPERTY_HOST_TYPE)
#define Uri_HAS_PORT            (1 << Uri_PROPERTY_PORT)
#define Uri_HAS_SCHEME          (1 << Uri_PROPERTY_SCHEME)
#define Uri_HAS_ZONE            (1 << Uri_PROPERTY_ZONE)

#define Uri_CREATE_ALLOW_RELATIVE                 0x00000001
#define Uri_CREATE_ALLOW_IMPLICIT_WILDCARD_SCHEME 0x00000002
#define Uri_CREATE_ALLOW_IMPLICIT_FILE_SCHEME     0x00000004
#define Uri_CREATE_NOFRAG                         0x00000008
#define Uri_CREATE_NO_CANONICALIZE                0x00000010
#define Uri_CREATE_CANONICALIZE                   0x00000100
#define Uri_CREATE_FILE_USE_DOS_PATH              0x00000020
#define Uri_CREATE_DECODE_EXTRA_INFO              0x00000040
#define Uri_CREATE_NO_DECODE_EXTRA_INFO           0x00000080
#define Uri_CREATE_CRACK_UNKNOWN_SCHEMES          0x00000200
#define Uri_CREATE_NO_CRACK_UNKNOWN_SCHEMES       0x00000400
#define Uri_CREATE_PRE_PROCESS_HTML_URI           0x00000800
#define Uri_CREATE_NO_PRE_PROCESS_HTML_URI        0x00001000
#define Uri_CREATE_IE_SETTINGS                    0x00002000
#define Uri_CREATE_NO_IE_SETTINGS                 0x00004000
#define Uri_CREATE_NO_ENCODE_FORBIDDEN_CHARACTERS 0x00008000

#define Uri_DISPLAY_NO_FRAGMENT               0x00000001
#define Uri_PUNYCODE_IDN_HOST                 0x00000002
#define Uri_DISPLAY_IDN_HOST                  0x00000004

#define Uri_ENCODING_USER_INFO_AND_PATH_IS_PERCENT_ENCODED_UTF8  0x00000001
#define Uri_ENCODING_USER_INFO_AND_PATH_IS_CP                    0x00000002
#define Uri_ENCODING_HOST_IS_IDN                                 0x00000004
#define Uri_ENCODING_HOST_IS_PERCENT_ENCODED_UTF8                0x00000008
#define Uri_ENCODING_HOST_IS_PERCENT_ENCODED_CP                  0x00000010
#define Uri_ENCODING_QUERY_AND_FRAGMENT_IS_PERCENT_ENCODED_UTF8  0x00000020
#define Uri_ENCODING_QUERY_AND_FRAGMENT_IS_CP                    0x00000040
#define Uri_ENCODING_RFC (Uri_ENCODING_USER_INFO_AND_PATH_IS_PERCENT_ENCODED_UTF8|Uri_ENCODING_HOST_IS_PERCENT_ENCODED_UTF8|Uri_ENCODING_QUERY_AND_FRAGMENT_IS_PERCENT_ENCODED_UTF8)

#define UriBuilder_USE_ORIGINAL_FLAGS  0x00000001

#endif /* _LPUri_DEFINED */


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0015_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0015_v0_0_s_ifspec;

#ifndef __IUriContainer_INTERFACE_DEFINED__
#define __IUriContainer_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUriContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a158a630-ed6f-45fb-b987-f68676f57752") IUriContainer : public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetIUri(IUri **ppIUri) = 0;
};
#else /* C style interface */
typedef struct IUriContainerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IUriContainer * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IUriContainer * This);
    ULONG (STDMETHODCALLTYPE * Release)(IUriContainer * This);
    HRESULT (STDMETHODCALLTYPE * GetIUri)(IUriContainer * This, IUri ** ppIUri);
    END_INTERFACE
} IUriContainerVtbl;

interface IUriContainer {
    CONST_VTBL struct IUriContainerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUriContainer_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IUriContainer_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IUriContainer_Release(This)  ((This)->lpVtbl->Release(This))
#define IUriContainer_GetIUri(This,ppIUri)  ((This)->lpVtbl->GetIUri(This,ppIUri))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IUriContainer_INTERFACE_DEFINED__ */


#ifndef __IUriBuilder_INTERFACE_DEFINED__
#define __IUriBuilder_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUriBuilder;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("4221B2E1-8955-46c0-BD5B-DE9897565DE7") IUriBuilder : public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateUriSimple(DWORD dwAllowEncodingPropertyMask, DWORD_PTR dwReserved, IUri ** ppIUri) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateUri(DWORD dwCreateFlags, DWORD dwAllowEncodingPropertyMask, DWORD_PTR dwReserved, IUri ** ppIUri) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateUriWithFlags(DWORD dwCreateFlags, DWORD dwUriBuilderFlags, DWORD dwAllowEncodingPropertyMask, DWORD_PTR dwReserved, IUri ** ppIUri) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIUri(IUri ** ppIUri) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetIUri(IUri * pIUri) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFragment(DWORD * pcchFragment, LPCWSTR * ppwzFragment) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHost(DWORD * pcchHost, LPCWSTR * ppwzHost) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPassword(DWORD * pcchPassword, LPCWSTR * ppwzPassword) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPath(DWORD * pcchPath, LPCWSTR * ppwzPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPort(BOOL * pfHasPort, DWORD * pdwPort) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetQuery(DWORD * pcchQuery, LPCWSTR * ppwzQuery) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSchemeName(DWORD * pcchSchemeName, LPCWSTR * ppwzSchemeName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUserName(DWORD * pcchUserName, LPCWSTR * ppwzUserName) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFragment(LPCWSTR pwzNewValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetHost(LPCWSTR pwzNewValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPassword(LPCWSTR pwzNewValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPath(LPCWSTR pwzNewValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPort(BOOL fHasPort, DWORD dwNewValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetQuery(LPCWSTR pwzNewValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSchemeName(LPCWSTR pwzNewValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetUserName(LPCWSTR pwzNewValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveProperties(DWORD dwPropertyMask) = 0;
    virtual HRESULT STDMETHODCALLTYPE HasBeenModified(BOOL * pfModified) = 0;
};

#else /* C style interface */
typedef struct IUriBuilderVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUriBuilder * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IUriBuilder * This);
    ULONG(STDMETHODCALLTYPE *Release)(IUriBuilder * This);
    HRESULT(STDMETHODCALLTYPE *CreateUriSimple)(IUriBuilder * This, DWORD dwAllowEncodingPropertyMask, DWORD_PTR dwReserved, IUri ** ppIUri);
    HRESULT(STDMETHODCALLTYPE *CreateUri)(IUriBuilder * This, DWORD dwCreateFlags, DWORD dwAllowEncodingPropertyMask, DWORD_PTR dwReserved, IUri ** ppIUri);
    HRESULT(STDMETHODCALLTYPE *CreateUriWithFlags)(IUriBuilder * This, DWORD dwCreateFlags, DWORD dwUriBuilderFlags, DWORD dwAllowEncodingPropertyMask, DWORD_PTR dwReserved, IUri ** ppIUri);
    HRESULT(STDMETHODCALLTYPE *GetIUri)(IUriBuilder * This, IUri ** ppIUri);
    HRESULT(STDMETHODCALLTYPE *SetIUri)(IUriBuilder * This, IUri * pIUri);
    HRESULT(STDMETHODCALLTYPE *GetFragment)(IUriBuilder * This, DWORD * pcchFragment, LPCWSTR * ppwzFragment);
    HRESULT(STDMETHODCALLTYPE *GetHost)(IUriBuilder * This, DWORD * pcchHost, LPCWSTR * ppwzHost);
    HRESULT(STDMETHODCALLTYPE *GetPassword)(IUriBuilder * This, DWORD * pcchPassword, LPCWSTR * ppwzPassword);
    HRESULT(STDMETHODCALLTYPE *GetPath)(IUriBuilder * This, DWORD * pcchPath, LPCWSTR * ppwzPath);
    HRESULT(STDMETHODCALLTYPE *GetPort)(IUriBuilder * This, BOOL * pfHasPort, DWORD * pdwPort);
    HRESULT(STDMETHODCALLTYPE *GetQuery)(IUriBuilder * This, DWORD * pcchQuery, LPCWSTR * ppwzQuery);
    HRESULT(STDMETHODCALLTYPE *GetSchemeName)(IUriBuilder * This, DWORD * pcchSchemeName, LPCWSTR * ppwzSchemeName);
    HRESULT(STDMETHODCALLTYPE *GetUserName)(IUriBuilder * This, DWORD * pcchUserName, LPCWSTR * ppwzUserName);
    HRESULT(STDMETHODCALLTYPE *SetFragment)(IUriBuilder * This, LPCWSTR pwzNewValue);
    HRESULT(STDMETHODCALLTYPE *SetHost)(IUriBuilder * This, LPCWSTR pwzNewValue);
    HRESULT(STDMETHODCALLTYPE *SetPassword)(IUriBuilder * This, LPCWSTR pwzNewValue);
    HRESULT(STDMETHODCALLTYPE *SetPath)(IUriBuilder * This, LPCWSTR pwzNewValue);
    HRESULT(STDMETHODCALLTYPE *SetPort)(IUriBuilder * This, BOOL fHasPort, DWORD dwNewValue);
    HRESULT(STDMETHODCALLTYPE *SetQuery)(IUriBuilder * This, LPCWSTR pwzNewValue);
    HRESULT(STDMETHODCALLTYPE *SetSchemeName)(IUriBuilder * This, LPCWSTR pwzNewValue);
    HRESULT(STDMETHODCALLTYPE *SetUserName)(IUriBuilder * This, LPCWSTR pwzNewValue);
    HRESULT(STDMETHODCALLTYPE *RemoveProperties)(IUriBuilder * This, DWORD dwPropertyMask);
    HRESULT(STDMETHODCALLTYPE *HasBeenModified)(IUriBuilder * This, BOOL * pfModified);
    END_INTERFACE
} IUriBuilderVtbl;

interface IUriBuilder {
    CONST_VTBL struct IUriBuilderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUriBuilder_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IUriBuilder_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IUriBuilder_Release(This)  ((This)->lpVtbl->Release(This))
#define IUriBuilder_CreateUriSimple(This,dwAllowEncodingPropertyMask,dwReserved,ppIUri)  ((This)->lpVtbl->CreateUriSimple(This,dwAllowEncodingPropertyMask,dwReserved,ppIUri))
#define IUriBuilder_CreateUri(This,dwCreateFlags,dwAllowEncodingPropertyMask,dwReserved,ppIUri)  ((This)->lpVtbl->CreateUri(This,dwCreateFlags,dwAllowEncodingPropertyMask,dwReserved,ppIUri))
#define IUriBuilder_CreateUriWithFlags(This,dwCreateFlags,dwUriBuilderFlags,dwAllowEncodingPropertyMask,dwReserved,ppIUri)  ((This)->lpVtbl->CreateUriWithFlags(This,dwCreateFlags,dwUriBuilderFlags,dwAllowEncodingPropertyMask,dwReserved,ppIUri))
#define IUriBuilder_GetIUri(This,ppIUri)  ((This)->lpVtbl->GetIUri(This,ppIUri))
#define IUriBuilder_SetIUri(This,pIUri)  ((This)->lpVtbl->SetIUri(This,pIUri))
#define IUriBuilder_GetFragment(This,pcchFragment,ppwzFragment)  ((This)->lpVtbl->GetFragment(This,pcchFragment,ppwzFragment))
#define IUriBuilder_GetHost(This,pcchHost,ppwzHost)  ((This)->lpVtbl->GetHost(This,pcchHost,ppwzHost))
#define IUriBuilder_GetPassword(This,pcchPassword,ppwzPassword)  ((This)->lpVtbl->GetPassword(This,pcchPassword,ppwzPassword))
#define IUriBuilder_GetPath(This,pcchPath,ppwzPath)  ((This)->lpVtbl->GetPath(This,pcchPath,ppwzPath))
#define IUriBuilder_GetPort(This,pfHasPort,pdwPort)  ((This)->lpVtbl->GetPort(This,pfHasPort,pdwPort))
#define IUriBuilder_GetQuery(This,pcchQuery,ppwzQuery)  ((This)->lpVtbl->GetQuery(This,pcchQuery,ppwzQuery))
#define IUriBuilder_GetSchemeName(This,pcchSchemeName,ppwzSchemeName)  ((This)->lpVtbl->GetSchemeName(This,pcchSchemeName,ppwzSchemeName))
#define IUriBuilder_GetUserName(This,pcchUserName,ppwzUserName)  ((This)->lpVtbl->GetUserName(This,pcchUserName,ppwzUserName))
#define IUriBuilder_SetFragment(This,pwzNewValue)  ((This)->lpVtbl->SetFragment(This,pwzNewValue))
#define IUriBuilder_SetHost(This,pwzNewValue)  ((This)->lpVtbl->SetHost(This,pwzNewValue))
#define IUriBuilder_SetPassword(This,pwzNewValue)  ((This)->lpVtbl->SetPassword(This,pwzNewValue))
#define IUriBuilder_SetPath(This,pwzNewValue)  ((This)->lpVtbl->SetPath(This,pwzNewValue))
#define IUriBuilder_SetPort(This,fHasPort,dwNewValue)  ((This)->lpVtbl->SetPort(This,fHasPort,dwNewValue))
#define IUriBuilder_SetQuery(This,pwzNewValue)  ((This)->lpVtbl->SetQuery(This,pwzNewValue))
#define IUriBuilder_SetSchemeName(This,pwzNewValue)  ((This)->lpVtbl->SetSchemeName(This,pwzNewValue))
#define IUriBuilder_SetUserName(This,pwzNewValue)  ((This)->lpVtbl->SetUserName(This,pwzNewValue))
#define IUriBuilder_RemoveProperties(This,dwPropertyMask)  ((This)->lpVtbl->RemoveProperties(This,dwPropertyMask))
#define IUriBuilder_HasBeenModified(This,pfModified)  ((This)->lpVtbl->HasBeenModified(This,pfModified))

#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IUriBuilder_INTERFACE_DEFINED__ */


#ifndef __IUriBuilderFactory_INTERFACE_DEFINED__
#define __IUriBuilderFactory_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUriBuilderFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("E982CE48-0B96-440c-BC37-0C869B27A29E") IUriBuilderFactory : public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateIUriBuilder(DWORD dwFlags, DWORD_PTR dwReserved, IUriBuilder ** ppIUriBuilder) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateInitializedIUriBuilder(DWORD dwFlags, DWORD_PTR dwReserved, IUriBuilder ** ppIUriBuilder) = 0;
};
#else /* C style interface */
typedef struct IUriBuilderFactoryVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IUriBuilderFactory * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IUriBuilderFactory * This);
    ULONG (STDMETHODCALLTYPE *Release)(IUriBuilderFactory * This);
    HRESULT (STDMETHODCALLTYPE *CreateIUriBuilder)(IUriBuilderFactory * This, DWORD dwFlags, DWORD_PTR dwReserved, IUriBuilder ** ppIUriBuilder);
    HRESULT (STDMETHODCALLTYPE *CreateInitializedIUriBuilder)(IUriBuilderFactory * This, DWORD dwFlags, DWORD_PTR dwReserved, IUriBuilder ** ppIUriBuilder);
    END_INTERFACE
} IUriBuilderFactoryVtbl;

interface IUriBuilderFactory {
    CONST_VTBL struct IUriBuilderFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUriBuilderFactory_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IUriBuilderFactory_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IUriBuilderFactory_Release(This)  ((This)->lpVtbl->Release(This))
#define IUriBuilderFactory_CreateIUriBuilder(This,dwFlags,dwReserved,ppIUriBuilder)  ((This)->lpVtbl->CreateIUriBuilder(This,dwFlags,dwReserved,ppIUriBuilder))
#define IUriBuilderFactory_CreateInitializedIUriBuilder(This,dwFlags,dwReserved,ppIUriBuilder)  ((This)->lpVtbl->CreateInitializedIUriBuilder(This,dwFlags,dwReserved,ppIUriBuilder))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IUriBuilderFactory_INTERFACE_DEFINED__ */

STDAPI CreateIUriBuilder(IUri *, DWORD , DWORD_PTR , IUriBuilder **);

#endif /* _WIN32_IE >= _WIN32_IE_IE70 */


#ifndef _LPWININETINFO_DEFINED
#define _LPWININETINFO_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0175_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0175_v0_0_s_ifspec;

#ifndef __IWinInetInfo_INTERFACE_DEFINED__
#define __IWinInetInfo_INTERFACE_DEFINED__

typedef IWinInetInfo *LPWININETINFO;
EXTERN_C const IID IID_IWinInetInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9d6-bafa-11ce-8c82-00aa004ba90b")
IWinInetInfo:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE QueryOption(DWORD,LPVOID,DWORD*) = 0;
};
#else
typedef struct IWinInetInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IWinInetInfo*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWinInetInfo*);
    ULONG(STDMETHODCALLTYPE *Release)(IWinInetInfo*);
    HRESULT(STDMETHODCALLTYPE *QueryOption)(IWinInetInfo*,DWORD,LPVOID,DWORD*);
    END_INTERFACE
} IWinInetInfoVtbl;

interface IWinInetInfo {
    CONST_VTBL struct IWinInetInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWinInetInfo_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWinInetInfo_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IWinInetInfo_Release(This)  (This)->lpVtbl->Release(This)
#define IWinInetInfo_QueryOption(This,dwOption,pBuffer,pcbBuf)  (This)->lpVtbl->QueryOption(This,dwOption,pBuffer,pcbBuf)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWinInetInfo_RemoteQueryOption_Proxy(IWinInetInfo*,DWORD,BYTE*,DWORD*);
void __RPC_STUB IWinInetInfo_RemoteQueryOption_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IWinInetInfo_INTERFACE_DEFINED__ */
#endif


#define WININETINFO_OPTION_LOCK_HANDLE  65534

#ifndef _LPHTTPSECURITY_DEFINED
#define _LPHTTPSECURITY_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0176_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0176_v0_0_s_ifspec;

#ifndef __IHttpSecurity_INTERFACE_DEFINED__
#define __IHttpSecurity_INTERFACE_DEFINED__

typedef IHttpSecurity *LPHTTPSECURITY;
EXTERN_C const IID IID_IHttpSecurity;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9d7-bafa-11ce-8c82-00aa004ba90b")
IHttpSecurity:public IWindowForBindingUI
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnSecurityProblem(DWORD dwProblem) = 0;
};
#else
typedef struct IHttpSecurityVtbl
{
    BEGIN_INTERFACE HRESULT(STDMETHODCALLTYPE *QueryInterface)(IHttpSecurity*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IHttpSecurity*);
    ULONG(STDMETHODCALLTYPE *Release)(IHttpSecurity*);
    HRESULT(STDMETHODCALLTYPE *GetWindow)(IHttpSecurity*,REFGUID,HWND*);
    HRESULT(STDMETHODCALLTYPE *OnSecurityProblem)(IHttpSecurity*,DWORD);
    END_INTERFACE
} IHttpSecurityVtbl;

interface IHttpSecurity {
    CONST_VTBL struct IHttpSecurityVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IHttpSecurity_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IHttpSecurity_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IHttpSecurity_Release(This)  (This)->lpVtbl->Release(This)
#define IHttpSecurity_GetWindow(This,rguidReason,phwnd)  (This)->lpVtbl->GetWindow(This,rguidReason,phwnd)
#define IHttpSecurity_OnSecurityProblem(This,dwProblem)  (This)->lpVtbl->OnSecurityProblem(This,dwProblem)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IHttpSecurity_OnSecurityProblem_Proxy(IHttpSecurity*,DWORD);
void __RPC_STUB IHttpSecurity_OnSecurityProblem_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IHttpSecurity_INTERFACE_DEFINED__ */
#endif


#ifndef _LPWININETHTTPINFO_DEFINED
#define _LPWININETHTTPINFO_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0177_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0177_v0_0_s_ifspec;

#ifndef __IWinInetHttpInfo_INTERFACE_DEFINED__
#define __IWinInetHttpInfo_INTERFACE_DEFINED__

typedef IWinInetHttpInfo *LPWININETHTTPINFO;
EXTERN_C const IID IID_IWinInetHttpInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9d8-bafa-11ce-8c82-00aa004ba90b")
IWinInetHttpInfo:public IWinInetInfo
{
    public:
    virtual HRESULT STDMETHODCALLTYPE QueryInfo(DWORD,LPVOID,DWORD*,DWORD*,DWORD*) = 0;
};
#else
typedef struct IWinInetHttpInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IWinInetHttpInfo*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWinInetHttpInfo*);
    ULONG(STDMETHODCALLTYPE *Release)(IWinInetHttpInfo*);
    HRESULT(STDMETHODCALLTYPE *QueryOption)(IWinInetHttpInfo*,DWORD,LPVOID,DWORD*);
    HRESULT(STDMETHODCALLTYPE *QueryInfo)(IWinInetHttpInfo*,DWORD,LPVOID,DWORD*,DWORD*,DWORD*);
    END_INTERFACE
} IWinInetHttpInfoVtbl;

interface IWinInetHttpInfo {
    CONST_VTBL struct IWinInetHttpInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWinInetHttpInfo_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IWinInetHttpInfo_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IWinInetHttpInfo_Release(This)  (This)->lpVtbl->Release(This)
#define IWinInetHttpInfo_QueryOption(This,dwOption,pBuffer,pcbBuf)  (This)->lpVtbl->QueryOption(This,dwOption,pBuffer,pcbBuf)
#define IWinInetHttpInfo_QueryInfo(This,dwOption,pBuffer,pcbBuf,pdwFlags,pdwReserved)  (This)->lpVtbl->QueryInfo(This,dwOption,pBuffer,pcbBuf,pdwFlags,pdwReserved)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IWinInetHttpInfo_RemoteQueryInfo_Proxy(IWinInetHttpInfo*,DWORD,BYTE*,DWORD*,DWORD*,DWORD*);
void __RPC_STUB IWinInetHttpInfo_RemoteQueryInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IWinInetHttpInfo_INTERFACE_DEFINED__ */
#endif

#if (_WIN32_IE >= _WIN32_IE_IE60SP2)

#ifndef _LPWININETCACHEHINTS_DEFINED
#define _LPWININETCACHEHINTS_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0021_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0021_v0_0_s_ifspec;

#ifndef __IWinInetCacheHints_INTERFACE_DEFINED__
#define __IWinInetCacheHints_INTERFACE_DEFINED__

typedef IWinInetCacheHints *LPWININETCACHEHINTS;

EXTERN_C const IID IID_IWinInetCacheHints;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("DD1EC3B3-8391-4fdb-A9E6-347C3CAAA7DD") IWinInetCacheHints : public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetCacheExtension(LPCWSTR pwzExt, LPVOID pszCacheFile, DWORD * pcbCacheFile, DWORD * pdwWinInetError, DWORD * pdwReserved) = 0;
};
#else /* C style interface */

typedef struct IWinInetCacheHintsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWinInetCacheHints * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWinInetCacheHints * This);
    ULONG (STDMETHODCALLTYPE *Release)(IWinInetCacheHints * This);
    HRESULT (STDMETHODCALLTYPE *SetCacheExtension)(IWinInetCacheHints * This, LPCWSTR pwzExt, LPVOID pszCacheFile, DWORD * pcbCacheFile, DWORD * pdwWinInetError, DWORD * pdwReserved);
    END_INTERFACE
} IWinInetCacheHintsVtbl;

interface IWinInetCacheHints {
    CONST_VTBL struct IWinInetCacheHintsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWinInetCacheHints_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWinInetCacheHints_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWinInetCacheHints_Release(This)  ((This)->lpVtbl->Release(This))
#define IWinInetCacheHints_SetCacheExtension(This,pwzExt,pszCacheFile,pcbCacheFile,pdwWinInetError,pdwReserved)  ((This)->lpVtbl->SetCacheExtension(This,pwzExt,pszCacheFile,pcbCacheFile,pdwWinInetError,pdwReserved))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWinInetCacheHints_INTERFACE_DEFINED__ */

#endif

#endif /*_WIN32_IE >= _WIN32_IE_IE60SP2 */

#if (_WIN32_IE >= _WIN32_IE_IE70)

#ifndef _LPWININETCACHEHINTS2_DEFINED
#define _LPWININETCACHEHINTS2_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0022_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0022_v0_0_s_ifspec;

#ifndef __IWinInetCacheHints2_INTERFACE_DEFINED__
#define __IWinInetCacheHints2_INTERFACE_DEFINED__

typedef IWinInetCacheHints2 *LPWININETCACHEHINTS2;

EXTERN_C const IID IID_IWinInetCacheHints2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7857AEAC-D31F-49bf-884E-DD46DF36780A") IWinInetCacheHints2 : public IWinInetCacheHints
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetCacheExtension2(LPCWSTR pwzExt, WCHAR * pwzCacheFile, DWORD * pcchCacheFile, DWORD * pdwWinInetError, DWORD * pdwReserved) = 0;
};
#else /* C style interface */
typedef struct IWinInetCacheHints2Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IWinInetCacheHints2 * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IWinInetCacheHints2 * This);
    ULONG (STDMETHODCALLTYPE *Release)(IWinInetCacheHints2 * This);
    HRESULT (STDMETHODCALLTYPE *SetCacheExtension)(IWinInetCacheHints2 * This, LPCWSTR pwzExt, LPVOID pszCacheFile, DWORD * pcbCacheFile, DWORD * pdwWinInetError, DWORD * pdwReserved);
    HRESULT (STDMETHODCALLTYPE *SetCacheExtension2)(IWinInetCacheHints2 * This, LPCWSTR pwzExt, WCHAR * pwzCacheFile, DWORD * pcchCacheFile, DWORD * pdwWinInetError, DWORD * pdwReserved);
    END_INTERFACE
} IWinInetCacheHints2Vtbl;

interface IWinInetCacheHints2 {
    CONST_VTBL struct IWinInetCacheHints2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWinInetCacheHints2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWinInetCacheHints2_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWinInetCacheHints2_Release(This)  ((This)->lpVtbl->Release(This))
#define IWinInetCacheHints2_SetCacheExtension(This,pwzExt,pszCacheFile,pcbCacheFile,pdwWinInetError,pdwReserved)  ((This)->lpVtbl->SetCacheExtension(This,pwzExt,pszCacheFile,pcbCacheFile,pdwWinInetError,pdwReserved))
#define IWinInetCacheHints2_SetCacheExtension2(This,pwzExt,pwzCacheFile,pcchCacheFile,pdwWinInetError,pdwReserved)  ((This)->lpVtbl->SetCacheExtension2(This,pwzExt,pwzCacheFile,pcchCacheFile,pdwWinInetError,pdwReserved))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IWinInetCacheHints2_INTERFACE_DEFINED__ */

#endif
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */


#define SID_IBindHost  IID_IBindHost
#define SID_SBindHost  IID_IBindHost

#ifndef _LPBINDHOST_DEFINED
#define _LPBINDHOST_DEFINED

EXTERN_C const GUID SID_BindHost;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0178_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0178_v0_0_s_ifspec;

#ifndef __IBindHost_INTERFACE_DEFINED__
#define __IBindHost_INTERFACE_DEFINED__

typedef IBindHost *LPBINDHOST;
EXTERN_C const IID IID_IBindHost;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("fc4801a1-2ba9-11cf-a229-00aa003d7352")
IBindHost:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateMoniker(LPOLESTR,IBindCtx*,IMoniker**,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE MonikerBindToStorage(IMoniker*,IBindCtx*,IBindStatusCallback*,REFIID,void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE MonikerBindToObject(IMoniker*,IBindCtx*,IBindStatusCallback*,REFIID,void**) = 0;
};
#else
typedef struct IBindHostVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IBindHost*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IBindHost*);
    ULONG(STDMETHODCALLTYPE *Release)(IBindHost*);
    HRESULT(STDMETHODCALLTYPE *CreateMoniker)(IBindHost*,LPOLESTR,IBindCtx*,IMoniker**,DWORD);
    HRESULT(STDMETHODCALLTYPE *MonikerBindToStorage)(IBindHost*,IMoniker*,IBindCtx*,IBindStatusCallback*,REFIID,void**ppvObj);
    HRESULT(STDMETHODCALLTYPE *MonikerBindToObject)(IBindHost*,IMoniker*,IBindCtx*,IBindStatusCallback*,REFIID,void**ppvObj);
    END_INTERFACE
} IBindHostVtbl;

interface IBindHost {
    CONST_VTBL struct IBindHostVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBindHost_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IBindHost_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IBindHost_Release(This)  (This)->lpVtbl->Release(This)
#define IBindHost_CreateMoniker(This,szName,pBC,ppmk,dwReserved)  (This)->lpVtbl->CreateMoniker(This,szName,pBC,ppmk,dwReserved)
#define IBindHost_MonikerBindToStorage(This,pMk,pBC,pBSC,riid,ppvObj)  (This)->lpVtbl->MonikerBindToStorage(This,pMk,pBC,pBSC,riid,ppvObj)
#define IBindHost_MonikerBindToObject(This,pMk,pBC,pBSC,riid,ppvObj)  (This)->lpVtbl->MonikerBindToObject(This,pMk,pBC,pBSC,riid,ppvObj)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IBindHost_CreateMoniker_Proxy(IBindHost*,LPOLESTR,IBindCtx*,IMoniker**,DWORD);
void __RPC_STUB IBindHost_CreateMoniker_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBindHost_RemoteMonikerBindToStorage_Proxy(IBindHost*,IMoniker*,IBindCtx*,IBindStatusCallback*,REFIID,IUnknown**);
void __RPC_STUB IBindHost_RemoteMonikerBindToStorage_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IBindHost_RemoteMonikerBindToObject_Proxy(IBindHost*,IMoniker*,IBindCtx*,IBindStatusCallback*,REFIID,IUnknown**);
void __RPC_STUB IBindHost_RemoteMonikerBindToObject_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IBindHost_INTERFACE_DEFINED__ */
#endif


#define URLOSTRM_USECACHEDCOPY_ONLY 0x1
#define URLOSTRM_USECACHEDCOPY  0x2
#define URLOSTRM_GETNEWESTVERSION  0x3
struct IBindStatusCallback;

STDAPI HlinkSimpleNavigateToString(LPCWSTR,LPCWSTR,LPCWSTR,IUnknown*,IBindCtx*,IBindStatusCallback*,DWORD,DWORD);
STDAPI HlinkSimpleNavigateToMoniker(IMoniker*,LPCWSTR,LPCWSTR,IUnknown*,IBindCtx*,IBindStatusCallback*,DWORD,DWORD);
STDAPI URLOpenStreamA(LPUNKNOWN,LPCSTR,DWORD,LPBINDSTATUSCALLBACK);
STDAPI URLOpenStreamW(LPUNKNOWN,LPCWSTR,DWORD,LPBINDSTATUSCALLBACK);
STDAPI URLOpenPullStreamA(LPUNKNOWN,LPCSTR,DWORD,LPBINDSTATUSCALLBACK);
STDAPI URLOpenPullStreamW(LPUNKNOWN,LPCWSTR,DWORD,LPBINDSTATUSCALLBACK);
STDAPI URLDownloadToFileA(LPUNKNOWN,LPCSTR,LPCSTR,DWORD,LPBINDSTATUSCALLBACK);
STDAPI URLDownloadToFileW(LPUNKNOWN,LPCWSTR,LPCWSTR,DWORD,LPBINDSTATUSCALLBACK);
STDAPI URLDownloadToCacheFileA(LPUNKNOWN,LPCSTR,LPTSTR,DWORD,DWORD,LPBINDSTATUSCALLBACK);
STDAPI URLDownloadToCacheFileW(LPUNKNOWN,LPCWSTR,LPWSTR,DWORD,DWORD,LPBINDSTATUSCALLBACK);
STDAPI URLOpenBlockingStreamA(LPUNKNOWN,LPCSTR,LPSTREAM*,DWORD,LPBINDSTATUSCALLBACK);
STDAPI URLOpenBlockingStreamW(LPUNKNOWN,LPCWSTR,LPSTREAM*,DWORD,LPBINDSTATUSCALLBACK);
#ifdef UNICODE
#define URLOpenStream  URLOpenStreamW
#define URLOpenPullStream  URLOpenPullStreamW
#define URLDownloadToFile  URLDownloadToFileW
#define URLDownloadToCacheFile  URLDownloadToCacheFileW
#define URLOpenBlockingStream  URLOpenBlockingStreamW
#else
#define URLOpenStream  URLOpenStreamA
#define URLOpenPullStream  URLOpenPullStreamA
#define URLDownloadToFile  URLDownloadToFileA
#define URLDownloadToCacheFile  URLDownloadToCacheFileA
#define URLOpenBlockingStream  URLOpenBlockingStreamA
#endif /* !UNICODE */

STDAPI HlinkGoBack(IUnknown*);
STDAPI HlinkGoForward(IUnknown*);
STDAPI HlinkNavigateString(IUnknown*,LPCWSTR);
STDAPI HlinkNavigateMoniker(IUnknown*,IMoniker*);

#ifndef _URLMON_NO_ASYNC_PLUGABLE_PROTOCOLS_
#ifndef _LPIINTERNET
#define _LPIINTERNET

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0179_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0179_v0_0_s_ifspec;

#ifndef __IInternet_INTERFACE_DEFINED__
#define __IInternet_INTERFACE_DEFINED__

typedef IInternet *LPIINTERNET;
EXTERN_C const IID IID_IInternet;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9e0-baf9-11ce-8c82-00aa004ba90b")
IInternet:public IUnknown
{
    public:
};
#else
typedef struct IInternetVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternet*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternet*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternet*);
    END_INTERFACE
} IInternetVtbl;

interface IInternet {
    CONST_VTBL struct IInternetVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternet_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternet_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternet_Release(This)  (This)->lpVtbl->Release(This)
#endif /* COBJMACROS */
#endif /* C style interface */
#endif /* __IInternet_INTERFACE_DEFINED__ */
#endif


#ifndef _LPIINTERNETBINDINFO
#define _LPIINTERNETBINDINFO

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0180_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0180_v0_0_s_ifspec;

#ifndef __IInternetBindInfo_INTERFACE_DEFINED__
#define __IInternetBindInfo_INTERFACE_DEFINED__

typedef IInternetBindInfo *LPIINTERNETBINDINFO;

typedef enum tagBINDSTRING {
    BINDSTRING_HEADERS = 1,
    BINDSTRING_ACCEPT_MIMES = BINDSTRING_HEADERS+1,
    BINDSTRING_EXTRA_URL = BINDSTRING_ACCEPT_MIMES+1,
    BINDSTRING_LANGUAGE = BINDSTRING_EXTRA_URL+1,
    BINDSTRING_USERNAME = BINDSTRING_LANGUAGE+1,
    BINDSTRING_PASSWORD = BINDSTRING_USERNAME+1,
    BINDSTRING_UA_PIXELS = BINDSTRING_PASSWORD+1,
    BINDSTRING_UA_COLOR = BINDSTRING_UA_PIXELS+1,
    BINDSTRING_OS = BINDSTRING_UA_COLOR+1,
    BINDSTRING_USER_AGENT = BINDSTRING_OS+1,
    BINDSTRING_ACCEPT_ENCODINGS = BINDSTRING_USER_AGENT+1,
    BINDSTRING_POST_COOKIE = BINDSTRING_ACCEPT_ENCODINGS+1,
    BINDSTRING_POST_DATA_MIME = BINDSTRING_POST_COOKIE+1,
    BINDSTRING_URL = BINDSTRING_POST_DATA_MIME+1,
    BINDSTRING_IID = BINDSTRING_URL+1,
    BINDSTRING_FLAG_BIND_TO_OBJECT = BINDSTRING_IID+1,
    BINDSTRING_PTR_BIND_CONTEXT = BINDSTRING_FLAG_BIND_TO_OBJECT+1
} BINDSTRING;

EXTERN_C const IID IID_IInternetBindInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9e1-baf9-11ce-8c82-00aa004ba90b")
IInternetBindInfo:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetBindInfo(DWORD* grfBINDF,BINDINFO*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBindString(ULONG,LPOLESTR*,ULONG,ULONG*) = 0;
};
#else
typedef struct IInternetBindInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternetBindInfo*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternetBindInfo*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternetBindInfo*);
    HRESULT(STDMETHODCALLTYPE *GetBindInfo)(IInternetBindInfo*,DWORD*,BINDINFO*);
    HRESULT(STDMETHODCALLTYPE *GetBindString)(IInternetBindInfo*,ULONG,LPOLESTR*,ULONG,ULONG*);
    END_INTERFACE
} IInternetBindInfoVtbl;

interface IInternetBindInfo {
    CONST_VTBL struct IInternetBindInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetBindInfo_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternetBindInfo_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternetBindInfo_Release(This)  (This)->lpVtbl->Release(This)
#define IInternetBindInfo_GetBindInfo(This,grfBINDF,pbindinfo)  (This)->lpVtbl->GetBindInfo(This,grfBINDF,pbindinfo)
#define IInternetBindInfo_GetBindString(This,ulStringType,ppwzStr,cEl,pcElFetched)  (This)->lpVtbl->GetBindString(This,ulStringType,ppwzStr,cEl,pcElFetched)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IInternetBindInfo_GetBindInfo_Proxy(IInternetBindInfo*,DWORD*,BINDINFO*);
void __RPC_STUB IInternetBindInfo_GetBindInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetBindInfo_GetBindString_Proxy(IInternetBindInfo*,ULONG,LPOLESTR*,ULONG,ULONG*);
void __RPC_STUB IInternetBindInfo_GetBindString_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IInternetBindInfo_INTERFACE_DEFINED__ */
#endif


#ifndef _LPIINTERNETPROTOCOLROOT_DEFINED
#define _LPIINTERNETPROTOCOLROOT_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0181_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0181_v0_0_s_ifspec;

#ifndef __IInternetProtocolRoot_INTERFACE_DEFINED__
#define __IInternetProtocolRoot_INTERFACE_DEFINED__

typedef IInternetProtocolRoot *LPIINTERNETPROTOCOLROOT;

typedef enum _tagPI_FLAGS {
    PI_PARSE_URL = 0x1,
    PI_FILTER_MODE = 0x2,
    PI_FORCE_ASYNC = 0x4,
    PI_USE_WORKERTHREAD = 0x8,
    PI_MIMEVERIFICATION = 0x10,
    PI_CLSIDLOOKUP = 0x20,
    PI_DATAPROGRESS = 0x40,
    PI_SYNCHRONOUS = 0x80,
    PI_APARTMENTTHREADED = 0x100,
    PI_CLASSINSTALL = 0x200,
    PI_PASSONBINDCTX = 0x2000,
    PI_NOMIMEHANDLER = 0x8000,
    PI_LOADAPPDIRECT = 0x4000,
    PD_FORCE_SWITCH = 0x10000
} PI_FLAGS;

typedef struct _tagPROTOCOLDATA {
    DWORD grfFlags;
    DWORD dwState;
    LPVOID pData;
    ULONG cbData;
} PROTOCOLDATA;

typedef struct _tagStartParam {
    IID iid;
    IBindCtx *pIBindCtx;
    IUnknown *pItf;
} StartParam;

EXTERN_C const IID IID_IInternetProtocolRoot;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9e3-baf9-11ce-8c82-00aa004ba90b")
IInternetProtocolRoot:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Start(LPCWSTR,IInternetProtocolSink*,IInternetBindInfo*,DWORD,HANDLE_PTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE Continue(PROTOCOLDATA*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Abort(HRESULT,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE Terminate(DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE Suspend(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Resume(void) = 0;
};
#else
typedef struct IInternetProtocolRootVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternetProtocolRoot*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternetProtocolRoot*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternetProtocolRoot*);
    HRESULT(STDMETHODCALLTYPE *Start)(IInternetProtocolRoot*,LPCWSTR,IInternetProtocolSink*,IInternetBindInfo*,DWORD,HANDLE_PTR);
    HRESULT(STDMETHODCALLTYPE *Continue)(IInternetProtocolRoot*,PROTOCOLDATA*);
    HRESULT(STDMETHODCALLTYPE *Abort)(IInternetProtocolRoot*,HRESULT,DWORD);
    HRESULT(STDMETHODCALLTYPE *Terminate)(IInternetProtocolRoot*,DWORD);
    HRESULT(STDMETHODCALLTYPE *Suspend)(IInternetProtocolRoot*);
    HRESULT(STDMETHODCALLTYPE *Resume)(IInternetProtocolRoot*);
    END_INTERFACE
} IInternetProtocolRootVtbl;

interface IInternetProtocolRoot {
    CONST_VTBL struct IInternetProtocolRootVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetProtocolRoot_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternetProtocolRoot_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternetProtocolRoot_Release(This)  (This)->lpVtbl->Release(This)
#define IInternetProtocolRoot_Start(This,szUrl,pOIProtSink,pOIBindInfo,grfPI,dwReserved)  (This)->lpVtbl->Start(This,szUrl,pOIProtSink,pOIBindInfo,grfPI,dwReserved)
#define IInternetProtocolRoot_Continue(This,pProtocolData)  (This)->lpVtbl->Continue(This,pProtocolData)
#define IInternetProtocolRoot_Abort(This,hrReason,dwOptions)  (This)->lpVtbl->Abort(This,hrReason,dwOptions)
#define IInternetProtocolRoot_Terminate(This,dwOptions)  (This)->lpVtbl->Terminate(This,dwOptions)
#define IInternetProtocolRoot_Suspend(This)  (This)->lpVtbl->Suspend(This)
#define IInternetProtocolRoot_Resume(This)  (This)->lpVtbl->Resume(This)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IInternetProtocolRoot_Start_Proxy(IInternetProtocolRoot*,LPCWSTR,IInternetProtocolSink*,IInternetBindInfo*,DWORD,HANDLE_PTR);
void __RPC_STUB IInternetProtocolRoot_Start_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocolRoot_Continue_Proxy(IInternetProtocolRoot*,PROTOCOLDATA*);
void __RPC_STUB IInternetProtocolRoot_Continue_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocolRoot_Abort_Proxy(IInternetProtocolRoot*,HRESULT,DWORD);
void __RPC_STUB IInternetProtocolRoot_Abort_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocolRoot_Terminate_Proxy(IInternetProtocolRoot*,DWORD);
void __RPC_STUB IInternetProtocolRoot_Terminate_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocolRoot_Suspend_Proxy(IInternetProtocolRoot*);
void __RPC_STUB IInternetProtocolRoot_Suspend_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocolRoot_Resume_Proxy(IInternetProtocolRoot*);
void __RPC_STUB IInternetProtocolRoot_Resume_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IInternetProtocolRoot_INTERFACE_DEFINED__ */
#endif


#ifndef _LPIINTERNETPROTOCOL_DEFINED
#define _LPIINTERNETPROTOCOL_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0182_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0182_v0_0_s_ifspec;

#ifndef __IInternetProtocol_INTERFACE_DEFINED__
#define __IInternetProtocol_INTERFACE_DEFINED__

typedef IInternetProtocol *LPIINTERNETPROTOCOL;
EXTERN_C const IID IID_IInternetProtocol;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9e4-baf9-11ce-8c82-00aa004ba90b")
IInternetProtocol:public IInternetProtocolRoot
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Read(void*,ULONG,ULONG*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER,DWORD,ULARGE_INTEGER*) = 0;
    virtual HRESULT STDMETHODCALLTYPE LockRequest(DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnlockRequest(void) = 0;
};
#else
typedef struct IInternetProtocolVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternetProtocol*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternetProtocol*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternetProtocol*);
    HRESULT(STDMETHODCALLTYPE *Start)(IInternetProtocol*,LPCWSTR,IInternetProtocolSink*,IInternetBindInfo*,DWORD,HANDLE_PTR);
    HRESULT(STDMETHODCALLTYPE *Continue)(IInternetProtocol*,PROTOCOLDATA*);
    HRESULT(STDMETHODCALLTYPE *Abort)(IInternetProtocol*,HRESULT,DWORD);
    HRESULT(STDMETHODCALLTYPE *Terminate)(IInternetProtocol*,DWORD);
    HRESULT(STDMETHODCALLTYPE *Suspend)(IInternetProtocol*);
    HRESULT(STDMETHODCALLTYPE *Resume)(IInternetProtocol*);
    HRESULT(STDMETHODCALLTYPE *Read)(IInternetProtocol*,void*,ULONG,ULONG*);
    HRESULT(STDMETHODCALLTYPE *Seek)(IInternetProtocol*,LARGE_INTEGER,DWORD,ULARGE_INTEGER*);
    HRESULT(STDMETHODCALLTYPE *LockRequest)(IInternetProtocol*,DWORD);
    HRESULT(STDMETHODCALLTYPE *UnlockRequest)(IInternetProtocol*);
    END_INTERFACE
} IInternetProtocolVtbl;

interface IInternetProtocol {
    CONST_VTBL struct IInternetProtocolVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetProtocol_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternetProtocol_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternetProtocol_Release(This)  (This)->lpVtbl->Release(This)
#define IInternetProtocol_Start(This,szUrl,pOIProtSink,pOIBindInfo,grfPI,dwReserved)  (This)->lpVtbl->Start(This,szUrl,pOIProtSink,pOIBindInfo,grfPI,dwReserved)
#define IInternetProtocol_Continue(This,pProtocolData)  (This)->lpVtbl->Continue(This,pProtocolData)
#define IInternetProtocol_Abort(This,hrReason,dwOptions)  (This)->lpVtbl->Abort(This,hrReason,dwOptions)
#define IInternetProtocol_Terminate(This,dwOptions)  (This)->lpVtbl->Terminate(This,dwOptions)
#define IInternetProtocol_Suspend(This)  (This)->lpVtbl->Suspend(This)
#define IInternetProtocol_Resume(This)  (This)->lpVtbl->Resume(This)
#define IInternetProtocol_Read(This,pv,cb,pcbRead)  (This)->lpVtbl->Read(This,pv,cb,pcbRead)
#define IInternetProtocol_Seek(This,dlibMove,dwOrigin,plibNewPosition)  (This)->lpVtbl->Seek(This,dlibMove,dwOrigin,plibNewPosition)
#define IInternetProtocol_LockRequest(This,dwOptions)  (This)->lpVtbl->LockRequest(This,dwOptions)
#define IInternetProtocol_UnlockRequest(This)  (This)->lpVtbl->UnlockRequest(This)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IInternetProtocol_Read_Proxy(IInternetProtocol*,void*,ULONG,ULONG*);
void __RPC_STUB IInternetProtocol_Read_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocol_Seek_Proxy(IInternetProtocol*,LARGE_INTEGER,DWORD,ULARGE_INTEGER*);
void __RPC_STUB IInternetProtocol_Seek_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocol_LockRequest_Proxy(IInternetProtocol*,DWORD);
void __RPC_STUB IInternetProtocol_LockRequest_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocol_UnlockRequest_Proxy(IInternetProtocol*);
void __RPC_STUB IInternetProtocol_UnlockRequest_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IInternetProtocol_INTERFACE_DEFINED__ */
#endif

#if (_WIN32_IE >= _WIN32_IE_IE70)

#ifndef _LPIINTERNETPROTOCOLEX_DEFINED
#define _LPIINTERNETPROTOCOLEX_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0029_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0029_v0_0_s_ifspec;

#ifndef __IInternetProtocolEx_INTERFACE_DEFINED__
#define __IInternetProtocolEx_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInternetProtocolEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("C7A98E66-1010-492c-A1C8-C809E1F75905") IInternetProtocolEx : public IInternetProtocol
{
    public:
    virtual HRESULT STDMETHODCALLTYPE StartEx(IUri * pUri,
        IInternetProtocolSink * pOIProtSink,
        IInternetBindInfo * pOIBindInfo,
        DWORD grfPI,
        HANDLE_PTR dwReserved) = 0;
};
#else /* C style interface */
typedef struct IInternetProtocolExVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IInternetProtocolEx * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IInternetProtocolEx * This);
    ULONG (STDMETHODCALLTYPE *Release)(IInternetProtocolEx * This);
    HRESULT (STDMETHODCALLTYPE *Start)(IInternetProtocolEx * This, LPCWSTR szUrl, IInternetProtocolSink * pOIProtSink, IInternetBindInfo * pOIBindInfo, DWORD grfPI, HANDLE_PTR dwReserved);
    HRESULT (STDMETHODCALLTYPE *Continue)(IInternetProtocolEx * This, PROTOCOLDATA * pProtocolData);
    HRESULT (STDMETHODCALLTYPE *Abort)(IInternetProtocolEx * This, HRESULT hrReason, DWORD dwOptions);
    HRESULT (STDMETHODCALLTYPE *Terminate)(IInternetProtocolEx * This, DWORD dwOptions);
    HRESULT (STDMETHODCALLTYPE *Suspend)(IInternetProtocolEx * This);
    HRESULT (STDMETHODCALLTYPE *Resume)(IInternetProtocolEx * This);
    HRESULT (STDMETHODCALLTYPE *Read)(IInternetProtocolEx * This, void *pv, ULONG cb, ULONG * pcbRead);
    HRESULT (STDMETHODCALLTYPE *Seek)(IInternetProtocolEx * This, LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER * plibNewPosition);
    HRESULT (STDMETHODCALLTYPE *LockRequest)(IInternetProtocolEx * This, DWORD dwOptions);
    HRESULT (STDMETHODCALLTYPE *UnlockRequest)(IInternetProtocolEx * This);
    HRESULT (STDMETHODCALLTYPE *StartEx)(IInternetProtocolEx * This, IUri * pUri, IInternetProtocolSink * pOIProtSink, IInternetBindInfo * pOIBindInfo, DWORD grfPI, HANDLE_PTR dwReserved);
    END_INTERFACE
} IInternetProtocolExVtbl;

interface IInternetProtocolEx {
    CONST_VTBL struct IInternetProtocolExVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetProtocolEx_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IInternetProtocolEx_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IInternetProtocolEx_Release(This)  ((This)->lpVtbl->Release(This))
#define IInternetProtocolEx_Start(This,szUrl,pOIProtSink,pOIBindInfo,grfPI,dwReserved)  ((This)->lpVtbl->Start(This,szUrl,pOIProtSink,pOIBindInfo,grfPI,dwReserved))
#define IInternetProtocolEx_Continue(This,pProtocolData)  ((This)->lpVtbl->Continue(This,pProtocolData))
#define IInternetProtocolEx_Abort(This,hrReason,dwOptions)  ((This)->lpVtbl->Abort(This,hrReason,dwOptions))
#define IInternetProtocolEx_Terminate(This,dwOptions)  ((This)->lpVtbl->Terminate(This,dwOptions))
#define IInternetProtocolEx_Suspend(This)  ((This)->lpVtbl->Suspend(This))
#define IInternetProtocolEx_Resume(This)  ((This)->lpVtbl->Resume(This))
#define IInternetProtocolEx_Read(This,pv,cb,pcbRead)  ((This)->lpVtbl->Read(This,pv,cb,pcbRead))
#define IInternetProtocolEx_Seek(This,dlibMove,dwOrigin,plibNewPosition)  ((This)->lpVtbl->Seek(This,dlibMove,dwOrigin,plibNewPosition))
#define IInternetProtocolEx_LockRequest(This,dwOptions)  ((This)->lpVtbl->LockRequest(This,dwOptions))
#define IInternetProtocolEx_UnlockRequest(This)  ((This)->lpVtbl->UnlockRequest(This))
#define IInternetProtocolEx_StartEx(This,pUri,pOIProtSink,pOIBindInfo,grfPI,dwReserved)  ((This)->lpVtbl->StartEx(This,pUri,pOIProtSink,pOIBindInfo,grfPI,dwReserved))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IInternetProtocolEx_INTERFACE_DEFINED__ */

#endif
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */


#ifndef _LPIINTERNETPROTOCOLSINK_DEFINED
#define _LPIINTERNETPROTOCOLSINK_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0183_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0183_v0_0_s_ifspec;

#ifndef __IInternetProtocolSink_INTERFACE_DEFINED__
#define __IInternetProtocolSink_INTERFACE_DEFINED__

typedef IInternetProtocolSink *LPIINTERNETPROTOCOLSINK;
EXTERN_C const IID IID_IInternetProtocolSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9e5-baf9-11ce-8c82-00aa004ba90b")
IInternetProtocolSink:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Switch(PROTOCOLDATA*) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReportProgress(ULONG,LPCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReportData(DWORD,ULONG,ULONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReportResult(HRESULT,DWORD,LPCWSTR) = 0;
};
#else
typedef struct IInternetProtocolSinkVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternetProtocolSink*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternetProtocolSink*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternetProtocolSink*);
    HRESULT(STDMETHODCALLTYPE *Switch)(IInternetProtocolSink*,PROTOCOLDATA*);
    HRESULT(STDMETHODCALLTYPE *ReportProgress)(IInternetProtocolSink*,ULONG,LPCWSTR);
    HRESULT(STDMETHODCALLTYPE *ReportData)(IInternetProtocolSink*,DWORD,ULONG,ULONG);
    HRESULT(STDMETHODCALLTYPE *ReportResult)(IInternetProtocolSink*,HRESULT,DWORD,LPCWSTR);
    END_INTERFACE
} IInternetProtocolSinkVtbl;

interface IInternetProtocolSink {
    CONST_VTBL struct IInternetProtocolSinkVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetProtocolSink_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternetProtocolSink_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternetProtocolSink_Release(This)  (This)->lpVtbl->Release(This)
#define IInternetProtocolSink_Switch(This,pProtocolData)  (This)->lpVtbl->Switch(This,pProtocolData)
#define IInternetProtocolSink_ReportProgress(This,ulStatusCode,szStatusText)  (This)->lpVtbl->ReportProgress(This,ulStatusCode,szStatusText)
#define IInternetProtocolSink_ReportData(This,grfBSCF,ulProgress,ulProgressMax)  (This)->lpVtbl->ReportData(This,grfBSCF,ulProgress,ulProgressMax)
#define IInternetProtocolSink_ReportResult(This,hrResult,dwError,szResult)  (This)->lpVtbl->ReportResult(This,hrResult,dwError,szResult)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IInternetProtocolSink_Switch_Proxy(IInternetProtocolSink*,PROTOCOLDATA*);
void __RPC_STUB IInternetProtocolSink_Switch_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocolSink_ReportProgress_Proxy(IInternetProtocolSink*,ULONG,LPCWSTR);
void __RPC_STUB IInternetProtocolSink_ReportProgress_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocolSink_ReportData_Proxy(IInternetProtocolSink*,DWORD,ULONG,ULONG);
void __RPC_STUB IInternetProtocolSink_ReportData_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocolSink_ReportResult_Proxy(IInternetProtocolSink*,HRESULT,DWORD,LPCWSTR);
void __RPC_STUB IInternetProtocolSink_ReportResult_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IInternetProtocolSink_INTERFACE_DEFINED__ */
#endif


#ifndef _LPIINTERNETPROTOCOLSINKSTACKABLE_DEFINED
#define _LPIINTERNETPROTOCOLSINKSTACKABLE_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0184_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0184_v0_0_s_ifspec;

#ifndef __IInternetProtocolSinkStackable_INTERFACE_DEFINED__
#define __IInternetProtocolSinkStackable_INTERFACE_DEFINED__

typedef IInternetProtocolSinkStackable *LPIINTERNETPROTOCOLSINKStackable;
EXTERN_C const IID IID_IInternetProtocolSinkStackable;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9f0-baf9-11ce-8c82-00aa004ba90b")
IInternetProtocolSinkStackable:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SwitchSink(IInternetProtocolSink*) = 0;
    virtual HRESULT STDMETHODCALLTYPE CommitSwitch(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE RollbackSwitch(void) = 0;
};
#else
typedef struct IInternetProtocolSinkStackableVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternetProtocolSinkStackable*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternetProtocolSinkStackable*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternetProtocolSinkStackable*);
    HRESULT(STDMETHODCALLTYPE *SwitchSink)(IInternetProtocolSinkStackable*,IInternetProtocolSink*);
    HRESULT(STDMETHODCALLTYPE *CommitSwitch)(IInternetProtocolSinkStackable*);
    HRESULT(STDMETHODCALLTYPE *RollbackSwitch)(IInternetProtocolSinkStackable*);
    END_INTERFACE
} IInternetProtocolSinkStackableVtbl;

interface IInternetProtocolSinkStackable {
    CONST_VTBL struct IInternetProtocolSinkStackableVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetProtocolSinkStackable_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternetProtocolSinkStackable_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternetProtocolSinkStackable_Release(This)  (This)->lpVtbl->Release(This)
#define IInternetProtocolSinkStackable_SwitchSink(This,pOIProtSink)  (This)->lpVtbl->SwitchSink(This,pOIProtSink)
#define IInternetProtocolSinkStackable_CommitSwitch(This)  (This)->lpVtbl->CommitSwitch(This)
#define IInternetProtocolSinkStackable_RollbackSwitch(This)  (This)->lpVtbl->RollbackSwitch(This)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IInternetProtocolSinkStackable_SwitchSink_Proxy(IInternetProtocolSinkStackable *,IInternetProtocolSink*);
void __RPC_STUB IInternetProtocolSinkStackable_SwitchSink_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocolSinkStackable_CommitSwitch_Proxy(IInternetProtocolSinkStackable*);
void __RPC_STUB IInternetProtocolSinkStackable_CommitSwitch_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocolSinkStackable_RollbackSwitch_Proxy(IInternetProtocolSinkStackable*);
void __RPC_STUB IInternetProtocolSinkStackable_RollbackSwitch_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IInternetProtocolSinkStackable_INTERFACE_DEFINED__ */
#endif


#ifndef _LPIINTERNETSESSION_DEFINED
#define _LPIINTERNETSESSION_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0185_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0185_v0_0_s_ifspec;

#ifndef __IInternetSession_INTERFACE_DEFINED__
#define __IInternetSession_INTERFACE_DEFINED__

typedef IInternetSession *LPIINTERNETSESSION;

typedef enum _tagOIBDG_FLAGS {
    OIBDG_APARTMENTTHREADED = 0x100,
    OIBDG_DATAONLY = 0x1000
} OIBDG_FLAGS;

EXTERN_C const IID IID_IInternetSession;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9e7-baf9-11ce-8c82-00aa004ba90b")
IInternetSession:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE RegisterNameSpace(IClassFactory*,REFCLSID,LPCWSTR,ULONG,const LPCWSTR*,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnregisterNameSpace(IClassFactory*,LPCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE RegisterMimeFilter(IClassFactory*,REFCLSID,LPCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnregisterMimeFilter(IClassFactory*,LPCWSTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBinding(LPBC,LPCWSTR,IUnknown*,IUnknown**,IInternetProtocol**,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSessionOption(DWORD,LPVOID,DWORD,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSessionOption(DWORD,LPVOID,DWORD*,DWORD) = 0;
};
#else
typedef struct IInternetSessionVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternetSession*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternetSession*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternetSession*);
    HRESULT(STDMETHODCALLTYPE *RegisterNameSpace)(IInternetSession*,IClassFactory*,REFCLSID,LPCWSTR,ULONG,const LPCWSTR*,DWORD);
    HRESULT(STDMETHODCALLTYPE *UnregisterNameSpace)(IInternetSession*,IClassFactory*,LPCWSTR);
    HRESULT(STDMETHODCALLTYPE *RegisterMimeFilter)(IInternetSession*,IClassFactory*,REFCLSID,LPCWSTR);
    HRESULT(STDMETHODCALLTYPE *UnregisterMimeFilter)(IInternetSession*,IClassFactory*,LPCWSTR);
    HRESULT(STDMETHODCALLTYPE *CreateBinding)(IInternetSession*,LPBC,LPCWSTR,IUnknown*,IUnknown**,IInternetProtocol**,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetSessionOption)(IInternetSession*,DWORD,LPVOID,DWORD,DWORD);
    HRESULT(STDMETHODCALLTYPE *GetSessionOption)(IInternetSession*,DWORD,LPVOID,DWORD*,DWORD);
    END_INTERFACE
} IInternetSessionVtbl;

interface IInternetSession {
    CONST_VTBL struct IInternetSessionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetSession_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternetSession_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternetSession_Release(This)  (This)->lpVtbl->Release(This)
#define IInternetSession_RegisterNameSpace(This,pCF,rclsid,pwzProtocol,cPatterns,ppwzPatterns,dwReserved)  (This)->lpVtbl->RegisterNameSpace(This,pCF,rclsid,pwzProtocol,cPatterns,ppwzPatterns,dwReserved)
#define IInternetSession_UnregisterNameSpace(This,pCF,pszProtocol)  (This)->lpVtbl->UnregisterNameSpace(This,pCF,pszProtocol)
#define IInternetSession_RegisterMimeFilter(This,pCF,rclsid,pwzType)  (This)->lpVtbl->RegisterMimeFilter(This,pCF,rclsid,pwzType)
#define IInternetSession_UnregisterMimeFilter(This,pCF,pwzType)  (This)->lpVtbl->UnregisterMimeFilter(This,pCF,pwzType)
#define IInternetSession_CreateBinding(This,pBC,szUrl,pUnkOuter,ppUnk,ppOInetProt,dwOption)  (This)->lpVtbl->CreateBinding(This,pBC,szUrl,pUnkOuter,ppUnk,ppOInetProt,dwOption)
#define IInternetSession_SetSessionOption(This,dwOption,pBuffer,dwBufferLength,dwReserved)  (This)->lpVtbl->SetSessionOption(This,dwOption,pBuffer,dwBufferLength,dwReserved)
#define IInternetSession_GetSessionOption(This,dwOption,pBuffer,pdwBufferLength,dwReserved)  (This)->lpVtbl->GetSessionOption(This,dwOption,pBuffer,pdwBufferLength,dwReserved)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IInternetSession_RegisterNameSpace_Proxy(IInternetSession*,IClassFactory*,REFCLSID,LPCWSTR,ULONG,const LPCWSTR*,DWORD);
void __RPC_STUB IInternetSession_RegisterNameSpace_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSession_UnregisterNameSpace_Proxy(IInternetSession*,IClassFactory*,LPCWSTR);
void __RPC_STUB IInternetSession_UnregisterNameSpace_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSession_RegisterMimeFilter_Proxy(IInternetSession*,IClassFactory*,REFCLSID,LPCWSTR);
void __RPC_STUB IInternetSession_RegisterMimeFilter_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSession_UnregisterMimeFilter_Proxy(IInternetSession*,IClassFactory*,LPCWSTR);
void __RPC_STUB IInternetSession_UnregisterMimeFilter_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSession_CreateBinding_Proxy(IInternetSession*,LPBC,LPCWSTR,IUnknown*,IUnknown**,IInternetProtocol**,DWORD);
void __RPC_STUB IInternetSession_CreateBinding_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSession_SetSessionOption_Proxy(IInternetSession*,DWORD,LPVOID,DWORD,DWORD);
void __RPC_STUB IInternetSession_SetSessionOption_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSession_GetSessionOption_Proxy(IInternetSession*,DWORD,LPVOID,DWORD*,DWORD);
void __RPC_STUB IInternetSession_GetSessionOption_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IInternetSession_INTERFACE_DEFINED__ */
#endif


#ifndef _LPIINTERNETTHREADSWITCH_DEFINED
#define _LPIINTERNETTHREADSWITCH_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0186_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0186_v0_0_s_ifspec;

#ifndef __IInternetThreadSwitch_INTERFACE_DEFINED__
#define __IInternetThreadSwitch_INTERFACE_DEFINED__

typedef IInternetThreadSwitch *LPIINTERNETTHREADSWITCH;
EXTERN_C const IID IID_IInternetThreadSwitch;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9e8-baf9-11ce-8c82-00aa004ba90b")
IInternetThreadSwitch:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Prepare(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Continue(void) = 0;
};
#else
typedef struct IInternetThreadSwitchVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternetThreadSwitch*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternetThreadSwitch*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternetThreadSwitch*);
    HRESULT(STDMETHODCALLTYPE *Prepare)(IInternetThreadSwitch*);
    HRESULT(STDMETHODCALLTYPE *Continue)(IInternetThreadSwitch*);
    END_INTERFACE
} IInternetThreadSwitchVtbl;

interface IInternetThreadSwitch {
    CONST_VTBL struct IInternetThreadSwitchVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetThreadSwitch_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternetThreadSwitch_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternetThreadSwitch_Release(This)  (This)->lpVtbl->Release(This)
#define IInternetThreadSwitch_Prepare(This)  (This)->lpVtbl->Prepare(This)
#define IInternetThreadSwitch_Continue(This)  (This)->lpVtbl->Continue(This)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IInternetThreadSwitch_Prepare_Proxy(IInternetThreadSwitch*);
void __RPC_STUB IInternetThreadSwitch_Prepare_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetThreadSwitch_Continue_Proxy(IInternetThreadSwitch*);
void __RPC_STUB IInternetThreadSwitch_Continue_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IInternetThreadSwitch_INTERFACE_DEFINED__ */
#endif


#ifndef _LPIINTERNETPRIORITY_DEFINED
#define _LPIINTERNETPRIORITY_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0187_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0187_v0_0_s_ifspec;

#ifndef __IInternetPriority_INTERFACE_DEFINED__
#define __IInternetPriority_INTERFACE_DEFINED__

typedef IInternetPriority *LPIINTERNETPRIORITY;
EXTERN_C const IID IID_IInternetPriority;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9eb-baf9-11ce-8c82-00aa004ba90b")
IInternetPriority:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetPriority(LONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPriority(LONG*) = 0;
};
#else
typedef struct IInternetPriorityVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternetPriority*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternetPriority*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternetPriority*);
    HRESULT(STDMETHODCALLTYPE *SetPriority)(IInternetPriority*,LONG);
    HRESULT(STDMETHODCALLTYPE *GetPriority)(IInternetPriority*,LONG*);
    END_INTERFACE
} IInternetPriorityVtbl;

interface IInternetPriority {
    CONST_VTBL struct IInternetPriorityVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetPriority_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternetPriority_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternetPriority_Release(This)  (This)->lpVtbl->Release(This)
#define IInternetPriority_SetPriority(This,nPriority)  (This)->lpVtbl->SetPriority(This,nPriority)
#define IInternetPriority_GetPriority(This,pnPriority)  (This)->lpVtbl->GetPriority(This,pnPriority)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IInternetPriority_SetPriority_Proxy(IInternetPriority*,LONG);
void __RPC_STUB IInternetPriority_SetPriority_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetPriority_GetPriority_Proxy(IInternetPriority*,LONG*);
void __RPC_STUB IInternetPriority_GetPriority_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IInternetPriority_INTERFACE_DEFINED__ */
#endif


#ifndef _LPIINTERNETPROTOCOLINFO_DEFINED
#define _LPIINTERNETPROTOCOLINFO_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0188_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0188_v0_0_s_ifspec;

#ifndef __IInternetProtocolInfo_INTERFACE_DEFINED__
#define __IInternetProtocolInfo_INTERFACE_DEFINED__

typedef IInternetProtocolInfo *LPIINTERNETPROTOCOLINFO;

typedef enum _tagPARSEACTION {
    PARSE_CANONICALIZE = 1,
    PARSE_FRIENDLY = PARSE_CANONICALIZE+1,
    PARSE_SECURITY_URL = PARSE_FRIENDLY+1,
    PARSE_ROOTDOCUMENT = PARSE_SECURITY_URL+1,
    PARSE_DOCUMENT = PARSE_ROOTDOCUMENT+1,
    PARSE_ANCHOR = PARSE_DOCUMENT+1,
    PARSE_ENCODE = PARSE_ANCHOR+1,
    PARSE_DECODE = PARSE_ENCODE+1,
    PARSE_PATH_FROM_URL = PARSE_DECODE+1,
    PARSE_URL_FROM_PATH = PARSE_PATH_FROM_URL+1,
    PARSE_MIME = PARSE_URL_FROM_PATH+1,
    PARSE_SERVER = PARSE_MIME+1,
    PARSE_SCHEMA = PARSE_SERVER+1,
    PARSE_SITE = PARSE_SCHEMA+1,
    PARSE_DOMAIN = PARSE_SITE+1,
    PARSE_LOCATION = PARSE_DOMAIN+1,
    PARSE_SECURITY_DOMAIN = PARSE_LOCATION+1,
    PARSE_ESCAPE = PARSE_SECURITY_DOMAIN+1,
    PARSE_UNESCAPE = PARSE_ESCAPE+1
} PARSEACTION;

typedef enum _tagPSUACTION {
    PSU_DEFAULT = 1,
    PSU_SECURITY_URL_ONLY = PSU_DEFAULT+1
} PSUACTION;

typedef enum _tagQUERYOPTION {
    QUERY_EXPIRATION_DATE = 1,
    QUERY_TIME_OF_LAST_CHANGE = QUERY_EXPIRATION_DATE+1,
    QUERY_CONTENT_ENCODING = QUERY_TIME_OF_LAST_CHANGE+1,
    QUERY_CONTENT_TYPE = QUERY_CONTENT_ENCODING+1,
    QUERY_REFRESH = QUERY_CONTENT_TYPE+1,
    QUERY_RECOMBINE = QUERY_REFRESH+1,
    QUERY_CAN_NAVIGATE = QUERY_RECOMBINE+1,
    QUERY_USES_NETWORK = QUERY_CAN_NAVIGATE+1,
    QUERY_IS_CACHED = QUERY_USES_NETWORK+1,
    QUERY_IS_INSTALLEDENTRY = QUERY_IS_CACHED+1,
    QUERY_IS_CACHED_OR_MAPPED = QUERY_IS_INSTALLEDENTRY+1,
    QUERY_USES_CACHE = QUERY_IS_CACHED_OR_MAPPED+1,
    QUERY_IS_SECURE = QUERY_USES_CACHE+1,
    QUERY_IS_SAFE = QUERY_IS_SECURE+1
} QUERYOPTION;

EXTERN_C const IID IID_IInternetProtocolInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9ec-baf9-11ce-8c82-00aa004ba90b")
IInternetProtocolInfo:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE ParseUrl(LPCWSTR,PARSEACTION,DWORD,LPWSTR,DWORD,DWORD*,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE CombineUrl(LPCWSTR,LPCWSTR,DWORD,LPWSTR,DWORD,DWORD*,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE CompareUrl(LPCWSTR,LPCWSTR,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryInfo(LPCWSTR,QUERYOPTION,DWORD,LPVOID,DWORD,DWORD*,DWORD) = 0;
};
#else
typedef struct IInternetProtocolInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternetProtocolInfo*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternetProtocolInfo*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternetProtocolInfo*);
    HRESULT(STDMETHODCALLTYPE *ParseUrl)(IInternetProtocolInfo*,LPCWSTR,PARSEACTION,DWORD,LPWSTR,DWORD,DWORD*,DWORD);
    HRESULT(STDMETHODCALLTYPE *CombineUrl)(IInternetProtocolInfo*,LPCWSTR,LPCWSTR,DWORD,LPWSTR,DWORD,DWORD*,DWORD);
    HRESULT(STDMETHODCALLTYPE *CompareUrl)(IInternetProtocolInfo*,LPCWSTR,LPCWSTR,DWORD);
    HRESULT(STDMETHODCALLTYPE *QueryInfo)(IInternetProtocolInfo*,LPCWSTR,QUERYOPTION,DWORD,LPVOID,DWORD,DWORD*,DWORD);
    END_INTERFACE
} IInternetProtocolInfoVtbl;

interface IInternetProtocolInfo {
    CONST_VTBL struct IInternetProtocolInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetProtocolInfo_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternetProtocolInfo_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternetProtocolInfo_Release(This)  (This)->lpVtbl->Release(This)
#define IInternetProtocolInfo_ParseUrl(This,pwzUrl,ParseAction,dwParseFlags,pwzResult,cchResult,pcchResult,dwReserved)  (This)->lpVtbl->ParseUrl(This,pwzUrl,ParseAction,dwParseFlags,pwzResult,cchResult,pcchResult,dwReserved)
#define IInternetProtocolInfo_CombineUrl(This,pwzBaseUrl,pwzRelativeUrl,dwCombineFlags,pwzResult,cchResult,pcchResult,dwReserved)  (This)->lpVtbl->CombineUrl(This,pwzBaseUrl,pwzRelativeUrl,dwCombineFlags,pwzResult,cchResult,pcchResult,dwReserved)
#define IInternetProtocolInfo_CompareUrl(This,pwzUrl1,pwzUrl2,dwCompareFlags)  (This)->lpVtbl->CompareUrl(This,pwzUrl1,pwzUrl2,dwCompareFlags)
#define IInternetProtocolInfo_QueryInfo(This,pwzUrl,OueryOption,dwQueryFlags,pBuffer,cbBuffer,pcbBuf,dwReserved)  (This)->lpVtbl->QueryInfo(This,pwzUrl,OueryOption,dwQueryFlags,pBuffer,cbBuffer,pcbBuf,dwReserved)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IInternetProtocolInfo_ParseUrl_Proxy(IInternetProtocolInfo*,LPCWSTR,PARSEACTION,DWORD,LPWSTR,DWORD,DWORD*,DWORD);
void __RPC_STUB IInternetProtocolInfo_ParseUrl_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocolInfo_CombineUrl_Proxy(IInternetProtocolInfo*,LPCWSTR,LPCWSTR,DWORD,LPWSTR,DWORD,DWORD*,DWORD);
void __RPC_STUB IInternetProtocolInfo_CombineUrl_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocolInfo_CompareUrl_Proxy(IInternetProtocolInfo*,LPCWSTR,LPCWSTR,DWORD);
void __RPC_STUB IInternetProtocolInfo_CompareUrl_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetProtocolInfo_QueryInfo_Proxy(IInternetProtocolInfo*,LPCWSTR,QUERYOPTION,DWORD,LPVOID,DWORD,DWORD*,DWORD);
void __RPC_STUB IInternetProtocolInfo_QueryInfo_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IInternetProtocolInfo_INTERFACE_DEFINED__ */
#endif


#define IOInet  IInternet
#define IOInetBindInfo  IInternetBindInfo
#define IOInetProtocolRoot  IInternetProtocolRoot
#define IOInetProtocol  IInternetProtocol
#if (_WIN32_IE >= _WIN32_IE_IE70)
#define IOInetProtocolEx  IInternetProtocolEx
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */
#define IOInetProtocolSink  IInternetProtocolSink
#define IOInetProtocolInfo  IInternetProtocolInfo
#define IOInetSession  IInternetSession
#define IOInetPriority  IInternetPriority
#define IOInetThreadSwitch  IInternetThreadSwitch
#define IOInetProtocolSinkStackable  IInternetProtocolSinkStackable
#define LPOINET  LPIINTERNET
#define LPOINETPROTOCOLINFO  LPIINTERNETPROTOCOLINFO
#define LPOINETBINDINFO  LPIINTERNETBINDINFO
#define LPOINETPROTOCOLROOT  LPIINTERNETPROTOCOLROOT
#define LPOINETPROTOCOL  LPIINTERNETPROTOCOL
#if (_WIN32_IE >= _WIN32_IE_IE70)
#define LPOINETPROTOCOLEX  LPIINTERNETPROTOCOLEX
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */
#define LPOINETPROTOCOLSINK  LPIINTERNETPROTOCOLSINK
#define LPOINETSESSION  LPIINTERNETSESSION
#define LPOINETTHREADSWITCH  LPIINTERNETTHREADSWITCH
#define LPOINETPRIORITY  LPIINTERNETPRIORITY
#define LPOINETPROTOCOLINFO  LPIINTERNETPROTOCOLINFO
#define LPOINETPROTOCOLSINKSTACKABLE  LPIINTERNETPROTOCOLSINKSTACKABLE
#define IID_IOInet  IID_IInternet
#define IID_IOInetBindInfo  IID_IInternetBindInfo
#define IID_IOInetProtocolRoot  IID_IInternetProtocolRoot
#define IID_IOInetProtocol  IID_IInternetProtocol
#if (_WIN32_IE >= _WIN32_IE_IE70)
#define IID_IOInetProtocolEx  IID_IInternetProtocolEx
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */
#define IID_IOInetProtocolSink  IID_IInternetProtocolSink
#define IID_IOInetProtocolInfo  IID_IInternetProtocolInfo
#define IID_IOInetSession  IID_IInternetSession
#define IID_IOInetPriority  IID_IInternetPriority
#define IID_IOInetThreadSwitch  IID_IInternetThreadSwitch
#define IID_IOInetProtocolSinkStackable IID_IInternetProtocolSinkStackable

STDAPI CoInternetParseUrl(LPCWSTR,PARSEACTION,DWORD,LPWSTR,DWORD,DWORD*,DWORD);
#if (_WIN32_IE >= _WIN32_IE_IE70)
STDAPI CoInternetParseIUri(IUri *,PARSEACTION,DWORD,LPWSTR,DWORD,DWORD*,DWORD_PTR);
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */
STDAPI CoInternetCombineUrl(LPCWSTR,LPCWSTR,DWORD,LPWSTR,DWORD,DWORD*,DWORD);
#if (_WIN32_IE >= _WIN32_IE_IE70)
STDAPI CoInternetCombineUrlEx(IUri*,LPCWSTR,DWORD,IUri**,DWORD_PTR);
STDAPI CoInternetCombineIUri(IUri*,IUri*,DWORD,IUri**,DWORD_PTR);
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */
STDAPI CoInternetCompareUrl(LPCWSTR,LPCWSTR,DWORD);
STDAPI CoInternetGetProtocolFlags(LPCWSTR,DWORD*,DWORD);
STDAPI CoInternetQueryInfo(LPCWSTR,QUERYOPTION,DWORD,LPVOID,DWORD,DWORD*,DWORD);
STDAPI CoInternetGetSession(DWORD,IInternetSession**,DWORD);
STDAPI CoInternetGetSecurityUrl(LPCWSTR,LPWSTR*,PSUACTION,DWORD);
STDAPI AsyncInstallDistributionUnit(LPCWSTR,LPCWSTR,LPCWSTR,DWORD,DWORD,LPCWSTR,IBindCtx*,LPVOID,DWORD);
#if (_WIN32_IE >= _WIN32_IE_IE70)
STDAPI CoInternetGetSecurityUrlEx(IUri*,IUri**,PSUACTION,DWORD_PTR);
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */

#if (_WIN32_IE >= _WIN32_IE_IE60SP2)

#ifndef _INTERNETFEATURELIST_DEFINED
#define _INTERNETFEATURELIST_DEFINED
typedef enum _tagINTERNETFEATURELIST {
    FEATURE_OBJECT_CACHING = 0,
    FEATURE_ZONE_ELEVATION = (FEATURE_OBJECT_CACHING + 1),
    FEATURE_MIME_HANDLING = (FEATURE_ZONE_ELEVATION + 1),
    FEATURE_MIME_SNIFFING = (FEATURE_MIME_HANDLING + 1),
    FEATURE_WINDOW_RESTRICTIONS = (FEATURE_MIME_SNIFFING + 1),
    FEATURE_WEBOC_POPUPMANAGEMENT = (FEATURE_WINDOW_RESTRICTIONS + 1),
    FEATURE_BEHAVIORS = (FEATURE_WEBOC_POPUPMANAGEMENT + 1),
    FEATURE_DISABLE_MK_PROTOCOL = (FEATURE_BEHAVIORS + 1),
    FEATURE_LOCALMACHINE_LOCKDOWN = (FEATURE_DISABLE_MK_PROTOCOL + 1),
    FEATURE_SECURITYBAND = (FEATURE_LOCALMACHINE_LOCKDOWN + 1),
    FEATURE_RESTRICT_ACTIVEXINSTALL = (FEATURE_SECURITYBAND + 1),
    FEATURE_VALIDATE_NAVIGATE_URL = (FEATURE_RESTRICT_ACTIVEXINSTALL + 1),
    FEATURE_RESTRICT_FILEDOWNLOAD = (FEATURE_VALIDATE_NAVIGATE_URL + 1),
    FEATURE_ADDON_MANAGEMENT = (FEATURE_RESTRICT_FILEDOWNLOAD + 1),
    FEATURE_PROTOCOL_LOCKDOWN = (FEATURE_ADDON_MANAGEMENT + 1),
    FEATURE_HTTP_USERNAME_PASSWORD_DISABLE = (FEATURE_PROTOCOL_LOCKDOWN + 1),
    FEATURE_SAFE_BINDTOOBJECT = (FEATURE_HTTP_USERNAME_PASSWORD_DISABLE + 1),
    FEATURE_UNC_SAVEDFILECHECK = (FEATURE_SAFE_BINDTOOBJECT + 1),
    FEATURE_GET_URL_DOM_FILEPATH_UNENCODED = (FEATURE_UNC_SAVEDFILECHECK + 1),
    FEATURE_TABBED_BROWSING = (FEATURE_GET_URL_DOM_FILEPATH_UNENCODED + 1),
    FEATURE_SSLUX = (FEATURE_TABBED_BROWSING + 1),
    FEATURE_DISABLE_NAVIGATION_SOUNDS = (FEATURE_SSLUX + 1),
    FEATURE_DISABLE_LEGACY_COMPRESSION = (FEATURE_DISABLE_NAVIGATION_SOUNDS + 1),
    FEATURE_FORCE_ADDR_AND_STATUS = (FEATURE_DISABLE_LEGACY_COMPRESSION + 1),
    FEATURE_XMLHTTP = (FEATURE_FORCE_ADDR_AND_STATUS + 1),
    FEATURE_DISABLE_TELNET_PROTOCOL = (FEATURE_XMLHTTP + 1),
    FEATURE_FEEDS = (FEATURE_DISABLE_TELNET_PROTOCOL + 1),
    FEATURE_BLOCK_INPUT_PROMPTS = (FEATURE_FEEDS + 1),
    FEATURE_ENTRY_COUNT = (FEATURE_BLOCK_INPUT_PROMPTS + 1)
} INTERNETFEATURELIST;

#define SET_FEATURE_ON_THREAD  0x00000001
#define SET_FEATURE_ON_PROCESS  0x00000002
#define SET_FEATURE_IN_REGISTRY  0x00000004
#define SET_FEATURE_ON_THREAD_LOCALMACHINE  0x00000008
#define SET_FEATURE_ON_THREAD_INTRANET  0x00000010
#define SET_FEATURE_ON_THREAD_TRUSTED  0x00000020
#define SET_FEATURE_ON_THREAD_INTERNET  0x00000040
#define SET_FEATURE_ON_THREAD_RESTRICTED  0x00000080

#define GET_FEATURE_FROM_THREAD  0x00000001
#define GET_FEATURE_FROM_PROCESS  0x00000002
#define GET_FEATURE_FROM_REGISTRY  0x00000004
#define GET_FEATURE_FROM_THREAD_LOCALMACHINE  0x00000008
#define GET_FEATURE_FROM_THREAD_INTRANET  0x00000010
#define GET_FEATURE_FROM_THREAD_TRUSTED  0x00000020
#define GET_FEATURE_FROM_THREAD_INTERNET  0x00000040
#define GET_FEATURE_FROM_THREAD_RESTRICTED  0x00000080
#endif /* _INTERNETFEATURELIST_DEFINED */

STDAPI CoInternetSetFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD, BOOL);
STDAPI CoInternetIsFeatureEnabled(INTERNETFEATURELIST FeatureEntry, DWORD);
STDAPI CoInternetIsFeatureEnabledForUrl(INTERNETFEATURELIST, DWORD, LPCWSTR, IInternetSecurityManager *);
STDAPI CoInternetIsFeatureEnabledForIUri(INTERNETFEATURELIST, DWORD, IUri *, IInternetSecurityManagerEx2 *);
STDAPI CoInternetIsFeatureZoneElevationEnabled(LPCWSTR, LPCWSTR, IInternetSecurityManager *, DWORD);
#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */

STDAPI CopyStgMedium(const STGMEDIUM*,STGMEDIUM*);
STDAPI CopyBindInfo(const BINDINFO*,BINDINFO*);
STDAPI_(void) ReleaseBindInfo(BINDINFO*);

#define INET_E_USE_DEFAULT_PROTOCOLHANDLER  _HRESULT_TYPEDEF_(0x800C0011L)
#define INET_E_USE_DEFAULT_SETTING  _HRESULT_TYPEDEF_(0x800C0012L)
#define INET_E_DEFAULT_ACTION  INET_E_USE_DEFAULT_PROTOCOLHANDLER
#define INET_E_QUERYOPTION_UNKNOWN  _HRESULT_TYPEDEF_(0x800C0013L)
#define INET_E_REDIRECTING  _HRESULT_TYPEDEF_(0x800C0014L)
#define OInetParseUrl  CoInternetParseUrl
#define OInetCombineUrl  CoInternetCombineUrl
#if (_WIN32_IE >= _WIN32_IE_IE70)
#define OInetCombineUrlEx  CoInternetCombineUrlEx
#define OInetCombineIUri  CoInternetCombineIUri
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */
#define OInetCompareUrl  CoInternetCompareUrl
#define OInetQueryInfo  CoInternetQueryInfo
#define OInetGetSession  CoInternetGetSession
#endif /* !_URLMON_NO_ASYNC_PLUGABLE_PROTOCOLS_ Static Protocol flags */

#define PROTOCOLFLAG_NO_PICS_CHECK  0x00000001

STDAPI CoInternetCreateSecurityManager(IServiceProvider*,IInternetSecurityManager**,DWORD);
STDAPI CoInternetCreateZoneManager(IServiceProvider*,IInternetZoneManager**,DWORD);

EXTERN_C const IID CLSID_InternetSecurityManager;
EXTERN_C const IID CLSID_InternetZoneManager;
#if (_WIN32_IE >= _WIN32_IE_IE60SP2)
EXTERN_C const IID CLSID_PersistentZoneIdentifier;
#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */

#define SID_SInternetSecurityManager  IID_IInternetSecurityManager
#define SID_SInternetHostSecurityManager  IID_IInternetHostSecurityManager
#if (_WIN32_IE >= _WIN32_IE_IE60SP2)
#define SID_SInternetSecurityManagerEx  IID_IInternetSecurityManagerEx
#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */
#if (_WIN32_IE >= _WIN32_IE_IE70)
#define SID_SInternetSecurityManagerEx2  IID_IInternetSecurityManagerEx2
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */


#ifndef _LPINTERNETSECURITYMGRSITE_DEFINED
#define _LPINTERNETSECURITYMGRSITE_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0189_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0189_v0_0_s_ifspec;

#ifndef __IInternetSecurityMgrSite_INTERFACE_DEFINED__
#define __IInternetSecurityMgrSite_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInternetSecurityMgrSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9ed-baf9-11ce-8c82-00aa004ba90b")
IInternetSecurityMgrSite:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetWindow(HWND*) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnableModeless(BOOL) = 0;
};
#else
typedef struct IInternetSecurityMgrSiteVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternetSecurityMgrSite*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternetSecurityMgrSite*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternetSecurityMgrSite*);
    HRESULT(STDMETHODCALLTYPE *GetWindow)(IInternetSecurityMgrSite*,HWND*);
    HRESULT(STDMETHODCALLTYPE *EnableModeless)(IInternetSecurityMgrSite*,BOOL);
    END_INTERFACE
} IInternetSecurityMgrSiteVtbl;

interface IInternetSecurityMgrSite {
    CONST_VTBL struct IInternetSecurityMgrSiteVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetSecurityMgrSite_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternetSecurityMgrSite_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternetSecurityMgrSite_Release(This)  (This)->lpVtbl->Release(This)
#define IInternetSecurityMgrSite_GetWindow(This,phwnd)  (This)->lpVtbl->GetWindow(This,phwnd)
#define IInternetSecurityMgrSite_EnableModeless(This,fEnable)  (This)->lpVtbl->EnableModeless(This,fEnable)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IInternetSecurityMgrSite_GetWindow_Proxy(IInternetSecurityMgrSite*,HWND*);
void __RPC_STUB IInternetSecurityMgrSite_GetWindow_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSecurityMgrSite_EnableModeless_Proxy(IInternetSecurityMgrSite*,BOOL);
void __RPC_STUB IInternetSecurityMgrSite_EnableModeless_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IInternetSecurityMgrSite_INTERFACE_DEFINED__ */
#endif


#ifndef _LPINTERNETSECURITYMANANGER_DEFINED
#define _LPINTERNETSECURITYMANANGER_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0190_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0190_v0_0_s_ifspec;

#ifndef __IInternetSecurityManager_INTERFACE_DEFINED__
#define __IInternetSecurityManager_INTERFACE_DEFINED__

#define MUTZ_NOSAVEDFILECHECK  0x00000001
#define MAX_SIZE_SECURITY_ID  512

typedef enum __MIDL_IInternetSecurityManager_0001 {
    PUAF_DEFAULT = 0,
    PUAF_NOUI = 0x1,
    PUAF_ISFILE = 0x2,
    PUAF_WARN_IF_DENIED = 0x4,
    PUAF_FORCEUI_FOREGROUND = 0x8,
    PUAF_CHECK_TIFS = 0x10,
    PUAF_DONTCHECKBOXINDIALOG = 0x20,
    PUAF_TRUSTED = 0x40,
    PUAF_ACCEPT_WILDCARD_SCHEME = 0x80
} PUAF;

typedef enum __MIDL_IInternetSecurityManager_0002 {
    SZM_CREATE = 0,
    SZM_DELETE = 0x1
} SZM_FLAGS;

EXTERN_C const IID IID_IInternetSecurityManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9ee-baf9-11ce-8c82-00aa004ba90b")
IInternetSecurityManager:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetSecuritySite(IInternetSecurityMgrSite*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSecuritySite(IInternetSecurityMgrSite**) = 0;
    virtual HRESULT STDMETHODCALLTYPE MapUrlToZone(LPCWSTR,DWORD*,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSecurityId(LPCWSTR,BYTE*,DWORD*,DWORD_PTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE ProcessUrlAction(LPCWSTR,DWORD,BYTE*,DWORD,BYTE*,DWORD,DWORD,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryCustomPolicy(LPCWSTR,REFGUID,BYTE**,DWORD*,BYTE*,DWORD,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetZoneMapping(DWORD,LPCWSTR,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetZoneMappings(DWORD,IEnumString**,DWORD) = 0;
};
#else
typedef struct IInternetSecurityManagerVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternetSecurityManager*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternetSecurityManager*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternetSecurityManager*);
    HRESULT(STDMETHODCALLTYPE *SetSecuritySite)(IInternetSecurityManager*,IInternetSecurityMgrSite*);
    HRESULT(STDMETHODCALLTYPE *GetSecuritySite)(IInternetSecurityManager*,IInternetSecurityMgrSite**);
    HRESULT(STDMETHODCALLTYPE *MapUrlToZone)(IInternetSecurityManager*,LPCWSTR,DWORD*,DWORD);
    HRESULT(STDMETHODCALLTYPE *GetSecurityId)(IInternetSecurityManager*,LPCWSTR,BYTE*,DWORD*,DWORD_PTR);
    HRESULT(STDMETHODCALLTYPE *ProcessUrlAction)(IInternetSecurityManager*,LPCWSTR,DWORD,BYTE*,DWORD,BYTE*,DWORD,DWORD,DWORD);
    HRESULT(STDMETHODCALLTYPE *QueryCustomPolicy)(IInternetSecurityManager*,LPCWSTR,REFGUID,BYTE**,DWORD*,BYTE*,DWORD,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetZoneMapping)(IInternetSecurityManager*,DWORD,LPCWSTR,DWORD);
    HRESULT(STDMETHODCALLTYPE *GetZoneMappings)(IInternetSecurityManager*,DWORD,IEnumString**,DWORD);
    END_INTERFACE
} IInternetSecurityManagerVtbl;

interface IInternetSecurityManager {
    CONST_VTBL struct IInternetSecurityManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetSecurityManager_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternetSecurityManager_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternetSecurityManager_Release(This)  (This)->lpVtbl->Release(This)
#define IInternetSecurityManager_SetSecuritySite(This,pSite)  (This)->lpVtbl->SetSecuritySite(This,pSite)
#define IInternetSecurityManager_GetSecuritySite(This,ppSite)  (This)->lpVtbl->GetSecuritySite(This,ppSite)
#define IInternetSecurityManager_MapUrlToZone(This,pwszUrl,pdwZone,dwFlags)  (This)->lpVtbl->MapUrlToZone(This,pwszUrl,pdwZone,dwFlags)
#define IInternetSecurityManager_GetSecurityId(This,pwszUrl,pbSecurityId,pcbSecurityId,dwReserved)  (This)->lpVtbl->GetSecurityId(This,pwszUrl,pbSecurityId,pcbSecurityId,dwReserved)
#define IInternetSecurityManager_ProcessUrlAction(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved)  (This)->lpVtbl->ProcessUrlAction(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved)
#define IInternetSecurityManager_QueryCustomPolicy(This,pwszUrl,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved)  (This)->lpVtbl->QueryCustomPolicy(This,pwszUrl,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved)
#define IInternetSecurityManager_SetZoneMapping(This,dwZone,lpszPattern,dwFlags)  (This)->lpVtbl->SetZoneMapping(This,dwZone,lpszPattern,dwFlags)
#define IInternetSecurityManager_GetZoneMappings(This,dwZone,ppenumString,dwFlags)  (This)->lpVtbl->GetZoneMappings(This,dwZone,ppenumString,dwFlags)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IInternetSecurityManager_SetSecuritySite_Proxy(IInternetSecurityManager*,IInternetSecurityMgrSite*);
void __RPC_STUB IInternetSecurityManager_SetSecuritySite_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSecurityManager_GetSecuritySite_Proxy(IInternetSecurityManager*,IInternetSecurityMgrSite**);
void __RPC_STUB IInternetSecurityManager_GetSecuritySite_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSecurityManager_MapUrlToZone_Proxy(IInternetSecurityManager*,LPCWSTR,DWORD*,DWORD);
void __RPC_STUB IInternetSecurityManager_MapUrlToZone_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSecurityManager_GetSecurityId_Proxy(IInternetSecurityManager*,LPCWSTR,BYTE*,DWORD*,DWORD_PTR);
void __RPC_STUB IInternetSecurityManager_GetSecurityId_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSecurityManager_ProcessUrlAction_Proxy(IInternetSecurityManager*,LPCWSTR,DWORD,BYTE*,DWORD,BYTE*,DWORD,DWORD,DWORD);
void __RPC_STUB IInternetSecurityManager_ProcessUrlAction_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSecurityManager_QueryCustomPolicy_Proxy(IInternetSecurityManager*,LPCWSTR,REFGUID,BYTE**,DWORD*,BYTE*,DWORD,DWORD);
void __RPC_STUB IInternetSecurityManager_QueryCustomPolicy_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSecurityManager_SetZoneMapping_Proxy(IInternetSecurityManager*,DWORD,LPCWSTR,DWORD);
void __RPC_STUB IInternetSecurityManager_SetZoneMapping_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetSecurityManager_GetZoneMappings_Proxy(IInternetSecurityManager*,DWORD,IEnumString**,DWORD);
void __RPC_STUB IInternetSecurityManager_GetZoneMappings_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IInternetSecurityManager_INTERFACE_DEFINED__ */
#endif


#if (_WIN32_IE >= _WIN32_IE_IE60SP2)

#ifndef _LPINTERNETSECURITYMANANGEREX_DEFINED
#define _LPINTERNETSECURITYMANANGEREX_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0038_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0038_v0_0_s_ifspec;

#ifndef __IInternetSecurityManagerEx_INTERFACE_DEFINED__
#define __IInternetSecurityManagerEx_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInternetSecurityManagerEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("F164EDF1-CC7C-4f0d-9A94-34222625C393") IInternetSecurityManagerEx : public IInternetSecurityManager
{
    public:
    virtual HRESULT STDMETHODCALLTYPE ProcessUrlActionEx(LPCWSTR pwszUrl,
        DWORD dwAction,
        BYTE * pPolicy,
        DWORD cbPolicy,
        BYTE * pContext,
        DWORD cbContext,
        DWORD dwFlags,
        DWORD dwReserved,
        DWORD * pdwOutFlags) = 0;

};
#else /* C style interface */
typedef struct IInternetSecurityManagerExVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IInternetSecurityManagerEx * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IInternetSecurityManagerEx * This);
    ULONG (STDMETHODCALLTYPE *Release)(IInternetSecurityManagerEx * This);
    HRESULT (STDMETHODCALLTYPE *SetSecuritySite)(IInternetSecurityManagerEx * This, IInternetSecurityMgrSite * pSite);
    HRESULT (STDMETHODCALLTYPE *GetSecuritySite)(IInternetSecurityManagerEx * This, IInternetSecurityMgrSite ** ppSite);
    HRESULT (STDMETHODCALLTYPE *MapUrlToZone)(IInternetSecurityManagerEx * This, LPCWSTR pwszUrl, DWORD * pdwZone, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *GetSecurityId)(IInternetSecurityManagerEx * This, LPCWSTR pwszUrl, BYTE * pbSecurityId, DWORD * pcbSecurityId, DWORD_PTR dwReserved);
    HRESULT (STDMETHODCALLTYPE *ProcessUrlAction)(IInternetSecurityManagerEx * This, LPCWSTR pwszUrl, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, BYTE * pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved);
    HRESULT (STDMETHODCALLTYPE *QueryCustomPolicy)(IInternetSecurityManagerEx * This, LPCWSTR pwszUrl, REFGUID guidKey, BYTE ** ppPolicy, DWORD * pcbPolicy, BYTE * pContext, DWORD cbContext, DWORD dwReserved);
    HRESULT (STDMETHODCALLTYPE *SetZoneMapping)(IInternetSecurityManagerEx * This, DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *GetZoneMappings)(IInternetSecurityManagerEx * This, DWORD dwZone, IEnumString ** ppenumString, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *ProcessUrlActionEx)(IInternetSecurityManagerEx * This, LPCWSTR pwszUrl, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, BYTE * pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved, DWORD * pdwOutFlags);
    END_INTERFACE
} IInternetSecurityManagerExVtbl;

interface IInternetSecurityManagerEx {
    CONST_VTBL struct IInternetSecurityManagerExVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetSecurityManagerEx_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IInternetSecurityManagerEx_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IInternetSecurityManagerEx_Release(This)  ((This)->lpVtbl->Release(This))
#define IInternetSecurityManagerEx_SetSecuritySite(This,pSite)  ((This)->lpVtbl->SetSecuritySite(This,pSite))
#define IInternetSecurityManagerEx_GetSecuritySite(This,ppSite)  ((This)->lpVtbl->GetSecuritySite(This,ppSite))
#define IInternetSecurityManagerEx_MapUrlToZone(This,pwszUrl,pdwZone,dwFlags)  ((This)->lpVtbl->MapUrlToZone(This,pwszUrl,pdwZone,dwFlags))
#define IInternetSecurityManagerEx_GetSecurityId(This,pwszUrl,pbSecurityId,pcbSecurityId,dwReserved)  ((This)->lpVtbl->GetSecurityId(This,pwszUrl,pbSecurityId,pcbSecurityId,dwReserved))
#define IInternetSecurityManagerEx_ProcessUrlAction(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved)  ((This)->lpVtbl->ProcessUrlAction(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved))
#define IInternetSecurityManagerEx_QueryCustomPolicy(This,pwszUrl,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved)  ((This)->lpVtbl->QueryCustomPolicy(This,pwszUrl,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved))
#define IInternetSecurityManagerEx_SetZoneMapping(This,dwZone,lpszPattern,dwFlags)  ((This)->lpVtbl->SetZoneMapping(This,dwZone,lpszPattern,dwFlags))
#define IInternetSecurityManagerEx_GetZoneMappings(This,dwZone,ppenumString,dwFlags)  ((This)->lpVtbl->GetZoneMappings(This,dwZone,ppenumString,dwFlags))
#define IInternetSecurityManagerEx_ProcessUrlActionEx(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved,pdwOutFlags)  ((This)->lpVtbl->ProcessUrlActionEx(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved,pdwOutFlags))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IInternetSecurityManagerEx_INTERFACE_DEFINED__ */

#endif
#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */

#if (_WIN32_IE >= _WIN32_IE_IE70)

#ifndef _LPINTERNETSECURITYMANANGEREx2_DEFINED
#define _LPINTERNETSECURITYMANANGEREx2_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0039_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0039_v0_0_s_ifspec;

#ifndef __IInternetSecurityManagerEx2_INTERFACE_DEFINED__
#define __IInternetSecurityManagerEx2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInternetSecurityManagerEx2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("F1E50292-A795-4117-8E09-2B560A72AC60") IInternetSecurityManagerEx2 : public IInternetSecurityManagerEx
{
    public:
    virtual HRESULT STDMETHODCALLTYPE MapUrlToZoneEx2(IUri * pUri,
        DWORD * pdwZone,
        DWORD dwFlags,
        LPWSTR * ppwszMappedUrl,
        DWORD * pdwOutFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE ProcessUrlActionEx2(IUri * pUri, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, BYTE * pContext, DWORD cbContext, DWORD dwFlags, DWORD_PTR dwReserved, DWORD * pdwOutFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSecurityIdEx2(IUri * pUri, BYTE * pbSecurityId, DWORD * pcbSecurityId, DWORD_PTR dwReserved) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryCustomPolicyEx2(IUri * pUri, REFGUID guidKey, BYTE ** ppPolicy, DWORD * pcbPolicy, BYTE * pContext, DWORD cbContext, DWORD_PTR dwReserved) = 0;
};
#else /* C style interface */
typedef struct IInternetSecurityManagerEx2Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IInternetSecurityManagerEx2 * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IInternetSecurityManagerEx2 * This);
    ULONG (STDMETHODCALLTYPE *Release)(IInternetSecurityManagerEx2 * This);
    HRESULT (STDMETHODCALLTYPE *SetSecuritySite)(IInternetSecurityManagerEx2 * This, IInternetSecurityMgrSite * pSite);
    HRESULT (STDMETHODCALLTYPE *GetSecuritySite)(IInternetSecurityManagerEx2 * This, IInternetSecurityMgrSite ** ppSite);
    HRESULT (STDMETHODCALLTYPE *MapUrlToZone)(IInternetSecurityManagerEx2 * This, LPCWSTR pwszUrl, DWORD * pdwZone, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *GetSecurityId)(IInternetSecurityManagerEx2 * This, LPCWSTR pwszUrl, BYTE * pbSecurityId, DWORD * pcbSecurityId, DWORD_PTR dwReserved);
    HRESULT (STDMETHODCALLTYPE *ProcessUrlAction)(IInternetSecurityManagerEx2 * This, LPCWSTR pwszUrl, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, BYTE * pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved);
    HRESULT (STDMETHODCALLTYPE *QueryCustomPolicy)(IInternetSecurityManagerEx2 * This, LPCWSTR pwszUrl, REFGUID guidKey, BYTE ** ppPolicy, DWORD * pcbPolicy, BYTE * pContext, DWORD cbContext, DWORD dwReserved);
    HRESULT (STDMETHODCALLTYPE *SetZoneMapping)(IInternetSecurityManagerEx2 * This, DWORD dwZone, LPCWSTR lpszPattern, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *GetZoneMappings)(IInternetSecurityManagerEx2 * This, DWORD dwZone, IEnumString ** ppenumString, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *ProcessUrlActionEx)(IInternetSecurityManagerEx2 * This, LPCWSTR pwszUrl, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, BYTE * pContext, DWORD cbContext, DWORD dwFlags, DWORD dwReserved, DWORD * pdwOutFlags);
    HRESULT (STDMETHODCALLTYPE *MapUrlToZoneEx2)(IInternetSecurityManagerEx2 * This, IUri * pUri, DWORD * pdwZone, DWORD dwFlags, LPWSTR * ppwszMappedUrl, DWORD * pdwOutFlags);
    HRESULT (STDMETHODCALLTYPE *ProcessUrlActionEx2)(IInternetSecurityManagerEx2 * This, IUri * pUri, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, BYTE * pContext, DWORD cbContext, DWORD dwFlags, DWORD_PTR dwReserved, DWORD * pdwOutFlags);
    HRESULT (STDMETHODCALLTYPE *GetSecurityIdEx2)(IInternetSecurityManagerEx2 * This, IUri * pUri, BYTE * pbSecurityId, DWORD * pcbSecurityId, DWORD_PTR dwReserved);
    HRESULT (STDMETHODCALLTYPE *QueryCustomPolicyEx2)(IInternetSecurityManagerEx2 * This, IUri * pUri, REFGUID guidKey, BYTE ** ppPolicy, DWORD * pcbPolicy, BYTE * pContext, DWORD cbContext, DWORD_PTR dwReserved);
    END_INTERFACE
} IInternetSecurityManagerEx2Vtbl;

interface IInternetSecurityManagerEx2 {
    CONST_VTBL struct IInternetSecurityManagerEx2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetSecurityManagerEx2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IInternetSecurityManagerEx2_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IInternetSecurityManagerEx2_Release(This)  ((This)->lpVtbl->Release(This))
#define IInternetSecurityManagerEx2_SetSecuritySite(This,pSite)  ((This)->lpVtbl->SetSecuritySite(This,pSite))
#define IInternetSecurityManagerEx2_GetSecuritySite(This,ppSite)  ((This)->lpVtbl->GetSecuritySite(This,ppSite))
#define IInternetSecurityManagerEx2_MapUrlToZone(This,pwszUrl,pdwZone,dwFlags)  ((This)->lpVtbl->MapUrlToZone(This,pwszUrl,pdwZone,dwFlags))
#define IInternetSecurityManagerEx2_GetSecurityId(This,pwszUrl,pbSecurityId,pcbSecurityId,dwReserved)  ((This)->lpVtbl->GetSecurityId(This,pwszUrl,pbSecurityId,pcbSecurityId,dwReserved))
#define IInternetSecurityManagerEx2_ProcessUrlAction(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved)  ((This)->lpVtbl->ProcessUrlAction(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved))
#define IInternetSecurityManagerEx2_QueryCustomPolicy(This,pwszUrl,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved)  ((This)->lpVtbl->QueryCustomPolicy(This,pwszUrl,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved))
#define IInternetSecurityManagerEx2_SetZoneMapping(This,dwZone,lpszPattern,dwFlags)  ((This)->lpVtbl->SetZoneMapping(This,dwZone,lpszPattern,dwFlags))
#define IInternetSecurityManagerEx2_GetZoneMappings(This,dwZone,ppenumString,dwFlags)  ((This)->lpVtbl->GetZoneMappings(This,dwZone,ppenumString,dwFlags))
#define IInternetSecurityManagerEx2_ProcessUrlActionEx(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved,pdwOutFlags)  ((This)->lpVtbl->ProcessUrlActionEx(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved,pdwOutFlags))
#define IInternetSecurityManagerEx2_MapUrlToZoneEx2(This,pUri,pdwZone,dwFlags,ppwszMappedUrl,pdwOutFlags)  ((This)->lpVtbl->MapUrlToZoneEx2(This,pUri,pdwZone,dwFlags,ppwszMappedUrl,pdwOutFlags))
#define IInternetSecurityManagerEx2_ProcessUrlActionEx2(This,pUri,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved,pdwOutFlags)  ((This)->lpVtbl->ProcessUrlActionEx2(This,pUri,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved,pdwOutFlags))
#define IInternetSecurityManagerEx2_GetSecurityIdEx2(This,pUri,pbSecurityId,pcbSecurityId,dwReserved)  ((This)->lpVtbl->GetSecurityIdEx2(This,pUri,pbSecurityId,pcbSecurityId,dwReserved))
#define IInternetSecurityManagerEx2_QueryCustomPolicyEx2(This,pUri,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved)  ((This)->lpVtbl->QueryCustomPolicyEx2(This,pUri,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IInternetSecurityManagerEx2_INTERFACE_DEFINED__ */

#endif
#endif /*_WIN32_IE >= _WIN32_IE_IE70 */

#if (_WIN32_IE >= _WIN32_IE_IE60SP2)

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0040_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0040_v0_0_s_ifspec;

#ifndef __IZoneIdentifier_INTERFACE_DEFINED__
#define __IZoneIdentifier_INTERFACE_DEFINED__

EXTERN_C const IID IID_IZoneIdentifier;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("cd45f185-1b21-48e2-967b-ead743a8914e") IZoneIdentifier : public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetId(DWORD * pdwZone) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetId(DWORD dwZone) = 0;
    virtual HRESULT STDMETHODCALLTYPE Remove(void) = 0;
};
#else /* C style interface */

typedef struct IZoneIdentifierVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IZoneIdentifier * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IZoneIdentifier * This);
    ULONG (STDMETHODCALLTYPE *Release)(IZoneIdentifier * This);
    HRESULT (STDMETHODCALLTYPE *GetId)(IZoneIdentifier * This, DWORD * pdwZone);
    HRESULT (STDMETHODCALLTYPE *SetId)(IZoneIdentifier * This, DWORD dwZone);
    HRESULT (STDMETHODCALLTYPE *Remove)(IZoneIdentifier * This);
    END_INTERFACE
} IZoneIdentifierVtbl;

interface IZoneIdentifier {
    CONST_VTBL struct IZoneIdentifierVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IZoneIdentifier_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IZoneIdentifier_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IZoneIdentifier_Release(This)  ((This)->lpVtbl->Release(This))
#define IZoneIdentifier_GetId(This,pdwZone)  ((This)->lpVtbl->GetId(This,pdwZone))
#define IZoneIdentifier_SetId(This,dwZone)  ((This)->lpVtbl->SetId(This,dwZone))
#define IZoneIdentifier_Remove(This)  ((This)->lpVtbl->Remove(This))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IZoneIdentifier_INTERFACE_DEFINED__ */

#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */


#ifndef _LPINTERNETHOSTSECURITYMANANGER_DEFINED
#define _LPINTERNETHOSTSECURITYMANANGER_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0191_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0191_v0_0_s_ifspec;

#ifndef __IInternetHostSecurityManager_INTERFACE_DEFINED__
#define __IInternetHostSecurityManager_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInternetHostSecurityManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3af280b6-cb3f-11d0-891e-00c04fb6bfc4")
IInternetHostSecurityManager:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetSecurityId(BYTE*,DWORD*,DWORD_PTR) = 0;
    virtual HRESULT STDMETHODCALLTYPE ProcessUrlAction(DWORD,BYTE*,DWORD,BYTE*,DWORD,DWORD,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE QueryCustomPolicy(REFGUID,BYTE**,DWORD*,BYTE*,DWORD,DWORD) = 0;
};
#else
typedef struct IInternetHostSecurityManagerVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternetHostSecurityManager*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternetHostSecurityManager*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternetHostSecurityManager*);
    HRESULT(STDMETHODCALLTYPE *GetSecurityId)(IInternetHostSecurityManager*,BYTE*,DWORD*,DWORD_PTR);
    HRESULT(STDMETHODCALLTYPE *ProcessUrlAction)(IInternetHostSecurityManager*,DWORD,BYTE*,DWORD,BYTE*,DWORD,DWORD,DWORD);
    HRESULT(STDMETHODCALLTYPE *QueryCustomPolicy)(IInternetHostSecurityManager*,REFGUID,BYTE**,DWORD*,BYTE*,DWORD,DWORD);
    END_INTERFACE
} IInternetHostSecurityManagerVtbl;

interface IInternetHostSecurityManager {
    CONST_VTBL struct IInternetHostSecurityManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetHostSecurityManager_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternetHostSecurityManager_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternetHostSecurityManager_Release(This)  (This)->lpVtbl->Release(This)
#define IInternetHostSecurityManager_GetSecurityId(This,pbSecurityId,pcbSecurityId,dwReserved)  (This)->lpVtbl->GetSecurityId(This,pbSecurityId,pcbSecurityId,dwReserved)
#define IInternetHostSecurityManager_ProcessUrlAction(This,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved)  (This)->lpVtbl->ProcessUrlAction(This,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved)
#define IInternetHostSecurityManager_QueryCustomPolicy(This,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved)  (This)->lpVtbl->QueryCustomPolicy(This,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IInternetHostSecurityManager_GetSecurityId_Proxy(IInternetHostSecurityManager*,BYTE*,DWORD*,DWORD_PTR);
void __RPC_STUB IInternetHostSecurityManager_GetSecurityId_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetHostSecurityManager_ProcessUrlAction_Proxy(IInternetHostSecurityManager*,DWORD,BYTE*,DWORD,BYTE*,DWORD,DWORD,DWORD);
void __RPC_STUB IInternetHostSecurityManager_ProcessUrlAction_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetHostSecurityManager_QueryCustomPolicy_Proxy(IInternetHostSecurityManager*,REFGUID,BYTE**,DWORD*,BYTE*,DWORD,DWORD);
void __RPC_STUB IInternetHostSecurityManager_QueryCustomPolicy_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IInternetHostSecurityManager_INTERFACE_DEFINED__ */
#endif


#define URLACTION_MIN  0x00001000
#define URLACTION_DOWNLOAD_MIN  0x00001000
#define URLACTION_DOWNLOAD_SIGNED_ACTIVEX  0x00001001
#define URLACTION_DOWNLOAD_UNSIGNED_ACTIVEX  0x00001004
#define URLACTION_DOWNLOAD_CURR_MAX  0x00001004
#define URLACTION_DOWNLOAD_MAX  0x000011FF
#define URLACTION_ACTIVEX_MIN  0x00001200
#define URLACTION_ACTIVEX_RUN  0x00001200
#define URLPOLICY_ACTIVEX_CHECK_LIST  0x00010000
#define URLACTION_ACTIVEX_OVERRIDE_OBJECT_SAFETY  0x00001201
#define URLACTION_ACTIVEX_OVERRIDE_DATA_SAFETY  0x00001202
#define URLACTION_ACTIVEX_OVERRIDE_SCRIPT_SAFETY  0x00001203
#define URLACTION_SCRIPT_OVERRIDE_SAFETY  0x00001401
#define URLACTION_ACTIVEX_CONFIRM_NOOBJECTSAFETY  0x00001204
#define URLACTION_ACTIVEX_TREATASUNTRUSTED  0x00001205
#define URLACTION_ACTIVEX_CURR_MAX  0x00001205
#define URLACTION_ACTIVEX_MAX  0x000013ff
#define URLACTION_SCRIPT_MIN  0x00001400
#define URLACTION_SCRIPT_RUN  0x00001400
#define URLACTION_SCRIPT_JAVA_USE  0x00001402
#define URLACTION_SCRIPT_SAFE_ACTIVEX  0x00001405
#define URLACTION_CROSS_DOMAIN_DATA  0x00001406
#define URLACTION_SCRIPT_PASTE  0x00001407
#define URLACTION_SCRIPT_CURR_MAX  0x00001407
#define URLACTION_SCRIPT_MAX  0x000015ff
#define URLACTION_HTML_MIN  0x00001600
#define URLACTION_HTML_SUBMIT_FORMS  0x00001601
#define URLACTION_HTML_SUBMIT_FORMS_FROM  0x00001602
#define URLACTION_HTML_SUBMIT_FORMS_TO  0x00001603
#define URLACTION_HTML_FONT_DOWNLOAD  0x00001604
#define URLACTION_HTML_JAVA_RUN  0x00001605
#define URLACTION_HTML_USERDATA_SAVE  0x00001606
#define URLACTION_HTML_SUBFRAME_NAVIGATE  0x00001607
#define URLACTION_HTML_CURR_MAX  0x00001607
#define URLACTION_HTML_MAX  0x000017ff
#define URLACTION_SHELL_MIN  0x00001800
#define URLACTION_SHELL_INSTALL_DTITEMS  0x00001800
#define URLACTION_SHELL_MOVE_OR_COPY  0x00001802
#define URLACTION_SHELL_FILE_DOWNLOAD  0x00001803
#define URLACTION_SHELL_VERB  0x00001804
#define URLACTION_SHELL_WEBVIEW_VERB  0x00001805
#define URLACTION_SHELL_CURR_MAX  0x00001805
#define URLACTION_SHELL_SHELLEXECUTE  0x00001806
#if (_WIN32_IE >= _WIN32_IE_IE60SP2)
#define URLACTION_SHELL_EXECUTE_HIGHRISK  0x00001806
#define URLACTION_SHELL_EXECUTE_MODRISK  0x00001807
#define URLACTION_SHELL_EXECUTE_LOWRISK  0x00001808
#define URLACTION_SHELL_POPUPMGR  0x00001809
#define URLACTION_SHELL_RTF_OBJECTS_LOAD  0x0000180A
#define URLACTION_SHELL_ENHANCED_DRAGDROP_SECURITY  0x0000180B
#define URLACTION_SHELL_EXTENSIONSECURITY  0x0000180C
#define URLACTION_SHELL_SECURE_DRAGSOURCE  0x0000180D
#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */
#if (_WIN32_IE >= _WIN32_IE_WIN7)
#define URLACTION_SHELL_REMOTEQUERY  0x0000180E
#define URLACTION_SHELL_PREVIEW  0x0000180F
#endif /* _WIN32_IE >= _WIN32_IE_WIN7 */

#define URLACTION_SHELL_MAX  0x000019ff
#define URLACTION_NETWORK_MIN  0x00001A00
#define URLACTION_CREDENTIALS_USE  0x00001A00
#define URLPOLICY_CREDENTIALS_SILENT_LOGON_OK  0x00000000
#define URLPOLICY_CREDENTIALS_MUST_PROMPT_USER  0x00010000
#define URLPOLICY_CREDENTIALS_CONDITIONAL_PROMPT  0x00020000
#define URLPOLICY_CREDENTIALS_ANONYMOUS_ONLY  0x00030000
#define URLACTION_AUTHENTICATE_CLIENT  0x00001A01
#define URLPOLICY_AUTHENTICATE_CLEARTEXT_OK  0x00000000
#define URLPOLICY_AUTHENTICATE_CHALLENGE_RESPONSE  0x00010000
#define URLPOLICY_AUTHENTICATE_MUTUAL_ONLY  0x00030000
#define URLACTION_COOKIES  0x00001A02
#define URLACTION_COOKIES_SESSION  0x00001A03
#define URLACTION_NETWORK_CURR_MAX  0x00001A03
#define URLACTION_NETWORK_MAX  0x00001Bff
#define URLACTION_JAVA_MIN  0x00001C00
#define URLACTION_JAVA_PERMISSIONS  0x00001C00
#define URLPOLICY_JAVA_PROHIBIT  0x00000000
#define URLPOLICY_JAVA_HIGH  0x00010000
#define URLPOLICY_JAVA_MEDIUM  0x00020000
#define URLPOLICY_JAVA_LOW  0x00030000
#define URLPOLICY_JAVA_CUSTOM  0x00800000
#define URLACTION_JAVA_CURR_MAX  0x00001C00
#define URLACTION_JAVA_MAX  0x00001Cff
#define URLACTION_INFODELIVERY_MIN  0x00001D00
#define URLACTION_INFODELIVERY_NO_ADDING_CHANNELS  0x00001D00
#define URLACTION_INFODELIVERY_NO_EDITING_CHANNELS  0x00001D01
#define URLACTION_INFODELIVERY_NO_REMOVING_CHANNELS  0x00001D02
#define URLACTION_INFODELIVERY_NO_ADDING_SUBSCRIPTIONS  0x00001D03
#define URLACTION_INFODELIVERY_NO_EDITING_SUBSCRIPTIONS  0x00001D04
#define URLACTION_INFODELIVERY_NO_REMOVING_SUBSCRIPTIONS  0x00001D05
#define URLACTION_INFODELIVERY_NO_CHANNEL_LOGGING  0x00001D06
#define URLACTION_INFODELIVERY_CURR_MAX  0x00001D06
#define URLACTION_INFODELIVERY_MAX  0x00001Dff
#define URLACTION_CHANNEL_SOFTDIST_MIN  0x00001E00
#define URLACTION_CHANNEL_SOFTDIST_PERMISSIONS  0x00001E05
#define URLPOLICY_CHANNEL_SOFTDIST_PROHIBIT  0x00010000
#define URLPOLICY_CHANNEL_SOFTDIST_PRECACHE  0x00020000
#define URLPOLICY_CHANNEL_SOFTDIST_AUTOINSTALL  0x00030000
#define URLACTION_CHANNEL_SOFTDIST_MAX  0x00001Eff
#if (_WIN32_IE >= _WIN32_IE_IE80)
#define URLACTION_DOTNET_USERCONTROLS  0x00002005
#endif /* _WIN32_IE >= _WIN32_IE_IE80 */
#if (_WIN32_IE >= _WIN32_IE_IE60SP2)
#define URLACTION_BEHAVIOR_MIN  0x00002000
#define URLACTION_BEHAVIOR_RUN  0x00002000
#define URLPOLICY_BEHAVIOR_CHECK_LIST  0x00010000
#define URLACTION_FEATURE_MIN  0x00002100
#define URLACTION_FEATURE_MIME_SNIFFING  0x00002100
#define URLACTION_FEATURE_ZONE_ELEVATION  0x00002101
#define URLACTION_FEATURE_WINDOW_RESTRICTIONS  0x00002102
#define URLACTION_FEATURE_SCRIPT_STATUS_BAR  0x00002103
#define URLACTION_FEATURE_FORCE_ADDR_AND_STATUS  0x00002104
#define URLACTION_FEATURE_BLOCK_INPUT_PROMPTS  0x00002105
#define URLACTION_FEATURE_DATA_BINDING  0x00002106
#define URLACTION_AUTOMATIC_DOWNLOAD_UI_MIN  0x00002200
#define URLACTION_AUTOMATIC_DOWNLOAD_UI  0x00002200
#define URLACTION_AUTOMATIC_ACTIVEX_UI  0x00002201
#define URLACTION_ALLOW_RESTRICTEDPROTOCOLS  0x00002300
#endif /*_WIN32_IE >= _WIN32_IE_IE60SP2 */
#if (_WIN32_IE >= _WIN32_IE_IE70)
#define URLACTION_ALLOW_APEVALUATION  0x00002301
#define URLACTION_WINDOWS_BROWSER_APPLICATIONS  0x00002400
#define URLACTION_XPS_DOCUMENTS  0x00002401
#define URLACTION_LOOSE_XAML  0x00002402
#define URLACTION_LOWRIGHTS  0x00002500
#define URLACTION_WINFX_SETUP  0x00002600
#define URLACTION_INPRIVATE_BLOCKING  0x00002700
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */

#define URLPOLICY_ALLOW  0x00
#define URLPOLICY_QUERY  0x01
#define URLPOLICY_DISALLOW  0x03
#define URLPOLICY_NOTIFY_ON_ALLOW  0x10
#define URLPOLICY_NOTIFY_ON_DISALLOW  0x20
#define URLPOLICY_LOG_ON_ALLOW  0x40
#define URLPOLICY_LOG_ON_DISALLOW  0x80
#define URLPOLICY_MASK_PERMISSIONS  0x0f

#define GetUrlPolicyPermissions(dw)  (dw & URLPOLICY_MASK_PERMISSIONS)
#define SetUrlPolicyPermissions(dw,dw2)  ((dw) = ((dw) &~(URLPOLICY_MASK_PERMISSIONS))|(dw2))
#define URLPOLICY_DONTCHECKDLGBOX  0x100
#if (_WIN32_IE >= _WIN32_IE_IE60SP2)
EXTERN_C const GUID GUID_CUSTOM_LOCALMACHINEZONEUNLOCKED; 
#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */


#ifndef _LPINTERNETZONEMANAGER_DEFINED
#define _LPINTERNETZONEMANAGER_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0192_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0192_v0_0_s_ifspec;

#ifndef __IInternetZoneManager_INTERFACE_DEFINED__
#define __IInternetZoneManager_INTERFACE_DEFINED__

typedef IInternetZoneManager *LPURLZONEMANAGER;

typedef enum tagURLZONE {
    URLZONE_PREDEFINED_MIN = 0,
    URLZONE_LOCAL_MACHINE = 0,
    URLZONE_INTRANET = URLZONE_LOCAL_MACHINE+1,
    URLZONE_TRUSTED = URLZONE_INTRANET+1,
    URLZONE_INTERNET = URLZONE_TRUSTED+1,
    URLZONE_UNTRUSTED = URLZONE_INTERNET+1,
    URLZONE_PREDEFINED_MAX = 999,
    URLZONE_USER_MIN = 1000,
    URLZONE_USER_MAX = 10000
} URLZONE;

typedef enum tagURLTEMPLATE {
    URLTEMPLATE_CUSTOM = 0,
    URLTEMPLATE_PREDEFINED_MIN = 0x10000,
    URLTEMPLATE_LOW = 0x10000,
    URLTEMPLATE_MEDLOW = 0x10500,
    URLTEMPLATE_MEDIUM = 0x11000,
    URLTEMPLATE_HIGH = 0x12000,
    URLTEMPLATE_PREDEFINED_MAX = 0x20000
} URLTEMPLATE;

enum __MIDL_IInternetZoneManager_0001 {
    MAX_ZONE_PATH = 260,
    MAX_ZONE_DESCRIPTION = 200
};

typedef enum __MIDL_IInternetZoneManager_0002 {
    ZAFLAGS_CUSTOM_EDIT = 0x1,
    ZAFLAGS_ADD_SITES = 0x2,
    ZAFLAGS_REQUIRE_VERIFICATION = 0x4,
    ZAFLAGS_INCLUDE_PROXY_OVERRIDE = 0x8,
    ZAFLAGS_INCLUDE_INTRANET_SITES = 0x10,
    ZAFLAGS_NO_UI = 0x20,
    ZAFLAGS_SUPPORTS_VERIFICATION = 0x40,
    ZAFLAGS_UNC_AS_INTRANET = 0x80
} ZAFLAGS;

typedef struct _ZONEATTRIBUTES {
    ULONG cbSize;
    WCHAR szDisplayName[260];
    WCHAR szDescription[200];
    WCHAR szIconPath[260];
    DWORD dwTemplateMinLevel;
    DWORD dwTemplateRecommended;
    DWORD dwTemplateCurrentLevel;
    DWORD dwFlags;
} ZONEATTRIBUTES,*LPZONEATTRIBUTES;

typedef enum _URLZONEREG {
    URLZONEREG_DEFAULT = 0,
    URLZONEREG_HKLM = URLZONEREG_DEFAULT+1,
    URLZONEREG_HKCU = URLZONEREG_HKLM+1
} URLZONEREG;

EXTERN_C const IID IID_IInternetZoneManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79eac9ef-baf9-11ce-8c82-00aa004ba90b")
IInternetZoneManager:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetZoneAttributes(DWORD,ZONEATTRIBUTES*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetZoneAttributes(DWORD,ZONEATTRIBUTES*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetZoneCustomPolicy(DWORD,REFGUID,BYTE**,DWORD*,URLZONEREG) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetZoneCustomPolicy(DWORD,REFGUID,BYTE*,DWORD,URLZONEREG) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetZoneActionPolicy(DWORD,DWORD,BYTE*,DWORD,URLZONEREG) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetZoneActionPolicy(DWORD,DWORD,BYTE*,DWORD,URLZONEREG) = 0;
    virtual HRESULT STDMETHODCALLTYPE PromptAction(DWORD,HWND,LPCWSTR,LPCWSTR,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE LogAction(DWORD,LPCWSTR,LPCWSTR,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateZoneEnumerator(DWORD*,DWORD*,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetZoneAt(DWORD,DWORD,DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE DestroyZoneEnumerator(DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyTemplatePoliciesToZone(DWORD,DWORD,DWORD) = 0;
};
#else
typedef struct IInternetZoneManagerVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInternetZoneManager*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IInternetZoneManager*);
    ULONG(STDMETHODCALLTYPE *Release)(IInternetZoneManager*);
    HRESULT(STDMETHODCALLTYPE *GetZoneAttributes)(IInternetZoneManager*,DWORD,ZONEATTRIBUTES*);
    HRESULT(STDMETHODCALLTYPE *SetZoneAttributes)(IInternetZoneManager*,DWORD,ZONEATTRIBUTES*);
    HRESULT(STDMETHODCALLTYPE *GetZoneCustomPolicy)(IInternetZoneManager*,DWORD,REFGUID,BYTE**,DWORD*,URLZONEREG);
    HRESULT(STDMETHODCALLTYPE *SetZoneCustomPolicy)(IInternetZoneManager*,DWORD,REFGUID,BYTE*,DWORD,URLZONEREG);
    HRESULT(STDMETHODCALLTYPE *GetZoneActionPolicy)(IInternetZoneManager*,DWORD,DWORD,BYTE*,DWORD,URLZONEREG);
    HRESULT(STDMETHODCALLTYPE *SetZoneActionPolicy)(IInternetZoneManager*,DWORD,DWORD,BYTE*,DWORD,URLZONEREG);
    HRESULT(STDMETHODCALLTYPE *PromptAction)(IInternetZoneManager*,DWORD,HWND hwndParent,LPCWSTR,LPCWSTR,DWORD);
    HRESULT(STDMETHODCALLTYPE *LogAction)(IInternetZoneManager*,DWORD,LPCWSTR,LPCWSTR,DWORD);
    HRESULT(STDMETHODCALLTYPE *CreateZoneEnumerator)(IInternetZoneManager*,DWORD*,DWORD*,DWORD);
    HRESULT(STDMETHODCALLTYPE *GetZoneAt)(IInternetZoneManager*,DWORD,DWORD,DWORD*);
    HRESULT(STDMETHODCALLTYPE *DestroyZoneEnumerator)(IInternetZoneManager*,DWORD);
    HRESULT(STDMETHODCALLTYPE *CopyTemplatePoliciesToZone)(IInternetZoneManager*,DWORD,DWORD,DWORD);
    END_INTERFACE
} IInternetZoneManagerVtbl;

interface IInternetZoneManager {
    CONST_VTBL struct IInternetZoneManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetZoneManager_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IInternetZoneManager_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IInternetZoneManager_Release(This)  (This)->lpVtbl->Release(This)
#define IInternetZoneManager_GetZoneAttributes(This,dwZone,pZoneAttributes)  (This)->lpVtbl->GetZoneAttributes(This,dwZone,pZoneAttributes)
#define IInternetZoneManager_SetZoneAttributes(This,dwZone,pZoneAttributes)  (This)->lpVtbl->SetZoneAttributes(This,dwZone,pZoneAttributes)
#define IInternetZoneManager_GetZoneCustomPolicy(This,dwZone,guidKey,ppPolicy,pcbPolicy,urlZoneReg)  (This)->lpVtbl->GetZoneCustomPolicy(This,dwZone,guidKey,ppPolicy,pcbPolicy,urlZoneReg)
#define IInternetZoneManager_SetZoneCustomPolicy(This,dwZone,guidKey,pPolicy,cbPolicy,urlZoneReg)  (This)->lpVtbl->SetZoneCustomPolicy(This,dwZone,guidKey,pPolicy,cbPolicy,urlZoneReg)
#define IInternetZoneManager_GetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)  (This)->lpVtbl->GetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)
#define IInternetZoneManager_SetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)  (This)->lpVtbl->SetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)
#define IInternetZoneManager_PromptAction(This,dwAction,hwndParent,pwszUrl,pwszText,dwPromptFlags)  (This)->lpVtbl->PromptAction(This,dwAction,hwndParent,pwszUrl,pwszText,dwPromptFlags)
#define IInternetZoneManager_LogAction(This,dwAction,pwszUrl,pwszText,dwLogFlags)  (This)->lpVtbl->LogAction(This,dwAction,pwszUrl,pwszText,dwLogFlags)
#define IInternetZoneManager_CreateZoneEnumerator(This,pdwEnum,pdwCount,dwFlags)  (This)->lpVtbl->CreateZoneEnumerator(This,pdwEnum,pdwCount,dwFlags)
#define IInternetZoneManager_GetZoneAt(This,dwEnum,dwIndex,pdwZone)  (This)->lpVtbl->GetZoneAt(This,dwEnum,dwIndex,pdwZone)
#define IInternetZoneManager_DestroyZoneEnumerator(This,dwEnum)  (This)->lpVtbl->DestroyZoneEnumerator(This,dwEnum)
#define IInternetZoneManager_CopyTemplatePoliciesToZone(This,dwTemplate,dwZone,dwReserved)  (This)->lpVtbl->CopyTemplatePoliciesToZone(This,dwTemplate,dwZone,dwReserved)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IInternetZoneManager_GetZoneAttributes_Proxy(IInternetZoneManager*,DWORD,ZONEATTRIBUTES *);
void __RPC_STUB IInternetZoneManager_GetZoneAttributes_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetZoneManager_SetZoneAttributes_Proxy(IInternetZoneManager*,DWORD,ZONEATTRIBUTES *);
void __RPC_STUB IInternetZoneManager_SetZoneAttributes_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetZoneManager_GetZoneCustomPolicy_Proxy(IInternetZoneManager*,DWORD,REFGUID,BYTE**,DWORD*,URLZONEREG);
void __RPC_STUB IInternetZoneManager_GetZoneCustomPolicy_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetZoneManager_SetZoneCustomPolicy_Proxy(IInternetZoneManager*,DWORD,REFGUID,BYTE*,DWORD,URLZONEREG);
void __RPC_STUB IInternetZoneManager_SetZoneCustomPolicy_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetZoneManager_GetZoneActionPolicy_Proxy(IInternetZoneManager*,DWORD,DWORD,BYTE*,DWORD,URLZONEREG);
void __RPC_STUB IInternetZoneManager_GetZoneActionPolicy_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetZoneManager_SetZoneActionPolicy_Proxy(IInternetZoneManager*,DWORD,DWORD,BYTE*,DWORD,URLZONEREG);
void __RPC_STUB IInternetZoneManager_SetZoneActionPolicy_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetZoneManager_PromptAction_Proxy(IInternetZoneManager*,DWORD,HWND,LPCWSTR,LPCWSTR,DWORD);
void __RPC_STUB IInternetZoneManager_PromptAction_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetZoneManager_LogAction_Proxy(IInternetZoneManager*,DWORD,LPCWSTR,LPCWSTR,DWORD);
void __RPC_STUB IInternetZoneManager_LogAction_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetZoneManager_CreateZoneEnumerator_Proxy(IInternetZoneManager*,DWORD*,DWORD*,DWORD);
void __RPC_STUB IInternetZoneManager_CreateZoneEnumerator_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetZoneManager_GetZoneAt_Proxy(IInternetZoneManager*,DWORD,DWORD,DWORD*);
void __RPC_STUB IInternetZoneManager_GetZoneAt_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetZoneManager_DestroyZoneEnumerator_Proxy(IInternetZoneManager*,DWORD);
void __RPC_STUB IInternetZoneManager_DestroyZoneEnumerator_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IInternetZoneManager_CopyTemplatePoliciesToZone_Proxy(IInternetZoneManager*,DWORD,DWORD,DWORD);
void __RPC_STUB IInternetZoneManager_CopyTemplatePoliciesToZone_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IInternetZoneManager_INTERFACE_DEFINED__ */
#endif


#if (_WIN32_IE >= _WIN32_IE_IE60SP2)

#ifndef _LPINTERNETZONEMANAGEREX_DEFINED
#define _LPINTERNETZONEMANAGEREX_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0043_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0043_v0_0_s_ifspec;

#ifndef __IInternetZoneManagerEx_INTERFACE_DEFINED__
#define __IInternetZoneManagerEx_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInternetZoneManagerEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A4C23339-8E06-431e-9BF4-7E711C085648") IInternetZoneManagerEx : public IInternetZoneManager
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetZoneActionPolicyEx(DWORD dwZone,
        DWORD dwAction,
        BYTE * pPolicy,
        DWORD cbPolicy,
        URLZONEREG urlZoneReg,
        DWORD dwFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetZoneActionPolicyEx(DWORD dwZone, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, URLZONEREG urlZoneReg, DWORD dwFlags) = 0;
};
#else /* C style interface */

typedef struct IInternetZoneManagerExVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IInternetZoneManagerEx * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IInternetZoneManagerEx * This);
    ULONG (STDMETHODCALLTYPE *Release)(IInternetZoneManagerEx * This);
    HRESULT (STDMETHODCALLTYPE *GetZoneAttributes)(IInternetZoneManagerEx * This, DWORD dwZone, ZONEATTRIBUTES * pZoneAttributes);
    HRESULT (STDMETHODCALLTYPE *SetZoneAttributes)(IInternetZoneManagerEx * This, DWORD dwZone, ZONEATTRIBUTES * pZoneAttributes);
    HRESULT (STDMETHODCALLTYPE *GetZoneCustomPolicy)(IInternetZoneManagerEx * This, DWORD dwZone, REFGUID guidKey, BYTE ** ppPolicy, DWORD * pcbPolicy, URLZONEREG urlZoneReg);
    HRESULT (STDMETHODCALLTYPE *SetZoneCustomPolicy)(IInternetZoneManagerEx * This, DWORD dwZone, REFGUID guidKey, BYTE * pPolicy, DWORD cbPolicy, URLZONEREG urlZoneReg);
    HRESULT (STDMETHODCALLTYPE *GetZoneActionPolicy)(IInternetZoneManagerEx * This, DWORD dwZone, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, URLZONEREG urlZoneReg);
    HRESULT (STDMETHODCALLTYPE *SetZoneActionPolicy)(IInternetZoneManagerEx * This, DWORD dwZone, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, URLZONEREG urlZoneReg);
    HRESULT (STDMETHODCALLTYPE *PromptAction)(IInternetZoneManagerEx * This, DWORD dwAction, HWND hwndParent, LPCWSTR pwszUrl, LPCWSTR pwszText, DWORD dwPromptFlags);
    HRESULT (STDMETHODCALLTYPE *LogAction)(IInternetZoneManagerEx * This, DWORD dwAction, LPCWSTR pwszUrl, LPCWSTR pwszText, DWORD dwLogFlags);
    HRESULT (STDMETHODCALLTYPE *CreateZoneEnumerator)(IInternetZoneManagerEx * This, DWORD * pdwEnum, DWORD * pdwCount, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *GetZoneAt)(IInternetZoneManagerEx * This, DWORD dwEnum, DWORD dwIndex, DWORD * pdwZone);
    HRESULT (STDMETHODCALLTYPE *DestroyZoneEnumerator)(IInternetZoneManagerEx * This, DWORD dwEnum);
    HRESULT (STDMETHODCALLTYPE *CopyTemplatePoliciesToZone)(IInternetZoneManagerEx * This, DWORD dwTemplate, DWORD dwZone, DWORD dwReserved);
    HRESULT (STDMETHODCALLTYPE *GetZoneActionPolicyEx)(IInternetZoneManagerEx * This, DWORD dwZone, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, URLZONEREG urlZoneReg, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *SetZoneActionPolicyEx)(IInternetZoneManagerEx * This, DWORD dwZone, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, URLZONEREG urlZoneReg, DWORD dwFlags);
    END_INTERFACE
} IInternetZoneManagerExVtbl;

interface IInternetZoneManagerEx {
    CONST_VTBL struct IInternetZoneManagerExVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetZoneManagerEx_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IInternetZoneManagerEx_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IInternetZoneManagerEx_Release(This)  ((This)->lpVtbl->Release(This))
#define IInternetZoneManagerEx_GetZoneAttributes(This,dwZone,pZoneAttributes)  ((This)->lpVtbl->GetZoneAttributes(This,dwZone,pZoneAttributes))
#define IInternetZoneManagerEx_SetZoneAttributes(This,dwZone,pZoneAttributes)  ((This)->lpVtbl->SetZoneAttributes(This,dwZone,pZoneAttributes))
#define IInternetZoneManagerEx_GetZoneCustomPolicy(This,dwZone,guidKey,ppPolicy,pcbPolicy,urlZoneReg)  ((This)->lpVtbl->GetZoneCustomPolicy(This,dwZone,guidKey,ppPolicy,pcbPolicy,urlZoneReg))
#define IInternetZoneManagerEx_SetZoneCustomPolicy(This,dwZone,guidKey,pPolicy,cbPolicy,urlZoneReg)  ((This)->lpVtbl->SetZoneCustomPolicy(This,dwZone,guidKey,pPolicy,cbPolicy,urlZoneReg))
#define IInternetZoneManagerEx_GetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)  ((This)->lpVtbl->GetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg))
#define IInternetZoneManagerEx_SetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)  ((This)->lpVtbl->SetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg))
#define IInternetZoneManagerEx_PromptAction(This,dwAction,hwndParent,pwszUrl,pwszText,dwPromptFlags)  ((This)->lpVtbl->PromptAction(This,dwAction,hwndParent,pwszUrl,pwszText,dwPromptFlags))
#define IInternetZoneManagerEx_LogAction(This,dwAction,pwszUrl,pwszText,dwLogFlags)  ((This)->lpVtbl->LogAction(This,dwAction,pwszUrl,pwszText,dwLogFlags))
#define IInternetZoneManagerEx_CreateZoneEnumerator(This,pdwEnum,pdwCount,dwFlags)  ((This)->lpVtbl->CreateZoneEnumerator(This,pdwEnum,pdwCount,dwFlags))
#define IInternetZoneManagerEx_GetZoneAt(This,dwEnum,dwIndex,pdwZone)  ((This)->lpVtbl->GetZoneAt(This,dwEnum,dwIndex,pdwZone))
#define IInternetZoneManagerEx_DestroyZoneEnumerator(This,dwEnum)  ((This)->lpVtbl->DestroyZoneEnumerator(This,dwEnum))
#define IInternetZoneManagerEx_CopyTemplatePoliciesToZone(This,dwTemplate,dwZone,dwReserved)  ((This)->lpVtbl->CopyTemplatePoliciesToZone(This,dwTemplate,dwZone,dwReserved))
#define IInternetZoneManagerEx_GetZoneActionPolicyEx(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg,dwFlags)  ((This)->lpVtbl->GetZoneActionPolicyEx(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg,dwFlags))
#define IInternetZoneManagerEx_SetZoneActionPolicyEx(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg,dwFlags)  ((This)->lpVtbl->SetZoneActionPolicyEx(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg,dwFlags))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IInternetZoneManagerEx_INTERFACE_DEFINED__ */

#endif

#endif /* _WIN32_IE >= _WIN32_IE_IE60SP2 */

#if (_WIN32_IE >= _WIN32_IE_IE70)

#ifndef _LPINTERNETZONEMANAGEREX2_DEFINED
#define _LPINTERNETZONEMANAGEREX2_DEFINED

#define SECURITY_IE_STATE_GREEN  0x00000000
#define SECURITY_IE_STATE_RED  0x00000001

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0044_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_0044_v0_0_s_ifspec;

#ifndef __IInternetZoneManagerEx2_INTERFACE_DEFINED__
#define __IInternetZoneManagerEx2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInternetZoneManagerEx2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("EDC17559-DD5D-4846-8EEF-8BECBA5A4ABF") IInternetZoneManagerEx2 : public IInternetZoneManagerEx
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetZoneAttributesEx(DWORD dwZone, ZONEATTRIBUTES * pZoneAttributes, DWORD dwFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetZoneSecurityState(DWORD dwZoneIndex, BOOL fRespectPolicy, LPDWORD pdwState, BOOL * pfPolicyEncountered) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIESecurityState(BOOL fRespectPolicy, LPDWORD pdwState, BOOL * pfPolicyEncountered, BOOL fNoCache) = 0;
    virtual HRESULT STDMETHODCALLTYPE FixUnsecureSettings(void) = 0;
};
#else /* C style interface */

typedef struct IInternetZoneManagerEx2Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IInternetZoneManagerEx2 * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IInternetZoneManagerEx2 * This);
    ULONG (STDMETHODCALLTYPE *Release)(IInternetZoneManagerEx2 * This);
    HRESULT (STDMETHODCALLTYPE *GetZoneAttributes)(IInternetZoneManagerEx2 * This, DWORD dwZone, ZONEATTRIBUTES * pZoneAttributes);
    HRESULT (STDMETHODCALLTYPE *SetZoneAttributes)(IInternetZoneManagerEx2 * This, DWORD dwZone, ZONEATTRIBUTES * pZoneAttributes);
    HRESULT (STDMETHODCALLTYPE *GetZoneCustomPolicy)(IInternetZoneManagerEx2 * This, DWORD dwZone, REFGUID guidKey, BYTE ** ppPolicy, DWORD * pcbPolicy, URLZONEREG urlZoneReg);
    HRESULT (STDMETHODCALLTYPE *SetZoneCustomPolicy)(IInternetZoneManagerEx2 * This, DWORD dwZone, REFGUID guidKey, BYTE * pPolicy, DWORD cbPolicy, URLZONEREG urlZoneReg);
    HRESULT (STDMETHODCALLTYPE *GetZoneActionPolicy)(IInternetZoneManagerEx2 * This, DWORD dwZone, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, URLZONEREG urlZoneReg);
    HRESULT (STDMETHODCALLTYPE *SetZoneActionPolicy)(IInternetZoneManagerEx2 * This, DWORD dwZone, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, URLZONEREG urlZoneReg);
    HRESULT (STDMETHODCALLTYPE *PromptAction)(IInternetZoneManagerEx2 * This, DWORD dwAction, HWND hwndParent, LPCWSTR pwszUrl, LPCWSTR pwszText, DWORD dwPromptFlags);
    HRESULT (STDMETHODCALLTYPE *LogAction)(IInternetZoneManagerEx2 * This, DWORD dwAction, LPCWSTR pwszUrl, LPCWSTR pwszText, DWORD dwLogFlags);
    HRESULT (STDMETHODCALLTYPE *CreateZoneEnumerator)(IInternetZoneManagerEx2 * This, DWORD * pdwEnum, DWORD * pdwCount, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *GetZoneAt)(IInternetZoneManagerEx2 * This, DWORD dwEnum, DWORD dwIndex, DWORD * pdwZone);
    HRESULT (STDMETHODCALLTYPE *DestroyZoneEnumerator)(IInternetZoneManagerEx2 * This, DWORD dwEnum);
    HRESULT (STDMETHODCALLTYPE *CopyTemplatePoliciesToZone)(IInternetZoneManagerEx2 * This, DWORD dwTemplate, DWORD dwZone, DWORD dwReserved);
    HRESULT (STDMETHODCALLTYPE *GetZoneActionPolicyEx)(IInternetZoneManagerEx2 * This, DWORD dwZone, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, URLZONEREG urlZoneReg, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *SetZoneActionPolicyEx)(IInternetZoneManagerEx2 * This, DWORD dwZone, DWORD dwAction, BYTE * pPolicy, DWORD cbPolicy, URLZONEREG urlZoneReg, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *GetZoneAttributesEx)(IInternetZoneManagerEx2 * This, DWORD dwZone, ZONEATTRIBUTES * pZoneAttributes, DWORD dwFlags);
    HRESULT (STDMETHODCALLTYPE *GetZoneSecurityState)(IInternetZoneManagerEx2 * This, DWORD dwZoneIndex, BOOL fRespectPolicy, LPDWORD pdwState, BOOL * pfPolicyEncountered);
    HRESULT (STDMETHODCALLTYPE *GetIESecurityState)(IInternetZoneManagerEx2 * This, BOOL fRespectPolicy, LPDWORD pdwState, BOOL * pfPolicyEncountered, BOOL fNoCache);
    HRESULT (STDMETHODCALLTYPE *FixUnsecureSettings)(IInternetZoneManagerEx2 * This);
    END_INTERFACE
} IInternetZoneManagerEx2Vtbl;

interface IInternetZoneManagerEx2 {
    CONST_VTBL struct IInternetZoneManagerEx2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInternetZoneManagerEx2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IInternetZoneManagerEx2_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IInternetZoneManagerEx2_Release(This)  ((This)->lpVtbl->Release(This))
#define IInternetZoneManagerEx2_GetZoneAttributes(This,dwZone,pZoneAttributes)  ((This)->lpVtbl->GetZoneAttributes(This,dwZone,pZoneAttributes))
#define IInternetZoneManagerEx2_SetZoneAttributes(This,dwZone,pZoneAttributes)  ((This)->lpVtbl->SetZoneAttributes(This,dwZone,pZoneAttributes))
#define IInternetZoneManagerEx2_GetZoneCustomPolicy(This,dwZone,guidKey,ppPolicy,pcbPolicy,urlZoneReg)  ((This)->lpVtbl->GetZoneCustomPolicy(This,dwZone,guidKey,ppPolicy,pcbPolicy,urlZoneReg))
#define IInternetZoneManagerEx2_SetZoneCustomPolicy(This,dwZone,guidKey,pPolicy,cbPolicy,urlZoneReg)  ((This)->lpVtbl->SetZoneCustomPolicy(This,dwZone,guidKey,pPolicy,cbPolicy,urlZoneReg))
#define IInternetZoneManagerEx2_GetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)  ((This)->lpVtbl->GetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg))
#define IInternetZoneManagerEx2_SetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)  ((This)->lpVtbl->SetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg))
#define IInternetZoneManagerEx2_PromptAction(This,dwAction,hwndParent,pwszUrl,pwszText,dwPromptFlags)  ((This)->lpVtbl->PromptAction(This,dwAction,hwndParent,pwszUrl,pwszText,dwPromptFlags))
#define IInternetZoneManagerEx2_LogAction(This,dwAction,pwszUrl,pwszText,dwLogFlags)  ((This)->lpVtbl->LogAction(This,dwAction,pwszUrl,pwszText,dwLogFlags))
#define IInternetZoneManagerEx2_CreateZoneEnumerator(This,pdwEnum,pdwCount,dwFlags)  ((This)->lpVtbl->CreateZoneEnumerator(This,pdwEnum,pdwCount,dwFlags))
#define IInternetZoneManagerEx2_GetZoneAt(This,dwEnum,dwIndex,pdwZone)  ((This)->lpVtbl->GetZoneAt(This,dwEnum,dwIndex,pdwZone))
#define IInternetZoneManagerEx2_DestroyZoneEnumerator(This,dwEnum)  ((This)->lpVtbl->DestroyZoneEnumerator(This,dwEnum))
#define IInternetZoneManagerEx2_CopyTemplatePoliciesToZone(This,dwTemplate,dwZone,dwReserved)  ((This)->lpVtbl->CopyTemplatePoliciesToZone(This,dwTemplate,dwZone,dwReserved))
#define IInternetZoneManagerEx2_GetZoneActionPolicyEx(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg,dwFlags)  ((This)->lpVtbl->GetZoneActionPolicyEx(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg,dwFlags))
#define IInternetZoneManagerEx2_SetZoneActionPolicyEx(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg,dwFlags)  ((This)->lpVtbl->SetZoneActionPolicyEx(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg,dwFlags))
#define IInternetZoneManagerEx2_GetZoneAttributesEx(This,dwZone,pZoneAttributes,dwFlags)  ((This)->lpVtbl->GetZoneAttributesEx(This,dwZone,pZoneAttributes,dwFlags))
#define IInternetZoneManagerEx2_GetZoneSecurityState(This,dwZoneIndex,fRespectPolicy,pdwState,pfPolicyEncountered)  ((This)->lpVtbl->GetZoneSecurityState(This,dwZoneIndex,fRespectPolicy,pdwState,pfPolicyEncountered))
#define IInternetZoneManagerEx2_GetIESecurityState(This,fRespectPolicy,pdwState,pfPolicyEncountered,fNoCache)  ((This)->lpVtbl->GetIESecurityState(This,fRespectPolicy,pdwState,pfPolicyEncountered,fNoCache))
#define IInternetZoneManagerEx2_FixUnsecureSettings(This)  ((This)->lpVtbl->FixUnsecureSettings(This))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IInternetZoneManagerEx2_INTERFACE_DEFINED__ */

#endif
#endif /* _WIN32_IE >= _WIN32_IE_IE70 */


EXTERN_C const IID CLSID_SoftDistExt;

#ifndef _LPSOFTDISTEXT_DEFINED
#define _LPSOFTDISTEXT_DEFINED
#define SOFTDIST_FLAG_USAGE_EMAIL  0x00000001
#define SOFTDIST_FLAG_USAGE_PRECACHE  0x00000002
#define SOFTDIST_FLAG_USAGE_AUTOINSTALL  0x00000004
#define SOFTDIST_FLAG_DELETE_SUBSCRIPTION  0x00000008
#define SOFTDIST_ADSTATE_NONE  0x00000000
#define SOFTDIST_ADSTATE_AVAILABLE  0x00000001
#define SOFTDIST_ADSTATE_DOWNLOADED  0x00000002
#define SOFTDIST_ADSTATE_INSTALLED  0x00000003

typedef struct _tagCODEBASEHOLD {
    ULONG cbSize;
    LPWSTR szDistUnit;
    LPWSTR szCodeBase;
    DWORD dwVersionMS;
    DWORD dwVersionLS;
    DWORD dwStyle;
} CODEBASEHOLD,*LPCODEBASEHOLD;

typedef struct _tagSOFTDISTINFO {
    ULONG cbSize;
    DWORD dwFlags;
    DWORD dwAdState;
    LPWSTR szTitle;
    LPWSTR szAbstract;
    LPWSTR szHREF;
    DWORD dwInstalledVersionMS;
    DWORD dwInstalledVersionLS;
    DWORD dwUpdateVersionMS;
    DWORD dwUpdateVersionLS;
    DWORD dwAdvertisedVersionMS;
    DWORD dwAdvertisedVersionLS;
    DWORD dwReserved;
} SOFTDISTINFO,*LPSOFTDISTINFO;

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0193_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0193_v0_0_s_ifspec;

#ifndef __ISoftDistExt_INTERFACE_DEFINED__
#define __ISoftDistExt_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISoftDistExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("B15B8DC1-C7E1-11d0-8680-00AA00BDCB71")
ISoftDistExt:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE ProcessSoftDist(LPCWSTR,IXMLElement*,LPSOFTDISTINFO) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFirstCodeBase(LPWSTR*,LPDWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNextCodeBase(LPWSTR*,LPDWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE AsyncInstallDistributionUnit(IBindCtx*,LPVOID,DWORD,LPCODEBASEHOLD) = 0;
};
#else
typedef struct ISoftDistExtVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ISoftDistExt*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ISoftDistExt*);
    ULONG(STDMETHODCALLTYPE *Release)(ISoftDistExt*);
    HRESULT(STDMETHODCALLTYPE *ProcessSoftDist)(ISoftDistExt*,LPCWSTR,IXMLElement*,LPSOFTDISTINFO);
    HRESULT(STDMETHODCALLTYPE *GetFirstCodeBase)(ISoftDistExt*,LPWSTR*,LPDWORD);
    HRESULT(STDMETHODCALLTYPE *GetNextCodeBase)(ISoftDistExt*,LPWSTR*,LPDWORD);
    HRESULT(STDMETHODCALLTYPE *AsyncInstallDistributionUnit)(ISoftDistExt*,IBindCtx*,LPVOID,DWORD,LPCODEBASEHOLD);
    END_INTERFACE
} ISoftDistExtVtbl;

interface ISoftDistExt {
    CONST_VTBL struct ISoftDistExtVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISoftDistExt_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ISoftDistExt_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ISoftDistExt_Release(This)  (This)->lpVtbl->Release(This)
#define ISoftDistExt_ProcessSoftDist(This,szCDFURL,pSoftDistElement,lpsdi)  (This)->lpVtbl->ProcessSoftDist(This,szCDFURL,pSoftDistElement,lpsdi)
#define ISoftDistExt_GetFirstCodeBase(This,szCodeBase,dwMaxSize)  (This)->lpVtbl->GetFirstCodeBase(This,szCodeBase,dwMaxSize)
#define ISoftDistExt_GetNextCodeBase(This,szCodeBase,dwMaxSize)  (This)->lpVtbl->GetNextCodeBase(This,szCodeBase,dwMaxSize)
#define ISoftDistExt_AsyncInstallDistributionUnit(This,pbc,pvReserved,flags,lpcbh)  (This)->lpVtbl->AsyncInstallDistributionUnit(This,pbc,pvReserved,flags,lpcbh)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE ISoftDistExt_ProcessSoftDist_Proxy(ISoftDistExt*,LPCWSTR,IXMLElement*,LPSOFTDISTINFO);
void __RPC_STUB ISoftDistExt_ProcessSoftDist_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ISoftDistExt_GetFirstCodeBase_Proxy(ISoftDistExt*,LPWSTR*,LPDWORD);
void __RPC_STUB ISoftDistExt_GetFirstCodeBase_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ISoftDistExt_GetNextCodeBase_Proxy(ISoftDistExt*,LPWSTR*,LPDWORD);
void __RPC_STUB ISoftDistExt_GetNextCodeBase_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ISoftDistExt_AsyncInstallDistributionUnit_Proxy(ISoftDistExt*,IBindCtx*,LPVOID,DWORD,LPCODEBASEHOLD);
void __RPC_STUB ISoftDistExt_AsyncInstallDistributionUnit_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __ISoftDistExt_INTERFACE_DEFINED__ */

STDAPI GetSoftwareUpdateInfo(LPCWSTR,LPSOFTDISTINFO);
STDAPI SetSoftwareUpdateAdvertisementState(LPCWSTR,DWORD,DWORD,DWORD);
#endif

#ifndef _LPCATALOGFILEINFO_DEFINED
#define _LPCATALOGFILEINFO_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0194_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0194_v0_0_s_ifspec;

#ifndef __ICatalogFileInfo_INTERFACE_DEFINED__
#define __ICatalogFileInfo_INTERFACE_DEFINED__

typedef ICatalogFileInfo *LPCATALOGFILEINFO;
EXTERN_C const IID IID_ICatalogFileInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("711C7600-6B48-11d1-B403-00AA00B92AF1")
ICatalogFileInfo:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCatalogFile(LPSTR*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetJavaTrust(void**) = 0;
};
#else
typedef struct ICatalogFileInfoVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICatalogFileInfo*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(ICatalogFileInfo*);
    ULONG(STDMETHODCALLTYPE *Release)(ICatalogFileInfo*);
    HRESULT(STDMETHODCALLTYPE *GetCatalogFile)(ICatalogFileInfo*,LPSTR*);
    HRESULT(STDMETHODCALLTYPE *GetJavaTrust)(ICatalogFileInfo*,void**);
    END_INTERFACE
} ICatalogFileInfoVtbl;

interface ICatalogFileInfo {
    CONST_VTBL struct ICatalogFileInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICatalogFileInfo_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define ICatalogFileInfo_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define ICatalogFileInfo_Release(This)  (This)->lpVtbl->Release(This)
#define ICatalogFileInfo_GetCatalogFile(This,ppszCatalogFile)  (This)->lpVtbl->GetCatalogFile(This,ppszCatalogFile)
#define ICatalogFileInfo_GetJavaTrust(This,ppJavaTrust)  (This)->lpVtbl->GetJavaTrust(This,ppJavaTrust)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE ICatalogFileInfo_GetCatalogFile_Proxy(ICatalogFileInfo*,LPSTR*);
void __RPC_STUB ICatalogFileInfo_GetCatalogFile_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE ICatalogFileInfo_GetJavaTrust_Proxy(ICatalogFileInfo*,void**);
void __RPC_STUB ICatalogFileInfo_GetJavaTrust_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __ICatalogFileInfo_INTERFACE_DEFINED__ */
#endif


#ifndef _LPDATAFILTER_DEFINED
#define _LPDATAFILTER_DEFINED

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0195_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0195_v0_0_s_ifspec;

#ifndef __IDataFilter_INTERFACE_DEFINED__
#define __IDataFilter_INTERFACE_DEFINED__

typedef IDataFilter *LPDATAFILTER;
EXTERN_C const IID IID_IDataFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("69d14c80-c18e-11d0-a9ce-006097942311")
IDataFilter:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE DoEncode(DWORD,LONG,BYTE*,LONG,BYTE*,LONG,LONG*,LONG*,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE DoDecode(DWORD,LONG,BYTE*,LONG,BYTE*,LONG,LONG*,LONG*,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEncodingLevel(DWORD dwEncLevel) = 0;
};
#else
typedef struct IDataFilterVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IDataFilter*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IDataFilter*);
    ULONG(STDMETHODCALLTYPE *Release)(IDataFilter*);
    HRESULT(STDMETHODCALLTYPE *DoEncode)(IDataFilter*,DWORD,LONG,BYTE*,LONG,BYTE*,LONG,LONG*,LONG*,DWORD);
    HRESULT(STDMETHODCALLTYPE *DoDecode)(IDataFilter*,DWORD,LONG,BYTE*,LONG,BYTE*,LONG,LONG*,LONG*,DWORD);
    HRESULT(STDMETHODCALLTYPE *SetEncodingLevel)(IDataFilter*,DWORD dwEncLevel);
    END_INTERFACE
} IDataFilterVtbl;

interface IDataFilter {
    CONST_VTBL struct IDataFilterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDataFilter_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IDataFilter_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IDataFilter_Release(This)  (This)->lpVtbl->Release(This)
#define IDataFilter_DoEncode(This,dwFlags,lInBufferSize,pbInBuffer,lOutBufferSize,pbOutBuffer,lInBytesAvailable,plInBytesRead,plOutBytesWritten,dwReserved)  (This)->lpVtbl->DoEncode(This,dwFlags,lInBufferSize,pbInBuffer,lOutBufferSize,pbOutBuffer,lInBytesAvailable,plInBytesRead,plOutBytesWritten,dwReserved)
#define IDataFilter_DoDecode(This,dwFlags,lInBufferSize,pbInBuffer,lOutBufferSize,pbOutBuffer,lInBytesAvailable,plInBytesRead,plOutBytesWritten,dwReserved)  (This)->lpVtbl->DoDecode(This,dwFlags,lInBufferSize,pbInBuffer,lOutBufferSize,pbOutBuffer,lInBytesAvailable,plInBytesRead,plOutBytesWritten,dwReserved)
#define IDataFilter_SetEncodingLevel(This,dwEncLevel)  (This)->lpVtbl->SetEncodingLevel(This,dwEncLevel)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IDataFilter_DoEncode_Proxy(IDataFilter*,DWORD,LONG,BYTE*,LONG,BYTE*,LONG,LONG*,LONG*,DWORD);
void __RPC_STUB IDataFilter_DoEncode_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDataFilter_DoDecode_Proxy(IDataFilter*,DWORD,LONG,BYTE*,LONG,BYTE*,LONG,LONG*,LONG*,DWORD);
void __RPC_STUB IDataFilter_DoDecode_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDataFilter_SetEncodingLevel_Proxy(IDataFilter*,DWORD);
void __RPC_STUB IDataFilter_SetEncodingLevel_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IDataFilter_INTERFACE_DEFINED__ */
#endif


#ifndef _LPENCODINGFILTERFACTORY_DEFINED
#define _LPENCODINGFILTERFACTORY_DEFINED

typedef struct _tagPROTOCOLFILTERDATA {
    DWORD cbSize;
    IInternetProtocolSink *pProtocolSink;
    IInternetProtocol *pProtocol;
    IUnknown *pUnk;
    DWORD dwFilterFlags;
} PROTOCOLFILTERDATA;

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0196_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0196_v0_0_s_ifspec;

#ifndef __IEncodingFilterFactory_INTERFACE_DEFINED__
#define __IEncodingFilterFactory_INTERFACE_DEFINED__

typedef IEncodingFilterFactory *LPENCODINGFILTERFACTORY;

typedef struct _tagDATAINFO {
    ULONG ulTotalSize;
    ULONG ulavrPacketSize;
    ULONG ulConnectSpeed;
    ULONG ulProcessorSpeed;
} DATAINFO;

EXTERN_C const IID IID_IEncodingFilterFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("70bdde00-c18e-11d0-a9ce-006097942311")
IEncodingFilterFactory:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE FindBestFilter(LPCWSTR,LPCWSTR,DATAINFO,IDataFilter**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDefaultFilter(LPCWSTR,LPCWSTR,IDataFilter**) = 0;
};
#else
typedef struct IEncodingFilterFactoryVtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IEncodingFilterFactory*,REFIID,void**);
    ULONG(STDMETHODCALLTYPE *AddRef)(IEncodingFilterFactory*);
    ULONG(STDMETHODCALLTYPE *Release)(IEncodingFilterFactory*);
    HRESULT(STDMETHODCALLTYPE *FindBestFilter)(IEncodingFilterFactory*,LPCWSTR,LPCWSTR,DATAINFO,IDataFilter**);
    HRESULT(STDMETHODCALLTYPE *GetDefaultFilter)(IEncodingFilterFactory*,LPCWSTR,LPCWSTR,IDataFilter**);
    END_INTERFACE
} IEncodingFilterFactoryVtbl;

interface IEncodingFilterFactory {
    CONST_VTBL struct IEncodingFilterFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEncodingFilterFactory_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IEncodingFilterFactory_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IEncodingFilterFactory_Release(This)  (This)->lpVtbl->Release(This)
#define IEncodingFilterFactory_FindBestFilter(This,pwzCodeIn,pwzCodeOut,info,ppDF)  (This)->lpVtbl->FindBestFilter(This,pwzCodeIn,pwzCodeOut,info,ppDF)
#define IEncodingFilterFactory_GetDefaultFilter(This,pwzCodeIn,pwzCodeOut,ppDF)  (This)->lpVtbl->GetDefaultFilter(This,pwzCodeIn,pwzCodeOut,ppDF)
#endif /* COBJMACROS */
#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IEncodingFilterFactory_FindBestFilter_Proxy(IEncodingFilterFactory*,LPCWSTR,LPCWSTR,DATAINFO,IDataFilter**);
void __RPC_STUB IEncodingFilterFactory_FindBestFilter_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IEncodingFilterFactory_GetDefaultFilter_Proxy(IEncodingFilterFactory*,LPCWSTR,LPCWSTR,IDataFilter**);
void __RPC_STUB IEncodingFilterFactory_GetDefaultFilter_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif /* __IEncodingFilterFactory_INTERFACE_DEFINED__ */
#endif


#ifndef _HITLOGGING_DEFINED
#define _HITLOGGING_DEFINED

BOOL WINAPI IsLoggingEnabledA(LPCSTR);
BOOL WINAPI IsLoggingEnabledW(LPCWSTR);
#ifdef UNICODE
#define IsLoggingEnabled  IsLoggingEnabledW
#else
#define IsLoggingEnabled  IsLoggingEnabledA
#endif /* !UNICODE */

typedef struct _tagHIT_LOGGING_INFO {
    DWORD dwStructSize;
    LPSTR lpszLoggedUrlName;
    SYSTEMTIME StartTime;
    SYSTEMTIME EndTime;
    LPSTR lpszExtendedInfo;
} HIT_LOGGING_INFO,*LPHIT_LOGGING_INFO;

BOOL WINAPI WriteHitLogging(LPHIT_LOGGING_INFO);

#define CONFIRMSAFETYACTION_LOADOBJECT  0x00000001

struct CONFIRMSAFETY {
    CLSID clsid;
    IUnknown *pUnk;
    DWORD dwFlags;
};

EXTERN_C const GUID GUID_CUSTOM_CONFIRMOBJECTSAFETY;

#endif

extern RPC_IF_HANDLE __MIDL_itf_urlmon_0197_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0197_v0_0_s_ifspec;

unsigned long __RPC_USER HWND_UserSize(unsigned long*,unsigned long,HWND*);
unsigned char *__RPC_USER HWND_UserMarshal(unsigned long*,unsigned char*,HWND*);
unsigned char *__RPC_USER HWND_UserUnmarshal(unsigned long*,unsigned char*,HWND*);
void __RPC_USER HWND_UserFree(unsigned long*,HWND*);
HRESULT STDMETHODCALLTYPE IBinding_GetBindResult_Proxy(IBinding*,CLSID*,DWORD*,LPOLESTR*,DWORD*);
HRESULT STDMETHODCALLTYPE IBinding_GetBindResult_Stub(IBinding*,CLSID*,DWORD*,LPOLESTR*,DWORD);
HRESULT STDMETHODCALLTYPE IBindStatusCallback_GetBindInfo_Proxy(IBindStatusCallback*,DWORD*,BINDINFO*);
HRESULT STDMETHODCALLTYPE IBindStatusCallback_GetBindInfo_Stub(IBindStatusCallback*,DWORD*,RemBINDINFO*,RemSTGMEDIUM*);
HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnDataAvailable_Proxy(IBindStatusCallback*,DWORD,DWORD,FORMATETC*,STGMEDIUM*);
HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnDataAvailable_Stub(IBindStatusCallback*,DWORD,DWORD,RemFORMATETC*,RemSTGMEDIUM*);
HRESULT STDMETHODCALLTYPE IWinInetInfo_QueryOption_Proxy(IWinInetInfo*,DWORD,LPVOID,DWORD*);
HRESULT STDMETHODCALLTYPE IWinInetInfo_QueryOption_Stub(IWinInetInfo*,DWORD,BYTE*,DWORD*);
HRESULT STDMETHODCALLTYPE IWinInetHttpInfo_QueryInfo_Proxy(IWinInetHttpInfo*,DWORD,LPVOID,DWORD*,DWORD*,DWORD*);
HRESULT STDMETHODCALLTYPE IWinInetHttpInfo_QueryInfo_Stub(IWinInetHttpInfo*,DWORD,BYTE*,DWORD*,DWORD*,DWORD*);
HRESULT STDMETHODCALLTYPE IBindHost_MonikerBindToStorage_Proxy(IBindHost*,IMoniker*,IBindCtx*,IBindStatusCallback*,REFIID,void**);
HRESULT STDMETHODCALLTYPE IBindHost_MonikerBindToStorage_Stub(IBindHost*,IMoniker*,IBindCtx*,IBindStatusCallback*,REFIID,IUnknown**);
HRESULT STDMETHODCALLTYPE IBindHost_MonikerBindToObject_Proxy(IBindHost*,IMoniker*,IBindCtx*,IBindStatusCallback*,REFIID,void**);
HRESULT STDMETHODCALLTYPE IBindHost_MonikerBindToObject_Stub(IBindHost*,IMoniker*,IBindCtx*,IBindStatusCallback*,REFIID,IUnknown**);

#ifdef __cplusplus
}
#endif

#endif /* _URLMON_H */
