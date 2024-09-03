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
#endif

#ifndef _UNKNWN_H
#define _UNKNWN_H

#ifdef __ORANGEC__ 
#define _FORCENAMELESSUNION
#pragma GCC system_header
#endif

/* Forward Declarations */

#ifndef __IUnknown_FWD_DEFINED__
#define __IUnknown_FWD_DEFINED__
typedef interface IUnknown IUnknown;
#endif

#ifndef __AsyncIUnknown_FWD_DEFINED__
#define __AsyncIUnknown_FWD_DEFINED__
typedef interface AsyncIUnknown AsyncIUnknown;
#endif

#ifndef __IClassFactory_FWD_DEFINED__
#define __IClassFactory_FWD_DEFINED__
typedef interface IClassFactory IClassFactory;
#endif

#include "wtypes.h"

#ifdef __cplusplus
extern "C"{
#endif

extern RPC_IF_HANDLE __MIDL_itf_unknwn_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_unknwn_0000_v0_0_s_ifspec;


#ifndef __IUnknown_INTERFACE_DEFINED__
#define __IUnknown_INTERFACE_DEFINED__

typedef IUnknown *LPUNKNOWN;
EXTERN_C const IID IID_IUnknown;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("00000000-0000-0000-C000-000000000046")
IUnknown {
    public:
    BEGIN_INTERFACE
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID,void**) = 0;
    virtual ULONG STDMETHODCALLTYPE AddRef(void) = 0;
    virtual ULONG STDMETHODCALLTYPE Release(void) = 0;
    END_INTERFACE
};

#else

typedef struct IUnknownVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IUnknown*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IUnknown*);
    ULONG (STDMETHODCALLTYPE *Release)(IUnknown*);
    END_INTERFACE
} IUnknownVtbl;

