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

#ifndef _APPXPACKAGING_H
#define _APPXPACKAGING_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IAppxFactory_FWD_DEFINED__
#define __IAppxFactory_FWD_DEFINED__
typedef interface IAppxFactory IAppxFactory;
#endif /* __IAppxFactory_FWD_DEFINED__ */

#ifndef __IAppxPackageReader_FWD_DEFINED__
#define __IAppxPackageReader_FWD_DEFINED__
typedef interface IAppxPackageReader IAppxPackageReader;
#endif /* __IAppxPackageReader_FWD_DEFINED__ */

#ifndef __IAppxPackageWriter_FWD_DEFINED__
#define __IAppxPackageWriter_FWD_DEFINED__
typedef interface IAppxPackageWriter IAppxPackageWriter;
#endif /* __IAppxPackageWriter_FWD_DEFINED__ */

#ifndef __IAppxFile_FWD_DEFINED__
#define __IAppxFile_FWD_DEFINED__
typedef interface IAppxFile IAppxFile;
#endif /* __IAppxFile_FWD_DEFINED__ */

#ifndef __IAppxFilesEnumerator_FWD_DEFINED__
#define __IAppxFilesEnumerator_FWD_DEFINED__
typedef interface IAppxFilesEnumerator IAppxFilesEnumerator;
#endif /* __IAppxFilesEnumerator_FWD_DEFINED__ */

#ifndef __IAppxBlockMapReader_FWD_DEFINED__
#define __IAppxBlockMapReader_FWD_DEFINED__
typedef interface IAppxBlockMapReader IAppxBlockMapReader;
#endif /* __IAppxBlockMapReader_FWD_DEFINED__ */

#ifndef __IAppxBlockMapFile_FWD_DEFINED__
#define __IAppxBlockMapFile_FWD_DEFINED__
typedef interface IAppxBlockMapFile IAppxBlockMapFile;
#endif /* __IAppxBlockMapFile_FWD_DEFINED__ */

#ifndef __IAppxBlockMapFilesEnumerator_FWD_DEFINED__
#define __IAppxBlockMapFilesEnumerator_FWD_DEFINED__
typedef interface IAppxBlockMapFilesEnumerator IAppxBlockMapFilesEnumerator;
#endif /* __IAppxBlockMapFilesEnumerator_FWD_DEFINED__ */

#ifndef __IAppxBlockMapBlock_FWD_DEFINED__
#define __IAppxBlockMapBlock_FWD_DEFINED__
typedef interface IAppxBlockMapBlock IAppxBlockMapBlock;
#endif /* __IAppxBlockMapBlock_FWD_DEFINED__ */

#ifndef __IAppxBlockMapBlocksEnumerator_FWD_DEFINED__
#define __IAppxBlockMapBlocksEnumerator_FWD_DEFINED__
typedef interface IAppxBlockMapBlocksEnumerator IAppxBlockMapBlocksEnumerator;
#endif /* __IAppxBlockMapBlocksEnumerator_FWD_DEFINED__ */

#ifndef __IAppxManifestReader_FWD_DEFINED__
#define __IAppxManifestReader_FWD_DEFINED__
typedef interface IAppxManifestReader IAppxManifestReader;
#endif /* __IAppxManifestReader_FWD_DEFINED__ */

#ifndef __IAppxManifestReader2_FWD_DEFINED__
#define __IAppxManifestReader2_FWD_DEFINED__
typedef interface IAppxManifestReader2 IAppxManifestReader2;
#endif /* __IAppxManifestReader2_FWD_DEFINED__ */

#ifndef __IAppxManifestReader3_FWD_DEFINED__
#define __IAppxManifestReader3_FWD_DEFINED__
typedef interface IAppxManifestReader3 IAppxManifestReader3;
#endif /* __IAppxManifestReader3_FWD_DEFINED__ */

#ifndef __IAppxManifestReader4_FWD_DEFINED__
#define __IAppxManifestReader4_FWD_DEFINED__
typedef interface IAppxManifestReader4 IAppxManifestReader4;
#endif /* __IAppxManifestReader4_FWD_DEFINED__ */

#ifndef __IAppxManifestOptionalPackageInfo_FWD_DEFINED__
#define __IAppxManifestOptionalPackageInfo_FWD_DEFINED__
typedef interface IAppxManifestOptionalPackageInfo IAppxManifestOptionalPackageInfo;
#endif /* __IAppxManifestOptionalPackageInfo_FWD_DEFINED__ */

#ifndef __IAppxManifestPackageId_FWD_DEFINED__
#define __IAppxManifestPackageId_FWD_DEFINED__
typedef interface IAppxManifestPackageId IAppxManifestPackageId;
#endif /* __IAppxManifestPackageId_FWD_DEFINED__ */

#ifndef __IAppxManifestProperties_FWD_DEFINED__
#define __IAppxManifestProperties_FWD_DEFINED__
typedef interface IAppxManifestProperties IAppxManifestProperties;
#endif /* __IAppxManifestProperties_FWD_DEFINED__ */

#ifndef __IAppxManifestTargetDeviceFamiliesEnumerator_FWD_DEFINED__
#define __IAppxManifestTargetDeviceFamiliesEnumerator_FWD_DEFINED__
typedef interface IAppxManifestTargetDeviceFamiliesEnumerator IAppxManifestTargetDeviceFamiliesEnumerator;
#endif /* __IAppxManifestTargetDeviceFamiliesEnumerator_FWD_DEFINED__ */

#ifndef __IAppxManifestTargetDeviceFamily_FWD_DEFINED__
#define __IAppxManifestTargetDeviceFamily_FWD_DEFINED__
typedef interface IAppxManifestTargetDeviceFamily IAppxManifestTargetDeviceFamily;
#endif /* __IAppxManifestTargetDeviceFamily_FWD_DEFINED__ */

#ifndef __IAppxManifestPackageDependenciesEnumerator_FWD_DEFINED__
#define __IAppxManifestPackageDependenciesEnumerator_FWD_DEFINED__
typedef interface IAppxManifestPackageDependenciesEnumerator IAppxManifestPackageDependenciesEnumerator;
#endif /* __IAppxManifestPackageDependenciesEnumerator_FWD_DEFINED__ */

#ifndef __IAppxManifestPackageDependency_FWD_DEFINED__
#define __IAppxManifestPackageDependency_FWD_DEFINED__
typedef interface IAppxManifestPackageDependency IAppxManifestPackageDependency;
#endif /* __IAppxManifestPackageDependency_FWD_DEFINED__ */

#ifndef __IAppxManifestPackageDependency2_FWD_DEFINED__
#define __IAppxManifestPackageDependency2_FWD_DEFINED__
typedef interface IAppxManifestPackageDependency2 IAppxManifestPackageDependency2;
#endif /* __IAppxManifestPackageDependency2_FWD_DEFINED__ */

#ifndef __IAppxManifestResourcesEnumerator_FWD_DEFINED__
#define __IAppxManifestResourcesEnumerator_FWD_DEFINED__
typedef interface IAppxManifestResourcesEnumerator IAppxManifestResourcesEnumerator;
#endif /* __IAppxManifestResourcesEnumerator_FWD_DEFINED__ */

#ifndef __IAppxManifestDeviceCapabilitiesEnumerator_FWD_DEFINED__
#define __IAppxManifestDeviceCapabilitiesEnumerator_FWD_DEFINED__
typedef interface IAppxManifestDeviceCapabilitiesEnumerator IAppxManifestDeviceCapabilitiesEnumerator;
#endif /* __IAppxManifestDeviceCapabilitiesEnumerator_FWD_DEFINED__ */

#ifndef __IAppxManifestCapabilitiesEnumerator_FWD_DEFINED__
#define __IAppxManifestCapabilitiesEnumerator_FWD_DEFINED__
typedef interface IAppxManifestCapabilitiesEnumerator IAppxManifestCapabilitiesEnumerator;
#endif /* __IAppxManifestCapabilitiesEnumerator_FWD_DEFINED__ */

#ifndef __IAppxManifestApplicationsEnumerator_FWD_DEFINED__
#define __IAppxManifestApplicationsEnumerator_FWD_DEFINED__
typedef interface IAppxManifestApplicationsEnumerator IAppxManifestApplicationsEnumerator;
#endif /* __IAppxManifestApplicationsEnumerator_FWD_DEFINED__ */

#ifndef __IAppxManifestApplication_FWD_DEFINED__
#define __IAppxManifestApplication_FWD_DEFINED__
typedef interface IAppxManifestApplication IAppxManifestApplication;
#endif /* __IAppxManifestApplication_FWD_DEFINED__ */

#ifndef __IAppxManifestQualifiedResourcesEnumerator_FWD_DEFINED__
#define __IAppxManifestQualifiedResourcesEnumerator_FWD_DEFINED__
typedef interface IAppxManifestQualifiedResourcesEnumerator IAppxManifestQualifiedResourcesEnumerator;
#endif /* __IAppxManifestQualifiedResourcesEnumerator_FWD_DEFINED__ */

#ifndef __IAppxManifestQualifiedResource_FWD_DEFINED__
#define __IAppxManifestQualifiedResource_FWD_DEFINED__
typedef interface IAppxManifestQualifiedResource IAppxManifestQualifiedResource;
#endif /* __IAppxManifestQualifiedResource_FWD_DEFINED__ */

#ifndef __IAppxBundleFactory_FWD_DEFINED__
#define __IAppxBundleFactory_FWD_DEFINED__
typedef interface IAppxBundleFactory IAppxBundleFactory;
#endif /* __IAppxBundleFactory_FWD_DEFINED__ */

#ifndef __IAppxBundleWriter_FWD_DEFINED__
#define __IAppxBundleWriter_FWD_DEFINED__
typedef interface IAppxBundleWriter IAppxBundleWriter;
#endif /* __IAppxBundleWriter_FWD_DEFINED__ */

#ifndef __IAppxBundleReader_FWD_DEFINED__
#define __IAppxBundleReader_FWD_DEFINED__
typedef interface IAppxBundleReader IAppxBundleReader;
#endif /* __IAppxBundleReader_FWD_DEFINED__ */

#ifndef __IAppxBundleManifestReader_FWD_DEFINED__
#define __IAppxBundleManifestReader_FWD_DEFINED__
typedef interface IAppxBundleManifestReader IAppxBundleManifestReader;
#endif /* __IAppxBundleManifestReader_FWD_DEFINED__ */

#ifndef __IAppxBundleManifestPackageInfoEnumerator_FWD_DEFINED__
#define __IAppxBundleManifestPackageInfoEnumerator_FWD_DEFINED__
typedef interface IAppxBundleManifestPackageInfoEnumerator IAppxBundleManifestPackageInfoEnumerator;
#endif /* __IAppxBundleManifestPackageInfoEnumerator_FWD_DEFINED__ */

#ifndef __IAppxBundleManifestPackageInfo_FWD_DEFINED__
#define __IAppxBundleManifestPackageInfo_FWD_DEFINED__
typedef interface IAppxBundleManifestPackageInfo IAppxBundleManifestPackageInfo;
#endif /* __IAppxBundleManifestPackageInfo_FWD_DEFINED__ */

#ifndef __IAppxEncryptionFactory_FWD_DEFINED__
#define __IAppxEncryptionFactory_FWD_DEFINED__
typedef interface IAppxEncryptionFactory IAppxEncryptionFactory;
#endif /* __IAppxEncryptionFactory_FWD_DEFINED__ */

#ifndef __IAppxEncryptedPackageWriter_FWD_DEFINED__
#define __IAppxEncryptedPackageWriter_FWD_DEFINED__
typedef interface IAppxEncryptedPackageWriter IAppxEncryptedPackageWriter;
#endif /* __IAppxEncryptedPackageWriter_FWD_DEFINED__ */

#ifndef __IAppxEncryptedBundleWriter_FWD_DEFINED__
#define __IAppxEncryptedBundleWriter_FWD_DEFINED__
typedef interface IAppxEncryptedBundleWriter IAppxEncryptedBundleWriter;
#endif /* __IAppxEncryptedBundleWriter_FWD_DEFINED__ */

#ifndef __AppxFactory_FWD_DEFINED__
#define __AppxFactory_FWD_DEFINED__

#ifdef __cplusplus
typedef class AppxFactory AppxFactory;
#else
typedef struct AppxFactory AppxFactory;
#endif /* __cplusplus */

#endif  /* __AppxFactory_FWD_DEFINED__ */


#ifndef __AppxBundleFactory_FWD_DEFINED__
#define __AppxBundleFactory_FWD_DEFINED__

#ifdef __cplusplus
typedef class AppxBundleFactory AppxBundleFactory;
#else
typedef struct AppxBundleFactory AppxBundleFactory;
#endif /* __cplusplus */

#endif  /* __AppxBundleFactory_FWD_DEFINED__ */

#ifndef __AppxEncryptionFactory_FWD_DEFINED__
#define __AppxEncryptionFactory_FWD_DEFINED__

#ifdef __cplusplus
typedef class AppxEncryptionFactory AppxEncryptionFactory;
#else
typedef struct AppxEncryptionFactory AppxEncryptionFactory;
#endif /* __cplusplus */

#endif  /* __AppxEncryptionFactory_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#if (NTDDI_VERSION >= NTDDI_WIN7)
typedef struct APPX_PACKAGE_SETTINGS {
    BOOL forceZip32;
    IUri *hashMethod;
} APPX_PACKAGE_SETTINGS;

typedef enum APPX_COMPRESSION_OPTION {
    APPX_COMPRESSION_OPTION_NONE = 0,
    APPX_COMPRESSION_OPTION_NORMAL = 1,
    APPX_COMPRESSION_OPTION_MAXIMUM = 2,
    APPX_COMPRESSION_OPTION_FAST = 3,
    APPX_COMPRESSION_OPTION_SUPERFAST = 4
} APPX_COMPRESSION_OPTION;

