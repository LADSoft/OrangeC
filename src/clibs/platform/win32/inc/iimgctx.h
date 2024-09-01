#ifndef _IIMGCTX_H
#define _IIMGCTX_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* IImgCtx interface definitions */

#ifdef __cplusplus
extern "C" {
#endif
typedef void (CALLBACK *PFNIMGCTXCALLBACK)(void*,void*);
#ifdef __cplusplus
}
#endif

DEFINE_GUID(IID_IImgCtx,0x3050f3d7,0x98b5,0x11cf,0xbb,0x82,0x00,0xaa,0x00,0xbd,0xce,0x0b);
DEFINE_GUID(CLSID_IImgCtx,0x3050f3d6,0x98b5,0x11cf,0xbb,0x82,0x00,0xaa,0x00,0xbd,0xce,0x0b);

#define IMGCHG_SIZE  0x0001
#define IMGCHG_VIEW  0x0002
#define IMGCHG_COMPLETE  0x0004
#define IMGCHG_ANIMATE  0x0008
#define IMGCHG_MASK  0x000F

#define IMGLOAD_NOTLOADED  0x00100000
#define IMGLOAD_LOADING  0x00200000
#define IMGLOAD_STOPPED  0x00400000
#define IMGLOAD_ERROR  0x00800000
#define IMGLOAD_COMPLETE  0x01000000
#define IMGLOAD_MASK  0x01F00000

#define IMGBITS_NONE  0x02000000
#define IMGBITS_PARTIAL  0x04000000
#define IMGBITS_TOTAL  0x08000000
#define IMGBITS_MASK  0x0E000000

#define IMGANIM_ANIMATED  0x10000000
#define IMGANIM_MASK  0x10000000

#define IMGTRANS_OPAQUE  0x20000000
#define IMGTRANS_MASK  0x20000000

#define DWN_COLORMODE  0x0000003F
#define DWN_DOWNLOADONLY  0x00000040
#define DWN_FORCEDITHER  0x00000080
#define DWN_RAWIMAGE  0x00000100
#define DWN_MIRRORIMAGE  0x00000200

#undef INTERFACE
#define INTERFACE IImgCtx
DECLARE_INTERFACE_(IImgCtx,IUnknown)
{
#ifndef NO_BASEINTERFACE_FUNCS
    STDMETHOD(QueryInterface)(THIS_ REFIID,LPVOID*) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
#endif
    STDMETHOD(Load)(THIS_ LPCWSTR,DWORD) PURE;
    STDMETHOD(SelectChanges)(THIS_ ULONG,ULONG,BOOL) PURE;
    STDMETHOD(SetCallback)(THIS_ PFNIMGCTXCALLBACK,void*) PURE;
    STDMETHOD(Disconnect)(THIS) PURE;
    STDMETHOD(GetUpdateRects)(THIS_ struct tagRECT*,struct tagRECT*,long*) PURE;
    STDMETHOD(GetStateInfo)(THIS_ ULONG*,struct tagSIZE*,BOOL) PURE;
    STDMETHOD(GetPalette)(THIS_ HPALETTE*) PURE;
    STDMETHOD(Draw)(THIS_ HDC,struct tagRECT*) PURE;
    STDMETHOD(Tile)(THIS_ HDC,struct tagPOINT*,struct tagRECT*,struct tagSIZE*) PURE;
    STDMETHOD(StretchBlt)(THIS_ HDC,int,int,int,int,int,int,int,int,DWORD) PURE;
};

#ifdef COBJMACROS
#define IImgCtx_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IImgCtx_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IImgCtx_Release(This)  (This)->lpVtbl->Release(This)
#define IImgCtx_Load(This,pszUrl,dwFlags)  (This)->lpVtbl->Load(This,pszUrl,dwFlags)
#define IImgCtx_Draw(This,hdc,prcBounds)  (This)->lpVtbl->Draw(This,hdc,prcBounds)
#define IImgCtx_Tile(This,hdc,pptBackOrg,prcClip,psize)  (This)->lpVtbl->Tile(This,hdc,pptBackOrg,prcClip,psize)
#define IImgCtx_GetUpdateRects(This,prc,prcImg,pcrc)  (This)->lpVtbl->GetUpdateRects(This,prc,prcImg,pcrc)
#define IImgCtx_GetStateInfo(This,pulState,psize,fClearChanges)  (This)->lpVtbl->GetStateInfo(This,pulState,psize,fClearChanges)
#define IImgCtx_GetPalette(This,phpal)  (This)->lpVtbl->GetPalette(This,phpal)
#define IImgCtx_SelectChanges(This,ulChgOn,ulChgOff,fSignal)  (This)->lpVtbl->SelectChanges(This,ulChgOn,ulChgOff,fSignal)
#define IImgCtx_SetCallback(This,pfnCallback,pvPrivateData)  (This)->lpVtbl->SetCallback(This,pfnCallback,pvPrivateData)
#define IImgCtx_Disconnect(This)  (This)->lpVtbl->Disconnect(This)
#define IImgCtx_StretchBlt(This,hdc,dstX,dstY,dstXE,dstYE,srcX,srcY,srcXE,srcYE,dwROP)  (This)->lpVtbl->StretchBlt(This,hdc,dstX,dstY,dstXE,dstYE,srcX,srcY,srcXE,srcYE,dwROP)
#endif /* COBJMACROS */

#endif /* _IIMGCTX_H */
