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

#ifndef _WUAPI_H
#define _WUAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows Update definitions */

#ifndef __IUpdateLockdown_FWD_DEFINED__
#define __IUpdateLockdown_FWD_DEFINED__
typedef interface IUpdateLockdown IUpdateLockdown;
#endif

#ifndef __IStringCollection_FWD_DEFINED__
#define __IStringCollection_FWD_DEFINED__
typedef interface IStringCollection IStringCollection;
#endif

#ifndef __IWebProxy_FWD_DEFINED__
#define __IWebProxy_FWD_DEFINED__
typedef interface IWebProxy IWebProxy;
#endif

#ifndef __ISystemInformation_FWD_DEFINED__
#define __ISystemInformation_FWD_DEFINED__
typedef interface ISystemInformation ISystemInformation;
#endif

#ifndef __IAutomaticUpdatesSettings_FWD_DEFINED__
#define __IAutomaticUpdatesSettings_FWD_DEFINED__
typedef interface IAutomaticUpdatesSettings IAutomaticUpdatesSettings;
#endif

#ifndef __IAutomaticUpdates_FWD_DEFINED__
#define __IAutomaticUpdates_FWD_DEFINED__
typedef interface IAutomaticUpdates IAutomaticUpdates;
#endif

#ifndef __IUpdateIdentity_FWD_DEFINED__
#define __IUpdateIdentity_FWD_DEFINED__
typedef interface IUpdateIdentity IUpdateIdentity;
#endif

#ifndef __IImageInformation_FWD_DEFINED__
#define __IImageInformation_FWD_DEFINED__
typedef interface IImageInformation IImageInformation;
#endif

#ifndef __ICategory_FWD_DEFINED__
#define __ICategory_FWD_DEFINED__
typedef interface ICategory ICategory;
#endif

#ifndef __ICategoryCollection_FWD_DEFINED__
#define __ICategoryCollection_FWD_DEFINED__
typedef interface ICategoryCollection ICategoryCollection;
#endif

#ifndef __IInstallationBehavior_FWD_DEFINED__
#define __IInstallationBehavior_FWD_DEFINED__
typedef interface IInstallationBehavior IInstallationBehavior;
#endif

#ifndef __IUpdateDownloadContent_FWD_DEFINED__
#define __IUpdateDownloadContent_FWD_DEFINED__
typedef interface IUpdateDownloadContent IUpdateDownloadContent;
#endif

#ifndef __IUpdateDownloadContentCollection_FWD_DEFINED__
#define __IUpdateDownloadContentCollection_FWD_DEFINED__
typedef interface IUpdateDownloadContentCollection IUpdateDownloadContentCollection;
#endif

#ifndef __IUpdate_FWD_DEFINED__
#define __IUpdate_FWD_DEFINED__
typedef interface IUpdate IUpdate;
#endif

#ifndef __IWindowsDriverUpdate_FWD_DEFINED__
#define __IWindowsDriverUpdate_FWD_DEFINED__
typedef interface IWindowsDriverUpdate IWindowsDriverUpdate;
#endif

#ifndef __IUpdate2_FWD_DEFINED__
#define __IUpdate2_FWD_DEFINED__
typedef interface IUpdate2 IUpdate2;
#endif

#ifndef __IWindowsDriverUpdate2_FWD_DEFINED__
#define __IWindowsDriverUpdate2_FWD_DEFINED__
typedef interface IWindowsDriverUpdate2 IWindowsDriverUpdate2;
#endif

#ifndef __IUpdateCollection_FWD_DEFINED__
#define __IUpdateCollection_FWD_DEFINED__
typedef interface IUpdateCollection IUpdateCollection;
#endif

#ifndef __IUpdateException_FWD_DEFINED__
#define __IUpdateException_FWD_DEFINED__
typedef interface IUpdateException IUpdateException;
#endif

#ifndef __IInvalidProductLicenseException_FWD_DEFINED__
#define __IInvalidProductLicenseException_FWD_DEFINED__
typedef interface IInvalidProductLicenseException IInvalidProductLicenseException;
#endif

#ifndef __IUpdateExceptionCollection_FWD_DEFINED__
#define __IUpdateExceptionCollection_FWD_DEFINED__
typedef interface IUpdateExceptionCollection IUpdateExceptionCollection;
#endif

#ifndef __ISearchResult_FWD_DEFINED__
#define __ISearchResult_FWD_DEFINED__
typedef interface ISearchResult ISearchResult;
#endif

#ifndef __ISearchJob_FWD_DEFINED__
#define __ISearchJob_FWD_DEFINED__
typedef interface ISearchJob ISearchJob;
#endif

#ifndef __ISearchCompletedCallbackArgs_FWD_DEFINED__
#define __ISearchCompletedCallbackArgs_FWD_DEFINED__
typedef interface ISearchCompletedCallbackArgs ISearchCompletedCallbackArgs;
#endif

#ifndef __ISearchCompletedCallback_FWD_DEFINED__
#define __ISearchCompletedCallback_FWD_DEFINED__
typedef interface ISearchCompletedCallback ISearchCompletedCallback;
#endif

#ifndef __IUpdateHistoryEntry_FWD_DEFINED__
#define __IUpdateHistoryEntry_FWD_DEFINED__
typedef interface IUpdateHistoryEntry IUpdateHistoryEntry;
#endif

#ifndef __IUpdateHistoryEntryCollection_FWD_DEFINED__
#define __IUpdateHistoryEntryCollection_FWD_DEFINED__
typedef interface IUpdateHistoryEntryCollection IUpdateHistoryEntryCollection;
#endif

#ifndef __IUpdateSearcher_FWD_DEFINED__
#define __IUpdateSearcher_FWD_DEFINED__
typedef interface IUpdateSearcher IUpdateSearcher;
#endif

#ifndef __IUpdateDownloadResult_FWD_DEFINED__
#define __IUpdateDownloadResult_FWD_DEFINED__
typedef interface IUpdateDownloadResult IUpdateDownloadResult;
#endif

#ifndef __IDownloadResult_FWD_DEFINED__
#define __IDownloadResult_FWD_DEFINED__
typedef interface IDownloadResult IDownloadResult;
#endif

#ifndef __IDownloadProgress_FWD_DEFINED__
#define __IDownloadProgress_FWD_DEFINED__
typedef interface IDownloadProgress IDownloadProgress;
#endif

#ifndef __IDownloadJob_FWD_DEFINED__
#define __IDownloadJob_FWD_DEFINED__
typedef interface IDownloadJob IDownloadJob;
#endif

#ifndef __IDownloadCompletedCallbackArgs_FWD_DEFINED__
#define __IDownloadCompletedCallbackArgs_FWD_DEFINED__
typedef interface IDownloadCompletedCallbackArgs IDownloadCompletedCallbackArgs;
#endif

#ifndef __IDownloadCompletedCallback_FWD_DEFINED__
#define __IDownloadCompletedCallback_FWD_DEFINED__
typedef interface IDownloadCompletedCallback IDownloadCompletedCallback;
#endif

#ifndef __IDownloadProgressChangedCallbackArgs_FWD_DEFINED__
#define __IDownloadProgressChangedCallbackArgs_FWD_DEFINED__
typedef interface IDownloadProgressChangedCallbackArgs IDownloadProgressChangedCallbackArgs;
#endif

#ifndef __IDownloadProgressChangedCallback_FWD_DEFINED__
#define __IDownloadProgressChangedCallback_FWD_DEFINED__
typedef interface IDownloadProgressChangedCallback IDownloadProgressChangedCallback;
#endif

#ifndef __IUpdateDownloader_FWD_DEFINED__
#define __IUpdateDownloader_FWD_DEFINED__
typedef interface IUpdateDownloader IUpdateDownloader;
#endif

#ifndef __IUpdateInstallationResult_FWD_DEFINED__
#define __IUpdateInstallationResult_FWD_DEFINED__
typedef interface IUpdateInstallationResult IUpdateInstallationResult;
#endif

#ifndef __IInstallationResult_FWD_DEFINED__
#define __IInstallationResult_FWD_DEFINED__
typedef interface IInstallationResult IInstallationResult;
#endif

#ifndef __IInstallationProgress_FWD_DEFINED__
#define __IInstallationProgress_FWD_DEFINED__
typedef interface IInstallationProgress IInstallationProgress;
#endif

#ifndef __IInstallationJob_FWD_DEFINED__
#define __IInstallationJob_FWD_DEFINED__
typedef interface IInstallationJob IInstallationJob;
#endif

#ifndef __IInstallationCompletedCallbackArgs_FWD_DEFINED__
#define __IInstallationCompletedCallbackArgs_FWD_DEFINED__
typedef interface IInstallationCompletedCallbackArgs IInstallationCompletedCallbackArgs;
#endif

#ifndef __IInstallationCompletedCallback_FWD_DEFINED__
#define __IInstallationCompletedCallback_FWD_DEFINED__
typedef interface IInstallationCompletedCallback IInstallationCompletedCallback;
#endif

#ifndef __IInstallationProgressChangedCallbackArgs_FWD_DEFINED__
#define __IInstallationProgressChangedCallbackArgs_FWD_DEFINED__
typedef interface IInstallationProgressChangedCallbackArgs IInstallationProgressChangedCallbackArgs;
#endif

#ifndef __IInstallationProgressChangedCallback_FWD_DEFINED__
#define __IInstallationProgressChangedCallback_FWD_DEFINED__
typedef interface IInstallationProgressChangedCallback IInstallationProgressChangedCallback;
#endif

#ifndef __IUpdateInstaller_FWD_DEFINED__
#define __IUpdateInstaller_FWD_DEFINED__
typedef interface IUpdateInstaller IUpdateInstaller;
#endif

#ifndef __IUpdateInstaller2_FWD_DEFINED__
#define __IUpdateInstaller2_FWD_DEFINED__
typedef interface IUpdateInstaller2 IUpdateInstaller2;
#endif

#ifndef __IUpdateSession_FWD_DEFINED__
#define __IUpdateSession_FWD_DEFINED__
typedef interface IUpdateSession IUpdateSession;
#endif

#ifndef __IUpdateSession2_FWD_DEFINED__
#define __IUpdateSession2_FWD_DEFINED__
typedef interface IUpdateSession2 IUpdateSession2;
#endif

#ifndef __IUpdateService_FWD_DEFINED__
#define __IUpdateService_FWD_DEFINED__
typedef interface IUpdateService IUpdateService;
#endif

#ifndef __IUpdateServiceCollection_FWD_DEFINED__
#define __IUpdateServiceCollection_FWD_DEFINED__
typedef interface IUpdateServiceCollection IUpdateServiceCollection;
#endif

#ifndef __IUpdateServiceManager_FWD_DEFINED__
#define __IUpdateServiceManager_FWD_DEFINED__
typedef interface IUpdateServiceManager IUpdateServiceManager;
#endif

#ifndef __IUpdate_FWD_DEFINED__
#define __IUpdate_FWD_DEFINED__
typedef interface IUpdate IUpdate;
#endif

#ifndef __IUpdate2_FWD_DEFINED__
#define __IUpdate2_FWD_DEFINED__
typedef interface IUpdate2 IUpdate2;
#endif

#ifndef __IWindowsDriverUpdate_FWD_DEFINED__
#define __IWindowsDriverUpdate_FWD_DEFINED__
typedef interface IWindowsDriverUpdate IWindowsDriverUpdate;
#endif

#ifndef __IWindowsDriverUpdate2_FWD_DEFINED__
#define __IWindowsDriverUpdate2_FWD_DEFINED__
typedef interface IWindowsDriverUpdate2 IWindowsDriverUpdate2;
#endif

#ifndef __IUpdateLockdown_FWD_DEFINED__
#define __IUpdateLockdown_FWD_DEFINED__
typedef interface IUpdateLockdown IUpdateLockdown;
#endif

#ifndef __IUpdateException_FWD_DEFINED__
#define __IUpdateException_FWD_DEFINED__
typedef interface IUpdateException IUpdateException;
#endif

#ifndef __IInvalidProductLicenseException_FWD_DEFINED__
#define __IInvalidProductLicenseException_FWD_DEFINED__
typedef interface IInvalidProductLicenseException IInvalidProductLicenseException;
#endif

#ifndef __ISearchCompletedCallback_FWD_DEFINED__
#define __ISearchCompletedCallback_FWD_DEFINED__
typedef interface ISearchCompletedCallback ISearchCompletedCallback;
#endif

#ifndef __IDownloadCompletedCallback_FWD_DEFINED__
#define __IDownloadCompletedCallback_FWD_DEFINED__
typedef interface IDownloadCompletedCallback IDownloadCompletedCallback;
#endif

#ifndef __IDownloadProgressChangedCallback_FWD_DEFINED__
#define __IDownloadProgressChangedCallback_FWD_DEFINED__
typedef interface IDownloadProgressChangedCallback IDownloadProgressChangedCallback;
#endif

#ifndef __IInstallationCompletedCallback_FWD_DEFINED__
#define __IInstallationCompletedCallback_FWD_DEFINED__
typedef interface IInstallationCompletedCallback IInstallationCompletedCallback;
#endif

#ifndef __IInstallationProgressChangedCallback_FWD_DEFINED__
#define __IInstallationProgressChangedCallback_FWD_DEFINED__
typedef interface IInstallationProgressChangedCallback IInstallationProgressChangedCallback;
#endif

#ifndef __StringCollection_FWD_DEFINED__
#define __StringCollection_FWD_DEFINED__

#ifdef __cplusplus
typedef class StringCollection StringCollection;
#else
typedef struct StringCollection StringCollection;
#endif 

#endif

#ifndef __UpdateSearcher_FWD_DEFINED__
#define __UpdateSearcher_FWD_DEFINED__

#ifdef __cplusplus
typedef class UpdateSearcher UpdateSearcher;
#else
typedef struct UpdateSearcher UpdateSearcher;
#endif 

#endif

#ifndef __WebProxy_FWD_DEFINED__
#define __WebProxy_FWD_DEFINED__

#ifdef __cplusplus
typedef class WebProxy WebProxy;
#else
typedef struct WebProxy WebProxy;
#endif 

#endif

#ifndef __SystemInformation_FWD_DEFINED__
#define __SystemInformation_FWD_DEFINED__

#ifdef __cplusplus
typedef class SystemInformation SystemInformation;
#else
typedef struct SystemInformation SystemInformation;
#endif 

#endif

#ifndef __AutomaticUpdates_FWD_DEFINED__
#define __AutomaticUpdates_FWD_DEFINED__

#ifdef __cplusplus
typedef class AutomaticUpdates AutomaticUpdates;
#else
typedef struct AutomaticUpdates AutomaticUpdates;
#endif 

#endif

#ifndef __UpdateCollection_FWD_DEFINED__
#define __UpdateCollection_FWD_DEFINED__

#ifdef __cplusplus
typedef class UpdateCollection UpdateCollection;
#else
typedef struct UpdateCollection UpdateCollection;
#endif 

#endif

#ifndef __UpdateDownloader_FWD_DEFINED__
#define __UpdateDownloader_FWD_DEFINED__

#ifdef __cplusplus
typedef class UpdateDownloader UpdateDownloader;
#else
typedef struct UpdateDownloader UpdateDownloader;
#endif 

#endif

#ifndef __UpdateInstaller_FWD_DEFINED__
#define __UpdateInstaller_FWD_DEFINED__

#ifdef __cplusplus
typedef class UpdateInstaller UpdateInstaller;
#else
typedef struct UpdateInstaller UpdateInstaller;
#endif 

#endif

#ifndef __UpdateSession_FWD_DEFINED__
#define __UpdateSession_FWD_DEFINED__

#ifdef __cplusplus
typedef class UpdateSession UpdateSession;
#else
typedef struct UpdateSession UpdateSession;
#endif 

#endif

#ifndef __UpdateServiceManager_FWD_DEFINED__
#define __UpdateServiceManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class UpdateServiceManager UpdateServiceManager;
#else
typedef struct UpdateServiceManager UpdateServiceManager;
#endif 

#endif

#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *);

#pragma comment(lib, "wuguid.lib")

DEFINE_GUID(LIBID_WUApiLib, 0xB596CC9F, 0x56E5, 0x419E, 0xA6, 0x22, 0xE0, 0x1B, 0xB4, 0x57, 0x43, 0x1E);
DEFINE_GUID(IID_IUpdateLockdown, 0xa976c28d, 0x75a1, 0x42aa, 0x94, 0xae, 0x8a, 0xf8, 0xb8, 0x72, 0x08, 0x9a);
DEFINE_GUID(IID_IStringCollection, 0xeff90582, 0x2ddc, 0x480f, 0xa0, 0x6d, 0x60, 0xf3, 0xfb, 0xc3, 0x62, 0xc3);
DEFINE_GUID(IID_IWebProxy, 0x174c81fe, 0xaecd, 0x4dae, 0xb8, 0xa0, 0x2c, 0x63, 0x18, 0xdd, 0x86, 0xa8);
DEFINE_GUID(IID_ISystemInformation, 0xade87bf7, 0x7b56, 0x4275, 0x8f, 0xab, 0xb9, 0xb0, 0xe5, 0x91, 0x84, 0x4b);
DEFINE_GUID(IID_IAutomaticUpdatesSettings, 0x2ee48f22, 0xaf3c, 0x405f, 0x89, 0x70, 0xf7, 0x1b, 0xe1, 0x2e, 0xe9, 0xa2);
DEFINE_GUID(IID_IAutomaticUpdates, 0x673425bf, 0xc082, 0x4c7c, 0xbd, 0xfd, 0x56, 0x94, 0x64, 0xb8, 0xe0, 0xce);
DEFINE_GUID(IID_IUpdateIdentity, 0x46297823, 0x9940, 0x4c09, 0xae, 0xd9, 0xcd, 0x3e, 0xa6, 0xd0, 0x59, 0x68);
DEFINE_GUID(IID_IImageInformation, 0x7c907864, 0x346c, 0x4aeb, 0x8f, 0x3f, 0x57, 0xda, 0x28, 0x9f, 0x96, 0x9f);
DEFINE_GUID(IID_ICategory, 0x81ddc1b8, 0x9d35, 0x47a6, 0xb4, 0x71, 0x5b, 0x80, 0xf5, 0x19, 0x22, 0x3b);
DEFINE_GUID(IID_ICategoryCollection, 0x3a56bfb8, 0x576c, 0x43f7, 0x93, 0x35, 0xfe, 0x48, 0x38, 0xfd, 0x7e, 0x37);
DEFINE_GUID(IID_IInstallationBehavior, 0xd9a59339, 0xe245, 0x4dbd, 0x96, 0x86, 0x4d, 0x57, 0x63, 0xe3, 0x96, 0x24);
DEFINE_GUID(IID_IUpdateDownloadContent, 0x54a2cb2d, 0x9a0c, 0x48b6, 0x8a, 0x50, 0x9a, 0xbb, 0x69, 0xee, 0x2d, 0x02);
DEFINE_GUID(IID_IUpdateDownloadContentCollection, 0xbc5513c8, 0xb3b8, 0x4bf7, 0xa4, 0xd4, 0x36, 0x1c, 0x0d, 0x8c, 0x88, 0xba);
DEFINE_GUID(IID_IUpdate, 0x6a92b07a, 0xd821, 0x4682, 0xb4, 0x23, 0x5c, 0x80, 0x50, 0x22, 0xcc, 0x4d);
DEFINE_GUID(IID_IUpdate2, 0x84617776, 0xdbeb, 0x4881, 0xb4, 0x10, 0xaa, 0x29, 0x42, 0x27, 0xc3, 0xfd);
DEFINE_GUID(IID_IWindowsDriverUpdate, 0xb383cd1a, 0x5ce9, 0x4504, 0x9f, 0x63, 0x76, 0x4b, 0x12, 0x36, 0xf1, 0x91);
DEFINE_GUID(IID_IWindowsDriverUpdate2, 0x84617777, 0xdbeb, 0x4881, 0xb4, 0x10, 0xaa, 0x29, 0x42, 0x27, 0xc3, 0xfe);
DEFINE_GUID(IID_IUpdateCollection, 0x07f7438c, 0x7709, 0x4ca5, 0xb5, 0x18, 0x91, 0x27, 0x92, 0x88, 0x13, 0x4e);
DEFINE_GUID(IID_IUpdateException, 0xa376dd5e, 0x09d4, 0x427f, 0xaf, 0x7c, 0xfe, 0xd5, 0xb6, 0xe1, 0xc1, 0xd6);
DEFINE_GUID(IID_IInvalidProductLicenseException, 0xa37d00f5, 0x7bb0, 0x4953, 0xb4, 0x14, 0xf9, 0xe9, 0x83, 0x26, 0xf2, 0xe8);
DEFINE_GUID(IID_IUpdateExceptionCollection, 0x503626a3, 0x8e14, 0x4729, 0x93, 0x55, 0x0f, 0xe6, 0x64, 0xbd, 0x23, 0x21);
DEFINE_GUID(IID_ISearchResult, 0xd40cff62, 0xe08c, 0x4498, 0x94, 0x1a, 0x01, 0xe2, 0x5f, 0x0f, 0xd3, 0x3c);
DEFINE_GUID(IID_ISearchJob, 0x7366ea16, 0x7a1a, 0x4ea2, 0xb0, 0x42, 0x97, 0x3d, 0x3e, 0x9c, 0xd9, 0x9b);
DEFINE_GUID(IID_ISearchCompletedCallbackArgs, 0xa700a634, 0x2850, 0x4c47, 0x93, 0x8a, 0x9e, 0x4b, 0x6e, 0x5a, 0xf9, 0xa6);
DEFINE_GUID(IID_ISearchCompletedCallback, 0x88aee058, 0xd4b0, 0x4725, 0xa2, 0xf1, 0x81, 0x4a, 0x67, 0xae, 0x96, 0x4c);
DEFINE_GUID(IID_IUpdateHistoryEntry, 0xbe56a644, 0xaf0e, 0x4e0e, 0xa3, 0x11, 0xc1, 0xd8, 0xe6, 0x95, 0xcb, 0xff);
DEFINE_GUID(IID_IUpdateHistoryEntryCollection, 0xa7f04f3c, 0xa290, 0x435b, 0xaa, 0xdf, 0xa1, 0x16, 0xc3, 0x35, 0x7a, 0x5c);
DEFINE_GUID(IID_IUpdateSearcher, 0x8f45abf1, 0xf9ae, 0x4b95, 0xa9, 0x33, 0xf0, 0xf6, 0x6e, 0x50, 0x56, 0xea);
DEFINE_GUID(IID_IUpdateDownloadResult, 0xbf99af76, 0xb575, 0x42ad, 0x8a, 0xa4, 0x33, 0xcb, 0xb5, 0x47, 0x7a, 0xf1);
DEFINE_GUID(IID_IDownloadResult, 0xdaa4fdd0, 0x4727, 0x4dbe, 0xa1, 0xe7, 0x74, 0x5d, 0xca, 0x31, 0x71, 0x44);
DEFINE_GUID(IID_IDownloadProgress, 0xd31a5bac, 0xf719, 0x4178, 0x9d, 0xbb, 0x5e, 0x2c, 0xb4, 0x7f, 0xd1, 0x8a);
DEFINE_GUID(IID_IDownloadJob, 0xc574de85, 0x7358, 0x43f6, 0xaa, 0xe8, 0x86, 0x97, 0xe6, 0x2d, 0x8b, 0xa7);
DEFINE_GUID(IID_IDownloadCompletedCallbackArgs, 0xfa565b23, 0x498c, 0x47a0, 0x97, 0x9d, 0xe7, 0xd5, 0xb1, 0x81, 0x33, 0x60);
DEFINE_GUID(IID_IDownloadCompletedCallback, 0x77254866, 0x9f5b, 0x4c8e, 0xb9, 0xe2, 0xc7, 0x7a, 0x85, 0x30, 0xd6, 0x4b);
DEFINE_GUID(IID_IDownloadProgressChangedCallbackArgs, 0x324ff2c6, 0x4981, 0x4b04, 0x94, 0x12, 0x57, 0x48, 0x17, 0x45, 0xab, 0x24);
DEFINE_GUID(IID_IDownloadProgressChangedCallback, 0x8c3f1cdd, 0x6173, 0x4591, 0xae, 0xbd, 0xa5, 0x6a, 0x53, 0xca, 0x77, 0xc1);
DEFINE_GUID(IID_IUpdateDownloader, 0x68f1c6f9, 0x7ecc, 0x4666, 0xa4, 0x64, 0x24, 0x7f, 0xe1, 0x24, 0x96, 0xc3);
DEFINE_GUID(IID_IUpdateInstallationResult, 0xd940f0f8, 0x3cbb, 0x4fd0, 0x99, 0x3f, 0x47, 0x1e, 0x7f, 0x23, 0x28, 0xad);
DEFINE_GUID(IID_IInstallationResult, 0xa43c56d6, 0x7451, 0x48d4, 0xaf, 0x96, 0xb6, 0xcd, 0x2d, 0x0d, 0x9b, 0x7a);
DEFINE_GUID(IID_IInstallationProgress, 0x345c8244, 0x43a3, 0x4e32, 0xa3, 0x68, 0x65, 0xf0, 0x73, 0xb7, 0x6f, 0x36);
DEFINE_GUID(IID_IInstallationJob, 0x5c209f0b, 0xbad5, 0x432a, 0x95, 0x56, 0x46, 0x99, 0xbe, 0xd2, 0x63, 0x8a);
DEFINE_GUID(IID_IInstallationCompletedCallbackArgs, 0x250e2106, 0x8efb, 0x4705, 0x96, 0x53, 0xef, 0x13, 0xc5, 0x81, 0xb6, 0xa1);
DEFINE_GUID(IID_IInstallationCompletedCallback, 0x45f4f6f3, 0xd602, 0x4f98, 0x9a, 0x8a, 0x3e, 0xfa, 0x15, 0x2a, 0xd2, 0xd3);
DEFINE_GUID(IID_IInstallationProgressChangedCallbackArgs, 0xe4f14e1e, 0x689d, 0x4218, 0xa0, 0xb9, 0xbc, 0x18, 0x9c, 0x48, 0x4a, 0x01);
DEFINE_GUID(IID_IInstallationProgressChangedCallback, 0xe01402d5, 0xf8da, 0x43ba, 0xa0, 0x12, 0x38, 0x89, 0x4b, 0xd0, 0x48, 0xf1);
DEFINE_GUID(IID_IUpdateInstaller, 0x7b929c68, 0xccdc, 0x4226, 0x96, 0xb1, 0x87, 0x24, 0x60, 0x0b, 0x54, 0xc2);
DEFINE_GUID(IID_IUpdateInstaller2, 0xf7c72712, 0xc7f3, 0x4aad, 0xbc, 0x6a, 0x6a, 0x0e, 0x96, 0x96, 0xa5, 0x0e);
DEFINE_GUID(IID_IUpdateSession, 0x816858a4, 0x260d, 0x4260, 0x93, 0x3a, 0x25, 0x85, 0xf1, 0xab, 0xc7, 0x6b);
DEFINE_GUID(IID_IUpdateSession2, 0x91caf7b0, 0xeb23, 0x49ed, 0x99, 0x37, 0xc5, 0x2d, 0x81, 0x7f, 0x46, 0xf7);
DEFINE_GUID(IID_IUpdateService, 0x8c3f0d06, 0x1eb9, 0x4f01, 0x8c, 0x2e, 0x1a, 0xdc, 0x74, 0x62, 0x78, 0x29);
DEFINE_GUID(IID_IUpdateServiceCollection, 0x9b0353aa, 0x0e52, 0x44ff, 0xb8, 0xb0, 0x1f, 0x7f, 0xa0, 0x43, 0x7f, 0x88);
DEFINE_GUID(IID_IUpdateServiceManager, 0xc165fa56, 0x6f87, 0x43aa, 0x9d, 0x92, 0x6c, 0xeb, 0xcc, 0x7b, 0xcf, 0xe4);
DEFINE_GUID(CLSID_StringCollection, 0x72C97D74, 0x7C3B, 0x40AE, 0xB7, 0x7D, 0xAB, 0xDB, 0x22, 0xEB, 0xA6, 0xFB);
DEFINE_GUID(CLSID_UpdateSearcher, 0xB699E5E8, 0x67FF, 0x4177, 0x88, 0xB0, 0x36, 0x84, 0xA3, 0x38, 0x8B, 0xFB);
DEFINE_GUID(CLSID_WebProxy, 0x650503cf, 0x9108, 0x4ddc, 0xa2, 0xce, 0x6c, 0x23, 0x41, 0xe1, 0xc5, 0x82);
DEFINE_GUID(CLSID_SystemInformation, 0xC01B9BA0, 0xBEA7, 0x41BA, 0xB6, 0x04, 0xD0, 0xA3, 0x6F, 0x46, 0x91, 0x33);
DEFINE_GUID(CLSID_AutomaticUpdates, 0xBFE18E9C, 0x6D87, 0x4450, 0xB3, 0x7C, 0xE0, 0x2F, 0x0B, 0x37, 0x38, 0x03);
DEFINE_GUID(CLSID_UpdateCollection, 0x13639463, 0x00DB, 0x4646, 0x80, 0x3D, 0x52, 0x80, 0x26, 0x14, 0x0D, 0x88);
DEFINE_GUID(CLSID_UpdateDownloader, 0x5BAF654A, 0x5A07, 0x4264, 0xA2, 0x55, 0x9F, 0xF5, 0x4C, 0x71, 0x51, 0xE7);
DEFINE_GUID(CLSID_UpdateInstaller, 0xD2E0FE7F, 0xD23E, 0x48E1, 0x93, 0xC0, 0x6F, 0xA8, 0xCC, 0x34, 0x64, 0x74);
DEFINE_GUID(CLSID_UpdateSession, 0x4CB43D7F, 0x7EEE, 0x4906, 0x86, 0x98, 0x60, 0xDA, 0x1C, 0x38, 0xF2, 0xFE);
DEFINE_GUID(CLSID_UpdateServiceManager, 0x91353063, 0x774C, 0x4F84, 0xB0, 0x5B, 0x49, 0x8F, 0xEC, 0x7F, 0xBE, 0x25);

typedef enum tagAutomaticUpdatesNotificationLevel {
    aunlNotConfigured = 0,
    aunlDisabled = 1,
    aunlNotifyBeforeDownload = 2,
    aunlNotifyBeforeInstallation = 3,
    aunlScheduledInstallation = 4
} AutomaticUpdatesNotificationLevel;