typedef enum APPX_FOOTPRINT_FILE_TYPE {
    APPX_FOOTPRINT_FILE_TYPE_MANIFEST = 0,
    APPX_FOOTPRINT_FILE_TYPE_BLOCKMAP = 1,
    APPX_FOOTPRINT_FILE_TYPE_SIGNATURE = 2,
    APPX_FOOTPRINT_FILE_TYPE_CODEINTEGRITY = 3
} APPX_FOOTPRINT_FILE_TYPE;

typedef enum APPX_BUNDLE_FOOTPRINT_FILE_TYPE {
    APPX_BUNDLE_FOOTPRINT_FILE_TYPE_FIRST = 0,
    APPX_BUNDLE_FOOTPRINT_FILE_TYPE_MANIFEST = 0,
    APPX_BUNDLE_FOOTPRINT_FILE_TYPE_BLOCKMAP = 1,
    APPX_BUNDLE_FOOTPRINT_FILE_TYPE_SIGNATURE = 2,
    APPX_BUNDLE_FOOTPRINT_FILE_TYPE_LAST = 2
} APPX_BUNDLE_FOOTPRINT_FILE_TYPE;

typedef enum APPX_CAPABILITIES {
    APPX_CAPABILITY_INTERNET_CLIENT = 0x1,
    APPX_CAPABILITY_INTERNET_CLIENT_SERVER = 0x2,
    APPX_CAPABILITY_PRIVATE_NETWORK_CLIENT_SERVER = 0x4,
    APPX_CAPABILITY_DOCUMENTS_LIBRARY = 0x8,
    APPX_CAPABILITY_PICTURES_LIBRARY = 0x10,
    APPX_CAPABILITY_VIDEOS_LIBRARY = 0x20,
    APPX_CAPABILITY_MUSIC_LIBRARY = 0x40,
    APPX_CAPABILITY_ENTERPRISE_AUTHENTICATION = 0x80,
    APPX_CAPABILITY_SHARED_USER_CERTIFICATES = 0x100,
    APPX_CAPABILITY_REMOVABLE_STORAGE = 0x200,
    APPX_CAPABILITY_APPOINTMENTS = 0x400,
    APPX_CAPABILITY_CONTACTS = 0x800
} APPX_CAPABILITIES;

/* DEFINE_ENUM_FLAG_OPERATORS(APPX_CAPABILITIES); */

typedef enum APPX_PACKAGE_ARCHITECTURE {
    APPX_PACKAGE_ARCHITECTURE_X86 = 0,
    APPX_PACKAGE_ARCHITECTURE_ARM = 5,
    APPX_PACKAGE_ARCHITECTURE_X64 = 9,
    APPX_PACKAGE_ARCHITECTURE_NEUTRAL = 11,
    APPX_PACKAGE_ARCHITECTURE_ARM64 = 12
} APPX_PACKAGE_ARCHITECTURE;

typedef enum APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE {
    APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE_APPLICATION = 0,
    APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE_RESOURCE = 1
} APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE;

typedef enum DX_FEATURE_LEVEL {
    DX_FEATURE_LEVEL_UNSPECIFIED = 0,
    DX_FEATURE_LEVEL_9 = 1,
    DX_FEATURE_LEVEL_10 = 2,
    DX_FEATURE_LEVEL_11 = 3
} DX_FEATURE_LEVEL;

typedef enum APPX_CAPABILITY_CLASS_TYPE {
    APPX_CAPABILITY_CLASS_DEFAULT = 0,
    APPX_CAPABILITY_CLASS_GENERAL = 0x1,
    APPX_CAPABILITY_CLASS_RESTRICTED = 0x2,
    APPX_CAPABILITY_CLASS_WINDOWS = 0x4,
    APPX_CAPABILITY_CLASS_ALL = 0x7
} APPX_CAPABILITY_CLASS_TYPE;


extern RPC_IF_HANDLE __MIDL_itf_appxpackaging_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_appxpackaging_0000_0000_v0_0_s_ifspec;

#ifndef __IAppxFactory_INTERFACE_DEFINED__
#define __IAppxFactory_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("beb94909-e451-438b-b5a7-d79e767b75d8")
IAppxFactory:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreatePackageWriter(IStream * outputStream, APPX_PACKAGE_SETTINGS * settings, IAppxPackageWriter ** packageWriter) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreatePackageReader(IStream * inputStream, IAppxPackageReader ** packageReader) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateManifestReader(IStream * inputStream, IAppxManifestReader ** manifestReader) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBlockMapReader(IStream * inputStream, IAppxBlockMapReader ** blockMapReader) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateValidatedBlockMapReader(IStream * blockMapStream, LPCWSTR signatureFileName, IAppxBlockMapReader ** blockMapReader) = 0;
};
#else   /* C style interface */

typedef struct IAppxFactoryVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxFactory * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxFactory * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxFactory * This);
    HRESULT (STDMETHODCALLTYPE * CreatePackageWriter)(IAppxFactory * This, IStream * outputStream, APPX_PACKAGE_SETTINGS * settings, IAppxPackageWriter ** packageWriter);
    HRESULT (STDMETHODCALLTYPE * CreatePackageReader)(IAppxFactory * This, IStream * inputStream, IAppxPackageReader ** packageReader);
    HRESULT (STDMETHODCALLTYPE * CreateManifestReader)(IAppxFactory * This, IStream * inputStream, IAppxManifestReader ** manifestReader);
    HRESULT (STDMETHODCALLTYPE * CreateBlockMapReader)(IAppxFactory * This, IStream * inputStream, IAppxBlockMapReader ** blockMapReader);
    HRESULT (STDMETHODCALLTYPE * CreateValidatedBlockMapReader)(IAppxFactory * This, IStream * blockMapStream, LPCWSTR signatureFileName, IAppxBlockMapReader ** blockMapReader);
    END_INTERFACE
} IAppxFactoryVtbl;

interface IAppxFactory {
    CONST_VTBL struct IAppxFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxFactory_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxFactory_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxFactory_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxFactory_CreatePackageWriter(This,outputStream,settings,packageWriter)  ((This)->lpVtbl->CreatePackageWriter(This,outputStream,settings,packageWriter)) 
#define IAppxFactory_CreatePackageReader(This,inputStream,packageReader)  ((This)->lpVtbl->CreatePackageReader(This,inputStream,packageReader)) 
#define IAppxFactory_CreateManifestReader(This,inputStream,manifestReader)  ((This)->lpVtbl->CreateManifestReader(This,inputStream,manifestReader)) 
#define IAppxFactory_CreateBlockMapReader(This,inputStream,blockMapReader)  ((This)->lpVtbl->CreateBlockMapReader(This,inputStream,blockMapReader)) 
#define IAppxFactory_CreateValidatedBlockMapReader(This,blockMapStream,signatureFileName,blockMapReader)  ((This)->lpVtbl->CreateValidatedBlockMapReader(This,blockMapStream,signatureFileName,blockMapReader)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxFactory_INTERFACE_DEFINED__ */


#ifndef __IAppxPackageReader_INTERFACE_DEFINED__
#define __IAppxPackageReader_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxPackageReader;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("b5c49650-99bc-481c-9a34-3d53a4106708")
IAppxPackageReader:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetBlockMap(IAppxBlockMapReader ** blockMapReader) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFootprintFile(APPX_FOOTPRINT_FILE_TYPE type, IAppxFile ** file) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPayloadFile(LPCWSTR fileName, IAppxFile ** file) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPayloadFiles(IAppxFilesEnumerator ** filesEnumerator) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetManifest(IAppxManifestReader ** manifestReader) = 0;
};
#else   /* C style interface */

typedef struct IAppxPackageReaderVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxPackageReader * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxPackageReader * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxPackageReader * This);
    HRESULT (STDMETHODCALLTYPE * GetBlockMap)(IAppxPackageReader * This, IAppxBlockMapReader ** blockMapReader);
    HRESULT (STDMETHODCALLTYPE * GetFootprintFile)(IAppxPackageReader * This, APPX_FOOTPRINT_FILE_TYPE type, IAppxFile ** file);
    HRESULT (STDMETHODCALLTYPE * GetPayloadFile)(IAppxPackageReader * This, LPCWSTR fileName, IAppxFile ** file);
    HRESULT (STDMETHODCALLTYPE * GetPayloadFiles)(IAppxPackageReader * This, IAppxFilesEnumerator ** filesEnumerator);
    HRESULT (STDMETHODCALLTYPE * GetManifest)(IAppxPackageReader * This, IAppxManifestReader ** manifestReader);
    END_INTERFACE
} IAppxPackageReaderVtbl;

interface IAppxPackageReader {
    CONST_VTBL struct IAppxPackageReaderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxPackageReader_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxPackageReader_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxPackageReader_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxPackageReader_GetBlockMap(This,blockMapReader)  ((This)->lpVtbl->GetBlockMap(This,blockMapReader)) 
#define IAppxPackageReader_GetFootprintFile(This,type,file)  ((This)->lpVtbl->GetFootprintFile(This,type,file)) 
#define IAppxPackageReader_GetPayloadFile(This,fileName,file)  ((This)->lpVtbl->GetPayloadFile(This,fileName,file)) 
#define IAppxPackageReader_GetPayloadFiles(This,filesEnumerator)  ((This)->lpVtbl->GetPayloadFiles(This,filesEnumerator)) 
#define IAppxPackageReader_GetManifest(This,manifestReader)  ((This)->lpVtbl->GetManifest(This,manifestReader)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxPackageReader_INTERFACE_DEFINED__ */

#ifndef __IAppxPackageWriter_INTERFACE_DEFINED__
#define __IAppxPackageWriter_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxPackageWriter;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("9099e33b-246f-41e4-881a-008eb613f858")
IAppxPackageWriter:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE AddPayloadFile(LPCWSTR fileName, LPCWSTR contentType, APPX_COMPRESSION_OPTION compressionOption, IStream * inputStream) = 0;
    virtual HRESULT STDMETHODCALLTYPE Close(IStream * manifest) = 0;
};
#else   /* C style interface */

typedef struct IAppxPackageWriterVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxPackageWriter * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxPackageWriter * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxPackageWriter * This);
    HRESULT (STDMETHODCALLTYPE * AddPayloadFile)(IAppxPackageWriter * This, LPCWSTR fileName, LPCWSTR contentType, APPX_COMPRESSION_OPTION compressionOption, IStream * inputStream);
    HRESULT (STDMETHODCALLTYPE * Close)(IAppxPackageWriter * This, IStream * manifest);
    END_INTERFACE
} IAppxPackageWriterVtbl;

interface IAppxPackageWriter {
    CONST_VTBL struct IAppxPackageWriterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxPackageWriter_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxPackageWriter_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxPackageWriter_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxPackageWriter_AddPayloadFile(This,fileName,contentType,compressionOption,inputStream)  ((This)->lpVtbl->AddPayloadFile(This,fileName,contentType,compressionOption,inputStream)) 
#define IAppxPackageWriter_Close(This,manifest)  ((This)->lpVtbl->Close(This,manifest)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxPackageWriter_INTERFACE_DEFINED__ */

#ifndef __IAppxFile_INTERFACE_DEFINED__
#define __IAppxFile_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxFile;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("91df827b-94fd-468f-827b-57f41b2f6f2e")
IAppxFile:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCompressionOption(APPX_COMPRESSION_OPTION * compressionOption) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetContentType(LPWSTR * contentType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetName(LPWSTR * fileName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSize(UINT64 * size) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStream(IStream ** stream) = 0;
};
#else   /* C style interface */

typedef struct IAppxFileVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxFile * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxFile * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxFile * This);
    HRESULT (STDMETHODCALLTYPE * GetCompressionOption)(IAppxFile * This, APPX_COMPRESSION_OPTION * compressionOption);
    HRESULT (STDMETHODCALLTYPE * GetContentType)(IAppxFile * This, LPWSTR * contentType);
    HRESULT (STDMETHODCALLTYPE * GetName)(IAppxFile * This, LPWSTR * fileName);
    HRESULT (STDMETHODCALLTYPE * GetSize)(IAppxFile * This, UINT64 * size);
    HRESULT (STDMETHODCALLTYPE * GetStream)(IAppxFile * This, IStream ** stream);
    END_INTERFACE
} IAppxFileVtbl;

interface IAppxFile {
    CONST_VTBL struct IAppxFileVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxFile_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxFile_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxFile_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxFile_GetCompressionOption(This,compressionOption)  ((This)->lpVtbl->GetCompressionOption(This,compressionOption)) 
#define IAppxFile_GetContentType(This,contentType)  ((This)->lpVtbl->GetContentType(This,contentType)) 
#define IAppxFile_GetName(This,fileName)  ((This)->lpVtbl->GetName(This,fileName)) 
#define IAppxFile_GetSize(This,size)  ((This)->lpVtbl->GetSize(This,size)) 
#define IAppxFile_GetStream(This,stream)  ((This)->lpVtbl->GetStream(This,stream)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxFile_INTERFACE_DEFINED__ */

#ifndef __IAppxFilesEnumerator_INTERFACE_DEFINED__
#define __IAppxFilesEnumerator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxFilesEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("f007eeaf-9831-411c-9847-917cdc62d1fe")
IAppxFilesEnumerator:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCurrent(IAppxFile ** file) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHasCurrent(BOOL * hasCurrent) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL * hasNext) = 0;
};
#else   /* C style interface */

typedef struct IAppxFilesEnumeratorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxFilesEnumerator * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxFilesEnumerator * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxFilesEnumerator * This);
    HRESULT (STDMETHODCALLTYPE * GetCurrent)(IAppxFilesEnumerator * This, IAppxFile ** file);
    HRESULT (STDMETHODCALLTYPE * GetHasCurrent)(IAppxFilesEnumerator * This, BOOL * hasCurrent);
    HRESULT (STDMETHODCALLTYPE * MoveNext)(IAppxFilesEnumerator * This, BOOL * hasNext);
    END_INTERFACE
} IAppxFilesEnumeratorVtbl;

interface IAppxFilesEnumerator {
    CONST_VTBL struct IAppxFilesEnumeratorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxFilesEnumerator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxFilesEnumerator_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxFilesEnumerator_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxFilesEnumerator_GetCurrent(This,file)  ((This)->lpVtbl->GetCurrent(This,file)) 
#define IAppxFilesEnumerator_GetHasCurrent(This,hasCurrent)  ((This)->lpVtbl->GetHasCurrent(This,hasCurrent)) 
#define IAppxFilesEnumerator_MoveNext(This,hasNext)  ((This)->lpVtbl->MoveNext(This,hasNext)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxFilesEnumerator_INTERFACE_DEFINED__ */

#ifndef __IAppxBlockMapReader_INTERFACE_DEFINED__
#define __IAppxBlockMapReader_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxBlockMapReader;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5efec991-bca3-42d1-9ec2-e92d609ec22a")
IAppxBlockMapReader:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetFile(LPCWSTR filename, IAppxBlockMapFile ** file) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFiles(IAppxBlockMapFilesEnumerator ** enumerator) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHashMethod(IUri ** hashMethod) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStream(IStream ** blockMapStream) = 0;
};
#else   /* C style interface */

typedef struct IAppxBlockMapReaderVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxBlockMapReader * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxBlockMapReader * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxBlockMapReader * This);
    HRESULT (STDMETHODCALLTYPE * GetFile)(IAppxBlockMapReader * This, LPCWSTR filename, IAppxBlockMapFile ** file);
    HRESULT (STDMETHODCALLTYPE * GetFiles)(IAppxBlockMapReader * This, IAppxBlockMapFilesEnumerator ** enumerator);
    HRESULT (STDMETHODCALLTYPE * GetHashMethod)(IAppxBlockMapReader * This, IUri ** hashMethod);
    HRESULT (STDMETHODCALLTYPE * GetStream)(IAppxBlockMapReader * This, IStream ** blockMapStream);
    END_INTERFACE
} IAppxBlockMapReaderVtbl;

interface IAppxBlockMapReader {
    CONST_VTBL struct IAppxBlockMapReaderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxBlockMapReader_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxBlockMapReader_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxBlockMapReader_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxBlockMapReader_GetFile(This,filename,file)  ((This)->lpVtbl->GetFile(This,filename,file)) 
#define IAppxBlockMapReader_GetFiles(This,enumerator)  ((This)->lpVtbl->GetFiles(This,enumerator)) 
#define IAppxBlockMapReader_GetHashMethod(This,hashMethod)  ((This)->lpVtbl->GetHashMethod(This,hashMethod)) 
#define IAppxBlockMapReader_GetStream(This,blockMapStream)  ((This)->lpVtbl->GetStream(This,blockMapStream)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxBlockMapReader_INTERFACE_DEFINED__ */

#ifndef __IAppxBlockMapFile_INTERFACE_DEFINED__
#define __IAppxBlockMapFile_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxBlockMapFile;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("277672ac-4f63-42c1-8abc-beae3600eb59")
IAppxBlockMapFile:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetBlocks(IAppxBlockMapBlocksEnumerator ** blocks) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetLocalFileHeaderSize(UINT32 * lfhSize) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetName(LPWSTR * name) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetUncompressedSize(UINT64 * size) = 0;
    virtual HRESULT STDMETHODCALLTYPE ValidateFileHash(IStream * fileStream, BOOL * isValid) = 0;
};
#else   /* C style interface */

typedef struct IAppxBlockMapFileVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxBlockMapFile * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxBlockMapFile * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxBlockMapFile * This);
    HRESULT (STDMETHODCALLTYPE * GetBlocks)(IAppxBlockMapFile * This, IAppxBlockMapBlocksEnumerator ** blocks);
    HRESULT (STDMETHODCALLTYPE * GetLocalFileHeaderSize)(IAppxBlockMapFile * This, UINT32 * lfhSize);
    HRESULT (STDMETHODCALLTYPE * GetName)(IAppxBlockMapFile * This, LPWSTR * name);
    HRESULT (STDMETHODCALLTYPE * GetUncompressedSize)(IAppxBlockMapFile * This, UINT64 * size);
    HRESULT (STDMETHODCALLTYPE * ValidateFileHash)(IAppxBlockMapFile * This, IStream * fileStream, BOOL * isValid);
    END_INTERFACE
} IAppxBlockMapFileVtbl;

interface IAppxBlockMapFile {
    CONST_VTBL struct IAppxBlockMapFileVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxBlockMapFile_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxBlockMapFile_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxBlockMapFile_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxBlockMapFile_GetBlocks(This,blocks)  ((This)->lpVtbl->GetBlocks(This,blocks)) 
#define IAppxBlockMapFile_GetLocalFileHeaderSize(This,lfhSize)  ((This)->lpVtbl->GetLocalFileHeaderSize(This,lfhSize)) 
#define IAppxBlockMapFile_GetName(This,name)  ((This)->lpVtbl->GetName(This,name)) 
#define IAppxBlockMapFile_GetUncompressedSize(This,size)  ((This)->lpVtbl->GetUncompressedSize(This,size)) 
#define IAppxBlockMapFile_ValidateFileHash(This,fileStream,isValid)  ((This)->lpVtbl->ValidateFileHash(This,fileStream,isValid)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxBlockMapFile_INTERFACE_DEFINED__ */

#ifndef __IAppxBlockMapFilesEnumerator_INTERFACE_DEFINED__
#define __IAppxBlockMapFilesEnumerator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxBlockMapFilesEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("02b856a2-4262-4070-bacb-1a8cbbc42305")
IAppxBlockMapFilesEnumerator:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCurrent(IAppxBlockMapFile ** file) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHasCurrent(BOOL * hasCurrent) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL * hasCurrent) = 0;
};
#else   /* C style interface */

typedef struct IAppxBlockMapFilesEnumeratorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxBlockMapFilesEnumerator * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxBlockMapFilesEnumerator * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxBlockMapFilesEnumerator * This);
    HRESULT (STDMETHODCALLTYPE * GetCurrent)(IAppxBlockMapFilesEnumerator * This, IAppxBlockMapFile ** file);
    HRESULT (STDMETHODCALLTYPE * GetHasCurrent)(IAppxBlockMapFilesEnumerator * This, BOOL * hasCurrent);
    HRESULT (STDMETHODCALLTYPE * MoveNext)(IAppxBlockMapFilesEnumerator * This, BOOL * hasCurrent);
    END_INTERFACE
} IAppxBlockMapFilesEnumeratorVtbl;

interface IAppxBlockMapFilesEnumerator {
    CONST_VTBL struct IAppxBlockMapFilesEnumeratorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxBlockMapFilesEnumerator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxBlockMapFilesEnumerator_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxBlockMapFilesEnumerator_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxBlockMapFilesEnumerator_GetCurrent(This,file)  ((This)->lpVtbl->GetCurrent(This,file)) 
#define IAppxBlockMapFilesEnumerator_GetHasCurrent(This,hasCurrent)  ((This)->lpVtbl->GetHasCurrent(This,hasCurrent)) 
#define IAppxBlockMapFilesEnumerator_MoveNext(This,hasCurrent)  ((This)->lpVtbl->MoveNext(This,hasCurrent)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxBlockMapFilesEnumerator_INTERFACE_DEFINED__ */

#ifndef __IAppxBlockMapBlock_INTERFACE_DEFINED__
#define __IAppxBlockMapBlock_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxBlockMapBlock;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("75cf3930-3244-4fe0-a8c8-e0bcb270b889")
IAppxBlockMapBlock:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetHash(UINT32 * bufferSize, BYTE ** buffer) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCompressedSize(UINT32 * size) = 0;
};
#else   /* C style interface */

typedef struct IAppxBlockMapBlockVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxBlockMapBlock * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxBlockMapBlock * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxBlockMapBlock * This);
    HRESULT (STDMETHODCALLTYPE * GetHash)(IAppxBlockMapBlock * This, UINT32 * bufferSize, BYTE ** buffer);
    HRESULT (STDMETHODCALLTYPE * GetCompressedSize)(IAppxBlockMapBlock * This, UINT32 * size);
    END_INTERFACE
} IAppxBlockMapBlockVtbl;

interface IAppxBlockMapBlock {
    CONST_VTBL struct IAppxBlockMapBlockVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxBlockMapBlock_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxBlockMapBlock_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxBlockMapBlock_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxBlockMapBlock_GetHash(This,bufferSize,buffer)  ((This)->lpVtbl->GetHash(This,bufferSize,buffer)) 
#define IAppxBlockMapBlock_GetCompressedSize(This,size)  ((This)->lpVtbl->GetCompressedSize(This,size)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxBlockMapBlock_INTERFACE_DEFINED__ */

#ifndef __IAppxBlockMapBlocksEnumerator_INTERFACE_DEFINED__
#define __IAppxBlockMapBlocksEnumerator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxBlockMapBlocksEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("6b429b5b-36ef-479e-b9eb-0c1482b49e16")
IAppxBlockMapBlocksEnumerator:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCurrent(IAppxBlockMapBlock ** block) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHasCurrent(BOOL * hasCurrent) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL * hasNext) = 0;
};
#else   /* C style interface */

typedef struct IAppxBlockMapBlocksEnumeratorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxBlockMapBlocksEnumerator * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxBlockMapBlocksEnumerator * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxBlockMapBlocksEnumerator * This);
    HRESULT (STDMETHODCALLTYPE * GetCurrent)(IAppxBlockMapBlocksEnumerator * This, IAppxBlockMapBlock ** block);
    HRESULT (STDMETHODCALLTYPE * GetHasCurrent)(IAppxBlockMapBlocksEnumerator * This, BOOL * hasCurrent);
    HRESULT (STDMETHODCALLTYPE * MoveNext)(IAppxBlockMapBlocksEnumerator * This, BOOL * hasNext);
    END_INTERFACE
} IAppxBlockMapBlocksEnumeratorVtbl;

interface IAppxBlockMapBlocksEnumerator {
    CONST_VTBL struct IAppxBlockMapBlocksEnumeratorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxBlockMapBlocksEnumerator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxBlockMapBlocksEnumerator_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxBlockMapBlocksEnumerator_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxBlockMapBlocksEnumerator_GetCurrent(This,block)  ((This)->lpVtbl->GetCurrent(This,block)) 
#define IAppxBlockMapBlocksEnumerator_GetHasCurrent(This,hasCurrent)  ((This)->lpVtbl->GetHasCurrent(This,hasCurrent)) 
#define IAppxBlockMapBlocksEnumerator_MoveNext(This,hasNext)  ((This)->lpVtbl->MoveNext(This,hasNext)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxBlockMapBlocksEnumerator_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestReader_INTERFACE_DEFINED__
#define __IAppxManifestReader_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestReader;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("4e1bd148-55a0-4480-a3d1-15544710637c")
IAppxManifestReader:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetPackageId(IAppxManifestPackageId ** packageId) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperties(IAppxManifestProperties ** packageProperties) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPackageDependencies(IAppxManifestPackageDependenciesEnumerator ** dependencies) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCapabilities(APPX_CAPABILITIES * capabilities) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResources(IAppxManifestResourcesEnumerator ** resources) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDeviceCapabilities(IAppxManifestDeviceCapabilitiesEnumerator ** deviceCapabilities) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPrerequisite(LPCWSTR name, UINT64 * value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetApplications(IAppxManifestApplicationsEnumerator ** applications) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStream(IStream ** manifestStream) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestReaderVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestReader * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestReader * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestReader * This);
    HRESULT (STDMETHODCALLTYPE * GetPackageId)(IAppxManifestReader * This, IAppxManifestPackageId ** packageId);
    HRESULT (STDMETHODCALLTYPE * GetProperties)(IAppxManifestReader * This, IAppxManifestProperties ** packageProperties);
    HRESULT (STDMETHODCALLTYPE * GetPackageDependencies)(IAppxManifestReader * This, IAppxManifestPackageDependenciesEnumerator ** dependencies);
    HRESULT (STDMETHODCALLTYPE * GetCapabilities)(IAppxManifestReader * This, APPX_CAPABILITIES * capabilities);
    HRESULT (STDMETHODCALLTYPE * GetResources)(IAppxManifestReader * This, IAppxManifestResourcesEnumerator ** resources);
    HRESULT (STDMETHODCALLTYPE * GetDeviceCapabilities)(IAppxManifestReader * This, IAppxManifestDeviceCapabilitiesEnumerator ** deviceCapabilities);
    HRESULT (STDMETHODCALLTYPE * GetPrerequisite)(IAppxManifestReader * This, LPCWSTR name, UINT64 * value);
    HRESULT (STDMETHODCALLTYPE * GetApplications)(IAppxManifestReader * This, IAppxManifestApplicationsEnumerator ** applications);
    HRESULT (STDMETHODCALLTYPE * GetStream)(IAppxManifestReader * This, IStream ** manifestStream);
    END_INTERFACE
} IAppxManifestReaderVtbl;

