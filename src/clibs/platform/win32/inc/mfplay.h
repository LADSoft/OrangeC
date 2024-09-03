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

#ifndef _MFPLAY_H
#define _MFPLAY_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IMFPMediaPlayer_FWD_DEFINED__
#define __IMFPMediaPlayer_FWD_DEFINED__
typedef interface IMFPMediaPlayer IMFPMediaPlayer;
#endif

#ifndef __IMFPMediaItem_FWD_DEFINED__
#define __IMFPMediaItem_FWD_DEFINED__
typedef interface IMFPMediaItem IMFPMediaItem;
#endif

#ifndef __IMFPMediaPlayerCallback_FWD_DEFINED__
#define __IMFPMediaPlayerCallback_FWD_DEFINED__
typedef interface IMFPMediaPlayerCallback IMFPMediaPlayerCallback;
#endif

#include "unknwn.h"
#include "propsys.h"
#include "mfidl.h"
#include "evr.h"

#ifdef __cplusplus
extern "C"{
#endif 

#if (WINVER >= _WIN32_WINNT_WIN7)
interface IMFMediaEvent;
interface IMFNetCredential;
typedef UINT32 MFP_CREATION_OPTIONS;

typedef enum _MFP_CREATION_OPTIONS {
    MFP_OPTION_NONE = 0,
    MFP_OPTION_FREE_THREADED_CALLBACK = 0x1,
    MFP_OPTION_NO_MMCSS = 0x2,
    MFP_OPTION_NO_REMOTE_DESKTOP_OPTIMIZATION = 0x4
} _MFP_CREATION_OPTIONS;

typedef enum MFP_MEDIAPLAYER_STATE {
    MFP_MEDIAPLAYER_STATE_EMPTY = 0,
    MFP_MEDIAPLAYER_STATE_STOPPED = 0x1,
    MFP_MEDIAPLAYER_STATE_PLAYING = 0x2,
    MFP_MEDIAPLAYER_STATE_PAUSED = 0x3,
    MFP_MEDIAPLAYER_STATE_SHUTDOWN = 0x4
} MFP_MEDIAPLAYER_STATE;

typedef UINT32 MFP_MEDIAITEM_CHARACTERISTICS;

typedef enum _MFP_MEDIAITEM_CHARACTERISTICS {
    MFP_MEDIAITEM_IS_LIVE = 0x1,
    MFP_MEDIAITEM_CAN_SEEK = 0x2,
    MFP_MEDIAITEM_CAN_PAUSE = 0x4,
    MFP_MEDIAITEM_HAS_SLOW_SEEK = 0x8
} _MFP_MEDIAITEM_CHARACTERISTICS;

typedef UINT32 MFP_CREDENTIAL_FLAGS;

typedef enum _MFP_CREDENTIAL_FLAGS {
    MFP_CREDENTIAL_PROMPT = 0x1,
    MFP_CREDENTIAL_SAVE = 0x2,
    MFP_CREDENTIAL_DO_NOT_CACHE = 0x4,
    MFP_CREDENTIAL_CLEAR_TEXT = 0x8,
    MFP_CREDENTIAL_PROXY = 0x10,
    MFP_CREDENTIAL_LOGGED_ON_USER = 0x20
} _MFP_CREDENTIAL_FLAGS;

STDAPI MFPCreateMediaPlayer(LPCWSTR, BOOL, MFP_CREATION_OPTIONS, IMFPMediaPlayerCallback *, HWND, IMFPMediaPlayer **);

extern RPC_IF_HANDLE __MIDL_itf_mfplay_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfplay_0000_0000_v0_0_s_ifspec;

#ifndef __IMFPMediaPlayer_INTERFACE_DEFINED__
#define __IMFPMediaPlayer_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFPMediaPlayer;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A714590A-58AF-430a-85BF-44F5EC838D85")IMFPMediaPlayer:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Play(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Pause(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Stop(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE FrameStep(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPosition(REFGUID guidPositionType, const PROPVARIANT *pvPositionValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPosition(REFGUID guidPositionType, PROPVARIANT *pvPositionValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDuration(REFGUID guidPositionType, PROPVARIANT *pvDurationValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetRate(float flRate) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRate(float *pflRate) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSupportedRates(BOOL fForwardDirection, float *pflSlowestRate, float *pflFastestRate) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetState(MFP_MEDIAPLAYER_STATE *peState) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateMediaItemFromURL(LPCWSTR pwszURL, BOOL fSync, DWORD_PTR dwUserData, IMFPMediaItem **ppMediaItem) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateMediaItemFromObject(IUnknown *pIUnknownObj, BOOL fSync, DWORD_PTR dwUserData, IMFPMediaItem **ppMediaItem) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMediaItem(IMFPMediaItem *pIMFPMediaItem) = 0;
    virtual HRESULT STDMETHODCALLTYPE ClearMediaItem(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMediaItem(IMFPMediaItem **ppIMFPMediaItem) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVolume(float *pflVolume) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVolume(float flVolume) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBalance(float *pflBalance) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetBalance(float flBalance) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMute(BOOL *pfMute) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMute(BOOL fMute) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNativeVideoSize(SIZE *pszVideo, SIZE *pszARVideo) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIdealVideoSize(SIZE *pszMin, SIZE *pszMax) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVideoSourceRect(const MFVideoNormalizedRect *pnrcSource) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVideoSourceRect(MFVideoNormalizedRect *pnrcSource) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetAspectRatioMode(DWORD dwAspectRatioMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAspectRatioMode(DWORD *pdwAspectRatioMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVideoWindow(HWND *phwndVideo) = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdateVideo(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetBorderColor(COLORREF Clr) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBorderColor(COLORREF *pClr) = 0;
    virtual HRESULT STDMETHODCALLTYPE InsertEffect(IUnknown *pEffect, BOOL fOptional) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveEffect(IUnknown *pEffect) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveAllEffects(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Shutdown(void) = 0;
};
#else
typedef struct IMFPMediaPlayerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface) (IMFPMediaPlayer *This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFPMediaPlayer *This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFPMediaPlayer *This);
    HRESULT(STDMETHODCALLTYPE *Play)(IMFPMediaPlayer *This);
    HRESULT(STDMETHODCALLTYPE *Pause)(IMFPMediaPlayer *This);
    HRESULT(STDMETHODCALLTYPE *Stop)(IMFPMediaPlayer *This);
    HRESULT(STDMETHODCALLTYPE *FrameStep)(IMFPMediaPlayer *This);
    HRESULT(STDMETHODCALLTYPE *SetPosition)(IMFPMediaPlayer *This, REFGUID guidPositionType, const PROPVARIANT *pvPositionValue);
    HRESULT(STDMETHODCALLTYPE *GetPosition)(IMFPMediaPlayer *This, REFGUID guidPositionType, PROPVARIANT *pvPositionValue);
    HRESULT(STDMETHODCALLTYPE *GetDuration)(IMFPMediaPlayer *This, REFGUID guidPositionType, PROPVARIANT *pvDurationValue);
    HRESULT(STDMETHODCALLTYPE *SetRate)(IMFPMediaPlayer *This, float flRate);
    HRESULT(STDMETHODCALLTYPE *GetRate)(IMFPMediaPlayer *This, float *pflRate);
    HRESULT(STDMETHODCALLTYPE *GetSupportedRates)(IMFPMediaPlayer *This, BOOL fForwardDirection, float *pflSlowestRate, float *pflFastestRate);
    HRESULT(STDMETHODCALLTYPE *GetState)(IMFPMediaPlayer *This, MFP_MEDIAPLAYER_STATE *peState);
    HRESULT(STDMETHODCALLTYPE *CreateMediaItemFromURL)(IMFPMediaPlayer *This, LPCWSTR pwszURL, BOOL fSync, DWORD_PTR dwUserData, IMFPMediaItem **ppMediaItem);
    HRESULT(STDMETHODCALLTYPE *CreateMediaItemFromObject)(IMFPMediaPlayer *This, IUnknown *pIUnknownObj, BOOL fSync, DWORD_PTR dwUserData, IMFPMediaItem **ppMediaItem);
    HRESULT(STDMETHODCALLTYPE *SetMediaItem)(IMFPMediaPlayer *This, IMFPMediaItem *pIMFPMediaItem);
    HRESULT(STDMETHODCALLTYPE *ClearMediaItem)(IMFPMediaPlayer *This);
    HRESULT(STDMETHODCALLTYPE *GetMediaItem)(IMFPMediaPlayer *This, IMFPMediaItem **ppIMFPMediaItem);
    HRESULT(STDMETHODCALLTYPE *GetVolume)(IMFPMediaPlayer *This, float *pflVolume);
    HRESULT(STDMETHODCALLTYPE *SetVolume)(IMFPMediaPlayer *This, float flVolume);
    HRESULT(STDMETHODCALLTYPE *GetBalance)(IMFPMediaPlayer *This, float *pflBalance);
    HRESULT(STDMETHODCALLTYPE *SetBalance)(IMFPMediaPlayer *This, float flBalance);
    HRESULT(STDMETHODCALLTYPE *GetMute)(IMFPMediaPlayer *This, BOOL *pfMute);
    HRESULT(STDMETHODCALLTYPE *SetMute)(IMFPMediaPlayer *This, BOOL fMute);
    HRESULT(STDMETHODCALLTYPE *GetNativeVideoSize)(IMFPMediaPlayer *This, SIZE *pszVideo, SIZE *pszARVideo);
    HRESULT(STDMETHODCALLTYPE *GetIdealVideoSize)(IMFPMediaPlayer *This, SIZE *pszMin, SIZE *pszMax);
    HRESULT(STDMETHODCALLTYPE *SetVideoSourceRect)(IMFPMediaPlayer *This, const MFVideoNormalizedRect *pnrcSource);
    HRESULT(STDMETHODCALLTYPE *GetVideoSourceRect)(IMFPMediaPlayer *This, MFVideoNormalizedRect *pnrcSource);
    HRESULT(STDMETHODCALLTYPE *SetAspectRatioMode)(IMFPMediaPlayer *This, DWORD dwAspectRatioMode);
    HRESULT(STDMETHODCALLTYPE *GetAspectRatioMode)(IMFPMediaPlayer *This, DWORD *pdwAspectRatioMode);
    HRESULT(STDMETHODCALLTYPE *GetVideoWindow)(IMFPMediaPlayer *This, HWND *phwndVideo);
    HRESULT(STDMETHODCALLTYPE *UpdateVideo)(IMFPMediaPlayer *This);
    HRESULT(STDMETHODCALLTYPE *SetBorderColor)(IMFPMediaPlayer *This, COLORREF Clr);
    HRESULT(STDMETHODCALLTYPE *GetBorderColor)(IMFPMediaPlayer *This, COLORREF *pClr);
    HRESULT(STDMETHODCALLTYPE *InsertEffect)(IMFPMediaPlayer *This, IUnknown *pEffect, BOOL fOptional);
    HRESULT(STDMETHODCALLTYPE *RemoveEffect)(IMFPMediaPlayer *This, IUnknown *pEffect);
    HRESULT(STDMETHODCALLTYPE *RemoveAllEffects)(IMFPMediaPlayer *This);
    HRESULT(STDMETHODCALLTYPE *Shutdown)(IMFPMediaPlayer *This);
    END_INTERFACE
} IMFPMediaPlayerVtbl;

interface IMFPMediaPlayer {
    CONST_VTBL struct IMFPMediaPlayerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFPMediaPlayer_QueryInterface(This,riid,ppvObject)  ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define IMFPMediaPlayer_AddRef(This)  ( (This)->lpVtbl -> AddRef(This) )
#define IMFPMediaPlayer_Release(This)  ( (This)->lpVtbl -> Release(This) )
#define IMFPMediaPlayer_Play(This)  ( (This)->lpVtbl -> Play(This) )
#define IMFPMediaPlayer_Pause(This)  ( (This)->lpVtbl -> Pause(This) )
#define IMFPMediaPlayer_Stop(This)  ( (This)->lpVtbl -> Stop(This) )
#define IMFPMediaPlayer_FrameStep(This)  ( (This)->lpVtbl -> FrameStep(This) )
#define IMFPMediaPlayer_SetPosition(This,guidPositionType,pvPositionValue)  ( (This)->lpVtbl -> SetPosition(This,guidPositionType,pvPositionValue) )
#define IMFPMediaPlayer_GetPosition(This,guidPositionType,pvPositionValue)  ( (This)->lpVtbl -> GetPosition(This,guidPositionType,pvPositionValue) )
#define IMFPMediaPlayer_GetDuration(This,guidPositionType,pvDurationValue)  ( (This)->lpVtbl -> GetDuration(This,guidPositionType,pvDurationValue) )
#define IMFPMediaPlayer_SetRate(This,flRate)  ( (This)->lpVtbl -> SetRate(This,flRate) )
#define IMFPMediaPlayer_GetRate(This,pflRate)  ( (This)->lpVtbl -> GetRate(This,pflRate) )
#define IMFPMediaPlayer_GetSupportedRates(This,fForwardDirection,pflSlowestRate,pflFastestRate)  ( (This)->lpVtbl -> GetSupportedRates(This,fForwardDirection,pflSlowestRate,pflFastestRate) )
#define IMFPMediaPlayer_GetState(This,peState)  ( (This)->lpVtbl -> GetState(This,peState) )
#define IMFPMediaPlayer_CreateMediaItemFromURL(This,pwszURL,fSync,dwUserData,ppMediaItem)  ( (This)->lpVtbl -> CreateMediaItemFromURL(This,pwszURL,fSync,dwUserData,ppMediaItem) )
#define IMFPMediaPlayer_CreateMediaItemFromObject(This,pIUnknownObj,fSync,dwUserData,ppMediaItem)  ( (This)->lpVtbl -> CreateMediaItemFromObject(This,pIUnknownObj,fSync,dwUserData,ppMediaItem) )
#define IMFPMediaPlayer_SetMediaItem(This,pIMFPMediaItem)  ( (This)->lpVtbl -> SetMediaItem(This,pIMFPMediaItem) )
#define IMFPMediaPlayer_ClearMediaItem(This)  ( (This)->lpVtbl -> ClearMediaItem(This) )
#define IMFPMediaPlayer_GetMediaItem(This,ppIMFPMediaItem)  ( (This)->lpVtbl -> GetMediaItem(This,ppIMFPMediaItem) )
#define IMFPMediaPlayer_GetVolume(This,pflVolume)  ( (This)->lpVtbl -> GetVolume(This,pflVolume) )
#define IMFPMediaPlayer_SetVolume(This,flVolume)  ( (This)->lpVtbl -> SetVolume(This,flVolume) )
#define IMFPMediaPlayer_GetBalance(This,pflBalance)  ( (This)->lpVtbl -> GetBalance(This,pflBalance) )
#define IMFPMediaPlayer_SetBalance(This,flBalance)  ( (This)->lpVtbl -> SetBalance(This,flBalance) )
#define IMFPMediaPlayer_GetMute(This,pfMute)  ( (This)->lpVtbl -> GetMute(This,pfMute) )
#define IMFPMediaPlayer_SetMute(This,fMute)  ( (This)->lpVtbl -> SetMute(This,fMute) )
#define IMFPMediaPlayer_GetNativeVideoSize(This,pszVideo,pszARVideo)  ( (This)->lpVtbl -> GetNativeVideoSize(This,pszVideo,pszARVideo) )
#define IMFPMediaPlayer_GetIdealVideoSize(This,pszMin,pszMax)  ( (This)->lpVtbl -> GetIdealVideoSize(This,pszMin,pszMax) )
#define IMFPMediaPlayer_SetVideoSourceRect(This,pnrcSource)  ( (This)->lpVtbl -> SetVideoSourceRect(This,pnrcSource) )
#define IMFPMediaPlayer_GetVideoSourceRect(This,pnrcSource)  ( (This)->lpVtbl -> GetVideoSourceRect(This,pnrcSource) )
#define IMFPMediaPlayer_SetAspectRatioMode(This,dwAspectRatioMode)  ( (This)->lpVtbl -> SetAspectRatioMode(This,dwAspectRatioMode) )
#define IMFPMediaPlayer_GetAspectRatioMode(This,pdwAspectRatioMode)  ( (This)->lpVtbl -> GetAspectRatioMode(This,pdwAspectRatioMode) )
#define IMFPMediaPlayer_GetVideoWindow(This,phwndVideo)  ( (This)->lpVtbl -> GetVideoWindow(This,phwndVideo) )
#define IMFPMediaPlayer_UpdateVideo(This)  ( (This)->lpVtbl -> UpdateVideo(This) )
#define IMFPMediaPlayer_SetBorderColor(This,Clr)  ( (This)->lpVtbl -> SetBorderColor(This,Clr) )
#define IMFPMediaPlayer_GetBorderColor(This,pClr)  ( (This)->lpVtbl -> GetBorderColor(This,pClr) )
#define IMFPMediaPlayer_InsertEffect(This,pEffect,fOptional)  ( (This)->lpVtbl -> InsertEffect(This,pEffect,fOptional) )
#define IMFPMediaPlayer_RemoveEffect(This,pEffect)  ( (This)->lpVtbl -> RemoveEffect(This,pEffect) )
#define IMFPMediaPlayer_RemoveAllEffects(This)  ( (This)->lpVtbl -> RemoveAllEffects(This) )
#define IMFPMediaPlayer_Shutdown(This)  ( (This)->lpVtbl -> Shutdown(This) )
#endif

#endif

#endif /* __IMFPMediaPlayer_INTERFACE_DEFINED__ */

EXTERN_GUID(MFP_POSITIONTYPE_100NS, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0);

extern RPC_IF_HANDLE __MIDL_itf_mfplay_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfplay_0000_0001_v0_0_s_ifspec;

#ifndef __IMFPMediaItem_INTERFACE_DEFINED__
#define __IMFPMediaItem_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFPMediaItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("90EB3E6B-ECBF-45cc-B1DA-C6FE3EA70D57")IMFPMediaItem:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetMediaPlayer(IMFPMediaPlayer ** ppMediaPlayer) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetURL(LPWSTR *ppwszURL) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetObject(IUnknown **ppIUnknown) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUserData(DWORD_PTR *pdwUserData) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetUserData(DWORD_PTR dwUserData) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStartStopPosition(GUID *pguidStartPositionType, PROPVARIANT *pvStartValue, GUID *pguidStopPositionType, PROPVARIANT *pvStopValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetStartStopPosition(const GUID *pguidStartPositionType, const PROPVARIANT *pvStartValue, const GUID *pguidStopPositionType, const PROPVARIANT *pvStopValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE HasVideo(BOOL *pfHasVideo, BOOL *pfSelected) = 0;
    virtual HRESULT STDMETHODCALLTYPE HasAudio(BOOL *pfHasAudio, BOOL *pfSelected) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsProtected(BOOL *pfProtected) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDuration(REFGUID guidPositionType, PROPVARIANT *pvDurationValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNumberOfStreams(DWORD *pdwStreamCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamSelection(DWORD dwStreamIndex, BOOL *pfEnabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetStreamSelection(DWORD dwStreamIndex, BOOL fEnabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamAttribute(DWORD dwStreamIndex, REFGUID guidMFAttribute, PROPVARIANT *pvValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPresentationAttribute(REFGUID guidMFAttribute, PROPVARIANT *pvValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCharacteristics(MFP_MEDIAITEM_CHARACTERISTICS *pCharacteristics) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetStreamSink(DWORD dwStreamIndex, IUnknown *pMediaSink) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMetadata(IPropertyStore **ppMetadataStore) = 0;
};
#else
typedef struct IMFPMediaItemVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFPMediaItem * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFPMediaItem * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFPMediaItem * This);
    HRESULT(STDMETHODCALLTYPE *GetMediaPlayer)(IMFPMediaItem * This, IMFPMediaPlayer ** ppMediaPlayer);
    HRESULT(STDMETHODCALLTYPE *GetURL)(IMFPMediaItem * This, LPWSTR * ppwszURL);
    HRESULT(STDMETHODCALLTYPE *GetObject)(IMFPMediaItem * This, IUnknown ** ppIUnknown);
    HRESULT(STDMETHODCALLTYPE *GetUserData)(IMFPMediaItem * This, DWORD_PTR * pdwUserData);
    HRESULT(STDMETHODCALLTYPE *SetUserData)(IMFPMediaItem * This, DWORD_PTR dwUserData);
    HRESULT(STDMETHODCALLTYPE *GetStartStopPosition)(IMFPMediaItem * This, GUID * pguidStartPositionType, PROPVARIANT * pvStartValue, GUID * pguidStopPositionType, PROPVARIANT * pvStopValue);
    HRESULT(STDMETHODCALLTYPE *SetStartStopPosition)(IMFPMediaItem * This, const GUID * pguidStartPositionType, const PROPVARIANT * pvStartValue, const GUID * pguidStopPositionType, const PROPVARIANT * pvStopValue);
    HRESULT(STDMETHODCALLTYPE *HasVideo)(IMFPMediaItem * This, BOOL * pfHasVideo, BOOL * pfSelected);
    HRESULT(STDMETHODCALLTYPE *HasAudio)(IMFPMediaItem * This, BOOL * pfHasAudio, BOOL * pfSelected);
    HRESULT(STDMETHODCALLTYPE *IsProtected)(IMFPMediaItem * This, BOOL * pfProtected);
    HRESULT(STDMETHODCALLTYPE *GetDuration)(IMFPMediaItem * This, REFGUID guidPositionType, PROPVARIANT * pvDurationValue);
    HRESULT(STDMETHODCALLTYPE *GetNumberOfStreams)(IMFPMediaItem * This, DWORD * pdwStreamCount);
    HRESULT(STDMETHODCALLTYPE *GetStreamSelection)(IMFPMediaItem * This, DWORD dwStreamIndex, BOOL * pfEnabled);
    HRESULT(STDMETHODCALLTYPE *SetStreamSelection)(IMFPMediaItem * This, DWORD dwStreamIndex, BOOL fEnabled);
    HRESULT(STDMETHODCALLTYPE *GetStreamAttribute)(IMFPMediaItem * This, DWORD dwStreamIndex, REFGUID guidMFAttribute, PROPVARIANT * pvValue);
    HRESULT(STDMETHODCALLTYPE *GetPresentationAttribute)(IMFPMediaItem * This, REFGUID guidMFAttribute, PROPVARIANT * pvValue);
    HRESULT(STDMETHODCALLTYPE *GetCharacteristics)(IMFPMediaItem * This, MFP_MEDIAITEM_CHARACTERISTICS * pCharacteristics);
    HRESULT(STDMETHODCALLTYPE *SetStreamSink)(IMFPMediaItem * This, DWORD dwStreamIndex, IUnknown * pMediaSink);
    HRESULT(STDMETHODCALLTYPE *GetMetadata)(IMFPMediaItem * This, IPropertyStore ** ppMetadataStore);
    END_INTERFACE
} IMFPMediaItemVtbl;

interface IMFPMediaItem {
    CONST_VTBL struct IMFPMediaItemVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFPMediaItem_QueryInterface(This,riid,ppvObject)  ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define IMFPMediaItem_AddRef(This)  ( (This)->lpVtbl -> AddRef(This) )
#define IMFPMediaItem_Release(This)  ( (This)->lpVtbl -> Release(This) )
#define IMFPMediaItem_GetMediaPlayer(This,ppMediaPlayer)  ( (This)->lpVtbl -> GetMediaPlayer(This,ppMediaPlayer) )
#define IMFPMediaItem_GetURL(This,ppwszURL)  ( (This)->lpVtbl -> GetURL(This,ppwszURL) )
#define IMFPMediaItem_GetObject(This,ppIUnknown)  ( (This)->lpVtbl -> GetObject(This,ppIUnknown) )
#define IMFPMediaItem_GetUserData(This,pdwUserData)  ( (This)->lpVtbl -> GetUserData(This,pdwUserData) )
#define IMFPMediaItem_SetUserData(This,dwUserData)  ( (This)->lpVtbl -> SetUserData(This,dwUserData) )
#define IMFPMediaItem_GetStartStopPosition(This,pguidStartPositionType,pvStartValue,pguidStopPositionType,pvStopValue)  ( (This)->lpVtbl -> GetStartStopPosition(This,pguidStartPositionType,pvStartValue,pguidStopPositionType,pvStopValue) )
#define IMFPMediaItem_SetStartStopPosition(This,pguidStartPositionType,pvStartValue,pguidStopPositionType,pvStopValue)  ( (This)->lpVtbl -> SetStartStopPosition(This,pguidStartPositionType,pvStartValue,pguidStopPositionType,pvStopValue) )
#define IMFPMediaItem_HasVideo(This,pfHasVideo,pfSelected)  ( (This)->lpVtbl -> HasVideo(This,pfHasVideo,pfSelected) )
#define IMFPMediaItem_HasAudio(This,pfHasAudio,pfSelected)  ( (This)->lpVtbl -> HasAudio(This,pfHasAudio,pfSelected) )
#define IMFPMediaItem_IsProtected(This,pfProtected)  ( (This)->lpVtbl -> IsProtected(This,pfProtected) )
#define IMFPMediaItem_GetDuration(This,guidPositionType,pvDurationValue)  ( (This)->lpVtbl -> GetDuration(This,guidPositionType,pvDurationValue) )
#define IMFPMediaItem_GetNumberOfStreams(This,pdwStreamCount)  ( (This)->lpVtbl -> GetNumberOfStreams(This,pdwStreamCount) )
#define IMFPMediaItem_GetStreamSelection(This,dwStreamIndex,pfEnabled)  ( (This)->lpVtbl -> GetStreamSelection(This,dwStreamIndex,pfEnabled) )
#define IMFPMediaItem_SetStreamSelection(This,dwStreamIndex,fEnabled)  ( (This)->lpVtbl -> SetStreamSelection(This,dwStreamIndex,fEnabled) )
#define IMFPMediaItem_GetStreamAttribute(This,dwStreamIndex,guidMFAttribute,pvValue)  ( (This)->lpVtbl -> GetStreamAttribute(This,dwStreamIndex,guidMFAttribute,pvValue) )
#define IMFPMediaItem_GetPresentationAttribute(This,guidMFAttribute,pvValue)  ( (This)->lpVtbl -> GetPresentationAttribute(This,guidMFAttribute,pvValue) )
#define IMFPMediaItem_GetCharacteristics(This,pCharacteristics)  ( (This)->lpVtbl -> GetCharacteristics(This,pCharacteristics) )
#define IMFPMediaItem_SetStreamSink(This,dwStreamIndex,pMediaSink)  ( (This)->lpVtbl -> SetStreamSink(This,dwStreamIndex,pMediaSink) )
#define IMFPMediaItem_GetMetadata(This,ppMetadataStore)  ( (This)->lpVtbl -> GetMetadata(This,ppMetadataStore) )
#endif

#endif

#endif /* __IMFPMediaItem_INTERFACE_DEFINED__ */

typedef enum MFP_EVENT_TYPE {
    MFP_EVENT_TYPE_PLAY = 0,
    MFP_EVENT_TYPE_PAUSE = 1,
    MFP_EVENT_TYPE_STOP = 2,
    MFP_EVENT_TYPE_POSITION_SET = 3,
    MFP_EVENT_TYPE_RATE_SET = 4,
    MFP_EVENT_TYPE_MEDIAITEM_CREATED = 5,
    MFP_EVENT_TYPE_MEDIAITEM_SET = 6,
    MFP_EVENT_TYPE_FRAME_STEP = 7,
    MFP_EVENT_TYPE_MEDIAITEM_CLEARED = 8,
    MFP_EVENT_TYPE_MF = 9,
    MFP_EVENT_TYPE_ERROR = 10,
    MFP_EVENT_TYPE_PLAYBACK_ENDED = 11,
    MFP_EVENT_TYPE_ACQUIRE_USER_CREDENTIAL = 12
} MFP_EVENT_TYPE;

typedef struct MFP_EVENT_HEADER {
    MFP_EVENT_TYPE eEventType;
    HRESULT hrEvent;
    IMFPMediaPlayer *pMediaPlayer;
    MFP_MEDIAPLAYER_STATE eState;
    IPropertyStore *pPropertyStore;
} MFP_EVENT_HEADER;

typedef struct MFP_PLAY_EVENT {
    MFP_EVENT_HEADER header;
    IMFPMediaItem *pMediaItem;
} MFP_PLAY_EVENT;

typedef struct MFP_PAUSE_EVENT {
    MFP_EVENT_HEADER header;
    IMFPMediaItem *pMediaItem;
} MFP_PAUSE_EVENT;

typedef struct MFP_STOP_EVENT {
    MFP_EVENT_HEADER header;
    IMFPMediaItem *pMediaItem;
} MFP_STOP_EVENT;

typedef struct MFP_POSITION_SET_EVENT {
    MFP_EVENT_HEADER header;
    IMFPMediaItem *pMediaItem;
} MFP_POSITION_SET_EVENT;

typedef struct MFP_RATE_SET_EVENT {
    MFP_EVENT_HEADER header;
    IMFPMediaItem *pMediaItem;
    float flRate;
} MFP_RATE_SET_EVENT;

typedef struct MFP_MEDIAITEM_CREATED_EVENT {
    MFP_EVENT_HEADER header;
    IMFPMediaItem *pMediaItem;
    DWORD_PTR dwUserData;
} MFP_MEDIAITEM_CREATED_EVENT;

typedef struct MFP_MEDIAITEM_SET_EVENT {
    MFP_EVENT_HEADER header;
    IMFPMediaItem *pMediaItem;
} MFP_MEDIAITEM_SET_EVENT;

typedef struct MFP_FRAME_STEP_EVENT {
    MFP_EVENT_HEADER header;
    IMFPMediaItem *pMediaItem;
} MFP_FRAME_STEP_EVENT;

typedef struct MFP_MEDIAITEM_CLEARED_EVENT {
    MFP_EVENT_HEADER header;
    IMFPMediaItem *pMediaItem;
} MFP_MEDIAITEM_CLEARED_EVENT;

typedef struct MFP_MF_EVENT {
    MFP_EVENT_HEADER header;
    MediaEventType MFEventType;
    IMFMediaEvent *pMFMediaEvent;
    IMFPMediaItem *pMediaItem;
} MFP_MF_EVENT;

typedef struct MFP_ERROR_EVENT {
    MFP_EVENT_HEADER header;
} MFP_ERROR_EVENT;

typedef struct MFP_PLAYBACK_ENDED_EVENT {
    MFP_EVENT_HEADER header;
    IMFPMediaItem *pMediaItem;
} MFP_PLAYBACK_ENDED_EVENT;

typedef struct MFP_ACQUIRE_USER_CREDENTIAL_EVENT {
    MFP_EVENT_HEADER header;
    DWORD_PTR dwUserData;
    BOOL fProceedWithAuthentication;
    HRESULT hrAuthenticationStatus;
    LPCWSTR pwszURL;
    LPCWSTR pwszSite;
    LPCWSTR pwszRealm;
    LPCWSTR pwszPackage;
    LONG nRetries;
    MFP_CREDENTIAL_FLAGS flags;
    IMFNetCredential *pCredential;
} MFP_ACQUIRE_USER_CREDENTIAL_EVENT;

EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFP_PKEY_StreamIndex = { {0xa7cf9740, 0xe8d9, 0x4a87, {0xbd, 0x8e, 0x29, 0x67, 0x0, 0x1f, 0xd3, 0xad}}, 0x00 };
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFP_PKEY_StreamRenderingResults = { {0xa7cf9740, 0xe8d9, 0x4a87, {0xbd, 0x8e, 0x29, 0x67, 0x0, 0x1f, 0xd3, 0xad}}, 0x01 };

#define __MFP_CAST_EVENT(pHdr, Tag)                  (((pHdr)->eEventType == MFP_EVENT_TYPE_##Tag) ? (MFP_##Tag##_EVENT*)(pHdr) : NULL)
#define MFP_GET_PLAY_EVENT(pHdr)                     __MFP_CAST_EVENT( pHdr, PLAY )
#define MFP_GET_PAUSE_EVENT(pHdr)                    __MFP_CAST_EVENT( pHdr, PAUSE )
#define MFP_GET_STOP_EVENT(pHdr)                     __MFP_CAST_EVENT( pHdr, STOP )
#define MFP_GET_POSITION_SET_EVENT(pHdr)             __MFP_CAST_EVENT( pHdr, POSITION_SET )
#define MFP_GET_RATE_SET_EVENT(pHdr)                 __MFP_CAST_EVENT( pHdr, RATE_SET )
#define MFP_GET_MEDIAITEM_CREATED_EVENT(pHdr)        __MFP_CAST_EVENT( pHdr, MEDIAITEM_CREATED )
#define MFP_GET_MEDIAITEM_SET_EVENT(pHdr)            __MFP_CAST_EVENT( pHdr, MEDIAITEM_SET )
#define MFP_GET_FRAME_STEP_EVENT(pHdr)               __MFP_CAST_EVENT( pHdr, FRAME_STEP )
#define MFP_GET_MEDIAITEM_CLEARED_EVENT(pHdr)        __MFP_CAST_EVENT( pHdr, MEDIAITEM_CLEARED )
#define MFP_GET_MF_EVENT(pHdr)                       __MFP_CAST_EVENT( pHdr, MF )
#define MFP_GET_ERROR_EVENT(pHdr)                    __MFP_CAST_EVENT( pHdr, ERROR )
#define MFP_GET_PLAYBACK_ENDED_EVENT(pHdr)           __MFP_CAST_EVENT( pHdr, PLAYBACK_ENDED )
#define MFP_GET_ACQUIRE_USER_CREDENTIAL_EVENT(pHdr)  __MFP_CAST_EVENT( pHdr, ACQUIRE_USER_CREDENTIAL )

extern RPC_IF_HANDLE __MIDL_itf_mfplay_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfplay_0000_0002_v0_0_s_ifspec;

#ifndef __IMFPMediaPlayerCallback_INTERFACE_DEFINED__
#define __IMFPMediaPlayerCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFPMediaPlayerCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("766C8FFB-5FDB-4fea-A28D-B912996F51BD")IMFPMediaPlayerCallback:public IUnknown
{
    public:
    virtual void STDMETHODCALLTYPE OnMediaPlayerEvent(MFP_EVENT_HEADER *pEventHeader) = 0;
};
#else
typedef struct IMFPMediaPlayerCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFPMediaPlayerCallback * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFPMediaPlayerCallback * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFPMediaPlayerCallback * This);
    void (STDMETHODCALLTYPE *OnMediaPlayerEvent)(IMFPMediaPlayerCallback * This, MFP_EVENT_HEADER * pEventHeader);
    END_INTERFACE
} IMFPMediaPlayerCallbackVtbl;

interface IMFPMediaPlayerCallback {
    CONST_VTBL struct IMFPMediaPlayerCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFPMediaPlayerCallback_QueryInterface(This,riid,ppvObject)  ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) )
#define IMFPMediaPlayerCallback_AddRef(This)  ( (This)->lpVtbl -> AddRef(This) )
#define IMFPMediaPlayerCallback_Release(This)  ( (This)->lpVtbl -> Release(This) )
#define IMFPMediaPlayerCallback_OnMediaPlayerEvent(This,pEventHeader)  ( (This)->lpVtbl -> OnMediaPlayerEvent(This,pEventHeader) )
#endif /* COBJMACROS */

#endif

#endif /* __IMFPMediaPlayerCallback_INTERFACE_DEFINED__ */

#endif /* WINVER >= _WIN32_WINNT_WIN7 */

extern RPC_IF_HANDLE __MIDL_itf_mfplay_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfplay_0000_0003_v0_0_s_ifspec;

#ifdef __cplusplus
}
#endif

#endif /* _MFPLAY_H */
