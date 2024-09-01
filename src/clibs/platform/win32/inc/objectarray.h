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
#endif /* COM_NO_WINDOWS_H */

#ifndef _OBJECTARRAY_H
#define _OBJECTARRAY_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */ 

#ifndef __IObjectArray_FWD_DEFINED__
#define __IObjectArray_FWD_DEFINED__
typedef interface IObjectArray IObjectArray;
#endif

#ifndef __IObjectCollection_FWD_DEFINED__
#define __IObjectCollection_FWD_DEFINED__
typedef interface IObjectCollection IObjectCollection;
#endif

#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IObjectArray_INTERFACE_DEFINED__
#define __IObjectArray_INTERFACE_DEFINED__

EXTERN_C const IID IID_IObjectArray;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("92CA9DCD-5622-4bba-A805-5E9F541BD8C9") IObjectArray : public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCount(UINT * pcObjects) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAt(UINT uiIndex, REFIID riid, void **ppv) = 0;
};
#else /* C style interface */

typedef struct IObjectArrayVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IObjectArray * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IObjectArray * This);
    ULONG (STDMETHODCALLTYPE *Release)(IObjectArray * This);
    HRESULT (STDMETHODCALLTYPE *GetCount)(IObjectArray * This, UINT * pcObjects);
    HRESULT (STDMETHODCALLTYPE *GetAt)(IObjectArray * This, UINT uiIndex, REFIID riid, void **ppv);
    END_INTERFACE
} IObjectArrayVtbl;

interface IObjectArray {
    CONST_VTBL struct IObjectArrayVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IObjectArray_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IObjectArray_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IObjectArray_Release(This)  ((This)->lpVtbl->Release(This))
#define IObjectArray_GetCount(This,pcObjects)  ((This)->lpVtbl->GetCount(This,pcObjects))
#define IObjectArray_GetAt(This,uiIndex,riid,ppv)  ((This)->lpVtbl->GetAt(This,uiIndex,riid,ppv)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IObjectArray_INTERFACE_DEFINED__ */


#ifndef __IObjectCollection_INTERFACE_DEFINED__
#define __IObjectCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_IObjectCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5632b1a4-e38a-400a-928a-d4cd63230295") IObjectCollection : public IObjectArray
{
    public:
    virtual HRESULT STDMETHODCALLTYPE AddObject(IUnknown * punk) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddFromArray(IObjectArray * poaSource) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveObjectAt(UINT uiIndex) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clear(void) = 0;
};
#else /* C style interface */
typedef struct IObjectCollectionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IObjectCollection * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IObjectCollection * This);
    ULONG (STDMETHODCALLTYPE *Release)(IObjectCollection * This);
    HRESULT (STDMETHODCALLTYPE *GetCount)(IObjectCollection * This, UINT * pcObjects);
    HRESULT (STDMETHODCALLTYPE *GetAt)(IObjectCollection * This, UINT uiIndex, REFIID riid, void **ppv);
    HRESULT (STDMETHODCALLTYPE *AddObject)(IObjectCollection * This, IUnknown * punk);
    HRESULT (STDMETHODCALLTYPE *AddFromArray)(IObjectCollection * This, IObjectArray * poaSource);
    HRESULT (STDMETHODCALLTYPE *RemoveObjectAt)(IObjectCollection * This, UINT uiIndex);
    HRESULT (STDMETHODCALLTYPE *Clear)(IObjectCollection * This);
    END_INTERFACE
} IObjectCollectionVtbl;

interface IObjectCollection {
    CONST_VTBL struct IObjectCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IObjectCollection_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IObjectCollection_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IObjectCollection_Release(This)  ((This)->lpVtbl->Release(This))
#define IObjectCollection_GetCount(This,pcObjects)  ((This)->lpVtbl->GetCount(This,pcObjects))
#define IObjectCollection_GetAt(This,uiIndex,riid,ppv)  ((This)->lpVtbl->GetAt(This,uiIndex,riid,ppv))
#define IObjectCollection_AddObject(This,punk)  ((This)->lpVtbl->AddObject(This,punk))
#define IObjectCollection_AddFromArray(This,poaSource)  ((This)->lpVtbl->AddFromArray(This,poaSource))
#define IObjectCollection_RemoveObjectAt(This,uiIndex)  ((This)->lpVtbl->RemoveObjectAt(This,uiIndex))
#define IObjectCollection_Clear(This)  ((This)->lpVtbl->Clear(This))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IObjectCollection_INTERFACE_DEFINED__ */

#ifdef __cplusplus
}
#endif

#endif /* _OBJECTARRAY_H */
