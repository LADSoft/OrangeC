#ifndef _QOSNAME_H
#define _QOSNAME_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Winsock Quality of Service name management definitions */

#define QT_1  "G711"
#define QT_2  "G723.1"
#define QT_3  "G729"
#define QT_4  "H263QCIF"
#define QT_5  "H263CIF"
#define QT_6  "H261QCIF"
#define QT_7  "H261CIF"
#define QT_8  "GSM6.10"

#define WSCINSTALL_QOS_TEMPLATE  "WSCInstallQOSTemplate"
#define WSCREMOVE_QOS_TEMPLATE  "WSCRemoveQOSTemplate"
#define WPUGET_QOS_TEMPLATE  "WPUGetQOSTemplate"

typedef BOOL (APIENTRY *WSC_INSTALL_QOS_TEMPLATE)(const LPGUID,LPWSABUF,LPQOS);
typedef BOOL (APIENTRY *WSC_REMOVE_QOS_TEMPLATE)(const LPGUID,LPWSABUF);
typedef BOOL (APIENTRY *WPU_GET_QOS_TEMPLATE)(const LPGUID,LPWSABUF,LPQOS);

#ifdef __PELLES_C_IMPDEF__
BOOL WINAPI WSCInstallQOSTemplate(const LPGUID,LPWSABUF,LPQOS);
BOOL WINAPI WSCRemoveQOSTemplate(const LPGUID,LPWSABUF);
INT WINAPI WPUGetQOSTemplate(const LPGUID,LPWSABUF,LPQOS);
#endif

#endif /* _QOSNAME_H */
