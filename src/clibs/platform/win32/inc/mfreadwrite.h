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

#ifndef _MFREADWRITE_H
#define _MFREADWRITE_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IMFReadWriteClassFactory_FWD_DEFINED__
#define __IMFReadWriteClassFactory_FWD_DEFINED__
typedef interface IMFReadWriteClassFactory IMFReadWriteClassFactory;
#endif

#ifndef __IMFSourceReader_FWD_DEFINED__
#define __IMFSourceReader_FWD_DEFINED__
typedef interface IMFSourceReader IMFSourceReader;
#endif

#ifndef __IMFSourceReaderEx_FWD_DEFINED__
#define __IMFSourceReaderEx_FWD_DEFINED__
typedef interface IMFSourceReaderEx IMFSourceReaderEx;
#endif

#ifndef __IMFSourceReaderCallback_FWD_DEFINED__
#define __IMFSourceReaderCallback_FWD_DEFINED__
typedef interface IMFSourceReaderCallback IMFSourceReaderCallback;
#endif

#ifndef __IMFSinkWriter_FWD_DEFINED__
#define __IMFSinkWriter_FWD_DEFINED__
typedef interface IMFSinkWriter IMFSinkWriter;
#endif

#ifndef __IMFSinkWriterEx_FWD_DEFINED__
#define __IMFSinkWriterEx_FWD_DEFINED__
typedef interface IMFSinkWriterEx IMFSinkWriterEx;
#endif

#ifndef __IMFSinkWriterEncoderConfig_FWD_DEFINED__
#define __IMFSinkWriterEncoderConfig_FWD_DEFINED__
typedef interface IMFSinkWriterEncoderConfig IMFSinkWriterEncoderConfig;
#endif

#ifndef __IMFSinkWriterCallback_FWD_DEFINED__
#define __IMFSinkWriterCallback_FWD_DEFINED__
typedef interface IMFSinkWriterCallback IMFSinkWriterCallback;
#endif

#include "mfobjects.h"

#ifdef __cplusplus
extern "C"{
#endif 


#if (WINVER >= _WIN32_WINNT_WIN7)
EXTERN_GUID(CLSID_MFReadWriteClassFactory, 0x48e2ed0f, 0x98c2, 0x4a37, 0xbe, 0xd5, 0x16, 0x63, 0x12, 0xdd, 0xd8, 0x3f);

extern RPC_IF_HANDLE __MIDL_itf_mfreadwrite_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfreadwrite_0000_0000_v0_0_s_ifspec;

#ifndef __IMFReadWriteClassFactory_INTERFACE_DEFINED__
#define __IMFReadWriteClassFactory_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFReadWriteClassFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("E7FE2E12-661C-40DA-92F9-4F002AB67627")IMFReadWriteClassFactory:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateInstanceFromURL(REFCLSID clsid, LPCWSTR pwszURL, IMFAttributes * pAttributes, REFIID riid, LPVOID * ppvObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateInstanceFromObject(REFCLSID clsid, IUnknown *punkObject, IMFAttributes *pAttributes, REFIID riid, LPVOID *ppvObject) = 0;
};
#else
typedef struct IMFReadWriteClassFactoryVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFReadWriteClassFactory * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFReadWriteClassFactory * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFReadWriteClassFactory * This);
    HRESULT(STDMETHODCALLTYPE *CreateInstanceFromURL)(IMFReadWriteClassFactory * This, REFCLSID clsid, LPCWSTR pwszURL, IMFAttributes * pAttributes, REFIID riid, LPVOID * ppvObject);
    HRESULT(STDMETHODCALLTYPE *CreateInstanceFromObject)(IMFReadWriteClassFactory * This, REFCLSID clsid, IUnknown * punkObject, IMFAttributes * pAttributes, REFIID riid, LPVOID * ppvObject);
    END_INTERFACE
} IMFReadWriteClassFactoryVtbl;

