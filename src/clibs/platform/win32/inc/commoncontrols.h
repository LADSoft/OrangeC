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
#endif

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /* COM_NO_WINDOWS_H */

#ifndef _COMMONCONTROLS_H
#define _COMMONCONTROLS_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IImageList_FWD_DEFINED__
#define __IImageList_FWD_DEFINED__
typedef interface IImageList IImageList;
#endif

#ifndef __IImageList2_FWD_DEFINED__
#define __IImageList2_FWD_DEFINED__
typedef interface IImageList2 IImageList2;
#endif

#ifndef __ImageList_FWD_DEFINED__
#define __ImageList_FWD_DEFINED__

#ifdef __cplusplus
typedef class ImageList ImageList;
#else
typedef struct ImageList ImageList;
#endif /* __cplusplus */

#endif /* __ImageList_FWD_DEFINED__ */

#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#if (_WIN32_IE >= 0x0501)
#ifndef WINCOMMCTRLAPI
#if !defined(_COMCTL32_) && defined(_WIN32)
#define WINCOMMCTRLAPI DECLSPEC_IMPORT
#else
#define WINCOMMCTRLAPI
#endif
#endif /* WINCOMMCTRLAPI */

#if _WIN32_WINNT >= 0x0600
WINCOMMCTRLAPI HRESULT WINAPI ImageList_CoCreateInstance(REFCLSID,const IUnknown *,REFIID,void **);
#endif

#define ILIF_ALPHA  0x00000001
#define ILIF_LOWQUALITY  0x00000002
#define ILDRF_IMAGELOWQUALITY  0x00000001
#define ILDRF_OVERLAYLOWQUALITY  0x00000010

extern RPC_IF_HANDLE __MIDL_itf_commoncontrols_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_commoncontrols_0000_0000_v0_0_s_ifspec;

#ifndef __IImageList_INTERFACE_DEFINED__
#define __IImageList_INTERFACE_DEFINED__

