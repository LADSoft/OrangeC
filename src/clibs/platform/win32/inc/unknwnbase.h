#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef _UNKNWNBASE_H
#define _UNKNWNBASE_H

#ifdef __ORANGEC__ 
#define _FORCENAMELESSUNION
#pragma GCC system_header
#endif

/* Forward Declarations */ 

#ifndef __IUnknown_FWD_DEFINED__
#define __IUnknown_FWD_DEFINED__
typedef interface IUnknown IUnknown;
#endif /* __IUnknown_FWD_DEFINED__ */

#ifndef __AsyncIUnknown_FWD_DEFINED__
#define __AsyncIUnknown_FWD_DEFINED__
typedef interface AsyncIUnknown AsyncIUnknown;
#endif /* __AsyncIUnknown_FWD_DEFINED__ */

#ifndef __IClassFactory_FWD_DEFINED__
#define __IClassFactory_FWD_DEFINED__
typedef interface IClassFactory IClassFactory;
#endif /* __IClassFactory_FWD_DEFINED__ */

#include "wtypesbase.h"

#ifdef __cplusplus
extern "C"{
#endif 

/* #include <winapifamily.h> */

extern RPC_IF_HANDLE __MIDL_itf_unknwnbase_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_unknwnbase_0000_0000_v0_0_s_ifspec;

#ifndef __IUnknown_INTERFACE_DEFINED__
#define __IUnknown_INTERFACE_DEFINED__

typedef IUnknown *LPUNKNOWN;

EXTERN_C const IID IID_IUnknown;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("00000000-0000-0000-C000-000000000046") IUnknown {
    public:
    BEGIN_INTERFACE
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) = 0;
    virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
    virtual ULONG STDMETHODCALLTYPE Release(void) = 0;
    END_INTERFACE
};

#else /* C style interface */

typedef struct IUnknownVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUnknown *This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IUnknown * This);
    ULONG(STDMETHODCALLTYPE *Release)(IUnknown * This);
    END_INTERFACE
} IUnknownVtbl;

interface IUnknown {
    CONST_VTBL struct IUnknownVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUnknown_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IUnknown_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IUnknown_Release(This)  ((This)->lpVtbl->Release(This)) 
#endif /* COBJMACROS */

#endif /* C style interface */


HRESULT STDMETHODCALLTYPE IUnknown_QueryInterface_Proxy(IUnknown *This, REFIID riid, void **ppvObject);
void __RPC_STUB IUnknown_QueryInterface_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
ULONG STDMETHODCALLTYPE IUnknown_AddRef_Proxy(IUnknown *This);
void __RPC_STUB IUnknown_AddRef_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
ULONG STDMETHODCALLTYPE IUnknown_Release_Proxy(IUnknown *This);
void __RPC_STUB IUnknown_Release_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);

#endif /* __IUnknown_INTERFACE_DEFINED__ */

extern RPC_IF_HANDLE __MIDL_itf_unknwnbase_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_unknwnbase_0000_0001_v0_0_s_ifspec;

#ifndef __AsyncIUnknown_INTERFACE_DEFINED__
#define __AsyncIUnknown_INTERFACE_DEFINED__

