#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__  500
#endif

#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__  100
#endif

/* Active Directory Service Interface definitions */

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif

#ifndef _IADS_H
#define _IADS_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IADs_FWD_DEFINED__
#define __IADs_FWD_DEFINED__
typedef interface IADs IADs;
#endif

#ifndef __IADsContainer_FWD_DEFINED__
#define __IADsContainer_FWD_DEFINED__
typedef interface IADsContainer IADsContainer;
#endif

#ifndef __IADsCollection_FWD_DEFINED__
#define __IADsCollection_FWD_DEFINED__
typedef interface IADsCollection IADsCollection;
#endif

#ifndef __IADsMembers_FWD_DEFINED__
#define __IADsMembers_FWD_DEFINED__
typedef interface IADsMembers IADsMembers;
#endif

#ifndef __IADsPropertyList_FWD_DEFINED__
#define __IADsPropertyList_FWD_DEFINED__
typedef interface IADsPropertyList IADsPropertyList;
#endif

#ifndef __IADsPropertyEntry_FWD_DEFINED__
#define __IADsPropertyEntry_FWD_DEFINED__
typedef interface IADsPropertyEntry IADsPropertyEntry;
#endif

#ifndef __PropertyEntry_FWD_DEFINED__
#define __PropertyEntry_FWD_DEFINED__
#ifdef __cplusplus
typedef class PropertyEntry PropertyEntry;
#else
typedef struct PropertyEntry PropertyEntry;
#endif
#endif

#ifndef __IADsPropertyValue_FWD_DEFINED__
#define __IADsPropertyValue_FWD_DEFINED__
typedef interface IADsPropertyValue IADsPropertyValue;
#endif

#ifndef __IADsPropertyValue2_FWD_DEFINED__
#define __IADsPropertyValue2_FWD_DEFINED__
typedef interface IADsPropertyValue2 IADsPropertyValue2;
#endif

#ifndef __PropertyValue_FWD_DEFINED__
#define __PropertyValue_FWD_DEFINED__
#ifdef __cplusplus
typedef class PropertyValue PropertyValue;
#else
typedef struct PropertyValue PropertyValue;
#endif
#endif

#ifndef __IPrivateDispatch_FWD_DEFINED__
#define __IPrivateDispatch_FWD_DEFINED__
typedef interface IPrivateDispatch IPrivateDispatch;
#endif

#ifndef __IPrivateUnknown_FWD_DEFINED__
#define __IPrivateUnknown_FWD_DEFINED__
typedef interface IPrivateUnknown IPrivateUnknown;
#endif

#ifndef __IADsExtension_FWD_DEFINED__
#define __IADsExtension_FWD_DEFINED__
typedef interface IADsExtension IADsExtension;
#endif

#ifndef __IADsDeleteOps_FWD_DEFINED__
#define __IADsDeleteOps_FWD_DEFINED__
typedef interface IADsDeleteOps IADsDeleteOps;
#endif

#ifndef __IADsNamespaces_FWD_DEFINED__
#define __IADsNamespaces_FWD_DEFINED__
typedef interface IADsNamespaces IADsNamespaces;
#endif

#ifndef __IADsClass_FWD_DEFINED__
#define __IADsClass_FWD_DEFINED__
typedef interface IADsClass IADsClass;
#endif

#ifndef __IADsProperty_FWD_DEFINED__
#define __IADsProperty_FWD_DEFINED__
typedef interface IADsProperty IADsProperty;
#endif

#ifndef __IADsSyntax_FWD_DEFINED__
#define __IADsSyntax_FWD_DEFINED__
typedef interface IADsSyntax IADsSyntax;
#endif

#ifndef __IADsLocality_FWD_DEFINED__
#define __IADsLocality_FWD_DEFINED__
typedef interface IADsLocality IADsLocality;
#endif

#ifndef __IADsO_FWD_DEFINED__
#define __IADsO_FWD_DEFINED__
typedef interface IADsO IADsO;
#endif

#ifndef __IADsOU_FWD_DEFINED__
#define __IADsOU_FWD_DEFINED__
typedef interface IADsOU IADsOU;
#endif

#ifndef __IADsDomain_FWD_DEFINED__
#define __IADsDomain_FWD_DEFINED__
typedef interface IADsDomain IADsDomain;
#endif

#ifndef __IADsComputer_FWD_DEFINED__
#define __IADsComputer_FWD_DEFINED__
typedef interface IADsComputer IADsComputer;
#endif

#ifndef __IADsComputerOperations_FWD_DEFINED__
#define __IADsComputerOperations_FWD_DEFINED__
typedef interface IADsComputerOperations IADsComputerOperations;
#endif

#ifndef __IADsGroup_FWD_DEFINED__
#define __IADsGroup_FWD_DEFINED__
typedef interface IADsGroup IADsGroup;
#endif

#ifndef __IADsUser_FWD_DEFINED__
#define __IADsUser_FWD_DEFINED__
typedef interface IADsUser IADsUser;
#endif

#ifndef __IADsPrintQueue_FWD_DEFINED__
#define __IADsPrintQueue_FWD_DEFINED__
typedef interface IADsPrintQueue IADsPrintQueue;
#endif

#ifndef __IADsPrintQueueOperations_FWD_DEFINED__
#define __IADsPrintQueueOperations_FWD_DEFINED__
typedef interface IADsPrintQueueOperations IADsPrintQueueOperations;
#endif

#ifndef __IADsPrintJob_FWD_DEFINED__
#define __IADsPrintJob_FWD_DEFINED__
typedef interface IADsPrintJob IADsPrintJob;
#endif

#ifndef __IADsPrintJobOperations_FWD_DEFINED__
#define __IADsPrintJobOperations_FWD_DEFINED__
typedef interface IADsPrintJobOperations IADsPrintJobOperations;
#endif

#ifndef __IADsService_FWD_DEFINED__
#define __IADsService_FWD_DEFINED__
typedef interface IADsService IADsService;
#endif

#ifndef __IADsServiceOperations_FWD_DEFINED__
#define __IADsServiceOperations_FWD_DEFINED__
typedef interface IADsServiceOperations IADsServiceOperations;
#endif

#ifndef __IADsFileService_FWD_DEFINED__
#define __IADsFileService_FWD_DEFINED__
typedef interface IADsFileService IADsFileService;
#endif

#ifndef __IADsFileServiceOperations_FWD_DEFINED__
#define __IADsFileServiceOperations_FWD_DEFINED__
typedef interface IADsFileServiceOperations IADsFileServiceOperations;
#endif

#ifndef __IADsFileShare_FWD_DEFINED__
#define __IADsFileShare_FWD_DEFINED__
typedef interface IADsFileShare IADsFileShare;
#endif

#ifndef __IADsSession_FWD_DEFINED__
#define __IADsSession_FWD_DEFINED__
typedef interface IADsSession IADsSession;
#endif

#ifndef __IADsResource_FWD_DEFINED__
#define __IADsResource_FWD_DEFINED__
typedef interface IADsResource IADsResource;
#endif

#ifndef __IADsOpenDSObject_FWD_DEFINED__
#define __IADsOpenDSObject_FWD_DEFINED__
typedef interface IADsOpenDSObject IADsOpenDSObject;
#endif

#ifndef __IDirectoryObject_FWD_DEFINED__
#define __IDirectoryObject_FWD_DEFINED__
typedef interface IDirectoryObject IDirectoryObject;
#endif

#ifndef __IDirectorySearch_FWD_DEFINED__
#define __IDirectorySearch_FWD_DEFINED__
typedef interface IDirectorySearch IDirectorySearch;
#endif

#ifndef __IDirectorySchemaMgmt_FWD_DEFINED__
#define __IDirectorySchemaMgmt_FWD_DEFINED__
typedef interface IDirectorySchemaMgmt IDirectorySchemaMgmt;
#endif

#ifndef __IADsAggregatee_FWD_DEFINED__
#define __IADsAggregatee_FWD_DEFINED__
typedef interface IADsAggregatee IADsAggregatee;
#endif

#ifndef __IADsAggregator_FWD_DEFINED__
#define __IADsAggregator_FWD_DEFINED__
typedef interface IADsAggregator IADsAggregator;
#endif

#ifndef __IADsAccessControlEntry_FWD_DEFINED__
#define __IADsAccessControlEntry_FWD_DEFINED__
typedef interface IADsAccessControlEntry IADsAccessControlEntry;
#endif

#ifndef __AccessControlEntry_FWD_DEFINED__
#define __AccessControlEntry_FWD_DEFINED__
#ifdef __cplusplus
typedef class AccessControlEntry AccessControlEntry;
#else
typedef struct AccessControlEntry AccessControlEntry;
#endif
#endif

#ifndef __IADsAccessControlList_FWD_DEFINED__
#define __IADsAccessControlList_FWD_DEFINED__
typedef interface IADsAccessControlList IADsAccessControlList;
#endif

#ifndef __AccessControlList_FWD_DEFINED__
#define __AccessControlList_FWD_DEFINED__
#ifdef __cplusplus
typedef class AccessControlList AccessControlList;
#else
typedef struct AccessControlList AccessControlList;
#endif
#endif

#ifndef __IADsSecurityDescriptor_FWD_DEFINED__
#define __IADsSecurityDescriptor_FWD_DEFINED__
typedef interface IADsSecurityDescriptor IADsSecurityDescriptor;
#endif

#ifndef __SecurityDescriptor_FWD_DEFINED__
#define __SecurityDescriptor_FWD_DEFINED__
#ifdef __cplusplus
typedef class SecurityDescriptor SecurityDescriptor;
#else
typedef struct SecurityDescriptor SecurityDescriptor;
#endif
#endif

#ifndef __IADsLargeInteger_FWD_DEFINED__
#define __IADsLargeInteger_FWD_DEFINED__
typedef interface IADsLargeInteger IADsLargeInteger;
#endif

#ifndef __LargeInteger_FWD_DEFINED__
#define __LargeInteger_FWD_DEFINED__
#ifdef __cplusplus
typedef class LargeInteger LargeInteger;
#else
typedef struct LargeInteger LargeInteger;
#endif
#endif

#ifndef __IADsNameTranslate_FWD_DEFINED__
#define __IADsNameTranslate_FWD_DEFINED__
typedef interface IADsNameTranslate IADsNameTranslate;
#endif

#ifndef __NameTranslate_FWD_DEFINED__
#define __NameTranslate_FWD_DEFINED__
#ifdef __cplusplus
typedef class NameTranslate NameTranslate;
#else
typedef struct NameTranslate NameTranslate;
#endif
#endif

#ifndef __IADsCaseIgnoreList_FWD_DEFINED__
#define __IADsCaseIgnoreList_FWD_DEFINED__
typedef interface IADsCaseIgnoreList IADsCaseIgnoreList;
#endif

#ifndef __CaseIgnoreList_FWD_DEFINED__
#define __CaseIgnoreList_FWD_DEFINED__
#ifdef __cplusplus
typedef class CaseIgnoreList CaseIgnoreList;
#else
typedef struct CaseIgnoreList CaseIgnoreList;
#endif
#endif

#ifndef __IADsFaxNumber_FWD_DEFINED__
#define __IADsFaxNumber_FWD_DEFINED__
typedef interface IADsFaxNumber IADsFaxNumber;
#endif

#ifndef __FaxNumber_FWD_DEFINED__
#define __FaxNumber_FWD_DEFINED__
#ifdef __cplusplus
typedef class FaxNumber FaxNumber;
#else
typedef struct FaxNumber FaxNumber;
#endif
#endif

#ifndef __IADsNetAddress_FWD_DEFINED__
#define __IADsNetAddress_FWD_DEFINED__
typedef interface IADsNetAddress IADsNetAddress;
#endif

#ifndef __NetAddress_FWD_DEFINED__
#define __NetAddress_FWD_DEFINED__
#ifdef __cplusplus
typedef class NetAddress NetAddress;
#else
typedef struct NetAddress NetAddress;
#endif
#endif

#ifndef __IADsOctetList_FWD_DEFINED__
#define __IADsOctetList_FWD_DEFINED__
typedef interface IADsOctetList IADsOctetList;
#endif

#ifndef __OctetList_FWD_DEFINED__
#define __OctetList_FWD_DEFINED__
#ifdef __cplusplus
typedef class OctetList OctetList;
#else
typedef struct OctetList OctetList;
#endif
#endif

#ifndef __IADsEmail_FWD_DEFINED__
#define __IADsEmail_FWD_DEFINED__
typedef interface IADsEmail IADsEmail;
#endif

#ifndef __Email_FWD_DEFINED__
#define __Email_FWD_DEFINED__
#ifdef __cplusplus
typedef class Email Email;
#else
typedef struct Email Email;
#endif
#endif

#ifndef __IADsPath_FWD_DEFINED__
#define __IADsPath_FWD_DEFINED__
typedef interface IADsPath IADsPath;
#endif

#ifndef __Path_FWD_DEFINED__
#define __Path_FWD_DEFINED__
#ifdef __cplusplus
typedef class Path Path;
#else
typedef struct Path Path;
#endif
#endif

#ifndef __IADsReplicaPointer_FWD_DEFINED__
#define __IADsReplicaPointer_FWD_DEFINED__
typedef interface IADsReplicaPointer IADsReplicaPointer;
#endif

#ifndef __ReplicaPointer_FWD_DEFINED__
#define __ReplicaPointer_FWD_DEFINED__
#ifdef __cplusplus
typedef class ReplicaPointer ReplicaPointer;
#else
typedef struct ReplicaPointer ReplicaPointer;
#endif
#endif

#ifndef __IADsAcl_FWD_DEFINED__
#define __IADsAcl_FWD_DEFINED__
typedef interface IADsAcl IADsAcl;
#endif

#ifndef __IADsTimestamp_FWD_DEFINED__
#define __IADsTimestamp_FWD_DEFINED__
typedef interface IADsTimestamp IADsTimestamp;
#endif

#ifndef __Timestamp_FWD_DEFINED__
#define __Timestamp_FWD_DEFINED__
#ifdef __cplusplus
typedef class Timestamp Timestamp;
#else
typedef struct Timestamp Timestamp;
#endif
#endif

#ifndef __IADsPostalAddress_FWD_DEFINED__
#define __IADsPostalAddress_FWD_DEFINED__
typedef interface IADsPostalAddress IADsPostalAddress;
#endif

#ifndef __PostalAddress_FWD_DEFINED__
#define __PostalAddress_FWD_DEFINED__
#ifdef __cplusplus
typedef class PostalAddress PostalAddress;
#else
typedef struct PostalAddress PostalAddress;
#endif
#endif

#ifndef __IADsBackLink_FWD_DEFINED__
#define __IADsBackLink_FWD_DEFINED__
typedef interface IADsBackLink IADsBackLink;
#endif

#ifndef __BackLink_FWD_DEFINED__
#define __BackLink_FWD_DEFINED__
#ifdef __cplusplus
typedef class BackLink BackLink;
#else
typedef struct BackLink BackLink;
#endif
#endif

#ifndef __IADsTypedName_FWD_DEFINED__
#define __IADsTypedName_FWD_DEFINED__
typedef interface IADsTypedName IADsTypedName;
#endif

#ifndef __TypedName_FWD_DEFINED__
#define __TypedName_FWD_DEFINED__
#ifdef __cplusplus
typedef class TypedName TypedName;
#else
typedef struct TypedName TypedName;
#endif
#endif

#ifndef __IADsHold_FWD_DEFINED__
#define __IADsHold_FWD_DEFINED__
typedef interface IADsHold IADsHold;
#endif

#ifndef __Hold_FWD_DEFINED__
#define __Hold_FWD_DEFINED__
#ifdef __cplusplus
typedef class Hold Hold;
#else
typedef struct Hold Hold;
#endif
#endif

#ifndef __IADsObjectOptions_FWD_DEFINED__
#define __IADsObjectOptions_FWD_DEFINED__
typedef interface IADsObjectOptions IADsObjectOptions;
#endif

#ifndef __IADsPathname_FWD_DEFINED__
#define __IADsPathname_FWD_DEFINED__
typedef interface IADsPathname IADsPathname;
#endif

#ifndef __Pathname_FWD_DEFINED__
#define __Pathname_FWD_DEFINED__
#ifdef __cplusplus
typedef class Pathname Pathname;
#else
typedef struct Pathname Pathname;
#endif
#endif

#ifndef __IADsADSystemInfo_FWD_DEFINED__
#define __IADsADSystemInfo_FWD_DEFINED__
typedef interface IADsADSystemInfo IADsADSystemInfo;
#endif

#ifndef __ADSystemInfo_FWD_DEFINED__
#define __ADSystemInfo_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADSystemInfo ADSystemInfo;
#else
typedef struct ADSystemInfo ADSystemInfo;
#endif
#endif

#ifndef __IADsWinNTSystemInfo_FWD_DEFINED__
#define __IADsWinNTSystemInfo_FWD_DEFINED__
typedef interface IADsWinNTSystemInfo IADsWinNTSystemInfo;
#endif

#ifndef __WinNTSystemInfo_FWD_DEFINED__
#define __WinNTSystemInfo_FWD_DEFINED__
#ifdef __cplusplus
typedef class WinNTSystemInfo WinNTSystemInfo;
#else
typedef struct WinNTSystemInfo WinNTSystemInfo;
#endif
#endif

#ifndef __IADsDNWithBinary_FWD_DEFINED__
#define __IADsDNWithBinary_FWD_DEFINED__
typedef interface IADsDNWithBinary IADsDNWithBinary;
#endif

#ifndef __DNWithBinary_FWD_DEFINED__
#define __DNWithBinary_FWD_DEFINED__
#ifdef __cplusplus
typedef class DNWithBinary DNWithBinary;
#else
typedef struct DNWithBinary DNWithBinary;
#endif
#endif

#ifndef __IADsDNWithString_FWD_DEFINED__
#define __IADsDNWithString_FWD_DEFINED__
typedef interface IADsDNWithString IADsDNWithString;
#endif

#ifndef __DNWithString_FWD_DEFINED__
#define __DNWithString_FWD_DEFINED__
#ifdef __cplusplus
typedef class DNWithString DNWithString;
#else
typedef struct DNWithString DNWithString;
#endif
#endif

#ifndef __IADsSecurityUtility_FWD_DEFINED__
#define __IADsSecurityUtility_FWD_DEFINED__
typedef interface IADsSecurityUtility IADsSecurityUtility;
#endif

#ifndef __ADsSecurityUtility_FWD_DEFINED__
#define __ADsSecurityUtility_FWD_DEFINED__
#ifdef __cplusplus
typedef class ADsSecurityUtility ADsSecurityUtility;
#else
typedef struct ADsSecurityUtility ADsSecurityUtility;
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif


void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *);

#ifndef __ActiveDs_LIBRARY_DEFINED__
#define __ActiveDs_LIBRARY_DEFINED__
typedef enum __MIDL___MIDL_itf_ads_0000_0001 {
    ADSTYPE_INVALID = 0,
    ADSTYPE_DN_STRING = ADSTYPE_INVALID + 1,
    ADSTYPE_CASE_EXACT_STRING = ADSTYPE_DN_STRING + 1,
    ADSTYPE_CASE_IGNORE_STRING = ADSTYPE_CASE_EXACT_STRING + 1,
    ADSTYPE_PRINTABLE_STRING = ADSTYPE_CASE_IGNORE_STRING + 1,
    ADSTYPE_NUMERIC_STRING = ADSTYPE_PRINTABLE_STRING + 1,
    ADSTYPE_BOOLEAN = ADSTYPE_NUMERIC_STRING + 1,
    ADSTYPE_INTEGER = ADSTYPE_BOOLEAN + 1,
    ADSTYPE_OCTET_STRING = ADSTYPE_INTEGER + 1,
    ADSTYPE_UTC_TIME = ADSTYPE_OCTET_STRING + 1,
    ADSTYPE_LARGE_INTEGER = ADSTYPE_UTC_TIME + 1,
    ADSTYPE_PROV_SPECIFIC = ADSTYPE_LARGE_INTEGER + 1,
    ADSTYPE_OBJECT_CLASS = ADSTYPE_PROV_SPECIFIC + 1,
    ADSTYPE_CASEIGNORE_LIST = ADSTYPE_OBJECT_CLASS + 1,
    ADSTYPE_OCTET_LIST = ADSTYPE_CASEIGNORE_LIST + 1,
    ADSTYPE_PATH = ADSTYPE_OCTET_LIST + 1,
    ADSTYPE_POSTALADDRESS = ADSTYPE_PATH + 1,
    ADSTYPE_TIMESTAMP = ADSTYPE_POSTALADDRESS + 1,
    ADSTYPE_BACKLINK = ADSTYPE_TIMESTAMP + 1,
    ADSTYPE_TYPEDNAME = ADSTYPE_BACKLINK + 1,
    ADSTYPE_HOLD = ADSTYPE_TYPEDNAME + 1,
    ADSTYPE_NETADDRESS = ADSTYPE_HOLD + 1,
    ADSTYPE_REPLICAPOINTER = ADSTYPE_NETADDRESS + 1,
    ADSTYPE_FAXNUMBER = ADSTYPE_REPLICAPOINTER + 1,
    ADSTYPE_EMAIL = ADSTYPE_FAXNUMBER + 1,
    ADSTYPE_NT_SECURITY_DESCRIPTOR = ADSTYPE_EMAIL + 1,
    ADSTYPE_UNKNOWN = ADSTYPE_NT_SECURITY_DESCRIPTOR + 1,
    ADSTYPE_DN_WITH_BINARY = ADSTYPE_UNKNOWN + 1,
    ADSTYPE_DN_WITH_STRING = ADSTYPE_DN_WITH_BINARY + 1
} ADSTYPEENUM;

typedef ADSTYPEENUM ADSTYPE;
typedef unsigned char BYTE;
typedef unsigned char *LPBYTE;
typedef unsigned char *PBYTE;
typedef LPWSTR ADS_DN_STRING;
typedef LPWSTR *PADS_DN_STRING;
typedef LPWSTR ADS_CASE_EXACT_STRING;
typedef LPWSTR *PADS_CASE_EXACT_STRING;
typedef LPWSTR ADS_CASE_IGNORE_STRING;
typedef LPWSTR *PADS_CASE_IGNORE_STRING;
typedef LPWSTR ADS_PRINTABLE_STRING;
typedef LPWSTR *PADS_PRINTABLE_STRING;
typedef LPWSTR ADS_NUMERIC_STRING;
typedef LPWSTR *PADS_NUMERIC_STRING;
typedef DWORD ADS_BOOLEAN;
typedef DWORD *LPNDS_BOOLEAN;
typedef DWORD ADS_INTEGER;
typedef DWORD *PADS_INTEGER;

typedef struct __MIDL___MIDL_itf_ads_0000_0002 {
    DWORD dwLength;
    LPBYTE lpValue;
} ADS_OCTET_STRING;
typedef struct __MIDL___MIDL_itf_ads_0000_0002 *PADS_OCTET_STRING;

typedef struct __MIDL___MIDL_itf_ads_0000_0003 {
    DWORD dwLength;
    LPBYTE lpValue;
} ADS_NT_SECURITY_DESCRIPTOR;
typedef struct __MIDL___MIDL_itf_ads_0000_0003 *PADS_NT_SECURITY_DESCRIPTOR;

typedef SYSTEMTIME ADS_UTC_TIME;
typedef SYSTEMTIME *PADS_UTC_TIME;
typedef LARGE_INTEGER ADS_LARGE_INTEGER;
typedef LARGE_INTEGER *PADS_LARGE_INTEGER;
typedef LPWSTR ADS_OBJECT_CLASS;
typedef LPWSTR *PADS_OBJECT_CLASS;

typedef struct __MIDL___MIDL_itf_ads_0000_0004 {
    DWORD dwLength;
    LPBYTE lpValue;
} ADS_PROV_SPECIFIC;
typedef struct __MIDL___MIDL_itf_ads_0000_0004 *PADS_PROV_SPECIFIC;

typedef struct _ADS_CASEIGNORE_LIST {
    struct _ADS_CASEIGNORE_LIST *Next;
    LPWSTR String;
} ADS_CASEIGNORE_LIST;
typedef struct _ADS_CASEIGNORE_LIST *PADS_CASEIGNORE_LIST;

typedef struct _ADS_OCTET_LIST {
    struct _ADS_OCTET_LIST *Next;
    DWORD Length;
    BYTE *Data;
} ADS_OCTET_LIST;
typedef struct _ADS_OCTET_LIST *PADS_OCTET_LIST;

typedef struct __MIDL___MIDL_itf_ads_0000_0005 {
    DWORD Type;
    LPWSTR VolumeName;
    LPWSTR Path;
} ADS_PATH;
typedef struct __MIDL___MIDL_itf_ads_0000_0005 *PADS_PATH;

typedef struct __MIDL___MIDL_itf_ads_0000_0006 {
    LPWSTR PostalAddress[6];
} ADS_POSTALADDRESS;
typedef struct __MIDL___MIDL_itf_ads_0000_0006 *PADS_POSTALADDRESS;

typedef struct __MIDL___MIDL_itf_ads_0000_0007 {
    DWORD WholeSeconds;
    DWORD EventID;
} ADS_TIMESTAMP;
typedef struct __MIDL___MIDL_itf_ads_0000_0007 *PADS_TIMESTAMP;

typedef struct __MIDL___MIDL_itf_ads_0000_0008 {
    DWORD RemoteID;
    LPWSTR ObjectName;
} ADS_BACKLINK;
typedef struct __MIDL___MIDL_itf_ads_0000_0008 *PADS_BACKLINK;

typedef struct __MIDL___MIDL_itf_ads_0000_0009 {
    LPWSTR ObjectName;
    DWORD Level;
    DWORD Interval;
} ADS_TYPEDNAME;
typedef struct __MIDL___MIDL_itf_ads_0000_0009 *PADS_TYPEDNAME;

typedef struct __MIDL___MIDL_itf_ads_0000_0010 {
    LPWSTR ObjectName;
    DWORD Amount;
} ADS_HOLD;
typedef struct __MIDL___MIDL_itf_ads_0000_0010 *PADS_HOLD;

typedef struct __MIDL___MIDL_itf_ads_0000_0011 {
    DWORD AddressType;
    DWORD AddressLength;
    BYTE *Address;
} ADS_NETADDRESS;
typedef struct __MIDL___MIDL_itf_ads_0000_0011 *PADS_NETADDRESS;

typedef struct __MIDL___MIDL_itf_ads_0000_0012 {
    LPWSTR ServerName;
    DWORD ReplicaType;
    DWORD ReplicaNumber;
    DWORD Count;
    PADS_NETADDRESS ReplicaAddressHints;
} ADS_REPLICAPOINTER;
typedef struct __MIDL___MIDL_itf_ads_0000_0012 *PADS_REPLICAPOINTER;

typedef struct __MIDL___MIDL_itf_ads_0000_0013 {
    LPWSTR TelephoneNumber;
    DWORD NumberOfBits;
    LPBYTE Parameters;
} ADS_FAXNUMBER;
typedef struct __MIDL___MIDL_itf_ads_0000_0013 *PADS_FAXNUMBER;

typedef struct __MIDL___MIDL_itf_ads_0000_0014 {
    LPWSTR Address;
    DWORD Type;
} ADS_EMAIL;
typedef struct __MIDL___MIDL_itf_ads_0000_0014 *PADS_EMAIL;

typedef struct __MIDL___MIDL_itf_ads_0000_0015 {
    DWORD dwLength;
    LPBYTE lpBinaryValue;
    LPWSTR pszDNString;
} ADS_DN_WITH_BINARY;
typedef struct __MIDL___MIDL_itf_ads_0000_0015 *PADS_DN_WITH_BINARY;

typedef struct __MIDL___MIDL_itf_ads_0000_0016 {
    LPWSTR pszStringValue;
    LPWSTR pszDNString;
} ADS_DN_WITH_STRING;
typedef struct __MIDL___MIDL_itf_ads_0000_0016 *PADS_DN_WITH_STRING;

typedef struct _adsvalue {
    ADSTYPE dwType;
    union {
        ADS_DN_STRING DNString;
        ADS_CASE_EXACT_STRING CaseExactString;
        ADS_CASE_IGNORE_STRING CaseIgnoreString;
        ADS_PRINTABLE_STRING PrintableString;
        ADS_NUMERIC_STRING NumericString;
        ADS_BOOLEAN Boolean;
        ADS_INTEGER Integer;
        ADS_OCTET_STRING OctetString;
        ADS_UTC_TIME UTCTime;
        ADS_LARGE_INTEGER LargeInteger;
        ADS_OBJECT_CLASS ClassName;
        ADS_PROV_SPECIFIC ProviderSpecific;
        PADS_CASEIGNORE_LIST pCaseIgnoreList;
        PADS_OCTET_LIST pOctetList;
        PADS_PATH pPath;
        PADS_POSTALADDRESS pPostalAddress;
        ADS_TIMESTAMP Timestamp;
        ADS_BACKLINK BackLink;
        PADS_TYPEDNAME pTypedName;
        ADS_HOLD Hold;
        PADS_NETADDRESS pNetAddress;
        PADS_REPLICAPOINTER pReplicaPointer;
        PADS_FAXNUMBER pFaxNumber;
        ADS_EMAIL Email;
        ADS_NT_SECURITY_DESCRIPTOR SecurityDescriptor;
        PADS_DN_WITH_BINARY pDNWithBinary;
        PADS_DN_WITH_STRING pDNWithString;
    };
} ADSVALUE;
typedef struct _adsvalue *PADSVALUE;
typedef struct _adsvalue *LPADSVALUE;

typedef struct _ads_attr_info {
    LPWSTR pszAttrName;
    DWORD dwControlCode;
    ADSTYPE dwADsType;
    PADSVALUE pADsValues;
    DWORD dwNumValues;
} ADS_ATTR_INFO;
typedef struct _ads_attr_info *PADS_ATTR_INFO;

typedef enum __MIDL___MIDL_itf_ads_0000_0018 {
    ADS_SECURE_AUTHENTICATION = 0x1,
    ADS_USE_ENCRYPTION = 0x2,
    ADS_USE_SSL = 0x2,
    ADS_READONLY_SERVER = 0x4,
    ADS_PROMPT_CREDENTIALS = 0x8,
    ADS_NO_AUTHENTICATION = 0x10,
    ADS_FAST_BIND = 0x20,
    ADS_USE_SIGNING = 0x40,
    ADS_USE_SEALING = 0x80,
    ADS_USE_DELEGATION = 0x100,
    ADS_SERVER_BIND = 0x200,
    ADS_AUTH_RESERVED = 0x80000000
} ADS_AUTHENTICATION_ENUM;

#define ADS_ATTR_CLEAR  (1)
#define ADS_ATTR_UPDATE  (2)
#define ADS_ATTR_APPEND  (3)
#define ADS_ATTR_DELETE  (4)

typedef struct _ads_object_info {
    LPWSTR pszRDN;
    LPWSTR pszObjectDN;
    LPWSTR pszParentDN;
    LPWSTR pszSchemaDN;
    LPWSTR pszClassName;
} ADS_OBJECT_INFO;
typedef struct _ads_object_info *PADS_OBJECT_INFO;

typedef enum __MIDL___MIDL_itf_ads_0000_0019 {
    ADS_STATUS_S_OK = 0,
    ADS_STATUS_INVALID_SEARCHPREF = ADS_STATUS_S_OK + 1,
    ADS_STATUS_INVALID_SEARCHPREFVALUE = ADS_STATUS_INVALID_SEARCHPREF + 1
} ADS_STATUSENUM;
typedef ADS_STATUSENUM ADS_STATUS;
typedef ADS_STATUSENUM *PADS_STATUS;

typedef enum __MIDL___MIDL_itf_ads_0000_0020 {
    ADS_DEREF_NEVER = 0,
    ADS_DEREF_SEARCHING = 1,
    ADS_DEREF_FINDING = 2,
    ADS_DEREF_ALWAYS = 3
} ADS_DEREFENUM;

typedef enum __MIDL___MIDL_itf_ads_0000_0021 {
    ADS_SCOPE_BASE = 0,
    ADS_SCOPE_ONELEVEL = 1,
    ADS_SCOPE_SUBTREE = 2
} ADS_SCOPEENUM;

typedef enum __MIDL___MIDL_itf_ads_0000_0022 {
    ADSIPROP_ASYNCHRONOUS = 0,
    ADSIPROP_DEREF_ALIASES = 0x1,
    ADSIPROP_SIZE_LIMIT = 0x2,
    ADSIPROP_TIME_LIMIT = 0x3,
    ADSIPROP_ATTRIBTYPES_ONLY = 0x4,
    ADSIPROP_SEARCH_SCOPE = 0x5,
    ADSIPROP_TIMEOUT = 0x6,
    ADSIPROP_PAGESIZE = 0x7,
    ADSIPROP_PAGED_TIME_LIMIT = 0x8,
    ADSIPROP_CHASE_REFERRALS = 0x9,
    ADSIPROP_SORT_ON = 0xa,
    ADSIPROP_CACHE_RESULTS = 0xb,
    ADSIPROP_ADSIFLAG = 0xc
} ADS_PREFERENCES_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0000_0023 {
    ADSI_DIALECT_LDAP = 0,
    ADSI_DIALECT_SQL = 0x1
} ADSI_DIALECT_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0000_0024 {
    ADS_CHASE_REFERRALS_NEVER = 0,
    ADS_CHASE_REFERRALS_SUBORDINATE = 0x20,
    ADS_CHASE_REFERRALS_EXTERNAL = 0x40,
    ADS_CHASE_REFERRALS_ALWAYS = ADS_CHASE_REFERRALS_SUBORDINATE | ADS_CHASE_REFERRALS_EXTERNAL
} ADS_CHASE_REFERRALS_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0000_0025 {
    ADS_SEARCHPREF_ASYNCHRONOUS = 0,
    ADS_SEARCHPREF_DEREF_ALIASES = ADS_SEARCHPREF_ASYNCHRONOUS + 1,
    ADS_SEARCHPREF_SIZE_LIMIT = ADS_SEARCHPREF_DEREF_ALIASES + 1,
    ADS_SEARCHPREF_TIME_LIMIT = ADS_SEARCHPREF_SIZE_LIMIT + 1,
    ADS_SEARCHPREF_ATTRIBTYPES_ONLY = ADS_SEARCHPREF_TIME_LIMIT + 1,
    ADS_SEARCHPREF_SEARCH_SCOPE = ADS_SEARCHPREF_ATTRIBTYPES_ONLY + 1,
    ADS_SEARCHPREF_TIMEOUT = ADS_SEARCHPREF_SEARCH_SCOPE + 1,
    ADS_SEARCHPREF_PAGESIZE = ADS_SEARCHPREF_TIMEOUT + 1,
    ADS_SEARCHPREF_PAGED_TIME_LIMIT = ADS_SEARCHPREF_PAGESIZE + 1,
    ADS_SEARCHPREF_CHASE_REFERRALS = ADS_SEARCHPREF_PAGED_TIME_LIMIT + 1,
    ADS_SEARCHPREF_SORT_ON = ADS_SEARCHPREF_CHASE_REFERRALS + 1,
    ADS_SEARCHPREF_CACHE_RESULTS = ADS_SEARCHPREF_SORT_ON + 1,
    ADS_SEARCHPREF_DIRSYNC = ADS_SEARCHPREF_CACHE_RESULTS + 1,
    ADS_SEARCHPREF_TOMBSTONE = ADS_SEARCHPREF_DIRSYNC + 1,
    ADS_SEARCHPREF_VLV = ADS_SEARCHPREF_TOMBSTONE + 1,
    ADS_SEARCHPREF_ATTRIBUTE_QUERY = ADS_SEARCHPREF_VLV + 1,
    ADS_SEARCHPREF_SECURITY_MASK = ADS_SEARCHPREF_ATTRIBUTE_QUERY + 1,
    ADS_SEARCHPREF_DIRSYNC_FLAG = ADS_SEARCHPREF_SECURITY_MASK + 1,
    ADS_SEARCHPREF_EXTENDED_DN = ADS_SEARCHPREF_DIRSYNC_FLAG + 1
} ADS_SEARCHPREF_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0000_0026 {
    ADS_PASSWORD_ENCODE_REQUIRE_SSL = 0,
    ADS_PASSWORD_ENCODE_CLEAR = 1
} ADS_PASSWORD_ENCODING_ENUM;

typedef ADS_SEARCHPREF_ENUM ADS_SEARCHPREF;

typedef struct ads_searchpref_info {
    ADS_SEARCHPREF dwSearchPref;
    ADSVALUE vValue;
    ADS_STATUS dwStatus;
} ADS_SEARCHPREF_INFO;
typedef struct ads_searchpref_info *PADS_SEARCHPREF_INFO;
typedef struct ads_searchpref_info *LPADS_SEARCHPREF_INFO;

#define ADS_DIRSYNC_COOKIE  (L"fc8cb04d-311d-406c-8cb9-1ae8b843b418")
#define ADS_VLV_RESPONSE  (L"fc8cb04d-311d-406c-8cb9-1ae8b843b419")

typedef HANDLE ADS_SEARCH_HANDLE;
typedef HANDLE *PADS_SEARCH_HANDLE;

typedef struct ads_search_column {
    LPWSTR pszAttrName;
    ADSTYPE dwADsType;
    PADSVALUE pADsValues;
    DWORD dwNumValues;
    HANDLE hReserved;
} ADS_SEARCH_COLUMN;
typedef struct ads_search_column *PADS_SEARCH_COLUMN;

typedef struct _ads_attr_def {
    LPWSTR pszAttrName;
    ADSTYPE dwADsType;
    DWORD dwMinRange;
    DWORD dwMaxRange;
    BOOL fMultiValued;
} ADS_ATTR_DEF;
typedef struct _ads_attr_def *PADS_ATTR_DEF;

typedef struct _ads_class_def {
    LPWSTR pszClassName;
    DWORD dwMandatoryAttrs;
    LPWSTR *ppszMandatoryAttrs;
    DWORD optionalAttrs;
    LPWSTR **ppszOptionalAttrs;
    DWORD dwNamingAttrs;
    LPWSTR **ppszNamingAttrs;
    DWORD dwSuperClasses;
    LPWSTR **ppszSuperClasses;
    BOOL fIsContainer;
} ADS_CLASS_DEF;
typedef struct _ads_class_def *PADS_CLASS_DEF;

typedef struct _ads_sortkey {
    LPWSTR pszAttrType;
    LPWSTR pszReserved;
    BOOLEAN fReverseorder;
} ADS_SORTKEY;
typedef struct _ads_sortkey *PADS_SORTKEY;

typedef struct _ads_vlv {
    DWORD dwBeforeCount;
    DWORD dwAfterCount;
    DWORD dwOffset;
    DWORD dwContentCount;
    LPWSTR pszTarget;
    DWORD dwContextIDLength;
    LPBYTE lpContextID;
} ADS_VLV;
typedef struct _ads_vlv *PADS_VLV;

#define ADS_EXT_MINEXTDISPID  (1)
#define ADS_EXT_MAXEXTDISPID  (16777215)
#define ADS_EXT_INITCREDENTIALS  (1)
#define ADS_EXT_INITIALIZE_COMPLETE  (2)

typedef enum __MIDL___MIDL_itf_ads_0000_0027 {
    ADS_PROPERTY_CLEAR = 1,
    ADS_PROPERTY_UPDATE = 2,
    ADS_PROPERTY_APPEND = 3,
    ADS_PROPERTY_DELETE = 4
} ADS_PROPERTY_OPERATION_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0130_0001 {
    ADS_SYSTEMFLAG_DISALLOW_DELETE = 0x80000000,
    ADS_SYSTEMFLAG_CONFIG_ALLOW_RENAME = 0x40000000,
    ADS_SYSTEMFLAG_CONFIG_ALLOW_MOVE = 0x20000000,
    ADS_SYSTEMFLAG_CONFIG_ALLOW_LIMITED_MOVE = 0x10000000,
    ADS_SYSTEMFLAG_DOMAIN_DISALLOW_RENAME = 0x8000000,
    ADS_SYSTEMFLAG_DOMAIN_DISALLOW_MOVE = 0x4000000,
    ADS_SYSTEMFLAG_CR_NTDS_NC = 0x1,
    ADS_SYSTEMFLAG_CR_NTDS_DOMAIN = 0x2,
    ADS_SYSTEMFLAG_ATTR_NOT_REPLICATED = 0x1,
    ADS_SYSTEMFLAG_ATTR_IS_CONSTRUCTED = 0x4
} ADS_SYSTEMFLAG_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0136_0001 {
    ADS_GROUP_TYPE_GLOBAL_GROUP = 0x2,
    ADS_GROUP_TYPE_DOMAIN_LOCAL_GROUP = 0x4,
    ADS_GROUP_TYPE_LOCAL_GROUP = 0x4,
    ADS_GROUP_TYPE_UNIVERSAL_GROUP = 0x8,
    ADS_GROUP_TYPE_SECURITY_ENABLED = 0x80000000
} ADS_GROUP_TYPE_ENUM;

typedef enum ADS_USER_FLAG {
    ADS_UF_SCRIPT = 0x1,
    ADS_UF_ACCOUNTDISABLE = 0x2,
    ADS_UF_HOMEDIR_REQUIRED = 0x8,
    ADS_UF_LOCKOUT = 0x10,
    ADS_UF_PASSWD_NOTREQD = 0x20,
    ADS_UF_PASSWD_CANT_CHANGE = 0x40,
    ADS_UF_ENCRYPTED_TEXT_PASSWORD_ALLOWED = 0x80,
    ADS_UF_TEMP_DUPLICATE_ACCOUNT = 0x100,
    ADS_UF_NORMAL_ACCOUNT = 0x200,
    ADS_UF_INTERDOMAIN_TRUST_ACCOUNT = 0x800,
    ADS_UF_WORKSTATION_TRUST_ACCOUNT = 0x1000,
    ADS_UF_SERVER_TRUST_ACCOUNT = 0x2000,
    ADS_UF_DONT_EXPIRE_PASSWD = 0x10000,
    ADS_UF_MNS_LOGON_ACCOUNT = 0x20000,
    ADS_UF_SMARTCARD_REQUIRED = 0x40000,
    ADS_UF_TRUSTED_FOR_DELEGATION = 0x80000,
    ADS_UF_NOT_DELEGATED = 0x100000,
    ADS_UF_USE_DES_KEY_ONLY = 0x200000,
    ADS_UF_DONT_REQUIRE_PREAUTH = 0x400000,
    ADS_UF_PASSWORD_EXPIRED = 0x800000,
    ADS_UF_TRUSTED_TO_AUTHENTICATE_FOR_DELEGATION = 0x1000000
} ADS_USER_FLAG_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0158_0001 {
    ADS_RIGHT_DELETE = 0x10000,
    ADS_RIGHT_READ_CONTROL = 0x20000,
    ADS_RIGHT_WRITE_DAC = 0x40000,
    ADS_RIGHT_WRITE_OWNER = 0x80000,
    ADS_RIGHT_SYNCHRONIZE = 0x100000,
    ADS_RIGHT_ACCESS_SYSTEM_SECURITY = 0x1000000,
    ADS_RIGHT_GENERIC_READ = 0x80000000,
    ADS_RIGHT_GENERIC_WRITE = 0x40000000,
    ADS_RIGHT_GENERIC_EXECUTE = 0x20000000,
    ADS_RIGHT_GENERIC_ALL = 0x10000000,
    ADS_RIGHT_DS_CREATE_CHILD = 0x1,
    ADS_RIGHT_DS_DELETE_CHILD = 0x2,
    ADS_RIGHT_ACTRL_DS_LIST = 0x4,
    ADS_RIGHT_DS_SELF = 0x8,
    ADS_RIGHT_DS_READ_PROP = 0x10,
    ADS_RIGHT_DS_WRITE_PROP = 0x20,
    ADS_RIGHT_DS_DELETE_TREE = 0x40,
    ADS_RIGHT_DS_LIST_OBJECT = 0x80,
    ADS_RIGHT_DS_CONTROL_ACCESS = 0x100
} ADS_RIGHTS_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0158_0002 {
    ADS_ACETYPE_ACCESS_ALLOWED = 0,
    ADS_ACETYPE_ACCESS_DENIED = 0x1,
    ADS_ACETYPE_SYSTEM_AUDIT = 0x2,
    ADS_ACETYPE_ACCESS_ALLOWED_OBJECT = 0x5,
    ADS_ACETYPE_ACCESS_DENIED_OBJECT = 0x6,
    ADS_ACETYPE_SYSTEM_AUDIT_OBJECT = 0x7,
    ADS_ACETYPE_SYSTEM_ALARM_OBJECT = 0x8,
    ADS_ACETYPE_ACCESS_ALLOWED_CALLBACK = 0x9,
    ADS_ACETYPE_ACCESS_DENIED_CALLBACK = 0xa,
    ADS_ACETYPE_ACCESS_ALLOWED_CALLBACK_OBJECT = 0xb,
    ADS_ACETYPE_ACCESS_DENIED_CALLBACK_OBJECT = 0xc,
    ADS_ACETYPE_SYSTEM_AUDIT_CALLBACK = 0xd,
    ADS_ACETYPE_SYSTEM_ALARM_CALLBACK = 0xe,
    ADS_ACETYPE_SYSTEM_AUDIT_CALLBACK_OBJECT = 0xf,
    ADS_ACETYPE_SYSTEM_ALARM_CALLBACK_OBJECT = 0x10
} ADS_ACETYPE_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0158_0003 {
    ADS_ACEFLAG_INHERIT_ACE = 0x2,
    ADS_ACEFLAG_NO_PROPAGATE_INHERIT_ACE = 0x4,
    ADS_ACEFLAG_INHERIT_ONLY_ACE = 0x8,
    ADS_ACEFLAG_INHERITED_ACE = 0x10,
    ADS_ACEFLAG_VALID_INHERIT_FLAGS = 0x1f,
    ADS_ACEFLAG_SUCCESSFUL_ACCESS = 0x40,
    ADS_ACEFLAG_FAILED_ACCESS = 0x80
} ADS_ACEFLAG_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0158_0004 {
    ADS_FLAG_OBJECT_TYPE_PRESENT = 0x1,
    ADS_FLAG_INHERITED_OBJECT_TYPE_PRESENT = 0x2
} ADS_FLAGTYPE_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0158_0005 {
    ADS_SD_CONTROL_SE_OWNER_DEFAULTED = 0x1,
    ADS_SD_CONTROL_SE_GROUP_DEFAULTED = 0x2,
    ADS_SD_CONTROL_SE_DACL_PRESENT = 0x4,
    ADS_SD_CONTROL_SE_DACL_DEFAULTED = 0x8,
    ADS_SD_CONTROL_SE_SACL_PRESENT = 0x10,
    ADS_SD_CONTROL_SE_SACL_DEFAULTED = 0x20,
    ADS_SD_CONTROL_SE_DACL_AUTO_INHERIT_REQ = 0x100,
    ADS_SD_CONTROL_SE_SACL_AUTO_INHERIT_REQ = 0x200,
    ADS_SD_CONTROL_SE_DACL_AUTO_INHERITED = 0x400,
    ADS_SD_CONTROL_SE_SACL_AUTO_INHERITED = 0x800,
    ADS_SD_CONTROL_SE_DACL_PROTECTED = 0x1000,
    ADS_SD_CONTROL_SE_SACL_PROTECTED = 0x2000,
    ADS_SD_CONTROL_SE_SELF_RELATIVE = 0x8000
} ADS_SD_CONTROL_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0158_0006 {
    ADS_SD_REVISION_DS = 4
} ADS_SD_REVISION_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0159_0001 {
    ADS_NAME_TYPE_1779 = 1,
    ADS_NAME_TYPE_CANONICAL = 2,
    ADS_NAME_TYPE_NT4 = 3,
    ADS_NAME_TYPE_DISPLAY = 4,
    ADS_NAME_TYPE_DOMAIN_SIMPLE = 5,
    ADS_NAME_TYPE_ENTERPRISE_SIMPLE = 6,
    ADS_NAME_TYPE_GUID = 7,
    ADS_NAME_TYPE_UNKNOWN = 8,
    ADS_NAME_TYPE_USER_PRINCIPAL_NAME = 9,
    ADS_NAME_TYPE_CANONICAL_EX = 10,
    ADS_NAME_TYPE_SERVICE_PRINCIPAL_NAME = 11,
    ADS_NAME_TYPE_SID_OR_SID_HISTORY_NAME = 12
} ADS_NAME_TYPE_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0159_0002 {
    ADS_NAME_INITTYPE_DOMAIN = 1,
    ADS_NAME_INITTYPE_SERVER = 2,
    ADS_NAME_INITTYPE_GC = 3
} ADS_NAME_INITTYPE_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0173_0001 {
    ADS_OPTION_SERVERNAME = 0,
    ADS_OPTION_REFERRALS = ADS_OPTION_SERVERNAME + 1,
    ADS_OPTION_PAGE_SIZE = ADS_OPTION_REFERRALS + 1,
    ADS_OPTION_SECURITY_MASK = ADS_OPTION_PAGE_SIZE + 1,
    ADS_OPTION_MUTUAL_AUTH_STATUS = ADS_OPTION_SECURITY_MASK + 1,
    ADS_OPTION_QUOTA = ADS_OPTION_MUTUAL_AUTH_STATUS + 1,
    ADS_OPTION_PASSWORD_PORTNUMBER = ADS_OPTION_QUOTA + 1,
    ADS_OPTION_PASSWORD_METHOD = ADS_OPTION_PASSWORD_PORTNUMBER + 1
} ADS_OPTION_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0173_0002 {
    ADS_SECURITY_INFO_OWNER = 0x1,
    ADS_SECURITY_INFO_GROUP = 0x2,
    ADS_SECURITY_INFO_DACL = 0x4,
    ADS_SECURITY_INFO_SACL = 0x8
} ADS_SECURITY_INFO_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0174_0001 {
    ADS_SETTYPE_FULL = 1,
    ADS_SETTYPE_PROVIDER = 2,
    ADS_SETTYPE_SERVER = 3,
    ADS_SETTYPE_DN = 4
} ADS_SETTYPE_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0174_0002 {
    ADS_FORMAT_WINDOWS = 1,
    ADS_FORMAT_WINDOWS_NO_SERVER = 2,
    ADS_FORMAT_WINDOWS_DN = 3,
    ADS_FORMAT_WINDOWS_PARENT = 4,
    ADS_FORMAT_X500 = 5,
    ADS_FORMAT_X500_NO_SERVER = 6,
    ADS_FORMAT_X500_DN = 7,
    ADS_FORMAT_X500_PARENT = 8,
    ADS_FORMAT_SERVER = 9,
    ADS_FORMAT_PROVIDER = 10,
    ADS_FORMAT_LEAF = 11
} ADS_FORMAT_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0174_0003 {
    ADS_DISPLAY_FULL = 1,
    ADS_DISPLAY_VALUE_ONLY = 2
} ADS_DISPLAY_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0174_0004 {
    ADS_ESCAPEDMODE_DEFAULT = 1,
    ADS_ESCAPEDMODE_ON = 2,
    ADS_ESCAPEDMODE_OFF = 3,
    ADS_ESCAPEDMODE_OFF_EX = 4
} ADS_ESCAPE_MODE_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0179_0001 {
    ADS_PATH_FILE = 1,
    ADS_PATH_FILESHARE = 2,
    ADS_PATH_REGISTRY = 3
} ADS_PATHTYPE_ENUM;

typedef enum __MIDL___MIDL_itf_ads_0179_0002 {
    ADS_SD_FORMAT_IID = 1,
    ADS_SD_FORMAT_RAW = 2,
    ADS_SD_FORMAT_HEXSTRING = 3
} ADS_SD_FORMAT_ENUM;

EXTERN_C const IID LIBID_ActiveDs;
#ifndef __IADs_INTERFACE_DEFINED__
#define __IADs_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADs;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("fd8256d0-fd15-11ce-abc4-02608c9e7553") IADs:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Name (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Class (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GUID (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ADsPath (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Parent (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Schema (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInfo (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetInfo (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Get (BSTR bstrName, VARIANT * pvProp) = 0;
    virtual HRESULT STDMETHODCALLTYPE Put (BSTR bstrName, VARIANT vProp) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEx (BSTR bstrName, VARIANT * pvProp) = 0;
    virtual HRESULT STDMETHODCALLTYPE PutEx (long lnControlCode, BSTR bstrName, VARIANT vProp) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInfoEx (VARIANT vProperties, long lnReserved) = 0;
};
#else
typedef struct IADsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADs * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADs * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADs * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADs * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADs * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADs * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADs * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADs * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADs * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADs * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADs * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADs * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADs * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADs * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADs * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADs * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADs * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADs * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADs * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADs * This, VARIANT vProperties, long lnReserved);
    END_INTERFACE
} IADsVtbl;
interface IADs {
    CONST_VTBL struct IADsVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADs_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADs_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADs_Release(This)  (This)->lpVtbl->Release(This)
#define IADs_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADs_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADs_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADs_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADs_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADs_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADs_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADs_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADs_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADs_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADs_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADs_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADs_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADs_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADs_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADs_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADs_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADs_get_Name_Proxy (IADs * This, BSTR * retval);
void __RPC_STUB IADs_get_Name_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADs_get_Class_Proxy (IADs * This, BSTR * retval);
void __RPC_STUB IADs_get_Class_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADs_get_GUID_Proxy (IADs * This, BSTR * retval);
void __RPC_STUB IADs_get_GUID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADs_get_ADsPath_Proxy (IADs * This, BSTR * retval);
void __RPC_STUB IADs_get_ADsPath_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADs_get_Parent_Proxy (IADs * This, BSTR * retval);
void __RPC_STUB IADs_get_Parent_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADs_get_Schema_Proxy (IADs * This, BSTR * retval);
void __RPC_STUB IADs_get_Schema_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADs_GetInfo_Proxy (IADs * This);
void __RPC_STUB IADs_GetInfo_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADs_SetInfo_Proxy (IADs * This);
void __RPC_STUB IADs_SetInfo_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADs_Get_Proxy (IADs * This, BSTR bstrName, VARIANT * pvProp);
void __RPC_STUB IADs_Get_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADs_Put_Proxy (IADs * This, BSTR bstrName, VARIANT vProp);
void __RPC_STUB IADs_Put_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADs_GetEx_Proxy (IADs * This, BSTR bstrName, VARIANT * pvProp);
void __RPC_STUB IADs_GetEx_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADs_PutEx_Proxy (IADs * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
void __RPC_STUB IADs_PutEx_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADs_GetInfoEx_Proxy (IADs * This, VARIANT vProperties, long lnReserved);
void __RPC_STUB IADs_GetInfoEx_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsContainer_INTERFACE_DEFINED__
#define __IADsContainer_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsContainer;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("001677d0-fd16-11ce-abc4-02608c9e7553") IADsContainer:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Count (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get__NewEnum (IUnknown ** retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Filter (VARIANT * pVar) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Filter (VARIANT Var) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Hints (VARIANT * pvFilter) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Hints (VARIANT vHints) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetObject (BSTR ClassName, BSTR RelativeName, IDispatch ** ppObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE Create (BSTR ClassName, BSTR RelativeName, IDispatch ** ppObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE Delete (BSTR bstrClassName, BSTR bstrRelativeName) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyHere (BSTR SourceName, BSTR NewName, IDispatch ** ppObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE MoveHere (BSTR SourceName, BSTR NewName, IDispatch ** ppObject) = 0;
};
#else
typedef struct IADsContainerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsContainer * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsContainer * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsContainer * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsContainer * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsContainer * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsContainer * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsContainer * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Count) (IADsContainer * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * get__NewEnum) (IADsContainer * This, IUnknown ** retval);
    HRESULT (STDMETHODCALLTYPE * get_Filter) (IADsContainer * This, VARIANT * pVar);
    HRESULT (STDMETHODCALLTYPE * put_Filter) (IADsContainer * This, VARIANT Var);
    HRESULT (STDMETHODCALLTYPE * get_Hints) (IADsContainer * This, VARIANT * pvFilter);
    HRESULT (STDMETHODCALLTYPE * put_Hints) (IADsContainer * This, VARIANT vHints);
    HRESULT (STDMETHODCALLTYPE * GetObject) (IADsContainer * This, BSTR ClassName, BSTR RelativeName, IDispatch ** ppObject);
    HRESULT (STDMETHODCALLTYPE * Create) (IADsContainer * This, BSTR ClassName, BSTR RelativeName, IDispatch ** ppObject);
    HRESULT (STDMETHODCALLTYPE * Delete) (IADsContainer * This, BSTR bstrClassName, BSTR bstrRelativeName);
    HRESULT (STDMETHODCALLTYPE * CopyHere) (IADsContainer * This, BSTR SourceName, BSTR NewName, IDispatch ** ppObject);
    HRESULT (STDMETHODCALLTYPE * MoveHere) (IADsContainer * This, BSTR SourceName, BSTR NewName, IDispatch ** ppObject);
    END_INTERFACE
} IADsContainerVtbl;
interface IADsContainer {
    CONST_VTBL struct IADsContainerVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsContainer_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsContainer_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsContainer_Release(This)  (This)->lpVtbl->Release(This)
#define IADsContainer_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsContainer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsContainer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsContainer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsContainer_get_Count(This,retval)  (This)->lpVtbl->get_Count(This,retval)
#define IADsContainer_get__NewEnum(This,retval)  (This)->lpVtbl->get__NewEnum(This,retval)
#define IADsContainer_get_Filter(This,pVar)  (This)->lpVtbl->get_Filter(This,pVar)
#define IADsContainer_put_Filter(This,Var)  (This)->lpVtbl->put_Filter(This,Var)
#define IADsContainer_get_Hints(This,pvFilter)  (This)->lpVtbl->get_Hints(This,pvFilter)
#define IADsContainer_put_Hints(This,vHints)  (This)->lpVtbl->put_Hints(This,vHints)
#define IADsContainer_GetObject(This,ClassName,RelativeName,ppObject)  (This)->lpVtbl->GetObject(This,ClassName,RelativeName,ppObject)
#define IADsContainer_Create(This,ClassName,RelativeName,ppObject)  (This)->lpVtbl->Create(This,ClassName,RelativeName,ppObject)
#define IADsContainer_Delete(This,bstrClassName,bstrRelativeName)  (This)->lpVtbl->Delete(This,bstrClassName,bstrRelativeName)
#define IADsContainer_CopyHere(This,SourceName,NewName,ppObject)  (This)->lpVtbl->CopyHere(This,SourceName,NewName,ppObject)
#define IADsContainer_MoveHere(This,SourceName,NewName,ppObject)  (This)->lpVtbl->MoveHere(This,SourceName,NewName,ppObject)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsContainer_get_Count_Proxy (IADsContainer * This, long *retval);
void __RPC_STUB IADsContainer_get_Count_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsContainer_get__NewEnum_Proxy (IADsContainer * This, IUnknown ** retval);
void __RPC_STUB IADsContainer_get__NewEnum_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsContainer_get_Filter_Proxy (IADsContainer * This, VARIANT * pVar);
void __RPC_STUB IADsContainer_get_Filter_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsContainer_put_Filter_Proxy (IADsContainer * This, VARIANT Var);
void __RPC_STUB IADsContainer_put_Filter_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsContainer_get_Hints_Proxy (IADsContainer * This, VARIANT * pvFilter);
void __RPC_STUB IADsContainer_get_Hints_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsContainer_put_Hints_Proxy (IADsContainer * This, VARIANT vHints);
void __RPC_STUB IADsContainer_put_Hints_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsContainer_GetObject_Proxy (IADsContainer * This, BSTR ClassName, BSTR RelativeName, IDispatch ** ppObject);
void __RPC_STUB IADsContainer_GetObject_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsContainer_Create_Proxy (IADsContainer * This, BSTR ClassName, BSTR RelativeName, IDispatch ** ppObject);
void __RPC_STUB IADsContainer_Create_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsContainer_Delete_Proxy (IADsContainer * This, BSTR bstrClassName, BSTR bstrRelativeName);
void __RPC_STUB IADsContainer_Delete_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsContainer_CopyHere_Proxy (IADsContainer * This, BSTR SourceName, BSTR NewName, IDispatch ** ppObject);
void __RPC_STUB IADsContainer_CopyHere_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsContainer_MoveHere_Proxy (IADsContainer * This, BSTR SourceName, BSTR NewName, IDispatch ** ppObject);
void __RPC_STUB IADsContainer_MoveHere_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsCollection_INTERFACE_DEFINED__
#define __IADsCollection_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsCollection;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("72b945e0-253b-11cf-a988-00aa006bc149") IADsCollection:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get__NewEnum (IUnknown ** ppEnumerator) = 0;
    virtual HRESULT STDMETHODCALLTYPE Add (BSTR bstrName, VARIANT vItem) = 0;
    virtual HRESULT STDMETHODCALLTYPE Remove (BSTR bstrItemToBeRemoved) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetObject (BSTR bstrName, VARIANT * pvItem) = 0;
};
#else
typedef struct IADsCollectionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsCollection * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsCollection * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsCollection * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsCollection * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsCollection * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsCollection * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsCollection * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get__NewEnum) (IADsCollection * This, IUnknown ** ppEnumerator);
    HRESULT (STDMETHODCALLTYPE * Add) (IADsCollection * This, BSTR bstrName, VARIANT vItem);
    HRESULT (STDMETHODCALLTYPE * Remove) (IADsCollection * This, BSTR bstrItemToBeRemoved);
    HRESULT (STDMETHODCALLTYPE * GetObject) (IADsCollection * This, BSTR bstrName, VARIANT * pvItem);
    END_INTERFACE
} IADsCollectionVtbl;
interface IADsCollection {
    CONST_VTBL struct IADsCollectionVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsCollection_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsCollection_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsCollection_Release(This)  (This)->lpVtbl->Release(This)
#define IADsCollection_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsCollection_get__NewEnum(This,ppEnumerator)  (This)->lpVtbl->get__NewEnum(This,ppEnumerator)
#define IADsCollection_Add(This,bstrName,vItem)  (This)->lpVtbl->Add(This,bstrName,vItem)
#define IADsCollection_Remove(This,bstrItemToBeRemoved)  (This)->lpVtbl->Remove(This,bstrItemToBeRemoved)
#define IADsCollection_GetObject(This,bstrName,pvItem)  (This)->lpVtbl->GetObject(This,bstrName,pvItem)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsCollection_get__NewEnum_Proxy (IADsCollection * This, IUnknown ** ppEnumerator);
void __RPC_STUB IADsCollection_get__NewEnum_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsCollection_Add_Proxy (IADsCollection * This, BSTR bstrName, VARIANT vItem);
void __RPC_STUB IADsCollection_Add_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsCollection_Remove_Proxy (IADsCollection * This, BSTR bstrItemToBeRemoved);
void __RPC_STUB IADsCollection_Remove_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsCollection_GetObject_Proxy (IADsCollection * This, BSTR bstrName, VARIANT * pvItem);
void __RPC_STUB IADsCollection_GetObject_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsMembers_INTERFACE_DEFINED__
#define __IADsMembers_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsMembers;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("451a0030-72ec-11cf-b03b-00aa006e0975") IADsMembers:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Count (long *plCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE get__NewEnum (IUnknown ** ppEnumerator) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Filter (VARIANT * pvFilter) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Filter (VARIANT pvFilter) = 0;
};
#else
typedef struct IADsMembersVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsMembers * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsMembers * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsMembers * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsMembers * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsMembers * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsMembers * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsMembers * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Count) (IADsMembers * This, long *plCount);
    HRESULT (STDMETHODCALLTYPE * get__NewEnum) (IADsMembers * This, IUnknown ** ppEnumerator);
    HRESULT (STDMETHODCALLTYPE * get_Filter) (IADsMembers * This, VARIANT * pvFilter);
    HRESULT (STDMETHODCALLTYPE * put_Filter) (IADsMembers * This, VARIANT pvFilter);
    END_INTERFACE
} IADsMembersVtbl;
interface IADsMembers {
    CONST_VTBL struct IADsMembersVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsMembers_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsMembers_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsMembers_Release(This)  (This)->lpVtbl->Release(This)
#define IADsMembers_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsMembers_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsMembers_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsMembers_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsMembers_get_Count(This,plCount)  (This)->lpVtbl->get_Count(This,plCount)
#define IADsMembers_get__NewEnum(This,ppEnumerator)  (This)->lpVtbl->get__NewEnum(This,ppEnumerator)
#define IADsMembers_get_Filter(This,pvFilter)  (This)->lpVtbl->get_Filter(This,pvFilter)
#define IADsMembers_put_Filter(This,pvFilter)  (This)->lpVtbl->put_Filter(This,pvFilter)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsMembers_get_Count_Proxy (IADsMembers * This, long *plCount);
void __RPC_STUB IADsMembers_get_Count_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsMembers_get__NewEnum_Proxy (IADsMembers * This, IUnknown ** ppEnumerator);
void __RPC_STUB IADsMembers_get__NewEnum_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsMembers_get_Filter_Proxy (IADsMembers * This, VARIANT * pvFilter);
void __RPC_STUB IADsMembers_get_Filter_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsMembers_put_Filter_Proxy (IADsMembers * This, VARIANT pvFilter);
void __RPC_STUB IADsMembers_put_Filter_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsPropertyList_INTERFACE_DEFINED__
#define __IADsPropertyList_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsPropertyList;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("c6f602b6-8f69-11d0-8528-00c04fd8d503") IADsPropertyList:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_PropertyCount (long *plCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE Next (VARIANT * pVariant) = 0;
    virtual HRESULT STDMETHODCALLTYPE Skip (long cElements) = 0;
    virtual HRESULT STDMETHODCALLTYPE Reset (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Item (VARIANT varIndex, VARIANT * pVariant) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPropertyItem (BSTR bstrName, LONG lnADsType, VARIANT * pVariant) = 0;
    virtual HRESULT STDMETHODCALLTYPE PutPropertyItem (VARIANT varData) = 0;
    virtual HRESULT STDMETHODCALLTYPE ResetPropertyItem (VARIANT varEntry) = 0;
    virtual HRESULT STDMETHODCALLTYPE PurgePropertyList (void) = 0;
};
#else
typedef struct IADsPropertyListVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsPropertyList * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsPropertyList * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsPropertyList * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsPropertyList * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsPropertyList * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsPropertyList * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsPropertyList * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_PropertyCount) (IADsPropertyList * This, long *plCount);
    HRESULT (STDMETHODCALLTYPE * Next) (IADsPropertyList * This, VARIANT * pVariant);
    HRESULT (STDMETHODCALLTYPE * Skip) (IADsPropertyList * This, long cElements);
    HRESULT (STDMETHODCALLTYPE * Reset) (IADsPropertyList * This);
    HRESULT (STDMETHODCALLTYPE * Item) (IADsPropertyList * This, VARIANT varIndex, VARIANT * pVariant);
    HRESULT (STDMETHODCALLTYPE * GetPropertyItem) (IADsPropertyList * This, BSTR bstrName, LONG lnADsType, VARIANT * pVariant);
    HRESULT (STDMETHODCALLTYPE * PutPropertyItem) (IADsPropertyList * This, VARIANT varData);
    HRESULT (STDMETHODCALLTYPE * ResetPropertyItem) (IADsPropertyList * This, VARIANT varEntry);
    HRESULT (STDMETHODCALLTYPE * PurgePropertyList) (IADsPropertyList * This);
    END_INTERFACE
} IADsPropertyListVtbl;
interface IADsPropertyList {
    CONST_VTBL struct IADsPropertyListVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsPropertyList_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsPropertyList_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsPropertyList_Release(This)  (This)->lpVtbl->Release(This)
#define IADsPropertyList_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsPropertyList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsPropertyList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsPropertyList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsPropertyList_get_PropertyCount(This,plCount)  (This)->lpVtbl->get_PropertyCount(This,plCount)
#define IADsPropertyList_Next(This,pVariant)  (This)->lpVtbl->Next(This,pVariant)
#define IADsPropertyList_Skip(This,cElements)  (This)->lpVtbl->Skip(This,cElements)
#define IADsPropertyList_Reset(This)  (This)->lpVtbl->Reset(This)
#define IADsPropertyList_Item(This,varIndex,pVariant)  (This)->lpVtbl->Item(This,varIndex,pVariant)
#define IADsPropertyList_GetPropertyItem(This,bstrName,lnADsType,pVariant)  (This)->lpVtbl->GetPropertyItem(This,bstrName,lnADsType,pVariant)
#define IADsPropertyList_PutPropertyItem(This,varData)  (This)->lpVtbl->PutPropertyItem(This,varData)
#define IADsPropertyList_ResetPropertyItem(This,varEntry)  (This)->lpVtbl->ResetPropertyItem(This,varEntry)
#define IADsPropertyList_PurgePropertyList(This)  (This)->lpVtbl->PurgePropertyList(This)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsPropertyList_get_PropertyCount_Proxy (IADsPropertyList * This, long *plCount);
void __RPC_STUB IADsPropertyList_get_PropertyCount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyList_Next_Proxy (IADsPropertyList * This, VARIANT * pVariant);
void __RPC_STUB IADsPropertyList_Next_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyList_Skip_Proxy (IADsPropertyList * This, long cElements);
void __RPC_STUB IADsPropertyList_Skip_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyList_Reset_Proxy (IADsPropertyList * This);
void __RPC_STUB IADsPropertyList_Reset_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyList_Item_Proxy (IADsPropertyList * This, VARIANT varIndex, VARIANT * pVariant);
void __RPC_STUB IADsPropertyList_Item_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyList_GetPropertyItem_Proxy (IADsPropertyList * This, BSTR bstrName, LONG lnADsType, VARIANT * pVariant);
void __RPC_STUB IADsPropertyList_GetPropertyItem_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyList_PutPropertyItem_Proxy (IADsPropertyList * This, VARIANT varData);
void __RPC_STUB IADsPropertyList_PutPropertyItem_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyList_ResetPropertyItem_Proxy (IADsPropertyList * This, VARIANT varEntry);
void __RPC_STUB IADsPropertyList_ResetPropertyItem_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyList_PurgePropertyList_Proxy (IADsPropertyList * This);
void __RPC_STUB IADsPropertyList_PurgePropertyList_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsPropertyEntry_INTERFACE_DEFINED__
#define __IADsPropertyEntry_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsPropertyEntry;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("05792c8e-941f-11d0-8529-00c04fd8d503") IADsPropertyEntry:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE Clear (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Name (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Name (BSTR bstrName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ADsType (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ADsType (long lnADsType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ControlCode (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ControlCode (long lnControlCode) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Values (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Values (VARIANT vValues) = 0;
};
#else
typedef struct IADsPropertyEntryVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsPropertyEntry * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsPropertyEntry * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsPropertyEntry * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsPropertyEntry * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsPropertyEntry * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsPropertyEntry * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsPropertyEntry * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * Clear) (IADsPropertyEntry * This);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsPropertyEntry * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Name) (IADsPropertyEntry * This, BSTR bstrName);
    HRESULT (STDMETHODCALLTYPE * get_ADsType) (IADsPropertyEntry * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_ADsType) (IADsPropertyEntry * This, long lnADsType);
    HRESULT (STDMETHODCALLTYPE * get_ControlCode) (IADsPropertyEntry * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_ControlCode) (IADsPropertyEntry * This, long lnControlCode);
    HRESULT (STDMETHODCALLTYPE * get_Values) (IADsPropertyEntry * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_Values) (IADsPropertyEntry * This, VARIANT vValues);
    END_INTERFACE
} IADsPropertyEntryVtbl;
interface IADsPropertyEntry {
    CONST_VTBL struct IADsPropertyEntryVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsPropertyEntry_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsPropertyEntry_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsPropertyEntry_Release(This)  (This)->lpVtbl->Release(This)
#define IADsPropertyEntry_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsPropertyEntry_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsPropertyEntry_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsPropertyEntry_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsPropertyEntry_Clear(This)  (This)->lpVtbl->Clear(This)
#define IADsPropertyEntry_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsPropertyEntry_put_Name(This,bstrName)  (This)->lpVtbl->put_Name(This,bstrName)
#define IADsPropertyEntry_get_ADsType(This,retval)  (This)->lpVtbl->get_ADsType(This,retval)
#define IADsPropertyEntry_put_ADsType(This,lnADsType)  (This)->lpVtbl->put_ADsType(This,lnADsType)
#define IADsPropertyEntry_get_ControlCode(This,retval)  (This)->lpVtbl->get_ControlCode(This,retval)
#define IADsPropertyEntry_put_ControlCode(This,lnControlCode)  (This)->lpVtbl->put_ControlCode(This,lnControlCode)
#define IADsPropertyEntry_get_Values(This,retval)  (This)->lpVtbl->get_Values(This,retval)
#define IADsPropertyEntry_put_Values(This,vValues)  (This)->lpVtbl->put_Values(This,vValues)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsPropertyEntry_Clear_Proxy (IADsPropertyEntry * This);
void __RPC_STUB IADsPropertyEntry_Clear_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyEntry_get_Name_Proxy (IADsPropertyEntry * This, BSTR * retval);
void __RPC_STUB IADsPropertyEntry_get_Name_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyEntry_put_Name_Proxy (IADsPropertyEntry * This, BSTR bstrName);
void __RPC_STUB IADsPropertyEntry_put_Name_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyEntry_get_ADsType_Proxy (IADsPropertyEntry * This, long *retval);
void __RPC_STUB IADsPropertyEntry_get_ADsType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyEntry_put_ADsType_Proxy (IADsPropertyEntry * This, long lnADsType);
void __RPC_STUB IADsPropertyEntry_put_ADsType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyEntry_get_ControlCode_Proxy (IADsPropertyEntry * This, long *retval);
void __RPC_STUB IADsPropertyEntry_get_ControlCode_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyEntry_put_ControlCode_Proxy (IADsPropertyEntry * This, long lnControlCode);
void __RPC_STUB IADsPropertyEntry_put_ControlCode_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyEntry_get_Values_Proxy (IADsPropertyEntry * This, VARIANT * retval);
void __RPC_STUB IADsPropertyEntry_get_Values_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyEntry_put_Values_Proxy (IADsPropertyEntry * This, VARIANT vValues);
void __RPC_STUB IADsPropertyEntry_put_Values_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_PropertyEntry;
#ifdef __cplusplus
class DECLSPEC_UUID ("72d3edc2-a4c4-11d0-8533-00c04fd8d503") PropertyEntry;
#endif

#ifndef __IADsPropertyValue_INTERFACE_DEFINED__
#define __IADsPropertyValue_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsPropertyValue;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("79fa9ad0-a97c-11d0-8534-00c04fd8d503") IADsPropertyValue:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE Clear (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ADsType (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ADsType (long lnADsType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DNString (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DNString (BSTR bstrDNString) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CaseExactString (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_CaseExactString (BSTR bstrCaseExactString) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CaseIgnoreString (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_CaseIgnoreString (BSTR bstrCaseIgnoreString) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PrintableString (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PrintableString (BSTR bstrPrintableString) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NumericString (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NumericString (BSTR bstrNumericString) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Boolean (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Boolean (long lnBoolean) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Integer (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Integer (long lnInteger) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OctetString (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_OctetString (VARIANT vOctetString) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SecurityDescriptor (IDispatch ** retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SecurityDescriptor (IDispatch * pSecurityDescriptor) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LargeInteger (IDispatch ** retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LargeInteger (IDispatch * pLargeInteger) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UTCTime (DATE * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_UTCTime (DATE daUTCTime) = 0;
};
#else
typedef struct IADsPropertyValueVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsPropertyValue * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsPropertyValue * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsPropertyValue * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsPropertyValue * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsPropertyValue * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsPropertyValue * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsPropertyValue * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * Clear) (IADsPropertyValue * This);
    HRESULT (STDMETHODCALLTYPE * get_ADsType) (IADsPropertyValue * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_ADsType) (IADsPropertyValue * This, long lnADsType);
    HRESULT (STDMETHODCALLTYPE * get_DNString) (IADsPropertyValue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_DNString) (IADsPropertyValue * This, BSTR bstrDNString);
    HRESULT (STDMETHODCALLTYPE * get_CaseExactString) (IADsPropertyValue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_CaseExactString) (IADsPropertyValue * This, BSTR bstrCaseExactString);
    HRESULT (STDMETHODCALLTYPE * get_CaseIgnoreString) (IADsPropertyValue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_CaseIgnoreString) (IADsPropertyValue * This, BSTR bstrCaseIgnoreString);
    HRESULT (STDMETHODCALLTYPE * get_PrintableString) (IADsPropertyValue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_PrintableString) (IADsPropertyValue * This, BSTR bstrPrintableString);
    HRESULT (STDMETHODCALLTYPE * get_NumericString) (IADsPropertyValue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_NumericString) (IADsPropertyValue * This, BSTR bstrNumericString);
    HRESULT (STDMETHODCALLTYPE * get_Boolean) (IADsPropertyValue * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Boolean) (IADsPropertyValue * This, long lnBoolean);
    HRESULT (STDMETHODCALLTYPE * get_Integer) (IADsPropertyValue * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Integer) (IADsPropertyValue * This, long lnInteger);
    HRESULT (STDMETHODCALLTYPE * get_OctetString) (IADsPropertyValue * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_OctetString) (IADsPropertyValue * This, VARIANT vOctetString);
    HRESULT (STDMETHODCALLTYPE * get_SecurityDescriptor) (IADsPropertyValue * This, IDispatch ** retval);
    HRESULT (STDMETHODCALLTYPE * put_SecurityDescriptor) (IADsPropertyValue * This, IDispatch * pSecurityDescriptor);
    HRESULT (STDMETHODCALLTYPE * get_LargeInteger) (IADsPropertyValue * This, IDispatch ** retval);
    HRESULT (STDMETHODCALLTYPE * put_LargeInteger) (IADsPropertyValue * This, IDispatch * pLargeInteger);
    HRESULT (STDMETHODCALLTYPE * get_UTCTime) (IADsPropertyValue * This, DATE * retval);
    HRESULT (STDMETHODCALLTYPE * put_UTCTime) (IADsPropertyValue * This, DATE daUTCTime);
    END_INTERFACE
} IADsPropertyValueVtbl;
interface IADsPropertyValue {
    CONST_VTBL struct IADsPropertyValueVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsPropertyValue_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsPropertyValue_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsPropertyValue_Release(This)  (This)->lpVtbl->Release(This)
#define IADsPropertyValue_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsPropertyValue_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsPropertyValue_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsPropertyValue_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsPropertyValue_Clear(This)  (This)->lpVtbl->Clear(This)
#define IADsPropertyValue_get_ADsType(This,retval)  (This)->lpVtbl->get_ADsType(This,retval)
#define IADsPropertyValue_put_ADsType(This,lnADsType)  (This)->lpVtbl->put_ADsType(This,lnADsType)
#define IADsPropertyValue_get_DNString(This,retval)  (This)->lpVtbl->get_DNString(This,retval)
#define IADsPropertyValue_put_DNString(This,bstrDNString)  (This)->lpVtbl->put_DNString(This,bstrDNString)
#define IADsPropertyValue_get_CaseExactString(This,retval)  (This)->lpVtbl->get_CaseExactString(This,retval)
#define IADsPropertyValue_put_CaseExactString(This,bstrCaseExactString)  (This)->lpVtbl->put_CaseExactString(This,bstrCaseExactString)
#define IADsPropertyValue_get_CaseIgnoreString(This,retval)  (This)->lpVtbl->get_CaseIgnoreString(This,retval)
#define IADsPropertyValue_put_CaseIgnoreString(This,bstrCaseIgnoreString)  (This)->lpVtbl->put_CaseIgnoreString(This,bstrCaseIgnoreString)
#define IADsPropertyValue_get_PrintableString(This,retval)  (This)->lpVtbl->get_PrintableString(This,retval)
#define IADsPropertyValue_put_PrintableString(This,bstrPrintableString)  (This)->lpVtbl->put_PrintableString(This,bstrPrintableString)
#define IADsPropertyValue_get_NumericString(This,retval)  (This)->lpVtbl->get_NumericString(This,retval)
#define IADsPropertyValue_put_NumericString(This,bstrNumericString)  (This)->lpVtbl->put_NumericString(This,bstrNumericString)
#define IADsPropertyValue_get_Boolean(This,retval)  (This)->lpVtbl->get_Boolean(This,retval)
#define IADsPropertyValue_put_Boolean(This,lnBoolean)  (This)->lpVtbl->put_Boolean(This,lnBoolean)
#define IADsPropertyValue_get_Integer(This,retval)  (This)->lpVtbl->get_Integer(This,retval)
#define IADsPropertyValue_put_Integer(This,lnInteger)  (This)->lpVtbl->put_Integer(This,lnInteger)
#define IADsPropertyValue_get_OctetString(This,retval)  (This)->lpVtbl->get_OctetString(This,retval)
#define IADsPropertyValue_put_OctetString(This,vOctetString)  (This)->lpVtbl->put_OctetString(This,vOctetString)
#define IADsPropertyValue_get_SecurityDescriptor(This,retval)  (This)->lpVtbl->get_SecurityDescriptor(This,retval)
#define IADsPropertyValue_put_SecurityDescriptor(This,pSecurityDescriptor)  (This)->lpVtbl->put_SecurityDescriptor(This,pSecurityDescriptor)
#define IADsPropertyValue_get_LargeInteger(This,retval)  (This)->lpVtbl->get_LargeInteger(This,retval)
#define IADsPropertyValue_put_LargeInteger(This,pLargeInteger)  (This)->lpVtbl->put_LargeInteger(This,pLargeInteger)
#define IADsPropertyValue_get_UTCTime(This,retval)  (This)->lpVtbl->get_UTCTime(This,retval)
#define IADsPropertyValue_put_UTCTime(This,daUTCTime)  (This)->lpVtbl->put_UTCTime(This,daUTCTime)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsPropertyValue_Clear_Proxy (IADsPropertyValue * This);
void __RPC_STUB IADsPropertyValue_Clear_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_get_ADsType_Proxy (IADsPropertyValue * This, long *retval);
void __RPC_STUB IADsPropertyValue_get_ADsType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_put_ADsType_Proxy (IADsPropertyValue * This, long lnADsType);
void __RPC_STUB IADsPropertyValue_put_ADsType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_get_DNString_Proxy (IADsPropertyValue * This, BSTR * retval);
void __RPC_STUB IADsPropertyValue_get_DNString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_put_DNString_Proxy (IADsPropertyValue * This, BSTR bstrDNString);
void __RPC_STUB IADsPropertyValue_put_DNString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_get_CaseExactString_Proxy (IADsPropertyValue * This, BSTR * retval);
void __RPC_STUB IADsPropertyValue_get_CaseExactString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_put_CaseExactString_Proxy (IADsPropertyValue * This, BSTR bstrCaseExactString);
void __RPC_STUB IADsPropertyValue_put_CaseExactString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_get_CaseIgnoreString_Proxy (IADsPropertyValue * This, BSTR * retval);
void __RPC_STUB IADsPropertyValue_get_CaseIgnoreString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_put_CaseIgnoreString_Proxy (IADsPropertyValue * This, BSTR bstrCaseIgnoreString);
void __RPC_STUB IADsPropertyValue_put_CaseIgnoreString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_get_PrintableString_Proxy (IADsPropertyValue * This, BSTR * retval);
void __RPC_STUB IADsPropertyValue_get_PrintableString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_put_PrintableString_Proxy (IADsPropertyValue * This, BSTR bstrPrintableString);
void __RPC_STUB IADsPropertyValue_put_PrintableString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_get_NumericString_Proxy (IADsPropertyValue * This, BSTR * retval);
void __RPC_STUB IADsPropertyValue_get_NumericString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_put_NumericString_Proxy (IADsPropertyValue * This, BSTR bstrNumericString);
void __RPC_STUB IADsPropertyValue_put_NumericString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_get_Boolean_Proxy (IADsPropertyValue * This, long *retval);
void __RPC_STUB IADsPropertyValue_get_Boolean_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_put_Boolean_Proxy (IADsPropertyValue * This, long lnBoolean);
void __RPC_STUB IADsPropertyValue_put_Boolean_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_get_Integer_Proxy (IADsPropertyValue * This, long *retval);
void __RPC_STUB IADsPropertyValue_get_Integer_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_put_Integer_Proxy (IADsPropertyValue * This, long lnInteger);
void __RPC_STUB IADsPropertyValue_put_Integer_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_get_OctetString_Proxy (IADsPropertyValue * This, VARIANT * retval);
void __RPC_STUB IADsPropertyValue_get_OctetString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_put_OctetString_Proxy (IADsPropertyValue * This, VARIANT vOctetString);
void __RPC_STUB IADsPropertyValue_put_OctetString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_get_SecurityDescriptor_Proxy (IADsPropertyValue * This, IDispatch ** retval);
void __RPC_STUB IADsPropertyValue_get_SecurityDescriptor_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_put_SecurityDescriptor_Proxy (IADsPropertyValue * This, IDispatch * pSecurityDescriptor);
void __RPC_STUB IADsPropertyValue_put_SecurityDescriptor_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_get_LargeInteger_Proxy (IADsPropertyValue * This, IDispatch ** retval);
void __RPC_STUB IADsPropertyValue_get_LargeInteger_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_put_LargeInteger_Proxy (IADsPropertyValue * This, IDispatch * pLargeInteger);
void __RPC_STUB IADsPropertyValue_put_LargeInteger_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_get_UTCTime_Proxy (IADsPropertyValue * This, DATE * retval);
void __RPC_STUB IADsPropertyValue_get_UTCTime_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue_put_UTCTime_Proxy (IADsPropertyValue * This, DATE daUTCTime);
void __RPC_STUB IADsPropertyValue_put_UTCTime_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsPropertyValue2_INTERFACE_DEFINED__
#define __IADsPropertyValue2_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsPropertyValue2;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("306e831c-5bc7-11d1-a3b8-00c04fb950dc") IADsPropertyValue2:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetObjectProperty (long *lnADsType, VARIANT * pvProp) = 0;
    virtual HRESULT STDMETHODCALLTYPE PutObjectProperty (long lnADsType, VARIANT vProp) = 0;
};
#else
typedef struct IADsPropertyValue2Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsPropertyValue2 * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsPropertyValue2 * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsPropertyValue2 * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsPropertyValue2 * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsPropertyValue2 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsPropertyValue2 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsPropertyValue2 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * GetObjectProperty) (IADsPropertyValue2 * This, long *lnADsType, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutObjectProperty) (IADsPropertyValue2 * This, long lnADsType, VARIANT vProp);
    END_INTERFACE
} IADsPropertyValue2Vtbl;
interface IADsPropertyValue2 {
    CONST_VTBL struct IADsPropertyValue2Vtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsPropertyValue2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsPropertyValue2_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsPropertyValue2_Release(This)  (This)->lpVtbl->Release(This)
#define IADsPropertyValue2_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsPropertyValue2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsPropertyValue2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsPropertyValue2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsPropertyValue2_GetObjectProperty(This,lnADsType,pvProp)  (This)->lpVtbl->GetObjectProperty(This,lnADsType,pvProp)
#define IADsPropertyValue2_PutObjectProperty(This,lnADsType,vProp)  (This)->lpVtbl->PutObjectProperty(This,lnADsType,vProp)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsPropertyValue2_GetObjectProperty_Proxy (IADsPropertyValue2 * This, long *lnADsType, VARIANT * pvProp);
void __RPC_STUB IADsPropertyValue2_GetObjectProperty_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPropertyValue2_PutObjectProperty_Proxy (IADsPropertyValue2 * This, long lnADsType, VARIANT vProp);
void __RPC_STUB IADsPropertyValue2_PutObjectProperty_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_PropertyValue;
#ifdef __cplusplus
class DECLSPEC_UUID ("7b9e38b0-a97c-11d0-8534-00c04fd8d503") PropertyValue;
#endif

#ifndef __IPrivateDispatch_INTERFACE_DEFINED__
#define __IPrivateDispatch_INTERFACE_DEFINED__
EXTERN_C const IID IID_IPrivateDispatch;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("86ab4bbe-65f6-11d1-8c13-00c04fd8d503") IPrivateDispatch:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE ADSIInitializeDispatchManager (long dwExtensionId) = 0;
    virtual HRESULT STDMETHODCALLTYPE ADSIGetTypeInfoCount (UINT * pctinfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE ADSIGetTypeInfo (UINT itinfo, LCID lcid, ITypeInfo ** pptinfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE ADSIGetIDsOfNames (REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgdispid) = 0;
    virtual HRESULT STDMETHODCALLTYPE ADSIInvoke (DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT * pvarResult, EXCEPINFO * pexcepinfo, UINT * puArgErr) = 0;
};
#else
typedef struct IPrivateDispatchVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IPrivateDispatch * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IPrivateDispatch * This);
    ULONG (STDMETHODCALLTYPE * Release) (IPrivateDispatch * This);
    HRESULT (STDMETHODCALLTYPE * ADSIInitializeDispatchManager) (IPrivateDispatch * This, long dwExtensionId);
    HRESULT (STDMETHODCALLTYPE * ADSIGetTypeInfoCount) (IPrivateDispatch * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * ADSIGetTypeInfo) (IPrivateDispatch * This, UINT itinfo, LCID lcid, ITypeInfo ** pptinfo);
    HRESULT (STDMETHODCALLTYPE * ADSIGetIDsOfNames) (IPrivateDispatch * This, REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgdispid);
    HRESULT (STDMETHODCALLTYPE * ADSIInvoke) (IPrivateDispatch * This, DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT * pvarResult, EXCEPINFO * pexcepinfo, UINT * puArgErr);
    END_INTERFACE
} IPrivateDispatchVtbl;
interface IPrivateDispatch {
    CONST_VTBL struct IPrivateDispatchVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IPrivateDispatch_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IPrivateDispatch_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IPrivateDispatch_Release(This)  (This)->lpVtbl->Release(This)
#define IPrivateDispatch_ADSIInitializeDispatchManager(This,dwExtensionId)  (This)->lpVtbl->ADSIInitializeDispatchManager(This,dwExtensionId)
#define IPrivateDispatch_ADSIGetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->ADSIGetTypeInfoCount(This,pctinfo)
#define IPrivateDispatch_ADSIGetTypeInfo(This,itinfo,lcid,pptinfo)  (This)->lpVtbl->ADSIGetTypeInfo(This,itinfo,lcid,pptinfo)
#define IPrivateDispatch_ADSIGetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgdispid)  (This)->lpVtbl->ADSIGetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgdispid)
#define IPrivateDispatch_ADSIInvoke(This,dispidMember,riid,lcid,wFlags,pdispparams,pvarResult,pexcepinfo,puArgErr)  (This)->lpVtbl->ADSIInvoke(This,dispidMember,riid,lcid,wFlags,pdispparams,pvarResult,pexcepinfo,puArgErr)
#endif
#endif
HRESULT STDMETHODCALLTYPE IPrivateDispatch_ADSIInitializeDispatchManager_Proxy (IPrivateDispatch * This, long dwExtensionId);
void __RPC_STUB IPrivateDispatch_ADSIInitializeDispatchManager_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IPrivateDispatch_ADSIGetTypeInfoCount_Proxy (IPrivateDispatch * This, UINT * pctinfo);
void __RPC_STUB IPrivateDispatch_ADSIGetTypeInfoCount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IPrivateDispatch_ADSIGetTypeInfo_Proxy (IPrivateDispatch * This, UINT itinfo, LCID lcid, ITypeInfo ** pptinfo);
void __RPC_STUB IPrivateDispatch_ADSIGetTypeInfo_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IPrivateDispatch_ADSIGetIDsOfNames_Proxy (IPrivateDispatch * This, REFIID riid, OLECHAR ** rgszNames, UINT cNames, LCID lcid, DISPID * rgdispid);
void __RPC_STUB IPrivateDispatch_ADSIGetIDsOfNames_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IPrivateDispatch_ADSIInvoke_Proxy (IPrivateDispatch * This, DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT * pvarResult, EXCEPINFO * pexcepinfo, UINT * puArgErr);
void __RPC_STUB IPrivateDispatch_ADSIInvoke_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IPrivateUnknown_INTERFACE_DEFINED__
#define __IPrivateUnknown_INTERFACE_DEFINED__
EXTERN_C const IID IID_IPrivateUnknown;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("89126bab-6ead-11d1-8c18-00c04fd8d503") IPrivateUnknown:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE ADSIInitializeObject (BSTR lpszUserName, BSTR lpszPassword, long lnReserved) = 0;
    virtual HRESULT STDMETHODCALLTYPE ADSIReleaseObject (void) = 0;
};
#else
typedef struct IPrivateUnknownVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IPrivateUnknown * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IPrivateUnknown * This);
    ULONG (STDMETHODCALLTYPE * Release) (IPrivateUnknown * This);
    HRESULT (STDMETHODCALLTYPE * ADSIInitializeObject) (IPrivateUnknown * This, BSTR lpszUserName, BSTR lpszPassword, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * ADSIReleaseObject) (IPrivateUnknown * This);
    END_INTERFACE
} IPrivateUnknownVtbl;
interface IPrivateUnknown {
    CONST_VTBL struct IPrivateUnknownVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IPrivateUnknown_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IPrivateUnknown_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IPrivateUnknown_Release(This)  (This)->lpVtbl->Release(This)
#define IPrivateUnknown_ADSIInitializeObject(This,lpszUserName,lpszPassword,lnReserved)  (This)->lpVtbl->ADSIInitializeObject(This,lpszUserName,lpszPassword,lnReserved)
#define IPrivateUnknown_ADSIReleaseObject(This)  (This)->lpVtbl->ADSIReleaseObject(This)
#endif
#endif
HRESULT STDMETHODCALLTYPE IPrivateUnknown_ADSIInitializeObject_Proxy (IPrivateUnknown * This, BSTR lpszUserName, BSTR lpszPassword, long lnReserved);
void __RPC_STUB IPrivateUnknown_ADSIInitializeObject_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IPrivateUnknown_ADSIReleaseObject_Proxy (IPrivateUnknown * This);
void __RPC_STUB IPrivateUnknown_ADSIReleaseObject_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsExtension_INTERFACE_DEFINED__
#define __IADsExtension_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsExtension;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("3d35553c-d2b0-11d1-b17b-0000f87593a0") IADsExtension:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE Operate (DWORD dwCode, VARIANT varData1, VARIANT varData2, VARIANT varData3) = 0;
    virtual HRESULT STDMETHODCALLTYPE PrivateGetIDsOfNames (REFIID riid, OLECHAR ** rgszNames, unsigned int cNames, LCID lcid, DISPID * rgDispid) = 0;
    virtual HRESULT STDMETHODCALLTYPE PrivateInvoke (DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT * pvarResult, EXCEPINFO * pexcepinfo, unsigned int *puArgErr) = 0;
};
#else
typedef struct IADsExtensionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsExtension * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsExtension * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsExtension * This);
    HRESULT (STDMETHODCALLTYPE * Operate) (IADsExtension * This, DWORD dwCode, VARIANT varData1, VARIANT varData2, VARIANT varData3);
    HRESULT (STDMETHODCALLTYPE * PrivateGetIDsOfNames) (IADsExtension * This, REFIID riid, OLECHAR ** rgszNames, unsigned int cNames, LCID lcid, DISPID * rgDispid);
    HRESULT (STDMETHODCALLTYPE * PrivateInvoke) (IADsExtension * This, DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT * pvarResult, EXCEPINFO * pexcepinfo, unsigned int *puArgErr);
    END_INTERFACE
} IADsExtensionVtbl;
interface IADsExtension {
    CONST_VTBL struct IADsExtensionVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsExtension_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsExtension_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsExtension_Release(This)  (This)->lpVtbl->Release(This)
#define IADsExtension_Operate(This,dwCode,varData1,varData2,varData3)  (This)->lpVtbl->Operate(This,dwCode,varData1,varData2,varData3)
#define IADsExtension_PrivateGetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispid)  (This)->lpVtbl->PrivateGetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispid)
#define IADsExtension_PrivateInvoke(This,dispidMember,riid,lcid,wFlags,pdispparams,pvarResult,pexcepinfo,puArgErr)  (This)->lpVtbl->PrivateInvoke(This,dispidMember,riid,lcid,wFlags,pdispparams,pvarResult,pexcepinfo,puArgErr)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsExtension_Operate_Proxy (IADsExtension * This, DWORD dwCode, VARIANT varData1, VARIANT varData2, VARIANT varData3);
void __RPC_STUB IADsExtension_Operate_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsExtension_PrivateGetIDsOfNames_Proxy (IADsExtension * This, REFIID riid, OLECHAR ** rgszNames, unsigned int cNames, LCID lcid, DISPID * rgDispid);
void __RPC_STUB IADsExtension_PrivateGetIDsOfNames_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsExtension_PrivateInvoke_Proxy (IADsExtension * This, DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pdispparams, VARIANT * pvarResult, EXCEPINFO * pexcepinfo, unsigned int *puArgErr);
void __RPC_STUB IADsExtension_PrivateInvoke_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsDeleteOps_INTERFACE_DEFINED__
#define __IADsDeleteOps_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsDeleteOps;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("b2bd0902-8878-11d1-8c21-00c04fd8d503") IADsDeleteOps:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE DeleteObject (long lnFlags) = 0;
};
#else
typedef struct IADsDeleteOpsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsDeleteOps * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsDeleteOps * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsDeleteOps * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsDeleteOps * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsDeleteOps * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsDeleteOps * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsDeleteOps * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * DeleteObject) (IADsDeleteOps * This, long lnFlags);
    END_INTERFACE
} IADsDeleteOpsVtbl;
interface IADsDeleteOps {
    CONST_VTBL struct IADsDeleteOpsVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsDeleteOps_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsDeleteOps_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsDeleteOps_Release(This)  (This)->lpVtbl->Release(This)
#define IADsDeleteOps_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsDeleteOps_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsDeleteOps_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsDeleteOps_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsDeleteOps_DeleteObject(This,lnFlags)  (This)->lpVtbl->DeleteObject(This,lnFlags)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsDeleteOps_DeleteObject_Proxy (IADsDeleteOps * This, long lnFlags);
void __RPC_STUB IADsDeleteOps_DeleteObject_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsNamespaces_INTERFACE_DEFINED__
#define __IADsNamespaces_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsNamespaces;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("28b96ba0-b330-11cf-a9ad-00aa006bc149") IADsNamespaces:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_DefaultContainer (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DefaultContainer (BSTR bstrDefaultContainer) = 0;
};
#else
typedef struct IADsNamespacesVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsNamespaces * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsNamespaces * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsNamespaces * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsNamespaces * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsNamespaces * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsNamespaces * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsNamespaces * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsNamespaces * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsNamespaces * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsNamespaces * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsNamespaces * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsNamespaces * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsNamespaces * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsNamespaces * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsNamespaces * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsNamespaces * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsNamespaces * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsNamespaces * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsNamespaces * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsNamespaces * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_DefaultContainer) (IADsNamespaces * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_DefaultContainer) (IADsNamespaces * This, BSTR bstrDefaultContainer);
    END_INTERFACE
} IADsNamespacesVtbl;
interface IADsNamespaces {
    CONST_VTBL struct IADsNamespacesVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsNamespaces_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsNamespaces_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsNamespaces_Release(This)  (This)->lpVtbl->Release(This)
#define IADsNamespaces_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsNamespaces_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsNamespaces_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsNamespaces_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsNamespaces_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsNamespaces_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsNamespaces_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsNamespaces_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsNamespaces_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsNamespaces_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsNamespaces_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsNamespaces_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsNamespaces_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsNamespaces_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsNamespaces_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsNamespaces_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsNamespaces_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsNamespaces_get_DefaultContainer(This,retval)  (This)->lpVtbl->get_DefaultContainer(This,retval)
#define IADsNamespaces_put_DefaultContainer(This,bstrDefaultContainer)  (This)->lpVtbl->put_DefaultContainer(This,bstrDefaultContainer)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsNamespaces_get_DefaultContainer_Proxy (IADsNamespaces * This, BSTR * retval);
void __RPC_STUB IADsNamespaces_get_DefaultContainer_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsNamespaces_put_DefaultContainer_Proxy (IADsNamespaces * This, BSTR bstrDefaultContainer);
void __RPC_STUB IADsNamespaces_put_DefaultContainer_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsClass_INTERFACE_DEFINED__
#define __IADsClass_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsClass;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("c8f93dd0-4ae0-11cf-9e73-00aa004a5691") IADsClass:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_PrimaryInterface (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CLSID (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_CLSID (BSTR bstrCLSID) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OID (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_OID (BSTR bstrOID) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Abstract (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Abstract (VARIANT_BOOL fAbstract) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Auxiliary (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Auxiliary (VARIANT_BOOL fAuxiliary) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MandatoryProperties (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MandatoryProperties (VARIANT vMandatoryProperties) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OptionalProperties (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_OptionalProperties (VARIANT vOptionalProperties) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NamingProperties (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NamingProperties (VARIANT vNamingProperties) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DerivedFrom (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DerivedFrom (VARIANT vDerivedFrom) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AuxDerivedFrom (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AuxDerivedFrom (VARIANT vAuxDerivedFrom) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PossibleSuperiors (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PossibleSuperiors (VARIANT vPossibleSuperiors) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Containment (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Containment (VARIANT vContainment) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Container (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Container (VARIANT_BOOL fContainer) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HelpFileName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HelpFileName (BSTR bstrHelpFileName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HelpFileContext (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HelpFileContext (long lnHelpFileContext) = 0;
    virtual HRESULT STDMETHODCALLTYPE Qualifiers (IADsCollection ** ppQualifiers) = 0;
};
#else
typedef struct IADsClassVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsClass * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsClass * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsClass * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsClass * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsClass * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsClass * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsClass * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsClass * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsClass * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsClass * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsClass * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsClass * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsClass * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsClass * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsClass * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsClass * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsClass * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsClass * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsClass * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsClass * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_PrimaryInterface) (IADsClass * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_CLSID) (IADsClass * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_CLSID) (IADsClass * This, BSTR bstrCLSID);
    HRESULT (STDMETHODCALLTYPE * get_OID) (IADsClass * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_OID) (IADsClass * This, BSTR bstrOID);
    HRESULT (STDMETHODCALLTYPE * get_Abstract) (IADsClass * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * put_Abstract) (IADsClass * This, VARIANT_BOOL fAbstract);
    HRESULT (STDMETHODCALLTYPE * get_Auxiliary) (IADsClass * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * put_Auxiliary) (IADsClass * This, VARIANT_BOOL fAuxiliary);
    HRESULT (STDMETHODCALLTYPE * get_MandatoryProperties) (IADsClass * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_MandatoryProperties) (IADsClass * This, VARIANT vMandatoryProperties);
    HRESULT (STDMETHODCALLTYPE * get_OptionalProperties) (IADsClass * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_OptionalProperties) (IADsClass * This, VARIANT vOptionalProperties);
    HRESULT (STDMETHODCALLTYPE * get_NamingProperties) (IADsClass * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_NamingProperties) (IADsClass * This, VARIANT vNamingProperties);
    HRESULT (STDMETHODCALLTYPE * get_DerivedFrom) (IADsClass * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_DerivedFrom) (IADsClass * This, VARIANT vDerivedFrom);
    HRESULT (STDMETHODCALLTYPE * get_AuxDerivedFrom) (IADsClass * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_AuxDerivedFrom) (IADsClass * This, VARIANT vAuxDerivedFrom);
    HRESULT (STDMETHODCALLTYPE * get_PossibleSuperiors) (IADsClass * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_PossibleSuperiors) (IADsClass * This, VARIANT vPossibleSuperiors);
    HRESULT (STDMETHODCALLTYPE * get_Containment) (IADsClass * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_Containment) (IADsClass * This, VARIANT vContainment);
    HRESULT (STDMETHODCALLTYPE * get_Container) (IADsClass * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * put_Container) (IADsClass * This, VARIANT_BOOL fContainer);
    HRESULT (STDMETHODCALLTYPE * get_HelpFileName) (IADsClass * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_HelpFileName) (IADsClass * This, BSTR bstrHelpFileName);
    HRESULT (STDMETHODCALLTYPE * get_HelpFileContext) (IADsClass * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_HelpFileContext) (IADsClass * This, long lnHelpFileContext);
    HRESULT (STDMETHODCALLTYPE * Qualifiers) (IADsClass * This, IADsCollection ** ppQualifiers);
    END_INTERFACE
} IADsClassVtbl;
interface IADsClass {
    CONST_VTBL struct IADsClassVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsClass_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsClass_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsClass_Release(This)  (This)->lpVtbl->Release(This)
#define IADsClass_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsClass_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsClass_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsClass_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsClass_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsClass_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsClass_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsClass_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsClass_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsClass_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsClass_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsClass_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsClass_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsClass_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsClass_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsClass_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsClass_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsClass_get_PrimaryInterface(This,retval)  (This)->lpVtbl->get_PrimaryInterface(This,retval)
#define IADsClass_get_CLSID(This,retval)  (This)->lpVtbl->get_CLSID(This,retval)
#define IADsClass_put_CLSID(This,bstrCLSID)  (This)->lpVtbl->put_CLSID(This,bstrCLSID)
#define IADsClass_get_OID(This,retval)  (This)->lpVtbl->get_OID(This,retval)
#define IADsClass_put_OID(This,bstrOID)  (This)->lpVtbl->put_OID(This,bstrOID)
#define IADsClass_get_Abstract(This,retval)  (This)->lpVtbl->get_Abstract(This,retval)
#define IADsClass_put_Abstract(This,fAbstract)  (This)->lpVtbl->put_Abstract(This,fAbstract)
#define IADsClass_get_Auxiliary(This,retval)  (This)->lpVtbl->get_Auxiliary(This,retval)
#define IADsClass_put_Auxiliary(This,fAuxiliary)  (This)->lpVtbl->put_Auxiliary(This,fAuxiliary)
#define IADsClass_get_MandatoryProperties(This,retval)  (This)->lpVtbl->get_MandatoryProperties(This,retval)
#define IADsClass_put_MandatoryProperties(This,vMandatoryProperties)  (This)->lpVtbl->put_MandatoryProperties(This,vMandatoryProperties)
#define IADsClass_get_OptionalProperties(This,retval)  (This)->lpVtbl->get_OptionalProperties(This,retval)
#define IADsClass_put_OptionalProperties(This,vOptionalProperties)  (This)->lpVtbl->put_OptionalProperties(This,vOptionalProperties)
#define IADsClass_get_NamingProperties(This,retval)  (This)->lpVtbl->get_NamingProperties(This,retval)
#define IADsClass_put_NamingProperties(This,vNamingProperties)  (This)->lpVtbl->put_NamingProperties(This,vNamingProperties)
#define IADsClass_get_DerivedFrom(This,retval)  (This)->lpVtbl->get_DerivedFrom(This,retval)
#define IADsClass_put_DerivedFrom(This,vDerivedFrom)  (This)->lpVtbl->put_DerivedFrom(This,vDerivedFrom)
#define IADsClass_get_AuxDerivedFrom(This,retval)  (This)->lpVtbl->get_AuxDerivedFrom(This,retval)
#define IADsClass_put_AuxDerivedFrom(This,vAuxDerivedFrom)  (This)->lpVtbl->put_AuxDerivedFrom(This,vAuxDerivedFrom)
#define IADsClass_get_PossibleSuperiors(This,retval)  (This)->lpVtbl->get_PossibleSuperiors(This,retval)
#define IADsClass_put_PossibleSuperiors(This,vPossibleSuperiors)  (This)->lpVtbl->put_PossibleSuperiors(This,vPossibleSuperiors)
#define IADsClass_get_Containment(This,retval)  (This)->lpVtbl->get_Containment(This,retval)
#define IADsClass_put_Containment(This,vContainment)  (This)->lpVtbl->put_Containment(This,vContainment)
#define IADsClass_get_Container(This,retval)  (This)->lpVtbl->get_Container(This,retval)
#define IADsClass_put_Container(This,fContainer)  (This)->lpVtbl->put_Container(This,fContainer)
#define IADsClass_get_HelpFileName(This,retval)  (This)->lpVtbl->get_HelpFileName(This,retval)
#define IADsClass_put_HelpFileName(This,bstrHelpFileName)  (This)->lpVtbl->put_HelpFileName(This,bstrHelpFileName)
#define IADsClass_get_HelpFileContext(This,retval)  (This)->lpVtbl->get_HelpFileContext(This,retval)
#define IADsClass_put_HelpFileContext(This,lnHelpFileContext)  (This)->lpVtbl->put_HelpFileContext(This,lnHelpFileContext)
#define IADsClass_Qualifiers(This,ppQualifiers)  (This)->lpVtbl->Qualifiers(This,ppQualifiers)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsClass_get_PrimaryInterface_Proxy (IADsClass * This, BSTR * retval);
void __RPC_STUB IADsClass_get_PrimaryInterface_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_CLSID_Proxy (IADsClass * This, BSTR * retval);
void __RPC_STUB IADsClass_get_CLSID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_CLSID_Proxy (IADsClass * This, BSTR bstrCLSID);
void __RPC_STUB IADsClass_put_CLSID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_OID_Proxy (IADsClass * This, BSTR * retval);
void __RPC_STUB IADsClass_get_OID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_OID_Proxy (IADsClass * This, BSTR bstrOID);
void __RPC_STUB IADsClass_put_OID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_Abstract_Proxy (IADsClass * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsClass_get_Abstract_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_Abstract_Proxy (IADsClass * This, VARIANT_BOOL fAbstract);
void __RPC_STUB IADsClass_put_Abstract_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_Auxiliary_Proxy (IADsClass * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsClass_get_Auxiliary_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_Auxiliary_Proxy (IADsClass * This, VARIANT_BOOL fAuxiliary);
void __RPC_STUB IADsClass_put_Auxiliary_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_MandatoryProperties_Proxy (IADsClass * This, VARIANT * retval);
void __RPC_STUB IADsClass_get_MandatoryProperties_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_MandatoryProperties_Proxy (IADsClass * This, VARIANT vMandatoryProperties);
void __RPC_STUB IADsClass_put_MandatoryProperties_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_OptionalProperties_Proxy (IADsClass * This, VARIANT * retval);
void __RPC_STUB IADsClass_get_OptionalProperties_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_OptionalProperties_Proxy (IADsClass * This, VARIANT vOptionalProperties);
void __RPC_STUB IADsClass_put_OptionalProperties_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_NamingProperties_Proxy (IADsClass * This, VARIANT * retval);
void __RPC_STUB IADsClass_get_NamingProperties_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_NamingProperties_Proxy (IADsClass * This, VARIANT vNamingProperties);
void __RPC_STUB IADsClass_put_NamingProperties_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_DerivedFrom_Proxy (IADsClass * This, VARIANT * retval);
void __RPC_STUB IADsClass_get_DerivedFrom_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_DerivedFrom_Proxy (IADsClass * This, VARIANT vDerivedFrom);
void __RPC_STUB IADsClass_put_DerivedFrom_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_AuxDerivedFrom_Proxy (IADsClass * This, VARIANT * retval);
void __RPC_STUB IADsClass_get_AuxDerivedFrom_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_AuxDerivedFrom_Proxy (IADsClass * This, VARIANT vAuxDerivedFrom);
void __RPC_STUB IADsClass_put_AuxDerivedFrom_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_PossibleSuperiors_Proxy (IADsClass * This, VARIANT * retval);
void __RPC_STUB IADsClass_get_PossibleSuperiors_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_PossibleSuperiors_Proxy (IADsClass * This, VARIANT vPossibleSuperiors);
void __RPC_STUB IADsClass_put_PossibleSuperiors_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_Containment_Proxy (IADsClass * This, VARIANT * retval);
void __RPC_STUB IADsClass_get_Containment_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_Containment_Proxy (IADsClass * This, VARIANT vContainment);
void __RPC_STUB IADsClass_put_Containment_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_Container_Proxy (IADsClass * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsClass_get_Container_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_Container_Proxy (IADsClass * This, VARIANT_BOOL fContainer);
void __RPC_STUB IADsClass_put_Container_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_HelpFileName_Proxy (IADsClass * This, BSTR * retval);
void __RPC_STUB IADsClass_get_HelpFileName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_HelpFileName_Proxy (IADsClass * This, BSTR bstrHelpFileName);
void __RPC_STUB IADsClass_put_HelpFileName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_get_HelpFileContext_Proxy (IADsClass * This, long *retval);
void __RPC_STUB IADsClass_get_HelpFileContext_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_put_HelpFileContext_Proxy (IADsClass * This, long lnHelpFileContext);
void __RPC_STUB IADsClass_put_HelpFileContext_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsClass_Qualifiers_Proxy (IADsClass * This, IADsCollection ** ppQualifiers);
void __RPC_STUB IADsClass_Qualifiers_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsProperty_INTERFACE_DEFINED__
#define __IADsProperty_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsProperty;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("c8f93dd3-4ae0-11cf-9e73-00aa004a5691") IADsProperty:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_OID (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_OID (BSTR bstrOID) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Syntax (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Syntax (BSTR bstrSyntax) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MaxRange (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MaxRange (long lnMaxRange) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MinRange (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MinRange (long lnMinRange) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MultiValued (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MultiValued (VARIANT_BOOL fMultiValued) = 0;
    virtual HRESULT STDMETHODCALLTYPE Qualifiers (IADsCollection ** ppQualifiers) = 0;
};
#else
typedef struct IADsPropertyVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsProperty * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsProperty * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsProperty * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsProperty * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsProperty * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsProperty * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsProperty * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsProperty * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsProperty * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsProperty * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsProperty * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsProperty * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsProperty * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsProperty * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsProperty * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsProperty * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsProperty * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsProperty * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsProperty * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsProperty * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_OID) (IADsProperty * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_OID) (IADsProperty * This, BSTR bstrOID);
    HRESULT (STDMETHODCALLTYPE * get_Syntax) (IADsProperty * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Syntax) (IADsProperty * This, BSTR bstrSyntax);
    HRESULT (STDMETHODCALLTYPE * get_MaxRange) (IADsProperty * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_MaxRange) (IADsProperty * This, long lnMaxRange);
    HRESULT (STDMETHODCALLTYPE * get_MinRange) (IADsProperty * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_MinRange) (IADsProperty * This, long lnMinRange);
    HRESULT (STDMETHODCALLTYPE * get_MultiValued) (IADsProperty * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * put_MultiValued) (IADsProperty * This, VARIANT_BOOL fMultiValued);
    HRESULT (STDMETHODCALLTYPE * Qualifiers) (IADsProperty * This, IADsCollection ** ppQualifiers);
    END_INTERFACE
} IADsPropertyVtbl;
interface IADsProperty {
    CONST_VTBL struct IADsPropertyVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsProperty_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsProperty_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsProperty_Release(This)  (This)->lpVtbl->Release(This)
#define IADsProperty_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsProperty_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsProperty_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsProperty_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsProperty_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsProperty_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsProperty_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsProperty_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsProperty_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsProperty_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsProperty_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsProperty_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsProperty_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsProperty_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsProperty_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsProperty_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsProperty_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsProperty_get_OID(This,retval)  (This)->lpVtbl->get_OID(This,retval)
#define IADsProperty_put_OID(This,bstrOID)  (This)->lpVtbl->put_OID(This,bstrOID)
#define IADsProperty_get_Syntax(This,retval)  (This)->lpVtbl->get_Syntax(This,retval)
#define IADsProperty_put_Syntax(This,bstrSyntax)  (This)->lpVtbl->put_Syntax(This,bstrSyntax)
#define IADsProperty_get_MaxRange(This,retval)  (This)->lpVtbl->get_MaxRange(This,retval)
#define IADsProperty_put_MaxRange(This,lnMaxRange)  (This)->lpVtbl->put_MaxRange(This,lnMaxRange)
#define IADsProperty_get_MinRange(This,retval)  (This)->lpVtbl->get_MinRange(This,retval)
#define IADsProperty_put_MinRange(This,lnMinRange)  (This)->lpVtbl->put_MinRange(This,lnMinRange)
#define IADsProperty_get_MultiValued(This,retval)  (This)->lpVtbl->get_MultiValued(This,retval)
#define IADsProperty_put_MultiValued(This,fMultiValued)  (This)->lpVtbl->put_MultiValued(This,fMultiValued)
#define IADsProperty_Qualifiers(This,ppQualifiers)  (This)->lpVtbl->Qualifiers(This,ppQualifiers)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsProperty_get_OID_Proxy (IADsProperty * This, BSTR * retval);
void __RPC_STUB IADsProperty_get_OID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsProperty_put_OID_Proxy (IADsProperty * This, BSTR bstrOID);
void __RPC_STUB IADsProperty_put_OID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsProperty_get_Syntax_Proxy (IADsProperty * This, BSTR * retval);
void __RPC_STUB IADsProperty_get_Syntax_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsProperty_put_Syntax_Proxy (IADsProperty * This, BSTR bstrSyntax);
void __RPC_STUB IADsProperty_put_Syntax_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsProperty_get_MaxRange_Proxy (IADsProperty * This, long *retval);
void __RPC_STUB IADsProperty_get_MaxRange_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsProperty_put_MaxRange_Proxy (IADsProperty * This, long lnMaxRange);
void __RPC_STUB IADsProperty_put_MaxRange_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsProperty_get_MinRange_Proxy (IADsProperty * This, long *retval);
void __RPC_STUB IADsProperty_get_MinRange_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsProperty_put_MinRange_Proxy (IADsProperty * This, long lnMinRange);
void __RPC_STUB IADsProperty_put_MinRange_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsProperty_get_MultiValued_Proxy (IADsProperty * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsProperty_get_MultiValued_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsProperty_put_MultiValued_Proxy (IADsProperty * This, VARIANT_BOOL fMultiValued);
void __RPC_STUB IADsProperty_put_MultiValued_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsProperty_Qualifiers_Proxy (IADsProperty * This, IADsCollection ** ppQualifiers);
void __RPC_STUB IADsProperty_Qualifiers_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsSyntax_INTERFACE_DEFINED__
#define __IADsSyntax_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsSyntax;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("c8f93dd2-4ae0-11cf-9e73-00aa004a5691") IADsSyntax:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_OleAutoDataType (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_OleAutoDataType (long lnOleAutoDataType) = 0;
};
#else
typedef struct IADsSyntaxVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsSyntax * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsSyntax * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsSyntax * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsSyntax * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsSyntax * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsSyntax * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsSyntax * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsSyntax * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsSyntax * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsSyntax * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsSyntax * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsSyntax * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsSyntax * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsSyntax * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsSyntax * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsSyntax * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsSyntax * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsSyntax * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsSyntax * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsSyntax * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_OleAutoDataType) (IADsSyntax * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_OleAutoDataType) (IADsSyntax * This, long lnOleAutoDataType);
    END_INTERFACE
} IADsSyntaxVtbl;
interface IADsSyntax {
    CONST_VTBL struct IADsSyntaxVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsSyntax_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsSyntax_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsSyntax_Release(This)  (This)->lpVtbl->Release(This)
#define IADsSyntax_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsSyntax_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsSyntax_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsSyntax_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsSyntax_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsSyntax_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsSyntax_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsSyntax_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsSyntax_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsSyntax_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsSyntax_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsSyntax_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsSyntax_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsSyntax_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsSyntax_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsSyntax_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsSyntax_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsSyntax_get_OleAutoDataType(This,retval)  (This)->lpVtbl->get_OleAutoDataType(This,retval)
#define IADsSyntax_put_OleAutoDataType(This,lnOleAutoDataType)  (This)->lpVtbl->put_OleAutoDataType(This,lnOleAutoDataType)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsSyntax_get_OleAutoDataType_Proxy (IADsSyntax * This, long *retval);
void __RPC_STUB IADsSyntax_get_OleAutoDataType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSyntax_put_OleAutoDataType_Proxy (IADsSyntax * This, long lnOleAutoDataType);
void __RPC_STUB IADsSyntax_put_OleAutoDataType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsLocality_INTERFACE_DEFINED__
#define __IADsLocality_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsLocality;
#if defined(__cplusplus) && !defined(CINTERFACE)
 MIDL_INTERFACE ("a05e03a2-effe-11cf-8abc-00c04fd8d503") IADsLocality:public IADs {
  public:
    virtual HRESULT STDMETHODCALLTYPE get_Description (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Description (BSTR bstrDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LocalityName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LocalityName (BSTR bstrLocalityName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PostalAddress (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PostalAddress (BSTR bstrPostalAddress) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SeeAlso (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SeeAlso (VARIANT vSeeAlso) = 0;
};
#else
typedef struct IADsLocalityVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsLocality * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsLocality * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsLocality * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsLocality * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsLocality * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsLocality * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsLocality * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsLocality * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsLocality * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsLocality * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsLocality * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsLocality * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsLocality * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsLocality * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsLocality * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsLocality * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsLocality * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsLocality * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsLocality * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsLocality * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_Description) (IADsLocality * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Description) (IADsLocality * This, BSTR bstrDescription);
    HRESULT (STDMETHODCALLTYPE * get_LocalityName) (IADsLocality * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_LocalityName) (IADsLocality * This, BSTR bstrLocalityName);
    HRESULT (STDMETHODCALLTYPE * get_PostalAddress) (IADsLocality * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_PostalAddress) (IADsLocality * This, BSTR bstrPostalAddress);
    HRESULT (STDMETHODCALLTYPE * get_SeeAlso) (IADsLocality * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_SeeAlso) (IADsLocality * This, VARIANT vSeeAlso);
    END_INTERFACE
} IADsLocalityVtbl;
interface IADsLocality {
    CONST_VTBL struct IADsLocalityVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsLocality_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsLocality_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsLocality_Release(This)  (This)->lpVtbl->Release(This)
#define IADsLocality_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsLocality_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsLocality_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsLocality_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsLocality_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsLocality_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsLocality_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsLocality_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsLocality_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsLocality_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsLocality_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsLocality_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsLocality_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsLocality_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsLocality_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsLocality_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsLocality_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsLocality_get_Description(This,retval)  (This)->lpVtbl->get_Description(This,retval)
#define IADsLocality_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IADsLocality_get_LocalityName(This,retval)  (This)->lpVtbl->get_LocalityName(This,retval)
#define IADsLocality_put_LocalityName(This,bstrLocalityName)  (This)->lpVtbl->put_LocalityName(This,bstrLocalityName)
#define IADsLocality_get_PostalAddress(This,retval)  (This)->lpVtbl->get_PostalAddress(This,retval)
#define IADsLocality_put_PostalAddress(This,bstrPostalAddress)  (This)->lpVtbl->put_PostalAddress(This,bstrPostalAddress)
#define IADsLocality_get_SeeAlso(This,retval)  (This)->lpVtbl->get_SeeAlso(This,retval)
#define IADsLocality_put_SeeAlso(This,vSeeAlso)  (This)->lpVtbl->put_SeeAlso(This,vSeeAlso)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsLocality_get_Description_Proxy (IADsLocality * This, BSTR * retval);
void __RPC_STUB IADsLocality_get_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsLocality_put_Description_Proxy (IADsLocality * This, BSTR bstrDescription);
void __RPC_STUB IADsLocality_put_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsLocality_get_LocalityName_Proxy (IADsLocality * This, BSTR * retval);
void __RPC_STUB IADsLocality_get_LocalityName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsLocality_put_LocalityName_Proxy (IADsLocality * This, BSTR bstrLocalityName);
void __RPC_STUB IADsLocality_put_LocalityName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsLocality_get_PostalAddress_Proxy (IADsLocality * This, BSTR * retval);
void __RPC_STUB IADsLocality_get_PostalAddress_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsLocality_put_PostalAddress_Proxy (IADsLocality * This, BSTR bstrPostalAddress);
void __RPC_STUB IADsLocality_put_PostalAddress_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsLocality_get_SeeAlso_Proxy (IADsLocality * This, VARIANT * retval);
void __RPC_STUB IADsLocality_get_SeeAlso_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsLocality_put_SeeAlso_Proxy (IADsLocality * This, VARIANT vSeeAlso);
void __RPC_STUB IADsLocality_put_SeeAlso_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsO_INTERFACE_DEFINED__
#define __IADsO_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsO;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("a1cd2dc6-effe-11cf-8abc-00c04fd8d503") IADsO:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Description (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Description (BSTR bstrDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LocalityName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LocalityName (BSTR bstrLocalityName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PostalAddress (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PostalAddress (BSTR bstrPostalAddress) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TelephoneNumber (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_TelephoneNumber (BSTR bstrTelephoneNumber) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_FaxNumber (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_FaxNumber (BSTR bstrFaxNumber) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SeeAlso (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SeeAlso (VARIANT vSeeAlso) = 0;
};
#else
typedef struct IADsOVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsO * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsO * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsO * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsO * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsO * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsO * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsO * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsO * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsO * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsO * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsO * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsO * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsO * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsO * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsO * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsO * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsO * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsO * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsO * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsO * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_Description) (IADsO * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Description) (IADsO * This, BSTR bstrDescription);
    HRESULT (STDMETHODCALLTYPE * get_LocalityName) (IADsO * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_LocalityName) (IADsO * This, BSTR bstrLocalityName);
    HRESULT (STDMETHODCALLTYPE * get_PostalAddress) (IADsO * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_PostalAddress) (IADsO * This, BSTR bstrPostalAddress);
    HRESULT (STDMETHODCALLTYPE * get_TelephoneNumber) (IADsO * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_TelephoneNumber) (IADsO * This, BSTR bstrTelephoneNumber);
    HRESULT (STDMETHODCALLTYPE * get_FaxNumber) (IADsO * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_FaxNumber) (IADsO * This, BSTR bstrFaxNumber);
    HRESULT (STDMETHODCALLTYPE * get_SeeAlso) (IADsO * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_SeeAlso) (IADsO * This, VARIANT vSeeAlso);
    END_INTERFACE
} IADsOVtbl;
interface IADsO {
    CONST_VTBL struct IADsOVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsO_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsO_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsO_Release(This)  (This)->lpVtbl->Release(This)
#define IADsO_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsO_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsO_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsO_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsO_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsO_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsO_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsO_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsO_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsO_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsO_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsO_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsO_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsO_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsO_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsO_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsO_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsO_get_Description(This,retval)  (This)->lpVtbl->get_Description(This,retval)
#define IADsO_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IADsO_get_LocalityName(This,retval)  (This)->lpVtbl->get_LocalityName(This,retval)
#define IADsO_put_LocalityName(This,bstrLocalityName)  (This)->lpVtbl->put_LocalityName(This,bstrLocalityName)
#define IADsO_get_PostalAddress(This,retval)  (This)->lpVtbl->get_PostalAddress(This,retval)
#define IADsO_put_PostalAddress(This,bstrPostalAddress)  (This)->lpVtbl->put_PostalAddress(This,bstrPostalAddress)
#define IADsO_get_TelephoneNumber(This,retval)  (This)->lpVtbl->get_TelephoneNumber(This,retval)
#define IADsO_put_TelephoneNumber(This,bstrTelephoneNumber)  (This)->lpVtbl->put_TelephoneNumber(This,bstrTelephoneNumber)
#define IADsO_get_FaxNumber(This,retval)  (This)->lpVtbl->get_FaxNumber(This,retval)
#define IADsO_put_FaxNumber(This,bstrFaxNumber)  (This)->lpVtbl->put_FaxNumber(This,bstrFaxNumber)
#define IADsO_get_SeeAlso(This,retval)  (This)->lpVtbl->get_SeeAlso(This,retval)
#define IADsO_put_SeeAlso(This,vSeeAlso)  (This)->lpVtbl->put_SeeAlso(This,vSeeAlso)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsO_get_Description_Proxy (IADsO * This, BSTR * retval);
void __RPC_STUB IADsO_get_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsO_put_Description_Proxy (IADsO * This, BSTR bstrDescription);
void __RPC_STUB IADsO_put_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsO_get_LocalityName_Proxy (IADsO * This, BSTR * retval);
void __RPC_STUB IADsO_get_LocalityName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsO_put_LocalityName_Proxy (IADsO * This, BSTR bstrLocalityName);
void __RPC_STUB IADsO_put_LocalityName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsO_get_PostalAddress_Proxy (IADsO * This, BSTR * retval);
void __RPC_STUB IADsO_get_PostalAddress_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsO_put_PostalAddress_Proxy (IADsO * This, BSTR bstrPostalAddress);
void __RPC_STUB IADsO_put_PostalAddress_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsO_get_TelephoneNumber_Proxy (IADsO * This, BSTR * retval);
void __RPC_STUB IADsO_get_TelephoneNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsO_put_TelephoneNumber_Proxy (IADsO * This, BSTR bstrTelephoneNumber);
void __RPC_STUB IADsO_put_TelephoneNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsO_get_FaxNumber_Proxy (IADsO * This, BSTR * retval);
void __RPC_STUB IADsO_get_FaxNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsO_put_FaxNumber_Proxy (IADsO * This, BSTR bstrFaxNumber);
void __RPC_STUB IADsO_put_FaxNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsO_get_SeeAlso_Proxy (IADsO * This, VARIANT * retval);
void __RPC_STUB IADsO_get_SeeAlso_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsO_put_SeeAlso_Proxy (IADsO * This, VARIANT vSeeAlso);
void __RPC_STUB IADsO_put_SeeAlso_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsOU_INTERFACE_DEFINED__
#define __IADsOU_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsOU;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("a2f733b8-effe-11cf-8abc-00c04fd8d503") IADsOU:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Description (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Description (BSTR bstrDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LocalityName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LocalityName (BSTR bstrLocalityName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PostalAddress (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PostalAddress (BSTR bstrPostalAddress) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TelephoneNumber (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_TelephoneNumber (BSTR bstrTelephoneNumber) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_FaxNumber (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_FaxNumber (BSTR bstrFaxNumber) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SeeAlso (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SeeAlso (VARIANT vSeeAlso) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BusinessCategory (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_BusinessCategory (BSTR bstrBusinessCategory) = 0;
};
#else
typedef struct IADsOUVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsOU * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsOU * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsOU * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsOU * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsOU * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsOU * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsOU * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsOU * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsOU * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsOU * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsOU * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsOU * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsOU * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsOU * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsOU * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsOU * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsOU * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsOU * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsOU * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsOU * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_Description) (IADsOU * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Description) (IADsOU * This, BSTR bstrDescription);
    HRESULT (STDMETHODCALLTYPE * get_LocalityName) (IADsOU * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_LocalityName) (IADsOU * This, BSTR bstrLocalityName);
    HRESULT (STDMETHODCALLTYPE * get_PostalAddress) (IADsOU * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_PostalAddress) (IADsOU * This, BSTR bstrPostalAddress);
    HRESULT (STDMETHODCALLTYPE * get_TelephoneNumber) (IADsOU * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_TelephoneNumber) (IADsOU * This, BSTR bstrTelephoneNumber);
    HRESULT (STDMETHODCALLTYPE * get_FaxNumber) (IADsOU * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_FaxNumber) (IADsOU * This, BSTR bstrFaxNumber);
    HRESULT (STDMETHODCALLTYPE * get_SeeAlso) (IADsOU * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_SeeAlso) (IADsOU * This, VARIANT vSeeAlso);
    HRESULT (STDMETHODCALLTYPE * get_BusinessCategory) (IADsOU * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_BusinessCategory) (IADsOU * This, BSTR bstrBusinessCategory);
    END_INTERFACE
} IADsOUVtbl;
interface IADsOU {
    CONST_VTBL struct IADsOUVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsOU_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsOU_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsOU_Release(This)  (This)->lpVtbl->Release(This)
#define IADsOU_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsOU_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsOU_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsOU_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsOU_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsOU_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsOU_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsOU_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsOU_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsOU_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsOU_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsOU_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsOU_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsOU_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsOU_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsOU_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsOU_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsOU_get_Description(This,retval)  (This)->lpVtbl->get_Description(This,retval)
#define IADsOU_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IADsOU_get_LocalityName(This,retval)  (This)->lpVtbl->get_LocalityName(This,retval)
#define IADsOU_put_LocalityName(This,bstrLocalityName)  (This)->lpVtbl->put_LocalityName(This,bstrLocalityName)
#define IADsOU_get_PostalAddress(This,retval)  (This)->lpVtbl->get_PostalAddress(This,retval)
#define IADsOU_put_PostalAddress(This,bstrPostalAddress)  (This)->lpVtbl->put_PostalAddress(This,bstrPostalAddress)
#define IADsOU_get_TelephoneNumber(This,retval)  (This)->lpVtbl->get_TelephoneNumber(This,retval)
#define IADsOU_put_TelephoneNumber(This,bstrTelephoneNumber)  (This)->lpVtbl->put_TelephoneNumber(This,bstrTelephoneNumber)
#define IADsOU_get_FaxNumber(This,retval)  (This)->lpVtbl->get_FaxNumber(This,retval)
#define IADsOU_put_FaxNumber(This,bstrFaxNumber)  (This)->lpVtbl->put_FaxNumber(This,bstrFaxNumber)
#define IADsOU_get_SeeAlso(This,retval)  (This)->lpVtbl->get_SeeAlso(This,retval)
#define IADsOU_put_SeeAlso(This,vSeeAlso)  (This)->lpVtbl->put_SeeAlso(This,vSeeAlso)
#define IADsOU_get_BusinessCategory(This,retval)  (This)->lpVtbl->get_BusinessCategory(This,retval)
#define IADsOU_put_BusinessCategory(This,bstrBusinessCategory)  (This)->lpVtbl->put_BusinessCategory(This,bstrBusinessCategory)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsOU_get_Description_Proxy (IADsOU * This, BSTR * retval);
void __RPC_STUB IADsOU_get_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOU_put_Description_Proxy (IADsOU * This, BSTR bstrDescription);
void __RPC_STUB IADsOU_put_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOU_get_LocalityName_Proxy (IADsOU * This, BSTR * retval);
void __RPC_STUB IADsOU_get_LocalityName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOU_put_LocalityName_Proxy (IADsOU * This, BSTR bstrLocalityName);
void __RPC_STUB IADsOU_put_LocalityName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOU_get_PostalAddress_Proxy (IADsOU * This, BSTR * retval);
void __RPC_STUB IADsOU_get_PostalAddress_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOU_put_PostalAddress_Proxy (IADsOU * This, BSTR bstrPostalAddress);
void __RPC_STUB IADsOU_put_PostalAddress_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOU_get_TelephoneNumber_Proxy (IADsOU * This, BSTR * retval);
void __RPC_STUB IADsOU_get_TelephoneNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOU_put_TelephoneNumber_Proxy (IADsOU * This, BSTR bstrTelephoneNumber);
void __RPC_STUB IADsOU_put_TelephoneNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOU_get_FaxNumber_Proxy (IADsOU * This, BSTR * retval);
void __RPC_STUB IADsOU_get_FaxNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOU_put_FaxNumber_Proxy (IADsOU * This, BSTR bstrFaxNumber);
void __RPC_STUB IADsOU_put_FaxNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOU_get_SeeAlso_Proxy (IADsOU * This, VARIANT * retval);
void __RPC_STUB IADsOU_get_SeeAlso_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOU_put_SeeAlso_Proxy (IADsOU * This, VARIANT vSeeAlso);
void __RPC_STUB IADsOU_put_SeeAlso_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOU_get_BusinessCategory_Proxy (IADsOU * This, BSTR * retval);
void __RPC_STUB IADsOU_get_BusinessCategory_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOU_put_BusinessCategory_Proxy (IADsOU * This, BSTR bstrBusinessCategory);
void __RPC_STUB IADsOU_put_BusinessCategory_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsDomain_INTERFACE_DEFINED__
#define __IADsDomain_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsDomain;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("00e4c220-fd16-11ce-abc4-02608c9e7553") IADsDomain:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_IsWorkgroup (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MinPasswordLength (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MinPasswordLength (long lnMinPasswordLength) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MinPasswordAge (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MinPasswordAge (long lnMinPasswordAge) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MaxPasswordAge (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MaxPasswordAge (long lnMaxPasswordAge) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MaxBadPasswordsAllowed (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MaxBadPasswordsAllowed (long lnMaxBadPasswordsAllowed) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PasswordHistoryLength (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PasswordHistoryLength (long lnPasswordHistoryLength) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PasswordAttributes (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PasswordAttributes (long lnPasswordAttributes) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AutoUnlockInterval (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AutoUnlockInterval (long lnAutoUnlockInterval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LockoutObservationInterval (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LockoutObservationInterval (long lnLockoutObservationInterval) = 0;
};
#else
typedef struct IADsDomainVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsDomain * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsDomain * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsDomain * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsDomain * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsDomain * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsDomain * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsDomain * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsDomain * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsDomain * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsDomain * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsDomain * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsDomain * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsDomain * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsDomain * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsDomain * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsDomain * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsDomain * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsDomain * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsDomain * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsDomain * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_IsWorkgroup) (IADsDomain * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * get_MinPasswordLength) (IADsDomain * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_MinPasswordLength) (IADsDomain * This, long lnMinPasswordLength);
    HRESULT (STDMETHODCALLTYPE * get_MinPasswordAge) (IADsDomain * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_MinPasswordAge) (IADsDomain * This, long lnMinPasswordAge);
    HRESULT (STDMETHODCALLTYPE * get_MaxPasswordAge) (IADsDomain * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_MaxPasswordAge) (IADsDomain * This, long lnMaxPasswordAge);
    HRESULT (STDMETHODCALLTYPE * get_MaxBadPasswordsAllowed) (IADsDomain * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_MaxBadPasswordsAllowed) (IADsDomain * This, long lnMaxBadPasswordsAllowed);
    HRESULT (STDMETHODCALLTYPE * get_PasswordHistoryLength) (IADsDomain * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_PasswordHistoryLength) (IADsDomain * This, long lnPasswordHistoryLength);
    HRESULT (STDMETHODCALLTYPE * get_PasswordAttributes) (IADsDomain * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_PasswordAttributes) (IADsDomain * This, long lnPasswordAttributes);
    HRESULT (STDMETHODCALLTYPE * get_AutoUnlockInterval) (IADsDomain * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_AutoUnlockInterval) (IADsDomain * This, long lnAutoUnlockInterval);
    HRESULT (STDMETHODCALLTYPE * get_LockoutObservationInterval) (IADsDomain * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_LockoutObservationInterval) (IADsDomain * This, long lnLockoutObservationInterval);
    END_INTERFACE
} IADsDomainVtbl;
interface IADsDomain {
    CONST_VTBL struct IADsDomainVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsDomain_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsDomain_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsDomain_Release(This)  (This)->lpVtbl->Release(This)
#define IADsDomain_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsDomain_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsDomain_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsDomain_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsDomain_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsDomain_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsDomain_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsDomain_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsDomain_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsDomain_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsDomain_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsDomain_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsDomain_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsDomain_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsDomain_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsDomain_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsDomain_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsDomain_get_IsWorkgroup(This,retval)  (This)->lpVtbl->get_IsWorkgroup(This,retval)
#define IADsDomain_get_MinPasswordLength(This,retval)  (This)->lpVtbl->get_MinPasswordLength(This,retval)
#define IADsDomain_put_MinPasswordLength(This,lnMinPasswordLength)  (This)->lpVtbl->put_MinPasswordLength(This,lnMinPasswordLength)
#define IADsDomain_get_MinPasswordAge(This,retval)  (This)->lpVtbl->get_MinPasswordAge(This,retval)
#define IADsDomain_put_MinPasswordAge(This,lnMinPasswordAge)  (This)->lpVtbl->put_MinPasswordAge(This,lnMinPasswordAge)
#define IADsDomain_get_MaxPasswordAge(This,retval)  (This)->lpVtbl->get_MaxPasswordAge(This,retval)
#define IADsDomain_put_MaxPasswordAge(This,lnMaxPasswordAge)  (This)->lpVtbl->put_MaxPasswordAge(This,lnMaxPasswordAge)
#define IADsDomain_get_MaxBadPasswordsAllowed(This,retval)  (This)->lpVtbl->get_MaxBadPasswordsAllowed(This,retval)
#define IADsDomain_put_MaxBadPasswordsAllowed(This,lnMaxBadPasswordsAllowed)  (This)->lpVtbl->put_MaxBadPasswordsAllowed(This,lnMaxBadPasswordsAllowed)
#define IADsDomain_get_PasswordHistoryLength(This,retval)  (This)->lpVtbl->get_PasswordHistoryLength(This,retval)
#define IADsDomain_put_PasswordHistoryLength(This,lnPasswordHistoryLength)  (This)->lpVtbl->put_PasswordHistoryLength(This,lnPasswordHistoryLength)
#define IADsDomain_get_PasswordAttributes(This,retval)  (This)->lpVtbl->get_PasswordAttributes(This,retval)
#define IADsDomain_put_PasswordAttributes(This,lnPasswordAttributes)  (This)->lpVtbl->put_PasswordAttributes(This,lnPasswordAttributes)
#define IADsDomain_get_AutoUnlockInterval(This,retval)  (This)->lpVtbl->get_AutoUnlockInterval(This,retval)
#define IADsDomain_put_AutoUnlockInterval(This,lnAutoUnlockInterval)  (This)->lpVtbl->put_AutoUnlockInterval(This,lnAutoUnlockInterval)
#define IADsDomain_get_LockoutObservationInterval(This,retval)  (This)->lpVtbl->get_LockoutObservationInterval(This,retval)
#define IADsDomain_put_LockoutObservationInterval(This,lnLockoutObservationInterval)  (This)->lpVtbl->put_LockoutObservationInterval(This,lnLockoutObservationInterval)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsDomain_get_IsWorkgroup_Proxy (IADsDomain * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsDomain_get_IsWorkgroup_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_get_MinPasswordLength_Proxy (IADsDomain * This, long *retval);
void __RPC_STUB IADsDomain_get_MinPasswordLength_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_put_MinPasswordLength_Proxy (IADsDomain * This, long lnMinPasswordLength);
void __RPC_STUB IADsDomain_put_MinPasswordLength_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_get_MinPasswordAge_Proxy (IADsDomain * This, long *retval);
void __RPC_STUB IADsDomain_get_MinPasswordAge_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_put_MinPasswordAge_Proxy (IADsDomain * This, long lnMinPasswordAge);
void __RPC_STUB IADsDomain_put_MinPasswordAge_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_get_MaxPasswordAge_Proxy (IADsDomain * This, long *retval);
void __RPC_STUB IADsDomain_get_MaxPasswordAge_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_put_MaxPasswordAge_Proxy (IADsDomain * This, long lnMaxPasswordAge);
void __RPC_STUB IADsDomain_put_MaxPasswordAge_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_get_MaxBadPasswordsAllowed_Proxy (IADsDomain * This, long *retval);
void __RPC_STUB IADsDomain_get_MaxBadPasswordsAllowed_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_put_MaxBadPasswordsAllowed_Proxy (IADsDomain * This, long lnMaxBadPasswordsAllowed);
void __RPC_STUB IADsDomain_put_MaxBadPasswordsAllowed_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_get_PasswordHistoryLength_Proxy (IADsDomain * This, long *retval);
void __RPC_STUB IADsDomain_get_PasswordHistoryLength_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_put_PasswordHistoryLength_Proxy (IADsDomain * This, long lnPasswordHistoryLength);
void __RPC_STUB IADsDomain_put_PasswordHistoryLength_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_get_PasswordAttributes_Proxy (IADsDomain * This, long *retval);
void __RPC_STUB IADsDomain_get_PasswordAttributes_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_put_PasswordAttributes_Proxy (IADsDomain * This, long lnPasswordAttributes);
void __RPC_STUB IADsDomain_put_PasswordAttributes_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_get_AutoUnlockInterval_Proxy (IADsDomain * This, long *retval);
void __RPC_STUB IADsDomain_get_AutoUnlockInterval_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_put_AutoUnlockInterval_Proxy (IADsDomain * This, long lnAutoUnlockInterval);
void __RPC_STUB IADsDomain_put_AutoUnlockInterval_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_get_LockoutObservationInterval_Proxy (IADsDomain * This, long *retval);
void __RPC_STUB IADsDomain_get_LockoutObservationInterval_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDomain_put_LockoutObservationInterval_Proxy (IADsDomain * This, long lnLockoutObservationInterval);
void __RPC_STUB IADsDomain_put_LockoutObservationInterval_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsComputer_INTERFACE_DEFINED__
#define __IADsComputer_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsComputer;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("efe3cc70-1d9f-11cf-b1f3-02608c9e7553") IADsComputer:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_ComputerID (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Site (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Description (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Description (BSTR bstrDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Location (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Location (BSTR bstrLocation) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PrimaryUser (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PrimaryUser (BSTR bstrPrimaryUser) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Owner (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Owner (BSTR bstrOwner) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Division (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Division (BSTR bstrDivision) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Department (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Department (BSTR bstrDepartment) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Role (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Role (BSTR bstrRole) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OperatingSystem (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_OperatingSystem (BSTR bstrOperatingSystem) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OperatingSystemVersion (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_OperatingSystemVersion (BSTR bstrOperatingSystemVersion) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Model (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Model (BSTR bstrModel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Processor (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Processor (BSTR bstrProcessor) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ProcessorCount (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ProcessorCount (BSTR bstrProcessorCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MemorySize (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MemorySize (BSTR bstrMemorySize) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StorageCapacity (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StorageCapacity (BSTR bstrStorageCapacity) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NetAddresses (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NetAddresses (VARIANT vNetAddresses) = 0;
};
#else
typedef struct IADsComputerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsComputer * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsComputer * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsComputer * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsComputer * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsComputer * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsComputer * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsComputer * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsComputer * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsComputer * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsComputer * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsComputer * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsComputer * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsComputer * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsComputer * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_ComputerID) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Site) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Description) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Description) (IADsComputer * This, BSTR bstrDescription);
    HRESULT (STDMETHODCALLTYPE * get_Location) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Location) (IADsComputer * This, BSTR bstrLocation);
    HRESULT (STDMETHODCALLTYPE * get_PrimaryUser) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_PrimaryUser) (IADsComputer * This, BSTR bstrPrimaryUser);
    HRESULT (STDMETHODCALLTYPE * get_Owner) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Owner) (IADsComputer * This, BSTR bstrOwner);
    HRESULT (STDMETHODCALLTYPE * get_Division) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Division) (IADsComputer * This, BSTR bstrDivision);
    HRESULT (STDMETHODCALLTYPE * get_Department) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Department) (IADsComputer * This, BSTR bstrDepartment);
    HRESULT (STDMETHODCALLTYPE * get_Role) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Role) (IADsComputer * This, BSTR bstrRole);
    HRESULT (STDMETHODCALLTYPE * get_OperatingSystem) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_OperatingSystem) (IADsComputer * This, BSTR bstrOperatingSystem);
    HRESULT (STDMETHODCALLTYPE * get_OperatingSystemVersion) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_OperatingSystemVersion) (IADsComputer * This, BSTR bstrOperatingSystemVersion);
    HRESULT (STDMETHODCALLTYPE * get_Model) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Model) (IADsComputer * This, BSTR bstrModel);
    HRESULT (STDMETHODCALLTYPE * get_Processor) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Processor) (IADsComputer * This, BSTR bstrProcessor);
    HRESULT (STDMETHODCALLTYPE * get_ProcessorCount) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_ProcessorCount) (IADsComputer * This, BSTR bstrProcessorCount);
    HRESULT (STDMETHODCALLTYPE * get_MemorySize) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_MemorySize) (IADsComputer * This, BSTR bstrMemorySize);
    HRESULT (STDMETHODCALLTYPE * get_StorageCapacity) (IADsComputer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_StorageCapacity) (IADsComputer * This, BSTR bstrStorageCapacity);
    HRESULT (STDMETHODCALLTYPE * get_NetAddresses) (IADsComputer * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_NetAddresses) (IADsComputer * This, VARIANT vNetAddresses);
    END_INTERFACE
} IADsComputerVtbl;
interface IADsComputer {
    CONST_VTBL struct IADsComputerVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsComputer_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsComputer_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsComputer_Release(This)  (This)->lpVtbl->Release(This)
#define IADsComputer_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsComputer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsComputer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsComputer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsComputer_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsComputer_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsComputer_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsComputer_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsComputer_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsComputer_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsComputer_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsComputer_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsComputer_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsComputer_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsComputer_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsComputer_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsComputer_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsComputer_get_ComputerID(This,retval)  (This)->lpVtbl->get_ComputerID(This,retval)
#define IADsComputer_get_Site(This,retval)  (This)->lpVtbl->get_Site(This,retval)
#define IADsComputer_get_Description(This,retval)  (This)->lpVtbl->get_Description(This,retval)
#define IADsComputer_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IADsComputer_get_Location(This,retval)  (This)->lpVtbl->get_Location(This,retval)
#define IADsComputer_put_Location(This,bstrLocation)  (This)->lpVtbl->put_Location(This,bstrLocation)
#define IADsComputer_get_PrimaryUser(This,retval)  (This)->lpVtbl->get_PrimaryUser(This,retval)
#define IADsComputer_put_PrimaryUser(This,bstrPrimaryUser)  (This)->lpVtbl->put_PrimaryUser(This,bstrPrimaryUser)
#define IADsComputer_get_Owner(This,retval)  (This)->lpVtbl->get_Owner(This,retval)
#define IADsComputer_put_Owner(This,bstrOwner)  (This)->lpVtbl->put_Owner(This,bstrOwner)
#define IADsComputer_get_Division(This,retval)  (This)->lpVtbl->get_Division(This,retval)
#define IADsComputer_put_Division(This,bstrDivision)  (This)->lpVtbl->put_Division(This,bstrDivision)
#define IADsComputer_get_Department(This,retval)  (This)->lpVtbl->get_Department(This,retval)
#define IADsComputer_put_Department(This,bstrDepartment)  (This)->lpVtbl->put_Department(This,bstrDepartment)
#define IADsComputer_get_Role(This,retval)  (This)->lpVtbl->get_Role(This,retval)
#define IADsComputer_put_Role(This,bstrRole)  (This)->lpVtbl->put_Role(This,bstrRole)
#define IADsComputer_get_OperatingSystem(This,retval)  (This)->lpVtbl->get_OperatingSystem(This,retval)
#define IADsComputer_put_OperatingSystem(This,bstrOperatingSystem)  (This)->lpVtbl->put_OperatingSystem(This,bstrOperatingSystem)
#define IADsComputer_get_OperatingSystemVersion(This,retval)  (This)->lpVtbl->get_OperatingSystemVersion(This,retval)
#define IADsComputer_put_OperatingSystemVersion(This,bstrOperatingSystemVersion)  (This)->lpVtbl->put_OperatingSystemVersion(This,bstrOperatingSystemVersion)
#define IADsComputer_get_Model(This,retval)  (This)->lpVtbl->get_Model(This,retval)
#define IADsComputer_put_Model(This,bstrModel)  (This)->lpVtbl->put_Model(This,bstrModel)
#define IADsComputer_get_Processor(This,retval)  (This)->lpVtbl->get_Processor(This,retval)
#define IADsComputer_put_Processor(This,bstrProcessor)  (This)->lpVtbl->put_Processor(This,bstrProcessor)
#define IADsComputer_get_ProcessorCount(This,retval)  (This)->lpVtbl->get_ProcessorCount(This,retval)
#define IADsComputer_put_ProcessorCount(This,bstrProcessorCount)  (This)->lpVtbl->put_ProcessorCount(This,bstrProcessorCount)
#define IADsComputer_get_MemorySize(This,retval)  (This)->lpVtbl->get_MemorySize(This,retval)
#define IADsComputer_put_MemorySize(This,bstrMemorySize)  (This)->lpVtbl->put_MemorySize(This,bstrMemorySize)
#define IADsComputer_get_StorageCapacity(This,retval)  (This)->lpVtbl->get_StorageCapacity(This,retval)
#define IADsComputer_put_StorageCapacity(This,bstrStorageCapacity)  (This)->lpVtbl->put_StorageCapacity(This,bstrStorageCapacity)
#define IADsComputer_get_NetAddresses(This,retval)  (This)->lpVtbl->get_NetAddresses(This,retval)
#define IADsComputer_put_NetAddresses(This,vNetAddresses)  (This)->lpVtbl->put_NetAddresses(This,vNetAddresses)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsComputer_get_ComputerID_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_ComputerID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_Site_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_Site_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_Description_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_Description_Proxy (IADsComputer * This, BSTR bstrDescription);
void __RPC_STUB IADsComputer_put_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_Location_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_Location_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_Location_Proxy (IADsComputer * This, BSTR bstrLocation);
void __RPC_STUB IADsComputer_put_Location_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_PrimaryUser_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_PrimaryUser_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_PrimaryUser_Proxy (IADsComputer * This, BSTR bstrPrimaryUser);
void __RPC_STUB IADsComputer_put_PrimaryUser_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_Owner_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_Owner_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_Owner_Proxy (IADsComputer * This, BSTR bstrOwner);
void __RPC_STUB IADsComputer_put_Owner_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_Division_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_Division_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_Division_Proxy (IADsComputer * This, BSTR bstrDivision);
void __RPC_STUB IADsComputer_put_Division_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_Department_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_Department_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_Department_Proxy (IADsComputer * This, BSTR bstrDepartment);
void __RPC_STUB IADsComputer_put_Department_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_Role_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_Role_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_Role_Proxy (IADsComputer * This, BSTR bstrRole);
void __RPC_STUB IADsComputer_put_Role_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_OperatingSystem_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_OperatingSystem_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_OperatingSystem_Proxy (IADsComputer * This, BSTR bstrOperatingSystem);
void __RPC_STUB IADsComputer_put_OperatingSystem_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_OperatingSystemVersion_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_OperatingSystemVersion_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_OperatingSystemVersion_Proxy (IADsComputer * This, BSTR bstrOperatingSystemVersion);
void __RPC_STUB IADsComputer_put_OperatingSystemVersion_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_Model_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_Model_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_Model_Proxy (IADsComputer * This, BSTR bstrModel);
void __RPC_STUB IADsComputer_put_Model_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_Processor_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_Processor_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_Processor_Proxy (IADsComputer * This, BSTR bstrProcessor);
void __RPC_STUB IADsComputer_put_Processor_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_ProcessorCount_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_ProcessorCount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_ProcessorCount_Proxy (IADsComputer * This, BSTR bstrProcessorCount);
void __RPC_STUB IADsComputer_put_ProcessorCount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_MemorySize_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_MemorySize_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_MemorySize_Proxy (IADsComputer * This, BSTR bstrMemorySize);
void __RPC_STUB IADsComputer_put_MemorySize_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_StorageCapacity_Proxy (IADsComputer * This, BSTR * retval);
void __RPC_STUB IADsComputer_get_StorageCapacity_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_StorageCapacity_Proxy (IADsComputer * This, BSTR bstrStorageCapacity);
void __RPC_STUB IADsComputer_put_StorageCapacity_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_get_NetAddresses_Proxy (IADsComputer * This, VARIANT * retval);
void __RPC_STUB IADsComputer_get_NetAddresses_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputer_put_NetAddresses_Proxy (IADsComputer * This, VARIANT vNetAddresses);
void __RPC_STUB IADsComputer_put_NetAddresses_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsComputerOperations_INTERFACE_DEFINED__
#define __IADsComputerOperations_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsComputerOperations;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("ef497680-1d9f-11cf-b1f3-02608c9e7553") IADsComputerOperations:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE Status (IDispatch ** ppObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE Shutdown (VARIANT_BOOL bReboot) = 0;
};
#else
typedef struct IADsComputerOperationsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsComputerOperations * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsComputerOperations * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsComputerOperations * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsComputerOperations * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsComputerOperations * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsComputerOperations * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsComputerOperations * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsComputerOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsComputerOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsComputerOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsComputerOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsComputerOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsComputerOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsComputerOperations * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsComputerOperations * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsComputerOperations * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsComputerOperations * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsComputerOperations * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsComputerOperations * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsComputerOperations * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * Status) (IADsComputerOperations * This, IDispatch ** ppObject);
    HRESULT (STDMETHODCALLTYPE * Shutdown) (IADsComputerOperations * This, VARIANT_BOOL bReboot);
    END_INTERFACE
} IADsComputerOperationsVtbl;
interface IADsComputerOperations {
    CONST_VTBL struct IADsComputerOperationsVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsComputerOperations_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsComputerOperations_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsComputerOperations_Release(This)  (This)->lpVtbl->Release(This)
#define IADsComputerOperations_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsComputerOperations_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsComputerOperations_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsComputerOperations_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsComputerOperations_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsComputerOperations_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsComputerOperations_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsComputerOperations_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsComputerOperations_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsComputerOperations_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsComputerOperations_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsComputerOperations_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsComputerOperations_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsComputerOperations_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsComputerOperations_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsComputerOperations_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsComputerOperations_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsComputerOperations_Status(This,ppObject)  (This)->lpVtbl->Status(This,ppObject)
#define IADsComputerOperations_Shutdown(This,bReboot)  (This)->lpVtbl->Shutdown(This,bReboot)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsComputerOperations_Status_Proxy (IADsComputerOperations * This, IDispatch ** ppObject);
void __RPC_STUB IADsComputerOperations_Status_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsComputerOperations_Shutdown_Proxy (IADsComputerOperations * This, VARIANT_BOOL bReboot);
void __RPC_STUB IADsComputerOperations_Shutdown_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsGroup_INTERFACE_DEFINED__
#define __IADsGroup_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsGroup;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("27636b00-410f-11cf-b1ff-02608c9e7553") IADsGroup:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Description (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Description (BSTR bstrDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE Members (IADsMembers ** ppMembers) = 0;
    virtual HRESULT STDMETHODCALLTYPE IsMember (BSTR bstrMember, VARIANT_BOOL * bMember) = 0;
    virtual HRESULT STDMETHODCALLTYPE Add (BSTR bstrNewItem) = 0;
    virtual HRESULT STDMETHODCALLTYPE Remove (BSTR bstrItemToBeRemoved) = 0;
};
#else
typedef struct IADsGroupVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsGroup * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsGroup * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsGroup * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsGroup * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsGroup * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsGroup * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsGroup * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsGroup * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsGroup * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsGroup * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsGroup * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsGroup * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsGroup * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsGroup * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsGroup * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsGroup * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsGroup * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsGroup * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsGroup * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsGroup * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_Description) (IADsGroup * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Description) (IADsGroup * This, BSTR bstrDescription);
    HRESULT (STDMETHODCALLTYPE * Members) (IADsGroup * This, IADsMembers ** ppMembers);
    HRESULT (STDMETHODCALLTYPE * IsMember) (IADsGroup * This, BSTR bstrMember, VARIANT_BOOL * bMember);
    HRESULT (STDMETHODCALLTYPE * Add) (IADsGroup * This, BSTR bstrNewItem);
    HRESULT (STDMETHODCALLTYPE * Remove) (IADsGroup * This, BSTR bstrItemToBeRemoved);
    END_INTERFACE
} IADsGroupVtbl;
interface IADsGroup {
    CONST_VTBL struct IADsGroupVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsGroup_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsGroup_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsGroup_Release(This)  (This)->lpVtbl->Release(This)
#define IADsGroup_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsGroup_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsGroup_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsGroup_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsGroup_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsGroup_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsGroup_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsGroup_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsGroup_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsGroup_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsGroup_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsGroup_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsGroup_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsGroup_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsGroup_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsGroup_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsGroup_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsGroup_get_Description(This,retval)  (This)->lpVtbl->get_Description(This,retval)
#define IADsGroup_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IADsGroup_Members(This,ppMembers)  (This)->lpVtbl->Members(This,ppMembers)
#define IADsGroup_IsMember(This,bstrMember,bMember)  (This)->lpVtbl->IsMember(This,bstrMember,bMember)
#define IADsGroup_Add(This,bstrNewItem)  (This)->lpVtbl->Add(This,bstrNewItem)
#define IADsGroup_Remove(This,bstrItemToBeRemoved)  (This)->lpVtbl->Remove(This,bstrItemToBeRemoved)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsGroup_get_Description_Proxy (IADsGroup * This, BSTR * retval);
void __RPC_STUB IADsGroup_get_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsGroup_put_Description_Proxy (IADsGroup * This, BSTR bstrDescription);
void __RPC_STUB IADsGroup_put_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsGroup_Members_Proxy (IADsGroup * This, IADsMembers ** ppMembers);
void __RPC_STUB IADsGroup_Members_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsGroup_IsMember_Proxy (IADsGroup * This, BSTR bstrMember, VARIANT_BOOL * bMember);
void __RPC_STUB IADsGroup_IsMember_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsGroup_Add_Proxy (IADsGroup * This, BSTR bstrNewItem);
void __RPC_STUB IADsGroup_Add_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsGroup_Remove_Proxy (IADsGroup * This, BSTR bstrItemToBeRemoved);
void __RPC_STUB IADsGroup_Remove_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsUser_INTERFACE_DEFINED__
#define __IADsUser_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsUser;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("3e37e320-17e2-11cf-abc4-02608c9e7553") IADsUser:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_BadLoginAddress (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BadLoginCount (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LastLogin (DATE * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LastLogoff (DATE * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LastFailedLogin (DATE * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PasswordLastChanged (DATE * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Description (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Description (BSTR bstrDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Division (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Division (BSTR bstrDivision) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Department (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Department (BSTR bstrDepartment) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EmployeeID (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EmployeeID (BSTR bstrEmployeeID) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_FullName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_FullName (BSTR bstrFullName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_FirstName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_FirstName (BSTR bstrFirstName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LastName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LastName (BSTR bstrLastName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OtherName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_OtherName (BSTR bstrOtherName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NamePrefix (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NamePrefix (BSTR bstrNamePrefix) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NameSuffix (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NameSuffix (BSTR bstrNameSuffix) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Title (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Title (BSTR bstrTitle) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Manager (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Manager (BSTR bstrManager) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TelephoneHome (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_TelephoneHome (VARIANT vTelephoneHome) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TelephoneMobile (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_TelephoneMobile (VARIANT vTelephoneMobile) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TelephoneNumber (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_TelephoneNumber (VARIANT vTelephoneNumber) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TelephonePager (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_TelephonePager (VARIANT vTelephonePager) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_FaxNumber (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_FaxNumber (VARIANT vFaxNumber) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OfficeLocations (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_OfficeLocations (VARIANT vOfficeLocations) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PostalAddresses (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PostalAddresses (VARIANT vPostalAddresses) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PostalCodes (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PostalCodes (VARIANT vPostalCodes) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SeeAlso (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SeeAlso (VARIANT vSeeAlso) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AccountDisabled (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AccountDisabled (VARIANT_BOOL fAccountDisabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AccountExpirationDate (DATE * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AccountExpirationDate (DATE daAccountExpirationDate) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GraceLoginsAllowed (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GraceLoginsAllowed (long lnGraceLoginsAllowed) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GraceLoginsRemaining (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GraceLoginsRemaining (long lnGraceLoginsRemaining) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsAccountLocked (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IsAccountLocked (VARIANT_BOOL fIsAccountLocked) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LoginHours (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LoginHours (VARIANT vLoginHours) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LoginWorkstations (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LoginWorkstations (VARIANT vLoginWorkstations) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MaxLogins (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MaxLogins (long lnMaxLogins) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MaxStorage (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MaxStorage (long lnMaxStorage) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PasswordExpirationDate (DATE * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PasswordExpirationDate (DATE daPasswordExpirationDate) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PasswordMinimumLength (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PasswordMinimumLength (long lnPasswordMinimumLength) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PasswordRequired (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PasswordRequired (VARIANT_BOOL fPasswordRequired) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RequireUniquePassword (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RequireUniquePassword (VARIANT_BOOL fRequireUniquePassword) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EmailAddress (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EmailAddress (BSTR bstrEmailAddress) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HomeDirectory (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HomeDirectory (BSTR bstrHomeDirectory) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Languages (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Languages (VARIANT vLanguages) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Profile (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Profile (BSTR bstrProfile) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LoginScript (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LoginScript (BSTR bstrLoginScript) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Picture (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Picture (VARIANT vPicture) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HomePage (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HomePage (BSTR bstrHomePage) = 0;
    virtual HRESULT STDMETHODCALLTYPE Groups (IADsMembers ** ppGroups) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPassword (BSTR NewPassword) = 0;
    virtual HRESULT STDMETHODCALLTYPE ChangePassword (BSTR bstrOldPassword, BSTR bstrNewPassword) = 0;
};
#else
typedef struct IADsUserVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsUser * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsUser * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsUser * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsUser * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsUser * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsUser * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsUser * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsUser * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsUser * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsUser * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsUser * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsUser * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsUser * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsUser * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_BadLoginAddress) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_BadLoginCount) (IADsUser * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * get_LastLogin) (IADsUser * This, DATE * retval);
    HRESULT (STDMETHODCALLTYPE * get_LastLogoff) (IADsUser * This, DATE * retval);
    HRESULT (STDMETHODCALLTYPE * get_LastFailedLogin) (IADsUser * This, DATE * retval);
    HRESULT (STDMETHODCALLTYPE * get_PasswordLastChanged) (IADsUser * This, DATE * retval);
    HRESULT (STDMETHODCALLTYPE * get_Description) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Description) (IADsUser * This, BSTR bstrDescription);
    HRESULT (STDMETHODCALLTYPE * get_Division) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Division) (IADsUser * This, BSTR bstrDivision);
    HRESULT (STDMETHODCALLTYPE * get_Department) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Department) (IADsUser * This, BSTR bstrDepartment);
    HRESULT (STDMETHODCALLTYPE * get_EmployeeID) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_EmployeeID) (IADsUser * This, BSTR bstrEmployeeID);
    HRESULT (STDMETHODCALLTYPE * get_FullName) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_FullName) (IADsUser * This, BSTR bstrFullName);
    HRESULT (STDMETHODCALLTYPE * get_FirstName) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_FirstName) (IADsUser * This, BSTR bstrFirstName);
    HRESULT (STDMETHODCALLTYPE * get_LastName) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_LastName) (IADsUser * This, BSTR bstrLastName);
    HRESULT (STDMETHODCALLTYPE * get_OtherName) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_OtherName) (IADsUser * This, BSTR bstrOtherName);
    HRESULT (STDMETHODCALLTYPE * get_NamePrefix) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_NamePrefix) (IADsUser * This, BSTR bstrNamePrefix);
    HRESULT (STDMETHODCALLTYPE * get_NameSuffix) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_NameSuffix) (IADsUser * This, BSTR bstrNameSuffix);
    HRESULT (STDMETHODCALLTYPE * get_Title) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Title) (IADsUser * This, BSTR bstrTitle);
    HRESULT (STDMETHODCALLTYPE * get_Manager) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Manager) (IADsUser * This, BSTR bstrManager);
    HRESULT (STDMETHODCALLTYPE * get_TelephoneHome) (IADsUser * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_TelephoneHome) (IADsUser * This, VARIANT vTelephoneHome);
    HRESULT (STDMETHODCALLTYPE * get_TelephoneMobile) (IADsUser * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_TelephoneMobile) (IADsUser * This, VARIANT vTelephoneMobile);
    HRESULT (STDMETHODCALLTYPE * get_TelephoneNumber) (IADsUser * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_TelephoneNumber) (IADsUser * This, VARIANT vTelephoneNumber);
    HRESULT (STDMETHODCALLTYPE * get_TelephonePager) (IADsUser * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_TelephonePager) (IADsUser * This, VARIANT vTelephonePager);
    HRESULT (STDMETHODCALLTYPE * get_FaxNumber) (IADsUser * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_FaxNumber) (IADsUser * This, VARIANT vFaxNumber);
    HRESULT (STDMETHODCALLTYPE * get_OfficeLocations) (IADsUser * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_OfficeLocations) (IADsUser * This, VARIANT vOfficeLocations);
    HRESULT (STDMETHODCALLTYPE * get_PostalAddresses) (IADsUser * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_PostalAddresses) (IADsUser * This, VARIANT vPostalAddresses);
    HRESULT (STDMETHODCALLTYPE * get_PostalCodes) (IADsUser * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_PostalCodes) (IADsUser * This, VARIANT vPostalCodes);
    HRESULT (STDMETHODCALLTYPE * get_SeeAlso) (IADsUser * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_SeeAlso) (IADsUser * This, VARIANT vSeeAlso);
    HRESULT (STDMETHODCALLTYPE * get_AccountDisabled) (IADsUser * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * put_AccountDisabled) (IADsUser * This, VARIANT_BOOL fAccountDisabled);
    HRESULT (STDMETHODCALLTYPE * get_AccountExpirationDate) (IADsUser * This, DATE * retval);
    HRESULT (STDMETHODCALLTYPE * put_AccountExpirationDate) (IADsUser * This, DATE daAccountExpirationDate);
    HRESULT (STDMETHODCALLTYPE * get_GraceLoginsAllowed) (IADsUser * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_GraceLoginsAllowed) (IADsUser * This, long lnGraceLoginsAllowed);
    HRESULT (STDMETHODCALLTYPE * get_GraceLoginsRemaining) (IADsUser * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_GraceLoginsRemaining) (IADsUser * This, long lnGraceLoginsRemaining);
    HRESULT (STDMETHODCALLTYPE * get_IsAccountLocked) (IADsUser * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * put_IsAccountLocked) (IADsUser * This, VARIANT_BOOL fIsAccountLocked);
    HRESULT (STDMETHODCALLTYPE * get_LoginHours) (IADsUser * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_LoginHours) (IADsUser * This, VARIANT vLoginHours);
    HRESULT (STDMETHODCALLTYPE * get_LoginWorkstations) (IADsUser * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_LoginWorkstations) (IADsUser * This, VARIANT vLoginWorkstations);
    HRESULT (STDMETHODCALLTYPE * get_MaxLogins) (IADsUser * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_MaxLogins) (IADsUser * This, long lnMaxLogins);
    HRESULT (STDMETHODCALLTYPE * get_MaxStorage) (IADsUser * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_MaxStorage) (IADsUser * This, long lnMaxStorage);
    HRESULT (STDMETHODCALLTYPE * get_PasswordExpirationDate) (IADsUser * This, DATE * retval);
    HRESULT (STDMETHODCALLTYPE * put_PasswordExpirationDate) (IADsUser * This, DATE daPasswordExpirationDate);
    HRESULT (STDMETHODCALLTYPE * get_PasswordMinimumLength) (IADsUser * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_PasswordMinimumLength) (IADsUser * This, long lnPasswordMinimumLength);
    HRESULT (STDMETHODCALLTYPE * get_PasswordRequired) (IADsUser * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * put_PasswordRequired) (IADsUser * This, VARIANT_BOOL fPasswordRequired);
    HRESULT (STDMETHODCALLTYPE * get_RequireUniquePassword) (IADsUser * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * put_RequireUniquePassword) (IADsUser * This, VARIANT_BOOL fRequireUniquePassword);
    HRESULT (STDMETHODCALLTYPE * get_EmailAddress) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_EmailAddress) (IADsUser * This, BSTR bstrEmailAddress);
    HRESULT (STDMETHODCALLTYPE * get_HomeDirectory) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_HomeDirectory) (IADsUser * This, BSTR bstrHomeDirectory);
    HRESULT (STDMETHODCALLTYPE * get_Languages) (IADsUser * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_Languages) (IADsUser * This, VARIANT vLanguages);
    HRESULT (STDMETHODCALLTYPE * get_Profile) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Profile) (IADsUser * This, BSTR bstrProfile);
    HRESULT (STDMETHODCALLTYPE * get_LoginScript) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_LoginScript) (IADsUser * This, BSTR bstrLoginScript);
    HRESULT (STDMETHODCALLTYPE * get_Picture) (IADsUser * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_Picture) (IADsUser * This, VARIANT vPicture);
    HRESULT (STDMETHODCALLTYPE * get_HomePage) (IADsUser * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_HomePage) (IADsUser * This, BSTR bstrHomePage);
    HRESULT (STDMETHODCALLTYPE * Groups) (IADsUser * This, IADsMembers ** ppGroups);
    HRESULT (STDMETHODCALLTYPE * SetPassword) (IADsUser * This, BSTR NewPassword);
    HRESULT (STDMETHODCALLTYPE * ChangePassword) (IADsUser * This, BSTR bstrOldPassword, BSTR bstrNewPassword);
    END_INTERFACE
} IADsUserVtbl;
interface IADsUser {
    CONST_VTBL struct IADsUserVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsUser_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsUser_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsUser_Release(This)  (This)->lpVtbl->Release(This)
#define IADsUser_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsUser_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsUser_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsUser_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsUser_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsUser_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsUser_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsUser_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsUser_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsUser_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsUser_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsUser_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsUser_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsUser_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsUser_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsUser_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsUser_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsUser_get_BadLoginAddress(This,retval)  (This)->lpVtbl->get_BadLoginAddress(This,retval)
#define IADsUser_get_BadLoginCount(This,retval)  (This)->lpVtbl->get_BadLoginCount(This,retval)
#define IADsUser_get_LastLogin(This,retval)  (This)->lpVtbl->get_LastLogin(This,retval)
#define IADsUser_get_LastLogoff(This,retval)  (This)->lpVtbl->get_LastLogoff(This,retval)
#define IADsUser_get_LastFailedLogin(This,retval)  (This)->lpVtbl->get_LastFailedLogin(This,retval)
#define IADsUser_get_PasswordLastChanged(This,retval)  (This)->lpVtbl->get_PasswordLastChanged(This,retval)
#define IADsUser_get_Description(This,retval)  (This)->lpVtbl->get_Description(This,retval)
#define IADsUser_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IADsUser_get_Division(This,retval)  (This)->lpVtbl->get_Division(This,retval)
#define IADsUser_put_Division(This,bstrDivision)  (This)->lpVtbl->put_Division(This,bstrDivision)
#define IADsUser_get_Department(This,retval)  (This)->lpVtbl->get_Department(This,retval)
#define IADsUser_put_Department(This,bstrDepartment)  (This)->lpVtbl->put_Department(This,bstrDepartment)
#define IADsUser_get_EmployeeID(This,retval)  (This)->lpVtbl->get_EmployeeID(This,retval)
#define IADsUser_put_EmployeeID(This,bstrEmployeeID)  (This)->lpVtbl->put_EmployeeID(This,bstrEmployeeID)
#define IADsUser_get_FullName(This,retval)  (This)->lpVtbl->get_FullName(This,retval)
#define IADsUser_put_FullName(This,bstrFullName)  (This)->lpVtbl->put_FullName(This,bstrFullName)
#define IADsUser_get_FirstName(This,retval)  (This)->lpVtbl->get_FirstName(This,retval)
#define IADsUser_put_FirstName(This,bstrFirstName)  (This)->lpVtbl->put_FirstName(This,bstrFirstName)
#define IADsUser_get_LastName(This,retval)  (This)->lpVtbl->get_LastName(This,retval)
#define IADsUser_put_LastName(This,bstrLastName)  (This)->lpVtbl->put_LastName(This,bstrLastName)
#define IADsUser_get_OtherName(This,retval)  (This)->lpVtbl->get_OtherName(This,retval)
#define IADsUser_put_OtherName(This,bstrOtherName)  (This)->lpVtbl->put_OtherName(This,bstrOtherName)
#define IADsUser_get_NamePrefix(This,retval)  (This)->lpVtbl->get_NamePrefix(This,retval)
#define IADsUser_put_NamePrefix(This,bstrNamePrefix)  (This)->lpVtbl->put_NamePrefix(This,bstrNamePrefix)
#define IADsUser_get_NameSuffix(This,retval)  (This)->lpVtbl->get_NameSuffix(This,retval)
#define IADsUser_put_NameSuffix(This,bstrNameSuffix)  (This)->lpVtbl->put_NameSuffix(This,bstrNameSuffix)
#define IADsUser_get_Title(This,retval)  (This)->lpVtbl->get_Title(This,retval)
#define IADsUser_put_Title(This,bstrTitle)  (This)->lpVtbl->put_Title(This,bstrTitle)
#define IADsUser_get_Manager(This,retval)  (This)->lpVtbl->get_Manager(This,retval)
#define IADsUser_put_Manager(This,bstrManager)  (This)->lpVtbl->put_Manager(This,bstrManager)
#define IADsUser_get_TelephoneHome(This,retval)  (This)->lpVtbl->get_TelephoneHome(This,retval)
#define IADsUser_put_TelephoneHome(This,vTelephoneHome)  (This)->lpVtbl->put_TelephoneHome(This,vTelephoneHome)
#define IADsUser_get_TelephoneMobile(This,retval)  (This)->lpVtbl->get_TelephoneMobile(This,retval)
#define IADsUser_put_TelephoneMobile(This,vTelephoneMobile)  (This)->lpVtbl->put_TelephoneMobile(This,vTelephoneMobile)
#define IADsUser_get_TelephoneNumber(This,retval)  (This)->lpVtbl->get_TelephoneNumber(This,retval)
#define IADsUser_put_TelephoneNumber(This,vTelephoneNumber)  (This)->lpVtbl->put_TelephoneNumber(This,vTelephoneNumber)
#define IADsUser_get_TelephonePager(This,retval)  (This)->lpVtbl->get_TelephonePager(This,retval)
#define IADsUser_put_TelephonePager(This,vTelephonePager)  (This)->lpVtbl->put_TelephonePager(This,vTelephonePager)
#define IADsUser_get_FaxNumber(This,retval)  (This)->lpVtbl->get_FaxNumber(This,retval)
#define IADsUser_put_FaxNumber(This,vFaxNumber)  (This)->lpVtbl->put_FaxNumber(This,vFaxNumber)
#define IADsUser_get_OfficeLocations(This,retval)  (This)->lpVtbl->get_OfficeLocations(This,retval)
#define IADsUser_put_OfficeLocations(This,vOfficeLocations)  (This)->lpVtbl->put_OfficeLocations(This,vOfficeLocations)
#define IADsUser_get_PostalAddresses(This,retval)  (This)->lpVtbl->get_PostalAddresses(This,retval)
#define IADsUser_put_PostalAddresses(This,vPostalAddresses)  (This)->lpVtbl->put_PostalAddresses(This,vPostalAddresses)
#define IADsUser_get_PostalCodes(This,retval)  (This)->lpVtbl->get_PostalCodes(This,retval)
#define IADsUser_put_PostalCodes(This,vPostalCodes)  (This)->lpVtbl->put_PostalCodes(This,vPostalCodes)
#define IADsUser_get_SeeAlso(This,retval)  (This)->lpVtbl->get_SeeAlso(This,retval)
#define IADsUser_put_SeeAlso(This,vSeeAlso)  (This)->lpVtbl->put_SeeAlso(This,vSeeAlso)
#define IADsUser_get_AccountDisabled(This,retval)  (This)->lpVtbl->get_AccountDisabled(This,retval)
#define IADsUser_put_AccountDisabled(This,fAccountDisabled)  (This)->lpVtbl->put_AccountDisabled(This,fAccountDisabled)
#define IADsUser_get_AccountExpirationDate(This,retval)  (This)->lpVtbl->get_AccountExpirationDate(This,retval)
#define IADsUser_put_AccountExpirationDate(This,daAccountExpirationDate)  (This)->lpVtbl->put_AccountExpirationDate(This,daAccountExpirationDate)
#define IADsUser_get_GraceLoginsAllowed(This,retval)  (This)->lpVtbl->get_GraceLoginsAllowed(This,retval)
#define IADsUser_put_GraceLoginsAllowed(This,lnGraceLoginsAllowed)  (This)->lpVtbl->put_GraceLoginsAllowed(This,lnGraceLoginsAllowed)
#define IADsUser_get_GraceLoginsRemaining(This,retval)  (This)->lpVtbl->get_GraceLoginsRemaining(This,retval)
#define IADsUser_put_GraceLoginsRemaining(This,lnGraceLoginsRemaining)  (This)->lpVtbl->put_GraceLoginsRemaining(This,lnGraceLoginsRemaining)
#define IADsUser_get_IsAccountLocked(This,retval)  (This)->lpVtbl->get_IsAccountLocked(This,retval)
#define IADsUser_put_IsAccountLocked(This,fIsAccountLocked)  (This)->lpVtbl->put_IsAccountLocked(This,fIsAccountLocked)
#define IADsUser_get_LoginHours(This,retval)  (This)->lpVtbl->get_LoginHours(This,retval)
#define IADsUser_put_LoginHours(This,vLoginHours)  (This)->lpVtbl->put_LoginHours(This,vLoginHours)
#define IADsUser_get_LoginWorkstations(This,retval)  (This)->lpVtbl->get_LoginWorkstations(This,retval)
#define IADsUser_put_LoginWorkstations(This,vLoginWorkstations)  (This)->lpVtbl->put_LoginWorkstations(This,vLoginWorkstations)
#define IADsUser_get_MaxLogins(This,retval)  (This)->lpVtbl->get_MaxLogins(This,retval)
#define IADsUser_put_MaxLogins(This,lnMaxLogins)  (This)->lpVtbl->put_MaxLogins(This,lnMaxLogins)
#define IADsUser_get_MaxStorage(This,retval)  (This)->lpVtbl->get_MaxStorage(This,retval)
#define IADsUser_put_MaxStorage(This,lnMaxStorage)  (This)->lpVtbl->put_MaxStorage(This,lnMaxStorage)
#define IADsUser_get_PasswordExpirationDate(This,retval)  (This)->lpVtbl->get_PasswordExpirationDate(This,retval)
#define IADsUser_put_PasswordExpirationDate(This,daPasswordExpirationDate)  (This)->lpVtbl->put_PasswordExpirationDate(This,daPasswordExpirationDate)
#define IADsUser_get_PasswordMinimumLength(This,retval)  (This)->lpVtbl->get_PasswordMinimumLength(This,retval)
#define IADsUser_put_PasswordMinimumLength(This,lnPasswordMinimumLength)  (This)->lpVtbl->put_PasswordMinimumLength(This,lnPasswordMinimumLength)
#define IADsUser_get_PasswordRequired(This,retval)  (This)->lpVtbl->get_PasswordRequired(This,retval)
#define IADsUser_put_PasswordRequired(This,fPasswordRequired)  (This)->lpVtbl->put_PasswordRequired(This,fPasswordRequired)
#define IADsUser_get_RequireUniquePassword(This,retval)  (This)->lpVtbl->get_RequireUniquePassword(This,retval)
#define IADsUser_put_RequireUniquePassword(This,fRequireUniquePassword)  (This)->lpVtbl->put_RequireUniquePassword(This,fRequireUniquePassword)
#define IADsUser_get_EmailAddress(This,retval)  (This)->lpVtbl->get_EmailAddress(This,retval)
#define IADsUser_put_EmailAddress(This,bstrEmailAddress)  (This)->lpVtbl->put_EmailAddress(This,bstrEmailAddress)
#define IADsUser_get_HomeDirectory(This,retval)  (This)->lpVtbl->get_HomeDirectory(This,retval)
#define IADsUser_put_HomeDirectory(This,bstrHomeDirectory)  (This)->lpVtbl->put_HomeDirectory(This,bstrHomeDirectory)
#define IADsUser_get_Languages(This,retval)  (This)->lpVtbl->get_Languages(This,retval)
#define IADsUser_put_Languages(This,vLanguages)  (This)->lpVtbl->put_Languages(This,vLanguages)
#define IADsUser_get_Profile(This,retval)  (This)->lpVtbl->get_Profile(This,retval)
#define IADsUser_put_Profile(This,bstrProfile)  (This)->lpVtbl->put_Profile(This,bstrProfile)
#define IADsUser_get_LoginScript(This,retval)  (This)->lpVtbl->get_LoginScript(This,retval)
#define IADsUser_put_LoginScript(This,bstrLoginScript)  (This)->lpVtbl->put_LoginScript(This,bstrLoginScript)
#define IADsUser_get_Picture(This,retval)  (This)->lpVtbl->get_Picture(This,retval)
#define IADsUser_put_Picture(This,vPicture)  (This)->lpVtbl->put_Picture(This,vPicture)
#define IADsUser_get_HomePage(This,retval)  (This)->lpVtbl->get_HomePage(This,retval)
#define IADsUser_put_HomePage(This,bstrHomePage)  (This)->lpVtbl->put_HomePage(This,bstrHomePage)
#define IADsUser_Groups(This,ppGroups)  (This)->lpVtbl->Groups(This,ppGroups)
#define IADsUser_SetPassword(This,NewPassword)  (This)->lpVtbl->SetPassword(This,NewPassword)
#define IADsUser_ChangePassword(This,bstrOldPassword,bstrNewPassword)  (This)->lpVtbl->ChangePassword(This,bstrOldPassword,bstrNewPassword)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsUser_get_BadLoginAddress_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_BadLoginAddress_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_BadLoginCount_Proxy (IADsUser * This, long *retval);
void __RPC_STUB IADsUser_get_BadLoginCount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_LastLogin_Proxy (IADsUser * This, DATE * retval);
void __RPC_STUB IADsUser_get_LastLogin_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_LastLogoff_Proxy (IADsUser * This, DATE * retval);
void __RPC_STUB IADsUser_get_LastLogoff_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_LastFailedLogin_Proxy (IADsUser * This, DATE * retval);
void __RPC_STUB IADsUser_get_LastFailedLogin_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_PasswordLastChanged_Proxy (IADsUser * This, DATE * retval);
void __RPC_STUB IADsUser_get_PasswordLastChanged_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_Description_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_Description_Proxy (IADsUser * This, BSTR bstrDescription);
void __RPC_STUB IADsUser_put_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_Division_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_Division_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_Division_Proxy (IADsUser * This, BSTR bstrDivision);
void __RPC_STUB IADsUser_put_Division_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_Department_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_Department_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_Department_Proxy (IADsUser * This, BSTR bstrDepartment);
void __RPC_STUB IADsUser_put_Department_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_EmployeeID_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_EmployeeID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_EmployeeID_Proxy (IADsUser * This, BSTR bstrEmployeeID);
void __RPC_STUB IADsUser_put_EmployeeID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_FullName_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_FullName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_FullName_Proxy (IADsUser * This, BSTR bstrFullName);
void __RPC_STUB IADsUser_put_FullName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_FirstName_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_FirstName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_FirstName_Proxy (IADsUser * This, BSTR bstrFirstName);
void __RPC_STUB IADsUser_put_FirstName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_LastName_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_LastName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_LastName_Proxy (IADsUser * This, BSTR bstrLastName);
void __RPC_STUB IADsUser_put_LastName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_OtherName_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_OtherName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_OtherName_Proxy (IADsUser * This, BSTR bstrOtherName);
void __RPC_STUB IADsUser_put_OtherName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_NamePrefix_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_NamePrefix_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_NamePrefix_Proxy (IADsUser * This, BSTR bstrNamePrefix);
void __RPC_STUB IADsUser_put_NamePrefix_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_NameSuffix_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_NameSuffix_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_NameSuffix_Proxy (IADsUser * This, BSTR bstrNameSuffix);
void __RPC_STUB IADsUser_put_NameSuffix_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_Title_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_Title_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_Title_Proxy (IADsUser * This, BSTR bstrTitle);
void __RPC_STUB IADsUser_put_Title_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_Manager_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_Manager_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_Manager_Proxy (IADsUser * This, BSTR bstrManager);
void __RPC_STUB IADsUser_put_Manager_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_TelephoneHome_Proxy (IADsUser * This, VARIANT * retval);
void __RPC_STUB IADsUser_get_TelephoneHome_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_TelephoneHome_Proxy (IADsUser * This, VARIANT vTelephoneHome);
void __RPC_STUB IADsUser_put_TelephoneHome_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_TelephoneMobile_Proxy (IADsUser * This, VARIANT * retval);
void __RPC_STUB IADsUser_get_TelephoneMobile_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_TelephoneMobile_Proxy (IADsUser * This, VARIANT vTelephoneMobile);
void __RPC_STUB IADsUser_put_TelephoneMobile_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_TelephoneNumber_Proxy (IADsUser * This, VARIANT * retval);
void __RPC_STUB IADsUser_get_TelephoneNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_TelephoneNumber_Proxy (IADsUser * This, VARIANT vTelephoneNumber);
void __RPC_STUB IADsUser_put_TelephoneNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_TelephonePager_Proxy (IADsUser * This, VARIANT * retval);
void __RPC_STUB IADsUser_get_TelephonePager_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_TelephonePager_Proxy (IADsUser * This, VARIANT vTelephonePager);
void __RPC_STUB IADsUser_put_TelephonePager_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_FaxNumber_Proxy (IADsUser * This, VARIANT * retval);
void __RPC_STUB IADsUser_get_FaxNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_FaxNumber_Proxy (IADsUser * This, VARIANT vFaxNumber);
void __RPC_STUB IADsUser_put_FaxNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_OfficeLocations_Proxy (IADsUser * This, VARIANT * retval);
void __RPC_STUB IADsUser_get_OfficeLocations_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_OfficeLocations_Proxy (IADsUser * This, VARIANT vOfficeLocations);
void __RPC_STUB IADsUser_put_OfficeLocations_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_PostalAddresses_Proxy (IADsUser * This, VARIANT * retval);
void __RPC_STUB IADsUser_get_PostalAddresses_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_PostalAddresses_Proxy (IADsUser * This, VARIANT vPostalAddresses);
void __RPC_STUB IADsUser_put_PostalAddresses_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_PostalCodes_Proxy (IADsUser * This, VARIANT * retval);
void __RPC_STUB IADsUser_get_PostalCodes_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_PostalCodes_Proxy (IADsUser * This, VARIANT vPostalCodes);
void __RPC_STUB IADsUser_put_PostalCodes_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_SeeAlso_Proxy (IADsUser * This, VARIANT * retval);
void __RPC_STUB IADsUser_get_SeeAlso_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_SeeAlso_Proxy (IADsUser * This, VARIANT vSeeAlso);
void __RPC_STUB IADsUser_put_SeeAlso_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_AccountDisabled_Proxy (IADsUser * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsUser_get_AccountDisabled_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_AccountDisabled_Proxy (IADsUser * This, VARIANT_BOOL fAccountDisabled);
void __RPC_STUB IADsUser_put_AccountDisabled_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_AccountExpirationDate_Proxy (IADsUser * This, DATE * retval);
void __RPC_STUB IADsUser_get_AccountExpirationDate_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_AccountExpirationDate_Proxy (IADsUser * This, DATE daAccountExpirationDate);
void __RPC_STUB IADsUser_put_AccountExpirationDate_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_GraceLoginsAllowed_Proxy (IADsUser * This, long *retval);
void __RPC_STUB IADsUser_get_GraceLoginsAllowed_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_GraceLoginsAllowed_Proxy (IADsUser * This, long lnGraceLoginsAllowed);
void __RPC_STUB IADsUser_put_GraceLoginsAllowed_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_GraceLoginsRemaining_Proxy (IADsUser * This, long *retval);
void __RPC_STUB IADsUser_get_GraceLoginsRemaining_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_GraceLoginsRemaining_Proxy (IADsUser * This, long lnGraceLoginsRemaining);
void __RPC_STUB IADsUser_put_GraceLoginsRemaining_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_IsAccountLocked_Proxy (IADsUser * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsUser_get_IsAccountLocked_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_IsAccountLocked_Proxy (IADsUser * This, VARIANT_BOOL fIsAccountLocked);
void __RPC_STUB IADsUser_put_IsAccountLocked_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_LoginHours_Proxy (IADsUser * This, VARIANT * retval);
void __RPC_STUB IADsUser_get_LoginHours_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_LoginHours_Proxy (IADsUser * This, VARIANT vLoginHours);
void __RPC_STUB IADsUser_put_LoginHours_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_LoginWorkstations_Proxy (IADsUser * This, VARIANT * retval);
void __RPC_STUB IADsUser_get_LoginWorkstations_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_LoginWorkstations_Proxy (IADsUser * This, VARIANT vLoginWorkstations);
void __RPC_STUB IADsUser_put_LoginWorkstations_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_MaxLogins_Proxy (IADsUser * This, long *retval);
void __RPC_STUB IADsUser_get_MaxLogins_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_MaxLogins_Proxy (IADsUser * This, long lnMaxLogins);
void __RPC_STUB IADsUser_put_MaxLogins_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_MaxStorage_Proxy (IADsUser * This, long *retval);
void __RPC_STUB IADsUser_get_MaxStorage_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_MaxStorage_Proxy (IADsUser * This, long lnMaxStorage);
void __RPC_STUB IADsUser_put_MaxStorage_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_PasswordExpirationDate_Proxy (IADsUser * This, DATE * retval);
void __RPC_STUB IADsUser_get_PasswordExpirationDate_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_PasswordExpirationDate_Proxy (IADsUser * This, DATE daPasswordExpirationDate);
void __RPC_STUB IADsUser_put_PasswordExpirationDate_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_PasswordMinimumLength_Proxy (IADsUser * This, long *retval);
void __RPC_STUB IADsUser_get_PasswordMinimumLength_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_PasswordMinimumLength_Proxy (IADsUser * This, long lnPasswordMinimumLength);
void __RPC_STUB IADsUser_put_PasswordMinimumLength_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_PasswordRequired_Proxy (IADsUser * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsUser_get_PasswordRequired_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_PasswordRequired_Proxy (IADsUser * This, VARIANT_BOOL fPasswordRequired);
void __RPC_STUB IADsUser_put_PasswordRequired_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_RequireUniquePassword_Proxy (IADsUser * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsUser_get_RequireUniquePassword_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_RequireUniquePassword_Proxy (IADsUser * This, VARIANT_BOOL fRequireUniquePassword);
void __RPC_STUB IADsUser_put_RequireUniquePassword_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_EmailAddress_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_EmailAddress_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_EmailAddress_Proxy (IADsUser * This, BSTR bstrEmailAddress);
void __RPC_STUB IADsUser_put_EmailAddress_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_HomeDirectory_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_HomeDirectory_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_HomeDirectory_Proxy (IADsUser * This, BSTR bstrHomeDirectory);
void __RPC_STUB IADsUser_put_HomeDirectory_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_Languages_Proxy (IADsUser * This, VARIANT * retval);
void __RPC_STUB IADsUser_get_Languages_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_Languages_Proxy (IADsUser * This, VARIANT vLanguages);
void __RPC_STUB IADsUser_put_Languages_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_Profile_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_Profile_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_Profile_Proxy (IADsUser * This, BSTR bstrProfile);
void __RPC_STUB IADsUser_put_Profile_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_LoginScript_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_LoginScript_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_LoginScript_Proxy (IADsUser * This, BSTR bstrLoginScript);
void __RPC_STUB IADsUser_put_LoginScript_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_Picture_Proxy (IADsUser * This, VARIANT * retval);
void __RPC_STUB IADsUser_get_Picture_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_Picture_Proxy (IADsUser * This, VARIANT vPicture);
void __RPC_STUB IADsUser_put_Picture_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_get_HomePage_Proxy (IADsUser * This, BSTR * retval);
void __RPC_STUB IADsUser_get_HomePage_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_put_HomePage_Proxy (IADsUser * This, BSTR bstrHomePage);
void __RPC_STUB IADsUser_put_HomePage_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_Groups_Proxy (IADsUser * This, IADsMembers ** ppGroups);
void __RPC_STUB IADsUser_Groups_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_SetPassword_Proxy (IADsUser * This, BSTR NewPassword);
void __RPC_STUB IADsUser_SetPassword_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsUser_ChangePassword_Proxy (IADsUser * This, BSTR bstrOldPassword, BSTR bstrNewPassword);
void __RPC_STUB IADsUser_ChangePassword_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsPrintQueue_INTERFACE_DEFINED__
#define __IADsPrintQueue_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsPrintQueue;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("b15160d0-1226-11cf-a985-00aa006bc149") IADsPrintQueue:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_PrinterPath (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PrinterPath (BSTR bstrPrinterPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Model (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Model (BSTR bstrModel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Datatype (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Datatype (BSTR bstrDatatype) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PrintProcessor (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PrintProcessor (BSTR bstrPrintProcessor) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Description (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Description (BSTR bstrDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Location (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Location (BSTR bstrLocation) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StartTime (DATE * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StartTime (DATE daStartTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UntilTime (DATE * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_UntilTime (DATE daUntilTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DefaultJobPriority (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DefaultJobPriority (long lnDefaultJobPriority) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Priority (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Priority (long lnPriority) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_BannerPage (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_BannerPage (BSTR bstrBannerPage) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PrintDevices (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PrintDevices (VARIANT vPrintDevices) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NetAddresses (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NetAddresses (VARIANT vNetAddresses) = 0;
};
#else
typedef struct IADsPrintQueueVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsPrintQueue * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsPrintQueue * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsPrintQueue * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsPrintQueue * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsPrintQueue * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsPrintQueue * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsPrintQueue * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsPrintQueue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsPrintQueue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsPrintQueue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsPrintQueue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsPrintQueue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsPrintQueue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsPrintQueue * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsPrintQueue * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsPrintQueue * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsPrintQueue * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsPrintQueue * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsPrintQueue * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsPrintQueue * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_PrinterPath) (IADsPrintQueue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_PrinterPath) (IADsPrintQueue * This, BSTR bstrPrinterPath);
    HRESULT (STDMETHODCALLTYPE * get_Model) (IADsPrintQueue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Model) (IADsPrintQueue * This, BSTR bstrModel);
    HRESULT (STDMETHODCALLTYPE * get_Datatype) (IADsPrintQueue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Datatype) (IADsPrintQueue * This, BSTR bstrDatatype);
    HRESULT (STDMETHODCALLTYPE * get_PrintProcessor) (IADsPrintQueue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_PrintProcessor) (IADsPrintQueue * This, BSTR bstrPrintProcessor);
    HRESULT (STDMETHODCALLTYPE * get_Description) (IADsPrintQueue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Description) (IADsPrintQueue * This, BSTR bstrDescription);
    HRESULT (STDMETHODCALLTYPE * get_Location) (IADsPrintQueue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Location) (IADsPrintQueue * This, BSTR bstrLocation);
    HRESULT (STDMETHODCALLTYPE * get_StartTime) (IADsPrintQueue * This, DATE * retval);
    HRESULT (STDMETHODCALLTYPE * put_StartTime) (IADsPrintQueue * This, DATE daStartTime);
    HRESULT (STDMETHODCALLTYPE * get_UntilTime) (IADsPrintQueue * This, DATE * retval);
    HRESULT (STDMETHODCALLTYPE * put_UntilTime) (IADsPrintQueue * This, DATE daUntilTime);
    HRESULT (STDMETHODCALLTYPE * get_DefaultJobPriority) (IADsPrintQueue * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_DefaultJobPriority) (IADsPrintQueue * This, long lnDefaultJobPriority);
    HRESULT (STDMETHODCALLTYPE * get_Priority) (IADsPrintQueue * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Priority) (IADsPrintQueue * This, long lnPriority);
    HRESULT (STDMETHODCALLTYPE * get_BannerPage) (IADsPrintQueue * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_BannerPage) (IADsPrintQueue * This, BSTR bstrBannerPage);
    HRESULT (STDMETHODCALLTYPE * get_PrintDevices) (IADsPrintQueue * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_PrintDevices) (IADsPrintQueue * This, VARIANT vPrintDevices);
    HRESULT (STDMETHODCALLTYPE * get_NetAddresses) (IADsPrintQueue * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_NetAddresses) (IADsPrintQueue * This, VARIANT vNetAddresses);
    END_INTERFACE
} IADsPrintQueueVtbl;
interface IADsPrintQueue {
    CONST_VTBL struct IADsPrintQueueVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsPrintQueue_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsPrintQueue_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsPrintQueue_Release(This)  (This)->lpVtbl->Release(This)
#define IADsPrintQueue_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsPrintQueue_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsPrintQueue_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsPrintQueue_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsPrintQueue_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsPrintQueue_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsPrintQueue_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsPrintQueue_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsPrintQueue_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsPrintQueue_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsPrintQueue_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsPrintQueue_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsPrintQueue_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsPrintQueue_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsPrintQueue_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsPrintQueue_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsPrintQueue_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsPrintQueue_get_PrinterPath(This,retval)  (This)->lpVtbl->get_PrinterPath(This,retval)
#define IADsPrintQueue_put_PrinterPath(This,bstrPrinterPath)  (This)->lpVtbl->put_PrinterPath(This,bstrPrinterPath)
#define IADsPrintQueue_get_Model(This,retval)  (This)->lpVtbl->get_Model(This,retval)
#define IADsPrintQueue_put_Model(This,bstrModel)  (This)->lpVtbl->put_Model(This,bstrModel)
#define IADsPrintQueue_get_Datatype(This,retval)  (This)->lpVtbl->get_Datatype(This,retval)
#define IADsPrintQueue_put_Datatype(This,bstrDatatype)  (This)->lpVtbl->put_Datatype(This,bstrDatatype)
#define IADsPrintQueue_get_PrintProcessor(This,retval)  (This)->lpVtbl->get_PrintProcessor(This,retval)
#define IADsPrintQueue_put_PrintProcessor(This,bstrPrintProcessor)  (This)->lpVtbl->put_PrintProcessor(This,bstrPrintProcessor)
#define IADsPrintQueue_get_Description(This,retval)  (This)->lpVtbl->get_Description(This,retval)
#define IADsPrintQueue_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IADsPrintQueue_get_Location(This,retval)  (This)->lpVtbl->get_Location(This,retval)
#define IADsPrintQueue_put_Location(This,bstrLocation)  (This)->lpVtbl->put_Location(This,bstrLocation)
#define IADsPrintQueue_get_StartTime(This,retval)  (This)->lpVtbl->get_StartTime(This,retval)
#define IADsPrintQueue_put_StartTime(This,daStartTime)  (This)->lpVtbl->put_StartTime(This,daStartTime)
#define IADsPrintQueue_get_UntilTime(This,retval)  (This)->lpVtbl->get_UntilTime(This,retval)
#define IADsPrintQueue_put_UntilTime(This,daUntilTime)  (This)->lpVtbl->put_UntilTime(This,daUntilTime)
#define IADsPrintQueue_get_DefaultJobPriority(This,retval)  (This)->lpVtbl->get_DefaultJobPriority(This,retval)
#define IADsPrintQueue_put_DefaultJobPriority(This,lnDefaultJobPriority)  (This)->lpVtbl->put_DefaultJobPriority(This,lnDefaultJobPriority)
#define IADsPrintQueue_get_Priority(This,retval)  (This)->lpVtbl->get_Priority(This,retval)
#define IADsPrintQueue_put_Priority(This,lnPriority)  (This)->lpVtbl->put_Priority(This,lnPriority)
#define IADsPrintQueue_get_BannerPage(This,retval)  (This)->lpVtbl->get_BannerPage(This,retval)
#define IADsPrintQueue_put_BannerPage(This,bstrBannerPage)  (This)->lpVtbl->put_BannerPage(This,bstrBannerPage)
#define IADsPrintQueue_get_PrintDevices(This,retval)  (This)->lpVtbl->get_PrintDevices(This,retval)
#define IADsPrintQueue_put_PrintDevices(This,vPrintDevices)  (This)->lpVtbl->put_PrintDevices(This,vPrintDevices)
#define IADsPrintQueue_get_NetAddresses(This,retval)  (This)->lpVtbl->get_NetAddresses(This,retval)
#define IADsPrintQueue_put_NetAddresses(This,vNetAddresses)  (This)->lpVtbl->put_NetAddresses(This,vNetAddresses)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsPrintQueue_get_PrinterPath_Proxy (IADsPrintQueue * This, BSTR * retval);
void __RPC_STUB IADsPrintQueue_get_PrinterPath_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_put_PrinterPath_Proxy (IADsPrintQueue * This, BSTR bstrPrinterPath);
void __RPC_STUB IADsPrintQueue_put_PrinterPath_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_get_Model_Proxy (IADsPrintQueue * This, BSTR * retval);
void __RPC_STUB IADsPrintQueue_get_Model_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_put_Model_Proxy (IADsPrintQueue * This, BSTR bstrModel);
void __RPC_STUB IADsPrintQueue_put_Model_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_get_Datatype_Proxy (IADsPrintQueue * This, BSTR * retval);
void __RPC_STUB IADsPrintQueue_get_Datatype_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_put_Datatype_Proxy (IADsPrintQueue * This, BSTR bstrDatatype);
void __RPC_STUB IADsPrintQueue_put_Datatype_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_get_PrintProcessor_Proxy (IADsPrintQueue * This, BSTR * retval);
void __RPC_STUB IADsPrintQueue_get_PrintProcessor_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_put_PrintProcessor_Proxy (IADsPrintQueue * This, BSTR bstrPrintProcessor);
void __RPC_STUB IADsPrintQueue_put_PrintProcessor_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_get_Description_Proxy (IADsPrintQueue * This, BSTR * retval);
void __RPC_STUB IADsPrintQueue_get_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_put_Description_Proxy (IADsPrintQueue * This, BSTR bstrDescription);
void __RPC_STUB IADsPrintQueue_put_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_get_Location_Proxy (IADsPrintQueue * This, BSTR * retval);
void __RPC_STUB IADsPrintQueue_get_Location_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_put_Location_Proxy (IADsPrintQueue * This, BSTR bstrLocation);
void __RPC_STUB IADsPrintQueue_put_Location_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_get_StartTime_Proxy (IADsPrintQueue * This, DATE * retval);
void __RPC_STUB IADsPrintQueue_get_StartTime_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_put_StartTime_Proxy (IADsPrintQueue * This, DATE daStartTime);
void __RPC_STUB IADsPrintQueue_put_StartTime_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_get_UntilTime_Proxy (IADsPrintQueue * This, DATE * retval);
void __RPC_STUB IADsPrintQueue_get_UntilTime_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_put_UntilTime_Proxy (IADsPrintQueue * This, DATE daUntilTime);
void __RPC_STUB IADsPrintQueue_put_UntilTime_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_get_DefaultJobPriority_Proxy (IADsPrintQueue * This, long *retval);
void __RPC_STUB IADsPrintQueue_get_DefaultJobPriority_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_put_DefaultJobPriority_Proxy (IADsPrintQueue * This, long lnDefaultJobPriority);
void __RPC_STUB IADsPrintQueue_put_DefaultJobPriority_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_get_Priority_Proxy (IADsPrintQueue * This, long *retval);
void __RPC_STUB IADsPrintQueue_get_Priority_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_put_Priority_Proxy (IADsPrintQueue * This, long lnPriority);
void __RPC_STUB IADsPrintQueue_put_Priority_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_get_BannerPage_Proxy (IADsPrintQueue * This, BSTR * retval);
void __RPC_STUB IADsPrintQueue_get_BannerPage_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_put_BannerPage_Proxy (IADsPrintQueue * This, BSTR bstrBannerPage);
void __RPC_STUB IADsPrintQueue_put_BannerPage_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_get_PrintDevices_Proxy (IADsPrintQueue * This, VARIANT * retval);
void __RPC_STUB IADsPrintQueue_get_PrintDevices_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_put_PrintDevices_Proxy (IADsPrintQueue * This, VARIANT vPrintDevices);
void __RPC_STUB IADsPrintQueue_put_PrintDevices_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_get_NetAddresses_Proxy (IADsPrintQueue * This, VARIANT * retval);
void __RPC_STUB IADsPrintQueue_get_NetAddresses_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueue_put_NetAddresses_Proxy (IADsPrintQueue * This, VARIANT vNetAddresses);
void __RPC_STUB IADsPrintQueue_put_NetAddresses_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsPrintQueueOperations_INTERFACE_DEFINED__
#define __IADsPrintQueueOperations_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsPrintQueueOperations;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("124be5c0-156e-11cf-a986-00aa006bc149") IADsPrintQueueOperations:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Status (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE PrintJobs (IADsCollection ** pObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE Pause (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Resume (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Purge (void) = 0;
};
#else
typedef struct IADsPrintQueueOperationsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsPrintQueueOperations * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsPrintQueueOperations * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsPrintQueueOperations * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsPrintQueueOperations * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsPrintQueueOperations * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsPrintQueueOperations * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsPrintQueueOperations * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsPrintQueueOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsPrintQueueOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsPrintQueueOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsPrintQueueOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsPrintQueueOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsPrintQueueOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsPrintQueueOperations * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsPrintQueueOperations * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsPrintQueueOperations * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsPrintQueueOperations * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsPrintQueueOperations * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsPrintQueueOperations * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsPrintQueueOperations * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_Status) (IADsPrintQueueOperations * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * PrintJobs) (IADsPrintQueueOperations * This, IADsCollection ** pObject);
    HRESULT (STDMETHODCALLTYPE * Pause) (IADsPrintQueueOperations * This);
    HRESULT (STDMETHODCALLTYPE * Resume) (IADsPrintQueueOperations * This);
    HRESULT (STDMETHODCALLTYPE * Purge) (IADsPrintQueueOperations * This);
    END_INTERFACE
} IADsPrintQueueOperationsVtbl;
interface IADsPrintQueueOperations {
    CONST_VTBL struct IADsPrintQueueOperationsVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsPrintQueueOperations_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsPrintQueueOperations_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsPrintQueueOperations_Release(This)  (This)->lpVtbl->Release(This)
#define IADsPrintQueueOperations_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsPrintQueueOperations_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsPrintQueueOperations_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsPrintQueueOperations_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsPrintQueueOperations_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsPrintQueueOperations_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsPrintQueueOperations_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsPrintQueueOperations_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsPrintQueueOperations_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsPrintQueueOperations_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsPrintQueueOperations_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsPrintQueueOperations_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsPrintQueueOperations_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsPrintQueueOperations_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsPrintQueueOperations_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsPrintQueueOperations_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsPrintQueueOperations_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsPrintQueueOperations_get_Status(This,retval)  (This)->lpVtbl->get_Status(This,retval)
#define IADsPrintQueueOperations_PrintJobs(This,pObject)  (This)->lpVtbl->PrintJobs(This,pObject)
#define IADsPrintQueueOperations_Pause(This)  (This)->lpVtbl->Pause(This)
#define IADsPrintQueueOperations_Resume(This)  (This)->lpVtbl->Resume(This)
#define IADsPrintQueueOperations_Purge(This)  (This)->lpVtbl->Purge(This)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsPrintQueueOperations_get_Status_Proxy (IADsPrintQueueOperations * This, long *retval);
void __RPC_STUB IADsPrintQueueOperations_get_Status_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueueOperations_PrintJobs_Proxy (IADsPrintQueueOperations * This, IADsCollection ** pObject);
void __RPC_STUB IADsPrintQueueOperations_PrintJobs_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueueOperations_Pause_Proxy (IADsPrintQueueOperations * This);
void __RPC_STUB IADsPrintQueueOperations_Pause_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueueOperations_Resume_Proxy (IADsPrintQueueOperations * This);
void __RPC_STUB IADsPrintQueueOperations_Resume_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintQueueOperations_Purge_Proxy (IADsPrintQueueOperations * This);
void __RPC_STUB IADsPrintQueueOperations_Purge_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsPrintJob_INTERFACE_DEFINED__
#define __IADsPrintJob_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsPrintJob;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("32fb6780-1ed0-11cf-a988-00aa006bc149") IADsPrintJob:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_HostPrintQueue (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_User (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UserPath (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TimeSubmitted (DATE * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TotalPages (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Size (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Description (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Description (BSTR bstrDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Priority (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Priority (long lnPriority) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StartTime (DATE * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StartTime (DATE daStartTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UntilTime (DATE * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_UntilTime (DATE daUntilTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Notify (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Notify (BSTR bstrNotify) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NotifyPath (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NotifyPath (BSTR bstrNotifyPath) = 0;
};
#else
typedef struct IADsPrintJobVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsPrintJob * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsPrintJob * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsPrintJob * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsPrintJob * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsPrintJob * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsPrintJob * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsPrintJob * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsPrintJob * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsPrintJob * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsPrintJob * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsPrintJob * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsPrintJob * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsPrintJob * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsPrintJob * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsPrintJob * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsPrintJob * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsPrintJob * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsPrintJob * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsPrintJob * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsPrintJob * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_HostPrintQueue) (IADsPrintJob * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_User) (IADsPrintJob * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_UserPath) (IADsPrintJob * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_TimeSubmitted) (IADsPrintJob * This, DATE * retval);
    HRESULT (STDMETHODCALLTYPE * get_TotalPages) (IADsPrintJob * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * get_Size) (IADsPrintJob * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * get_Description) (IADsPrintJob * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Description) (IADsPrintJob * This, BSTR bstrDescription);
    HRESULT (STDMETHODCALLTYPE * get_Priority) (IADsPrintJob * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Priority) (IADsPrintJob * This, long lnPriority);
    HRESULT (STDMETHODCALLTYPE * get_StartTime) (IADsPrintJob * This, DATE * retval);
    HRESULT (STDMETHODCALLTYPE * put_StartTime) (IADsPrintJob * This, DATE daStartTime);
    HRESULT (STDMETHODCALLTYPE * get_UntilTime) (IADsPrintJob * This, DATE * retval);
    HRESULT (STDMETHODCALLTYPE * put_UntilTime) (IADsPrintJob * This, DATE daUntilTime);
    HRESULT (STDMETHODCALLTYPE * get_Notify) (IADsPrintJob * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Notify) (IADsPrintJob * This, BSTR bstrNotify);
    HRESULT (STDMETHODCALLTYPE * get_NotifyPath) (IADsPrintJob * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_NotifyPath) (IADsPrintJob * This, BSTR bstrNotifyPath);
    END_INTERFACE
} IADsPrintJobVtbl;
interface IADsPrintJob {
    CONST_VTBL struct IADsPrintJobVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsPrintJob_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsPrintJob_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsPrintJob_Release(This)  (This)->lpVtbl->Release(This)
#define IADsPrintJob_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsPrintJob_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsPrintJob_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsPrintJob_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsPrintJob_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsPrintJob_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsPrintJob_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsPrintJob_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsPrintJob_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsPrintJob_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsPrintJob_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsPrintJob_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsPrintJob_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsPrintJob_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsPrintJob_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsPrintJob_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsPrintJob_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsPrintJob_get_HostPrintQueue(This,retval)  (This)->lpVtbl->get_HostPrintQueue(This,retval)
#define IADsPrintJob_get_User(This,retval)  (This)->lpVtbl->get_User(This,retval)
#define IADsPrintJob_get_UserPath(This,retval)  (This)->lpVtbl->get_UserPath(This,retval)
#define IADsPrintJob_get_TimeSubmitted(This,retval)  (This)->lpVtbl->get_TimeSubmitted(This,retval)
#define IADsPrintJob_get_TotalPages(This,retval)  (This)->lpVtbl->get_TotalPages(This,retval)
#define IADsPrintJob_get_Size(This,retval)  (This)->lpVtbl->get_Size(This,retval)
#define IADsPrintJob_get_Description(This,retval)  (This)->lpVtbl->get_Description(This,retval)
#define IADsPrintJob_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IADsPrintJob_get_Priority(This,retval)  (This)->lpVtbl->get_Priority(This,retval)
#define IADsPrintJob_put_Priority(This,lnPriority)  (This)->lpVtbl->put_Priority(This,lnPriority)
#define IADsPrintJob_get_StartTime(This,retval)  (This)->lpVtbl->get_StartTime(This,retval)
#define IADsPrintJob_put_StartTime(This,daStartTime)  (This)->lpVtbl->put_StartTime(This,daStartTime)
#define IADsPrintJob_get_UntilTime(This,retval)  (This)->lpVtbl->get_UntilTime(This,retval)
#define IADsPrintJob_put_UntilTime(This,daUntilTime)  (This)->lpVtbl->put_UntilTime(This,daUntilTime)
#define IADsPrintJob_get_Notify(This,retval)  (This)->lpVtbl->get_Notify(This,retval)
#define IADsPrintJob_put_Notify(This,bstrNotify)  (This)->lpVtbl->put_Notify(This,bstrNotify)
#define IADsPrintJob_get_NotifyPath(This,retval)  (This)->lpVtbl->get_NotifyPath(This,retval)
#define IADsPrintJob_put_NotifyPath(This,bstrNotifyPath)  (This)->lpVtbl->put_NotifyPath(This,bstrNotifyPath)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsPrintJob_get_HostPrintQueue_Proxy (IADsPrintJob * This, BSTR * retval);
void __RPC_STUB IADsPrintJob_get_HostPrintQueue_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_get_User_Proxy (IADsPrintJob * This, BSTR * retval);
void __RPC_STUB IADsPrintJob_get_User_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_get_UserPath_Proxy (IADsPrintJob * This, BSTR * retval);
void __RPC_STUB IADsPrintJob_get_UserPath_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_get_TimeSubmitted_Proxy (IADsPrintJob * This, DATE * retval);
void __RPC_STUB IADsPrintJob_get_TimeSubmitted_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_get_TotalPages_Proxy (IADsPrintJob * This, long *retval);
void __RPC_STUB IADsPrintJob_get_TotalPages_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_get_Size_Proxy (IADsPrintJob * This, long *retval);
void __RPC_STUB IADsPrintJob_get_Size_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_get_Description_Proxy (IADsPrintJob * This, BSTR * retval);
void __RPC_STUB IADsPrintJob_get_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_put_Description_Proxy (IADsPrintJob * This, BSTR bstrDescription);
void __RPC_STUB IADsPrintJob_put_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_get_Priority_Proxy (IADsPrintJob * This, long *retval);
void __RPC_STUB IADsPrintJob_get_Priority_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_put_Priority_Proxy (IADsPrintJob * This, long lnPriority);
void __RPC_STUB IADsPrintJob_put_Priority_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_get_StartTime_Proxy (IADsPrintJob * This, DATE * retval);
void __RPC_STUB IADsPrintJob_get_StartTime_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_put_StartTime_Proxy (IADsPrintJob * This, DATE daStartTime);
void __RPC_STUB IADsPrintJob_put_StartTime_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_get_UntilTime_Proxy (IADsPrintJob * This, DATE * retval);
void __RPC_STUB IADsPrintJob_get_UntilTime_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_put_UntilTime_Proxy (IADsPrintJob * This, DATE daUntilTime);
void __RPC_STUB IADsPrintJob_put_UntilTime_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_get_Notify_Proxy (IADsPrintJob * This, BSTR * retval);
void __RPC_STUB IADsPrintJob_get_Notify_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_put_Notify_Proxy (IADsPrintJob * This, BSTR bstrNotify);
void __RPC_STUB IADsPrintJob_put_Notify_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_get_NotifyPath_Proxy (IADsPrintJob * This, BSTR * retval);
void __RPC_STUB IADsPrintJob_get_NotifyPath_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJob_put_NotifyPath_Proxy (IADsPrintJob * This, BSTR bstrNotifyPath);
void __RPC_STUB IADsPrintJob_put_NotifyPath_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsPrintJobOperations_INTERFACE_DEFINED__
#define __IADsPrintJobOperations_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsPrintJobOperations;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("9a52db30-1ecf-11cf-a988-00aa006bc149") IADsPrintJobOperations:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Status (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TimeElapsed (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PagesPrinted (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Position (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Position (long lnPosition) = 0;
    virtual HRESULT STDMETHODCALLTYPE Pause (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Resume (void) = 0;
};
#else
typedef struct IADsPrintJobOperationsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsPrintJobOperations * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsPrintJobOperations * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsPrintJobOperations * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsPrintJobOperations * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsPrintJobOperations * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsPrintJobOperations * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsPrintJobOperations * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsPrintJobOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsPrintJobOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsPrintJobOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsPrintJobOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsPrintJobOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsPrintJobOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsPrintJobOperations * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsPrintJobOperations * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsPrintJobOperations * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsPrintJobOperations * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsPrintJobOperations * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsPrintJobOperations * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsPrintJobOperations * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_Status) (IADsPrintJobOperations * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * get_TimeElapsed) (IADsPrintJobOperations * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * get_PagesPrinted) (IADsPrintJobOperations * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * get_Position) (IADsPrintJobOperations * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Position) (IADsPrintJobOperations * This, long lnPosition);
    HRESULT (STDMETHODCALLTYPE * Pause) (IADsPrintJobOperations * This);
    HRESULT (STDMETHODCALLTYPE * Resume) (IADsPrintJobOperations * This);
    END_INTERFACE
} IADsPrintJobOperationsVtbl;
interface IADsPrintJobOperations {
    CONST_VTBL struct IADsPrintJobOperationsVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsPrintJobOperations_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsPrintJobOperations_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsPrintJobOperations_Release(This)  (This)->lpVtbl->Release(This)
#define IADsPrintJobOperations_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsPrintJobOperations_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsPrintJobOperations_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsPrintJobOperations_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsPrintJobOperations_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsPrintJobOperations_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsPrintJobOperations_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsPrintJobOperations_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsPrintJobOperations_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsPrintJobOperations_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsPrintJobOperations_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsPrintJobOperations_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsPrintJobOperations_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsPrintJobOperations_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsPrintJobOperations_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsPrintJobOperations_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsPrintJobOperations_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsPrintJobOperations_get_Status(This,retval)  (This)->lpVtbl->get_Status(This,retval)
#define IADsPrintJobOperations_get_TimeElapsed(This,retval)  (This)->lpVtbl->get_TimeElapsed(This,retval)
#define IADsPrintJobOperations_get_PagesPrinted(This,retval)  (This)->lpVtbl->get_PagesPrinted(This,retval)
#define IADsPrintJobOperations_get_Position(This,retval)  (This)->lpVtbl->get_Position(This,retval)
#define IADsPrintJobOperations_put_Position(This,lnPosition)  (This)->lpVtbl->put_Position(This,lnPosition)
#define IADsPrintJobOperations_Pause(This)  (This)->lpVtbl->Pause(This)
#define IADsPrintJobOperations_Resume(This)  (This)->lpVtbl->Resume(This)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsPrintJobOperations_get_Status_Proxy (IADsPrintJobOperations * This, long *retval);
void __RPC_STUB IADsPrintJobOperations_get_Status_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJobOperations_get_TimeElapsed_Proxy (IADsPrintJobOperations * This, long *retval);
void __RPC_STUB IADsPrintJobOperations_get_TimeElapsed_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJobOperations_get_PagesPrinted_Proxy (IADsPrintJobOperations * This, long *retval);
void __RPC_STUB IADsPrintJobOperations_get_PagesPrinted_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJobOperations_get_Position_Proxy (IADsPrintJobOperations * This, long *retval);
void __RPC_STUB IADsPrintJobOperations_get_Position_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJobOperations_put_Position_Proxy (IADsPrintJobOperations * This, long lnPosition);
void __RPC_STUB IADsPrintJobOperations_put_Position_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJobOperations_Pause_Proxy (IADsPrintJobOperations * This);
void __RPC_STUB IADsPrintJobOperations_Pause_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPrintJobOperations_Resume_Proxy (IADsPrintJobOperations * This);
void __RPC_STUB IADsPrintJobOperations_Resume_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsService_INTERFACE_DEFINED__
#define __IADsService_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsService;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("68af66e0-31ca-11cf-a98a-00aa006bc149") IADsService:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_HostComputer (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HostComputer (BSTR bstrHostComputer) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisplayName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DisplayName (BSTR bstrDisplayName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Version (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Version (BSTR bstrVersion) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ServiceType (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ServiceType (long lnServiceType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StartType (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StartType (long lnStartType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Path (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Path (BSTR bstrPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StartupParameters (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StartupParameters (BSTR bstrStartupParameters) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ErrorControl (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ErrorControl (long lnErrorControl) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LoadOrderGroup (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LoadOrderGroup (BSTR bstrLoadOrderGroup) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ServiceAccountName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ServiceAccountName (BSTR bstrServiceAccountName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ServiceAccountPath (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ServiceAccountPath (BSTR bstrServiceAccountPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Dependencies (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Dependencies (VARIANT vDependencies) = 0;
};
#else
typedef struct IADsServiceVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsService * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsService * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsService * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsService * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsService * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsService * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsService * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsService * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsService * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsService * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsService * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsService * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsService * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsService * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_HostComputer) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_HostComputer) (IADsService * This, BSTR bstrHostComputer);
    HRESULT (STDMETHODCALLTYPE * get_DisplayName) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_DisplayName) (IADsService * This, BSTR bstrDisplayName);
    HRESULT (STDMETHODCALLTYPE * get_Version) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Version) (IADsService * This, BSTR bstrVersion);
    HRESULT (STDMETHODCALLTYPE * get_ServiceType) (IADsService * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_ServiceType) (IADsService * This, long lnServiceType);
    HRESULT (STDMETHODCALLTYPE * get_StartType) (IADsService * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_StartType) (IADsService * This, long lnStartType);
    HRESULT (STDMETHODCALLTYPE * get_Path) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Path) (IADsService * This, BSTR bstrPath);
    HRESULT (STDMETHODCALLTYPE * get_StartupParameters) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_StartupParameters) (IADsService * This, BSTR bstrStartupParameters);
    HRESULT (STDMETHODCALLTYPE * get_ErrorControl) (IADsService * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_ErrorControl) (IADsService * This, long lnErrorControl);
    HRESULT (STDMETHODCALLTYPE * get_LoadOrderGroup) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_LoadOrderGroup) (IADsService * This, BSTR bstrLoadOrderGroup);
    HRESULT (STDMETHODCALLTYPE * get_ServiceAccountName) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_ServiceAccountName) (IADsService * This, BSTR bstrServiceAccountName);
    HRESULT (STDMETHODCALLTYPE * get_ServiceAccountPath) (IADsService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_ServiceAccountPath) (IADsService * This, BSTR bstrServiceAccountPath);
    HRESULT (STDMETHODCALLTYPE * get_Dependencies) (IADsService * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_Dependencies) (IADsService * This, VARIANT vDependencies);
    END_INTERFACE
} IADsServiceVtbl;
interface IADsService {
    CONST_VTBL struct IADsServiceVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsService_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsService_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsService_Release(This)  (This)->lpVtbl->Release(This)
#define IADsService_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsService_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsService_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsService_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsService_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsService_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsService_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsService_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsService_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsService_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsService_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsService_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsService_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsService_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsService_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsService_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsService_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsService_get_HostComputer(This,retval)  (This)->lpVtbl->get_HostComputer(This,retval)
#define IADsService_put_HostComputer(This,bstrHostComputer)  (This)->lpVtbl->put_HostComputer(This,bstrHostComputer)
#define IADsService_get_DisplayName(This,retval)  (This)->lpVtbl->get_DisplayName(This,retval)
#define IADsService_put_DisplayName(This,bstrDisplayName)  (This)->lpVtbl->put_DisplayName(This,bstrDisplayName)
#define IADsService_get_Version(This,retval)  (This)->lpVtbl->get_Version(This,retval)
#define IADsService_put_Version(This,bstrVersion)  (This)->lpVtbl->put_Version(This,bstrVersion)
#define IADsService_get_ServiceType(This,retval)  (This)->lpVtbl->get_ServiceType(This,retval)
#define IADsService_put_ServiceType(This,lnServiceType)  (This)->lpVtbl->put_ServiceType(This,lnServiceType)
#define IADsService_get_StartType(This,retval)  (This)->lpVtbl->get_StartType(This,retval)
#define IADsService_put_StartType(This,lnStartType)  (This)->lpVtbl->put_StartType(This,lnStartType)
#define IADsService_get_Path(This,retval)  (This)->lpVtbl->get_Path(This,retval)
#define IADsService_put_Path(This,bstrPath)  (This)->lpVtbl->put_Path(This,bstrPath)
#define IADsService_get_StartupParameters(This,retval)  (This)->lpVtbl->get_StartupParameters(This,retval)
#define IADsService_put_StartupParameters(This,bstrStartupParameters)  (This)->lpVtbl->put_StartupParameters(This,bstrStartupParameters)
#define IADsService_get_ErrorControl(This,retval)  (This)->lpVtbl->get_ErrorControl(This,retval)
#define IADsService_put_ErrorControl(This,lnErrorControl)  (This)->lpVtbl->put_ErrorControl(This,lnErrorControl)
#define IADsService_get_LoadOrderGroup(This,retval)  (This)->lpVtbl->get_LoadOrderGroup(This,retval)
#define IADsService_put_LoadOrderGroup(This,bstrLoadOrderGroup)  (This)->lpVtbl->put_LoadOrderGroup(This,bstrLoadOrderGroup)
#define IADsService_get_ServiceAccountName(This,retval)  (This)->lpVtbl->get_ServiceAccountName(This,retval)
#define IADsService_put_ServiceAccountName(This,bstrServiceAccountName)  (This)->lpVtbl->put_ServiceAccountName(This,bstrServiceAccountName)
#define IADsService_get_ServiceAccountPath(This,retval)  (This)->lpVtbl->get_ServiceAccountPath(This,retval)
#define IADsService_put_ServiceAccountPath(This,bstrServiceAccountPath)  (This)->lpVtbl->put_ServiceAccountPath(This,bstrServiceAccountPath)
#define IADsService_get_Dependencies(This,retval)  (This)->lpVtbl->get_Dependencies(This,retval)
#define IADsService_put_Dependencies(This,vDependencies)  (This)->lpVtbl->put_Dependencies(This,vDependencies)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsService_get_HostComputer_Proxy (IADsService * This, BSTR * retval);
void __RPC_STUB IADsService_get_HostComputer_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_put_HostComputer_Proxy (IADsService * This, BSTR bstrHostComputer);
void __RPC_STUB IADsService_put_HostComputer_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_get_DisplayName_Proxy (IADsService * This, BSTR * retval);
void __RPC_STUB IADsService_get_DisplayName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_put_DisplayName_Proxy (IADsService * This, BSTR bstrDisplayName);
void __RPC_STUB IADsService_put_DisplayName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_get_Version_Proxy (IADsService * This, BSTR * retval);
void __RPC_STUB IADsService_get_Version_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_put_Version_Proxy (IADsService * This, BSTR bstrVersion);
void __RPC_STUB IADsService_put_Version_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_get_ServiceType_Proxy (IADsService * This, long *retval);
void __RPC_STUB IADsService_get_ServiceType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_put_ServiceType_Proxy (IADsService * This, long lnServiceType);
void __RPC_STUB IADsService_put_ServiceType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_get_StartType_Proxy (IADsService * This, long *retval);
void __RPC_STUB IADsService_get_StartType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_put_StartType_Proxy (IADsService * This, long lnStartType);
void __RPC_STUB IADsService_put_StartType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_get_Path_Proxy (IADsService * This, BSTR * retval);
void __RPC_STUB IADsService_get_Path_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_put_Path_Proxy (IADsService * This, BSTR bstrPath);
void __RPC_STUB IADsService_put_Path_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_get_StartupParameters_Proxy (IADsService * This, BSTR * retval);
void __RPC_STUB IADsService_get_StartupParameters_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_put_StartupParameters_Proxy (IADsService * This, BSTR bstrStartupParameters);
void __RPC_STUB IADsService_put_StartupParameters_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_get_ErrorControl_Proxy (IADsService * This, long *retval);
void __RPC_STUB IADsService_get_ErrorControl_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_put_ErrorControl_Proxy (IADsService * This, long lnErrorControl);
void __RPC_STUB IADsService_put_ErrorControl_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_get_LoadOrderGroup_Proxy (IADsService * This, BSTR * retval);
void __RPC_STUB IADsService_get_LoadOrderGroup_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_put_LoadOrderGroup_Proxy (IADsService * This, BSTR bstrLoadOrderGroup);
void __RPC_STUB IADsService_put_LoadOrderGroup_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_get_ServiceAccountName_Proxy (IADsService * This, BSTR * retval);
void __RPC_STUB IADsService_get_ServiceAccountName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_put_ServiceAccountName_Proxy (IADsService * This, BSTR bstrServiceAccountName);
void __RPC_STUB IADsService_put_ServiceAccountName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_get_ServiceAccountPath_Proxy (IADsService * This, BSTR * retval);
void __RPC_STUB IADsService_get_ServiceAccountPath_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_put_ServiceAccountPath_Proxy (IADsService * This, BSTR bstrServiceAccountPath);
void __RPC_STUB IADsService_put_ServiceAccountPath_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_get_Dependencies_Proxy (IADsService * This, VARIANT * retval);
void __RPC_STUB IADsService_get_Dependencies_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsService_put_Dependencies_Proxy (IADsService * This, VARIANT vDependencies);
void __RPC_STUB IADsService_put_Dependencies_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsServiceOperations_INTERFACE_DEFINED__
#define __IADsServiceOperations_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsServiceOperations;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("5d7b33f0-31ca-11cf-a98a-00aa006bc149") IADsServiceOperations:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Status (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE Start (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Stop (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Pause (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Continue (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetPassword (BSTR bstrNewPassword) = 0;
};
#else
typedef struct IADsServiceOperationsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsServiceOperations * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsServiceOperations * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsServiceOperations * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsServiceOperations * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsServiceOperations * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsServiceOperations * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsServiceOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsServiceOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsServiceOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsServiceOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsServiceOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsServiceOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsServiceOperations * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsServiceOperations * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsServiceOperations * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsServiceOperations * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsServiceOperations * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_Status) (IADsServiceOperations * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * Start) (IADsServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * Stop) (IADsServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * Pause) (IADsServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * Continue) (IADsServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * SetPassword) (IADsServiceOperations * This, BSTR bstrNewPassword);
    END_INTERFACE
} IADsServiceOperationsVtbl;
interface IADsServiceOperations {
    CONST_VTBL struct IADsServiceOperationsVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsServiceOperations_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsServiceOperations_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsServiceOperations_Release(This)  (This)->lpVtbl->Release(This)
#define IADsServiceOperations_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsServiceOperations_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsServiceOperations_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsServiceOperations_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsServiceOperations_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsServiceOperations_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsServiceOperations_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsServiceOperations_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsServiceOperations_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsServiceOperations_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsServiceOperations_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsServiceOperations_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsServiceOperations_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsServiceOperations_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsServiceOperations_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsServiceOperations_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsServiceOperations_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsServiceOperations_get_Status(This,retval)  (This)->lpVtbl->get_Status(This,retval)
#define IADsServiceOperations_Start(This)  (This)->lpVtbl->Start(This)
#define IADsServiceOperations_Stop(This)  (This)->lpVtbl->Stop(This)
#define IADsServiceOperations_Pause(This)  (This)->lpVtbl->Pause(This)
#define IADsServiceOperations_Continue(This)  (This)->lpVtbl->Continue(This)
#define IADsServiceOperations_SetPassword(This,bstrNewPassword)  (This)->lpVtbl->SetPassword(This,bstrNewPassword)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsServiceOperations_get_Status_Proxy (IADsServiceOperations * This, long *retval);
void __RPC_STUB IADsServiceOperations_get_Status_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsServiceOperations_Start_Proxy (IADsServiceOperations * This);
void __RPC_STUB IADsServiceOperations_Start_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsServiceOperations_Stop_Proxy (IADsServiceOperations * This);
void __RPC_STUB IADsServiceOperations_Stop_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsServiceOperations_Pause_Proxy (IADsServiceOperations * This);
void __RPC_STUB IADsServiceOperations_Pause_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsServiceOperations_Continue_Proxy (IADsServiceOperations * This);
void __RPC_STUB IADsServiceOperations_Continue_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsServiceOperations_SetPassword_Proxy (IADsServiceOperations * This, BSTR bstrNewPassword);
void __RPC_STUB IADsServiceOperations_SetPassword_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsFileService_INTERFACE_DEFINED__
#define __IADsFileService_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsFileService;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("a89d1900-31ca-11cf-a98a-00aa006bc149") IADsFileService:public IADsService {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Description (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Description (BSTR bstrDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MaxUserCount (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MaxUserCount (long lnMaxUserCount) = 0;
};
#else
typedef struct IADsFileServiceVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsFileService * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsFileService * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsFileService * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsFileService * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsFileService * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsFileService * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsFileService * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsFileService * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsFileService * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsFileService * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsFileService * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsFileService * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsFileService * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsFileService * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_HostComputer) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_HostComputer) (IADsFileService * This, BSTR bstrHostComputer);
    HRESULT (STDMETHODCALLTYPE * get_DisplayName) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_DisplayName) (IADsFileService * This, BSTR bstrDisplayName);
    HRESULT (STDMETHODCALLTYPE * get_Version) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Version) (IADsFileService * This, BSTR bstrVersion);
    HRESULT (STDMETHODCALLTYPE * get_ServiceType) (IADsFileService * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_ServiceType) (IADsFileService * This, long lnServiceType);
    HRESULT (STDMETHODCALLTYPE * get_StartType) (IADsFileService * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_StartType) (IADsFileService * This, long lnStartType);
    HRESULT (STDMETHODCALLTYPE * get_Path) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Path) (IADsFileService * This, BSTR bstrPath);
    HRESULT (STDMETHODCALLTYPE * get_StartupParameters) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_StartupParameters) (IADsFileService * This, BSTR bstrStartupParameters);
    HRESULT (STDMETHODCALLTYPE * get_ErrorControl) (IADsFileService * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_ErrorControl) (IADsFileService * This, long lnErrorControl);
    HRESULT (STDMETHODCALLTYPE * get_LoadOrderGroup) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_LoadOrderGroup) (IADsFileService * This, BSTR bstrLoadOrderGroup);
    HRESULT (STDMETHODCALLTYPE * get_ServiceAccountName) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_ServiceAccountName) (IADsFileService * This, BSTR bstrServiceAccountName);
    HRESULT (STDMETHODCALLTYPE * get_ServiceAccountPath) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_ServiceAccountPath) (IADsFileService * This, BSTR bstrServiceAccountPath);
    HRESULT (STDMETHODCALLTYPE * get_Dependencies) (IADsFileService * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_Dependencies) (IADsFileService * This, VARIANT vDependencies);
    HRESULT (STDMETHODCALLTYPE * get_Description) (IADsFileService * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Description) (IADsFileService * This, BSTR bstrDescription);
    HRESULT (STDMETHODCALLTYPE * get_MaxUserCount) (IADsFileService * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_MaxUserCount) (IADsFileService * This, long lnMaxUserCount);
    END_INTERFACE
} IADsFileServiceVtbl;
interface IADsFileService {
    CONST_VTBL struct IADsFileServiceVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsFileService_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsFileService_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsFileService_Release(This)  (This)->lpVtbl->Release(This)
#define IADsFileService_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsFileService_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsFileService_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsFileService_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsFileService_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsFileService_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsFileService_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsFileService_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsFileService_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsFileService_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsFileService_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsFileService_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsFileService_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsFileService_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsFileService_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsFileService_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsFileService_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsFileService_get_HostComputer(This,retval)  (This)->lpVtbl->get_HostComputer(This,retval)
#define IADsFileService_put_HostComputer(This,bstrHostComputer)  (This)->lpVtbl->put_HostComputer(This,bstrHostComputer)
#define IADsFileService_get_DisplayName(This,retval)  (This)->lpVtbl->get_DisplayName(This,retval)
#define IADsFileService_put_DisplayName(This,bstrDisplayName)  (This)->lpVtbl->put_DisplayName(This,bstrDisplayName)
#define IADsFileService_get_Version(This,retval)  (This)->lpVtbl->get_Version(This,retval)
#define IADsFileService_put_Version(This,bstrVersion)  (This)->lpVtbl->put_Version(This,bstrVersion)
#define IADsFileService_get_ServiceType(This,retval)  (This)->lpVtbl->get_ServiceType(This,retval)
#define IADsFileService_put_ServiceType(This,lnServiceType)  (This)->lpVtbl->put_ServiceType(This,lnServiceType)
#define IADsFileService_get_StartType(This,retval)  (This)->lpVtbl->get_StartType(This,retval)
#define IADsFileService_put_StartType(This,lnStartType)  (This)->lpVtbl->put_StartType(This,lnStartType)
#define IADsFileService_get_Path(This,retval)  (This)->lpVtbl->get_Path(This,retval)
#define IADsFileService_put_Path(This,bstrPath)  (This)->lpVtbl->put_Path(This,bstrPath)
#define IADsFileService_get_StartupParameters(This,retval)  (This)->lpVtbl->get_StartupParameters(This,retval)
#define IADsFileService_put_StartupParameters(This,bstrStartupParameters)  (This)->lpVtbl->put_StartupParameters(This,bstrStartupParameters)
#define IADsFileService_get_ErrorControl(This,retval)  (This)->lpVtbl->get_ErrorControl(This,retval)
#define IADsFileService_put_ErrorControl(This,lnErrorControl)  (This)->lpVtbl->put_ErrorControl(This,lnErrorControl)
#define IADsFileService_get_LoadOrderGroup(This,retval)  (This)->lpVtbl->get_LoadOrderGroup(This,retval)
#define IADsFileService_put_LoadOrderGroup(This,bstrLoadOrderGroup)  (This)->lpVtbl->put_LoadOrderGroup(This,bstrLoadOrderGroup)
#define IADsFileService_get_ServiceAccountName(This,retval)  (This)->lpVtbl->get_ServiceAccountName(This,retval)
#define IADsFileService_put_ServiceAccountName(This,bstrServiceAccountName)  (This)->lpVtbl->put_ServiceAccountName(This,bstrServiceAccountName)
#define IADsFileService_get_ServiceAccountPath(This,retval)  (This)->lpVtbl->get_ServiceAccountPath(This,retval)
#define IADsFileService_put_ServiceAccountPath(This,bstrServiceAccountPath)  (This)->lpVtbl->put_ServiceAccountPath(This,bstrServiceAccountPath)
#define IADsFileService_get_Dependencies(This,retval)  (This)->lpVtbl->get_Dependencies(This,retval)
#define IADsFileService_put_Dependencies(This,vDependencies)  (This)->lpVtbl->put_Dependencies(This,vDependencies)
#define IADsFileService_get_Description(This,retval)  (This)->lpVtbl->get_Description(This,retval)
#define IADsFileService_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IADsFileService_get_MaxUserCount(This,retval)  (This)->lpVtbl->get_MaxUserCount(This,retval)
#define IADsFileService_put_MaxUserCount(This,lnMaxUserCount)  (This)->lpVtbl->put_MaxUserCount(This,lnMaxUserCount)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsFileService_get_Description_Proxy (IADsFileService * This, BSTR * retval);
void __RPC_STUB IADsFileService_get_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFileService_put_Description_Proxy (IADsFileService * This, BSTR bstrDescription);
void __RPC_STUB IADsFileService_put_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFileService_get_MaxUserCount_Proxy (IADsFileService * This, long *retval);
void __RPC_STUB IADsFileService_get_MaxUserCount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFileService_put_MaxUserCount_Proxy (IADsFileService * This, long lnMaxUserCount);
void __RPC_STUB IADsFileService_put_MaxUserCount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsFileServiceOperations_INTERFACE_DEFINED__
#define __IADsFileServiceOperations_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsFileServiceOperations;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("a02ded10-31ca-11cf-a98a-00aa006bc149") IADsFileServiceOperations:public IADsServiceOperations {
    public:
    virtual HRESULT STDMETHODCALLTYPE Sessions (IADsCollection ** ppSessions) = 0;
    virtual HRESULT STDMETHODCALLTYPE Resources (IADsCollection ** ppResources) = 0;
};
#else
typedef struct IADsFileServiceOperationsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsFileServiceOperations * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsFileServiceOperations * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsFileServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsFileServiceOperations * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsFileServiceOperations * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsFileServiceOperations * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsFileServiceOperations * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsFileServiceOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsFileServiceOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsFileServiceOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsFileServiceOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsFileServiceOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsFileServiceOperations * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsFileServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsFileServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsFileServiceOperations * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsFileServiceOperations * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsFileServiceOperations * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsFileServiceOperations * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsFileServiceOperations * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_Status) (IADsFileServiceOperations * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * Start) (IADsFileServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * Stop) (IADsFileServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * Pause) (IADsFileServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * Continue) (IADsFileServiceOperations * This);
    HRESULT (STDMETHODCALLTYPE * SetPassword) (IADsFileServiceOperations * This, BSTR bstrNewPassword);
    HRESULT (STDMETHODCALLTYPE * Sessions) (IADsFileServiceOperations * This, IADsCollection ** ppSessions);
    HRESULT (STDMETHODCALLTYPE * Resources) (IADsFileServiceOperations * This, IADsCollection ** ppResources);
    END_INTERFACE
} IADsFileServiceOperationsVtbl;
interface IADsFileServiceOperations {
    CONST_VTBL struct IADsFileServiceOperationsVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsFileServiceOperations_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsFileServiceOperations_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsFileServiceOperations_Release(This)  (This)->lpVtbl->Release(This)
#define IADsFileServiceOperations_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsFileServiceOperations_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsFileServiceOperations_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsFileServiceOperations_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsFileServiceOperations_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsFileServiceOperations_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsFileServiceOperations_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsFileServiceOperations_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsFileServiceOperations_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsFileServiceOperations_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsFileServiceOperations_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsFileServiceOperations_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsFileServiceOperations_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsFileServiceOperations_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsFileServiceOperations_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsFileServiceOperations_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsFileServiceOperations_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsFileServiceOperations_get_Status(This,retval)  (This)->lpVtbl->get_Status(This,retval)
#define IADsFileServiceOperations_Start(This)  (This)->lpVtbl->Start(This)
#define IADsFileServiceOperations_Stop(This)  (This)->lpVtbl->Stop(This)
#define IADsFileServiceOperations_Pause(This)  (This)->lpVtbl->Pause(This)
#define IADsFileServiceOperations_Continue(This)  (This)->lpVtbl->Continue(This)
#define IADsFileServiceOperations_SetPassword(This,bstrNewPassword)  (This)->lpVtbl->SetPassword(This,bstrNewPassword)
#define IADsFileServiceOperations_Sessions(This,ppSessions)  (This)->lpVtbl->Sessions(This,ppSessions)
#define IADsFileServiceOperations_Resources(This,ppResources)  (This)->lpVtbl->Resources(This,ppResources)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsFileServiceOperations_Sessions_Proxy (IADsFileServiceOperations * This, IADsCollection ** ppSessions);
void __RPC_STUB IADsFileServiceOperations_Sessions_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFileServiceOperations_Resources_Proxy (IADsFileServiceOperations * This, IADsCollection ** ppResources);
void __RPC_STUB IADsFileServiceOperations_Resources_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsFileShare_INTERFACE_DEFINED__
#define __IADsFileShare_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsFileShare;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("eb6dcaf0-4b83-11cf-a995-00aa006bc149") IADsFileShare:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_CurrentUserCount (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Description (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Description (BSTR bstrDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HostComputer (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HostComputer (BSTR bstrHostComputer) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Path (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Path (BSTR bstrPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MaxUserCount (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MaxUserCount (long lnMaxUserCount) = 0;
};
#else
typedef struct IADsFileShareVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsFileShare * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsFileShare * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsFileShare * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsFileShare * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsFileShare * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsFileShare * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsFileShare * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsFileShare * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsFileShare * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsFileShare * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsFileShare * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsFileShare * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsFileShare * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsFileShare * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsFileShare * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsFileShare * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsFileShare * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsFileShare * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsFileShare * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsFileShare * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_CurrentUserCount) (IADsFileShare * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * get_Description) (IADsFileShare * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Description) (IADsFileShare * This, BSTR bstrDescription);
    HRESULT (STDMETHODCALLTYPE * get_HostComputer) (IADsFileShare * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_HostComputer) (IADsFileShare * This, BSTR bstrHostComputer);
    HRESULT (STDMETHODCALLTYPE * get_Path) (IADsFileShare * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Path) (IADsFileShare * This, BSTR bstrPath);
    HRESULT (STDMETHODCALLTYPE * get_MaxUserCount) (IADsFileShare * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_MaxUserCount) (IADsFileShare * This, long lnMaxUserCount);
    END_INTERFACE
} IADsFileShareVtbl;
interface IADsFileShare {
    CONST_VTBL struct IADsFileShareVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsFileShare_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsFileShare_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsFileShare_Release(This)  (This)->lpVtbl->Release(This)
#define IADsFileShare_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsFileShare_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsFileShare_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsFileShare_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsFileShare_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsFileShare_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsFileShare_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsFileShare_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsFileShare_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsFileShare_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsFileShare_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsFileShare_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsFileShare_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsFileShare_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsFileShare_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsFileShare_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsFileShare_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsFileShare_get_CurrentUserCount(This,retval)  (This)->lpVtbl->get_CurrentUserCount(This,retval)
#define IADsFileShare_get_Description(This,retval)  (This)->lpVtbl->get_Description(This,retval)
#define IADsFileShare_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IADsFileShare_get_HostComputer(This,retval)  (This)->lpVtbl->get_HostComputer(This,retval)
#define IADsFileShare_put_HostComputer(This,bstrHostComputer)  (This)->lpVtbl->put_HostComputer(This,bstrHostComputer)
#define IADsFileShare_get_Path(This,retval)  (This)->lpVtbl->get_Path(This,retval)
#define IADsFileShare_put_Path(This,bstrPath)  (This)->lpVtbl->put_Path(This,bstrPath)
#define IADsFileShare_get_MaxUserCount(This,retval)  (This)->lpVtbl->get_MaxUserCount(This,retval)
#define IADsFileShare_put_MaxUserCount(This,lnMaxUserCount)  (This)->lpVtbl->put_MaxUserCount(This,lnMaxUserCount)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsFileShare_get_CurrentUserCount_Proxy (IADsFileShare * This, long *retval);
void __RPC_STUB IADsFileShare_get_CurrentUserCount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFileShare_get_Description_Proxy (IADsFileShare * This, BSTR * retval);
void __RPC_STUB IADsFileShare_get_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFileShare_put_Description_Proxy (IADsFileShare * This, BSTR bstrDescription);
void __RPC_STUB IADsFileShare_put_Description_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFileShare_get_HostComputer_Proxy (IADsFileShare * This, BSTR * retval);
void __RPC_STUB IADsFileShare_get_HostComputer_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFileShare_put_HostComputer_Proxy (IADsFileShare * This, BSTR bstrHostComputer);
void __RPC_STUB IADsFileShare_put_HostComputer_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFileShare_get_Path_Proxy (IADsFileShare * This, BSTR * retval);
void __RPC_STUB IADsFileShare_get_Path_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFileShare_put_Path_Proxy (IADsFileShare * This, BSTR bstrPath);
void __RPC_STUB IADsFileShare_put_Path_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFileShare_get_MaxUserCount_Proxy (IADsFileShare * This, long *retval);
void __RPC_STUB IADsFileShare_get_MaxUserCount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFileShare_put_MaxUserCount_Proxy (IADsFileShare * This, long lnMaxUserCount);
void __RPC_STUB IADsFileShare_put_MaxUserCount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsSession_INTERFACE_DEFINED__
#define __IADsSession_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsSession;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("398b7da0-4aab-11cf-ae2c-00aa006ebfb9") IADsSession:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_User (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UserPath (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Computer (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ComputerPath (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ConnectTime (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IdleTime (long *retval) = 0;
};
#else
typedef struct IADsSessionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsSession * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsSession * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsSession * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsSession * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsSession * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsSession * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsSession * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsSession * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsSession * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsSession * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsSession * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsSession * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsSession * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsSession * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsSession * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsSession * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsSession * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsSession * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsSession * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsSession * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_User) (IADsSession * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_UserPath) (IADsSession * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Computer) (IADsSession * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ComputerPath) (IADsSession * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ConnectTime) (IADsSession * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * get_IdleTime) (IADsSession * This, long *retval);
    END_INTERFACE
} IADsSessionVtbl;
interface IADsSession {
    CONST_VTBL struct IADsSessionVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsSession_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsSession_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsSession_Release(This)  (This)->lpVtbl->Release(This)
#define IADsSession_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsSession_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsSession_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsSession_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsSession_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsSession_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsSession_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsSession_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsSession_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsSession_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsSession_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsSession_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsSession_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsSession_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsSession_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsSession_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsSession_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsSession_get_User(This,retval)  (This)->lpVtbl->get_User(This,retval)
#define IADsSession_get_UserPath(This,retval)  (This)->lpVtbl->get_UserPath(This,retval)
#define IADsSession_get_Computer(This,retval)  (This)->lpVtbl->get_Computer(This,retval)
#define IADsSession_get_ComputerPath(This,retval)  (This)->lpVtbl->get_ComputerPath(This,retval)
#define IADsSession_get_ConnectTime(This,retval)  (This)->lpVtbl->get_ConnectTime(This,retval)
#define IADsSession_get_IdleTime(This,retval)  (This)->lpVtbl->get_IdleTime(This,retval)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsSession_get_User_Proxy (IADsSession * This, BSTR * retval);
void __RPC_STUB IADsSession_get_User_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSession_get_UserPath_Proxy (IADsSession * This, BSTR * retval);
void __RPC_STUB IADsSession_get_UserPath_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSession_get_Computer_Proxy (IADsSession * This, BSTR * retval);
void __RPC_STUB IADsSession_get_Computer_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSession_get_ComputerPath_Proxy (IADsSession * This, BSTR * retval);
void __RPC_STUB IADsSession_get_ComputerPath_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSession_get_ConnectTime_Proxy (IADsSession * This, long *retval);
void __RPC_STUB IADsSession_get_ConnectTime_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSession_get_IdleTime_Proxy (IADsSession * This, long *retval);
void __RPC_STUB IADsSession_get_IdleTime_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsResource_INTERFACE_DEFINED__
#define __IADsResource_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsResource;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("34a05b20-4aab-11cf-ae2c-00aa006ebfb9") IADsResource:public IADs {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_User (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UserPath (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Path (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LockCount (long *retval) = 0;
};
#else
typedef struct IADsResourceVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsResource * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsResource * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsResource * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsResource * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsResource * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsResource * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsResource * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Name) (IADsResource * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Class) (IADsResource * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_GUID) (IADsResource * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ADsPath) (IADsResource * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Parent) (IADsResource * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Schema) (IADsResource * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * GetInfo) (IADsResource * This);
    HRESULT (STDMETHODCALLTYPE * SetInfo) (IADsResource * This);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsResource * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * Put) (IADsResource * This, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsResource * This, BSTR bstrName, VARIANT * pvProp);
    HRESULT (STDMETHODCALLTYPE * PutEx) (IADsResource * This, long lnControlCode, BSTR bstrName, VARIANT vProp);
    HRESULT (STDMETHODCALLTYPE * GetInfoEx) (IADsResource * This, VARIANT vProperties, long lnReserved);
    HRESULT (STDMETHODCALLTYPE * get_User) (IADsResource * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_UserPath) (IADsResource * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_Path) (IADsResource * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_LockCount) (IADsResource * This, long *retval);
    END_INTERFACE
} IADsResourceVtbl;
interface IADsResource {
    CONST_VTBL struct IADsResourceVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsResource_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsResource_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsResource_Release(This)  (This)->lpVtbl->Release(This)
#define IADsResource_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsResource_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsResource_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsResource_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsResource_get_Name(This,retval)  (This)->lpVtbl->get_Name(This,retval)
#define IADsResource_get_Class(This,retval)  (This)->lpVtbl->get_Class(This,retval)
#define IADsResource_get_GUID(This,retval)  (This)->lpVtbl->get_GUID(This,retval)
#define IADsResource_get_ADsPath(This,retval)  (This)->lpVtbl->get_ADsPath(This,retval)
#define IADsResource_get_Parent(This,retval)  (This)->lpVtbl->get_Parent(This,retval)
#define IADsResource_get_Schema(This,retval)  (This)->lpVtbl->get_Schema(This,retval)
#define IADsResource_GetInfo(This)  (This)->lpVtbl->GetInfo(This)
#define IADsResource_SetInfo(This)  (This)->lpVtbl->SetInfo(This)
#define IADsResource_Get(This,bstrName,pvProp)  (This)->lpVtbl->Get(This,bstrName,pvProp)
#define IADsResource_Put(This,bstrName,vProp)  (This)->lpVtbl->Put(This,bstrName,vProp)
#define IADsResource_GetEx(This,bstrName,pvProp)  (This)->lpVtbl->GetEx(This,bstrName,pvProp)
#define IADsResource_PutEx(This,lnControlCode,bstrName,vProp)  (This)->lpVtbl->PutEx(This,lnControlCode,bstrName,vProp)
#define IADsResource_GetInfoEx(This,vProperties,lnReserved)  (This)->lpVtbl->GetInfoEx(This,vProperties,lnReserved)
#define IADsResource_get_User(This,retval)  (This)->lpVtbl->get_User(This,retval)
#define IADsResource_get_UserPath(This,retval)  (This)->lpVtbl->get_UserPath(This,retval)
#define IADsResource_get_Path(This,retval)  (This)->lpVtbl->get_Path(This,retval)
#define IADsResource_get_LockCount(This,retval)  (This)->lpVtbl->get_LockCount(This,retval)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsResource_get_User_Proxy (IADsResource * This, BSTR * retval);
void __RPC_STUB IADsResource_get_User_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsResource_get_UserPath_Proxy (IADsResource * This, BSTR * retval);
void __RPC_STUB IADsResource_get_UserPath_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsResource_get_Path_Proxy (IADsResource * This, BSTR * retval);
void __RPC_STUB IADsResource_get_Path_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsResource_get_LockCount_Proxy (IADsResource * This, long *retval);
void __RPC_STUB IADsResource_get_LockCount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsOpenDSObject_INTERFACE_DEFINED__
#define __IADsOpenDSObject_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsOpenDSObject;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("ddf2891e-0f9c-11d0-8ad4-00c04fd8d503") IADsOpenDSObject:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE OpenDSObject (BSTR lpszDNName, BSTR lpszUserName, BSTR lpszPassword, long lnReserved, IDispatch ** ppOleDsObj) = 0;
};
#else
typedef struct IADsOpenDSObjectVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsOpenDSObject * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsOpenDSObject * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsOpenDSObject * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsOpenDSObject * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsOpenDSObject * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsOpenDSObject * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsOpenDSObject * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * OpenDSObject) (IADsOpenDSObject * This, BSTR lpszDNName, BSTR lpszUserName, BSTR lpszPassword, long lnReserved, IDispatch ** ppOleDsObj);
    END_INTERFACE
} IADsOpenDSObjectVtbl;
interface IADsOpenDSObject {
    CONST_VTBL struct IADsOpenDSObjectVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsOpenDSObject_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsOpenDSObject_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsOpenDSObject_Release(This)  (This)->lpVtbl->Release(This)
#define IADsOpenDSObject_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsOpenDSObject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsOpenDSObject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsOpenDSObject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsOpenDSObject_OpenDSObject(This,lpszDNName,lpszUserName,lpszPassword,lnReserved,ppOleDsObj)  (This)->lpVtbl->OpenDSObject(This,lpszDNName,lpszUserName,lpszPassword,lnReserved,ppOleDsObj)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsOpenDSObject_OpenDSObject_Proxy (IADsOpenDSObject * This, BSTR lpszDNName, BSTR lpszUserName, BSTR lpszPassword, long lnReserved, IDispatch ** ppOleDsObj);
void __RPC_STUB IADsOpenDSObject_OpenDSObject_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IDirectoryObject_INTERFACE_DEFINED__
#define __IDirectoryObject_INTERFACE_DEFINED__
EXTERN_C const IID IID_IDirectoryObject;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("e798de2c-22e4-11d0-84fe-00c04fd8d503") IDirectoryObject:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetObjectInformation (PADS_OBJECT_INFO * ppObjInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetObjectAttributes (LPWSTR * pAttributeNames, DWORD dwNumberAttributes, PADS_ATTR_INFO * ppAttributeEntries, DWORD * pdwNumAttributesReturned) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetObjectAttributes (PADS_ATTR_INFO pAttributeEntries, DWORD dwNumAttributes, DWORD * pdwNumAttributesModified) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateDSObject (LPWSTR pszRDNName, PADS_ATTR_INFO pAttributeEntries, DWORD dwNumAttributes, IDispatch ** ppObject) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteDSObject (LPWSTR pszRDNName) = 0;
};
#else
typedef struct IDirectoryObjectVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IDirectoryObject * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IDirectoryObject * This);
    ULONG (STDMETHODCALLTYPE * Release) (IDirectoryObject * This);
    HRESULT (STDMETHODCALLTYPE * GetObjectInformation) (IDirectoryObject * This, PADS_OBJECT_INFO * ppObjInfo);
    HRESULT (STDMETHODCALLTYPE * GetObjectAttributes) (IDirectoryObject * This, LPWSTR * pAttributeNames, DWORD dwNumberAttributes, PADS_ATTR_INFO * ppAttributeEntries, DWORD * pdwNumAttributesReturned);
    HRESULT (STDMETHODCALLTYPE * SetObjectAttributes) (IDirectoryObject * This, PADS_ATTR_INFO pAttributeEntries, DWORD dwNumAttributes, DWORD * pdwNumAttributesModified);
    HRESULT (STDMETHODCALLTYPE * CreateDSObject) (IDirectoryObject * This, LPWSTR pszRDNName, PADS_ATTR_INFO pAttributeEntries, DWORD dwNumAttributes, IDispatch ** ppObject);
    HRESULT (STDMETHODCALLTYPE * DeleteDSObject) (IDirectoryObject * This, LPWSTR pszRDNName);
    END_INTERFACE
} IDirectoryObjectVtbl;
interface IDirectoryObject {
    CONST_VTBL struct IDirectoryObjectVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IDirectoryObject_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IDirectoryObject_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IDirectoryObject_Release(This)  (This)->lpVtbl->Release(This)
#define IDirectoryObject_GetObjectInformation(This,ppObjInfo)  (This)->lpVtbl->GetObjectInformation(This,ppObjInfo)
#define IDirectoryObject_GetObjectAttributes(This,pAttributeNames,dwNumberAttributes,ppAttributeEntries,pdwNumAttributesReturned)  (This)->lpVtbl->GetObjectAttributes(This,pAttributeNames,dwNumberAttributes,ppAttributeEntries,pdwNumAttributesReturned)
#define IDirectoryObject_SetObjectAttributes(This,pAttributeEntries,dwNumAttributes,pdwNumAttributesModified)  (This)->lpVtbl->SetObjectAttributes(This,pAttributeEntries,dwNumAttributes,pdwNumAttributesModified)
#define IDirectoryObject_CreateDSObject(This,pszRDNName,pAttributeEntries,dwNumAttributes,ppObject)  (This)->lpVtbl->CreateDSObject(This,pszRDNName,pAttributeEntries,dwNumAttributes,ppObject)
#define IDirectoryObject_DeleteDSObject(This,pszRDNName)  (This)->lpVtbl->DeleteDSObject(This,pszRDNName)
#endif
#endif
HRESULT STDMETHODCALLTYPE IDirectoryObject_GetObjectInformation_Proxy (IDirectoryObject * This, PADS_OBJECT_INFO * ppObjInfo);
void __RPC_STUB IDirectoryObject_GetObjectInformation_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectoryObject_GetObjectAttributes_Proxy (IDirectoryObject * This, LPWSTR * pAttributeNames, DWORD dwNumberAttributes, PADS_ATTR_INFO * ppAttributeEntries, DWORD * pdwNumAttributesReturned);
void __RPC_STUB IDirectoryObject_GetObjectAttributes_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectoryObject_SetObjectAttributes_Proxy (IDirectoryObject * This, PADS_ATTR_INFO pAttributeEntries, DWORD dwNumAttributes, DWORD * pdwNumAttributesModified);
void __RPC_STUB IDirectoryObject_SetObjectAttributes_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectoryObject_CreateDSObject_Proxy (IDirectoryObject * This, LPWSTR pszRDNName, PADS_ATTR_INFO pAttributeEntries, DWORD dwNumAttributes, IDispatch ** ppObject);
void __RPC_STUB IDirectoryObject_CreateDSObject_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectoryObject_DeleteDSObject_Proxy (IDirectoryObject * This, LPWSTR pszRDNName);
void __RPC_STUB IDirectoryObject_DeleteDSObject_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IDirectorySearch_INTERFACE_DEFINED__
#define __IDirectorySearch_INTERFACE_DEFINED__
EXTERN_C const IID IID_IDirectorySearch;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("109ba8ec-92f0-11d0-a790-00c04fd8d5a8") IDirectorySearch:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE SetSearchPreference (PADS_SEARCHPREF_INFO pSearchPrefs, DWORD dwNumPrefs) = 0;
    virtual HRESULT STDMETHODCALLTYPE ExecuteSearch (LPWSTR pszSearchFilter, LPWSTR * pAttributeNames, DWORD dwNumberAttributes, PADS_SEARCH_HANDLE phSearchResult) = 0;
    virtual HRESULT STDMETHODCALLTYPE AbandonSearch (ADS_SEARCH_HANDLE phSearchResult) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFirstRow (ADS_SEARCH_HANDLE hSearchResult) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNextRow (ADS_SEARCH_HANDLE hSearchResult) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetPreviousRow (ADS_SEARCH_HANDLE hSearchResult) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNextColumnName (ADS_SEARCH_HANDLE hSearchHandle, LPWSTR * ppszColumnName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetColumn (ADS_SEARCH_HANDLE hSearchResult, LPWSTR szColumnName, PADS_SEARCH_COLUMN pSearchColumn) = 0;
    virtual HRESULT STDMETHODCALLTYPE FreeColumn (PADS_SEARCH_COLUMN pSearchColumn) = 0;
    virtual HRESULT STDMETHODCALLTYPE CloseSearchHandle (ADS_SEARCH_HANDLE hSearchResult) = 0;
};
#else
typedef struct IDirectorySearchVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IDirectorySearch * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IDirectorySearch * This);
    ULONG (STDMETHODCALLTYPE * Release) (IDirectorySearch * This);
    HRESULT (STDMETHODCALLTYPE * SetSearchPreference) (IDirectorySearch * This, PADS_SEARCHPREF_INFO pSearchPrefs, DWORD dwNumPrefs);
    HRESULT (STDMETHODCALLTYPE * ExecuteSearch) (IDirectorySearch * This, LPWSTR pszSearchFilter, LPWSTR * pAttributeNames, DWORD dwNumberAttributes, PADS_SEARCH_HANDLE phSearchResult);
    HRESULT (STDMETHODCALLTYPE * AbandonSearch) (IDirectorySearch * This, ADS_SEARCH_HANDLE phSearchResult);
    HRESULT (STDMETHODCALLTYPE * GetFirstRow) (IDirectorySearch * This, ADS_SEARCH_HANDLE hSearchResult);
    HRESULT (STDMETHODCALLTYPE * GetNextRow) (IDirectorySearch * This, ADS_SEARCH_HANDLE hSearchResult);
    HRESULT (STDMETHODCALLTYPE * GetPreviousRow) (IDirectorySearch * This, ADS_SEARCH_HANDLE hSearchResult);
    HRESULT (STDMETHODCALLTYPE * GetNextColumnName) (IDirectorySearch * This, ADS_SEARCH_HANDLE hSearchHandle, LPWSTR * ppszColumnName);
    HRESULT (STDMETHODCALLTYPE * GetColumn) (IDirectorySearch * This, ADS_SEARCH_HANDLE hSearchResult, LPWSTR szColumnName, PADS_SEARCH_COLUMN pSearchColumn);
    HRESULT (STDMETHODCALLTYPE * FreeColumn) (IDirectorySearch * This, PADS_SEARCH_COLUMN pSearchColumn);
    HRESULT (STDMETHODCALLTYPE * CloseSearchHandle) (IDirectorySearch * This, ADS_SEARCH_HANDLE hSearchResult);
    END_INTERFACE
} IDirectorySearchVtbl;
interface IDirectorySearch {
    CONST_VTBL struct IDirectorySearchVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IDirectorySearch_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IDirectorySearch_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IDirectorySearch_Release(This)  (This)->lpVtbl->Release(This)
#define IDirectorySearch_SetSearchPreference(This,pSearchPrefs,dwNumPrefs)  (This)->lpVtbl->SetSearchPreference(This,pSearchPrefs,dwNumPrefs)
#define IDirectorySearch_ExecuteSearch(This,pszSearchFilter,pAttributeNames,dwNumberAttributes,phSearchResult)  (This)->lpVtbl->ExecuteSearch(This,pszSearchFilter,pAttributeNames,dwNumberAttributes,phSearchResult)
#define IDirectorySearch_AbandonSearch(This,phSearchResult)  (This)->lpVtbl->AbandonSearch(This,phSearchResult)
#define IDirectorySearch_GetFirstRow(This,hSearchResult)  (This)->lpVtbl->GetFirstRow(This,hSearchResult)
#define IDirectorySearch_GetNextRow(This,hSearchResult)  (This)->lpVtbl->GetNextRow(This,hSearchResult)
#define IDirectorySearch_GetPreviousRow(This,hSearchResult)  (This)->lpVtbl->GetPreviousRow(This,hSearchResult)
#define IDirectorySearch_GetNextColumnName(This,hSearchHandle,ppszColumnName)  (This)->lpVtbl->GetNextColumnName(This,hSearchHandle,ppszColumnName)
#define IDirectorySearch_GetColumn(This,hSearchResult,szColumnName,pSearchColumn)  (This)->lpVtbl->GetColumn(This,hSearchResult,szColumnName,pSearchColumn)
#define IDirectorySearch_FreeColumn(This,pSearchColumn)  (This)->lpVtbl->FreeColumn(This,pSearchColumn)
#define IDirectorySearch_CloseSearchHandle(This,hSearchResult)  (This)->lpVtbl->CloseSearchHandle(This,hSearchResult)
#endif
#endif
HRESULT STDMETHODCALLTYPE IDirectorySearch_SetSearchPreference_Proxy (IDirectorySearch * This, PADS_SEARCHPREF_INFO pSearchPrefs, DWORD dwNumPrefs);
void __RPC_STUB IDirectorySearch_SetSearchPreference_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySearch_ExecuteSearch_Proxy (IDirectorySearch * This, LPWSTR pszSearchFilter, LPWSTR * pAttributeNames, DWORD dwNumberAttributes, PADS_SEARCH_HANDLE phSearchResult);
void __RPC_STUB IDirectorySearch_ExecuteSearch_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySearch_AbandonSearch_Proxy (IDirectorySearch * This, ADS_SEARCH_HANDLE phSearchResult);
void __RPC_STUB IDirectorySearch_AbandonSearch_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySearch_GetFirstRow_Proxy (IDirectorySearch * This, ADS_SEARCH_HANDLE hSearchResult);
void __RPC_STUB IDirectorySearch_GetFirstRow_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySearch_GetNextRow_Proxy (IDirectorySearch * This, ADS_SEARCH_HANDLE hSearchResult);
void __RPC_STUB IDirectorySearch_GetNextRow_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySearch_GetPreviousRow_Proxy (IDirectorySearch * This, ADS_SEARCH_HANDLE hSearchResult);
void __RPC_STUB IDirectorySearch_GetPreviousRow_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySearch_GetNextColumnName_Proxy (IDirectorySearch * This, ADS_SEARCH_HANDLE hSearchHandle, LPWSTR * ppszColumnName);
void __RPC_STUB IDirectorySearch_GetNextColumnName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySearch_GetColumn_Proxy (IDirectorySearch * This, ADS_SEARCH_HANDLE hSearchResult, LPWSTR szColumnName, PADS_SEARCH_COLUMN pSearchColumn);
void __RPC_STUB IDirectorySearch_GetColumn_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySearch_FreeColumn_Proxy (IDirectorySearch * This, PADS_SEARCH_COLUMN pSearchColumn);
void __RPC_STUB IDirectorySearch_FreeColumn_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySearch_CloseSearchHandle_Proxy (IDirectorySearch * This, ADS_SEARCH_HANDLE hSearchResult);
void __RPC_STUB IDirectorySearch_CloseSearchHandle_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IDirectorySchemaMgmt_INTERFACE_DEFINED__
#define __IDirectorySchemaMgmt_INTERFACE_DEFINED__
EXTERN_C const IID IID_IDirectorySchemaMgmt;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("75db3b9c-a4d8-11d0-a79c-00c04fd8d5a8") IDirectorySchemaMgmt:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE EnumAttributes (LPWSTR * ppszAttrNames, DWORD dwNumAttributes, PADS_ATTR_DEF * ppAttrDefinition, DWORD * pdwNumAttributes) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateAttributeDefinition (LPWSTR pszAttributeName, PADS_ATTR_DEF pAttributeDefinition) = 0;
    virtual HRESULT STDMETHODCALLTYPE WriteAttributeDefinition (LPWSTR pszAttributeName, PADS_ATTR_DEF pAttributeDefinition) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteAttributeDefinition (LPWSTR pszAttributeName) = 0;
    virtual HRESULT STDMETHODCALLTYPE EnumClasses (LPWSTR * ppszClassNames, DWORD dwNumClasses, PADS_CLASS_DEF * ppClassDefinition, DWORD * pdwNumClasses) = 0;
    virtual HRESULT STDMETHODCALLTYPE WriteClassDefinition (LPWSTR pszClassName, PADS_CLASS_DEF pClassDefinition) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateClassDefinition (LPWSTR pszClassName, PADS_CLASS_DEF pClassDefinition) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteClassDefinition (LPWSTR pszClassName) = 0;
};
#else
typedef struct IDirectorySchemaMgmtVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IDirectorySchemaMgmt * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IDirectorySchemaMgmt * This);
    ULONG (STDMETHODCALLTYPE * Release) (IDirectorySchemaMgmt * This);
    HRESULT (STDMETHODCALLTYPE * EnumAttributes) (IDirectorySchemaMgmt * This, LPWSTR * ppszAttrNames, DWORD dwNumAttributes, PADS_ATTR_DEF * ppAttrDefinition, DWORD * pdwNumAttributes);
    HRESULT (STDMETHODCALLTYPE * CreateAttributeDefinition) (IDirectorySchemaMgmt * This, LPWSTR pszAttributeName, PADS_ATTR_DEF pAttributeDefinition);
    HRESULT (STDMETHODCALLTYPE * WriteAttributeDefinition) (IDirectorySchemaMgmt * This, LPWSTR pszAttributeName, PADS_ATTR_DEF pAttributeDefinition);
    HRESULT (STDMETHODCALLTYPE * DeleteAttributeDefinition) (IDirectorySchemaMgmt * This, LPWSTR pszAttributeName);
    HRESULT (STDMETHODCALLTYPE * EnumClasses) (IDirectorySchemaMgmt * This, LPWSTR * ppszClassNames, DWORD dwNumClasses, PADS_CLASS_DEF * ppClassDefinition, DWORD * pdwNumClasses);
    HRESULT (STDMETHODCALLTYPE * WriteClassDefinition) (IDirectorySchemaMgmt * This, LPWSTR pszClassName, PADS_CLASS_DEF pClassDefinition);
    HRESULT (STDMETHODCALLTYPE * CreateClassDefinition) (IDirectorySchemaMgmt * This, LPWSTR pszClassName, PADS_CLASS_DEF pClassDefinition);
    HRESULT (STDMETHODCALLTYPE * DeleteClassDefinition) (IDirectorySchemaMgmt * This, LPWSTR pszClassName);
    END_INTERFACE
} IDirectorySchemaMgmtVtbl;
interface IDirectorySchemaMgmt {
    CONST_VTBL struct IDirectorySchemaMgmtVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IDirectorySchemaMgmt_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IDirectorySchemaMgmt_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IDirectorySchemaMgmt_Release(This)  (This)->lpVtbl->Release(This)
#define IDirectorySchemaMgmt_EnumAttributes(This,ppszAttrNames,dwNumAttributes,ppAttrDefinition,pdwNumAttributes)  (This)->lpVtbl->EnumAttributes(This,ppszAttrNames,dwNumAttributes,ppAttrDefinition,pdwNumAttributes)
#define IDirectorySchemaMgmt_CreateAttributeDefinition(This,pszAttributeName,pAttributeDefinition)  (This)->lpVtbl->CreateAttributeDefinition(This,pszAttributeName,pAttributeDefinition)
#define IDirectorySchemaMgmt_WriteAttributeDefinition(This,pszAttributeName,pAttributeDefinition)  (This)->lpVtbl->WriteAttributeDefinition(This,pszAttributeName,pAttributeDefinition)
#define IDirectorySchemaMgmt_DeleteAttributeDefinition(This,pszAttributeName)  (This)->lpVtbl->DeleteAttributeDefinition(This,pszAttributeName)
#define IDirectorySchemaMgmt_EnumClasses(This,ppszClassNames,dwNumClasses,ppClassDefinition,pdwNumClasses)  (This)->lpVtbl->EnumClasses(This,ppszClassNames,dwNumClasses,ppClassDefinition,pdwNumClasses)
#define IDirectorySchemaMgmt_WriteClassDefinition(This,pszClassName,pClassDefinition)  (This)->lpVtbl->WriteClassDefinition(This,pszClassName,pClassDefinition)
#define IDirectorySchemaMgmt_CreateClassDefinition(This,pszClassName,pClassDefinition)  (This)->lpVtbl->CreateClassDefinition(This,pszClassName,pClassDefinition)
#define IDirectorySchemaMgmt_DeleteClassDefinition(This,pszClassName)  (This)->lpVtbl->DeleteClassDefinition(This,pszClassName)
#endif
#endif
HRESULT STDMETHODCALLTYPE IDirectorySchemaMgmt_EnumAttributes_Proxy (IDirectorySchemaMgmt * This, LPWSTR * ppszAttrNames, DWORD dwNumAttributes, PADS_ATTR_DEF * ppAttrDefinition, DWORD * pdwNumAttributes);
void __RPC_STUB IDirectorySchemaMgmt_EnumAttributes_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySchemaMgmt_CreateAttributeDefinition_Proxy (IDirectorySchemaMgmt * This, LPWSTR pszAttributeName, PADS_ATTR_DEF pAttributeDefinition);
void __RPC_STUB IDirectorySchemaMgmt_CreateAttributeDefinition_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySchemaMgmt_WriteAttributeDefinition_Proxy (IDirectorySchemaMgmt * This, LPWSTR pszAttributeName, PADS_ATTR_DEF pAttributeDefinition);
void __RPC_STUB IDirectorySchemaMgmt_WriteAttributeDefinition_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySchemaMgmt_DeleteAttributeDefinition_Proxy (IDirectorySchemaMgmt * This, LPWSTR pszAttributeName);
void __RPC_STUB IDirectorySchemaMgmt_DeleteAttributeDefinition_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySchemaMgmt_EnumClasses_Proxy (IDirectorySchemaMgmt * This, LPWSTR * ppszClassNames, DWORD dwNumClasses, PADS_CLASS_DEF * ppClassDefinition, DWORD * pdwNumClasses);
void __RPC_STUB IDirectorySchemaMgmt_EnumClasses_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySchemaMgmt_WriteClassDefinition_Proxy (IDirectorySchemaMgmt * This, LPWSTR pszClassName, PADS_CLASS_DEF pClassDefinition);
void __RPC_STUB IDirectorySchemaMgmt_WriteClassDefinition_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySchemaMgmt_CreateClassDefinition_Proxy (IDirectorySchemaMgmt * This, LPWSTR pszClassName, PADS_CLASS_DEF pClassDefinition);
void __RPC_STUB IDirectorySchemaMgmt_CreateClassDefinition_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IDirectorySchemaMgmt_DeleteClassDefinition_Proxy (IDirectorySchemaMgmt * This, LPWSTR pszClassName);
void __RPC_STUB IDirectorySchemaMgmt_DeleteClassDefinition_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsAggregatee_INTERFACE_DEFINED__
#define __IADsAggregatee_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsAggregatee;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("1346ce8c-9039-11d0-8528-00c04fd8d503") IADsAggregatee:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE ConnectAsAggregatee (IUnknown * pOuterUnknown) = 0;
    virtual HRESULT STDMETHODCALLTYPE DisconnectAsAggregatee (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE RelinquishInterface (REFIID riid) = 0;
    virtual HRESULT STDMETHODCALLTYPE RestoreInterface (REFIID riid) = 0;
};
#else
typedef struct IADsAggregateeVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsAggregatee * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsAggregatee * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsAggregatee * This);
    HRESULT (STDMETHODCALLTYPE * ConnectAsAggregatee) (IADsAggregatee * This, IUnknown * pOuterUnknown);
    HRESULT (STDMETHODCALLTYPE * DisconnectAsAggregatee) (IADsAggregatee * This);
    HRESULT (STDMETHODCALLTYPE * RelinquishInterface) (IADsAggregatee * This, REFIID riid);
    HRESULT (STDMETHODCALLTYPE * RestoreInterface) (IADsAggregatee * This, REFIID riid);
    END_INTERFACE
} IADsAggregateeVtbl;
interface IADsAggregatee {
    CONST_VTBL struct IADsAggregateeVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsAggregatee_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsAggregatee_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsAggregatee_Release(This)  (This)->lpVtbl->Release(This)
#define IADsAggregatee_ConnectAsAggregatee(This,pOuterUnknown)  (This)->lpVtbl->ConnectAsAggregatee(This,pOuterUnknown)
#define IADsAggregatee_DisconnectAsAggregatee(This)  (This)->lpVtbl->DisconnectAsAggregatee(This)
#define IADsAggregatee_RelinquishInterface(This,riid)  (This)->lpVtbl->RelinquishInterface(This,riid)
#define IADsAggregatee_RestoreInterface(This,riid)  (This)->lpVtbl->RestoreInterface(This,riid)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsAggregatee_ConnectAsAggregatee_Proxy (IADsAggregatee * This, IUnknown * pOuterUnknown);
void __RPC_STUB IADsAggregatee_ConnectAsAggregatee_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAggregatee_DisconnectAsAggregatee_Proxy (IADsAggregatee * This);
void __RPC_STUB IADsAggregatee_DisconnectAsAggregatee_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAggregatee_RelinquishInterface_Proxy (IADsAggregatee * This, REFIID riid);
void __RPC_STUB IADsAggregatee_RelinquishInterface_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAggregatee_RestoreInterface_Proxy (IADsAggregatee * This, REFIID riid);
void __RPC_STUB IADsAggregatee_RestoreInterface_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsAggregator_INTERFACE_DEFINED__
#define __IADsAggregator_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsAggregator;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("52db5fb0-941f-11d0-8529-00c04fd8d503") IADsAggregator:public IUnknown {
    public:
    virtual HRESULT STDMETHODCALLTYPE ConnectAsAggregator (IUnknown * pAggregatee) = 0;
    virtual HRESULT STDMETHODCALLTYPE DisconnectAsAggregator (void) = 0;
};
#else
typedef struct IADsAggregatorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsAggregator * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsAggregator * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsAggregator * This);
    HRESULT (STDMETHODCALLTYPE * ConnectAsAggregator) (IADsAggregator * This, IUnknown * pAggregatee);
    HRESULT (STDMETHODCALLTYPE * DisconnectAsAggregator) (IADsAggregator * This);
    END_INTERFACE
} IADsAggregatorVtbl;
interface IADsAggregator {
    CONST_VTBL struct IADsAggregatorVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsAggregator_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsAggregator_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsAggregator_Release(This)  (This)->lpVtbl->Release(This)
#define IADsAggregator_ConnectAsAggregator(This,pAggregatee)  (This)->lpVtbl->ConnectAsAggregator(This,pAggregatee)
#define IADsAggregator_DisconnectAsAggregator(This)  (This)->lpVtbl->DisconnectAsAggregator(This)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsAggregator_ConnectAsAggregator_Proxy (IADsAggregator * This, IUnknown * pAggregatee);
void __RPC_STUB IADsAggregator_ConnectAsAggregator_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAggregator_DisconnectAsAggregator_Proxy (IADsAggregator * This);
void __RPC_STUB IADsAggregator_DisconnectAsAggregator_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsAccessControlEntry_INTERFACE_DEFINED__
#define __IADsAccessControlEntry_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsAccessControlEntry;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("b4f3a14c-9bdd-11d0-852c-00c04fd8d503") IADsAccessControlEntry:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_AccessMask (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AccessMask (long lnAccessMask) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AceType (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AceType (long lnAceType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AceFlags (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AceFlags (long lnAceFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Flags (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Flags (long lnFlags) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ObjectType (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ObjectType (BSTR bstrObjectType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_InheritedObjectType (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_InheritedObjectType (BSTR bstrInheritedObjectType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Trustee (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Trustee (BSTR bstrTrustee) = 0;
};
#else
typedef struct IADsAccessControlEntryVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsAccessControlEntry * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsAccessControlEntry * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsAccessControlEntry * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsAccessControlEntry * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsAccessControlEntry * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsAccessControlEntry * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsAccessControlEntry * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_AccessMask) (IADsAccessControlEntry * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_AccessMask) (IADsAccessControlEntry * This, long lnAccessMask);
    HRESULT (STDMETHODCALLTYPE * get_AceType) (IADsAccessControlEntry * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_AceType) (IADsAccessControlEntry * This, long lnAceType);
    HRESULT (STDMETHODCALLTYPE * get_AceFlags) (IADsAccessControlEntry * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_AceFlags) (IADsAccessControlEntry * This, long lnAceFlags);
    HRESULT (STDMETHODCALLTYPE * get_Flags) (IADsAccessControlEntry * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Flags) (IADsAccessControlEntry * This, long lnFlags);
    HRESULT (STDMETHODCALLTYPE * get_ObjectType) (IADsAccessControlEntry * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_ObjectType) (IADsAccessControlEntry * This, BSTR bstrObjectType);
    HRESULT (STDMETHODCALLTYPE * get_InheritedObjectType) (IADsAccessControlEntry * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_InheritedObjectType) (IADsAccessControlEntry * This, BSTR bstrInheritedObjectType);
    HRESULT (STDMETHODCALLTYPE * get_Trustee) (IADsAccessControlEntry * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Trustee) (IADsAccessControlEntry * This, BSTR bstrTrustee);
    END_INTERFACE
} IADsAccessControlEntryVtbl;
interface IADsAccessControlEntry {
    CONST_VTBL struct IADsAccessControlEntryVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsAccessControlEntry_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsAccessControlEntry_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsAccessControlEntry_Release(This)  (This)->lpVtbl->Release(This)
#define IADsAccessControlEntry_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsAccessControlEntry_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsAccessControlEntry_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsAccessControlEntry_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsAccessControlEntry_get_AccessMask(This,retval)  (This)->lpVtbl->get_AccessMask(This,retval)
#define IADsAccessControlEntry_put_AccessMask(This,lnAccessMask)  (This)->lpVtbl->put_AccessMask(This,lnAccessMask)
#define IADsAccessControlEntry_get_AceType(This,retval)  (This)->lpVtbl->get_AceType(This,retval)
#define IADsAccessControlEntry_put_AceType(This,lnAceType)  (This)->lpVtbl->put_AceType(This,lnAceType)
#define IADsAccessControlEntry_get_AceFlags(This,retval)  (This)->lpVtbl->get_AceFlags(This,retval)
#define IADsAccessControlEntry_put_AceFlags(This,lnAceFlags)  (This)->lpVtbl->put_AceFlags(This,lnAceFlags)
#define IADsAccessControlEntry_get_Flags(This,retval)  (This)->lpVtbl->get_Flags(This,retval)
#define IADsAccessControlEntry_put_Flags(This,lnFlags)  (This)->lpVtbl->put_Flags(This,lnFlags)
#define IADsAccessControlEntry_get_ObjectType(This,retval)  (This)->lpVtbl->get_ObjectType(This,retval)
#define IADsAccessControlEntry_put_ObjectType(This,bstrObjectType)  (This)->lpVtbl->put_ObjectType(This,bstrObjectType)
#define IADsAccessControlEntry_get_InheritedObjectType(This,retval)  (This)->lpVtbl->get_InheritedObjectType(This,retval)
#define IADsAccessControlEntry_put_InheritedObjectType(This,bstrInheritedObjectType)  (This)->lpVtbl->put_InheritedObjectType(This,bstrInheritedObjectType)
#define IADsAccessControlEntry_get_Trustee(This,retval)  (This)->lpVtbl->get_Trustee(This,retval)
#define IADsAccessControlEntry_put_Trustee(This,bstrTrustee)  (This)->lpVtbl->put_Trustee(This,bstrTrustee)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_get_AccessMask_Proxy (IADsAccessControlEntry * This, long *retval);
void __RPC_STUB IADsAccessControlEntry_get_AccessMask_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_put_AccessMask_Proxy (IADsAccessControlEntry * This, long lnAccessMask);
void __RPC_STUB IADsAccessControlEntry_put_AccessMask_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_get_AceType_Proxy (IADsAccessControlEntry * This, long *retval);
void __RPC_STUB IADsAccessControlEntry_get_AceType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_put_AceType_Proxy (IADsAccessControlEntry * This, long lnAceType);
void __RPC_STUB IADsAccessControlEntry_put_AceType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_get_AceFlags_Proxy (IADsAccessControlEntry * This, long *retval);
void __RPC_STUB IADsAccessControlEntry_get_AceFlags_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_put_AceFlags_Proxy (IADsAccessControlEntry * This, long lnAceFlags);
void __RPC_STUB IADsAccessControlEntry_put_AceFlags_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_get_Flags_Proxy (IADsAccessControlEntry * This, long *retval);
void __RPC_STUB IADsAccessControlEntry_get_Flags_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_put_Flags_Proxy (IADsAccessControlEntry * This, long lnFlags);
void __RPC_STUB IADsAccessControlEntry_put_Flags_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_get_ObjectType_Proxy (IADsAccessControlEntry * This, BSTR * retval);
void __RPC_STUB IADsAccessControlEntry_get_ObjectType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_put_ObjectType_Proxy (IADsAccessControlEntry * This, BSTR bstrObjectType);
void __RPC_STUB IADsAccessControlEntry_put_ObjectType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_get_InheritedObjectType_Proxy (IADsAccessControlEntry * This, BSTR * retval);
void __RPC_STUB IADsAccessControlEntry_get_InheritedObjectType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_put_InheritedObjectType_Proxy (IADsAccessControlEntry * This, BSTR bstrInheritedObjectType);
void __RPC_STUB IADsAccessControlEntry_put_InheritedObjectType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_get_Trustee_Proxy (IADsAccessControlEntry * This, BSTR * retval);
void __RPC_STUB IADsAccessControlEntry_get_Trustee_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlEntry_put_Trustee_Proxy (IADsAccessControlEntry * This, BSTR bstrTrustee);
void __RPC_STUB IADsAccessControlEntry_put_Trustee_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_AccessControlEntry;
#ifdef __cplusplus
class DECLSPEC_UUID ("b75ac000-9bdd-11d0-852c-00c04fd8d503") AccessControlEntry;
#endif

#ifndef __IADsAccessControlList_INTERFACE_DEFINED__
#define __IADsAccessControlList_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsAccessControlList;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("b7ee91cc-9bdd-11d0-852c-00c04fd8d503") IADsAccessControlList:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_AclRevision (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AclRevision (long lnAclRevision) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AceCount (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AceCount (long lnAceCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddAce (IDispatch * pAccessControlEntry) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveAce (IDispatch * pAccessControlEntry) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyAccessList (IDispatch ** ppAccessControlList) = 0;
    virtual HRESULT STDMETHODCALLTYPE get__NewEnum (IUnknown ** retval) = 0;
};
#else
typedef struct IADsAccessControlListVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsAccessControlList * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsAccessControlList * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsAccessControlList * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsAccessControlList * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsAccessControlList * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsAccessControlList * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsAccessControlList * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_AclRevision) (IADsAccessControlList * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_AclRevision) (IADsAccessControlList * This, long lnAclRevision);
    HRESULT (STDMETHODCALLTYPE * get_AceCount) (IADsAccessControlList * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_AceCount) (IADsAccessControlList * This, long lnAceCount);
    HRESULT (STDMETHODCALLTYPE * AddAce) (IADsAccessControlList * This, IDispatch * pAccessControlEntry);
    HRESULT (STDMETHODCALLTYPE * RemoveAce) (IADsAccessControlList * This, IDispatch * pAccessControlEntry);
    HRESULT (STDMETHODCALLTYPE * CopyAccessList) (IADsAccessControlList * This, IDispatch ** ppAccessControlList);
    HRESULT (STDMETHODCALLTYPE * get__NewEnum) (IADsAccessControlList * This, IUnknown ** retval);
    END_INTERFACE
} IADsAccessControlListVtbl;
interface IADsAccessControlList {
    CONST_VTBL struct IADsAccessControlListVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsAccessControlList_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsAccessControlList_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsAccessControlList_Release(This)  (This)->lpVtbl->Release(This)
#define IADsAccessControlList_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsAccessControlList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsAccessControlList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsAccessControlList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsAccessControlList_get_AclRevision(This,retval)  (This)->lpVtbl->get_AclRevision(This,retval)
#define IADsAccessControlList_put_AclRevision(This,lnAclRevision)  (This)->lpVtbl->put_AclRevision(This,lnAclRevision)
#define IADsAccessControlList_get_AceCount(This,retval)  (This)->lpVtbl->get_AceCount(This,retval)
#define IADsAccessControlList_put_AceCount(This,lnAceCount)  (This)->lpVtbl->put_AceCount(This,lnAceCount)
#define IADsAccessControlList_AddAce(This,pAccessControlEntry)  (This)->lpVtbl->AddAce(This,pAccessControlEntry)
#define IADsAccessControlList_RemoveAce(This,pAccessControlEntry)  (This)->lpVtbl->RemoveAce(This,pAccessControlEntry)
#define IADsAccessControlList_CopyAccessList(This,ppAccessControlList)  (This)->lpVtbl->CopyAccessList(This,ppAccessControlList)
#define IADsAccessControlList_get__NewEnum(This,retval)  (This)->lpVtbl->get__NewEnum(This,retval)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsAccessControlList_get_AclRevision_Proxy (IADsAccessControlList * This, long *retval);
void __RPC_STUB IADsAccessControlList_get_AclRevision_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlList_put_AclRevision_Proxy (IADsAccessControlList * This, long lnAclRevision);
void __RPC_STUB IADsAccessControlList_put_AclRevision_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlList_get_AceCount_Proxy (IADsAccessControlList * This, long *retval);
void __RPC_STUB IADsAccessControlList_get_AceCount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlList_put_AceCount_Proxy (IADsAccessControlList * This, long lnAceCount);
void __RPC_STUB IADsAccessControlList_put_AceCount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlList_AddAce_Proxy (IADsAccessControlList * This, IDispatch * pAccessControlEntry);
void __RPC_STUB IADsAccessControlList_AddAce_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlList_RemoveAce_Proxy (IADsAccessControlList * This, IDispatch * pAccessControlEntry);
void __RPC_STUB IADsAccessControlList_RemoveAce_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlList_CopyAccessList_Proxy (IADsAccessControlList * This, IDispatch ** ppAccessControlList);
void __RPC_STUB IADsAccessControlList_CopyAccessList_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAccessControlList_get__NewEnum_Proxy (IADsAccessControlList * This, IUnknown ** retval);
void __RPC_STUB IADsAccessControlList_get__NewEnum_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_AccessControlList;
#ifdef __cplusplus
class DECLSPEC_UUID ("b85ea052-9bdd-11d0-852c-00c04fd8d503") AccessControlList;
#endif

#ifndef __IADsSecurityDescriptor_INTERFACE_DEFINED__
#define __IADsSecurityDescriptor_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsSecurityDescriptor;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("b8c787ca-9bdd-11d0-852c-00c04fd8d503") IADsSecurityDescriptor:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Revision (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Revision (long lnRevision) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Control (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Control (long lnControl) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Owner (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Owner (BSTR bstrOwner) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_OwnerDefaulted (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_OwnerDefaulted (VARIANT_BOOL fOwnerDefaulted) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Group (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Group (BSTR bstrGroup) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GroupDefaulted (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GroupDefaulted (VARIANT_BOOL fGroupDefaulted) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DiscretionaryAcl (IDispatch ** retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DiscretionaryAcl (IDispatch * pDiscretionaryAcl) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DaclDefaulted (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DaclDefaulted (VARIANT_BOOL fDaclDefaulted) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SystemAcl (IDispatch ** retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SystemAcl (IDispatch * pSystemAcl) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SaclDefaulted (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SaclDefaulted (VARIANT_BOOL fSaclDefaulted) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopySecurityDescriptor (IDispatch ** ppSecurityDescriptor) = 0;
};
#else
typedef struct IADsSecurityDescriptorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsSecurityDescriptor * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsSecurityDescriptor * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsSecurityDescriptor * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsSecurityDescriptor * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsSecurityDescriptor * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsSecurityDescriptor * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsSecurityDescriptor * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Revision) (IADsSecurityDescriptor * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Revision) (IADsSecurityDescriptor * This, long lnRevision);
    HRESULT (STDMETHODCALLTYPE * get_Control) (IADsSecurityDescriptor * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Control) (IADsSecurityDescriptor * This, long lnControl);
    HRESULT (STDMETHODCALLTYPE * get_Owner) (IADsSecurityDescriptor * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Owner) (IADsSecurityDescriptor * This, BSTR bstrOwner);
    HRESULT (STDMETHODCALLTYPE * get_OwnerDefaulted) (IADsSecurityDescriptor * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * put_OwnerDefaulted) (IADsSecurityDescriptor * This, VARIANT_BOOL fOwnerDefaulted);
    HRESULT (STDMETHODCALLTYPE * get_Group) (IADsSecurityDescriptor * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Group) (IADsSecurityDescriptor * This, BSTR bstrGroup);
    HRESULT (STDMETHODCALLTYPE * get_GroupDefaulted) (IADsSecurityDescriptor * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * put_GroupDefaulted) (IADsSecurityDescriptor * This, VARIANT_BOOL fGroupDefaulted);
    HRESULT (STDMETHODCALLTYPE * get_DiscretionaryAcl) (IADsSecurityDescriptor * This, IDispatch ** retval);
    HRESULT (STDMETHODCALLTYPE * put_DiscretionaryAcl) (IADsSecurityDescriptor * This, IDispatch * pDiscretionaryAcl);
    HRESULT (STDMETHODCALLTYPE * get_DaclDefaulted) (IADsSecurityDescriptor * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * put_DaclDefaulted) (IADsSecurityDescriptor * This, VARIANT_BOOL fDaclDefaulted);
    HRESULT (STDMETHODCALLTYPE * get_SystemAcl) (IADsSecurityDescriptor * This, IDispatch ** retval);
    HRESULT (STDMETHODCALLTYPE * put_SystemAcl) (IADsSecurityDescriptor * This, IDispatch * pSystemAcl);
    HRESULT (STDMETHODCALLTYPE * get_SaclDefaulted) (IADsSecurityDescriptor * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * put_SaclDefaulted) (IADsSecurityDescriptor * This, VARIANT_BOOL fSaclDefaulted);
    HRESULT (STDMETHODCALLTYPE * CopySecurityDescriptor) (IADsSecurityDescriptor * This, IDispatch ** ppSecurityDescriptor);
    END_INTERFACE
} IADsSecurityDescriptorVtbl;
interface IADsSecurityDescriptor {
    CONST_VTBL struct IADsSecurityDescriptorVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsSecurityDescriptor_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsSecurityDescriptor_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsSecurityDescriptor_Release(This)  (This)->lpVtbl->Release(This)
#define IADsSecurityDescriptor_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsSecurityDescriptor_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsSecurityDescriptor_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsSecurityDescriptor_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsSecurityDescriptor_get_Revision(This,retval)  (This)->lpVtbl->get_Revision(This,retval)
#define IADsSecurityDescriptor_put_Revision(This,lnRevision)  (This)->lpVtbl->put_Revision(This,lnRevision)
#define IADsSecurityDescriptor_get_Control(This,retval)  (This)->lpVtbl->get_Control(This,retval)
#define IADsSecurityDescriptor_put_Control(This,lnControl)  (This)->lpVtbl->put_Control(This,lnControl)
#define IADsSecurityDescriptor_get_Owner(This,retval)  (This)->lpVtbl->get_Owner(This,retval)
#define IADsSecurityDescriptor_put_Owner(This,bstrOwner)  (This)->lpVtbl->put_Owner(This,bstrOwner)
#define IADsSecurityDescriptor_get_OwnerDefaulted(This,retval)  (This)->lpVtbl->get_OwnerDefaulted(This,retval)
#define IADsSecurityDescriptor_put_OwnerDefaulted(This,fOwnerDefaulted)  (This)->lpVtbl->put_OwnerDefaulted(This,fOwnerDefaulted)
#define IADsSecurityDescriptor_get_Group(This,retval)  (This)->lpVtbl->get_Group(This,retval)
#define IADsSecurityDescriptor_put_Group(This,bstrGroup)  (This)->lpVtbl->put_Group(This,bstrGroup)
#define IADsSecurityDescriptor_get_GroupDefaulted(This,retval)  (This)->lpVtbl->get_GroupDefaulted(This,retval)
#define IADsSecurityDescriptor_put_GroupDefaulted(This,fGroupDefaulted)  (This)->lpVtbl->put_GroupDefaulted(This,fGroupDefaulted)
#define IADsSecurityDescriptor_get_DiscretionaryAcl(This,retval)  (This)->lpVtbl->get_DiscretionaryAcl(This,retval)
#define IADsSecurityDescriptor_put_DiscretionaryAcl(This,pDiscretionaryAcl)  (This)->lpVtbl->put_DiscretionaryAcl(This,pDiscretionaryAcl)
#define IADsSecurityDescriptor_get_DaclDefaulted(This,retval)  (This)->lpVtbl->get_DaclDefaulted(This,retval)
#define IADsSecurityDescriptor_put_DaclDefaulted(This,fDaclDefaulted)  (This)->lpVtbl->put_DaclDefaulted(This,fDaclDefaulted)
#define IADsSecurityDescriptor_get_SystemAcl(This,retval)  (This)->lpVtbl->get_SystemAcl(This,retval)
#define IADsSecurityDescriptor_put_SystemAcl(This,pSystemAcl)  (This)->lpVtbl->put_SystemAcl(This,pSystemAcl)
#define IADsSecurityDescriptor_get_SaclDefaulted(This,retval)  (This)->lpVtbl->get_SaclDefaulted(This,retval)
#define IADsSecurityDescriptor_put_SaclDefaulted(This,fSaclDefaulted)  (This)->lpVtbl->put_SaclDefaulted(This,fSaclDefaulted)
#define IADsSecurityDescriptor_CopySecurityDescriptor(This,ppSecurityDescriptor)  (This)->lpVtbl->CopySecurityDescriptor(This,ppSecurityDescriptor)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_get_Revision_Proxy (IADsSecurityDescriptor * This, long *retval);
void __RPC_STUB IADsSecurityDescriptor_get_Revision_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_put_Revision_Proxy (IADsSecurityDescriptor * This, long lnRevision);
void __RPC_STUB IADsSecurityDescriptor_put_Revision_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_get_Control_Proxy (IADsSecurityDescriptor * This, long *retval);
void __RPC_STUB IADsSecurityDescriptor_get_Control_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_put_Control_Proxy (IADsSecurityDescriptor * This, long lnControl);
void __RPC_STUB IADsSecurityDescriptor_put_Control_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_get_Owner_Proxy (IADsSecurityDescriptor * This, BSTR * retval);
void __RPC_STUB IADsSecurityDescriptor_get_Owner_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_put_Owner_Proxy (IADsSecurityDescriptor * This, BSTR bstrOwner);
void __RPC_STUB IADsSecurityDescriptor_put_Owner_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_get_OwnerDefaulted_Proxy (IADsSecurityDescriptor * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsSecurityDescriptor_get_OwnerDefaulted_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_put_OwnerDefaulted_Proxy (IADsSecurityDescriptor * This, VARIANT_BOOL fOwnerDefaulted);
void __RPC_STUB IADsSecurityDescriptor_put_OwnerDefaulted_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_get_Group_Proxy (IADsSecurityDescriptor * This, BSTR * retval);
void __RPC_STUB IADsSecurityDescriptor_get_Group_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_put_Group_Proxy (IADsSecurityDescriptor * This, BSTR bstrGroup);
void __RPC_STUB IADsSecurityDescriptor_put_Group_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_get_GroupDefaulted_Proxy (IADsSecurityDescriptor * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsSecurityDescriptor_get_GroupDefaulted_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_put_GroupDefaulted_Proxy (IADsSecurityDescriptor * This, VARIANT_BOOL fGroupDefaulted);
void __RPC_STUB IADsSecurityDescriptor_put_GroupDefaulted_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_get_DiscretionaryAcl_Proxy (IADsSecurityDescriptor * This, IDispatch ** retval);
void __RPC_STUB IADsSecurityDescriptor_get_DiscretionaryAcl_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_put_DiscretionaryAcl_Proxy (IADsSecurityDescriptor * This, IDispatch * pDiscretionaryAcl);
void __RPC_STUB IADsSecurityDescriptor_put_DiscretionaryAcl_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_get_DaclDefaulted_Proxy (IADsSecurityDescriptor * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsSecurityDescriptor_get_DaclDefaulted_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_put_DaclDefaulted_Proxy (IADsSecurityDescriptor * This, VARIANT_BOOL fDaclDefaulted);
void __RPC_STUB IADsSecurityDescriptor_put_DaclDefaulted_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_get_SystemAcl_Proxy (IADsSecurityDescriptor * This, IDispatch ** retval);
void __RPC_STUB IADsSecurityDescriptor_get_SystemAcl_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_put_SystemAcl_Proxy (IADsSecurityDescriptor * This, IDispatch * pSystemAcl);
void __RPC_STUB IADsSecurityDescriptor_put_SystemAcl_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_get_SaclDefaulted_Proxy (IADsSecurityDescriptor * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsSecurityDescriptor_get_SaclDefaulted_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_put_SaclDefaulted_Proxy (IADsSecurityDescriptor * This, VARIANT_BOOL fSaclDefaulted);
void __RPC_STUB IADsSecurityDescriptor_put_SaclDefaulted_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityDescriptor_CopySecurityDescriptor_Proxy (IADsSecurityDescriptor * This, IDispatch ** ppSecurityDescriptor);
void __RPC_STUB IADsSecurityDescriptor_CopySecurityDescriptor_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_SecurityDescriptor;
#ifdef __cplusplus
class DECLSPEC_UUID ("b958f73c-9bdd-11d0-852c-00c04fd8d503") SecurityDescriptor;
#endif

#ifndef __IADsLargeInteger_INTERFACE_DEFINED__
#define __IADsLargeInteger_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsLargeInteger;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("9068270b-0939-11d1-8be1-00c04fd8d503") IADsLargeInteger:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_HighPart (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HighPart (long lnHighPart) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LowPart (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LowPart (long lnLowPart) = 0;
};
#else
typedef struct IADsLargeIntegerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsLargeInteger * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsLargeInteger * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsLargeInteger * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsLargeInteger * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsLargeInteger * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsLargeInteger * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsLargeInteger * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_HighPart) (IADsLargeInteger * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_HighPart) (IADsLargeInteger * This, long lnHighPart);
    HRESULT (STDMETHODCALLTYPE * get_LowPart) (IADsLargeInteger * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_LowPart) (IADsLargeInteger * This, long lnLowPart);
    END_INTERFACE
} IADsLargeIntegerVtbl;
interface IADsLargeInteger {
    CONST_VTBL struct IADsLargeIntegerVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsLargeInteger_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsLargeInteger_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsLargeInteger_Release(This)  (This)->lpVtbl->Release(This)
#define IADsLargeInteger_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsLargeInteger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsLargeInteger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsLargeInteger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsLargeInteger_get_HighPart(This,retval)  (This)->lpVtbl->get_HighPart(This,retval)
#define IADsLargeInteger_put_HighPart(This,lnHighPart)  (This)->lpVtbl->put_HighPart(This,lnHighPart)
#define IADsLargeInteger_get_LowPart(This,retval)  (This)->lpVtbl->get_LowPart(This,retval)
#define IADsLargeInteger_put_LowPart(This,lnLowPart)  (This)->lpVtbl->put_LowPart(This,lnLowPart)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsLargeInteger_get_HighPart_Proxy (IADsLargeInteger * This, long *retval);
void __RPC_STUB IADsLargeInteger_get_HighPart_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsLargeInteger_put_HighPart_Proxy (IADsLargeInteger * This, long lnHighPart);
void __RPC_STUB IADsLargeInteger_put_HighPart_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsLargeInteger_get_LowPart_Proxy (IADsLargeInteger * This, long *retval);
void __RPC_STUB IADsLargeInteger_get_LowPart_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsLargeInteger_put_LowPart_Proxy (IADsLargeInteger * This, long lnLowPart);
void __RPC_STUB IADsLargeInteger_put_LowPart_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_LargeInteger;
#ifdef __cplusplus
class DECLSPEC_UUID ("927971f5-0939-11d1-8be1-00c04fd8d503") LargeInteger;
#endif

#ifndef __IADsNameTranslate_INTERFACE_DEFINED__
#define __IADsNameTranslate_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsNameTranslate;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("b1b272a3-3625-11d1-a3a4-00c04fb950dc") IADsNameTranslate:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE put_ChaseReferral (long lnChaseReferral) = 0;
    virtual HRESULT STDMETHODCALLTYPE Init (long lnSetType, BSTR bstrADsPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE InitEx (long lnSetType, BSTR bstrADsPath, BSTR bstrUserID, BSTR bstrDomain, BSTR bstrPassword) = 0;
    virtual HRESULT STDMETHODCALLTYPE Set (long lnSetType, BSTR bstrADsPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE Get (long lnFormatType, BSTR * pbstrADsPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEx (long lnFormatType, VARIANT pvar) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEx (long lnFormatType, VARIANT * pvar) = 0;
};
#else
typedef struct IADsNameTranslateVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsNameTranslate * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsNameTranslate * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsNameTranslate * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsNameTranslate * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsNameTranslate * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsNameTranslate * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsNameTranslate * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * put_ChaseReferral) (IADsNameTranslate * This, long lnChaseReferral);
    HRESULT (STDMETHODCALLTYPE * Init) (IADsNameTranslate * This, long lnSetType, BSTR bstrADsPath);
    HRESULT (STDMETHODCALLTYPE * InitEx) (IADsNameTranslate * This, long lnSetType, BSTR bstrADsPath, BSTR bstrUserID, BSTR bstrDomain, BSTR bstrPassword);
    HRESULT (STDMETHODCALLTYPE * Set) (IADsNameTranslate * This, long lnSetType, BSTR bstrADsPath);
    HRESULT (STDMETHODCALLTYPE * Get) (IADsNameTranslate * This, long lnFormatType, BSTR * pbstrADsPath);
    HRESULT (STDMETHODCALLTYPE * SetEx) (IADsNameTranslate * This, long lnFormatType, VARIANT pvar);
    HRESULT (STDMETHODCALLTYPE * GetEx) (IADsNameTranslate * This, long lnFormatType, VARIANT * pvar);
    END_INTERFACE
} IADsNameTranslateVtbl;
interface IADsNameTranslate {
    CONST_VTBL struct IADsNameTranslateVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsNameTranslate_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsNameTranslate_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsNameTranslate_Release(This)  (This)->lpVtbl->Release(This)
#define IADsNameTranslate_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsNameTranslate_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsNameTranslate_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsNameTranslate_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsNameTranslate_put_ChaseReferral(This,lnChaseReferral)  (This)->lpVtbl->put_ChaseReferral(This,lnChaseReferral)
#define IADsNameTranslate_Init(This,lnSetType,bstrADsPath)  (This)->lpVtbl->Init(This,lnSetType,bstrADsPath)
#define IADsNameTranslate_InitEx(This,lnSetType,bstrADsPath,bstrUserID,bstrDomain,bstrPassword)  (This)->lpVtbl->InitEx(This,lnSetType,bstrADsPath,bstrUserID,bstrDomain,bstrPassword)
#define IADsNameTranslate_Set(This,lnSetType,bstrADsPath)  (This)->lpVtbl->Set(This,lnSetType,bstrADsPath)
#define IADsNameTranslate_Get(This,lnFormatType,pbstrADsPath)  (This)->lpVtbl->Get(This,lnFormatType,pbstrADsPath)
#define IADsNameTranslate_SetEx(This,lnFormatType,pvar)  (This)->lpVtbl->SetEx(This,lnFormatType,pvar)
#define IADsNameTranslate_GetEx(This,lnFormatType,pvar)  (This)->lpVtbl->GetEx(This,lnFormatType,pvar)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsNameTranslate_put_ChaseReferral_Proxy (IADsNameTranslate * This, long lnChaseReferral);
void __RPC_STUB IADsNameTranslate_put_ChaseReferral_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsNameTranslate_Init_Proxy (IADsNameTranslate * This, long lnSetType, BSTR bstrADsPath);
void __RPC_STUB IADsNameTranslate_Init_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsNameTranslate_InitEx_Proxy (IADsNameTranslate * This, long lnSetType, BSTR bstrADsPath, BSTR bstrUserID, BSTR bstrDomain, BSTR bstrPassword);
void __RPC_STUB IADsNameTranslate_InitEx_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsNameTranslate_Set_Proxy (IADsNameTranslate * This, long lnSetType, BSTR bstrADsPath);
void __RPC_STUB IADsNameTranslate_Set_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsNameTranslate_Get_Proxy (IADsNameTranslate * This, long lnFormatType, BSTR * pbstrADsPath);
void __RPC_STUB IADsNameTranslate_Get_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsNameTranslate_SetEx_Proxy (IADsNameTranslate * This, long lnFormatType, VARIANT pvar);
void __RPC_STUB IADsNameTranslate_SetEx_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsNameTranslate_GetEx_Proxy (IADsNameTranslate * This, long lnFormatType, VARIANT * pvar);
void __RPC_STUB IADsNameTranslate_GetEx_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_NameTranslate;
#ifdef __cplusplus
class DECLSPEC_UUID ("274fae1f-3626-11d1-a3a4-00c04fb950dc") NameTranslate;
#endif

#ifndef __IADsCaseIgnoreList_INTERFACE_DEFINED__
#define __IADsCaseIgnoreList_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsCaseIgnoreList;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("7b66b533-4680-11d1-a3b4-00c04fb950dc") IADsCaseIgnoreList:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_CaseIgnoreList (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_CaseIgnoreList (VARIANT vCaseIgnoreList) = 0;
};
#else
typedef struct IADsCaseIgnoreListVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsCaseIgnoreList * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsCaseIgnoreList * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsCaseIgnoreList * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsCaseIgnoreList * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsCaseIgnoreList * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsCaseIgnoreList * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsCaseIgnoreList * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_CaseIgnoreList) (IADsCaseIgnoreList * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_CaseIgnoreList) (IADsCaseIgnoreList * This, VARIANT vCaseIgnoreList);
    END_INTERFACE
} IADsCaseIgnoreListVtbl;
interface IADsCaseIgnoreList {
    CONST_VTBL struct IADsCaseIgnoreListVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsCaseIgnoreList_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsCaseIgnoreList_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsCaseIgnoreList_Release(This)  (This)->lpVtbl->Release(This)
#define IADsCaseIgnoreList_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsCaseIgnoreList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsCaseIgnoreList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsCaseIgnoreList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsCaseIgnoreList_get_CaseIgnoreList(This,retval)  (This)->lpVtbl->get_CaseIgnoreList(This,retval)
#define IADsCaseIgnoreList_put_CaseIgnoreList(This,vCaseIgnoreList)  (This)->lpVtbl->put_CaseIgnoreList(This,vCaseIgnoreList)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsCaseIgnoreList_get_CaseIgnoreList_Proxy (IADsCaseIgnoreList * This, VARIANT * retval);
void __RPC_STUB IADsCaseIgnoreList_get_CaseIgnoreList_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsCaseIgnoreList_put_CaseIgnoreList_Proxy (IADsCaseIgnoreList * This, VARIANT vCaseIgnoreList);
void __RPC_STUB IADsCaseIgnoreList_put_CaseIgnoreList_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_CaseIgnoreList;
#ifdef __cplusplus
class DECLSPEC_UUID ("15f88a55-4680-11d1-a3b4-00c04fb950dc") CaseIgnoreList;
#endif

#ifndef __IADsFaxNumber_INTERFACE_DEFINED__
#define __IADsFaxNumber_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsFaxNumber;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("a910dea9-4680-11d1-a3b4-00c04fb950dc") IADsFaxNumber:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_TelephoneNumber (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_TelephoneNumber (BSTR bstrTelephoneNumber) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Parameters (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Parameters (VARIANT vParameters) = 0;
};
#else
typedef struct IADsFaxNumberVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsFaxNumber * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsFaxNumber * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsFaxNumber * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsFaxNumber * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsFaxNumber * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsFaxNumber * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsFaxNumber * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_TelephoneNumber) (IADsFaxNumber * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_TelephoneNumber) (IADsFaxNumber * This, BSTR bstrTelephoneNumber);
    HRESULT (STDMETHODCALLTYPE * get_Parameters) (IADsFaxNumber * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_Parameters) (IADsFaxNumber * This, VARIANT vParameters);
    END_INTERFACE
} IADsFaxNumberVtbl;
interface IADsFaxNumber {
    CONST_VTBL struct IADsFaxNumberVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsFaxNumber_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsFaxNumber_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsFaxNumber_Release(This)  (This)->lpVtbl->Release(This)
#define IADsFaxNumber_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsFaxNumber_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsFaxNumber_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsFaxNumber_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsFaxNumber_get_TelephoneNumber(This,retval)  (This)->lpVtbl->get_TelephoneNumber(This,retval)
#define IADsFaxNumber_put_TelephoneNumber(This,bstrTelephoneNumber)  (This)->lpVtbl->put_TelephoneNumber(This,bstrTelephoneNumber)
#define IADsFaxNumber_get_Parameters(This,retval)  (This)->lpVtbl->get_Parameters(This,retval)
#define IADsFaxNumber_put_Parameters(This,vParameters)  (This)->lpVtbl->put_Parameters(This,vParameters)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsFaxNumber_get_TelephoneNumber_Proxy (IADsFaxNumber * This, BSTR * retval);
void __RPC_STUB IADsFaxNumber_get_TelephoneNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFaxNumber_put_TelephoneNumber_Proxy (IADsFaxNumber * This, BSTR bstrTelephoneNumber);
void __RPC_STUB IADsFaxNumber_put_TelephoneNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFaxNumber_get_Parameters_Proxy (IADsFaxNumber * This, VARIANT * retval);
void __RPC_STUB IADsFaxNumber_get_Parameters_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsFaxNumber_put_Parameters_Proxy (IADsFaxNumber * This, VARIANT vParameters);
void __RPC_STUB IADsFaxNumber_put_Parameters_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_FaxNumber;
#ifdef __cplusplus
class DECLSPEC_UUID ("a5062215-4681-11d1-a3b4-00c04fb950dc") FaxNumber;
#endif

#ifndef __IADsNetAddress_INTERFACE_DEFINED__
#define __IADsNetAddress_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsNetAddress;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("b21a50a9-4080-11d1-a3ac-00c04fb950dc") IADsNetAddress:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_AddressType (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AddressType (long lnAddressType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Address (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Address (VARIANT vAddress) = 0;
};
#else
typedef struct IADsNetAddressVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsNetAddress * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsNetAddress * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsNetAddress * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsNetAddress * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsNetAddress * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsNetAddress * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsNetAddress * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_AddressType) (IADsNetAddress * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_AddressType) (IADsNetAddress * This, long lnAddressType);
    HRESULT (STDMETHODCALLTYPE * get_Address) (IADsNetAddress * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_Address) (IADsNetAddress * This, VARIANT vAddress);
    END_INTERFACE
} IADsNetAddressVtbl;
interface IADsNetAddress {
    CONST_VTBL struct IADsNetAddressVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsNetAddress_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsNetAddress_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsNetAddress_Release(This)  (This)->lpVtbl->Release(This)
#define IADsNetAddress_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsNetAddress_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsNetAddress_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsNetAddress_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsNetAddress_get_AddressType(This,retval)  (This)->lpVtbl->get_AddressType(This,retval)
#define IADsNetAddress_put_AddressType(This,lnAddressType)  (This)->lpVtbl->put_AddressType(This,lnAddressType)
#define IADsNetAddress_get_Address(This,retval)  (This)->lpVtbl->get_Address(This,retval)
#define IADsNetAddress_put_Address(This,vAddress)  (This)->lpVtbl->put_Address(This,vAddress)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsNetAddress_get_AddressType_Proxy (IADsNetAddress * This, long *retval);
void __RPC_STUB IADsNetAddress_get_AddressType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsNetAddress_put_AddressType_Proxy (IADsNetAddress * This, long lnAddressType);
void __RPC_STUB IADsNetAddress_put_AddressType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsNetAddress_get_Address_Proxy (IADsNetAddress * This, VARIANT * retval);
void __RPC_STUB IADsNetAddress_get_Address_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsNetAddress_put_Address_Proxy (IADsNetAddress * This, VARIANT vAddress);
void __RPC_STUB IADsNetAddress_put_Address_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_NetAddress;
#ifdef __cplusplus
class DECLSPEC_UUID ("b0b71247-4080-11d1-a3ac-00c04fb950dc") NetAddress;
#endif

#ifndef __IADsOctetList_INTERFACE_DEFINED__
#define __IADsOctetList_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsOctetList;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("7b28b80f-4680-11d1-a3b4-00c04fb950dc") IADsOctetList:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_OctetList (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_OctetList (VARIANT vOctetList) = 0;
};
#else
typedef struct IADsOctetListVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsOctetList * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsOctetList * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsOctetList * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsOctetList * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsOctetList * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsOctetList * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsOctetList * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_OctetList) (IADsOctetList * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_OctetList) (IADsOctetList * This, VARIANT vOctetList);
    END_INTERFACE
} IADsOctetListVtbl;
interface IADsOctetList {
    CONST_VTBL struct IADsOctetListVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsOctetList_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsOctetList_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsOctetList_Release(This)  (This)->lpVtbl->Release(This)
#define IADsOctetList_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsOctetList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsOctetList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsOctetList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsOctetList_get_OctetList(This,retval)  (This)->lpVtbl->get_OctetList(This,retval)
#define IADsOctetList_put_OctetList(This,vOctetList)  (This)->lpVtbl->put_OctetList(This,vOctetList)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsOctetList_get_OctetList_Proxy (IADsOctetList * This, VARIANT * retval);
void __RPC_STUB IADsOctetList_get_OctetList_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsOctetList_put_OctetList_Proxy (IADsOctetList * This, VARIANT vOctetList);
void __RPC_STUB IADsOctetList_put_OctetList_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_OctetList;
#ifdef __cplusplus
class DECLSPEC_UUID ("1241400f-4680-11d1-a3b4-00c04fb950dc") OctetList;
#endif

#ifndef __IADsEmail_INTERFACE_DEFINED__
#define __IADsEmail_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsEmail;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("97af011a-478e-11d1-a3b4-00c04fb950dc") IADsEmail:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Type (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Type (long lnType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Address (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Address (BSTR bstrAddress) = 0;
};
#else
typedef struct IADsEmailVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsEmail * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsEmail * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsEmail * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsEmail * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsEmail * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsEmail * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsEmail * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Type) (IADsEmail * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Type) (IADsEmail * This, long lnType);
    HRESULT (STDMETHODCALLTYPE * get_Address) (IADsEmail * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Address) (IADsEmail * This, BSTR bstrAddress);
    END_INTERFACE
} IADsEmailVtbl;
interface IADsEmail {
    CONST_VTBL struct IADsEmailVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsEmail_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsEmail_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsEmail_Release(This)  (This)->lpVtbl->Release(This)
#define IADsEmail_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsEmail_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsEmail_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsEmail_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsEmail_get_Type(This,retval)  (This)->lpVtbl->get_Type(This,retval)
#define IADsEmail_put_Type(This,lnType)  (This)->lpVtbl->put_Type(This,lnType)
#define IADsEmail_get_Address(This,retval)  (This)->lpVtbl->get_Address(This,retval)
#define IADsEmail_put_Address(This,bstrAddress)  (This)->lpVtbl->put_Address(This,bstrAddress)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsEmail_get_Type_Proxy (IADsEmail * This, long *retval);
void __RPC_STUB IADsEmail_get_Type_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsEmail_put_Type_Proxy (IADsEmail * This, long lnType);
void __RPC_STUB IADsEmail_put_Type_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsEmail_get_Address_Proxy (IADsEmail * This, BSTR * retval);
void __RPC_STUB IADsEmail_get_Address_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsEmail_put_Address_Proxy (IADsEmail * This, BSTR bstrAddress);
void __RPC_STUB IADsEmail_put_Address_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_Email;
#ifdef __cplusplus
class DECLSPEC_UUID ("8f92a857-478e-11d1-a3b4-00c04fb950dc") Email;
#endif

#ifndef __IADsPath_INTERFACE_DEFINED__
#define __IADsPath_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsPath;
#if defined(__cplusplus) && !defined(CINTERFACE)
 MIDL_INTERFACE ("b287fcd5-4080-11d1-a3ac-00c04fb950dc") IADsPath:public IDispatch {
  public:
    virtual HRESULT STDMETHODCALLTYPE get_Type (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Type (long lnType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_VolumeName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_VolumeName (BSTR bstrVolumeName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Path (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Path (BSTR bstrPath) = 0;
};
#else
typedef struct IADsPathVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsPath * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsPath * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsPath * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsPath * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsPath * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsPath * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsPath * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_Type) (IADsPath * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Type) (IADsPath * This, long lnType);
    HRESULT (STDMETHODCALLTYPE * get_VolumeName) (IADsPath * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_VolumeName) (IADsPath * This, BSTR bstrVolumeName);
    HRESULT (STDMETHODCALLTYPE * get_Path) (IADsPath * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_Path) (IADsPath * This, BSTR bstrPath);
    END_INTERFACE
} IADsPathVtbl;
interface IADsPath {
    CONST_VTBL struct IADsPathVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsPath_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsPath_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsPath_Release(This)  (This)->lpVtbl->Release(This)
#define IADsPath_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsPath_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsPath_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsPath_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsPath_get_Type(This,retval)  (This)->lpVtbl->get_Type(This,retval)
#define IADsPath_put_Type(This,lnType)  (This)->lpVtbl->put_Type(This,lnType)
#define IADsPath_get_VolumeName(This,retval)  (This)->lpVtbl->get_VolumeName(This,retval)
#define IADsPath_put_VolumeName(This,bstrVolumeName)  (This)->lpVtbl->put_VolumeName(This,bstrVolumeName)
#define IADsPath_get_Path(This,retval)  (This)->lpVtbl->get_Path(This,retval)
#define IADsPath_put_Path(This,bstrPath)  (This)->lpVtbl->put_Path(This,bstrPath)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsPath_get_Type_Proxy (IADsPath * This, long *retval);
void __RPC_STUB IADsPath_get_Type_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPath_put_Type_Proxy (IADsPath * This, long lnType);
void __RPC_STUB IADsPath_put_Type_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPath_get_VolumeName_Proxy (IADsPath * This, BSTR * retval);
void __RPC_STUB IADsPath_get_VolumeName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPath_put_VolumeName_Proxy (IADsPath * This, BSTR bstrVolumeName);
void __RPC_STUB IADsPath_put_VolumeName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPath_get_Path_Proxy (IADsPath * This, BSTR * retval);
void __RPC_STUB IADsPath_get_Path_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPath_put_Path_Proxy (IADsPath * This, BSTR bstrPath);
void __RPC_STUB IADsPath_put_Path_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_Path;
#ifdef __cplusplus
class DECLSPEC_UUID ("b2538919-4080-11d1-a3ac-00c04fb950dc") Path;
#endif

#ifndef __IADsReplicaPointer_INTERFACE_DEFINED__
#define __IADsReplicaPointer_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsReplicaPointer;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("f60fb803-4080-11d1-a3ac-00c04fb950dc") IADsReplicaPointer:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_ServerName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ServerName (BSTR bstrServerName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ReplicaType (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ReplicaType (long lnReplicaType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ReplicaNumber (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ReplicaNumber (long lnReplicaNumber) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Count (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Count (long lnCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ReplicaAddressHints (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ReplicaAddressHints (VARIANT vReplicaAddressHints) = 0;
};
#else
typedef struct IADsReplicaPointerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsReplicaPointer * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsReplicaPointer * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsReplicaPointer * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsReplicaPointer * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsReplicaPointer * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsReplicaPointer * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsReplicaPointer * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_ServerName) (IADsReplicaPointer * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_ServerName) (IADsReplicaPointer * This, BSTR bstrServerName);
    HRESULT (STDMETHODCALLTYPE * get_ReplicaType) (IADsReplicaPointer * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_ReplicaType) (IADsReplicaPointer * This, long lnReplicaType);
    HRESULT (STDMETHODCALLTYPE * get_ReplicaNumber) (IADsReplicaPointer * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_ReplicaNumber) (IADsReplicaPointer * This, long lnReplicaNumber);
    HRESULT (STDMETHODCALLTYPE * get_Count) (IADsReplicaPointer * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Count) (IADsReplicaPointer * This, long lnCount);
    HRESULT (STDMETHODCALLTYPE * get_ReplicaAddressHints) (IADsReplicaPointer * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_ReplicaAddressHints) (IADsReplicaPointer * This, VARIANT vReplicaAddressHints);
    END_INTERFACE
} IADsReplicaPointerVtbl;
interface IADsReplicaPointer {
    CONST_VTBL struct IADsReplicaPointerVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsReplicaPointer_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsReplicaPointer_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsReplicaPointer_Release(This)  (This)->lpVtbl->Release(This)
#define IADsReplicaPointer_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsReplicaPointer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsReplicaPointer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsReplicaPointer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsReplicaPointer_get_ServerName(This,retval)  (This)->lpVtbl->get_ServerName(This,retval)
#define IADsReplicaPointer_put_ServerName(This,bstrServerName)  (This)->lpVtbl->put_ServerName(This,bstrServerName)
#define IADsReplicaPointer_get_ReplicaType(This,retval)  (This)->lpVtbl->get_ReplicaType(This,retval)
#define IADsReplicaPointer_put_ReplicaType(This,lnReplicaType)  (This)->lpVtbl->put_ReplicaType(This,lnReplicaType)
#define IADsReplicaPointer_get_ReplicaNumber(This,retval)  (This)->lpVtbl->get_ReplicaNumber(This,retval)
#define IADsReplicaPointer_put_ReplicaNumber(This,lnReplicaNumber)  (This)->lpVtbl->put_ReplicaNumber(This,lnReplicaNumber)
#define IADsReplicaPointer_get_Count(This,retval)  (This)->lpVtbl->get_Count(This,retval)
#define IADsReplicaPointer_put_Count(This,lnCount)  (This)->lpVtbl->put_Count(This,lnCount)
#define IADsReplicaPointer_get_ReplicaAddressHints(This,retval)  (This)->lpVtbl->get_ReplicaAddressHints(This,retval)
#define IADsReplicaPointer_put_ReplicaAddressHints(This,vReplicaAddressHints)  (This)->lpVtbl->put_ReplicaAddressHints(This,vReplicaAddressHints)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsReplicaPointer_get_ServerName_Proxy (IADsReplicaPointer * This, BSTR * retval);
void __RPC_STUB IADsReplicaPointer_get_ServerName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsReplicaPointer_put_ServerName_Proxy (IADsReplicaPointer * This, BSTR bstrServerName);
void __RPC_STUB IADsReplicaPointer_put_ServerName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsReplicaPointer_get_ReplicaType_Proxy (IADsReplicaPointer * This, long *retval);
void __RPC_STUB IADsReplicaPointer_get_ReplicaType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsReplicaPointer_put_ReplicaType_Proxy (IADsReplicaPointer * This, long lnReplicaType);
void __RPC_STUB IADsReplicaPointer_put_ReplicaType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsReplicaPointer_get_ReplicaNumber_Proxy (IADsReplicaPointer * This, long *retval);
void __RPC_STUB IADsReplicaPointer_get_ReplicaNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsReplicaPointer_put_ReplicaNumber_Proxy (IADsReplicaPointer * This, long lnReplicaNumber);
void __RPC_STUB IADsReplicaPointer_put_ReplicaNumber_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsReplicaPointer_get_Count_Proxy (IADsReplicaPointer * This, long *retval);
void __RPC_STUB IADsReplicaPointer_get_Count_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsReplicaPointer_put_Count_Proxy (IADsReplicaPointer * This, long lnCount);
void __RPC_STUB IADsReplicaPointer_put_Count_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsReplicaPointer_get_ReplicaAddressHints_Proxy (IADsReplicaPointer * This, VARIANT * retval);
void __RPC_STUB IADsReplicaPointer_get_ReplicaAddressHints_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsReplicaPointer_put_ReplicaAddressHints_Proxy (IADsReplicaPointer * This, VARIANT vReplicaAddressHints);
void __RPC_STUB IADsReplicaPointer_put_ReplicaAddressHints_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_ReplicaPointer;
#ifdef __cplusplus
class DECLSPEC_UUID ("f5d1badf-4080-11d1-a3ac-00c04fb950dc") ReplicaPointer;
#endif

#ifndef __IADsAcl_INTERFACE_DEFINED__
#define __IADsAcl_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsAcl;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("8452d3ab-0869-11d1-a377-00c04fb950dc") IADsAcl:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_ProtectedAttrName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ProtectedAttrName (BSTR bstrProtectedAttrName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SubjectName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SubjectName (BSTR bstrSubjectName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Privileges (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Privileges (long lnPrivileges) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyAcl (IDispatch ** ppAcl) = 0;
};
#else
typedef struct IADsAclVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsAcl * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsAcl * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsAcl * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsAcl * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsAcl * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsAcl * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsAcl * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_ProtectedAttrName) (IADsAcl * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_ProtectedAttrName) (IADsAcl * This, BSTR bstrProtectedAttrName);
    HRESULT (STDMETHODCALLTYPE * get_SubjectName) (IADsAcl * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_SubjectName) (IADsAcl * This, BSTR bstrSubjectName);
    HRESULT (STDMETHODCALLTYPE * get_Privileges) (IADsAcl * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Privileges) (IADsAcl * This, long lnPrivileges);
    HRESULT (STDMETHODCALLTYPE * CopyAcl) (IADsAcl * This, IDispatch ** ppAcl);
    END_INTERFACE
} IADsAclVtbl;
interface IADsAcl {
    CONST_VTBL struct IADsAclVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsAcl_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsAcl_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsAcl_Release(This)  (This)->lpVtbl->Release(This)
#define IADsAcl_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsAcl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsAcl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsAcl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsAcl_get_ProtectedAttrName(This,retval)  (This)->lpVtbl->get_ProtectedAttrName(This,retval)
#define IADsAcl_put_ProtectedAttrName(This,bstrProtectedAttrName)  (This)->lpVtbl->put_ProtectedAttrName(This,bstrProtectedAttrName)
#define IADsAcl_get_SubjectName(This,retval)  (This)->lpVtbl->get_SubjectName(This,retval)
#define IADsAcl_put_SubjectName(This,bstrSubjectName)  (This)->lpVtbl->put_SubjectName(This,bstrSubjectName)
#define IADsAcl_get_Privileges(This,retval)  (This)->lpVtbl->get_Privileges(This,retval)
#define IADsAcl_put_Privileges(This,lnPrivileges)  (This)->lpVtbl->put_Privileges(This,lnPrivileges)
#define IADsAcl_CopyAcl(This,ppAcl)  (This)->lpVtbl->CopyAcl(This,ppAcl)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsAcl_get_ProtectedAttrName_Proxy (IADsAcl * This, BSTR * retval);
void __RPC_STUB IADsAcl_get_ProtectedAttrName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAcl_put_ProtectedAttrName_Proxy (IADsAcl * This, BSTR bstrProtectedAttrName);
void __RPC_STUB IADsAcl_put_ProtectedAttrName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAcl_get_SubjectName_Proxy (IADsAcl * This, BSTR * retval);
void __RPC_STUB IADsAcl_get_SubjectName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAcl_put_SubjectName_Proxy (IADsAcl * This, BSTR bstrSubjectName);
void __RPC_STUB IADsAcl_put_SubjectName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAcl_get_Privileges_Proxy (IADsAcl * This, long *retval);
void __RPC_STUB IADsAcl_get_Privileges_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAcl_put_Privileges_Proxy (IADsAcl * This, long lnPrivileges);
void __RPC_STUB IADsAcl_put_Privileges_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsAcl_CopyAcl_Proxy (IADsAcl * This, IDispatch ** ppAcl);
void __RPC_STUB IADsAcl_CopyAcl_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsTimestamp_INTERFACE_DEFINED__
#define __IADsTimestamp_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsTimestamp;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("b2f5a901-4080-11d1-a3ac-00c04fb950dc") IADsTimestamp:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_WholeSeconds (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_WholeSeconds (long lnWholeSeconds) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EventID (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EventID (long lnEventID) = 0;
};
#else
typedef struct IADsTimestampVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsTimestamp * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsTimestamp * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsTimestamp * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsTimestamp * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsTimestamp * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsTimestamp * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsTimestamp * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_WholeSeconds) (IADsTimestamp * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_WholeSeconds) (IADsTimestamp * This, long lnWholeSeconds);
    HRESULT (STDMETHODCALLTYPE * get_EventID) (IADsTimestamp * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_EventID) (IADsTimestamp * This, long lnEventID);
    END_INTERFACE
} IADsTimestampVtbl;
interface IADsTimestamp {
    CONST_VTBL struct IADsTimestampVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsTimestamp_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsTimestamp_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsTimestamp_Release(This)  (This)->lpVtbl->Release(This)
#define IADsTimestamp_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsTimestamp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsTimestamp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsTimestamp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsTimestamp_get_WholeSeconds(This,retval)  (This)->lpVtbl->get_WholeSeconds(This,retval)
#define IADsTimestamp_put_WholeSeconds(This,lnWholeSeconds)  (This)->lpVtbl->put_WholeSeconds(This,lnWholeSeconds)
#define IADsTimestamp_get_EventID(This,retval)  (This)->lpVtbl->get_EventID(This,retval)
#define IADsTimestamp_put_EventID(This,lnEventID)  (This)->lpVtbl->put_EventID(This,lnEventID)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsTimestamp_get_WholeSeconds_Proxy (IADsTimestamp * This, long *retval);
void __RPC_STUB IADsTimestamp_get_WholeSeconds_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsTimestamp_put_WholeSeconds_Proxy (IADsTimestamp * This, long lnWholeSeconds);
void __RPC_STUB IADsTimestamp_put_WholeSeconds_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsTimestamp_get_EventID_Proxy (IADsTimestamp * This, long *retval);
void __RPC_STUB IADsTimestamp_get_EventID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsTimestamp_put_EventID_Proxy (IADsTimestamp * This, long lnEventID);
void __RPC_STUB IADsTimestamp_put_EventID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_Timestamp;
#ifdef __cplusplus
class DECLSPEC_UUID ("b2bed2eb-4080-11d1-a3ac-00c04fb950dc") Timestamp;
#endif

#ifndef __IADsPostalAddress_INTERFACE_DEFINED__
#define __IADsPostalAddress_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsPostalAddress;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("7adecf29-4680-11d1-a3b4-00c04fb950dc") IADsPostalAddress:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_PostalAddress (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_PostalAddress (VARIANT vPostalAddress) = 0;
};
#else
typedef struct IADsPostalAddressVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsPostalAddress * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsPostalAddress * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsPostalAddress * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsPostalAddress * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsPostalAddress * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsPostalAddress * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsPostalAddress * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_PostalAddress) (IADsPostalAddress * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_PostalAddress) (IADsPostalAddress * This, VARIANT vPostalAddress);
    END_INTERFACE
} IADsPostalAddressVtbl;
interface IADsPostalAddress {
    CONST_VTBL struct IADsPostalAddressVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsPostalAddress_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsPostalAddress_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsPostalAddress_Release(This)  (This)->lpVtbl->Release(This)
#define IADsPostalAddress_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsPostalAddress_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsPostalAddress_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsPostalAddress_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsPostalAddress_get_PostalAddress(This,retval)  (This)->lpVtbl->get_PostalAddress(This,retval)
#define IADsPostalAddress_put_PostalAddress(This,vPostalAddress)  (This)->lpVtbl->put_PostalAddress(This,vPostalAddress)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsPostalAddress_get_PostalAddress_Proxy (IADsPostalAddress * This, VARIANT * retval);
void __RPC_STUB IADsPostalAddress_get_PostalAddress_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPostalAddress_put_PostalAddress_Proxy (IADsPostalAddress * This, VARIANT vPostalAddress);
void __RPC_STUB IADsPostalAddress_put_PostalAddress_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_PostalAddress;
#ifdef __cplusplus
class DECLSPEC_UUID ("0a75afcd-4680-11d1-a3b4-00c04fb950dc") PostalAddress;
#endif

#ifndef __IADsBackLink_INTERFACE_DEFINED__
#define __IADsBackLink_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsBackLink;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("fd1302bd-4080-11d1-a3ac-00c04fb950dc") IADsBackLink:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_RemoteID (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RemoteID (long lnRemoteID) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ObjectName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ObjectName (BSTR bstrObjectName) = 0;
};
#else
typedef struct IADsBackLinkVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsBackLink * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsBackLink * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsBackLink * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsBackLink * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsBackLink * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsBackLink * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsBackLink * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_RemoteID) (IADsBackLink * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_RemoteID) (IADsBackLink * This, long lnRemoteID);
    HRESULT (STDMETHODCALLTYPE * get_ObjectName) (IADsBackLink * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_ObjectName) (IADsBackLink * This, BSTR bstrObjectName);
    END_INTERFACE
} IADsBackLinkVtbl;
interface IADsBackLink {
    CONST_VTBL struct IADsBackLinkVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsBackLink_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsBackLink_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsBackLink_Release(This)  (This)->lpVtbl->Release(This)
#define IADsBackLink_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsBackLink_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsBackLink_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsBackLink_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsBackLink_get_RemoteID(This,retval)  (This)->lpVtbl->get_RemoteID(This,retval)
#define IADsBackLink_put_RemoteID(This,lnRemoteID)  (This)->lpVtbl->put_RemoteID(This,lnRemoteID)
#define IADsBackLink_get_ObjectName(This,retval)  (This)->lpVtbl->get_ObjectName(This,retval)
#define IADsBackLink_put_ObjectName(This,bstrObjectName)  (This)->lpVtbl->put_ObjectName(This,bstrObjectName)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsBackLink_get_RemoteID_Proxy (IADsBackLink * This, long *retval);
void __RPC_STUB IADsBackLink_get_RemoteID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsBackLink_put_RemoteID_Proxy (IADsBackLink * This, long lnRemoteID);
void __RPC_STUB IADsBackLink_put_RemoteID_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsBackLink_get_ObjectName_Proxy (IADsBackLink * This, BSTR * retval);
void __RPC_STUB IADsBackLink_get_ObjectName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsBackLink_put_ObjectName_Proxy (IADsBackLink * This, BSTR bstrObjectName);
void __RPC_STUB IADsBackLink_put_ObjectName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_BackLink;
#ifdef __cplusplus
class DECLSPEC_UUID ("fcbf906f-4080-11d1-a3ac-00c04fb950dc") BackLink;
#endif

#ifndef __IADsTypedName_INTERFACE_DEFINED__
#define __IADsTypedName_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsTypedName;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("b371a349-4080-11d1-a3ac-00c04fb950dc") IADsTypedName:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_ObjectName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ObjectName (BSTR bstrObjectName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Level (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Level (long lnLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Interval (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Interval (long lnInterval) = 0;
};
#else
typedef struct IADsTypedNameVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsTypedName * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsTypedName * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsTypedName * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsTypedName * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsTypedName * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsTypedName * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsTypedName * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_ObjectName) (IADsTypedName * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_ObjectName) (IADsTypedName * This, BSTR bstrObjectName);
    HRESULT (STDMETHODCALLTYPE * get_Level) (IADsTypedName * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Level) (IADsTypedName * This, long lnLevel);
    HRESULT (STDMETHODCALLTYPE * get_Interval) (IADsTypedName * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Interval) (IADsTypedName * This, long lnInterval);
    END_INTERFACE
} IADsTypedNameVtbl;
interface IADsTypedName {
    CONST_VTBL struct IADsTypedNameVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsTypedName_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsTypedName_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsTypedName_Release(This)  (This)->lpVtbl->Release(This)
#define IADsTypedName_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsTypedName_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsTypedName_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsTypedName_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsTypedName_get_ObjectName(This,retval)  (This)->lpVtbl->get_ObjectName(This,retval)
#define IADsTypedName_put_ObjectName(This,bstrObjectName)  (This)->lpVtbl->put_ObjectName(This,bstrObjectName)
#define IADsTypedName_get_Level(This,retval)  (This)->lpVtbl->get_Level(This,retval)
#define IADsTypedName_put_Level(This,lnLevel)  (This)->lpVtbl->put_Level(This,lnLevel)
#define IADsTypedName_get_Interval(This,retval)  (This)->lpVtbl->get_Interval(This,retval)
#define IADsTypedName_put_Interval(This,lnInterval)  (This)->lpVtbl->put_Interval(This,lnInterval)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsTypedName_get_ObjectName_Proxy (IADsTypedName * This, BSTR * retval);
void __RPC_STUB IADsTypedName_get_ObjectName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsTypedName_put_ObjectName_Proxy (IADsTypedName * This, BSTR bstrObjectName);
void __RPC_STUB IADsTypedName_put_ObjectName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsTypedName_get_Level_Proxy (IADsTypedName * This, long *retval);
void __RPC_STUB IADsTypedName_get_Level_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsTypedName_put_Level_Proxy (IADsTypedName * This, long lnLevel);
void __RPC_STUB IADsTypedName_put_Level_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsTypedName_get_Interval_Proxy (IADsTypedName * This, long *retval);
void __RPC_STUB IADsTypedName_get_Interval_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsTypedName_put_Interval_Proxy (IADsTypedName * This, long lnInterval);
void __RPC_STUB IADsTypedName_put_Interval_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_TypedName;
#ifdef __cplusplus
class DECLSPEC_UUID ("b33143cb-4080-11d1-a3ac-00c04fb950dc") TypedName;
#endif

#ifndef __IADsHold_INTERFACE_DEFINED__
#define __IADsHold_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsHold;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("b3eb3b37-4080-11d1-a3ac-00c04fb950dc") IADsHold:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_ObjectName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ObjectName (BSTR bstrObjectName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Amount (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Amount (long lnAmount) = 0;
};
#else
typedef struct IADsHoldVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsHold * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsHold * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsHold * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsHold * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsHold * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsHold * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsHold * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_ObjectName) (IADsHold * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_ObjectName) (IADsHold * This, BSTR bstrObjectName);
    HRESULT (STDMETHODCALLTYPE * get_Amount) (IADsHold * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_Amount) (IADsHold * This, long lnAmount);
    END_INTERFACE
} IADsHoldVtbl;
interface IADsHold {
    CONST_VTBL struct IADsHoldVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsHold_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsHold_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsHold_Release(This)  (This)->lpVtbl->Release(This)
#define IADsHold_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsHold_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsHold_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsHold_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsHold_get_ObjectName(This,retval)  (This)->lpVtbl->get_ObjectName(This,retval)
#define IADsHold_put_ObjectName(This,bstrObjectName)  (This)->lpVtbl->put_ObjectName(This,bstrObjectName)
#define IADsHold_get_Amount(This,retval)  (This)->lpVtbl->get_Amount(This,retval)
#define IADsHold_put_Amount(This,lnAmount)  (This)->lpVtbl->put_Amount(This,lnAmount)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsHold_get_ObjectName_Proxy (IADsHold * This, BSTR * retval);
void __RPC_STUB IADsHold_get_ObjectName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsHold_put_ObjectName_Proxy (IADsHold * This, BSTR bstrObjectName);
void __RPC_STUB IADsHold_put_ObjectName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsHold_get_Amount_Proxy (IADsHold * This, long *retval);
void __RPC_STUB IADsHold_get_Amount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsHold_put_Amount_Proxy (IADsHold * This, long lnAmount);
void __RPC_STUB IADsHold_put_Amount_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_Hold;
#ifdef __cplusplus
class DECLSPEC_UUID ("b3ad3e13-4080-11d1-a3ac-00c04fb950dc") Hold;
#endif

#ifndef __IADsObjectOptions_INTERFACE_DEFINED__
#define __IADsObjectOptions_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsObjectOptions;
#if defined(__cplusplus) && !defined(CINTERFACE)
 MIDL_INTERFACE ("46f14fda-232b-11d1-a808-00c04fd8d5a8") IADsObjectOptions:public IDispatch {
  public:
    virtual HRESULT STDMETHODCALLTYPE GetOption (long lnOption, VARIANT * pvValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOption (long lnOption, VARIANT vValue) = 0;
};
#else
typedef struct IADsObjectOptionsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsObjectOptions * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsObjectOptions * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsObjectOptions * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsObjectOptions * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsObjectOptions * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsObjectOptions * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsObjectOptions * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * GetOption) (IADsObjectOptions * This, long lnOption, VARIANT * pvValue);
    HRESULT (STDMETHODCALLTYPE * SetOption) (IADsObjectOptions * This, long lnOption, VARIANT vValue);
    END_INTERFACE
} IADsObjectOptionsVtbl;
interface IADsObjectOptions {
    CONST_VTBL struct IADsObjectOptionsVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsObjectOptions_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsObjectOptions_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsObjectOptions_Release(This)  (This)->lpVtbl->Release(This)
#define IADsObjectOptions_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsObjectOptions_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsObjectOptions_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsObjectOptions_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsObjectOptions_GetOption(This,lnOption,pvValue)  (This)->lpVtbl->GetOption(This,lnOption,pvValue)
#define IADsObjectOptions_SetOption(This,lnOption,vValue)  (This)->lpVtbl->SetOption(This,lnOption,vValue)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsObjectOptions_GetOption_Proxy (IADsObjectOptions * This, long lnOption, VARIANT * pvValue);
void __RPC_STUB IADsObjectOptions_GetOption_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsObjectOptions_SetOption_Proxy (IADsObjectOptions * This, long lnOption, VARIANT vValue);
void __RPC_STUB IADsObjectOptions_SetOption_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif

#ifndef __IADsPathname_INTERFACE_DEFINED__
#define __IADsPathname_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsPathname;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("d592aed4-f420-11d0-a36e-00c04fb950dc") IADsPathname:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE Set (BSTR bstrADsPath, long lnSetType) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetDisplayType (long lnDisplayType) = 0;
    virtual HRESULT STDMETHODCALLTYPE Retrieve (long lnFormatType, BSTR * pbstrADsPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetNumElements (long *plnNumPathElements) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetElement (long lnElementIndex, BSTR * pbstrElement) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddLeafElement (BSTR bstrLeafElement) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveLeafElement (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE CopyPath (IDispatch ** ppAdsPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEscapedElement (long lnReserved, BSTR bstrInStr, BSTR * pbstrOutStr) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EscapedMode (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EscapedMode (long lnEscapedMode) = 0;
};
#else
typedef struct IADsPathnameVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsPathname * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsPathname * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsPathname * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsPathname * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsPathname * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsPathname * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsPathname * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * Set) (IADsPathname * This, BSTR bstrADsPath, long lnSetType);
    HRESULT (STDMETHODCALLTYPE * SetDisplayType) (IADsPathname * This, long lnDisplayType);
    HRESULT (STDMETHODCALLTYPE * Retrieve) (IADsPathname * This, long lnFormatType, BSTR * pbstrADsPath);
    HRESULT (STDMETHODCALLTYPE * GetNumElements) (IADsPathname * This, long *plnNumPathElements);
    HRESULT (STDMETHODCALLTYPE * GetElement) (IADsPathname * This, long lnElementIndex, BSTR * pbstrElement);
    HRESULT (STDMETHODCALLTYPE * AddLeafElement) (IADsPathname * This, BSTR bstrLeafElement);
    HRESULT (STDMETHODCALLTYPE * RemoveLeafElement) (IADsPathname * This);
    HRESULT (STDMETHODCALLTYPE * CopyPath) (IADsPathname * This, IDispatch ** ppAdsPath);
    HRESULT (STDMETHODCALLTYPE * GetEscapedElement) (IADsPathname * This, long lnReserved, BSTR bstrInStr, BSTR * pbstrOutStr);
    HRESULT (STDMETHODCALLTYPE * get_EscapedMode) (IADsPathname * This, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_EscapedMode) (IADsPathname * This, long lnEscapedMode);
    END_INTERFACE
} IADsPathnameVtbl;
interface IADsPathname {
    CONST_VTBL struct IADsPathnameVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsPathname_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsPathname_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsPathname_Release(This)  (This)->lpVtbl->Release(This)
#define IADsPathname_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsPathname_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsPathname_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsPathname_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsPathname_Set(This,bstrADsPath,lnSetType)  (This)->lpVtbl->Set(This,bstrADsPath,lnSetType)
#define IADsPathname_SetDisplayType(This,lnDisplayType)  (This)->lpVtbl->SetDisplayType(This,lnDisplayType)
#define IADsPathname_Retrieve(This,lnFormatType,pbstrADsPath)  (This)->lpVtbl->Retrieve(This,lnFormatType,pbstrADsPath)
#define IADsPathname_GetNumElements(This,plnNumPathElements)  (This)->lpVtbl->GetNumElements(This,plnNumPathElements)
#define IADsPathname_GetElement(This,lnElementIndex,pbstrElement)  (This)->lpVtbl->GetElement(This,lnElementIndex,pbstrElement)
#define IADsPathname_AddLeafElement(This,bstrLeafElement)  (This)->lpVtbl->AddLeafElement(This,bstrLeafElement)
#define IADsPathname_RemoveLeafElement(This)  (This)->lpVtbl->RemoveLeafElement(This)
#define IADsPathname_CopyPath(This,ppAdsPath)  (This)->lpVtbl->CopyPath(This,ppAdsPath)
#define IADsPathname_GetEscapedElement(This,lnReserved,bstrInStr,pbstrOutStr)  (This)->lpVtbl->GetEscapedElement(This,lnReserved,bstrInStr,pbstrOutStr)
#define IADsPathname_get_EscapedMode(This,retval)  (This)->lpVtbl->get_EscapedMode(This,retval)
#define IADsPathname_put_EscapedMode(This,lnEscapedMode)  (This)->lpVtbl->put_EscapedMode(This,lnEscapedMode)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsPathname_Set_Proxy (IADsPathname * This, BSTR bstrADsPath, long lnSetType);
void __RPC_STUB IADsPathname_Set_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPathname_SetDisplayType_Proxy (IADsPathname * This, long lnDisplayType);
void __RPC_STUB IADsPathname_SetDisplayType_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPathname_Retrieve_Proxy (IADsPathname * This, long lnFormatType, BSTR * pbstrADsPath);
void __RPC_STUB IADsPathname_Retrieve_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPathname_GetNumElements_Proxy (IADsPathname * This, long *plnNumPathElements);
void __RPC_STUB IADsPathname_GetNumElements_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPathname_GetElement_Proxy (IADsPathname * This, long lnElementIndex, BSTR * pbstrElement);
void __RPC_STUB IADsPathname_GetElement_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPathname_AddLeafElement_Proxy (IADsPathname * This, BSTR bstrLeafElement);
void __RPC_STUB IADsPathname_AddLeafElement_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPathname_RemoveLeafElement_Proxy (IADsPathname * This);
void __RPC_STUB IADsPathname_RemoveLeafElement_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPathname_CopyPath_Proxy (IADsPathname * This, IDispatch ** ppAdsPath);
void __RPC_STUB IADsPathname_CopyPath_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPathname_GetEscapedElement_Proxy (IADsPathname * This, long lnReserved, BSTR bstrInStr, BSTR * pbstrOutStr);
void __RPC_STUB IADsPathname_GetEscapedElement_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPathname_get_EscapedMode_Proxy (IADsPathname * This, long *retval);
void __RPC_STUB IADsPathname_get_EscapedMode_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsPathname_put_EscapedMode_Proxy (IADsPathname * This, long lnEscapedMode);
void __RPC_STUB IADsPathname_put_EscapedMode_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_Pathname;
#ifdef __cplusplus
class DECLSPEC_UUID ("080d0d78-f421-11d0-a36e-00c04fb950dc") Pathname;
#endif

#ifndef __IADsADSystemInfo_INTERFACE_DEFINED__
#define __IADsADSystemInfo_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsADSystemInfo;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("5BB11929-AFD1-11d2-9CB9-0000F87A369E") IADsADSystemInfo:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_UserName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ComputerName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SiteName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DomainShortName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DomainDNSName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ForestDNSName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PDCRoleOwner (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SchemaRoleOwner (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IsNativeMode (VARIANT_BOOL * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAnyDCName (BSTR * pszDCName) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetDCSiteName (BSTR szServer, BSTR * pszSiteName) = 0;
    virtual HRESULT STDMETHODCALLTYPE RefreshSchemaCache (void) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTrees (VARIANT * pvTrees) = 0;
};
#else
typedef struct IADsADSystemInfoVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsADSystemInfo * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsADSystemInfo * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsADSystemInfo * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsADSystemInfo * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsADSystemInfo * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsADSystemInfo * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsADSystemInfo * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_UserName) (IADsADSystemInfo * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ComputerName) (IADsADSystemInfo * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_SiteName) (IADsADSystemInfo * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_DomainShortName) (IADsADSystemInfo * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_DomainDNSName) (IADsADSystemInfo * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ForestDNSName) (IADsADSystemInfo * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_PDCRoleOwner) (IADsADSystemInfo * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_SchemaRoleOwner) (IADsADSystemInfo * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_IsNativeMode) (IADsADSystemInfo * This, VARIANT_BOOL * retval);
    HRESULT (STDMETHODCALLTYPE * GetAnyDCName) (IADsADSystemInfo * This, BSTR * pszDCName);
    HRESULT (STDMETHODCALLTYPE * GetDCSiteName) (IADsADSystemInfo * This, BSTR szServer, BSTR * pszSiteName);
    HRESULT (STDMETHODCALLTYPE * RefreshSchemaCache) (IADsADSystemInfo * This);
    HRESULT (STDMETHODCALLTYPE * GetTrees) (IADsADSystemInfo * This, VARIANT * pvTrees);
    END_INTERFACE
} IADsADSystemInfoVtbl;
interface IADsADSystemInfo {
    CONST_VTBL struct IADsADSystemInfoVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsADSystemInfo_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsADSystemInfo_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsADSystemInfo_Release(This)  (This)->lpVtbl->Release(This)
#define IADsADSystemInfo_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsADSystemInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsADSystemInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsADSystemInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsADSystemInfo_get_UserName(This,retval)  (This)->lpVtbl->get_UserName(This,retval)
#define IADsADSystemInfo_get_ComputerName(This,retval)  (This)->lpVtbl->get_ComputerName(This,retval)
#define IADsADSystemInfo_get_SiteName(This,retval)  (This)->lpVtbl->get_SiteName(This,retval)
#define IADsADSystemInfo_get_DomainShortName(This,retval)  (This)->lpVtbl->get_DomainShortName(This,retval)
#define IADsADSystemInfo_get_DomainDNSName(This,retval)  (This)->lpVtbl->get_DomainDNSName(This,retval)
#define IADsADSystemInfo_get_ForestDNSName(This,retval)  (This)->lpVtbl->get_ForestDNSName(This,retval)
#define IADsADSystemInfo_get_PDCRoleOwner(This,retval)  (This)->lpVtbl->get_PDCRoleOwner(This,retval)
#define IADsADSystemInfo_get_SchemaRoleOwner(This,retval)  (This)->lpVtbl->get_SchemaRoleOwner(This,retval)
#define IADsADSystemInfo_get_IsNativeMode(This,retval)  (This)->lpVtbl->get_IsNativeMode(This,retval)
#define IADsADSystemInfo_GetAnyDCName(This,pszDCName)  (This)->lpVtbl->GetAnyDCName(This,pszDCName)
#define IADsADSystemInfo_GetDCSiteName(This,szServer,pszSiteName)  (This)->lpVtbl->GetDCSiteName(This,szServer,pszSiteName)
#define IADsADSystemInfo_RefreshSchemaCache(This)  (This)->lpVtbl->RefreshSchemaCache(This)
#define IADsADSystemInfo_GetTrees(This,pvTrees)  (This)->lpVtbl->GetTrees(This,pvTrees)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsADSystemInfo_get_UserName_Proxy (IADsADSystemInfo * This, BSTR * retval);
void __RPC_STUB IADsADSystemInfo_get_UserName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsADSystemInfo_get_ComputerName_Proxy (IADsADSystemInfo * This, BSTR * retval);
void __RPC_STUB IADsADSystemInfo_get_ComputerName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsADSystemInfo_get_SiteName_Proxy (IADsADSystemInfo * This, BSTR * retval);
void __RPC_STUB IADsADSystemInfo_get_SiteName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsADSystemInfo_get_DomainShortName_Proxy (IADsADSystemInfo * This, BSTR * retval);
void __RPC_STUB IADsADSystemInfo_get_DomainShortName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsADSystemInfo_get_DomainDNSName_Proxy (IADsADSystemInfo * This, BSTR * retval);
void __RPC_STUB IADsADSystemInfo_get_DomainDNSName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsADSystemInfo_get_ForestDNSName_Proxy (IADsADSystemInfo * This, BSTR * retval);
void __RPC_STUB IADsADSystemInfo_get_ForestDNSName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsADSystemInfo_get_PDCRoleOwner_Proxy (IADsADSystemInfo * This, BSTR * retval);
void __RPC_STUB IADsADSystemInfo_get_PDCRoleOwner_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsADSystemInfo_get_SchemaRoleOwner_Proxy (IADsADSystemInfo * This, BSTR * retval);
void __RPC_STUB IADsADSystemInfo_get_SchemaRoleOwner_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsADSystemInfo_get_IsNativeMode_Proxy (IADsADSystemInfo * This, VARIANT_BOOL * retval);
void __RPC_STUB IADsADSystemInfo_get_IsNativeMode_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsADSystemInfo_GetAnyDCName_Proxy (IADsADSystemInfo * This, BSTR * pszDCName);
void __RPC_STUB IADsADSystemInfo_GetAnyDCName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsADSystemInfo_GetDCSiteName_Proxy (IADsADSystemInfo * This, BSTR szServer, BSTR * pszSiteName);
void __RPC_STUB IADsADSystemInfo_GetDCSiteName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsADSystemInfo_RefreshSchemaCache_Proxy (IADsADSystemInfo * This);
void __RPC_STUB IADsADSystemInfo_RefreshSchemaCache_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsADSystemInfo_GetTrees_Proxy (IADsADSystemInfo * This, VARIANT * pvTrees);
void __RPC_STUB IADsADSystemInfo_GetTrees_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_ADSystemInfo;
#ifdef __cplusplus
class DECLSPEC_UUID ("50B6327F-AFD1-11d2-9CB9-0000F87A369E") ADSystemInfo;
#endif

#ifndef __IADsWinNTSystemInfo_INTERFACE_DEFINED__
#define __IADsWinNTSystemInfo_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsWinNTSystemInfo;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("6C6D65DC-AFD1-11d2-9CB9-0000F87A369E") IADsWinNTSystemInfo:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_UserName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ComputerName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DomainName (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_PDC (BSTR * retval) = 0;
};
#else
typedef struct IADsWinNTSystemInfoVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsWinNTSystemInfo * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsWinNTSystemInfo * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsWinNTSystemInfo * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsWinNTSystemInfo * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsWinNTSystemInfo * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsWinNTSystemInfo * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsWinNTSystemInfo * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_UserName) (IADsWinNTSystemInfo * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_ComputerName) (IADsWinNTSystemInfo * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_DomainName) (IADsWinNTSystemInfo * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * get_PDC) (IADsWinNTSystemInfo * This, BSTR * retval);
    END_INTERFACE
} IADsWinNTSystemInfoVtbl;
interface IADsWinNTSystemInfo {
    CONST_VTBL struct IADsWinNTSystemInfoVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsWinNTSystemInfo_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsWinNTSystemInfo_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsWinNTSystemInfo_Release(This)  (This)->lpVtbl->Release(This)
#define IADsWinNTSystemInfo_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsWinNTSystemInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsWinNTSystemInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsWinNTSystemInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsWinNTSystemInfo_get_UserName(This,retval)  (This)->lpVtbl->get_UserName(This,retval)
#define IADsWinNTSystemInfo_get_ComputerName(This,retval)  (This)->lpVtbl->get_ComputerName(This,retval)
#define IADsWinNTSystemInfo_get_DomainName(This,retval)  (This)->lpVtbl->get_DomainName(This,retval)
#define IADsWinNTSystemInfo_get_PDC(This,retval)  (This)->lpVtbl->get_PDC(This,retval)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsWinNTSystemInfo_get_UserName_Proxy (IADsWinNTSystemInfo * This, BSTR * retval);
void __RPC_STUB IADsWinNTSystemInfo_get_UserName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsWinNTSystemInfo_get_ComputerName_Proxy (IADsWinNTSystemInfo * This, BSTR * retval);
void __RPC_STUB IADsWinNTSystemInfo_get_ComputerName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsWinNTSystemInfo_get_DomainName_Proxy (IADsWinNTSystemInfo * This, BSTR * retval);
void __RPC_STUB IADsWinNTSystemInfo_get_DomainName_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsWinNTSystemInfo_get_PDC_Proxy (IADsWinNTSystemInfo * This, BSTR * retval);
void __RPC_STUB IADsWinNTSystemInfo_get_PDC_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_WinNTSystemInfo;
#ifdef __cplusplus
class DECLSPEC_UUID ("66182EC4-AFD1-11d2-9CB9-0000F87A369E") WinNTSystemInfo;
#endif

#ifndef __IADsDNWithBinary_INTERFACE_DEFINED__
#define __IADsDNWithBinary_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsDNWithBinary;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("7e99c0a2-f935-11d2-ba96-00c04fb6d0d1") IADsDNWithBinary:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_BinaryValue (VARIANT * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_BinaryValue (VARIANT vBinaryValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DNString (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DNString (BSTR bstrDNString) = 0;
};
#else
typedef struct IADsDNWithBinaryVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsDNWithBinary * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsDNWithBinary * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsDNWithBinary * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsDNWithBinary * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsDNWithBinary * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsDNWithBinary * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsDNWithBinary * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_BinaryValue) (IADsDNWithBinary * This, VARIANT * retval);
    HRESULT (STDMETHODCALLTYPE * put_BinaryValue) (IADsDNWithBinary * This, VARIANT vBinaryValue);
    HRESULT (STDMETHODCALLTYPE * get_DNString) (IADsDNWithBinary * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_DNString) (IADsDNWithBinary * This, BSTR bstrDNString);
    END_INTERFACE
} IADsDNWithBinaryVtbl;
interface IADsDNWithBinary {
    CONST_VTBL struct IADsDNWithBinaryVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsDNWithBinary_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsDNWithBinary_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsDNWithBinary_Release(This)  (This)->lpVtbl->Release(This)
#define IADsDNWithBinary_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsDNWithBinary_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsDNWithBinary_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsDNWithBinary_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsDNWithBinary_get_BinaryValue(This,retval)  (This)->lpVtbl->get_BinaryValue(This,retval)
#define IADsDNWithBinary_put_BinaryValue(This,vBinaryValue)  (This)->lpVtbl->put_BinaryValue(This,vBinaryValue)
#define IADsDNWithBinary_get_DNString(This,retval)  (This)->lpVtbl->get_DNString(This,retval)
#define IADsDNWithBinary_put_DNString(This,bstrDNString)  (This)->lpVtbl->put_DNString(This,bstrDNString)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsDNWithBinary_get_BinaryValue_Proxy (IADsDNWithBinary * This, VARIANT * retval);
void __RPC_STUB IADsDNWithBinary_get_BinaryValue_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDNWithBinary_put_BinaryValue_Proxy (IADsDNWithBinary * This, VARIANT vBinaryValue);
void __RPC_STUB IADsDNWithBinary_put_BinaryValue_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDNWithBinary_get_DNString_Proxy (IADsDNWithBinary * This, BSTR * retval);
void __RPC_STUB IADsDNWithBinary_get_DNString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDNWithBinary_put_DNString_Proxy (IADsDNWithBinary * This, BSTR bstrDNString);
void __RPC_STUB IADsDNWithBinary_put_DNString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_DNWithBinary;
#ifdef __cplusplus
class DECLSPEC_UUID ("7e99c0a3-f935-11d2-ba96-00c04fb6d0d1") DNWithBinary;
#endif

#ifndef __IADsDNWithString_INTERFACE_DEFINED__
#define __IADsDNWithString_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsDNWithString;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("370df02e-f934-11d2-ba96-00c04fb6d0d1") IADsDNWithString:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE get_StringValue (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StringValue (BSTR bstrStringValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DNString (BSTR * retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DNString (BSTR bstrDNString) = 0;
};
#else
typedef struct IADsDNWithStringVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsDNWithString * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsDNWithString * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsDNWithString * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsDNWithString * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsDNWithString * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsDNWithString * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsDNWithString * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * get_StringValue) (IADsDNWithString * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_StringValue) (IADsDNWithString * This, BSTR bstrStringValue);
    HRESULT (STDMETHODCALLTYPE * get_DNString) (IADsDNWithString * This, BSTR * retval);
    HRESULT (STDMETHODCALLTYPE * put_DNString) (IADsDNWithString * This, BSTR bstrDNString);
    END_INTERFACE
} IADsDNWithStringVtbl;
interface IADsDNWithString {
    CONST_VTBL struct IADsDNWithStringVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsDNWithString_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsDNWithString_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsDNWithString_Release(This)  (This)->lpVtbl->Release(This)
#define IADsDNWithString_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsDNWithString_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsDNWithString_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsDNWithString_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsDNWithString_get_StringValue(This,retval)  (This)->lpVtbl->get_StringValue(This,retval)
#define IADsDNWithString_put_StringValue(This,bstrStringValue)  (This)->lpVtbl->put_StringValue(This,bstrStringValue)
#define IADsDNWithString_get_DNString(This,retval)  (This)->lpVtbl->get_DNString(This,retval)
#define IADsDNWithString_put_DNString(This,bstrDNString)  (This)->lpVtbl->put_DNString(This,bstrDNString)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsDNWithString_get_StringValue_Proxy (IADsDNWithString * This, BSTR * retval);
void __RPC_STUB IADsDNWithString_get_StringValue_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDNWithString_put_StringValue_Proxy (IADsDNWithString * This, BSTR bstrStringValue);
void __RPC_STUB IADsDNWithString_put_StringValue_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDNWithString_get_DNString_Proxy (IADsDNWithString * This, BSTR * retval);
void __RPC_STUB IADsDNWithString_get_DNString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsDNWithString_put_DNString_Proxy (IADsDNWithString * This, BSTR bstrDNString);
void __RPC_STUB IADsDNWithString_put_DNString_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_DNWithString;
#ifdef __cplusplus
class DECLSPEC_UUID ("334857cc-f934-11d2-ba96-00c04fb6d0d1") DNWithString;
#endif

#ifndef __IADsSecurityUtility_INTERFACE_DEFINED__
#define __IADsSecurityUtility_INTERFACE_DEFINED__
EXTERN_C const IID IID_IADsSecurityUtility;
#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE ("a63251b2-5f21-474b-ab52-4a8efad10895") IADsSecurityUtility:public IDispatch {
    public:
    virtual HRESULT STDMETHODCALLTYPE GetSecurityDescriptor (VARIANT varPath, long lPathFormat, long lFormat, VARIANT * pVariant) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSecurityDescriptor (VARIANT varPath, long lPathFormat, VARIANT varData, long lDataFormat) = 0;
    virtual HRESULT STDMETHODCALLTYPE ConvertSecurityDescriptor (VARIANT varSD, long lDataFormat, long lOutFormat, VARIANT * pResult) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SecurityMask (long *retval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SecurityMask (long lnSecurityMask) = 0;
};
#else
typedef struct IADsSecurityUtilityVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IADsSecurityUtility*, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE * AddRef) (IADsSecurityUtility * This);
    ULONG (STDMETHODCALLTYPE * Release) (IADsSecurityUtility * This);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfoCount) (IADsSecurityUtility*, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE * GetTypeInfo) (IADsSecurityUtility*, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE * GetIDsOfNames) (IADsSecurityUtility*, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE * Invoke) (IADsSecurityUtility*, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE * GetSecurityDescriptor) (IADsSecurityUtility*, VARIANT varPath, long lPathFormat, long lFormat, VARIANT * pVariant);
    HRESULT (STDMETHODCALLTYPE * SetSecurityDescriptor) (IADsSecurityUtility*, VARIANT varPath, long lPathFormat, VARIANT varData, long lDataFormat);
    HRESULT (STDMETHODCALLTYPE * ConvertSecurityDescriptor) (IADsSecurityUtility*, VARIANT varSD, long lDataFormat, long lOutFormat, VARIANT * pResult);
    HRESULT (STDMETHODCALLTYPE * get_SecurityMask) (IADsSecurityUtility*, long *retval);
    HRESULT (STDMETHODCALLTYPE * put_SecurityMask) (IADsSecurityUtility*, long lnSecurityMask);
    END_INTERFACE
} IADsSecurityUtilityVtbl;
interface IADsSecurityUtility {
    CONST_VTBL struct IADsSecurityUtilityVtbl *lpVtbl;
};
#ifdef COBJMACROS
#define IADsSecurityUtility_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IADsSecurityUtility_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IADsSecurityUtility_Release(This)  (This)->lpVtbl->Release(This)
#define IADsSecurityUtility_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IADsSecurityUtility_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IADsSecurityUtility_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IADsSecurityUtility_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IADsSecurityUtility_GetSecurityDescriptor(This,varPath,lPathFormat,lFormat,pVariant)  (This)->lpVtbl->GetSecurityDescriptor(This,varPath,lPathFormat,lFormat,pVariant)
#define IADsSecurityUtility_SetSecurityDescriptor(This,varPath,lPathFormat,varData,lDataFormat)  (This)->lpVtbl->SetSecurityDescriptor(This,varPath,lPathFormat,varData,lDataFormat)
#define IADsSecurityUtility_ConvertSecurityDescriptor(This,varSD,lDataFormat,lOutFormat,pResult)  (This)->lpVtbl->ConvertSecurityDescriptor(This,varSD,lDataFormat,lOutFormat,pResult)
#define IADsSecurityUtility_get_SecurityMask(This,retval)  (This)->lpVtbl->get_SecurityMask(This,retval)
#define IADsSecurityUtility_put_SecurityMask(This,lnSecurityMask)  (This)->lpVtbl->put_SecurityMask(This,lnSecurityMask)
#endif
#endif
HRESULT STDMETHODCALLTYPE IADsSecurityUtility_GetSecurityDescriptor_Proxy (IADsSecurityUtility*, VARIANT varPath, long lPathFormat, long lFormat, VARIANT * pVariant);
void __RPC_STUB IADsSecurityUtility_GetSecurityDescriptor_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityUtility_SetSecurityDescriptor_Proxy (IADsSecurityUtility*, VARIANT varPath, long lPathFormat, VARIANT varData, long lDataFormat);
void __RPC_STUB IADsSecurityUtility_SetSecurityDescriptor_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityUtility_ConvertSecurityDescriptor_Proxy (IADsSecurityUtility*, VARIANT varSD, long lDataFormat, long lOutFormat, VARIANT * pResult);
void __RPC_STUB IADsSecurityUtility_ConvertSecurityDescriptor_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityUtility_get_SecurityMask_Proxy (IADsSecurityUtility*, long *retval);
void __RPC_STUB IADsSecurityUtility_get_SecurityMask_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
HRESULT STDMETHODCALLTYPE IADsSecurityUtility_put_SecurityMask_Proxy (IADsSecurityUtility*, long lnSecurityMask);
void __RPC_STUB IADsSecurityUtility_put_SecurityMask_Stub (IRpcStubBuffer*,IRpcChannelBuffer*,PRPC_MESSAGE,DWORD*);
#endif
EXTERN_C const CLSID CLSID_ADsSecurityUtility;
#ifdef __cplusplus
class DECLSPEC_UUID ("f270c64a-ffb8-4ae4-85fe-3a75e5347966") ADsSecurityUtility;
#endif
#endif


#ifdef __cplusplus
}
#endif

#endif /* _IADS_H */