EXTERN_C const IID IID_IImageList;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("46EB5926-582E-4017-9FDF-E8998DAA0950") IImageList : public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Add(HBITMAP,HBITMAP,int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReplaceIcon(int,HICON,int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOverlayImage(int,int) = 0;
    virtual HRESULT STDMETHODCALLTYPE Replace(int,HBITMAP,HBITMAP) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddMasked(HBITMAP,COLORREF,int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Draw(IMAGELISTDRAWPARAMS*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Remove(int) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIcon(int,UINT,HICON*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetImageInfo(int,IMAGEINFO*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Copy(int,IUnknown*,int,UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE Merge(int,IUnknown*,int,int,int,REFIID,void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(REFIID,void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetImageRect(int,RECT*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIconSize(int*,int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetIconSize(int,int) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetImageCount(int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetImageCount(UINT) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetBkColor(COLORREF,COLORREF*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBkColor(COLORREF*) = 0;
    virtual HRESULT STDMETHODCALLTYPE BeginDrag(int,int,int) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndDrag(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE DragEnter(HWND,int,int) = 0;
    virtual HRESULT STDMETHODCALLTYPE DragLeave(HWND) = 0;
    virtual HRESULT STDMETHODCALLTYPE DragMove(int,int) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDragCursorImage(IUnknown*,int,int,int) = 0;
    virtual HRESULT STDMETHODCALLTYPE DragShowNolock(BOOL) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDragImage(POINT*,POINT*,REFIID,void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetItemFlags(int,DWORD*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOverlayImage(int,int*) = 0;
};
#else /* C style interface */
typedef struct IImageListVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IImageList*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IImageList*);
    ULONG (STDMETHODCALLTYPE *Release)(IImageList*);
    HRESULT (STDMETHODCALLTYPE *Add)(IImageList*,HBITMAP,HBITMAP,int*);
    HRESULT (STDMETHODCALLTYPE *ReplaceIcon)(IImageList*,int,HICON,int*);
    HRESULT (STDMETHODCALLTYPE *SetOverlayImage)(IImageList*,int,int);
    HRESULT (STDMETHODCALLTYPE *Replace)(IImageList*,int,HBITMAP,HBITMAP);
    HRESULT (STDMETHODCALLTYPE *AddMasked)(IImageList*,HBITMAP,COLORREF,int*);
    HRESULT (STDMETHODCALLTYPE *Draw)(IImageList*,IMAGELISTDRAWPARAMS*);
    HRESULT (STDMETHODCALLTYPE *Remove)(IImageList*,int);
    HRESULT (STDMETHODCALLTYPE *GetIcon)(IImageList*,int,UINT,HICON*);
    HRESULT (STDMETHODCALLTYPE *GetImageInfo)(IImageList*,int,IMAGEINFO*);
    HRESULT (STDMETHODCALLTYPE *Copy)(IImageList*,int,IUnknown*,int,UINT);
    HRESULT (STDMETHODCALLTYPE *Merge)(IImageList*,int,IUnknown*,int,int,int,REFIID,void**);
    HRESULT (STDMETHODCALLTYPE *Clone)(IImageList*,REFIID,void**);
    HRESULT (STDMETHODCALLTYPE *GetImageRect)(IImageList*,int,RECT*);
    HRESULT (STDMETHODCALLTYPE *GetIconSize)(IImageList*,int*,int*);
    HRESULT (STDMETHODCALLTYPE *SetIconSize)(IImageList*,int,int);
    HRESULT (STDMETHODCALLTYPE *GetImageCount)(IImageList*,int*);
    HRESULT (STDMETHODCALLTYPE *SetImageCount)(IImageList*,UINT);
    HRESULT (STDMETHODCALLTYPE *SetBkColor)(IImageList*,COLORREF,COLORREF*);
    HRESULT (STDMETHODCALLTYPE *GetBkColor)(IImageList*,COLORREF*);
    HRESULT (STDMETHODCALLTYPE *BeginDrag)(IImageList*,int,int,int);
    HRESULT (STDMETHODCALLTYPE *EndDrag)(IImageList*);
    HRESULT (STDMETHODCALLTYPE *DragEnter)(IImageList*,HWND,int,int);
    HRESULT (STDMETHODCALLTYPE *DragLeave)(IImageList*,HWND);
    HRESULT (STDMETHODCALLTYPE *DragMove)(IImageList*,int,int);
    HRESULT (STDMETHODCALLTYPE *SetDragCursorImage)(IImageList*,IUnknown*,int,int,int);
    HRESULT (STDMETHODCALLTYPE *DragShowNolock)(IImageList*,BOOL);
    HRESULT (STDMETHODCALLTYPE *GetDragImage)(IImageList*,POINT*,POINT*,REFIID,void**);
    HRESULT (STDMETHODCALLTYPE *GetItemFlags)(IImageList*,int,DWORD*);
    HRESULT (STDMETHODCALLTYPE *GetOverlayImage)(IImageList*,int,int*);
    END_INTERFACE
} IImageListVtbl;

interface IImageList {
    CONST_VTBL struct IImageListVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IImageList_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IImageList_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IImageList_Release(This)  ((This)->lpVtbl->Release(This))
#define IImageList_Add(This,hbmImage,hbmMask,pi)  ((This)->lpVtbl->Add(This,hbmImage,hbmMask,pi))
#define IImageList_ReplaceIcon(This,i,hicon,pi)  ((This)->lpVtbl->ReplaceIcon(This,i,hicon,pi))
#define IImageList_SetOverlayImage(This,iImage,iOverlay)  ((This)->lpVtbl->SetOverlayImage(This,iImage,iOverlay))
#define IImageList_Replace(This,i,hbmImage,hbmMask)  ((This)->lpVtbl->Replace(This,i,hbmImage,hbmMask))
#define IImageList_AddMasked(This,hbmImage,crMask,pi)  ((This)->lpVtbl->AddMasked(This,hbmImage,crMask,pi))
#define IImageList_Draw(This,pimldp)  ((This)->lpVtbl->Draw(This,pimldp))
#define IImageList_Remove(This,i)  ((This)->lpVtbl->Remove(This,i))
#define IImageList_GetIcon(This,i,flags,picon)  ((This)->lpVtbl->GetIcon(This,i,flags,picon))
#define IImageList_GetImageInfo(This,i,pImageInfo)  ((This)->lpVtbl->GetImageInfo(This,i,pImageInfo))
#define IImageList_Copy(This,iDst,punkSrc,iSrc,uFlags)  ((This)->lpVtbl->Copy(This,iDst,punkSrc,iSrc,uFlags))
#define IImageList_Merge(This,i1,punk2,i2,dx,dy,riid,ppv)  ((This)->lpVtbl->Merge(This,i1,punk2,i2,dx,dy,riid,ppv))
#define IImageList_Clone(This,riid,ppv)  ((This)->lpVtbl->Clone(This,riid,ppv))
#define IImageList_GetImageRect(This,i,prc)  ((This)->lpVtbl->GetImageRect(This,i,prc))
#define IImageList_GetIconSize(This,cx,cy)  ((This)->lpVtbl->GetIconSize(This,cx,cy))
#define IImageList_SetIconSize(This,cx,cy)  ((This)->lpVtbl->SetIconSize(This,cx,cy))
#define IImageList_GetImageCount(This,pi)  ((This)->lpVtbl->GetImageCount(This,pi))
#define IImageList_SetImageCount(This,uNewCount)  ((This)->lpVtbl->SetImageCount(This,uNewCount))
#define IImageList_SetBkColor(This,clrBk,pclr)  ((This)->lpVtbl->SetBkColor(This,clrBk,pclr))
#define IImageList_GetBkColor(This,pclr)  ((This)->lpVtbl->GetBkColor(This,pclr))
#define IImageList_BeginDrag(This,iTrack,dxHotspot,dyHotspot)  ((This)->lpVtbl->BeginDrag(This,iTrack,dxHotspot,dyHotspot))
#define IImageList_EndDrag(This)  ((This)->lpVtbl->EndDrag(This))
#define IImageList_DragEnter(This,hwndLock,x,y)  ((This)->lpVtbl->DragEnter(This,hwndLock,x,y))
#define IImageList_DragLeave(This,hwndLock)  ((This)->lpVtbl->DragLeave(This,hwndLock))
#define IImageList_DragMove(This,x,y)  ((This)->lpVtbl->DragMove(This,x,y))
#define IImageList_SetDragCursorImage(This,punk,iDrag,dxHotspot,dyHotspot)  ((This)->lpVtbl->SetDragCursorImage(This,punk,iDrag,dxHotspot,dyHotspot))
#define IImageList_DragShowNolock(This,fShow)  ((This)->lpVtbl->DragShowNolock(This,fShow))
#define IImageList_GetDragImage(This,ppt,pptHotspot,riid,ppv)  ((This)->lpVtbl->GetDragImage(This,ppt,pptHotspot,riid,ppv))
#define IImageList_GetItemFlags(This,i,dwFlags)  ((This)->lpVtbl->GetItemFlags(This,i,dwFlags))
#define IImageList_GetOverlayImage(This,iOverlay,piIndex)  ((This)->lpVtbl->GetOverlayImage(This,iOverlay,piIndex))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IImageList_INTERFACE_DEFINED__ */

#define ILR_DEFAULT  0x0000
#define ILR_HORIZONTAL_LEFT  0x0000
#define ILR_HORIZONTAL_CENTER  0x0001
#define ILR_HORIZONTAL_RIGHT  0x0002
#define ILR_VERTICAL_TOP  0x0000
#define ILR_VERTICAL_CENTER  0x0010
#define ILR_VERTICAL_BOTTOM  0x0020
#define ILR_SCALE_CLIP  0x0000
#define ILR_SCALE_ASPECTRATIO  0x0100

extern RPC_IF_HANDLE __MIDL_itf_commoncontrols_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_commoncontrols_0000_0001_v0_0_s_ifspec;

#ifndef __IImageList2_INTERFACE_DEFINED__
#define __IImageList2_INTERFACE_DEFINED__

#define ILGOS_ALWAYS  0x00000000
#define ILGOS_FROMSTANDBY  0x00000001
#define ILFIP_ALWAYS  0x00000000
#define ILFIP_FROMSTANDBY  0x00000001
#define ILDI_PURGE  0x00000001
#define ILDI_STANDBY  0x00000002
#define ILDI_RESETACCESS  0x00000004
#define ILDI_QUERYACCESS  0x00000008

typedef struct tagIMAGELISTSTATS {
    DWORD cbSize;
    int cAlloc;
    int cUsed;
    int cStandby;
} IMAGELISTSTATS;


EXTERN_C const IID IID_IImageList2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("192b9d83-50fc-457b-90a0-2b82a8b5dae1") IImageList2 : public IImageList {
    public:
    virtual HRESULT STDMETHODCALLTYPE Resize(int,int) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOriginalSize(int,DWORD,int*,int*) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOriginalSize(int,int,int) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCallback(IUnknown*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCallback(REFIID,void**) = 0;
    virtual HRESULT STDMETHODCALLTYPE ForceImagePresent(int,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE DiscardImages(int,int,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE PreloadImages(IMAGELISTDRAWPARAMS*) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStatistics(IMAGELISTSTATS*) = 0;
    virtual HRESULT STDMETHODCALLTYPE Initialize(int,int,UINT,int,int) = 0;
    virtual HRESULT STDMETHODCALLTYPE Replace2(int,HBITMAP,HBITMAP,IUnknown*,DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReplaceFromImageList(int,IImageList*,int,IUnknown*,DWORD) = 0;
};
#else /* C style interface */
typedef struct IImageList2Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IImageList2*,REFIID,void**);
    ULONG (STDMETHODCALLTYPE *AddRef)(IImageList2*);
    ULONG (STDMETHODCALLTYPE *Release)(IImageList2*);
    HRESULT (STDMETHODCALLTYPE *Add)(IImageList2*,HBITMAP,HBITMAP,int*);
    HRESULT (STDMETHODCALLTYPE *ReplaceIcon)(IImageList2*,int,HICON,int*);
    HRESULT (STDMETHODCALLTYPE *SetOverlayImage)(IImageList2*,int,int);
    HRESULT (STDMETHODCALLTYPE *Replace)(IImageList2*,int,HBITMAP,HBITMAP);
    HRESULT (STDMETHODCALLTYPE *AddMasked)(IImageList2*,HBITMAP,COLORREF,int*);
    HRESULT (STDMETHODCALLTYPE *Draw)(IImageList2*,IMAGELISTDRAWPARAMS*);
    HRESULT (STDMETHODCALLTYPE *Remove)(IImageList2*,int);
    HRESULT (STDMETHODCALLTYPE *GetIcon)(IImageList2*,int,UINT,HICON*);
    HRESULT (STDMETHODCALLTYPE *GetImageInfo)(IImageList2*,int,IMAGEINFO*);
    HRESULT (STDMETHODCALLTYPE *Copy)(IImageList2*,int,IUnknown*,int,UINT);
    HRESULT (STDMETHODCALLTYPE *Merge)(IImageList2*,int,IUnknown*,int,int,int,REFIID,void**);
    HRESULT (STDMETHODCALLTYPE *Clone)(IImageList2*,REFIID,void**);
    HRESULT (STDMETHODCALLTYPE *GetImageRect)(IImageList2*,int,RECT*);
    HRESULT (STDMETHODCALLTYPE *GetIconSize)(IImageList2*,int*,int*);
    HRESULT (STDMETHODCALLTYPE *SetIconSize)(IImageList2*,int,int);
    HRESULT (STDMETHODCALLTYPE *GetImageCount)(IImageList2*,int*);
    HRESULT (STDMETHODCALLTYPE *SetImageCount)(IImageList2*,UINT);
    HRESULT (STDMETHODCALLTYPE *SetBkColor)(IImageList2*,COLORREF,COLORREF*);
    HRESULT (STDMETHODCALLTYPE *GetBkColor)(IImageList2*,COLORREF*);
    HRESULT (STDMETHODCALLTYPE *BeginDrag)(IImageList2*,int,int,int);
    HRESULT (STDMETHODCALLTYPE *EndDrag)(IImageList2*);
    HRESULT (STDMETHODCALLTYPE *DragEnter)(IImageList2*,HWND,int,int);
    HRESULT (STDMETHODCALLTYPE *DragLeave)(IImageList2*,HWND);
    HRESULT (STDMETHODCALLTYPE *DragMove)(IImageList2*,int,int);
    HRESULT (STDMETHODCALLTYPE *SetDragCursorImage)(IImageList2*,IUnknown*,int,int,int);
    HRESULT (STDMETHODCALLTYPE *DragShowNolock)(IImageList2*,BOOL);
    HRESULT (STDMETHODCALLTYPE *GetDragImage)(IImageList2*,POINT*,POINT*,REFIID,void**);
    HRESULT (STDMETHODCALLTYPE *GetItemFlags)(IImageList2*,int,DWORD*);
    HRESULT (STDMETHODCALLTYPE *GetOverlayImage)(IImageList2*,int,int*);
    HRESULT (STDMETHODCALLTYPE *Resize)(IImageList2*,int,int);
    HRESULT (STDMETHODCALLTYPE *GetOriginalSize)(IImageList2*,int,DWORD,int*,int*);
    HRESULT (STDMETHODCALLTYPE *SetOriginalSize)(IImageList2*,int,int,int);
    HRESULT (STDMETHODCALLTYPE *SetCallback)(IImageList2*,IUnknown*);
    HRESULT (STDMETHODCALLTYPE *GetCallback)(IImageList2*,REFIID,void**);
    HRESULT (STDMETHODCALLTYPE *ForceImagePresent)(IImageList2*,int,DWORD);
    HRESULT (STDMETHODCALLTYPE *DiscardImages)(IImageList2*,int,int,DWORD);
    HRESULT (STDMETHODCALLTYPE *PreloadImages)(IImageList2*,IMAGELISTDRAWPARAMS*);
    HRESULT (STDMETHODCALLTYPE *GetStatistics)(IImageList2*,IMAGELISTSTATS*);
    HRESULT (STDMETHODCALLTYPE *Initialize)(IImageList2*,int,int,UINT,int,int);
    HRESULT (STDMETHODCALLTYPE *Replace2)(IImageList2*,int,HBITMAP,HBITMAP,IUnknown*,DWORD);
    HRESULT (STDMETHODCALLTYPE *ReplaceFromImageList)(IImageList2*,int,IImageList*,int,IUnknown*,DWORD);
    END_INTERFACE
} IImageList2Vtbl;

interface IImageList2 {
    CONST_VTBL struct IImageList2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IImageList2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IImageList2_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IImageList2_Release(This)  ((This)->lpVtbl->Release(This))
#define IImageList2_Add(This,hbmImage,hbmMask,pi)  ((This)->lpVtbl->Add(This,hbmImage,hbmMask,pi))
#define IImageList2_ReplaceIcon(This,i,hicon,pi)  ((This)->lpVtbl->ReplaceIcon(This,i,hicon,pi))
#define IImageList2_SetOverlayImage(This,iImage,iOverlay)  ((This)->lpVtbl->SetOverlayImage(This,iImage,iOverlay))
#define IImageList2_Replace(This,i,hbmImage,hbmMask)  ((This)->lpVtbl->Replace(This,i,hbmImage,hbmMask))
#define IImageList2_AddMasked(This,hbmImage,crMask,pi)  ((This)->lpVtbl->AddMasked(This,hbmImage,crMask,pi))
#define IImageList2_Draw(This,pimldp)  ((This)->lpVtbl->Draw(This,pimldp))
#define IImageList2_Remove(This,i)  ((This)->lpVtbl->Remove(This,i))
#define IImageList2_GetIcon(This,i,flags,picon)  ((This)->lpVtbl->GetIcon(This,i,flags,picon))
#define IImageList2_GetImageInfo(This,i,pImageInfo)  ((This)->lpVtbl->GetImageInfo(This,i,pImageInfo))
#define IImageList2_Copy(This,iDst,punkSrc,iSrc,uFlags)  ((This)->lpVtbl->Copy(This,iDst,punkSrc,iSrc,uFlags))
#define IImageList2_Merge(This,i1,punk2,i2,dx,dy,riid,ppv)  ((This)->lpVtbl->Merge(This,i1,punk2,i2,dx,dy,riid,ppv))
#define IImageList2_Clone(This,riid,ppv)  ((This)->lpVtbl->Clone(This,riid,ppv))
#define IImageList2_GetImageRect(This,i,prc)  ((This)->lpVtbl->GetImageRect(This,i,prc))
#define IImageList2_GetIconSize(This,cx,cy)  ((This)->lpVtbl->GetIconSize(This,cx,cy))
#define IImageList2_SetIconSize(This,cx,cy)  ((This)->lpVtbl->SetIconSize(This,cx,cy))
#define IImageList2_GetImageCount(This,pi)  ((This)->lpVtbl->GetImageCount(This,pi))
#define IImageList2_SetImageCount(This,uNewCount)  ((This)->lpVtbl->SetImageCount(This,uNewCount))
#define IImageList2_SetBkColor(This,clrBk,pclr)  ((This)->lpVtbl->SetBkColor(This,clrBk,pclr))
#define IImageList2_GetBkColor(This,pclr)  ((This)->lpVtbl->GetBkColor(This,pclr))
#define IImageList2_BeginDrag(This,iTrack,dxHotspot,dyHotspot)  ((This)->lpVtbl->BeginDrag(This,iTrack,dxHotspot,dyHotspot))
#define IImageList2_EndDrag(This)  ((This)->lpVtbl->EndDrag(This))
#define IImageList2_DragEnter(This,hwndLock,x,y)  ((This)->lpVtbl->DragEnter(This,hwndLock,x,y))
#define IImageList2_DragLeave(This,hwndLock)  ((This)->lpVtbl->DragLeave(This,hwndLock))
#define IImageList2_DragMove(This,x,y)  ((This)->lpVtbl->DragMove(This,x,y))
#define IImageList2_SetDragCursorImage(This,punk,iDrag,dxHotspot,dyHotspot)  ((This)->lpVtbl->SetDragCursorImage(This,punk,iDrag,dxHotspot,dyHotspot))
#define IImageList2_DragShowNolock(This,fShow)  ((This)->lpVtbl->DragShowNolock(This,fShow))
#define IImageList2_GetDragImage(This,ppt,pptHotspot,riid,ppv)  ((This)->lpVtbl->GetDragImage(This,ppt,pptHotspot,riid,ppv))
#define IImageList2_GetItemFlags(This,i,dwFlags)  ((This)->lpVtbl->GetItemFlags(This,i,dwFlags))
#define IImageList2_GetOverlayImage(This,iOverlay,piIndex)  ((This)->lpVtbl->GetOverlayImage(This,iOverlay,piIndex))
#define IImageList2_Resize(This,cxNewIconSize,cyNewIconSize)  ((This)->lpVtbl->Resize(This,cxNewIconSize,cyNewIconSize))
#define IImageList2_GetOriginalSize(This,iImage,dwFlags,pcx,pcy)  ((This)->lpVtbl->GetOriginalSize(This,iImage,dwFlags,pcx,pcy))
#define IImageList2_SetOriginalSize(This,iImage,cx,cy)  ((This)->lpVtbl->SetOriginalSize(This,iImage,cx,cy))
#define IImageList2_SetCallback(This,punk)  ((This)->lpVtbl->SetCallback(This,punk))
#define IImageList2_GetCallback(This,riid,ppv)  ((This)->lpVtbl->GetCallback(This,riid,ppv))
#define IImageList2_ForceImagePresent(This,iImage,dwFlags)  ((This)->lpVtbl->ForceImagePresent(This,iImage,dwFlags))
#define IImageList2_DiscardImages(This,iFirstImage,iLastImage,dwFlags)  ((This)->lpVtbl->DiscardImages(This,iFirstImage,iLastImage,dwFlags))
#define IImageList2_PreloadImages(This,pimldp)  ((This)->lpVtbl->PreloadImages(This,pimldp))
#define IImageList2_GetStatistics(This,pils)  ((This)->lpVtbl->GetStatistics(This,pils))
#define IImageList2_Initialize(This,cx,cy,flags,cInitial,cGrow)  ((This)->lpVtbl->Initialize(This,cx,cy,flags,cInitial,cGrow))
#define IImageList2_Replace2(This,i,hbmImage,hbmMask,punk,dwFlags)  ((This)->lpVtbl->Replace2(This,i,hbmImage,hbmMask,punk,dwFlags))
#define IImageList2_ReplaceFromImageList(This,i,pil,iSrc,punk,dwFlags)  ((This)->lpVtbl->ReplaceFromImageList(This,i,pil,iSrc,punk,dwFlags))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IImageList2_INTERFACE_DEFINED__ */

#ifndef __CommonControlObjects_LIBRARY_DEFINED__
#define __CommonControlObjects_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_CommonControlObjects;
EXTERN_C const CLSID CLSID_ImageList;

#ifdef __cplusplus
class DECLSPEC_UUID("7C476BA2-02B1-48f4-8048-B24619DDC058") ImageList;
#endif

#endif /* __CommonControlObjects_LIBRARY_DEFINED__ */

#endif /* if (_WIN32_IE >= 0x0501) */

extern RPC_IF_HANDLE __MIDL_itf_commoncontrols_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_commoncontrols_0000_0002_v0_0_s_ifspec;

#ifdef __cplusplus
}
#endif

#endif /* _COMMONCONTROLS_H */