interface IMFReadWriteClassFactory {
    CONST_VTBL struct IMFReadWriteClassFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFReadWriteClassFactory_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFReadWriteClassFactory_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFReadWriteClassFactory_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFReadWriteClassFactory_CreateInstanceFromURL(This,clsid,pwszURL,pAttributes,riid,ppvObject)  ((This)->lpVtbl->CreateInstanceFromURL(This,clsid,pwszURL,pAttributes,riid,ppvObject))
#define IMFReadWriteClassFactory_CreateInstanceFromObject(This,clsid,punkObject,pAttributes,riid,ppvObject)  ((This)->lpVtbl->CreateInstanceFromObject(This,clsid,punkObject,pAttributes,riid,ppvObject))
#endif /* COBJMACROS */

#endif

#endif /* __IMFReadWriteClassFactory_INTERFACE_DEFINED__ */

STDAPI MFCreateSourceReaderFromURL(LPCWSTR , IMFAttributes *, IMFSourceReader **);
STDAPI MFCreateSourceReaderFromByteStream(IMFByteStream *, IMFAttributes *, IMFSourceReader **);
STDAPI MFCreateSourceReaderFromMediaSource(IMFMediaSource *, IMFAttributes *, IMFSourceReader **);

EXTERN_GUID(CLSID_MFSourceReader, 0x1777133c, 0x0881, 0x411b, 0xa5, 0x77, 0xad, 0x54, 0x5f, 0x07, 0x14, 0xc4);
EXTERN_GUID(MF_SOURCE_READER_ASYNC_CALLBACK, 0x1e3dbeac, 0xbb43, 0x4c35, 0xb5, 0x07, 0xcd, 0x64, 0x44, 0x64, 0xc9, 0x65);
EXTERN_GUID(MF_SOURCE_READER_D3D_MANAGER, 0xec822da2, 0xe1e9, 0x4b29, 0xa0, 0xd8, 0x56, 0x3c, 0x71, 0x9f, 0x52, 0x69);
EXTERN_GUID(MF_SOURCE_READER_DISABLE_DXVA, 0xaa456cfd, 0x3943, 0x4a1e, 0xa7, 0x7d, 0x18, 0x38, 0xc0, 0xea, 0x2e, 0x35);
EXTERN_GUID(MF_SOURCE_READER_MEDIASOURCE_CONFIG, 0x9085abeb, 0x0354, 0x48f9, 0xab, 0xb5, 0x20, 0x0d, 0xf8, 0x38, 0xc6, 0x8e);
EXTERN_GUID(MF_SOURCE_READER_MEDIASOURCE_CHARACTERISTICS, 0x6d23f5c8, 0xc5d7, 0x4a9b, 0x99, 0x71, 0x5d, 0x11, 0xf8, 0xbc, 0xa8, 0x80);
EXTERN_GUID(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, 0xfb394f3d, 0xccf1, 0x42ee, 0xbb, 0xb3, 0xf9, 0xb8, 0x45, 0xd5, 0x68, 0x1d);
#if (WINVER >= _WIN32_WINNT_WIN8) 
EXTERN_GUID( MF_SOURCE_READER_ENABLE_ADVANCED_VIDEO_PROCESSING, 0xf81da2c, 0xb537, 0x4672, 0xa8, 0xb2, 0xa6, 0x81, 0xb1, 0x73, 0x7, 0xa3);
EXTERN_GUID( MF_SOURCE_READER_DISABLE_CAMERA_PLUGINS, 0x9d3365dd, 0x58f, 0x4cfb, 0x9f, 0x97, 0xb3, 0x14, 0xcc, 0x99, 0xc8, 0xad );
#endif /* WINVER >= _WIN32_WINNT_WIN8 */
EXTERN_GUID(MF_SOURCE_READER_DISCONNECT_MEDIASOURCE_ON_SHUTDOWN, 0x56b67165, 0x219e, 0x456d, 0xa2, 0x2e, 0x2d, 0x30, 0x04, 0xc7, 0xfe, 0x56);

typedef enum MF_SOURCE_READER_FLAG {
    MF_SOURCE_READERF_ERROR = 0x1,
    MF_SOURCE_READERF_ENDOFSTREAM = 0x2,
    MF_SOURCE_READERF_NEWSTREAM = 0x4,
    MF_SOURCE_READERF_NATIVEMEDIATYPECHANGED = 0x10,
    MF_SOURCE_READERF_CURRENTMEDIATYPECHANGED = 0x20,
    MF_SOURCE_READERF_STREAMTICK = 0x100
} MF_SOURCE_READER_FLAG;

/* DEFINE_ENUM_FLAG_OPERATORS(MF_SOURCE_READER_FLAG) */

typedef enum MF_SOURCE_READER_CONTROL_FLAG {
    MF_SOURCE_READER_CONTROLF_DRAIN = 0x1
} MF_SOURCE_READER_CONTROL_FLAG;

/* DEFINE_ENUM_FLAG_OPERATORS(MF_SOURCE_READER_CONTROL_FLAG) */

enum __MIDL___MIDL_itf_mfreadwrite_0000_0001_0001 {
    MF_SOURCE_READER_INVALID_STREAM_INDEX = 0xffffffff,
    MF_SOURCE_READER_ALL_STREAMS = 0xfffffffe,
    MF_SOURCE_READER_ANY_STREAM = 0xfffffffe,
    MF_SOURCE_READER_FIRST_AUDIO_STREAM = 0xfffffffd,
    MF_SOURCE_READER_FIRST_VIDEO_STREAM = 0xfffffffc,
    MF_SOURCE_READER_MEDIASOURCE = 0xffffffff
};

#if (WINVER >= _WIN32_WINNT_WIN8) 
enum __MIDL___MIDL_itf_mfreadwrite_0000_0001_0002 {
    MF_SOURCE_READER_CURRENT_TYPE_INDEX	= 0xffffffff
};
#endif /* WINVER >= _WIN32_WINNT_WIN8 */

extern RPC_IF_HANDLE __MIDL_itf_mfreadwrite_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfreadwrite_0000_0001_v0_0_s_ifspec;

#ifndef __IMFSourceReader_INTERFACE_DEFINED__
#define __IMFSourceReader_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSourceReader;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("70ae66f2-c809-4e4f-8915-bdcb406b7993")IMFSourceReader:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetStreamSelection(DWORD dwStreamIndex, BOOL * pfSelected) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetStreamSelection(DWORD dwStreamIndex, BOOL fSelected) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNativeMediaType(DWORD dwStreamIndex, DWORD dwMediaTypeIndex, IMFMediaType **ppMediaType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentMediaType(DWORD dwStreamIndex, IMFMediaType **ppMediaType) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCurrentMediaType(DWORD dwStreamIndex, DWORD *pdwReserved, IMFMediaType *pMediaType) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCurrentPosition(REFGUID guidTimeFormat, REFPROPVARIANT varPosition) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReadSample(DWORD dwStreamIndex, DWORD dwControlFlags, DWORD *pdwActualStreamIndex, DWORD *pdwStreamFlags, LONGLONG *pllTimestamp, IMFSample **ppSample) = 0;
    virtual HRESULT STDMETHODCALLTYPE Flush(DWORD dwStreamIndex) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetServiceForStream(DWORD dwStreamIndex, REFGUID guidService, REFIID riid, LPVOID *ppvObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPresentationAttribute(DWORD dwStreamIndex, REFGUID guidAttribute, PROPVARIANT *pvarAttribute) = 0;
};
#else
typedef struct IMFSourceReaderVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSourceReader * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSourceReader * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSourceReader * This);
    HRESULT(STDMETHODCALLTYPE *GetStreamSelection)(IMFSourceReader * This, DWORD dwStreamIndex, BOOL * pfSelected);
    HRESULT(STDMETHODCALLTYPE *SetStreamSelection)(IMFSourceReader * This, DWORD dwStreamIndex, BOOL fSelected);
    HRESULT(STDMETHODCALLTYPE *GetNativeMediaType)(IMFSourceReader * This, DWORD dwStreamIndex, DWORD dwMediaTypeIndex, IMFMediaType ** ppMediaType);
    HRESULT(STDMETHODCALLTYPE *GetCurrentMediaType)(IMFSourceReader * This, DWORD dwStreamIndex, IMFMediaType ** ppMediaType);
    HRESULT(STDMETHODCALLTYPE *SetCurrentMediaType)(IMFSourceReader * This, DWORD dwStreamIndex, DWORD * pdwReserved, IMFMediaType * pMediaType);
    HRESULT(STDMETHODCALLTYPE *SetCurrentPosition)(IMFSourceReader * This, REFGUID guidTimeFormat, REFPROPVARIANT varPosition);
    HRESULT(STDMETHODCALLTYPE *ReadSample)(IMFSourceReader * This, DWORD dwStreamIndex, DWORD dwControlFlags, DWORD * pdwActualStreamIndex, DWORD * pdwStreamFlags, LONGLONG * pllTimestamp, IMFSample ** ppSample);
    HRESULT(STDMETHODCALLTYPE *Flush)(IMFSourceReader * This, DWORD dwStreamIndex);
    HRESULT(STDMETHODCALLTYPE *GetServiceForStream)(IMFSourceReader * This, DWORD dwStreamIndex, REFGUID guidService, REFIID riid, LPVOID * ppvObject);
    HRESULT(STDMETHODCALLTYPE *GetPresentationAttribute)(IMFSourceReader * This, DWORD dwStreamIndex, REFGUID guidAttribute, PROPVARIANT * pvarAttribute);
    END_INTERFACE
} IMFSourceReaderVtbl;

