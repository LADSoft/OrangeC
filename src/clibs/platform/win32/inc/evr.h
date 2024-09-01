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
#endif 

#ifndef _EVR_H
#define _EVR_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IMFVideoPositionMapper_FWD_DEFINED__
#define __IMFVideoPositionMapper_FWD_DEFINED__
typedef interface IMFVideoPositionMapper IMFVideoPositionMapper;
#endif

#ifndef __IMFVideoDeviceID_FWD_DEFINED__
#define __IMFVideoDeviceID_FWD_DEFINED__
typedef interface IMFVideoDeviceID IMFVideoDeviceID;
#endif

#ifndef __IMFVideoDisplayControl_FWD_DEFINED__
#define __IMFVideoDisplayControl_FWD_DEFINED__
typedef interface IMFVideoDisplayControl IMFVideoDisplayControl;
#endif

#ifndef __IMFVideoPresenter_FWD_DEFINED__
#define __IMFVideoPresenter_FWD_DEFINED__
typedef interface IMFVideoPresenter IMFVideoPresenter;
#endif

#ifndef __IMFDesiredSample_FWD_DEFINED__
#define __IMFDesiredSample_FWD_DEFINED__
typedef interface IMFDesiredSample IMFDesiredSample;
#endif

#ifndef __IMFTrackedSample_FWD_DEFINED__
#define __IMFTrackedSample_FWD_DEFINED__
typedef interface IMFTrackedSample IMFTrackedSample;
#endif

#ifndef __IMFVideoMixerControl_FWD_DEFINED__
#define __IMFVideoMixerControl_FWD_DEFINED__
typedef interface IMFVideoMixerControl IMFVideoMixerControl;
#endif

#ifndef __IMFVideoMixerControl2_FWD_DEFINED__
#define __IMFVideoMixerControl2_FWD_DEFINED__
typedef interface IMFVideoMixerControl2 IMFVideoMixerControl2;
#endif

#ifndef __IMFVideoRenderer_FWD_DEFINED__
#define __IMFVideoRenderer_FWD_DEFINED__
typedef interface IMFVideoRenderer IMFVideoRenderer;
#endif

#ifndef __IEVRFilterConfig_FWD_DEFINED__
#define __IEVRFilterConfig_FWD_DEFINED__
typedef interface IEVRFilterConfig IEVRFilterConfig;
#endif

#ifndef __IEVRFilterConfigEx_FWD_DEFINED__
#define __IEVRFilterConfigEx_FWD_DEFINED__
typedef interface IEVRFilterConfigEx IEVRFilterConfigEx;
#endif

#ifndef __IMFTopologyServiceLookup_FWD_DEFINED__
#define __IMFTopologyServiceLookup_FWD_DEFINED__
typedef interface IMFTopologyServiceLookup IMFTopologyServiceLookup;
#endif

#ifndef __IMFTopologyServiceLookupClient_FWD_DEFINED__
#define __IMFTopologyServiceLookupClient_FWD_DEFINED__
typedef interface IMFTopologyServiceLookupClient IMFTopologyServiceLookupClient;
#endif

#ifndef __IEVRTrustedVideoPlugin_FWD_DEFINED__
#define __IEVRTrustedVideoPlugin_FWD_DEFINED__
typedef interface IEVRTrustedVideoPlugin IEVRTrustedVideoPlugin;
#endif

#include "unknwn.h"
#include "propidl.h"
#include "mfidl.h"
/* #include "strmif.h" */

#ifdef __cplusplus
extern "C"{
#endif 

DEFINE_GUID(MR_VIDEO_RENDER_SERVICE, 0x1092a86c, 0xab1a, 0x459a, 0xa3, 0x36, 0x83, 0x1f, 0xbc, 0x4d, 0x11, 0xff);
DEFINE_GUID(MR_VIDEO_MIXER_SERVICE, 0x73cd2fc, 0x6cf4, 0x40b7, 0x88, 0x59, 0xe8, 0x95, 0x52, 0xc8, 0x41, 0xf8);
DEFINE_GUID(MR_VIDEO_ACCELERATION_SERVICE, 0xefef5175, 0x5c7d, 0x4ce2, 0xbb, 0xbd, 0x34, 0xff, 0x8b, 0xca, 0x65, 0x54);
DEFINE_GUID(MR_BUFFER_SERVICE, 0xa562248c, 0x9ac6, 0x4ffc, 0x9f, 0xba, 0x3a, 0xf8, 0xf8, 0xad, 0x1a, 0x4d);
DEFINE_GUID(VIDEO_ZOOM_RECT, 0x7aaa1638, 0x1b7f, 0x4c93, 0xbd, 0x89, 0x5b, 0x9c, 0x9f, 0xb6, 0xfc, 0xf0);

extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0000_v0_0_s_ifspec;

#ifndef __IMFVideoPositionMapper_INTERFACE_DEFINED__
#define __IMFVideoPositionMapper_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFVideoPositionMapper;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("1F6A9F17-E70B-4e24-8AE4-0B2C3BA7A4AE")IMFVideoPositionMapper:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE MapOutputCoordinateToInputStream(float xOut, float yOut, DWORD dwOutputStreamIndex, DWORD dwInputStreamIndex, float *pxIn, float *pyIn) = 0;
};
#else
typedef struct IMFVideoPositionMapperVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IMFVideoPositionMapper * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFVideoPositionMapper * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFVideoPositionMapper * This);
    HRESULT(STDMETHODCALLTYPE *MapOutputCoordinateToInputStream)(IMFVideoPositionMapper * This, float xOut, float yOut, DWORD dwOutputStreamIndex, DWORD dwInputStreamIndex, float *pxIn, float *pyIn);
    END_INTERFACE
} IMFVideoPositionMapperVtbl;

