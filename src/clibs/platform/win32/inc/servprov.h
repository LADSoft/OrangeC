#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__  500
#endif

#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__  100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /* COM_NO_WINDOWS_H */

#ifndef _SERVPROV_H
#define _SERVPROV_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IServiceProvider_FWD_DEFINED__
#define __IServiceProvider_FWD_DEFINED__
typedef interface IServiceProvider IServiceProvider;
#endif

#include "objidl.h"

#ifdef __cplusplus
extern "C" {
#endif

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void*);

extern RPC_IF_HANDLE __MIDL_itf_servprov_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_servprov_0000_v0_0_s_ifspec;

#ifndef __IServiceProvider_INTERFACE_DEFINED__
#define __IServiceProvider_INTERFACE_DEFINED__

typedef IServiceProvider *LPSERVICEPROVIDER;

EXTERN_C const IID IID_IServiceProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("6d5140c1-7436-11ce-8034-00aa006009fa")
IServiceProvider : public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE QueryService(REFGUID,REFIID,void**) = 0;
};
#else
typedef struct IServiceProviderVtbl
{
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IServiceProvider*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IServiceProvider*);
    ULONG (STDMETHODCALLTYPE *Release)(IServiceProvider*);
    HRESULT (STDMETHODCALLTYPE *QueryService)(IServiceProvider*,REFGUID,REFIID,void**);
    END_INTERFACE
} IServiceProviderVtbl;

interface IServiceProvider {
    CONST_VTBL struct IServiceProviderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IServiceProvider_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IServiceProvider_AddRef(This) (This)->lpVtbl->AddRef(This)
#define IServiceProvider_Release(This)  (This)->lpVtbl->Release(This)
#define IServiceProvider_QueryService(This,guidService,riid,ppvObject)  (This)->lpVtbl->QueryService(This,guidService,riid,ppvObject)
#endif /* COBJMACROS */
#endif /* __cplusplus && !CINTERFACE */

HRESULT STDMETHODCALLTYPE IServiceProvider_RemoteQueryService_Proxy(IServiceProvider*,REFGUID,REFIID,IUnknown**);
void __RPC_STUB IServiceProvider_RemoteQueryService_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif  /* __IServiceProvider_INTERFACE_DEFINED__ */

extern RPC_IF_HANDLE __MIDL_itf_servprov_0083_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_servprov_0083_v0_0_s_ifspec;

HRESULT STDMETHODCALLTYPE IServiceProvider_QueryService_Proxy(IServiceProvider*,REFGUID,REFIID,void**);
HRESULT STDMETHODCALLTYPE IServiceProvider_QueryService_Stub(IServiceProvider*,REFGUID,REFIID,IUnknown**);

#ifdef __cplusplus
}
#endif

#endif /* _SERVPROV_H */