interface IMFSourceReader {
    CONST_VTBL struct IMFSourceReaderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSourceReader_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSourceReader_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSourceReader_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFSourceReader_GetStreamSelection(This,dwStreamIndex,pfSelected)  ((This)->lpVtbl->GetStreamSelection(This,dwStreamIndex,pfSelected))
#define IMFSourceReader_SetStreamSelection(This,dwStreamIndex,fSelected)  ((This)->lpVtbl->SetStreamSelection(This,dwStreamIndex,fSelected))
#define IMFSourceReader_GetNativeMediaType(This,dwStreamIndex,dwMediaTypeIndex,ppMediaType)  ((This)->lpVtbl->GetNativeMediaType(This,dwStreamIndex,dwMediaTypeIndex,ppMediaType))
#define IMFSourceReader_GetCurrentMediaType(This,dwStreamIndex,ppMediaType)  ((This)->lpVtbl->GetCurrentMediaType(This,dwStreamIndex,ppMediaType))
#define IMFSourceReader_SetCurrentMediaType(This,dwStreamIndex,pdwReserved,pMediaType)  ((This)->lpVtbl->SetCurrentMediaType(This,dwStreamIndex,pdwReserved,pMediaType))
#define IMFSourceReader_SetCurrentPosition(This,guidTimeFormat,varPosition)  ((This)->lpVtbl->SetCurrentPosition(This,guidTimeFormat,varPosition))
#define IMFSourceReader_ReadSample(This,dwStreamIndex,dwControlFlags,pdwActualStreamIndex,pdwStreamFlags,pllTimestamp,ppSample)  ((This)->lpVtbl->ReadSample(This,dwStreamIndex,dwControlFlags,pdwActualStreamIndex,pdwStreamFlags,pllTimestamp,ppSample))
#define IMFSourceReader_Flush(This,dwStreamIndex)  ((This)->lpVtbl->Flush(This,dwStreamIndex))
#define IMFSourceReader_GetServiceForStream(This,dwStreamIndex,guidService,riid,ppvObject)  ((This)->lpVtbl->GetServiceForStream(This,dwStreamIndex,guidService,riid,ppvObject))
#define IMFSourceReader_GetPresentationAttribute(This,dwStreamIndex,guidAttribute,pvarAttribute)  ((This)->lpVtbl->GetPresentationAttribute(This,dwStreamIndex,guidAttribute,pvarAttribute))
#endif /* COBJMACROS */

#endif

#endif /* __IMFSourceReader_INTERFACE_DEFINED__ */

#if (WINVER >= _WIN32_WINNT_WIN8)

extern RPC_IF_HANDLE __MIDL_itf_mfreadwrite_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfreadwrite_0000_0002_v0_0_s_ifspec;

#ifndef __IMFSourceReaderEx_INTERFACE_DEFINED__
#define __IMFSourceReaderEx_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSourceReaderEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7b981cf0-560e-4116-9875-b099895f23d7") IMFSourceReaderEx:public IMFSourceReader
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetNativeMediaType(DWORD, IMFMediaType *, DWORD *) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddTransformForStream(DWORD, IUnknown *) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveAllTransformsForStream(DWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTransformForStream(DWORD, DWORD, GUID *, IMFTransform **) = 0;
};
#else /* C style interface */
typedef struct IMFSourceReaderExVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IMFSourceReaderEx *, REFIID, void **);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSourceReaderEx *);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSourceReaderEx *);
    HRESULT(STDMETHODCALLTYPE *GetStreamSelection)(IMFSourceReaderEx *, DWORD, BOOL *);
    HRESULT(STDMETHODCALLTYPE *SetStreamSelection)(IMFSourceReaderEx *, DWORD, BOOL);
    HRESULT(STDMETHODCALLTYPE *GetNativeMediaType)(IMFSourceReaderEx *, DWORD, DWORD, IMFMediaType **);
    HRESULT(STDMETHODCALLTYPE *GetCurrentMediaType)(IMFSourceReaderEx *, DWORD, IMFMediaType **);
    HRESULT(STDMETHODCALLTYPE *SetCurrentMediaType)(IMFSourceReaderEx *, DWORD, DWORD *, IMFMediaType *);
    HRESULT(STDMETHODCALLTYPE *SetCurrentPosition)(IMFSourceReaderEx *, REFGUID, REFPROPVARIANT);
    HRESULT(STDMETHODCALLTYPE *ReadSample)(IMFSourceReaderEx *, DWORD, DWORD, DWORD *, DWORD *, LONGLONG *, IMFSample **);
    HRESULT(STDMETHODCALLTYPE *Flush)(IMFSourceReaderEx *, DWORD);
    HRESULT(STDMETHODCALLTYPE *GetServiceForStream)(IMFSourceReaderEx *, DWORD, REFGUID, REFIID, LPVOID *);
    HRESULT(STDMETHODCALLTYPE *GetPresentationAttribute)(IMFSourceReaderEx *, DWORD, REFGUID, PROPVARIANT *);
    HRESULT(STDMETHODCALLTYPE *SetNativeMediaType)(IMFSourceReaderEx *, DWORD, IMFMediaType *, DWORD *);
    HRESULT(STDMETHODCALLTYPE *AddTransformForStream)(IMFSourceReaderEx *, DWORD, IUnknown *);
    HRESULT(STDMETHODCALLTYPE *RemoveAllTransformsForStream)(IMFSourceReaderEx *, DWORD);
    HRESULT(STDMETHODCALLTYPE *GetTransformForStream)(IMFSourceReaderEx *, DWORD, DWORD, GUID *, IMFTransform **);
    END_INTERFACE
} IMFSourceReaderExVtbl;