interface IMFVideoPositionMapper {
    CONST_VTBL struct IMFVideoPositionMapperVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFVideoPositionMapper_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IMFVideoPositionMapper_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IMFVideoPositionMapper_Release(This)  ((This)->lpVtbl -> Release(This))
#define IMFVideoPositionMapper_MapOutputCoordinateToInputStream(This,xOut,yOut,dwOutputStreamIndex,dwInputStreamIndex,pxIn,pyIn)  ((This)->lpVtbl -> MapOutputCoordinateToInputStream(This,xOut,yOut,dwOutputStreamIndex,dwInputStreamIndex,pxIn,pyIn))
#endif /* COBJMACROS */

#endif

#endif

#ifndef __IMFVideoDeviceID_INTERFACE_DEFINED__
#define __IMFVideoDeviceID_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFVideoDeviceID;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A38D9567-5A9C-4f3c-B293-8EB415B279BA")IMFVideoDeviceID:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetDeviceID(IID * pDeviceID) = 0;
};
#else
typedef struct IMFVideoDeviceIDVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFVideoDeviceID * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFVideoDeviceID * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFVideoDeviceID * This);
    HRESULT(STDMETHODCALLTYPE *GetDeviceID)(IMFVideoDeviceID * This, IID * pDeviceID);
    END_INTERFACE
} IMFVideoDeviceIDVtbl;

