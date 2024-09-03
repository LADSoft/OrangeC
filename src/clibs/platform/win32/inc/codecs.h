#ifndef _CODECS_H
#define _CODECS_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* CODEC definitions */

#include <pshpack8.h>
#include "appavcap.h"

#define IC_E_CAPS_INSTANTIATION_FAILURE  0x83010001
#define IC_E_CAPS_INITIALIZATION_FAILURE  0x83010002
#define IC_E_NO_FORMATS  0x83010003
#define IC_E_NO_SUCH_FORMAT  0x83010005
#define IC_E_INTERNAL_ERROR  0x83010006

#ifndef DECLARE_INTERFACE_PTR
#ifdef __cplusplus
#define DECLARE_INTERFACE_PTR(iface,piface)  interface iface; typedef iface *piface
#else
#define DECLARE_INTERFACE_PTR(iface,piface)  typedef interface iface iface, *piface
#endif
#endif /* DECLARE_INTERFACE_PTR */

#define IUNKNOWN_METHODS(IPURE) \
    STDMETHOD (QueryInterface) \
        (THIS_ REFIID riid, LPVOID *ppvObj) IPURE; \
    STDMETHOD_(ULONG,AddRef)  (THIS) IPURE; \
    STDMETHOD_(ULONG,Release) (THIS) IPURE;

#define IINSTALLAUDIOCODECS_METHODS(IPURE) \
    STDMETHOD(AddACMFormat) \
            (THIS_ LPWAVEFORMATEX lpwfx, PAUDCAP_INFO pAudCapInfo) IPURE; \
    STDMETHOD (RemoveACMFormat) \
            (THIS_ LPWAVEFORMATEX lpwfx) IPURE; \
    STDMETHOD (ReorderFormats) \
            (THIS_ PAUDCAP_INFO_LIST pAudCapInfoList) IPURE; \
    STDMETHOD (EnumFormats) \
            (THIS_ PAUDCAP_INFO_LIST *ppAudCapInfoList) IPURE; \
    STDMETHOD (FreeBuffer) (THIS_ LPVOID lpBuffer) IPURE;

#define IINSTALLVIDEOCODECS_METHODS(IPURE) \
        STDMETHOD(AddVCMFormat) \
                (THIS_ PVIDCAP_INFO pVidCapInfo) IPURE; \
        STDMETHOD (RemoveVCMFormat) \
                (THIS_ PVIDCAP_INFO pVidCapInfo) IPURE; \
        STDMETHOD (ReorderFormats) \
                (THIS_ PVIDCAP_INFO_LIST pVidCapInfoList) IPURE; \
        STDMETHOD (EnumFormats) \
                (THIS_ PVIDCAP_INFO_LIST *ppVidCapInfoList) IPURE; \
        STDMETHOD (FreeBuffer) (THIS_ LPVOID lpBuffer) IPURE; \

#undef INTERFACE
#define INTERFACE IInstallAudioCodecs
DECLARE_INTERFACE_(IInstallAudioCodecs,IUnknown)
{
    IUNKNOWN_METHODS(PURE)
    IINSTALLAUDIOCODECS_METHODS(PURE)
};
DECLARE_INTERFACE_PTR(IInstallAudioCodecs,LPINSTALLAUDIOCODECS);

#undef INTERFACE
#define INTERFACE IInstallVideoCodecs
DECLARE_INTERFACE_(IInstallVideoCodecs,IUnknown)
{
    IUNKNOWN_METHODS(PURE)
    IINSTALLVIDEOCODECS_METHODS(PURE)
};
DECLARE_INTERFACE_PTR(IInstallVideoCodecs,LPINSTALLVIDEOCODECS);

#undef INTERFACE
#define INTERFACE  IInstallCodecs
DECLARE_INTERFACE_(IInstallCodecs,IUnknown)
{
    IUNKNOWN_METHODS(PURE)
};
DECLARE_INTERFACE_PTR(IInstallCodecs,LPINSTALLCODECS);

EXTERN_C HRESULT WINAPI CreateInstallCodecs(IUnknown*,REFIID,void**);

typedef HRESULT (WINAPI *PFNCREATEINSTALLCODECS)(IUnknown*,REFIID,void**);

DEFINE_GUID(CLSID_InstallCodecs,0x8ed14cc0,0x7a1f,0x11d0,0x92,0xf6,0x0,0xa0,0xc9,0x22,0xe6,0xb2);
DEFINE_GUID(IID_IInstallCodecs,0x8ed14cc1,0x7a1f,0x11d0,0x92,0xf6,0x0,0xa0,0xc9,0x22,0xe6,0xb2);
DEFINE_GUID(IID_IInstallAudioCodecs,0x8ed14cc2,0x7a1f,0x11d0,0x92,0xf6,0x0,0xa0,0xc9,0x22,0xe6,0xb2);
DEFINE_GUID(IID_IInstallVideoCodecs,0x8ed14cc3,0x7a1f,0x11d0,0x92,0xf6,0x0,0xa0,0xc9,0x22,0xe6,0xb2);

#include <poppack.h>

#endif /* _CODECS_H */