EXTERN_C const IID IID_AsyncIUnknown;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("000e0000-0000-0000-C000-000000000046")
AsyncIUnknown:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Begin_QueryInterface(REFIID riid) = 0;
    virtual HRESULT STDMETHODCALLTYPE Finish_QueryInterface(void **ppvObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE Begin_AddRef(void) = 0;
    virtual ULONG STDMETHODCALLTYPE Finish_AddRef(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Begin_Release(void) = 0;
    virtual ULONG STDMETHODCALLTYPE Finish_Release(void) = 0;
};

#else /* C style interface */

typedef struct AsyncIUnknownVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(AsyncIUnknown *This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(AsyncIUnknown * This);
    ULONG (STDMETHODCALLTYPE *Release)(AsyncIUnknown * This);
    HRESULT (STDMETHODCALLTYPE *Begin_QueryInterface)(AsyncIUnknown * This, REFIID riid);
    HRESULT (STDMETHODCALLTYPE *Finish_QueryInterface)(AsyncIUnknown * This, void **ppvObject);
    HRESULT (STDMETHODCALLTYPE *Begin_AddRef)(AsyncIUnknown * This);
    ULONG (STDMETHODCALLTYPE *Finish_AddRef)(AsyncIUnknown * This);
    HRESULT (STDMETHODCALLTYPE *Begin_Release)(AsyncIUnknown * This);
    ULONG (STDMETHODCALLTYPE *Finish_Release)(AsyncIUnknown * This);
    END_INTERFACE
} AsyncIUnknownVtbl;

interface AsyncIUnknown {
    CONST_VTBL struct AsyncIUnknownVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define AsyncIUnknown_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define AsyncIUnknown_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define AsyncIUnknown_Release(This)  ((This)->lpVtbl->Release(This)) 
#define AsyncIUnknown_Begin_QueryInterface(This,riid)  ((This)->lpVtbl->Begin_QueryInterface(This,riid)) 
#define AsyncIUnknown_Finish_QueryInterface(This,ppvObject)  ((This)->lpVtbl->Finish_QueryInterface(This,ppvObject)) 
#define AsyncIUnknown_Begin_AddRef(This)  ((This)->lpVtbl->Begin_AddRef(This)) 
#define AsyncIUnknown_Finish_AddRef(This)  ((This)->lpVtbl->Finish_AddRef(This)) 
#define AsyncIUnknown_Begin_Release(This)  ((This)->lpVtbl->Begin_Release(This)) 
#define AsyncIUnknown_Finish_Release(This)  ((This)->lpVtbl->Finish_Release(This)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __AsyncIUnknown_INTERFACE_DEFINED__ */

extern RPC_IF_HANDLE __MIDL_itf_unknwnbase_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_unknwnbase_0000_0002_v0_0_s_ifspec;

#ifndef __IClassFactory_INTERFACE_DEFINED__
#define __IClassFactory_INTERFACE_DEFINED__

typedef IClassFactory *LPCLASSFACTORY;

EXTERN_C const IID IID_IClassFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("00000001-0000-0000-C000-000000000046")
IClassFactory:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown * pUnkOuter, REFIID riid, void **ppvObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE LockServer(BOOL fLock) = 0;
};

#else /* C style interface */

typedef struct IClassFactoryVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IClassFactory *This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IClassFactory *This);
    ULONG(STDMETHODCALLTYPE *Release)(IClassFactory *This);
    HRESULT(STDMETHODCALLTYPE *CreateInstance)(IClassFactory *This, IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
    HRESULT(STDMETHODCALLTYPE *LockServer)(IClassFactory * This, BOOL fLock);
    END_INTERFACE
} IClassFactoryVtbl;

interface IClassFactory {
    CONST_VTBL struct IClassFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IClassFactory_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IClassFactory_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IClassFactory_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IClassFactory_CreateInstance(This,pUnkOuter,riid,ppvObject)  ((This)->lpVtbl->CreateInstance(This,pUnkOuter,riid,ppvObject)) 
#define IClassFactory_LockServer(This,fLock)  ((This)->lpVtbl->LockServer(This,fLock)) 
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IClassFactory_RemoteCreateInstance_Proxy(IClassFactory *This, REFIID riid, IUnknown **ppvObject);
void __RPC_STUB IClassFactory_RemoteCreateInstance_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
HRESULT __stdcall IClassFactory_RemoteLockServer_Proxy(IClassFactory *This, BOOL fLock);
void __RPC_STUB IClassFactory_RemoteLockServer_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);

#endif /* __IClassFactory_INTERFACE_DEFINED__ */

extern RPC_IF_HANDLE __MIDL_itf_unknwnbase_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_unknwnbase_0000_0003_v0_0_s_ifspec;

HRESULT STDMETHODCALLTYPE IClassFactory_CreateInstance_Proxy(IClassFactory *This, IUnknown *pUnkOuter, REFIID riid, void **ppvObject);
HRESULT STDMETHODCALLTYPE IClassFactory_CreateInstance_Stub(IClassFactory *This, REFIID riid, IUnknown **ppvObject);
HRESULT STDMETHODCALLTYPE IClassFactory_LockServer_Proxy(IClassFactory *This, BOOL fLock);
HRESULT __stdcall IClassFactory_LockServer_Stub(IClassFactory *This, BOOL fLock);

#ifdef __cplusplus
}
#endif

#endif /* _UNKNWNBASE_H */