interface IMFVideoDeviceID {
    CONST_VTBL struct IMFVideoDeviceIDVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFVideoDeviceID_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IMFVideoDeviceID_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IMFVideoDeviceID_Release(This)  ((This)->lpVtbl -> Release(This))
#define IMFVideoDeviceID_GetDeviceID(This,pDeviceID)  ((This)->lpVtbl -> GetDeviceID(This,pDeviceID))
#endif /* COBJMACROS */

#endif

#endif

typedef enum MFVideoAspectRatioMode {
    MFVideoARMode_None = 0,
    MFVideoARMode_PreservePicture = 0x1,
    MFVideoARMode_PreservePixel = 0x2,
    MFVideoARMode_NonLinearStretch = 0x4,
    MFVideoARMode_Mask = 0x7
} MFVideoAspectRatioMode;

typedef enum MFVideoRenderPrefs {
    MFVideoRenderPrefs_DoNotRenderBorder = 0x1,
    MFVideoRenderPrefs_DoNotClipToDevice = 0x2,
    MFVideoRenderPrefs_AllowOutputThrottling = 0x4,
    MFVideoRenderPrefs_ForceOutputThrottling = 0x8,
    MFVideoRenderPrefs_ForceBatching = 0x10,
    MFVideoRenderPrefs_AllowBatching = 0x20,
    MFVideoRenderPrefs_ForceScaling = 0x40,
    MFVideoRenderPrefs_AllowScaling = 0x80,
    MFVideoRenderPrefs_DoNotRepaintOnStop = 0x100,
    MFVideoRenderPrefs_Mask = 0x1ff
} MFVideoRenderPrefs;

typedef struct MFVideoNormalizedRect {
    float left;
    float top;
    float right;
    float bottom;
} MFVideoNormalizedRect;

extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0002_v0_0_s_ifspec;

#ifndef __IMFVideoDisplayControl_INTERFACE_DEFINED__
#define __IMFVideoDisplayControl_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFVideoDisplayControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a490b1e4-ab84-4d31-a1b2-181e03b1077a")IMFVideoDisplayControl:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetNativeVideoSize(SIZE * pszVideo, SIZE * pszARVideo) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIdealVideoSize(SIZE *pszMin, SIZE *pszMax) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVideoPosition(const MFVideoNormalizedRect *pnrcSource, const LPRECT prcDest) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVideoPosition(MFVideoNormalizedRect *pnrcSource, LPRECT prcDest) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetAspectRatioMode(DWORD dwAspectRatioMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAspectRatioMode(DWORD *pdwAspectRatioMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVideoWindow(HWND hwndVideo) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVideoWindow(HWND *phwndVideo) = 0;
    virtual HRESULT STDMETHODCALLTYPE RepaintVideo(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentImage(BITMAPINFOHEADER *pBih, BYTE **pDib, DWORD *pcbDib, LONGLONG *pTimeStamp) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetBorderColor(COLORREF Clr) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBorderColor(COLORREF *pClr) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetRenderingPrefs(DWORD dwRenderFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRenderingPrefs(DWORD *pdwRenderFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetFullscreen(BOOL fFullscreen) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFullscreen(BOOL *pfFullscreen) = 0;
};
#else
typedef struct IMFVideoDisplayControlVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFVideoDisplayControl * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFVideoDisplayControl * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFVideoDisplayControl * This);
    HRESULT(STDMETHODCALLTYPE *GetNativeVideoSize)(IMFVideoDisplayControl * This, SIZE * pszVideo, SIZE * pszARVideo);
    HRESULT(STDMETHODCALLTYPE *GetIdealVideoSize)(IMFVideoDisplayControl * This, SIZE * pszMin, SIZE * pszMax);
    HRESULT(STDMETHODCALLTYPE *SetVideoPosition)(IMFVideoDisplayControl * This, const MFVideoNormalizedRect * pnrcSource, const LPRECT prcDest);
    HRESULT(STDMETHODCALLTYPE *GetVideoPosition)(IMFVideoDisplayControl * This, MFVideoNormalizedRect * pnrcSource, LPRECT prcDest);
    HRESULT(STDMETHODCALLTYPE *SetAspectRatioMode)(IMFVideoDisplayControl * This, DWORD dwAspectRatioMode);
    HRESULT(STDMETHODCALLTYPE *GetAspectRatioMode)(IMFVideoDisplayControl * This, DWORD * pdwAspectRatioMode);
    HRESULT(STDMETHODCALLTYPE *SetVideoWindow)(IMFVideoDisplayControl * This, HWND hwndVideo);
    HRESULT(STDMETHODCALLTYPE *GetVideoWindow)(IMFVideoDisplayControl * This, HWND * phwndVideo);
    HRESULT(STDMETHODCALLTYPE *RepaintVideo)(IMFVideoDisplayControl * This);
    HRESULT(STDMETHODCALLTYPE *GetCurrentImage)(IMFVideoDisplayControl * This, BITMAPINFOHEADER * pBih, BYTE ** pDib, DWORD * pcbDib, LONGLONG * pTimeStamp);
    HRESULT(STDMETHODCALLTYPE *SetBorderColor)(IMFVideoDisplayControl * This, COLORREF Clr);
    HRESULT(STDMETHODCALLTYPE *GetBorderColor)(IMFVideoDisplayControl * This, COLORREF * pClr);
    HRESULT(STDMETHODCALLTYPE *SetRenderingPrefs)(IMFVideoDisplayControl * This, DWORD dwRenderFlags);
    HRESULT(STDMETHODCALLTYPE *GetRenderingPrefs)(IMFVideoDisplayControl * This, DWORD * pdwRenderFlags);
    HRESULT(STDMETHODCALLTYPE *SetFullscreen)(IMFVideoDisplayControl * This, BOOL fFullscreen);
    HRESULT(STDMETHODCALLTYPE *GetFullscreen)(IMFVideoDisplayControl * This, BOOL * pfFullscreen);
    END_INTERFACE
} IMFVideoDisplayControlVtbl;

interface IMFVideoDisplayControl {
    CONST_VTBL struct IMFVideoDisplayControlVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFVideoDisplayControl_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IMFVideoDisplayControl_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IMFVideoDisplayControl_Release(This)  ((This)->lpVtbl -> Release(This))
#define IMFVideoDisplayControl_GetNativeVideoSize(This,pszVideo,pszARVideo)  ((This)->lpVtbl -> GetNativeVideoSize(This,pszVideo,pszARVideo))
#define IMFVideoDisplayControl_GetIdealVideoSize(This,pszMin,pszMax)  ((This)->lpVtbl -> GetIdealVideoSize(This,pszMin,pszMax))
#define IMFVideoDisplayControl_SetVideoPosition(This,pnrcSource,prcDest)  ((This)->lpVtbl -> SetVideoPosition(This,pnrcSource,prcDest))
#define IMFVideoDisplayControl_GetVideoPosition(This,pnrcSource,prcDest)  ((This)->lpVtbl -> GetVideoPosition(This,pnrcSource,prcDest))
#define IMFVideoDisplayControl_SetAspectRatioMode(This,dwAspectRatioMode)  ((This)->lpVtbl -> SetAspectRatioMode(This,dwAspectRatioMode))
#define IMFVideoDisplayControl_GetAspectRatioMode(This,pdwAspectRatioMode)  ((This)->lpVtbl -> GetAspectRatioMode(This,pdwAspectRatioMode))
#define IMFVideoDisplayControl_SetVideoWindow(This,hwndVideo)  ((This)->lpVtbl -> SetVideoWindow(This,hwndVideo))
#define IMFVideoDisplayControl_GetVideoWindow(This,phwndVideo)  ((This)->lpVtbl -> GetVideoWindow(This,phwndVideo))
#define IMFVideoDisplayControl_RepaintVideo(This)  ((This)->lpVtbl -> RepaintVideo(This))
#define IMFVideoDisplayControl_GetCurrentImage(This,pBih,pDib,pcbDib,pTimeStamp)  ((This)->lpVtbl -> GetCurrentImage(This,pBih,pDib,pcbDib,pTimeStamp))
#define IMFVideoDisplayControl_SetBorderColor(This,Clr)  ((This)->lpVtbl -> SetBorderColor(This,Clr))
#define IMFVideoDisplayControl_GetBorderColor(This,pClr)  ((This)->lpVtbl -> GetBorderColor(This,pClr))
#define IMFVideoDisplayControl_SetRenderingPrefs(This,dwRenderFlags)  ((This)->lpVtbl -> SetRenderingPrefs(This,dwRenderFlags))
#define IMFVideoDisplayControl_GetRenderingPrefs(This,pdwRenderFlags)  ((This)->lpVtbl -> GetRenderingPrefs(This,pdwRenderFlags))
#define IMFVideoDisplayControl_SetFullscreen(This,fFullscreen)  ((This)->lpVtbl -> SetFullscreen(This,fFullscreen))
#define IMFVideoDisplayControl_GetFullscreen(This,pfFullscreen)  ((This)->lpVtbl -> GetFullscreen(This,pfFullscreen))
#endif /* COBJMACROS */

#endif

#endif

typedef enum MFVP_MESSAGE_TYPE {
    MFVP_MESSAGE_FLUSH = 0,
    MFVP_MESSAGE_INVALIDATEMEDIATYPE = 0x1,
    MFVP_MESSAGE_PROCESSINPUTNOTIFY = 0x2,
    MFVP_MESSAGE_BEGINSTREAMING = 0x3,
    MFVP_MESSAGE_ENDSTREAMING = 0x4,
    MFVP_MESSAGE_ENDOFSTREAM = 0x5,
    MFVP_MESSAGE_STEP = 0x6,
    MFVP_MESSAGE_CANCELSTEP = 0x7
} MFVP_MESSAGE_TYPE;

extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0003_v0_0_s_ifspec;

#ifndef __IMFVideoPresenter_INTERFACE_DEFINED__
#define __IMFVideoPresenter_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFVideoPresenter;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("29AFF080-182A-4a5d-AF3B-448F3A6346CB")IMFVideoPresenter:public IMFClockStateSink
{
    public:
    virtual HRESULT STDMETHODCALLTYPE ProcessMessage(MFVP_MESSAGE_TYPE eMessage, ULONG_PTR ulParam) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentMediaType(IMFVideoMediaType **ppMediaType) = 0;
};
#else
typedef struct IMFVideoPresenterVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFVideoPresenter * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFVideoPresenter * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFVideoPresenter * This);
    HRESULT(STDMETHODCALLTYPE *OnClockStart)(IMFVideoPresenter * This, MFTIME hnsSystemTime, LONGLONG llClockStartOffset);
    HRESULT(STDMETHODCALLTYPE *OnClockStop)(IMFVideoPresenter * This, MFTIME hnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *OnClockPause)(IMFVideoPresenter * This, MFTIME hnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *OnClockRestart)(IMFVideoPresenter * This, MFTIME hnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *OnClockSetRate)(IMFVideoPresenter * This, MFTIME hnsSystemTime, float flRate);
    HRESULT(STDMETHODCALLTYPE *ProcessMessage)(IMFVideoPresenter * This, MFVP_MESSAGE_TYPE eMessage, ULONG_PTR ulParam);
    HRESULT(STDMETHODCALLTYPE *GetCurrentMediaType)(IMFVideoPresenter * This, IMFVideoMediaType ** ppMediaType);
    END_INTERFACE
} IMFVideoPresenterVtbl;