typedef enum tagAutomaticUpdatesScheduledInstallationDay {
    ausidEveryDay = 0,
    ausidEverySunday = 1,
    ausidEveryMonday = 2,
    ausidEveryTuesday = 3,
    ausidEveryWednesday = 4,
    ausidEveryThursday = 5,
    ausidEveryFriday = 6,
    ausidEverySaturday = 7
} AutomaticUpdatesScheduledInstallationDay;

typedef enum tagDownloadPhase {
    dphInitializing = 1,
    dphDownloading = 2,
    dphVerifying = 3
} DownloadPhase;

typedef enum tagDownloadPriority {
    dpLow = 1,
    dpNormal = 2,
    dpHigh = 3
} DownloadPriority;

typedef enum tagInstallationImpact {
    iiNormal = 0,
    iiMinor = 1,
    iiRequiresExclusiveHandling = 2
} InstallationImpact;

typedef enum tagInstallationRebootBehavior {
    irbNeverReboots = 0,
    irbAlwaysRequiresReboot = 1,
    irbCanRequestReboot = 2
} InstallationRebootBehavior;

typedef enum tagOperationResultCode { 
    orcNotStarted = 0,
    orcInProgress = 1,
    orcSucceeded = 2,
    orcSucceededWithErrors = 3,
    orcFailed = 4,
    orcAborted = 5
} OperationResultCode;

typedef enum tagServerSelection { 
    ssDefault = 0,
    ssManagedServer = 1,
    ssWindowsUpdate = 2,
    ssOthers = 3
} ServerSelection;

typedef enum tagUpdateType { 
    utSoftware = 1,
    utDriver = 2
} UpdateType;

typedef enum tagUpdateOperation { 
    uoInstallation = 1,
    uoUninstallation = 2
} UpdateOperation;

typedef enum tagDeploymentAction { 
    daNone = 0,
    daInstallation = 1,
    daUninstallation = 2
} DeploymentAction;

typedef enum tagUpdateExceptionContext { 
    uecGeneral = 1,
    uecWindowsDriver = 2,
    uecWindowsInstaller = 3
} UpdateExceptionContext;

typedef enum tagInstalledState { 
    isNotInstalled = 0,
    isPendingReboot = 1,
    isInstalled = 2
} InstalledState;

#define UPDATE_LOCKDOWN_WEBSITE_ACCESS  (0x1)

extern RPC_IF_HANDLE __MIDL_itf_wuapi_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wuapi_0000_v0_0_s_ifspec;

#ifndef __IUpdateLockdown_INTERFACE_DEFINED__
#define __IUpdateLockdown_INTERFACE_DEFINED__

	EXTERN_C const IID IID_IUpdateLockdown;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a976c28d-75a1-42aa-94ae-8af8b872089a") IUpdateLockdown:public IUnknown
{
	public:
	virtual HRESULT STDMETHODCALLTYPE LockDown(LONG flags) = 0;
};
#else
typedef struct IUpdateLockdownVtbl {
    BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateLockdown * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateLockdown * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateLockdown * This);
	HRESULT(STDMETHODCALLTYPE *LockDown)(IUpdateLockdown * This, LONG flags);
    END_INTERFACE
} IUpdateLockdownVtbl;

interface IUpdateLockdown {
    CONST_VTBL struct IUpdateLockdownVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateLockdown_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateLockdown_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateLockdown_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateLockdown_LockDown(This,flags)  (This)->lpVtbl -> LockDown(This,flags)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateLockdown_LockDown_Proxy(IUpdateLockdown * This, LONG flags);
void __RPC_STUB IUpdateLockdown_LockDown_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
#endif

#ifndef __IStringCollection_INTERFACE_DEFINED__
#define __IStringCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_IStringCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("eff90582-2ddc-480f-a06d-60f3fbc362c3")IStringCollection:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Item(LONG index, BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Item(LONG index, BSTR value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ReadOnly(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE Add(BSTR value, LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE Clear(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE Copy(IStringCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE Insert(LONG index, BSTR value) = 0;
	virtual HRESULT STDMETHODCALLTYPE RemoveAt(LONG index) = 0;
};
#else
typedef struct IStringCollectionVtbl {
    BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IStringCollection * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IStringCollection * This);
	ULONG(STDMETHODCALLTYPE *Release)(IStringCollection * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IStringCollection * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IStringCollection * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IStringCollection * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IStringCollection * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Item)(IStringCollection * This, LONG index, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *put_Item)(IStringCollection * This, LONG index, BSTR value);
	HRESULT(STDMETHODCALLTYPE *get__NewEnum)(IStringCollection * This, IUnknown ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Count)(IStringCollection * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_ReadOnly)(IStringCollection * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *Add)(IStringCollection * This, BSTR value, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *Clear)(IStringCollection * This);
	HRESULT(STDMETHODCALLTYPE *Copy)(IStringCollection * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *Insert)(IStringCollection * This, LONG index, BSTR value);
	HRESULT(STDMETHODCALLTYPE *RemoveAt)(IStringCollection * This, LONG index);
    END_INTERFACE
} IStringCollectionVtbl;

interface IStringCollection {
    CONST_VTBL struct IStringCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IStringCollection_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IStringCollection_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IStringCollection_Release(This)  (This)->lpVtbl -> Release(This)
#define IStringCollection_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IStringCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IStringCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IStringCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IStringCollection_get_Item(This,index,retval)  (This)->lpVtbl -> get_Item(This,index,retval)
#define IStringCollection_put_Item(This,index,value)  (This)->lpVtbl -> put_Item(This,index,value)
#define IStringCollection_get__NewEnum(This,retval)  (This)->lpVtbl -> get__NewEnum(This,retval)
#define IStringCollection_get_Count(This,retval)  (This)->lpVtbl -> get_Count(This,retval)
#define IStringCollection_get_ReadOnly(This,retval)  (This)->lpVtbl -> get_ReadOnly(This,retval)
#define IStringCollection_Add(This,value,retval)  (This)->lpVtbl -> Add(This,value,retval)
#define IStringCollection_Clear(This)  (This)->lpVtbl -> Clear(This)
#define IStringCollection_Copy(This,retval)  (This)->lpVtbl -> Copy(This,retval)
#define IStringCollection_Insert(This,index,value)  (This)->lpVtbl -> Insert(This,index,value)
#define IStringCollection_RemoveAt(This,index)  (This)->lpVtbl -> RemoveAt(This,index)
#endif

#endif

HRESULT STDMETHODCALLTYPE IStringCollection_get_Item_Proxy(IStringCollection * This, LONG index, BSTR * retval);
void __RPC_STUB IStringCollection_get_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IStringCollection_put_Item_Proxy(IStringCollection * This, LONG index, BSTR value);
void __RPC_STUB IStringCollection_put_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IStringCollection_get__NewEnum_Proxy(IStringCollection * This, IUnknown ** retval);
void __RPC_STUB IStringCollection_get__NewEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IStringCollection_get_Count_Proxy(IStringCollection * This, LONG * retval);
void __RPC_STUB IStringCollection_get_Count_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IStringCollection_get_ReadOnly_Proxy(IStringCollection * This, VARIANT_BOOL * retval);
void __RPC_STUB IStringCollection_get_ReadOnly_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IStringCollection_Add_Proxy(IStringCollection * This, BSTR value, LONG * retval);
void __RPC_STUB IStringCollection_Add_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IStringCollection_Clear_Proxy(IStringCollection * This);
void __RPC_STUB IStringCollection_Clear_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IStringCollection_Copy_Proxy(IStringCollection * This, IStringCollection ** retval);
void __RPC_STUB IStringCollection_Copy_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IStringCollection_Insert_Proxy(IStringCollection * This, LONG index, BSTR value);
void __RPC_STUB IStringCollection_Insert_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IStringCollection_RemoveAt_Proxy(IStringCollection * This, LONG index);
void __RPC_STUB IStringCollection_RemoveAt_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
#endif


#ifndef __IWebProxy_INTERFACE_DEFINED__
#define __IWebProxy_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWebProxy;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("174c81fe-aecd-4dae-b8a0-2c6318dd86a8")IWebProxy:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Address(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Address(BSTR value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_BypassList(IStringCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_BypassList(IStringCollection * value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_BypassProxyOnLocal(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_BypassProxyOnLocal(VARIANT_BOOL value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ReadOnly(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UserName(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_UserName(BSTR value) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetPassword(BSTR value) = 0;
	virtual HRESULT STDMETHODCALLTYPE PromptForCredentials(IUnknown * pParentWindow, BSTR bstrTitle) = 0;
	virtual HRESULT STDMETHODCALLTYPE PromptForCredentialsFromHwnd(HWND hwndParent, BSTR bstrTitle) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_AutoDetect(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_AutoDetect(VARIANT_BOOL value) = 0;
};
#else
typedef struct IWebProxyVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IWebProxy * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IWebProxy * This);
	ULONG(STDMETHODCALLTYPE *Release)(IWebProxy * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IWebProxy * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IWebProxy * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IWebProxy * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IWebProxy * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Address)(IWebProxy * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *put_Address)(IWebProxy * This, BSTR value);
	HRESULT(STDMETHODCALLTYPE *get_BypassList)(IWebProxy * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *put_BypassList)(IWebProxy * This, IStringCollection * value);
	HRESULT(STDMETHODCALLTYPE *get_BypassProxyOnLocal)(IWebProxy * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_BypassProxyOnLocal)(IWebProxy * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *get_ReadOnly)(IWebProxy * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_UserName)(IWebProxy * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *put_UserName)(IWebProxy * This, BSTR value);
	HRESULT(STDMETHODCALLTYPE *SetPassword)(IWebProxy * This, BSTR value);
	HRESULT(STDMETHODCALLTYPE *PromptForCredentials)(IWebProxy * This, IUnknown * pParentWindow, BSTR bstrTitle);
	HRESULT(STDMETHODCALLTYPE *PromptForCredentialsFromHwnd)(IWebProxy * This, HWND hwndParent, BSTR bstrTitle);
	HRESULT(STDMETHODCALLTYPE *get_AutoDetect)(IWebProxy * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_AutoDetect)(IWebProxy * This, VARIANT_BOOL value);
	END_INTERFACE
} IWebProxyVtbl;

interface IWebProxy {
    CONST_VTBL struct IWebProxyVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWebProxy_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IWebProxy_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IWebProxy_Release(This)  (This)->lpVtbl -> Release(This)
#define IWebProxy_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IWebProxy_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IWebProxy_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IWebProxy_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IWebProxy_get_Address(This,retval)  (This)->lpVtbl -> get_Address(This,retval)
#define IWebProxy_put_Address(This,value)  (This)->lpVtbl -> put_Address(This,value)
#define IWebProxy_get_BypassList(This,retval)  (This)->lpVtbl -> get_BypassList(This,retval)
#define IWebProxy_put_BypassList(This,value)  (This)->lpVtbl -> put_BypassList(This,value)
#define IWebProxy_get_BypassProxyOnLocal(This,retval)  (This)->lpVtbl -> get_BypassProxyOnLocal(This,retval)
#define IWebProxy_put_BypassProxyOnLocal(This,value)  (This)->lpVtbl -> put_BypassProxyOnLocal(This,value)
#define IWebProxy_get_ReadOnly(This,retval)  (This)->lpVtbl -> get_ReadOnly(This,retval)
#define IWebProxy_get_UserName(This,retval)  (This)->lpVtbl -> get_UserName(This,retval)
#define IWebProxy_put_UserName(This,value)  (This)->lpVtbl -> put_UserName(This,value)
#define IWebProxy_SetPassword(This,value)  (This)->lpVtbl -> SetPassword(This,value)
#define IWebProxy_PromptForCredentials(This,pParentWindow,bstrTitle)  (This)->lpVtbl -> PromptForCredentials(This,pParentWindow,bstrTitle)
#define IWebProxy_PromptForCredentialsFromHwnd(This,hwndParent,bstrTitle)  (This)->lpVtbl -> PromptForCredentialsFromHwnd(This,hwndParent,bstrTitle)
#define IWebProxy_get_AutoDetect(This,retval)  (This)->lpVtbl -> get_AutoDetect(This,retval)
#define IWebProxy_put_AutoDetect(This,value)  (This)->lpVtbl -> put_AutoDetect(This,value)
#endif

#endif

HRESULT STDMETHODCALLTYPE IWebProxy_get_Address_Proxy(IWebProxy * This, BSTR * retval);
void __RPC_STUB IWebProxy_get_Address_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWebProxy_put_Address_Proxy(IWebProxy * This, BSTR value);
void __RPC_STUB IWebProxy_put_Address_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWebProxy_get_BypassList_Proxy(IWebProxy * This, IStringCollection ** retval);
void __RPC_STUB IWebProxy_get_BypassList_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWebProxy_put_BypassList_Proxy(IWebProxy * This, IStringCollection * value);
void __RPC_STUB IWebProxy_put_BypassList_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWebProxy_get_BypassProxyOnLocal_Proxy(IWebProxy * This, VARIANT_BOOL * retval);
void __RPC_STUB IWebProxy_get_BypassProxyOnLocal_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWebProxy_put_BypassProxyOnLocal_Proxy(IWebProxy * This, VARIANT_BOOL value);
void __RPC_STUB IWebProxy_put_BypassProxyOnLocal_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWebProxy_get_ReadOnly_Proxy(IWebProxy * This, VARIANT_BOOL * retval);
void __RPC_STUB IWebProxy_get_ReadOnly_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWebProxy_get_UserName_Proxy(IWebProxy * This, BSTR * retval);
void __RPC_STUB IWebProxy_get_UserName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWebProxy_put_UserName_Proxy(IWebProxy * This, BSTR value);
void __RPC_STUB IWebProxy_put_UserName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWebProxy_SetPassword_Proxy(IWebProxy * This, BSTR value);
void __RPC_STUB IWebProxy_SetPassword_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWebProxy_PromptForCredentials_Proxy(IWebProxy * This, IUnknown * pParentWindow, BSTR bstrTitle);
void __RPC_STUB IWebProxy_PromptForCredentials_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWebProxy_PromptForCredentialsFromHwnd_Proxy(IWebProxy * This, HWND hwndParent, BSTR bstrTitle);
void __RPC_STUB IWebProxy_PromptForCredentialsFromHwnd_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWebProxy_get_AutoDetect_Proxy(IWebProxy * This, VARIANT_BOOL * retval);
void __RPC_STUB IWebProxy_get_AutoDetect_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWebProxy_put_AutoDetect_Proxy(IWebProxy * This, VARIANT_BOOL value);
void __RPC_STUB IWebProxy_put_AutoDetect_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
#endif

#ifndef __ISystemInformation_INTERFACE_DEFINED__
#define __ISystemInformation_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISystemInformation;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("ade87bf7-7b56-4275-8fab-b9b0e591844b")ISystemInformation:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_OemHardwareSupportLink(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_RebootRequired(VARIANT_BOOL * retval) = 0;
};
#else
typedef struct ISystemInformationVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(ISystemInformation * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(ISystemInformation * This);
	ULONG(STDMETHODCALLTYPE *Release)(ISystemInformation * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ISystemInformation * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ISystemInformation * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ISystemInformation * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(ISystemInformation * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_OemHardwareSupportLink)(ISystemInformation * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_RebootRequired)(ISystemInformation * This, VARIANT_BOOL * retval);
	END_INTERFACE
} ISystemInformationVtbl;

interface ISystemInformation {
    CONST_VTBL struct ISystemInformationVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISystemInformation_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ISystemInformation_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define ISystemInformation_Release(This)  (This)->lpVtbl -> Release(This)
#define ISystemInformation_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define ISystemInformation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ISystemInformation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ISystemInformation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ISystemInformation_get_OemHardwareSupportLink(This,retval)  (This)->lpVtbl -> get_OemHardwareSupportLink(This,retval)
#define ISystemInformation_get_RebootRequired(This,retval)  (This)->lpVtbl -> get_RebootRequired(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE ISystemInformation_get_OemHardwareSupportLink_Proxy(ISystemInformation * This, BSTR * retval);
void __RPC_STUB ISystemInformation_get_OemHardwareSupportLink_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ISystemInformation_get_RebootRequired_Proxy(ISystemInformation * This, VARIANT_BOOL * retval);
void __RPC_STUB ISystemInformation_get_RebootRequired_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAutomaticUpdatesSettings_INTERFACE_DEFINED__
#define __IAutomaticUpdatesSettings_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAutomaticUpdatesSettings;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("2ee48f22-af3c-405f-8970-f71be12ee9a2")IAutomaticUpdatesSettings:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_NotificationLevel(AutomaticUpdatesNotificationLevel * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_NotificationLevel(AutomaticUpdatesNotificationLevel value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ReadOnly(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Required(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ScheduledInstallationDay(AutomaticUpdatesScheduledInstallationDay * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ScheduledInstallationDay(AutomaticUpdatesScheduledInstallationDay value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ScheduledInstallationTime(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ScheduledInstallationTime(LONG value) = 0;
	virtual HRESULT STDMETHODCALLTYPE Refresh(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE Save(void) = 0;
};
#else
typedef struct IAutomaticUpdatesSettingsVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IAutomaticUpdatesSettings * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAutomaticUpdatesSettings * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAutomaticUpdatesSettings * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAutomaticUpdatesSettings * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAutomaticUpdatesSettings * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAutomaticUpdatesSettings * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAutomaticUpdatesSettings * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_NotificationLevel)(IAutomaticUpdatesSettings * This, AutomaticUpdatesNotificationLevel * retval);
	HRESULT(STDMETHODCALLTYPE *put_NotificationLevel)(IAutomaticUpdatesSettings * This, AutomaticUpdatesNotificationLevel value);
	HRESULT(STDMETHODCALLTYPE *get_ReadOnly)(IAutomaticUpdatesSettings * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Required)(IAutomaticUpdatesSettings * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_ScheduledInstallationDay)(IAutomaticUpdatesSettings * This, AutomaticUpdatesScheduledInstallationDay * retval);
	HRESULT(STDMETHODCALLTYPE *put_ScheduledInstallationDay)(IAutomaticUpdatesSettings * This, AutomaticUpdatesScheduledInstallationDay value);
	HRESULT(STDMETHODCALLTYPE *get_ScheduledInstallationTime)(IAutomaticUpdatesSettings * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *put_ScheduledInstallationTime)(IAutomaticUpdatesSettings * This, LONG value);
	HRESULT(STDMETHODCALLTYPE *Refresh)(IAutomaticUpdatesSettings * This);
	HRESULT(STDMETHODCALLTYPE *Save)(IAutomaticUpdatesSettings * This);
	END_INTERFACE
} IAutomaticUpdatesSettingsVtbl;

interface IAutomaticUpdatesSettings {
    CONST_VTBL struct IAutomaticUpdatesSettingsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAutomaticUpdatesSettings_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IAutomaticUpdatesSettings_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IAutomaticUpdatesSettings_Release(This)  (This)->lpVtbl -> Release(This)
#define IAutomaticUpdatesSettings_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IAutomaticUpdatesSettings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAutomaticUpdatesSettings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAutomaticUpdatesSettings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAutomaticUpdatesSettings_get_NotificationLevel(This,retval)  (This)->lpVtbl -> get_NotificationLevel(This,retval)
#define IAutomaticUpdatesSettings_put_NotificationLevel(This,value)  (This)->lpVtbl -> put_NotificationLevel(This,value)
#define IAutomaticUpdatesSettings_get_ReadOnly(This,retval)  (This)->lpVtbl -> get_ReadOnly(This,retval)
#define IAutomaticUpdatesSettings_get_Required(This,retval)  (This)->lpVtbl -> get_Required(This,retval)
#define IAutomaticUpdatesSettings_get_ScheduledInstallationDay(This,retval)  (This)->lpVtbl -> get_ScheduledInstallationDay(This,retval)
#define IAutomaticUpdatesSettings_put_ScheduledInstallationDay(This,value)  (This)->lpVtbl -> put_ScheduledInstallationDay(This,value)
#define IAutomaticUpdatesSettings_get_ScheduledInstallationTime(This,retval)  (This)->lpVtbl -> get_ScheduledInstallationTime(This,retval)
#define IAutomaticUpdatesSettings_put_ScheduledInstallationTime(This,value)  (This)->lpVtbl -> put_ScheduledInstallationTime(This,value)
#define IAutomaticUpdatesSettings_Refresh(This)  (This)->lpVtbl -> Refresh(This)
#define IAutomaticUpdatesSettings_Save(This)  (This)->lpVtbl -> Save(This)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAutomaticUpdatesSettings_get_NotificationLevel_Proxy(IAutomaticUpdatesSettings * This, AutomaticUpdatesNotificationLevel * retval);
void __RPC_STUB IAutomaticUpdatesSettings_get_NotificationLevel_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdatesSettings_put_NotificationLevel_Proxy(IAutomaticUpdatesSettings * This, AutomaticUpdatesNotificationLevel value);
void __RPC_STUB IAutomaticUpdatesSettings_put_NotificationLevel_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdatesSettings_get_ReadOnly_Proxy(IAutomaticUpdatesSettings * This, VARIANT_BOOL * retval);
void __RPC_STUB IAutomaticUpdatesSettings_get_ReadOnly_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdatesSettings_get_Required_Proxy(IAutomaticUpdatesSettings * This, VARIANT_BOOL * retval);
void __RPC_STUB IAutomaticUpdatesSettings_get_Required_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdatesSettings_get_ScheduledInstallationDay_Proxy(IAutomaticUpdatesSettings * This, AutomaticUpdatesScheduledInstallationDay * retval);
void __RPC_STUB IAutomaticUpdatesSettings_get_ScheduledInstallationDay_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdatesSettings_put_ScheduledInstallationDay_Proxy(IAutomaticUpdatesSettings * This, AutomaticUpdatesScheduledInstallationDay value);
void __RPC_STUB IAutomaticUpdatesSettings_put_ScheduledInstallationDay_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdatesSettings_get_ScheduledInstallationTime_Proxy(IAutomaticUpdatesSettings * This, LONG * retval);
void __RPC_STUB IAutomaticUpdatesSettings_get_ScheduledInstallationTime_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdatesSettings_put_ScheduledInstallationTime_Proxy(IAutomaticUpdatesSettings * This, LONG value);
void __RPC_STUB IAutomaticUpdatesSettings_put_ScheduledInstallationTime_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdatesSettings_Refresh_Proxy(IAutomaticUpdatesSettings * This);
void __RPC_STUB IAutomaticUpdatesSettings_Refresh_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdatesSettings_Save_Proxy(IAutomaticUpdatesSettings * This);
void __RPC_STUB IAutomaticUpdatesSettings_Save_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAutomaticUpdates_INTERFACE_DEFINED__
#define __IAutomaticUpdates_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAutomaticUpdates;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("673425bf-c082-4c7c-bdfd-569464b8e0ce")IAutomaticUpdates:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE DetectNow(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE Pause(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE Resume(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE ShowSettingsDialog(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Settings(IAutomaticUpdatesSettings ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ServiceEnabled(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE EnableService(void) = 0;
};
#else
typedef struct IAutomaticUpdatesVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IAutomaticUpdates * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAutomaticUpdates * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAutomaticUpdates * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAutomaticUpdates * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAutomaticUpdates * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAutomaticUpdates * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAutomaticUpdates * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *DetectNow)(IAutomaticUpdates * This);
	HRESULT(STDMETHODCALLTYPE *Pause)(IAutomaticUpdates * This);
	HRESULT(STDMETHODCALLTYPE *Resume)(IAutomaticUpdates * This);
	HRESULT(STDMETHODCALLTYPE *ShowSettingsDialog)(IAutomaticUpdates * This);
	HRESULT(STDMETHODCALLTYPE *get_Settings)(IAutomaticUpdates * This, IAutomaticUpdatesSettings ** retval);
	HRESULT(STDMETHODCALLTYPE *get_ServiceEnabled)(IAutomaticUpdates * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *EnableService)(IAutomaticUpdates * This);
	END_INTERFACE
} IAutomaticUpdatesVtbl;

interface IAutomaticUpdates {
    CONST_VTBL struct IAutomaticUpdatesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAutomaticUpdates_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IAutomaticUpdates_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IAutomaticUpdates_Release(This)  (This)->lpVtbl -> Release(This)
#define IAutomaticUpdates_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IAutomaticUpdates_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAutomaticUpdates_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAutomaticUpdates_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAutomaticUpdates_DetectNow(This)  (This)->lpVtbl -> DetectNow(This)
#define IAutomaticUpdates_Pause(This)  (This)->lpVtbl -> Pause(This)
#define IAutomaticUpdates_Resume(This)  (This)->lpVtbl -> Resume(This)
#define IAutomaticUpdates_ShowSettingsDialog(This)  (This)->lpVtbl -> ShowSettingsDialog(This)
#define IAutomaticUpdates_get_Settings(This,retval)  (This)->lpVtbl -> get_Settings(This,retval)
#define IAutomaticUpdates_get_ServiceEnabled(This,retval)  (This)->lpVtbl -> get_ServiceEnabled(This,retval)
#define IAutomaticUpdates_EnableService(This)  (This)->lpVtbl -> EnableService(This)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAutomaticUpdates_DetectNow_Proxy(IAutomaticUpdates * This);
void __RPC_STUB IAutomaticUpdates_DetectNow_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdates_Pause_Proxy(IAutomaticUpdates * This);
void __RPC_STUB IAutomaticUpdates_Pause_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdates_Resume_Proxy(IAutomaticUpdates * This);
void __RPC_STUB IAutomaticUpdates_Resume_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdates_ShowSettingsDialog_Proxy(IAutomaticUpdates * This);
void __RPC_STUB IAutomaticUpdates_ShowSettingsDialog_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdates_get_Settings_Proxy(IAutomaticUpdates * This, IAutomaticUpdatesSettings ** retval);
void __RPC_STUB IAutomaticUpdates_get_Settings_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdates_get_ServiceEnabled_Proxy(IAutomaticUpdates * This, VARIANT_BOOL * retval);
void __RPC_STUB IAutomaticUpdates_get_ServiceEnabled_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAutomaticUpdates_EnableService_Proxy(IAutomaticUpdates * This);
void __RPC_STUB IAutomaticUpdates_EnableService_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateIdentity_INTERFACE_DEFINED__
#define __IUpdateIdentity_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateIdentity;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("46297823-9940-4c09-aed9-cd3ea6d05968")IUpdateIdentity:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_RevisionNumber(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UpdateID(BSTR * retval) = 0;
};
#else
typedef struct IUpdateIdentityVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateIdentity * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateIdentity * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateIdentity * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateIdentity * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateIdentity * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateIdentity * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateIdentity * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_RevisionNumber)(IUpdateIdentity * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_UpdateID)(IUpdateIdentity * This, BSTR * retval);
	END_INTERFACE
} IUpdateIdentityVtbl;

interface IUpdateIdentity {
    CONST_VTBL struct IUpdateIdentityVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateIdentity_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateIdentity_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateIdentity_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateIdentity_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateIdentity_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateIdentity_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateIdentity_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateIdentity_get_RevisionNumber(This,retval)  (This)->lpVtbl -> get_RevisionNumber(This,retval)
#define IUpdateIdentity_get_UpdateID(This,retval)  (This)->lpVtbl -> get_UpdateID(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateIdentity_get_RevisionNumber_Proxy(IUpdateIdentity * This, LONG * retval);
void __RPC_STUB IUpdateIdentity_get_RevisionNumber_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateIdentity_get_UpdateID_Proxy(IUpdateIdentity * This, BSTR * retval);
void __RPC_STUB IUpdateIdentity_get_UpdateID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
#endif

#ifndef __IImageInformation_INTERFACE_DEFINED__
#define __IImageInformation_INTERFACE_DEFINED__

EXTERN_C const IID IID_IImageInformation;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7c907864-346c-4aeb-8f3f-57da289f969f")IImageInformation:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_AltText(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Height(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Source(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Width(LONG * retval) = 0;
};
#else
typedef struct IImageInformationVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IImageInformation * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IImageInformation * This);
	ULONG(STDMETHODCALLTYPE *Release)(IImageInformation * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IImageInformation * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IImageInformation * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IImageInformation * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IImageInformation * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_AltText)(IImageInformation * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Height)(IImageInformation * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_Source)(IImageInformation * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Width)(IImageInformation * This, LONG * retval);
	END_INTERFACE
} IImageInformationVtbl;

interface IImageInformation {
    CONST_VTBL struct IImageInformationVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IImageInformation_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IImageInformation_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IImageInformation_Release(This)  (This)->lpVtbl -> Release(This)
#define IImageInformation_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IImageInformation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IImageInformation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IImageInformation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IImageInformation_get_AltText(This,retval)  (This)->lpVtbl -> get_AltText(This,retval)
#define IImageInformation_get_Height(This,retval)  (This)->lpVtbl -> get_Height(This,retval)
#define IImageInformation_get_Source(This,retval)  (This)->lpVtbl -> get_Source(This,retval)
#define IImageInformation_get_Width(This,retval)  (This)->lpVtbl -> get_Width(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IImageInformation_get_AltText_Proxy(IImageInformation * This, BSTR * retval);
void __RPC_STUB IImageInformation_get_AltText_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IImageInformation_get_Height_Proxy(IImageInformation * This, LONG * retval);
void __RPC_STUB IImageInformation_get_Height_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IImageInformation_get_Source_Proxy(IImageInformation * This, BSTR * retval);
void __RPC_STUB IImageInformation_get_Source_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IImageInformation_get_Width_Proxy(IImageInformation * This, LONG * retval);
void __RPC_STUB IImageInformation_get_Width_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __ICategory_INTERFACE_DEFINED__
#define __ICategory_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICategory;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("81ddc1b8-9d35-47a6-b471-5b80f519223b")ICategory:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_CategoryID(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Children(ICategoryCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Description(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Image(IImageInformation ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Order(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Parent(ICategory ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Type(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Updates(IUpdateCollection ** retval) = 0;
};
#else
typedef struct ICategoryVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICategory * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(ICategory * This);
	ULONG(STDMETHODCALLTYPE *Release)(ICategory * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ICategory * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ICategory * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ICategory * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(ICategory * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Name)(ICategory * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_CategoryID)(ICategory * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Children)(ICategory * This, ICategoryCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Description)(ICategory * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Image)(ICategory * This, IImageInformation ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Order)(ICategory * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_Parent)(ICategory * This, ICategory ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Type)(ICategory * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Updates)(ICategory * This, IUpdateCollection ** retval);
	END_INTERFACE
} ICategoryVtbl;