interface IMFSourceReaderEx {
    CONST_VTBL struct IMFSourceReaderExVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSourceReaderEx_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFSourceReaderEx_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFSourceReaderEx_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFSourceReaderEx_GetStreamSelection(This,dwStreamIndex,pfSelected)  ((This)->lpVtbl->GetStreamSelection(This,dwStreamIndex,pfSelected)) 
#define IMFSourceReaderEx_SetStreamSelection(This,dwStreamIndex,fSelected)  ((This)->lpVtbl->SetStreamSelection(This,dwStreamIndex,fSelected)) 
#define IMFSourceReaderEx_GetNativeMediaType(This,dwStreamIndex,dwMediaTypeIndex,ppMediaType)  ((This)->lpVtbl->GetNativeMediaType(This,dwStreamIndex,dwMediaTypeIndex,ppMediaType)) 
#define IMFSourceReaderEx_GetCurrentMediaType(This,dwStreamIndex,ppMediaType)  ((This)->lpVtbl->GetCurrentMediaType(This,dwStreamIndex,ppMediaType)) 
#define IMFSourceReaderEx_SetCurrentMediaType(This,dwStreamIndex,pdwReserved,pMediaType)  ((This)->lpVtbl->SetCurrentMediaType(This,dwStreamIndex,pdwReserved,pMediaType)) 
#define IMFSourceReaderEx_SetCurrentPosition(This,guidTimeFormat,varPosition)  ((This)->lpVtbl->SetCurrentPosition(This,guidTimeFormat,varPosition)) 
#define IMFSourceReaderEx_ReadSample(This,dwStreamIndex,dwControlFlags,pdwActualStreamIndex,pdwStreamFlags,pllTimestamp,ppSample)  ((This)->lpVtbl->ReadSample(This,dwStreamIndex,dwControlFlags,pdwActualStreamIndex,pdwStreamFlags,pllTimestamp,ppSample)) 
#define IMFSourceReaderEx_Flush(This,dwStreamIndex)  ((This)->lpVtbl->Flush(This,dwStreamIndex)) 
#define IMFSourceReaderEx_GetServiceForStream(This,dwStreamIndex,guidService,riid,ppvObject)  ((This)->lpVtbl->GetServiceForStream(This,dwStreamIndex,guidService,riid,ppvObject)) 
#define IMFSourceReaderEx_GetPresentationAttribute(This,dwStreamIndex,guidAttribute,pvarAttribute)  ((This)->lpVtbl->GetPresentationAttribute(This,dwStreamIndex,guidAttribute,pvarAttribute)) 
#define IMFSourceReaderEx_SetNativeMediaType(This,dwStreamIndex,pMediaType,pdwStreamFlags)  ((This)->lpVtbl->SetNativeMediaType(This,dwStreamIndex,pMediaType,pdwStreamFlags)) 
#define IMFSourceReaderEx_AddTransformForStream(This,dwStreamIndex,pTransformOrActivate)  ((This)->lpVtbl->AddTransformForStream(This,dwStreamIndex,pTransformOrActivate)) 
#define IMFSourceReaderEx_RemoveAllTransformsForStream(This,dwStreamIndex)  ((This)->lpVtbl->RemoveAllTransformsForStream(This,dwStreamIndex)) 
#define IMFSourceReaderEx_GetTransformForStream(This,dwStreamIndex,dwTransformIndex,pGuidCategory,ppTransform)  ((This)->lpVtbl->GetTransformForStream(This,dwStreamIndex,dwTransformIndex,pGuidCategory,ppTransform)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFSourceReaderEx_INTERFACE_DEFINED__ */

#endif

#ifndef __IMFSourceReaderCallback_INTERFACE_DEFINED__
#define __IMFSourceReaderCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSourceReaderCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("deec8d99-fa1d-4d82-84c2-2c8969944867")IMFSourceReaderCallback:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnReadSample(HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample * pSample) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnFlush(DWORD dwStreamIndex) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnEvent(DWORD dwStreamIndex, IMFMediaEvent *pEvent) = 0;
};
#else
typedef struct IMFSourceReaderCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSourceReaderCallback * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSourceReaderCallback * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSourceReaderCallback * This);
    HRESULT(STDMETHODCALLTYPE *OnReadSample)(IMFSourceReaderCallback * This, HRESULT hrStatus, DWORD dwStreamIndex, DWORD dwStreamFlags, LONGLONG llTimestamp, IMFSample * pSample);
    HRESULT(STDMETHODCALLTYPE *OnFlush)(IMFSourceReaderCallback * This, DWORD dwStreamIndex);
    HRESULT(STDMETHODCALLTYPE *OnEvent)(IMFSourceReaderCallback * This, DWORD dwStreamIndex, IMFMediaEvent * pEvent);
    END_INTERFACE
} IMFSourceReaderCallbackVtbl;