interface IMFVideoPresenter {
    CONST_VTBL struct IMFVideoPresenterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFVideoPresenter_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IMFVideoPresenter_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IMFVideoPresenter_Release(This)  ((This)->lpVtbl -> Release(This))
#define IMFVideoPresenter_OnClockStart(This,hnsSystemTime,llClockStartOffset)  ((This)->lpVtbl -> OnClockStart(This,hnsSystemTime,llClockStartOffset))
#define IMFVideoPresenter_OnClockStop(This,hnsSystemTime)  ((This)->lpVtbl -> OnClockStop(This,hnsSystemTime))
#define IMFVideoPresenter_OnClockPause(This,hnsSystemTime)  ((This)->lpVtbl -> OnClockPause(This,hnsSystemTime))
#define IMFVideoPresenter_OnClockRestart(This,hnsSystemTime)  ((This)->lpVtbl -> OnClockRestart(This,hnsSystemTime))
#define IMFVideoPresenter_OnClockSetRate(This,hnsSystemTime,flRate)  ((This)->lpVtbl -> OnClockSetRate(This,hnsSystemTime,flRate))
#define IMFVideoPresenter_ProcessMessage(This,eMessage,ulParam)  ((This)->lpVtbl -> ProcessMessage(This,eMessage,ulParam))
#define IMFVideoPresenter_GetCurrentMediaType(This,ppMediaType)  ((This)->lpVtbl -> GetCurrentMediaType(This,ppMediaType))
#endif /* COBJMACROS */

#endif

#endif

#ifndef __IMFDesiredSample_INTERFACE_DEFINED__
#define __IMFDesiredSample_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFDesiredSample;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("56C294D0-753E-4260-8D61-A3D8820B1D54")IMFDesiredSample:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetDesiredSampleTimeAndDuration(LONGLONG * phnsSampleTime, LONGLONG * phnsSampleDuration) = 0;
    virtual void STDMETHODCALLTYPE SetDesiredSampleTimeAndDuration(LONGLONG hnsSampleTime, LONGLONG hnsSampleDuration) = 0;
    virtual void STDMETHODCALLTYPE Clear(void) = 0;
};
#else
typedef struct IMFDesiredSampleVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFDesiredSample * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFDesiredSample * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFDesiredSample * This);
    HRESULT(STDMETHODCALLTYPE *GetDesiredSampleTimeAndDuration)(IMFDesiredSample * This, LONGLONG * phnsSampleTime, LONGLONG * phnsSampleDuration);
    void (STDMETHODCALLTYPE *SetDesiredSampleTimeAndDuration)(IMFDesiredSample * This, LONGLONG hnsSampleTime, LONGLONG hnsSampleDuration);
    void (STDMETHODCALLTYPE *Clear)(IMFDesiredSample * This);
    END_INTERFACE
} IMFDesiredSampleVtbl;