interface IAppxManifestReader {
    CONST_VTBL struct IAppxManifestReaderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestReader_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestReader_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestReader_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestReader_GetPackageId(This,packageId)  ((This)->lpVtbl->GetPackageId(This,packageId)) 
#define IAppxManifestReader_GetProperties(This,packageProperties)  ((This)->lpVtbl->GetProperties(This,packageProperties)) 
#define IAppxManifestReader_GetPackageDependencies(This,dependencies)  ((This)->lpVtbl->GetPackageDependencies(This,dependencies)) 
#define IAppxManifestReader_GetCapabilities(This,capabilities)  ((This)->lpVtbl->GetCapabilities(This,capabilities)) 
#define IAppxManifestReader_GetResources(This,resources)  ((This)->lpVtbl->GetResources(This,resources)) 
#define IAppxManifestReader_GetDeviceCapabilities(This,deviceCapabilities)  ((This)->lpVtbl->GetDeviceCapabilities(This,deviceCapabilities)) 
#define IAppxManifestReader_GetPrerequisite(This,name,value)  ((This)->lpVtbl->GetPrerequisite(This,name,value)) 
#define IAppxManifestReader_GetApplications(This,applications)  ((This)->lpVtbl->GetApplications(This,applications)) 
#define IAppxManifestReader_GetStream(This,manifestStream)  ((This)->lpVtbl->GetStream(This,manifestStream)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestReader_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestReader2_INTERFACE_DEFINED__
#define __IAppxManifestReader2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestReader2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("d06f67bc-b31d-4eba-a8af-638e73e77b4d")
IAppxManifestReader2:public IAppxManifestReader
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetQualifiedResources(IAppxManifestQualifiedResourcesEnumerator ** resources) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestReader2Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestReader2 * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestReader2 * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestReader2 * This);
    HRESULT (STDMETHODCALLTYPE * GetPackageId)(IAppxManifestReader2 * This, IAppxManifestPackageId ** packageId);
    HRESULT (STDMETHODCALLTYPE * GetProperties)(IAppxManifestReader2 * This, IAppxManifestProperties ** packageProperties);
    HRESULT (STDMETHODCALLTYPE * GetPackageDependencies)(IAppxManifestReader2 * This, IAppxManifestPackageDependenciesEnumerator ** dependencies);
    HRESULT (STDMETHODCALLTYPE * GetCapabilities)(IAppxManifestReader2 * This, APPX_CAPABILITIES * capabilities);
    HRESULT (STDMETHODCALLTYPE * GetResources)(IAppxManifestReader2 * This, IAppxManifestResourcesEnumerator ** resources);
    HRESULT (STDMETHODCALLTYPE * GetDeviceCapabilities)(IAppxManifestReader2 * This, IAppxManifestDeviceCapabilitiesEnumerator ** deviceCapabilities);
    HRESULT (STDMETHODCALLTYPE * GetPrerequisite)(IAppxManifestReader2 * This, LPCWSTR name, UINT64 * value);
    HRESULT (STDMETHODCALLTYPE * GetApplications)(IAppxManifestReader2 * This, IAppxManifestApplicationsEnumerator ** applications);
    HRESULT (STDMETHODCALLTYPE * GetStream)(IAppxManifestReader2 * This, IStream ** manifestStream);
    HRESULT (STDMETHODCALLTYPE * GetQualifiedResources)(IAppxManifestReader2 * This, IAppxManifestQualifiedResourcesEnumerator ** resources);
    END_INTERFACE
} IAppxManifestReader2Vtbl;

interface IAppxManifestReader2 {
    CONST_VTBL struct IAppxManifestReader2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestReader2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestReader2_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestReader2_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestReader2_GetPackageId(This,packageId)  ((This)->lpVtbl->GetPackageId(This,packageId)) 
#define IAppxManifestReader2_GetProperties(This,packageProperties)  ((This)->lpVtbl->GetProperties(This,packageProperties)) 
#define IAppxManifestReader2_GetPackageDependencies(This,dependencies)  ((This)->lpVtbl->GetPackageDependencies(This,dependencies)) 
#define IAppxManifestReader2_GetCapabilities(This,capabilities)  ((This)->lpVtbl->GetCapabilities(This,capabilities)) 
#define IAppxManifestReader2_GetResources(This,resources)  ((This)->lpVtbl->GetResources(This,resources)) 
#define IAppxManifestReader2_GetDeviceCapabilities(This,deviceCapabilities)  ((This)->lpVtbl->GetDeviceCapabilities(This,deviceCapabilities)) 
#define IAppxManifestReader2_GetPrerequisite(This,name,value)  ((This)->lpVtbl->GetPrerequisite(This,name,value)) 
#define IAppxManifestReader2_GetApplications(This,applications)  ((This)->lpVtbl->GetApplications(This,applications)) 
#define IAppxManifestReader2_GetStream(This,manifestStream)  ((This)->lpVtbl->GetStream(This,manifestStream)) 
#define IAppxManifestReader2_GetQualifiedResources(This,resources)   ((This)->lpVtbl->GetQualifiedResources(This,resources)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestReader2_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestReader3_INTERFACE_DEFINED__
#define __IAppxManifestReader3_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestReader3;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("C43825AB-69B7-400A-9709-CC37F5A72D24")
IAppxManifestReader3:public IAppxManifestReader2
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCapabilitiesByCapabilityClass(APPX_CAPABILITY_CLASS_TYPE capabilityClass, IAppxManifestCapabilitiesEnumerator ** capabilities) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTargetDeviceFamilies(IAppxManifestTargetDeviceFamiliesEnumerator ** targetDeviceFamilies) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestReader3Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestReader3 * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestReader3 * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestReader3 * This);
    HRESULT (STDMETHODCALLTYPE * GetPackageId)(IAppxManifestReader3 * This, IAppxManifestPackageId ** packageId);
    HRESULT (STDMETHODCALLTYPE * GetProperties)(IAppxManifestReader3 * This, IAppxManifestProperties ** packageProperties);
    HRESULT (STDMETHODCALLTYPE * GetPackageDependencies)(IAppxManifestReader3 * This, IAppxManifestPackageDependenciesEnumerator ** dependencies);
    HRESULT (STDMETHODCALLTYPE * GetCapabilities)(IAppxManifestReader3 * This, APPX_CAPABILITIES * capabilities);
    HRESULT (STDMETHODCALLTYPE * GetResources)(IAppxManifestReader3 * This, IAppxManifestResourcesEnumerator ** resources);
    HRESULT (STDMETHODCALLTYPE * GetDeviceCapabilities)(IAppxManifestReader3 * This, IAppxManifestDeviceCapabilitiesEnumerator ** deviceCapabilities);
    HRESULT (STDMETHODCALLTYPE * GetPrerequisite)(IAppxManifestReader3 * This, LPCWSTR name, UINT64 * value);
    HRESULT (STDMETHODCALLTYPE * GetApplications)(IAppxManifestReader3 * This, IAppxManifestApplicationsEnumerator ** applications);
    HRESULT (STDMETHODCALLTYPE * GetStream)(IAppxManifestReader3 * This, IStream ** manifestStream);
    HRESULT (STDMETHODCALLTYPE * GetQualifiedResources)(IAppxManifestReader3 * This, IAppxManifestQualifiedResourcesEnumerator ** resources);
    HRESULT (STDMETHODCALLTYPE * GetCapabilitiesByCapabilityClass)(IAppxManifestReader3 * This, APPX_CAPABILITY_CLASS_TYPE capabilityClass, IAppxManifestCapabilitiesEnumerator ** capabilities);
    HRESULT (STDMETHODCALLTYPE * GetTargetDeviceFamilies)(IAppxManifestReader3 * This, IAppxManifestTargetDeviceFamiliesEnumerator ** targetDeviceFamilies);
    END_INTERFACE
} IAppxManifestReader3Vtbl;

interface IAppxManifestReader3 {
    CONST_VTBL struct IAppxManifestReader3Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestReader3_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestReader3_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestReader3_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestReader3_GetPackageId(This,packageId)  ((This)->lpVtbl->GetPackageId(This,packageId)) 
#define IAppxManifestReader3_GetProperties(This,packageProperties)  ((This)->lpVtbl->GetProperties(This,packageProperties)) 
#define IAppxManifestReader3_GetPackageDependencies(This,dependencies)  ((This)->lpVtbl->GetPackageDependencies(This,dependencies)) 
#define IAppxManifestReader3_GetCapabilities(This,capabilities)  ((This)->lpVtbl->GetCapabilities(This,capabilities)) 
#define IAppxManifestReader3_GetResources(This,resources)  ((This)->lpVtbl->GetResources(This,resources)) 
#define IAppxManifestReader3_GetDeviceCapabilities(This,deviceCapabilities)  ((This)->lpVtbl->GetDeviceCapabilities(This,deviceCapabilities)) 
#define IAppxManifestReader3_GetPrerequisite(This,name,value)  ((This)->lpVtbl->GetPrerequisite(This,name,value)) 
#define IAppxManifestReader3_GetApplications(This,applications)  ((This)->lpVtbl->GetApplications(This,applications)) 
#define IAppxManifestReader3_GetStream(This,manifestStream)  ((This)->lpVtbl->GetStream(This,manifestStream)) 
#define IAppxManifestReader3_GetQualifiedResources(This,resources)  ((This)->lpVtbl->GetQualifiedResources(This,resources)) 
#define IAppxManifestReader3_GetCapabilitiesByCapabilityClass(This,capabilityClass,capabilities)  ((This)->lpVtbl->GetCapabilitiesByCapabilityClass(This,capabilityClass,capabilities)) 
#define IAppxManifestReader3_GetTargetDeviceFamilies(This,targetDeviceFamilies)  ((This)->lpVtbl->GetTargetDeviceFamilies(This,targetDeviceFamilies)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestReader3_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestReader4_INTERFACE_DEFINED__
#define __IAppxManifestReader4_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestReader4;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("4579BB7C-741D-4161-B5A1-47BD3B78AD9B")
IAppxManifestReader4:public IAppxManifestReader3
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetOptionalPackageInfo(IAppxManifestOptionalPackageInfo ** optionalPackageInfo) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestReader4Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestReader4 * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestReader4 * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestReader4 * This);
    HRESULT (STDMETHODCALLTYPE * GetPackageId)(IAppxManifestReader4 * This, IAppxManifestPackageId ** packageId);
    HRESULT (STDMETHODCALLTYPE * GetProperties)(IAppxManifestReader4 * This, IAppxManifestProperties ** packageProperties);
    HRESULT (STDMETHODCALLTYPE * GetPackageDependencies)(IAppxManifestReader4 * This, IAppxManifestPackageDependenciesEnumerator ** dependencies);
    HRESULT (STDMETHODCALLTYPE * GetCapabilities)(IAppxManifestReader4 * This, APPX_CAPABILITIES * capabilities);
    HRESULT (STDMETHODCALLTYPE * GetResources)(IAppxManifestReader4 * This, IAppxManifestResourcesEnumerator ** resources);
    HRESULT (STDMETHODCALLTYPE * GetDeviceCapabilities)(IAppxManifestReader4 * This, IAppxManifestDeviceCapabilitiesEnumerator ** deviceCapabilities);
    HRESULT (STDMETHODCALLTYPE * GetPrerequisite)(IAppxManifestReader4 * This, LPCWSTR name, UINT64 * value);
    HRESULT (STDMETHODCALLTYPE * GetApplications)(IAppxManifestReader4 * This, IAppxManifestApplicationsEnumerator ** applications);
    HRESULT (STDMETHODCALLTYPE * GetStream)(IAppxManifestReader4 * This, IStream ** manifestStream);
    HRESULT (STDMETHODCALLTYPE * GetQualifiedResources)(IAppxManifestReader4 * This, IAppxManifestQualifiedResourcesEnumerator ** resources);
    HRESULT (STDMETHODCALLTYPE * GetCapabilitiesByCapabilityClass)(IAppxManifestReader4 * This, APPX_CAPABILITY_CLASS_TYPE capabilityClass, IAppxManifestCapabilitiesEnumerator ** capabilities);
    HRESULT (STDMETHODCALLTYPE * GetTargetDeviceFamilies)(IAppxManifestReader4 * This, IAppxManifestTargetDeviceFamiliesEnumerator ** targetDeviceFamilies);
    HRESULT (STDMETHODCALLTYPE * GetOptionalPackageInfo)(IAppxManifestReader4 * This, IAppxManifestOptionalPackageInfo ** optionalPackageInfo);
    END_INTERFACE
} IAppxManifestReader4Vtbl;