interface IMFSourceReaderCallback {
    CONST_VTBL struct IMFSourceReaderCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSourceReaderCallback_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSourceReaderCallback_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSourceReaderCallback_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFSourceReaderCallback_OnReadSample(This,hrStatus,dwStreamIndex,dwStreamFlags,llTimestamp,pSample)  ((This)->lpVtbl->OnReadSample(This,hrStatus,dwStreamIndex,dwStreamFlags,llTimestamp,pSample))
#define IMFSourceReaderCallback_OnFlush(This,dwStreamIndex)  ((This)->lpVtbl->OnFlush(This,dwStreamIndex))
#define IMFSourceReaderCallback_OnEvent(This,dwStreamIndex,pEvent)  ((This)->lpVtbl->OnEvent(This,dwStreamIndex,pEvent))
#endif /* COBJMACROS */

#endif

#endif /* __IMFSourceReaderCallback_INTERFACE_DEFINED__ */

STDAPI MFCreateSinkWriterFromURL(LPCWSTR, IMFByteStream *, IMFAttributes *, IMFSinkWriter **);
STDAPI MFCreateSinkWriterFromMediaSink(IMFMediaSink *, IMFAttributes *, IMFSinkWriter **);

EXTERN_GUID(CLSID_MFSinkWriter, 0xa3bbfb17, 0x8273, 0x4e52, 0x9e, 0x0e, 0x97, 0x39, 0xdc, 0x88, 0x79, 0x90);
EXTERN_GUID(MF_SINK_WRITER_ASYNC_CALLBACK, 0x48cb183e, 0x7b0b, 0x46f4, 0x82, 0x2e, 0x5e, 0x1d, 0x2d, 0xda, 0x43, 0x54);
EXTERN_GUID(MF_SINK_WRITER_DISABLE_THROTTLING, 0x08b845d8, 0x2b74, 0x4afe, 0x9d, 0x53, 0xbe, 0x16, 0xd2, 0xd5, 0xae, 0x4f);
#if (WINVER >= _WIN32_WINNT_WIN8) 
EXTERN_GUID( MF_SINK_WRITER_D3D_MANAGER, 0xec822da2, 0xe1e9, 0x4b29, 0xa0, 0xd8, 0x56, 0x3c, 0x71, 0x9f, 0x52, 0x69);
EXTERN_GUID( MF_SINK_WRITER_ENCODER_CONFIG, 0xad91cd04, 0xa7cc, 0x4ac7, 0x99, 0xb6, 0xa5, 0x7b, 0x9a, 0x4a, 0x7c, 0x70);
#endif /* WINVER >= _WIN32_WINNT_WIN8 */

enum __MIDL___MIDL_itf_mfreadwrite_0000_0003_0001 {
    MF_SINK_WRITER_INVALID_STREAM_INDEX = 0xffffffff,
    MF_SINK_WRITER_ALL_STREAMS = 0xfffffffe,
    MF_SINK_WRITER_MEDIASINK = 0xffffffff
};

typedef struct _MF_SINK_WRITER_STATISTICS {
    DWORD cb;
    LONGLONG llLastTimestampReceived;
    LONGLONG llLastTimestampEncoded;
    LONGLONG llLastTimestampProcessed;
    LONGLONG llLastStreamTickReceived;
    LONGLONG llLastSinkSampleRequest;
    QWORD qwNumSamplesReceived;
    QWORD qwNumSamplesEncoded;
    QWORD qwNumSamplesProcessed;
    QWORD qwNumStreamTicksReceived;
    DWORD dwByteCountQueued;
    QWORD qwByteCountProcessed;
    DWORD dwNumOutstandingSinkSampleRequests;
    DWORD dwAverageSampleRateReceived;
    DWORD dwAverageSampleRateEncoded;
    DWORD dwAverageSampleRateProcessed;
} MF_SINK_WRITER_STATISTICS;

extern RPC_IF_HANDLE __MIDL_itf_mfreadwrite_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfreadwrite_0000_0003_v0_0_s_ifspec;

#ifndef __IMFSinkWriter_INTERFACE_DEFINED__
#define __IMFSinkWriter_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSinkWriter;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3137f1cd-fe5e-4805-a5d8-fb477448cb3d")IMFSinkWriter:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE AddStream(IMFMediaType * pTargetMediaType, DWORD * pdwStreamIndex) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetInputMediaType(DWORD dwStreamIndex, IMFMediaType *pInputMediaType, IMFAttributes *pEncodingParameters) = 0;
    virtual HRESULT STDMETHODCALLTYPE BeginWriting(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE WriteSample(DWORD dwStreamIndex, IMFSample *pSample) = 0;
    virtual HRESULT STDMETHODCALLTYPE SendStreamTick(DWORD dwStreamIndex, LONGLONG llTimestamp) = 0;
    virtual HRESULT STDMETHODCALLTYPE PlaceMarker(DWORD dwStreamIndex, LPVOID pvContext) = 0;
    virtual HRESULT STDMETHODCALLTYPE NotifyEndOfSegment(DWORD dwStreamIndex) = 0;
    virtual HRESULT STDMETHODCALLTYPE Flush(DWORD dwStreamIndex) = 0;
    virtual HRESULT STDMETHODCALLTYPE Finalize(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetServiceForStream(DWORD dwStreamIndex, REFGUID guidService, REFIID riid, LPVOID *ppvObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStatistics(DWORD dwStreamIndex, MF_SINK_WRITER_STATISTICS *pStats) = 0;
};
#else
typedef struct IMFSinkWriterVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSinkWriter * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSinkWriter * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSinkWriter * This);
    HRESULT(STDMETHODCALLTYPE *AddStream)(IMFSinkWriter * This, IMFMediaType * pTargetMediaType, DWORD * pdwStreamIndex);
    HRESULT(STDMETHODCALLTYPE *SetInputMediaType)(IMFSinkWriter * This, DWORD dwStreamIndex, IMFMediaType * pInputMediaType, IMFAttributes * pEncodingParameters);
    HRESULT(STDMETHODCALLTYPE *BeginWriting)(IMFSinkWriter * This);
    HRESULT(STDMETHODCALLTYPE *WriteSample)(IMFSinkWriter * This, DWORD dwStreamIndex, IMFSample * pSample);
    HRESULT(STDMETHODCALLTYPE *SendStreamTick)(IMFSinkWriter * This, DWORD dwStreamIndex, LONGLONG llTimestamp);
    HRESULT(STDMETHODCALLTYPE *PlaceMarker)(IMFSinkWriter * This, DWORD dwStreamIndex, LPVOID pvContext);
    HRESULT(STDMETHODCALLTYPE *NotifyEndOfSegment)(IMFSinkWriter * This, DWORD dwStreamIndex);
    HRESULT(STDMETHODCALLTYPE *Flush)(IMFSinkWriter * This, DWORD dwStreamIndex);
    HRESULT(STDMETHODCALLTYPE *Finalize)(IMFSinkWriter * This);
    HRESULT(STDMETHODCALLTYPE *GetServiceForStream)(IMFSinkWriter * This, DWORD dwStreamIndex, REFGUID guidService, REFIID riid, LPVOID * ppvObject);
    HRESULT(STDMETHODCALLTYPE *GetStatistics)(IMFSinkWriter * This, DWORD dwStreamIndex, MF_SINK_WRITER_STATISTICS * pStats);
    END_INTERFACE
} IMFSinkWriterVtbl;

interface IMFSinkWriter {
    CONST_VTBL struct IMFSinkWriterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSinkWriter_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSinkWriter_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSinkWriter_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFSinkWriter_AddStream(This,pTargetMediaType,pdwStreamIndex)  ((This)->lpVtbl->AddStream(This,pTargetMediaType,pdwStreamIndex))
#define IMFSinkWriter_SetInputMediaType(This,dwStreamIndex,pInputMediaType,pEncodingParameters)  ((This)->lpVtbl->SetInputMediaType(This,dwStreamIndex,pInputMediaType,pEncodingParameters))
#define IMFSinkWriter_BeginWriting(This)  ((This)->lpVtbl->BeginWriting(This))
#define IMFSinkWriter_WriteSample(This,dwStreamIndex,pSample)  ((This)->lpVtbl->WriteSample(This,dwStreamIndex,pSample))
#define IMFSinkWriter_SendStreamTick(This,dwStreamIndex,llTimestamp)  ((This)->lpVtbl->SendStreamTick(This,dwStreamIndex,llTimestamp))
#define IMFSinkWriter_PlaceMarker(This,dwStreamIndex,pvContext)  ((This)->lpVtbl->PlaceMarker(This,dwStreamIndex,pvContext))
#define IMFSinkWriter_NotifyEndOfSegment(This,dwStreamIndex)  ((This)->lpVtbl->NotifyEndOfSegment(This,dwStreamIndex))
#define IMFSinkWriter_Flush(This,dwStreamIndex)  ((This)->lpVtbl->Flush(This,dwStreamIndex))
#define IMFSinkWriter_Finalize(This)  ((This)->lpVtbl->Finalize(This))
#define IMFSinkWriter_GetServiceForStream(This,dwStreamIndex,guidService,riid,ppvObject)  ((This)->lpVtbl->GetServiceForStream(This,dwStreamIndex,guidService,riid,ppvObject))
#define IMFSinkWriter_GetStatistics(This,dwStreamIndex,pStats)  ((This)->lpVtbl->GetStatistics(This,dwStreamIndex,pStats))
#endif /* COBJMACROS */

#endif

#endif /* __IMFSinkWriter_INTERFACE_DEFINED__ */

#if (WINVER >= _WIN32_WINNT_WIN8) 

extern RPC_IF_HANDLE __MIDL_itf_mfreadwrite_0000_0005_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfreadwrite_0000_0005_v0_0_s_ifspec;

#ifndef __IMFSinkWriterEx_INTERFACE_DEFINED__
#define __IMFSinkWriterEx_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSinkWriterEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("588d72ab-5Bc1-496a-8714-b70617141b25")IMFSinkWriterEx:public IMFSinkWriter
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetTransformForStream(DWORD, DWORD, GUID *, IMFTransform **) = 0;
};
#else /* C style interface */
typedef struct IMFSinkWriterExVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IMFSinkWriterEx *, REFIID, void **);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSinkWriterEx *);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSinkWriterEx *);
    HRESULT(STDMETHODCALLTYPE *AddStream)(IMFSinkWriterEx *, IMFMediaType *, DWORD *);
    HRESULT(STDMETHODCALLTYPE *SetInputMediaType)(IMFSinkWriterEx *, DWORD, IMFMediaType *, IMFAttributes *);
    HRESULT(STDMETHODCALLTYPE *BeginWriting)(IMFSinkWriterEx *);
    HRESULT(STDMETHODCALLTYPE *WriteSample)(IMFSinkWriterEx *, DWORD, IMFSample *);
    HRESULT(STDMETHODCALLTYPE *SendStreamTick)(IMFSinkWriterEx *, DWORD, LONGLONG);
    HRESULT(STDMETHODCALLTYPE *PlaceMarker)(IMFSinkWriterEx *, DWORD, LPVOID);
    HRESULT(STDMETHODCALLTYPE *NotifyEndOfSegment)(IMFSinkWriterEx *, DWORD);
    HRESULT(STDMETHODCALLTYPE *Flush)(IMFSinkWriterEx *, DWORD);
    HRESULT(STDMETHODCALLTYPE *Finalize)(IMFSinkWriterEx * This);
    HRESULT(STDMETHODCALLTYPE *GetServiceForStream)(IMFSinkWriterEx *, DWORD, REFGUID, REFIID, LPVOID *);
    HRESULT(STDMETHODCALLTYPE *GetStatistics)(IMFSinkWriterEx *, DWORD, MF_SINK_WRITER_STATISTICS *);
    HRESULT(STDMETHODCALLTYPE *GetTransformForStream)(IMFSinkWriterEx *, DWORD, DWORD, GUID *, IMFTransform **);
    END_INTERFACE
} IMFSinkWriterExVtbl;