interface IMFDesiredSample {
    CONST_VTBL struct IMFDesiredSampleVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFDesiredSample_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IMFDesiredSample_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IMFDesiredSample_Release(This)  ((This)->lpVtbl -> Release(This))
#define IMFDesiredSample_GetDesiredSampleTimeAndDuration(This,phnsSampleTime,phnsSampleDuration)  ((This)->lpVtbl -> GetDesiredSampleTimeAndDuration(This,phnsSampleTime,phnsSampleDuration))
#define IMFDesiredSample_SetDesiredSampleTimeAndDuration(This,hnsSampleTime,hnsSampleDuration)  ((This)->lpVtbl -> SetDesiredSampleTimeAndDuration(This,hnsSampleTime,hnsSampleDuration))
#define IMFDesiredSample_Clear(This)  ((This)->lpVtbl -> Clear(This))
#endif /* COBJMACROS */

#endif

#endif

#ifndef __IMFTrackedSample_INTERFACE_DEFINED__
#define __IMFTrackedSample_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTrackedSample;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("245BF8E9-0755-40f7-88A5-AE0F18D55E17")IMFTrackedSample:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetAllocator(IMFAsyncCallback * pSampleAllocator, IUnknown * pUnkState) = 0;
};
#else
typedef struct IMFTrackedSampleVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTrackedSample * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFTrackedSample * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFTrackedSample * This);
    HRESULT(STDMETHODCALLTYPE *SetAllocator)(IMFTrackedSample * This, IMFAsyncCallback * pSampleAllocator, IUnknown * pUnkState);
    END_INTERFACE
} IMFTrackedSampleVtbl;

interface IMFTrackedSample {
    CONST_VTBL struct IMFTrackedSampleVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFTrackedSample_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IMFTrackedSample_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IMFTrackedSample_Release(This)  ((This)->lpVtbl -> Release(This))
#define IMFTrackedSample_SetAllocator(This,pSampleAllocator,pUnkState)  ((This)->lpVtbl -> SetAllocator(This,pSampleAllocator,pUnkState))
#endif /* COBJMACROS */

#endif

#endif

#ifndef __IMFVideoMixerControl_INTERFACE_DEFINED__
#define __IMFVideoMixerControl_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFVideoMixerControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A5C6C53F-C202-4aa5-9695-175BA8C508A5")IMFVideoMixerControl:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetStreamZOrder(DWORD dwStreamID, DWORD dwZ) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamZOrder(DWORD dwStreamID, DWORD *pdwZ) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetStreamOutputRect(DWORD dwStreamID, const MFVideoNormalizedRect *pnrcOutput) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamOutputRect(DWORD dwStreamID, MFVideoNormalizedRect *pnrcOutput) = 0;
};
#else
typedef struct IMFVideoMixerControlVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFVideoMixerControl * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFVideoMixerControl * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFVideoMixerControl * This);
    HRESULT(STDMETHODCALLTYPE *SetStreamZOrder)(IMFVideoMixerControl * This, DWORD dwStreamID, DWORD dwZ);
    HRESULT(STDMETHODCALLTYPE *GetStreamZOrder)(IMFVideoMixerControl * This, DWORD dwStreamID, DWORD * pdwZ);
    HRESULT(STDMETHODCALLTYPE *SetStreamOutputRect)(IMFVideoMixerControl * This, DWORD dwStreamID, const MFVideoNormalizedRect * pnrcOutput);
    HRESULT(STDMETHODCALLTYPE *GetStreamOutputRect)(IMFVideoMixerControl * This, DWORD dwStreamID, MFVideoNormalizedRect * pnrcOutput);
    END_INTERFACE
} IMFVideoMixerControlVtbl;

interface IMFVideoMixerControl {
    CONST_VTBL struct IMFVideoMixerControlVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFVideoMixerControl_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IMFVideoMixerControl_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IMFVideoMixerControl_Release(This)  ((This)->lpVtbl -> Release(This))
#define IMFVideoMixerControl_SetStreamZOrder(This,dwStreamID,dwZ)  ((This)->lpVtbl -> SetStreamZOrder(This,dwStreamID,dwZ))
#define IMFVideoMixerControl_GetStreamZOrder(This,dwStreamID,pdwZ)  ((This)->lpVtbl -> GetStreamZOrder(This,dwStreamID,pdwZ))
#define IMFVideoMixerControl_SetStreamOutputRect(This,dwStreamID,pnrcOutput)  ((This)->lpVtbl -> SetStreamOutputRect(This,dwStreamID,pnrcOutput))
#define IMFVideoMixerControl_GetStreamOutputRect(This,dwStreamID,pnrcOutput)  ((This)->lpVtbl -> GetStreamOutputRect(This,dwStreamID,pnrcOutput))
#endif /* COBJMACROS */

#endif

#endif

typedef enum _MFVideoMixPrefs {
    MFVideoMixPrefs_ForceHalfInterlace = 0x1,
    MFVideoMixPrefs_AllowDropToHalfInterlace = 0x2,
    MFVideoMixPrefs_AllowDropToBob = 0x4,
    MFVideoMixPrefs_ForceBob = 0x8,
    MFVideoMixPrefs_Mask = 0xf
} MFVideoMixPrefs;

extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0007_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0007_v0_0_s_ifspec;

#ifndef __IMFVideoMixerControl2_INTERFACE_DEFINED__
#define __IMFVideoMixerControl2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFVideoMixerControl2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("8459616d-966e-4930-b658-54fa7e5a16d3")IMFVideoMixerControl2:public IMFVideoMixerControl
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetMixingPrefs(DWORD dwMixFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMixingPrefs(DWORD *pdwMixFlags) = 0;
};
#else
typedef struct IMFVideoMixerControl2Vtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFVideoMixerControl2 * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFVideoMixerControl2 * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFVideoMixerControl2 * This);
    HRESULT(STDMETHODCALLTYPE *SetStreamZOrder)(IMFVideoMixerControl2 * This, DWORD dwStreamID, DWORD dwZ);
    HRESULT(STDMETHODCALLTYPE *GetStreamZOrder)(IMFVideoMixerControl2 * This, DWORD dwStreamID, DWORD * pdwZ);
    HRESULT(STDMETHODCALLTYPE *SetStreamOutputRect)(IMFVideoMixerControl2 * This, DWORD dwStreamID, const MFVideoNormalizedRect * pnrcOutput);
    HRESULT(STDMETHODCALLTYPE *GetStreamOutputRect)(IMFVideoMixerControl2 * This, DWORD dwStreamID, MFVideoNormalizedRect * pnrcOutput);
    HRESULT(STDMETHODCALLTYPE *SetMixingPrefs)(IMFVideoMixerControl2 * This, DWORD dwMixFlags);
    HRESULT(STDMETHODCALLTYPE *GetMixingPrefs)(IMFVideoMixerControl2 * This, DWORD * pdwMixFlags);
    END_INTERFACE
} IMFVideoMixerControl2Vtbl;

