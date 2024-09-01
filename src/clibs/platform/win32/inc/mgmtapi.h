#ifndef _MGMTAPI_H
#define _MGMTAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* SNMP Management API definitions */

#include <snmp.h>
#include <winsock.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SNMP_MGMTAPI_TIMEOUT  40
#define SNMP_MGMTAPI_SELECT_FDERRORS  41
#define SNMP_MGMTAPI_TRAP_ERRORS  42
#define SNMP_MGMTAPI_TRAP_DUPINIT  43
#define SNMP_MGMTAPI_NOTRAPS  44
#define SNMP_MGMTAPI_AGAIN  45
#define SNMP_MGMTAPI_INVALID_CTL  46
#define SNMP_MGMTAPI_INVALID_SESSION  47
#define SNMP_MGMTAPI_INVALID_BUFFER  48

#define MGMCTL_SETAGENTPORT  0x01

typedef PVOID LPSNMP_MGR_SESSION;

LPSNMP_MGR_SESSION SNMP_FUNC_TYPE SnmpMgrOpen(LPSTR,LPSTR,INT,INT);
BOOL SNMP_FUNC_TYPE SnmpMgrCtl(LPSNMP_MGR_SESSION,DWORD,LPVOID,DWORD,LPVOID,DWORD,LPDWORD);
BOOL SNMP_FUNC_TYPE SnmpMgrClose(LPSNMP_MGR_SESSION);
SNMPAPI SNMP_FUNC_TYPE SnmpMgrRequest(LPSNMP_MGR_SESSION,BYTE,RFC1157VarBindList*,AsnInteger*,AsnInteger*);
BOOL SNMP_FUNC_TYPE SnmpMgrStrToOid(LPSTR,AsnObjectIdentifier*);
BOOL SNMP_FUNC_TYPE SnmpMgrOidToStr(AsnObjectIdentifier*,LPSTR*);
BOOL SNMP_FUNC_TYPE SnmpMgrTrapListen(HANDLE*);
BOOL SNMP_FUNC_TYPE SnmpMgrGetTrap(AsnObjectIdentifier*,AsnNetworkAddress*,AsnInteger*,AsnInteger*,AsnTimeticks*,RFC1157VarBindList*);
BOOL SNMP_FUNC_TYPE SnmpMgrGetTrapEx(AsnObjectIdentifier*,AsnNetworkAddress*,AsnNetworkAddress*,AsnInteger*,AsnInteger*,AsnOctetString*,AsnTimeticks*,RFC1157VarBindList*);

#ifdef __cplusplus
}
#endif

#endif /* _MGMTAPI_H */
