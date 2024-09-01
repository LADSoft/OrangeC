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

#ifndef _MFIDL_H
#define _MFIDL_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IMFMediaSession_FWD_DEFINED__
#define __IMFMediaSession_FWD_DEFINED__
typedef interface IMFMediaSession IMFMediaSession;
#endif

#ifndef __IMFSourceResolver_FWD_DEFINED__
#define __IMFSourceResolver_FWD_DEFINED__
typedef interface IMFSourceResolver IMFSourceResolver;
#endif

#ifndef __IMFMediaSource_FWD_DEFINED__
#define __IMFMediaSource_FWD_DEFINED__
typedef interface IMFMediaSource IMFMediaSource;
#endif

#ifndef __IMFMediaSourceEx_FWD_DEFINED__
#define __IMFMediaSourceEx_FWD_DEFINED__
typedef interface IMFMediaSourceEx IMFMediaSourceEx;
#endif

#ifndef __IMFMediaStream_FWD_DEFINED__
#define __IMFMediaStream_FWD_DEFINED__
typedef interface IMFMediaStream IMFMediaStream;
#endif

#ifndef __IMFMediaSink_FWD_DEFINED__
#define __IMFMediaSink_FWD_DEFINED__
typedef interface IMFMediaSink IMFMediaSink;
#endif

#ifndef __IMFStreamSink_FWD_DEFINED__
#define __IMFStreamSink_FWD_DEFINED__
typedef interface IMFStreamSink IMFStreamSink;
#endif

#ifndef __IMFVideoSampleAllocator_FWD_DEFINED__
#define __IMFVideoSampleAllocator_FWD_DEFINED__
typedef interface IMFVideoSampleAllocator IMFVideoSampleAllocator;
#endif

#ifndef __IMFVideoSampleAllocatorNotify_FWD_DEFINED__
#define __IMFVideoSampleAllocatorNotify_FWD_DEFINED__
typedef interface IMFVideoSampleAllocatorNotify IMFVideoSampleAllocatorNotify;
#endif

#ifndef __IMFVideoSampleAllocatorCallback_FWD_DEFINED__
#define __IMFVideoSampleAllocatorCallback_FWD_DEFINED__
typedef interface IMFVideoSampleAllocatorCallback IMFVideoSampleAllocatorCallback;
#endif

#ifndef __IMFVideoSampleAllocatorEx_FWD_DEFINED__
#define __IMFVideoSampleAllocatorEx_FWD_DEFINED__
typedef interface IMFVideoSampleAllocatorEx IMFVideoSampleAllocatorEx;
#endif

#ifndef __IMFDXGIDeviceManagerSource_FWD_DEFINED__
#define __IMFDXGIDeviceManagerSource_FWD_DEFINED__
typedef interface IMFDXGIDeviceManagerSource IMFDXGIDeviceManagerSource;
#endif

#ifndef __IMFVideoProcessorControl_FWD_DEFINED__
#define __IMFVideoProcessorControl_FWD_DEFINED__
typedef interface IMFVideoProcessorControl IMFVideoProcessorControl;
#endif

#ifndef __IMFTopology_FWD_DEFINED__
#define __IMFTopology_FWD_DEFINED__
typedef interface IMFTopology IMFTopology;
#endif

#ifndef __IMFTopologyNode_FWD_DEFINED__
#define __IMFTopologyNode_FWD_DEFINED__
typedef interface IMFTopologyNode IMFTopologyNode;
#endif

#ifndef __IMFGetService_FWD_DEFINED__
#define __IMFGetService_FWD_DEFINED__
typedef interface IMFGetService IMFGetService;
#endif

#ifndef __IMFClock_FWD_DEFINED__
#define __IMFClock_FWD_DEFINED__
typedef interface IMFClock IMFClock;
#endif

#ifndef __IMFPresentationClock_FWD_DEFINED__
#define __IMFPresentationClock_FWD_DEFINED__
typedef interface IMFPresentationClock IMFPresentationClock;
#endif

#ifndef __IMFPresentationTimeSource_FWD_DEFINED__
#define __IMFPresentationTimeSource_FWD_DEFINED__
typedef interface IMFPresentationTimeSource IMFPresentationTimeSource;
#endif

#ifndef __IMFClockStateSink_FWD_DEFINED__
#define __IMFClockStateSink_FWD_DEFINED__
typedef interface IMFClockStateSink IMFClockStateSink;
#endif

#ifndef __IMFPresentationDescriptor_FWD_DEFINED__
#define __IMFPresentationDescriptor_FWD_DEFINED__
typedef interface IMFPresentationDescriptor IMFPresentationDescriptor;
#endif

#ifndef __IMFStreamDescriptor_FWD_DEFINED__
#define __IMFStreamDescriptor_FWD_DEFINED__
typedef interface IMFStreamDescriptor IMFStreamDescriptor;
#endif

#ifndef __IMFMediaTypeHandler_FWD_DEFINED__
#define __IMFMediaTypeHandler_FWD_DEFINED__
typedef interface IMFMediaTypeHandler IMFMediaTypeHandler;
#endif

#ifndef __IMFTimer_FWD_DEFINED__
#define __IMFTimer_FWD_DEFINED__
typedef interface IMFTimer IMFTimer;
#endif

#ifndef __IMFShutdown_FWD_DEFINED__
#define __IMFShutdown_FWD_DEFINED__
typedef interface IMFShutdown IMFShutdown;
#endif

#ifndef __IMFTopoLoader_FWD_DEFINED__
#define __IMFTopoLoader_FWD_DEFINED__
typedef interface IMFTopoLoader IMFTopoLoader;
#endif

#ifndef __IMFContentProtectionManager_FWD_DEFINED__
#define __IMFContentProtectionManager_FWD_DEFINED__
typedef interface IMFContentProtectionManager IMFContentProtectionManager;
#endif

#ifndef __IMFContentEnabler_FWD_DEFINED__
#define __IMFContentEnabler_FWD_DEFINED__
typedef interface IMFContentEnabler IMFContentEnabler;
#endif

#ifndef __IMFMetadata_FWD_DEFINED__
#define __IMFMetadata_FWD_DEFINED__
typedef interface IMFMetadata IMFMetadata;
#endif

#ifndef __IMFMetadataProvider_FWD_DEFINED__
#define __IMFMetadataProvider_FWD_DEFINED__
typedef interface IMFMetadataProvider IMFMetadataProvider;
#endif

#ifndef __IMFRateSupport_FWD_DEFINED__
#define __IMFRateSupport_FWD_DEFINED__
typedef interface IMFRateSupport IMFRateSupport;
#endif

#ifndef __IMFRateControl_FWD_DEFINED__
#define __IMFRateControl_FWD_DEFINED__
typedef interface IMFRateControl IMFRateControl;
#endif

#ifndef __IMFTimecodeTranslate_FWD_DEFINED__
#define __IMFTimecodeTranslate_FWD_DEFINED__
typedef interface IMFTimecodeTranslate IMFTimecodeTranslate;
#endif

#ifndef __IMFSeekInfo_FWD_DEFINED__
#define __IMFSeekInfo_FWD_DEFINED__
typedef interface IMFSeekInfo IMFSeekInfo;
#endif

#ifndef __IMFSimpleAudioVolume_FWD_DEFINED__
#define __IMFSimpleAudioVolume_FWD_DEFINED__
typedef interface IMFSimpleAudioVolume IMFSimpleAudioVolume;
#endif

#ifndef __IMFAudioStreamVolume_FWD_DEFINED__
#define __IMFAudioStreamVolume_FWD_DEFINED__
typedef interface IMFAudioStreamVolume IMFAudioStreamVolume;
#endif

#ifndef __IMFAudioPolicy_FWD_DEFINED__
#define __IMFAudioPolicy_FWD_DEFINED__
typedef interface IMFAudioPolicy IMFAudioPolicy;
#endif

#ifndef __IMFSampleGrabberSinkCallback_FWD_DEFINED__
#define __IMFSampleGrabberSinkCallback_FWD_DEFINED__
typedef interface IMFSampleGrabberSinkCallback IMFSampleGrabberSinkCallback;
#endif

#ifndef __IMFSampleGrabberSinkCallback2_FWD_DEFINED__
#define __IMFSampleGrabberSinkCallback2_FWD_DEFINED__
typedef interface IMFSampleGrabberSinkCallback2 IMFSampleGrabberSinkCallback2;
#endif

#ifndef __IMFWorkQueueServices_FWD_DEFINED__
#define __IMFWorkQueueServices_FWD_DEFINED__
typedef interface IMFWorkQueueServices IMFWorkQueueServices;
#endif

#ifndef __IMFWorkQueueServicesEx_FWD_DEFINED__
#define __IMFWorkQueueServicesEx_FWD_DEFINED__
typedef interface IMFWorkQueueServicesEx IMFWorkQueueServicesEx;
#endif

#ifndef __IMFQualityManager_FWD_DEFINED__
#define __IMFQualityManager_FWD_DEFINED__
typedef interface IMFQualityManager IMFQualityManager;
#endif

#ifndef __IMFQualityAdvise_FWD_DEFINED__
#define __IMFQualityAdvise_FWD_DEFINED__
typedef interface IMFQualityAdvise IMFQualityAdvise;
#endif

#ifndef __IMFQualityAdvise2_FWD_DEFINED__
#define __IMFQualityAdvise2_FWD_DEFINED__
typedef interface IMFQualityAdvise2 IMFQualityAdvise2;
#endif

#ifndef __IMFQualityAdviseLimits_FWD_DEFINED__
#define __IMFQualityAdviseLimits_FWD_DEFINED__
typedef interface IMFQualityAdviseLimits IMFQualityAdviseLimits;
#endif

#ifndef __IMFRealTimeClient_FWD_DEFINED__
#define __IMFRealTimeClient_FWD_DEFINED__
typedef interface IMFRealTimeClient IMFRealTimeClient;
#endif

#ifndef __IMFRealTimeClientEx_FWD_DEFINED__
#define __IMFRealTimeClientEx_FWD_DEFINED__
typedef interface IMFRealTimeClientEx IMFRealTimeClientEx;
#endif

#ifndef __IMFSequencerSource_FWD_DEFINED__
#define __IMFSequencerSource_FWD_DEFINED__
typedef interface IMFSequencerSource IMFSequencerSource;
#endif

#ifndef __IMFMediaSourceTopologyProvider_FWD_DEFINED__
#define __IMFMediaSourceTopologyProvider_FWD_DEFINED__
typedef interface IMFMediaSourceTopologyProvider IMFMediaSourceTopologyProvider;
#endif

#ifndef __IMFMediaSourcePresentationProvider_FWD_DEFINED__
#define __IMFMediaSourcePresentationProvider_FWD_DEFINED__
typedef interface IMFMediaSourcePresentationProvider IMFMediaSourcePresentationProvider;
#endif

#ifndef __IMFTopologyNodeAttributeEditor_FWD_DEFINED__
#define __IMFTopologyNodeAttributeEditor_FWD_DEFINED__
typedef interface IMFTopologyNodeAttributeEditor IMFTopologyNodeAttributeEditor;
#endif

#ifndef __IMFByteStreamBuffering_FWD_DEFINED__
#define __IMFByteStreamBuffering_FWD_DEFINED__
typedef interface IMFByteStreamBuffering IMFByteStreamBuffering;
#endif

#ifndef __IMFByteStreamCacheControl_FWD_DEFINED__
#define __IMFByteStreamCacheControl_FWD_DEFINED__
typedef interface IMFByteStreamCacheControl IMFByteStreamCacheControl;
#endif

#ifndef __IMFByteStreamTimeSeek_FWD_DEFINED__
#define __IMFByteStreamTimeSeek_FWD_DEFINED__
typedef interface IMFByteStreamTimeSeek IMFByteStreamTimeSeek;
#endif

#ifndef __IMFByteStreamCacheControl2_FWD_DEFINED__
#define __IMFByteStreamCacheControl2_FWD_DEFINED__
typedef interface IMFByteStreamCacheControl2 IMFByteStreamCacheControl2;
#endif

#ifndef __IMFNetCredential_FWD_DEFINED__
#define __IMFNetCredential_FWD_DEFINED__
typedef interface IMFNetCredential IMFNetCredential;
#endif

#ifndef __IMFNetCredentialManager_FWD_DEFINED__
#define __IMFNetCredentialManager_FWD_DEFINED__
typedef interface IMFNetCredentialManager IMFNetCredentialManager;
#endif

#ifndef __IMFNetCredentialCache_FWD_DEFINED__
#define __IMFNetCredentialCache_FWD_DEFINED__
typedef interface IMFNetCredentialCache IMFNetCredentialCache;
#endif

#ifndef __IMFSSLCertificateManager_FWD_DEFINED__
#define __IMFSSLCertificateManager_FWD_DEFINED__
typedef interface IMFSSLCertificateManager IMFSSLCertificateManager;
#endif

#ifndef __IMFNetResourceFilter_FWD_DEFINED__
#define __IMFNetResourceFilter_FWD_DEFINED__
typedef interface IMFNetResourceFilter IMFNetResourceFilter;
#endif

#ifndef __IMFSourceOpenMonitor_FWD_DEFINED__
#define __IMFSourceOpenMonitor_FWD_DEFINED__
typedef interface IMFSourceOpenMonitor IMFSourceOpenMonitor;
#endif

#ifndef __IMFNetProxyLocator_FWD_DEFINED__
#define __IMFNetProxyLocator_FWD_DEFINED__
typedef interface IMFNetProxyLocator IMFNetProxyLocator;
#endif

#ifndef __IMFNetProxyLocatorFactory_FWD_DEFINED__
#define __IMFNetProxyLocatorFactory_FWD_DEFINED__
typedef interface IMFNetProxyLocatorFactory IMFNetProxyLocatorFactory;
#endif

#ifndef __IMFSaveJob_FWD_DEFINED__
#define __IMFSaveJob_FWD_DEFINED__
typedef interface IMFSaveJob IMFSaveJob;
#endif

#ifndef __IMFNetSchemeHandlerConfig_FWD_DEFINED__
#define __IMFNetSchemeHandlerConfig_FWD_DEFINED__
typedef interface IMFNetSchemeHandlerConfig IMFNetSchemeHandlerConfig;
#endif

#ifndef __IMFSchemeHandler_FWD_DEFINED__
#define __IMFSchemeHandler_FWD_DEFINED__
typedef interface IMFSchemeHandler IMFSchemeHandler;
#endif

#ifndef __IMFByteStreamHandler_FWD_DEFINED__
#define __IMFByteStreamHandler_FWD_DEFINED__
typedef interface IMFByteStreamHandler IMFByteStreamHandler;
#endif

#ifndef __IMFTrustedInput_FWD_DEFINED__
#define __IMFTrustedInput_FWD_DEFINED__
typedef interface IMFTrustedInput IMFTrustedInput;
#endif

#ifndef __IMFInputTrustAuthority_FWD_DEFINED__
#define __IMFInputTrustAuthority_FWD_DEFINED__
typedef interface IMFInputTrustAuthority IMFInputTrustAuthority;
#endif

#ifndef __IMFTrustedOutput_FWD_DEFINED__
#define __IMFTrustedOutput_FWD_DEFINED__
typedef interface IMFTrustedOutput IMFTrustedOutput;
#endif

#ifndef __IMFOutputTrustAuthority_FWD_DEFINED__
#define __IMFOutputTrustAuthority_FWD_DEFINED__
typedef interface IMFOutputTrustAuthority IMFOutputTrustAuthority;
#endif

#ifndef __IMFOutputPolicy_FWD_DEFINED__
#define __IMFOutputPolicy_FWD_DEFINED__
typedef interface IMFOutputPolicy IMFOutputPolicy;
#endif

#ifndef __IMFOutputSchema_FWD_DEFINED__
#define __IMFOutputSchema_FWD_DEFINED__
typedef interface IMFOutputSchema IMFOutputSchema;
#endif

#ifndef __IMFSecureChannel_FWD_DEFINED__
#define __IMFSecureChannel_FWD_DEFINED__
typedef interface IMFSecureChannel IMFSecureChannel;
#endif

#ifndef __IMFSampleProtection_FWD_DEFINED__
#define __IMFSampleProtection_FWD_DEFINED__
typedef interface IMFSampleProtection IMFSampleProtection;
#endif

#ifndef __IMFMediaSinkPreroll_FWD_DEFINED__
#define __IMFMediaSinkPreroll_FWD_DEFINED__
typedef interface IMFMediaSinkPreroll IMFMediaSinkPreroll;
#endif

#ifndef __IMFFinalizableMediaSink_FWD_DEFINED__
#define __IMFFinalizableMediaSink_FWD_DEFINED__
typedef interface IMFFinalizableMediaSink IMFFinalizableMediaSink;
#endif

#ifndef __IMFStreamingSinkConfig_FWD_DEFINED__
#define __IMFStreamingSinkConfig_FWD_DEFINED__
typedef interface IMFStreamingSinkConfig IMFStreamingSinkConfig;
#endif

#ifndef __IMFRemoteProxy_FWD_DEFINED__
#define __IMFRemoteProxy_FWD_DEFINED__
typedef interface IMFRemoteProxy IMFRemoteProxy;
#endif

#ifndef __IMFObjectReferenceStream_FWD_DEFINED__
#define __IMFObjectReferenceStream_FWD_DEFINED__
typedef interface IMFObjectReferenceStream IMFObjectReferenceStream;
#endif

#ifndef __IMFPMPHost_FWD_DEFINED__
#define __IMFPMPHost_FWD_DEFINED__
typedef interface IMFPMPHost IMFPMPHost;
#endif

#ifndef __IMFPMPClient_FWD_DEFINED__
#define __IMFPMPClient_FWD_DEFINED__
typedef interface IMFPMPClient IMFPMPClient;
#endif

#ifndef __IMFPMPServer_FWD_DEFINED__
#define __IMFPMPServer_FWD_DEFINED__
typedef interface IMFPMPServer IMFPMPServer;
#endif

#ifndef __IMFRemoteDesktopPlugin_FWD_DEFINED__
#define __IMFRemoteDesktopPlugin_FWD_DEFINED__
typedef interface IMFRemoteDesktopPlugin IMFRemoteDesktopPlugin;
#endif

#ifndef __IMFSAMIStyle_FWD_DEFINED__
#define __IMFSAMIStyle_FWD_DEFINED__
typedef interface IMFSAMIStyle IMFSAMIStyle;
#endif

#ifndef __IMFTranscodeProfile_FWD_DEFINED__
#define __IMFTranscodeProfile_FWD_DEFINED__
typedef interface IMFTranscodeProfile IMFTranscodeProfile;
#endif

#ifndef __IMFTranscodeSinkInfoProvider_FWD_DEFINED__
#define __IMFTranscodeSinkInfoProvider_FWD_DEFINED__
typedef interface IMFTranscodeSinkInfoProvider IMFTranscodeSinkInfoProvider;
#endif

#ifndef __IMFFieldOfUseMFTUnlock_FWD_DEFINED__
#define __IMFFieldOfUseMFTUnlock_FWD_DEFINED__
typedef interface IMFFieldOfUseMFTUnlock IMFFieldOfUseMFTUnlock;
#endif

#ifndef __IMFLocalMFTRegistration_FWD_DEFINED__
#define __IMFLocalMFTRegistration_FWD_DEFINED__
typedef interface IMFLocalMFTRegistration IMFLocalMFTRegistration;
#endif

#ifndef __IMFPMPHostApp_FWD_DEFINED__
#define __IMFPMPHostApp_FWD_DEFINED__
typedef interface IMFPMPHostApp IMFPMPHostApp;
#endif

#ifndef __IMFPMPClientApp_FWD_DEFINED__
#define __IMFPMPClientApp_FWD_DEFINED__
typedef interface IMFPMPClientApp IMFPMPClientApp;
#endif

#ifndef __IMFMediaStreamSourceSampleRequest_FWD_DEFINED__
#define __IMFMediaStreamSourceSampleRequest_FWD_DEFINED__
typedef interface IMFMediaStreamSourceSampleRequest IMFMediaStreamSourceSampleRequest;
#endif

#ifndef __IMFTrackedSample_FWD_DEFINED__
#define __IMFTrackedSample_FWD_DEFINED__
typedef interface IMFTrackedSample IMFTrackedSample;
#endif

#ifndef __IMFProtectedEnvironmentAccess_FWD_DEFINED__
#define __IMFProtectedEnvironmentAccess_FWD_DEFINED__
typedef interface IMFProtectedEnvironmentAccess IMFProtectedEnvironmentAccess;
#endif

#ifndef __IMFSignedLibrary_FWD_DEFINED__
#define __IMFSignedLibrary_FWD_DEFINED__
typedef interface IMFSignedLibrary IMFSignedLibrary;
#endif

#ifndef __IMFSystemId_FWD_DEFINED__
#define __IMFSystemId_FWD_DEFINED__
typedef interface IMFSystemId IMFSystemId;
#endif

#include "mfobjects.h"
#include "mftransform.h"

#ifdef __cplusplus
extern "C"{
#endif 


#include <windef.h>

typedef enum MFSESSION_SETTOPOLOGY_FLAGS {
    MFSESSION_SETTOPOLOGY_IMMEDIATE = 0x1,
    MFSESSION_SETTOPOLOGY_NORESOLUTION = 0x2,
    MFSESSION_SETTOPOLOGY_CLEAR_CURRENT = 0x4
} MFSESSION_SETTOPOLOGY_FLAGS;

typedef enum MFSESSION_GETFULLTOPOLOGY_FLAGS {
    MFSESSION_GETFULLTOPOLOGY_CURRENT = 0x1
} MFSESSION_GETFULLTOPOLOGY_FLAGS;

typedef enum MFPMPSESSION_CREATION_FLAGS {
    MFPMPSESSION_UNPROTECTED_PROCESS = 0x1
} MFPMPSESSION_CREATION_FLAGS;

typedef unsigned __int64 TOPOID;

EXTERN_GUID(MF_WVC1_PROG_SINGLE_SLICE_CONTENT,0x67EC2559,0x0F2F,0x4420,0xA4,0xDD,0x2F,0x8E,0xE7,0xA5,0x73,0x8B);
EXTERN_GUID(MF_PROGRESSIVE_CODING_CONTENT,0x8F020EEA,0x1508,0x471F,0x9D,0xA6,0x50,0x7D,0x7C,0xFA,0x40,0xDB);
EXTERN_GUID(MF_NALU_LENGTH_SET,0xA7911D53,0x12A4,0x4965,0xAE,0x70,0x6E,0xAD,0xD6,0xFF,0x05,0x51);
EXTERN_GUID(MF_NALU_LENGTH_INFORMATION,0x19124E7C,0xAD4B,0x465F,0xBB,0x18,0x20,0x18,0x62,0x87,0xB6,0xAF);
EXTERN_GUID(MF_USER_DATA_PAYLOAD,0xd1d4985d,0xdc92,0x457a,0xb3,0xa0,0x65,0x1a,0x33,0xa3,0x10,0x47);
EXTERN_GUID(MF_MPEG4SINK_SPSPPS_PASSTHROUGH,0x5601a134,0x2005,0x4ad2,0xb3,0x7d,0x22,0xa6,0xc5,0x54,0xde,0xb2);
EXTERN_GUID(MF_MPEG4SINK_MOOV_BEFORE_MDAT,0xf672e3ac,0xe1e6,0x4f10,0xb5,0xec,0x5f,0x3b,0x30,0x82,0x88,0x16);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0000_v0_0_s_ifspec;

#ifndef __IMFMediaSession_INTERFACE_DEFINED__
#define __IMFMediaSession_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFMediaSession;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("90377834-21D0-4dee-8214-BA2E3E6C1127")IMFMediaSession:public IMFMediaEventGenerator
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetTopology(DWORD dwSetTopologyFlags, IMFTopology * pTopology) = 0;
    virtual HRESULT STDMETHODCALLTYPE ClearTopologies(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Start(const GUID *pguidTimeFormat, const PROPVARIANT *pvarStartPosition) = 0;
    virtual HRESULT STDMETHODCALLTYPE Pause(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Stop(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Close(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Shutdown(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetClock(IMFClock **ppClock) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSessionCapabilities(DWORD *pdwCaps) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFullTopology(DWORD dwGetFullTopologyFlags, TOPOID TopoId, IMFTopology **ppFullTopology) = 0;
};
#else
typedef struct IMFMediaSessionVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFMediaSession * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFMediaSession * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFMediaSession * This);
    HRESULT(STDMETHODCALLTYPE *GetEvent)(IMFMediaSession * This, DWORD dwFlags, IMFMediaEvent ** ppEvent);
    HRESULT(STDMETHODCALLTYPE *BeginGetEvent)(IMFMediaSession * This, IMFAsyncCallback * pCallback, IUnknown * punkState);
    HRESULT(STDMETHODCALLTYPE *EndGetEvent)(IMFMediaSession * This, IMFAsyncResult * pResult, IMFMediaEvent ** ppEvent);
    HRESULT(STDMETHODCALLTYPE *QueueEvent)(IMFMediaSession * This, MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT * pvValue);
    HRESULT(STDMETHODCALLTYPE *SetTopology)(IMFMediaSession * This, DWORD dwSetTopologyFlags, IMFTopology * pTopology);
    HRESULT(STDMETHODCALLTYPE *ClearTopologies)(IMFMediaSession * This);
    HRESULT(STDMETHODCALLTYPE *Start)(IMFMediaSession * This, const GUID * pguidTimeFormat, const PROPVARIANT * pvarStartPosition);
    HRESULT(STDMETHODCALLTYPE *Pause)(IMFMediaSession * This);
    HRESULT(STDMETHODCALLTYPE *Stop)(IMFMediaSession * This);
    HRESULT(STDMETHODCALLTYPE *Close)(IMFMediaSession * This);
    HRESULT(STDMETHODCALLTYPE *Shutdown)(IMFMediaSession * This);
    HRESULT(STDMETHODCALLTYPE *GetClock)(IMFMediaSession * This, IMFClock ** ppClock);
    HRESULT(STDMETHODCALLTYPE *GetSessionCapabilities)(IMFMediaSession * This, DWORD * pdwCaps);
    HRESULT(STDMETHODCALLTYPE *GetFullTopology)(IMFMediaSession * This, DWORD dwGetFullTopologyFlags, TOPOID TopoId, IMFTopology ** ppFullTopology);
    END_INTERFACE
} IMFMediaSessionVtbl;

interface IMFMediaSession {
    CONST_VTBL struct IMFMediaSessionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFMediaSession_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFMediaSession_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFMediaSession_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFMediaSession_GetEvent(This,dwFlags,ppEvent)  ((This)->lpVtbl->GetEvent(This,dwFlags,ppEvent))
#define IMFMediaSession_BeginGetEvent(This,pCallback,punkState)  ((This)->lpVtbl->BeginGetEvent(This,pCallback,punkState))
#define IMFMediaSession_EndGetEvent(This,pResult,ppEvent)  ((This)->lpVtbl->EndGetEvent(This,pResult,ppEvent))
#define IMFMediaSession_QueueEvent(This,met,guidExtendedType,hrStatus,pvValue)  ((This)->lpVtbl->QueueEvent(This,met,guidExtendedType,hrStatus,pvValue))
#define IMFMediaSession_SetTopology(This,dwSetTopologyFlags,pTopology)  ((This)->lpVtbl->SetTopology(This,dwSetTopologyFlags,pTopology))
#define IMFMediaSession_ClearTopologies(This)  ((This)->lpVtbl->ClearTopologies(This))
#define IMFMediaSession_Start(This,pguidTimeFormat,pvarStartPosition)  ((This)->lpVtbl->Start(This,pguidTimeFormat,pvarStartPosition))
#define IMFMediaSession_Pause(This)  ((This)->lpVtbl->Pause(This))
#define IMFMediaSession_Stop(This)  ((This)->lpVtbl->Stop(This))
#define IMFMediaSession_Close(This)  ((This)->lpVtbl->Close(This))
#define IMFMediaSession_Shutdown(This)  ((This)->lpVtbl->Shutdown(This))
#define IMFMediaSession_GetClock(This,ppClock)  ((This)->lpVtbl->GetClock(This,ppClock))
#define IMFMediaSession_GetSessionCapabilities(This,pdwCaps)  ((This)->lpVtbl->GetSessionCapabilities(This,pdwCaps))
#define IMFMediaSession_GetFullTopology(This,dwGetFullTopologyFlags,TopoId,ppFullTopology)  ((This)->lpVtbl->GetFullTopology(This,dwGetFullTopologyFlags,TopoId,ppFullTopology))
#endif

#endif

#endif

EXTERN_GUID(MF_SESSION_TOPOLOADER, 0x1e83d482, 0x1f1c, 0x4571, 0x84, 0x5, 0x88, 0xf4, 0xb2, 0x18, 0x1f, 0x71);
EXTERN_GUID(MF_SESSION_GLOBAL_TIME, 0x1e83d482, 0x1f1c, 0x4571, 0x84, 0x5, 0x88, 0xf4, 0xb2, 0x18, 0x1f, 0x72);
EXTERN_GUID(MF_SESSION_QUALITY_MANAGER, 0x1e83d482, 0x1f1c, 0x4571, 0x84, 0x5, 0x88, 0xf4, 0xb2, 0x18, 0x1f, 0x73);
EXTERN_GUID(MF_SESSION_CONTENT_PROTECTION_MANAGER, 0x1e83d482, 0x1f1c, 0x4571, 0x84, 0x5, 0x88, 0xf4, 0xb2, 0x18, 0x1f, 0x74);
EXTERN_GUID(MF_SESSION_SERVER_CONTEXT, 0xafe5b291, 0x50fa, 0x46e8, 0xb9, 0xbe, 0xc, 0xc, 0x3c, 0xe4, 0xb3, 0xa5);
EXTERN_GUID(MF_SESSION_REMOTE_SOURCE_MODE, 0xf4033ef4, 0x9bb3, 0x4378, 0x94, 0x1f, 0x85, 0xa0, 0x85, 0x6b, 0xc2, 0x44);
EXTERN_GUID(MF_SESSION_APPROX_EVENT_OCCURRENCE_TIME, 0x190e852f, 0x6238, 0x42d1, 0xb5, 0xaf, 0x69, 0xea, 0x33, 0x8e, 0xf8, 0x50);
EXTERN_GUID(MF_PMP_SERVER_CONTEXT, 0x2f00c910, 0xd2cf, 0x4278, 0x8b, 0x6a, 0xd0, 0x77, 0xfa, 0xc3, 0xa2, 0x5f);

STDAPI MFCreateMediaSession(IMFAttributes *, IMFMediaSession **);
STDAPI MFCreatePMPMediaSession(DWORD, IMFAttributes *, IMFMediaSession **, IMFActivate **);

typedef enum MF_OBJECT_TYPE {
    MF_OBJECT_MEDIASOURCE = 0,
    MF_OBJECT_BYTESTREAM = (MF_OBJECT_MEDIASOURCE + 1),
    MF_OBJECT_INVALID = (MF_OBJECT_BYTESTREAM + 1)
} MF_OBJECT_TYPE;

enum __MIDL___MIDL_itf_mfidl_0000_0001_0001 {
    MF_RESOLUTION_MEDIASOURCE = 0x1,
    MF_RESOLUTION_BYTESTREAM = 0x2,
    MF_RESOLUTION_CONTENT_DOES_NOT_HAVE_TO_MATCH_EXTENSION_OR_MIME_TYPE = 0x10,
    MF_RESOLUTION_KEEP_BYTE_STREAM_ALIVE_ON_FAIL = 0x20,
    MF_RESOLUTION_READ = 0x10000,
    MF_RESOLUTION_WRITE = 0x20000
};
typedef enum _MF_CONNECT_METHOD {
    MF_CONNECT_DIRECT = 0,
    MF_CONNECT_ALLOW_CONVERTER = 0x1,
    MF_CONNECT_ALLOW_DECODER = 0x3,
    MF_CONNECT_RESOLVE_INDEPENDENT_OUTPUTTYPES = 0x4,
    MF_CONNECT_AS_OPTIONAL = 0x10000,
    MF_CONNECT_AS_OPTIONAL_BRANCH = 0x20000
} MF_CONNECT_METHOD;

typedef enum _MF_TOPOLOGY_RESOLUTION_STATUS_FLAGS {
    MF_TOPOLOGY_RESOLUTION_SUCCEEDED = 0,
    MF_OPTIONAL_NODE_REJECTED_MEDIA_TYPE = 0x1,
    MF_OPTIONAL_NODE_REJECTED_PROTECTED_PROCESS = 0x2
} MF_TOPOLOGY_RESOLUTION_STATUS_FLAGS;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0001_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0001_v0_0_s_ifspec;

#ifndef __IMFSourceResolver_INTERFACE_DEFINED__
#define __IMFSourceResolver_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSourceResolver;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("FBE5A32D-A497-4b61-BB85-97B1A848A6E3")IMFSourceResolver:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateObjectFromURL(LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore * pProps, MF_OBJECT_TYPE * pObjectType, IUnknown ** ppObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateObjectFromByteStream(IMFByteStream *pByteStream, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore *pProps, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE BeginCreateObjectFromURL(LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore *pProps, IUnknown **ppIUnknownCancelCookie, IMFAsyncCallback *pCallback, IUnknown *punkState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndCreateObjectFromURL(IMFAsyncResult *pResult, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE BeginCreateObjectFromByteStream(IMFByteStream *pByteStream, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore *pProps, IUnknown **ppIUnknownCancelCookie, IMFAsyncCallback *pCallback, IUnknown *punkState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndCreateObjectFromByteStream(IMFAsyncResult *pResult, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE CancelObjectCreation(IUnknown *pIUnknownCancelCookie) = 0;
};
#else
typedef struct IMFSourceResolverVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface) (IMFSourceResolver *This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSourceResolver *This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSourceResolver *This);
    HRESULT(STDMETHODCALLTYPE *CreateObjectFromURL)(IMFSourceResolver *This, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore *pProps, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject);
    HRESULT(STDMETHODCALLTYPE *CreateObjectFromByteStream)(IMFSourceResolver *This, IMFByteStream *pByteStream, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore *pProps, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject);
    HRESULT(STDMETHODCALLTYPE *BeginCreateObjectFromURL)(IMFSourceResolver *This, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore *pProps, IUnknown **ppIUnknownCancelCookie, IMFAsyncCallback *pCallback, IUnknown *punkState);
    HRESULT(STDMETHODCALLTYPE *EndCreateObjectFromURL)(IMFSourceResolver *This, IMFAsyncResult *pResult, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject);
    HRESULT(STDMETHODCALLTYPE *BeginCreateObjectFromByteStream)(IMFSourceResolver *This, IMFByteStream *pByteStream, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore *pProps, IUnknown **ppIUnknownCancelCookie, IMFAsyncCallback *pCallback, IUnknown *punkState);
    HRESULT(STDMETHODCALLTYPE *EndCreateObjectFromByteStream)(IMFSourceResolver *This, IMFAsyncResult *pResult, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject);
    HRESULT(STDMETHODCALLTYPE *CancelObjectCreation)(IMFSourceResolver *This, IUnknown *pIUnknownCancelCookie);
    END_INTERFACE
} IMFSourceResolverVtbl;

interface IMFSourceResolver {
    CONST_VTBL struct IMFSourceResolverVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSourceResolver_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSourceResolver_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSourceResolver_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFSourceResolver_CreateObjectFromURL(This,pwszURL,dwFlags,pProps,pObjectType,ppObject)  ((This)->lpVtbl->CreateObjectFromURL(This,pwszURL,dwFlags,pProps,pObjectType,ppObject))
#define IMFSourceResolver_CreateObjectFromByteStream(This,pByteStream,pwszURL,dwFlags,pProps,pObjectType,ppObject)  ((This)->lpVtbl->CreateObjectFromByteStream(This,pByteStream,pwszURL,dwFlags,pProps,pObjectType,ppObject))
#define IMFSourceResolver_BeginCreateObjectFromURL(This,pwszURL,dwFlags,pProps,ppIUnknownCancelCookie,pCallback,punkState)  ((This)->lpVtbl->BeginCreateObjectFromURL(This,pwszURL,dwFlags,pProps,ppIUnknownCancelCookie,pCallback,punkState))
#define IMFSourceResolver_EndCreateObjectFromURL(This,pResult,pObjectType,ppObject)  ((This)->lpVtbl->EndCreateObjectFromURL(This,pResult,pObjectType,ppObject))
#define IMFSourceResolver_BeginCreateObjectFromByteStream(This,pByteStream,pwszURL,dwFlags,pProps,ppIUnknownCancelCookie,pCallback,punkState)  ((This)->lpVtbl->BeginCreateObjectFromByteStream(This,pByteStream,pwszURL,dwFlags,pProps,ppIUnknownCancelCookie, pCallback, punkState))
#define IMFSourceResolver_EndCreateObjectFromByteStream(This,pResult,pObjectType,ppObject)  ((This)->lpVtbl->EndCreateObjectFromByteStream(This,pResult,pObjectType,ppObject))
#define IMFSourceResolver_CancelObjectCreation(This,pIUnknownCancelCookie)  ((This)->lpVtbl->CancelObjectCreation(This,pIUnknownCancelCookie))
#endif

#endif

HRESULT STDMETHODCALLTYPE IMFSourceResolver_RemoteBeginCreateObjectFromURL_Proxy(IMFSourceResolver *This, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore *pProps, IMFRemoteAsyncCallback *pCallback);
void __RPC_STUB IMFSourceResolver_RemoteBeginCreateObjectFromURL_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE IMFSourceResolver_RemoteEndCreateObjectFromURL_Proxy(IMFSourceResolver *This, IUnknown *pResult, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject);
void __RPC_STUB IMFSourceResolver_RemoteEndCreateObjectFromURL_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE IMFSourceResolver_RemoteBeginCreateObjectFromByteStream_Proxy(IMFSourceResolver *This, IMFByteStream *pByteStream, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore *pProps, IMFRemoteAsyncCallback *pCallback);
void __RPC_STUB IMFSourceResolver_RemoteBeginCreateObjectFromByteStream_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE IMFSourceResolver_RemoteEndCreateObjectFromByteStream_Proxy(IMFSourceResolver *This, IUnknown *pResult, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject);
void __RPC_STUB IMFSourceResolver_RemoteEndCreateObjectFromByteStream_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
#endif

STDAPI MFCreateSourceResolver(IMFSourceResolver **ppISourceResolver);
STDAPI CreatePropertyStore(IPropertyStore **ppStore);
STDAPI MFGetSupportedSchemes(PROPVARIANT *pPropVarSchemeArray);
STDAPI MFGetSupportedMimeTypes(PROPVARIANT *pPropVarMimeTypeArray);

EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_SourceOpenMonitor = { { 0x074d4637, 0xb5ae, 0x465d, 0xaf, 0x17, 0x1a, 0x53, 0x8d, 0x28, 0x59, 0xdd }, 0x02 };
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_ASFMediaSource_ApproxSeek = { { 0xb4cd270f, 0x244d, 0x4969, 0xbb, 0x92, 0x3f, 0x0f, 0xb8, 0x31, 0x6f, 0x10 }, 0x01 };
#if (WINVER >= _WIN32_WINNT_WIN7)
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_ASFMediaSource_IterativeSeekIfNoIndex = { { 0x170b65dc, 0x4a4e, 0x407a, 0xac, 0x22, 0x57, 0x7f, 0x50, 0xe4, 0xa3, 0x7c }, 0x01 };
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_ASFMediaSource_IterativeSeek_Max_Count = { { 0x170b65dc, 0x4a4e, 0x407a, 0xac, 0x22, 0x57, 0x7f, 0x50, 0xe4, 0xa3, 0x7c }, 0x02 };
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_ASFMediaSource_IterativeSeek_Tolerance_In_MilliSecond = { { 0x170b65dc, 0x4a4e, 0x407a, 0xac, 0x22, 0x57, 0x7f, 0x50, 0xe4, 0xa3, 0x7c }, 0x03 };
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_Content_DLNA_Profile_ID = { { 0xcfa31b45, 0x525d, 0x4998, 0xbb, 0x44, 0x3f, 0x7d, 0x81, 0x54, 0x2f, 0xa4 }, 0x01 };
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_MediaSource_DisableReadAhead = { { 0x26366c14, 0xc5bf, 0x4c76, 0x88, 0x7b, 0x9f, 0x17, 0x54, 0xdb, 0x5f, 0x9 }, 0x01 };
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_SBESourceMode = { { 0x3fae10bb, 0xf859, 0x4192, 0xb5, 0x62, 0x18, 0x68, 0xd3, 0xda, 0x3a, 0x02}, 0x01 }; 
#endif /* WINVER >= _WIN32_WINNT_WIN7 */
#if (WINVER >= _WIN32_WINNT_WIN8) 
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_PMP_Creation_Callback = { { 0x28bb4de2, 0x26a2, 0x4870, 0xb7, 0x20, 0xd2, 0x6b, 0xbe, 0xb1, 0x49, 0x42}, 0x01 }; 
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_HTTP_ByteStream_Enable_Urlmon = { { 0xeda8afdf, 0xc171, 0x417f, 0x8d, 0x17, 0x2e, 0x09, 0x18, 0x30, 0x32, 0x92}, 0x01 }; 
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_HTTP_ByteStream_Urlmon_Bind_Flags = { { 0xeda8afdf, 0xc171, 0x417f, 0x8d, 0x17, 0x2e, 0x09, 0x18, 0x30, 0x32, 0x92}, 0x02 }; 
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_HTTP_ByteStream_Urlmon_Security_Id = { { 0xeda8afdf, 0xc171, 0x417f, 0x8d, 0x17, 0x2e, 0x09, 0x18, 0x30, 0x32, 0x92}, 0x03 }; 
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_HTTP_ByteStream_Urlmon_Window = { { 0xeda8afdf, 0xc171, 0x417f, 0x8d, 0x17, 0x2e, 0x09, 0x18, 0x30, 0x32, 0x92}, 0x04 }; 
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_HTTP_ByteStream_Urlmon_Callback_QueryService = { { 0xeda8afdf, 0xc171, 0x417f, 0x8d, 0x17, 0x2e, 0x09, 0x18, 0x30, 0x32, 0x92}, 0x05 }; 
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_MediaProtectionSystemId =  { { 0x636b271d, 0xddc7, 0x49e9, 0xa6, 0xc6, 0x47, 0x38, 0x59, 0x62, 0xe5, 0xbd}, 0x01 }; 
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_MediaProtectionSystemContext =  { { 0x636b271d, 0xddc7, 0x49e9, 0xa6, 0xc6, 0x47, 0x38, 0x59, 0x62, 0xe5, 0xbd}, 0x02 }; 
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_MediaProtectionSystemIdMapping =  { { 0x636b271d, 0xddc7, 0x49e9, 0xa6, 0xc6, 0x47, 0x38, 0x59, 0x62, 0xe5, 0xbd}, 0x03 }; 
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_MediaProtectionContainerGuid =  { { 0x42af3d7c, 0xcf, 0x4a0f, 0x81, 0xf0, 0xad, 0xf5, 0x24, 0xa5, 0xa5, 0xb5}, 0x1 }; 
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_MediaProtectionSystemContextsPerTrack =  { { 0x4454b092, 0xd3da, 0x49b0, 0x84, 0x52, 0x68, 0x50, 0xc7, 0xdb, 0x76, 0x4d }, 0x03 }; 
EXTERN_C const DECLSPEC_SELECTANY PROPERTYKEY MFPKEY_HTTP_ByteStream_Download_Mode = { { 0x817f11b7, 0xa982, 0x46ec, 0xa4, 0x49, 0xef, 0x58, 0xae, 0xd5, 0x3c, 0xa8 }, 0x01 }; 
#endif /* WINVER >= _WIN32_WINNT_WIN8 */

typedef enum _MFMEDIASOURCE_CHARACTERISTICS {
    MFMEDIASOURCE_IS_LIVE = 0x1,
    MFMEDIASOURCE_CAN_SEEK = 0x2,
    MFMEDIASOURCE_CAN_PAUSE = 0x4,
    MFMEDIASOURCE_HAS_SLOW_SEEK = 0x8,
    MFMEDIASOURCE_HAS_MULTIPLE_PRESENTATIONS = 0x10,
    MFMEDIASOURCE_CAN_SKIPFORWARD = 0x20,
    MFMEDIASOURCE_CAN_SKIPBACKWARD = 0x40,
    MFMEDIASOURCE_DOES_NOT_USE_NETWORK = 0x80,
} MFMEDIASOURCE_CHARACTERISTICS;

#if (WINVER >= _WIN32_WINNT_WIN7)
EXTERN_GUID(MF_TIME_FORMAT_ENTRY_RELATIVE, 0x4399f178, 0x46d3, 0x4504, 0xaf, 0xda, 0x20, 0xd3, 0x2e, 0x9b, 0xa3, 0x60);
#endif /* WINVER >= _WIN32_WINNT_WIN7 */

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0002_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0002_v0_0_s_ifspec;

#ifndef __IMFMediaSource_INTERFACE_DEFINED__
#define __IMFMediaSource_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFMediaSource;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("279a808d-aec7-40c8-9c6b-a6b492c78a66")IMFMediaSource:public IMFMediaEventGenerator
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCharacteristics(DWORD * pdwCharacteristics) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreatePresentationDescriptor(IMFPresentationDescriptor **ppPresentationDescriptor) = 0;
    virtual HRESULT STDMETHODCALLTYPE Start(IMFPresentationDescriptor *pPresentationDescriptor, const GUID *pguidTimeFormat, const PROPVARIANT *pvarStartPosition) = 0;
    virtual HRESULT STDMETHODCALLTYPE Stop(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Pause(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Shutdown(void) = 0;
};
#else
typedef struct IMFMediaSourceVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFMediaSource * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFMediaSource * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFMediaSource * This);
    HRESULT(STDMETHODCALLTYPE *GetEvent)(IMFMediaSource * This, DWORD dwFlags, IMFMediaEvent ** ppEvent);
    HRESULT(STDMETHODCALLTYPE *BeginGetEvent)(IMFMediaSource * This, IMFAsyncCallback * pCallback, IUnknown * punkState);
    HRESULT(STDMETHODCALLTYPE *EndGetEvent)(IMFMediaSource * This, IMFAsyncResult * pResult, IMFMediaEvent ** ppEvent);
    HRESULT(STDMETHODCALLTYPE *QueueEvent)(IMFMediaSource * This, MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT * pvValue);
    HRESULT(STDMETHODCALLTYPE *GetCharacteristics)(IMFMediaSource * This, DWORD * pdwCharacteristics);
    HRESULT(STDMETHODCALLTYPE *CreatePresentationDescriptor)(IMFMediaSource * This, IMFPresentationDescriptor ** ppPresentationDescriptor);
    HRESULT(STDMETHODCALLTYPE *Start)(IMFMediaSource * This, IMFPresentationDescriptor * pPresentationDescriptor, const GUID * pguidTimeFormat, const PROPVARIANT * pvarStartPosition);
    HRESULT(STDMETHODCALLTYPE *Stop)(IMFMediaSource * This);
    HRESULT(STDMETHODCALLTYPE *Pause)(IMFMediaSource * This);
    HRESULT(STDMETHODCALLTYPE *Shutdown)(IMFMediaSource * This);
    END_INTERFACE
} IMFMediaSourceVtbl;

interface IMFMediaSource {
    CONST_VTBL struct IMFMediaSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFMediaSource_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFMediaSource_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFMediaSource_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFMediaSource_GetEvent(This,dwFlags,ppEvent)  ((This)->lpVtbl->GetEvent(This,dwFlags,ppEvent))
#define IMFMediaSource_BeginGetEvent(This,pCallback,punkState)  ((This)->lpVtbl->BeginGetEvent(This,pCallback,punkState))
#define IMFMediaSource_EndGetEvent(This,pResult,ppEvent)  ((This)->lpVtbl->EndGetEvent(This,pResult,ppEvent))
#define IMFMediaSource_QueueEvent(This,met,guidExtendedType,hrStatus,pvValue)  ((This)->lpVtbl->QueueEvent(This,met,guidExtendedType,hrStatus,pvValue))
#define IMFMediaSource_GetCharacteristics(This,pdwCharacteristics)  ((This)->lpVtbl->GetCharacteristics(This,pdwCharacteristics))
#define IMFMediaSource_CreatePresentationDescriptor(This,ppPresentationDescriptor)  ((This)->lpVtbl->CreatePresentationDescriptor(This,ppPresentationDescriptor))
#define IMFMediaSource_Start(This,pPresentationDescriptor,pguidTimeFormat,pvarStartPosition)  ((This)->lpVtbl->Start(This,pPresentationDescriptor,pguidTimeFormat,pvarStartPosition))
#define IMFMediaSource_Stop(This)  ((This)->lpVtbl->Stop(This))
#define IMFMediaSource_Pause(This)  ((This)->lpVtbl->Pause(This))
#define IMFMediaSource_Shutdown(This)  ((This)->lpVtbl->Shutdown(This))
#endif

#endif

HRESULT STDMETHODCALLTYPE IMFMediaSource_RemoteCreatePresentationDescriptor_Proxy(IMFMediaSource *This, DWORD *pcbPD, BYTE **pbPD, IMFPresentationDescriptor **ppRemotePD);
void __RPC_STUB IMFMediaSource_RemoteCreatePresentationDescriptor_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
#endif

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0003_v0_0_s_ifspec;

#ifndef __IMFMediaSourceEx_INTERFACE_DEFINED__
#define __IMFMediaSourceEx_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFMediaSourceEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3C9B2EB9-86D5-4514-A394-F56664F9F0D8")IMFMediaSourceEx:public IMFMediaSource
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetSourceAttributes(IMFAttributes **) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamAttributes(DWORD, IMFAttributes **) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetD3DManager(IUnknown *) = 0;
};
#else /* C style interface */
typedef struct IMFMediaSourceExVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IMFMediaSourceEx *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IMFMediaSourceEx *);
    ULONG (STDMETHODCALLTYPE *Release)(IMFMediaSourceEx *);
    HRESULT (STDMETHODCALLTYPE *GetEvent)(IMFMediaSourceEx *, DWORD, IMFMediaEvent **);
    HRESULT (STDMETHODCALLTYPE *BeginGetEvent)(IMFMediaSourceEx *, IMFAsyncCallback *, IUnknown *);
    HRESULT (STDMETHODCALLTYPE *EndGetEvent)(IMFMediaSourceEx *, IMFAsyncResult *, IMFMediaEvent **);
    HRESULT (STDMETHODCALLTYPE *QueueEvent)(IMFMediaSourceEx *, MediaEventType, REFGUID, HRESULT, const PROPVARIANT *);
    HRESULT (STDMETHODCALLTYPE *GetCharacteristics)(IMFMediaSourceEx *, DWORD *);
    HRESULT (STDMETHODCALLTYPE *CreatePresentationDescriptor)(IMFMediaSourceEx *, IMFPresentationDescriptor **);
    HRESULT (STDMETHODCALLTYPE *Start)(IMFMediaSourceEx *, IMFPresentationDescriptor *, const GUID *, const PROPVARIANT *);
    HRESULT (STDMETHODCALLTYPE *Stop)(IMFMediaSourceEx *);
    HRESULT (STDMETHODCALLTYPE *Pause)(IMFMediaSourceEx *);
    HRESULT (STDMETHODCALLTYPE *Shutdown)(IMFMediaSourceEx *);
    HRESULT (STDMETHODCALLTYPE *GetSourceAttributes)(IMFMediaSourceEx *, IMFAttributes **);
    HRESULT (STDMETHODCALLTYPE *GetStreamAttributes)(IMFMediaSourceEx *, DWORD, IMFAttributes **);
    HRESULT (STDMETHODCALLTYPE *SetD3DManager)(IMFMediaSourceEx *, IUnknown *);
    END_INTERFACE
} IMFMediaSourceExVtbl;

interface IMFMediaSourceEx {
    CONST_VTBL struct IMFMediaSourceExVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFMediaSourceEx_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFMediaSourceEx_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFMediaSourceEx_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFMediaSourceEx_GetEvent(This,dwFlags,ppEvent)  ((This)->lpVtbl->GetEvent(This,dwFlags,ppEvent)) 
#define IMFMediaSourceEx_BeginGetEvent(This,pCallback,punkState)  ((This)->lpVtbl->BeginGetEvent(This,pCallback,punkState)) 
#define IMFMediaSourceEx_EndGetEvent(This,pResult,ppEvent)  ((This)->lpVtbl->EndGetEvent(This,pResult,ppEvent)) 
#define IMFMediaSourceEx_QueueEvent(This,met,guidExtendedType,hrStatus,pvValue)  ((This)->lpVtbl->QueueEvent(This,met,guidExtendedType,hrStatus,pvValue)) 
#define IMFMediaSourceEx_GetCharacteristics(This,pdwCharacteristics)  ((This)->lpVtbl->GetCharacteristics(This,pdwCharacteristics)) 
#define IMFMediaSourceEx_CreatePresentationDescriptor(This,ppPresentationDescriptor)  ((This)->lpVtbl->CreatePresentationDescriptor(This,ppPresentationDescriptor)) 
#define IMFMediaSourceEx_Start(This,pPresentationDescriptor,pguidTimeFormat,pvarStartPosition)  ((This)->lpVtbl->Start(This,pPresentationDescriptor,pguidTimeFormat,pvarStartPosition)) 
#define IMFMediaSourceEx_Stop(This)  ((This)->lpVtbl->Stop(This)) 
#define IMFMediaSourceEx_Pause(This)  ((This)->lpVtbl->Pause(This)) 
#define IMFMediaSourceEx_Shutdown(This)  ((This)->lpVtbl->Shutdown(This)) 
#define IMFMediaSourceEx_GetSourceAttributes(This,ppAttributes)  ((This)->lpVtbl->GetSourceAttributes(This,ppAttributes)) 
#define IMFMediaSourceEx_GetStreamAttributes(This,dwStreamIdentifier,ppAttributes)  ((This)->lpVtbl->GetStreamAttributes(This,dwStreamIdentifier,ppAttributes)) 
#define IMFMediaSourceEx_SetD3DManager(This,pManager)  ((This)->lpVtbl->SetD3DManager(This,pManager)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFMediaSourceEx_INTERFACE_DEFINED__ */

EXTERN_GUID(MF_SOURCE_STREAM_SUPPORTS_HW_CONNECTION,0xa38253aa,0x6314,0x42fd,0xa3,0xce,0xbb,0x27,0xb6,0x85,0x99,0x46);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0004_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0004_v0_0_s_ifspec;

#ifndef __IMFMediaStream_INTERFACE_DEFINED__
#define __IMFMediaStream_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFMediaStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("D182108F-4EC6-443f-AA42-A71106EC825F")IMFMediaStream:public IMFMediaEventGenerator
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetMediaSource(IMFMediaSource ** ppMediaSource) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamDescriptor(IMFStreamDescriptor **ppStreamDescriptor) = 0;
    virtual HRESULT STDMETHODCALLTYPE RequestSample(IUnknown *pToken) = 0;
};
#else
typedef struct IMFMediaStreamVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFMediaStream * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFMediaStream * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFMediaStream * This);
    HRESULT(STDMETHODCALLTYPE *GetEvent)(IMFMediaStream * This, DWORD dwFlags, IMFMediaEvent ** ppEvent);
    HRESULT(STDMETHODCALLTYPE *BeginGetEvent)(IMFMediaStream * This, IMFAsyncCallback * pCallback, IUnknown * punkState);
    HRESULT(STDMETHODCALLTYPE *EndGetEvent)(IMFMediaStream * This, IMFAsyncResult * pResult, IMFMediaEvent ** ppEvent);
    HRESULT(STDMETHODCALLTYPE *QueueEvent)(IMFMediaStream * This, MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT * pvValue);
    HRESULT(STDMETHODCALLTYPE *GetMediaSource)(IMFMediaStream * This, IMFMediaSource ** ppMediaSource);
    HRESULT(STDMETHODCALLTYPE *GetStreamDescriptor)(IMFMediaStream * This, IMFStreamDescriptor ** ppStreamDescriptor);
    HRESULT(STDMETHODCALLTYPE *RequestSample)(IMFMediaStream * This, IUnknown * pToken);
    END_INTERFACE
} IMFMediaStreamVtbl;

interface IMFMediaStream {
    CONST_VTBL struct IMFMediaStreamVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFMediaStream_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFMediaStream_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFMediaStream_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFMediaStream_GetEvent(This,dwFlags,ppEvent)  ((This)->lpVtbl->GetEvent(This,dwFlags,ppEvent))
#define IMFMediaStream_BeginGetEvent(This,pCallback,punkState)  ((This)->lpVtbl->BeginGetEvent(This,pCallback,punkState))
#define IMFMediaStream_EndGetEvent(This,pResult,ppEvent)  ((This)->lpVtbl->EndGetEvent(This,pResult,ppEvent))
#define IMFMediaStream_QueueEvent(This,met,guidExtendedType,hrStatus,pvValue)  ((This)->lpVtbl->QueueEvent(This,met,guidExtendedType,hrStatus,pvValue))
#define IMFMediaStream_GetMediaSource(This,ppMediaSource)  ((This)->lpVtbl->GetMediaSource(This,ppMediaSource))
#define IMFMediaStream_GetStreamDescriptor(This,ppStreamDescriptor)  ((This)->lpVtbl->GetStreamDescriptor(This,ppStreamDescriptor))
#define IMFMediaStream_RequestSample(This,pToken)  ((This)->lpVtbl->RequestSample(This,pToken))
#endif

#endif

HRESULT STDMETHODCALLTYPE IMFMediaStream_RemoteRequestSample_Proxy(IMFMediaStream *This);
void __RPC_STUB IMFMediaStream_RemoteRequestSample_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);

#endif

#if (WINVER >= _WIN32_WINNT_WIN8) 
EXTERN_GUID(MF_STREAM_SINK_SUPPORTS_HW_CONNECTION,0x9b465cbf,0x597,0x4f9e,0x9f,0x3c,0xb9,0x7e,0xee,0xf9,0x3, 0x59);
EXTERN_GUID(MF_STREAM_SINK_SUPPORTS_ROTATION,0xb3e96280,0xbd05,0x41a5,0x97,0xad,0x8a,0x7f,0xee,0x24,0xb9,0x12);
#endif /* WINVER >= _WIN32_WINNT_WIN8 */

#define MEDIASINK_FIXED_STREAMS  0x00000001
#define MEDIASINK_CANNOT_MATCH_CLOCK  0x00000002
#define MEDIASINK_RATELESS  0x00000004
#define MEDIASINK_CLOCK_REQUIRED  0x00000008
#define MEDIASINK_CAN_PREROLL  0x00000010
#define MEDIASINK_REQUIRE_REFERENCE_MEDIATYPE  0x00000020

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0004_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0004_v0_0_s_ifspec;

#ifndef __IMFMediaSink_INTERFACE_DEFINED__
#define __IMFMediaSink_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFMediaSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("6ef2a660-47c0-4666-b13d-cbb717f2fa2c")IMFMediaSink:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCharacteristics(DWORD * pdwCharacteristics) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddStreamSink(DWORD dwStreamSinkIdentifier, IMFMediaType *pMediaType, IMFStreamSink **ppStreamSink) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveStreamSink(DWORD dwStreamSinkIdentifier) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamSinkCount(DWORD *pcStreamSinkCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamSinkByIndex(DWORD dwIndex, IMFStreamSink **ppStreamSink) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamSinkById(DWORD dwStreamSinkIdentifier, IMFStreamSink **ppStreamSink) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPresentationClock(IMFPresentationClock *pPresentationClock) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPresentationClock(IMFPresentationClock **ppPresentationClock) = 0;
    virtual HRESULT STDMETHODCALLTYPE Shutdown(void) = 0;
};
#else
typedef struct IMFMediaSinkVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFMediaSink * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFMediaSink * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFMediaSink * This);
    HRESULT(STDMETHODCALLTYPE *GetCharacteristics)(IMFMediaSink * This, DWORD * pdwCharacteristics);
    HRESULT(STDMETHODCALLTYPE *AddStreamSink)(IMFMediaSink * This, DWORD dwStreamSinkIdentifier, IMFMediaType * pMediaType, IMFStreamSink ** ppStreamSink);
    HRESULT(STDMETHODCALLTYPE *RemoveStreamSink)(IMFMediaSink * This, DWORD dwStreamSinkIdentifier);
    HRESULT(STDMETHODCALLTYPE *GetStreamSinkCount)(IMFMediaSink * This, DWORD * pcStreamSinkCount);
    HRESULT(STDMETHODCALLTYPE *GetStreamSinkByIndex)(IMFMediaSink * This, DWORD dwIndex, IMFStreamSink ** ppStreamSink);
    HRESULT(STDMETHODCALLTYPE *GetStreamSinkById)(IMFMediaSink * This, DWORD dwStreamSinkIdentifier, IMFStreamSink ** ppStreamSink);
    HRESULT(STDMETHODCALLTYPE *SetPresentationClock)(IMFMediaSink * This, IMFPresentationClock * pPresentationClock);
    HRESULT(STDMETHODCALLTYPE *GetPresentationClock)(IMFMediaSink * This, IMFPresentationClock ** ppPresentationClock);
    HRESULT(STDMETHODCALLTYPE *Shutdown)(IMFMediaSink * This);
    END_INTERFACE
} IMFMediaSinkVtbl;

interface IMFMediaSink {
    CONST_VTBL struct IMFMediaSinkVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFMediaSink_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFMediaSink_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFMediaSink_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFMediaSink_GetCharacteristics(This,pdwCharacteristics)  ((This)->lpVtbl->GetCharacteristics(This,pdwCharacteristics))
#define IMFMediaSink_AddStreamSink(This,dwStreamSinkIdentifier,pMediaType,ppStreamSink)  ((This)->lpVtbl->AddStreamSink(This,dwStreamSinkIdentifier,pMediaType,ppStreamSink))
#define IMFMediaSink_RemoveStreamSink(This,dwStreamSinkIdentifier)  ((This)->lpVtbl->RemoveStreamSink(This,dwStreamSinkIdentifier))
#define IMFMediaSink_GetStreamSinkCount(This,pcStreamSinkCount)  ((This)->lpVtbl->GetStreamSinkCount(This,pcStreamSinkCount))
#define IMFMediaSink_GetStreamSinkByIndex(This,dwIndex,ppStreamSink)  ((This)->lpVtbl->GetStreamSinkByIndex(This,dwIndex,ppStreamSink))
#define IMFMediaSink_GetStreamSinkById(This,dwStreamSinkIdentifier,ppStreamSink)  ((This)->lpVtbl->GetStreamSinkById(This,dwStreamSinkIdentifier,ppStreamSink))
#define IMFMediaSink_SetPresentationClock(This,pPresentationClock)  ((This)->lpVtbl->SetPresentationClock(This,pPresentationClock))
#define IMFMediaSink_GetPresentationClock(This,ppPresentationClock)  ((This)->lpVtbl->GetPresentationClock(This,ppPresentationClock))
#define IMFMediaSink_Shutdown(This)  ((This)->lpVtbl->Shutdown(This))
#endif

#endif

#endif

typedef enum _MFSTREAMSINK_MARKER_TYPE {
    MFSTREAMSINK_MARKER_DEFAULT = 0,
    MFSTREAMSINK_MARKER_ENDOFSEGMENT = (MFSTREAMSINK_MARKER_DEFAULT + 1),
    MFSTREAMSINK_MARKER_TICK = (MFSTREAMSINK_MARKER_ENDOFSEGMENT + 1),
    MFSTREAMSINK_MARKER_EVENT = (MFSTREAMSINK_MARKER_TICK + 1)
} MFSTREAMSINK_MARKER_TYPE;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0005_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0005_v0_0_s_ifspec;

#ifndef __IMFStreamSink_INTERFACE_DEFINED__
#define __IMFStreamSink_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFStreamSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0A97B3CF-8E7C-4a3d-8F8C-0C843DC247FB")IMFStreamSink:public IMFMediaEventGenerator
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetMediaSink(IMFMediaSink ** ppMediaSink) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIdentifier(DWORD *pdwIdentifier) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMediaTypeHandler(IMFMediaTypeHandler **ppHandler) = 0;
    virtual HRESULT STDMETHODCALLTYPE ProcessSample(IMFSample *pSample) = 0;
    virtual HRESULT STDMETHODCALLTYPE PlaceMarker(MFSTREAMSINK_MARKER_TYPE eMarkerType, const PROPVARIANT *pvarMarkerValue, const PROPVARIANT *pvarContextValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE Flush(void) = 0;
};
#else
typedef struct IMFStreamSinkVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFStreamSink * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFStreamSink * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFStreamSink * This);
    HRESULT(STDMETHODCALLTYPE *GetEvent)(IMFStreamSink * This, DWORD dwFlags, IMFMediaEvent ** ppEvent);
    HRESULT(STDMETHODCALLTYPE *BeginGetEvent)(IMFStreamSink * This, IMFAsyncCallback * pCallback, IUnknown * punkState);
    HRESULT(STDMETHODCALLTYPE *EndGetEvent)(IMFStreamSink * This, IMFAsyncResult * pResult, IMFMediaEvent ** ppEvent);
    HRESULT(STDMETHODCALLTYPE *QueueEvent)(IMFStreamSink * This, MediaEventType met, REFGUID guidExtendedType, HRESULT hrStatus, const PROPVARIANT * pvValue);
    HRESULT(STDMETHODCALLTYPE *GetMediaSink)(IMFStreamSink * This, IMFMediaSink ** ppMediaSink);
    HRESULT(STDMETHODCALLTYPE *GetIdentifier)(IMFStreamSink * This, DWORD * pdwIdentifier);
    HRESULT(STDMETHODCALLTYPE *GetMediaTypeHandler)(IMFStreamSink * This, IMFMediaTypeHandler ** ppHandler);
    HRESULT(STDMETHODCALLTYPE *ProcessSample)(IMFStreamSink * This, IMFSample * pSample);
    HRESULT(STDMETHODCALLTYPE *PlaceMarker)(IMFStreamSink * This, MFSTREAMSINK_MARKER_TYPE eMarkerType, const PROPVARIANT * pvarMarkerValue, const PROPVARIANT * pvarContextValue);
    HRESULT(STDMETHODCALLTYPE *Flush)(IMFStreamSink * This);
    END_INTERFACE
} IMFStreamSinkVtbl;

interface IMFStreamSink {
    CONST_VTBL struct IMFStreamSinkVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFStreamSink_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFStreamSink_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFStreamSink_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFStreamSink_GetEvent(This,dwFlags,ppEvent)  ((This)->lpVtbl->GetEvent(This,dwFlags,ppEvent))
#define IMFStreamSink_BeginGetEvent(This,pCallback,punkState)  ((This)->lpVtbl->BeginGetEvent(This,pCallback,punkState))
#define IMFStreamSink_EndGetEvent(This,pResult,ppEvent)  ((This)->lpVtbl->EndGetEvent(This,pResult,ppEvent))
#define IMFStreamSink_QueueEvent(This,met,guidExtendedType,hrStatus,pvValue)  ((This)->lpVtbl->QueueEvent(This,met,guidExtendedType,hrStatus,pvValue))
#define IMFStreamSink_GetMediaSink(This,ppMediaSink)  ((This)->lpVtbl->GetMediaSink(This,ppMediaSink))
#define IMFStreamSink_GetIdentifier(This,pdwIdentifier)  ((This)->lpVtbl->GetIdentifier(This,pdwIdentifier))
#define IMFStreamSink_GetMediaTypeHandler(This,ppHandler)  ((This)->lpVtbl->GetMediaTypeHandler(This,ppHandler))
#define IMFStreamSink_ProcessSample(This,pSample)  ((This)->lpVtbl->ProcessSample(This,pSample))
#define IMFStreamSink_PlaceMarker(This,eMarkerType,pvarMarkerValue,pvarContextValue)  ((This)->lpVtbl->PlaceMarker(This,eMarkerType,pvarMarkerValue,pvarContextValue))
#define IMFStreamSink_Flush(This)  ((This)->lpVtbl->Flush(This))
#endif

#endif

#endif

#ifndef __IMFVideoSampleAllocator_INTERFACE_DEFINED__
#define __IMFVideoSampleAllocator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFVideoSampleAllocator;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("86cbc910-e533-4751-8e3b-f19b5b806a03")IMFVideoSampleAllocator:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetDirectXManager(IUnknown * pManager) = 0;
    virtual HRESULT STDMETHODCALLTYPE UninitializeSampleAllocator(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitializeSampleAllocator(DWORD cRequestedFrames, IMFMediaType *pMediaType) = 0;
    virtual HRESULT STDMETHODCALLTYPE AllocateSample(IMFSample **ppSample) = 0;
};
#else
typedef struct IMFVideoSampleAllocatorVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFVideoSampleAllocator * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFVideoSampleAllocator * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFVideoSampleAllocator * This);
    HRESULT(STDMETHODCALLTYPE *SetDirectXManager)(IMFVideoSampleAllocator * This, IUnknown * pManager);
    HRESULT(STDMETHODCALLTYPE *UninitializeSampleAllocator)(IMFVideoSampleAllocator * This);
    HRESULT(STDMETHODCALLTYPE *InitializeSampleAllocator)(IMFVideoSampleAllocator * This, DWORD cRequestedFrames, IMFMediaType * pMediaType);
    HRESULT(STDMETHODCALLTYPE *AllocateSample)(IMFVideoSampleAllocator * This, IMFSample ** ppSample);
    END_INTERFACE
} IMFVideoSampleAllocatorVtbl;

interface IMFVideoSampleAllocator {
    CONST_VTBL struct IMFVideoSampleAllocatorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFVideoSampleAllocator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFVideoSampleAllocator_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFVideoSampleAllocator_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFVideoSampleAllocator_SetDirectXManager(This,pManager)  ((This)->lpVtbl->SetDirectXManager(This,pManager))
#define IMFVideoSampleAllocator_UninitializeSampleAllocator(This)  ((This)->lpVtbl->UninitializeSampleAllocator(This))
#define IMFVideoSampleAllocator_InitializeSampleAllocator(This,cRequestedFrames,pMediaType)  ((This)->lpVtbl->InitializeSampleAllocator(This,cRequestedFrames,pMediaType))
#define IMFVideoSampleAllocator_AllocateSample(This,ppSample)  ((This)->lpVtbl->AllocateSample(This,ppSample))
#endif

#endif

#endif

#if (WINVER >= _WIN32_WINNT_WIN7)

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0007_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0007_v0_0_s_ifspec;

#ifndef __IMFVideoSampleAllocatorNotify_INTERFACE_DEFINED__
#define __IMFVideoSampleAllocatorNotify_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFVideoSampleAllocatorNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A792CDBE-C374-4e89-8335-278E7B9956A4")IMFVideoSampleAllocatorNotify:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE NotifyRelease(void) = 0;
};
#else
typedef struct IMFVideoSampleAllocatorNotifyVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFVideoSampleAllocatorNotify * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFVideoSampleAllocatorNotify * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFVideoSampleAllocatorNotify * This);
    HRESULT(STDMETHODCALLTYPE *NotifyRelease)(IMFVideoSampleAllocatorNotify * This);
    END_INTERFACE
} IMFVideoSampleAllocatorNotifyVtbl;

interface IMFVideoSampleAllocatorNotify {
    CONST_VTBL struct IMFVideoSampleAllocatorNotifyVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFVideoSampleAllocatorNotify_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFVideoSampleAllocatorNotify_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFVideoSampleAllocatorNotify_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFVideoSampleAllocatorNotify_NotifyRelease(This)  ((This)->lpVtbl->NotifyRelease(This))
#endif

#endif

#endif

#ifndef __IMFVideoSampleAllocatorCallback_INTERFACE_DEFINED__
#define __IMFVideoSampleAllocatorCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFVideoSampleAllocatorCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("992388B4-3372-4f67-8B6F-C84C071F4751")IMFVideoSampleAllocatorCallback:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetCallback(IMFVideoSampleAllocatorNotify * pNotify) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFreeSampleCount(LONG *plSamples) = 0;
};
#else
typedef struct IMFVideoSampleAllocatorCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFVideoSampleAllocatorCallback * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFVideoSampleAllocatorCallback * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFVideoSampleAllocatorCallback * This);
    HRESULT(STDMETHODCALLTYPE *SetCallback)(IMFVideoSampleAllocatorCallback * This, IMFVideoSampleAllocatorNotify * pNotify);
    HRESULT(STDMETHODCALLTYPE *GetFreeSampleCount)(IMFVideoSampleAllocatorCallback * This, LONG * plSamples);
    END_INTERFACE
} IMFVideoSampleAllocatorCallbackVtbl;

interface IMFVideoSampleAllocatorCallback {
    CONST_VTBL struct IMFVideoSampleAllocatorCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFVideoSampleAllocatorCallback_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFVideoSampleAllocatorCallback_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFVideoSampleAllocatorCallback_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFVideoSampleAllocatorCallback_SetCallback(This,pNotify)  ((This)->lpVtbl->SetCallback(This,pNotify))
#define IMFVideoSampleAllocatorCallback_GetFreeSampleCount(This,plSamples)  ((This)->lpVtbl->GetFreeSampleCount(This,plSamples))
#endif

#endif

#endif /* __IMFVideoSampleAllocatorCallback_INTERFACE_DEFINED__ */

#ifndef __IMFVideoSampleAllocatorEx_INTERFACE_DEFINED__
#define __IMFVideoSampleAllocatorEx_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFVideoSampleAllocatorEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("545b3a48-3283-4f62-866f-a62d8f598f9f")IMFVideoSampleAllocatorEx:public IMFVideoSampleAllocator
{
    public:
    virtual HRESULT STDMETHODCALLTYPE InitializeSampleAllocatorEx(DWORD cInitialSamples, DWORD cMaximumSamples, IMFAttributes * pAttributes, IMFMediaType * pMediaType) = 0;
};
#else /* C style interface */
typedef struct IMFVideoSampleAllocatorExVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFVideoSampleAllocatorEx * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IMFVideoSampleAllocatorEx * This);
    ULONG(STDMETHODCALLTYPE * Release) (IMFVideoSampleAllocatorEx * This);
    HRESULT(STDMETHODCALLTYPE * SetDirectXManager) (IMFVideoSampleAllocatorEx * This, IUnknown * pManager);
    HRESULT(STDMETHODCALLTYPE * UninitializeSampleAllocator) (IMFVideoSampleAllocatorEx * This);
    HRESULT(STDMETHODCALLTYPE * InitializeSampleAllocator) (IMFVideoSampleAllocatorEx * This, DWORD cRequestedFrames, IMFMediaType * pMediaType);
    HRESULT(STDMETHODCALLTYPE * AllocateSample) (IMFVideoSampleAllocatorEx * This, IMFSample ** ppSample);
    HRESULT(STDMETHODCALLTYPE * InitializeSampleAllocatorEx) (IMFVideoSampleAllocatorEx * This, DWORD cInitialSamples, DWORD cMaximumSamples, IMFAttributes * pAttributes, IMFMediaType * pMediaType);
    END_INTERFACE
} IMFVideoSampleAllocatorExVtbl;

interface IMFVideoSampleAllocatorEx {
    CONST_VTBL struct IMFVideoSampleAllocatorExVtbl *lpVtbl;
};
    

#ifdef COBJMACROS
#define IMFVideoSampleAllocatorEx_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFVideoSampleAllocatorEx_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFVideoSampleAllocatorEx_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFVideoSampleAllocatorEx_SetDirectXManager(This,pManager)  ((This)->lpVtbl->SetDirectXManager(This,pManager)) 
#define IMFVideoSampleAllocatorEx_UninitializeSampleAllocator(This)  ((This)->lpVtbl->UninitializeSampleAllocator(This)) 
#define IMFVideoSampleAllocatorEx_InitializeSampleAllocator(This,cRequestedFrames,pMediaType)  ((This)->lpVtbl->InitializeSampleAllocator(This,cRequestedFrames,pMediaType)) 
#define IMFVideoSampleAllocatorEx_AllocateSample(This,ppSample)  ((This)->lpVtbl->AllocateSample(This,ppSample)) 
#define IMFVideoSampleAllocatorEx_InitializeSampleAllocatorEx(This,cInitialSamples,cMaximumSamples,pAttributes,pMediaType)  ((This)->lpVtbl->InitializeSampleAllocatorEx(This,cInitialSamples,cMaximumSamples,pAttributes,pMediaType)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFVideoSampleAllocatorEx_INTERFACE_DEFINED__ */

#endif /* WINVER >= _WIN32_WINNT_WIN7 */

#if (WINVER >= _WIN32_WINNT_WINBLUE) 

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0011_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0011_v0_0_s_ifspec;

#ifndef __IMFDXGIDeviceManagerSource_INTERFACE_DEFINED__
#define __IMFDXGIDeviceManagerSource_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFDXGIDeviceManagerSource;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("20bc074b-7a8d-4609-8c3b-64a0a3b5d7ce")IMFDXGIDeviceManagerSource:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetManager(IMFDXGIDeviceManager ** ppManager) = 0;
};
#else /* C style interface */
typedef struct IMFDXGIDeviceManagerSourceVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFDXGIDeviceManagerSource * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IMFDXGIDeviceManagerSource * This);
    ULONG(STDMETHODCALLTYPE * Release) (IMFDXGIDeviceManagerSource * This);
    HRESULT(STDMETHODCALLTYPE * GetManager) (IMFDXGIDeviceManagerSource * This, IMFDXGIDeviceManager ** ppManager);
    END_INTERFACE
} IMFDXGIDeviceManagerSourceVtbl;

interface IMFDXGIDeviceManagerSource {
    CONST_VTBL struct IMFDXGIDeviceManagerSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFDXGIDeviceManagerSource_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFDXGIDeviceManagerSource_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFDXGIDeviceManagerSource_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFDXGIDeviceManagerSource_GetManager(This,ppManager)  ((This)->lpVtbl->GetManager(This,ppManager)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFDXGIDeviceManagerSource_INTERFACE_DEFINED__ */

#endif /* WINVER >= _WIN32_WINNT_WINBLUE */

#if (WINVER >= _WIN32_WINNT_WIN8) 

typedef enum _MF_VIDEO_PROCESSOR_ROTATION {
    ROTATION_NONE = 0,
    ROTATION_NORMAL = 1
} MF_VIDEO_PROCESSOR_ROTATION;

typedef enum _MF_VIDEO_PROCESSOR_MIRROR {
    MIRROR_NONE = 0,
    MIRROR_HORIZONTAL = 1,
    MIRROR_VERTICAL = 2
} MF_VIDEO_PROCESSOR_MIRROR;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0012_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0012_v0_0_s_ifspec;

#ifndef __IMFVideoProcessorControl_INTERFACE_DEFINED__
#define __IMFVideoProcessorControl_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFVideoProcessorControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A3F675D5-6119-4f7f-A100-1D8B280F0EFB")IMFVideoProcessorControl:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetBorderColor(MFARGB *) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSourceRectangle(RECT *) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDestinationRectangle(RECT *) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMirror(MF_VIDEO_PROCESSOR_MIRROR) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetRotation(MF_VIDEO_PROCESSOR_ROTATION) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetConstrictionSize(SIZE *) = 0;
};
#else /* C style interface */
typedef struct IMFVideoProcessorControlVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IMFVideoProcessorControl *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IMFVideoProcessorControl *);
    ULONG (STDMETHODCALLTYPE *Release)(IMFVideoProcessorControl *);
    HRESULT (STDMETHODCALLTYPE *SetBorderColor)(IMFVideoProcessorControl *, MFARGB *);
    HRESULT (STDMETHODCALLTYPE *SetSourceRectangle)(IMFVideoProcessorControl *, RECT *);
    HRESULT (STDMETHODCALLTYPE *SetDestinationRectangle)(IMFVideoProcessorControl *, RECT *);
    HRESULT (STDMETHODCALLTYPE *SetMirror)(IMFVideoProcessorControl *, MF_VIDEO_PROCESSOR_MIRROR);
    HRESULT (STDMETHODCALLTYPE *SetRotation)(IMFVideoProcessorControl *, MF_VIDEO_PROCESSOR_ROTATION);
    HRESULT (STDMETHODCALLTYPE *SetConstrictionSize)(IMFVideoProcessorControl *, SIZE *);
    END_INTERFACE
} IMFVideoProcessorControlVtbl;

interface IMFVideoProcessorControl {
    CONST_VTBL struct IMFVideoProcessorControlVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFVideoProcessorControl_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFVideoProcessorControl_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFVideoProcessorControl_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFVideoProcessorControl_SetBorderColor(This,pBorderColor)  ((This)->lpVtbl->SetBorderColor(This,pBorderColor)) 
#define IMFVideoProcessorControl_SetSourceRectangle(This,pSrcRect)  ((This)->lpVtbl->SetSourceRectangle(This,pSrcRect)) 
#define IMFVideoProcessorControl_SetDestinationRectangle(This,pDstRect)  ((This)->lpVtbl->SetDestinationRectangle(This,pDstRect)) 
#define IMFVideoProcessorControl_SetMirror(This,eMirror)  ((This)->lpVtbl->SetMirror(This,eMirror)) 
#define IMFVideoProcessorControl_SetRotation(This,eRotation)  ((This)->lpVtbl->SetRotation(This,eRotation)) 
#define IMFVideoProcessorControl_SetConstrictionSize(This,pConstrictionSize)  ((This)->lpVtbl->SetConstrictionSize(This,pConstrictionSize)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFVideoProcessorControl_INTERFACE_DEFINED__ */

#endif /* WINVER >= _WIN32_WINNT_WIN8 */

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0009_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0009_v0_0_s_ifspec;

#ifndef __IMFTopology_INTERFACE_DEFINED__
#define __IMFTopology_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTopology;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("83CF873A-F6DA-4bc8-823F-BACFD55DC433")IMFTopology:public IMFAttributes
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetTopologyID(TOPOID * pID) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddNode(IMFTopologyNode *pNode) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveNode(IMFTopologyNode *pNode) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNodeCount(WORD *pwNodes) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNode(WORD wIndex, IMFTopologyNode **ppNode) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clear(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE CloneFrom(IMFTopology *pTopology) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNodeByID(TOPOID qwTopoNodeID, IMFTopologyNode **ppNode) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSourceNodeCollection(IMFCollection **ppCollection) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOutputNodeCollection(IMFCollection **ppCollection) = 0;
};
#else
typedef struct IMFTopologyVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTopology * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFTopology * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFTopology * This);
    HRESULT(STDMETHODCALLTYPE *GetItem)(IMFTopology * This, REFGUID guidKey, PROPVARIANT * pValue);
    HRESULT(STDMETHODCALLTYPE *GetItemType)(IMFTopology * This, REFGUID guidKey, MF_ATTRIBUTE_TYPE * pType);
    HRESULT(STDMETHODCALLTYPE *CompareItem)(IMFTopology * This, REFGUID guidKey, REFPROPVARIANT Value, BOOL * pbResult);
    HRESULT(STDMETHODCALLTYPE *Compare)(IMFTopology * This, IMFAttributes * pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL * pbResult);
    HRESULT(STDMETHODCALLTYPE *GetUINT32)(IMFTopology * This, REFGUID guidKey, UINT32 * punValue);
    HRESULT(STDMETHODCALLTYPE *GetUINT64)(IMFTopology * This, REFGUID guidKey, UINT64 * punValue);
    HRESULT(STDMETHODCALLTYPE *GetDouble)(IMFTopology * This, REFGUID guidKey, double *pfValue);
    HRESULT(STDMETHODCALLTYPE *GetGUID)(IMFTopology * This, REFGUID guidKey, GUID * pguidValue);
    HRESULT(STDMETHODCALLTYPE *GetStringLength)(IMFTopology * This, REFGUID guidKey, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetString)(IMFTopology * This, REFGUID guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetAllocatedString)(IMFTopology * This, REFGUID guidKey, LPWSTR * ppwszValue, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetBlobSize)(IMFTopology * This, REFGUID guidKey, UINT32 * pcbBlobSize);
    HRESULT(STDMETHODCALLTYPE *GetBlob)(IMFTopology * This, REFGUID guidKey, UINT8 * pBuf, UINT32 cbBufSize, UINT32 * pcbBlobSize);
    HRESULT(STDMETHODCALLTYPE *GetAllocatedBlob)(IMFTopology * This, REFGUID guidKey, UINT8 ** ppBuf, UINT32 * pcbSize);
    HRESULT(STDMETHODCALLTYPE *GetUnknown)(IMFTopology * This, REFGUID guidKey, REFIID riid, LPVOID * ppv);
    HRESULT(STDMETHODCALLTYPE *SetItem)(IMFTopology * This, REFGUID guidKey, REFPROPVARIANT Value);
    HRESULT(STDMETHODCALLTYPE *DeleteItem)(IMFTopology * This, REFGUID guidKey);
    HRESULT(STDMETHODCALLTYPE *DeleteAllItems)(IMFTopology * This);
    HRESULT(STDMETHODCALLTYPE *SetUINT32)(IMFTopology * This, REFGUID guidKey, UINT32 unValue);
    HRESULT(STDMETHODCALLTYPE *SetUINT64)(IMFTopology * This, REFGUID guidKey, UINT64 unValue);
    HRESULT(STDMETHODCALLTYPE *SetDouble)(IMFTopology * This, REFGUID guidKey, double fValue);
    HRESULT(STDMETHODCALLTYPE *SetGUID)(IMFTopology * This, REFGUID guidKey, REFGUID guidValue);
    HRESULT(STDMETHODCALLTYPE *SetString)(IMFTopology * This, REFGUID guidKey, LPCWSTR wszValue);
    HRESULT(STDMETHODCALLTYPE *SetBlob)(IMFTopology * This, REFGUID guidKey, const UINT8 * pBuf, UINT32 cbBufSize);
    HRESULT(STDMETHODCALLTYPE *SetUnknown)(IMFTopology * This, REFGUID guidKey, IUnknown * pUnknown);
    HRESULT(STDMETHODCALLTYPE *LockStore)(IMFTopology * This);
    HRESULT(STDMETHODCALLTYPE *UnlockStore)(IMFTopology * This);
    HRESULT(STDMETHODCALLTYPE *GetCount)(IMFTopology * This, UINT32 * pcItems);
    HRESULT(STDMETHODCALLTYPE *GetItemByIndex)(IMFTopology * This, UINT32 unIndex, GUID * pguidKey, PROPVARIANT * pValue);
    HRESULT(STDMETHODCALLTYPE *CopyAllItems)(IMFTopology * This, IMFAttributes * pDest);
    HRESULT(STDMETHODCALLTYPE *GetTopologyID)(IMFTopology * This, TOPOID * pID);
    HRESULT(STDMETHODCALLTYPE *AddNode)(IMFTopology * This, IMFTopologyNode * pNode);
    HRESULT(STDMETHODCALLTYPE *RemoveNode)(IMFTopology * This, IMFTopologyNode * pNode);
    HRESULT(STDMETHODCALLTYPE *GetNodeCount)(IMFTopology * This, WORD * pwNodes);
    HRESULT(STDMETHODCALLTYPE *GetNode)(IMFTopology * This, WORD wIndex, IMFTopologyNode ** ppNode);
    HRESULT(STDMETHODCALLTYPE *Clear)(IMFTopology * This);
    HRESULT(STDMETHODCALLTYPE *CloneFrom)(IMFTopology * This, IMFTopology * pTopology);
    HRESULT(STDMETHODCALLTYPE *GetNodeByID)(IMFTopology * This, TOPOID qwTopoNodeID, IMFTopologyNode ** ppNode);
    HRESULT(STDMETHODCALLTYPE *GetSourceNodeCollection)(IMFTopology * This, IMFCollection ** ppCollection);
    HRESULT(STDMETHODCALLTYPE *GetOutputNodeCollection)(IMFTopology * This, IMFCollection ** ppCollection);
    END_INTERFACE
} IMFTopologyVtbl;

interface IMFTopology {
    CONST_VTBL struct IMFTopologyVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFTopology_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFTopology_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFTopology_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFTopology_GetItem(This,guidKey,pValue)  ((This)->lpVtbl->GetItem(This,guidKey,pValue))
#define IMFTopology_GetItemType(This,guidKey,pType)  ((This)->lpVtbl->GetItemType(This,guidKey,pType))
#define IMFTopology_CompareItem(This,guidKey,Value,pbResult)  ((This)->lpVtbl->CompareItem(This,guidKey,Value,pbResult))
#define IMFTopology_Compare(This,pTheirs,MatchType,pbResult)  ((This)->lpVtbl->Compare(This,pTheirs,MatchType,pbResult))
#define IMFTopology_GetUINT32(This,guidKey,punValue)  ((This)->lpVtbl->GetUINT32(This,guidKey,punValue))
#define IMFTopology_GetUINT64(This,guidKey,punValue)  ((This)->lpVtbl->GetUINT64(This,guidKey,punValue))
#define IMFTopology_GetDouble(This,guidKey,pfValue)  ((This)->lpVtbl->GetDouble(This,guidKey,pfValue))
#define IMFTopology_GetGUID(This,guidKey,pguidValue)  ((This)->lpVtbl->GetGUID(This,guidKey,pguidValue))
#define IMFTopology_GetStringLength(This,guidKey,pcchLength)  ((This)->lpVtbl->GetStringLength(This,guidKey,pcchLength))
#define IMFTopology_GetString(This,guidKey,pwszValue,cchBufSize,pcchLength)  ((This)->lpVtbl->GetString(This,guidKey,pwszValue,cchBufSize,pcchLength))
#define IMFTopology_GetAllocatedString(This,guidKey,ppwszValue,pcchLength)  ((This)->lpVtbl->GetAllocatedString(This,guidKey,ppwszValue,pcchLength))
#define IMFTopology_GetBlobSize(This,guidKey,pcbBlobSize)  ((This)->lpVtbl->GetBlobSize(This,guidKey,pcbBlobSize))
#define IMFTopology_GetBlob(This,guidKey,pBuf,cbBufSize,pcbBlobSize)  ((This)->lpVtbl->GetBlob(This,guidKey,pBuf,cbBufSize,pcbBlobSize))
#define IMFTopology_GetAllocatedBlob(This,guidKey,ppBuf,pcbSize)  ((This)->lpVtbl->GetAllocatedBlob(This,guidKey,ppBuf,pcbSize))
#define IMFTopology_GetUnknown(This,guidKey,riid,ppv)  ((This)->lpVtbl->GetUnknown(This,guidKey,riid,ppv))
#define IMFTopology_SetItem(This,guidKey,Value)  ((This)->lpVtbl->SetItem(This,guidKey,Value))
#define IMFTopology_DeleteItem(This,guidKey)  ((This)->lpVtbl->DeleteItem(This,guidKey))
#define IMFTopology_DeleteAllItems(This)  ((This)->lpVtbl->DeleteAllItems(This))
#define IMFTopology_SetUINT32(This,guidKey,unValue)  ((This)->lpVtbl->SetUINT32(This,guidKey,unValue))
#define IMFTopology_SetUINT64(This,guidKey,unValue)  ((This)->lpVtbl->SetUINT64(This,guidKey,unValue))
#define IMFTopology_SetDouble(This,guidKey,fValue)  ((This)->lpVtbl->SetDouble(This,guidKey,fValue))
#define IMFTopology_SetGUID(This,guidKey,guidValue)  ((This)->lpVtbl->SetGUID(This,guidKey,guidValue))
#define IMFTopology_SetString(This,guidKey,wszValue)  ((This)->lpVtbl->SetString(This,guidKey,wszValue))
#define IMFTopology_SetBlob(This,guidKey,pBuf,cbBufSize)  ((This)->lpVtbl->SetBlob(This,guidKey,pBuf,cbBufSize))
#define IMFTopology_SetUnknown(This,guidKey,pUnknown)  ((This)->lpVtbl->SetUnknown(This,guidKey,pUnknown))
#define IMFTopology_LockStore(This)  ((This)->lpVtbl->LockStore(This))
#define IMFTopology_UnlockStore(This)  ((This)->lpVtbl->UnlockStore(This))
#define IMFTopology_GetCount(This,pcItems)  ((This)->lpVtbl->GetCount(This,pcItems))
#define IMFTopology_GetItemByIndex(This,unIndex,pguidKey,pValue)  ((This)->lpVtbl->GetItemByIndex(This,unIndex,pguidKey,pValue))
#define IMFTopology_CopyAllItems(This,pDest)  ((This)->lpVtbl->CopyAllItems(This,pDest))
#define IMFTopology_GetTopologyID(This,pID)  ((This)->lpVtbl->GetTopologyID(This,pID))
#define IMFTopology_AddNode(This,pNode)  ((This)->lpVtbl->AddNode(This,pNode))
#define IMFTopology_RemoveNode(This,pNode)  ((This)->lpVtbl->RemoveNode(This,pNode))
#define IMFTopology_GetNodeCount(This,pwNodes)  ((This)->lpVtbl->GetNodeCount(This,pwNodes))
#define IMFTopology_GetNode(This,wIndex,ppNode)  ((This)->lpVtbl->GetNode(This,wIndex,ppNode))
#define IMFTopology_Clear(This)  ((This)->lpVtbl->Clear(This))
#define IMFTopology_CloneFrom(This,pTopology)  ((This)->lpVtbl->CloneFrom(This,pTopology))
#define IMFTopology_GetNodeByID(This,qwTopoNodeID,ppNode)  ((This)->lpVtbl->GetNodeByID(This,qwTopoNodeID,ppNode))
#define IMFTopology_GetSourceNodeCollection(This,ppCollection)  ((This)->lpVtbl->GetSourceNodeCollection(This,ppCollection))
#define IMFTopology_GetOutputNodeCollection(This,ppCollection)  ((This)->lpVtbl->GetOutputNodeCollection(This,ppCollection))
#endif

#endif

#endif

EXTERN_GUID(MF_TOPOLOGY_PROJECTSTART, 0x7ed3f802, 0x86bb, 0x4b3f, 0xb7, 0xe4, 0x7c, 0xb4, 0x3a, 0xfd, 0x4b, 0x80);
EXTERN_GUID(MF_TOPOLOGY_PROJECTSTOP, 0x7ed3f803, 0x86bb, 0x4b3f, 0xb7, 0xe4, 0x7c, 0xb4, 0x3a, 0xfd, 0x4b, 0x80);
EXTERN_GUID(MF_TOPOLOGY_NO_MARKIN_MARKOUT, 0x7ed3f804, 0x86bb, 0x4b3f, 0xb7, 0xe4, 0x7c, 0xb4, 0x3a, 0xfd, 0x4b, 0x80);

#if (WINVER >= _WIN32_WINNT_WIN7)
typedef enum MFTOPOLOGY_DXVA_MODE {
    MFTOPOLOGY_DXVA_DEFAULT = 0,
    MFTOPOLOGY_DXVA_NONE = 1,
    MFTOPOLOGY_DXVA_FULL = 2
} MFTOPOLOGY_DXVA_MODE;

EXTERN_GUID(MF_TOPOLOGY_DXVA_MODE, 0x1e8d34f6, 0xf5ab, 0x4e23, 0xbb, 0x88, 0x87, 0x4a, 0xa3, 0xa1, 0xa7, 0x4d);
EXTERN_GUID(MF_TOPOLOGY_ENABLE_XVP_FOR_PLAYBACK, 0x1967731f, 0xcd78, 0x42fc, 0xb0, 0x26, 0x9, 0x92, 0xa5, 0x6e, 0x56, 0x93);
EXTERN_GUID(MF_TOPOLOGY_STATIC_PLAYBACK_OPTIMIZATIONS, 0xb86cac42, 0x41a6, 0x4b79, 0x89, 0x7a, 0x1a, 0xb0, 0xe5, 0x2b, 0x4a, 0x1b);
EXTERN_GUID(MF_TOPOLOGY_PLAYBACK_MAX_DIMS, 0x5715cf19, 0x5768, 0x44aa, 0xad, 0x6e, 0x87, 0x21, 0xf1, 0xb0, 0xf9, 0xbb);

typedef enum MFTOPOLOGY_HARDWARE_MODE {
    MFTOPOLOGY_HWMODE_SOFTWARE_ONLY = 0,
    MFTOPOLOGY_HWMODE_USE_HARDWARE = 1,
    MFTOPOLOGY_HWMODE_USE_ONLY_HARDWARE = 2,
} MFTOPOLOGY_HARDWARE_MODE;

EXTERN_GUID(MF_TOPOLOGY_HARDWARE_MODE, 0xd2d362fd, 0x4e4f, 0x4191, 0xa5, 0x79, 0xc6, 0x18, 0xb6, 0x67, 0x6, 0xaf);
EXTERN_GUID(MF_TOPOLOGY_PLAYBACK_FRAMERATE, 0xc164737a, 0xc2b1, 0x4553, 0x83, 0xbb, 0x5a, 0x52, 0x60, 0x72, 0x44, 0x8f);
EXTERN_GUID(MF_TOPOLOGY_DYNAMIC_CHANGE_NOT_ALLOWED, 0xd529950b, 0xd484, 0x4527, 0xa9, 0xcd, 0xb1, 0x90, 0x95, 0x32, 0xb5, 0xb0);
EXTERN_GUID(MF_TOPOLOGY_ENUMERATE_SOURCE_TYPES, 0x6248c36d, 0x5d0b, 0x4f40, 0xa0, 0xbb, 0xb0, 0xb3, 0x05, 0xf7, 0x76, 0x98);
EXTERN_GUID(MF_TOPOLOGY_START_TIME_ON_PRESENTATION_SWITCH, 0xc8cc113f, 0x7951, 0x4548, 0xaa, 0xd6, 0x9e, 0xd6, 0x20, 0x2e, 0x62, 0xb3);
#if (WINVER >= _WIN32_WINNT_WIN8) 
EXTERN_GUID(MF_DISABLE_LOCALLY_REGISTERED_PLUGINS, 0x66b16da9, 0xadd4, 0x47e0, 0xa1, 0x6b, 0x5a, 0xf1, 0xfb, 0x48, 0x36, 0x34);
EXTERN_GUID(MF_LOCAL_PLUGIN_CONTROL_POLICY, 0xd91b0085, 0xc86d, 0x4f81, 0x88, 0x22, 0x8c, 0x68, 0xe1, 0xd7, 0xfa, 0x04);
#endif /* WINVER >= _WIN32_WINNT_WIN8 */
#endif /* WINVER >= _WIN32_WINNT_WIN7 */

STDAPI MFCreateTopology(IMFTopology **ppTopo);

typedef enum MF_TOPOLOGY_TYPE {
    MF_TOPOLOGY_OUTPUT_NODE = 0,
    MF_TOPOLOGY_SOURCESTREAM_NODE = (MF_TOPOLOGY_OUTPUT_NODE + 1),
    MF_TOPOLOGY_TRANSFORM_NODE = (MF_TOPOLOGY_SOURCESTREAM_NODE + 1),
    MF_TOPOLOGY_TEE_NODE = (MF_TOPOLOGY_TRANSFORM_NODE + 1),
    MF_TOPOLOGY_MAX = 0xffffffff
} MF_TOPOLOGY_TYPE;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0010_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0010_v0_0_s_ifspec;

#ifndef __IMFTopologyNode_INTERFACE_DEFINED__
#define __IMFTopologyNode_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTopologyNode;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("83CF873A-F6DA-4bc8-823F-BACFD55DC430")IMFTopologyNode:public IMFAttributes
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetObject(IUnknown * pObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetObject(IUnknown **ppObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNodeType(MF_TOPOLOGY_TYPE *pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTopoNodeID(TOPOID *pID) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetTopoNodeID(TOPOID ullTopoID) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInputCount(DWORD *pcInputs) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOutputCount(DWORD *pcOutputs) = 0;
    virtual HRESULT STDMETHODCALLTYPE ConnectOutput(DWORD dwOutputIndex, IMFTopologyNode *pDownstreamNode, DWORD dwInputIndexOnDownstreamNode) = 0;
    virtual HRESULT STDMETHODCALLTYPE DisconnectOutput(DWORD dwOutputIndex) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInput(DWORD dwInputIndex, IMFTopologyNode **ppUpstreamNode, DWORD *pdwOutputIndexOnUpstreamNode) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOutput(DWORD dwOutputIndex, IMFTopologyNode **ppDownstreamNode, DWORD *pdwInputIndexOnDownstreamNode) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOutputPrefType(DWORD dwOutputIndex, IMFMediaType *pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOutputPrefType(DWORD dwOutputIndex, IMFMediaType **ppType) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetInputPrefType(DWORD dwInputIndex, IMFMediaType *pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInputPrefType(DWORD dwInputIndex, IMFMediaType **ppType) = 0;
    virtual HRESULT STDMETHODCALLTYPE CloneFrom(IMFTopologyNode *pNode) = 0;
};
#else
typedef struct IMFTopologyNodeVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTopologyNode * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFTopologyNode * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFTopologyNode * This);
    HRESULT(STDMETHODCALLTYPE *GetItem)(IMFTopologyNode * This, REFGUID guidKey, PROPVARIANT * pValue);
    HRESULT(STDMETHODCALLTYPE *GetItemType)(IMFTopologyNode * This, REFGUID guidKey, MF_ATTRIBUTE_TYPE * pType);
    HRESULT(STDMETHODCALLTYPE *CompareItem)(IMFTopologyNode * This, REFGUID guidKey, REFPROPVARIANT Value, BOOL * pbResult);
    HRESULT(STDMETHODCALLTYPE *Compare)(IMFTopologyNode * This, IMFAttributes * pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL * pbResult);
    HRESULT(STDMETHODCALLTYPE *GetUINT32)(IMFTopologyNode * This, REFGUID guidKey, UINT32 * punValue);
    HRESULT(STDMETHODCALLTYPE *GetUINT64)(IMFTopologyNode * This, REFGUID guidKey, UINT64 * punValue);
    HRESULT(STDMETHODCALLTYPE *GetDouble)(IMFTopologyNode * This, REFGUID guidKey, double *pfValue);
    HRESULT(STDMETHODCALLTYPE *GetGUID)(IMFTopologyNode * This, REFGUID guidKey, GUID * pguidValue);
    HRESULT(STDMETHODCALLTYPE *GetStringLength)(IMFTopologyNode * This, REFGUID guidKey, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetString)(IMFTopologyNode * This, REFGUID guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetAllocatedString)(IMFTopologyNode * This, REFGUID guidKey, LPWSTR * ppwszValue, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetBlobSize)(IMFTopologyNode * This, REFGUID guidKey, UINT32 * pcbBlobSize);
    HRESULT(STDMETHODCALLTYPE *GetBlob)(IMFTopologyNode * This, REFGUID guidKey, UINT8 * pBuf, UINT32 cbBufSize, UINT32 * pcbBlobSize);
    HRESULT(STDMETHODCALLTYPE *GetAllocatedBlob)(IMFTopologyNode * This, REFGUID guidKey, UINT8 ** ppBuf, UINT32 * pcbSize);
    HRESULT(STDMETHODCALLTYPE *GetUnknown)(IMFTopologyNode * This, REFGUID guidKey, REFIID riid, LPVOID * ppv);
    HRESULT(STDMETHODCALLTYPE *SetItem)(IMFTopologyNode * This, REFGUID guidKey, REFPROPVARIANT Value);
    HRESULT(STDMETHODCALLTYPE *DeleteItem)(IMFTopologyNode * This, REFGUID guidKey);
    HRESULT(STDMETHODCALLTYPE *DeleteAllItems)(IMFTopologyNode * This);
    HRESULT(STDMETHODCALLTYPE *SetUINT32)(IMFTopologyNode * This, REFGUID guidKey, UINT32 unValue);
    HRESULT(STDMETHODCALLTYPE *SetUINT64)(IMFTopologyNode * This, REFGUID guidKey, UINT64 unValue);
    HRESULT(STDMETHODCALLTYPE *SetDouble)(IMFTopologyNode * This, REFGUID guidKey, double fValue);
    HRESULT(STDMETHODCALLTYPE *SetGUID)(IMFTopologyNode * This, REFGUID guidKey, REFGUID guidValue);
    HRESULT(STDMETHODCALLTYPE *SetString)(IMFTopologyNode * This, REFGUID guidKey, LPCWSTR wszValue);
    HRESULT(STDMETHODCALLTYPE *SetBlob)(IMFTopologyNode * This, REFGUID guidKey, const UINT8 * pBuf, UINT32 cbBufSize);
    HRESULT(STDMETHODCALLTYPE *SetUnknown)(IMFTopologyNode * This, REFGUID guidKey, IUnknown * pUnknown);
    HRESULT(STDMETHODCALLTYPE *LockStore)(IMFTopologyNode * This);
    HRESULT(STDMETHODCALLTYPE *UnlockStore)(IMFTopologyNode * This);
    HRESULT(STDMETHODCALLTYPE *GetCount)(IMFTopologyNode * This, UINT32 * pcItems);
    HRESULT(STDMETHODCALLTYPE *GetItemByIndex)(IMFTopologyNode * This, UINT32 unIndex, GUID * pguidKey, PROPVARIANT * pValue);
    HRESULT(STDMETHODCALLTYPE *CopyAllItems)(IMFTopologyNode * This, IMFAttributes * pDest);
    HRESULT(STDMETHODCALLTYPE *SetObject)(IMFTopologyNode * This, IUnknown * pObject);
    HRESULT(STDMETHODCALLTYPE *GetObject)(IMFTopologyNode * This, IUnknown ** ppObject);
    HRESULT(STDMETHODCALLTYPE *GetNodeType)(IMFTopologyNode * This, MF_TOPOLOGY_TYPE * pType);
    HRESULT(STDMETHODCALLTYPE *GetTopoNodeID)(IMFTopologyNode * This, TOPOID * pID);
    HRESULT(STDMETHODCALLTYPE *SetTopoNodeID)(IMFTopologyNode * This, TOPOID ullTopoID);
    HRESULT(STDMETHODCALLTYPE *GetInputCount)(IMFTopologyNode * This, DWORD * pcInputs);
    HRESULT(STDMETHODCALLTYPE *GetOutputCount)(IMFTopologyNode * This, DWORD * pcOutputs);
    HRESULT(STDMETHODCALLTYPE *ConnectOutput)(IMFTopologyNode * This, DWORD dwOutputIndex, IMFTopologyNode * pDownstreamNode, DWORD dwInputIndexOnDownstreamNode);
    HRESULT(STDMETHODCALLTYPE *DisconnectOutput)(IMFTopologyNode * This, DWORD dwOutputIndex);
    HRESULT(STDMETHODCALLTYPE *GetInput)(IMFTopologyNode * This, DWORD dwInputIndex, IMFTopologyNode ** ppUpstreamNode, DWORD * pdwOutputIndexOnUpstreamNode);
    HRESULT(STDMETHODCALLTYPE *GetOutput)(IMFTopologyNode * This, DWORD dwOutputIndex, IMFTopologyNode ** ppDownstreamNode, DWORD * pdwInputIndexOnDownstreamNode);
    HRESULT(STDMETHODCALLTYPE *SetOutputPrefType)(IMFTopologyNode * This, DWORD dwOutputIndex, IMFMediaType * pType);
    HRESULT(STDMETHODCALLTYPE *GetOutputPrefType)(IMFTopologyNode * This, DWORD dwOutputIndex, IMFMediaType ** ppType);
    HRESULT(STDMETHODCALLTYPE *SetInputPrefType)(IMFTopologyNode * This, DWORD dwInputIndex, IMFMediaType * pType);
    HRESULT(STDMETHODCALLTYPE *GetInputPrefType)(IMFTopologyNode * This, DWORD dwInputIndex, IMFMediaType ** ppType);
    HRESULT(STDMETHODCALLTYPE *CloneFrom)(IMFTopologyNode * This, IMFTopologyNode * pNode);
    END_INTERFACE
} IMFTopologyNodeVtbl;

interface IMFTopologyNode {
    CONST_VTBL struct IMFTopologyNodeVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFTopologyNode_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFTopologyNode_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFTopologyNode_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFTopologyNode_GetItem(This,guidKey,pValue)  ((This)->lpVtbl->GetItem(This,guidKey,pValue))
#define IMFTopologyNode_GetItemType(This,guidKey,pType)  ((This)->lpVtbl->GetItemType(This,guidKey,pType))
#define IMFTopologyNode_CompareItem(This,guidKey,Value,pbResult)  ((This)->lpVtbl->CompareItem(This,guidKey,Value,pbResult))
#define IMFTopologyNode_Compare(This,pTheirs,MatchType,pbResult)  ((This)->lpVtbl->Compare(This,pTheirs,MatchType,pbResult))
#define IMFTopologyNode_GetUINT32(This,guidKey,punValue)  ((This)->lpVtbl->GetUINT32(This,guidKey,punValue))
#define IMFTopologyNode_GetUINT64(This,guidKey,punValue)  ((This)->lpVtbl->GetUINT64(This,guidKey,punValue))
#define IMFTopologyNode_GetDouble(This,guidKey,pfValue)  ((This)->lpVtbl->GetDouble(This,guidKey,pfValue))
#define IMFTopologyNode_GetGUID(This,guidKey,pguidValue)  ((This)->lpVtbl->GetGUID(This,guidKey,pguidValue))
#define IMFTopologyNode_GetStringLength(This,guidKey,pcchLength)  ((This)->lpVtbl->GetStringLength(This,guidKey,pcchLength))
#define IMFTopologyNode_GetString(This,guidKey,pwszValue,cchBufSize,pcchLength)  ((This)->lpVtbl->GetString(This,guidKey,pwszValue,cchBufSize,pcchLength))
#define IMFTopologyNode_GetAllocatedString(This,guidKey,ppwszValue,pcchLength)  ((This)->lpVtbl->GetAllocatedString(This,guidKey,ppwszValue,pcchLength))
#define IMFTopologyNode_GetBlobSize(This,guidKey,pcbBlobSize)  ((This)->lpVtbl->GetBlobSize(This,guidKey,pcbBlobSize))
#define IMFTopologyNode_GetBlob(This,guidKey,pBuf,cbBufSize,pcbBlobSize)  ((This)->lpVtbl->GetBlob(This,guidKey,pBuf,cbBufSize,pcbBlobSize))
#define IMFTopologyNode_GetAllocatedBlob(This,guidKey,ppBuf,pcbSize)  ((This)->lpVtbl->GetAllocatedBlob(This,guidKey,ppBuf,pcbSize))
#define IMFTopologyNode_GetUnknown(This,guidKey,riid,ppv)  ((This)->lpVtbl->GetUnknown(This,guidKey,riid,ppv))
#define IMFTopologyNode_SetItem(This,guidKey,Value)  ((This)->lpVtbl->SetItem(This,guidKey,Value))
#define IMFTopologyNode_DeleteItem(This,guidKey)  ((This)->lpVtbl->DeleteItem(This,guidKey))
#define IMFTopologyNode_DeleteAllItems(This)  ((This)->lpVtbl->DeleteAllItems(This))
#define IMFTopologyNode_SetUINT32(This,guidKey,unValue)  ((This)->lpVtbl->SetUINT32(This,guidKey,unValue))
#define IMFTopologyNode_SetUINT64(This,guidKey,unValue)  ((This)->lpVtbl->SetUINT64(This,guidKey,unValue))
#define IMFTopologyNode_SetDouble(This,guidKey,fValue)  ((This)->lpVtbl->SetDouble(This,guidKey,fValue))
#define IMFTopologyNode_SetGUID(This,guidKey,guidValue)  ((This)->lpVtbl->SetGUID(This,guidKey,guidValue))
#define IMFTopologyNode_SetString(This,guidKey,wszValue)  ((This)->lpVtbl->SetString(This,guidKey,wszValue))
#define IMFTopologyNode_SetBlob(This,guidKey,pBuf,cbBufSize)  ((This)->lpVtbl->SetBlob(This,guidKey,pBuf,cbBufSize))
#define IMFTopologyNode_SetUnknown(This,guidKey,pUnknown)  ((This)->lpVtbl->SetUnknown(This,guidKey,pUnknown))
#define IMFTopologyNode_LockStore(This)  ((This)->lpVtbl->LockStore(This))
#define IMFTopologyNode_UnlockStore(This)  ((This)->lpVtbl->UnlockStore(This))
#define IMFTopologyNode_GetCount(This,pcItems)  ((This)->lpVtbl->GetCount(This,pcItems))
#define IMFTopologyNode_GetItemByIndex(This,unIndex,pguidKey,pValue)  ((This)->lpVtbl->GetItemByIndex(This,unIndex,pguidKey,pValue))
#define IMFTopologyNode_CopyAllItems(This,pDest)  ((This)->lpVtbl->CopyAllItems(This,pDest))
#define IMFTopologyNode_SetObject(This,pObject)  ((This)->lpVtbl->SetObject(This,pObject))
#define IMFTopologyNode_GetObject(This,ppObject)  ((This)->lpVtbl->GetObject(This,ppObject))
#define IMFTopologyNode_GetNodeType(This,pType)  ((This)->lpVtbl->GetNodeType(This,pType))
#define IMFTopologyNode_GetTopoNodeID(This,pID)  ((This)->lpVtbl->GetTopoNodeID(This,pID))
#define IMFTopologyNode_SetTopoNodeID(This,ullTopoID)  ((This)->lpVtbl->SetTopoNodeID(This,ullTopoID))
#define IMFTopologyNode_GetInputCount(This,pcInputs)  ((This)->lpVtbl->GetInputCount(This,pcInputs))
#define IMFTopologyNode_GetOutputCount(This,pcOutputs)  ((This)->lpVtbl->GetOutputCount(This,pcOutputs))
#define IMFTopologyNode_ConnectOutput(This,dwOutputIndex,pDownstreamNode,dwInputIndexOnDownstreamNode)  ((This)->lpVtbl->ConnectOutput(This,dwOutputIndex,pDownstreamNode,dwInputIndexOnDownstreamNode))
#define IMFTopologyNode_DisconnectOutput(This,dwOutputIndex)  ((This)->lpVtbl->DisconnectOutput(This,dwOutputIndex))
#define IMFTopologyNode_GetInput(This,dwInputIndex,ppUpstreamNode,pdwOutputIndexOnUpstreamNode)  ((This)->lpVtbl->GetInput(This,dwInputIndex,ppUpstreamNode,pdwOutputIndexOnUpstreamNode))
#define IMFTopologyNode_GetOutput(This,dwOutputIndex,ppDownstreamNode,pdwInputIndexOnDownstreamNode)  ((This)->lpVtbl->GetOutput(This,dwOutputIndex,ppDownstreamNode,pdwInputIndexOnDownstreamNode))
#define IMFTopologyNode_SetOutputPrefType(This,dwOutputIndex,pType)  ((This)->lpVtbl->SetOutputPrefType(This,dwOutputIndex,pType))
#define IMFTopologyNode_GetOutputPrefType(This,dwOutputIndex,ppType)  ((This)->lpVtbl->GetOutputPrefType(This,dwOutputIndex,ppType))
#define IMFTopologyNode_SetInputPrefType(This,dwInputIndex,pType)  ((This)->lpVtbl->SetInputPrefType(This,dwInputIndex,pType))
#define IMFTopologyNode_GetInputPrefType(This,dwInputIndex,ppType)  ((This)->lpVtbl->GetInputPrefType(This,dwInputIndex,ppType))
#define IMFTopologyNode_CloneFrom(This,pNode)  ((This)->lpVtbl->CloneFrom(This,pNode))
#endif

#endif

HRESULT STDMETHODCALLTYPE IMFTopologyNode_RemoteGetOutputPrefType_Proxy(IMFTopologyNode *This, DWORD dwOutputIndex, DWORD *pcbData, BYTE **ppbData);
void __RPC_STUB IMFTopologyNode_RemoteGetOutputPrefType_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE IMFTopologyNode_RemoteGetInputPrefType_Proxy(IMFTopologyNode *This, DWORD dwInputIndex, DWORD *pcbData, BYTE **ppbData);
void __RPC_STUB IMFTopologyNode_RemoteGetInputPrefType_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
#endif

typedef enum _MF_TOPONODE_FLUSH_MODE {
    MF_TOPONODE_FLUSH_ALWAYS = 0,
    MF_TOPONODE_FLUSH_SEEK = (MF_TOPONODE_FLUSH_ALWAYS + 1),
    MF_TOPONODE_FLUSH_NEVER = (MF_TOPONODE_FLUSH_SEEK + 1)
} MF_TOPONODE_FLUSH_MODE;

EXTERN_GUID(MF_TOPONODE_FLUSH, 0x494bbce8, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);

typedef enum _MF_TOPONODE_DRAIN_MODE {
    MF_TOPONODE_DRAIN_DEFAULT = 0,
    MF_TOPONODE_DRAIN_ALWAYS = (MF_TOPONODE_DRAIN_DEFAULT + 1),
    MF_TOPONODE_DRAIN_NEVER = (MF_TOPONODE_DRAIN_ALWAYS + 1)
} MF_TOPONODE_DRAIN_MODE;

EXTERN_GUID(MF_TOPONODE_DRAIN, 0x494bbce9, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_D3DAWARE, 0x494bbced, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPOLOGY_RESOLUTION_STATUS, 0x494bbcde, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_ERRORCODE, 0x494bbcee, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_CONNECT_METHOD, 0x494bbcf1, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_LOCKED, 0x494bbcf7, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_WORKQUEUE_ID, 0x494bbcf8, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_WORKQUEUE_MMCSS_CLASS, 0x494bbcf9, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_DECRYPTOR, 0x494bbcfa, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_DISCARDABLE, 0x494bbcfb, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_ERROR_MAJORTYPE, 0x494bbcfd, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_ERROR_SUBTYPE, 0x494bbcfe, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_WORKQUEUE_MMCSS_TASKID, 0x494bbcff, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_WORKQUEUE_MMCSS_PRIORITY, 0x5001f840, 0x2816, 0x48f4, 0x93, 0x64, 0xad, 0x1e, 0xf6, 0x61, 0xa1, 0x23);
EXTERN_GUID(MF_TOPONODE_WORKQUEUE_ITEM_PRIORITY, 0xa1ff99be, 0x5e97, 0x4a53, 0xb4, 0x94, 0x56, 0x8c, 0x64, 0x2c, 0x0f, 0xf3);
EXTERN_GUID(MF_TOPONODE_MARKIN_HERE, 0x494bbd00, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_MARKOUT_HERE, 0x494bbd01, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_DECODER, 0x494bbd02, 0xb031, 0x4e38, 0x97, 0xc4, 0xd5, 0x42, 0x2d, 0xd6, 0x18, 0xdc);
EXTERN_GUID(MF_TOPONODE_MEDIASTART, 0x835c58ea, 0xe075, 0x4bc7, 0xbc, 0xba, 0x4d, 0xe0, 0x00, 0xdf, 0x9a, 0xe6);
EXTERN_GUID(MF_TOPONODE_MEDIASTOP, 0x835c58eb, 0xe075, 0x4bc7, 0xbc, 0xba, 0x4d, 0xe0, 0x00, 0xdf, 0x9a, 0xe6);
EXTERN_GUID(MF_TOPONODE_SOURCE, 0x835c58ec, 0xe075, 0x4bc7, 0xbc, 0xba, 0x4d, 0xe0, 0x00, 0xdf, 0x9a, 0xe6);
EXTERN_GUID(MF_TOPONODE_PRESENTATION_DESCRIPTOR, 0x835c58ed, 0xe075, 0x4bc7, 0xbc, 0xba, 0x4d, 0xe0, 0x00, 0xdf, 0x9a, 0xe6);
EXTERN_GUID(MF_TOPONODE_STREAM_DESCRIPTOR, 0x835c58ee, 0xe075, 0x4bc7, 0xbc, 0xba, 0x4d, 0xe0, 0x00, 0xdf, 0x9a, 0xe6);
EXTERN_GUID(MF_TOPONODE_SEQUENCE_ELEMENTID, 0x835c58ef, 0xe075, 0x4bc7, 0xbc, 0xba, 0x4d, 0xe0, 0x00, 0xdf, 0x9a, 0xe6);
EXTERN_GUID(MF_TOPONODE_TRANSFORM_OBJECTID, 0x88dcc0c9, 0x293e, 0x4e8b, 0x9a, 0xeb, 0xa, 0xd6, 0x4c, 0xc0, 0x16, 0xb0);
EXTERN_GUID(MF_TOPONODE_STREAMID, 0x14932f9b, 0x9087, 0x4bb4, 0x84, 0x12, 0x51, 0x67, 0x14, 0x5c, 0xbe, 0x04);
EXTERN_GUID(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, 0x14932f9c, 0x9087, 0x4bb4, 0x84, 0x12, 0x51, 0x67, 0x14, 0x5c, 0xbe, 0x04);
EXTERN_GUID(MF_TOPONODE_RATELESS, 0x14932f9d, 0x9087, 0x4bb4, 0x84, 0x12, 0x51, 0x67, 0x14, 0x5c, 0xbe, 0x04);
EXTERN_GUID(MF_TOPONODE_DISABLE_PREROLL, 0x14932f9e, 0x9087, 0x4bb4, 0x84, 0x12, 0x51, 0x67, 0x14, 0x5c, 0xbe, 0x04);
EXTERN_GUID(MF_TOPONODE_PRIMARYOUTPUT, 0x6304ef99, 0x16b2, 0x4ebe, 0x9d, 0x67, 0xe4, 0xc5, 0x39, 0xb3, 0xa2, 0x59);

STDAPI MFCreateTopologyNode(MF_TOPOLOGY_TYPE NodeType, IMFTopologyNode **ppNode);

#if (WINVER >= _WIN32_WINNT_WIN7)
STDAPI MFGetTopoNodeCurrentType(IMFTopologyNode *pNode, DWORD dwStreamIndex, BOOL fOutput, IMFMediaType **ppType);
#endif /* WINVER >= _WIN32_WINNT_WIN7 */

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0011_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0011_v0_0_s_ifspec;

#ifndef __IMFGetService_INTERFACE_DEFINED__
#define __IMFGetService_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFGetService;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("fa993888-4383-415a-a930-dd472a8cf6f7")IMFGetService:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetService(REFGUID guidService, REFIID riid, LPVOID * ppvObject) = 0;
};
#else
typedef struct IMFGetServiceVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFGetService * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFGetService * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFGetService * This);
    HRESULT(STDMETHODCALLTYPE *GetService)(IMFGetService * This, REFGUID guidService, REFIID riid, LPVOID * ppvObject);
    END_INTERFACE
} IMFGetServiceVtbl;

interface IMFGetService {
    CONST_VTBL struct IMFGetServiceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFGetService_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFGetService_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFGetService_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFGetService_GetService(This,guidService,riid,ppvObject)  ((This)->lpVtbl->GetService(This,guidService,riid,ppvObject))
#endif

#endif

#endif

STDAPI MFGetService(IUnknown *punkObject, REFGUID guidService, REFIID riid, LPVOID *ppvObject);

typedef LONGLONG MFTIME;

typedef enum _MFCLOCK_CHARACTERISTICS_FLAGS {
    MFCLOCK_CHARACTERISTICS_FLAG_FREQUENCY_10MHZ = 0x2,
    MFCLOCK_CHARACTERISTICS_FLAG_ALWAYS_RUNNING = 0x4,
    MFCLOCK_CHARACTERISTICS_FLAG_IS_SYSTEM_CLOCK = 0x8
} MFCLOCK_CHARACTERISTICS_FLAGS;

typedef enum _MFCLOCK_STATE {
    MFCLOCK_STATE_INVALID = 0,
    MFCLOCK_STATE_RUNNING = (MFCLOCK_STATE_INVALID + 1),
    MFCLOCK_STATE_STOPPED = (MFCLOCK_STATE_RUNNING + 1),
    MFCLOCK_STATE_PAUSED = (MFCLOCK_STATE_STOPPED + 1)
} MFCLOCK_STATE;

typedef enum _MFCLOCK_RELATIONAL_FLAGS {
    MFCLOCK_RELATIONAL_FLAG_JITTER_NEVER_AHEAD = 0x1
} MFCLOCK_RELATIONAL_FLAGS;

typedef struct _MFCLOCK_PROPERTIES {
    unsigned __int64 qwCorrelationRate;
    GUID guidClockId;
    DWORD dwClockFlags;
    unsigned __int64 qwClockFrequency;
    DWORD dwClockTolerance;
    DWORD dwClockJitter;
} MFCLOCK_PROPERTIES;

#define MFCLOCK_FREQUENCY_HNS  10000000
#define MFCLOCK_TOLERANCE_UNKNOWN  50000
#define MFCLOCK_JITTER_ISR  1000
#define MFCLOCK_JITTER_DPC  4000
#define MFCLOCK_JITTER_PASSIVE  10000

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0012_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0012_v0_0_s_ifspec;

#ifndef __IMFClock_INTERFACE_DEFINED__
#define __IMFClock_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFClock;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("2eb1e945-18b8-4139-9b1a-d5d584818530")IMFClock:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetClockCharacteristics(DWORD * pdwCharacteristics) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCorrelatedTime(DWORD dwReserved, LONGLONG *pllClockTime, MFTIME *phnsSystemTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetContinuityKey(DWORD *pdwContinuityKey) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetState(DWORD dwReserved, MFCLOCK_STATE *peClockState) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperties(MFCLOCK_PROPERTIES *pClockProperties) = 0;
};
#else
typedef struct IMFClockVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFClock * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFClock * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFClock * This);
    HRESULT(STDMETHODCALLTYPE *GetClockCharacteristics)(IMFClock * This, DWORD * pdwCharacteristics);
    HRESULT(STDMETHODCALLTYPE *GetCorrelatedTime)(IMFClock * This, DWORD dwReserved, LONGLONG * pllClockTime, MFTIME * phnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *GetContinuityKey)(IMFClock * This, DWORD * pdwContinuityKey);
    HRESULT(STDMETHODCALLTYPE *GetState)(IMFClock * This, DWORD dwReserved, MFCLOCK_STATE * peClockState);
    HRESULT(STDMETHODCALLTYPE *GetProperties)(IMFClock * This, MFCLOCK_PROPERTIES * pClockProperties);
    END_INTERFACE
} IMFClockVtbl;

interface IMFClock {
    CONST_VTBL struct IMFClockVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFClock_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFClock_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFClock_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFClock_GetClockCharacteristics(This,pdwCharacteristics)  ((This)->lpVtbl->GetClockCharacteristics(This,pdwCharacteristics))
#define IMFClock_GetCorrelatedTime(This,dwReserved,pllClockTime,phnsSystemTime)  ((This)->lpVtbl->GetCorrelatedTime(This,dwReserved,pllClockTime,phnsSystemTime))
#define IMFClock_GetContinuityKey(This,pdwContinuityKey)  ((This)->lpVtbl->GetContinuityKey(This,pdwContinuityKey))
#define IMFClock_GetState(This,dwReserved,peClockState)  ((This)->lpVtbl->GetState(This,dwReserved,peClockState))
#define IMFClock_GetProperties(This,pClockProperties)  ((This)->lpVtbl->GetProperties(This,pClockProperties))
#endif

#endif

#endif

STDAPI_(MFTIME) MFGetSystemTime(void);

#define PRESENTATION_CURRENT_POSITION   0x7fffffffffffffff

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0013_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0013_v0_0_s_ifspec;

#ifndef __IMFPresentationClock_INTERFACE_DEFINED__
#define __IMFPresentationClock_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFPresentationClock;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("868CE85C-8EA9-4f55-AB82-B009A910A805")IMFPresentationClock:public IMFClock
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetTimeSource(IMFPresentationTimeSource * pTimeSource) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTimeSource(IMFPresentationTimeSource **ppTimeSource) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTime(MFTIME *phnsClockTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddClockStateSink(IMFClockStateSink *pStateSink) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveClockStateSink(IMFClockStateSink *pStateSink) = 0;
    virtual HRESULT STDMETHODCALLTYPE Start(LONGLONG llClockStartOffset) = 0;
    virtual HRESULT STDMETHODCALLTYPE Stop(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Pause(void) = 0;
};
#else
typedef struct IMFPresentationClockVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFPresentationClock * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFPresentationClock * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFPresentationClock * This);
    HRESULT(STDMETHODCALLTYPE *GetClockCharacteristics)(IMFPresentationClock * This, DWORD * pdwCharacteristics);
    HRESULT(STDMETHODCALLTYPE *GetCorrelatedTime)(IMFPresentationClock * This, DWORD dwReserved, LONGLONG * pllClockTime, MFTIME * phnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *GetContinuityKey)(IMFPresentationClock * This, DWORD * pdwContinuityKey);
    HRESULT(STDMETHODCALLTYPE *GetState)(IMFPresentationClock * This, DWORD dwReserved, MFCLOCK_STATE * peClockState);
    HRESULT(STDMETHODCALLTYPE *GetProperties)(IMFPresentationClock * This, MFCLOCK_PROPERTIES * pClockProperties);
    HRESULT(STDMETHODCALLTYPE *SetTimeSource)(IMFPresentationClock * This, IMFPresentationTimeSource * pTimeSource);
    HRESULT(STDMETHODCALLTYPE *GetTimeSource)(IMFPresentationClock * This, IMFPresentationTimeSource ** ppTimeSource);
    HRESULT(STDMETHODCALLTYPE *GetTime)(IMFPresentationClock * This, MFTIME * phnsClockTime);
    HRESULT(STDMETHODCALLTYPE *AddClockStateSink)(IMFPresentationClock * This, IMFClockStateSink * pStateSink);
    HRESULT(STDMETHODCALLTYPE *RemoveClockStateSink)(IMFPresentationClock * This, IMFClockStateSink * pStateSink);
    HRESULT(STDMETHODCALLTYPE *Start)(IMFPresentationClock * This, LONGLONG llClockStartOffset);
    HRESULT(STDMETHODCALLTYPE *Stop)(IMFPresentationClock * This);
    HRESULT(STDMETHODCALLTYPE *Pause)(IMFPresentationClock * This);
    END_INTERFACE
} IMFPresentationClockVtbl;

interface IMFPresentationClock {
    CONST_VTBL struct IMFPresentationClockVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFPresentationClock_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFPresentationClock_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFPresentationClock_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFPresentationClock_GetClockCharacteristics(This,pdwCharacteristics)  ((This)->lpVtbl->GetClockCharacteristics(This,pdwCharacteristics))
#define IMFPresentationClock_GetCorrelatedTime(This,dwReserved,pllClockTime,phnsSystemTime)  ((This)->lpVtbl->GetCorrelatedTime(This,dwReserved,pllClockTime,phnsSystemTime))
#define IMFPresentationClock_GetContinuityKey(This,pdwContinuityKey)  ((This)->lpVtbl->GetContinuityKey(This,pdwContinuityKey))
#define IMFPresentationClock_GetState(This,dwReserved,peClockState)  ((This)->lpVtbl->GetState(This,dwReserved,peClockState))
#define IMFPresentationClock_GetProperties(This,pClockProperties)  ((This)->lpVtbl->GetProperties(This,pClockProperties))
#define IMFPresentationClock_SetTimeSource(This,pTimeSource)  ((This)->lpVtbl->SetTimeSource(This,pTimeSource))
#define IMFPresentationClock_GetTimeSource(This,ppTimeSource)  ((This)->lpVtbl->GetTimeSource(This,ppTimeSource))
#define IMFPresentationClock_GetTime(This,phnsClockTime)  ((This)->lpVtbl->GetTime(This,phnsClockTime))
#define IMFPresentationClock_AddClockStateSink(This,pStateSink)  ((This)->lpVtbl->AddClockStateSink(This,pStateSink))
#define IMFPresentationClock_RemoveClockStateSink(This,pStateSink)  ((This)->lpVtbl->RemoveClockStateSink(This,pStateSink))
#define IMFPresentationClock_Start(This,llClockStartOffset)  ((This)->lpVtbl->Start(This,llClockStartOffset))
#define IMFPresentationClock_Stop(This)  ((This)->lpVtbl->Stop(This))
#define IMFPresentationClock_Pause(This)  ((This)->lpVtbl->Pause(This))
#endif

#endif

#endif

STDAPI MFCreatePresentationClock(IMFPresentationClock **ppPresentationClock);


extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0014_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0014_v0_0_s_ifspec;

#ifndef __IMFPresentationTimeSource_INTERFACE_DEFINED__
#define __IMFPresentationTimeSource_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFPresentationTimeSource;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7FF12CCE-F76F-41c2-863B-1666C8E5E139")IMFPresentationTimeSource:public IMFClock
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetUnderlyingClock(IMFClock ** ppClock) = 0;
};
#else
typedef struct IMFPresentationTimeSourceVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFPresentationTimeSource * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFPresentationTimeSource * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFPresentationTimeSource * This);
    HRESULT(STDMETHODCALLTYPE *GetClockCharacteristics)(IMFPresentationTimeSource * This, DWORD * pdwCharacteristics);
    HRESULT(STDMETHODCALLTYPE *GetCorrelatedTime)(IMFPresentationTimeSource * This, DWORD dwReserved, LONGLONG * pllClockTime, MFTIME * phnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *GetContinuityKey)(IMFPresentationTimeSource * This, DWORD * pdwContinuityKey);
    HRESULT(STDMETHODCALLTYPE *GetState)(IMFPresentationTimeSource * This, DWORD dwReserved, MFCLOCK_STATE * peClockState);
    HRESULT(STDMETHODCALLTYPE *GetProperties)(IMFPresentationTimeSource * This, MFCLOCK_PROPERTIES * pClockProperties);
    HRESULT(STDMETHODCALLTYPE *GetUnderlyingClock)(IMFPresentationTimeSource * This, IMFClock ** ppClock);
    END_INTERFACE
} IMFPresentationTimeSourceVtbl;

interface IMFPresentationTimeSource {
    CONST_VTBL struct IMFPresentationTimeSourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFPresentationTimeSource_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFPresentationTimeSource_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFPresentationTimeSource_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFPresentationTimeSource_GetClockCharacteristics(This,pdwCharacteristics)  ((This)->lpVtbl->GetClockCharacteristics(This,pdwCharacteristics))
#define IMFPresentationTimeSource_GetCorrelatedTime(This,dwReserved,pllClockTime,phnsSystemTime)  ((This)->lpVtbl->GetCorrelatedTime(This,dwReserved,pllClockTime,phnsSystemTime))
#define IMFPresentationTimeSource_GetContinuityKey(This,pdwContinuityKey)  ((This)->lpVtbl->GetContinuityKey(This,pdwContinuityKey))
#define IMFPresentationTimeSource_GetState(This,dwReserved,peClockState)  ((This)->lpVtbl->GetState(This,dwReserved,peClockState))
#define IMFPresentationTimeSource_GetProperties(This,pClockProperties)  ((This)->lpVtbl->GetProperties(This,pClockProperties))
#define IMFPresentationTimeSource_GetUnderlyingClock(This,ppClock)  ((This)->lpVtbl->GetUnderlyingClock(This,ppClock))
#endif

#endif

#endif

STDAPI MFCreateSystemTimeSource(IMFPresentationTimeSource **ppSystemTimeSource);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0015_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0015_v0_0_s_ifspec;

#ifndef __IMFClockStateSink_INTERFACE_DEFINED__
#define __IMFClockStateSink_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFClockStateSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("F6696E82-74F7-4f3d-A178-8A5E09C3659F")IMFClockStateSink:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnClockStart(MFTIME hnsSystemTime, LONGLONG llClockStartOffset) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnClockStop(MFTIME hnsSystemTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnClockPause(MFTIME hnsSystemTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnClockRestart(MFTIME hnsSystemTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnClockSetRate(MFTIME hnsSystemTime, float flRate) = 0;
};
#else
typedef struct IMFClockStateSinkVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFClockStateSink * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFClockStateSink * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFClockStateSink * This);
    HRESULT(STDMETHODCALLTYPE *OnClockStart)(IMFClockStateSink * This, MFTIME hnsSystemTime, LONGLONG llClockStartOffset);
    HRESULT(STDMETHODCALLTYPE *OnClockStop)(IMFClockStateSink * This, MFTIME hnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *OnClockPause)(IMFClockStateSink * This, MFTIME hnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *OnClockRestart)(IMFClockStateSink * This, MFTIME hnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *OnClockSetRate)(IMFClockStateSink * This, MFTIME hnsSystemTime, float flRate);
    END_INTERFACE
} IMFClockStateSinkVtbl;

interface IMFClockStateSink {
    CONST_VTBL struct IMFClockStateSinkVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFClockStateSink_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFClockStateSink_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFClockStateSink_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFClockStateSink_OnClockStart(This,hnsSystemTime,llClockStartOffset)  ((This)->lpVtbl->OnClockStart(This,hnsSystemTime,llClockStartOffset))
#define IMFClockStateSink_OnClockStop(This,hnsSystemTime)  ((This)->lpVtbl->OnClockStop(This,hnsSystemTime))
#define IMFClockStateSink_OnClockPause(This,hnsSystemTime)  ((This)->lpVtbl->OnClockPause(This,hnsSystemTime))
#define IMFClockStateSink_OnClockRestart(This,hnsSystemTime)  ((This)->lpVtbl->OnClockRestart(This,hnsSystemTime))
#define IMFClockStateSink_OnClockSetRate(This,hnsSystemTime,flRate)  ((This)->lpVtbl->OnClockSetRate(This,hnsSystemTime,flRate))
#endif

#endif

#endif

EXTERN_GUID(MF_PD_PMPHOST_CONTEXT, 0x6c990d31, 0xbb8e, 0x477a, 0x85, 0x98, 0xd, 0x5d, 0x96, 0xfc, 0xd8, 0x8a);
EXTERN_GUID(MF_PD_APP_CONTEXT, 0x6c990d32, 0xbb8e, 0x477a, 0x85, 0x98, 0xd, 0x5d, 0x96, 0xfc, 0xd8, 0x8a);
EXTERN_GUID(MF_PD_DURATION, 0x6c990d33, 0xbb8e, 0x477a, 0x85, 0x98, 0xd, 0x5d, 0x96, 0xfc, 0xd8, 0x8a);
EXTERN_GUID(MF_PD_TOTAL_FILE_SIZE, 0x6c990d34, 0xbb8e, 0x477a, 0x85, 0x98, 0xd, 0x5d, 0x96, 0xfc, 0xd8, 0x8a);
EXTERN_GUID(MF_PD_AUDIO_ENCODING_BITRATE, 0x6c990d35, 0xbb8e, 0x477a, 0x85, 0x98, 0xd, 0x5d, 0x96, 0xfc, 0xd8, 0x8a);
EXTERN_GUID(MF_PD_VIDEO_ENCODING_BITRATE, 0x6c990d36, 0xbb8e, 0x477a, 0x85, 0x98, 0xd, 0x5d, 0x96, 0xfc, 0xd8, 0x8a);
EXTERN_GUID(MF_PD_MIME_TYPE, 0x6c990d37, 0xbb8e, 0x477a, 0x85, 0x98, 0xd, 0x5d, 0x96, 0xfc, 0xd8, 0x8a);
EXTERN_GUID(MF_PD_LAST_MODIFIED_TIME, 0x6c990d38, 0xbb8e, 0x477a, 0x85, 0x98, 0xd, 0x5d, 0x96, 0xfc, 0xd8, 0x8a);
#if (WINVER >= _WIN32_WINNT_WIN7)
EXTERN_GUID(MF_PD_PLAYBACK_ELEMENT_ID, 0x6c990d39, 0xbb8e, 0x477a, 0x85, 0x98, 0xd, 0x5d, 0x96, 0xfc, 0xd8, 0x8a);
EXTERN_GUID(MF_PD_PREFERRED_LANGUAGE, 0x6c990d3A, 0xbb8e, 0x477a, 0x85, 0x98, 0xd, 0x5d, 0x96, 0xfc, 0xd8, 0x8a);
EXTERN_GUID(MF_PD_PLAYBACK_BOUNDARY_TIME, 0x6c990d3b, 0xbb8e, 0x477a, 0x85, 0x98, 0xd, 0x5d, 0x96, 0xfc, 0xd8, 0x8a);
EXTERN_GUID(MF_PD_AUDIO_ISVARIABLEBITRATE, 0x33026ee0, 0xe387, 0x4582, 0xae, 0x0a, 0x34, 0xa2, 0xad, 0x3b, 0xaa, 0x18);
#endif /* WINVER >= _WIN32_WINNT_WIN7 */

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0016_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0016_v0_0_s_ifspec;

#ifndef __IMFPresentationDescriptor_INTERFACE_DEFINED__
#define __IMFPresentationDescriptor_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFPresentationDescriptor;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("03cb2711-24d7-4db6-a17f-f3a7a479a536")IMFPresentationDescriptor:public IMFAttributes
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetStreamDescriptorCount(DWORD * pdwDescriptorCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStreamDescriptorByIndex(DWORD dwIndex, BOOL *pfSelected, IMFStreamDescriptor **ppDescriptor) = 0;
    virtual HRESULT STDMETHODCALLTYPE SelectStream(DWORD dwDescriptorIndex) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeselectStream(DWORD dwDescriptorIndex) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IMFPresentationDescriptor **ppPresentationDescriptor) = 0;
};
#else
typedef struct IMFPresentationDescriptorVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFPresentationDescriptor * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFPresentationDescriptor * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFPresentationDescriptor * This);
    HRESULT(STDMETHODCALLTYPE *GetItem)(IMFPresentationDescriptor * This, REFGUID guidKey, PROPVARIANT * pValue);
    HRESULT(STDMETHODCALLTYPE *GetItemType)(IMFPresentationDescriptor * This, REFGUID guidKey, MF_ATTRIBUTE_TYPE * pType);
    HRESULT(STDMETHODCALLTYPE *CompareItem)(IMFPresentationDescriptor * This, REFGUID guidKey, REFPROPVARIANT Value, BOOL * pbResult);
    HRESULT(STDMETHODCALLTYPE *Compare)(IMFPresentationDescriptor * This, IMFAttributes * pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL * pbResult);
    HRESULT(STDMETHODCALLTYPE *GetUINT32)(IMFPresentationDescriptor * This, REFGUID guidKey, UINT32 * punValue);
    HRESULT(STDMETHODCALLTYPE *GetUINT64)(IMFPresentationDescriptor * This, REFGUID guidKey, UINT64 * punValue);
    HRESULT(STDMETHODCALLTYPE *GetDouble)(IMFPresentationDescriptor * This, REFGUID guidKey, double *pfValue);
    HRESULT(STDMETHODCALLTYPE *GetGUID)(IMFPresentationDescriptor * This, REFGUID guidKey, GUID * pguidValue);
    HRESULT(STDMETHODCALLTYPE *GetStringLength)(IMFPresentationDescriptor * This, REFGUID guidKey, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetString)(IMFPresentationDescriptor * This, REFGUID guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetAllocatedString)(IMFPresentationDescriptor * This, REFGUID guidKey, LPWSTR * ppwszValue, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetBlobSize)(IMFPresentationDescriptor * This, REFGUID guidKey, UINT32 * pcbBlobSize);
    HRESULT(STDMETHODCALLTYPE *GetBlob)(IMFPresentationDescriptor * This, REFGUID guidKey, UINT8 * pBuf, UINT32 cbBufSize, UINT32 * pcbBlobSize);
    HRESULT(STDMETHODCALLTYPE *GetAllocatedBlob)(IMFPresentationDescriptor * This, REFGUID guidKey, UINT8 ** ppBuf, UINT32 * pcbSize);
    HRESULT(STDMETHODCALLTYPE *GetUnknown)(IMFPresentationDescriptor * This, REFGUID guidKey, REFIID riid, LPVOID * ppv);
    HRESULT(STDMETHODCALLTYPE *SetItem)(IMFPresentationDescriptor * This, REFGUID guidKey, REFPROPVARIANT Value);
    HRESULT(STDMETHODCALLTYPE *DeleteItem)(IMFPresentationDescriptor * This, REFGUID guidKey);
    HRESULT(STDMETHODCALLTYPE *DeleteAllItems)(IMFPresentationDescriptor * This);
    HRESULT(STDMETHODCALLTYPE *SetUINT32)(IMFPresentationDescriptor * This, REFGUID guidKey, UINT32 unValue);
    HRESULT(STDMETHODCALLTYPE *SetUINT64)(IMFPresentationDescriptor * This, REFGUID guidKey, UINT64 unValue);
    HRESULT(STDMETHODCALLTYPE *SetDouble)(IMFPresentationDescriptor * This, REFGUID guidKey, double fValue);
    HRESULT(STDMETHODCALLTYPE *SetGUID)(IMFPresentationDescriptor * This, REFGUID guidKey, REFGUID guidValue);
    HRESULT(STDMETHODCALLTYPE *SetString)(IMFPresentationDescriptor * This, REFGUID guidKey, LPCWSTR wszValue);
    HRESULT(STDMETHODCALLTYPE *SetBlob)(IMFPresentationDescriptor * This, REFGUID guidKey, const UINT8 * pBuf, UINT32 cbBufSize);
    HRESULT(STDMETHODCALLTYPE *SetUnknown)(IMFPresentationDescriptor * This, REFGUID guidKey, IUnknown * pUnknown);
    HRESULT(STDMETHODCALLTYPE *LockStore)(IMFPresentationDescriptor * This);
    HRESULT(STDMETHODCALLTYPE *UnlockStore)(IMFPresentationDescriptor * This);
    HRESULT(STDMETHODCALLTYPE *GetCount)(IMFPresentationDescriptor * This, UINT32 * pcItems);
    HRESULT(STDMETHODCALLTYPE *GetItemByIndex)(IMFPresentationDescriptor * This, UINT32 unIndex, GUID * pguidKey, PROPVARIANT * pValue);
    HRESULT(STDMETHODCALLTYPE *CopyAllItems)(IMFPresentationDescriptor * This, IMFAttributes * pDest);
    HRESULT(STDMETHODCALLTYPE *GetStreamDescriptorCount)(IMFPresentationDescriptor * This, DWORD * pdwDescriptorCount);
    HRESULT(STDMETHODCALLTYPE *GetStreamDescriptorByIndex)(IMFPresentationDescriptor * This, DWORD dwIndex, BOOL * pfSelected, IMFStreamDescriptor ** ppDescriptor);
    HRESULT(STDMETHODCALLTYPE *SelectStream)(IMFPresentationDescriptor * This, DWORD dwDescriptorIndex);
    HRESULT(STDMETHODCALLTYPE *DeselectStream)(IMFPresentationDescriptor * This, DWORD dwDescriptorIndex);
    HRESULT(STDMETHODCALLTYPE *Clone)(IMFPresentationDescriptor * This, IMFPresentationDescriptor ** ppPresentationDescriptor);
    END_INTERFACE
} IMFPresentationDescriptorVtbl;

interface IMFPresentationDescriptor {
    CONST_VTBL struct IMFPresentationDescriptorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFPresentationDescriptor_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFPresentationDescriptor_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFPresentationDescriptor_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFPresentationDescriptor_GetItem(This,guidKey,pValue)  ((This)->lpVtbl->GetItem(This,guidKey,pValue))
#define IMFPresentationDescriptor_GetItemType(This,guidKey,pType)  ((This)->lpVtbl->GetItemType(This,guidKey,pType))
#define IMFPresentationDescriptor_CompareItem(This,guidKey,Value,pbResult)  ((This)->lpVtbl->CompareItem(This,guidKey,Value,pbResult))
#define IMFPresentationDescriptor_Compare(This,pTheirs,MatchType,pbResult)  ((This)->lpVtbl->Compare(This,pTheirs,MatchType,pbResult))
#define IMFPresentationDescriptor_GetUINT32(This,guidKey,punValue)  ((This)->lpVtbl->GetUINT32(This,guidKey,punValue))
#define IMFPresentationDescriptor_GetUINT64(This,guidKey,punValue)  ((This)->lpVtbl->GetUINT64(This,guidKey,punValue))
#define IMFPresentationDescriptor_GetDouble(This,guidKey,pfValue)  ((This)->lpVtbl->GetDouble(This,guidKey,pfValue))
#define IMFPresentationDescriptor_GetGUID(This,guidKey,pguidValue)  ((This)->lpVtbl->GetGUID(This,guidKey,pguidValue))
#define IMFPresentationDescriptor_GetStringLength(This,guidKey,pcchLength)  ((This)->lpVtbl->GetStringLength(This,guidKey,pcchLength))
#define IMFPresentationDescriptor_GetString(This,guidKey,pwszValue,cchBufSize,pcchLength)  ((This)->lpVtbl->GetString(This,guidKey,pwszValue,cchBufSize,pcchLength))
#define IMFPresentationDescriptor_GetAllocatedString(This,guidKey,ppwszValue,pcchLength)  ((This)->lpVtbl->GetAllocatedString(This,guidKey,ppwszValue,pcchLength))
#define IMFPresentationDescriptor_GetBlobSize(This,guidKey,pcbBlobSize)  ((This)->lpVtbl->GetBlobSize(This,guidKey,pcbBlobSize))
#define IMFPresentationDescriptor_GetBlob(This,guidKey,pBuf,cbBufSize,pcbBlobSize)  ((This)->lpVtbl->GetBlob(This,guidKey,pBuf,cbBufSize,pcbBlobSize))
#define IMFPresentationDescriptor_GetAllocatedBlob(This,guidKey,ppBuf,pcbSize)  ((This)->lpVtbl->GetAllocatedBlob(This,guidKey,ppBuf,pcbSize))
#define IMFPresentationDescriptor_GetUnknown(This,guidKey,riid,ppv)  ((This)->lpVtbl->GetUnknown(This,guidKey,riid,ppv))
#define IMFPresentationDescriptor_SetItem(This,guidKey,Value)  ((This)->lpVtbl->SetItem(This,guidKey,Value))
#define IMFPresentationDescriptor_DeleteItem(This,guidKey)  ((This)->lpVtbl->DeleteItem(This,guidKey))
#define IMFPresentationDescriptor_DeleteAllItems(This)  ((This)->lpVtbl->DeleteAllItems(This))
#define IMFPresentationDescriptor_SetUINT32(This,guidKey,unValue)  ((This)->lpVtbl->SetUINT32(This,guidKey,unValue))
#define IMFPresentationDescriptor_SetUINT64(This,guidKey,unValue)  ((This)->lpVtbl->SetUINT64(This,guidKey,unValue))
#define IMFPresentationDescriptor_SetDouble(This,guidKey,fValue)  ((This)->lpVtbl->SetDouble(This,guidKey,fValue))
#define IMFPresentationDescriptor_SetGUID(This,guidKey,guidValue)  ((This)->lpVtbl->SetGUID(This,guidKey,guidValue))
#define IMFPresentationDescriptor_SetString(This,guidKey,wszValue)  ((This)->lpVtbl->SetString(This,guidKey,wszValue))
#define IMFPresentationDescriptor_SetBlob(This,guidKey,pBuf,cbBufSize)  ((This)->lpVtbl->SetBlob(This,guidKey,pBuf,cbBufSize))
#define IMFPresentationDescriptor_SetUnknown(This,guidKey,pUnknown)  ((This)->lpVtbl->SetUnknown(This,guidKey,pUnknown))
#define IMFPresentationDescriptor_LockStore(This)  ((This)->lpVtbl->LockStore(This))
#define IMFPresentationDescriptor_UnlockStore(This)  ((This)->lpVtbl->UnlockStore(This))
#define IMFPresentationDescriptor_GetCount(This,pcItems)  ((This)->lpVtbl->GetCount(This,pcItems))
#define IMFPresentationDescriptor_GetItemByIndex(This,unIndex,pguidKey,pValue)  ((This)->lpVtbl->GetItemByIndex(This,unIndex,pguidKey,pValue))
#define IMFPresentationDescriptor_CopyAllItems(This,pDest)  ((This)->lpVtbl->CopyAllItems(This,pDest))
#define IMFPresentationDescriptor_GetStreamDescriptorCount(This,pdwDescriptorCount)  ((This)->lpVtbl->GetStreamDescriptorCount(This,pdwDescriptorCount))
#define IMFPresentationDescriptor_GetStreamDescriptorByIndex(This,dwIndex,pfSelected,ppDescriptor)  ((This)->lpVtbl->GetStreamDescriptorByIndex(This,dwIndex,pfSelected,ppDescriptor))
#define IMFPresentationDescriptor_SelectStream(This,dwDescriptorIndex)  ((This)->lpVtbl->SelectStream(This,dwDescriptorIndex))
#define IMFPresentationDescriptor_DeselectStream(This,dwDescriptorIndex)  ((This)->lpVtbl->DeselectStream(This,dwDescriptorIndex))
#define IMFPresentationDescriptor_Clone(This,ppPresentationDescriptor)  ((This)->lpVtbl->Clone(This,ppPresentationDescriptor))
#endif

#endif

#endif

STDAPI MFCreatePresentationDescriptor(DWORD cStreamDescriptors, IMFStreamDescriptor **apStreamDescriptors, IMFPresentationDescriptor **ppPresentationDescriptor);
STDAPI MFRequireProtectedEnvironment(IMFPresentationDescriptor *pPresentationDescriptor);
STDAPI MFSerializePresentationDescriptor(IMFPresentationDescriptor *pPD, DWORD *pcbData, BYTE **ppbData);
STDAPI MFDeserializePresentationDescriptor(DWORD cbData, BYTE *pbData, IMFPresentationDescriptor **ppPD);

EXTERN_GUID(MF_SD_LANGUAGE, 0xaf2180, 0xbdc2, 0x423c, 0xab, 0xca, 0xf5, 0x3, 0x59, 0x3b, 0xc1, 0x21);
EXTERN_GUID(MF_SD_PROTECTED, 0xaf2181, 0xbdc2, 0x423c, 0xab, 0xca, 0xf5, 0x3, 0x59, 0x3b, 0xc1, 0x21);
EXTERN_GUID(MF_SD_STREAM_NAME, 0x4f1b099d, 0xd314, 0x41e5, 0xa7, 0x81, 0x7f, 0xef, 0xaa, 0x4c, 0x50, 0x1f);
EXTERN_GUID(MF_SD_MUTUALLY_EXCLUSIVE, 0x23ef79c, 0x388d, 0x487f, 0xac, 0x17, 0x69, 0x6c, 0xd6, 0xe3, 0xc6, 0xf5);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0017_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0017_v0_0_s_ifspec;

#ifndef __IMFStreamDescriptor_INTERFACE_DEFINED__
#define __IMFStreamDescriptor_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFStreamDescriptor;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("56c03d9c-9dbb-45f5-ab4b-d80f47c05938")IMFStreamDescriptor:public IMFAttributes
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetStreamIdentifier(DWORD * pdwStreamIdentifier) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMediaTypeHandler(IMFMediaTypeHandler **ppMediaTypeHandler) = 0;
};
#else
typedef struct IMFStreamDescriptorVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFStreamDescriptor * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFStreamDescriptor * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFStreamDescriptor * This);
    HRESULT(STDMETHODCALLTYPE *GetItem)(IMFStreamDescriptor * This, REFGUID guidKey, PROPVARIANT * pValue);
    HRESULT(STDMETHODCALLTYPE *GetItemType)(IMFStreamDescriptor * This, REFGUID guidKey, MF_ATTRIBUTE_TYPE * pType);
    HRESULT(STDMETHODCALLTYPE *CompareItem)(IMFStreamDescriptor * This, REFGUID guidKey, REFPROPVARIANT Value, BOOL * pbResult);
    HRESULT(STDMETHODCALLTYPE *Compare)(IMFStreamDescriptor * This, IMFAttributes * pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL * pbResult);
    HRESULT(STDMETHODCALLTYPE *GetUINT32)(IMFStreamDescriptor * This, REFGUID guidKey, UINT32 * punValue);
    HRESULT(STDMETHODCALLTYPE *GetUINT64)(IMFStreamDescriptor * This, REFGUID guidKey, UINT64 * punValue);
    HRESULT(STDMETHODCALLTYPE *GetDouble)(IMFStreamDescriptor * This, REFGUID guidKey, double *pfValue);
    HRESULT(STDMETHODCALLTYPE *GetGUID)(IMFStreamDescriptor * This, REFGUID guidKey, GUID * pguidValue);
    HRESULT(STDMETHODCALLTYPE *GetStringLength)(IMFStreamDescriptor * This, REFGUID guidKey, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetString)(IMFStreamDescriptor * This, REFGUID guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetAllocatedString)(IMFStreamDescriptor * This, REFGUID guidKey, LPWSTR * ppwszValue, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetBlobSize)(IMFStreamDescriptor * This, REFGUID guidKey, UINT32 * pcbBlobSize);
    HRESULT(STDMETHODCALLTYPE *GetBlob)(IMFStreamDescriptor * This, REFGUID guidKey, UINT8 * pBuf, UINT32 cbBufSize, UINT32 * pcbBlobSize);
    HRESULT(STDMETHODCALLTYPE *GetAllocatedBlob)(IMFStreamDescriptor * This, REFGUID guidKey, UINT8 ** ppBuf, UINT32 * pcbSize);
    HRESULT(STDMETHODCALLTYPE *GetUnknown)(IMFStreamDescriptor * This, REFGUID guidKey, REFIID riid, LPVOID * ppv);
    HRESULT(STDMETHODCALLTYPE *SetItem)(IMFStreamDescriptor * This, REFGUID guidKey, REFPROPVARIANT Value);
    HRESULT(STDMETHODCALLTYPE *DeleteItem)(IMFStreamDescriptor * This, REFGUID guidKey);
    HRESULT(STDMETHODCALLTYPE *DeleteAllItems)(IMFStreamDescriptor * This);
    HRESULT(STDMETHODCALLTYPE *SetUINT32)(IMFStreamDescriptor * This, REFGUID guidKey, UINT32 unValue);
    HRESULT(STDMETHODCALLTYPE *SetUINT64)(IMFStreamDescriptor * This, REFGUID guidKey, UINT64 unValue);
    HRESULT(STDMETHODCALLTYPE *SetDouble)(IMFStreamDescriptor * This, REFGUID guidKey, double fValue);
    HRESULT(STDMETHODCALLTYPE *SetGUID)(IMFStreamDescriptor * This, REFGUID guidKey, REFGUID guidValue);
    HRESULT(STDMETHODCALLTYPE *SetString)(IMFStreamDescriptor * This, REFGUID guidKey, LPCWSTR wszValue);
    HRESULT(STDMETHODCALLTYPE *SetBlob)(IMFStreamDescriptor * This, REFGUID guidKey, const UINT8 * pBuf, UINT32 cbBufSize);
    HRESULT(STDMETHODCALLTYPE *SetUnknown)(IMFStreamDescriptor * This, REFGUID guidKey, IUnknown * pUnknown);
    HRESULT(STDMETHODCALLTYPE *LockStore)(IMFStreamDescriptor * This);
    HRESULT(STDMETHODCALLTYPE *UnlockStore)(IMFStreamDescriptor * This);
    HRESULT(STDMETHODCALLTYPE *GetCount)(IMFStreamDescriptor * This, UINT32 * pcItems);
    HRESULT(STDMETHODCALLTYPE *GetItemByIndex)(IMFStreamDescriptor * This, UINT32 unIndex, GUID * pguidKey, PROPVARIANT * pValue);
    HRESULT(STDMETHODCALLTYPE *CopyAllItems)(IMFStreamDescriptor * This, IMFAttributes * pDest);
    HRESULT(STDMETHODCALLTYPE *GetStreamIdentifier)(IMFStreamDescriptor * This, DWORD * pdwStreamIdentifier);
    HRESULT(STDMETHODCALLTYPE *GetMediaTypeHandler)(IMFStreamDescriptor * This, IMFMediaTypeHandler ** ppMediaTypeHandler);
    END_INTERFACE
} IMFStreamDescriptorVtbl;

interface IMFStreamDescriptor {
    CONST_VTBL struct IMFStreamDescriptorVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFStreamDescriptor_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFStreamDescriptor_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFStreamDescriptor_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFStreamDescriptor_GetItem(This,guidKey,pValue)  ((This)->lpVtbl->GetItem(This,guidKey,pValue))
#define IMFStreamDescriptor_GetItemType(This,guidKey,pType)  ((This)->lpVtbl->GetItemType(This,guidKey,pType))
#define IMFStreamDescriptor_CompareItem(This,guidKey,Value,pbResult)  ((This)->lpVtbl->CompareItem(This,guidKey,Value,pbResult))
#define IMFStreamDescriptor_Compare(This,pTheirs,MatchType,pbResult)  ((This)->lpVtbl->Compare(This,pTheirs,MatchType,pbResult))
#define IMFStreamDescriptor_GetUINT32(This,guidKey,punValue)  ((This)->lpVtbl->GetUINT32(This,guidKey,punValue))
#define IMFStreamDescriptor_GetUINT64(This,guidKey,punValue)  ((This)->lpVtbl->GetUINT64(This,guidKey,punValue))
#define IMFStreamDescriptor_GetDouble(This,guidKey,pfValue)  ((This)->lpVtbl->GetDouble(This,guidKey,pfValue))
#define IMFStreamDescriptor_GetGUID(This,guidKey,pguidValue)  ((This)->lpVtbl->GetGUID(This,guidKey,pguidValue))
#define IMFStreamDescriptor_GetStringLength(This,guidKey,pcchLength)  ((This)->lpVtbl->GetStringLength(This,guidKey,pcchLength))
#define IMFStreamDescriptor_GetString(This,guidKey,pwszValue,cchBufSize,pcchLength)  ((This)->lpVtbl->GetString(This,guidKey,pwszValue,cchBufSize,pcchLength))
#define IMFStreamDescriptor_GetAllocatedString(This,guidKey,ppwszValue,pcchLength)  ((This)->lpVtbl->GetAllocatedString(This,guidKey,ppwszValue,pcchLength))
#define IMFStreamDescriptor_GetBlobSize(This,guidKey,pcbBlobSize)  ((This)->lpVtbl->GetBlobSize(This,guidKey,pcbBlobSize))
#define IMFStreamDescriptor_GetBlob(This,guidKey,pBuf,cbBufSize,pcbBlobSize)  ((This)->lpVtbl->GetBlob(This,guidKey,pBuf,cbBufSize,pcbBlobSize))
#define IMFStreamDescriptor_GetAllocatedBlob(This,guidKey,ppBuf,pcbSize)  ((This)->lpVtbl->GetAllocatedBlob(This,guidKey,ppBuf,pcbSize))
#define IMFStreamDescriptor_GetUnknown(This,guidKey,riid,ppv)  ((This)->lpVtbl->GetUnknown(This,guidKey,riid,ppv))
#define IMFStreamDescriptor_SetItem(This,guidKey,Value)  ((This)->lpVtbl->SetItem(This,guidKey,Value))
#define IMFStreamDescriptor_DeleteItem(This,guidKey)  ((This)->lpVtbl->DeleteItem(This,guidKey))
#define IMFStreamDescriptor_DeleteAllItems(This)  ((This)->lpVtbl->DeleteAllItems(This))
#define IMFStreamDescriptor_SetUINT32(This,guidKey,unValue)  ((This)->lpVtbl->SetUINT32(This,guidKey,unValue))
#define IMFStreamDescriptor_SetUINT64(This,guidKey,unValue)  ((This)->lpVtbl->SetUINT64(This,guidKey,unValue))
#define IMFStreamDescriptor_SetDouble(This,guidKey,fValue)  ((This)->lpVtbl->SetDouble(This,guidKey,fValue))
#define IMFStreamDescriptor_SetGUID(This,guidKey,guidValue)  ((This)->lpVtbl->SetGUID(This,guidKey,guidValue))
#define IMFStreamDescriptor_SetString(This,guidKey,wszValue)  ((This)->lpVtbl->SetString(This,guidKey,wszValue))
#define IMFStreamDescriptor_SetBlob(This,guidKey,pBuf,cbBufSize)  ((This)->lpVtbl->SetBlob(This,guidKey,pBuf,cbBufSize))
#define IMFStreamDescriptor_SetUnknown(This,guidKey,pUnknown)  ((This)->lpVtbl->SetUnknown(This,guidKey,pUnknown))
#define IMFStreamDescriptor_LockStore(This)  ((This)->lpVtbl->LockStore(This))
#define IMFStreamDescriptor_UnlockStore(This)  ((This)->lpVtbl->UnlockStore(This))
#define IMFStreamDescriptor_GetCount(This,pcItems)  ((This)->lpVtbl->GetCount(This,pcItems))
#define IMFStreamDescriptor_GetItemByIndex(This,unIndex,pguidKey,pValue)  ((This)->lpVtbl->GetItemByIndex(This,unIndex,pguidKey,pValue))
#define IMFStreamDescriptor_CopyAllItems(This,pDest)  ((This)->lpVtbl->CopyAllItems(This,pDest))
#define IMFStreamDescriptor_GetStreamIdentifier(This,pdwStreamIdentifier)  ((This)->lpVtbl->GetStreamIdentifier(This,pdwStreamIdentifier))
#define IMFStreamDescriptor_GetMediaTypeHandler(This,ppMediaTypeHandler)  ((This)->lpVtbl->GetMediaTypeHandler(This,ppMediaTypeHandler))
#endif

#endif

#endif

STDAPI MFCreateStreamDescriptor(DWORD dwStreamIdentifier, DWORD cMediaTypes, IMFMediaType **apMediaTypes, IMFStreamDescriptor **ppDescriptor);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0018_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0018_v0_0_s_ifspec;

#ifndef __IMFMediaTypeHandler_INTERFACE_DEFINED__
#define __IMFMediaTypeHandler_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFMediaTypeHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e93dcf6c-4b07-4e1e-8123-aa16ed6eadf5")IMFMediaTypeHandler:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE IsMediaTypeSupported(IMFMediaType * pMediaType, IMFMediaType ** ppMediaType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMediaTypeCount(DWORD *pdwTypeCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMediaTypeByIndex(DWORD dwIndex, IMFMediaType **ppType) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCurrentMediaType(IMFMediaType *pMediaType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentMediaType(IMFMediaType **ppMediaType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMajorType(GUID *pguidMajorType) = 0;
};
#else
typedef struct IMFMediaTypeHandlerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFMediaTypeHandler * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFMediaTypeHandler * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFMediaTypeHandler * This);
    HRESULT(STDMETHODCALLTYPE *IsMediaTypeSupported)(IMFMediaTypeHandler * This, IMFMediaType * pMediaType, IMFMediaType ** ppMediaType);
    HRESULT(STDMETHODCALLTYPE *GetMediaTypeCount)(IMFMediaTypeHandler * This, DWORD * pdwTypeCount);
    HRESULT(STDMETHODCALLTYPE *GetMediaTypeByIndex)(IMFMediaTypeHandler * This, DWORD dwIndex, IMFMediaType ** ppType);
    HRESULT(STDMETHODCALLTYPE *SetCurrentMediaType)(IMFMediaTypeHandler * This, IMFMediaType * pMediaType);
    HRESULT(STDMETHODCALLTYPE *GetCurrentMediaType)(IMFMediaTypeHandler * This, IMFMediaType ** ppMediaType);
    HRESULT(STDMETHODCALLTYPE *GetMajorType)(IMFMediaTypeHandler * This, GUID * pguidMajorType);
    END_INTERFACE
} IMFMediaTypeHandlerVtbl;

interface IMFMediaTypeHandler {
    CONST_VTBL struct IMFMediaTypeHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFMediaTypeHandler_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFMediaTypeHandler_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFMediaTypeHandler_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFMediaTypeHandler_IsMediaTypeSupported(This,pMediaType,ppMediaType)  ((This)->lpVtbl->IsMediaTypeSupported(This,pMediaType,ppMediaType))
#define IMFMediaTypeHandler_GetMediaTypeCount(This,pdwTypeCount)  ((This)->lpVtbl->GetMediaTypeCount(This,pdwTypeCount))
#define IMFMediaTypeHandler_GetMediaTypeByIndex(This,dwIndex,ppType)  ((This)->lpVtbl->GetMediaTypeByIndex(This,dwIndex,ppType))
#define IMFMediaTypeHandler_SetCurrentMediaType(This,pMediaType)  ((This)->lpVtbl->SetCurrentMediaType(This,pMediaType))
#define IMFMediaTypeHandler_GetCurrentMediaType(This,ppMediaType)  ((This)->lpVtbl->GetCurrentMediaType(This,ppMediaType))
#define IMFMediaTypeHandler_GetMajorType(This,pguidMajorType)  ((This)->lpVtbl->GetMajorType(This,pguidMajorType))
#endif

#endif

HRESULT STDMETHODCALLTYPE IMFMediaTypeHandler_RemoteGetCurrentMediaType_Proxy(IMFMediaTypeHandler *This, BYTE **ppbData, DWORD *pcbData);
void __RPC_STUB IMFMediaTypeHandler_RemoteGetCurrentMediaType_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);

#endif

STDAPI MFCreateSimpleTypeHandler(IMFMediaTypeHandler **ppHandler);

typedef enum MFTIMER_FLAGS {
    MFTIMER_RELATIVE = 0x1
} MFTIMER_FLAGS;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0019_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0019_v0_0_s_ifspec;

#ifndef __IMFTimer_INTERFACE_DEFINED__
#define __IMFTimer_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTimer;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e56e4cbd-8f70-49d8-a0f8-edb3d6ab9bf2")IMFTimer:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetTimer(DWORD dwFlags, LONGLONG llClockTime, IMFAsyncCallback * pCallback, IUnknown * punkState, IUnknown ** ppunkKey) = 0;
    virtual HRESULT STDMETHODCALLTYPE CancelTimer(IUnknown *punkKey) = 0;
};
#else
typedef struct IMFTimerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTimer * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFTimer * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFTimer * This);
    HRESULT(STDMETHODCALLTYPE *SetTimer)(IMFTimer * This, DWORD dwFlags, LONGLONG llClockTime, IMFAsyncCallback * pCallback, IUnknown * punkState, IUnknown ** ppunkKey);
    HRESULT(STDMETHODCALLTYPE *CancelTimer)(IMFTimer * This, IUnknown * punkKey);
    END_INTERFACE
} IMFTimerVtbl;

interface IMFTimer {
    CONST_VTBL struct IMFTimerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFTimer_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFTimer_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFTimer_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFTimer_SetTimer(This,dwFlags,llClockTime,pCallback,punkState,ppunkKey)  ((This)->lpVtbl->SetTimer(This,dwFlags,llClockTime,pCallback,punkState,ppunkKey))
#define IMFTimer_CancelTimer(This,punkKey)  ((This)->lpVtbl->CancelTimer(This,punkKey))
#endif

#endif

#endif

EXTERN_GUID(MF_ACTIVATE_CUSTOM_VIDEO_MIXER_CLSID, 0xba491360, 0xbe50, 0x451e, 0x95, 0xab, 0x6d, 0x4a, 0xcc, 0xc7, 0xda, 0xd8);
EXTERN_GUID(MF_ACTIVATE_CUSTOM_VIDEO_MIXER_ACTIVATE, 0xba491361, 0xbe50, 0x451e, 0x95, 0xab, 0x6d, 0x4a, 0xcc, 0xc7, 0xda, 0xd8);
EXTERN_GUID(MF_ACTIVATE_CUSTOM_VIDEO_MIXER_FLAGS, 0xba491362, 0xbe50, 0x451e, 0x95, 0xab, 0x6d, 0x4a, 0xcc, 0xc7, 0xda, 0xd8);
EXTERN_GUID(MF_ACTIVATE_CUSTOM_VIDEO_PRESENTER_CLSID, 0xba491364, 0xbe50, 0x451e, 0x95, 0xab, 0x6d, 0x4a, 0xcc, 0xc7, 0xda, 0xd8);
EXTERN_GUID(MF_ACTIVATE_CUSTOM_VIDEO_PRESENTER_ACTIVATE, 0xba491365, 0xbe50, 0x451e, 0x95, 0xab, 0x6d, 0x4a, 0xcc, 0xc7, 0xda, 0xd8);
EXTERN_GUID(MF_ACTIVATE_CUSTOM_VIDEO_PRESENTER_FLAGS, 0xba491366, 0xbe50, 0x451e, 0x95, 0xab, 0x6d, 0x4a, 0xcc, 0xc7, 0xda, 0xd8);

enum __MIDL___MIDL_itf_mfidl_0000_0020_0001 {
    MF_ACTIVATE_CUSTOM_MIXER_ALLOWFAIL = 0x1
};

enum __MIDL___MIDL_itf_mfidl_0000_0020_0002 {
    MF_ACTIVATE_CUSTOM_PRESENTER_ALLOWFAIL = 0x1
};
EXTERN_GUID(MF_ACTIVATE_MFT_LOCKED, 0xc1f6093c, 0x7f65, 0x4fbd, 0x9e, 0x39, 0x5f, 0xae, 0xc3, 0xc4, 0xfb, 0xd7);
EXTERN_GUID(MF_ACTIVATE_VIDEO_WINDOW, 0x9a2dbbdd, 0xf57e, 0x4162, 0x82, 0xb9, 0x68, 0x31, 0x37, 0x76, 0x82, 0xd3);
typedef
    enum _MFSHUTDOWN_STATUS { MFSHUTDOWN_INITIATED = 0,
    MFSHUTDOWN_COMPLETED = (MFSHUTDOWN_INITIATED + 1)
} MFSHUTDOWN_STATUS;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0020_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0020_v0_0_s_ifspec;

#ifndef __IMFShutdown_INTERFACE_DEFINED__
#define __IMFShutdown_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFShutdown;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("97ec2ea4-0e42-4937-97ac-9d6d328824e1")IMFShutdown:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Shutdown(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetShutdownStatus(MFSHUTDOWN_STATUS *pStatus) = 0;
};
#else
typedef struct IMFShutdownVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFShutdown * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFShutdown * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFShutdown * This);
    HRESULT(STDMETHODCALLTYPE *Shutdown)(IMFShutdown * This);
    HRESULT(STDMETHODCALLTYPE *GetShutdownStatus)(IMFShutdown * This, MFSHUTDOWN_STATUS * pStatus);
    END_INTERFACE
} IMFShutdownVtbl;

interface IMFShutdown {
    CONST_VTBL struct IMFShutdownVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFShutdown_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFShutdown_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFShutdown_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFShutdown_Shutdown(This)  ((This)->lpVtbl->Shutdown(This))
#define IMFShutdown_GetShutdownStatus(This,pStatus)  ((This)->lpVtbl->GetShutdownStatus(This,pStatus))
#endif

#endif

#endif

STDAPI MFShutdownObject(IUnknown *pUnk);
STDAPI MFCreateAudioRenderer(IMFAttributes *pAudioAttributes, IMFMediaSink **ppSink);
STDAPI MFCreateAudioRendererActivate(IMFActivate **ppActivate);

EXTERN_GUID(MF_AUDIO_RENDERER_ATTRIBUTE_FLAGS, 0xede4b5e0, 0xf805, 0x4d6c, 0x99, 0xb3, 0xdb, 0x01, 0xbf, 0x95, 0xdf, 0xab);

#define MF_AUDIO_RENDERER_ATTRIBUTE_FLAGS_CROSSPROCESS  0x00000001
#define MF_AUDIO_RENDERER_ATTRIBUTE_FLAGS_NOPERSIST  0x00000002
#if (WINVER >= _WIN32_WINNT_WIN7)
#define MF_AUDIO_RENDERER_ATTRIBUTE_FLAGS_DONT_ALLOW_FORMAT_CHANGES  0x00000004
#endif /* WINVER >= _WIN32_WINNT_WIN7 */

EXTERN_GUID(MF_AUDIO_RENDERER_ATTRIBUTE_SESSION_ID, 0xede4b5e3, 0xf805, 0x4d6c, 0x99, 0xb3, 0xdb, 0x01, 0xbf, 0x95, 0xdf, 0xab);
EXTERN_GUID(MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ID, 0xb10aaec3, 0xef71, 0x4cc3, 0xb8, 0x73, 0x5, 0xa9, 0xa0, 0x8b, 0x9f, 0x8e);
EXTERN_GUID(MF_AUDIO_RENDERER_ATTRIBUTE_ENDPOINT_ROLE, 0x6ba644ff, 0x27c5, 0x4d02, 0x98, 0x87, 0xc2, 0x86, 0x19, 0xfd, 0xb9, 0x1b);
EXTERN_GUID( MF_AUDIO_RENDERER_ATTRIBUTE_STREAM_CATEGORY, 0xa9770471, 0x92ec, 0x4df4, 0x94, 0xfe, 0x81, 0xc3, 0x6f, 0xc, 0x3a, 0x7a);

STDAPI MFCreateVideoRendererActivate(HWND hwndVideo, IMFActivate **ppActivate);
#if (WINVER >= _WIN32_WINNT_WIN7)
STDAPI MFCreateMPEG4MediaSink(IMFByteStream *pIByteStream, IMFMediaType *pVideoMediaType, IMFMediaType *pAudioMediaType, IMFMediaSink **ppIMediaSink);
STDAPI MFCreate3GPMediaSink(IMFByteStream *pIByteStream, IMFMediaType *pVideoMediaType, IMFMediaType *pAudioMediaType, IMFMediaSink **ppIMediaSink);
STDAPI MFCreateMP3MediaSink(IMFByteStream *pTargetByteStream, IMFMediaSink **ppMediaSink);
#endif /* WINVER >= _WIN32_WINNT_WIN7 */
#if (WINVER >= _WIN32_WINNT_WIN8) 
STDAPI MFCreateAC3MediaSink(IMFByteStream *, IMFMediaType *, IMFMediaSink **);
STDAPI MFCreateADTSMediaSink(IMFByteStream *, IMFMediaType *, IMFMediaSink **);
STDAPI MFCreateMuxSink(GUID guidOutputSubType, IMFAttributes *, IMFByteStream *, IMFMediaSink **);
STDAPI MFCreateFMPEG4MediaSink(IMFByteStream *, IMFMediaType *, IMFMediaType *, IMFMediaSink **);
#endif /* WINVER >= _WIN32_WINNT_WIN8 */
#if (WINVER >= _WIN32_WINNT_WINBLUE)
STDAPI MFCreateAVIMediaSink(IMFByteStream *, IMFMediaType *, IMFMediaType *, IMFMediaSink **);
#endif /* WINVER >= _WIN32_WINNT_WINBLUE */
#if (WINVER >= _WIN32_WINNT_WINBLUE)
STDAPI MFCreateWAVEMediaSink(IMFByteStream *pTargeByteStream, IMFMediaType *, IMFMediaSink **);
#endif /* WINVER >= _WIN32_WINNT_WINBLUE */

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0021_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0021_v0_0_s_ifspec;

#ifndef __IMFTopoLoader_INTERFACE_DEFINED__
#define __IMFTopoLoader_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTopoLoader;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("DE9A6157-F660-4643-B56A-DF9F7998C7CD")IMFTopoLoader:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Load(IMFTopology * pInputTopo, IMFTopology ** ppOutputTopo, IMFTopology * pCurrentTopo) = 0;
};
#else
typedef struct IMFTopoLoaderVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTopoLoader * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFTopoLoader * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFTopoLoader * This);
    HRESULT(STDMETHODCALLTYPE *Load)(IMFTopoLoader * This, IMFTopology * pInputTopo, IMFTopology ** ppOutputTopo, IMFTopology * pCurrentTopo);
    END_INTERFACE
} IMFTopoLoaderVtbl;

interface IMFTopoLoader {
    CONST_VTBL struct IMFTopoLoaderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFTopoLoader_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFTopoLoader_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFTopoLoader_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFTopoLoader_Load(This,pInputTopo,ppOutputTopo,pCurrentTopo)  ((This)->lpVtbl->Load(This,pInputTopo,ppOutputTopo,pCurrentTopo))
#endif

#endif

#endif

STDAPI MFCreateTopoLoader(IMFTopoLoader **ppObj);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0022_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0022_v0_0_s_ifspec;

#ifndef __IMFContentProtectionManager_INTERFACE_DEFINED__
#define __IMFContentProtectionManager_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFContentProtectionManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("ACF92459-6A61-42bd-B57C-B43E51203CB0")IMFContentProtectionManager:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE BeginEnableContent(IMFActivate * pEnablerActivate, IMFTopology * pTopo, IMFAsyncCallback * pCallback, IUnknown * punkState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndEnableContent(IMFAsyncResult *pResult) = 0;
};
#else
typedef struct IMFContentProtectionManagerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFContentProtectionManager * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFContentProtectionManager * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFContentProtectionManager * This);
    HRESULT(STDMETHODCALLTYPE *BeginEnableContent)(IMFContentProtectionManager * This, IMFActivate * pEnablerActivate, IMFTopology * pTopo, IMFAsyncCallback * pCallback, IUnknown * punkState);
    HRESULT(STDMETHODCALLTYPE *EndEnableContent)(IMFContentProtectionManager * This, IMFAsyncResult * pResult);
    END_INTERFACE
} IMFContentProtectionManagerVtbl;

interface IMFContentProtectionManager {
    CONST_VTBL struct IMFContentProtectionManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFContentProtectionManager_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFContentProtectionManager_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFContentProtectionManager_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFContentProtectionManager_BeginEnableContent(This,pEnablerActivate,pTopo,pCallback,punkState)  ((This)->lpVtbl->BeginEnableContent(This,pEnablerActivate,pTopo,pCallback,punkState))
#define IMFContentProtectionManager_EndEnableContent(This,pResult)  ((This)->lpVtbl->EndEnableContent(This,pResult))
#endif

#endif

HRESULT STDMETHODCALLTYPE IMFContentProtectionManager_RemoteBeginEnableContent_Proxy(IMFContentProtectionManager *This, REFCLSID clsidType, BYTE *pbData, DWORD cbData, IMFRemoteAsyncCallback *pCallback);
void __RPC_STUB IMFContentProtectionManager_RemoteBeginEnableContent_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE IMFContentProtectionManager_RemoteEndEnableContent_Proxy(IMFContentProtectionManager *This, IUnknown *pResult);
void __RPC_STUB IMFContentProtectionManager_RemoteEndEnableContent_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
#endif

typedef enum __MIDL___MIDL_itf_mfidl_0000_0023_0001 {
    MF_LICENSE_URL_UNTRUSTED = 0,
    MF_LICENSE_URL_TRUSTED = (MF_LICENSE_URL_UNTRUSTED + 1),
    MF_LICENSE_URL_TAMPERED = (MF_LICENSE_URL_TRUSTED + 1)
} MF_URL_TRUST_STATUS;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0023_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0023_v0_0_s_ifspec;

#ifndef __IMFContentEnabler_INTERFACE_DEFINED__
#define __IMFContentEnabler_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFContentEnabler;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("D3C4EF59-49CE-4381-9071-D5BCD044C770")IMFContentEnabler:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetEnableType(GUID * pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEnableURL(LPWSTR *ppwszURL, DWORD *pcchURL, MF_URL_TRUST_STATUS *pTrustStatus) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEnableData(BYTE **ppbData, DWORD *pcbData) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsAutomaticSupported(BOOL *pfAutomatic) = 0;
    virtual HRESULT STDMETHODCALLTYPE AutomaticEnable(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE MonitorEnable(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Cancel(void) = 0;
};
#else
typedef struct IMFContentEnablerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFContentEnabler * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFContentEnabler * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFContentEnabler * This);
    HRESULT(STDMETHODCALLTYPE *GetEnableType)(IMFContentEnabler * This, GUID * pType);
    HRESULT(STDMETHODCALLTYPE *GetEnableURL)(IMFContentEnabler * This, LPWSTR * ppwszURL, DWORD * pcchURL, MF_URL_TRUST_STATUS * pTrustStatus);
    HRESULT(STDMETHODCALLTYPE *GetEnableData)(IMFContentEnabler * This, BYTE ** ppbData, DWORD * pcbData);
    HRESULT(STDMETHODCALLTYPE *IsAutomaticSupported)(IMFContentEnabler * This, BOOL * pfAutomatic);
    HRESULT(STDMETHODCALLTYPE *AutomaticEnable)(IMFContentEnabler * This);
    HRESULT(STDMETHODCALLTYPE *MonitorEnable)(IMFContentEnabler * This);
    HRESULT(STDMETHODCALLTYPE *Cancel)(IMFContentEnabler * This);
    END_INTERFACE
} IMFContentEnablerVtbl;

interface IMFContentEnabler {
    CONST_VTBL struct IMFContentEnablerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFContentEnabler_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFContentEnabler_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFContentEnabler_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFContentEnabler_GetEnableType(This,pType)  ((This)->lpVtbl->GetEnableType(This,pType))
#define IMFContentEnabler_GetEnableURL(This,ppwszURL,pcchURL,pTrustStatus)  ((This)->lpVtbl->GetEnableURL(This,ppwszURL,pcchURL,pTrustStatus))
#define IMFContentEnabler_GetEnableData(This,ppbData,pcbData)  ((This)->lpVtbl->GetEnableData(This,ppbData,pcbData))
#define IMFContentEnabler_IsAutomaticSupported(This,pfAutomatic)  ((This)->lpVtbl->IsAutomaticSupported(This,pfAutomatic))
#define IMFContentEnabler_AutomaticEnable(This)  ((This)->lpVtbl->AutomaticEnable(This))
#define IMFContentEnabler_MonitorEnable(This)  ((This)->lpVtbl->MonitorEnable(This))
#define IMFContentEnabler_Cancel(This)  ((This)->lpVtbl->Cancel(This))
#endif

#endif

#endif

EXTERN_GUID(MFENABLETYPE_WMDRMV1_LicenseAcquisition, 0x4ff6eeaf, 0xb43, 0x4797, 0x9b, 0x85, 0xab, 0xf3, 0x18, 0x15, 0xe7, 0xb0);
EXTERN_GUID(MFENABLETYPE_WMDRMV7_LicenseAcquisition, 0x3306df, 0x4a06, 0x4884, 0xa0, 0x97, 0xef, 0x6d, 0x22, 0xec, 0x84, 0xa3);
EXTERN_GUID(MFENABLETYPE_WMDRMV7_Individualization, 0xacd2c84a, 0xb303, 0x4f65, 0xbc, 0x2c, 0x2c, 0x84, 0x8d, 0x1, 0xa9, 0x89);
EXTERN_GUID(MFENABLETYPE_MF_UpdateRevocationInformation, 0xe558b0b5, 0xb3c4, 0x44a0, 0x92, 0x4c, 0x50, 0xd1, 0x78, 0x93, 0x23, 0x85);
EXTERN_GUID(MFENABLETYPE_MF_UpdateUntrustedComponent, 0x9879f3d6, 0xcee2, 0x48e6, 0xb5, 0x73, 0x97, 0x67, 0xab, 0x17, 0x2f, 0x16);
EXTERN_GUID(MFENABLETYPE_MF_RebootRequired, 0x6d4d3d4b, 0x0ece, 0x4652, 0x8b, 0x3a, 0xf2, 0xd2, 0x42, 0x60, 0xd8, 0x87);

#ifndef MFRR_INFO_VERSION
#define MFRR_INFO_VERSION 0
#endif

#define MF_USER_MODE_COMPONENT_LOAD  0x00000001
#define MF_KERNEL_MODE_COMPONENT_LOAD  0x00000002
#define MF_GRL_LOAD_FAILED  0x00000010
#define MF_INVALID_GRL_SIGNATURE  0x00000020
#define MF_GRL_ABSENT  0x00001000
#define MF_COMPONENT_REVOKED  0x00002000
#define MF_COMPONENT_INVALID_EKU  0x00004000
#define MF_COMPONENT_CERT_REVOKED  0x00008000
#define MF_COMPONENT_INVALID_ROOT  0x00010000
#define MF_COMPONENT_HS_CERT_REVOKED  0x00020000
#define MF_COMPONENT_LS_CERT_REVOKED  0x00040000
#define MF_BOOT_DRIVER_VERIFICATION_FAILED  0x00100000
#define MF_TEST_SIGNED_COMPONENT_LOADING   0x01000000
#define MF_MINCRYPT_FAILURE  0x10000000

#define SHA_HASH_LEN  20
#define STR_HASH_LEN  (SHA_HASH_LEN*2 + 3)
typedef struct _MFRR_COMPONENT_HASH_INFO {
    DWORD ulReason;
    WCHAR rgHeaderHash[STR_HASH_LEN];
    WCHAR rgPublicKeyHash[STR_HASH_LEN];
    WCHAR wszName[MAX_PATH];
} MFRR_COMPONENT_HASH_INFO, *PMFRR_COMPONENT_HASH_INFO;

typedef struct _MFRR_COMPONENTS {
    DWORD dwRRInfoVersion;
    DWORD dwRRComponents;
    PMFRR_COMPONENT_HASH_INFO pRRComponents;
} MFRR_COMPONENTS, *PMFRR_COMPONENTS;

#pragma pack (push)
#pragma pack (1)
typedef struct _ASFFlatPicture {
    BYTE bPictureType;
    DWORD dwDataLen;
} ASF_FLAT_PICTURE;
#pragma pack (pop)

#pragma pack (push)
#pragma pack (1)
typedef struct _ASFFlatSynchronisedLyrics {
    BYTE bTimeStampFormat;
    BYTE bContentType;
    DWORD dwLyricsLen;
} ASF_FLAT_SYNCHRONISED_LYRICS;
#pragma pack (pop)

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0024_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0024_v0_0_s_ifspec;

#ifndef __IMFMetadata_INTERFACE_DEFINED__
#define __IMFMetadata_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFMetadata;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("F88CFB8C-EF16-4991-B450-CB8C69E51704")IMFMetadata:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetLanguage(LPCWSTR pwszRFC1766) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLanguage(LPWSTR *ppwszRFC1766) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAllLanguages(PROPVARIANT *ppvLanguages) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProperty(LPCWSTR pwszName, const PROPVARIANT *ppvValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(LPCWSTR pwszName, PROPVARIANT *ppvValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteProperty(LPCWSTR pwszName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAllPropertyNames(PROPVARIANT *ppvNames) = 0;
};
#else
typedef struct IMFMetadataVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFMetadata * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFMetadata * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFMetadata * This);
    HRESULT(STDMETHODCALLTYPE *SetLanguage)(IMFMetadata * This, LPCWSTR pwszRFC1766);
    HRESULT(STDMETHODCALLTYPE *GetLanguage)(IMFMetadata * This, LPWSTR * ppwszRFC1766);
    HRESULT(STDMETHODCALLTYPE *GetAllLanguages)(IMFMetadata * This, PROPVARIANT * ppvLanguages);
    HRESULT(STDMETHODCALLTYPE *SetProperty)(IMFMetadata * This, LPCWSTR pwszName, const PROPVARIANT * ppvValue);
    HRESULT(STDMETHODCALLTYPE *GetProperty)(IMFMetadata * This, LPCWSTR pwszName, PROPVARIANT * ppvValue);
    HRESULT(STDMETHODCALLTYPE *DeleteProperty)(IMFMetadata * This, LPCWSTR pwszName);
    HRESULT(STDMETHODCALLTYPE *GetAllPropertyNames)(IMFMetadata * This, PROPVARIANT * ppvNames);
    END_INTERFACE
} IMFMetadataVtbl;

interface IMFMetadata {
    CONST_VTBL struct IMFMetadataVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFMetadata_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFMetadata_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFMetadata_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFMetadata_SetLanguage(This,pwszRFC1766)  ((This)->lpVtbl->SetLanguage(This,pwszRFC1766))
#define IMFMetadata_GetLanguage(This,ppwszRFC1766)  ((This)->lpVtbl->GetLanguage(This,ppwszRFC1766))
#define IMFMetadata_GetAllLanguages(This,ppvLanguages)  ((This)->lpVtbl->GetAllLanguages(This,ppvLanguages))
#define IMFMetadata_SetProperty(This,pwszName,ppvValue)  ((This)->lpVtbl->SetProperty(This,pwszName,ppvValue))
#define IMFMetadata_GetProperty(This,pwszName,ppvValue)  ((This)->lpVtbl->GetProperty(This,pwszName,ppvValue))
#define IMFMetadata_DeleteProperty(This,pwszName)  ((This)->lpVtbl->DeleteProperty(This,pwszName))
#define IMFMetadata_GetAllPropertyNames(This,ppvNames)  ((This)->lpVtbl->GetAllPropertyNames(This,ppvNames))
#endif

#endif

#endif

#ifndef __IMFMetadataProvider_INTERFACE_DEFINED__
#define __IMFMetadataProvider_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFMetadataProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("56181D2D-E221-4adb-B1C8-3CEE6A53F76F")IMFMetadataProvider:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetMFMetadata(IMFPresentationDescriptor * pPresentationDescriptor, DWORD dwStreamIdentifier, DWORD dwFlags, IMFMetadata ** ppMFMetadata) = 0;
};
#else
typedef struct IMFMetadataProviderVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFMetadataProvider * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFMetadataProvider * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFMetadataProvider * This);
    HRESULT(STDMETHODCALLTYPE *GetMFMetadata)(IMFMetadataProvider * This, IMFPresentationDescriptor * pPresentationDescriptor, DWORD dwStreamIdentifier, DWORD dwFlags, IMFMetadata ** ppMFMetadata);
    END_INTERFACE
} IMFMetadataProviderVtbl;

interface IMFMetadataProvider {
    CONST_VTBL struct IMFMetadataProviderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFMetadataProvider_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFMetadataProvider_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFMetadataProvider_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFMetadataProvider_GetMFMetadata(This,pPresentationDescriptor,dwStreamIdentifier,dwFlags,ppMFMetadata)  ((This)->lpVtbl->GetMFMetadata(This,pPresentationDescriptor,dwStreamIdentifier,dwFlags,ppMFMetadata))
#endif

#endif

#endif

EXTERN_GUID(MF_METADATA_PROVIDER_SERVICE, 0xdb214084, 0x58a4, 0x4d2e, 0xb8, 0x4f, 0x6f, 0x75, 0x5b, 0x2f, 0x7a, 0xd);
#if (WINVER >= _WIN32_WINNT_WIN7)
EXTERN_GUID(MF_PROPERTY_HANDLER_SERVICE, 0xa3face02, 0x32b8, 0x41dd, 0x90, 0xe7, 0x5f, 0xef, 0x7c, 0x89, 0x91, 0xb5);
#endif

typedef enum _MFRATE_DIRECTION {
    MFRATE_FORWARD = 0,
    MFRATE_REVERSE = (MFRATE_FORWARD + 1)
} MFRATE_DIRECTION;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0026_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0026_v0_0_s_ifspec;

#ifndef __IMFRateSupport_INTERFACE_DEFINED__
#define __IMFRateSupport_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFRateSupport;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0a9ccdbc-d797-4563-9667-94ec5d79292d")IMFRateSupport:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetSlowestRate(MFRATE_DIRECTION eDirection, BOOL fThin, float *pflRate) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFastestRate(MFRATE_DIRECTION eDirection, BOOL fThin, float *pflRate) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsRateSupported(BOOL fThin, float flRate, float *pflNearestSupportedRate) = 0;
};
#else
typedef struct IMFRateSupportVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFRateSupport * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFRateSupport * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFRateSupport * This);
    HRESULT(STDMETHODCALLTYPE *GetSlowestRate)(IMFRateSupport * This, MFRATE_DIRECTION eDirection, BOOL fThin, float *pflRate);
    HRESULT(STDMETHODCALLTYPE *GetFastestRate)(IMFRateSupport * This, MFRATE_DIRECTION eDirection, BOOL fThin, float *pflRate);
    HRESULT(STDMETHODCALLTYPE *IsRateSupported)(IMFRateSupport * This, BOOL fThin, float flRate, float *pflNearestSupportedRate);
    END_INTERFACE
} IMFRateSupportVtbl;

interface IMFRateSupport {
    CONST_VTBL struct IMFRateSupportVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFRateSupport_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFRateSupport_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFRateSupport_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFRateSupport_GetSlowestRate(This,eDirection,fThin,pflRate)  ((This)->lpVtbl->GetSlowestRate(This,eDirection,fThin,pflRate))
#define IMFRateSupport_GetFastestRate(This,eDirection,fThin,pflRate)  ((This)->lpVtbl->GetFastestRate(This,eDirection,fThin,pflRate))
#define IMFRateSupport_IsRateSupported(This,fThin,flRate,pflNearestSupportedRate)  ((This)->lpVtbl->IsRateSupported(This,fThin,flRate,pflNearestSupportedRate))
#endif

#endif

#endif

EXTERN_GUID(MF_RATE_CONTROL_SERVICE, 0x866fa297, 0xb802, 0x4bf8, 0x9d, 0xc9, 0x5e, 0x3b, 0x6a, 0x9f, 0x53, 0xc9);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0027_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0027_v0_0_s_ifspec;

#ifndef __IMFRateControl_INTERFACE_DEFINED__
#define __IMFRateControl_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFRateControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("88ddcd21-03c3-4275-91ed-55ee3929328f")IMFRateControl:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetRate(BOOL fThin, float flRate) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRate(BOOL *pfThin, float *pflRate) = 0;
};
#else
typedef struct IMFRateControlVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFRateControl * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFRateControl * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFRateControl * This);
    HRESULT(STDMETHODCALLTYPE *SetRate)(IMFRateControl * This, BOOL fThin, float flRate);
    HRESULT(STDMETHODCALLTYPE *GetRate)(IMFRateControl * This, BOOL * pfThin, float *pflRate);
    END_INTERFACE
} IMFRateControlVtbl;

interface IMFRateControl {
    CONST_VTBL struct IMFRateControlVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFRateControl_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFRateControl_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFRateControl_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFRateControl_SetRate(This,fThin,flRate)  ((This)->lpVtbl->SetRate(This,fThin,flRate))
#define IMFRateControl_GetRate(This,pfThin,pflRate)  ((This)->lpVtbl->GetRate(This,pfThin,pflRate))
#endif

#endif

#endif

#if (WINVER >= _WIN32_WINNT_WIN7)

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0028_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0028_v0_0_s_ifspec;

#ifndef __IMFTimecodeTranslate_INTERFACE_DEFINED__
#define __IMFTimecodeTranslate_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTimecodeTranslate;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("ab9d8661-f7e8-4ef4-9861-89f334f94e74")IMFTimecodeTranslate:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE BeginConvertTimecodeToHNS(const PROPVARIANT *pPropVarTimecode, IMFAsyncCallback *pCallback, IUnknown *punkState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndConvertTimecodeToHNS(IMFAsyncResult *pResult, MFTIME *phnsTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE BeginConvertHNSToTimecode(MFTIME hnsTime, IMFAsyncCallback *pCallback, IUnknown *punkState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndConvertHNSToTimecode(IMFAsyncResult *pResult, PROPVARIANT *pPropVarTimecode) = 0;
};
#else
typedef struct IMFTimecodeTranslateVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTimecodeTranslate * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFTimecodeTranslate * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFTimecodeTranslate * This);
    HRESULT(STDMETHODCALLTYPE *BeginConvertTimecodeToHNS)(IMFTimecodeTranslate * This, const PROPVARIANT * pPropVarTimecode, IMFAsyncCallback * pCallback, IUnknown * punkState);
    HRESULT(STDMETHODCALLTYPE *EndConvertTimecodeToHNS)(IMFTimecodeTranslate * This, IMFAsyncResult * pResult, MFTIME * phnsTime);
    HRESULT(STDMETHODCALLTYPE *BeginConvertHNSToTimecode)(IMFTimecodeTranslate * This, MFTIME hnsTime, IMFAsyncCallback * pCallback, IUnknown * punkState);
    HRESULT(STDMETHODCALLTYPE *EndConvertHNSToTimecode)(IMFTimecodeTranslate * This, IMFAsyncResult * pResult, PROPVARIANT * pPropVarTimecode);
    END_INTERFACE
} IMFTimecodeTranslateVtbl;

interface IMFTimecodeTranslate {
    CONST_VTBL struct IMFTimecodeTranslateVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFTimecodeTranslate_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFTimecodeTranslate_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFTimecodeTranslate_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFTimecodeTranslate_BeginConvertTimecodeToHNS(This,pPropVarTimecode,pCallback,punkState)  ((This)->lpVtbl->BeginConvertTimecodeToHNS(This,pPropVarTimecode,pCallback,punkState))
#define IMFTimecodeTranslate_EndConvertTimecodeToHNS(This,pResult,phnsTime)  ((This)->lpVtbl->EndConvertTimecodeToHNS(This,pResult,phnsTime))
#define IMFTimecodeTranslate_BeginConvertHNSToTimecode(This,hnsTime,pCallback,punkState)  ((This)->lpVtbl->BeginConvertHNSToTimecode(This,hnsTime,pCallback,punkState))
#define IMFTimecodeTranslate_EndConvertHNSToTimecode(This,pResult,pPropVarTimecode)  ((This)->lpVtbl->EndConvertHNSToTimecode(This,pResult,pPropVarTimecode))
#endif

#endif

#endif

EXTERN_GUID(MF_TIMECODE_SERVICE, 0xa0d502a7, 0x0eb3, 0x4885, 0xb1, 0xb9, 0x9f, 0xeb, 0x0d, 0x08, 0x34, 0x54);
#endif

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0029_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0029_v0_0_s_ifspec;

#if (WINVER >= _WIN32_WINNT_WIN8) 

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0033_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0033_v0_0_s_ifspec;

#ifndef __IMFSeekInfo_INTERFACE_DEFINED__
#define __IMFSeekInfo_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSeekInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("26AFEA53-D9ED-42B5-AB80-E64F9EE34779")IMFSeekInfo:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetNearestKeyFrames(const GUID *, const PROPVARIANT *, PROPVARIANT *, PROPVARIANT *) = 0;

};
#else /* C style interface */
typedef struct IMFSeekInfoVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(IMFSeekInfo *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(IMFSeekInfo *);
    ULONG (STDMETHODCALLTYPE *Release)(IMFSeekInfo *);
    HRESULT (STDMETHODCALLTYPE *GetNearestKeyFrames)(IMFSeekInfo *, const GUID *, const PROPVARIANT *, PROPVARIANT *, PROPVARIANT *);
    END_INTERFACE
} IMFSeekInfoVtbl;

interface IMFSeekInfo {
    CONST_VTBL struct IMFSeekInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSeekInfo_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFSeekInfo_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFSeekInfo_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFSeekInfo_GetNearestKeyFrames(This,pguidTimeFormat,pvarStartPosition,pvarPreviousKeyFrame,pvarNextKeyFrame)  ((This)->lpVtbl->GetNearestKeyFrames(This,pguidTimeFormat,pvarStartPosition,pvarPreviousKeyFrame,pvarNextKeyFrame)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFSeekInfo_INTERFACE_DEFINED__ */

EXTERN_C const GUID MF_SCRUBBING_SERVICE;
#endif /* WINVER >= _WIN32_WINNT_WIN8 */

#ifndef __IMFSimpleAudioVolume_INTERFACE_DEFINED__
#define __IMFSimpleAudioVolume_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSimpleAudioVolume;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("089EDF13-CF71-4338-8D13-9E569DBDC319")IMFSimpleAudioVolume:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetMasterVolume(float fLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMasterVolume(float *pfLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetMute(const BOOL bMute) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMute(BOOL *pbMute) = 0;
};

#else

typedef struct IMFSimpleAudioVolumeVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSimpleAudioVolume * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSimpleAudioVolume * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSimpleAudioVolume * This);
    HRESULT(STDMETHODCALLTYPE *SetMasterVolume)(IMFSimpleAudioVolume * This, float fLevel);
    HRESULT(STDMETHODCALLTYPE *GetMasterVolume)(IMFSimpleAudioVolume * This, float *pfLevel);
    HRESULT(STDMETHODCALLTYPE *SetMute)(IMFSimpleAudioVolume * This, const BOOL bMute);
    HRESULT(STDMETHODCALLTYPE *GetMute)(IMFSimpleAudioVolume * This, BOOL * pbMute);
    END_INTERFACE
} IMFSimpleAudioVolumeVtbl;

interface IMFSimpleAudioVolume {
    CONST_VTBL struct IMFSimpleAudioVolumeVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSimpleAudioVolume_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSimpleAudioVolume_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSimpleAudioVolume_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFSimpleAudioVolume_SetMasterVolume(This,fLevel)  ((This)->lpVtbl->SetMasterVolume(This,fLevel))
#define IMFSimpleAudioVolume_GetMasterVolume(This,pfLevel)  ((This)->lpVtbl->GetMasterVolume(This,pfLevel))
#define IMFSimpleAudioVolume_SetMute(This,bMute)  ((This)->lpVtbl->SetMute(This,bMute))
#define IMFSimpleAudioVolume_GetMute(This,pbMute)  ((This)->lpVtbl->GetMute(This,pbMute))
#endif

#endif

#endif

EXTERN_GUID(MR_POLICY_VOLUME_SERVICE, 0x1abaa2ac, 0x9d3b, 0x47c6, 0xab, 0x48, 0xc5, 0x95, 0x6, 0xde, 0x78, 0x4d);
#if (WINVER >= _WIN32_WINNT_WIN8) 
EXTERN_GUID(MR_CAPTURE_POLICY_VOLUME_SERVICE, 0x24030acd, 0x107a, 0x4265, 0x97, 0x5c, 0x41, 0x4e, 0x33, 0xe6, 0x5f, 0x2a);
#endif // (WINVER >= _WIN32_WINNT_WIN8) 

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0030_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0030_v0_0_s_ifspec;

#ifndef __IMFAudioStreamVolume_INTERFACE_DEFINED__
#define __IMFAudioStreamVolume_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFAudioStreamVolume;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("76B1BBDB-4EC8-4f36-B106-70A9316DF593")IMFAudioStreamVolume:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetChannelCount(UINT32 * pdwCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetChannelVolume(UINT32 dwIndex, const float fLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetChannelVolume(UINT32 dwIndex, float *pfLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetAllVolumes(UINT32 dwCount, const float *pfVolumes) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAllVolumes(UINT32 dwCount, float *pfVolumes) = 0;
};
#else
typedef struct IMFAudioStreamVolumeVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFAudioStreamVolume * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFAudioStreamVolume * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFAudioStreamVolume * This);
    HRESULT(STDMETHODCALLTYPE *GetChannelCount)(IMFAudioStreamVolume * This, UINT32 * pdwCount);
    HRESULT(STDMETHODCALLTYPE *SetChannelVolume)(IMFAudioStreamVolume * This, UINT32 dwIndex, const float fLevel);
    HRESULT(STDMETHODCALLTYPE *GetChannelVolume)(IMFAudioStreamVolume * This, UINT32 dwIndex, float *pfLevel);
    HRESULT(STDMETHODCALLTYPE *SetAllVolumes)(IMFAudioStreamVolume * This, UINT32 dwCount, const float *pfVolumes);
    HRESULT(STDMETHODCALLTYPE *GetAllVolumes)(IMFAudioStreamVolume * This, UINT32 dwCount, float *pfVolumes);
    END_INTERFACE
} IMFAudioStreamVolumeVtbl;

interface IMFAudioStreamVolume {
    CONST_VTBL struct IMFAudioStreamVolumeVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFAudioStreamVolume_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFAudioStreamVolume_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFAudioStreamVolume_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFAudioStreamVolume_GetChannelCount(This,pdwCount)  ((This)->lpVtbl->GetChannelCount(This,pdwCount))
#define IMFAudioStreamVolume_SetChannelVolume(This,dwIndex,fLevel)  ((This)->lpVtbl->SetChannelVolume(This,dwIndex,fLevel))
#define IMFAudioStreamVolume_GetChannelVolume(This,dwIndex,pfLevel)  ((This)->lpVtbl->GetChannelVolume(This,dwIndex,pfLevel))
#define IMFAudioStreamVolume_SetAllVolumes(This,dwCount,pfVolumes)  ((This)->lpVtbl->SetAllVolumes(This,dwCount,pfVolumes))
#define IMFAudioStreamVolume_GetAllVolumes(This,dwCount,pfVolumes)  ((This)->lpVtbl->GetAllVolumes(This,dwCount,pfVolumes))
#endif

#endif

#endif

EXTERN_GUID(MR_STREAM_VOLUME_SERVICE, 0xf8b5fa2f, 0x32ef, 0x46f5, 0xb1, 0x72, 0x13, 0x21, 0x21, 0x2f, 0xb2, 0xc4);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0031_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0031_v0_0_s_ifspec;

#ifndef __IMFAudioPolicy_INTERFACE_DEFINED__
#define __IMFAudioPolicy_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFAudioPolicy;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a0638c2b-6465-4395-9ae7-a321a9fd2856")IMFAudioPolicy:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetGroupingParam(REFGUID rguidClass) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetGroupingParam(GUID *pguidClass) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDisplayName(LPCWSTR pszName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDisplayName(LPWSTR *pszName) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetIconPath(LPCWSTR pszPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetIconPath(LPWSTR *pszPath) = 0;
};
#else
typedef struct IMFAudioPolicyVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFAudioPolicy * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFAudioPolicy * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFAudioPolicy * This);
    HRESULT(STDMETHODCALLTYPE *SetGroupingParam)(IMFAudioPolicy * This, REFGUID rguidClass);
    HRESULT(STDMETHODCALLTYPE *GetGroupingParam)(IMFAudioPolicy * This, GUID * pguidClass);
    HRESULT(STDMETHODCALLTYPE *SetDisplayName)(IMFAudioPolicy * This, LPCWSTR pszName);
    HRESULT(STDMETHODCALLTYPE *GetDisplayName)(IMFAudioPolicy * This, LPWSTR * pszName);
    HRESULT(STDMETHODCALLTYPE *SetIconPath)(IMFAudioPolicy * This, LPCWSTR pszPath);
    HRESULT(STDMETHODCALLTYPE *GetIconPath)(IMFAudioPolicy * This, LPWSTR * pszPath);
    END_INTERFACE
} IMFAudioPolicyVtbl;

interface IMFAudioPolicy {
    CONST_VTBL struct IMFAudioPolicyVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFAudioPolicy_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFAudioPolicy_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFAudioPolicy_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFAudioPolicy_SetGroupingParam(This,rguidClass)  ((This)->lpVtbl->SetGroupingParam(This,rguidClass))
#define IMFAudioPolicy_GetGroupingParam(This,pguidClass)  ((This)->lpVtbl->GetGroupingParam(This,pguidClass))
#define IMFAudioPolicy_SetDisplayName(This,pszName)  ((This)->lpVtbl->SetDisplayName(This,pszName))
#define IMFAudioPolicy_GetDisplayName(This,pszName)  ((This)->lpVtbl->GetDisplayName(This,pszName))
#define IMFAudioPolicy_SetIconPath(This,pszPath)  ((This)->lpVtbl->SetIconPath(This,pszPath))
#define IMFAudioPolicy_GetIconPath(This,pszPath)  ((This)->lpVtbl->GetIconPath(This,pszPath))
#endif

#endif

#endif

EXTERN_GUID(MR_AUDIO_POLICY_SERVICE, 0x911fd737, 0x6775, 0x4ab0, 0xa6, 0x14, 0x29, 0x78, 0x62, 0xfd, 0xac, 0x88);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0032_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0032_v0_0_s_ifspec;

#ifndef __IMFSampleGrabberSinkCallback_INTERFACE_DEFINED__
#define __IMFSampleGrabberSinkCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSampleGrabberSinkCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("8C7B80BF-EE42-4b59-B1DF-55668E1BDCA8")IMFSampleGrabberSinkCallback:public IMFClockStateSink
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnSetPresentationClock(IMFPresentationClock * pPresentationClock) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnProcessSample(REFGUID guidMajorMediaType, DWORD dwSampleFlags, LONGLONG llSampleTime, LONGLONG llSampleDuration, const BYTE *pSampleBuffer, DWORD dwSampleSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnShutdown(void) = 0;
};
#else
typedef struct IMFSampleGrabberSinkCallbackVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSampleGrabberSinkCallback * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSampleGrabberSinkCallback * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSampleGrabberSinkCallback * This);
    HRESULT(STDMETHODCALLTYPE *OnClockStart)(IMFSampleGrabberSinkCallback * This, MFTIME hnsSystemTime, LONGLONG llClockStartOffset);
    HRESULT(STDMETHODCALLTYPE *OnClockStop)(IMFSampleGrabberSinkCallback * This, MFTIME hnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *OnClockPause)(IMFSampleGrabberSinkCallback * This, MFTIME hnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *OnClockRestart)(IMFSampleGrabberSinkCallback * This, MFTIME hnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *OnClockSetRate)(IMFSampleGrabberSinkCallback * This, MFTIME hnsSystemTime, float flRate);
    HRESULT(STDMETHODCALLTYPE *OnSetPresentationClock)(IMFSampleGrabberSinkCallback * This, IMFPresentationClock * pPresentationClock);
    HRESULT(STDMETHODCALLTYPE *OnProcessSample)(IMFSampleGrabberSinkCallback * This, REFGUID guidMajorMediaType, DWORD dwSampleFlags, LONGLONG llSampleTime, LONGLONG llSampleDuration, const BYTE * pSampleBuffer, DWORD dwSampleSize);
    HRESULT(STDMETHODCALLTYPE *OnShutdown)(IMFSampleGrabberSinkCallback * This);
    END_INTERFACE
} IMFSampleGrabberSinkCallbackVtbl;

interface IMFSampleGrabberSinkCallback {
    CONST_VTBL struct IMFSampleGrabberSinkCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSampleGrabberSinkCallback_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSampleGrabberSinkCallback_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSampleGrabberSinkCallback_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFSampleGrabberSinkCallback_OnClockStart(This,hnsSystemTime,llClockStartOffset)  ((This)->lpVtbl->OnClockStart(This,hnsSystemTime,llClockStartOffset))
#define IMFSampleGrabberSinkCallback_OnClockStop(This,hnsSystemTime)  ((This)->lpVtbl->OnClockStop(This,hnsSystemTime))
#define IMFSampleGrabberSinkCallback_OnClockPause(This,hnsSystemTime)  ((This)->lpVtbl->OnClockPause(This,hnsSystemTime))
#define IMFSampleGrabberSinkCallback_OnClockRestart(This,hnsSystemTime)  ((This)->lpVtbl->OnClockRestart(This,hnsSystemTime))
#define IMFSampleGrabberSinkCallback_OnClockSetRate(This,hnsSystemTime,flRate)  ((This)->lpVtbl->OnClockSetRate(This,hnsSystemTime,flRate))
#define IMFSampleGrabberSinkCallback_OnSetPresentationClock(This,pPresentationClock)  ((This)->lpVtbl->OnSetPresentationClock(This,pPresentationClock))
#define IMFSampleGrabberSinkCallback_OnProcessSample(This,guidMajorMediaType,dwSampleFlags,llSampleTime,llSampleDuration,pSampleBuffer,dwSampleSize)  ((This)->lpVtbl->OnProcessSample(This,guidMajorMediaType,dwSampleFlags,llSampleTime,llSampleDuration,pSampleBuffer, dwSampleSize))
#define IMFSampleGrabberSinkCallback_OnShutdown(This)  ((This)->lpVtbl->OnShutdown(This))
#endif

#endif

#endif

STDAPI MFCreateSampleGrabberSinkActivate(IMFMediaType *pIMFMediaType, IMFSampleGrabberSinkCallback *pIMFSampleGrabberSinkCallback, IMFActivate **ppIActivate);

EXTERN_GUID(MF_SAMPLEGRABBERSINK_SAMPLE_TIME_OFFSET, 0x62e3d776, 0x8100, 0x4e03, 0xa6, 0xe8, 0xbd, 0x38, 0x57, 0xac, 0x9c, 0x47);
#if (WINVER >= _WIN32_WINNT_WIN7)
EXTERN_GUID(MF_SAMPLEGRABBERSINK_IGNORE_CLOCK, 0x0efda2c0, 0x2b69, 0x4e2e, 0xab, 0x8d, 0x46, 0xdc, 0xbf, 0xf7, 0xd2, 0x5d);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0033_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0033_v0_0_s_ifspec;

#ifndef __IMFSampleGrabberSinkCallback2_INTERFACE_DEFINED__
#define __IMFSampleGrabberSinkCallback2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSampleGrabberSinkCallback2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("ca86aa50-c46e-429e-ab27-16d6ac6844cb")IMFSampleGrabberSinkCallback2:public IMFSampleGrabberSinkCallback
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnProcessSampleEx(REFGUID guidMajorMediaType, DWORD dwSampleFlags, LONGLONG llSampleTime, LONGLONG llSampleDuration, const BYTE *pSampleBuffer, DWORD dwSampleSize, IMFAttributes *pAttributes) = 0;
};
#else
typedef struct IMFSampleGrabberSinkCallback2Vtbl
{
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(IMFSampleGrabberSinkCallback2 * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSampleGrabberSinkCallback2 * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSampleGrabberSinkCallback2 * This);
    HRESULT(STDMETHODCALLTYPE *OnClockStart)(IMFSampleGrabberSinkCallback2 * This, MFTIME hnsSystemTime, LONGLONG llClockStartOffset);
    HRESULT(STDMETHODCALLTYPE *OnClockStop)(IMFSampleGrabberSinkCallback2 * This, MFTIME hnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *OnClockPause)(IMFSampleGrabberSinkCallback2 * This, MFTIME hnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *OnClockRestart)(IMFSampleGrabberSinkCallback2 * This, MFTIME hnsSystemTime);
    HRESULT(STDMETHODCALLTYPE *OnClockSetRate)(IMFSampleGrabberSinkCallback2 * This, MFTIME hnsSystemTime, float flRate);
    HRESULT(STDMETHODCALLTYPE *OnSetPresentationClock)(IMFSampleGrabberSinkCallback2 * This, IMFPresentationClock * pPresentationClock);
    HRESULT(STDMETHODCALLTYPE *OnProcessSample)(IMFSampleGrabberSinkCallback2 * This, REFGUID guidMajorMediaType, DWORD dwSampleFlags, LONGLONG llSampleTime, LONGLONG llSampleDuration, const BYTE * pSampleBuffer, DWORD dwSampleSize);
    HRESULT(STDMETHODCALLTYPE *OnShutdown)(IMFSampleGrabberSinkCallback2 * This);
    HRESULT(STDMETHODCALLTYPE *OnProcessSampleEx)(IMFSampleGrabberSinkCallback2 * This, REFGUID guidMajorMediaType, DWORD dwSampleFlags, LONGLONG llSampleTime, LONGLONG llSampleDuration, const BYTE * pSampleBuffer, DWORD dwSampleSize, IMFAttributes * pAttributes);
    END_INTERFACE
} IMFSampleGrabberSinkCallback2Vtbl;

interface IMFSampleGrabberSinkCallback2 {
    CONST_VTBL struct IMFSampleGrabberSinkCallback2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSampleGrabberSinkCallback2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSampleGrabberSinkCallback2_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSampleGrabberSinkCallback2_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFSampleGrabberSinkCallback2_OnClockStart(This,hnsSystemTime,llClockStartOffset)  ((This)->lpVtbl->OnClockStart(This,hnsSystemTime,llClockStartOffset))
#define IMFSampleGrabberSinkCallback2_OnClockStop(This,hnsSystemTime)  ((This)->lpVtbl->OnClockStop(This,hnsSystemTime))
#define IMFSampleGrabberSinkCallback2_OnClockPause(This,hnsSystemTime)  ((This)->lpVtbl->OnClockPause(This,hnsSystemTime))
#define IMFSampleGrabberSinkCallback2_OnClockRestart(This,hnsSystemTime)  ((This)->lpVtbl->OnClockRestart(This,hnsSystemTime))
#define IMFSampleGrabberSinkCallback2_OnClockSetRate(This,hnsSystemTime,flRate)  ((This)->lpVtbl->OnClockSetRate(This,hnsSystemTime,flRate))
#define IMFSampleGrabberSinkCallback2_OnSetPresentationClock(This,pPresentationClock)  ((This)->lpVtbl->OnSetPresentationClock(This,pPresentationClock))
#define IMFSampleGrabberSinkCallback2_OnProcessSample(This,guidMajorMediaType,dwSampleFlags,llSampleTime,llSampleDuration,pSampleBuffer,dwSampleSize)  ((This)->lpVtbl->OnProcessSample(This,guidMajorMediaType,dwSampleFlags,llSampleTime,llSampleDuration,pSampleBuffer, dwSampleSize))
#define IMFSampleGrabberSinkCallback2_OnShutdown(This)  ((This)->lpVtbl->OnShutdown(This))
#define IMFSampleGrabberSinkCallback2_OnProcessSampleEx(This,guidMajorMediaType,dwSampleFlags,llSampleTime,llSampleDuration,pSampleBuffer,dwSampleSize,pAttributes)  ((This)->lpVtbl->OnProcessSampleEx(This,guidMajorMediaType,dwSampleFlags,llSampleTime,llSampleDuration, pSampleBuffer, dwSampleSize, pAttributes))
#endif

#endif

#endif

#endif

EXTERN_GUID(MF_QUALITY_SERVICES, 0xb7e2be11, 0x2f96, 0x4640, 0xb5, 0x2c, 0x28, 0x23, 0x65, 0xbd, 0xf1, 0x6c);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0034_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0034_v0_0_s_ifspec;

#ifndef __IMFWorkQueueServices_INTERFACE_DEFINED__
#define __IMFWorkQueueServices_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFWorkQueueServices;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("35FE1BB8-A3A9-40fe-BBEC-EB569C9CCCA3")IMFWorkQueueServices:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE BeginRegisterTopologyWorkQueuesWithMMCSS(IMFAsyncCallback * pCallback, IUnknown * pState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndRegisterTopologyWorkQueuesWithMMCSS(IMFAsyncResult *pResult) = 0;
    virtual HRESULT STDMETHODCALLTYPE BeginUnregisterTopologyWorkQueuesWithMMCSS(IMFAsyncCallback *pCallback, IUnknown *pState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndUnregisterTopologyWorkQueuesWithMMCSS(IMFAsyncResult *pResult) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTopologyWorkQueueMMCSSClass(DWORD dwTopologyWorkQueueId, LPWSTR pwszClass, DWORD *pcchClass) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTopologyWorkQueueMMCSSTaskId(DWORD dwTopologyWorkQueueId, DWORD *pdwTaskId) = 0;
    virtual HRESULT STDMETHODCALLTYPE BeginRegisterPlatformWorkQueueWithMMCSS(DWORD dwPlatformWorkQueue, LPCWSTR wszClass, DWORD dwTaskId, IMFAsyncCallback *pCallback, IUnknown *pState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndRegisterPlatformWorkQueueWithMMCSS(IMFAsyncResult *pResult, DWORD *pdwTaskId) = 0;
    virtual HRESULT STDMETHODCALLTYPE BeginUnregisterPlatformWorkQueueWithMMCSS(DWORD dwPlatformWorkQueue, IMFAsyncCallback *pCallback, IUnknown *pState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndUnregisterPlatformWorkQueueWithMMCSS(IMFAsyncResult *pResult) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPlaftormWorkQueueMMCSSClass(DWORD dwPlatformWorkQueueId, LPWSTR pwszClass, DWORD *pcchClass) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPlatformWorkQueueMMCSSTaskId(DWORD dwPlatformWorkQueueId, DWORD *pdwTaskId) = 0;
};
#else
typedef struct IMFWorkQueueServicesVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFWorkQueueServices * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFWorkQueueServices * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFWorkQueueServices * This);
    HRESULT(STDMETHODCALLTYPE *BeginRegisterTopologyWorkQueuesWithMMCSS)(IMFWorkQueueServices * This, IMFAsyncCallback * pCallback, IUnknown * pState);
    HRESULT(STDMETHODCALLTYPE *EndRegisterTopologyWorkQueuesWithMMCSS)(IMFWorkQueueServices * This, IMFAsyncResult * pResult);
    HRESULT(STDMETHODCALLTYPE *BeginUnregisterTopologyWorkQueuesWithMMCSS)(IMFWorkQueueServices * This, IMFAsyncCallback * pCallback, IUnknown * pState);
    HRESULT(STDMETHODCALLTYPE *EndUnregisterTopologyWorkQueuesWithMMCSS)(IMFWorkQueueServices * This, IMFAsyncResult * pResult);
    HRESULT(STDMETHODCALLTYPE *GetTopologyWorkQueueMMCSSClass)(IMFWorkQueueServices * This, DWORD dwTopologyWorkQueueId, LPWSTR pwszClass, DWORD * pcchClass);
    HRESULT(STDMETHODCALLTYPE *GetTopologyWorkQueueMMCSSTaskId)(IMFWorkQueueServices * This, DWORD dwTopologyWorkQueueId, DWORD * pdwTaskId);
    HRESULT(STDMETHODCALLTYPE *BeginRegisterPlatformWorkQueueWithMMCSS)(IMFWorkQueueServices * This, DWORD dwPlatformWorkQueue, LPCWSTR wszClass, DWORD dwTaskId, IMFAsyncCallback * pCallback, IUnknown * pState);
    HRESULT(STDMETHODCALLTYPE *EndRegisterPlatformWorkQueueWithMMCSS)(IMFWorkQueueServices * This, IMFAsyncResult * pResult, DWORD * pdwTaskId);
    HRESULT(STDMETHODCALLTYPE *BeginUnregisterPlatformWorkQueueWithMMCSS)(IMFWorkQueueServices * This, DWORD dwPlatformWorkQueue, IMFAsyncCallback * pCallback, IUnknown * pState);
    HRESULT(STDMETHODCALLTYPE *EndUnregisterPlatformWorkQueueWithMMCSS)(IMFWorkQueueServices * This, IMFAsyncResult * pResult);
    HRESULT(STDMETHODCALLTYPE *GetPlaftormWorkQueueMMCSSClass)(IMFWorkQueueServices * This, DWORD dwPlatformWorkQueueId, LPWSTR pwszClass, DWORD * pcchClass);
    HRESULT(STDMETHODCALLTYPE *GetPlatformWorkQueueMMCSSTaskId)(IMFWorkQueueServices * This, DWORD dwPlatformWorkQueueId, DWORD * pdwTaskId);
    END_INTERFACE
} IMFWorkQueueServicesVtbl;

interface IMFWorkQueueServices {
    CONST_VTBL struct IMFWorkQueueServicesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFWorkQueueServices_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFWorkQueueServices_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFWorkQueueServices_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFWorkQueueServices_BeginRegisterTopologyWorkQueuesWithMMCSS(This,pCallback,pState)  ((This)->lpVtbl->BeginRegisterTopologyWorkQueuesWithMMCSS(This,pCallback,pState))
#define IMFWorkQueueServices_EndRegisterTopologyWorkQueuesWithMMCSS(This,pResult)  ((This)->lpVtbl->EndRegisterTopologyWorkQueuesWithMMCSS(This,pResult))
#define IMFWorkQueueServices_BeginUnregisterTopologyWorkQueuesWithMMCSS(This,pCallback,pState)  ((This)->lpVtbl->BeginUnregisterTopologyWorkQueuesWithMMCSS(This,pCallback,pState))
#define IMFWorkQueueServices_EndUnregisterTopologyWorkQueuesWithMMCSS(This,pResult)  ((This)->lpVtbl->EndUnregisterTopologyWorkQueuesWithMMCSS(This,pResult))
#define IMFWorkQueueServices_GetTopologyWorkQueueMMCSSClass(This,dwTopologyWorkQueueId,pwszClass,pcchClass)  ((This)->lpVtbl->GetTopologyWorkQueueMMCSSClass(This,dwTopologyWorkQueueId,pwszClass,pcchClass))
#define IMFWorkQueueServices_GetTopologyWorkQueueMMCSSTaskId(This,dwTopologyWorkQueueId,pdwTaskId)  ((This)->lpVtbl->GetTopologyWorkQueueMMCSSTaskId(This,dwTopologyWorkQueueId,pdwTaskId))
#define IMFWorkQueueServices_BeginRegisterPlatformWorkQueueWithMMCSS(This,dwPlatformWorkQueue,wszClass,dwTaskId,pCallback,pState)  ((This)->lpVtbl->BeginRegisterPlatformWorkQueueWithMMCSS(This,dwPlatformWorkQueue,wszClass,dwTaskId,pCallback,pState))
#define IMFWorkQueueServices_EndRegisterPlatformWorkQueueWithMMCSS(This,pResult,pdwTaskId)  ((This)->lpVtbl->EndRegisterPlatformWorkQueueWithMMCSS(This,pResult,pdwTaskId))
#define IMFWorkQueueServices_BeginUnregisterPlatformWorkQueueWithMMCSS(This,dwPlatformWorkQueue,pCallback,pState)  ((This)->lpVtbl->BeginUnregisterPlatformWorkQueueWithMMCSS(This,dwPlatformWorkQueue,pCallback,pState))
#define IMFWorkQueueServices_EndUnregisterPlatformWorkQueueWithMMCSS(This,pResult)  ((This)->lpVtbl->EndUnregisterPlatformWorkQueueWithMMCSS(This,pResult))
#define IMFWorkQueueServices_GetPlaftormWorkQueueMMCSSClass(This,dwPlatformWorkQueueId,pwszClass,pcchClass)  ((This)->lpVtbl->GetPlaftormWorkQueueMMCSSClass(This,dwPlatformWorkQueueId,pwszClass,pcchClass))
#define IMFWorkQueueServices_GetPlatformWorkQueueMMCSSTaskId(This,dwPlatformWorkQueueId,pdwTaskId)  ((This)->lpVtbl->GetPlatformWorkQueueMMCSSTaskId(This,dwPlatformWorkQueueId,pdwTaskId))
#endif

#endif

HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_RemoteBeginRegisterTopologyWorkQueuesWithMMCSS_Proxy(IMFWorkQueueServices *This, IMFRemoteAsyncCallback *pCallback);
void __RPC_STUB IMFWorkQueueServices_RemoteBeginRegisterTopologyWorkQueuesWithMMCSS_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_RemoteEndRegisterTopologyWorkQueuesWithMMCSS_Proxy(IMFWorkQueueServices *This, IUnknown *pResult);
void __RPC_STUB IMFWorkQueueServices_RemoteEndRegisterTopologyWorkQueuesWithMMCSS_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_RemoteBeginUnregisterTopologyWorkQueuesWithMMCSS_Proxy(IMFWorkQueueServices *This, IMFRemoteAsyncCallback *pCallback);
void __RPC_STUB IMFWorkQueueServices_RemoteBeginUnregisterTopologyWorkQueuesWithMMCSS_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_RemoteEndUnregisterTopologyWorkQueuesWithMMCSS_Proxy(IMFWorkQueueServices *This, IUnknown *pResult);
void __RPC_STUB IMFWorkQueueServices_RemoteEndUnregisterTopologyWorkQueuesWithMMCSS_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_RemoteBeginRegisterPlatformWorkQueueWithMMCSS_Proxy(IMFWorkQueueServices *This, DWORD dwPlatformWorkQueue, LPCWSTR wszClass, DWORD dwTaskId, IMFRemoteAsyncCallback *pCallback);
void __RPC_STUB IMFWorkQueueServices_RemoteBeginRegisterPlatformWorkQueueWithMMCSS_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_RemoteEndRegisterPlatformWorkQueueWithMMCSS_Proxy(IMFWorkQueueServices *This, IUnknown *pResult, DWORD *pdwTaskId);
void __RPC_STUB IMFWorkQueueServices_RemoteEndRegisterPlatformWorkQueueWithMMCSS_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_RemoteBeginUnregisterPlatformWorkQueueWithMMCSS_Proxy(IMFWorkQueueServices *This, DWORD dwPlatformWorkQueue, IMFRemoteAsyncCallback *pCallback);
void __RPC_STUB IMFWorkQueueServices_RemoteBeginUnregisterPlatformWorkQueueWithMMCSS_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_RemoteEndUnregisterPlatformWorkQueueWithMMCSS_Proxy(IMFWorkQueueServices *This, IUnknown *pResult);
void __RPC_STUB IMFWorkQueueServices_RemoteEndUnregisterPlatformWorkQueueWithMMCSS_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);
#endif

EXTERN_GUID(MF_WORKQUEUE_SERVICES, 0x8e37d489, 0x41e0, 0x413a, 0x90, 0x68, 0x28, 0x7c, 0x88, 0x6d, 0x8d, 0xda);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0040_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0040_v0_0_s_ifspec;

#ifndef __IMFWorkQueueServicesEx_INTERFACE_DEFINED__
#define __IMFWorkQueueServicesEx_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFWorkQueueServicesEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("96bf961b-40fe-42f1-ba9d-320238b49700")IMFWorkQueueServicesEx:public IMFWorkQueueServices
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetTopologyWorkQueueMMCSSPriority(DWORD dwTopologyWorkQueueId, LONG * plPriority) = 0;
    virtual HRESULT STDMETHODCALLTYPE BeginRegisterPlatformWorkQueueWithMMCSSEx(DWORD dwPlatformWorkQueue, LPCWSTR wszClass, DWORD dwTaskId, LONG lPriority, IMFAsyncCallback *pCallback, IUnknown *pState) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPlatformWorkQueueMMCSSPriority(DWORD dwPlatformWorkQueueId, LONG *plPriority) = 0;
};
#else /* C style interface */
typedef struct IMFWorkQueueServicesExVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IMFWorkQueueServicesEx *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef) (IMFWorkQueueServicesEx *);
    ULONG (STDMETHODCALLTYPE *Release) (IMFWorkQueueServicesEx *);
    HRESULT (STDMETHODCALLTYPE *BeginRegisterTopologyWorkQueuesWithMMCSS)(IMFWorkQueueServicesEx *, IMFAsyncCallback *, IUnknown *);
    HRESULT (STDMETHODCALLTYPE *EndRegisterTopologyWorkQueuesWithMMCSS)(IMFWorkQueueServicesEx *, IMFAsyncResult *);
    HRESULT (STDMETHODCALLTYPE *BeginUnregisterTopologyWorkQueuesWithMMCSS)(IMFWorkQueueServicesEx *, IMFAsyncCallback *, IUnknown *);
    HRESULT (STDMETHODCALLTYPE *EndUnregisterTopologyWorkQueuesWithMMCSS)(IMFWorkQueueServicesEx *, IMFAsyncResult *);
    HRESULT (STDMETHODCALLTYPE *GetTopologyWorkQueueMMCSSClass)(IMFWorkQueueServicesEx *, DWORD, LPWSTR, DWORD *);
    HRESULT (STDMETHODCALLTYPE *GetTopologyWorkQueueMMCSSTaskId)(IMFWorkQueueServicesEx *, DWORD, DWORD *);
    HRESULT (STDMETHODCALLTYPE *BeginRegisterPlatformWorkQueueWithMMCSS)(IMFWorkQueueServicesEx *, DWORD, LPCWSTR, DWORD, IMFAsyncCallback *, IUnknown *);
    HRESULT (STDMETHODCALLTYPE *EndRegisterPlatformWorkQueueWithMMCSS)(IMFWorkQueueServicesEx *, IMFAsyncResult *, DWORD *);
    HRESULT (STDMETHODCALLTYPE *BeginUnregisterPlatformWorkQueueWithMMCSS)(IMFWorkQueueServicesEx *, DWORD, IMFAsyncCallback *, IUnknown *);
    HRESULT (STDMETHODCALLTYPE *EndUnregisterPlatformWorkQueueWithMMCSS)(IMFWorkQueueServicesEx *, IMFAsyncResult *);
    HRESULT (STDMETHODCALLTYPE *GetPlaftormWorkQueueMMCSSClass)(IMFWorkQueueServicesEx *, DWORD, LPWSTR, DWORD *);
    HRESULT (STDMETHODCALLTYPE *GetPlatformWorkQueueMMCSSTaskId)(IMFWorkQueueServicesEx *, DWORD, DWORD *);
    HRESULT (STDMETHODCALLTYPE *GetTopologyWorkQueueMMCSSPriority)(IMFWorkQueueServicesEx *, DWORD, LONG *);
    HRESULT (STDMETHODCALLTYPE *BeginRegisterPlatformWorkQueueWithMMCSSEx)(IMFWorkQueueServicesEx *, DWORD, LPCWSTR, DWORD, LONG, IMFAsyncCallback *, IUnknown *);
    HRESULT (STDMETHODCALLTYPE *GetPlatformWorkQueueMMCSSPriority)(IMFWorkQueueServicesEx *, DWORD, LONG *);
    END_INTERFACE
} IMFWorkQueueServicesExVtbl;

interface IMFWorkQueueServicesEx {
    CONST_VTBL struct IMFWorkQueueServicesExVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFWorkQueueServicesEx_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFWorkQueueServicesEx_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFWorkQueueServicesEx_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFWorkQueueServicesEx_BeginRegisterTopologyWorkQueuesWithMMCSS(This,pCallback,pState)  ((This)->lpVtbl->BeginRegisterTopologyWorkQueuesWithMMCSS(This,pCallback,pState)) 
#define IMFWorkQueueServicesEx_EndRegisterTopologyWorkQueuesWithMMCSS(This,pResult)  ((This)->lpVtbl->EndRegisterTopologyWorkQueuesWithMMCSS(This,pResult)) 
#define IMFWorkQueueServicesEx_BeginUnregisterTopologyWorkQueuesWithMMCSS(This,pCallback,pState)  ((This)->lpVtbl->BeginUnregisterTopologyWorkQueuesWithMMCSS(This,pCallback,pState)) 
#define IMFWorkQueueServicesEx_EndUnregisterTopologyWorkQueuesWithMMCSS(This,pResult)  ((This)->lpVtbl->EndUnregisterTopologyWorkQueuesWithMMCSS(This,pResult)) 
#define IMFWorkQueueServicesEx_GetTopologyWorkQueueMMCSSClass(This,dwTopologyWorkQueueId,pwszClass,pcchClass)  ((This)->lpVtbl->GetTopologyWorkQueueMMCSSClass(This,dwTopologyWorkQueueId,pwszClass,pcchClass)) 
#define IMFWorkQueueServicesEx_GetTopologyWorkQueueMMCSSTaskId(This,dwTopologyWorkQueueId,pdwTaskId)  ((This)->lpVtbl->GetTopologyWorkQueueMMCSSTaskId(This,dwTopologyWorkQueueId,pdwTaskId)) 
#define IMFWorkQueueServicesEx_BeginRegisterPlatformWorkQueueWithMMCSS(This,dwPlatformWorkQueue,wszClass,dwTaskId,pCallback,pState)  ((This)->lpVtbl->BeginRegisterPlatformWorkQueueWithMMCSS(This,dwPlatformWorkQueue,wszClass,dwTaskId,pCallback,pState)) 
#define IMFWorkQueueServicesEx_EndRegisterPlatformWorkQueueWithMMCSS(This,pResult,pdwTaskId)  ((This)->lpVtbl->EndRegisterPlatformWorkQueueWithMMCSS(This,pResult,pdwTaskId)) 
#define IMFWorkQueueServicesEx_BeginUnregisterPlatformWorkQueueWithMMCSS(This,dwPlatformWorkQueue,pCallback,pState)  ((This)->lpVtbl->BeginUnregisterPlatformWorkQueueWithMMCSS(This,dwPlatformWorkQueue,pCallback,pState)) 
#define IMFWorkQueueServicesEx_EndUnregisterPlatformWorkQueueWithMMCSS(This,pResult)  ((This)->lpVtbl->EndUnregisterPlatformWorkQueueWithMMCSS(This,pResult)) 
#define IMFWorkQueueServicesEx_GetPlaftormWorkQueueMMCSSClass(This,dwPlatformWorkQueueId,pwszClass,pcchClass)  ((This)->lpVtbl->GetPlaftormWorkQueueMMCSSClass(This,dwPlatformWorkQueueId,pwszClass,pcchClass)) 
#define IMFWorkQueueServicesEx_GetPlatformWorkQueueMMCSSTaskId(This,dwPlatformWorkQueueId,pdwTaskId)  ((This)->lpVtbl->GetPlatformWorkQueueMMCSSTaskId(This,dwPlatformWorkQueueId,pdwTaskId)) 
#define IMFWorkQueueServicesEx_GetTopologyWorkQueueMMCSSPriority(This,dwTopologyWorkQueueId,plPriority)  ((This)->lpVtbl->GetTopologyWorkQueueMMCSSPriority(This,dwTopologyWorkQueueId,plPriority)) 
#define IMFWorkQueueServicesEx_BeginRegisterPlatformWorkQueueWithMMCSSEx(This,dwPlatformWorkQueue,wszClass,dwTaskId,lPriority,pCallback,pState)  ((This)->lpVtbl->BeginRegisterPlatformWorkQueueWithMMCSSEx(This,dwPlatformWorkQueue,wszClass,dwTaskId,lPriority,pCallback,pState)) 
#define IMFWorkQueueServicesEx_GetPlatformWorkQueueMMCSSPriority(This,dwPlatformWorkQueueId,plPriority)  ((This)->lpVtbl->GetPlatformWorkQueueMMCSSPriority(This,dwPlatformWorkQueueId,plPriority)) 
#endif /* COBJMACROS */

#endif /* C style interface */

HRESULT STDMETHODCALLTYPE IMFWorkQueueServicesEx_RemoteBeginRegisterPlatformWorkQueueWithMMCSSEx_Proxy(IMFWorkQueueServicesEx *, DWORD, LPCWSTR, DWORD, LONG, IMFRemoteAsyncCallback *);
void __RPC_STUB IMFWorkQueueServicesEx_RemoteBeginRegisterPlatformWorkQueueWithMMCSSEx_Stub(IRpcStubBuffer *, IRpcChannelBuffer *, PRPC_MESSAGE, DWORD *);

#endif /* __IMFWorkQueueServicesEx_INTERFACE_DEFINED__ */

typedef enum _MF_QUALITY_DROP_MODE {
    MF_DROP_MODE_NONE = 0,
    MF_DROP_MODE_1 = 0x1,
    MF_DROP_MODE_2 = 0x2,
    MF_DROP_MODE_3 = 0x3,
    MF_DROP_MODE_4 = 0x4,
    MF_DROP_MODE_5 = 0x5,
    MF_NUM_DROP_MODES = 0x6
} MF_QUALITY_DROP_MODE;

typedef enum _MF_QUALITY_LEVEL {
    MF_QUALITY_NORMAL = 0,
    MF_QUALITY_NORMAL_MINUS_1 = 0x1,
    MF_QUALITY_NORMAL_MINUS_2 = 0x2,
    MF_QUALITY_NORMAL_MINUS_3 = 0x3,
    MF_QUALITY_NORMAL_MINUS_4 = 0x4,
    MF_QUALITY_NORMAL_MINUS_5 = 0x5,
    MF_NUM_QUALITY_LEVELS = 0x6
} MF_QUALITY_LEVEL;

#if (WINVER >= _WIN32_WINNT_WIN7)
typedef enum _MF_QUALITY_ADVISE_FLAGS {
    MF_QUALITY_CANNOT_KEEP_UP = 0x1
} MF_QUALITY_ADVISE_FLAGS;

#endif

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0035_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0035_v0_0_s_ifspec;

#ifndef __IMFQualityManager_INTERFACE_DEFINED__
#define __IMFQualityManager_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFQualityManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("8D009D86-5B9F-4115-B1FC-9F80D52AB8AB")IMFQualityManager:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE NotifyTopology(IMFTopology * pTopology) = 0;
    virtual HRESULT STDMETHODCALLTYPE NotifyPresentationClock(IMFPresentationClock *pClock) = 0;
    virtual HRESULT STDMETHODCALLTYPE NotifyProcessInput(IMFTopologyNode *pNode, long lInputIndex, IMFSample *pSample) = 0;
    virtual HRESULT STDMETHODCALLTYPE NotifyProcessOutput(IMFTopologyNode *pNode, long lOutputIndex, IMFSample *pSample) = 0;
    virtual HRESULT STDMETHODCALLTYPE NotifyQualityEvent(IUnknown *pObject, IMFMediaEvent *pEvent) = 0;
    virtual HRESULT STDMETHODCALLTYPE Shutdown(void) = 0;
};
#else
typedef struct IMFQualityManagerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFQualityManager * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFQualityManager * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFQualityManager * This);
    HRESULT(STDMETHODCALLTYPE *NotifyTopology)(IMFQualityManager * This, IMFTopology * pTopology);
    HRESULT(STDMETHODCALLTYPE *NotifyPresentationClock)(IMFQualityManager * This, IMFPresentationClock * pClock);
    HRESULT(STDMETHODCALLTYPE *NotifyProcessInput)(IMFQualityManager * This, IMFTopologyNode * pNode, long lInputIndex, IMFSample * pSample);
    HRESULT(STDMETHODCALLTYPE *NotifyProcessOutput)(IMFQualityManager * This, IMFTopologyNode * pNode, long lOutputIndex, IMFSample * pSample);
    HRESULT(STDMETHODCALLTYPE *NotifyQualityEvent)(IMFQualityManager * This, IUnknown * pObject, IMFMediaEvent * pEvent);
    HRESULT(STDMETHODCALLTYPE *Shutdown)(IMFQualityManager * This);
    END_INTERFACE
} IMFQualityManagerVtbl;

interface IMFQualityManager {
    CONST_VTBL struct IMFQualityManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFQualityManager_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFQualityManager_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFQualityManager_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFQualityManager_NotifyTopology(This,pTopology)  ((This)->lpVtbl->NotifyTopology(This,pTopology))
#define IMFQualityManager_NotifyPresentationClock(This,pClock)  ((This)->lpVtbl->NotifyPresentationClock(This,pClock))
#define IMFQualityManager_NotifyProcessInput(This,pNode,lInputIndex,pSample)  ((This)->lpVtbl->NotifyProcessInput(This,pNode,lInputIndex,pSample))
#define IMFQualityManager_NotifyProcessOutput(This,pNode,lOutputIndex,pSample)  ((This)->lpVtbl->NotifyProcessOutput(This,pNode,lOutputIndex,pSample))
#define IMFQualityManager_NotifyQualityEvent(This,pObject,pEvent)  ((This)->lpVtbl->NotifyQualityEvent(This,pObject,pEvent))
#define IMFQualityManager_Shutdown(This)  ((This)->lpVtbl->Shutdown(This))
#endif

#endif

#endif

STDAPI MFCreateStandardQualityManager(IMFQualityManager **ppQualityManager);

EXTERN_GUID(MF_QUALITY_NOTIFY_PROCESSING_LATENCY, 0xf6b44af8, 0x604d, 0x46fe, 0xa9, 0x5d, 0x45, 0x47, 0x9b, 0x10, 0xc9, 0xbc);
EXTERN_GUID(MF_QUALITY_NOTIFY_SAMPLE_LAG, 0x30d15206, 0xed2a, 0x4760, 0xbe, 0x17, 0xeb, 0x4a, 0x9f, 0x12, 0x29, 0x5c);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0036_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0036_v0_0_s_ifspec;

#ifndef __IMFQualityAdvise_INTERFACE_DEFINED__
#define __IMFQualityAdvise_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFQualityAdvise;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("EC15E2E9-E36B-4f7c-8758-77D452EF4CE7")IMFQualityAdvise:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetDropMode(MF_QUALITY_DROP_MODE eDropMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetQualityLevel(MF_QUALITY_LEVEL eQualityLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDropMode(MF_QUALITY_DROP_MODE *peDropMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetQualityLevel(MF_QUALITY_LEVEL *peQualityLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE DropTime(LONGLONG hnsAmountToDrop) = 0;
};
#else
typedef struct IMFQualityAdviseVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFQualityAdvise * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFQualityAdvise * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFQualityAdvise * This);
    HRESULT(STDMETHODCALLTYPE *SetDropMode)(IMFQualityAdvise * This, MF_QUALITY_DROP_MODE eDropMode);
    HRESULT(STDMETHODCALLTYPE *SetQualityLevel)(IMFQualityAdvise * This, MF_QUALITY_LEVEL eQualityLevel);
    HRESULT(STDMETHODCALLTYPE *GetDropMode)(IMFQualityAdvise * This, MF_QUALITY_DROP_MODE * peDropMode);
    HRESULT(STDMETHODCALLTYPE *GetQualityLevel)(IMFQualityAdvise * This, MF_QUALITY_LEVEL * peQualityLevel);
    HRESULT(STDMETHODCALLTYPE *DropTime)(IMFQualityAdvise * This, LONGLONG hnsAmountToDrop);
    END_INTERFACE
} IMFQualityAdviseVtbl;

interface IMFQualityAdvise {
    CONST_VTBL struct IMFQualityAdviseVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFQualityAdvise_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFQualityAdvise_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFQualityAdvise_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFQualityAdvise_SetDropMode(This,eDropMode)  ((This)->lpVtbl->SetDropMode(This,eDropMode))
#define IMFQualityAdvise_SetQualityLevel(This,eQualityLevel)  ((This)->lpVtbl->SetQualityLevel(This,eQualityLevel))
#define IMFQualityAdvise_GetDropMode(This,peDropMode)  ((This)->lpVtbl->GetDropMode(This,peDropMode))
#define IMFQualityAdvise_GetQualityLevel(This,peQualityLevel)  ((This)->lpVtbl->GetQualityLevel(This,peQualityLevel))
#define IMFQualityAdvise_DropTime(This,hnsAmountToDrop)  ((This)->lpVtbl->DropTime(This,hnsAmountToDrop))
#endif

#endif

#endif

#if (WINVER >= _WIN32_WINNT_WIN7)

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0037_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0037_v0_0_s_ifspec;

#ifndef __IMFQualityAdvise2_INTERFACE_DEFINED__
#define __IMFQualityAdvise2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFQualityAdvise2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("F3706F0D-8EA2-4886-8000-7155E9EC2EAE")IMFQualityAdvise2:public IMFQualityAdvise
{
    public:
    virtual HRESULT STDMETHODCALLTYPE NotifyQualityEvent(IMFMediaEvent * pEvent, DWORD * pdwFlags) = 0;
};

#else

typedef struct IMFQualityAdvise2Vtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFQualityAdvise2 * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFQualityAdvise2 * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFQualityAdvise2 * This);
    HRESULT(STDMETHODCALLTYPE *SetDropMode)(IMFQualityAdvise2 * This, MF_QUALITY_DROP_MODE eDropMode);
    HRESULT(STDMETHODCALLTYPE *SetQualityLevel)(IMFQualityAdvise2 * This, MF_QUALITY_LEVEL eQualityLevel);
    HRESULT(STDMETHODCALLTYPE *GetDropMode)(IMFQualityAdvise2 * This, MF_QUALITY_DROP_MODE * peDropMode);
    HRESULT(STDMETHODCALLTYPE *GetQualityLevel)(IMFQualityAdvise2 * This, MF_QUALITY_LEVEL * peQualityLevel);
    HRESULT(STDMETHODCALLTYPE *DropTime)(IMFQualityAdvise2 * This, LONGLONG hnsAmountToDrop);
    HRESULT(STDMETHODCALLTYPE *NotifyQualityEvent)(IMFQualityAdvise2 * This, IMFMediaEvent * pEvent, DWORD * pdwFlags);
    END_INTERFACE
} IMFQualityAdvise2Vtbl;

interface IMFQualityAdvise2 {
    CONST_VTBL struct IMFQualityAdvise2Vtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFQualityAdvise2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFQualityAdvise2_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFQualityAdvise2_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFQualityAdvise2_SetDropMode(This,eDropMode)  ((This)->lpVtbl->SetDropMode(This,eDropMode))
#define IMFQualityAdvise2_SetQualityLevel(This,eQualityLevel)  ((This)->lpVtbl->SetQualityLevel(This,eQualityLevel))
#define IMFQualityAdvise2_GetDropMode(This,peDropMode)  ((This)->lpVtbl->GetDropMode(This,peDropMode))
#define IMFQualityAdvise2_GetQualityLevel(This,peQualityLevel)  ((This)->lpVtbl->GetQualityLevel(This,peQualityLevel))
#define IMFQualityAdvise2_DropTime(This,hnsAmountToDrop)  ((This)->lpVtbl->DropTime(This,hnsAmountToDrop))

#define IMFQualityAdvise2_NotifyQualityEvent(This,pEvent,pdwFlags)  ((This)->lpVtbl->NotifyQualityEvent(This,pEvent,pdwFlags))
#endif


#endif




#endif


#ifndef __IMFQualityAdviseLimits_INTERFACE_DEFINED__
#define __IMFQualityAdviseLimits_INTERFACE_DEFINED__









    EXTERN_C const IID IID_IMFQualityAdviseLimits;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("dfcd8e4d-30b5-4567-acaa-8eb5b7853dc9")IMFQualityAdviseLimits:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetMaximumDropMode(MF_QUALITY_DROP_MODE * peDropMode) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMinimumQualityLevel(MF_QUALITY_LEVEL *peQualityLevel) = 0;
};

#else

typedef struct IMFQualityAdviseLimitsVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFQualityAdviseLimits * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFQualityAdviseLimits * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFQualityAdviseLimits * This);
    HRESULT(STDMETHODCALLTYPE *GetMaximumDropMode)(IMFQualityAdviseLimits * This, MF_QUALITY_DROP_MODE * peDropMode);
    HRESULT(STDMETHODCALLTYPE *GetMinimumQualityLevel)(IMFQualityAdviseLimits * This, MF_QUALITY_LEVEL * peQualityLevel);
    END_INTERFACE
} IMFQualityAdviseLimitsVtbl;

interface IMFQualityAdviseLimits {
    CONST_VTBL struct IMFQualityAdviseLimitsVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFQualityAdviseLimits_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFQualityAdviseLimits_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFQualityAdviseLimits_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFQualityAdviseLimits_GetMaximumDropMode(This,peDropMode)  ((This)->lpVtbl->GetMaximumDropMode(This,peDropMode))
#define IMFQualityAdviseLimits_GetMinimumQualityLevel(This,peQualityLevel)  ((This)->lpVtbl->GetMinimumQualityLevel(This,peQualityLevel))
#endif


#endif




#endif





#endif

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0039_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0039_v0_0_s_ifspec;

#ifndef __IMFRealTimeClient_INTERFACE_DEFINED__
#define __IMFRealTimeClient_INTERFACE_DEFINED__





    EXTERN_C const IID IID_IMFRealTimeClient;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("2347D60B-3FB5-480c-8803-8DF3ADCD3EF0")IMFRealTimeClient:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE RegisterThreads(DWORD dwTaskIndex, LPCWSTR wszClass) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnregisterThreads(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetWorkQueue(DWORD dwWorkQueueId) = 0;
};

#else

typedef struct IMFRealTimeClientVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFRealTimeClient * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFRealTimeClient * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFRealTimeClient * This);
    HRESULT(STDMETHODCALLTYPE *RegisterThreads)(IMFRealTimeClient * This, DWORD dwTaskIndex, LPCWSTR wszClass);
    HRESULT(STDMETHODCALLTYPE *UnregisterThreads)(IMFRealTimeClient * This);
    HRESULT(STDMETHODCALLTYPE *SetWorkQueue)(IMFRealTimeClient * This, DWORD dwWorkQueueId);
    END_INTERFACE
} IMFRealTimeClientVtbl;

interface IMFRealTimeClient {
    CONST_VTBL struct IMFRealTimeClientVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFRealTimeClient_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFRealTimeClient_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFRealTimeClient_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFRealTimeClient_RegisterThreads(This,dwTaskIndex,wszClass)  ((This)->lpVtbl->RegisterThreads(This,dwTaskIndex,wszClass))
#define IMFRealTimeClient_UnregisterThreads(This)  ((This)->lpVtbl->UnregisterThreads(This))
#define IMFRealTimeClient_SetWorkQueue(This,dwWorkQueueId)  ((This)->lpVtbl->SetWorkQueue(This,dwWorkQueueId))
#endif


#endif




#endif

#if (WINVER >= _WIN32_WINNT_WIN8) 

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0046_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0046_v0_0_s_ifspec;

#ifndef __IMFRealTimeClientEx_INTERFACE_DEFINED__
#define __IMFRealTimeClientEx_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFRealTimeClientEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("03910848-AB16-4611-B100-17B88AE2F248")IMFRealTimeClientEx:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE RegisterThreadsEx(DWORD *, LPCWSTR, LONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnregisterThreads(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetWorkQueueEx(DWORD, LONG) = 0;
};
#else /* C style interface */
typedef struct IMFRealTimeClientExVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFRealTimeClientEx *, REFIID, void **);
    ULONG(STDMETHODCALLTYPE * AddRef) (IMFRealTimeClientEx *);
    ULONG(STDMETHODCALLTYPE * Release) (IMFRealTimeClientEx *);
    HRESULT(STDMETHODCALLTYPE * RegisterThreadsEx) (IMFRealTimeClientEx *, DWORD *, LPCWSTR, LONG);
    HRESULT(STDMETHODCALLTYPE * UnregisterThreads) (IMFRealTimeClientEx *);
    HRESULT(STDMETHODCALLTYPE * SetWorkQueueEx) (IMFRealTimeClientEx *, DWORD, LONG);
    END_INTERFACE
} IMFRealTimeClientExVtbl;

interface IMFRealTimeClientEx {
    CONST_VTBL struct IMFRealTimeClientExVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFRealTimeClientEx_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFRealTimeClientEx_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFRealTimeClientEx_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFRealTimeClientEx_RegisterThreadsEx(This,pdwTaskIndex,wszClassName,lBasePriority)  ((This)->lpVtbl->RegisterThreadsEx(This,pdwTaskIndex,wszClassName,lBasePriority)) 
#define IMFRealTimeClientEx_UnregisterThreads(This)  ((This)->lpVtbl->UnregisterThreads(This)) 
#define IMFRealTimeClientEx_SetWorkQueueEx(This,dwMultithreadedWorkQueueId,lWorkItemBasePriority)  ((This)->lpVtbl->SetWorkQueueEx(This,dwMultithreadedWorkQueueId,lWorkItemBasePriority)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFRealTimeClientEx_INTERFACE_DEFINED__ */

#endif /* WINVER >= _WIN32_WINNT_WIN8 */

typedef DWORD MFSequencerElementId;

#define MFSEQUENCER_INVALID_ELEMENT_ID  (0xffffffff)

typedef enum _MFSequencerTopologyFlags {
    SequencerTopologyFlags_Last = 0x1
} MFSequencerTopologyFlags;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0040_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0040_v0_0_s_ifspec;

#ifndef __IMFSequencerSource_INTERFACE_DEFINED__
#define __IMFSequencerSource_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSequencerSource;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("197CD219-19CB-4de1-A64C-ACF2EDCBE59E")IMFSequencerSource:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE AppendTopology(IMFTopology * pTopology, DWORD dwFlags, MFSequencerElementId * pdwId) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteTopology(MFSequencerElementId dwId) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPresentationContext(IMFPresentationDescriptor *pPD, MFSequencerElementId *pId, IMFTopology **ppTopology) = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdateTopology(MFSequencerElementId dwId, IMFTopology *pTopology) = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdateTopologyFlags(MFSequencerElementId dwId, DWORD dwFlags) = 0;
};
#else
typedef struct IMFSequencerSourceVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSequencerSource * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSequencerSource * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSequencerSource * This);
    HRESULT(STDMETHODCALLTYPE *AppendTopology)(IMFSequencerSource * This, IMFTopology * pTopology, DWORD dwFlags, MFSequencerElementId * pdwId);
    HRESULT(STDMETHODCALLTYPE *DeleteTopology)(IMFSequencerSource * This, MFSequencerElementId dwId);
    HRESULT(STDMETHODCALLTYPE *GetPresentationContext)(IMFSequencerSource * This, IMFPresentationDescriptor * pPD, MFSequencerElementId * pId, IMFTopology ** ppTopology);
    HRESULT(STDMETHODCALLTYPE *UpdateTopology)(IMFSequencerSource * This, MFSequencerElementId dwId, IMFTopology * pTopology);
    HRESULT(STDMETHODCALLTYPE *UpdateTopologyFlags)(IMFSequencerSource * This, MFSequencerElementId dwId, DWORD dwFlags);
    END_INTERFACE
} IMFSequencerSourceVtbl;

interface IMFSequencerSource {
    CONST_VTBL struct IMFSequencerSourceVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFSequencerSource_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSequencerSource_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSequencerSource_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFSequencerSource_AppendTopology(This,pTopology,dwFlags,pdwId)  ((This)->lpVtbl->AppendTopology(This,pTopology,dwFlags,pdwId))
#define IMFSequencerSource_DeleteTopology(This,dwId)  ((This)->lpVtbl->DeleteTopology(This,dwId))
#define IMFSequencerSource_GetPresentationContext(This,pPD,pId,ppTopology)  ((This)->lpVtbl->GetPresentationContext(This,pPD,pId,ppTopology))
#define IMFSequencerSource_UpdateTopology(This,dwId,pTopology)  ((This)->lpVtbl->UpdateTopology(This,dwId,pTopology))
#define IMFSequencerSource_UpdateTopologyFlags(This,dwId,dwFlags)  ((This)->lpVtbl->UpdateTopologyFlags(This,dwId,dwFlags))
#endif


#endif




#endif

EXTERN_GUID(MF_TIME_FORMAT_SEGMENT_OFFSET, 0xc8b8be77, 0x869c, 0x431d, 0x81, 0x2e, 0x16, 0x96, 0x93, 0xf6, 0x5a, 0x39);

STDAPI MFCreateSequencerSource(IUnknown *pReserved, IMFSequencerSource **ppSequencerSource);
STDAPI MFCreateSequencerSegmentOffset(MFSequencerElementId dwId, MFTIME hnsOffset, PROPVARIANT *pvarSegmentOffset);
#if (WINVER >= _WIN32_WINNT_WIN7)
STDAPI MFCreateAggregateSource(IMFCollection *pSourceCollection, IMFMediaSource **ppAggSource);
#endif

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0041_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0041_v0_0_s_ifspec;

#ifndef __IMFMediaSourceTopologyProvider_INTERFACE_DEFINED__
#define __IMFMediaSourceTopologyProvider_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFMediaSourceTopologyProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0E1D6009-C9F3-442d-8C51-A42D2D49452F")IMFMediaSourceTopologyProvider:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetMediaSourceTopology(IMFPresentationDescriptor * pPresentationDescriptor, IMFTopology ** ppTopology) = 0;
};
#else
typedef struct IMFMediaSourceTopologyProviderVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFMediaSourceTopologyProvider * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFMediaSourceTopologyProvider * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFMediaSourceTopologyProvider * This);
    HRESULT(STDMETHODCALLTYPE *GetMediaSourceTopology)(IMFMediaSourceTopologyProvider * This, IMFPresentationDescriptor * pPresentationDescriptor, IMFTopology ** ppTopology);
    END_INTERFACE
} IMFMediaSourceTopologyProviderVtbl;

interface IMFMediaSourceTopologyProvider {
    CONST_VTBL struct IMFMediaSourceTopologyProviderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFMediaSourceTopologyProvider_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFMediaSourceTopologyProvider_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFMediaSourceTopologyProvider_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFMediaSourceTopologyProvider_GetMediaSourceTopology(This,pPresentationDescriptor,ppTopology)  ((This)->lpVtbl->GetMediaSourceTopology(This,pPresentationDescriptor,ppTopology))
#endif

#endif

#endif

#ifndef __IMFMediaSourcePresentationProvider_INTERFACE_DEFINED__
#define __IMFMediaSourcePresentationProvider_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFMediaSourcePresentationProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0E1D600a-C9F3-442d-8C51-A42D2D49452F")IMFMediaSourcePresentationProvider:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE ForceEndOfPresentation(IMFPresentationDescriptor * pPresentationDescriptor) = 0;
};
#else
typedef struct IMFMediaSourcePresentationProviderVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFMediaSourcePresentationProvider * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFMediaSourcePresentationProvider * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFMediaSourcePresentationProvider * This);
    HRESULT(STDMETHODCALLTYPE *ForceEndOfPresentation)(IMFMediaSourcePresentationProvider * This, IMFPresentationDescriptor * pPresentationDescriptor);
    END_INTERFACE
} IMFMediaSourcePresentationProviderVtbl;

interface IMFMediaSourcePresentationProvider {
    CONST_VTBL struct IMFMediaSourcePresentationProviderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFMediaSourcePresentationProvider_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFMediaSourcePresentationProvider_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFMediaSourcePresentationProvider_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFMediaSourcePresentationProvider_ForceEndOfPresentation(This,pPresentationDescriptor)  ((This)->lpVtbl->ForceEndOfPresentation(This,pPresentationDescriptor))
#endif

#endif

#endif

EXTERN_GUID(MF_SOURCE_PRESENTATION_PROVIDER_SERVICE, 0xe002aadc, 0xf4af, 0x4ee5, 0x98, 0x47, 0x05, 0x3e, 0xdf, 0x84, 0x04, 0x26);

typedef struct _MFTOPONODE_ATTRIBUTE_UPDATE {
    TOPOID NodeId;
    GUID guidAttributeKey;
    MF_ATTRIBUTE_TYPE attrType;
    union {
        UINT32 u32;
        UINT64 u64;
        double d;

    };
} MFTOPONODE_ATTRIBUTE_UPDATE;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0043_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0043_v0_0_s_ifspec;

#ifndef __IMFTopologyNodeAttributeEditor_INTERFACE_DEFINED__
#define __IMFTopologyNodeAttributeEditor_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTopologyNodeAttributeEditor;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("676aa6dd-238a-410d-bb99-65668d01605a")IMFTopologyNodeAttributeEditor:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE UpdateNodeAttributes(TOPOID TopoId, DWORD cUpdates, MFTOPONODE_ATTRIBUTE_UPDATE * pUpdates) = 0;
};
#else
typedef struct IMFTopologyNodeAttributeEditorVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTopologyNodeAttributeEditor * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFTopologyNodeAttributeEditor * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFTopologyNodeAttributeEditor * This);
    HRESULT(STDMETHODCALLTYPE *UpdateNodeAttributes)(IMFTopologyNodeAttributeEditor * This, TOPOID TopoId, DWORD cUpdates, MFTOPONODE_ATTRIBUTE_UPDATE * pUpdates);
    END_INTERFACE
} IMFTopologyNodeAttributeEditorVtbl;

interface IMFTopologyNodeAttributeEditor {
    CONST_VTBL struct IMFTopologyNodeAttributeEditorVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFTopologyNodeAttributeEditor_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFTopologyNodeAttributeEditor_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFTopologyNodeAttributeEditor_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFTopologyNodeAttributeEditor_UpdateNodeAttributes(This,TopoId,cUpdates,pUpdates)  ((This)->lpVtbl->UpdateNodeAttributes(This,TopoId,cUpdates,pUpdates))
#endif


#endif




#endif

EXTERN_GUID(MF_TOPONODE_ATTRIBUTE_EDITOR_SERVICE, 0x65656e1a, 0x077f, 0x4472, 0x83, 0xef, 0x31, 0x6f, 0x11, 0xd5, 0x08, 0x7a);

typedef struct _MF_LEAKY_BUCKET_PAIR {
    DWORD dwBitrate;
    DWORD msBufferWindow;
} MF_LEAKY_BUCKET_PAIR;

typedef struct _MFBYTESTREAM_BUFFERING_PARAMS {
    QWORD cbTotalFileSize;
    QWORD cbPlayableDataSize;
    MF_LEAKY_BUCKET_PAIR *prgBuckets;
    DWORD cBuckets;
    QWORD qwNetBufferingTime;
    QWORD qwExtraBufferingTimeDuringSeek;
    QWORD qwPlayDuration;
    float dRate;
} MFBYTESTREAM_BUFFERING_PARAMS;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0044_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0044_v0_0_s_ifspec;

#ifndef __IMFByteStreamBuffering_INTERFACE_DEFINED__
#define __IMFByteStreamBuffering_INTERFACE_DEFINED__





    EXTERN_C const IID IID_IMFByteStreamBuffering;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("6d66d782-1d4f-4db7-8c63-cb8c77f1ef5e")IMFByteStreamBuffering:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetBufferingParams(MFBYTESTREAM_BUFFERING_PARAMS * pParams) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnableBuffering(BOOL fEnable) = 0;
    virtual HRESULT STDMETHODCALLTYPE StopBuffering(void) = 0;
};

#else

typedef struct IMFByteStreamBufferingVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFByteStreamBuffering * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFByteStreamBuffering * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFByteStreamBuffering * This);
    HRESULT(STDMETHODCALLTYPE *SetBufferingParams)(IMFByteStreamBuffering * This, MFBYTESTREAM_BUFFERING_PARAMS * pParams);
    HRESULT(STDMETHODCALLTYPE *EnableBuffering)(IMFByteStreamBuffering * This, BOOL fEnable);
    HRESULT(STDMETHODCALLTYPE *StopBuffering)(IMFByteStreamBuffering * This);
    END_INTERFACE
} IMFByteStreamBufferingVtbl;

interface IMFByteStreamBuffering {
    CONST_VTBL struct IMFByteStreamBufferingVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFByteStreamBuffering_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFByteStreamBuffering_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFByteStreamBuffering_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFByteStreamBuffering_SetBufferingParams(This,pParams)  ((This)->lpVtbl->SetBufferingParams(This,pParams))
#define IMFByteStreamBuffering_EnableBuffering(This,fEnable)  ((This)->lpVtbl->EnableBuffering(This,fEnable))
#define IMFByteStreamBuffering_StopBuffering(This)  ((This)->lpVtbl->StopBuffering(This))
#endif


#endif




#endif

#ifndef __IMFByteStreamCacheControl_INTERFACE_DEFINED__
#define __IMFByteStreamCacheControl_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFByteStreamCacheControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("F5042EA4-7A96-4a75-AA7B-2BE1EF7F88D5")IMFByteStreamCacheControl:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE StopBackgroundTransfer(void) = 0;
};
#else
typedef struct IMFByteStreamCacheControlVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFByteStreamCacheControl * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFByteStreamCacheControl * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFByteStreamCacheControl * This);
    HRESULT(STDMETHODCALLTYPE *StopBackgroundTransfer)(IMFByteStreamCacheControl * This);
    END_INTERFACE
} IMFByteStreamCacheControlVtbl;

interface IMFByteStreamCacheControl {
    CONST_VTBL struct IMFByteStreamCacheControlVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFByteStreamCacheControl_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFByteStreamCacheControl_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFByteStreamCacheControl_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFByteStreamCacheControl_StopBackgroundTransfer(This)  ((This)->lpVtbl->StopBackgroundTransfer(This))
#endif


#endif




#endif

#ifndef __IMFByteStreamTimeSeek_INTERFACE_DEFINED__
#define __IMFByteStreamTimeSeek_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFByteStreamTimeSeek;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("64976BFA-FB61-4041-9069-8C9A5F659BEB")IMFByteStreamTimeSeek:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE IsTimeSeekSupported(BOOL *) = 0;
    virtual HRESULT STDMETHODCALLTYPE TimeSeek(QWORD) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTimeSeekResult(QWORD *, QWORD *, QWORD *) = 0;
};
#else /* C style interface */
typedef struct IMFByteStreamTimeSeekVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFByteStreamTimeSeek *, REFIID, void **);
    ULONG(STDMETHODCALLTYPE * AddRef) (IMFByteStreamTimeSeek *);
    ULONG(STDMETHODCALLTYPE * Release) (IMFByteStreamTimeSeek *);
    HRESULT(STDMETHODCALLTYPE * IsTimeSeekSupported) (IMFByteStreamTimeSeek *, BOOL *);
    HRESULT(STDMETHODCALLTYPE * TimeSeek) (IMFByteStreamTimeSeek *, QWORD);
    HRESULT(STDMETHODCALLTYPE * GetTimeSeekResult) (IMFByteStreamTimeSeek *, QWORD *, QWORD *, QWORD *);
    END_INTERFACE
} IMFByteStreamTimeSeekVtbl;

interface IMFByteStreamTimeSeek {
    CONST_VTBL struct IMFByteStreamTimeSeekVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFByteStreamTimeSeek_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFByteStreamTimeSeek_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFByteStreamTimeSeek_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFByteStreamTimeSeek_IsTimeSeekSupported(This,pfTimeSeekIsSupported)  ((This)->lpVtbl->IsTimeSeekSupported(This,pfTimeSeekIsSupported)) 
#define IMFByteStreamTimeSeek_TimeSeek(This,qwTimePosition)  ((This)->lpVtbl->TimeSeek(This,qwTimePosition)) 
#define IMFByteStreamTimeSeek_GetTimeSeekResult(This,pqwStartTime,pqwStopTime,pqwDuration)  ((This)->lpVtbl->GetTimeSeekResult(This,pqwStartTime,pqwStopTime,pqwDuration)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFByteStreamTimeSeek_INTERFACE_DEFINED__ */

#if (WINVER >= _WIN32_WINNT_WIN8) 
typedef struct __MIDL___MIDL_itf_mfidl_0000_0054_0001 {
    QWORD qwStartOffset;
    QWORD qwEndOffset;
} MF_BYTE_STREAM_CACHE_RANGE;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0054_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0054_v0_0_s_ifspec;

#ifndef __IMFByteStreamCacheControl2_INTERFACE_DEFINED__
#define __IMFByteStreamCacheControl2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFByteStreamCacheControl2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("71CE469C-F34B-49EA-A56B-2D2A10E51149")IMFByteStreamCacheControl2:public IMFByteStreamCacheControl
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetByteRanges(DWORD * pcRanges, MF_BYTE_STREAM_CACHE_RANGE ** ppRanges) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetCacheLimit(QWORD qwBytes) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsBackgroundTransferActive(BOOL *pfActive) = 0;
};
#else /* C style interface */
typedef struct IMFByteStreamCacheControl2Vtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFByteStreamCacheControl2 * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IMFByteStreamCacheControl2 * This);
    ULONG(STDMETHODCALLTYPE * Release) (IMFByteStreamCacheControl2 * This);
    HRESULT(STDMETHODCALLTYPE * StopBackgroundTransfer) (IMFByteStreamCacheControl2 * This);
    HRESULT(STDMETHODCALLTYPE * GetByteRanges) (IMFByteStreamCacheControl2 * This, DWORD * pcRanges, MF_BYTE_STREAM_CACHE_RANGE ** ppRanges);
    HRESULT(STDMETHODCALLTYPE * SetCacheLimit) (IMFByteStreamCacheControl2 * This, QWORD qwBytes);
    HRESULT(STDMETHODCALLTYPE * IsBackgroundTransferActive) (IMFByteStreamCacheControl2 * This, BOOL * pfActive);
    END_INTERFACE
} IMFByteStreamCacheControl2Vtbl;

interface IMFByteStreamCacheControl2 {
    CONST_VTBL struct IMFByteStreamCacheControl2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFByteStreamCacheControl2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFByteStreamCacheControl2_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFByteStreamCacheControl2_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFByteStreamCacheControl2_StopBackgroundTransfer(This)  ((This)->lpVtbl->StopBackgroundTransfer(This)) 
#define IMFByteStreamCacheControl2_GetByteRanges(This,pcRanges,ppRanges)  ((This)->lpVtbl->GetByteRanges(This,pcRanges,ppRanges)) 
#define IMFByteStreamCacheControl2_SetCacheLimit(This,qwBytes)  ((This)->lpVtbl->SetCacheLimit(This,qwBytes)) 
#define IMFByteStreamCacheControl2_IsBackgroundTransferActive(This,pfActive)  ((This)->lpVtbl->IsBackgroundTransferActive(This,pfActive)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFByteStreamCacheControl2_INTERFACE_DEFINED__ */

#endif /* WINVER >= _WIN32_WINNT_WIN8 */

#ifndef __IMFNetCredential_INTERFACE_DEFINED__
#define __IMFNetCredential_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFNetCredential;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5b87ef6a-7ed8-434f-ba0e-184fac1628d1")IMFNetCredential:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetUser(BYTE * pbData, DWORD cbData, BOOL fDataIsEncrypted) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPassword(BYTE *pbData, DWORD cbData, BOOL fDataIsEncrypted) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUser(BYTE *pbData, DWORD *pcbData, BOOL fEncryptData) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPassword(BYTE *pbData, DWORD *pcbData, BOOL fEncryptData) = 0;
    virtual HRESULT STDMETHODCALLTYPE LoggedOnUser(BOOL *pfLoggedOnUser) = 0;
};

#else

typedef struct IMFNetCredentialVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFNetCredential * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFNetCredential * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFNetCredential * This);
    HRESULT(STDMETHODCALLTYPE *SetUser)(IMFNetCredential * This, BYTE * pbData, DWORD cbData, BOOL fDataIsEncrypted);
    HRESULT(STDMETHODCALLTYPE *SetPassword)(IMFNetCredential * This, BYTE * pbData, DWORD cbData, BOOL fDataIsEncrypted);
    HRESULT(STDMETHODCALLTYPE *GetUser)(IMFNetCredential * This, BYTE * pbData, DWORD * pcbData, BOOL fEncryptData);
    HRESULT(STDMETHODCALLTYPE *GetPassword)(IMFNetCredential * This, BYTE * pbData, DWORD * pcbData, BOOL fEncryptData);
    HRESULT(STDMETHODCALLTYPE *LoggedOnUser)(IMFNetCredential * This, BOOL * pfLoggedOnUser);
    END_INTERFACE
} IMFNetCredentialVtbl;

interface IMFNetCredential {
    CONST_VTBL struct IMFNetCredentialVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFNetCredential_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFNetCredential_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFNetCredential_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFNetCredential_SetUser(This,pbData,cbData,fDataIsEncrypted)  ((This)->lpVtbl->SetUser(This,pbData,cbData,fDataIsEncrypted))
#define IMFNetCredential_SetPassword(This,pbData,cbData,fDataIsEncrypted)  ((This)->lpVtbl->SetPassword(This,pbData,cbData,fDataIsEncrypted))
#define IMFNetCredential_GetUser(This,pbData,pcbData,fEncryptData)  ((This)->lpVtbl->GetUser(This,pbData,pcbData,fEncryptData))
#define IMFNetCredential_GetPassword(This,pbData,pcbData,fEncryptData)  ((This)->lpVtbl->GetPassword(This,pbData,pcbData,fEncryptData))
#define IMFNetCredential_LoggedOnUser(This,pfLoggedOnUser)  ((This)->lpVtbl->LoggedOnUser(This,pfLoggedOnUser))
#endif


#endif




#endif

#ifndef __IMFNetCredentialManager_INTERFACE_DEFINED__
#define __IMFNetCredentialManager_INTERFACE_DEFINED__

typedef struct _MFNetCredentialManagerGetParam {
    HRESULT hrOp;
    BOOL fAllowLoggedOnUser;
    BOOL fClearTextPackage;
    LPCWSTR pszUrl;
    LPCWSTR pszSite;
    LPCWSTR pszRealm;
    LPCWSTR pszPackage;
    LONG nRetries;
} MFNetCredentialManagerGetParam;


    EXTERN_C const IID IID_IMFNetCredentialManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5b87ef6b-7ed8-434f-ba0e-184fac1628d1")IMFNetCredentialManager:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE BeginGetCredentials(MFNetCredentialManagerGetParam * pParam, IMFAsyncCallback * pCallback, IUnknown * pState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndGetCredentials(IMFAsyncResult *pResult, IMFNetCredential **ppCred) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetGood(IMFNetCredential *pCred, BOOL fGood) = 0;
};

#else

typedef struct IMFNetCredentialManagerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFNetCredentialManager * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFNetCredentialManager * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFNetCredentialManager * This);
    HRESULT(STDMETHODCALLTYPE *BeginGetCredentials)(IMFNetCredentialManager * This, MFNetCredentialManagerGetParam * pParam, IMFAsyncCallback * pCallback, IUnknown * pState);
    HRESULT(STDMETHODCALLTYPE *EndGetCredentials)(IMFNetCredentialManager * This, IMFAsyncResult * pResult, IMFNetCredential ** ppCred);
    HRESULT(STDMETHODCALLTYPE *SetGood)(IMFNetCredentialManager * This, IMFNetCredential * pCred, BOOL fGood);
    END_INTERFACE
} IMFNetCredentialManagerVtbl;

interface IMFNetCredentialManager {
    CONST_VTBL struct IMFNetCredentialManagerVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFNetCredentialManager_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFNetCredentialManager_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFNetCredentialManager_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFNetCredentialManager_BeginGetCredentials(This,pParam,pCallback,pState)  ((This)->lpVtbl->BeginGetCredentials(This,pParam,pCallback,pState))
#define IMFNetCredentialManager_EndGetCredentials(This,pResult,ppCred)  ((This)->lpVtbl->EndGetCredentials(This,pResult,ppCred))
#define IMFNetCredentialManager_SetGood(This,pCred,fGood)  ((This)->lpVtbl->SetGood(This,pCred,fGood))
#endif


#endif




#endif

#ifndef __IMFNetCredentialCache_INTERFACE_DEFINED__
#define __IMFNetCredentialCache_INTERFACE_DEFINED__

typedef enum _MFNetCredentialRequirements {
    REQUIRE_PROMPT = 0x1,
    REQUIRE_SAVE_SELECTED = 0x2
} MFNetCredentialRequirements;

typedef enum _MFNetCredentialOptions {
    MFNET_CREDENTIAL_SAVE = 0x1,
    MFNET_CREDENTIAL_DONT_CACHE = 0x2,
    MFNET_CREDENTIAL_ALLOW_CLEAR_TEXT = 0x4
} MFNetCredentialOptions;

typedef enum _MFNetAuthenticationFlags {
    MFNET_AUTHENTICATION_PROXY = 0x1,
    MFNET_AUTHENTICATION_CLEAR_TEXT = 0x2,
    MFNET_AUTHENTICATION_LOGGED_ON_USER = 0x4
} MFNetAuthenticationFlags;

EXTERN_C const IID IID_IMFNetCredentialCache;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5b87ef6c-7ed8-434f-ba0e-184fac1628d1")IMFNetCredentialCache:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCredential(LPCWSTR pszUrl, LPCWSTR pszRealm, DWORD dwAuthenticationFlags, IMFNetCredential ** ppCred, DWORD * pdwRequirementsFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetGood(IMFNetCredential *pCred, BOOL fGood) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetUserOptions(IMFNetCredential *pCred, DWORD dwOptionsFlags) = 0;
};

#else

typedef struct IMFNetCredentialCacheVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFNetCredentialCache * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFNetCredentialCache * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFNetCredentialCache * This);
    HRESULT(STDMETHODCALLTYPE *GetCredential)(IMFNetCredentialCache * This, LPCWSTR pszUrl, LPCWSTR pszRealm, DWORD dwAuthenticationFlags, IMFNetCredential ** ppCred, DWORD * pdwRequirementsFlags);
    HRESULT(STDMETHODCALLTYPE *SetGood)(IMFNetCredentialCache * This, IMFNetCredential * pCred, BOOL fGood);
    HRESULT(STDMETHODCALLTYPE *SetUserOptions)(IMFNetCredentialCache * This, IMFNetCredential * pCred, DWORD dwOptionsFlags);
    END_INTERFACE
} IMFNetCredentialCacheVtbl;

interface IMFNetCredentialCache {
    CONST_VTBL struct IMFNetCredentialCacheVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFNetCredentialCache_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFNetCredentialCache_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFNetCredentialCache_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFNetCredentialCache_GetCredential(This,pszUrl,pszRealm,dwAuthenticationFlags,ppCred,pdwRequirementsFlags)  ((This)->lpVtbl->GetCredential(This,pszUrl,pszRealm,dwAuthenticationFlags,ppCred,pdwRequirementsFlags))
#define IMFNetCredentialCache_SetGood(This,pCred,fGood)  ((This)->lpVtbl->SetGood(This,pCred,fGood))
#define IMFNetCredentialCache_SetUserOptions(This,pCred,dwOptionsFlags)  ((This)->lpVtbl->SetUserOptions(This,pCred,dwOptionsFlags))
#endif


#endif




#endif

STDAPI MFCreateCredentialCache(IMFNetCredentialCache **ppCache);

#if (WINVER >= _WIN32_WINNT_WIN7)

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0049_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0049_v0_0_s_ifspec;

#ifndef __IMFSSLCertificateManager_INTERFACE_DEFINED__
#define __IMFSSLCertificateManager_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSSLCertificateManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("61f7d887-1230-4a8b-aeba-8ad434d1a64d")IMFSSLCertificateManager:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetClientCertificate(LPCWSTR pszURL, BYTE ** ppbData, DWORD * pcbData) = 0;
    virtual HRESULT STDMETHODCALLTYPE BeginGetClientCertificate(LPCWSTR pszURL, IMFAsyncCallback *pCallback, IUnknown *pState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndGetClientCertificate(IMFAsyncResult *pResult, BYTE **ppbData, DWORD *pcbData) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCertificatePolicy(LPCWSTR pszURL, BOOL *pfOverrideAutomaticCheck, BOOL *pfClientCertificateAvailable) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnServerCertificate(LPCWSTR pszURL, BYTE *pbData, DWORD cbData, BOOL *pfIsGood) = 0;
};
#else
typedef struct IMFSSLCertificateManagerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSSLCertificateManager * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSSLCertificateManager * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSSLCertificateManager * This);
    HRESULT(STDMETHODCALLTYPE *GetClientCertificate)(IMFSSLCertificateManager * This, LPCWSTR pszURL, BYTE ** ppbData, DWORD * pcbData);
    HRESULT(STDMETHODCALLTYPE *BeginGetClientCertificate)(IMFSSLCertificateManager * This, LPCWSTR pszURL, IMFAsyncCallback * pCallback, IUnknown * pState);
    HRESULT(STDMETHODCALLTYPE *EndGetClientCertificate)(IMFSSLCertificateManager * This, IMFAsyncResult * pResult, BYTE ** ppbData, DWORD * pcbData);
    HRESULT(STDMETHODCALLTYPE *GetCertificatePolicy)(IMFSSLCertificateManager * This, LPCWSTR pszURL, BOOL * pfOverrideAutomaticCheck, BOOL * pfClientCertificateAvailable);
    HRESULT(STDMETHODCALLTYPE *OnServerCertificate)(IMFSSLCertificateManager * This, LPCWSTR pszURL, BYTE * pbData, DWORD cbData, BOOL * pfIsGood);
    END_INTERFACE
} IMFSSLCertificateManagerVtbl;

interface IMFSSLCertificateManager {
    CONST_VTBL struct IMFSSLCertificateManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSSLCertificateManager_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSSLCertificateManager_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSSLCertificateManager_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFSSLCertificateManager_GetClientCertificate(This,pszURL,ppbData,pcbData)  ((This)->lpVtbl->GetClientCertificate(This,pszURL,ppbData,pcbData))
#define IMFSSLCertificateManager_BeginGetClientCertificate(This,pszURL,pCallback,pState)  ((This)->lpVtbl->BeginGetClientCertificate(This,pszURL,pCallback,pState))
#define IMFSSLCertificateManager_EndGetClientCertificate(This,pResult,ppbData,pcbData)  ((This)->lpVtbl->EndGetClientCertificate(This,pResult,ppbData,pcbData))
#define IMFSSLCertificateManager_GetCertificatePolicy(This,pszURL,pfOverrideAutomaticCheck,pfClientCertificateAvailable)  ((This)->lpVtbl->GetCertificatePolicy(This,pszURL,pfOverrideAutomaticCheck,pfClientCertificateAvailable))
#define IMFSSLCertificateManager_OnServerCertificate(This,pszURL,pbData,cbData,pfIsGood)  ((This)->lpVtbl->OnServerCertificate(This,pszURL,pbData,cbData,pfIsGood))
#endif

#endif

#endif

EXTERN_GUID(MFNETSOURCE_SSLCERTIFICATE_MANAGER, 0x55e6cb27, 0xe69b, 0x4267, 0x94, 0x0c, 0x2d, 0x7e, 0xc5, 0xbb, 0x8a, 0x0f);

#ifndef __IMFNetResourceFilter_INTERFACE_DEFINED__
#define __IMFNetResourceFilter_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFNetResourceFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("091878a3-bf11-4a5c-bc9f-33995b06ef2d")IMFNetResourceFilter:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnRedirect(LPCWSTR pszUrl, VARIANT_BOOL * pvbCancel) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnSendingRequest(LPCWSTR pszUrl) = 0;
};
#else /* C style interface */
typedef struct IMFNetResourceFilterVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFNetResourceFilter * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IMFNetResourceFilter * This);
    ULONG(STDMETHODCALLTYPE * Release) (IMFNetResourceFilter * This);
    HRESULT(STDMETHODCALLTYPE * OnRedirect) (IMFNetResourceFilter * This, LPCWSTR pszUrl, VARIANT_BOOL * pvbCancel);
    HRESULT(STDMETHODCALLTYPE * OnSendingRequest) (IMFNetResourceFilter * This, LPCWSTR pszUrl);
    END_INTERFACE
} IMFNetResourceFilterVtbl;

interface IMFNetResourceFilter {
    CONST_VTBL struct IMFNetResourceFilterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFNetResourceFilter_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFNetResourceFilter_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFNetResourceFilter_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFNetResourceFilter_OnRedirect(This,pszUrl,pvbCancel)  ((This)->lpVtbl->OnRedirect(This,pszUrl,pvbCancel)) 
#define IMFNetResourceFilter_OnSendingRequest(This,pszUrl)  ((This)->lpVtbl->OnSendingRequest(This,pszUrl)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFNetResourceFilter_INTERFACE_DEFINED__ */

#endif /* WINVER >= _WIN32_WINNT_WIN7 */

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0050_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0050_v0_0_s_ifspec;

#ifndef __IMFSourceOpenMonitor_INTERFACE_DEFINED__
#define __IMFSourceOpenMonitor_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSourceOpenMonitor;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("059054B3-027C-494C-A27D-9113291CF87F")IMFSourceOpenMonitor:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnSourceEvent(IMFMediaEvent * pEvent) = 0;
};
#else
typedef struct IMFSourceOpenMonitorVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSourceOpenMonitor * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSourceOpenMonitor * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSourceOpenMonitor * This);
    HRESULT(STDMETHODCALLTYPE *OnSourceEvent)(IMFSourceOpenMonitor * This, IMFMediaEvent * pEvent);
    END_INTERFACE
} IMFSourceOpenMonitorVtbl;

interface IMFSourceOpenMonitor {
    CONST_VTBL struct IMFSourceOpenMonitorVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFSourceOpenMonitor_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSourceOpenMonitor_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSourceOpenMonitor_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFSourceOpenMonitor_OnSourceEvent(This,pEvent)  ((This)->lpVtbl->OnSourceEvent(This,pEvent))
#endif


#endif




#endif

#ifndef __IMFNetProxyLocator_INTERFACE_DEFINED__
#define __IMFNetProxyLocator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFNetProxyLocator;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e9cd0383-a268-4bb4-82de-658d53574d41")IMFNetProxyLocator:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE FindFirstProxy(LPCWSTR pszHost, LPCWSTR pszUrl, BOOL fReserved) = 0;
    virtual HRESULT STDMETHODCALLTYPE FindNextProxy(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE RegisterProxyResult(HRESULT hrOp) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCurrentProxy(LPWSTR pszStr, DWORD *pcchStr) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clone(IMFNetProxyLocator **ppProxyLocator) = 0;
};
#else
typedef struct IMFNetProxyLocatorVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFNetProxyLocator * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFNetProxyLocator * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFNetProxyLocator * This);
    HRESULT(STDMETHODCALLTYPE *FindFirstProxy)(IMFNetProxyLocator * This, LPCWSTR pszHost, LPCWSTR pszUrl, BOOL fReserved);
    HRESULT(STDMETHODCALLTYPE *FindNextProxy)(IMFNetProxyLocator * This);
    HRESULT(STDMETHODCALLTYPE *RegisterProxyResult)(IMFNetProxyLocator * This, HRESULT hrOp);
    HRESULT(STDMETHODCALLTYPE *GetCurrentProxy)(IMFNetProxyLocator * This, LPWSTR pszStr, DWORD * pcchStr);
    HRESULT(STDMETHODCALLTYPE *Clone)(IMFNetProxyLocator * This, IMFNetProxyLocator ** ppProxyLocator);
    END_INTERFACE
} IMFNetProxyLocatorVtbl;

interface IMFNetProxyLocator {
    CONST_VTBL struct IMFNetProxyLocatorVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFNetProxyLocator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFNetProxyLocator_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFNetProxyLocator_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFNetProxyLocator_FindFirstProxy(This,pszHost,pszUrl,fReserved)  ((This)->lpVtbl->FindFirstProxy(This,pszHost,pszUrl,fReserved))
#define IMFNetProxyLocator_FindNextProxy(This)  ((This)->lpVtbl->FindNextProxy(This))
#define IMFNetProxyLocator_RegisterProxyResult(This,hrOp)  ((This)->lpVtbl->RegisterProxyResult(This,hrOp))
#define IMFNetProxyLocator_GetCurrentProxy(This,pszStr,pcchStr)  ((This)->lpVtbl->GetCurrentProxy(This,pszStr,pcchStr))
#define IMFNetProxyLocator_Clone(This,ppProxyLocator)  ((This)->lpVtbl->Clone(This,ppProxyLocator))
#endif


#endif




#endif

STDAPI MFCreateProxyLocator(LPCWSTR pszProtocol, IPropertyStore *pProxyConfig, IMFNetProxyLocator **ppProxyLocator);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0052_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0052_v0_0_s_ifspec;

#ifndef __IMFNetProxyLocatorFactory_INTERFACE_DEFINED__
#define __IMFNetProxyLocatorFactory_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFNetProxyLocatorFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e9cd0384-a268-4bb4-82de-658d53574d41")IMFNetProxyLocatorFactory:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateProxyLocator(LPCWSTR pszProtocol, IMFNetProxyLocator ** ppProxyLocator) = 0;
};

#else

typedef struct IMFNetProxyLocatorFactoryVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFNetProxyLocatorFactory * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFNetProxyLocatorFactory * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFNetProxyLocatorFactory * This);
    HRESULT(STDMETHODCALLTYPE *CreateProxyLocator)(IMFNetProxyLocatorFactory * This, LPCWSTR pszProtocol, IMFNetProxyLocator ** ppProxyLocator);
    END_INTERFACE
} IMFNetProxyLocatorFactoryVtbl;

interface IMFNetProxyLocatorFactory {
    CONST_VTBL struct IMFNetProxyLocatorFactoryVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFNetProxyLocatorFactory_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFNetProxyLocatorFactory_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFNetProxyLocatorFactory_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFNetProxyLocatorFactory_CreateProxyLocator(This,pszProtocol,ppProxyLocator)  ((This)->lpVtbl->CreateProxyLocator(This,pszProtocol,ppProxyLocator))
#endif


#endif




#endif

#ifndef __IMFSaveJob_INTERFACE_DEFINED__
#define __IMFSaveJob_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSaveJob;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e9931663-80bf-4c6e-98af-5dcf58747d1f")IMFSaveJob:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE BeginSave(IMFByteStream * pStream, IMFAsyncCallback * pCallback, IUnknown * pState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndSave(IMFAsyncResult *pResult) = 0;
    virtual HRESULT STDMETHODCALLTYPE CancelSave(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProgress(DWORD *pdwPercentComplete) = 0;
};

#else

typedef struct IMFSaveJobVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSaveJob * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSaveJob * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSaveJob * This);
    HRESULT(STDMETHODCALLTYPE *BeginSave)(IMFSaveJob * This, IMFByteStream * pStream, IMFAsyncCallback * pCallback, IUnknown * pState);
    HRESULT(STDMETHODCALLTYPE *EndSave)(IMFSaveJob * This, IMFAsyncResult * pResult);
    HRESULT(STDMETHODCALLTYPE *CancelSave)(IMFSaveJob * This);
    HRESULT(STDMETHODCALLTYPE *GetProgress)(IMFSaveJob * This, DWORD * pdwPercentComplete);
    END_INTERFACE
} IMFSaveJobVtbl;

interface IMFSaveJob {
    CONST_VTBL struct IMFSaveJobVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFSaveJob_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSaveJob_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSaveJob_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFSaveJob_BeginSave(This,pStream,pCallback,pState)  ((This)->lpVtbl->BeginSave(This,pStream,pCallback,pState))
#define IMFSaveJob_EndSave(This,pResult)  ((This)->lpVtbl->EndSave(This,pResult))
#define IMFSaveJob_CancelSave(This)  ((This)->lpVtbl->CancelSave(This))
#define IMFSaveJob_GetProgress(This,pdwPercentComplete)  ((This)->lpVtbl->GetProgress(This,pdwPercentComplete))
#endif


#endif




#endif

EXTERN_GUID(MFNET_SAVEJOB_SERVICE, 0xb85a587f, 0x3d02, 0x4e52, 0x95, 0x65, 0x55, 0xd3, 0xec, 0x1e, 0x7f, 0xf7);

typedef enum _MFNETSOURCE_PROTOCOL_TYPE {
    MFNETSOURCE_UNDEFINED = 0,
    MFNETSOURCE_HTTP = 0x1,
    MFNETSOURCE_RTSP = 0x2,
    MFNETSOURCE_FILE = 0x3,
    MFNETSOURCE_MULTICAST = 0x4
} MFNETSOURCE_PROTOCOL_TYPE;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0054_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0054_v0_0_s_ifspec;

#ifndef __IMFNetSchemeHandlerConfig_INTERFACE_DEFINED__
#define __IMFNetSchemeHandlerConfig_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFNetSchemeHandlerConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7BE19E73-C9BF-468a-AC5A-A5E8653BEC87")IMFNetSchemeHandlerConfig:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetNumberOfSupportedProtocols(ULONG * pcProtocols) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSupportedProtocolType(ULONG nProtocolIndex, MFNETSOURCE_PROTOCOL_TYPE *pnProtocolType) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetProtocolRolloverSettings(void) = 0;
};

#else

typedef struct IMFNetSchemeHandlerConfigVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFNetSchemeHandlerConfig * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFNetSchemeHandlerConfig * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFNetSchemeHandlerConfig * This);
    HRESULT(STDMETHODCALLTYPE *GetNumberOfSupportedProtocols)(IMFNetSchemeHandlerConfig * This, ULONG * pcProtocols);
    HRESULT(STDMETHODCALLTYPE *GetSupportedProtocolType)(IMFNetSchemeHandlerConfig * This, ULONG nProtocolIndex, MFNETSOURCE_PROTOCOL_TYPE * pnProtocolType);
    HRESULT(STDMETHODCALLTYPE *ResetProtocolRolloverSettings)(IMFNetSchemeHandlerConfig * This);
    END_INTERFACE
} IMFNetSchemeHandlerConfigVtbl;

interface IMFNetSchemeHandlerConfig {
    CONST_VTBL struct IMFNetSchemeHandlerConfigVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFNetSchemeHandlerConfig_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFNetSchemeHandlerConfig_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFNetSchemeHandlerConfig_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFNetSchemeHandlerConfig_GetNumberOfSupportedProtocols(This,pcProtocols)  ((This)->lpVtbl->GetNumberOfSupportedProtocols(This,pcProtocols))
#define IMFNetSchemeHandlerConfig_GetSupportedProtocolType(This,nProtocolIndex,pnProtocolType)  ((This)->lpVtbl->GetSupportedProtocolType(This,nProtocolIndex,pnProtocolType))
#define IMFNetSchemeHandlerConfig_ResetProtocolRolloverSettings(This)  ((This)->lpVtbl->ResetProtocolRolloverSettings(This))
#endif


#endif




#endif

STDAPI MFCreateNetSchemePlugin(REFIID riid, LPVOID *ppvHandler);

typedef enum _MFNETSOURCE_TRANSPORT_TYPE {
    MFNETSOURCE_UDP = 0,
    MFNETSOURCE_TCP = (MFNETSOURCE_UDP + 1)
} MFNETSOURCE_TRANSPORT_TYPE;

typedef enum _MFNETSOURCE_CACHE_STATE {
    MFNETSOURCE_CACHE_UNAVAILABLE = 0,
    MFNETSOURCE_CACHE_ACTIVE_WRITING = (MFNETSOURCE_CACHE_UNAVAILABLE + 1),
    MFNETSOURCE_CACHE_ACTIVE_COMPLETE = (MFNETSOURCE_CACHE_ACTIVE_WRITING + 1)
} MFNETSOURCE_CACHE_STATE;

typedef enum _MFNETSOURCE_STATISTICS_IDS {
    MFNETSOURCE_RECVPACKETS_ID = 0,
    MFNETSOURCE_LOSTPACKETS_ID = (MFNETSOURCE_RECVPACKETS_ID + 1),
    MFNETSOURCE_RESENDSREQUESTED_ID = (MFNETSOURCE_LOSTPACKETS_ID + 1),
    MFNETSOURCE_RESENDSRECEIVED_ID = (MFNETSOURCE_RESENDSREQUESTED_ID + 1),
    MFNETSOURCE_RECOVEREDBYECCPACKETS_ID = (MFNETSOURCE_RESENDSRECEIVED_ID + 1),
    MFNETSOURCE_RECOVEREDBYRTXPACKETS_ID = (MFNETSOURCE_RECOVEREDBYECCPACKETS_ID + 1),
    MFNETSOURCE_OUTPACKETS_ID = (MFNETSOURCE_RECOVEREDBYRTXPACKETS_ID + 1),
    MFNETSOURCE_RECVRATE_ID = (MFNETSOURCE_OUTPACKETS_ID + 1),
    MFNETSOURCE_AVGBANDWIDTHBPS_ID = (MFNETSOURCE_RECVRATE_ID + 1),
    MFNETSOURCE_BYTESRECEIVED_ID = (MFNETSOURCE_AVGBANDWIDTHBPS_ID + 1),
    MFNETSOURCE_PROTOCOL_ID = (MFNETSOURCE_BYTESRECEIVED_ID + 1),
    MFNETSOURCE_TRANSPORT_ID = (MFNETSOURCE_PROTOCOL_ID + 1),
    MFNETSOURCE_CACHE_STATE_ID = (MFNETSOURCE_TRANSPORT_ID + 1),
    MFNETSOURCE_LINKBANDWIDTH_ID = (MFNETSOURCE_CACHE_STATE_ID + 1),
    MFNETSOURCE_CONTENTBITRATE_ID = (MFNETSOURCE_LINKBANDWIDTH_ID + 1),
    MFNETSOURCE_SPEEDFACTOR_ID = (MFNETSOURCE_CONTENTBITRATE_ID + 1),
    MFNETSOURCE_BUFFERSIZE_ID = (MFNETSOURCE_SPEEDFACTOR_ID + 1),
    MFNETSOURCE_BUFFERPROGRESS_ID = (MFNETSOURCE_BUFFERSIZE_ID + 1),
    MFNETSOURCE_LASTBWSWITCHTS_ID = (MFNETSOURCE_BUFFERPROGRESS_ID + 1),
    MFNETSOURCE_SEEKRANGESTART_ID = (MFNETSOURCE_LASTBWSWITCHTS_ID + 1),
    MFNETSOURCE_SEEKRANGEEND_ID = (MFNETSOURCE_SEEKRANGESTART_ID + 1),
    MFNETSOURCE_BUFFERINGCOUNT_ID = (MFNETSOURCE_SEEKRANGEEND_ID + 1),
    MFNETSOURCE_INCORRECTLYSIGNEDPACKETS_ID = (MFNETSOURCE_BUFFERINGCOUNT_ID + 1),
    MFNETSOURCE_SIGNEDSESSION_ID = (MFNETSOURCE_INCORRECTLYSIGNEDPACKETS_ID + 1),
    MFNETSOURCE_MAXBITRATE_ID = (MFNETSOURCE_SIGNEDSESSION_ID + 1),
    MFNETSOURCE_RECEPTION_QUALITY_ID = (MFNETSOURCE_MAXBITRATE_ID + 1),
    MFNETSOURCE_RECOVEREDPACKETS_ID = (MFNETSOURCE_RECEPTION_QUALITY_ID + 1),
    MFNETSOURCE_VBR_ID = (MFNETSOURCE_RECOVEREDPACKETS_ID + 1),
    MFNETSOURCE_DOWNLOADPROGRESS_ID = (MFNETSOURCE_VBR_ID + 1),
    MFNETSOURCE_UNPREDEFINEDPROTOCOLNAME_ID = (MFNETSOURCE_DOWNLOADPROGRESS_ID + 1)
} MFNETSOURCE_STATISTICS_IDS;

EXTERN_GUID(MFNETSOURCE_STATISTICS_SERVICE, 0x3cb1f275, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_STATISTICS, 0x3cb1f274, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_BUFFERINGTIME, 0x3cb1f276, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_ACCELERATEDSTREAMINGDURATION, 0x3cb1f277, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_MAXUDPACCELERATEDSTREAMINGDURATION, 0x4aab2879, 0xbbe1, 0x4994, 0x9f, 0xf0, 0x54, 0x95, 0xbd, 0x25, 0x1, 0x29);
EXTERN_GUID(MFNETSOURCE_MAXBUFFERTIMEMS, 0x408b24e6, 0x4038, 0x4401, 0xb5, 0xb2, 0xfe, 0x70, 0x1a, 0x9e, 0xbf, 0x10);
EXTERN_GUID(MFNETSOURCE_CONNECTIONBANDWIDTH, 0x3cb1f278, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_CACHEENABLED, 0x3cb1f279, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_AUTORECONNECTLIMIT, 0x3cb1f27a, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_RESENDSENABLED, 0x3cb1f27b, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_THINNINGENABLED, 0x3cb1f27c, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_PROTOCOL, 0x3cb1f27d, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_TRANSPORT, 0x3cb1f27e, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
#if (WINVER >= _WIN32_WINNT_WIN7)
EXTERN_GUID(MFNETSOURCE_PREVIEWMODEENABLED, 0x3cb1f27f, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
#endif
EXTERN_GUID(MFNETSOURCE_CREDENTIAL_MANAGER, 0x3cb1f280, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_PPBANDWIDTH, 0x3cb1f281, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_AUTORECONNECTPROGRESS, 0x3cb1f282, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_PROXYLOCATORFACTORY, 0x3cb1f283, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_BROWSERUSERAGENT, 0x3cb1f28b, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_BROWSERWEBPAGE, 0x3cb1f28c, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_PLAYERVERSION, 0x3cb1f28d, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_PLAYERID, 0x3cb1f28e, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_HOSTEXE, 0x3cb1f28f, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_HOSTVERSION, 0x3cb1f291, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_PLAYERUSERAGENT, 0x3cb1f292, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
#if (WINVER >= _WIN32_WINNT_WIN7)
EXTERN_GUID(MFNETSOURCE_CLIENTGUID, 0x60a2c4a6, 0xf197, 0x4c14, 0xa5, 0xbf, 0x88, 0x83, 0xd, 0x24, 0x58, 0xaf);
#endif
EXTERN_GUID(MFNETSOURCE_LOGURL, 0x3cb1f293, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_ENABLE_UDP, 0x3cb1f294, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_ENABLE_TCP, 0x3cb1f295, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_ENABLE_MSB, 0x3cb1f296, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_ENABLE_RTSP, 0x3cb1f298, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_ENABLE_HTTP, 0x3cb1f299, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_ENABLE_STREAMING, 0x3cb1f29c, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_ENABLE_DOWNLOAD, 0x3cb1f29d, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_ENABLE_PRIVATEMODE, 0x824779d8, 0xf18b, 0x4405, 0x8c, 0xf1, 0x46, 0x4f, 0xb5, 0xaa, 0x8f, 0x71 );
EXTERN_GUID(MFNETSOURCE_UDP_PORT_RANGE, 0x3cb1f29a, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_PROXYINFO, 0x3cb1f29b, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_DRMNET_LICENSE_REPRESENTATION, 0x47eae1bd, 0xbdfe, 0x42e2, 0x82, 0xf3, 0x54, 0xa4, 0x8c, 0x17, 0x96, 0x2d);
EXTERN_GUID(MFNETSOURCE_PROXYSETTINGS, 0x3cb1f287, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_PROXYHOSTNAME, 0x3cb1f284, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_PROXYPORT, 0x3cb1f288, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_PROXYEXCEPTIONLIST, 0x3cb1f285, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_PROXYBYPASSFORLOCAL, 0x3cb1f286, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
EXTERN_GUID(MFNETSOURCE_PROXYRERUNAUTODETECTION, 0x3cb1f289, 0x0505, 0x4c5d, 0xae, 0x71, 0x0a, 0x55, 0x63, 0x44, 0xef, 0xa1);
#if (WINVER >= _WIN32_WINNT_WIN7)
EXTERN_GUID(MFNETSOURCE_STREAM_LANGUAGE, 0x9ab44318, 0xf7cd, 0x4f2d, 0x8d, 0x6d, 0xfa, 0x35, 0xb4, 0x92, 0xce, 0xcb);
EXTERN_GUID(MFNETSOURCE_LOGPARAMS, 0x64936ae8, 0x9418, 0x453a, 0x8c, 0xda, 0x3e, 0xa, 0x66, 0x8b, 0x35, 0x3b);
#endif /* WINVER >= _WIN32_WINNT_WIN7 */
#if (WINVER >= _WIN32_WINNT_WIN8) 
EXTERN_GUID(MFNETSOURCE_PEERMANAGER, 0x48b29adb, 0xfebf, 0x45ee, 0xa9, 0xbf, 0xef, 0xb8, 0x1c, 0x49, 0x2e, 0xfc );
EXTERN_GUID(MFNETSOURCE_FRIENDLYNAME, 0x5b2a7757, 0xbc6b, 0x447e, 0xaa, 0x06, 0x0d, 0xda, 0x1c, 0x64, 0x6e, 0x2f );
#endif /* WINVER >= _WIN32_WINNT_WIN8 */

typedef enum _MFNET_PROXYSETTINGS {
    MFNET_PROXYSETTING_NONE = 0,
    MFNET_PROXYSETTING_MANUAL = 1,
    MFNET_PROXYSETTING_AUTO = 2,
    MFNET_PROXYSETTING_BROWSER = 3
} MFNET_PROXYSETTINGS;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0055_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0055_v0_0_s_ifspec;

#ifndef __IMFSchemeHandler_INTERFACE_DEFINED__
#define __IMFSchemeHandler_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSchemeHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("6D4C7B74-52A0-4bb7-B0DB-55F29F47A668")IMFSchemeHandler:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE BeginCreateObject(LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore * pProps, IUnknown ** ppIUnknownCancelCookie, IMFAsyncCallback * pCallback, IUnknown * punkState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndCreateObject(IMFAsyncResult *pResult, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE CancelObjectCreation(IUnknown *pIUnknownCancelCookie) = 0;
};

#else

typedef struct IMFSchemeHandlerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSchemeHandler * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSchemeHandler * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSchemeHandler * This);
    HRESULT(STDMETHODCALLTYPE *BeginCreateObject)(IMFSchemeHandler * This, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore * pProps, IUnknown ** ppIUnknownCancelCookie, IMFAsyncCallback * pCallback, IUnknown * punkState);
    HRESULT(STDMETHODCALLTYPE *EndCreateObject)(IMFSchemeHandler * This, IMFAsyncResult * pResult, MF_OBJECT_TYPE * pObjectType, IUnknown ** ppObject);
    HRESULT(STDMETHODCALLTYPE *CancelObjectCreation)(IMFSchemeHandler * This, IUnknown * pIUnknownCancelCookie);
    END_INTERFACE
} IMFSchemeHandlerVtbl;

interface IMFSchemeHandler {
    CONST_VTBL struct IMFSchemeHandlerVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFSchemeHandler_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSchemeHandler_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSchemeHandler_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFSchemeHandler_BeginCreateObject(This,pwszURL,dwFlags,pProps,ppIUnknownCancelCookie,pCallback,punkState)  ((This)->lpVtbl->BeginCreateObject(This,pwszURL,dwFlags,pProps,ppIUnknownCancelCookie,pCallback,punkState))
#define IMFSchemeHandler_EndCreateObject(This,pResult,pObjectType,ppObject)  ((This)->lpVtbl->EndCreateObject(This,pResult,pObjectType,ppObject))
#define IMFSchemeHandler_CancelObjectCreation(This,pIUnknownCancelCookie)  ((This)->lpVtbl->CancelObjectCreation(This,pIUnknownCancelCookie))
#endif


#endif




#endif

#if (WINVER >= _WIN32_WINNT_WIN7)
EXTERN_GUID(MF_BYTESTREAMHANDLER_ACCEPTS_SHARE_WRITE, 0xa6e1f733, 0x3001, 0x4915, 0x81, 0x50, 0x15, 0x58, 0xa2, 0x18, 0xe, 0xc8);
#endif

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0056_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0056_v0_0_s_ifspec;

#ifndef __IMFByteStreamHandler_INTERFACE_DEFINED__
#define __IMFByteStreamHandler_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFByteStreamHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("BB420AA4-765B-4a1f-91FE-D6A8A143924C")IMFByteStreamHandler:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE BeginCreateObject(IMFByteStream * pByteStream, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore * pProps, IUnknown ** ppIUnknownCancelCookie, IMFAsyncCallback * pCallback, IUnknown * punkState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndCreateObject(IMFAsyncResult *pResult, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE CancelObjectCreation(IUnknown *pIUnknownCancelCookie) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMaxNumberOfBytesRequiredForResolution(QWORD *pqwBytes) = 0;
};

#else

typedef struct IMFByteStreamHandlerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFByteStreamHandler * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFByteStreamHandler * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFByteStreamHandler * This);
    HRESULT(STDMETHODCALLTYPE *BeginCreateObject)(IMFByteStreamHandler * This, IMFByteStream * pByteStream, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore * pProps, IUnknown ** ppIUnknownCancelCookie, IMFAsyncCallback * pCallback, IUnknown * punkState);
    HRESULT(STDMETHODCALLTYPE *EndCreateObject)(IMFByteStreamHandler * This, IMFAsyncResult * pResult, MF_OBJECT_TYPE * pObjectType, IUnknown ** ppObject);
    HRESULT(STDMETHODCALLTYPE *CancelObjectCreation)(IMFByteStreamHandler * This, IUnknown * pIUnknownCancelCookie);
    HRESULT(STDMETHODCALLTYPE *GetMaxNumberOfBytesRequiredForResolution)(IMFByteStreamHandler * This, QWORD * pqwBytes);
    END_INTERFACE
} IMFByteStreamHandlerVtbl;

interface IMFByteStreamHandler {
    CONST_VTBL struct IMFByteStreamHandlerVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFByteStreamHandler_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFByteStreamHandler_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFByteStreamHandler_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFByteStreamHandler_BeginCreateObject(This,pByteStream,pwszURL,dwFlags,pProps,ppIUnknownCancelCookie,pCallback,punkState)  ((This)->lpVtbl->BeginCreateObject(This,pByteStream,pwszURL,dwFlags,pProps,ppIUnknownCancelCookie,pCallback,punkState))
#define IMFByteStreamHandler_EndCreateObject(This,pResult,pObjectType,ppObject)  ((This)->lpVtbl->EndCreateObject(This,pResult,pObjectType,ppObject))
#define IMFByteStreamHandler_CancelObjectCreation(This,pIUnknownCancelCookie)  ((This)->lpVtbl->CancelObjectCreation(This,pIUnknownCancelCookie))
#define IMFByteStreamHandler_GetMaxNumberOfBytesRequiredForResolution(This,pqwBytes)  ((This)->lpVtbl->GetMaxNumberOfBytesRequiredForResolution(This,pqwBytes))
#endif


#endif




#endif

EXTERN_GUID(MF_BYTESTREAM_SERVICE, 0xab025e2b, 0x16d9, 0x4180, 0xa1, 0x27, 0xba, 0x6c, 0x70, 0x15, 0x61, 0x61 );

#ifndef __IMFTrustedInput_INTERFACE_DEFINED__
#define __IMFTrustedInput_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTrustedInput;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("542612C4-A1B8-4632-B521-DE11EA64A0B0")IMFTrustedInput:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetInputTrustAuthority(DWORD dwStreamID, REFIID riid, IUnknown ** ppunkObject) = 0;
};

#else

typedef struct IMFTrustedInputVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTrustedInput * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFTrustedInput * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFTrustedInput * This);
    HRESULT(STDMETHODCALLTYPE *GetInputTrustAuthority)(IMFTrustedInput * This, DWORD dwStreamID, REFIID riid, IUnknown ** ppunkObject);
    END_INTERFACE
} IMFTrustedInputVtbl;

interface IMFTrustedInput {
    CONST_VTBL struct IMFTrustedInputVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFTrustedInput_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFTrustedInput_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFTrustedInput_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFTrustedInput_GetInputTrustAuthority(This,dwStreamID,riid,ppunkObject)  ((This)->lpVtbl->GetInputTrustAuthority(This,dwStreamID,riid,ppunkObject))
#endif


#endif




#endif

typedef enum _MFPOLICYMANAGER_ACTION {
    PEACTION_NO = 0,
    PEACTION_PLAY = 1,
    PEACTION_COPY = 2,
    PEACTION_EXPORT = 3,
    PEACTION_EXTRACT = 4,
    PEACTION_RESERVED1 = 5,
    PEACTION_RESERVED2 = 6,
    PEACTION_RESERVED3 = 7,
    PEACTION_LAST = 7
} MFPOLICYMANAGER_ACTION;

typedef struct _MFINPUTTRUSTAUTHORITY_ACTION {
    MFPOLICYMANAGER_ACTION Action;
    BYTE *pbTicket;
    DWORD cbTicket;
} MFINPUTTRUSTAUTHORITY_ACCESS_ACTION;

typedef struct _MFINPUTTRUSTAUTHORITY_ACCESS_PARAMS {
    DWORD dwSize;
    DWORD dwVer;
    DWORD cbSignatureOffset;
    DWORD cbSignatureSize;
    DWORD cbExtensionOffset;
    DWORD cbExtensionSize;
    DWORD cActions;
    MFINPUTTRUSTAUTHORITY_ACCESS_ACTION rgOutputActions[1];
} MFINPUTTRUSTAUTHORITY_ACCESS_PARAMS;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0058_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0058_v0_0_s_ifspec;

#ifndef __IMFInputTrustAuthority_INTERFACE_DEFINED__
#define __IMFInputTrustAuthority_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFInputTrustAuthority;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("D19F8E98-B126-4446-890C-5DCB7AD71453")IMFInputTrustAuthority:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetDecrypter(REFIID riid, void **ppv) = 0;
    virtual HRESULT STDMETHODCALLTYPE RequestAccess(MFPOLICYMANAGER_ACTION Action, IMFActivate **ppContentEnablerActivate) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPolicy(MFPOLICYMANAGER_ACTION Action, IMFOutputPolicy **ppPolicy) = 0;
    virtual HRESULT STDMETHODCALLTYPE BindAccess(MFINPUTTRUSTAUTHORITY_ACCESS_PARAMS *pParam) = 0;
    virtual HRESULT STDMETHODCALLTYPE UpdateAccess(MFINPUTTRUSTAUTHORITY_ACCESS_PARAMS *pParam) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset(void) = 0;
};

#else

typedef struct IMFInputTrustAuthorityVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFInputTrustAuthority * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFInputTrustAuthority * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFInputTrustAuthority * This);
    HRESULT(STDMETHODCALLTYPE *GetDecrypter)(IMFInputTrustAuthority * This, REFIID riid, void **ppv);
    HRESULT(STDMETHODCALLTYPE *RequestAccess)(IMFInputTrustAuthority * This, MFPOLICYMANAGER_ACTION Action, IMFActivate ** ppContentEnablerActivate);
    HRESULT(STDMETHODCALLTYPE *GetPolicy)(IMFInputTrustAuthority * This, MFPOLICYMANAGER_ACTION Action, IMFOutputPolicy ** ppPolicy);
    HRESULT(STDMETHODCALLTYPE *BindAccess)(IMFInputTrustAuthority * This, MFINPUTTRUSTAUTHORITY_ACCESS_PARAMS * pParam);
    HRESULT(STDMETHODCALLTYPE *UpdateAccess)(IMFInputTrustAuthority * This, MFINPUTTRUSTAUTHORITY_ACCESS_PARAMS * pParam);
    HRESULT(STDMETHODCALLTYPE *Reset)(IMFInputTrustAuthority * This);
    END_INTERFACE
} IMFInputTrustAuthorityVtbl;

interface IMFInputTrustAuthority {
    CONST_VTBL struct IMFInputTrustAuthorityVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFInputTrustAuthority_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFInputTrustAuthority_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFInputTrustAuthority_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFInputTrustAuthority_GetDecrypter(This,riid,ppv)  ((This)->lpVtbl->GetDecrypter(This,riid,ppv))
#define IMFInputTrustAuthority_RequestAccess(This,Action,ppContentEnablerActivate)  ((This)->lpVtbl->RequestAccess(This,Action,ppContentEnablerActivate))
#define IMFInputTrustAuthority_GetPolicy(This,Action,ppPolicy)  ((This)->lpVtbl->GetPolicy(This,Action,ppPolicy))
#define IMFInputTrustAuthority_BindAccess(This,pParam)  ((This)->lpVtbl->BindAccess(This,pParam))
#define IMFInputTrustAuthority_UpdateAccess(This,pParam)  ((This)->lpVtbl->UpdateAccess(This,pParam))
#define IMFInputTrustAuthority_Reset(This)  ((This)->lpVtbl->Reset(This))
#endif


#endif




#endif

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0059_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0059_v0_0_s_ifspec;

#ifndef __IMFTrustedOutput_INTERFACE_DEFINED__
#define __IMFTrustedOutput_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTrustedOutput;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("D19F8E95-B126-4446-890C-5DCB7AD71453")IMFTrustedOutput:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetOutputTrustAuthorityCount(DWORD * pcOutputTrustAuthorities) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOutputTrustAuthorityByIndex(DWORD dwIndex, IMFOutputTrustAuthority **ppauthority) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsFinal(BOOL *pfIsFinal) = 0;
};

#else

typedef struct IMFTrustedOutputVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTrustedOutput * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFTrustedOutput * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFTrustedOutput * This);
    HRESULT(STDMETHODCALLTYPE *GetOutputTrustAuthorityCount)(IMFTrustedOutput * This, DWORD * pcOutputTrustAuthorities);
    HRESULT(STDMETHODCALLTYPE *GetOutputTrustAuthorityByIndex)(IMFTrustedOutput * This, DWORD dwIndex, IMFOutputTrustAuthority ** ppauthority);
    HRESULT(STDMETHODCALLTYPE *IsFinal)(IMFTrustedOutput * This, BOOL * pfIsFinal);
    END_INTERFACE
} IMFTrustedOutputVtbl;

interface IMFTrustedOutput {
    CONST_VTBL struct IMFTrustedOutputVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFTrustedOutput_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFTrustedOutput_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFTrustedOutput_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFTrustedOutput_GetOutputTrustAuthorityCount(This,pcOutputTrustAuthorities)  ((This)->lpVtbl->GetOutputTrustAuthorityCount(This,pcOutputTrustAuthorities))
#define IMFTrustedOutput_GetOutputTrustAuthorityByIndex(This,dwIndex,ppauthority)  ((This)->lpVtbl->GetOutputTrustAuthorityByIndex(This,dwIndex,ppauthority))
#define IMFTrustedOutput_IsFinal(This,pfIsFinal)  ((This)->lpVtbl->IsFinal(This,pfIsFinal))
#endif


#endif




#endif

#ifndef __IMFOutputTrustAuthority_INTERFACE_DEFINED__
#define __IMFOutputTrustAuthority_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFOutputTrustAuthority;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("D19F8E94-B126-4446-890C-5DCB7AD71453")IMFOutputTrustAuthority:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetAction(MFPOLICYMANAGER_ACTION * pAction) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPolicy(IMFOutputPolicy **ppPolicy, DWORD nPolicy, BYTE **ppbTicket, DWORD *pcbTicket) = 0;
};

#else

typedef struct IMFOutputTrustAuthorityVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFOutputTrustAuthority * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFOutputTrustAuthority * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFOutputTrustAuthority * This);
    HRESULT(STDMETHODCALLTYPE *GetAction)(IMFOutputTrustAuthority * This, MFPOLICYMANAGER_ACTION * pAction);
    HRESULT(STDMETHODCALLTYPE *SetPolicy)(IMFOutputTrustAuthority * This, IMFOutputPolicy ** ppPolicy, DWORD nPolicy, BYTE ** ppbTicket, DWORD * pcbTicket);
    END_INTERFACE
} IMFOutputTrustAuthorityVtbl;

interface IMFOutputTrustAuthority {
    CONST_VTBL struct IMFOutputTrustAuthorityVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFOutputTrustAuthority_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFOutputTrustAuthority_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFOutputTrustAuthority_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFOutputTrustAuthority_GetAction(This,pAction)  ((This)->lpVtbl->GetAction(This,pAction))
#define IMFOutputTrustAuthority_SetPolicy(This,ppPolicy,nPolicy,ppbTicket,pcbTicket)  ((This)->lpVtbl->SetPolicy(This,ppPolicy,nPolicy,ppbTicket,pcbTicket))
#endif


#endif




#endif

#ifndef __IMFOutputPolicy_INTERFACE_DEFINED__
#define __IMFOutputPolicy_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFOutputPolicy;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7F00F10A-DAED-41AF-AB26-5FDFA4DFBA3C")IMFOutputPolicy:public IMFAttributes
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GenerateRequiredSchemas(DWORD dwAttributes, GUID guidOutputSubType, GUID * rgGuidProtectionSchemasSupported, DWORD cProtectionSchemasSupported, IMFCollection ** ppRequiredProtectionSchemas) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOriginatorID(GUID *pguidOriginatorID) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMinimumGRLVersion(DWORD *pdwMinimumGRLVersion) = 0;
};

#else

typedef struct IMFOutputPolicyVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFOutputPolicy * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFOutputPolicy * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFOutputPolicy * This);
    HRESULT(STDMETHODCALLTYPE *GetItem)(IMFOutputPolicy * This, REFGUID guidKey, PROPVARIANT * pValue);
    HRESULT(STDMETHODCALLTYPE *GetItemType)(IMFOutputPolicy * This, REFGUID guidKey, MF_ATTRIBUTE_TYPE * pType);
    HRESULT(STDMETHODCALLTYPE *CompareItem)(IMFOutputPolicy * This, REFGUID guidKey, REFPROPVARIANT Value, BOOL * pbResult);
    HRESULT(STDMETHODCALLTYPE *Compare)(IMFOutputPolicy * This, IMFAttributes * pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL * pbResult);
    HRESULT(STDMETHODCALLTYPE *GetUINT32)(IMFOutputPolicy * This, REFGUID guidKey, UINT32 * punValue);
    HRESULT(STDMETHODCALLTYPE *GetUINT64)(IMFOutputPolicy * This, REFGUID guidKey, UINT64 * punValue);
    HRESULT(STDMETHODCALLTYPE *GetDouble)(IMFOutputPolicy * This, REFGUID guidKey, double *pfValue);
    HRESULT(STDMETHODCALLTYPE *GetGUID)(IMFOutputPolicy * This, REFGUID guidKey, GUID * pguidValue);
    HRESULT(STDMETHODCALLTYPE *GetStringLength)(IMFOutputPolicy * This, REFGUID guidKey, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetString)(IMFOutputPolicy * This, REFGUID guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetAllocatedString)(IMFOutputPolicy * This, REFGUID guidKey, LPWSTR * ppwszValue, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetBlobSize)(IMFOutputPolicy * This, REFGUID guidKey, UINT32 * pcbBlobSize);
    HRESULT(STDMETHODCALLTYPE *GetBlob)(IMFOutputPolicy * This, REFGUID guidKey, UINT8 * pBuf, UINT32 cbBufSize, UINT32 * pcbBlobSize);
    HRESULT(STDMETHODCALLTYPE *GetAllocatedBlob)(IMFOutputPolicy * This, REFGUID guidKey, UINT8 ** ppBuf, UINT32 * pcbSize);
    HRESULT(STDMETHODCALLTYPE *GetUnknown)(IMFOutputPolicy * This, REFGUID guidKey, REFIID riid, LPVOID * ppv);
    HRESULT(STDMETHODCALLTYPE *SetItem)(IMFOutputPolicy * This, REFGUID guidKey, REFPROPVARIANT Value);
    HRESULT(STDMETHODCALLTYPE *DeleteItem)(IMFOutputPolicy * This, REFGUID guidKey);
    HRESULT(STDMETHODCALLTYPE *DeleteAllItems)(IMFOutputPolicy * This);
    HRESULT(STDMETHODCALLTYPE *SetUINT32)(IMFOutputPolicy * This, REFGUID guidKey, UINT32 unValue);
    HRESULT(STDMETHODCALLTYPE *SetUINT64)(IMFOutputPolicy * This, REFGUID guidKey, UINT64 unValue);
    HRESULT(STDMETHODCALLTYPE *SetDouble)(IMFOutputPolicy * This, REFGUID guidKey, double fValue);
    HRESULT(STDMETHODCALLTYPE *SetGUID)(IMFOutputPolicy * This, REFGUID guidKey, REFGUID guidValue);
    HRESULT(STDMETHODCALLTYPE *SetString)(IMFOutputPolicy * This, REFGUID guidKey, LPCWSTR wszValue);
    HRESULT(STDMETHODCALLTYPE *SetBlob)(IMFOutputPolicy * This, REFGUID guidKey, const UINT8 * pBuf, UINT32 cbBufSize);
    HRESULT(STDMETHODCALLTYPE *SetUnknown)(IMFOutputPolicy * This, REFGUID guidKey, IUnknown * pUnknown);
    HRESULT(STDMETHODCALLTYPE *LockStore)(IMFOutputPolicy * This);
    HRESULT(STDMETHODCALLTYPE *UnlockStore)(IMFOutputPolicy * This);
    HRESULT(STDMETHODCALLTYPE *GetCount)(IMFOutputPolicy * This, UINT32 * pcItems);
    HRESULT(STDMETHODCALLTYPE *GetItemByIndex)(IMFOutputPolicy * This, UINT32 unIndex, GUID * pguidKey, PROPVARIANT * pValue);
    HRESULT(STDMETHODCALLTYPE *CopyAllItems)(IMFOutputPolicy * This, IMFAttributes * pDest);
    HRESULT(STDMETHODCALLTYPE *GenerateRequiredSchemas)(IMFOutputPolicy * This, DWORD dwAttributes, GUID guidOutputSubType, GUID * rgGuidProtectionSchemasSupported, DWORD cProtectionSchemasSupported, IMFCollection ** ppRequiredProtectionSchemas);
    HRESULT(STDMETHODCALLTYPE *GetOriginatorID)(IMFOutputPolicy * This, GUID * pguidOriginatorID);
    HRESULT(STDMETHODCALLTYPE *GetMinimumGRLVersion)(IMFOutputPolicy * This, DWORD * pdwMinimumGRLVersion);
    END_INTERFACE
} IMFOutputPolicyVtbl;

interface IMFOutputPolicy {
    CONST_VTBL struct IMFOutputPolicyVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFOutputPolicy_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFOutputPolicy_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFOutputPolicy_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFOutputPolicy_GetItem(This,guidKey,pValue)  ((This)->lpVtbl->GetItem(This,guidKey,pValue))
#define IMFOutputPolicy_GetItemType(This,guidKey,pType)  ((This)->lpVtbl->GetItemType(This,guidKey,pType))
#define IMFOutputPolicy_CompareItem(This,guidKey,Value,pbResult)  ((This)->lpVtbl->CompareItem(This,guidKey,Value,pbResult))
#define IMFOutputPolicy_Compare(This,pTheirs,MatchType,pbResult)  ((This)->lpVtbl->Compare(This,pTheirs,MatchType,pbResult))
#define IMFOutputPolicy_GetUINT32(This,guidKey,punValue)  ((This)->lpVtbl->GetUINT32(This,guidKey,punValue))
#define IMFOutputPolicy_GetUINT64(This,guidKey,punValue)  ((This)->lpVtbl->GetUINT64(This,guidKey,punValue))
#define IMFOutputPolicy_GetDouble(This,guidKey,pfValue)  ((This)->lpVtbl->GetDouble(This,guidKey,pfValue))
#define IMFOutputPolicy_GetGUID(This,guidKey,pguidValue)  ((This)->lpVtbl->GetGUID(This,guidKey,pguidValue))
#define IMFOutputPolicy_GetStringLength(This,guidKey,pcchLength)  ((This)->lpVtbl->GetStringLength(This,guidKey,pcchLength))
#define IMFOutputPolicy_GetString(This,guidKey,pwszValue,cchBufSize,pcchLength)  ((This)->lpVtbl->GetString(This,guidKey,pwszValue,cchBufSize,pcchLength))
#define IMFOutputPolicy_GetAllocatedString(This,guidKey,ppwszValue,pcchLength)  ((This)->lpVtbl->GetAllocatedString(This,guidKey,ppwszValue,pcchLength))
#define IMFOutputPolicy_GetBlobSize(This,guidKey,pcbBlobSize)  ((This)->lpVtbl->GetBlobSize(This,guidKey,pcbBlobSize))
#define IMFOutputPolicy_GetBlob(This,guidKey,pBuf,cbBufSize,pcbBlobSize)  ((This)->lpVtbl->GetBlob(This,guidKey,pBuf,cbBufSize,pcbBlobSize))
#define IMFOutputPolicy_GetAllocatedBlob(This,guidKey,ppBuf,pcbSize)  ((This)->lpVtbl->GetAllocatedBlob(This,guidKey,ppBuf,pcbSize))
#define IMFOutputPolicy_GetUnknown(This,guidKey,riid,ppv)  ((This)->lpVtbl->GetUnknown(This,guidKey,riid,ppv))
#define IMFOutputPolicy_SetItem(This,guidKey,Value)  ((This)->lpVtbl->SetItem(This,guidKey,Value))
#define IMFOutputPolicy_DeleteItem(This,guidKey)  ((This)->lpVtbl->DeleteItem(This,guidKey))
#define IMFOutputPolicy_DeleteAllItems(This)  ((This)->lpVtbl->DeleteAllItems(This))
#define IMFOutputPolicy_SetUINT32(This,guidKey,unValue)  ((This)->lpVtbl->SetUINT32(This,guidKey,unValue))
#define IMFOutputPolicy_SetUINT64(This,guidKey,unValue)  ((This)->lpVtbl->SetUINT64(This,guidKey,unValue))
#define IMFOutputPolicy_SetDouble(This,guidKey,fValue)  ((This)->lpVtbl->SetDouble(This,guidKey,fValue))
#define IMFOutputPolicy_SetGUID(This,guidKey,guidValue)  ((This)->lpVtbl->SetGUID(This,guidKey,guidValue))
#define IMFOutputPolicy_SetString(This,guidKey,wszValue)  ((This)->lpVtbl->SetString(This,guidKey,wszValue))
#define IMFOutputPolicy_SetBlob(This,guidKey,pBuf,cbBufSize)  ((This)->lpVtbl->SetBlob(This,guidKey,pBuf,cbBufSize))
#define IMFOutputPolicy_SetUnknown(This,guidKey,pUnknown)  ((This)->lpVtbl->SetUnknown(This,guidKey,pUnknown))
#define IMFOutputPolicy_LockStore(This)  ((This)->lpVtbl->LockStore(This))
#define IMFOutputPolicy_UnlockStore(This)  ((This)->lpVtbl->UnlockStore(This))
#define IMFOutputPolicy_GetCount(This,pcItems)  ((This)->lpVtbl->GetCount(This,pcItems))
#define IMFOutputPolicy_GetItemByIndex(This,unIndex,pguidKey,pValue)  ((This)->lpVtbl->GetItemByIndex(This,unIndex,pguidKey,pValue))
#define IMFOutputPolicy_CopyAllItems(This,pDest)  ((This)->lpVtbl->CopyAllItems(This,pDest))

#define IMFOutputPolicy_GenerateRequiredSchemas(This,dwAttributes,guidOutputSubType,rgGuidProtectionSchemasSupported,cProtectionSchemasSupported,ppRequiredProtectionSchemas)  ((This)->lpVtbl->GenerateRequiredSchemas(This,dwAttributes,guidOutputSubType,r
gGuidProtectionSchemasSupported, cProtectionSchemasSupported, ppRequiredProtectionSchemas))
#define IMFOutputPolicy_GetOriginatorID(This,pguidOriginatorID)  ((This)->lpVtbl->GetOriginatorID(This,pguidOriginatorID))
#define IMFOutputPolicy_GetMinimumGRLVersion(This,pdwMinimumGRLVersion)  ((This)->lpVtbl->GetMinimumGRLVersion(This,pdwMinimumGRLVersion))
#endif
#endif
#endif

#define MFOUTPUTATTRIBUTE_DIGITAL  ((DWORD)0x00000001)
#define MFOUTPUTATTRIBUTE_NONSTANDARDIMPLEMENTATION ((DWORD)0x00000002)
#define MFOUTPUTATTRIBUTE_VIDEO  ((DWORD)0x00000004)
#define MFOUTPUTATTRIBUTE_COMPRESSED  ((DWORD)0x00000008)
#define MFOUTPUTATTRIBUTE_SOFTWARE  ((DWORD)0x00000010)
#define MFOUTPUTATTRIBUTE_BUS  ((DWORD)0x00000020)
#define MFOUTPUTATTRIBUTE_BUSIMPLEMENTATION  ((DWORD)0x0000FF00)

EXTERN_GUID(MFCONNECTOR_SPDIF, 0xb94a712, 0xad3e, 0x4cee, 0x83, 0xce, 0xce, 0x32, 0xe3, 0xdb, 0x65, 0x22);

EXTERN_GUID(MFCONNECTOR_UNKNOWN, 0xac3aef5c, 0xce43, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_PCI, 0xac3aef5d, 0xce43, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_PCIX, 0xac3aef5e, 0xce43, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_PCI_Express, 0xac3aef5f, 0xce43, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_AGP, 0xac3aef60, 0xce43, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);


EXTERN_GUID(MFCONNECTOR_VGA, 0x57cd5968, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_SVIDEO, 0x57cd5969, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_COMPOSITE, 0x57cd596a, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_COMPONENT, 0x57cd596b, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_DVI, 0x57cd596c, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_HDMI, 0x57cd596d, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_LVDS, 0x57cd596e, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_D_JPN, 0x57cd5970, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_SDI, 0x57cd5971, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_DISPLAYPORT_EXTERNAL, 0x57cd5972, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_DISPLAYPORT_EMBEDDED, 0x57cd5973, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_UDI_EXTERNAL, 0x57cd5974, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_UDI_EMBEDDED, 0x57cd5975, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);
EXTERN_GUID(MFCONNECTOR_MIRACAST, 0x57cd5977, 0xce47, 0x11d9, 0x92, 0xdb, 0x00, 0x0b, 0xdb, 0x28, 0xff, 0x98);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0062_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0062_v0_0_s_ifspec;

#ifndef __IMFOutputSchema_INTERFACE_DEFINED__
#define __IMFOutputSchema_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFOutputSchema;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7BE0FC5B-ABD9-44FB-A5C8-F50136E71599")IMFOutputSchema:public IMFAttributes
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetSchemaType(GUID * pguidSchemaType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetConfigurationData(DWORD *pdwVal) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOriginatorID(GUID *pguidOriginatorID) = 0;
};

#else

typedef struct IMFOutputSchemaVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFOutputSchema * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFOutputSchema * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFOutputSchema * This);
    HRESULT(STDMETHODCALLTYPE *GetItem)(IMFOutputSchema * This, REFGUID guidKey, PROPVARIANT * pValue);
    HRESULT(STDMETHODCALLTYPE *GetItemType)(IMFOutputSchema * This, REFGUID guidKey, MF_ATTRIBUTE_TYPE * pType);
    HRESULT(STDMETHODCALLTYPE *CompareItem)(IMFOutputSchema * This, REFGUID guidKey, REFPROPVARIANT Value, BOOL * pbResult);
    HRESULT(STDMETHODCALLTYPE *Compare)(IMFOutputSchema * This, IMFAttributes * pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, BOOL * pbResult);
    HRESULT(STDMETHODCALLTYPE *GetUINT32)(IMFOutputSchema * This, REFGUID guidKey, UINT32 * punValue);
    HRESULT(STDMETHODCALLTYPE *GetUINT64)(IMFOutputSchema * This, REFGUID guidKey, UINT64 * punValue);
    HRESULT(STDMETHODCALLTYPE *GetDouble)(IMFOutputSchema * This, REFGUID guidKey, double *pfValue);
    HRESULT(STDMETHODCALLTYPE *GetGUID)(IMFOutputSchema * This, REFGUID guidKey, GUID * pguidValue);
    HRESULT(STDMETHODCALLTYPE *GetStringLength)(IMFOutputSchema * This, REFGUID guidKey, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetString)(IMFOutputSchema * This, REFGUID guidKey, LPWSTR pwszValue, UINT32 cchBufSize, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetAllocatedString)(IMFOutputSchema * This, REFGUID guidKey, LPWSTR * ppwszValue, UINT32 * pcchLength);
    HRESULT(STDMETHODCALLTYPE *GetBlobSize)(IMFOutputSchema * This, REFGUID guidKey, UINT32 * pcbBlobSize);
    HRESULT(STDMETHODCALLTYPE *GetBlob)(IMFOutputSchema * This, REFGUID guidKey, UINT8 * pBuf, UINT32 cbBufSize, UINT32 * pcbBlobSize);
    HRESULT(STDMETHODCALLTYPE *GetAllocatedBlob)(IMFOutputSchema * This, REFGUID guidKey, UINT8 ** ppBuf, UINT32 * pcbSize);
    HRESULT(STDMETHODCALLTYPE *GetUnknown)(IMFOutputSchema * This, REFGUID guidKey, REFIID riid, LPVOID * ppv);
    HRESULT(STDMETHODCALLTYPE *SetItem)(IMFOutputSchema * This, REFGUID guidKey, REFPROPVARIANT Value);
    HRESULT(STDMETHODCALLTYPE *DeleteItem)(IMFOutputSchema * This, REFGUID guidKey);
    HRESULT(STDMETHODCALLTYPE *DeleteAllItems)(IMFOutputSchema * This);
    HRESULT(STDMETHODCALLTYPE *SetUINT32)(IMFOutputSchema * This, REFGUID guidKey, UINT32 unValue);
    HRESULT(STDMETHODCALLTYPE *SetUINT64)(IMFOutputSchema * This, REFGUID guidKey, UINT64 unValue);
    HRESULT(STDMETHODCALLTYPE *SetDouble)(IMFOutputSchema * This, REFGUID guidKey, double fValue);
    HRESULT(STDMETHODCALLTYPE *SetGUID)(IMFOutputSchema * This, REFGUID guidKey, REFGUID guidValue);
    HRESULT(STDMETHODCALLTYPE *SetString)(IMFOutputSchema * This, REFGUID guidKey, LPCWSTR wszValue);
    HRESULT(STDMETHODCALLTYPE *SetBlob)(IMFOutputSchema * This, REFGUID guidKey, const UINT8 * pBuf, UINT32 cbBufSize);
    HRESULT(STDMETHODCALLTYPE *SetUnknown)(IMFOutputSchema * This, REFGUID guidKey, IUnknown * pUnknown);
    HRESULT(STDMETHODCALLTYPE *LockStore)(IMFOutputSchema * This);
    HRESULT(STDMETHODCALLTYPE *UnlockStore)(IMFOutputSchema * This);
    HRESULT(STDMETHODCALLTYPE *GetCount)(IMFOutputSchema * This, UINT32 * pcItems);
    HRESULT(STDMETHODCALLTYPE *GetItemByIndex)(IMFOutputSchema * This, UINT32 unIndex, GUID * pguidKey, PROPVARIANT * pValue);
    HRESULT(STDMETHODCALLTYPE *CopyAllItems)(IMFOutputSchema * This, IMFAttributes * pDest);
    HRESULT(STDMETHODCALLTYPE *GetSchemaType)(IMFOutputSchema * This, GUID * pguidSchemaType);
    HRESULT(STDMETHODCALLTYPE *GetConfigurationData)(IMFOutputSchema * This, DWORD * pdwVal);
    HRESULT(STDMETHODCALLTYPE *GetOriginatorID)(IMFOutputSchema * This, GUID * pguidOriginatorID);
    END_INTERFACE
} IMFOutputSchemaVtbl;

interface IMFOutputSchema {
    CONST_VTBL struct IMFOutputSchemaVtbl *lpVtbl;
};



#ifdef COBJMACROS


#define IMFOutputSchema_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFOutputSchema_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFOutputSchema_Release(This)  ((This)->lpVtbl->Release(This))

#define IMFOutputSchema_GetItem(This,guidKey,pValue)  ((This)->lpVtbl->GetItem(This,guidKey,pValue))
#define IMFOutputSchema_GetItemType(This,guidKey,pType)  ((This)->lpVtbl->GetItemType(This,guidKey,pType))
#define IMFOutputSchema_CompareItem(This,guidKey,Value,pbResult)  ((This)->lpVtbl->CompareItem(This,guidKey,Value,pbResult))
#define IMFOutputSchema_Compare(This,pTheirs,MatchType,pbResult)  ((This)->lpVtbl->Compare(This,pTheirs,MatchType,pbResult))
#define IMFOutputSchema_GetUINT32(This,guidKey,punValue)  ((This)->lpVtbl->GetUINT32(This,guidKey,punValue))
#define IMFOutputSchema_GetUINT64(This,guidKey,punValue)  ((This)->lpVtbl->GetUINT64(This,guidKey,punValue))
#define IMFOutputSchema_GetDouble(This,guidKey,pfValue)  ((This)->lpVtbl->GetDouble(This,guidKey,pfValue))
#define IMFOutputSchema_GetGUID(This,guidKey,pguidValue)  ((This)->lpVtbl->GetGUID(This,guidKey,pguidValue))
#define IMFOutputSchema_GetStringLength(This,guidKey,pcchLength)  ((This)->lpVtbl->GetStringLength(This,guidKey,pcchLength))
#define IMFOutputSchema_GetString(This,guidKey,pwszValue,cchBufSize,pcchLength)  ((This)->lpVtbl->GetString(This,guidKey,pwszValue,cchBufSize,pcchLength))
#define IMFOutputSchema_GetAllocatedString(This,guidKey,ppwszValue,pcchLength)  ((This)->lpVtbl->GetAllocatedString(This,guidKey,ppwszValue,pcchLength))
#define IMFOutputSchema_GetBlobSize(This,guidKey,pcbBlobSize)  ((This)->lpVtbl->GetBlobSize(This,guidKey,pcbBlobSize))
#define IMFOutputSchema_GetBlob(This,guidKey,pBuf,cbBufSize,pcbBlobSize)  ((This)->lpVtbl->GetBlob(This,guidKey,pBuf,cbBufSize,pcbBlobSize))
#define IMFOutputSchema_GetAllocatedBlob(This,guidKey,ppBuf,pcbSize)  ((This)->lpVtbl->GetAllocatedBlob(This,guidKey,ppBuf,pcbSize))
#define IMFOutputSchema_GetUnknown(This,guidKey,riid,ppv)  ((This)->lpVtbl->GetUnknown(This,guidKey,riid,ppv))
#define IMFOutputSchema_SetItem(This,guidKey,Value)  ((This)->lpVtbl->SetItem(This,guidKey,Value))
#define IMFOutputSchema_DeleteItem(This,guidKey)  ((This)->lpVtbl->DeleteItem(This,guidKey))
#define IMFOutputSchema_DeleteAllItems(This)  ((This)->lpVtbl->DeleteAllItems(This))
#define IMFOutputSchema_SetUINT32(This,guidKey,unValue)  ((This)->lpVtbl->SetUINT32(This,guidKey,unValue))
#define IMFOutputSchema_SetUINT64(This,guidKey,unValue)  ((This)->lpVtbl->SetUINT64(This,guidKey,unValue))
#define IMFOutputSchema_SetDouble(This,guidKey,fValue)  ((This)->lpVtbl->SetDouble(This,guidKey,fValue))
#define IMFOutputSchema_SetGUID(This,guidKey,guidValue)  ((This)->lpVtbl->SetGUID(This,guidKey,guidValue))
#define IMFOutputSchema_SetString(This,guidKey,wszValue)  ((This)->lpVtbl->SetString(This,guidKey,wszValue))
#define IMFOutputSchema_SetBlob(This,guidKey,pBuf,cbBufSize)  ((This)->lpVtbl->SetBlob(This,guidKey,pBuf,cbBufSize))
#define IMFOutputSchema_SetUnknown(This,guidKey,pUnknown)  ((This)->lpVtbl->SetUnknown(This,guidKey,pUnknown))
#define IMFOutputSchema_LockStore(This)  ((This)->lpVtbl->LockStore(This))
#define IMFOutputSchema_UnlockStore(This)  ((This)->lpVtbl->UnlockStore(This))
#define IMFOutputSchema_GetCount(This,pcItems)  ((This)->lpVtbl->GetCount(This,pcItems))
#define IMFOutputSchema_GetItemByIndex(This,unIndex,pguidKey,pValue)  ((This)->lpVtbl->GetItemByIndex(This,unIndex,pguidKey,pValue))
#define IMFOutputSchema_CopyAllItems(This,pDest)  ((This)->lpVtbl->CopyAllItems(This,pDest))

#define IMFOutputSchema_GetSchemaType(This,pguidSchemaType)  ((This)->lpVtbl->GetSchemaType(This,pguidSchemaType))
#define IMFOutputSchema_GetConfigurationData(This,pdwVal)  ((This)->lpVtbl->GetConfigurationData(This,pdwVal))
#define IMFOutputSchema_GetOriginatorID(This,pguidOriginatorID)  ((This)->lpVtbl->GetOriginatorID(This,pguidOriginatorID))
#endif


#endif




#endif

EXTERN_GUID(MFPROTECTION_DISABLE, 0x8cc6d81b, 0xfec6, 0x4d8f, 0x96, 0x4b, 0xcf, 0xba, 0x0b, 0x0d, 0xad, 0x0d);
EXTERN_GUID(MFPROTECTION_CONSTRICTVIDEO, 0x193370ce, 0xc5e4, 0x4c3a, 0x8a, 0x66, 0x69, 0x59, 0xb4, 0xda, 0x44, 0x42);
EXTERN_GUID(MFPROTECTION_CONSTRICTVIDEO_NOOPM, 0xa580e8cd, 0xc247, 0x4957, 0xb9, 0x83, 0x3c, 0x2e, 0xeb, 0xd1, 0xff, 0x59);
EXTERN_GUID(MFPROTECTION_CONSTRICTAUDIO, 0xffc99b44, 0xdf48, 0x4e16, 0x8e, 0x66, 0x09, 0x68, 0x92, 0xc1, 0x57, 0x8a);
EXTERN_GUID(MFPROTECTION_TRUSTEDAUDIODRIVERS, 0x65bdf3d2, 0x0168, 0x4816, 0xa5, 0x33, 0x55, 0xd4, 0x7b, 0x02, 0x71, 0x01);
EXTERN_GUID(MFPROTECTION_HDCP, 0xAE7CC03D, 0xC828, 0x4021, 0xac, 0xb7, 0xd5, 0x78, 0xd2, 0x7a, 0xaf, 0x13);
EXTERN_GUID(MFPROTECTION_CGMSA, 0xE57E69E9, 0x226B, 0x4d31, 0xB4, 0xE3, 0xD3, 0xDB, 0x00, 0x87, 0x36, 0xDD);
EXTERN_GUID(MFPROTECTION_ACP, 0xc3fd11c6, 0xf8b7, 0x4d20, 0xb0, 0x08, 0x1d, 0xb1, 0x7d, 0x61, 0xf2, 0xda);
EXTERN_GUID(MFPROTECTION_WMDRMOTA, 0xa267a6a1, 0x362e, 0x47d0, 0x88, 0x05, 0x46, 0x28, 0x59, 0x8a, 0x23, 0xe4);
EXTERN_GUID(MFPROTECTION_FFT, 0x462a56b2, 0x2866, 0x4bb6, 0x98, 0x0d, 0x6d, 0x8d, 0x9e, 0xdb, 0x1a, 0x8c);
EXTERN_GUID(MFPROTECTION_PROTECTED_SURFACE, 0x4f5d9566, 0xe742, 0x4a25, 0x8d, 0x1f, 0xd2, 0x87, 0xb5, 0xfa, 0x0a, 0xde);
EXTERN_GUID(MFPROTECTION_DISABLE_SCREEN_SCRAPE, 0xa21179a4, 0xb7cd, 0x40d8, 0x96, 0x14, 0x8e, 0xf2, 0x37, 0x1b, 0xa7, 0x8d);

typedef enum _MF_OPM_CGMSA_PROTECTION_LEVEL {
    MF_OPM_CGMSA_OFF = 0,
    MF_OPM_CGMSA_COPY_FREELY = 0x1,
    MF_OPM_CGMSA_COPY_NO_MORE = 0x2,
    MF_OPM_CGMSA_COPY_ONE_GENERATION = 0x3,
    MF_OPM_CGMSA_COPY_NEVER = 0x4,
    MF_OPM_CGMSA_REDISTRIBUTION_CONTROL_REQUIRED = 0x8
} MF_OPM_CGMSA_PROTECTION_LEVEL;

typedef enum _MF_OPM_ACP_PROTECTION_LEVEL {
    MF_OPM_ACP_OFF = 0,
    MF_OPM_ACP_LEVEL_ONE = 1,
    MF_OPM_ACP_LEVEL_TWO = 2,
    MF_OPM_ACP_LEVEL_THREE = 3,
    MF_OPM_ACP_FORCE_ULONG = 0x7fffffff
} MF_OPM_ACP_PROTECTION_LEVEL;

EXTERN_GUID(MFPROTECTION_VIDEO_FRAMES, 0x36a59cbc, 0x7401, 0x4a8c, 0xbc, 0x20, 0x46, 0xa7, 0xc9, 0xe5, 0x97, 0xf0);
EXTERN_GUID(MFPROTECTIONATTRIBUTE_BEST_EFFORT, 0xc8e06331, 0x75f0, 0x4ec1, 0x8e, 0x77, 0x17, 0x57, 0x8f, 0x77, 0x3b, 0x46);

EXTERN_GUID(MFPROTECTIONATTRIBUTE_FAIL_OVER, 0x8536abc5, 0x38f1, 0x4151, 0x9c, 0xce, 0xf5, 0x5d, 0x94, 0x12, 0x29, 0xac);

#if (WINVER >= _WIN32_WINNT_WIN8) 
EXTERN_GUID(MFPROTECTION_GRAPHICS_TRANSFER_AES_ENCRYPTION, 0xc873de64, 0xd8a5, 0x49e6, 0x88, 0xbb, 0xfb, 0x96, 0x3f, 0xd3, 0xd4, 0xce);
#endif /* WINVER >= _WIN32_WINNT_WIN8 */

EXTERN_GUID(MFPROTECTIONATTRIBUTE_CONSTRICTVIDEO_IMAGESIZE, 0x8476fc, 0x4b58, 0x4d80, 0xa7, 0x90, 0xe7, 0x29, 0x76, 0x73, 0x16, 0x1d);
EXTERN_GUID(MFPROTECTIONATTRIBUTE_HDCP_SRM, 0x6f302107, 0x3477, 0x4468, 0x8a, 0x8, 0xee, 0xf9, 0xdb, 0x10, 0xe2, 0xf);

#define MAKE_MFPROTECTIONDATA_DISABLE(Disable)  \
    ((DWORD)(Disable ? 0x00000001 : 0))

#define EXTRACT_MFPROTECTIONDATA_DISABLE_ON(Data) \
    (0 != ((Data) & 0x00000001))

#define EXTRACT_MFPROTECTIONDATA_DISABLE_RESERVED(Data) \
    (((DWORD)((Data) & 0xFFFFFFFE)) >> 1)


#define MAKE_MFPROTECTIONDATA_CONSTRICTAUDIO(Level) \
    ((DWORD)(Level))

#define EXTRACT_MFPROTECTIONDATA_CONSTRICTAUDIO_LEVEL(Data) \
    ((DWORD)((Data) & 0x000000FF))

#define EXTRACT_MFPROTECTIONDATA_CONSTRICTAUDIO_RESERVED(Data) \
    (((DWORD)((Data) & 0xFFFFFF00)) >> 8)

typedef enum _MFAudioConstriction {
    MFaudioConstrictionOff = 0,
    MFaudioConstriction48_16 = (MFaudioConstrictionOff + 1),
    MFaudioConstriction44_16 = (MFaudioConstriction48_16 + 1),
    MFaudioConstriction14_14 = (MFaudioConstriction44_16 + 1),
    MFaudioConstrictionMute = (MFaudioConstriction14_14 + 1)
} MFAudioConstriction;

#define MAKE_MFPROTECTIONDATA_TRUSTEDAUDIODRIVERS(TestCertificateEnable, DigitalOutputDisable, DrmLevel) \
    (((DWORD)((TestCertificateEnable) ? 0x00020000 : 0)) | \
     ((DWORD)((DigitalOutputDisable) ? 0x00010000 : 0)) | \
     ((DWORD)(DrmLevel)))


#if (WINVER >= _WIN32_WINNT_WIN7)
#define MAKE_MFPROTECTIONDATA_TRUSTEDAUDIODRIVERS2(TestCertificateEnable, DigitalOutputDisable, CopyOK, DrmLevel) \
    (((DWORD)((TestCertificateEnable) ? 0x00020000 : 0)) | \
     ((DWORD)((DigitalOutputDisable) ? 0x00010000 : 0)) | \
     ((DWORD)((CopyOK) ? 0x00040000 : 0)) | \
     ((DWORD)(DrmLevel)))
#endif /* WINVER >= _WIN32_WINNT_WIN7 */

#define EXTRACT_MFPROTECTIONDATA_TRUSTEDAUDIODRIVERS_DRMLEVEL(Data) \
    ((DWORD)((Data) & 0x0000FFFF))

#define EXTRACT_MFPROTECTIONDATA_TRUSTEDAUDIODRIVERS_DIGITALOUTPUTDISABLE(Data) \
    (0 != ((Data) & 0x00010000))

#define EXTRACT_MFPROTECTIONDATA_TRUSTEDAUDIODRIVERS_TESTCERTIFICATEENABLE(Data) \
    (0 != ((Data) & 0x00020000))

#if (WINVER >= _WIN32_WINNT_WIN7)
#define EXTRACT_MFPROTECTIONDATA_TRUSTEDAUDIODRIVERS_COPYOK(Data) \
    (0 != ((Data) & 0x00040000))

#define EXTRACT_MFPROTECTIONDATA_TRUSTEDAUDIODRIVERS_RESERVED(Data) \
    (((DWORD)((Data) & 0xFFF80000)) >> 19)

#else
#define EXTRACT_MFPROTECTIONDATA_TRUSTEDAUDIODRIVERS_RESERVED(Data) \
    (((DWORD)((Data) & 0xFFF80000)) >> 18)

#endif


extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0063_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0063_v0_0_s_ifspec;

#ifndef __IMFSecureChannel_INTERFACE_DEFINED__
#define __IMFSecureChannel_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSecureChannel;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("d0ae555d-3b12-4d97-b060-0990bc5aeb67")IMFSecureChannel:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCertificate(BYTE ** ppCert, DWORD * pcbCert) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetupSession(BYTE *pbEncryptedSessionKey, DWORD cbSessionKey) = 0;
};

#else

typedef struct IMFSecureChannelVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSecureChannel * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSecureChannel * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSecureChannel * This);
    HRESULT(STDMETHODCALLTYPE *GetCertificate)(IMFSecureChannel * This, BYTE ** ppCert, DWORD * pcbCert);
    HRESULT(STDMETHODCALLTYPE *SetupSession)(IMFSecureChannel * This, BYTE * pbEncryptedSessionKey, DWORD cbSessionKey);
    END_INTERFACE
} IMFSecureChannelVtbl;

interface IMFSecureChannel {
    CONST_VTBL struct IMFSecureChannelVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSecureChannel_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSecureChannel_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSecureChannel_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFSecureChannel_GetCertificate(This,ppCert,pcbCert)  ((This)->lpVtbl->GetCertificate(This,ppCert,pcbCert))
#define IMFSecureChannel_SetupSession(This,pbEncryptedSessionKey,cbSessionKey)  ((This)->lpVtbl->SetupSession(This,pbEncryptedSessionKey,cbSessionKey))
#endif

#endif

#endif

typedef enum SAMPLE_PROTECTION_VERSION {
    SAMPLE_PROTECTION_VERSION_NO = 0,
    SAMPLE_PROTECTION_VERSION_BASIC_LOKI = 1,
    SAMPLE_PROTECTION_VERSION_SCATTER = 2,
    SAMPLE_PROTECTION_VERSION_RC4 = 3
} SAMPLE_PROTECTION_VERSION;

EXTERN_GUID(MF_SampleProtectionSalt, 0x5403deee, 0xb9ee, 0x438f, 0xaa, 0x83, 0x38, 0x4, 0x99, 0x7e, 0x56, 0x9d);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0064_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0064_v0_0_s_ifspec;

#ifndef __IMFSampleProtection_INTERFACE_DEFINED__
#define __IMFSampleProtection_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSampleProtection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("8e36395f-c7b9-43c4-a54d-512b4af63c95")IMFSampleProtection:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetInputProtectionVersion(DWORD * pdwVersion) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOutputProtectionVersion(DWORD *pdwVersion) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProtectionCertificate(DWORD dwVersion, BYTE **ppCert, DWORD *pcbCert) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitOutputProtection(DWORD dwVersion, DWORD dwOutputId, BYTE *pbCert, DWORD cbCert, BYTE **ppbSeed, DWORD *pcbSeed) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitInputProtection(DWORD dwVersion, DWORD dwInputId, BYTE *pbSeed, DWORD cbSeed) = 0;
};
#else
typedef struct IMFSampleProtectionVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSampleProtection * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSampleProtection * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSampleProtection * This);
    HRESULT(STDMETHODCALLTYPE *GetInputProtectionVersion)(IMFSampleProtection * This, DWORD * pdwVersion);
    HRESULT(STDMETHODCALLTYPE *GetOutputProtectionVersion)(IMFSampleProtection * This, DWORD * pdwVersion);
    HRESULT(STDMETHODCALLTYPE *GetProtectionCertificate)(IMFSampleProtection * This, DWORD dwVersion, BYTE ** ppCert, DWORD * pcbCert);
    HRESULT(STDMETHODCALLTYPE *InitOutputProtection)(IMFSampleProtection * This, DWORD dwVersion, DWORD dwOutputId, BYTE * pbCert, DWORD cbCert, BYTE ** ppbSeed, DWORD * pcbSeed);
    HRESULT(STDMETHODCALLTYPE *InitInputProtection)(IMFSampleProtection * This, DWORD dwVersion, DWORD dwInputId, BYTE * pbSeed, DWORD cbSeed);
    END_INTERFACE
} IMFSampleProtectionVtbl;

interface IMFSampleProtection {
    CONST_VTBL struct IMFSampleProtectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSampleProtection_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSampleProtection_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSampleProtection_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFSampleProtection_GetInputProtectionVersion(This,pdwVersion)  ((This)->lpVtbl->GetInputProtectionVersion(This,pdwVersion))
#define IMFSampleProtection_GetOutputProtectionVersion(This,pdwVersion)  ((This)->lpVtbl->GetOutputProtectionVersion(This,pdwVersion))
#define IMFSampleProtection_GetProtectionCertificate(This,dwVersion,ppCert,pcbCert)  ((This)->lpVtbl->GetProtectionCertificate(This,dwVersion,ppCert,pcbCert))
#define IMFSampleProtection_InitOutputProtection(This,dwVersion,dwOutputId,pbCert,cbCert,ppbSeed,pcbSeed)  ((This)->lpVtbl->InitOutputProtection(This,dwVersion,dwOutputId,pbCert,cbCert,ppbSeed,pcbSeed))
#define IMFSampleProtection_InitInputProtection(This,dwVersion,dwInputId,pbSeed,cbSeed)  ((This)->lpVtbl->InitInputProtection(This,dwVersion,dwInputId,pbSeed,cbSeed))
#endif

#endif

#endif

#ifndef __IMFMediaSinkPreroll_INTERFACE_DEFINED__
#define __IMFMediaSinkPreroll_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFMediaSinkPreroll;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5dfd4b2a-7674-4110-a4e6-8a68fd5f3688")IMFMediaSinkPreroll:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE NotifyPreroll(MFTIME hnsUpcomingStartTime) = 0;
};
#else
typedef struct IMFMediaSinkPrerollVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFMediaSinkPreroll * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFMediaSinkPreroll * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFMediaSinkPreroll * This);
    HRESULT(STDMETHODCALLTYPE *NotifyPreroll)(IMFMediaSinkPreroll * This, MFTIME hnsUpcomingStartTime);
    END_INTERFACE
} IMFMediaSinkPrerollVtbl;

interface IMFMediaSinkPreroll {
    CONST_VTBL struct IMFMediaSinkPrerollVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFMediaSinkPreroll_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFMediaSinkPreroll_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFMediaSinkPreroll_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFMediaSinkPreroll_NotifyPreroll(This,hnsUpcomingStartTime)  ((This)->lpVtbl->NotifyPreroll(This,hnsUpcomingStartTime))
#endif

#endif

#endif

#ifndef __IMFFinalizableMediaSink_INTERFACE_DEFINED__
#define __IMFFinalizableMediaSink_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFFinalizableMediaSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("EAECB74A-9A50-42ce-9541-6A7F57AA4AD7")IMFFinalizableMediaSink:public IMFMediaSink
{
    public:
    virtual HRESULT STDMETHODCALLTYPE BeginFinalize(IMFAsyncCallback * pCallback, IUnknown * punkState) = 0;
    virtual HRESULT STDMETHODCALLTYPE EndFinalize(IMFAsyncResult *pResult) = 0;
};
#else
typedef struct IMFFinalizableMediaSinkVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFFinalizableMediaSink * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFFinalizableMediaSink * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFFinalizableMediaSink * This);
    HRESULT(STDMETHODCALLTYPE *GetCharacteristics)(IMFFinalizableMediaSink * This, DWORD * pdwCharacteristics);
    HRESULT(STDMETHODCALLTYPE *AddStreamSink)(IMFFinalizableMediaSink * This, DWORD dwStreamSinkIdentifier, IMFMediaType * pMediaType, IMFStreamSink ** ppStreamSink);
    HRESULT(STDMETHODCALLTYPE *RemoveStreamSink)(IMFFinalizableMediaSink * This, DWORD dwStreamSinkIdentifier);
    HRESULT(STDMETHODCALLTYPE *GetStreamSinkCount)(IMFFinalizableMediaSink * This, DWORD * pcStreamSinkCount);
    HRESULT(STDMETHODCALLTYPE *GetStreamSinkByIndex)(IMFFinalizableMediaSink * This, DWORD dwIndex, IMFStreamSink ** ppStreamSink);
    HRESULT(STDMETHODCALLTYPE *GetStreamSinkById)(IMFFinalizableMediaSink * This, DWORD dwStreamSinkIdentifier, IMFStreamSink ** ppStreamSink);
    HRESULT(STDMETHODCALLTYPE *SetPresentationClock)(IMFFinalizableMediaSink * This, IMFPresentationClock * pPresentationClock);
    HRESULT(STDMETHODCALLTYPE *GetPresentationClock)(IMFFinalizableMediaSink * This, IMFPresentationClock ** ppPresentationClock);
    HRESULT(STDMETHODCALLTYPE *Shutdown)(IMFFinalizableMediaSink * This);
    HRESULT(STDMETHODCALLTYPE *BeginFinalize)(IMFFinalizableMediaSink * This, IMFAsyncCallback * pCallback, IUnknown * punkState);
    HRESULT(STDMETHODCALLTYPE *EndFinalize)(IMFFinalizableMediaSink * This, IMFAsyncResult * pResult);
    END_INTERFACE
} IMFFinalizableMediaSinkVtbl;

interface IMFFinalizableMediaSink {
    CONST_VTBL struct IMFFinalizableMediaSinkVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFFinalizableMediaSink_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFFinalizableMediaSink_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFFinalizableMediaSink_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFFinalizableMediaSink_GetCharacteristics(This,pdwCharacteristics)  ((This)->lpVtbl->GetCharacteristics(This,pdwCharacteristics))
#define IMFFinalizableMediaSink_AddStreamSink(This,dwStreamSinkIdentifier,pMediaType,ppStreamSink)  ((This)->lpVtbl->AddStreamSink(This,dwStreamSinkIdentifier,pMediaType,ppStreamSink))
#define IMFFinalizableMediaSink_RemoveStreamSink(This,dwStreamSinkIdentifier)  ((This)->lpVtbl->RemoveStreamSink(This,dwStreamSinkIdentifier))
#define IMFFinalizableMediaSink_GetStreamSinkCount(This,pcStreamSinkCount)  ((This)->lpVtbl->GetStreamSinkCount(This,pcStreamSinkCount))
#define IMFFinalizableMediaSink_GetStreamSinkByIndex(This,dwIndex,ppStreamSink)  ((This)->lpVtbl->GetStreamSinkByIndex(This,dwIndex,ppStreamSink))
#define IMFFinalizableMediaSink_GetStreamSinkById(This,dwStreamSinkIdentifier,ppStreamSink)  ((This)->lpVtbl->GetStreamSinkById(This,dwStreamSinkIdentifier,ppStreamSink))
#define IMFFinalizableMediaSink_SetPresentationClock(This,pPresentationClock)  ((This)->lpVtbl->SetPresentationClock(This,pPresentationClock))
#define IMFFinalizableMediaSink_GetPresentationClock(This,ppPresentationClock)  ((This)->lpVtbl->GetPresentationClock(This,ppPresentationClock))
#define IMFFinalizableMediaSink_Shutdown(This)  ((This)->lpVtbl->Shutdown(This))
#define IMFFinalizableMediaSink_BeginFinalize(This,pCallback,punkState)  ((This)->lpVtbl->BeginFinalize(This,pCallback,punkState))
#define IMFFinalizableMediaSink_EndFinalize(This,pResult)  ((This)->lpVtbl->EndFinalize(This,pResult))
#endif

#endif

#endif

#if (WINVER >= _WIN32_WINNT_WIN7)

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0067_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0067_v0_0_s_ifspec;

#ifndef __IMFStreamingSinkConfig_INTERFACE_DEFINED__
#define __IMFStreamingSinkConfig_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFStreamingSinkConfig;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("9db7aa41-3cc5-40d4-8509-555804ad34cc")IMFStreamingSinkConfig:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE StartStreaming(BOOL fSeekOffsetIsByteOffset, QWORD qwSeekOffset) = 0;
};
#else
typedef struct IMFStreamingSinkConfigVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFStreamingSinkConfig * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFStreamingSinkConfig * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFStreamingSinkConfig * This);
    HRESULT(STDMETHODCALLTYPE *StartStreaming)(IMFStreamingSinkConfig * This, BOOL fSeekOffsetIsByteOffset, QWORD qwSeekOffset);
    END_INTERFACE
} IMFStreamingSinkConfigVtbl;

interface IMFStreamingSinkConfig {
    CONST_VTBL struct IMFStreamingSinkConfigVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFStreamingSinkConfig_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFStreamingSinkConfig_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFStreamingSinkConfig_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFStreamingSinkConfig_StartStreaming(This,fSeekOffsetIsByteOffset,qwSeekOffset)  ((This)->lpVtbl->StartStreaming(This,fSeekOffsetIsByteOffset,qwSeekOffset))
#endif

#endif

#endif

#endif

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0068_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0068_v0_0_s_ifspec;

#ifndef __IMFRemoteProxy_INTERFACE_DEFINED__
#define __IMFRemoteProxy_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFRemoteProxy;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("994e23ad-1cc2-493c-b9fa-46f1cb040fa4")IMFRemoteProxy:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetRemoteObject(REFIID riid, void **ppv) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRemoteHost(REFIID riid, void **ppv) = 0;
};
#else
typedef struct IMFRemoteProxyVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFRemoteProxy * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFRemoteProxy * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFRemoteProxy * This);
    HRESULT(STDMETHODCALLTYPE *GetRemoteObject)(IMFRemoteProxy * This, REFIID riid, void **ppv);
    HRESULT(STDMETHODCALLTYPE *GetRemoteHost)(IMFRemoteProxy * This, REFIID riid, void **ppv);
    END_INTERFACE
} IMFRemoteProxyVtbl;

interface IMFRemoteProxy {
    CONST_VTBL struct IMFRemoteProxyVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFRemoteProxy_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFRemoteProxy_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFRemoteProxy_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFRemoteProxy_GetRemoteObject(This,riid,ppv)  ((This)->lpVtbl->GetRemoteObject(This,riid,ppv))
#define IMFRemoteProxy_GetRemoteHost(This,riid,ppv)  ((This)->lpVtbl->GetRemoteHost(This,riid,ppv))
#endif

#endif

#endif

EXTERN_GUID(MF_REMOTE_PROXY, 0x2f00c90e, 0xd2cf, 0x4278, 0x8b, 0x6a, 0xd0, 0x77, 0xfa, 0xc3, 0xa2, 0x5f);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0069_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0069_v0_0_s_ifspec;

#ifndef __IMFObjectReferenceStream_INTERFACE_DEFINED__
#define __IMFObjectReferenceStream_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFObjectReferenceStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("09EF5BE3-C8A7-469e-8B70-73BF25BB193F")IMFObjectReferenceStream:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SaveReference(REFIID riid, IUnknown * pUnk) = 0;
    virtual HRESULT STDMETHODCALLTYPE LoadReference(REFIID riid, void **ppv) = 0;
};
#else
typedef struct IMFObjectReferenceStreamVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFObjectReferenceStream * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFObjectReferenceStream * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFObjectReferenceStream * This);
    HRESULT(STDMETHODCALLTYPE *SaveReference)(IMFObjectReferenceStream * This, REFIID riid, IUnknown * pUnk);
    HRESULT(STDMETHODCALLTYPE *LoadReference)(IMFObjectReferenceStream * This, REFIID riid, void **ppv);
    END_INTERFACE
} IMFObjectReferenceStreamVtbl;

interface IMFObjectReferenceStream {
    CONST_VTBL struct IMFObjectReferenceStreamVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFObjectReferenceStream_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFObjectReferenceStream_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFObjectReferenceStream_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFObjectReferenceStream_SaveReference(This,riid,pUnk)  ((This)->lpVtbl->SaveReference(This,riid,pUnk))
#define IMFObjectReferenceStream_LoadReference(This,riid,ppv)  ((This)->lpVtbl->LoadReference(This,riid,ppv))
#endif

#endif

#endif

EXTERN_GUID(CLSID_CreateMediaExtensionObject, 0xef65a54d, 0x0788, 0x45b8, 0x8b, 0x14, 0xbc, 0x0f, 0x6a, 0x6b, 0x51, 0x37);

#ifndef __IMFPMPHost_INTERFACE_DEFINED__
#define __IMFPMPHost_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFPMPHost;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("F70CA1A9-FDC7-4782-B994-ADFFB1C98606")IMFPMPHost:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE LockProcess(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnlockProcess(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateObjectByCLSID(REFCLSID clsid, IStream *pStream, REFIID riid, void **ppv) = 0;
};
#else
typedef struct IMFPMPHostVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFPMPHost * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFPMPHost * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFPMPHost * This);
    HRESULT(STDMETHODCALLTYPE *LockProcess)(IMFPMPHost * This);
    HRESULT(STDMETHODCALLTYPE *UnlockProcess)(IMFPMPHost * This);
    HRESULT(STDMETHODCALLTYPE *CreateObjectByCLSID)(IMFPMPHost * This, REFCLSID clsid, IStream * pStream, REFIID riid, void **ppv);
    END_INTERFACE
} IMFPMPHostVtbl;

interface IMFPMPHost {
    CONST_VTBL struct IMFPMPHostVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFPMPHost_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFPMPHost_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFPMPHost_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFPMPHost_LockProcess(This)  ((This)->lpVtbl->LockProcess(This))
#define IMFPMPHost_UnlockProcess(This)  ((This)->lpVtbl->UnlockProcess(This))
#define IMFPMPHost_CreateObjectByCLSID(This,clsid,pStream,riid,ppv)  ((This)->lpVtbl->CreateObjectByCLSID(This,clsid,pStream,riid,ppv))
#endif

#endif

HRESULT STDMETHODCALLTYPE IMFPMPHost_RemoteCreateObjectByCLSID_Proxy(IMFPMPHost *This, REFCLSID clsid, BYTE *pbData, DWORD cbData, REFIID riid, void **ppv);
void __RPC_STUB IMFPMPHost_RemoteCreateObjectByCLSID_Stub(IRpcStubBuffer *This, IRpcChannelBuffer *_pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD *_pdwStubPhase);

#endif

#if (WINVER >= _WIN32_WINNT_WIN7)
EXTERN_C const GUID MF_PMP_SERVICE;
#endif

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0071_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0071_v0_0_s_ifspec;

#ifndef __IMFPMPClient_INTERFACE_DEFINED__
#define __IMFPMPClient_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFPMPClient;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("6C4E655D-EAD8-4421-B6B9-54DCDBBDF820")IMFPMPClient:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetPMPHost(IMFPMPHost * pPMPHost) = 0;
};
#else
typedef struct IMFPMPClientVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFPMPClient * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFPMPClient * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFPMPClient * This);
    HRESULT(STDMETHODCALLTYPE *SetPMPHost)(IMFPMPClient * This, IMFPMPHost * pPMPHost);
    END_INTERFACE
} IMFPMPClientVtbl;

interface IMFPMPClient {
    CONST_VTBL struct IMFPMPClientVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFPMPClient_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFPMPClient_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFPMPClient_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFPMPClient_SetPMPHost(This,pPMPHost)  ((This)->lpVtbl->SetPMPHost(This,pPMPHost))
#endif

#endif

#endif

#ifndef __IMFPMPServer_INTERFACE_DEFINED__
#define __IMFPMPServer_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFPMPServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("994e23af-1cc2-493c-b9fa-46f1cb040fa4")IMFPMPServer:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE LockProcess(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnlockProcess(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateObjectByCLSID(REFCLSID clsid, REFIID riid, void **ppObject) = 0;
};
#else
typedef struct IMFPMPServerVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFPMPServer * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFPMPServer * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFPMPServer * This);
    HRESULT(STDMETHODCALLTYPE *LockProcess)(IMFPMPServer * This);
    HRESULT(STDMETHODCALLTYPE *UnlockProcess)(IMFPMPServer * This);
    HRESULT(STDMETHODCALLTYPE *CreateObjectByCLSID)(IMFPMPServer * This, REFCLSID clsid, REFIID riid, void **ppObject);
    END_INTERFACE
} IMFPMPServerVtbl;

interface IMFPMPServer {
    CONST_VTBL struct IMFPMPServerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFPMPServer_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFPMPServer_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFPMPServer_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFPMPServer_LockProcess(This)  ((This)->lpVtbl->LockProcess(This))
#define IMFPMPServer_UnlockProcess(This)  ((This)->lpVtbl->UnlockProcess(This))
#define IMFPMPServer_CreateObjectByCLSID(This,clsid,riid,ppObject)  ((This)->lpVtbl->CreateObjectByCLSID(This,clsid,riid,ppObject))
#endif

#endif

#endif

STDAPI MFCreatePMPServer(DWORD dwCreationFlags, IMFPMPServer **ppPMPServer);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0073_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0073_v0_0_s_ifspec;

#ifndef __IMFRemoteDesktopPlugin_INTERFACE_DEFINED__
#define __IMFRemoteDesktopPlugin_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFRemoteDesktopPlugin;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("1cde6309-cae0-4940-907e-c1ec9c3d1d4a")IMFRemoteDesktopPlugin:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE UpdateTopology(IMFTopology * pTopology) = 0;
};
#else
typedef struct IMFRemoteDesktopPluginVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFRemoteDesktopPlugin * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFRemoteDesktopPlugin * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFRemoteDesktopPlugin * This);
    HRESULT(STDMETHODCALLTYPE *UpdateTopology)(IMFRemoteDesktopPlugin * This, IMFTopology * pTopology);
    END_INTERFACE
} IMFRemoteDesktopPluginVtbl;

interface IMFRemoteDesktopPlugin {
    CONST_VTBL struct IMFRemoteDesktopPluginVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFRemoteDesktopPlugin_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFRemoteDesktopPlugin_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFRemoteDesktopPlugin_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFRemoteDesktopPlugin_UpdateTopology(This,pTopology)  ((This)->lpVtbl->UpdateTopology(This,pTopology))
#endif

#endif

#endif

STDAPI MFCreateRemoteDesktopPlugin(IMFRemoteDesktopPlugin **ppPlugin);
EXTERN_C HRESULT STDAPICALLTYPE CreateNamedPropertyStore(INamedPropertyStore **ppStore);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0074_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0074_v0_0_s_ifspec;

#ifndef __IMFSAMIStyle_INTERFACE_DEFINED__
#define __IMFSAMIStyle_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSAMIStyle;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A7E025DD-5303-4a62-89D6-E747E1EFAC73")IMFSAMIStyle:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetStyleCount(DWORD * pdwCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStyles(PROPVARIANT *pPropVarStyleArray) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSelectedStyle(LPCWSTR pwszStyle) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSelectedStyle(LPWSTR *ppwszStyle) = 0;
};
#else
typedef struct IMFSAMIStyleVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSAMIStyle * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFSAMIStyle * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFSAMIStyle * This);
    HRESULT(STDMETHODCALLTYPE *GetStyleCount)(IMFSAMIStyle * This, DWORD * pdwCount);
    HRESULT(STDMETHODCALLTYPE *GetStyles)(IMFSAMIStyle * This, PROPVARIANT * pPropVarStyleArray);
    HRESULT(STDMETHODCALLTYPE *SetSelectedStyle)(IMFSAMIStyle * This, LPCWSTR pwszStyle);
    HRESULT(STDMETHODCALLTYPE *GetSelectedStyle)(IMFSAMIStyle * This, LPWSTR * ppwszStyle);
    END_INTERFACE
} IMFSAMIStyleVtbl;

interface IMFSAMIStyle {
    CONST_VTBL struct IMFSAMIStyleVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSAMIStyle_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFSAMIStyle_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFSAMIStyle_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFSAMIStyle_GetStyleCount(This,pdwCount)  ((This)->lpVtbl->GetStyleCount(This,pdwCount))
#define IMFSAMIStyle_GetStyles(This,pPropVarStyleArray)  ((This)->lpVtbl->GetStyles(This,pPropVarStyleArray))
#define IMFSAMIStyle_SetSelectedStyle(This,pwszStyle)  ((This)->lpVtbl->SetSelectedStyle(This,pwszStyle))
#define IMFSAMIStyle_GetSelectedStyle(This,ppwszStyle)  ((This)->lpVtbl->GetSelectedStyle(This,ppwszStyle))
#endif

#endif

#endif

EXTERN_GUID(MF_SAMI_SERVICE, 0x49a89ae7, 0xb4d9, 0x4ef2, 0xaa, 0x5c, 0xf6, 0x5a, 0x3e, 0x5, 0xae, 0x4e);
EXTERN_GUID(MF_PD_SAMI_STYLELIST, 0xe0b73c7f, 0x486d, 0x484e, 0x98, 0x72, 0x4d, 0xe5, 0x19, 0x2a, 0x7b, 0xf8);
EXTERN_GUID(MF_SD_SAMI_LANGUAGE, 0x36fcb98a, 0x6cd0, 0x44cb, 0xac, 0xb9, 0xa8, 0xf5, 0x60, 0xd, 0xd0, 0xbb);
#if (WINVER >= _WIN32_WINNT_WIN7)
STDAPI MFCreateSampleCopierMFT(IMFTransform **ppCopierMFT);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0075_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0075_v0_0_s_ifspec;

#ifndef __IMFTranscodeProfile_INTERFACE_DEFINED__
#define __IMFTranscodeProfile_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTranscodeProfile;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("4ADFDBA3-7AB0-4953-A62B-461E7FF3DA1E")IMFTranscodeProfile:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetAudioAttributes(IMFAttributes * pAttrs) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAudioAttributes(IMFAttributes **ppAttrs) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetVideoAttributes(IMFAttributes *pAttrs) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVideoAttributes(IMFAttributes **ppAttrs) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetContainerAttributes(IMFAttributes *pAttrs) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetContainerAttributes(IMFAttributes **ppAttrs) = 0;
};
#else
typedef struct IMFTranscodeProfileVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTranscodeProfile * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFTranscodeProfile * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFTranscodeProfile * This);
    HRESULT(STDMETHODCALLTYPE *SetAudioAttributes)(IMFTranscodeProfile * This, IMFAttributes * pAttrs);
    HRESULT(STDMETHODCALLTYPE *GetAudioAttributes)(IMFTranscodeProfile * This, IMFAttributes ** ppAttrs);
    HRESULT(STDMETHODCALLTYPE *SetVideoAttributes)(IMFTranscodeProfile * This, IMFAttributes * pAttrs);
    HRESULT(STDMETHODCALLTYPE *GetVideoAttributes)(IMFTranscodeProfile * This, IMFAttributes ** ppAttrs);
    HRESULT(STDMETHODCALLTYPE *SetContainerAttributes)(IMFTranscodeProfile * This, IMFAttributes * pAttrs);
    HRESULT(STDMETHODCALLTYPE *GetContainerAttributes)(IMFTranscodeProfile * This, IMFAttributes ** ppAttrs);
    END_INTERFACE
} IMFTranscodeProfileVtbl;

interface IMFTranscodeProfile {
    CONST_VTBL struct IMFTranscodeProfileVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFTranscodeProfile_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFTranscodeProfile_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFTranscodeProfile_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFTranscodeProfile_SetAudioAttributes(This,pAttrs)  ((This)->lpVtbl->SetAudioAttributes(This,pAttrs))
#define IMFTranscodeProfile_GetAudioAttributes(This,ppAttrs)  ((This)->lpVtbl->GetAudioAttributes(This,ppAttrs))
#define IMFTranscodeProfile_SetVideoAttributes(This,pAttrs)  ((This)->lpVtbl->SetVideoAttributes(This,pAttrs))
#define IMFTranscodeProfile_GetVideoAttributes(This,ppAttrs)  ((This)->lpVtbl->GetVideoAttributes(This,ppAttrs))
#define IMFTranscodeProfile_SetContainerAttributes(This,pAttrs)  ((This)->lpVtbl->SetContainerAttributes(This,pAttrs))
#define IMFTranscodeProfile_GetContainerAttributes(This,ppAttrs)  ((This)->lpVtbl->GetContainerAttributes(This,ppAttrs))
#endif

#endif

#endif

EXTERN_GUID(MF_TRANSCODE_CONTAINERTYPE, 0x150ff23f, 0x4abc, 0x478b, 0xac, 0x4f, 0xe1, 0x91, 0x6f, 0xba, 0x1c, 0xca);
EXTERN_GUID(MFTranscodeContainerType_ASF, 0x430f6f6e, 0xb6bf, 0x4fc1, 0xa0, 0xbd, 0x9e, 0xe4, 0x6e, 0xee, 0x2a, 0xfb);
EXTERN_GUID(MFTranscodeContainerType_MPEG4, 0xdc6cd05d, 0xb9d0, 0x40ef, 0xbd, 0x35, 0xfa, 0x62, 0x2c, 0x1a, 0xb2, 0x8a);
EXTERN_GUID(MFTranscodeContainerType_MP3, 0xe438b912, 0x83f1, 0x4de6, 0x9e, 0x3a, 0x9f, 0xfb, 0xc6, 0xdd, 0x24, 0xd1);
EXTERN_GUID(MFTranscodeContainerType_3GP, 0x34c50167, 0x4472, 0x4f34, 0x9e, 0xa0, 0xc4, 0x9f, 0xba, 0xcf, 0x03, 0x7d);
EXTERN_GUID(MF_TRANSCODE_SKIP_METADATA_TRANSFER, 0x4e4469ef, 0xb571, 0x4959, 0x8f, 0x83, 0x3d, 0xcf, 0xba, 0x33, 0xa3, 0x93);
EXTERN_GUID(MF_TRANSCODE_TOPOLOGYMODE, 0x3e3df610, 0x394a, 0x40b2, 0x9d, 0xea, 0x3b, 0xab, 0x65, 0xb, 0xeb, 0xf2);
typedef
    enum _MF_TRANSCODE_TOPOLOGYMODE_FLAGS { MF_TRANSCODE_TOPOLOGYMODE_SOFTWARE_ONLY = 0,
    MF_TRANSCODE_TOPOLOGYMODE_HARDWARE_ALLOWED = 1
} MF_TRANSCODE_TOPOLOGYMODE_FLAGS;

EXTERN_GUID(MF_TRANSCODE_ADJUST_PROFILE, 0x9c37c21b, 0x60f, 0x487c, 0xa6, 0x90, 0x80, 0xd7, 0xf5, 0xd, 0x1c, 0x72);
typedef
    enum _MF_TRANSCODE_ADJUST_PROFILE_FLAGS { MF_TRANSCODE_ADJUST_PROFILE_DEFAULT = 0,
    MF_TRANSCODE_ADJUST_PROFILE_USE_SOURCE_ATTRIBUTES = 1
} MF_TRANSCODE_ADJUST_PROFILE_FLAGS;

EXTERN_GUID(MF_TRANSCODE_ENCODINGPROFILE, 0x6947787c, 0xf508, 0x4ea9, 0xb1, 0xe9, 0xa1, 0xfe, 0x3a, 0x49, 0xfb, 0xc9);
EXTERN_GUID(MF_TRANSCODE_QUALITYVSSPEED, 0x98332df8, 0x03cd, 0x476b, 0x89, 0xfa, 0x3f, 0x9e, 0x44, 0x2d, 0xec, 0x9f);
EXTERN_GUID(MF_TRANSCODE_DONOT_INSERT_ENCODER, 0xf45aa7ce, 0xab24, 0x4012, 0xa1, 0x1b, 0xdc, 0x82, 0x20, 0x20, 0x14, 0x10);
STDAPI MFCreateTranscodeProfile(IMFTranscodeProfile **ppTranscodeProfile);
STDAPI MFCreateTranscodeTopology(IMFMediaSource *pSrc, LPCWSTR pwszOutputFilePath, IMFTranscodeProfile *pProfile, IMFTopology **ppTranscodeTopo);
STDAPI MFTranscodeGetAudioOutputAvailableTypes(REFGUID guidSubType, DWORD dwMFTFlags, IMFAttributes *pCodecConfig, IMFCollection **ppAvailableTypes);
typedef struct _MF_TRANSCODE_SINK_INFO {
    DWORD dwVideoStreamID;
    IMFMediaType *pVideoMediaType;
    DWORD dwAudioStreamID;
    IMFMediaType *pAudioMediaType;
} MF_TRANSCODE_SINK_INFO;

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0076_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0076_v0_0_s_ifspec;

#ifndef __IMFTranscodeSinkInfoProvider_INTERFACE_DEFINED__
#define __IMFTranscodeSinkInfoProvider_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTranscodeSinkInfoProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("8CFFCD2E-5A03-4a3a-AFF7-EDCD107C620E")IMFTranscodeSinkInfoProvider:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetOutputFile(LPCWSTR pwszFileName) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOutputByteStream(IMFActivate *pByteStreamActivate) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProfile(IMFTranscodeProfile *pProfile) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSinkInfo(MF_TRANSCODE_SINK_INFO *pSinkInfo) = 0;
};
#else
typedef struct IMFTranscodeSinkInfoProviderVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTranscodeSinkInfoProvider * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMFTranscodeSinkInfoProvider * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFTranscodeSinkInfoProvider * This);
    HRESULT(STDMETHODCALLTYPE *SetOutputFile)(IMFTranscodeSinkInfoProvider * This, LPCWSTR pwszFileName);
    HRESULT(STDMETHODCALLTYPE *SetOutputByteStream)(IMFTranscodeSinkInfoProvider * This, IMFActivate * pByteStreamActivate);
    HRESULT(STDMETHODCALLTYPE *SetProfile)(IMFTranscodeSinkInfoProvider * This, IMFTranscodeProfile * pProfile);
    HRESULT(STDMETHODCALLTYPE *GetSinkInfo)(IMFTranscodeSinkInfoProvider * This, MF_TRANSCODE_SINK_INFO * pSinkInfo);
    END_INTERFACE
} IMFTranscodeSinkInfoProviderVtbl;

interface IMFTranscodeSinkInfoProvider {
    CONST_VTBL struct IMFTranscodeSinkInfoProviderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFTranscodeSinkInfoProvider_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFTranscodeSinkInfoProvider_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFTranscodeSinkInfoProvider_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFTranscodeSinkInfoProvider_SetOutputFile(This,pwszFileName)  ((This)->lpVtbl->SetOutputFile(This,pwszFileName))
#define IMFTranscodeSinkInfoProvider_SetOutputByteStream(This,pByteStreamActivate)  ((This)->lpVtbl->SetOutputByteStream(This,pByteStreamActivate))
#define IMFTranscodeSinkInfoProvider_SetProfile(This,pProfile)  ((This)->lpVtbl->SetProfile(This,pProfile))
#define IMFTranscodeSinkInfoProvider_GetSinkInfo(This,pSinkInfo)  ((This)->lpVtbl->GetSinkInfo(This,pSinkInfo))
#endif

#endif

#endif

STDAPI MFCreateTranscodeSinkActivate(IMFActivate **ppActivate);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0077_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0077_v0_0_s_ifspec;

#ifndef __IMFFieldOfUseMFTUnlock_INTERFACE_DEFINED__
#define __IMFFieldOfUseMFTUnlock_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFFieldOfUseMFTUnlock;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("508E71D3-EC66-4fc3-8775-B4B9ED6BA847")IMFFieldOfUseMFTUnlock:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Unlock(IUnknown * pUnkMFT) = 0;
};
#else
typedef struct IMFFieldOfUseMFTUnlockVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFFieldOfUseMFTUnlock * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFFieldOfUseMFTUnlock * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFFieldOfUseMFTUnlock * This);
    HRESULT(STDMETHODCALLTYPE *Unlock)(IMFFieldOfUseMFTUnlock * This, IUnknown * pUnkMFT);
    END_INTERFACE
} IMFFieldOfUseMFTUnlockVtbl;

interface IMFFieldOfUseMFTUnlock {
    CONST_VTBL struct IMFFieldOfUseMFTUnlockVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFFieldOfUseMFTUnlock_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFFieldOfUseMFTUnlock_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFFieldOfUseMFTUnlock_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFFieldOfUseMFTUnlock_Unlock(This,pUnkMFT)  ((This)->lpVtbl->Unlock(This,pUnkMFT))
#endif

#endif

#endif

typedef struct _MFT_REGISTRATION_INFO {
    CLSID clsid;
    GUID guidCategory;
    UINT32 uiFlags;
    LPCWSTR pszName;
    DWORD cInTypes;
    MFT_REGISTER_TYPE_INFO *pInTypes;
    DWORD cOutTypes;
    MFT_REGISTER_TYPE_INFO *pOutTypes;
} MFT_REGISTRATION_INFO;

EXTERN_GUID(MF_LOCAL_MFT_REGISTRATION_SERVICE, 0xddf5cf9c, 0x4506, 0x45aa, 0xab, 0xf0, 0x6d, 0x5d, 0x94, 0xdd, 0x1b, 0x4a);

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0078_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0078_v0_0_s_ifspec;

#ifndef __IMFLocalMFTRegistration_INTERFACE_DEFINED__
#define __IMFLocalMFTRegistration_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFLocalMFTRegistration;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("149c4d73-b4be-4f8d-8b87-079e926b6add")IMFLocalMFTRegistration:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE RegisterMFTs(MFT_REGISTRATION_INFO * pMFTs, DWORD cMFTs) = 0;
};
#else
typedef struct IMFLocalMFTRegistrationVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFLocalMFTRegistration * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMFLocalMFTRegistration * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMFLocalMFTRegistration * This);
    HRESULT(STDMETHODCALLTYPE *RegisterMFTs)(IMFLocalMFTRegistration * This, MFT_REGISTRATION_INFO * pMFTs, DWORD cMFTs);
    END_INTERFACE
} IMFLocalMFTRegistrationVtbl;

interface IMFLocalMFTRegistration {
    CONST_VTBL struct IMFLocalMFTRegistrationVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFLocalMFTRegistration_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMFLocalMFTRegistration_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMFLocalMFTRegistration_Release(This)  ((This)->lpVtbl->Release(This))
#define IMFLocalMFTRegistration_RegisterMFTs(This,pMFTs,cMFTs)  ((This)->lpVtbl->RegisterMFTs(This,pMFTs,cMFTs))
#endif

#endif

#endif /* __IMFLocalMFTRegistration_INTERFACE_DEFINED__ */

#if (WINVER >= _WIN32_WINNT_WIN8) 

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0089_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0089_v0_0_s_ifspec;

#ifndef __IMFPMPHostApp_INTERFACE_DEFINED__
#define __IMFPMPHostApp_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFPMPHostApp;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("84d2054a-3aa1-4728-a3b0-440a418cf49c")IMFPMPHostApp:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE LockProcess(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE UnlockProcess(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE ActivateClassById(LPCWSTR id, IStream *pStream, REFIID riid, void **ppv) = 0;
};
#else /* C style interface */
typedef struct IMFPMPHostAppVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFPMPHostApp * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IMFPMPHostApp * This);
    ULONG(STDMETHODCALLTYPE * Release) (IMFPMPHostApp * This);
    HRESULT(STDMETHODCALLTYPE * LockProcess) (IMFPMPHostApp * This);
    HRESULT(STDMETHODCALLTYPE * UnlockProcess) (IMFPMPHostApp * This);
    HRESULT(STDMETHODCALLTYPE * ActivateClassById) (IMFPMPHostApp * This, LPCWSTR id, IStream * pStream, REFIID riid, void **ppv);
    END_INTERFACE
} IMFPMPHostAppVtbl;

interface IMFPMPHostApp {
    CONST_VTBL struct IMFPMPHostAppVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFPMPHostApp_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFPMPHostApp_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFPMPHostApp_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFPMPHostApp_LockProcess(This)  ((This)->lpVtbl->LockProcess(This)) 
#define IMFPMPHostApp_UnlockProcess(This)  ((This)->lpVtbl->UnlockProcess(This)) 
#define IMFPMPHostApp_ActivateClassById(This,id,pStream,riid,ppv)  ((This)->lpVtbl->ActivateClassById(This,id,pStream,riid,ppv)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFPMPHostApp_INTERFACE_DEFINED__ */

#ifndef __IMFPMPClientApp_INTERFACE_DEFINED__
#define __IMFPMPClientApp_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFPMPClientApp;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("c004f646-be2c-48f3-93a2-a0983eba1108")IMFPMPClientApp:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetPMPHost(IMFPMPHostApp * pPMPHost) = 0;

};
#else /* C style interface */
typedef struct IMFPMPClientAppVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFPMPClientApp * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IMFPMPClientApp * This);
    ULONG(STDMETHODCALLTYPE * Release) (IMFPMPClientApp * This);
    HRESULT(STDMETHODCALLTYPE * SetPMPHost) (IMFPMPClientApp * This, IMFPMPHostApp * pPMPHost);
    END_INTERFACE
} IMFPMPClientAppVtbl;

interface IMFPMPClientApp {
    CONST_VTBL struct IMFPMPClientAppVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFPMPClientApp_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFPMPClientApp_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFPMPClientApp_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFPMPClientApp_SetPMPHost(This,pPMPHost)  ((This)->lpVtbl->SetPMPHost(This,pPMPHost)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFPMPClientApp_INTERFACE_DEFINED__ */

#endif 

#if (WINVER >= _WIN32_WINNT_WINBLUE) 

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0091_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0091_v0_0_s_ifspec;

#ifndef __IMFMediaStreamSourceSampleRequest_INTERFACE_DEFINED__
#define __IMFMediaStreamSourceSampleRequest_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFMediaStreamSourceSampleRequest;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("380b9af9-a85b-4e78-a2af-ea5ce645c6b4")IMFMediaStreamSourceSampleRequest:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetSample(IMFSample * value) = 0;

};
#else /* C style interface */
typedef struct IMFMediaStreamSourceSampleRequestVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFMediaStreamSourceSampleRequest * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IMFMediaStreamSourceSampleRequest * This);
    ULONG(STDMETHODCALLTYPE * Release) (IMFMediaStreamSourceSampleRequest * This);
    HRESULT(STDMETHODCALLTYPE * SetSample) (IMFMediaStreamSourceSampleRequest * This, IMFSample * value);
    END_INTERFACE
} IMFMediaStreamSourceSampleRequestVtbl;

interface IMFMediaStreamSourceSampleRequest {
    CONST_VTBL struct IMFMediaStreamSourceSampleRequestVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFMediaStreamSourceSampleRequest_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFMediaStreamSourceSampleRequest_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFMediaStreamSourceSampleRequest_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFMediaStreamSourceSampleRequest_SetSample(This,value)  ((This)->lpVtbl->SetSample(This,value)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFMediaStreamSourceSampleRequest_INTERFACE_DEFINED__ */

#endif 

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0092_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0092_v0_0_s_ifspec;

#ifndef __IMFTrackedSample_INTERFACE_DEFINED__
#define __IMFTrackedSample_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFTrackedSample;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("245BF8E9-0755-40f7-88A5-AE0F18D55E17")IMFTrackedSample:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE SetAllocator(IMFAsyncCallback * pSampleAllocator, IUnknown * pUnkState) = 0;
};
#else /* C style interface */
typedef struct IMFTrackedSampleVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFTrackedSample * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IMFTrackedSample * This);
    ULONG(STDMETHODCALLTYPE * Release) (IMFTrackedSample * This);
    HRESULT(STDMETHODCALLTYPE * SetAllocator) (IMFTrackedSample * This, IMFAsyncCallback * pSampleAllocator, IUnknown * pUnkState);
    END_INTERFACE
} IMFTrackedSampleVtbl;

interface IMFTrackedSample {
    CONST_VTBL struct IMFTrackedSampleVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFTrackedSample_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFTrackedSample_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFTrackedSample_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFTrackedSample_SetAllocator(This,pSampleAllocator,pUnkState)  ((This)->lpVtbl->SetAllocator(This,pSampleAllocator,pUnkState)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFTrackedSample_INTERFACE_DEFINED__ */

STDAPI MFCreateTrackedSample(IMFTrackedSample **);
STDAPI MFCreateMFByteStreamOnStream(IStream *, IMFByteStream **);
STDAPI MFCreateStreamOnMFByteStream(IMFByteStream *, IStream **);
STDAPI MFCreateMFByteStreamOnStreamEx(IUnknown *, IMFByteStream **);
STDAPI MFCreateStreamOnMFByteStreamEx(IMFByteStream *, REFIID, void **);
STDAPI MFCreateMediaTypeFromProperties(IUnknown *, IMFMediaType **);
STDAPI MFCreatePropertiesFromMediaType(IMFMediaType *, REFIID, void **);

#if (WINVER >= _WIN32_WINNT_WINBLUE) 
DEFINE_GUID(MF_WRAPPED_BUFFER_SERVICE, 0xab544072, 0xc269, 0x4ebc, 0xa5, 0x52, 0x1c, 0x3b, 0x32, 0xbe, 0xd5, 0xca);
EXTERN_GUID(MF_WRAPPED_SAMPLE_SERVICE, 0x31f52bf2, 0xd03e, 0x4048, 0x80, 0xd0, 0x9c, 0x10, 0x46, 0xd8, 0x7c, 0x61);
#endif /* WINVER >= _WIN32_WINNT_WINBLUE */
EXTERN_GUID(MF_WRAPPED_OBJECT, 0x2b182c4c, 0xd6ac, 0x49f4, 0x89, 0x15, 0xf7, 0x18, 0x87, 0xdb, 0x70, 0xcd);
EXTERN_GUID(CLSID_HttpSchemePlugin, 0x44cb442b, 0x9da9, 0x49df, 0xb3, 0xfd, 0x2, 0x37, 0x77, 0xb1, 0x6e, 0x50);
EXTERN_GUID(CLSID_UrlmonSchemePlugin, 0x9ec4b4f9, 0x3029, 0x45ad, 0x94, 0x7b, 0x34, 0x4d, 0xe2, 0xa2, 0x49, 0xe2);
EXTERN_GUID(CLSID_NetSchemePlugin, 0xe9f4ebab, 0xd97b, 0x463e, 0xa2, 0xb1, 0xc5, 0x4e, 0xe3, 0xf9, 0x41, 0x4d);

STDAPI MFCreateMFByteStreamOnStream(IStream *pStream, IMFByteStream **ppByteStream);
STDAPI MFEnumDeviceSources(IMFAttributes *pAttributes, IMFActivate ***pppSourceActivate, UINT32 *pcSourceActivate);
STDAPI MFCreateDeviceSource(IMFAttributes *pAttributes, IMFMediaSource **ppSource);
STDAPI MFCreateDeviceSourceActivate(IMFAttributes *pAttributes, IMFActivate **ppActivate);

EXTERN_GUID(CLSID_UrlmonSchemePlugin, 0x9ec4b4f9, 0x3029, 0x45ad, 0x94, 0x7b, 0x34, 0x4d, 0xe2, 0xa2, 0x49, 0xe2);
EXTERN_GUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE, 0xc60ac5fe, 0x252a, 0x478f, 0xa0, 0xef, 0xbc, 0x8f, 0xa5, 0xf7, 0xca, 0xd3);
EXTERN_GUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_HW_SOURCE, 0xde7046ba, 0x54d6, 0x4487, 0xa2, 0xa4, 0xec, 0x7c, 0xd, 0x1b, 0xd1, 0x63);
EXTERN_GUID(MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME, 0x60d0e559, 0x52f8, 0x4fa2, 0xbb, 0xce, 0xac, 0xdb, 0x34, 0xa8, 0xec, 0x1);
EXTERN_GUID(MF_DEVSOURCE_ATTRIBUTE_MEDIA_TYPE, 0x56a819ca, 0xc78, 0x4de4, 0xa0, 0xa7, 0x3d, 0xda, 0xba, 0xf, 0x24, 0xd4);
EXTERN_GUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_CATEGORY, 0x77f0ae69, 0xc3bd, 0x4509, 0x94, 0x1d, 0x46, 0x7e, 0x4d, 0x24, 0x89, 0x9e);
EXTERN_GUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK, 0x58f0aad8, 0x22bf, 0x4f8a, 0xbb, 0x3d, 0xd2, 0xc4, 0x97, 0x8c, 0x6e, 0x2f);
EXTERN_GUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_MAX_BUFFERS, 0x7dd9b730, 0x4f2d, 0x41d5, 0x8f, 0x95, 0xc, 0xc9, 0xa9, 0x12, 0xba, 0x26);
EXTERN_GUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ENDPOINT_ID, 0x30da9258, 0xfeb9, 0x47a7, 0xa4, 0x53, 0x76, 0x3a, 0x7a, 0x8e, 0x1c, 0x5f);
EXTERN_GUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_ROLE, 0xbc9d118e, 0x8c67, 0x4a18, 0x85, 0xd4, 0x12, 0xd3, 0x0, 0x40, 0x5, 0x52);
EXTERN_GUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_GUID, 0x14dd9a1c, 0x7cff, 0x41be, 0xb1, 0xb9, 0xba, 0x1a, 0xc6, 0xec, 0xb5, 0x71);
EXTERN_GUID(MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_GUID, 0x8ac3587a, 0x4ae7, 0x42d8, 0x99, 0xe0, 0x0a, 0x60, 0x13, 0xee, 0xf9, 0x0f);
EXTERN_GUID(MFSampleExtension_DeviceTimestamp, 0x8f3e35e7, 0x2dcd, 0x4887, 0x86, 0x22, 0x2a, 0x58, 0xba, 0xa6, 0x52, 0xb0);
#endif

extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0079_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mfidl_0000_0079_v0_0_s_ifspec;

#ifndef __IMFProtectedEnvironmentAccess_INTERFACE_DEFINED__
#define __IMFProtectedEnvironmentAccess_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFProtectedEnvironmentAccess;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("ef5dc845-f0d9-4ec9-b00c-cb5183d38434")IMFProtectedEnvironmentAccess:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Call(UINT32 inputLength, const BYTE *input, UINT32 outputLength, BYTE *output) = 0;
    virtual HRESULT STDMETHODCALLTYPE ReadGRL(UINT32 *outputLength, BYTE **output) = 0;
};
#else /* C style interface */
typedef struct IMFProtectedEnvironmentAccessVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFProtectedEnvironmentAccess * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IMFProtectedEnvironmentAccess * This);
    ULONG(STDMETHODCALLTYPE * Release) (IMFProtectedEnvironmentAccess * This);
    HRESULT(STDMETHODCALLTYPE * Call) (IMFProtectedEnvironmentAccess * This, UINT32 inputLength, const BYTE * input, UINT32 outputLength, BYTE * output);
    HRESULT(STDMETHODCALLTYPE * ReadGRL) (IMFProtectedEnvironmentAccess * This, UINT32 * outputLength, BYTE ** output);
    END_INTERFACE
} IMFProtectedEnvironmentAccessVtbl;

interface IMFProtectedEnvironmentAccess {
    CONST_VTBL struct IMFProtectedEnvironmentAccessVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFProtectedEnvironmentAccess_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFProtectedEnvironmentAccess_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFProtectedEnvironmentAccess_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFProtectedEnvironmentAccess_Call(This,inputLength,input,outputLength,output)  ((This)->lpVtbl->Call(This,inputLength,input,outputLength,output)) 
#define IMFProtectedEnvironmentAccess_ReadGRL(This,outputLength,output)  ((This)->lpVtbl->ReadGRL(This,outputLength,output)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFProtectedEnvironmentAccess_INTERFACE_DEFINED__ */

#ifndef __IMFSignedLibrary_INTERFACE_DEFINED__
#define __IMFSignedLibrary_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSignedLibrary;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("4a724bca-ff6a-4c07-8e0d-7a358421cf06")IMFSignedLibrary:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetProcedureAddress(LPCSTR name, PVOID * address) = 0;
};
#else /* C style interface */
typedef struct IMFSignedLibraryVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSignedLibrary * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IMFSignedLibrary * This);
    ULONG(STDMETHODCALLTYPE * Release) (IMFSignedLibrary * This);
    HRESULT(STDMETHODCALLTYPE * GetProcedureAddress) (IMFSignedLibrary * This, LPCSTR name, PVOID * address);
    END_INTERFACE
} IMFSignedLibraryVtbl;

interface IMFSignedLibrary {
    CONST_VTBL struct IMFSignedLibraryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSignedLibrary_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFSignedLibrary_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFSignedLibrary_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFSignedLibrary_GetProcedureAddress(This,name,address)  ((This)->lpVtbl->GetProcedureAddress(This,name,address)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFSignedLibrary_INTERFACE_DEFINED__ */

#ifndef __IMFSystemId_INTERFACE_DEFINED__
#define __IMFSystemId_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMFSystemId;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("fff4af3a-1fc1-4ef9-a29b-d26c49e2f31a")IMFSystemId:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetData(UINT32 * size, BYTE ** data) = 0;
    virtual HRESULT STDMETHODCALLTYPE Setup(UINT32 stage, UINT32 cbIn, const BYTE *pbIn, UINT32 *pcbOut, BYTE **ppbOut) = 0;
};
#else /* C style interface */
typedef struct IMFSystemIdVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE * QueryInterface) (IMFSystemId * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE * AddRef) (IMFSystemId * This);
    ULONG(STDMETHODCALLTYPE * Release) (IMFSystemId * This);
    HRESULT(STDMETHODCALLTYPE * GetData) (IMFSystemId * This, UINT32 * size, BYTE ** data);
    HRESULT(STDMETHODCALLTYPE * Setup) (IMFSystemId * This, UINT32 stage, UINT32 cbIn, const BYTE * pbIn, UINT32 * pcbOut, BYTE ** ppbOut);
    END_INTERFACE
} IMFSystemIdVtbl;

interface IMFSystemId {
    CONST_VTBL struct IMFSystemIdVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMFSystemId_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IMFSystemId_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IMFSystemId_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IMFSystemId_GetData(This,size,data)  ((This)->lpVtbl->GetData(This,size,data)) 
#define IMFSystemId_Setup(This,stage,cbIn,pbIn,pcbOut,ppbOut)  ((This)->lpVtbl->Setup(This,stage,cbIn,pbIn,pcbOut,ppbOut)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __IMFSystemId_INTERFACE_DEFINED__ */

STDAPI MFCreateProtectedEnvironmentAccess(IMFProtectedEnvironmentAccess **);
STDAPI MFLoadSignedLibrary(LPCWSTR, IMFSignedLibrary **);
STDAPI MFGetSystemId(IMFSystemId **);
STDAPI MFGetLocalId(const BYTE *, UINT32, LPWSTR *);
DEFINE_GUID(CLSID_MPEG2ByteStreamPlugin, 0x40871c59, 0xab40, 0x471f, 0x8d, 0xc3, 0x1f, 0x25, 0x9d, 0x86, 0x24, 0x79);
EXTERN_GUID(MF_MEDIASOURCE_SERVICE, 0xf09992f7, 0x9fba, 0x4c4a, 0xa3, 0x7f, 0x8c, 0x47, 0xb4, 0xe1, 0xdf, 0xe7);

unsigned long __RPC_USER BSTR_UserSize(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree(unsigned long *, BSTR *);

unsigned long __RPC_USER LPSAFEARRAY_UserSize(unsigned long *, unsigned long, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserMarshal(unsigned long *, unsigned char *, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY *);
void __RPC_USER LPSAFEARRAY_UserFree(unsigned long *, LPSAFEARRAY *);

unsigned long __RPC_USER BSTR_UserSize64(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal64(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree64(unsigned long *, BSTR *);

unsigned long __RPC_USER LPSAFEARRAY_UserSize64(unsigned long *, unsigned long, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserMarshal64(unsigned long *, unsigned char *, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserUnmarshal64(unsigned long *, unsigned char *, LPSAFEARRAY *);
void __RPC_USER LPSAFEARRAY_UserFree64(unsigned long *, LPSAFEARRAY *);

HRESULT STDMETHODCALLTYPE IMFSourceResolver_BeginCreateObjectFromURL_Proxy(IMFSourceResolver *This, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore *pProps, IUnknown **ppIUnknownCancelCookie, IMFAsyncCallback *pCallback, IUnknown *punkState);
HRESULT STDMETHODCALLTYPE IMFSourceResolver_BeginCreateObjectFromURL_Stub(IMFSourceResolver *This, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore *pProps, IMFRemoteAsyncCallback *pCallback);
HRESULT STDMETHODCALLTYPE IMFSourceResolver_EndCreateObjectFromURL_Proxy(IMFSourceResolver *This, IMFAsyncResult *pResult, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject);
HRESULT STDMETHODCALLTYPE IMFSourceResolver_EndCreateObjectFromURL_Stub(IMFSourceResolver *This, IUnknown *pResult, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject);
HRESULT STDMETHODCALLTYPE IMFSourceResolver_BeginCreateObjectFromByteStream_Proxy(IMFSourceResolver *This, IMFByteStream *pByteStream, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore *pProps, IUnknown **ppIUnknownCancelCookie, IMFAsyncCallback *pCallback, IUnknown *punkState);
HRESULT STDMETHODCALLTYPE IMFSourceResolver_BeginCreateObjectFromByteStream_Stub(IMFSourceResolver *This, IMFByteStream *pByteStream, LPCWSTR pwszURL, DWORD dwFlags, IPropertyStore *pProps, IMFRemoteAsyncCallback *pCallback);
HRESULT STDMETHODCALLTYPE IMFSourceResolver_EndCreateObjectFromByteStream_Proxy(IMFSourceResolver *This, IMFAsyncResult *pResult, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject);
HRESULT STDMETHODCALLTYPE IMFSourceResolver_EndCreateObjectFromByteStream_Stub(IMFSourceResolver *This, IUnknown *pResult, MF_OBJECT_TYPE *pObjectType, IUnknown **ppObject);
HRESULT STDMETHODCALLTYPE IMFMediaSource_CreatePresentationDescriptor_Proxy(IMFMediaSource *This, IMFPresentationDescriptor **ppPresentationDescriptor);
HRESULT STDMETHODCALLTYPE IMFMediaSource_CreatePresentationDescriptor_Stub(IMFMediaSource *This, DWORD *pcbPD, BYTE **pbPD, IMFPresentationDescriptor **ppRemotePD);
HRESULT STDMETHODCALLTYPE IMFMediaStream_RequestSample_Proxy(IMFMediaStream *This, IUnknown *pToken);
HRESULT STDMETHODCALLTYPE IMFMediaStream_RequestSample_Stub(IMFMediaStream *This);
HRESULT STDMETHODCALLTYPE IMFTopologyNode_GetOutputPrefType_Proxy(IMFTopologyNode *This, DWORD dwOutputIndex, IMFMediaType **ppType);
HRESULT STDMETHODCALLTYPE IMFTopologyNode_GetOutputPrefType_Stub(IMFTopologyNode *This, DWORD dwOutputIndex, DWORD *pcbData, BYTE **ppbData);
HRESULT STDMETHODCALLTYPE IMFTopologyNode_GetInputPrefType_Proxy(IMFTopologyNode *This, DWORD dwInputIndex, IMFMediaType **ppType);
HRESULT STDMETHODCALLTYPE IMFTopologyNode_GetInputPrefType_Stub(IMFTopologyNode *This, DWORD dwInputIndex, DWORD *pcbData, BYTE **ppbData);
HRESULT STDMETHODCALLTYPE IMFMediaTypeHandler_GetCurrentMediaType_Proxy(IMFMediaTypeHandler *This, IMFMediaType **ppMediaType);
HRESULT STDMETHODCALLTYPE IMFMediaTypeHandler_GetCurrentMediaType_Stub(IMFMediaTypeHandler *This, BYTE **ppbData, DWORD *pcbData);
HRESULT STDMETHODCALLTYPE IMFContentProtectionManager_BeginEnableContent_Proxy(IMFContentProtectionManager *This, IMFActivate *pEnablerActivate, IMFTopology *pTopo, IMFAsyncCallback *pCallback, IUnknown *punkState);
HRESULT STDMETHODCALLTYPE IMFContentProtectionManager_BeginEnableContent_Stub(IMFContentProtectionManager *This, REFCLSID clsidType, BYTE *pbData, DWORD cbData, IMFRemoteAsyncCallback *pCallback);
HRESULT STDMETHODCALLTYPE IMFContentProtectionManager_EndEnableContent_Proxy(IMFContentProtectionManager *This, IMFAsyncResult *pResult);
HRESULT STDMETHODCALLTYPE IMFContentProtectionManager_EndEnableContent_Stub(IMFContentProtectionManager *This, IUnknown *pResult);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_BeginRegisterTopologyWorkQueuesWithMMCSS_Proxy(IMFWorkQueueServices *This, IMFAsyncCallback *pCallback, IUnknown *pState);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_BeginRegisterTopologyWorkQueuesWithMMCSS_Stub(IMFWorkQueueServices *This, IMFRemoteAsyncCallback *pCallback);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_EndRegisterTopologyWorkQueuesWithMMCSS_Proxy(IMFWorkQueueServices *This, IMFAsyncResult *pResult);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_EndRegisterTopologyWorkQueuesWithMMCSS_Stub(IMFWorkQueueServices *This, IUnknown *pResult);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_BeginUnregisterTopologyWorkQueuesWithMMCSS_Proxy(IMFWorkQueueServices *This, IMFAsyncCallback *pCallback, IUnknown *pState);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_BeginUnregisterTopologyWorkQueuesWithMMCSS_Stub(IMFWorkQueueServices *This, IMFRemoteAsyncCallback *pCallback);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_EndUnregisterTopologyWorkQueuesWithMMCSS_Proxy(IMFWorkQueueServices *This, IMFAsyncResult *pResult);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_EndUnregisterTopologyWorkQueuesWithMMCSS_Stub(IMFWorkQueueServices *This, IUnknown *pResult);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_BeginRegisterPlatformWorkQueueWithMMCSS_Proxy(IMFWorkQueueServices *This, DWORD dwPlatformWorkQueue, LPCWSTR wszClass, DWORD dwTaskId, IMFAsyncCallback *pCallback, IUnknown *pState);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_BeginRegisterPlatformWorkQueueWithMMCSS_Stub(IMFWorkQueueServices *This, DWORD dwPlatformWorkQueue, LPCWSTR wszClass, DWORD dwTaskId, IMFRemoteAsyncCallback *pCallback);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_EndRegisterPlatformWorkQueueWithMMCSS_Proxy(IMFWorkQueueServices *This, IMFAsyncResult *pResult, DWORD *pdwTaskId);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_EndRegisterPlatformWorkQueueWithMMCSS_Stub(IMFWorkQueueServices *This, IUnknown *pResult, DWORD *pdwTaskId);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_BeginUnregisterPlatformWorkQueueWithMMCSS_Proxy(IMFWorkQueueServices *This, DWORD dwPlatformWorkQueue, IMFAsyncCallback *pCallback, IUnknown *pState);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_BeginUnregisterPlatformWorkQueueWithMMCSS_Stub(IMFWorkQueueServices *This, DWORD dwPlatformWorkQueue, IMFRemoteAsyncCallback *pCallback);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_EndUnregisterPlatformWorkQueueWithMMCSS_Proxy(IMFWorkQueueServices *This, IMFAsyncResult *pResult);
HRESULT STDMETHODCALLTYPE IMFWorkQueueServices_EndUnregisterPlatformWorkQueueWithMMCSS_Stub(IMFWorkQueueServices *This, IUnknown *pResult);
HRESULT STDMETHODCALLTYPE IMFPMPHost_CreateObjectByCLSID_Proxy(IMFPMPHost *This, REFCLSID clsid, IStream *pStream, REFIID riid, void **ppv);
HRESULT STDMETHODCALLTYPE IMFPMPHost_CreateObjectByCLSID_Stub(IMFPMPHost *This, REFCLSID clsid, BYTE *pbData, DWORD cbData, REFIID riid, void **ppv);


#ifdef __cplusplus
}
#endif

#endif /* _MFIDL_H */