interface IMFVideoMixerControl2 {
    CONST_VTBL struct IMFVideoMixerControl2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFVideoMixerControl2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IMFVideoMixerControl2_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IMFVideoMixerControl2_Release(This)  ((This)->lpVtbl -> Release(This))
#define IMFVideoMixerControl2_SetStreamZOrder(This,dwStreamID,dwZ)  ((This)->lpVtbl -> SetStreamZOrder(This,dwStreamID,dwZ))
#define IMFVideoMixerControl2_GetStreamZOrder(This,dwStreamID,pdwZ)  ((This)->lpVtbl -> GetStreamZOrder(This,dwStreamID,pdwZ))
#define IMFVideoMixerControl2_SetStreamOutputRect(This,dwStreamID,pnrcOutput)  ((This)->lpVtbl -> SetStreamOutputRect(This,dwStreamID,pnrcOutput))
#define IMFVideoMixerControl2_GetStreamOutputRect(This,dwStreamID,pnrcOutput)  ((This)->lpVtbl -> GetStreamOutputRect(This,dwStreamID,pnrcOutput))
#define IMFVideoMixerControl2_SetMixingPrefs(This,dwMixFlags)  ((This)->lpVtbl -> SetMixingPrefs(This,dwMixFlags))
#define IMFVideoMixerControl2_GetMixingPrefs(This,pdwMixFlags)  ((This)->lpVtbl -> GetMixingPrefs(This,pdwMixFlags))
#endif /* COBJMACROS */

#endif

#endif

#ifndef __IMFVideoRenderer_INTERFACE_DEFINED__
#define __IMFVideoRenderer_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFVideoRenderer;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("DFDFD197-A9CA-43d8-B341-6AF3503792CD")IMFVideoRenderer:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE InitializeRenderer(IMFTransform * pVideoMixer, IMFVideoPresenter * pVideoPresenter) = 0;
};
#else
typedef struct IMFVideoRendererVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFVideoRenderer * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFVideoRenderer * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFVideoRenderer * This);
    HRESULT(STDMETHODCALLTYPE *InitializeRenderer)(IMFVideoRenderer * This, IMFTransform * pVideoMixer, IMFVideoPresenter * pVideoPresenter);
    END_INTERFACE
} IMFVideoRendererVtbl;

interface IMFVideoRenderer {
    CONST_VTBL struct IMFVideoRendererVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFVideoRenderer_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IMFVideoRenderer_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IMFVideoRenderer_Release(This)  ((This)->lpVtbl -> Release(This))
#define IMFVideoRenderer_InitializeRenderer(This,pVideoMixer,pVideoPresenter)  ((This)->lpVtbl -> InitializeRenderer(This,pVideoMixer,pVideoPresenter))
#endif /* COBJMACROS */

#endif

#endif

#ifndef __IEVRFilterConfig_INTERFACE_DEFINED__
#define __IEVRFilterConfig_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEVRFilterConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("83E91E85-82C1-4ea7-801D-85DC50B75086")IEVRFilterConfig:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetNumberOfStreams(DWORD dwMaxStreams) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNumberOfStreams(DWORD *pdwMaxStreams) = 0;
};
#else
typedef struct IEVRFilterConfigVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IEVRFilterConfig * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IEVRFilterConfig * This);
    ULONG(STDMETHODCALLTYPE *Release)(IEVRFilterConfig * This);
    HRESULT(STDMETHODCALLTYPE *SetNumberOfStreams)(IEVRFilterConfig * This, DWORD dwMaxStreams);
    HRESULT(STDMETHODCALLTYPE *GetNumberOfStreams)(IEVRFilterConfig * This, DWORD * pdwMaxStreams);
    END_INTERFACE
} IEVRFilterConfigVtbl;

