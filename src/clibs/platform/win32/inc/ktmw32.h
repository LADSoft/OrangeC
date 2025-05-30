#ifndef _KTMW32_H
#define _KTMW32_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Kernel Transaction Manager definitions */

#ifdef __cplusplus
extern "C" {
#endif

#include <ktmtypes.h>

HANDLE APIENTRY CreateTransaction(LPSECURITY_ATTRIBUTES,LPGUID,DWORD,DWORD,DWORD,DWORD,LPWSTR);
HANDLE APIENTRY OpenTransaction(DWORD,LPGUID);
BOOL APIENTRY CommitTransaction(HANDLE);
BOOL APIENTRY CommitTransactionAsync(HANDLE);
BOOL APIENTRY RollbackTransaction(HANDLE);
BOOL APIENTRY RollbackTransactionAsync(HANDLE);
BOOL APIENTRY RollbackSavepoint(HANDLE,SAVEPOINT_ID);
BOOL APIENTRY SavepointTransaction(HANDLE,PSAVEPOINT_ID);
BOOL APIENTRY ClearSavepointTransaction(HANDLE,SAVEPOINT_ID);
BOOL APIENTRY ClearAllSavepointsTransaction(HANDLE);
BOOL APIENTRY GetTransactionId(HANDLE,LPGUID);
BOOL APIENTRY GetTransactionInformation(HANDLE,PDWORD,PDWORD,PDWORD,PDWORD,DWORD,LPWSTR);
BOOL APIENTRY SetTransactionInformation(HANDLE,DWORD,DWORD,DWORD,LPWSTR);
HANDLE APIENTRY CreateTransactionManager(LPSECURITY_ATTRIBUTES,LPWSTR,ULONG,ULONG);
HANDLE APIENTRY OpenTransactionManager(LPWSTR,ACCESS_MASK,ULONG);
HANDLE APIENTRY OpenTransactionManagerById(LPGUID,ACCESS_MASK,ULONG);
BOOL APIENTRY RollforwardTransactionManager(HANDLE,PLARGE_INTEGER);
BOOL APIENTRY RecoverTransactionManager(HANDLE);
BOOL GetCurrentClockTransactionManager(HANDLE,PLARGE_INTEGER);
BOOL GetTransactionManagerId(HANDLE,LPGUID);
BOOL IsLogWritableTransactionManager(HANDLE);
HANDLE APIENTRY CreateResourceManager(LPSECURITY_ATTRIBUTES,LPGUID,DWORD,HANDLE,LPWSTR);
HANDLE APIENTRY OpenResourceManager(DWORD,HANDLE,LPGUID);
BOOL APIENTRY RecoverResourceManager(HANDLE);
BOOL APIENTRY GetNotificationResourceManager(HANDLE,PTRANSACTION_NOTIFICATION,ULONG,DWORD,PULONG);
BOOL APIENTRY GetNotificationResourceManagerAsync(HANDLE,PTRANSACTION_NOTIFICATION,ULONG,PULONG,LPOVERLAPPED);
BOOL APIENTRY SetResourceManagerCompletionPort(HANDLE,HANDLE,ULONG_PTR);
HANDLE APIENTRY CreateEnlistment(LPSECURITY_ATTRIBUTES,HANDLE,HANDLE,NOTIFICATION_MASK,DWORD,PVOID);
HANDLE APIENTRY OpenEnlistment(DWORD,HANDLE,LPGUID);
BOOL APIENTRY RecoverEnlistment(HANDLE,PVOID);
BOOL APIENTRY GetEnlistmentRecoveryInformation(HANDLE,ULONG,PVOID,PULONG);
BOOL APIENTRY GetEnlistmentId(HANDLE,LPGUID);
BOOL APIENTRY SetEnlistmentRecoveryInformation(HANDLE,ULONG,PVOID);
BOOL APIENTRY PrepareEnlistment(HANDLE,PLARGE_INTEGER);
BOOL APIENTRY PrePrepareEnlistment(HANDLE,PLARGE_INTEGER);
BOOL APIENTRY CommitEnlistment(HANDLE,PLARGE_INTEGER);
BOOL APIENTRY RollbackEnlistment(HANDLE,PLARGE_INTEGER);
BOOL APIENTRY PrePrepareComplete(HANDLE,PLARGE_INTEGER);
BOOL APIENTRY PrepareComplete(HANDLE,PLARGE_INTEGER);
BOOL APIENTRY ReadOnlyEnlistment(HANDLE,PLARGE_INTEGER);
BOOL APIENTRY CommitComplete(HANDLE,PLARGE_INTEGER);
BOOL APIENTRY RollbackComplete(HANDLE,PLARGE_INTEGER);
BOOL APIENTRY SavepointComplete(HANDLE,PLARGE_INTEGER);
BOOL APIENTRY SinglePhaseReject(HANDLE,PLARGE_INTEGER);
BOOL APIENTRY RegisterProtocolAddressInformation(HANDLE,PCRM_PROTOCOL_ID,ULONG,PVOID,DWORD);
HANDLE APIENTRY PullTransaction(ACCESS_MASK,ULONG,PCRM_PROTOCOL_ID,ULONG,PVOID);
BOOL APIENTRY MarshalTransaction(HANDLE,ULONG,PCRM_PROTOCOL_ID,ULONG,PVOID,PULONG);
BOOL APIENTRY MarshalComplete(HANDLE,ULONG,ULONG,PVOID);
BOOL APIENTRY PropagationComplete(HANDLE,ULONG,ULONG,PVOID);
BOOL APIENTRY PropagationFailed(HANDLE,ULONG);

#ifdef __cplusplus
}
#endif

#endif /* _KTMW32_H */