interface IAppxManifestReader4 {
    CONST_VTBL struct IAppxManifestReader4Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestReader4_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestReader4_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestReader4_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestReader4_GetPackageId(This,packageId)  ((This)->lpVtbl->GetPackageId(This,packageId)) 
#define IAppxManifestReader4_GetProperties(This,packageProperties)  ((This)->lpVtbl->GetProperties(This,packageProperties)) 
#define IAppxManifestReader4_GetPackageDependencies(This,dependencies)  ((This)->lpVtbl->GetPackageDependencies(This,dependencies)) 
#define IAppxManifestReader4_GetCapabilities(This,capabilities)  ((This)->lpVtbl->GetCapabilities(This,capabilities)) 
#define IAppxManifestReader4_GetResources(This,resources)  ((This)->lpVtbl->GetResources(This,resources)) 
#define IAppxManifestReader4_GetDeviceCapabilities(This,deviceCapabilities)  ((This)->lpVtbl->GetDeviceCapabilities(This,deviceCapabilities)) 
#define IAppxManifestReader4_GetPrerequisite(This,name,value)  ((This)->lpVtbl->GetPrerequisite(This,name,value)) 
#define IAppxManifestReader4_GetApplications(This,applications)  ((This)->lpVtbl->GetApplications(This,applications)) 
#define IAppxManifestReader4_GetStream(This,manifestStream)  ((This)->lpVtbl->GetStream(This,manifestStream)) 
#define IAppxManifestReader4_GetQualifiedResources(This,resources)  ((This)->lpVtbl->GetQualifiedResources(This,resources)) 
#define IAppxManifestReader4_GetCapabilitiesByCapabilityClass(This,capabilityClass,capabilities)  ((This)->lpVtbl->GetCapabilitiesByCapabilityClass(This,capabilityClass,capabilities)) 
#define IAppxManifestReader4_GetTargetDeviceFamilies(This,targetDeviceFamilies)  ((This)->lpVtbl->GetTargetDeviceFamilies(This,targetDeviceFamilies)) 
#define IAppxManifestReader4_GetOptionalPackageInfo(This,optionalPackageInfo)  ((This)->lpVtbl->GetOptionalPackageInfo(This,optionalPackageInfo)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestReader4_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestOptionalPackageInfo_INTERFACE_DEFINED__
#define __IAppxManifestOptionalPackageInfo_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestOptionalPackageInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("2634847D-5B5D-4FE5-A243-002FF95EDC7E")
IAppxManifestOptionalPackageInfo:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetIsOptionalPackage(BOOL * isOptionalPackage) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMainPackageName(LPWSTR * mainPackageName) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestOptionalPackageInfoVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestOptionalPackageInfo * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestOptionalPackageInfo * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestOptionalPackageInfo * This);
    HRESULT (STDMETHODCALLTYPE * GetIsOptionalPackage)(IAppxManifestOptionalPackageInfo * This, BOOL * isOptionalPackage);
    HRESULT (STDMETHODCALLTYPE * GetMainPackageName)(IAppxManifestOptionalPackageInfo * This, LPWSTR * mainPackageName);
    END_INTERFACE
} IAppxManifestOptionalPackageInfoVtbl;

interface IAppxManifestOptionalPackageInfo {
    CONST_VTBL struct IAppxManifestOptionalPackageInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestOptionalPackageInfo_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestOptionalPackageInfo_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestOptionalPackageInfo_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestOptionalPackageInfo_GetIsOptionalPackage(This,isOptionalPackage)  ((This)->lpVtbl->GetIsOptionalPackage(This,isOptionalPackage)) 
#define IAppxManifestOptionalPackageInfo_GetMainPackageName(This,mainPackageName)  ((This)->lpVtbl->GetMainPackageName(This,mainPackageName)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestOptionalPackageInfo_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestPackageId_INTERFACE_DEFINED__
#define __IAppxManifestPackageId_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestPackageId;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("283ce2d7-7153-4a91-9649-7a0f7240945f")
IAppxManifestPackageId:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetName(LPWSTR * name) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetArchitecture(APPX_PACKAGE_ARCHITECTURE * architecture) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPublisher(LPWSTR * publisher) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetVersion(UINT64 * packageVersion) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResourceId(LPWSTR * resourceId) = 0;
    virtual HRESULT STDMETHODCALLTYPE ComparePublisher(LPCWSTR other, BOOL * isSame) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPackageFullName(LPWSTR * packageFullName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPackageFamilyName(LPWSTR * packageFamilyName) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestPackageIdVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestPackageId * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestPackageId * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestPackageId * This);
    HRESULT (STDMETHODCALLTYPE * GetName)(IAppxManifestPackageId * This, LPWSTR * name);
    HRESULT (STDMETHODCALLTYPE * GetArchitecture)(IAppxManifestPackageId * This, APPX_PACKAGE_ARCHITECTURE * architecture);
    HRESULT (STDMETHODCALLTYPE * GetPublisher)(IAppxManifestPackageId * This, LPWSTR * publisher);
    HRESULT (STDMETHODCALLTYPE * GetVersion)(IAppxManifestPackageId * This, UINT64 * packageVersion);
    HRESULT (STDMETHODCALLTYPE * GetResourceId)(IAppxManifestPackageId * This, LPWSTR * resourceId);
    HRESULT (STDMETHODCALLTYPE * ComparePublisher)(IAppxManifestPackageId * This, LPCWSTR other, BOOL * isSame);
    HRESULT (STDMETHODCALLTYPE * GetPackageFullName)(IAppxManifestPackageId * This, LPWSTR * packageFullName);
    HRESULT (STDMETHODCALLTYPE * GetPackageFamilyName)(IAppxManifestPackageId * This, LPWSTR * packageFamilyName);
    END_INTERFACE
} IAppxManifestPackageIdVtbl;

interface IAppxManifestPackageId {
    CONST_VTBL struct IAppxManifestPackageIdVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestPackageId_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestPackageId_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestPackageId_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestPackageId_GetName(This,name)  ((This)->lpVtbl->GetName(This,name)) 
#define IAppxManifestPackageId_GetArchitecture(This,architecture)  ((This)->lpVtbl->GetArchitecture(This,architecture)) 
#define IAppxManifestPackageId_GetPublisher(This,publisher)  ((This)->lpVtbl->GetPublisher(This,publisher)) 
#define IAppxManifestPackageId_GetVersion(This,packageVersion)  ((This)->lpVtbl->GetVersion(This,packageVersion)) 
#define IAppxManifestPackageId_GetResourceId(This,resourceId)  ((This)->lpVtbl->GetResourceId(This,resourceId)) 
#define IAppxManifestPackageId_ComparePublisher(This,other,isSame)  ((This)->lpVtbl->ComparePublisher(This,other,isSame)) 
#define IAppxManifestPackageId_GetPackageFullName(This,packageFullName)  ((This)->lpVtbl->GetPackageFullName(This,packageFullName)) 
#define IAppxManifestPackageId_GetPackageFamilyName(This,packageFamilyName)  ((This)->lpVtbl->GetPackageFamilyName(This,packageFamilyName)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestPackageId_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestProperties_INTERFACE_DEFINED__
#define __IAppxManifestProperties_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestProperties;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("03faf64d-f26f-4b2c-aaf7-8fe7789b8bca")
IAppxManifestProperties:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetBoolValue(LPCWSTR name, BOOL * value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStringValue(LPCWSTR name, LPWSTR * value) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestPropertiesVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestProperties * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestProperties * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestProperties * This);
    HRESULT (STDMETHODCALLTYPE * GetBoolValue)(IAppxManifestProperties * This, LPCWSTR name, BOOL * value);
    HRESULT (STDMETHODCALLTYPE * GetStringValue)(IAppxManifestProperties * This, LPCWSTR name, LPWSTR * value);
    END_INTERFACE
} IAppxManifestPropertiesVtbl;

interface IAppxManifestProperties {
    CONST_VTBL struct IAppxManifestPropertiesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestProperties_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestProperties_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestProperties_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestProperties_GetBoolValue(This,name,value)  ((This)->lpVtbl->GetBoolValue(This,name,value)) 
#define IAppxManifestProperties_GetStringValue(This,name,value)  ((This)->lpVtbl->GetStringValue(This,name,value)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestProperties_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestTargetDeviceFamiliesEnumerator_INTERFACE_DEFINED__
#define __IAppxManifestTargetDeviceFamiliesEnumerator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestTargetDeviceFamiliesEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("36537F36-27A4-4788-88C0-733819575017")
IAppxManifestTargetDeviceFamiliesEnumerator:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCurrent(IAppxManifestTargetDeviceFamily ** targetDeviceFamily) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHasCurrent(BOOL * hasCurrent) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL * hasNext) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestTargetDeviceFamiliesEnumeratorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestTargetDeviceFamiliesEnumerator * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestTargetDeviceFamiliesEnumerator * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestTargetDeviceFamiliesEnumerator * This);
    HRESULT (STDMETHODCALLTYPE * GetCurrent)(IAppxManifestTargetDeviceFamiliesEnumerator * This, IAppxManifestTargetDeviceFamily ** targetDeviceFamily);
    HRESULT (STDMETHODCALLTYPE * GetHasCurrent)(IAppxManifestTargetDeviceFamiliesEnumerator * This, BOOL * hasCurrent);
    HRESULT (STDMETHODCALLTYPE * MoveNext)(IAppxManifestTargetDeviceFamiliesEnumerator * This, BOOL * hasNext);
    END_INTERFACE
} IAppxManifestTargetDeviceFamiliesEnumeratorVtbl;

interface IAppxManifestTargetDeviceFamiliesEnumerator {
    CONST_VTBL struct IAppxManifestTargetDeviceFamiliesEnumeratorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestTargetDeviceFamiliesEnumerator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestTargetDeviceFamiliesEnumerator_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestTargetDeviceFamiliesEnumerator_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestTargetDeviceFamiliesEnumerator_GetCurrent(This,targetDeviceFamily)  ((This)->lpVtbl->GetCurrent(This,targetDeviceFamily)) 
#define IAppxManifestTargetDeviceFamiliesEnumerator_GetHasCurrent(This,hasCurrent)  ((This)->lpVtbl->GetHasCurrent(This,hasCurrent)) 
#define IAppxManifestTargetDeviceFamiliesEnumerator_MoveNext(This,hasNext)  ((This)->lpVtbl->MoveNext(This,hasNext)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestTargetDeviceFamiliesEnumerator_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestTargetDeviceFamily_INTERFACE_DEFINED__
#define __IAppxManifestTargetDeviceFamily_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestTargetDeviceFamily;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("9091B09B-C8D5-4F31-8687-A338259FAEFB")
IAppxManifestTargetDeviceFamily:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetName(LPWSTR * name) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMinVersion(UINT64 * minVersion) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMaxVersionTested(UINT64 * maxVersionTested) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestTargetDeviceFamilyVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestTargetDeviceFamily * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestTargetDeviceFamily * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestTargetDeviceFamily * This);
    HRESULT (STDMETHODCALLTYPE * GetName)(IAppxManifestTargetDeviceFamily * This, LPWSTR * name);
    HRESULT (STDMETHODCALLTYPE * GetMinVersion)(IAppxManifestTargetDeviceFamily * This, UINT64 * minVersion);
    HRESULT (STDMETHODCALLTYPE * GetMaxVersionTested)(IAppxManifestTargetDeviceFamily * This, UINT64 * maxVersionTested);
    END_INTERFACE
} IAppxManifestTargetDeviceFamilyVtbl;

interface IAppxManifestTargetDeviceFamily {
    CONST_VTBL struct IAppxManifestTargetDeviceFamilyVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestTargetDeviceFamily_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestTargetDeviceFamily_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestTargetDeviceFamily_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestTargetDeviceFamily_GetName(This,name)  ((This)->lpVtbl->GetName(This,name)) 
#define IAppxManifestTargetDeviceFamily_GetMinVersion(This,minVersion)  ((This)->lpVtbl->GetMinVersion(This,minVersion)) 
#define IAppxManifestTargetDeviceFamily_GetMaxVersionTested(This,maxVersionTested)  ((This)->lpVtbl->GetMaxVersionTested(This,maxVersionTested)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestTargetDeviceFamily_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestPackageDependenciesEnumerator_INTERFACE_DEFINED__
#define __IAppxManifestPackageDependenciesEnumerator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestPackageDependenciesEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("b43bbcf9-65a6-42dd-bac0-8c6741e7f5a4")
IAppxManifestPackageDependenciesEnumerator:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCurrent(IAppxManifestPackageDependency ** dependency) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHasCurrent(BOOL * hasCurrent) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL * hasNext) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestPackageDependenciesEnumeratorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestPackageDependenciesEnumerator * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestPackageDependenciesEnumerator * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestPackageDependenciesEnumerator * This);
    HRESULT (STDMETHODCALLTYPE * GetCurrent)(IAppxManifestPackageDependenciesEnumerator * This, IAppxManifestPackageDependency ** dependency);
    HRESULT (STDMETHODCALLTYPE * GetHasCurrent)(IAppxManifestPackageDependenciesEnumerator * This, BOOL * hasCurrent);
    HRESULT (STDMETHODCALLTYPE * MoveNext)(IAppxManifestPackageDependenciesEnumerator * This, BOOL * hasNext);
    END_INTERFACE
} IAppxManifestPackageDependenciesEnumeratorVtbl;

interface IAppxManifestPackageDependenciesEnumerator {
    CONST_VTBL struct IAppxManifestPackageDependenciesEnumeratorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestPackageDependenciesEnumerator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestPackageDependenciesEnumerator_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestPackageDependenciesEnumerator_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestPackageDependenciesEnumerator_GetCurrent(This,dependency)  ((This)->lpVtbl->GetCurrent(This,dependency)) 
#define IAppxManifestPackageDependenciesEnumerator_GetHasCurrent(This,hasCurrent)  ((This)->lpVtbl->GetHasCurrent(This,hasCurrent)) 
#define IAppxManifestPackageDependenciesEnumerator_MoveNext(This,hasNext)  ((This)->lpVtbl->MoveNext(This,hasNext)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestPackageDependenciesEnumerator_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestPackageDependency_INTERFACE_DEFINED__
#define __IAppxManifestPackageDependency_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestPackageDependency;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("e4946b59-733e-43f0-a724-3bde4c1285a0")
IAppxManifestPackageDependency:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetName(LPWSTR * name) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPublisher(LPWSTR * publisher) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetMinVersion(UINT64 * minVersion) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestPackageDependencyVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestPackageDependency * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestPackageDependency * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestPackageDependency * This);
    HRESULT (STDMETHODCALLTYPE * GetName)(IAppxManifestPackageDependency * This, LPWSTR * name);
    HRESULT (STDMETHODCALLTYPE * GetPublisher)(IAppxManifestPackageDependency * This, LPWSTR * publisher);
    HRESULT (STDMETHODCALLTYPE * GetMinVersion)(IAppxManifestPackageDependency * This, UINT64 * minVersion);
    END_INTERFACE
} IAppxManifestPackageDependencyVtbl;