interface ICategory {
    CONST_VTBL struct ICategoryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICategory_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ICategory_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define ICategory_Release(This)  (This)->lpVtbl -> Release(This)
#define ICategory_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define ICategory_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ICategory_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ICategory_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ICategory_get_Name(This,retval)  (This)->lpVtbl -> get_Name(This,retval)
#define ICategory_get_CategoryID(This,retval)  (This)->lpVtbl -> get_CategoryID(This,retval)
#define ICategory_get_Children(This,retval)  (This)->lpVtbl -> get_Children(This,retval)
#define ICategory_get_Description(This,retval)  (This)->lpVtbl -> get_Description(This,retval)
#define ICategory_get_Image(This,retval)  (This)->lpVtbl -> get_Image(This,retval)
#define ICategory_get_Order(This,retval)  (This)->lpVtbl -> get_Order(This,retval)
#define ICategory_get_Parent(This,retval)  (This)->lpVtbl -> get_Parent(This,retval)
#define ICategory_get_Type(This,retval)  (This)->lpVtbl -> get_Type(This,retval)
#define ICategory_get_Updates(This,retval)  (This)->lpVtbl -> get_Updates(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE ICategory_get_Name_Proxy(ICategory * This, BSTR * retval);
void __RPC_STUB ICategory_get_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ICategory_get_CategoryID_Proxy(ICategory * This, BSTR * retval);
void __RPC_STUB ICategory_get_CategoryID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ICategory_get_Children_Proxy(ICategory * This, ICategoryCollection ** retval);
void __RPC_STUB ICategory_get_Children_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ICategory_get_Description_Proxy(ICategory * This, BSTR * retval);
void __RPC_STUB ICategory_get_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ICategory_get_Image_Proxy(ICategory * This, IImageInformation ** retval);
void __RPC_STUB ICategory_get_Image_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ICategory_get_Order_Proxy(ICategory * This, LONG * retval);
void __RPC_STUB ICategory_get_Order_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ICategory_get_Parent_Proxy(ICategory * This, ICategory ** retval);
void __RPC_STUB ICategory_get_Parent_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ICategory_get_Type_Proxy(ICategory * This, BSTR * retval);
void __RPC_STUB ICategory_get_Type_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ICategory_get_Updates_Proxy(ICategory * This, IUpdateCollection ** retval);
void __RPC_STUB ICategory_get_Updates_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __ICategoryCollection_INTERFACE_DEFINED__
#define __ICategoryCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_ICategoryCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3a56bfb8-576c-43f7-9335-fe4838fd7e37")ICategoryCollection:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Item(LONG index, ICategory ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * retval) = 0;
};
#else
typedef struct ICategoryCollectionVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(ICategoryCollection * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(ICategoryCollection * This);
	ULONG(STDMETHODCALLTYPE *Release)(ICategoryCollection * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ICategoryCollection * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ICategoryCollection * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ICategoryCollection * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(ICategoryCollection * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Item)(ICategoryCollection * This, LONG index, ICategory ** retval);
	HRESULT(STDMETHODCALLTYPE *get__NewEnum)(ICategoryCollection * This, IUnknown ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Count)(ICategoryCollection * This, LONG * retval);
	END_INTERFACE
} ICategoryCollectionVtbl;

interface ICategoryCollection {
    CONST_VTBL struct ICategoryCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ICategoryCollection_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ICategoryCollection_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define ICategoryCollection_Release(This)  (This)->lpVtbl -> Release(This)
#define ICategoryCollection_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define ICategoryCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ICategoryCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ICategoryCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ICategoryCollection_get_Item(This,index,retval)  (This)->lpVtbl -> get_Item(This,index,retval)
#define ICategoryCollection_get__NewEnum(This,retval)  (This)->lpVtbl -> get__NewEnum(This,retval)
#define ICategoryCollection_get_Count(This,retval)  (This)->lpVtbl -> get_Count(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE ICategoryCollection_get_Item_Proxy(ICategoryCollection * This, LONG index, ICategory ** retval);
void __RPC_STUB ICategoryCollection_get_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ICategoryCollection_get__NewEnum_Proxy(ICategoryCollection * This, IUnknown ** retval);
void __RPC_STUB ICategoryCollection_get__NewEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ICategoryCollection_get_Count_Proxy(ICategoryCollection * This, LONG * retval);
void __RPC_STUB ICategoryCollection_get_Count_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IInstallationBehavior_INTERFACE_DEFINED__
#define __IInstallationBehavior_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInstallationBehavior;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("d9a59339-e245-4dbd-9686-4d5763e39624")IInstallationBehavior:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_CanRequestUserInput(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Impact(InstallationImpact * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_RebootBehavior(InstallationRebootBehavior * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_RequiresNetworkConnectivity(VARIANT_BOOL * retval) = 0;
};
#else
typedef struct IInstallationBehaviorVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInstallationBehavior * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IInstallationBehavior * This);
	ULONG(STDMETHODCALLTYPE *Release)(IInstallationBehavior * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IInstallationBehavior * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IInstallationBehavior * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IInstallationBehavior * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IInstallationBehavior * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_CanRequestUserInput)(IInstallationBehavior * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Impact)(IInstallationBehavior * This, InstallationImpact * retval);
	HRESULT(STDMETHODCALLTYPE *get_RebootBehavior)(IInstallationBehavior * This, InstallationRebootBehavior * retval);
	HRESULT(STDMETHODCALLTYPE *get_RequiresNetworkConnectivity)(IInstallationBehavior * This, VARIANT_BOOL * retval);
	END_INTERFACE
} IInstallationBehaviorVtbl;

interface IInstallationBehavior {
    CONST_VTBL struct IInstallationBehaviorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInstallationBehavior_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IInstallationBehavior_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IInstallationBehavior_Release(This)  (This)->lpVtbl -> Release(This)
#define IInstallationBehavior_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IInstallationBehavior_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IInstallationBehavior_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IInstallationBehavior_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IInstallationBehavior_get_CanRequestUserInput(This,retval)  (This)->lpVtbl -> get_CanRequestUserInput(This,retval)
#define IInstallationBehavior_get_Impact(This,retval)  (This)->lpVtbl -> get_Impact(This,retval)
#define IInstallationBehavior_get_RebootBehavior(This,retval)  (This)->lpVtbl -> get_RebootBehavior(This,retval)
#define IInstallationBehavior_get_RequiresNetworkConnectivity(This,retval)  (This)->lpVtbl -> get_RequiresNetworkConnectivity(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IInstallationBehavior_get_CanRequestUserInput_Proxy(IInstallationBehavior * This, VARIANT_BOOL * retval);
void __RPC_STUB IInstallationBehavior_get_CanRequestUserInput_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationBehavior_get_Impact_Proxy(IInstallationBehavior * This, InstallationImpact * retval);
void __RPC_STUB IInstallationBehavior_get_Impact_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationBehavior_get_RebootBehavior_Proxy(IInstallationBehavior * This, InstallationRebootBehavior * retval);
void __RPC_STUB IInstallationBehavior_get_RebootBehavior_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationBehavior_get_RequiresNetworkConnectivity_Proxy(IInstallationBehavior * This, VARIANT_BOOL * retval);
void __RPC_STUB IInstallationBehavior_get_RequiresNetworkConnectivity_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateDownloadContent_INTERFACE_DEFINED__
#define __IUpdateDownloadContent_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateDownloadContent;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("54a2cb2d-9a0c-48b6-8a50-9abb69ee2d02")IUpdateDownloadContent:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_DownloadUrl(BSTR * retval) = 0;
};
#else
typedef struct IUpdateDownloadContentVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateDownloadContent * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateDownloadContent * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateDownloadContent * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateDownloadContent * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateDownloadContent * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateDownloadContent * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateDownloadContent * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_DownloadUrl)(IUpdateDownloadContent * This, BSTR * retval);
	END_INTERFACE
} IUpdateDownloadContentVtbl;

interface IUpdateDownloadContent {
    CONST_VTBL struct IUpdateDownloadContentVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateDownloadContent_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateDownloadContent_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateDownloadContent_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateDownloadContent_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateDownloadContent_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateDownloadContent_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateDownloadContent_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateDownloadContent_get_DownloadUrl(This,retval)  (This)->lpVtbl -> get_DownloadUrl(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateDownloadContent_get_DownloadUrl_Proxy(IUpdateDownloadContent * This, BSTR * retval);
void __RPC_STUB IUpdateDownloadContent_get_DownloadUrl_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateDownloadContentCollection_INTERFACE_DEFINED__
#define __IUpdateDownloadContentCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateDownloadContentCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("bc5513c8-b3b8-4bf7-a4d4-361c0d8c88ba")IUpdateDownloadContentCollection:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Item(LONG index, IUpdateDownloadContent ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * retval) = 0;
};
#else
typedef struct IUpdateDownloadContentCollectionVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateDownloadContentCollection * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateDownloadContentCollection * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateDownloadContentCollection * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateDownloadContentCollection * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateDownloadContentCollection * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateDownloadContentCollection * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateDownloadContentCollection * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Item)(IUpdateDownloadContentCollection * This, LONG index, IUpdateDownloadContent ** retval);
	HRESULT(STDMETHODCALLTYPE *get__NewEnum)(IUpdateDownloadContentCollection * This, IUnknown ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Count)(IUpdateDownloadContentCollection * This, LONG * retval);
	END_INTERFACE
} IUpdateDownloadContentCollectionVtbl;

interface IUpdateDownloadContentCollection {
    CONST_VTBL struct IUpdateDownloadContentCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateDownloadContentCollection_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateDownloadContentCollection_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateDownloadContentCollection_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateDownloadContentCollection_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateDownloadContentCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateDownloadContentCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateDownloadContentCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateDownloadContentCollection_get_Item(This,index,retval)  (This)->lpVtbl -> get_Item(This,index,retval)
#define IUpdateDownloadContentCollection_get__NewEnum(This,retval)  (This)->lpVtbl -> get__NewEnum(This,retval)
#define IUpdateDownloadContentCollection_get_Count(This,retval)  (This)->lpVtbl -> get_Count(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateDownloadContentCollection_get_Item_Proxy(IUpdateDownloadContentCollection * This, LONG index, IUpdateDownloadContent ** retval);
void __RPC_STUB IUpdateDownloadContentCollection_get_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateDownloadContentCollection_get__NewEnum_Proxy(IUpdateDownloadContentCollection * This, IUnknown ** retval);
void __RPC_STUB IUpdateDownloadContentCollection_get__NewEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateDownloadContentCollection_get_Count_Proxy(IUpdateDownloadContentCollection * This, LONG * retval);
void __RPC_STUB IUpdateDownloadContentCollection_get_Count_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdate_INTERFACE_DEFINED__
#define __IUpdate_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdate;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("6a92b07a-d821-4682-b423-5c805022cc4d")IUpdate:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Title(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_AutoSelectOnWebSites(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_BundledUpdates(IUpdateCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_CanRequireSource(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Categories(ICategoryCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Deadline(VARIANT * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DeltaCompressedContentAvailable(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DeltaCompressedContentPreferred(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Description(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_EulaAccepted(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_EulaText(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_HandlerID(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Identity(IUpdateIdentity ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Image(IImageInformation ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_InstallationBehavior(IInstallationBehavior ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsBeta(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsDownloaded(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsHidden(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_IsHidden(VARIANT_BOOL value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsInstalled(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsMandatory(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsUninstallable(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Languages(IStringCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_LastDeploymentChangeTime(DATE * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_MaxDownloadSize(DECIMAL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_MinDownloadSize(DECIMAL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_MoreInfoUrls(IStringCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_MsrcSeverity(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_RecommendedCpuSpeed(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_RecommendedHardDiskSpace(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_RecommendedMemory(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ReleaseNotes(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_SecurityBulletinIDs(IStringCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_SupersededUpdateIDs(IStringCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_SupportUrl(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Type(UpdateType * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UninstallationNotes(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UninstallationBehavior(IInstallationBehavior ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UninstallationSteps(IStringCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_KBArticleIDs(IStringCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE AcceptEula(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DeploymentAction(DeploymentAction * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE CopyFromCache(BSTR path, VARIANT_BOOL toExtractCabFiles) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DownloadPriority(DownloadPriority * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DownloadContents(IUpdateDownloadContentCollection ** retval) = 0;
};
#else
typedef struct IUpdateVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdate * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdate * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdate * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdate * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdate * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdate * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdate * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Title)(IUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_AutoSelectOnWebSites)(IUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_BundledUpdates)(IUpdate * This, IUpdateCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_CanRequireSource)(IUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Categories)(IUpdate * This, ICategoryCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Deadline)(IUpdate * This, VARIANT * retval);
	HRESULT(STDMETHODCALLTYPE *get_DeltaCompressedContentAvailable)(IUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_DeltaCompressedContentPreferred)(IUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_EulaAccepted)(IUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_EulaText)(IUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_HandlerID)(IUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Identity)(IUpdate * This, IUpdateIdentity ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Image)(IUpdate * This, IImageInformation ** retval);
	HRESULT(STDMETHODCALLTYPE *get_InstallationBehavior)(IUpdate * This, IInstallationBehavior ** retval);
	HRESULT(STDMETHODCALLTYPE *get_IsBeta)(IUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsDownloaded)(IUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsHidden)(IUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_IsHidden)(IUpdate * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *get_IsInstalled)(IUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsMandatory)(IUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsUninstallable)(IUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Languages)(IUpdate * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_LastDeploymentChangeTime)(IUpdate * This, DATE * retval);
	HRESULT(STDMETHODCALLTYPE *get_MaxDownloadSize)(IUpdate * This, DECIMAL * retval);
	HRESULT(STDMETHODCALLTYPE *get_MinDownloadSize)(IUpdate * This, DECIMAL * retval);
	HRESULT(STDMETHODCALLTYPE *get_MoreInfoUrls)(IUpdate * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_MsrcSeverity)(IUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_RecommendedCpuSpeed)(IUpdate * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_RecommendedHardDiskSpace)(IUpdate * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_RecommendedMemory)(IUpdate * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_ReleaseNotes)(IUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_SecurityBulletinIDs)(IUpdate * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_SupersededUpdateIDs)(IUpdate * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_SupportUrl)(IUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Type)(IUpdate * This, UpdateType * retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationNotes)(IUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationBehavior)(IUpdate * This, IInstallationBehavior ** retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationSteps)(IUpdate * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_KBArticleIDs)(IUpdate * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *AcceptEula)(IUpdate * This);
	HRESULT(STDMETHODCALLTYPE *get_DeploymentAction)(IUpdate * This, DeploymentAction * retval);
	HRESULT(STDMETHODCALLTYPE *CopyFromCache)(IUpdate * This, BSTR path, VARIANT_BOOL toExtractCabFiles);
	HRESULT(STDMETHODCALLTYPE *get_DownloadPriority)(IUpdate * This, DownloadPriority * retval);
	HRESULT(STDMETHODCALLTYPE *get_DownloadContents)(IUpdate * This, IUpdateDownloadContentCollection ** retval);
	END_INTERFACE
} IUpdateVtbl;

