#ifndef __RPCPROXY_H_VERSION__
#define __RPCPROXY_H_VERSION__  (475)

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif
#endif

#ifndef _RPCPROXY_H
#define _RPCPROXY_H

/* Windows RPC Proxy stub definitions */

#define __midl_proxy

#if defined(_M_IA64) || defined(_M_AMD64)
#include <pshpack8.h>
#endif

#include <basetsd.h>

#ifndef GUID_DEFINED
#include <guiddef.h>
#endif

struct tagCInterfaceStubVtbl;
struct tagCInterfaceProxyVtbl;

typedef struct tagCInterfaceStubVtbl *PCInterfaceStubVtblList;
typedef struct tagCInterfaceProxyVtbl *PCInterfaceProxyVtblList;
typedef const char *PCInterfaceName;
typedef int __stdcall IIDLookupRtn(const IID*,int*);
typedef IIDLookupRtn *PIIDLookup;

typedef struct tagProxyFileInfo {
    const PCInterfaceProxyVtblList *pProxyVtblList;
    const PCInterfaceStubVtblList *pStubVtblList;
    const PCInterfaceName *pNamesArray;
    const IID **pDelegatedIIDs;
    const PIIDLookup pIIDLookupRtn;
    unsigned short TableSize;
    unsigned short TableVersion;
    const IID **pAsyncIIDLookup;
    LONG_PTR Filler2;
    LONG_PTR Filler3;
    LONG_PTR Filler4;
} ProxyFileInfo;

typedef ProxyFileInfo ExtendedProxyFileInfo;

#include <rpc.h>
#include <rpcndr.h>
#include <string.h>

typedef struct tagCInterfaceProxyHeader {
#ifdef USE_STUBLESS_PROXY
    const void *pStublessProxyInfo;
#endif
    const IID *piid;
} CInterfaceProxyHeader;

#define CINTERFACE_PROXY_VTABLE(n)  struct { CInterfaceProxyHeader header; void *Vtbl[n]; }

typedef struct tagCInterfaceProxyVtbl {
    CInterfaceProxyHeader header;
    void *Vtbl[];
} CInterfaceProxyVtbl;