interface IMFSinkWriterEx {
    CONST_VTBL struct IMFSinkWriterExVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSinkWriterEx_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFSinkWriterEx_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFSinkWriterEx_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFSinkWriterEx_AddStream(This,pTargetMediaType,pdwStreamIndex)  ((This)->lpVtbl->AddStream(This,pTargetMediaType,pdwStreamIndex)) 
#define IMFSinkWriterEx_SetInputMediaType(This,dwStreamIndex,pInputMediaType,pEncodingParameters)  ((This)->lpVtbl->SetInputMediaType(This,dwStreamIndex,pInputMediaType,pEncodingParameters)) 
#define IMFSinkWriterEx_BeginWriting(This)  ((This)->lpVtbl->BeginWriting(This)) 
#define IMFSinkWriterEx_WriteSample(This,dwStreamIndex,pSample)  ((This)->lpVtbl->WriteSample(This,dwStreamIndex,pSample)) 
#define IMFSinkWriterEx_SendStreamTick(This,dwStreamIndex,llTimestamp)  ((This)->lpVtbl->SendStreamTick(This,dwStreamIndex,llTimestamp)) 
#define IMFSinkWriterEx_PlaceMarker(This,dwStreamIndex,pvContext)  ((This)->lpVtbl->PlaceMarker(This,dwStreamIndex,pvContext)) 
#define IMFSinkWriterEx_NotifyEndOfSegment(This,dwStreamIndex)  ((This)->lpVtbl->NotifyEndOfSegment(This,dwStreamIndex)) 
#define IMFSinkWriterEx_Flush(This,dwStreamIndex)  ((This)->lpVtbl->Flush(This,dwStreamIndex)) 
#define IMFSinkWriterEx_Finalize(This)  ((This)->lpVtbl->Finalize(This)) 
#define IMFSinkWriterEx_GetServiceForStream(This,dwStreamIndex,guidService,riid,ppvObject)  ((This)->lpVtbl->GetServiceForStream(This,dwStreamIndex,guidService,riid,ppvObject)) 
#define IMFSinkWriterEx_GetStatistics(This,dwStreamIndex,pStats)  ((This)->lpVtbl->GetStatistics(This,dwStreamIndex,pStats)) 
#define IMFSinkWriterEx_GetTransformForStream(This,dwStreamIndex,dwTransformIndex,pGuidCategory,ppTransform)  ((This)->lpVtbl->GetTransformForStream(This,dwStreamIndex,dwTransformIndex,pGuidCategory,ppTransform)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFSinkWriterEx_INTERFACE_DEFINED__ */

#ifndef __IMFSinkWriterEncoderConfig_INTERFACE_DEFINED__
#define __IMFSinkWriterEncoderConfig_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSinkWriterEncoderConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("17C3779E-3CDE-4EDE-8C60-3899F5F53AD6")IMFSinkWriterEncoderConfig:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetTargetMediaType(DWORD, IMFMediaType *, IMFAttributes *) = 0;
    virtual HRESULT STDMETHODCALLTYPE PlaceEncodingParameters(DWORD, IMFAttributes *) = 0;
};
#else 	/* C style interface */