interface IAppxManifestPackageDependency {
    CONST_VTBL struct IAppxManifestPackageDependencyVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestPackageDependency_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestPackageDependency_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestPackageDependency_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestPackageDependency_GetName(This,name)  ((This)->lpVtbl->GetName(This,name)) 
#define IAppxManifestPackageDependency_GetPublisher(This,publisher)  ((This)->lpVtbl->GetPublisher(This,publisher)) 
#define IAppxManifestPackageDependency_GetMinVersion(This,minVersion)  ((This)->lpVtbl->GetMinVersion(This,minVersion)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestPackageDependency_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestPackageDependency2_INTERFACE_DEFINED__
#define __IAppxManifestPackageDependency2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestPackageDependency2;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("DDA0B713-F3FF-49D3-898A-2786780C5D98")
IAppxManifestPackageDependency2:public IAppxManifestPackageDependency
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetMaxMajorVersionTested(UINT16 * maxMajorVersionTested) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestPackageDependency2Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestPackageDependency2 * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestPackageDependency2 * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestPackageDependency2 * This);
    HRESULT (STDMETHODCALLTYPE * GetName)(IAppxManifestPackageDependency2 * This, LPWSTR * name);
    HRESULT (STDMETHODCALLTYPE * GetPublisher)(IAppxManifestPackageDependency2 * This, LPWSTR * publisher);
    HRESULT (STDMETHODCALLTYPE * GetMinVersion)(IAppxManifestPackageDependency2 * This, UINT64 * minVersion);
    HRESULT (STDMETHODCALLTYPE * GetMaxMajorVersionTested)(IAppxManifestPackageDependency2 * This, UINT16 * maxMajorVersionTested);
    END_INTERFACE
} IAppxManifestPackageDependency2Vtbl;

interface IAppxManifestPackageDependency2 {
    CONST_VTBL struct IAppxManifestPackageDependency2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestPackageDependency2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestPackageDependency2_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestPackageDependency2_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestPackageDependency2_GetName(This,name)  ((This)->lpVtbl->GetName(This,name)) 
#define IAppxManifestPackageDependency2_GetPublisher(This,publisher)  ((This)->lpVtbl->GetPublisher(This,publisher)) 
#define IAppxManifestPackageDependency2_GetMinVersion(This,minVersion)  ((This)->lpVtbl->GetMinVersion(This,minVersion)) 
#define IAppxManifestPackageDependency2_GetMaxMajorVersionTested(This,maxMajorVersionTested)  ((This)->lpVtbl->GetMaxMajorVersionTested(This,maxMajorVersionTested)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestPackageDependency2_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestResourcesEnumerator_INTERFACE_DEFINED__
#define __IAppxManifestResourcesEnumerator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestResourcesEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("de4dfbbd-881a-48bb-858c-d6f2baeae6ed")
IAppxManifestResourcesEnumerator:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCurrent(LPWSTR * resource) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHasCurrent(BOOL * hasCurrent) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL * hasNext) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestResourcesEnumeratorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestResourcesEnumerator * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestResourcesEnumerator * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestResourcesEnumerator * This);
    HRESULT (STDMETHODCALLTYPE * GetCurrent)(IAppxManifestResourcesEnumerator * This, LPWSTR * resource);
    HRESULT (STDMETHODCALLTYPE * GetHasCurrent)(IAppxManifestResourcesEnumerator * This, BOOL * hasCurrent);
    HRESULT (STDMETHODCALLTYPE * MoveNext)(IAppxManifestResourcesEnumerator * This, BOOL * hasNext);
    END_INTERFACE
} IAppxManifestResourcesEnumeratorVtbl;

interface IAppxManifestResourcesEnumerator {
    CONST_VTBL struct IAppxManifestResourcesEnumeratorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestResourcesEnumerator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestResourcesEnumerator_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestResourcesEnumerator_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestResourcesEnumerator_GetCurrent(This,resource)  ((This)->lpVtbl->GetCurrent(This,resource)) 
#define IAppxManifestResourcesEnumerator_GetHasCurrent(This,hasCurrent)  ((This)->lpVtbl->GetHasCurrent(This,hasCurrent)) 
#define IAppxManifestResourcesEnumerator_MoveNext(This,hasNext)  ((This)->lpVtbl->MoveNext(This,hasNext)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestResourcesEnumerator_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestDeviceCapabilitiesEnumerator_INTERFACE_DEFINED__
#define __IAppxManifestDeviceCapabilitiesEnumerator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestDeviceCapabilitiesEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("30204541-427b-4a1c-bacf-655bf463a540")
IAppxManifestDeviceCapabilitiesEnumerator:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCurrent(LPWSTR * deviceCapability) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHasCurrent(BOOL * hasCurrent) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL * hasNext) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestDeviceCapabilitiesEnumeratorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestDeviceCapabilitiesEnumerator * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestDeviceCapabilitiesEnumerator * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestDeviceCapabilitiesEnumerator * This);
    HRESULT (STDMETHODCALLTYPE * GetCurrent)(IAppxManifestDeviceCapabilitiesEnumerator * This, LPWSTR * deviceCapability);
    HRESULT (STDMETHODCALLTYPE * GetHasCurrent)(IAppxManifestDeviceCapabilitiesEnumerator * This, BOOL * hasCurrent);
    HRESULT (STDMETHODCALLTYPE * MoveNext)(IAppxManifestDeviceCapabilitiesEnumerator * This, BOOL * hasNext);
    END_INTERFACE
} IAppxManifestDeviceCapabilitiesEnumeratorVtbl;

interface IAppxManifestDeviceCapabilitiesEnumerator {
    CONST_VTBL struct IAppxManifestDeviceCapabilitiesEnumeratorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestDeviceCapabilitiesEnumerator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestDeviceCapabilitiesEnumerator_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestDeviceCapabilitiesEnumerator_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestDeviceCapabilitiesEnumerator_GetCurrent(This,deviceCapability)  ((This)->lpVtbl->GetCurrent(This,deviceCapability)) 
#define IAppxManifestDeviceCapabilitiesEnumerator_GetHasCurrent(This,hasCurrent)  ((This)->lpVtbl->GetHasCurrent(This,hasCurrent)) 
#define IAppxManifestDeviceCapabilitiesEnumerator_MoveNext(This,hasNext)  ((This)->lpVtbl->MoveNext(This,hasNext)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestDeviceCapabilitiesEnumerator_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestCapabilitiesEnumerator_INTERFACE_DEFINED__
#define __IAppxManifestCapabilitiesEnumerator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestCapabilitiesEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("11D22258-F470-42C1-B291-8361C5437E41")
IAppxManifestCapabilitiesEnumerator:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCurrent(LPWSTR * capability) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHasCurrent(BOOL * hasCurrent) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL * hasNext) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestCapabilitiesEnumeratorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestCapabilitiesEnumerator * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestCapabilitiesEnumerator * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestCapabilitiesEnumerator * This);
    HRESULT (STDMETHODCALLTYPE * GetCurrent)(IAppxManifestCapabilitiesEnumerator * This, LPWSTR * capability);
    HRESULT (STDMETHODCALLTYPE * GetHasCurrent)(IAppxManifestCapabilitiesEnumerator * This, BOOL * hasCurrent);
    HRESULT (STDMETHODCALLTYPE * MoveNext)(IAppxManifestCapabilitiesEnumerator * This, BOOL * hasNext);
    END_INTERFACE
} IAppxManifestCapabilitiesEnumeratorVtbl;

interface IAppxManifestCapabilitiesEnumerator {
    CONST_VTBL struct IAppxManifestCapabilitiesEnumeratorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestCapabilitiesEnumerator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestCapabilitiesEnumerator_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestCapabilitiesEnumerator_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestCapabilitiesEnumerator_GetCurrent(This,capability)  ((This)->lpVtbl->GetCurrent(This,capability)) 
#define IAppxManifestCapabilitiesEnumerator_GetHasCurrent(This,hasCurrent)  ((This)->lpVtbl->GetHasCurrent(This,hasCurrent)) 
#define IAppxManifestCapabilitiesEnumerator_MoveNext(This,hasNext)  ((This)->lpVtbl->MoveNext(This,hasNext)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestCapabilitiesEnumerator_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestApplicationsEnumerator_INTERFACE_DEFINED__
#define __IAppxManifestApplicationsEnumerator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestApplicationsEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("9eb8a55a-f04b-4d0d-808d-686185d4847a")
IAppxManifestApplicationsEnumerator:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCurrent(IAppxManifestApplication ** application) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHasCurrent(BOOL * hasCurrent) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL * hasNext) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestApplicationsEnumeratorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestApplicationsEnumerator * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestApplicationsEnumerator * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestApplicationsEnumerator * This);
    HRESULT (STDMETHODCALLTYPE * GetCurrent)(IAppxManifestApplicationsEnumerator * This, IAppxManifestApplication ** application);
    HRESULT (STDMETHODCALLTYPE * GetHasCurrent)(IAppxManifestApplicationsEnumerator * This, BOOL * hasCurrent);
    HRESULT (STDMETHODCALLTYPE * MoveNext)(IAppxManifestApplicationsEnumerator * This, BOOL * hasNext);
    END_INTERFACE
} IAppxManifestApplicationsEnumeratorVtbl;

interface IAppxManifestApplicationsEnumerator {
    CONST_VTBL struct IAppxManifestApplicationsEnumeratorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestApplicationsEnumerator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestApplicationsEnumerator_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestApplicationsEnumerator_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestApplicationsEnumerator_GetCurrent(This,application)  ((This)->lpVtbl->GetCurrent(This,application)) 
#define IAppxManifestApplicationsEnumerator_GetHasCurrent(This,hasCurrent)  ((This)->lpVtbl->GetHasCurrent(This,hasCurrent)) 
#define IAppxManifestApplicationsEnumerator_MoveNext(This,hasNext)  ((This)->lpVtbl->MoveNext(This,hasNext)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestApplicationsEnumerator_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestApplication_INTERFACE_DEFINED__
#define __IAppxManifestApplication_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestApplication;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5da89bf4-3773-46be-b650-7e744863b7e8")
IAppxManifestApplication:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetStringValue(LPCWSTR name, LPWSTR * value) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAppUserModelId(LPWSTR * appUserModelId) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestApplicationVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestApplication * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestApplication * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestApplication * This);
    HRESULT (STDMETHODCALLTYPE * GetStringValue)(IAppxManifestApplication * This, LPCWSTR name, LPWSTR * value);
    HRESULT (STDMETHODCALLTYPE * GetAppUserModelId)(IAppxManifestApplication * This, LPWSTR * appUserModelId);
    END_INTERFACE
} IAppxManifestApplicationVtbl;

interface IAppxManifestApplication {
    CONST_VTBL struct IAppxManifestApplicationVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestApplication_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestApplication_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestApplication_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestApplication_GetStringValue(This,name,value)  ((This)->lpVtbl->GetStringValue(This,name,value)) 
#define IAppxManifestApplication_GetAppUserModelId(This,appUserModelId)  ((This)->lpVtbl->GetAppUserModelId(This,appUserModelId)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestApplication_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestQualifiedResourcesEnumerator_INTERFACE_DEFINED__
#define __IAppxManifestQualifiedResourcesEnumerator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestQualifiedResourcesEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("8ef6adfe-3762-4a8f-9373-2fc5d444c8d2")
IAppxManifestQualifiedResourcesEnumerator:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCurrent(IAppxManifestQualifiedResource ** resource) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHasCurrent(BOOL * hasCurrent) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL * hasNext) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestQualifiedResourcesEnumeratorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestQualifiedResourcesEnumerator * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestQualifiedResourcesEnumerator * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestQualifiedResourcesEnumerator * This);
    HRESULT (STDMETHODCALLTYPE * GetCurrent)(IAppxManifestQualifiedResourcesEnumerator * This, IAppxManifestQualifiedResource ** resource);
    HRESULT (STDMETHODCALLTYPE * GetHasCurrent)(IAppxManifestQualifiedResourcesEnumerator * This, BOOL * hasCurrent);
    HRESULT (STDMETHODCALLTYPE * MoveNext)(IAppxManifestQualifiedResourcesEnumerator * This, BOOL * hasNext);
    END_INTERFACE
} IAppxManifestQualifiedResourcesEnumeratorVtbl;

interface IAppxManifestQualifiedResourcesEnumerator {
    CONST_VTBL struct IAppxManifestQualifiedResourcesEnumeratorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestQualifiedResourcesEnumerator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestQualifiedResourcesEnumerator_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestQualifiedResourcesEnumerator_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestQualifiedResourcesEnumerator_GetCurrent(This,resource)  ((This)->lpVtbl->GetCurrent(This,resource)) 
#define IAppxManifestQualifiedResourcesEnumerator_GetHasCurrent(This,hasCurrent)  ((This)->lpVtbl->GetHasCurrent(This,hasCurrent)) 
#define IAppxManifestQualifiedResourcesEnumerator_MoveNext(This,hasNext)  ((This)->lpVtbl->MoveNext(This,hasNext)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestQualifiedResourcesEnumerator_INTERFACE_DEFINED__ */

#ifndef __IAppxManifestQualifiedResource_INTERFACE_DEFINED__
#define __IAppxManifestQualifiedResource_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxManifestQualifiedResource;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3b53a497-3c5c-48d1-9ea3-bb7eac8cd7d4")
IAppxManifestQualifiedResource:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetLanguage(LPWSTR * language) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetScale(UINT32 * scale) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDXFeatureLevel(DX_FEATURE_LEVEL * dxFeatureLevel) = 0;
};
#else   /* C style interface */

typedef struct IAppxManifestQualifiedResourceVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxManifestQualifiedResource * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxManifestQualifiedResource * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxManifestQualifiedResource * This);
    HRESULT (STDMETHODCALLTYPE * GetLanguage)(IAppxManifestQualifiedResource * This, LPWSTR * language);
    HRESULT (STDMETHODCALLTYPE * GetScale)(IAppxManifestQualifiedResource * This, UINT32 * scale);
    HRESULT (STDMETHODCALLTYPE * GetDXFeatureLevel)(IAppxManifestQualifiedResource * This, DX_FEATURE_LEVEL * dxFeatureLevel);
    END_INTERFACE
} IAppxManifestQualifiedResourceVtbl;