typedef void (__RPC_STUB *PRPC_STUB_FUNCTION)(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

typedef struct tagCInterfaceStubHeader {
    const IID *piid;
    const MIDL_SERVER_INFO *pServerInfo;
    unsigned long DispatchTableCount;
    const PRPC_STUB_FUNCTION *pDispatchTable;
} CInterfaceStubHeader;

typedef struct tagCInterfaceStubVtbl {
    CInterfaceStubHeader header;
    IRpcStubBufferVtbl Vtbl;
} CInterfaceStubVtbl;

typedef struct tagCStdStubBuffer {
    const struct IRpcStubBufferVtbl *lpVtbl;
    long RefCount;
    struct IUnknown *pvServerObject;
    const struct ICallFactoryVtbl *pCallFactoryVtbl;
    const IID *pAsyncIID;
    struct IPSFactoryBuffer *pPSFactory;
    const struct IReleaseMarshalBuffersVtbl *pRMBVtbl;
} CStdStubBuffer;

typedef struct tagCStdPSFactoryBuffer {
    const IPSFactoryBufferVtbl *lpVtbl;
    long RefCount;
    const ProxyFileInfo **pProxyFileList;
    long Filler1;
} CStdPSFactoryBuffer;

void __RPC_STUB NdrStubForwardingFunction(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#define CStdStubBuffer_METHODS \
    CStdStubBuffer_QueryInterface,\
    CStdStubBuffer_AddRef, \
    CStdStubBuffer_Release, \
    CStdStubBuffer_Connect, \
    CStdStubBuffer_Disconnect, \
    CStdStubBuffer_Invoke, \
    CStdStubBuffer_IsIIDSupported, \
    CStdStubBuffer_CountRefs, \
    CStdStubBuffer_DebugServerQueryInterface, \
    CStdStubBuffer_DebugServerRelease

#define CStdAsyncStubBuffer_METHODS  0,0,0,0,0,0,0,0,0,0
#define CStdAsyncStubBuffer_DELEGATING_METHODS  0,0,0,0,0,0,0,0,0,0

#define IID_GENERIC_CHECK_IID(name,pIID,index)  memcmp(pIID,name##_ProxyVtblList[index]->header.piid,16)
#define IID_BS_LOOKUP_SETUP  int result, low=-1;
#define IID_BS_LOOKUP_INITIAL_TEST(name,sz,split)  result = name##_CHECK_IID(split); if (result > 0) { low = sz - split; } else if (!result) { low = split; goto found_label; }
#define IID_BS_LOOKUP_NEXT_TEST(name,split)  result = name##_CHECK_IID(low+split); if (result>=0) { low = low + split; if (!result) goto found_label; }
#define IID_BS_LOOKUP_RETURN_RESULT(name,sz,index)  low = low + 1; if (low >= sz) goto not_found_label; result = name##_CHECK_IID(low); if (result) goto not_found_label; found_label: (index) = low; return 1; not_found_label: return 0;

RPCRTAPI void RPC_ENTRY NdrProxyInitialize(void*,PRPC_MESSAGE,PMIDL_STUB_MESSAGE,PMIDL_STUB_DESC,unsigned int);
RPCRTAPI void RPC_ENTRY NdrProxyGetBuffer(void*,PMIDL_STUB_MESSAGE);
RPCRTAPI void RPC_ENTRY NdrProxySendReceive(void*,MIDL_STUB_MESSAGE*);
RPCRTAPI void RPC_ENTRY NdrProxyFreeBuffer(void*,MIDL_STUB_MESSAGE*);
RPCRTAPI HRESULT RPC_ENTRY NdrProxyErrorHandler(DWORD);
RPCRTAPI void RPC_ENTRY NdrStubInitialize(PRPC_MESSAGE,PMIDL_STUB_MESSAGE,PMIDL_STUB_DESC,IRpcChannelBuffer*);
RPCRTAPI void RPC_ENTRY NdrStubInitializePartial(PRPC_MESSAGE,PMIDL_STUB_MESSAGE,PMIDL_STUB_DESC,IRpcChannelBuffer*,unsigned long);
RPCRTAPI void RPC_ENTRY NdrStubGetBuffer(IRpcStubBuffer*,IRpcChannelBuffer*,PMIDL_STUB_MESSAGE);
RPCRTAPI HRESULT RPC_ENTRY NdrStubErrorHandler(DWORD);

HRESULT STDMETHODCALLTYPE CStdStubBuffer_QueryInterface(IRpcStubBuffer*,REFIID,void**);
ULONG STDMETHODCALLTYPE CStdStubBuffer_AddRef(IRpcStubBuffer*);
ULONG STDMETHODCALLTYPE CStdStubBuffer_Release(IRpcStubBuffer*);
ULONG STDMETHODCALLTYPE NdrCStdStubBuffer_Release(IRpcStubBuffer*,IPSFactoryBuffer*);
HRESULT STDMETHODCALLTYPE CStdStubBuffer_Connect(IRpcStubBuffer*,IUnknown*);
void STDMETHODCALLTYPE CStdStubBuffer_Disconnect(IRpcStubBuffer*);
HRESULT STDMETHODCALLTYPE CStdStubBuffer_Invoke(IRpcStubBuffer*,RPCOLEMESSAGE*,IRpcChannelBuffer*);
IRpcStubBuffer * STDMETHODCALLTYPE CStdStubBuffer_IsIIDSupported(IRpcStubBuffer*,REFIID);
ULONG STDMETHODCALLTYPE CStdStubBuffer_CountRefs(IRpcStubBuffer*);
HRESULT STDMETHODCALLTYPE CStdStubBuffer_DebugServerQueryInterface(IRpcStubBuffer*,void**);
void STDMETHODCALLTYPE CStdStubBuffer_DebugServerRelease(IRpcStubBuffer*,void*);

RPCRTAPI HRESULT RPC_ENTRY NdrDllGetClassObject(REFCLSID,REFIID,void**,const ProxyFileInfo**,const CLSID*,CStdPSFactoryBuffer*);
RPCRTAPI HRESULT RPC_ENTRY NdrDllCanUnloadNow(CStdPSFactoryBuffer*);

#ifndef ENTRY_PREFIX
#define ENTRY_PREFIX
#ifndef DllMain
#define DISABLE_THREAD_LIBRARY_CALLS(x)  DisableThreadLibraryCalls(x)
#endif /* DllMain */
#define DLLREGISTERSERVER_ENTRY  DllRegisterServer
#define DLLUNREGISTERSERVER_ENTRY  DllUnregisterServer
#define DLLMAIN_ENTRY  DllMain
#define DLLGETCLASSOBJECT_ENTRY  DllGetClassObject
#define DLLCANUNLOADNOW_ENTRY  DllCanUnloadNow
#else /* ENTRY_PREFIX */
#define __rpc_macro_expand2(a, b) a##b
#define __rpc_macro_expand(a, b) __rpc_macro_expand2(a,b)
#define DLLREGISTERSERVER_ENTRY  __rpc_macro_expand(ENTRY_PREFIX,DllRegisterServer)
#define DLLUNREGISTERSERVER_ENTRY  __rpc_macro_expand(ENTRY_PREFIX,DllUnregisterServer)
#define DLLMAIN_ENTRY  __rpc_macro_expand(ENTRY_PREFIX,DllMain)
#define DLLGETCLASSOBJECT_ENTRY  __rpc_macro_expand(ENTRY_PREFIX,DllGetClassObject)
#define DLLCANUNLOADNOW_ENTRY  __rpc_macro_expand(ENTRY_PREFIX,DllCanUnloadNow)
#endif /* ENTRY_PREFIX */

#ifndef DISABLE_THREAD_LIBRARY_CALLS
#define DISABLE_THREAD_LIBRARY_CALLS(x)
#endif

RPCRTAPI HRESULT RPC_ENTRY NdrDllRegisterProxy(HMODULE,const ProxyFileInfo**,const CLSID*);
RPCRTAPI HRESULT RPC_ENTRY NdrDllUnregisterProxy(HMODULE,const ProxyFileInfo**,const CLSID*);

#define REGISTER_PROXY_DLL_ROUTINES(pProxyFileList,pClsID) \
    HINSTANCE hProxyDll = 0; \
    BOOL WINAPI DLLMAIN_ENTRY(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) \
    { \
        if (fdwReason == DLL_PROCESS_ATTACH) \
        { \
            hProxyDll = hinstDLL; \
            DISABLE_THREAD_LIBRARY_CALLS(hinstDLL); \
        } \
        return TRUE; \
    } \
    HRESULT STDAPICALLTYPE DLLREGISTERSERVER_ENTRY(void) \
    { \
        return NdrDllRegisterProxy(hProxyDll, pProxyFileList, pClsID); \
    }  \
    HRESULT STDAPICALLTYPE DLLUNREGISTERSERVER_ENTRY(void) \
    { \
        return NdrDllUnregisterProxy(hProxyDll, pProxyFileList, pClsID); \
    }

#define STUB_FORWARDING_FUNCTION  NdrStubForwardingFunction

ULONG STDMETHODCALLTYPE CStdStubBuffer2_Release(IRpcStubBuffer*);
ULONG STDMETHODCALLTYPE NdrCStdStubBuffer2_Release(IRpcStubBuffer*,IPSFactoryBuffer*);

#define CStdStubBuffer_DELEGATING_METHODS 0, 0, CStdStubBuffer2_Release, 0, 0, 0, 0, 0, 0, 0

#ifdef PROXY_CLSID
#define CLSID_PSFACTORYBUFFER extern CLSID PROXY_CLSID;
#else
#ifdef PROXY_CLSID_IS
#define CLSID_PSFACTORYBUFFER const CLSID CLSID_PSFactoryBuffer = PROXY_CLSID_IS;
#define PROXY_CLSID  CLSID_PSFactoryBuffer
#else
#define CLSID_PSFACTORYBUFFER
#endif /* PROXY_CLSID_IS */
#endif /* PROXY_CLSID */

#ifndef PROXY_CLSID
#define GET_DLL_CLSID  (aProxyFileList[0]->pStubVtblList[0] != 0 ? aProxyFileList[0]->pStubVtblList[0]->header.piid : 0)
#else
#define GET_DLL_CLSID  &PROXY_CLSID
#endif

#define EXTERN_PROXY_FILE(name)  EXTERN_C const ProxyFileInfo name##_ProxyFileInfo;
#define PROXYFILE_LIST_START  const ProxyFileInfo *aProxyFileList[] = {
#define REFERENCE_PROXY_FILE(name)  &name##_ProxyFileInfo
#define PROXYFILE_LIST_END  0 };

#define DLLDATA_GETPROXYDLLINFO(pPFList,pClsid)  void RPC_ENTRY GetProxyDllInfo(const ProxyFileInfo***pInfo, const CLSID **pId) { *pInfo = pPFList; *pId = pClsid; };
#define DLLGETCLASSOBJECTROUTINE(pPFlist,pClsid,pFactory)  HRESULT STDAPICALLTYPE DLLGETCLASSOBJECT_ENTRY (REFCLSID rclsid, REFIID riid, void **ppv) { return NdrDllGetClassObject(rclsid,riid,ppv,pPFlist,pClsid,pFactory ); }
#define DLLCANUNLOADNOW(pFactory)  HRESULT STDAPICALLTYPE DLLCANUNLOADNOW_ENTRY(void) { return NdrDllCanUnloadNow(pFactory); }
#define DLLDUMMYPURECALL  void __cdecl _purecall(void) {}
#define CSTDSTUBBUFFERRELEASE(pFactory)  ULONG STDMETHODCALLTYPE CStdStubBuffer_Release(IRpcStubBuffer *This) { return NdrCStdStubBuffer_Release(This,(IPSFactoryBuffer *)pFactory); }   \

#ifdef PROXY_DELEGATION
#define CSTDSTUBBUFFER2RELEASE(pFactory)  ULONG STDMETHODCALLTYPE CStdStubBuffer2_Release(IRpcStubBuffer *This) { return NdrCStdStubBuffer2_Release(This,(IPSFactoryBuffer *)pFactory); }
#else
#define CSTDSTUBBUFFER2RELEASE(pFactory)
#endif /* PROXY_DELEGATION */

#ifdef REGISTER_PROXY_DLL
#define DLLREGISTRY_ROUTINES(pProxyFileList,pClsID) REGISTER_PROXY_DLL_ROUTINES(pProxyFileList,pClsID)
#else
#define DLLREGISTRY_ROUTINES(pProxyFileList,pClsID)
#endif /* REGISTER_PROXY_DLL */

#define DLLDATA_ROUTINES(pProxyFileList,pClsID) \
    CLSID_PSFACTORYBUFFER \
    CStdPSFactoryBuffer gPFactory = {0,0,0,0}; \
    DLLDATA_GETPROXYDLLINFO(pProxyFileList,pClsID) \
    DLLGETCLASSOBJECTROUTINE(pProxyFileList,pClsID,&gPFactory) \
    DLLCANUNLOADNOW(&gPFactory) \
    CSTDSTUBBUFFERRELEASE(&gPFactory) \
    CSTDSTUBBUFFER2RELEASE(&gPFactory) \
    DLLDUMMYPURECALL \
    DLLREGISTRY_ROUTINES(pProxyFileList, pClsID) \

#define DLLDATA_STANDARD_ROUTINES  DLLDATA_ROUTINES((const ProxyFileInfo**)pProxyFileList,&CLSID_PSFactoryBuffer)

#if defined(_M_IA64) || defined(_M_AMD64)
#include <poppack.h>
#endif

#endif /* _RPCPROXY_H */