typedef struct IMFSinkWriterEncoderConfigVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IMFSinkWriterEncoderConfig *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IMFSinkWriterEncoderConfig *);
    ULONG (STDMETHODCALLTYPE *Release)(IMFSinkWriterEncoderConfig *);
    HRESULT (STDMETHODCALLTYPE *SetTargetMediaType)(IMFSinkWriterEncoderConfig *, DWORD, IMFMediaType *, IMFAttributes *);
    HRESULT (STDMETHODCALLTYPE *PlaceEncodingParameters)(IMFSinkWriterEncoderConfig *, DWORD, IMFAttributes *);
    END_INTERFACE
} IMFSinkWriterEncoderConfigVtbl;

interface IMFSinkWriterEncoderConfig {
    CONST_VTBL struct IMFSinkWriterEncoderConfigVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSinkWriterEncoderConfig_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFSinkWriterEncoderConfig_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFSinkWriterEncoderConfig_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFSinkWriterEncoderConfig_SetTargetMediaType(This,dwStreamIndex,pTargetMediaType,pEncodingParameters)  ((This)->lpVtbl->SetTargetMediaType(This,dwStreamIndex,pTargetMediaType,pEncodingParameters)) 
#define IMFSinkWriterEncoderConfig_PlaceEncodingParameters(This,dwStreamIndex,pEncodingParameters)  ((This)->lpVtbl->PlaceEncodingParameters(This,dwStreamIndex,pEncodingParameters)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFSinkWriterEncoderConfig_INTERFACE_DEFINED__ */

#endif

#ifndef __IMFSinkWriterCallback_INTERFACE_DEFINED__
#define __IMFSinkWriterCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSinkWriterCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("666f76de-33d2-41b9-a458-29ed0a972c58")IMFSinkWriterCallback:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnFinalize(HRESULT hrStatus) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnMarker(DWORD dwStreamIndex, LPVOID pvContext) = 0;
};
#else
typedef struct IMFSinkWriterCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSinkWriterCallback * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSinkWriterCallback * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSinkWriterCallback * This);
    HRESULT(STDMETHODCALLTYPE *OnFinalize)(IMFSinkWriterCallback * This, HRESULT hrStatus);
    HRESULT(STDMETHODCALLTYPE *OnMarker)(IMFSinkWriterCallback * This, DWORD dwStreamIndex, LPVOID pvContext);
    END_INTERFACE
} IMFSinkWriterCallbackVtbl;