interface IEVRFilterConfig {
    CONST_VTBL struct IEVRFilterConfigVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEVRFilterConfig_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IEVRFilterConfig_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IEVRFilterConfig_Release(This)  ((This)->lpVtbl -> Release(This))
#define IEVRFilterConfig_SetNumberOfStreams(This,dwMaxStreams)  ((This)->lpVtbl -> SetNumberOfStreams(This,dwMaxStreams))
#define IEVRFilterConfig_GetNumberOfStreams(This,pdwMaxStreams)  ((This)->lpVtbl -> GetNumberOfStreams(This,pdwMaxStreams))
#endif /* COBJMACROS */

#endif

#endif

typedef enum _EVRFilterConfig_Prefs {
    EVRFilterConfigPrefs_EnableQoS = 0x1,
    EVRFilterConfigPrefs_Mask = 0x1
} EVRFilterConfigPrefs;

extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0010_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0010_v0_0_s_ifspec;

#ifndef __IEVRFilterConfigEx_INTERFACE_DEFINED__
#define __IEVRFilterConfigEx_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEVRFilterConfigEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("aea36028-796d-454f-beee-b48071e24304")IEVRFilterConfigEx:public IEVRFilterConfig
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetConfigPrefs(DWORD dwConfigFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetConfigPrefs(DWORD *pdwConfigFlags) = 0;
};
#else
typedef struct IEVRFilterConfigExVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IEVRFilterConfigEx * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IEVRFilterConfigEx * This);
    ULONG(STDMETHODCALLTYPE *Release)(IEVRFilterConfigEx * This);
    HRESULT(STDMETHODCALLTYPE *SetNumberOfStreams)(IEVRFilterConfigEx * This, DWORD dwMaxStreams);
    HRESULT(STDMETHODCALLTYPE *GetNumberOfStreams)(IEVRFilterConfigEx * This, DWORD * pdwMaxStreams);
    HRESULT(STDMETHODCALLTYPE *SetConfigPrefs)(IEVRFilterConfigEx * This, DWORD dwConfigFlags);
    HRESULT(STDMETHODCALLTYPE *GetConfigPrefs)(IEVRFilterConfigEx * This, DWORD * pdwConfigFlags);
    END_INTERFACE
} IEVRFilterConfigExVtbl;

interface IEVRFilterConfigEx {
    CONST_VTBL struct IEVRFilterConfigExVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEVRFilterConfigEx_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IEVRFilterConfigEx_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IEVRFilterConfigEx_Release(This)  ((This)->lpVtbl -> Release(This))
#define IEVRFilterConfigEx_SetNumberOfStreams(This,dwMaxStreams)  ((This)->lpVtbl -> SetNumberOfStreams(This,dwMaxStreams))
#define IEVRFilterConfigEx_GetNumberOfStreams(This,pdwMaxStreams)  ((This)->lpVtbl -> GetNumberOfStreams(This,pdwMaxStreams))
#define IEVRFilterConfigEx_SetConfigPrefs(This,dwConfigFlags)  ((This)->lpVtbl -> SetConfigPrefs(This,dwConfigFlags))
#define IEVRFilterConfigEx_GetConfigPrefs(This,pdwConfigFlags)  ((This)->lpVtbl -> GetConfigPrefs(This,pdwConfigFlags))
#endif /* COBJMACROS */

#endif

#endif

typedef enum _MF_SERVICE_LOOKUP_TYPE {
    MF_SERVICE_LOOKUP_UPSTREAM = 0,
    MF_SERVICE_LOOKUP_UPSTREAM_DIRECT = (MF_SERVICE_LOOKUP_UPSTREAM + 1),
    MF_SERVICE_LOOKUP_DOWNSTREAM = (MF_SERVICE_LOOKUP_UPSTREAM_DIRECT + 1),
    MF_SERVICE_LOOKUP_DOWNSTREAM_DIRECT = (MF_SERVICE_LOOKUP_DOWNSTREAM + 1),
    MF_SERVICE_LOOKUP_ALL = (MF_SERVICE_LOOKUP_DOWNSTREAM_DIRECT + 1),
    MF_SERVICE_LOOKUP_GLOBAL = (MF_SERVICE_LOOKUP_ALL + 1)
} MF_SERVICE_LOOKUP_TYPE;

extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0011_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0011_v0_0_s_ifspec;

#ifndef __IMFTopologyServiceLookup_INTERFACE_DEFINED__
#define __IMFTopologyServiceLookup_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTopologyServiceLookup;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("fa993889-4383-415a-a930-dd472a8cf6f7")IMFTopologyServiceLookup:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE LookupService(MF_SERVICE_LOOKUP_TYPE Type, DWORD dwIndex, REFGUID guidService, REFIID riid, LPVOID * ppvObjects, DWORD * pnObjects) = 0;
};
#else
typedef struct IMFTopologyServiceLookupVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTopologyServiceLookup * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFTopologyServiceLookup * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFTopologyServiceLookup * This);
    HRESULT(STDMETHODCALLTYPE *LookupService)(IMFTopologyServiceLookup * This, MF_SERVICE_LOOKUP_TYPE Type, DWORD dwIndex, REFGUID guidService, REFIID riid, LPVOID * ppvObjects, DWORD * pnObjects);
    END_INTERFACE
} IMFTopologyServiceLookupVtbl;

interface IMFTopologyServiceLookup {
    CONST_VTBL struct IMFTopologyServiceLookupVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFTopologyServiceLookup_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IMFTopologyServiceLookup_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IMFTopologyServiceLookup_Release(This)  ((This)->lpVtbl -> Release(This))
#define IMFTopologyServiceLookup_LookupService(This,Type,dwIndex,guidService,riid,ppvObjects,pnObjects)  ((This)->lpVtbl -> LookupService(This,Type,dwIndex,guidService,riid,ppvObjects,pnObjects))
#endif /* COBJMACROS */

#endif

#endif

#ifndef __IMFTopologyServiceLookupClient_INTERFACE_DEFINED__
#define __IMFTopologyServiceLookupClient_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTopologyServiceLookupClient;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("fa99388a-4383-415a-a930-dd472a8cf6f7")IMFTopologyServiceLookupClient:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE InitServicePointers(IMFTopologyServiceLookup * pLookup) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReleaseServicePointers(void) = 0;
};
#else
typedef struct IMFTopologyServiceLookupClientVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTopologyServiceLookupClient * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFTopologyServiceLookupClient * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFTopologyServiceLookupClient * This);
    HRESULT(STDMETHODCALLTYPE *InitServicePointers)(IMFTopologyServiceLookupClient * This, IMFTopologyServiceLookup * pLookup);
    HRESULT(STDMETHODCALLTYPE *ReleaseServicePointers)(IMFTopologyServiceLookupClient * This);
    END_INTERFACE
} IMFTopologyServiceLookupClientVtbl;