interface IAppxManifestQualifiedResource {
    CONST_VTBL struct IAppxManifestQualifiedResourceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxManifestQualifiedResource_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxManifestQualifiedResource_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxManifestQualifiedResource_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxManifestQualifiedResource_GetLanguage(This,language)  ((This)->lpVtbl->GetLanguage(This,language)) 
#define IAppxManifestQualifiedResource_GetScale(This,scale)  ((This)->lpVtbl->GetScale(This,scale)) 
#define IAppxManifestQualifiedResource_GetDXFeatureLevel(This,dxFeatureLevel)  ((This)->lpVtbl->GetDXFeatureLevel(This,dxFeatureLevel)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxManifestQualifiedResource_INTERFACE_DEFINED__ */

#ifndef __IAppxBundleFactory_INTERFACE_DEFINED__
#define __IAppxBundleFactory_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxBundleFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("BBA65864-965F-4A5F-855F-F074BDBF3A7B")
IAppxBundleFactory:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE CreateBundleWriter(IStream * outputStream, UINT64 bundleVersion, IAppxBundleWriter ** bundleWriter) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBundleReader(IStream * inputStream, IAppxBundleReader ** bundleReader) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateBundleManifestReader(IStream * inputStream, IAppxBundleManifestReader ** manifestReader) = 0;
};
#else   /* C style interface */

typedef struct IAppxBundleFactoryVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxBundleFactory * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxBundleFactory * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxBundleFactory * This);
    HRESULT (STDMETHODCALLTYPE * CreateBundleWriter)(IAppxBundleFactory * This, IStream * outputStream, UINT64 bundleVersion, IAppxBundleWriter ** bundleWriter);
    HRESULT (STDMETHODCALLTYPE * CreateBundleReader)(IAppxBundleFactory * This, IStream * inputStream, IAppxBundleReader ** bundleReader);
    HRESULT (STDMETHODCALLTYPE * CreateBundleManifestReader)(IAppxBundleFactory * This, IStream * inputStream, IAppxBundleManifestReader ** manifestReader);
    END_INTERFACE
} IAppxBundleFactoryVtbl;

interface IAppxBundleFactory {
    CONST_VTBL struct IAppxBundleFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxBundleFactory_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxBundleFactory_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxBundleFactory_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxBundleFactory_CreateBundleWriter(This,outputStream,bundleVersion,bundleWriter)  ((This)->lpVtbl->CreateBundleWriter(This,outputStream,bundleVersion,bundleWriter)) 
#define IAppxBundleFactory_CreateBundleReader(This,inputStream,bundleReader)  ((This)->lpVtbl->CreateBundleReader(This,inputStream,bundleReader)) 
#define IAppxBundleFactory_CreateBundleManifestReader(This,inputStream,manifestReader)  ((This)->lpVtbl->CreateBundleManifestReader(This,inputStream,manifestReader)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxBundleFactory_INTERFACE_DEFINED__ */

#ifndef __IAppxBundleWriter_INTERFACE_DEFINED__
#define __IAppxBundleWriter_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxBundleWriter;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("EC446FE8-BFEC-4C64-AB4F-49F038F0C6D2")
IAppxBundleWriter:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE AddPayloadPackage(LPCWSTR fileName, IStream * packageStream) = 0;
    virtual HRESULT STDMETHODCALLTYPE Close(void) = 0;
};
#else   /* C style interface */

typedef struct IAppxBundleWriterVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxBundleWriter * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxBundleWriter * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxBundleWriter * This);
    HRESULT (STDMETHODCALLTYPE * AddPayloadPackage)(IAppxBundleWriter * This, LPCWSTR fileName, IStream * packageStream);
    HRESULT (STDMETHODCALLTYPE * Close)(IAppxBundleWriter * This);
    END_INTERFACE
} IAppxBundleWriterVtbl;

interface IAppxBundleWriter {
    CONST_VTBL struct IAppxBundleWriterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxBundleWriter_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxBundleWriter_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxBundleWriter_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxBundleWriter_AddPayloadPackage(This,fileName,packageStream)  ((This)->lpVtbl->AddPayloadPackage(This,fileName,packageStream)) 
#define IAppxBundleWriter_Close(This)  ((This)->lpVtbl->Close(This)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxBundleWriter_INTERFACE_DEFINED__ */

#ifndef __IAppxBundleReader_INTERFACE_DEFINED__
#define __IAppxBundleReader_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxBundleReader;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("DD75B8C0-BA76-43B0-AE0F-68656A1DC5C8")
IAppxBundleReader:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetFootprintFile(APPX_BUNDLE_FOOTPRINT_FILE_TYPE fileType, IAppxFile ** footprintFile) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetBlockMap(IAppxBlockMapReader ** blockMapReader) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetManifest(IAppxBundleManifestReader ** manifestReader) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPayloadPackages(IAppxFilesEnumerator ** payloadPackages) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPayloadPackage(LPCWSTR fileName, IAppxFile ** payloadPackage) = 0;
};
#else   /* C style interface */

typedef struct IAppxBundleReaderVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxBundleReader * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxBundleReader * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxBundleReader * This);
    HRESULT (STDMETHODCALLTYPE * GetFootprintFile)(IAppxBundleReader * This, APPX_BUNDLE_FOOTPRINT_FILE_TYPE fileType, IAppxFile ** footprintFile);
    HRESULT (STDMETHODCALLTYPE * GetBlockMap)(IAppxBundleReader * This, IAppxBlockMapReader ** blockMapReader);
    HRESULT (STDMETHODCALLTYPE * GetManifest)(IAppxBundleReader * This, IAppxBundleManifestReader ** manifestReader);
    HRESULT (STDMETHODCALLTYPE * GetPayloadPackages)(IAppxBundleReader * This, IAppxFilesEnumerator ** payloadPackages);
    HRESULT (STDMETHODCALLTYPE * GetPayloadPackage)(IAppxBundleReader * This, LPCWSTR fileName, IAppxFile ** payloadPackage);
    END_INTERFACE
} IAppxBundleReaderVtbl;

interface IAppxBundleReader {
    CONST_VTBL struct IAppxBundleReaderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxBundleReader_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxBundleReader_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxBundleReader_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxBundleReader_GetFootprintFile(This,fileType,footprintFile)  ((This)->lpVtbl->GetFootprintFile(This,fileType,footprintFile)) 
#define IAppxBundleReader_GetBlockMap(This,blockMapReader)  ((This)->lpVtbl->GetBlockMap(This,blockMapReader)) 
#define IAppxBundleReader_GetManifest(This,manifestReader)  ((This)->lpVtbl->GetManifest(This,manifestReader)) 
#define IAppxBundleReader_GetPayloadPackages(This,payloadPackages)  ((This)->lpVtbl->GetPayloadPackages(This,payloadPackages)) 
#define IAppxBundleReader_GetPayloadPackage(This,fileName,payloadPackage)  ((This)->lpVtbl->GetPayloadPackage(This,fileName,payloadPackage)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxBundleReader_INTERFACE_DEFINED__ */

#ifndef __IAppxBundleManifestReader_INTERFACE_DEFINED__
#define __IAppxBundleManifestReader_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxBundleManifestReader;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("CF0EBBC1-CC99-4106-91EB-E67462E04FB0")
IAppxBundleManifestReader:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetPackageId(IAppxManifestPackageId ** packageId) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPackageInfoItems(IAppxBundleManifestPackageInfoEnumerator ** packageInfoItems) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetStream(IStream ** manifestStream) = 0;
};
#else   /* C style interface */

typedef struct IAppxBundleManifestReaderVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxBundleManifestReader * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxBundleManifestReader * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxBundleManifestReader * This);
    HRESULT (STDMETHODCALLTYPE * GetPackageId)(IAppxBundleManifestReader * This, IAppxManifestPackageId ** packageId);
    HRESULT (STDMETHODCALLTYPE * GetPackageInfoItems)(IAppxBundleManifestReader * This, IAppxBundleManifestPackageInfoEnumerator ** packageInfoItems);
    HRESULT (STDMETHODCALLTYPE * GetStream)(IAppxBundleManifestReader * This, IStream ** manifestStream);
    END_INTERFACE
} IAppxBundleManifestReaderVtbl;

interface IAppxBundleManifestReader {
    CONST_VTBL struct IAppxBundleManifestReaderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxBundleManifestReader_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxBundleManifestReader_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxBundleManifestReader_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxBundleManifestReader_GetPackageId(This,packageId)  ((This)->lpVtbl->GetPackageId(This,packageId)) 
#define IAppxBundleManifestReader_GetPackageInfoItems(This,packageInfoItems)  ((This)->lpVtbl->GetPackageInfoItems(This,packageInfoItems)) 
#define IAppxBundleManifestReader_GetStream(This,manifestStream)  ((This)->lpVtbl->GetStream(This,manifestStream)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxBundleManifestReader_INTERFACE_DEFINED__ */

#ifndef __IAppxBundleManifestPackageInfoEnumerator_INTERFACE_DEFINED__
#define __IAppxBundleManifestPackageInfoEnumerator_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxBundleManifestPackageInfoEnumerator;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("F9B856EE-49A6-4E19-B2B0-6A2406D63A32")
IAppxBundleManifestPackageInfoEnumerator:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetCurrent(IAppxBundleManifestPackageInfo ** packageInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetHasCurrent(BOOL * hasCurrent) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveNext(BOOL * hasNext) = 0;
};
#else   /* C style interface */

typedef struct IAppxBundleManifestPackageInfoEnumeratorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxBundleManifestPackageInfoEnumerator * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxBundleManifestPackageInfoEnumerator * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxBundleManifestPackageInfoEnumerator * This);
    HRESULT (STDMETHODCALLTYPE * GetCurrent)(IAppxBundleManifestPackageInfoEnumerator * This, IAppxBundleManifestPackageInfo ** packageInfo);
    HRESULT (STDMETHODCALLTYPE * GetHasCurrent)(IAppxBundleManifestPackageInfoEnumerator * This, BOOL * hasCurrent);
    HRESULT (STDMETHODCALLTYPE * MoveNext)(IAppxBundleManifestPackageInfoEnumerator * This, BOOL * hasNext);
    END_INTERFACE
} IAppxBundleManifestPackageInfoEnumeratorVtbl;

interface IAppxBundleManifestPackageInfoEnumerator {
    CONST_VTBL struct IAppxBundleManifestPackageInfoEnumeratorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxBundleManifestPackageInfoEnumerator_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxBundleManifestPackageInfoEnumerator_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxBundleManifestPackageInfoEnumerator_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxBundleManifestPackageInfoEnumerator_GetCurrent(This,packageInfo)  ((This)->lpVtbl->GetCurrent(This,packageInfo)) 
#define IAppxBundleManifestPackageInfoEnumerator_GetHasCurrent(This,hasCurrent)  ((This)->lpVtbl->GetHasCurrent(This,hasCurrent)) 
#define IAppxBundleManifestPackageInfoEnumerator_MoveNext(This,hasNext)  ((This)->lpVtbl->MoveNext(This,hasNext)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxBundleManifestPackageInfoEnumerator_INTERFACE_DEFINED__ */

#ifndef __IAppxBundleManifestPackageInfo_INTERFACE_DEFINED__
#define __IAppxBundleManifestPackageInfo_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxBundleManifestPackageInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("54CD06C1-268F-40BB-8ED2-757A9EBAEC8D")
IAppxBundleManifestPackageInfo:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetPackageType(APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE * packageType) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPackageId(IAppxManifestPackageId ** packageId) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFileName(LPWSTR * fileName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetOffset(UINT64 * offset) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSize(UINT64 * size) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetResources(IAppxManifestQualifiedResourcesEnumerator ** resources) = 0;
};
#else   /* C style interface */

typedef struct IAppxBundleManifestPackageInfoVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxBundleManifestPackageInfo * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxBundleManifestPackageInfo * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxBundleManifestPackageInfo * This);
    HRESULT (STDMETHODCALLTYPE * GetPackageType)(IAppxBundleManifestPackageInfo * This, APPX_BUNDLE_PAYLOAD_PACKAGE_TYPE * packageType);
    HRESULT (STDMETHODCALLTYPE * GetPackageId)(IAppxBundleManifestPackageInfo * This, IAppxManifestPackageId ** packageId);
    HRESULT (STDMETHODCALLTYPE * GetFileName)(IAppxBundleManifestPackageInfo * This, LPWSTR * fileName);
    HRESULT (STDMETHODCALLTYPE * GetOffset)(IAppxBundleManifestPackageInfo * This, UINT64 * offset);
    HRESULT (STDMETHODCALLTYPE * GetSize)(IAppxBundleManifestPackageInfo * This, UINT64 * size);
    HRESULT (STDMETHODCALLTYPE * GetResources)(IAppxBundleManifestPackageInfo * This, IAppxManifestQualifiedResourcesEnumerator ** resources);
    END_INTERFACE
} IAppxBundleManifestPackageInfoVtbl;