interface IUpdate {
    CONST_VTBL struct IUpdateVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdate_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdate_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdate_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdate_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdate_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdate_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdate_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdate_get_Title(This,retval)  (This)->lpVtbl -> get_Title(This,retval)
#define IUpdate_get_AutoSelectOnWebSites(This,retval)  (This)->lpVtbl -> get_AutoSelectOnWebSites(This,retval)
#define IUpdate_get_BundledUpdates(This,retval)  (This)->lpVtbl -> get_BundledUpdates(This,retval)
#define IUpdate_get_CanRequireSource(This,retval)  (This)->lpVtbl -> get_CanRequireSource(This,retval)
#define IUpdate_get_Categories(This,retval)  (This)->lpVtbl -> get_Categories(This,retval)
#define IUpdate_get_Deadline(This,retval)  (This)->lpVtbl -> get_Deadline(This,retval)
#define IUpdate_get_DeltaCompressedContentAvailable(This,retval)  (This)->lpVtbl -> get_DeltaCompressedContentAvailable(This,retval)
#define IUpdate_get_DeltaCompressedContentPreferred(This,retval)  (This)->lpVtbl -> get_DeltaCompressedContentPreferred(This,retval)
#define IUpdate_get_Description(This,retval)  (This)->lpVtbl -> get_Description(This,retval)
#define IUpdate_get_EulaAccepted(This,retval)  (This)->lpVtbl -> get_EulaAccepted(This,retval)
#define IUpdate_get_EulaText(This,retval)  (This)->lpVtbl -> get_EulaText(This,retval)
#define IUpdate_get_HandlerID(This,retval)  (This)->lpVtbl -> get_HandlerID(This,retval)
#define IUpdate_get_Identity(This,retval)  (This)->lpVtbl -> get_Identity(This,retval)
#define IUpdate_get_Image(This,retval)  (This)->lpVtbl -> get_Image(This,retval)
#define IUpdate_get_InstallationBehavior(This,retval)  (This)->lpVtbl -> get_InstallationBehavior(This,retval)
#define IUpdate_get_IsBeta(This,retval)  (This)->lpVtbl -> get_IsBeta(This,retval)
#define IUpdate_get_IsDownloaded(This,retval)  (This)->lpVtbl -> get_IsDownloaded(This,retval)
#define IUpdate_get_IsHidden(This,retval)  (This)->lpVtbl -> get_IsHidden(This,retval)
#define IUpdate_put_IsHidden(This,value)  (This)->lpVtbl -> put_IsHidden(This,value)
#define IUpdate_get_IsInstalled(This,retval)  (This)->lpVtbl -> get_IsInstalled(This,retval)
#define IUpdate_get_IsMandatory(This,retval)  (This)->lpVtbl -> get_IsMandatory(This,retval)
#define IUpdate_get_IsUninstallable(This,retval)  (This)->lpVtbl -> get_IsUninstallable(This,retval)
#define IUpdate_get_Languages(This,retval)  (This)->lpVtbl -> get_Languages(This,retval)
#define IUpdate_get_LastDeploymentChangeTime(This,retval)  (This)->lpVtbl -> get_LastDeploymentChangeTime(This,retval)
#define IUpdate_get_MaxDownloadSize(This,retval)  (This)->lpVtbl -> get_MaxDownloadSize(This,retval)
#define IUpdate_get_MinDownloadSize(This,retval)  (This)->lpVtbl -> get_MinDownloadSize(This,retval)
#define IUpdate_get_MoreInfoUrls(This,retval)  (This)->lpVtbl -> get_MoreInfoUrls(This,retval)
#define IUpdate_get_MsrcSeverity(This,retval)  (This)->lpVtbl -> get_MsrcSeverity(This,retval)
#define IUpdate_get_RecommendedCpuSpeed(This,retval)  (This)->lpVtbl -> get_RecommendedCpuSpeed(This,retval)
#define IUpdate_get_RecommendedHardDiskSpace(This,retval)  (This)->lpVtbl -> get_RecommendedHardDiskSpace(This,retval)
#define IUpdate_get_RecommendedMemory(This,retval)  (This)->lpVtbl -> get_RecommendedMemory(This,retval)
#define IUpdate_get_ReleaseNotes(This,retval)  (This)->lpVtbl -> get_ReleaseNotes(This,retval)
#define IUpdate_get_SecurityBulletinIDs(This,retval)  (This)->lpVtbl -> get_SecurityBulletinIDs(This,retval)
#define IUpdate_get_SupersededUpdateIDs(This,retval)  (This)->lpVtbl -> get_SupersededUpdateIDs(This,retval)
#define IUpdate_get_SupportUrl(This,retval)  (This)->lpVtbl -> get_SupportUrl(This,retval)
#define IUpdate_get_Type(This,retval)  (This)->lpVtbl -> get_Type(This,retval)
#define IUpdate_get_UninstallationNotes(This,retval)  (This)->lpVtbl -> get_UninstallationNotes(This,retval)
#define IUpdate_get_UninstallationBehavior(This,retval)  (This)->lpVtbl -> get_UninstallationBehavior(This,retval)
#define IUpdate_get_UninstallationSteps(This,retval)  (This)->lpVtbl -> get_UninstallationSteps(This,retval)
#define IUpdate_get_KBArticleIDs(This,retval)  (This)->lpVtbl -> get_KBArticleIDs(This,retval)
#define IUpdate_AcceptEula(This)  (This)->lpVtbl -> AcceptEula(This)
#define IUpdate_get_DeploymentAction(This,retval)  (This)->lpVtbl -> get_DeploymentAction(This,retval)
#define IUpdate_CopyFromCache(This,path,toExtractCabFiles)  (This)->lpVtbl -> CopyFromCache(This,path,toExtractCabFiles)
#define IUpdate_get_DownloadPriority(This,retval)  (This)->lpVtbl -> get_DownloadPriority(This,retval)
#define IUpdate_get_DownloadContents(This,retval)  (This)->lpVtbl -> get_DownloadContents(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdate_get_Title_Proxy(IUpdate * This, BSTR * retval);
void __RPC_STUB IUpdate_get_Title_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_AutoSelectOnWebSites_Proxy(IUpdate * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdate_get_AutoSelectOnWebSites_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_BundledUpdates_Proxy(IUpdate * This, IUpdateCollection ** retval);
void __RPC_STUB IUpdate_get_BundledUpdates_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_CanRequireSource_Proxy(IUpdate * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdate_get_CanRequireSource_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_Categories_Proxy(IUpdate * This, ICategoryCollection ** retval);
void __RPC_STUB IUpdate_get_Categories_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_Deadline_Proxy(IUpdate * This, VARIANT * retval);
void __RPC_STUB IUpdate_get_Deadline_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_DeltaCompressedContentAvailable_Proxy(IUpdate * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdate_get_DeltaCompressedContentAvailable_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_DeltaCompressedContentPreferred_Proxy(IUpdate * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdate_get_DeltaCompressedContentPreferred_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_Description_Proxy(IUpdate * This, BSTR * retval);
void __RPC_STUB IUpdate_get_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_EulaAccepted_Proxy(IUpdate * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdate_get_EulaAccepted_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_EulaText_Proxy(IUpdate * This, BSTR * retval);
void __RPC_STUB IUpdate_get_EulaText_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_HandlerID_Proxy(IUpdate * This, BSTR * retval);
void __RPC_STUB IUpdate_get_HandlerID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_Identity_Proxy(IUpdate * This, IUpdateIdentity ** retval);
void __RPC_STUB IUpdate_get_Identity_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_Image_Proxy(IUpdate * This, IImageInformation ** retval);
void __RPC_STUB IUpdate_get_Image_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_InstallationBehavior_Proxy(IUpdate * This, IInstallationBehavior ** retval);
void __RPC_STUB IUpdate_get_InstallationBehavior_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_IsBeta_Proxy(IUpdate * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdate_get_IsBeta_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_IsDownloaded_Proxy(IUpdate * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdate_get_IsDownloaded_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_IsHidden_Proxy(IUpdate * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdate_get_IsHidden_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_put_IsHidden_Proxy(IUpdate * This, VARIANT_BOOL value);
void __RPC_STUB IUpdate_put_IsHidden_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_IsInstalled_Proxy(IUpdate * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdate_get_IsInstalled_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_IsMandatory_Proxy(IUpdate * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdate_get_IsMandatory_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_IsUninstallable_Proxy(IUpdate * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdate_get_IsUninstallable_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_Languages_Proxy(IUpdate * This, IStringCollection ** retval);
void __RPC_STUB IUpdate_get_Languages_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_LastDeploymentChangeTime_Proxy(IUpdate * This, DATE * retval);
void __RPC_STUB IUpdate_get_LastDeploymentChangeTime_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_MaxDownloadSize_Proxy(IUpdate * This, DECIMAL * retval);
void __RPC_STUB IUpdate_get_MaxDownloadSize_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_MinDownloadSize_Proxy(IUpdate * This, DECIMAL * retval);
void __RPC_STUB IUpdate_get_MinDownloadSize_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_MoreInfoUrls_Proxy(IUpdate * This, IStringCollection ** retval);
void __RPC_STUB IUpdate_get_MoreInfoUrls_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_MsrcSeverity_Proxy(IUpdate * This, BSTR * retval);
void __RPC_STUB IUpdate_get_MsrcSeverity_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_RecommendedCpuSpeed_Proxy(IUpdate * This, LONG * retval);
void __RPC_STUB IUpdate_get_RecommendedCpuSpeed_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_RecommendedHardDiskSpace_Proxy(IUpdate * This, LONG * retval);
void __RPC_STUB IUpdate_get_RecommendedHardDiskSpace_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_RecommendedMemory_Proxy(IUpdate * This, LONG * retval);
void __RPC_STUB IUpdate_get_RecommendedMemory_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_ReleaseNotes_Proxy(IUpdate * This, BSTR * retval);
void __RPC_STUB IUpdate_get_ReleaseNotes_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_SecurityBulletinIDs_Proxy(IUpdate * This, IStringCollection ** retval);
void __RPC_STUB IUpdate_get_SecurityBulletinIDs_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_SupersededUpdateIDs_Proxy(IUpdate * This, IStringCollection ** retval);
void __RPC_STUB IUpdate_get_SupersededUpdateIDs_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_SupportUrl_Proxy(IUpdate * This, BSTR * retval);
void __RPC_STUB IUpdate_get_SupportUrl_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_Type_Proxy(IUpdate * This, UpdateType * retval);
void __RPC_STUB IUpdate_get_Type_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_UninstallationNotes_Proxy(IUpdate * This, BSTR * retval);
void __RPC_STUB IUpdate_get_UninstallationNotes_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_UninstallationBehavior_Proxy(IUpdate * This, IInstallationBehavior ** retval);
void __RPC_STUB IUpdate_get_UninstallationBehavior_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_UninstallationSteps_Proxy(IUpdate * This, IStringCollection ** retval);
void __RPC_STUB IUpdate_get_UninstallationSteps_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_KBArticleIDs_Proxy(IUpdate * This, IStringCollection ** retval);
void __RPC_STUB IUpdate_get_KBArticleIDs_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_AcceptEula_Proxy(IUpdate * This);
void __RPC_STUB IUpdate_AcceptEula_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_DeploymentAction_Proxy(IUpdate * This, DeploymentAction * retval);
void __RPC_STUB IUpdate_get_DeploymentAction_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_CopyFromCache_Proxy(IUpdate * This, BSTR path, VARIANT_BOOL toExtractCabFiles);
void __RPC_STUB IUpdate_CopyFromCache_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_DownloadPriority_Proxy(IUpdate * This, DownloadPriority * retval);
void __RPC_STUB IUpdate_get_DownloadPriority_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate_get_DownloadContents_Proxy(IUpdate * This, IUpdateDownloadContentCollection ** retval);
void __RPC_STUB IUpdate_get_DownloadContents_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IWindowsDriverUpdate_INTERFACE_DEFINED__
#define __IWindowsDriverUpdate_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWindowsDriverUpdate;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("b383cd1a-5ce9-4504-9f63-764b1236f191")IWindowsDriverUpdate:public IUpdate
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_DriverClass(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DriverHardwareID(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DriverManufacturer(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DriverModel(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DriverProvider(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DriverVerDate(DATE * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DeviceProblemNumber(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DeviceStatus(LONG * retval) = 0;
};
#else
typedef struct IWindowsDriverUpdateVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IWindowsDriverUpdate * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IWindowsDriverUpdate * This);
	ULONG(STDMETHODCALLTYPE *Release)(IWindowsDriverUpdate * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IWindowsDriverUpdate * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IWindowsDriverUpdate * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IWindowsDriverUpdate * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IWindowsDriverUpdate * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Title)(IWindowsDriverUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_AutoSelectOnWebSites)(IWindowsDriverUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_BundledUpdates)(IWindowsDriverUpdate * This, IUpdateCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_CanRequireSource)(IWindowsDriverUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Categories)(IWindowsDriverUpdate * This, ICategoryCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Deadline)(IWindowsDriverUpdate * This, VARIANT * retval);
	HRESULT(STDMETHODCALLTYPE *get_DeltaCompressedContentAvailable)(IWindowsDriverUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_DeltaCompressedContentPreferred)(IWindowsDriverUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IWindowsDriverUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_EulaAccepted)(IWindowsDriverUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_EulaText)(IWindowsDriverUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_HandlerID)(IWindowsDriverUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Identity)(IWindowsDriverUpdate * This, IUpdateIdentity ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Image)(IWindowsDriverUpdate * This, IImageInformation ** retval);
	HRESULT(STDMETHODCALLTYPE *get_InstallationBehavior)(IWindowsDriverUpdate * This, IInstallationBehavior ** retval);
	HRESULT(STDMETHODCALLTYPE *get_IsBeta)(IWindowsDriverUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsDownloaded)(IWindowsDriverUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsHidden)(IWindowsDriverUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_IsHidden)(IWindowsDriverUpdate * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *get_IsInstalled)(IWindowsDriverUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsMandatory)(IWindowsDriverUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsUninstallable)(IWindowsDriverUpdate * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Languages)(IWindowsDriverUpdate * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_LastDeploymentChangeTime)(IWindowsDriverUpdate * This, DATE * retval);
	HRESULT(STDMETHODCALLTYPE *get_MaxDownloadSize)(IWindowsDriverUpdate * This, DECIMAL * retval);
	HRESULT(STDMETHODCALLTYPE *get_MinDownloadSize)(IWindowsDriverUpdate * This, DECIMAL * retval);
	HRESULT(STDMETHODCALLTYPE *get_MoreInfoUrls)(IWindowsDriverUpdate * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_MsrcSeverity)(IWindowsDriverUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_RecommendedCpuSpeed)(IWindowsDriverUpdate * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_RecommendedHardDiskSpace)(IWindowsDriverUpdate * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_RecommendedMemory)(IWindowsDriverUpdate * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_ReleaseNotes)(IWindowsDriverUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_SecurityBulletinIDs)(IWindowsDriverUpdate * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_SupersededUpdateIDs)(IWindowsDriverUpdate * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_SupportUrl)(IWindowsDriverUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Type)(IWindowsDriverUpdate * This, UpdateType * retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationNotes)(IWindowsDriverUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationBehavior)(IWindowsDriverUpdate * This, IInstallationBehavior ** retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationSteps)(IWindowsDriverUpdate * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_KBArticleIDs)(IWindowsDriverUpdate * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *AcceptEula)(IWindowsDriverUpdate * This);
	HRESULT(STDMETHODCALLTYPE *get_DeploymentAction)(IWindowsDriverUpdate * This, DeploymentAction * retval);
	HRESULT(STDMETHODCALLTYPE *CopyFromCache)(IWindowsDriverUpdate * This, BSTR path, VARIANT_BOOL toExtractCabFiles);
	HRESULT(STDMETHODCALLTYPE *get_DownloadPriority)(IWindowsDriverUpdate * This, DownloadPriority * retval);
	HRESULT(STDMETHODCALLTYPE *get_DownloadContents)(IWindowsDriverUpdate * This, IUpdateDownloadContentCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_DriverClass)(IWindowsDriverUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_DriverHardwareID)(IWindowsDriverUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_DriverManufacturer)(IWindowsDriverUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_DriverModel)(IWindowsDriverUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_DriverProvider)(IWindowsDriverUpdate * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_DriverVerDate)(IWindowsDriverUpdate * This, DATE * retval);
	HRESULT(STDMETHODCALLTYPE *get_DeviceProblemNumber)(IWindowsDriverUpdate * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_DeviceStatus)(IWindowsDriverUpdate * This, LONG * retval);
	END_INTERFACE
} IWindowsDriverUpdateVtbl;

interface IWindowsDriverUpdate {
    CONST_VTBL struct IWindowsDriverUpdateVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWindowsDriverUpdate_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IWindowsDriverUpdate_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IWindowsDriverUpdate_Release(This)  (This)->lpVtbl -> Release(This)
#define IWindowsDriverUpdate_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IWindowsDriverUpdate_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IWindowsDriverUpdate_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IWindowsDriverUpdate_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IWindowsDriverUpdate_get_Title(This,retval)  (This)->lpVtbl -> get_Title(This,retval)
#define IWindowsDriverUpdate_get_AutoSelectOnWebSites(This,retval)  (This)->lpVtbl -> get_AutoSelectOnWebSites(This,retval)
#define IWindowsDriverUpdate_get_BundledUpdates(This,retval)  (This)->lpVtbl -> get_BundledUpdates(This,retval)
#define IWindowsDriverUpdate_get_CanRequireSource(This,retval)  (This)->lpVtbl -> get_CanRequireSource(This,retval)
#define IWindowsDriverUpdate_get_Categories(This,retval)  (This)->lpVtbl -> get_Categories(This,retval)
#define IWindowsDriverUpdate_get_Deadline(This,retval)  (This)->lpVtbl -> get_Deadline(This,retval)
#define IWindowsDriverUpdate_get_DeltaCompressedContentAvailable(This,retval)  (This)->lpVtbl -> get_DeltaCompressedContentAvailable(This,retval)
#define IWindowsDriverUpdate_get_DeltaCompressedContentPreferred(This,retval)  (This)->lpVtbl -> get_DeltaCompressedContentPreferred(This,retval)
#define IWindowsDriverUpdate_get_Description(This,retval)  (This)->lpVtbl -> get_Description(This,retval)
#define IWindowsDriverUpdate_get_EulaAccepted(This,retval)  (This)->lpVtbl -> get_EulaAccepted(This,retval)
#define IWindowsDriverUpdate_get_EulaText(This,retval)  (This)->lpVtbl -> get_EulaText(This,retval)
#define IWindowsDriverUpdate_get_HandlerID(This,retval)  (This)->lpVtbl -> get_HandlerID(This,retval)
#define IWindowsDriverUpdate_get_Identity(This,retval)  (This)->lpVtbl -> get_Identity(This,retval)
#define IWindowsDriverUpdate_get_Image(This,retval)  (This)->lpVtbl -> get_Image(This,retval)
#define IWindowsDriverUpdate_get_InstallationBehavior(This,retval)  (This)->lpVtbl -> get_InstallationBehavior(This,retval)
#define IWindowsDriverUpdate_get_IsBeta(This,retval)  (This)->lpVtbl -> get_IsBeta(This,retval)
#define IWindowsDriverUpdate_get_IsDownloaded(This,retval)  (This)->lpVtbl -> get_IsDownloaded(This,retval)
#define IWindowsDriverUpdate_get_IsHidden(This,retval)  (This)->lpVtbl -> get_IsHidden(This,retval)
#define IWindowsDriverUpdate_put_IsHidden(This,value)  (This)->lpVtbl -> put_IsHidden(This,value)
#define IWindowsDriverUpdate_get_IsInstalled(This,retval)  (This)->lpVtbl -> get_IsInstalled(This,retval)
#define IWindowsDriverUpdate_get_IsMandatory(This,retval)  (This)->lpVtbl -> get_IsMandatory(This,retval)
#define IWindowsDriverUpdate_get_IsUninstallable(This,retval)  (This)->lpVtbl -> get_IsUninstallable(This,retval)
#define IWindowsDriverUpdate_get_Languages(This,retval)  (This)->lpVtbl -> get_Languages(This,retval)
#define IWindowsDriverUpdate_get_LastDeploymentChangeTime(This,retval)  (This)->lpVtbl -> get_LastDeploymentChangeTime(This,retval)
#define IWindowsDriverUpdate_get_MaxDownloadSize(This,retval)  (This)->lpVtbl -> get_MaxDownloadSize(This,retval)
#define IWindowsDriverUpdate_get_MinDownloadSize(This,retval)  (This)->lpVtbl -> get_MinDownloadSize(This,retval)
#define IWindowsDriverUpdate_get_MoreInfoUrls(This,retval)  (This)->lpVtbl -> get_MoreInfoUrls(This,retval)
#define IWindowsDriverUpdate_get_MsrcSeverity(This,retval)  (This)->lpVtbl -> get_MsrcSeverity(This,retval)
#define IWindowsDriverUpdate_get_RecommendedCpuSpeed(This,retval)  (This)->lpVtbl -> get_RecommendedCpuSpeed(This,retval)
#define IWindowsDriverUpdate_get_RecommendedHardDiskSpace(This,retval)  (This)->lpVtbl -> get_RecommendedHardDiskSpace(This,retval)
#define IWindowsDriverUpdate_get_RecommendedMemory(This,retval)  (This)->lpVtbl -> get_RecommendedMemory(This,retval)
#define IWindowsDriverUpdate_get_ReleaseNotes(This,retval)  (This)->lpVtbl -> get_ReleaseNotes(This,retval)
#define IWindowsDriverUpdate_get_SecurityBulletinIDs(This,retval)  (This)->lpVtbl -> get_SecurityBulletinIDs(This,retval)
#define IWindowsDriverUpdate_get_SupersededUpdateIDs(This,retval)  (This)->lpVtbl -> get_SupersededUpdateIDs(This,retval)
#define IWindowsDriverUpdate_get_SupportUrl(This,retval)  (This)->lpVtbl -> get_SupportUrl(This,retval)
#define IWindowsDriverUpdate_get_Type(This,retval)  (This)->lpVtbl -> get_Type(This,retval)
#define IWindowsDriverUpdate_get_UninstallationNotes(This,retval)  (This)->lpVtbl -> get_UninstallationNotes(This,retval)
#define IWindowsDriverUpdate_get_UninstallationBehavior(This,retval)  (This)->lpVtbl -> get_UninstallationBehavior(This,retval)
#define IWindowsDriverUpdate_get_UninstallationSteps(This,retval)  (This)->lpVtbl -> get_UninstallationSteps(This,retval)
#define IWindowsDriverUpdate_get_KBArticleIDs(This,retval)  (This)->lpVtbl -> get_KBArticleIDs(This,retval)
#define IWindowsDriverUpdate_AcceptEula(This)  (This)->lpVtbl -> AcceptEula(This)
#define IWindowsDriverUpdate_get_DeploymentAction(This,retval)  (This)->lpVtbl -> get_DeploymentAction(This,retval)
#define IWindowsDriverUpdate_CopyFromCache(This,path,toExtractCabFiles)  (This)->lpVtbl -> CopyFromCache(This,path,toExtractCabFiles)
#define IWindowsDriverUpdate_get_DownloadPriority(This,retval)  (This)->lpVtbl -> get_DownloadPriority(This,retval)
#define IWindowsDriverUpdate_get_DownloadContents(This,retval)  (This)->lpVtbl -> get_DownloadContents(This,retval)
#define IWindowsDriverUpdate_get_DriverClass(This,retval)  (This)->lpVtbl -> get_DriverClass(This,retval)
#define IWindowsDriverUpdate_get_DriverHardwareID(This,retval)  (This)->lpVtbl -> get_DriverHardwareID(This,retval)
#define IWindowsDriverUpdate_get_DriverManufacturer(This,retval)  (This)->lpVtbl -> get_DriverManufacturer(This,retval)
#define IWindowsDriverUpdate_get_DriverModel(This,retval)  (This)->lpVtbl -> get_DriverModel(This,retval)
#define IWindowsDriverUpdate_get_DriverProvider(This,retval)  (This)->lpVtbl -> get_DriverProvider(This,retval)
#define IWindowsDriverUpdate_get_DriverVerDate(This,retval)  (This)->lpVtbl -> get_DriverVerDate(This,retval)
#define IWindowsDriverUpdate_get_DeviceProblemNumber(This,retval)  (This)->lpVtbl -> get_DeviceProblemNumber(This,retval)
#define IWindowsDriverUpdate_get_DeviceStatus(This,retval)  (This)->lpVtbl -> get_DeviceStatus(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IWindowsDriverUpdate_get_DriverClass_Proxy(IWindowsDriverUpdate * This, BSTR * retval);
void __RPC_STUB IWindowsDriverUpdate_get_DriverClass_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWindowsDriverUpdate_get_DriverHardwareID_Proxy(IWindowsDriverUpdate * This, BSTR * retval);
void __RPC_STUB IWindowsDriverUpdate_get_DriverHardwareID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWindowsDriverUpdate_get_DriverManufacturer_Proxy(IWindowsDriverUpdate * This, BSTR * retval);
void __RPC_STUB IWindowsDriverUpdate_get_DriverManufacturer_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWindowsDriverUpdate_get_DriverModel_Proxy(IWindowsDriverUpdate * This, BSTR * retval);
void __RPC_STUB IWindowsDriverUpdate_get_DriverModel_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWindowsDriverUpdate_get_DriverProvider_Proxy(IWindowsDriverUpdate * This, BSTR * retval);
void __RPC_STUB IWindowsDriverUpdate_get_DriverProvider_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWindowsDriverUpdate_get_DriverVerDate_Proxy(IWindowsDriverUpdate * This, DATE * retval);
void __RPC_STUB IWindowsDriverUpdate_get_DriverVerDate_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWindowsDriverUpdate_get_DeviceProblemNumber_Proxy(IWindowsDriverUpdate * This, LONG * retval);
void __RPC_STUB IWindowsDriverUpdate_get_DeviceProblemNumber_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWindowsDriverUpdate_get_DeviceStatus_Proxy(IWindowsDriverUpdate * This, LONG * retval);
void __RPC_STUB IWindowsDriverUpdate_get_DeviceStatus_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdate2_INTERFACE_DEFINED__
#define __IUpdate2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdate2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("84617776-dbeb-4881-b410-aa294227c3fd")IUpdate2:public IUpdate
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_InstallState(InstalledState * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE CopyToCache(IStringCollection * pFiles, VARIANT_BOOL ignoreDigestMismatch) = 0;
};
#else
typedef struct IUpdate2Vtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdate2 * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdate2 * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdate2 * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdate2 * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdate2 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdate2 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdate2 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Title)(IUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_AutoSelectOnWebSites)(IUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_BundledUpdates)(IUpdate2 * This, IUpdateCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_CanRequireSource)(IUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Categories)(IUpdate2 * This, ICategoryCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Deadline)(IUpdate2 * This, VARIANT * retval);
	HRESULT(STDMETHODCALLTYPE *get_DeltaCompressedContentAvailable)(IUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_DeltaCompressedContentPreferred)(IUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_EulaAccepted)(IUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_EulaText)(IUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_HandlerID)(IUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Identity)(IUpdate2 * This, IUpdateIdentity ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Image)(IUpdate2 * This, IImageInformation ** retval);
	HRESULT(STDMETHODCALLTYPE *get_InstallationBehavior)(IUpdate2 * This, IInstallationBehavior ** retval);
	HRESULT(STDMETHODCALLTYPE *get_IsBeta)(IUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsDownloaded)(IUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsHidden)(IUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_IsHidden)(IUpdate2 * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *get_IsInstalled)(IUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsMandatory)(IUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsUninstallable)(IUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Languages)(IUpdate2 * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_LastDeploymentChangeTime)(IUpdate2 * This, DATE * retval);
	HRESULT(STDMETHODCALLTYPE *get_MaxDownloadSize)(IUpdate2 * This, DECIMAL * retval);
	HRESULT(STDMETHODCALLTYPE *get_MinDownloadSize)(IUpdate2 * This, DECIMAL * retval);
	HRESULT(STDMETHODCALLTYPE *get_MoreInfoUrls)(IUpdate2 * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_MsrcSeverity)(IUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_RecommendedCpuSpeed)(IUpdate2 * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_RecommendedHardDiskSpace)(IUpdate2 * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_RecommendedMemory)(IUpdate2 * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_ReleaseNotes)(IUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_SecurityBulletinIDs)(IUpdate2 * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_SupersededUpdateIDs)(IUpdate2 * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_SupportUrl)(IUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Type)(IUpdate2 * This, UpdateType * retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationNotes)(IUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationBehavior)(IUpdate2 * This, IInstallationBehavior ** retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationSteps)(IUpdate2 * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_KBArticleIDs)(IUpdate2 * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *AcceptEula)(IUpdate2 * This);
	HRESULT(STDMETHODCALLTYPE *get_DeploymentAction)(IUpdate2 * This, DeploymentAction * retval);
	HRESULT(STDMETHODCALLTYPE *CopyFromCache)(IUpdate2 * This, BSTR path, VARIANT_BOOL toExtractCabFiles);
	HRESULT(STDMETHODCALLTYPE *get_DownloadPriority)(IUpdate2 * This, DownloadPriority * retval);
	HRESULT(STDMETHODCALLTYPE *get_DownloadContents)(IUpdate2 * This, IUpdateDownloadContentCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_InstallState)(IUpdate2 * This, InstalledState * retval);
	HRESULT(STDMETHODCALLTYPE *CopyToCache)(IUpdate2 * This, IStringCollection * pFiles, VARIANT_BOOL ignoreDigestMismatch);
	END_INTERFACE
} IUpdate2Vtbl;

interface IUpdate2 {
    CONST_VTBL struct IUpdate2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdate2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdate2_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdate2_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdate2_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdate2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdate2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdate2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdate2_get_Title(This,retval)  (This)->lpVtbl -> get_Title(This,retval)
#define IUpdate2_get_AutoSelectOnWebSites(This,retval)  (This)->lpVtbl -> get_AutoSelectOnWebSites(This,retval)
#define IUpdate2_get_BundledUpdates(This,retval)  (This)->lpVtbl -> get_BundledUpdates(This,retval)
#define IUpdate2_get_CanRequireSource(This,retval)  (This)->lpVtbl -> get_CanRequireSource(This,retval)
#define IUpdate2_get_Categories(This,retval)  (This)->lpVtbl -> get_Categories(This,retval)
#define IUpdate2_get_Deadline(This,retval)  (This)->lpVtbl -> get_Deadline(This,retval)
#define IUpdate2_get_DeltaCompressedContentAvailable(This,retval)  (This)->lpVtbl -> get_DeltaCompressedContentAvailable(This,retval)
#define IUpdate2_get_DeltaCompressedContentPreferred(This,retval)  (This)->lpVtbl -> get_DeltaCompressedContentPreferred(This,retval)
#define IUpdate2_get_Description(This,retval)  (This)->lpVtbl -> get_Description(This,retval)
#define IUpdate2_get_EulaAccepted(This,retval)  (This)->lpVtbl -> get_EulaAccepted(This,retval)
#define IUpdate2_get_EulaText(This,retval)  (This)->lpVtbl -> get_EulaText(This,retval)
#define IUpdate2_get_HandlerID(This,retval)  (This)->lpVtbl -> get_HandlerID(This,retval)
#define IUpdate2_get_Identity(This,retval)  (This)->lpVtbl -> get_Identity(This,retval)
#define IUpdate2_get_Image(This,retval)  (This)->lpVtbl -> get_Image(This,retval)
#define IUpdate2_get_InstallationBehavior(This,retval)  (This)->lpVtbl -> get_InstallationBehavior(This,retval)
#define IUpdate2_get_IsBeta(This,retval)  (This)->lpVtbl -> get_IsBeta(This,retval)
#define IUpdate2_get_IsDownloaded(This,retval)  (This)->lpVtbl -> get_IsDownloaded(This,retval)
#define IUpdate2_get_IsHidden(This,retval)  (This)->lpVtbl -> get_IsHidden(This,retval)
#define IUpdate2_put_IsHidden(This,value)  (This)->lpVtbl -> put_IsHidden(This,value)
#define IUpdate2_get_IsInstalled(This,retval)  (This)->lpVtbl -> get_IsInstalled(This,retval)
#define IUpdate2_get_IsMandatory(This,retval)  (This)->lpVtbl -> get_IsMandatory(This,retval)
#define IUpdate2_get_IsUninstallable(This,retval)  (This)->lpVtbl -> get_IsUninstallable(This,retval)
#define IUpdate2_get_Languages(This,retval)  (This)->lpVtbl -> get_Languages(This,retval)
#define IUpdate2_get_LastDeploymentChangeTime(This,retval)  (This)->lpVtbl -> get_LastDeploymentChangeTime(This,retval)
#define IUpdate2_get_MaxDownloadSize(This,retval)  (This)->lpVtbl -> get_MaxDownloadSize(This,retval)
#define IUpdate2_get_MinDownloadSize(This,retval)  (This)->lpVtbl -> get_MinDownloadSize(This,retval)
#define IUpdate2_get_MoreInfoUrls(This,retval)  (This)->lpVtbl -> get_MoreInfoUrls(This,retval)
#define IUpdate2_get_MsrcSeverity(This,retval)  (This)->lpVtbl -> get_MsrcSeverity(This,retval)
#define IUpdate2_get_RecommendedCpuSpeed(This,retval)  (This)->lpVtbl -> get_RecommendedCpuSpeed(This,retval)
#define IUpdate2_get_RecommendedHardDiskSpace(This,retval)  (This)->lpVtbl -> get_RecommendedHardDiskSpace(This,retval)
#define IUpdate2_get_RecommendedMemory(This,retval)  (This)->lpVtbl -> get_RecommendedMemory(This,retval)
#define IUpdate2_get_ReleaseNotes(This,retval)  (This)->lpVtbl -> get_ReleaseNotes(This,retval)
#define IUpdate2_get_SecurityBulletinIDs(This,retval)  (This)->lpVtbl -> get_SecurityBulletinIDs(This,retval)
#define IUpdate2_get_SupersededUpdateIDs(This,retval)  (This)->lpVtbl -> get_SupersededUpdateIDs(This,retval)
#define IUpdate2_get_SupportUrl(This,retval)  (This)->lpVtbl -> get_SupportUrl(This,retval)
#define IUpdate2_get_Type(This,retval)  (This)->lpVtbl -> get_Type(This,retval)
#define IUpdate2_get_UninstallationNotes(This,retval)  (This)->lpVtbl -> get_UninstallationNotes(This,retval)
#define IUpdate2_get_UninstallationBehavior(This,retval)  (This)->lpVtbl -> get_UninstallationBehavior(This,retval)
#define IUpdate2_get_UninstallationSteps(This,retval)  (This)->lpVtbl -> get_UninstallationSteps(This,retval)
#define IUpdate2_get_KBArticleIDs(This,retval)  (This)->lpVtbl -> get_KBArticleIDs(This,retval)
#define IUpdate2_AcceptEula(This)  (This)->lpVtbl -> AcceptEula(This)
#define IUpdate2_get_DeploymentAction(This,retval)  (This)->lpVtbl -> get_DeploymentAction(This,retval)
#define IUpdate2_CopyFromCache(This,path,toExtractCabFiles)  (This)->lpVtbl -> CopyFromCache(This,path,toExtractCabFiles)
#define IUpdate2_get_DownloadPriority(This,retval)  (This)->lpVtbl -> get_DownloadPriority(This,retval)
#define IUpdate2_get_DownloadContents(This,retval)  (This)->lpVtbl -> get_DownloadContents(This,retval)
#define IUpdate2_get_InstallState(This,retval)  (This)->lpVtbl -> get_InstallState(This,retval)
#define IUpdate2_CopyToCache(This,pFiles,ignoreDigestMismatch)  (This)->lpVtbl -> CopyToCache(This,pFiles,ignoreDigestMismatch)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdate2_get_InstallState_Proxy(IUpdate2 * This, InstalledState * retval);
void __RPC_STUB IUpdate2_get_InstallState_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdate2_CopyToCache_Proxy(IUpdate2 * This, IStringCollection * pFiles, VARIANT_BOOL ignoreDigestMismatch);
void __RPC_STUB IUpdate2_CopyToCache_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IWindowsDriverUpdate2_INTERFACE_DEFINED__
#define __IWindowsDriverUpdate2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWindowsDriverUpdate2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("84617777-dbeb-4881-b410-aa294227c3fe")IWindowsDriverUpdate2:public IWindowsDriverUpdate
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_InstallState(InstalledState * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE CopyToCache(IStringCollection * pFiles, VARIANT_BOOL ignoreDigestMismatch) = 0;
};
#else
typedef struct IWindowsDriverUpdate2Vtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IWindowsDriverUpdate2 * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IWindowsDriverUpdate2 * This);
	ULONG(STDMETHODCALLTYPE *Release)(IWindowsDriverUpdate2 * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IWindowsDriverUpdate2 * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IWindowsDriverUpdate2 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IWindowsDriverUpdate2 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IWindowsDriverUpdate2 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Title)(IWindowsDriverUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_AutoSelectOnWebSites)(IWindowsDriverUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_BundledUpdates)(IWindowsDriverUpdate2 * This, IUpdateCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_CanRequireSource)(IWindowsDriverUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Categories)(IWindowsDriverUpdate2 * This, ICategoryCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Deadline)(IWindowsDriverUpdate2 * This, VARIANT * retval);
	HRESULT(STDMETHODCALLTYPE *get_DeltaCompressedContentAvailable)(IWindowsDriverUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_DeltaCompressedContentPreferred)(IWindowsDriverUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IWindowsDriverUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_EulaAccepted)(IWindowsDriverUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_EulaText)(IWindowsDriverUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_HandlerID)(IWindowsDriverUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Identity)(IWindowsDriverUpdate2 * This, IUpdateIdentity ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Image)(IWindowsDriverUpdate2 * This, IImageInformation ** retval);
	HRESULT(STDMETHODCALLTYPE *get_InstallationBehavior)(IWindowsDriverUpdate2 * This, IInstallationBehavior ** retval);
	HRESULT(STDMETHODCALLTYPE *get_IsBeta)(IWindowsDriverUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsDownloaded)(IWindowsDriverUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsHidden)(IWindowsDriverUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_IsHidden)(IWindowsDriverUpdate2 * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *get_IsInstalled)(IWindowsDriverUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsMandatory)(IWindowsDriverUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsUninstallable)(IWindowsDriverUpdate2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Languages)(IWindowsDriverUpdate2 * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_LastDeploymentChangeTime)(IWindowsDriverUpdate2 * This, DATE * retval);
	HRESULT(STDMETHODCALLTYPE *get_MaxDownloadSize)(IWindowsDriverUpdate2 * This, DECIMAL * retval);
	HRESULT(STDMETHODCALLTYPE *get_MinDownloadSize)(IWindowsDriverUpdate2 * This, DECIMAL * retval);
	HRESULT(STDMETHODCALLTYPE *get_MoreInfoUrls)(IWindowsDriverUpdate2 * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_MsrcSeverity)(IWindowsDriverUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_RecommendedCpuSpeed)(IWindowsDriverUpdate2 * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_RecommendedHardDiskSpace)(IWindowsDriverUpdate2 * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_RecommendedMemory)(IWindowsDriverUpdate2 * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_ReleaseNotes)(IWindowsDriverUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_SecurityBulletinIDs)(IWindowsDriverUpdate2 * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_SupersededUpdateIDs)(IWindowsDriverUpdate2 * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_SupportUrl)(IWindowsDriverUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Type)(IWindowsDriverUpdate2 * This, UpdateType * retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationNotes)(IWindowsDriverUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationBehavior)(IWindowsDriverUpdate2 * This, IInstallationBehavior ** retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationSteps)(IWindowsDriverUpdate2 * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_KBArticleIDs)(IWindowsDriverUpdate2 * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *AcceptEula)(IWindowsDriverUpdate2 * This);
	HRESULT(STDMETHODCALLTYPE *get_DeploymentAction)(IWindowsDriverUpdate2 * This, DeploymentAction * retval);
	HRESULT(STDMETHODCALLTYPE *CopyFromCache)(IWindowsDriverUpdate2 * This, BSTR path, VARIANT_BOOL toExtractCabFiles);
	HRESULT(STDMETHODCALLTYPE *get_DownloadPriority)(IWindowsDriverUpdate2 * This, DownloadPriority * retval);
	HRESULT(STDMETHODCALLTYPE *get_DownloadContents)(IWindowsDriverUpdate2 * This, IUpdateDownloadContentCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_DriverClass)(IWindowsDriverUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_DriverHardwareID)(IWindowsDriverUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_DriverManufacturer)(IWindowsDriverUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_DriverModel)(IWindowsDriverUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_DriverProvider)(IWindowsDriverUpdate2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_DriverVerDate)(IWindowsDriverUpdate2 * This, DATE * retval);
	HRESULT(STDMETHODCALLTYPE *get_DeviceProblemNumber)(IWindowsDriverUpdate2 * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_DeviceStatus)(IWindowsDriverUpdate2 * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_InstallState)(IWindowsDriverUpdate2 * This, InstalledState * retval);
	HRESULT(STDMETHODCALLTYPE *CopyToCache)(IWindowsDriverUpdate2 * This, IStringCollection * pFiles, VARIANT_BOOL ignoreDigestMismatch);
	END_INTERFACE
} IWindowsDriverUpdate2Vtbl;

interface IWindowsDriverUpdate2 {
    CONST_VTBL struct IWindowsDriverUpdate2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWindowsDriverUpdate2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IWindowsDriverUpdate2_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IWindowsDriverUpdate2_Release(This)  (This)->lpVtbl -> Release(This)
#define IWindowsDriverUpdate2_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IWindowsDriverUpdate2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IWindowsDriverUpdate2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IWindowsDriverUpdate2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IWindowsDriverUpdate2_get_Title(This,retval)  (This)->lpVtbl -> get_Title(This,retval)
#define IWindowsDriverUpdate2_get_AutoSelectOnWebSites(This,retval)  (This)->lpVtbl -> get_AutoSelectOnWebSites(This,retval)
#define IWindowsDriverUpdate2_get_BundledUpdates(This,retval)  (This)->lpVtbl -> get_BundledUpdates(This,retval)
#define IWindowsDriverUpdate2_get_CanRequireSource(This,retval)  (This)->lpVtbl -> get_CanRequireSource(This,retval)
#define IWindowsDriverUpdate2_get_Categories(This,retval)  (This)->lpVtbl -> get_Categories(This,retval)
#define IWindowsDriverUpdate2_get_Deadline(This,retval)  (This)->lpVtbl -> get_Deadline(This,retval)
#define IWindowsDriverUpdate2_get_DeltaCompressedContentAvailable(This,retval)  (This)->lpVtbl -> get_DeltaCompressedContentAvailable(This,retval)
#define IWindowsDriverUpdate2_get_DeltaCompressedContentPreferred(This,retval)  (This)->lpVtbl -> get_DeltaCompressedContentPreferred(This,retval)
#define IWindowsDriverUpdate2_get_Description(This,retval)  (This)->lpVtbl -> get_Description(This,retval)
#define IWindowsDriverUpdate2_get_EulaAccepted(This,retval)  (This)->lpVtbl -> get_EulaAccepted(This,retval)
#define IWindowsDriverUpdate2_get_EulaText(This,retval)  (This)->lpVtbl -> get_EulaText(This,retval)
#define IWindowsDriverUpdate2_get_HandlerID(This,retval)  (This)->lpVtbl -> get_HandlerID(This,retval)
#define IWindowsDriverUpdate2_get_Identity(This,retval)  (This)->lpVtbl -> get_Identity(This,retval)
#define IWindowsDriverUpdate2_get_Image(This,retval)  (This)->lpVtbl -> get_Image(This,retval)
#define IWindowsDriverUpdate2_get_InstallationBehavior(This,retval)  (This)->lpVtbl -> get_InstallationBehavior(This,retval)
#define IWindowsDriverUpdate2_get_IsBeta(This,retval)  (This)->lpVtbl -> get_IsBeta(This,retval)
#define IWindowsDriverUpdate2_get_IsDownloaded(This,retval)  (This)->lpVtbl -> get_IsDownloaded(This,retval)
#define IWindowsDriverUpdate2_get_IsHidden(This,retval)  (This)->lpVtbl -> get_IsHidden(This,retval)
#define IWindowsDriverUpdate2_put_IsHidden(This,value)  (This)->lpVtbl -> put_IsHidden(This,value)
#define IWindowsDriverUpdate2_get_IsInstalled(This,retval)  (This)->lpVtbl -> get_IsInstalled(This,retval)
#define IWindowsDriverUpdate2_get_IsMandatory(This,retval)  (This)->lpVtbl -> get_IsMandatory(This,retval)
#define IWindowsDriverUpdate2_get_IsUninstallable(This,retval)  (This)->lpVtbl -> get_IsUninstallable(This,retval)
#define IWindowsDriverUpdate2_get_Languages(This,retval)  (This)->lpVtbl -> get_Languages(This,retval)
#define IWindowsDriverUpdate2_get_LastDeploymentChangeTime(This,retval)  (This)->lpVtbl -> get_LastDeploymentChangeTime(This,retval)
#define IWindowsDriverUpdate2_get_MaxDownloadSize(This,retval)  (This)->lpVtbl -> get_MaxDownloadSize(This,retval)
#define IWindowsDriverUpdate2_get_MinDownloadSize(This,retval)  (This)->lpVtbl -> get_MinDownloadSize(This,retval)
#define IWindowsDriverUpdate2_get_MoreInfoUrls(This,retval)  (This)->lpVtbl -> get_MoreInfoUrls(This,retval)
#define IWindowsDriverUpdate2_get_MsrcSeverity(This,retval)  (This)->lpVtbl -> get_MsrcSeverity(This,retval)
#define IWindowsDriverUpdate2_get_RecommendedCpuSpeed(This,retval)  (This)->lpVtbl -> get_RecommendedCpuSpeed(This,retval)
#define IWindowsDriverUpdate2_get_RecommendedHardDiskSpace(This,retval)  (This)->lpVtbl -> get_RecommendedHardDiskSpace(This,retval)
#define IWindowsDriverUpdate2_get_RecommendedMemory(This,retval)  (This)->lpVtbl -> get_RecommendedMemory(This,retval)
#define IWindowsDriverUpdate2_get_ReleaseNotes(This,retval)  (This)->lpVtbl -> get_ReleaseNotes(This,retval)
#define IWindowsDriverUpdate2_get_SecurityBulletinIDs(This,retval)  (This)->lpVtbl -> get_SecurityBulletinIDs(This,retval)
#define IWindowsDriverUpdate2_get_SupersededUpdateIDs(This,retval)  (This)->lpVtbl -> get_SupersededUpdateIDs(This,retval)
#define IWindowsDriverUpdate2_get_SupportUrl(This,retval)  (This)->lpVtbl -> get_SupportUrl(This,retval)
#define IWindowsDriverUpdate2_get_Type(This,retval)  (This)->lpVtbl -> get_Type(This,retval)
#define IWindowsDriverUpdate2_get_UninstallationNotes(This,retval)  (This)->lpVtbl -> get_UninstallationNotes(This,retval)
#define IWindowsDriverUpdate2_get_UninstallationBehavior(This,retval)  (This)->lpVtbl -> get_UninstallationBehavior(This,retval)
#define IWindowsDriverUpdate2_get_UninstallationSteps(This,retval)  (This)->lpVtbl -> get_UninstallationSteps(This,retval)
#define IWindowsDriverUpdate2_get_KBArticleIDs(This,retval)  (This)->lpVtbl -> get_KBArticleIDs(This,retval)
#define IWindowsDriverUpdate2_AcceptEula(This)  (This)->lpVtbl -> AcceptEula(This)
#define IWindowsDriverUpdate2_get_DeploymentAction(This,retval)  (This)->lpVtbl -> get_DeploymentAction(This,retval)
#define IWindowsDriverUpdate2_CopyFromCache(This,path,toExtractCabFiles)  (This)->lpVtbl -> CopyFromCache(This,path,toExtractCabFiles)
#define IWindowsDriverUpdate2_get_DownloadPriority(This,retval)  (This)->lpVtbl -> get_DownloadPriority(This,retval)
#define IWindowsDriverUpdate2_get_DownloadContents(This,retval)  (This)->lpVtbl -> get_DownloadContents(This,retval)
#define IWindowsDriverUpdate2_get_DriverClass(This,retval)  (This)->lpVtbl -> get_DriverClass(This,retval)
#define IWindowsDriverUpdate2_get_DriverHardwareID(This,retval)  (This)->lpVtbl -> get_DriverHardwareID(This,retval)
#define IWindowsDriverUpdate2_get_DriverManufacturer(This,retval)  (This)->lpVtbl -> get_DriverManufacturer(This,retval)
#define IWindowsDriverUpdate2_get_DriverModel(This,retval)  (This)->lpVtbl -> get_DriverModel(This,retval)
#define IWindowsDriverUpdate2_get_DriverProvider(This,retval)  (This)->lpVtbl -> get_DriverProvider(This,retval)
#define IWindowsDriverUpdate2_get_DriverVerDate(This,retval)  (This)->lpVtbl -> get_DriverVerDate(This,retval)
#define IWindowsDriverUpdate2_get_DeviceProblemNumber(This,retval)  (This)->lpVtbl -> get_DeviceProblemNumber(This,retval)
#define IWindowsDriverUpdate2_get_DeviceStatus(This,retval)  (This)->lpVtbl -> get_DeviceStatus(This,retval)
#define IWindowsDriverUpdate2_get_InstallState(This,retval)  (This)->lpVtbl -> get_InstallState(This,retval)
#define IWindowsDriverUpdate2_CopyToCache(This,pFiles,ignoreDigestMismatch)  (This)->lpVtbl -> CopyToCache(This,pFiles,ignoreDigestMismatch)
#endif

#endif

HRESULT STDMETHODCALLTYPE IWindowsDriverUpdate2_get_InstallState_Proxy(IWindowsDriverUpdate2 * This, InstalledState * retval);
void __RPC_STUB IWindowsDriverUpdate2_get_InstallState_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IWindowsDriverUpdate2_CopyToCache_Proxy(IWindowsDriverUpdate2 * This, IStringCollection * pFiles, VARIANT_BOOL ignoreDigestMismatch);
void __RPC_STUB IWindowsDriverUpdate2_CopyToCache_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateCollection_INTERFACE_DEFINED__
#define __IUpdateCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("07f7438c-7709-4ca5-b518-91279288134e")IUpdateCollection:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Item(LONG index, IUpdate ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Item(LONG index, IUpdate * value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ReadOnly(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE Add(IUpdate * value, LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE Clear(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE Copy(IUpdateCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE Insert(LONG index, IUpdate * value) = 0;
	virtual HRESULT STDMETHODCALLTYPE RemoveAt(LONG index) = 0;
};
#else
typedef struct IUpdateCollectionVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateCollection * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateCollection * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateCollection * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateCollection * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateCollection * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateCollection * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateCollection * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Item)(IUpdateCollection * This, LONG index, IUpdate ** retval);
	HRESULT(STDMETHODCALLTYPE *put_Item)(IUpdateCollection * This, LONG index, IUpdate * value);
	HRESULT(STDMETHODCALLTYPE *get__NewEnum)(IUpdateCollection * This, IUnknown ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Count)(IUpdateCollection * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_ReadOnly)(IUpdateCollection * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *Add)(IUpdateCollection * This, IUpdate * value, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *Clear)(IUpdateCollection * This);
	HRESULT(STDMETHODCALLTYPE *Copy)(IUpdateCollection * This, IUpdateCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *Insert)(IUpdateCollection * This, LONG index, IUpdate * value);
	HRESULT(STDMETHODCALLTYPE *RemoveAt)(IUpdateCollection * This, LONG index);
	END_INTERFACE
} IUpdateCollectionVtbl;

interface IUpdateCollection {
    CONST_VTBL struct IUpdateCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateCollection_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateCollection_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateCollection_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateCollection_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateCollection_get_Item(This,index,retval)  (This)->lpVtbl -> get_Item(This,index,retval)
#define IUpdateCollection_put_Item(This,index,value)  (This)->lpVtbl -> put_Item(This,index,value)
#define IUpdateCollection_get__NewEnum(This,retval)  (This)->lpVtbl -> get__NewEnum(This,retval)
#define IUpdateCollection_get_Count(This,retval)  (This)->lpVtbl -> get_Count(This,retval)
#define IUpdateCollection_get_ReadOnly(This,retval)  (This)->lpVtbl -> get_ReadOnly(This,retval)
#define IUpdateCollection_Add(This,value,retval)  (This)->lpVtbl -> Add(This,value,retval)
#define IUpdateCollection_Clear(This)  (This)->lpVtbl -> Clear(This)
#define IUpdateCollection_Copy(This,retval)  (This)->lpVtbl -> Copy(This,retval)
#define IUpdateCollection_Insert(This,index,value)  (This)->lpVtbl -> Insert(This,index,value)
#define IUpdateCollection_RemoveAt(This,index)  (This)->lpVtbl -> RemoveAt(This,index)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateCollection_get_Item_Proxy(IUpdateCollection * This, LONG index, IUpdate ** retval);
void __RPC_STUB IUpdateCollection_get_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateCollection_put_Item_Proxy(IUpdateCollection * This, LONG index, IUpdate * value);
void __RPC_STUB IUpdateCollection_put_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateCollection_get__NewEnum_Proxy(IUpdateCollection * This, IUnknown ** retval);
void __RPC_STUB IUpdateCollection_get__NewEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateCollection_get_Count_Proxy(IUpdateCollection * This, LONG * retval);
void __RPC_STUB IUpdateCollection_get_Count_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateCollection_get_ReadOnly_Proxy(IUpdateCollection * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateCollection_get_ReadOnly_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateCollection_Add_Proxy(IUpdateCollection * This, IUpdate * value, LONG * retval);
void __RPC_STUB IUpdateCollection_Add_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateCollection_Clear_Proxy(IUpdateCollection * This);
void __RPC_STUB IUpdateCollection_Clear_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateCollection_Copy_Proxy(IUpdateCollection * This, IUpdateCollection ** retval);
void __RPC_STUB IUpdateCollection_Copy_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateCollection_Insert_Proxy(IUpdateCollection * This, LONG index, IUpdate * value);
void __RPC_STUB IUpdateCollection_Insert_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateCollection_RemoveAt_Proxy(IUpdateCollection * This, LONG index);
void __RPC_STUB IUpdateCollection_RemoveAt_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateException_INTERFACE_DEFINED__
#define __IUpdateException_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateException;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a376dd5e-09d4-427f-af7c-fed5b6e1c1d6")IUpdateException:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Message(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_HResult(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Context(UpdateExceptionContext * retval) = 0;
};
#else
typedef struct IUpdateExceptionVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateException * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateException * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateException * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateException * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateException * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateException * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateException * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Message)(IUpdateException * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_HResult)(IUpdateException * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_Context)(IUpdateException * This, UpdateExceptionContext * retval);
	END_INTERFACE
} IUpdateExceptionVtbl;

interface IUpdateException {
    CONST_VTBL struct IUpdateExceptionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateException_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateException_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateException_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateException_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateException_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateException_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateException_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateException_get_Message(This,retval)  (This)->lpVtbl -> get_Message(This,retval)
#define IUpdateException_get_HResult(This,retval)  (This)->lpVtbl -> get_HResult(This,retval)
#define IUpdateException_get_Context(This,retval)  (This)->lpVtbl -> get_Context(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateException_get_Message_Proxy(IUpdateException * This, BSTR * retval);
void __RPC_STUB IUpdateException_get_Message_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateException_get_HResult_Proxy(IUpdateException * This, LONG * retval);
void __RPC_STUB IUpdateException_get_HResult_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateException_get_Context_Proxy(IUpdateException * This, UpdateExceptionContext * retval);
void __RPC_STUB IUpdateException_get_Context_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IInvalidProductLicenseException_INTERFACE_DEFINED__
#define __IInvalidProductLicenseException_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInvalidProductLicenseException;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a37d00f5-7bb0-4953-b414-f9e98326f2e8")IInvalidProductLicenseException:public IUpdateException
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Product(BSTR * retval) = 0;
};
#else
typedef struct IInvalidProductLicenseExceptionVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInvalidProductLicenseException * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IInvalidProductLicenseException * This);
	ULONG(STDMETHODCALLTYPE *Release)(IInvalidProductLicenseException * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IInvalidProductLicenseException * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IInvalidProductLicenseException * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IInvalidProductLicenseException * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IInvalidProductLicenseException * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Message)(IInvalidProductLicenseException * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_HResult)(IInvalidProductLicenseException * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_Context)(IInvalidProductLicenseException * This, UpdateExceptionContext * retval);
	HRESULT(STDMETHODCALLTYPE *get_Product)(IInvalidProductLicenseException * This, BSTR * retval);
	END_INTERFACE
} IInvalidProductLicenseExceptionVtbl;

interface IInvalidProductLicenseException {
    CONST_VTBL struct IInvalidProductLicenseExceptionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInvalidProductLicenseException_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IInvalidProductLicenseException_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IInvalidProductLicenseException_Release(This)  (This)->lpVtbl -> Release(This)
#define IInvalidProductLicenseException_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IInvalidProductLicenseException_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IInvalidProductLicenseException_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IInvalidProductLicenseException_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IInvalidProductLicenseException_get_Message(This,retval)  (This)->lpVtbl -> get_Message(This,retval)
#define IInvalidProductLicenseException_get_HResult(This,retval)  (This)->lpVtbl -> get_HResult(This,retval)
#define IInvalidProductLicenseException_get_Context(This,retval)  (This)->lpVtbl -> get_Context(This,retval)
#define IInvalidProductLicenseException_get_Product(This,retval)  (This)->lpVtbl -> get_Product(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IInvalidProductLicenseException_get_Product_Proxy(IInvalidProductLicenseException * This, BSTR * retval);
void __RPC_STUB IInvalidProductLicenseException_get_Product_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateExceptionCollection_INTERFACE_DEFINED__
#define __IUpdateExceptionCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateExceptionCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("503626a3-8e14-4729-9355-0fe664bd2321")IUpdateExceptionCollection:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Item(LONG index, IUpdateException ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * retval) = 0;
};
#else
typedef struct IUpdateExceptionCollectionVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateExceptionCollection * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateExceptionCollection * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateExceptionCollection * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateExceptionCollection * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateExceptionCollection * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateExceptionCollection * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateExceptionCollection * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Item)(IUpdateExceptionCollection * This, LONG index, IUpdateException ** retval);
	HRESULT(STDMETHODCALLTYPE *get__NewEnum)(IUpdateExceptionCollection * This, IUnknown ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Count)(IUpdateExceptionCollection * This, LONG * retval);
	END_INTERFACE
} IUpdateExceptionCollectionVtbl;

interface IUpdateExceptionCollection {
    CONST_VTBL struct IUpdateExceptionCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateExceptionCollection_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateExceptionCollection_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateExceptionCollection_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateExceptionCollection_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateExceptionCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateExceptionCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateExceptionCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateExceptionCollection_get_Item(This,index,retval)  (This)->lpVtbl -> get_Item(This,index,retval)
#define IUpdateExceptionCollection_get__NewEnum(This,retval)  (This)->lpVtbl -> get__NewEnum(This,retval)
#define IUpdateExceptionCollection_get_Count(This,retval)  (This)->lpVtbl -> get_Count(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateExceptionCollection_get_Item_Proxy(IUpdateExceptionCollection * This, LONG index, IUpdateException ** retval);
void __RPC_STUB IUpdateExceptionCollection_get_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateExceptionCollection_get__NewEnum_Proxy(IUpdateExceptionCollection * This, IUnknown ** retval);
void __RPC_STUB IUpdateExceptionCollection_get__NewEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateExceptionCollection_get_Count_Proxy(IUpdateExceptionCollection * This, LONG * retval);
void __RPC_STUB IUpdateExceptionCollection_get_Count_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __ISearchResult_INTERFACE_DEFINED__
#define __ISearchResult_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISearchResult;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("d40cff62-e08c-4498-941a-01e25f0fd33c")ISearchResult:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_ResultCode(OperationResultCode * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_RootCategories(ICategoryCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Updates(IUpdateCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Warnings(IUpdateExceptionCollection ** retval) = 0;
};
#else
typedef struct ISearchResultVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(ISearchResult * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(ISearchResult * This);
	ULONG(STDMETHODCALLTYPE *Release)(ISearchResult * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ISearchResult * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ISearchResult * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ISearchResult * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(ISearchResult * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_ResultCode)(ISearchResult * This, OperationResultCode * retval);
	HRESULT(STDMETHODCALLTYPE *get_RootCategories)(ISearchResult * This, ICategoryCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Updates)(ISearchResult * This, IUpdateCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Warnings)(ISearchResult * This, IUpdateExceptionCollection ** retval);
	END_INTERFACE
} ISearchResultVtbl;

interface ISearchResult {
    CONST_VTBL struct ISearchResultVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISearchResult_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ISearchResult_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define ISearchResult_Release(This)  (This)->lpVtbl -> Release(This)
#define ISearchResult_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define ISearchResult_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ISearchResult_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ISearchResult_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ISearchResult_get_ResultCode(This,retval)  (This)->lpVtbl -> get_ResultCode(This,retval)
#define ISearchResult_get_RootCategories(This,retval)  (This)->lpVtbl -> get_RootCategories(This,retval)
#define ISearchResult_get_Updates(This,retval)  (This)->lpVtbl -> get_Updates(This,retval)
#define ISearchResult_get_Warnings(This,retval)  (This)->lpVtbl -> get_Warnings(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE ISearchResult_get_ResultCode_Proxy(ISearchResult * This, OperationResultCode * retval);
void __RPC_STUB ISearchResult_get_ResultCode_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ISearchResult_get_RootCategories_Proxy(ISearchResult * This, ICategoryCollection ** retval);
void __RPC_STUB ISearchResult_get_RootCategories_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ISearchResult_get_Updates_Proxy(ISearchResult * This, IUpdateCollection ** retval);
void __RPC_STUB ISearchResult_get_Updates_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ISearchResult_get_Warnings_Proxy(ISearchResult * This, IUpdateExceptionCollection ** retval);
void __RPC_STUB ISearchResult_get_Warnings_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __ISearchJob_INTERFACE_DEFINED__
#define __ISearchJob_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISearchJob;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7366ea16-7a1a-4ea2-b042-973d3e9cd99b")ISearchJob:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_AsyncState(VARIANT * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsCompleted(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE CleanUp(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE RequestAbort(void) = 0;
};
#else
typedef struct ISearchJobVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(ISearchJob * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(ISearchJob * This);
	ULONG(STDMETHODCALLTYPE *Release)(ISearchJob * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ISearchJob * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ISearchJob * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ISearchJob * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(ISearchJob * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_AsyncState)(ISearchJob * This, VARIANT * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsCompleted)(ISearchJob * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *CleanUp)(ISearchJob * This);
	HRESULT(STDMETHODCALLTYPE *RequestAbort)(ISearchJob * This);
	END_INTERFACE
} ISearchJobVtbl;

interface ISearchJob {
    CONST_VTBL struct ISearchJobVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISearchJob_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ISearchJob_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define ISearchJob_Release(This)  (This)->lpVtbl -> Release(This)
#define ISearchJob_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define ISearchJob_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ISearchJob_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ISearchJob_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define ISearchJob_get_AsyncState(This,retval)  (This)->lpVtbl -> get_AsyncState(This,retval)
#define ISearchJob_get_IsCompleted(This,retval)  (This)->lpVtbl -> get_IsCompleted(This,retval)
#define ISearchJob_CleanUp(This)  (This)->lpVtbl -> CleanUp(This)
#define ISearchJob_RequestAbort(This)  (This)->lpVtbl -> RequestAbort(This)
#endif

#endif

HRESULT STDMETHODCALLTYPE ISearchJob_get_AsyncState_Proxy(ISearchJob * This, VARIANT * retval);
void __RPC_STUB ISearchJob_get_AsyncState_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ISearchJob_get_IsCompleted_Proxy(ISearchJob * This, VARIANT_BOOL * retval);
void __RPC_STUB ISearchJob_get_IsCompleted_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ISearchJob_CleanUp_Proxy(ISearchJob * This);
void __RPC_STUB ISearchJob_CleanUp_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE ISearchJob_RequestAbort_Proxy(ISearchJob * This);
void __RPC_STUB ISearchJob_RequestAbort_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __ISearchCompletedCallbackArgs_INTERFACE_DEFINED__
#define __ISearchCompletedCallbackArgs_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISearchCompletedCallbackArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a700a634-2850-4c47-938a-9e4b6e5af9a6")ISearchCompletedCallbackArgs:public IDispatch
{
	public:
};
#else
typedef struct ISearchCompletedCallbackArgsVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(ISearchCompletedCallbackArgs * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(ISearchCompletedCallbackArgs * This);
	ULONG(STDMETHODCALLTYPE *Release)(ISearchCompletedCallbackArgs * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(ISearchCompletedCallbackArgs * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(ISearchCompletedCallbackArgs * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(ISearchCompletedCallbackArgs * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(ISearchCompletedCallbackArgs * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	END_INTERFACE
} ISearchCompletedCallbackArgsVtbl;

interface ISearchCompletedCallbackArgs {
    CONST_VTBL struct ISearchCompletedCallbackArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISearchCompletedCallbackArgs_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ISearchCompletedCallbackArgs_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define ISearchCompletedCallbackArgs_Release(This)  (This)->lpVtbl -> Release(This)
#define ISearchCompletedCallbackArgs_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define ISearchCompletedCallbackArgs_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define ISearchCompletedCallbackArgs_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define ISearchCompletedCallbackArgs_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#endif

#endif

#endif

#ifndef __ISearchCompletedCallback_INTERFACE_DEFINED__
#define __ISearchCompletedCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISearchCompletedCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("88aee058-d4b0-4725-a2f1-814a67ae964c")ISearchCompletedCallback:public IUnknown
{
	public:
	virtual HRESULT STDMETHODCALLTYPE Invoke(ISearchJob * searchJob, ISearchCompletedCallbackArgs * callbackArgs) = 0;
};
#else
typedef struct ISearchCompletedCallbackVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(ISearchCompletedCallback * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(ISearchCompletedCallback * This);
	ULONG(STDMETHODCALLTYPE *Release)(ISearchCompletedCallback * This);
	HRESULT(STDMETHODCALLTYPE *Invoke)(ISearchCompletedCallback * This, ISearchJob * searchJob, ISearchCompletedCallbackArgs * callbackArgs);
	END_INTERFACE
} ISearchCompletedCallbackVtbl;

interface ISearchCompletedCallback {
    CONST_VTBL struct ISearchCompletedCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISearchCompletedCallback_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define ISearchCompletedCallback_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define ISearchCompletedCallback_Release(This)  (This)->lpVtbl -> Release(This)
#define ISearchCompletedCallback_Invoke(This,searchJob,callbackArgs)  (This)->lpVtbl -> Invoke(This,searchJob,callbackArgs)
#endif

#endif

HRESULT STDMETHODCALLTYPE ISearchCompletedCallback_Invoke_Proxy(ISearchCompletedCallback * This, ISearchJob * searchJob, ISearchCompletedCallbackArgs * callbackArgs);
void __RPC_STUB ISearchCompletedCallback_Invoke_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateHistoryEntry_INTERFACE_DEFINED__
#define __IUpdateHistoryEntry_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateHistoryEntry;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("be56a644-af0e-4e0e-a311-c1d8e695cbff")IUpdateHistoryEntry:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Operation(UpdateOperation * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ResultCode(OperationResultCode * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_HResult(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Date(DATE * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UpdateIdentity(IUpdateIdentity ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Title(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Description(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UnmappedResultCode(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ClientApplicationID(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ServerSelection(ServerSelection * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ServiceID(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UninstallationSteps(IStringCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UninstallationNotes(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_SupportUrl(BSTR * retval) = 0;
};
#else
typedef struct IUpdateHistoryEntryVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateHistoryEntry * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateHistoryEntry * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateHistoryEntry * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateHistoryEntry * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateHistoryEntry * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateHistoryEntry * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateHistoryEntry * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Operation)(IUpdateHistoryEntry * This, UpdateOperation * retval);
	HRESULT(STDMETHODCALLTYPE *get_ResultCode)(IUpdateHistoryEntry * This, OperationResultCode * retval);
	HRESULT(STDMETHODCALLTYPE *get_HResult)(IUpdateHistoryEntry * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_Date)(IUpdateHistoryEntry * This, DATE * retval);
	HRESULT(STDMETHODCALLTYPE *get_UpdateIdentity)(IUpdateHistoryEntry * This, IUpdateIdentity ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Title)(IUpdateHistoryEntry * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IUpdateHistoryEntry * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_UnmappedResultCode)(IUpdateHistoryEntry * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_ClientApplicationID)(IUpdateHistoryEntry * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_ServerSelection)(IUpdateHistoryEntry * This, ServerSelection * retval);
	HRESULT(STDMETHODCALLTYPE *get_ServiceID)(IUpdateHistoryEntry * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationSteps)(IUpdateHistoryEntry * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_UninstallationNotes)(IUpdateHistoryEntry * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_SupportUrl)(IUpdateHistoryEntry * This, BSTR * retval);
	END_INTERFACE
} IUpdateHistoryEntryVtbl;

interface IUpdateHistoryEntry {
    CONST_VTBL struct IUpdateHistoryEntryVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateHistoryEntry_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateHistoryEntry_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateHistoryEntry_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateHistoryEntry_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateHistoryEntry_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateHistoryEntry_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateHistoryEntry_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateHistoryEntry_get_Operation(This,retval)  (This)->lpVtbl -> get_Operation(This,retval)
#define IUpdateHistoryEntry_get_ResultCode(This,retval)  (This)->lpVtbl -> get_ResultCode(This,retval)
#define IUpdateHistoryEntry_get_HResult(This,retval)  (This)->lpVtbl -> get_HResult(This,retval)
#define IUpdateHistoryEntry_get_Date(This,retval)  (This)->lpVtbl -> get_Date(This,retval)
#define IUpdateHistoryEntry_get_UpdateIdentity(This,retval)  (This)->lpVtbl -> get_UpdateIdentity(This,retval)
#define IUpdateHistoryEntry_get_Title(This,retval)  (This)->lpVtbl -> get_Title(This,retval)
#define IUpdateHistoryEntry_get_Description(This,retval)  (This)->lpVtbl -> get_Description(This,retval)
#define IUpdateHistoryEntry_get_UnmappedResultCode(This,retval)  (This)->lpVtbl -> get_UnmappedResultCode(This,retval)
#define IUpdateHistoryEntry_get_ClientApplicationID(This,retval)  (This)->lpVtbl -> get_ClientApplicationID(This,retval)
#define IUpdateHistoryEntry_get_ServerSelection(This,retval)  (This)->lpVtbl -> get_ServerSelection(This,retval)
#define IUpdateHistoryEntry_get_ServiceID(This,retval)  (This)->lpVtbl -> get_ServiceID(This,retval)
#define IUpdateHistoryEntry_get_UninstallationSteps(This,retval)  (This)->lpVtbl -> get_UninstallationSteps(This,retval)
#define IUpdateHistoryEntry_get_UninstallationNotes(This,retval)  (This)->lpVtbl -> get_UninstallationNotes(This,retval)
#define IUpdateHistoryEntry_get_SupportUrl(This,retval)  (This)->lpVtbl -> get_SupportUrl(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_Operation_Proxy(IUpdateHistoryEntry * This, UpdateOperation * retval);
void __RPC_STUB IUpdateHistoryEntry_get_Operation_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_ResultCode_Proxy(IUpdateHistoryEntry * This, OperationResultCode * retval);
void __RPC_STUB IUpdateHistoryEntry_get_ResultCode_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_HResult_Proxy(IUpdateHistoryEntry * This, LONG * retval);
void __RPC_STUB IUpdateHistoryEntry_get_HResult_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_Date_Proxy(IUpdateHistoryEntry * This, DATE * retval);
void __RPC_STUB IUpdateHistoryEntry_get_Date_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_UpdateIdentity_Proxy(IUpdateHistoryEntry * This, IUpdateIdentity ** retval);
void __RPC_STUB IUpdateHistoryEntry_get_UpdateIdentity_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_Title_Proxy(IUpdateHistoryEntry * This, BSTR * retval);
void __RPC_STUB IUpdateHistoryEntry_get_Title_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_Description_Proxy(IUpdateHistoryEntry * This, BSTR * retval);
void __RPC_STUB IUpdateHistoryEntry_get_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_UnmappedResultCode_Proxy(IUpdateHistoryEntry * This, LONG * retval);
void __RPC_STUB IUpdateHistoryEntry_get_UnmappedResultCode_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_ClientApplicationID_Proxy(IUpdateHistoryEntry * This, BSTR * retval);
void __RPC_STUB IUpdateHistoryEntry_get_ClientApplicationID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_ServerSelection_Proxy(IUpdateHistoryEntry * This, ServerSelection * retval);
void __RPC_STUB IUpdateHistoryEntry_get_ServerSelection_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_ServiceID_Proxy(IUpdateHistoryEntry * This, BSTR * retval);
void __RPC_STUB IUpdateHistoryEntry_get_ServiceID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_UninstallationSteps_Proxy(IUpdateHistoryEntry * This, IStringCollection ** retval);
void __RPC_STUB IUpdateHistoryEntry_get_UninstallationSteps_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_UninstallationNotes_Proxy(IUpdateHistoryEntry * This, BSTR * retval);
void __RPC_STUB IUpdateHistoryEntry_get_UninstallationNotes_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntry_get_SupportUrl_Proxy(IUpdateHistoryEntry * This, BSTR * retval);
void __RPC_STUB IUpdateHistoryEntry_get_SupportUrl_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateHistoryEntryCollection_INTERFACE_DEFINED__
#define __IUpdateHistoryEntryCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateHistoryEntryCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a7f04f3c-a290-435b-aadf-a116c3357a5c")IUpdateHistoryEntryCollection:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Item(LONG index, IUpdateHistoryEntry ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * retval) = 0;
};
#else
typedef struct IUpdateHistoryEntryCollectionVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateHistoryEntryCollection * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateHistoryEntryCollection * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateHistoryEntryCollection * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateHistoryEntryCollection * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateHistoryEntryCollection * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateHistoryEntryCollection * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateHistoryEntryCollection * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Item)(IUpdateHistoryEntryCollection * This, LONG index, IUpdateHistoryEntry ** retval);
	HRESULT(STDMETHODCALLTYPE *get__NewEnum)(IUpdateHistoryEntryCollection * This, IUnknown ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Count)(IUpdateHistoryEntryCollection * This, LONG * retval);
	END_INTERFACE
} IUpdateHistoryEntryCollectionVtbl;

interface IUpdateHistoryEntryCollection {
    CONST_VTBL struct IUpdateHistoryEntryCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateHistoryEntryCollection_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateHistoryEntryCollection_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateHistoryEntryCollection_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateHistoryEntryCollection_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateHistoryEntryCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateHistoryEntryCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateHistoryEntryCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateHistoryEntryCollection_get_Item(This,index,retval)  (This)->lpVtbl -> get_Item(This,index,retval)
#define IUpdateHistoryEntryCollection_get__NewEnum(This,retval)  (This)->lpVtbl -> get__NewEnum(This,retval)
#define IUpdateHistoryEntryCollection_get_Count(This,retval)  (This)->lpVtbl -> get_Count(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateHistoryEntryCollection_get_Item_Proxy(IUpdateHistoryEntryCollection * This, LONG index, IUpdateHistoryEntry ** retval);
void __RPC_STUB IUpdateHistoryEntryCollection_get_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntryCollection_get__NewEnum_Proxy(IUpdateHistoryEntryCollection * This, IUnknown ** retval);
void __RPC_STUB IUpdateHistoryEntryCollection_get__NewEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateHistoryEntryCollection_get_Count_Proxy(IUpdateHistoryEntryCollection * This, LONG * retval);
void __RPC_STUB IUpdateHistoryEntryCollection_get_Count_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateSearcher_INTERFACE_DEFINED__
#define __IUpdateSearcher_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateSearcher;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("8f45abf1-f9ae-4b95-a933-f0f66e5056ea")IUpdateSearcher:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_CanAutomaticallyUpgradeService(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_CanAutomaticallyUpgradeService(VARIANT_BOOL value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ClientApplicationID(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ClientApplicationID(BSTR value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IncludePotentiallySupersededUpdates(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_IncludePotentiallySupersededUpdates(VARIANT_BOOL value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ServerSelection(ServerSelection * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ServerSelection(ServerSelection value) = 0;
	virtual HRESULT STDMETHODCALLTYPE BeginSearch(BSTR criteria, IUnknown * onCompleted, VARIANT state, ISearchJob ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE EndSearch(ISearchJob * searchJob, ISearchResult ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE EscapeString(BSTR unescaped, BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE QueryHistory(LONG startIndex, LONG count, IUpdateHistoryEntryCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE Search(BSTR criteria, ISearchResult ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Online(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Online(VARIANT_BOOL value) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetTotalHistoryCount(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ServiceID(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ServiceID(BSTR value) = 0;
};
#else
typedef struct IUpdateSearcherVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateSearcher * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateSearcher * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateSearcher * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateSearcher * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateSearcher * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateSearcher * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateSearcher * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_CanAutomaticallyUpgradeService)(IUpdateSearcher * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_CanAutomaticallyUpgradeService)(IUpdateSearcher * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *get_ClientApplicationID)(IUpdateSearcher * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *put_ClientApplicationID)(IUpdateSearcher * This, BSTR value);
	HRESULT(STDMETHODCALLTYPE *get_IncludePotentiallySupersededUpdates)(IUpdateSearcher * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_IncludePotentiallySupersededUpdates)(IUpdateSearcher * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *get_ServerSelection)(IUpdateSearcher * This, ServerSelection * retval);
	HRESULT(STDMETHODCALLTYPE *put_ServerSelection)(IUpdateSearcher * This, ServerSelection value);
	HRESULT(STDMETHODCALLTYPE *BeginSearch)(IUpdateSearcher * This, BSTR criteria, IUnknown * onCompleted, VARIANT state, ISearchJob ** retval);
	HRESULT(STDMETHODCALLTYPE *EndSearch)(IUpdateSearcher * This, ISearchJob * searchJob, ISearchResult ** retval);
	HRESULT(STDMETHODCALLTYPE *EscapeString)(IUpdateSearcher * This, BSTR unescaped, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *QueryHistory)(IUpdateSearcher * This, LONG startIndex, LONG count, IUpdateHistoryEntryCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *Search)(IUpdateSearcher * This, BSTR criteria, ISearchResult ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Online)(IUpdateSearcher * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_Online)(IUpdateSearcher * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *GetTotalHistoryCount)(IUpdateSearcher * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_ServiceID)(IUpdateSearcher * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *put_ServiceID)(IUpdateSearcher * This, BSTR value);
	END_INTERFACE
} IUpdateSearcherVtbl;

interface IUpdateSearcher {
    CONST_VTBL struct IUpdateSearcherVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateSearcher_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateSearcher_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateSearcher_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateSearcher_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateSearcher_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateSearcher_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateSearcher_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateSearcher_get_CanAutomaticallyUpgradeService(This,retval)  (This)->lpVtbl -> get_CanAutomaticallyUpgradeService(This,retval)
#define IUpdateSearcher_put_CanAutomaticallyUpgradeService(This,value)  (This)->lpVtbl -> put_CanAutomaticallyUpgradeService(This,value)
#define IUpdateSearcher_get_ClientApplicationID(This,retval)  (This)->lpVtbl -> get_ClientApplicationID(This,retval)
#define IUpdateSearcher_put_ClientApplicationID(This,value)  (This)->lpVtbl -> put_ClientApplicationID(This,value)
#define IUpdateSearcher_get_IncludePotentiallySupersededUpdates(This,retval)  (This)->lpVtbl -> get_IncludePotentiallySupersededUpdates(This,retval)
#define IUpdateSearcher_put_IncludePotentiallySupersededUpdates(This,value)  (This)->lpVtbl -> put_IncludePotentiallySupersededUpdates(This,value)
#define IUpdateSearcher_get_ServerSelection(This,retval)  (This)->lpVtbl -> get_ServerSelection(This,retval)
#define IUpdateSearcher_put_ServerSelection(This,value)  (This)->lpVtbl -> put_ServerSelection(This,value)
#define IUpdateSearcher_BeginSearch(This,criteria,onCompleted,state,retval)  (This)->lpVtbl -> BeginSearch(This,criteria,onCompleted,state,retval)
#define IUpdateSearcher_EndSearch(This,searchJob,retval)  (This)->lpVtbl -> EndSearch(This,searchJob,retval)
#define IUpdateSearcher_EscapeString(This,unescaped,retval)  (This)->lpVtbl -> EscapeString(This,unescaped,retval)
#define IUpdateSearcher_QueryHistory(This,startIndex,count,retval)  (This)->lpVtbl -> QueryHistory(This,startIndex,count,retval)
#define IUpdateSearcher_Search(This,criteria,retval)  (This)->lpVtbl -> Search(This,criteria,retval)
#define IUpdateSearcher_get_Online(This,retval)  (This)->lpVtbl -> get_Online(This,retval)
#define IUpdateSearcher_put_Online(This,value)  (This)->lpVtbl -> put_Online(This,value)
#define IUpdateSearcher_GetTotalHistoryCount(This,retval)  (This)->lpVtbl -> GetTotalHistoryCount(This,retval)
#define IUpdateSearcher_get_ServiceID(This,retval)  (This)->lpVtbl -> get_ServiceID(This,retval)
#define IUpdateSearcher_put_ServiceID(This,value)  (This)->lpVtbl -> put_ServiceID(This,value)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateSearcher_get_CanAutomaticallyUpgradeService_Proxy(IUpdateSearcher * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateSearcher_get_CanAutomaticallyUpgradeService_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_put_CanAutomaticallyUpgradeService_Proxy(IUpdateSearcher * This, VARIANT_BOOL value);
void __RPC_STUB IUpdateSearcher_put_CanAutomaticallyUpgradeService_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_get_ClientApplicationID_Proxy(IUpdateSearcher * This, BSTR * retval);
void __RPC_STUB IUpdateSearcher_get_ClientApplicationID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_put_ClientApplicationID_Proxy(IUpdateSearcher * This, BSTR value);
void __RPC_STUB IUpdateSearcher_put_ClientApplicationID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_get_IncludePotentiallySupersededUpdates_Proxy(IUpdateSearcher * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateSearcher_get_IncludePotentiallySupersededUpdates_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_put_IncludePotentiallySupersededUpdates_Proxy(IUpdateSearcher * This, VARIANT_BOOL value);
void __RPC_STUB IUpdateSearcher_put_IncludePotentiallySupersededUpdates_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_get_ServerSelection_Proxy(IUpdateSearcher * This, ServerSelection * retval);
void __RPC_STUB IUpdateSearcher_get_ServerSelection_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_put_ServerSelection_Proxy(IUpdateSearcher * This, ServerSelection value);
void __RPC_STUB IUpdateSearcher_put_ServerSelection_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_BeginSearch_Proxy(IUpdateSearcher * This, BSTR criteria, IUnknown * onCompleted, VARIANT state, ISearchJob ** retval);
void __RPC_STUB IUpdateSearcher_BeginSearch_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_EndSearch_Proxy(IUpdateSearcher * This, ISearchJob * searchJob, ISearchResult ** retval);
void __RPC_STUB IUpdateSearcher_EndSearch_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_EscapeString_Proxy(IUpdateSearcher * This, BSTR unescaped, BSTR * retval);
void __RPC_STUB IUpdateSearcher_EscapeString_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_QueryHistory_Proxy(IUpdateSearcher * This, LONG startIndex, LONG count, IUpdateHistoryEntryCollection ** retval);
void __RPC_STUB IUpdateSearcher_QueryHistory_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_Search_Proxy(IUpdateSearcher * This, BSTR criteria, ISearchResult ** retval);
void __RPC_STUB IUpdateSearcher_Search_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_get_Online_Proxy(IUpdateSearcher * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateSearcher_get_Online_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_put_Online_Proxy(IUpdateSearcher * This, VARIANT_BOOL value);
void __RPC_STUB IUpdateSearcher_put_Online_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_GetTotalHistoryCount_Proxy(IUpdateSearcher * This, LONG * retval);
void __RPC_STUB IUpdateSearcher_GetTotalHistoryCount_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_get_ServiceID_Proxy(IUpdateSearcher * This, BSTR * retval);
void __RPC_STUB IUpdateSearcher_get_ServiceID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSearcher_put_ServiceID_Proxy(IUpdateSearcher * This, BSTR value);
void __RPC_STUB IUpdateSearcher_put_ServiceID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateDownloadResult_INTERFACE_DEFINED__
#define __IUpdateDownloadResult_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateDownloadResult;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("bf99af76-b575-42ad-8aa4-33cbb5477af1")IUpdateDownloadResult:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_HResult(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ResultCode(OperationResultCode * retval) = 0;
};
#else
typedef struct IUpdateDownloadResultVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateDownloadResult * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateDownloadResult * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateDownloadResult * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateDownloadResult * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateDownloadResult * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateDownloadResult * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateDownloadResult * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_HResult)(IUpdateDownloadResult * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_ResultCode)(IUpdateDownloadResult * This, OperationResultCode * retval);
	END_INTERFACE
} IUpdateDownloadResultVtbl;

interface IUpdateDownloadResult {
    CONST_VTBL struct IUpdateDownloadResultVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateDownloadResult_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateDownloadResult_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateDownloadResult_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateDownloadResult_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateDownloadResult_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateDownloadResult_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateDownloadResult_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateDownloadResult_get_HResult(This,retval)  (This)->lpVtbl -> get_HResult(This,retval)
#define IUpdateDownloadResult_get_ResultCode(This,retval)  (This)->lpVtbl -> get_ResultCode(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateDownloadResult_get_HResult_Proxy(IUpdateDownloadResult * This, LONG * retval);
void __RPC_STUB IUpdateDownloadResult_get_HResult_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateDownloadResult_get_ResultCode_Proxy(IUpdateDownloadResult * This, OperationResultCode * retval);
void __RPC_STUB IUpdateDownloadResult_get_ResultCode_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IDownloadResult_INTERFACE_DEFINED__
#define __IDownloadResult_INTERFACE_DEFINED__

EXTERN_C const IID IID_IDownloadResult;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("daa4fdd0-4727-4dbe-a1e7-745dca317144")IDownloadResult:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_HResult(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ResultCode(OperationResultCode * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetUpdateResult(LONG updateIndex, IUpdateDownloadResult ** retval) = 0;
};
#else
typedef struct IDownloadResultVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IDownloadResult * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IDownloadResult * This);
	ULONG(STDMETHODCALLTYPE *Release)(IDownloadResult * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IDownloadResult * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IDownloadResult * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IDownloadResult * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IDownloadResult * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_HResult)(IDownloadResult * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_ResultCode)(IDownloadResult * This, OperationResultCode * retval);
	HRESULT(STDMETHODCALLTYPE *GetUpdateResult)(IDownloadResult * This, LONG updateIndex, IUpdateDownloadResult ** retval);
	END_INTERFACE
} IDownloadResultVtbl;

interface IDownloadResult {
    CONST_VTBL struct IDownloadResultVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDownloadResult_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IDownloadResult_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IDownloadResult_Release(This)  (This)->lpVtbl -> Release(This)
#define IDownloadResult_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IDownloadResult_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IDownloadResult_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IDownloadResult_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IDownloadResult_get_HResult(This,retval)  (This)->lpVtbl -> get_HResult(This,retval)
#define IDownloadResult_get_ResultCode(This,retval)  (This)->lpVtbl -> get_ResultCode(This,retval)
#define IDownloadResult_GetUpdateResult(This,updateIndex,retval)  (This)->lpVtbl -> GetUpdateResult(This,updateIndex,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IDownloadResult_get_HResult_Proxy(IDownloadResult * This, LONG * retval);
void __RPC_STUB IDownloadResult_get_HResult_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadResult_get_ResultCode_Proxy(IDownloadResult * This, OperationResultCode * retval);
void __RPC_STUB IDownloadResult_get_ResultCode_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadResult_GetUpdateResult_Proxy(IDownloadResult * This, LONG updateIndex, IUpdateDownloadResult ** retval);
void __RPC_STUB IDownloadResult_GetUpdateResult_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IDownloadProgress_INTERFACE_DEFINED__
#define __IDownloadProgress_INTERFACE_DEFINED__

EXTERN_C const IID IID_IDownloadProgress;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("d31a5bac-f719-4178-9dbb-5e2cb47fd18a")IDownloadProgress:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_CurrentUpdateBytesDownloaded(DECIMAL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_CurrentUpdateBytesToDownload(DECIMAL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_CurrentUpdateIndex(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PercentComplete(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_TotalBytesDownloaded(DECIMAL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_TotalBytesToDownload(DECIMAL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetUpdateResult(LONG updateIndex, IUpdateDownloadResult ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_CurrentUpdateDownloadPhase(DownloadPhase * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_CurrentUpdatePercentComplete(LONG * retval) = 0;
};
#else
typedef struct IDownloadProgressVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IDownloadProgress * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IDownloadProgress * This);
	ULONG(STDMETHODCALLTYPE *Release)(IDownloadProgress * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IDownloadProgress * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IDownloadProgress * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IDownloadProgress * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IDownloadProgress * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_CurrentUpdateBytesDownloaded)(IDownloadProgress * This, DECIMAL * retval);
	HRESULT(STDMETHODCALLTYPE *get_CurrentUpdateBytesToDownload)(IDownloadProgress * This, DECIMAL * retval);
	HRESULT(STDMETHODCALLTYPE *get_CurrentUpdateIndex)(IDownloadProgress * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_PercentComplete)(IDownloadProgress * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_TotalBytesDownloaded)(IDownloadProgress * This, DECIMAL * retval);
	HRESULT(STDMETHODCALLTYPE *get_TotalBytesToDownload)(IDownloadProgress * This, DECIMAL * retval);
	HRESULT(STDMETHODCALLTYPE *GetUpdateResult)(IDownloadProgress * This, LONG updateIndex, IUpdateDownloadResult ** retval);
	HRESULT(STDMETHODCALLTYPE *get_CurrentUpdateDownloadPhase)(IDownloadProgress * This, DownloadPhase * retval);
	HRESULT(STDMETHODCALLTYPE *get_CurrentUpdatePercentComplete)(IDownloadProgress * This, LONG * retval);
	END_INTERFACE
} IDownloadProgressVtbl;

interface IDownloadProgress {
    CONST_VTBL struct IDownloadProgressVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDownloadProgress_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IDownloadProgress_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IDownloadProgress_Release(This)  (This)->lpVtbl -> Release(This)
#define IDownloadProgress_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IDownloadProgress_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IDownloadProgress_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IDownloadProgress_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IDownloadProgress_get_CurrentUpdateBytesDownloaded(This,retval)  (This)->lpVtbl -> get_CurrentUpdateBytesDownloaded(This,retval)
#define IDownloadProgress_get_CurrentUpdateBytesToDownload(This,retval)  (This)->lpVtbl -> get_CurrentUpdateBytesToDownload(This,retval)
#define IDownloadProgress_get_CurrentUpdateIndex(This,retval)  (This)->lpVtbl -> get_CurrentUpdateIndex(This,retval)
#define IDownloadProgress_get_PercentComplete(This,retval)  (This)->lpVtbl -> get_PercentComplete(This,retval)
#define IDownloadProgress_get_TotalBytesDownloaded(This,retval)  (This)->lpVtbl -> get_TotalBytesDownloaded(This,retval)
#define IDownloadProgress_get_TotalBytesToDownload(This,retval)  (This)->lpVtbl -> get_TotalBytesToDownload(This,retval)
#define IDownloadProgress_GetUpdateResult(This,updateIndex,retval)  (This)->lpVtbl -> GetUpdateResult(This,updateIndex,retval)
#define IDownloadProgress_get_CurrentUpdateDownloadPhase(This,retval)  (This)->lpVtbl -> get_CurrentUpdateDownloadPhase(This,retval)
#define IDownloadProgress_get_CurrentUpdatePercentComplete(This,retval)  (This)->lpVtbl -> get_CurrentUpdatePercentComplete(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IDownloadProgress_get_CurrentUpdateBytesDownloaded_Proxy(IDownloadProgress * This, DECIMAL * retval);
void __RPC_STUB IDownloadProgress_get_CurrentUpdateBytesDownloaded_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadProgress_get_CurrentUpdateBytesToDownload_Proxy(IDownloadProgress * This, DECIMAL * retval);
void __RPC_STUB IDownloadProgress_get_CurrentUpdateBytesToDownload_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadProgress_get_CurrentUpdateIndex_Proxy(IDownloadProgress * This, LONG * retval);
void __RPC_STUB IDownloadProgress_get_CurrentUpdateIndex_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadProgress_get_PercentComplete_Proxy(IDownloadProgress * This, LONG * retval);
void __RPC_STUB IDownloadProgress_get_PercentComplete_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadProgress_get_TotalBytesDownloaded_Proxy(IDownloadProgress * This, DECIMAL * retval);
void __RPC_STUB IDownloadProgress_get_TotalBytesDownloaded_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadProgress_get_TotalBytesToDownload_Proxy(IDownloadProgress * This, DECIMAL * retval);
void __RPC_STUB IDownloadProgress_get_TotalBytesToDownload_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadProgress_GetUpdateResult_Proxy(IDownloadProgress * This, LONG updateIndex, IUpdateDownloadResult ** retval);
void __RPC_STUB IDownloadProgress_GetUpdateResult_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadProgress_get_CurrentUpdateDownloadPhase_Proxy(IDownloadProgress * This, DownloadPhase * retval);
void __RPC_STUB IDownloadProgress_get_CurrentUpdateDownloadPhase_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadProgress_get_CurrentUpdatePercentComplete_Proxy(IDownloadProgress * This, LONG * retval);
void __RPC_STUB IDownloadProgress_get_CurrentUpdatePercentComplete_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IDownloadJob_INTERFACE_DEFINED__
#define __IDownloadJob_INTERFACE_DEFINED__

EXTERN_C const IID IID_IDownloadJob;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("c574de85-7358-43f6-aae8-8697e62d8ba7")IDownloadJob:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_AsyncState(VARIANT * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsCompleted(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Updates(IUpdateCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE CleanUp(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetProgress(IDownloadProgress ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE RequestAbort(void) = 0;
};
#else
typedef struct IDownloadJobVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IDownloadJob * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IDownloadJob * This);
	ULONG(STDMETHODCALLTYPE *Release)(IDownloadJob * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IDownloadJob * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IDownloadJob * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IDownloadJob * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IDownloadJob * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_AsyncState)(IDownloadJob * This, VARIANT * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsCompleted)(IDownloadJob * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Updates)(IDownloadJob * This, IUpdateCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *CleanUp)(IDownloadJob * This);
	HRESULT(STDMETHODCALLTYPE *GetProgress)(IDownloadJob * This, IDownloadProgress ** retval);
	HRESULT(STDMETHODCALLTYPE *RequestAbort)(IDownloadJob * This);
	END_INTERFACE
} IDownloadJobVtbl;

interface IDownloadJob {
    CONST_VTBL struct IDownloadJobVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDownloadJob_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IDownloadJob_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IDownloadJob_Release(This)  (This)->lpVtbl -> Release(This)
#define IDownloadJob_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IDownloadJob_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IDownloadJob_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IDownloadJob_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IDownloadJob_get_AsyncState(This,retval)  (This)->lpVtbl -> get_AsyncState(This,retval)
#define IDownloadJob_get_IsCompleted(This,retval)  (This)->lpVtbl -> get_IsCompleted(This,retval)
#define IDownloadJob_get_Updates(This,retval)  (This)->lpVtbl -> get_Updates(This,retval)
#define IDownloadJob_CleanUp(This)  (This)->lpVtbl -> CleanUp(This)
#define IDownloadJob_GetProgress(This,retval)  (This)->lpVtbl -> GetProgress(This,retval)
#define IDownloadJob_RequestAbort(This)  (This)->lpVtbl -> RequestAbort(This)
#endif

#endif

HRESULT STDMETHODCALLTYPE IDownloadJob_get_AsyncState_Proxy(IDownloadJob * This, VARIANT * retval);
void __RPC_STUB IDownloadJob_get_AsyncState_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadJob_get_IsCompleted_Proxy(IDownloadJob * This, VARIANT_BOOL * retval);
void __RPC_STUB IDownloadJob_get_IsCompleted_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadJob_get_Updates_Proxy(IDownloadJob * This, IUpdateCollection ** retval);
void __RPC_STUB IDownloadJob_get_Updates_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadJob_CleanUp_Proxy(IDownloadJob * This);
void __RPC_STUB IDownloadJob_CleanUp_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadJob_GetProgress_Proxy(IDownloadJob * This, IDownloadProgress ** retval);
void __RPC_STUB IDownloadJob_GetProgress_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IDownloadJob_RequestAbort_Proxy(IDownloadJob * This);
void __RPC_STUB IDownloadJob_RequestAbort_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IDownloadCompletedCallbackArgs_INTERFACE_DEFINED__
#define __IDownloadCompletedCallbackArgs_INTERFACE_DEFINED__

EXTERN_C const IID IID_IDownloadCompletedCallbackArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("fa565b23-498c-47a0-979d-e7d5b1813360")IDownloadCompletedCallbackArgs:public IDispatch
{
	public:
};
#else
typedef struct IDownloadCompletedCallbackArgsVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IDownloadCompletedCallbackArgs * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IDownloadCompletedCallbackArgs * This);
	ULONG(STDMETHODCALLTYPE *Release)(IDownloadCompletedCallbackArgs * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IDownloadCompletedCallbackArgs * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IDownloadCompletedCallbackArgs * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IDownloadCompletedCallbackArgs * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IDownloadCompletedCallbackArgs * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	END_INTERFACE
} IDownloadCompletedCallbackArgsVtbl;

interface IDownloadCompletedCallbackArgs {
    CONST_VTBL struct IDownloadCompletedCallbackArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDownloadCompletedCallbackArgs_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IDownloadCompletedCallbackArgs_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IDownloadCompletedCallbackArgs_Release(This)  (This)->lpVtbl -> Release(This)
#define IDownloadCompletedCallbackArgs_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IDownloadCompletedCallbackArgs_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IDownloadCompletedCallbackArgs_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IDownloadCompletedCallbackArgs_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#endif

#endif

#endif

#ifndef __IDownloadCompletedCallback_INTERFACE_DEFINED__
#define __IDownloadCompletedCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IDownloadCompletedCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("77254866-9f5b-4c8e-b9e2-c77a8530d64b")IDownloadCompletedCallback:public IUnknown
{
	public:
	virtual HRESULT STDMETHODCALLTYPE Invoke(IDownloadJob * downloadJob, IDownloadCompletedCallbackArgs * callbackArgs) = 0;
};
#else
typedef struct IDownloadCompletedCallbackVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IDownloadCompletedCallback * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IDownloadCompletedCallback * This);
	ULONG(STDMETHODCALLTYPE *Release)(IDownloadCompletedCallback * This);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IDownloadCompletedCallback * This, IDownloadJob * downloadJob, IDownloadCompletedCallbackArgs * callbackArgs);
	END_INTERFACE
} IDownloadCompletedCallbackVtbl;

interface IDownloadCompletedCallback {
    CONST_VTBL struct IDownloadCompletedCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDownloadCompletedCallback_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IDownloadCompletedCallback_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IDownloadCompletedCallback_Release(This)  (This)->lpVtbl -> Release(This)
#define IDownloadCompletedCallback_Invoke(This,downloadJob,callbackArgs)  (This)->lpVtbl -> Invoke(This,downloadJob,callbackArgs)
#endif

#endif

HRESULT STDMETHODCALLTYPE IDownloadCompletedCallback_Invoke_Proxy(IDownloadCompletedCallback * This, IDownloadJob * downloadJob, IDownloadCompletedCallbackArgs * callbackArgs);
void __RPC_STUB IDownloadCompletedCallback_Invoke_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IDownloadProgressChangedCallbackArgs_INTERFACE_DEFINED__
#define __IDownloadProgressChangedCallbackArgs_INTERFACE_DEFINED__

EXTERN_C const IID IID_IDownloadProgressChangedCallbackArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("324ff2c6-4981-4b04-9412-57481745ab24")IDownloadProgressChangedCallbackArgs:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Progress(IDownloadProgress ** retval) = 0;
};
#else
typedef struct IDownloadProgressChangedCallbackArgsVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IDownloadProgressChangedCallbackArgs * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IDownloadProgressChangedCallbackArgs * This);
	ULONG(STDMETHODCALLTYPE *Release)(IDownloadProgressChangedCallbackArgs * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IDownloadProgressChangedCallbackArgs * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IDownloadProgressChangedCallbackArgs * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IDownloadProgressChangedCallbackArgs * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IDownloadProgressChangedCallbackArgs * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Progress)(IDownloadProgressChangedCallbackArgs * This, IDownloadProgress ** retval);
	END_INTERFACE
} IDownloadProgressChangedCallbackArgsVtbl;

interface IDownloadProgressChangedCallbackArgs {
    CONST_VTBL struct IDownloadProgressChangedCallbackArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDownloadProgressChangedCallbackArgs_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IDownloadProgressChangedCallbackArgs_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IDownloadProgressChangedCallbackArgs_Release(This)  (This)->lpVtbl -> Release(This)
#define IDownloadProgressChangedCallbackArgs_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IDownloadProgressChangedCallbackArgs_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IDownloadProgressChangedCallbackArgs_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IDownloadProgressChangedCallbackArgs_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IDownloadProgressChangedCallbackArgs_get_Progress(This,retval)  (This)->lpVtbl -> get_Progress(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IDownloadProgressChangedCallbackArgs_get_Progress_Proxy(IDownloadProgressChangedCallbackArgs * This, IDownloadProgress ** retval);
void __RPC_STUB IDownloadProgressChangedCallbackArgs_get_Progress_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IDownloadProgressChangedCallback_INTERFACE_DEFINED__
#define __IDownloadProgressChangedCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IDownloadProgressChangedCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("8c3f1cdd-6173-4591-aebd-a56a53ca77c1")IDownloadProgressChangedCallback:public IUnknown
{
	public:
	virtual HRESULT STDMETHODCALLTYPE Invoke(IDownloadJob * downloadJob, IDownloadProgressChangedCallbackArgs * callbackArgs) = 0;
};
#else
typedef struct IDownloadProgressChangedCallbackVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IDownloadProgressChangedCallback * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IDownloadProgressChangedCallback * This);
	ULONG(STDMETHODCALLTYPE *Release)(IDownloadProgressChangedCallback * This);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IDownloadProgressChangedCallback * This, IDownloadJob * downloadJob, IDownloadProgressChangedCallbackArgs * callbackArgs);
	END_INTERFACE
} IDownloadProgressChangedCallbackVtbl;

interface IDownloadProgressChangedCallback {
    CONST_VTBL struct IDownloadProgressChangedCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDownloadProgressChangedCallback_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IDownloadProgressChangedCallback_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IDownloadProgressChangedCallback_Release(This)  (This)->lpVtbl -> Release(This)
#define IDownloadProgressChangedCallback_Invoke(This,downloadJob,callbackArgs)  (This)->lpVtbl -> Invoke(This,downloadJob,callbackArgs)
#endif

#endif

HRESULT STDMETHODCALLTYPE IDownloadProgressChangedCallback_Invoke_Proxy(IDownloadProgressChangedCallback * This, IDownloadJob * downloadJob, IDownloadProgressChangedCallbackArgs * callbackArgs);
void __RPC_STUB IDownloadProgressChangedCallback_Invoke_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateDownloader_INTERFACE_DEFINED__
#define __IUpdateDownloader_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateDownloader;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("68f1c6f9-7ecc-4666-a464-247fe12496c3")IUpdateDownloader:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_ClientApplicationID(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ClientApplicationID(BSTR value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsForced(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_IsForced(VARIANT_BOOL value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Priority(DownloadPriority * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Priority(DownloadPriority value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Updates(IUpdateCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Updates(IUpdateCollection * value) = 0;
	virtual HRESULT STDMETHODCALLTYPE BeginDownload(IUnknown * onProgressChanged, IUnknown * onCompleted, VARIANT state, IDownloadJob ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE Download(IDownloadResult ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE EndDownload(IDownloadJob * value, IDownloadResult ** retval) = 0;
};
#else
typedef struct IUpdateDownloaderVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateDownloader * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateDownloader * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateDownloader * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateDownloader * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateDownloader * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateDownloader * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateDownloader * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_ClientApplicationID)(IUpdateDownloader * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *put_ClientApplicationID)(IUpdateDownloader * This, BSTR value);
	HRESULT(STDMETHODCALLTYPE *get_IsForced)(IUpdateDownloader * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_IsForced)(IUpdateDownloader * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *get_Priority)(IUpdateDownloader * This, DownloadPriority * retval);
	HRESULT(STDMETHODCALLTYPE *put_Priority)(IUpdateDownloader * This, DownloadPriority value);
	HRESULT(STDMETHODCALLTYPE *get_Updates)(IUpdateDownloader * This, IUpdateCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *put_Updates)(IUpdateDownloader * This, IUpdateCollection * value);
	HRESULT(STDMETHODCALLTYPE *BeginDownload)(IUpdateDownloader * This, IUnknown * onProgressChanged, IUnknown * onCompleted, VARIANT state, IDownloadJob ** retval);
	HRESULT(STDMETHODCALLTYPE *Download)(IUpdateDownloader * This, IDownloadResult ** retval);
	HRESULT(STDMETHODCALLTYPE *EndDownload)(IUpdateDownloader * This, IDownloadJob * value, IDownloadResult ** retval);
	END_INTERFACE
} IUpdateDownloaderVtbl;

interface IUpdateDownloader {
    CONST_VTBL struct IUpdateDownloaderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateDownloader_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateDownloader_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateDownloader_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateDownloader_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateDownloader_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateDownloader_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateDownloader_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateDownloader_get_ClientApplicationID(This,retval)  (This)->lpVtbl -> get_ClientApplicationID(This,retval)
#define IUpdateDownloader_put_ClientApplicationID(This,value)  (This)->lpVtbl -> put_ClientApplicationID(This,value)
#define IUpdateDownloader_get_IsForced(This,retval)  (This)->lpVtbl -> get_IsForced(This,retval)
#define IUpdateDownloader_put_IsForced(This,value)  (This)->lpVtbl -> put_IsForced(This,value)
#define IUpdateDownloader_get_Priority(This,retval)  (This)->lpVtbl -> get_Priority(This,retval)
#define IUpdateDownloader_put_Priority(This,value)  (This)->lpVtbl -> put_Priority(This,value)
#define IUpdateDownloader_get_Updates(This,retval)  (This)->lpVtbl -> get_Updates(This,retval)
#define IUpdateDownloader_put_Updates(This,value)  (This)->lpVtbl -> put_Updates(This,value)
#define IUpdateDownloader_BeginDownload(This,onProgressChanged,onCompleted,state,retval)  (This)->lpVtbl -> BeginDownload(This,onProgressChanged,onCompleted,state,retval)
#define IUpdateDownloader_Download(This,retval)  (This)->lpVtbl -> Download(This,retval)
#define IUpdateDownloader_EndDownload(This,value,retval)  (This)->lpVtbl -> EndDownload(This,value,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateDownloader_get_ClientApplicationID_Proxy(IUpdateDownloader * This, BSTR * retval);
void __RPC_STUB IUpdateDownloader_get_ClientApplicationID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateDownloader_put_ClientApplicationID_Proxy(IUpdateDownloader * This, BSTR value);
void __RPC_STUB IUpdateDownloader_put_ClientApplicationID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateDownloader_get_IsForced_Proxy(IUpdateDownloader * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateDownloader_get_IsForced_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateDownloader_put_IsForced_Proxy(IUpdateDownloader * This, VARIANT_BOOL value);
void __RPC_STUB IUpdateDownloader_put_IsForced_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateDownloader_get_Priority_Proxy(IUpdateDownloader * This, DownloadPriority * retval);
void __RPC_STUB IUpdateDownloader_get_Priority_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateDownloader_put_Priority_Proxy(IUpdateDownloader * This, DownloadPriority value);
void __RPC_STUB IUpdateDownloader_put_Priority_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateDownloader_get_Updates_Proxy(IUpdateDownloader * This, IUpdateCollection ** retval);
void __RPC_STUB IUpdateDownloader_get_Updates_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateDownloader_put_Updates_Proxy(IUpdateDownloader * This, IUpdateCollection * value);
void __RPC_STUB IUpdateDownloader_put_Updates_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateDownloader_BeginDownload_Proxy(IUpdateDownloader * This, IUnknown * onProgressChanged, IUnknown * onCompleted, VARIANT state, IDownloadJob ** retval);
void __RPC_STUB IUpdateDownloader_BeginDownload_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateDownloader_Download_Proxy(IUpdateDownloader * This, IDownloadResult ** retval);
void __RPC_STUB IUpdateDownloader_Download_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateDownloader_EndDownload_Proxy(IUpdateDownloader * This, IDownloadJob * value, IDownloadResult ** retval);
void __RPC_STUB IUpdateDownloader_EndDownload_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateInstallationResult_INTERFACE_DEFINED__
#define __IUpdateInstallationResult_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateInstallationResult;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("d940f0f8-3cbb-4fd0-993f-471e7f2328ad")IUpdateInstallationResult:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_HResult(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_RebootRequired(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ResultCode(OperationResultCode * retval) = 0;
};
#else
typedef struct IUpdateInstallationResultVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateInstallationResult * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateInstallationResult * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateInstallationResult * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateInstallationResult * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateInstallationResult * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateInstallationResult * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateInstallationResult * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_HResult)(IUpdateInstallationResult * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_RebootRequired)(IUpdateInstallationResult * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_ResultCode)(IUpdateInstallationResult * This, OperationResultCode * retval);
	END_INTERFACE
} IUpdateInstallationResultVtbl;

interface IUpdateInstallationResult {
    CONST_VTBL struct IUpdateInstallationResultVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateInstallationResult_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateInstallationResult_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateInstallationResult_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateInstallationResult_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateInstallationResult_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateInstallationResult_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateInstallationResult_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateInstallationResult_get_HResult(This,retval)  (This)->lpVtbl -> get_HResult(This,retval)
#define IUpdateInstallationResult_get_RebootRequired(This,retval)  (This)->lpVtbl -> get_RebootRequired(This,retval)
#define IUpdateInstallationResult_get_ResultCode(This,retval)  (This)->lpVtbl -> get_ResultCode(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateInstallationResult_get_HResult_Proxy(IUpdateInstallationResult * This, LONG * retval);
void __RPC_STUB IUpdateInstallationResult_get_HResult_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstallationResult_get_RebootRequired_Proxy(IUpdateInstallationResult * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateInstallationResult_get_RebootRequired_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstallationResult_get_ResultCode_Proxy(IUpdateInstallationResult * This, OperationResultCode * retval);
void __RPC_STUB IUpdateInstallationResult_get_ResultCode_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IInstallationResult_INTERFACE_DEFINED__
#define __IInstallationResult_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInstallationResult;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("a43c56d6-7451-48d4-af96-b6cd2d0d9b7a")IInstallationResult:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_HResult(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_RebootRequired(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ResultCode(OperationResultCode * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetUpdateResult(LONG updateIndex, IUpdateInstallationResult ** retval) = 0;
};
#else
typedef struct IInstallationResultVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInstallationResult * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IInstallationResult * This);
	ULONG(STDMETHODCALLTYPE *Release)(IInstallationResult * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IInstallationResult * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IInstallationResult * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IInstallationResult * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IInstallationResult * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_HResult)(IInstallationResult * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_RebootRequired)(IInstallationResult * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_ResultCode)(IInstallationResult * This, OperationResultCode * retval);
	HRESULT(STDMETHODCALLTYPE *GetUpdateResult)(IInstallationResult * This, LONG updateIndex, IUpdateInstallationResult ** retval);
	END_INTERFACE
} IInstallationResultVtbl;

interface IInstallationResult {
    CONST_VTBL struct IInstallationResultVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInstallationResult_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IInstallationResult_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IInstallationResult_Release(This)  (This)->lpVtbl -> Release(This)
#define IInstallationResult_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IInstallationResult_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IInstallationResult_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IInstallationResult_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IInstallationResult_get_HResult(This,retval)  (This)->lpVtbl -> get_HResult(This,retval)
#define IInstallationResult_get_RebootRequired(This,retval)  (This)->lpVtbl -> get_RebootRequired(This,retval)
#define IInstallationResult_get_ResultCode(This,retval)  (This)->lpVtbl -> get_ResultCode(This,retval)
#define IInstallationResult_GetUpdateResult(This,updateIndex,retval)  (This)->lpVtbl -> GetUpdateResult(This,updateIndex,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IInstallationResult_get_HResult_Proxy(IInstallationResult * This, LONG * retval);
void __RPC_STUB IInstallationResult_get_HResult_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationResult_get_RebootRequired_Proxy(IInstallationResult * This, VARIANT_BOOL * retval);
void __RPC_STUB IInstallationResult_get_RebootRequired_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationResult_get_ResultCode_Proxy(IInstallationResult * This, OperationResultCode * retval);
void __RPC_STUB IInstallationResult_get_ResultCode_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationResult_GetUpdateResult_Proxy(IInstallationResult * This, LONG updateIndex, IUpdateInstallationResult ** retval);
void __RPC_STUB IInstallationResult_GetUpdateResult_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IInstallationProgress_INTERFACE_DEFINED__
#define __IInstallationProgress_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInstallationProgress;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("345c8244-43a3-4e32-a368-65f073b76f36")IInstallationProgress:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_CurrentUpdateIndex(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_CurrentUpdatePercentComplete(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PercentComplete(LONG * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetUpdateResult(LONG updateIndex, IUpdateInstallationResult ** retval) = 0;
};
#else
typedef struct IInstallationProgressVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInstallationProgress * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IInstallationProgress * This);
	ULONG(STDMETHODCALLTYPE *Release)(IInstallationProgress * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IInstallationProgress * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IInstallationProgress * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IInstallationProgress * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IInstallationProgress * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_CurrentUpdateIndex)(IInstallationProgress * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_CurrentUpdatePercentComplete)(IInstallationProgress * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *get_PercentComplete)(IInstallationProgress * This, LONG * retval);
	HRESULT(STDMETHODCALLTYPE *GetUpdateResult)(IInstallationProgress * This, LONG updateIndex, IUpdateInstallationResult ** retval);
	END_INTERFACE
} IInstallationProgressVtbl;

interface IInstallationProgress {
    CONST_VTBL struct IInstallationProgressVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInstallationProgress_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IInstallationProgress_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IInstallationProgress_Release(This)  (This)->lpVtbl -> Release(This)
#define IInstallationProgress_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IInstallationProgress_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IInstallationProgress_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IInstallationProgress_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IInstallationProgress_get_CurrentUpdateIndex(This,retval)  (This)->lpVtbl -> get_CurrentUpdateIndex(This,retval)
#define IInstallationProgress_get_CurrentUpdatePercentComplete(This,retval)  (This)->lpVtbl -> get_CurrentUpdatePercentComplete(This,retval)
#define IInstallationProgress_get_PercentComplete(This,retval)  (This)->lpVtbl -> get_PercentComplete(This,retval)
#define IInstallationProgress_GetUpdateResult(This,updateIndex,retval)  (This)->lpVtbl -> GetUpdateResult(This,updateIndex,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IInstallationProgress_get_CurrentUpdateIndex_Proxy(IInstallationProgress * This, LONG * retval);
void __RPC_STUB IInstallationProgress_get_CurrentUpdateIndex_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationProgress_get_CurrentUpdatePercentComplete_Proxy(IInstallationProgress * This, LONG * retval);
void __RPC_STUB IInstallationProgress_get_CurrentUpdatePercentComplete_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationProgress_get_PercentComplete_Proxy(IInstallationProgress * This, LONG * retval);
void __RPC_STUB IInstallationProgress_get_PercentComplete_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationProgress_GetUpdateResult_Proxy(IInstallationProgress * This, LONG updateIndex, IUpdateInstallationResult ** retval);
void __RPC_STUB IInstallationProgress_GetUpdateResult_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IInstallationJob_INTERFACE_DEFINED__
#define __IInstallationJob_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInstallationJob;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5c209f0b-bad5-432a-9556-4699bed2638a")IInstallationJob:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_AsyncState(VARIANT * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsCompleted(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Updates(IUpdateCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE CleanUp(void) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetProgress(IInstallationProgress ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE RequestAbort(void) = 0;
};
#else
typedef struct IInstallationJobVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInstallationJob * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IInstallationJob * This);
	ULONG(STDMETHODCALLTYPE *Release)(IInstallationJob * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IInstallationJob * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IInstallationJob * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IInstallationJob * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IInstallationJob * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_AsyncState)(IInstallationJob * This, VARIANT * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsCompleted)(IInstallationJob * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_Updates)(IInstallationJob * This, IUpdateCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *CleanUp)(IInstallationJob * This);
	HRESULT(STDMETHODCALLTYPE *GetProgress)(IInstallationJob * This, IInstallationProgress ** retval);
	HRESULT(STDMETHODCALLTYPE *RequestAbort)(IInstallationJob * This);
	END_INTERFACE
} IInstallationJobVtbl;

interface IInstallationJob {
    CONST_VTBL struct IInstallationJobVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInstallationJob_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IInstallationJob_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IInstallationJob_Release(This)  (This)->lpVtbl -> Release(This)
#define IInstallationJob_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IInstallationJob_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IInstallationJob_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IInstallationJob_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IInstallationJob_get_AsyncState(This,retval)  (This)->lpVtbl -> get_AsyncState(This,retval)
#define IInstallationJob_get_IsCompleted(This,retval)  (This)->lpVtbl -> get_IsCompleted(This,retval)
#define IInstallationJob_get_Updates(This,retval)  (This)->lpVtbl -> get_Updates(This,retval)
#define IInstallationJob_CleanUp(This)  (This)->lpVtbl -> CleanUp(This)
#define IInstallationJob_GetProgress(This,retval)  (This)->lpVtbl -> GetProgress(This,retval)
#define IInstallationJob_RequestAbort(This)  (This)->lpVtbl -> RequestAbort(This)
#endif

#endif

HRESULT STDMETHODCALLTYPE IInstallationJob_get_AsyncState_Proxy(IInstallationJob * This, VARIANT * retval);
void __RPC_STUB IInstallationJob_get_AsyncState_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationJob_get_IsCompleted_Proxy(IInstallationJob * This, VARIANT_BOOL * retval);
void __RPC_STUB IInstallationJob_get_IsCompleted_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationJob_get_Updates_Proxy(IInstallationJob * This, IUpdateCollection ** retval);
void __RPC_STUB IInstallationJob_get_Updates_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationJob_CleanUp_Proxy(IInstallationJob * This);
void __RPC_STUB IInstallationJob_CleanUp_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationJob_GetProgress_Proxy(IInstallationJob * This, IInstallationProgress ** retval);
void __RPC_STUB IInstallationJob_GetProgress_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IInstallationJob_RequestAbort_Proxy(IInstallationJob * This);
void __RPC_STUB IInstallationJob_RequestAbort_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IInstallationCompletedCallbackArgs_INTERFACE_DEFINED__
#define __IInstallationCompletedCallbackArgs_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInstallationCompletedCallbackArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("250e2106-8efb-4705-9653-ef13c581b6a1")IInstallationCompletedCallbackArgs:public IDispatch
{
	public:
};
#else
typedef struct IInstallationCompletedCallbackArgsVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInstallationCompletedCallbackArgs * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IInstallationCompletedCallbackArgs * This);
	ULONG(STDMETHODCALLTYPE *Release)(IInstallationCompletedCallbackArgs * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IInstallationCompletedCallbackArgs * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IInstallationCompletedCallbackArgs * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IInstallationCompletedCallbackArgs * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IInstallationCompletedCallbackArgs * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	END_INTERFACE
} IInstallationCompletedCallbackArgsVtbl;

interface IInstallationCompletedCallbackArgs {
    CONST_VTBL struct IInstallationCompletedCallbackArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInstallationCompletedCallbackArgs_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IInstallationCompletedCallbackArgs_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IInstallationCompletedCallbackArgs_Release(This)  (This)->lpVtbl -> Release(This)
#define IInstallationCompletedCallbackArgs_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IInstallationCompletedCallbackArgs_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IInstallationCompletedCallbackArgs_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IInstallationCompletedCallbackArgs_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#endif

#endif

#endif

#ifndef __IInstallationCompletedCallback_INTERFACE_DEFINED__
#define __IInstallationCompletedCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInstallationCompletedCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("45f4f6f3-d602-4f98-9a8a-3efa152ad2d3")IInstallationCompletedCallback:public IUnknown
{
	public:
	virtual HRESULT STDMETHODCALLTYPE Invoke(IInstallationJob * installationJob, IInstallationCompletedCallbackArgs * callbackArgs) = 0;
};
#else
typedef struct IInstallationCompletedCallbackVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInstallationCompletedCallback * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IInstallationCompletedCallback * This);
	ULONG(STDMETHODCALLTYPE *Release)(IInstallationCompletedCallback * This);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IInstallationCompletedCallback * This, IInstallationJob * installationJob, IInstallationCompletedCallbackArgs * callbackArgs);
	END_INTERFACE
} IInstallationCompletedCallbackVtbl;

interface IInstallationCompletedCallback {
    CONST_VTBL struct IInstallationCompletedCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInstallationCompletedCallback_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IInstallationCompletedCallback_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IInstallationCompletedCallback_Release(This)  (This)->lpVtbl -> Release(This)
#define IInstallationCompletedCallback_Invoke(This,installationJob,callbackArgs)  (This)->lpVtbl -> Invoke(This,installationJob,callbackArgs)
#endif

#endif

HRESULT STDMETHODCALLTYPE IInstallationCompletedCallback_Invoke_Proxy(IInstallationCompletedCallback * This, IInstallationJob * installationJob, IInstallationCompletedCallbackArgs * callbackArgs);
void __RPC_STUB IInstallationCompletedCallback_Invoke_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IInstallationProgressChangedCallbackArgs_INTERFACE_DEFINED__
#define __IInstallationProgressChangedCallbackArgs_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInstallationProgressChangedCallbackArgs;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e4f14e1e-689d-4218-a0b9-bc189c484a01")IInstallationProgressChangedCallbackArgs:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Progress(IInstallationProgress ** retval) = 0;
};
#else
typedef struct IInstallationProgressChangedCallbackArgsVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInstallationProgressChangedCallbackArgs * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IInstallationProgressChangedCallbackArgs * This);
	ULONG(STDMETHODCALLTYPE *Release)(IInstallationProgressChangedCallbackArgs * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IInstallationProgressChangedCallbackArgs * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IInstallationProgressChangedCallbackArgs * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IInstallationProgressChangedCallbackArgs * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IInstallationProgressChangedCallbackArgs * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Progress)(IInstallationProgressChangedCallbackArgs * This, IInstallationProgress ** retval);
	END_INTERFACE
} IInstallationProgressChangedCallbackArgsVtbl;

interface IInstallationProgressChangedCallbackArgs {
    CONST_VTBL struct IInstallationProgressChangedCallbackArgsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInstallationProgressChangedCallbackArgs_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IInstallationProgressChangedCallbackArgs_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IInstallationProgressChangedCallbackArgs_Release(This)  (This)->lpVtbl -> Release(This)
#define IInstallationProgressChangedCallbackArgs_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IInstallationProgressChangedCallbackArgs_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IInstallationProgressChangedCallbackArgs_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IInstallationProgressChangedCallbackArgs_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IInstallationProgressChangedCallbackArgs_get_Progress(This,retval)  (This)->lpVtbl -> get_Progress(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IInstallationProgressChangedCallbackArgs_get_Progress_Proxy(IInstallationProgressChangedCallbackArgs * This, IInstallationProgress ** retval);
void __RPC_STUB IInstallationProgressChangedCallbackArgs_get_Progress_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IInstallationProgressChangedCallback_INTERFACE_DEFINED__
#define __IInstallationProgressChangedCallback_INTERFACE_DEFINED__

EXTERN_C const IID IID_IInstallationProgressChangedCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e01402d5-f8da-43ba-a012-38894bd048f1")IInstallationProgressChangedCallback:public IUnknown
{
	public:
	virtual HRESULT STDMETHODCALLTYPE Invoke(IInstallationJob * installationJob, IInstallationProgressChangedCallbackArgs * callbackArgs) = 0;
};
#else
typedef struct IInstallationProgressChangedCallbackVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IInstallationProgressChangedCallback * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IInstallationProgressChangedCallback * This);
	ULONG(STDMETHODCALLTYPE *Release)(IInstallationProgressChangedCallback * This);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IInstallationProgressChangedCallback * This, IInstallationJob * installationJob, IInstallationProgressChangedCallbackArgs * callbackArgs);
	END_INTERFACE
} IInstallationProgressChangedCallbackVtbl;

interface IInstallationProgressChangedCallback {
    CONST_VTBL struct IInstallationProgressChangedCallbackVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IInstallationProgressChangedCallback_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IInstallationProgressChangedCallback_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IInstallationProgressChangedCallback_Release(This)  (This)->lpVtbl -> Release(This)
#define IInstallationProgressChangedCallback_Invoke(This,installationJob,callbackArgs)  (This)->lpVtbl -> Invoke(This,installationJob,callbackArgs)
#endif

#endif

HRESULT STDMETHODCALLTYPE IInstallationProgressChangedCallback_Invoke_Proxy(IInstallationProgressChangedCallback * This, IInstallationJob * installationJob, IInstallationProgressChangedCallbackArgs * callbackArgs);
void __RPC_STUB IInstallationProgressChangedCallback_Invoke_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateInstaller_INTERFACE_DEFINED__
#define __IUpdateInstaller_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateInstaller;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7b929c68-ccdc-4226-96b1-8724600b54c2")IUpdateInstaller:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_ClientApplicationID(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ClientApplicationID(BSTR value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsForced(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_IsForced(VARIANT_BOOL value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ParentHwnd(HWND * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ParentHwnd(HWND value) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ParentWindow(IUnknown * value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ParentWindow(IUnknown ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Updates(IUpdateCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Updates(IUpdateCollection * value) = 0;
	virtual HRESULT STDMETHODCALLTYPE BeginInstall(IUnknown * onProgressChanged, IUnknown * onCompleted, VARIANT state, IInstallationJob ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE BeginUninstall(IUnknown * onProgressChanged, IUnknown * onCompleted, VARIANT state, IInstallationJob ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE EndInstall(IInstallationJob * value, IInstallationResult ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE EndUninstall(IInstallationJob * value, IInstallationResult ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE Install(IInstallationResult ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE RunWizard(BSTR dialogTitle, IInstallationResult ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsBusy(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE Uninstall(IInstallationResult ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_AllowSourcePrompts(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_AllowSourcePrompts(VARIANT_BOOL value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_RebootRequiredBeforeInstallation(VARIANT_BOOL * retval) = 0;
};
#else
typedef struct IUpdateInstallerVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateInstaller * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateInstaller * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateInstaller * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateInstaller * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateInstaller * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateInstaller * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateInstaller * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_ClientApplicationID)(IUpdateInstaller * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *put_ClientApplicationID)(IUpdateInstaller * This, BSTR value);
	HRESULT(STDMETHODCALLTYPE *get_IsForced)(IUpdateInstaller * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_IsForced)(IUpdateInstaller * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *get_ParentHwnd)(IUpdateInstaller * This, HWND * retval);
	HRESULT(STDMETHODCALLTYPE *put_ParentHwnd)(IUpdateInstaller * This, HWND value);
	HRESULT(STDMETHODCALLTYPE *put_ParentWindow)(IUpdateInstaller * This, IUnknown * value);
	HRESULT(STDMETHODCALLTYPE *get_ParentWindow)(IUpdateInstaller * This, IUnknown ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Updates)(IUpdateInstaller * This, IUpdateCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *put_Updates)(IUpdateInstaller * This, IUpdateCollection * value);
	HRESULT(STDMETHODCALLTYPE *BeginInstall)(IUpdateInstaller * This, IUnknown * onProgressChanged, IUnknown * onCompleted, VARIANT state, IInstallationJob ** retval);
	HRESULT(STDMETHODCALLTYPE *BeginUninstall)(IUpdateInstaller * This, IUnknown * onProgressChanged, IUnknown * onCompleted, VARIANT state, IInstallationJob ** retval);
	HRESULT(STDMETHODCALLTYPE *EndInstall)(IUpdateInstaller * This, IInstallationJob * value, IInstallationResult ** retval);
	HRESULT(STDMETHODCALLTYPE *EndUninstall)(IUpdateInstaller * This, IInstallationJob * value, IInstallationResult ** retval);
	HRESULT(STDMETHODCALLTYPE *Install)(IUpdateInstaller * This, IInstallationResult ** retval);
	HRESULT(STDMETHODCALLTYPE *RunWizard)(IUpdateInstaller * This, BSTR dialogTitle, IInstallationResult ** retval);
	HRESULT(STDMETHODCALLTYPE *get_IsBusy)(IUpdateInstaller * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *Uninstall)(IUpdateInstaller * This, IInstallationResult ** retval);
	HRESULT(STDMETHODCALLTYPE *get_AllowSourcePrompts)(IUpdateInstaller * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_AllowSourcePrompts)(IUpdateInstaller * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *get_RebootRequiredBeforeInstallation)(IUpdateInstaller * This, VARIANT_BOOL * retval);
	END_INTERFACE
} IUpdateInstallerVtbl;

interface IUpdateInstaller {
    CONST_VTBL struct IUpdateInstallerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateInstaller_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateInstaller_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateInstaller_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateInstaller_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateInstaller_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateInstaller_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateInstaller_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateInstaller_get_ClientApplicationID(This,retval)  (This)->lpVtbl -> get_ClientApplicationID(This,retval)
#define IUpdateInstaller_put_ClientApplicationID(This,value)  (This)->lpVtbl -> put_ClientApplicationID(This,value)
#define IUpdateInstaller_get_IsForced(This,retval)  (This)->lpVtbl -> get_IsForced(This,retval)
#define IUpdateInstaller_put_IsForced(This,value)  (This)->lpVtbl -> put_IsForced(This,value)
#define IUpdateInstaller_get_ParentHwnd(This,retval)  (This)->lpVtbl -> get_ParentHwnd(This,retval)
#define IUpdateInstaller_put_ParentHwnd(This,value)  (This)->lpVtbl -> put_ParentHwnd(This,value)
#define IUpdateInstaller_put_ParentWindow(This,value)  (This)->lpVtbl -> put_ParentWindow(This,value)
#define IUpdateInstaller_get_ParentWindow(This,retval)  (This)->lpVtbl -> get_ParentWindow(This,retval)
#define IUpdateInstaller_get_Updates(This,retval)  (This)->lpVtbl -> get_Updates(This,retval)
#define IUpdateInstaller_put_Updates(This,value)  (This)->lpVtbl -> put_Updates(This,value)
#define IUpdateInstaller_BeginInstall(This,onProgressChanged,onCompleted,state,retval)  (This)->lpVtbl -> BeginInstall(This,onProgressChanged,onCompleted,state,retval)
#define IUpdateInstaller_BeginUninstall(This,onProgressChanged,onCompleted,state,retval)  (This)->lpVtbl -> BeginUninstall(This,onProgressChanged,onCompleted,state,retval)
#define IUpdateInstaller_EndInstall(This,value,retval)  (This)->lpVtbl -> EndInstall(This,value,retval)
#define IUpdateInstaller_EndUninstall(This,value,retval)  (This)->lpVtbl -> EndUninstall(This,value,retval)
#define IUpdateInstaller_Install(This,retval)  (This)->lpVtbl -> Install(This,retval)
#define IUpdateInstaller_RunWizard(This,dialogTitle,retval)  (This)->lpVtbl -> RunWizard(This,dialogTitle,retval)
#define IUpdateInstaller_get_IsBusy(This,retval)  (This)->lpVtbl -> get_IsBusy(This,retval)
#define IUpdateInstaller_Uninstall(This,retval)  (This)->lpVtbl -> Uninstall(This,retval)
#define IUpdateInstaller_get_AllowSourcePrompts(This,retval)  (This)->lpVtbl -> get_AllowSourcePrompts(This,retval)
#define IUpdateInstaller_put_AllowSourcePrompts(This,value)  (This)->lpVtbl -> put_AllowSourcePrompts(This,value)
#define IUpdateInstaller_get_RebootRequiredBeforeInstallation(This,retval)  (This)->lpVtbl -> get_RebootRequiredBeforeInstallation(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateInstaller_get_ClientApplicationID_Proxy(IUpdateInstaller * This, BSTR * retval);
void __RPC_STUB IUpdateInstaller_get_ClientApplicationID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_put_ClientApplicationID_Proxy(IUpdateInstaller * This, BSTR value);
void __RPC_STUB IUpdateInstaller_put_ClientApplicationID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_get_IsForced_Proxy(IUpdateInstaller * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateInstaller_get_IsForced_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_put_IsForced_Proxy(IUpdateInstaller * This, VARIANT_BOOL value);
void __RPC_STUB IUpdateInstaller_put_IsForced_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_get_ParentHwnd_Proxy(IUpdateInstaller * This, HWND * retval);
void __RPC_STUB IUpdateInstaller_get_ParentHwnd_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_put_ParentHwnd_Proxy(IUpdateInstaller * This, HWND value);
void __RPC_STUB IUpdateInstaller_put_ParentHwnd_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_put_ParentWindow_Proxy(IUpdateInstaller * This, IUnknown * value);
void __RPC_STUB IUpdateInstaller_put_ParentWindow_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_get_ParentWindow_Proxy(IUpdateInstaller * This, IUnknown ** retval);
void __RPC_STUB IUpdateInstaller_get_ParentWindow_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_get_Updates_Proxy(IUpdateInstaller * This, IUpdateCollection ** retval);
void __RPC_STUB IUpdateInstaller_get_Updates_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_put_Updates_Proxy(IUpdateInstaller * This, IUpdateCollection * value);
void __RPC_STUB IUpdateInstaller_put_Updates_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_BeginInstall_Proxy(IUpdateInstaller * This, IUnknown * onProgressChanged, IUnknown * onCompleted, VARIANT state, IInstallationJob ** retval);
void __RPC_STUB IUpdateInstaller_BeginInstall_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_BeginUninstall_Proxy(IUpdateInstaller * This, IUnknown * onProgressChanged, IUnknown * onCompleted, VARIANT state, IInstallationJob ** retval);
void __RPC_STUB IUpdateInstaller_BeginUninstall_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_EndInstall_Proxy(IUpdateInstaller * This, IInstallationJob * value, IInstallationResult ** retval);
void __RPC_STUB IUpdateInstaller_EndInstall_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_EndUninstall_Proxy(IUpdateInstaller * This, IInstallationJob * value, IInstallationResult ** retval);
void __RPC_STUB IUpdateInstaller_EndUninstall_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_Install_Proxy(IUpdateInstaller * This, IInstallationResult ** retval);
void __RPC_STUB IUpdateInstaller_Install_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_RunWizard_Proxy(IUpdateInstaller * This, BSTR dialogTitle, IInstallationResult ** retval);
void __RPC_STUB IUpdateInstaller_RunWizard_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_get_IsBusy_Proxy(IUpdateInstaller * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateInstaller_get_IsBusy_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_Uninstall_Proxy(IUpdateInstaller * This, IInstallationResult ** retval);
void __RPC_STUB IUpdateInstaller_Uninstall_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_get_AllowSourcePrompts_Proxy(IUpdateInstaller * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateInstaller_get_AllowSourcePrompts_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_put_AllowSourcePrompts_Proxy(IUpdateInstaller * This, VARIANT_BOOL value);
void __RPC_STUB IUpdateInstaller_put_AllowSourcePrompts_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller_get_RebootRequiredBeforeInstallation_Proxy(IUpdateInstaller * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateInstaller_get_RebootRequiredBeforeInstallation_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateInstaller2_INTERFACE_DEFINED__
#define __IUpdateInstaller2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateInstaller2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("f7c72712-c7f3-4aad-bc6a-6a0e9696a50e")IUpdateInstaller2:public IUpdateInstaller
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_ForceQuiet(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ForceQuiet(VARIANT_BOOL value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IgnoreDigests(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_IgnoreDigests(VARIANT_BOOL value) = 0;
};
#else
typedef struct IUpdateInstaller2Vtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateInstaller2 * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateInstaller2 * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateInstaller2 * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateInstaller2 * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateInstaller2 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateInstaller2 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateInstaller2 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_ClientApplicationID)(IUpdateInstaller2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *put_ClientApplicationID)(IUpdateInstaller2 * This, BSTR value);
	HRESULT(STDMETHODCALLTYPE *get_IsForced)(IUpdateInstaller2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_IsForced)(IUpdateInstaller2 * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *get_ParentHwnd)(IUpdateInstaller2 * This, HWND * retval);
	HRESULT(STDMETHODCALLTYPE *put_ParentHwnd)(IUpdateInstaller2 * This, HWND value);
	HRESULT(STDMETHODCALLTYPE *put_ParentWindow)(IUpdateInstaller2 * This, IUnknown * value);
	HRESULT(STDMETHODCALLTYPE *get_ParentWindow)(IUpdateInstaller2 * This, IUnknown ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Updates)(IUpdateInstaller2 * This, IUpdateCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *put_Updates)(IUpdateInstaller2 * This, IUpdateCollection * value);
	HRESULT(STDMETHODCALLTYPE *BeginInstall)(IUpdateInstaller2 * This, IUnknown * onProgressChanged, IUnknown * onCompleted, VARIANT state, IInstallationJob ** retval);
	HRESULT(STDMETHODCALLTYPE *BeginUninstall)(IUpdateInstaller2 * This, IUnknown * onProgressChanged, IUnknown * onCompleted, VARIANT state, IInstallationJob ** retval);
	HRESULT(STDMETHODCALLTYPE *EndInstall)(IUpdateInstaller2 * This, IInstallationJob * value, IInstallationResult ** retval);
	HRESULT(STDMETHODCALLTYPE *EndUninstall)(IUpdateInstaller2 * This, IInstallationJob * value, IInstallationResult ** retval);
	HRESULT(STDMETHODCALLTYPE *Install)(IUpdateInstaller2 * This, IInstallationResult ** retval);
	HRESULT(STDMETHODCALLTYPE *RunWizard)(IUpdateInstaller2 * This, BSTR dialogTitle, IInstallationResult ** retval);
	HRESULT(STDMETHODCALLTYPE *get_IsBusy)(IUpdateInstaller2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *Uninstall)(IUpdateInstaller2 * This, IInstallationResult ** retval);
	HRESULT(STDMETHODCALLTYPE *get_AllowSourcePrompts)(IUpdateInstaller2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_AllowSourcePrompts)(IUpdateInstaller2 * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *get_RebootRequiredBeforeInstallation)(IUpdateInstaller2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_ForceQuiet)(IUpdateInstaller2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_ForceQuiet)(IUpdateInstaller2 * This, VARIANT_BOOL value);
	HRESULT(STDMETHODCALLTYPE *get_IgnoreDigests)(IUpdateInstaller2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *put_IgnoreDigests)(IUpdateInstaller2 * This, VARIANT_BOOL value);
	END_INTERFACE
} IUpdateInstaller2Vtbl;

interface IUpdateInstaller2 {
    CONST_VTBL struct IUpdateInstaller2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateInstaller2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateInstaller2_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateInstaller2_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateInstaller2_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateInstaller2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateInstaller2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateInstaller2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateInstaller2_get_ClientApplicationID(This,retval)  (This)->lpVtbl -> get_ClientApplicationID(This,retval)
#define IUpdateInstaller2_put_ClientApplicationID(This,value)  (This)->lpVtbl -> put_ClientApplicationID(This,value)
#define IUpdateInstaller2_get_IsForced(This,retval)  (This)->lpVtbl -> get_IsForced(This,retval)
#define IUpdateInstaller2_put_IsForced(This,value)  (This)->lpVtbl -> put_IsForced(This,value)
#define IUpdateInstaller2_get_ParentHwnd(This,retval)  (This)->lpVtbl -> get_ParentHwnd(This,retval)
#define IUpdateInstaller2_put_ParentHwnd(This,value)  (This)->lpVtbl -> put_ParentHwnd(This,value)
#define IUpdateInstaller2_put_ParentWindow(This,value)  (This)->lpVtbl -> put_ParentWindow(This,value)
#define IUpdateInstaller2_get_ParentWindow(This,retval)  (This)->lpVtbl -> get_ParentWindow(This,retval)
#define IUpdateInstaller2_get_Updates(This,retval)  (This)->lpVtbl -> get_Updates(This,retval)
#define IUpdateInstaller2_put_Updates(This,value)  (This)->lpVtbl -> put_Updates(This,value)
#define IUpdateInstaller2_BeginInstall(This,onProgressChanged,onCompleted,state,retval)  (This)->lpVtbl -> BeginInstall(This,onProgressChanged,onCompleted,state,retval)
#define IUpdateInstaller2_BeginUninstall(This,onProgressChanged,onCompleted,state,retval)  (This)->lpVtbl -> BeginUninstall(This,onProgressChanged,onCompleted,state,retval)
#define IUpdateInstaller2_EndInstall(This,value,retval)  (This)->lpVtbl -> EndInstall(This,value,retval)
#define IUpdateInstaller2_EndUninstall(This,value,retval)  (This)->lpVtbl -> EndUninstall(This,value,retval)
#define IUpdateInstaller2_Install(This,retval)  (This)->lpVtbl -> Install(This,retval)
#define IUpdateInstaller2_RunWizard(This,dialogTitle,retval)  (This)->lpVtbl -> RunWizard(This,dialogTitle,retval)
#define IUpdateInstaller2_get_IsBusy(This,retval)  (This)->lpVtbl -> get_IsBusy(This,retval)
#define IUpdateInstaller2_Uninstall(This,retval)  (This)->lpVtbl -> Uninstall(This,retval)
#define IUpdateInstaller2_get_AllowSourcePrompts(This,retval)  (This)->lpVtbl -> get_AllowSourcePrompts(This,retval)
#define IUpdateInstaller2_put_AllowSourcePrompts(This,value)  (This)->lpVtbl -> put_AllowSourcePrompts(This,value)
#define IUpdateInstaller2_get_RebootRequiredBeforeInstallation(This,retval)  (This)->lpVtbl -> get_RebootRequiredBeforeInstallation(This,retval)
#define IUpdateInstaller2_get_ForceQuiet(This,retval)  (This)->lpVtbl -> get_ForceQuiet(This,retval)
#define IUpdateInstaller2_put_ForceQuiet(This,value)  (This)->lpVtbl -> put_ForceQuiet(This,value)
#define IUpdateInstaller2_get_IgnoreDigests(This,retval)  (This)->lpVtbl -> get_IgnoreDigests(This,retval)
#define IUpdateInstaller2_put_IgnoreDigests(This,value)  (This)->lpVtbl -> put_IgnoreDigests(This,value)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateInstaller2_get_ForceQuiet_Proxy(IUpdateInstaller2 * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateInstaller2_get_ForceQuiet_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller2_put_ForceQuiet_Proxy(IUpdateInstaller2 * This, VARIANT_BOOL value);
void __RPC_STUB IUpdateInstaller2_put_ForceQuiet_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller2_get_IgnoreDigests_Proxy(IUpdateInstaller2 * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateInstaller2_get_IgnoreDigests_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateInstaller2_put_IgnoreDigests_Proxy(IUpdateInstaller2 * This, VARIANT_BOOL value);
void __RPC_STUB IUpdateInstaller2_put_IgnoreDigests_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateSession_INTERFACE_DEFINED__
#define __IUpdateSession_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateSession;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("816858a4-260d-4260-933a-2585f1abc76b")IUpdateSession:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_ClientApplicationID(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ClientApplicationID(BSTR value) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ReadOnly(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_WebProxy(IWebProxy ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_WebProxy(IWebProxy * value) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateUpdateSearcher(IUpdateSearcher ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateUpdateDownloader(IUpdateDownloader ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateUpdateInstaller(IUpdateInstaller ** retval) = 0;
};
#else
typedef struct IUpdateSessionVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateSession * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateSession * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateSession * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateSession * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateSession * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateSession * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateSession * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_ClientApplicationID)(IUpdateSession * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *put_ClientApplicationID)(IUpdateSession * This, BSTR value);
	HRESULT(STDMETHODCALLTYPE *get_ReadOnly)(IUpdateSession * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_WebProxy)(IUpdateSession * This, IWebProxy ** retval);
	HRESULT(STDMETHODCALLTYPE *put_WebProxy)(IUpdateSession * This, IWebProxy * value);
	HRESULT(STDMETHODCALLTYPE *CreateUpdateSearcher)(IUpdateSession * This, IUpdateSearcher ** retval);
	HRESULT(STDMETHODCALLTYPE *CreateUpdateDownloader)(IUpdateSession * This, IUpdateDownloader ** retval);
	HRESULT(STDMETHODCALLTYPE *CreateUpdateInstaller)(IUpdateSession * This, IUpdateInstaller ** retval);
	END_INTERFACE
} IUpdateSessionVtbl;

interface IUpdateSession {
    CONST_VTBL struct IUpdateSessionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateSession_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateSession_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateSession_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateSession_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateSession_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateSession_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateSession_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateSession_get_ClientApplicationID(This,retval)  (This)->lpVtbl -> get_ClientApplicationID(This,retval)
#define IUpdateSession_put_ClientApplicationID(This,value)  (This)->lpVtbl -> put_ClientApplicationID(This,value)
#define IUpdateSession_get_ReadOnly(This,retval)  (This)->lpVtbl -> get_ReadOnly(This,retval)
#define IUpdateSession_get_WebProxy(This,retval)  (This)->lpVtbl -> get_WebProxy(This,retval)
#define IUpdateSession_put_WebProxy(This,value)  (This)->lpVtbl -> put_WebProxy(This,value)
#define IUpdateSession_CreateUpdateSearcher(This,retval)  (This)->lpVtbl -> CreateUpdateSearcher(This,retval)
#define IUpdateSession_CreateUpdateDownloader(This,retval)  (This)->lpVtbl -> CreateUpdateDownloader(This,retval)
#define IUpdateSession_CreateUpdateInstaller(This,retval)  (This)->lpVtbl -> CreateUpdateInstaller(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateSession_get_ClientApplicationID_Proxy(IUpdateSession * This, BSTR * retval);
void __RPC_STUB IUpdateSession_get_ClientApplicationID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSession_put_ClientApplicationID_Proxy(IUpdateSession * This, BSTR value);
void __RPC_STUB IUpdateSession_put_ClientApplicationID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSession_get_ReadOnly_Proxy(IUpdateSession * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateSession_get_ReadOnly_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSession_get_WebProxy_Proxy(IUpdateSession * This, IWebProxy ** retval);
void __RPC_STUB IUpdateSession_get_WebProxy_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSession_put_WebProxy_Proxy(IUpdateSession * This, IWebProxy * value);
void __RPC_STUB IUpdateSession_put_WebProxy_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSession_CreateUpdateSearcher_Proxy(IUpdateSession * This, IUpdateSearcher ** retval);
void __RPC_STUB IUpdateSession_CreateUpdateSearcher_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSession_CreateUpdateDownloader_Proxy(IUpdateSession * This, IUpdateDownloader ** retval);
void __RPC_STUB IUpdateSession_CreateUpdateDownloader_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSession_CreateUpdateInstaller_Proxy(IUpdateSession * This, IUpdateInstaller ** retval);
void __RPC_STUB IUpdateSession_CreateUpdateInstaller_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateSession2_INTERFACE_DEFINED__
#define __IUpdateSession2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateSession2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("91caf7b0-eb23-49ed-9937-c52d817f46f7")IUpdateSession2:public IUpdateSession
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_UserLocale(LCID * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_UserLocale(LCID lcid) = 0;
};
#else
typedef struct IUpdateSession2Vtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateSession2 * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateSession2 * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateSession2 * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateSession2 * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateSession2 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateSession2 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateSession2 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_ClientApplicationID)(IUpdateSession2 * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *put_ClientApplicationID)(IUpdateSession2 * This, BSTR value);
	HRESULT(STDMETHODCALLTYPE *get_ReadOnly)(IUpdateSession2 * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_WebProxy)(IUpdateSession2 * This, IWebProxy ** retval);
	HRESULT(STDMETHODCALLTYPE *put_WebProxy)(IUpdateSession2 * This, IWebProxy * value);
	HRESULT(STDMETHODCALLTYPE *CreateUpdateSearcher)(IUpdateSession2 * This, IUpdateSearcher ** retval);
	HRESULT(STDMETHODCALLTYPE *CreateUpdateDownloader)(IUpdateSession2 * This, IUpdateDownloader ** retval);
	HRESULT(STDMETHODCALLTYPE *CreateUpdateInstaller)(IUpdateSession2 * This, IUpdateInstaller ** retval);
	HRESULT(STDMETHODCALLTYPE *get_UserLocale)(IUpdateSession2 * This, LCID * retval);
	HRESULT(STDMETHODCALLTYPE *put_UserLocale)(IUpdateSession2 * This, LCID lcid);
	END_INTERFACE
} IUpdateSession2Vtbl;

interface IUpdateSession2 {
    CONST_VTBL struct IUpdateSession2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateSession2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateSession2_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateSession2_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateSession2_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateSession2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateSession2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateSession2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateSession2_get_ClientApplicationID(This,retval)  (This)->lpVtbl -> get_ClientApplicationID(This,retval)
#define IUpdateSession2_put_ClientApplicationID(This,value)  (This)->lpVtbl -> put_ClientApplicationID(This,value)
#define IUpdateSession2_get_ReadOnly(This,retval)  (This)->lpVtbl -> get_ReadOnly(This,retval)
#define IUpdateSession2_get_WebProxy(This,retval)  (This)->lpVtbl -> get_WebProxy(This,retval)
#define IUpdateSession2_put_WebProxy(This,value)  (This)->lpVtbl -> put_WebProxy(This,value)
#define IUpdateSession2_CreateUpdateSearcher(This,retval)  (This)->lpVtbl -> CreateUpdateSearcher(This,retval)
#define IUpdateSession2_CreateUpdateDownloader(This,retval)  (This)->lpVtbl -> CreateUpdateDownloader(This,retval)
#define IUpdateSession2_CreateUpdateInstaller(This,retval)  (This)->lpVtbl -> CreateUpdateInstaller(This,retval)
#define IUpdateSession2_get_UserLocale(This,retval)  (This)->lpVtbl -> get_UserLocale(This,retval)
#define IUpdateSession2_put_UserLocale(This,lcid)  (This)->lpVtbl -> put_UserLocale(This,lcid)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateSession2_get_UserLocale_Proxy(IUpdateSession2 * This, LCID * retval);
void __RPC_STUB IUpdateSession2_get_UserLocale_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateSession2_put_UserLocale_Proxy(IUpdateSession2 * This, LCID lcid);
void __RPC_STUB IUpdateSession2_put_UserLocale_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateService_INTERFACE_DEFINED__
#define __IUpdateService_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateService;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("8c3f0d06-1eb9-4f01-8c2e-1adc74627829")IUpdateService:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ContentValidationCert(VARIANT * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ExpirationDate(DATE * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsManaged(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsRegisteredWithAU(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IssueDate(DATE * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_OffersWindowsUpdates(VARIANT_BOOL * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_RedirectUrls(IStringCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ServiceID(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UIPluginClsid(BSTR * retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsScanPackageService(VARIANT_BOOL * retval) = 0;
};
#else
typedef struct IUpdateServiceVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateService * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateService * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateService * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateService * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateService * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateService * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateService * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Name)(IUpdateService * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_ContentValidationCert)(IUpdateService * This, VARIANT * retval);
	HRESULT(STDMETHODCALLTYPE *get_ExpirationDate)(IUpdateService * This, DATE * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsManaged)(IUpdateService * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsRegisteredWithAU)(IUpdateService * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_IssueDate)(IUpdateService * This, DATE * retval);
	HRESULT(STDMETHODCALLTYPE *get_OffersWindowsUpdates)(IUpdateService * This, VARIANT_BOOL * retval);
	HRESULT(STDMETHODCALLTYPE *get_RedirectUrls)(IUpdateService * This, IStringCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *get_ServiceID)(IUpdateService * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_UIPluginClsid)(IUpdateService * This, BSTR * retval);
	HRESULT(STDMETHODCALLTYPE *get_IsScanPackageService)(IUpdateService * This, VARIANT_BOOL * retval);
	END_INTERFACE
} IUpdateServiceVtbl;

interface IUpdateService {
    CONST_VTBL struct IUpdateServiceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateService_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateService_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateService_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateService_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateService_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateService_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateService_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateService_get_Name(This,retval)  (This)->lpVtbl -> get_Name(This,retval)
#define IUpdateService_get_ContentValidationCert(This,retval)  (This)->lpVtbl -> get_ContentValidationCert(This,retval)
#define IUpdateService_get_ExpirationDate(This,retval)  (This)->lpVtbl -> get_ExpirationDate(This,retval)
#define IUpdateService_get_IsManaged(This,retval)  (This)->lpVtbl -> get_IsManaged(This,retval)
#define IUpdateService_get_IsRegisteredWithAU(This,retval)  (This)->lpVtbl -> get_IsRegisteredWithAU(This,retval)
#define IUpdateService_get_IssueDate(This,retval)  (This)->lpVtbl -> get_IssueDate(This,retval)
#define IUpdateService_get_OffersWindowsUpdates(This,retval)  (This)->lpVtbl -> get_OffersWindowsUpdates(This,retval)
#define IUpdateService_get_RedirectUrls(This,retval)  (This)->lpVtbl -> get_RedirectUrls(This,retval)
#define IUpdateService_get_ServiceID(This,retval)  (This)->lpVtbl -> get_ServiceID(This,retval)
#define IUpdateService_get_UIPluginClsid(This,retval)  (This)->lpVtbl -> get_UIPluginClsid(This,retval)
#define IUpdateService_get_IsScanPackageService(This,retval)  (This)->lpVtbl -> get_IsScanPackageService(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateService_get_Name_Proxy(IUpdateService * This, BSTR * retval);
void __RPC_STUB IUpdateService_get_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateService_get_ContentValidationCert_Proxy(IUpdateService * This, VARIANT * retval);
void __RPC_STUB IUpdateService_get_ContentValidationCert_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateService_get_ExpirationDate_Proxy(IUpdateService * This, DATE * retval);
void __RPC_STUB IUpdateService_get_ExpirationDate_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateService_get_IsManaged_Proxy(IUpdateService * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateService_get_IsManaged_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateService_get_IsRegisteredWithAU_Proxy(IUpdateService * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateService_get_IsRegisteredWithAU_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateService_get_IssueDate_Proxy(IUpdateService * This, DATE * retval);
void __RPC_STUB IUpdateService_get_IssueDate_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateService_get_OffersWindowsUpdates_Proxy(IUpdateService * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateService_get_OffersWindowsUpdates_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateService_get_RedirectUrls_Proxy(IUpdateService * This, IStringCollection ** retval);
void __RPC_STUB IUpdateService_get_RedirectUrls_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateService_get_ServiceID_Proxy(IUpdateService * This, BSTR * retval);
void __RPC_STUB IUpdateService_get_ServiceID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateService_get_UIPluginClsid_Proxy(IUpdateService * This, BSTR * retval);
void __RPC_STUB IUpdateService_get_UIPluginClsid_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateService_get_IsScanPackageService_Proxy(IUpdateService * This, VARIANT_BOOL * retval);
void __RPC_STUB IUpdateService_get_IsScanPackageService_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateServiceCollection_INTERFACE_DEFINED__
#define __IUpdateServiceCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateServiceCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("9b0353aa-0e52-44ff-b8b0-1f7fa0437f88")IUpdateServiceCollection:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Item(LONG index, IUpdateService ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * retval) = 0;
};
#else
typedef struct IUpdateServiceCollectionVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateServiceCollection * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateServiceCollection * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateServiceCollection * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateServiceCollection * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateServiceCollection * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateServiceCollection * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateServiceCollection * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Item)(IUpdateServiceCollection * This, LONG index, IUpdateService ** retval);
	HRESULT(STDMETHODCALLTYPE *get__NewEnum)(IUpdateServiceCollection * This, IUnknown ** retval);
	HRESULT(STDMETHODCALLTYPE *get_Count)(IUpdateServiceCollection * This, LONG * retval);
	END_INTERFACE
} IUpdateServiceCollectionVtbl;

interface IUpdateServiceCollection {
    CONST_VTBL struct IUpdateServiceCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateServiceCollection_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateServiceCollection_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateServiceCollection_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateServiceCollection_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateServiceCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateServiceCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateServiceCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateServiceCollection_get_Item(This,index,retval)  (This)->lpVtbl -> get_Item(This,index,retval)
#define IUpdateServiceCollection_get__NewEnum(This,retval)  (This)->lpVtbl -> get__NewEnum(This,retval)
#define IUpdateServiceCollection_get_Count(This,retval)  (This)->lpVtbl -> get_Count(This,retval)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateServiceCollection_get_Item_Proxy(IUpdateServiceCollection * This, LONG index, IUpdateService ** retval);
void __RPC_STUB IUpdateServiceCollection_get_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateServiceCollection_get__NewEnum_Proxy(IUpdateServiceCollection * This, IUnknown ** retval);
void __RPC_STUB IUpdateServiceCollection_get__NewEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateServiceCollection_get_Count_Proxy(IUpdateServiceCollection * This, LONG * retval);
void __RPC_STUB IUpdateServiceCollection_get_Count_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IUpdateServiceManager_INTERFACE_DEFINED__
#define __IUpdateServiceManager_INTERFACE_DEFINED__

EXTERN_C const IID IID_IUpdateServiceManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("c165fa56-6f87-43aa-9d92-6cebcc7bcfe4")IUpdateServiceManager:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Services(IUpdateServiceCollection ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddService(BSTR serviceID, BSTR authorizationCabPath, IUpdateService ** retval) = 0;
	virtual HRESULT STDMETHODCALLTYPE RegisterServiceWithAU(BSTR serviceID) = 0;
	virtual HRESULT STDMETHODCALLTYPE RemoveService(BSTR serviceID) = 0;
	virtual HRESULT STDMETHODCALLTYPE UnregisterServiceWithAU(BSTR serviceID) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddScanPackageService(BSTR bstrServiceName, BSTR bstrScanFileLocation, IUpdateService ** ppService) = 0;
};
#else
typedef struct IUpdateServiceManagerVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE *QueryInterface)(IUpdateServiceManager * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IUpdateServiceManager * This);
	ULONG(STDMETHODCALLTYPE *Release)(IUpdateServiceManager * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IUpdateServiceManager * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IUpdateServiceManager * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IUpdateServiceManager * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IUpdateServiceManager * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Services)(IUpdateServiceManager * This, IUpdateServiceCollection ** retval);
	HRESULT(STDMETHODCALLTYPE *AddService)(IUpdateServiceManager * This, BSTR serviceID, BSTR authorizationCabPath, IUpdateService ** retval);
	HRESULT(STDMETHODCALLTYPE *RegisterServiceWithAU)(IUpdateServiceManager * This, BSTR serviceID);
	HRESULT(STDMETHODCALLTYPE *RemoveService)(IUpdateServiceManager * This, BSTR serviceID);
	HRESULT(STDMETHODCALLTYPE *UnregisterServiceWithAU)(IUpdateServiceManager * This, BSTR serviceID);
	HRESULT(STDMETHODCALLTYPE *AddScanPackageService)(IUpdateServiceManager * This, BSTR bstrServiceName, BSTR bstrScanFileLocation, IUpdateService ** ppService);
	END_INTERFACE
} IUpdateServiceManagerVtbl;

interface IUpdateServiceManager {
    CONST_VTBL struct IUpdateServiceManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IUpdateServiceManager_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)
#define IUpdateServiceManager_AddRef(This)  (This)->lpVtbl -> AddRef(This)
#define IUpdateServiceManager_Release(This)  (This)->lpVtbl -> Release(This)
#define IUpdateServiceManager_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)
#define IUpdateServiceManager_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IUpdateServiceManager_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IUpdateServiceManager_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IUpdateServiceManager_get_Services(This,retval)  (This)->lpVtbl -> get_Services(This,retval)
#define IUpdateServiceManager_AddService(This,serviceID,authorizationCabPath,retval)  (This)->lpVtbl -> AddService(This,serviceID,authorizationCabPath,retval)
#define IUpdateServiceManager_RegisterServiceWithAU(This,serviceID)  (This)->lpVtbl -> RegisterServiceWithAU(This,serviceID)
#define IUpdateServiceManager_RemoveService(This,serviceID)  (This)->lpVtbl -> RemoveService(This,serviceID)
#define IUpdateServiceManager_UnregisterServiceWithAU(This,serviceID)  (This)->lpVtbl -> UnregisterServiceWithAU(This,serviceID)
#define IUpdateServiceManager_AddScanPackageService(This,bstrServiceName,bstrScanFileLocation,ppService)  (This)->lpVtbl -> AddScanPackageService(This,bstrServiceName,bstrScanFileLocation,ppService)
#endif

#endif

HRESULT STDMETHODCALLTYPE IUpdateServiceManager_get_Services_Proxy(IUpdateServiceManager * This, IUpdateServiceCollection ** retval);
void __RPC_STUB IUpdateServiceManager_get_Services_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateServiceManager_AddService_Proxy(IUpdateServiceManager * This, BSTR serviceID, BSTR authorizationCabPath, IUpdateService ** retval);
void __RPC_STUB IUpdateServiceManager_AddService_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateServiceManager_RegisterServiceWithAU_Proxy(IUpdateServiceManager * This, BSTR serviceID);
void __RPC_STUB IUpdateServiceManager_RegisterServiceWithAU_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateServiceManager_RemoveService_Proxy(IUpdateServiceManager * This, BSTR serviceID);
void __RPC_STUB IUpdateServiceManager_RemoveService_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateServiceManager_UnregisterServiceWithAU_Proxy(IUpdateServiceManager * This, BSTR serviceID);
void __RPC_STUB IUpdateServiceManager_UnregisterServiceWithAU_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IUpdateServiceManager_AddScanPackageService_Proxy(IUpdateServiceManager * This, BSTR bstrServiceName, BSTR bstrScanFileLocation, IUpdateService ** ppService);
void __RPC_STUB IUpdateServiceManager_AddScanPackageService_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __WUApiLib_LIBRARY_DEFINED__
#define __WUApiLib_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_WUApiLib;
EXTERN_C const CLSID CLSID_StringCollection;

#ifdef __cplusplus
class DECLSPEC_UUID("72C97D74-7C3B-40AE-B77D-ABDB22EBA6FB") StringCollection;
#endif

EXTERN_C const CLSID CLSID_UpdateSearcher;

#ifdef __cplusplus
class DECLSPEC_UUID("B699E5E8-67FF-4177-88B0-3684A3388BFB") UpdateSearcher;
#endif

EXTERN_C const CLSID CLSID_WebProxy;

#ifdef __cplusplus
class DECLSPEC_UUID("650503cf-9108-4ddc-a2ce-6c2341e1c582") WebProxy;
#endif

EXTERN_C const CLSID CLSID_SystemInformation;

#ifdef __cplusplus
class DECLSPEC_UUID("C01B9BA0-BEA7-41BA-B604-D0A36F469133") SystemInformation;
#endif

EXTERN_C const CLSID CLSID_AutomaticUpdates;

#ifdef __cplusplus
class DECLSPEC_UUID("BFE18E9C-6D87-4450-B37C-E02F0B373803") AutomaticUpdates;
#endif

EXTERN_C const CLSID CLSID_UpdateCollection;

#ifdef __cplusplus
class DECLSPEC_UUID("13639463-00DB-4646-803D-528026140D88") UpdateCollection;
#endif

EXTERN_C const CLSID CLSID_UpdateDownloader;

#ifdef __cplusplus
class DECLSPEC_UUID("5BAF654A-5A07-4264-A255-9FF54C7151E7") UpdateDownloader;
#endif

EXTERN_C const CLSID CLSID_UpdateInstaller;

#ifdef __cplusplus
class DECLSPEC_UUID("D2E0FE7F-D23E-48E1-93C0-6FA8CC346474") UpdateInstaller;
#endif

EXTERN_C const CLSID CLSID_UpdateSession;

#ifdef __cplusplus
class DECLSPEC_UUID("4CB43D7F-7EEE-4906-8698-60DA1C38F2FE") UpdateSession;
#endif

EXTERN_C const CLSID CLSID_UpdateServiceManager;

#ifdef __cplusplus
class DECLSPEC_UUID("91353063-774C-4F84-B05B-498FEC7FBE25") UpdateServiceManager;
#endif

#endif

unsigned long __RPC_USER BSTR_UserSize(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree(unsigned long *, BSTR *);

unsigned long __RPC_USER HWND_UserSize(unsigned long *, unsigned long, HWND *);
unsigned char *__RPC_USER HWND_UserMarshal(unsigned long *, unsigned char *, HWND *);
unsigned char *__RPC_USER HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND *);
void __RPC_USER HWND_UserFree(unsigned long *, HWND *);

unsigned long __RPC_USER VARIANT_UserSize(unsigned long *, unsigned long, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserMarshal(unsigned long *, unsigned char *, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT *);
void __RPC_USER VARIANT_UserFree(unsigned long *, VARIANT *);

#ifdef __cplusplus
}
#endif

#endif