interface IMFSinkWriterCallback {
    CONST_VTBL struct IMFSinkWriterCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSinkWriterCallback_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSinkWriterCallback_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSinkWriterCallback_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFSinkWriterCallback_OnFinalize(This,hrStatus)  ((This)->lpVtbl->OnFinalize(This,hrStatus))
#define IMFSinkWriterCallback_OnMarker(This,dwStreamIndex,pvContext)  ((This)->lpVtbl->OnMarker(This,dwStreamIndex,pvContext))
#endif /* COBJMACROS */

#endif

#endif /* __IMFSinkWriterCallback_INTERFACE_DEFINED__ */

EXTERN_GUID(MF_READWRITE_DISABLE_CONVERTERS, 0x98d5b065, 0x1374, 0x4847, 0x8d, 0x5d, 0x31, 0x52, 0x0f, 0xee, 0x71, 0x56);
EXTERN_GUID(MF_READWRITE_ENABLE_HARDWARE_TRANSFORMS, 0xa634a91c, 0x822b, 0x41b9, 0xa4, 0x94, 0x4d, 0xe4, 0x64, 0x36, 0x12, 0xb0);
#endif /* WINVER >= _WIN32_WINNT_WIN7 */

#if (WINVER >= _WIN32_WINNT_WIN8) 
EXTERN_GUID( MF_READWRITE_MMCSS_CLASS, 0x39384300, 0xd0eb, 0x40b1, 0x87, 0xa0, 0x33, 0x18, 0x87, 0x1b, 0x5a, 0x53);
EXTERN_GUID( MF_READWRITE_MMCSS_PRIORITY, 0x43ad19ce, 0xf33f, 0x4ba9, 0xa5, 0x80, 0xe4, 0xcd, 0x12, 0xf2, 0xd1, 0x44);
EXTERN_GUID( MF_READWRITE_MMCSS_CLASS_AUDIO, 0x430847da, 0x0890, 0x4b0e, 0x93, 0x8c, 0x05, 0x43, 0x32, 0xc5, 0x47, 0xe1);
EXTERN_GUID( MF_READWRITE_MMCSS_PRIORITY_AUDIO, 0x273db885, 0x2de2, 0x4db2, 0xa6, 0xa7, 0xfd, 0xb6, 0x6f, 0xb4, 0x0b, 0x61);
EXTERN_GUID( MF_READWRITE_D3D_OPTIONAL, 0x216479d9, 0x3071, 0x42ca, 0xbb, 0x6c, 0x4c, 0x22, 0x10, 0x2e, 0x1d, 0x18);
#endif /* WINVER >= _WIN32_WINNT_WIN8 */

extern RPC_IF_HANDLE __MIDL_itf_mfreadwrite_0000_0005_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfreadwrite_0000_0005_v0_0_s_ifspec;


#ifdef __cplusplus
}
#endif

#endif /* _MFREADWRITE_H */