interface IAppxBundleManifestPackageInfo {
    CONST_VTBL struct IAppxBundleManifestPackageInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxBundleManifestPackageInfo_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxBundleManifestPackageInfo_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxBundleManifestPackageInfo_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxBundleManifestPackageInfo_GetPackageType(This,packageType)  ((This)->lpVtbl->GetPackageType(This,packageType)) 
#define IAppxBundleManifestPackageInfo_GetPackageId(This,packageId)  ((This)->lpVtbl->GetPackageId(This,packageId)) 
#define IAppxBundleManifestPackageInfo_GetFileName(This,fileName)  ((This)->lpVtbl->GetFileName(This,fileName)) 
#define IAppxBundleManifestPackageInfo_GetOffset(This,offset)  ((This)->lpVtbl->GetOffset(This,offset)) 
#define IAppxBundleManifestPackageInfo_GetSize(This,size)  ((This)->lpVtbl->GetSize(This,size)) 
#define IAppxBundleManifestPackageInfo_GetResources(This,resources)  ((This)->lpVtbl->GetResources(This,resources)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxBundleManifestPackageInfo_INTERFACE_DEFINED__ */

typedef struct APPX_ENCRYPTED_PACKAGE_SETTINGS {
    UINT32 keyLength;
    LPCWSTR encryptionAlgorithm;
    BOOL useDiffusion;
    IUri *blockMapHashAlgorithm;
} APPX_ENCRYPTED_PACKAGE_SETTINGS;

typedef struct APPX_KEY_INFO {
    UINT32 keyLength;
    UINT32 keyIdLength;
    BYTE *key;
    BYTE *keyId;
} APPX_KEY_INFO;

typedef struct APPX_ENCRYPTED_EXEMPTIONS {
    UINT32 count;
    LPCWSTR *plainTextFiles;
} APPX_ENCRYPTED_EXEMPTIONS;

extern RPC_IF_HANDLE __MIDL_itf_appxpackaging_0000_0035_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_appxpackaging_0000_0035_v0_0_s_ifspec;

#ifndef __IAppxEncryptionFactory_INTERFACE_DEFINED__
#define __IAppxEncryptionFactory_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxEncryptionFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("80E8E04D-8C88-44AE-A011-7CADF6FB2E72")
IAppxEncryptionFactory:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE EncryptPackage(IStream * inputStream, IStream * outputStream, const APPX_ENCRYPTED_PACKAGE_SETTINGS * settings, const APPX_KEY_INFO * keyInfo, const APPX_ENCRYPTED_EXEMPTIONS * exemptedFiles) = 0;
    virtual HRESULT STDMETHODCALLTYPE DecryptPackage(IStream * inputStream, IStream * outputStream, const APPX_KEY_INFO * keyInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateEncryptedPackageWriter(IStream * outputStream, IStream * manifestStream, const APPX_ENCRYPTED_PACKAGE_SETTINGS * settings, const APPX_KEY_INFO * keyInfo, const APPX_ENCRYPTED_EXEMPTIONS * exemptedFiles, IAppxEncryptedPackageWriter ** packageWriter) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateEncryptedPackageReader(IStream * inputStream, const APPX_KEY_INFO * keyInfo, IAppxPackageReader ** packageReader) = 0;
    virtual HRESULT STDMETHODCALLTYPE EncryptBundle(IStream * inputStream, IStream * outputStream, const APPX_ENCRYPTED_PACKAGE_SETTINGS * settings, const APPX_KEY_INFO * keyInfo, const APPX_ENCRYPTED_EXEMPTIONS * exemptedFiles) = 0;
    virtual HRESULT STDMETHODCALLTYPE DecryptBundle(IStream * inputStream, IStream * outputStream, const APPX_KEY_INFO * keyInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateEncryptedBundleWriter(IStream * outputStream, UINT64 bundleVersion, const APPX_ENCRYPTED_PACKAGE_SETTINGS * settings, const APPX_KEY_INFO * keyInfo, const APPX_ENCRYPTED_EXEMPTIONS * exemptedFiles, IAppxEncryptedBundleWriter ** bundleWriter) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateEncryptedBundleReader(IStream * inputStream, const APPX_KEY_INFO * keyInfo, IAppxBundleReader ** bundleReader) = 0;
};
#else   /* C style interface */

typedef struct IAppxEncryptionFactoryVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxEncryptionFactory * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxEncryptionFactory * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxEncryptionFactory * This);
    HRESULT (STDMETHODCALLTYPE * EncryptPackage)(IAppxEncryptionFactory * This, IStream * inputStream, IStream * outputStream, const APPX_ENCRYPTED_PACKAGE_SETTINGS * settings, const APPX_KEY_INFO * keyInfo, const APPX_ENCRYPTED_EXEMPTIONS * exemptedFiles);
    HRESULT (STDMETHODCALLTYPE * DecryptPackage)(IAppxEncryptionFactory * This, IStream * inputStream, IStream * outputStream, const APPX_KEY_INFO * keyInfo);
    HRESULT (STDMETHODCALLTYPE * CreateEncryptedPackageWriter)(IAppxEncryptionFactory * This, IStream * outputStream, IStream * manifestStream, const APPX_ENCRYPTED_PACKAGE_SETTINGS * settings, const APPX_KEY_INFO * keyInfo, const APPX_ENCRYPTED_EXEMPTIONS * exemptedFiles, IAppxEncryptedPackageWriter ** packageWriter);
    HRESULT (STDMETHODCALLTYPE * CreateEncryptedPackageReader)(IAppxEncryptionFactory * This, IStream * inputStream, const APPX_KEY_INFO * keyInfo, IAppxPackageReader ** packageReader);
    HRESULT (STDMETHODCALLTYPE * EncryptBundle)(IAppxEncryptionFactory * This, IStream * inputStream, IStream * outputStream, const APPX_ENCRYPTED_PACKAGE_SETTINGS * settings, const APPX_KEY_INFO * keyInfo, const APPX_ENCRYPTED_EXEMPTIONS * exemptedFiles);
    HRESULT (STDMETHODCALLTYPE * DecryptBundle)(IAppxEncryptionFactory * This, IStream * inputStream, IStream * outputStream, const APPX_KEY_INFO * keyInfo);
    HRESULT (STDMETHODCALLTYPE * CreateEncryptedBundleWriter)(IAppxEncryptionFactory * This, IStream * outputStream, UINT64 bundleVersion, const APPX_ENCRYPTED_PACKAGE_SETTINGS * settings, const APPX_KEY_INFO * keyInfo, const APPX_ENCRYPTED_EXEMPTIONS * exemptedFiles, IAppxEncryptedBundleWriter ** bundleWriter);
    HRESULT (STDMETHODCALLTYPE * CreateEncryptedBundleReader)(IAppxEncryptionFactory * This, IStream * inputStream, const APPX_KEY_INFO * keyInfo, IAppxBundleReader ** bundleReader);
    END_INTERFACE
} IAppxEncryptionFactoryVtbl;

interface IAppxEncryptionFactory {
    CONST_VTBL struct IAppxEncryptionFactoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxEncryptionFactory_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxEncryptionFactory_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxEncryptionFactory_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxEncryptionFactory_EncryptPackage(This,inputStream,outputStream,settings,keyInfo,exemptedFiles)  ((This)->lpVtbl->EncryptPackage(This,inputStream,outputStream,settings,keyInfo,exemptedFiles)) 
#define IAppxEncryptionFactory_DecryptPackage(This,inputStream,outputStream,keyInfo)  ((This)->lpVtbl->DecryptPackage(This,inputStream,outputStream,keyInfo)) 
#define IAppxEncryptionFactory_CreateEncryptedPackageWriter(This,outputStream,manifestStream,settings,keyInfo,exemptedFiles,packageWriter)  ((This)->lpVtbl->CreateEncryptedPackageWriter(This,outputStream,manifestStream,settings,keyInfo,exemptedFiles,packageWriter)) 
#define IAppxEncryptionFactory_CreateEncryptedPackageReader(This,inputStream,keyInfo,packageReader)  ((This)->lpVtbl->CreateEncryptedPackageReader(This,inputStream,keyInfo,packageReader)) 
#define IAppxEncryptionFactory_EncryptBundle(This,inputStream,outputStream,settings,keyInfo,exemptedFiles)  ((This)->lpVtbl->EncryptBundle(This,inputStream,outputStream,settings,keyInfo,exemptedFiles)) 
#define IAppxEncryptionFactory_DecryptBundle(This,inputStream,outputStream,keyInfo)  ((This)->lpVtbl->DecryptBundle(This,inputStream,outputStream,keyInfo)) 
#define IAppxEncryptionFactory_CreateEncryptedBundleWriter(This,outputStream,bundleVersion,settings,keyInfo,exemptedFiles,bundleWriter)  ((This)->lpVtbl->CreateEncryptedBundleWriter(This,outputStream,bundleVersion,settings,keyInfo,exemptedFiles,bundleWriter)) 
#define IAppxEncryptionFactory_CreateEncryptedBundleReader(This,inputStream,keyInfo,bundleReader)  ((This)->lpVtbl->CreateEncryptedBundleReader(This,inputStream,keyInfo,bundleReader)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxEncryptionFactory_INTERFACE_DEFINED__ */

#ifndef __IAppxEncryptedPackageWriter_INTERFACE_DEFINED__
#define __IAppxEncryptedPackageWriter_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxEncryptedPackageWriter;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("F43D0B0B-1379-40E2-9B29-682EA2BF42AF")
IAppxEncryptedPackageWriter:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE AddPayloadFileEncrypted(LPCWSTR fileName, APPX_COMPRESSION_OPTION compressionOption, IStream * inputStream) = 0;
    virtual HRESULT STDMETHODCALLTYPE Close(void) = 0;
};
#else   /* C style interface */

typedef struct IAppxEncryptedPackageWriterVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxEncryptedPackageWriter * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxEncryptedPackageWriter * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxEncryptedPackageWriter * This);
    HRESULT (STDMETHODCALLTYPE * AddPayloadFileEncrypted)(IAppxEncryptedPackageWriter * This, LPCWSTR fileName, APPX_COMPRESSION_OPTION compressionOption, IStream * inputStream);
    HRESULT (STDMETHODCALLTYPE * Close)(IAppxEncryptedPackageWriter * This);
    END_INTERFACE
} IAppxEncryptedPackageWriterVtbl;

interface IAppxEncryptedPackageWriter {
    CONST_VTBL struct IAppxEncryptedPackageWriterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxEncryptedPackageWriter_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxEncryptedPackageWriter_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxEncryptedPackageWriter_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxEncryptedPackageWriter_AddPayloadFileEncrypted(This,fileName,compressionOption,inputStream)  ((This)->lpVtbl->AddPayloadFileEncrypted(This,fileName,compressionOption,inputStream)) 
#define IAppxEncryptedPackageWriter_Close(This)  ((This)->lpVtbl->Close(This)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxEncryptedPackageWriter_INTERFACE_DEFINED__ */

#ifndef __IAppxEncryptedBundleWriter_INTERFACE_DEFINED__
#define __IAppxEncryptedBundleWriter_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAppxEncryptedBundleWriter;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("80B0902F-7BF0-4117-B8C6-4279EF81EE77")
IAppxEncryptedBundleWriter:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE AddPayloadPackageEncrypted(LPCWSTR fileName, IStream * packageStream) = 0;
    virtual HRESULT STDMETHODCALLTYPE Close(void) = 0;
};
#else   /* C style interface */

typedef struct IAppxEncryptedBundleWriterVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IAppxEncryptedBundleWriter * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef)(IAppxEncryptedBundleWriter * This);
    ULONG (STDMETHODCALLTYPE * Release)(IAppxEncryptedBundleWriter * This);
    HRESULT (STDMETHODCALLTYPE * AddPayloadPackageEncrypted)(IAppxEncryptedBundleWriter * This, LPCWSTR fileName, IStream * packageStream);
    HRESULT (STDMETHODCALLTYPE * Close)(IAppxEncryptedBundleWriter * This);
    END_INTERFACE
} IAppxEncryptedBundleWriterVtbl;

interface IAppxEncryptedBundleWriter {
    CONST_VTBL struct IAppxEncryptedBundleWriterVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAppxEncryptedBundleWriter_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define IAppxEncryptedBundleWriter_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define IAppxEncryptedBundleWriter_Release(This)  ((This)->lpVtbl->Release(This)) 
#define IAppxEncryptedBundleWriter_AddPayloadPackageEncrypted(This,fileName,packageStream)  ((This)->lpVtbl->AddPayloadPackageEncrypted(This,fileName,packageStream)) 
#define IAppxEncryptedBundleWriter_Close(This)  ((This)->lpVtbl->Close(This)) 
#endif /* COBJMACROS */

#endif  /* C style interface */

#endif  /* __IAppxEncryptedBundleWriter_INTERFACE_DEFINED__ */

extern RPC_IF_HANDLE __MIDL_itf_appxpackaging_0000_0038_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_appxpackaging_0000_0038_v0_0_s_ifspec;

#ifndef __APPXPACKAGING_LIBRARY_DEFINED__
#define __APPXPACKAGING_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_APPXPACKAGING;
EXTERN_C const CLSID CLSID_AppxFactory;

#ifdef __cplusplus
class DECLSPEC_UUID("5842a140-ff9f-4166-8f5c-62f5b7b0c781")
AppxFactory;
#endif

EXTERN_C const CLSID CLSID_AppxBundleFactory;

#ifdef __cplusplus
class DECLSPEC_UUID("378E0446-5384-43B7-8877-E7DBDD883446")
AppxBundleFactory;
#endif

EXTERN_C const CLSID CLSID_AppxEncryptionFactory;

#ifdef __cplusplus
class DECLSPEC_UUID("DC664FDD-D868-46EE-8780-8D196CB739F7")
AppxEncryptionFactory;
#endif

#endif /* __APPXPACKAGING_LIBRARY_DEFINED__ */

#endif // (NTDDI >= NTDDI_WIN7)

extern RPC_IF_HANDLE __MIDL_itf_appxpackaging_0000_0039_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_appxpackaging_0000_0039_v0_0_s_ifspec;

#endif /* _APPXPACKAGING_H */