interface IUnknown {
    CONST_VTBL struct IUnknownVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUnknown_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IUnknown_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IUnknown_Release(This)  (This)->lpVtbl->Release(This)
#endif /* COBJMACROS */
#endif /* __cplusplus || !CINTERFACE */

HRESULT STDMETHODCALLTYPE IUnknown_QueryInterface_Proxy(IUnknown*,REFIID,void**);
ULONG STDMETHODCALLTYPE IUnknown_AddRef_Proxy(IUnknown*);
ULONG STDMETHODCALLTYPE IUnknown_Release_Proxy(IUnknown*);

void __RPC_STUB IUnknown_QueryInterface_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
void __RPC_STUB IUnknown_AddRef_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
void __RPC_STUB IUnknown_Release_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif  /* __IUnknown_INTERFACE_DEFINED__ */


extern RPC_IF_HANDLE __MIDL_itf_unknwn_0005_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_unknwn_0005_v0_0_s_ifspec;

#ifndef __AsyncIUnknown_INTERFACE_DEFINED__
#define __AsyncIUnknown_INTERFACE_DEFINED__

EXTERN_C const IID IID_AsyncIUnknown;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("000e0000-0000-0000-C000-000000000046")
AsyncIUnknown : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Begin_QueryInterface(REFIID) = 0;
    virtual HRESULT STDMETHODCALLTYPE Finish_QueryInterface(void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE Begin_AddRef(void) = 0;
    virtual ULONG STDMETHODCALLTYPE Finish_AddRef(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Begin_Release(void) = 0;
    virtual ULONG STDMETHODCALLTYPE Finish_Release(void) = 0;
};

#else

typedef struct AsyncIUnknownVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(AsyncIUnknown*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(AsyncIUnknown*);
    ULONG (STDMETHODCALLTYPE *Release)(AsyncIUnknown*);
    HRESULT (STDMETHODCALLTYPE *Begin_QueryInterface)(AsyncIUnknown*,REFIID);
    HRESULT (STDMETHODCALLTYPE *Finish_QueryInterface)(AsyncIUnknown*,void**);
    HRESULT (STDMETHODCALLTYPE *Begin_AddRef)(AsyncIUnknown*);
    ULONG (STDMETHODCALLTYPE *Finish_AddRef)(AsyncIUnknown*);
    HRESULT (STDMETHODCALLTYPE *Begin_Release)(AsyncIUnknown*);
    ULONG (STDMETHODCALLTYPE *Finish_Release)(AsyncIUnknown*);
    END_INTERFACE
} AsyncIUnknownVtbl;

interface AsyncIUnknown {
    CONST_VTBL struct AsyncIUnknownVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define AsyncIUnknown_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define AsyncIUnknown_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define AsyncIUnknown_Release(This)  (This)->lpVtbl->Release(This)
#define AsyncIUnknown_Begin_QueryInterface(This,riid)  (This)->lpVtbl->Begin_QueryInterface(This,riid)
#define AsyncIUnknown_Finish_QueryInterface(This,ppvObject)  (This)->lpVtbl->Finish_QueryInterface(This,ppvObject)
#define AsyncIUnknown_Begin_AddRef(This)  (This)->lpVtbl->Begin_AddRef(This)
#define AsyncIUnknown_Finish_AddRef(This)  (This)->lpVtbl->Finish_AddRef(This)
#define AsyncIUnknown_Begin_Release(This)  (This)->lpVtbl->Begin_Release(This)
#define AsyncIUnknown_Finish_Release(This)  (This)->lpVtbl->Finish_Release(This)
#endif /* COBJMACROS */
#endif /* __cplusplus || !CINTERFACE */

HRESULT STDMETHODCALLTYPE AsyncIUnknown_Begin_QueryInterface_Proxy(AsyncIUnknown*,REFIID);
HRESULT STDMETHODCALLTYPE AsyncIUnknown_Finish_QueryInterface_Proxy(AsyncIUnknown*,void**);
HRESULT STDMETHODCALLTYPE AsyncIUnknown_Begin_AddRef_Proxy(AsyncIUnknown*);
ULONG STDMETHODCALLTYPE AsyncIUnknown_Finish_AddRef_Proxy(AsyncIUnknown*);
HRESULT STDMETHODCALLTYPE AsyncIUnknown_Begin_Release_Proxy(AsyncIUnknown*);
ULONG STDMETHODCALLTYPE AsyncIUnknown_Finish_Release_Proxy(AsyncIUnknown*);

void __RPC_STUB AsyncIUnknown_Begin_QueryInterface_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
void __RPC_STUB AsyncIUnknown_Finish_QueryInterface_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
void __RPC_STUB AsyncIUnknown_Begin_AddRef_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
void __RPC_STUB AsyncIUnknown_Finish_AddRef_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
void __RPC_STUB AsyncIUnknown_Begin_Release_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
void __RPC_STUB AsyncIUnknown_Finish_Release_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __AsyncIUnknown_INTERFACE_DEFINED__ */


#ifndef __IClassFactory_INTERFACE_DEFINED__
#define __IClassFactory_INTERFACE_DEFINED__

typedef IClassFactory *LPCLASSFACTORY;
EXTERN_C const IID IID_IClassFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00000001-0000-0000-C000-000000000046")
IClassFactory : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown*,REFIID,void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE LockServer(BOOL) = 0;
};
#else
typedef struct IClassFactoryVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IClassFactory*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IClassFactory*);
    ULONG (STDMETHODCALLTYPE *Release)(IClassFactory*);
    HRESULT (STDMETHODCALLTYPE *CreateInstance )(IClassFactory*,IUnknown*,REFIID,void**);
    HRESULT (STDMETHODCALLTYPE *LockServer)(IClassFactory*,BOOL);
    END_INTERFACE
} IClassFactoryVtbl;

interface IClassFactory {
    CONST_VTBL struct IClassFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IClassFactory_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IClassFactory_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IClassFactory_Release(This)  (This)->lpVtbl->Release(This)
#define IClassFactory_CreateInstance(This,pUnkOuter,riid,ppvObject)  (This)->lpVtbl->CreateInstance(This,pUnkOuter,riid,ppvObject)
#define IClassFactory_LockServer(This,fLock)  (This)->lpVtbl->LockServer(This,fLock)
#endif /* COBJMACROS */
#endif /* __cplusplus || !CINTERFACE */

HRESULT STDMETHODCALLTYPE IClassFactory_RemoteCreateInstance_Proxy(IClassFactory*,REFIID,IUnknown**);
HRESULT __stdcall IClassFactory_RemoteLockServer_Proxy(IClassFactory*,BOOL);

void __RPC_STUB IClassFactory_RemoteCreateInstance_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
void __RPC_STUB IClassFactory_RemoteLockServer_Stub(IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);

#endif /* __IClassFactory_INTERFACE_DEFINED__ */


HRESULT STDMETHODCALLTYPE IClassFactory_CreateInstance_Proxy(IClassFactory*,IUnknown*,REFIID,void**);
HRESULT STDMETHODCALLTYPE IClassFactory_CreateInstance_Stub(IClassFactory*,REFIID,IUnknown**);
HRESULT STDMETHODCALLTYPE IClassFactory_LockServer_Proxy(IClassFactory*,BOOL);
HRESULT __stdcall IClassFactory_LockServer_Stub(IClassFactory*,BOOL);

#ifdef __cplusplus
}
#endif

#endif /* _UNKNWN_H */