interface IMFTopologyServiceLookupClient {
    CONST_VTBL struct IMFTopologyServiceLookupClientVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFTopologyServiceLookupClient_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IMFTopologyServiceLookupClient_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IMFTopologyServiceLookupClient_Release(This)  ((This)->lpVtbl -> Release(This))
#define IMFTopologyServiceLookupClient_InitServicePointers(This,pLookup)  ((This)->lpVtbl -> InitServicePointers(This,pLookup))
#define IMFTopologyServiceLookupClient_ReleaseServicePointers(This)  ((This)->lpVtbl -> ReleaseServicePointers(This))
#endif /* COBJMACROS */

#endif

#endif

#ifndef __IEVRTrustedVideoPlugin_INTERFACE_DEFINED__
#define __IEVRTrustedVideoPlugin_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEVRTrustedVideoPlugin;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("83A4CE40-7710-494b-A893-A472049AF630")IEVRTrustedVideoPlugin:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE IsInTrustedVideoMode(BOOL * pYes) = 0;
    virtual HRESULT STDMETHODCALLTYPE CanConstrict(BOOL *pYes) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetConstriction(DWORD dwKPix) = 0;
    virtual HRESULT STDMETHODCALLTYPE DisableImageExport(BOOL bDisable) = 0;
};
#else
typedef struct IEVRTrustedVideoPluginVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IEVRTrustedVideoPlugin * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IEVRTrustedVideoPlugin * This);
    ULONG(STDMETHODCALLTYPE *Release)(IEVRTrustedVideoPlugin * This);
    HRESULT(STDMETHODCALLTYPE *IsInTrustedVideoMode)(IEVRTrustedVideoPlugin * This, BOOL * pYes);
    HRESULT(STDMETHODCALLTYPE *CanConstrict)(IEVRTrustedVideoPlugin * This, BOOL * pYes);
    HRESULT(STDMETHODCALLTYPE *SetConstriction)(IEVRTrustedVideoPlugin * This, DWORD dwKPix);
    HRESULT(STDMETHODCALLTYPE *DisableImageExport)(IEVRTrustedVideoPlugin * This, BOOL bDisable);
    END_INTERFACE
} IEVRTrustedVideoPluginVtbl;

interface IEVRTrustedVideoPlugin {
    CONST_VTBL struct IEVRTrustedVideoPluginVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEVRTrustedVideoPlugin_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl -> QueryInterface(This,riid,ppvObject))
#define IEVRTrustedVideoPlugin_AddRef(This)  ((This)->lpVtbl -> AddRef(This))
#define IEVRTrustedVideoPlugin_Release(This)  ((This)->lpVtbl -> Release(This))
#define IEVRTrustedVideoPlugin_IsInTrustedVideoMode(This,pYes)  ((This)->lpVtbl -> IsInTrustedVideoMode(This,pYes))
#define IEVRTrustedVideoPlugin_CanConstrict(This,pYes)  ((This)->lpVtbl -> CanConstrict(This,pYes))
#define IEVRTrustedVideoPlugin_SetConstriction(This,dwKPix)  ((This)->lpVtbl -> SetConstriction(This,dwKPix))
#define IEVRTrustedVideoPlugin_DisableImageExport(This,bDisable)  ((This)->lpVtbl -> DisableImageExport(This,bDisable))
#endif /* COBJMACROS */

#endif

#endif

#if MFEVRDLL
#define EVRPUBLIC(func) STDAPI _##func
#else
#define EVRPUBLIC(func) STDAPI func
#endif

EVRPUBLIC(MFCreateVideoPresenter)(IUnknown * pOwner, REFIID riidDevice, REFIID riid, void **ppVideoPresenter);
EVRPUBLIC(MFCreateVideoMixer)(IUnknown * pOwner, REFIID riidDevice, REFIID riid, void **ppVideoMixer);
EVRPUBLIC(MFCreateVideoMixerAndPresenter)(IUnknown * pMixerOwner, IUnknown * pPresenterOwner, REFIID riidMixer, void **ppvVideoMixer, REFIID riidPresenter, void **ppvVideoPresenter);
EVRPUBLIC(MFCreateVideoRenderer)(REFIID riidRenderer, void **ppVideoRenderer);
EVRPUBLIC(MFCreateVideoSampleFromSurface)(IUnknown * pUnkSurface, IMFSample ** ppSample);

extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0014_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_evr_0000_0014_v0_0_s_ifspec;

unsigned long __RPC_USER HWND_UserSize(unsigned long *, unsigned long, HWND *);
unsigned char *__RPC_USER HWND_UserMarshal(unsigned long *, unsigned char *, HWND *);
unsigned char *__RPC_USER HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND *);
void __RPC_USER HWND_UserFree(unsigned long *, HWND *);

unsigned long __RPC_USER HWND_UserSize64(unsigned long *, unsigned long, HWND *);
unsigned char *__RPC_USER HWND_UserMarshal64(unsigned long *, unsigned char *, HWND *);
unsigned char *__RPC_USER HWND_UserUnmarshal64(unsigned long *, unsigned char *, HWND *);
void __RPC_USER HWND_UserFree64(unsigned long *, HWND *);

#ifdef __cplusplus
}
#endif

#endif /* _EVR_H */
