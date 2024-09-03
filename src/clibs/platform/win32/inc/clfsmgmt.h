#ifndef _CLFSMGMT_H
#define _CLFSMGMT_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
typedef enum _CLFS_MGMT_POLICY_TYPE {
    ClfsMgmtPolicyMaximumSize = 0x0,
    ClfsMgmtPolicyMinimumSize,
    ClfsMgmtPolicyNewContainerSize,
    ClfsMgmtPolicyGrowthRate,
    ClfsMgmtPolicyLogTail,
    ClfsMgmtPolicyAutoShrink,
    ClfsMgmtPolicyAutoGrow,
    ClfsMgmtPolicyNewContainerPrefix,
    ClfsMgmtPolicyNewContainerSuffix,
    ClfsMgmtPolicyNewContainerExtension,
    ClfsMgmtPolicyInvalid
} CLFS_MGMT_POLICY_TYPE, *PCLFS_MGMT_POLICY_TYPE;
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
#define CLFS_MGMT_NUM_POLICIES ((ULONG)ClfsMgmtPolicyInvalid)
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
#define CLFS_LOG_SIZE_MINIMUM ((ULONGLONG)(0))
#define CLFS_LOG_SIZE_MAXIMUM ((ULONGLONG)(-1))
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
#define CLFS_MGMT_POLICY_VERSION (0x01)
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
#define LOG_POLICY_OVERWRITE   (0x01)
#define LOG_POLICY_PERSIST     (0x02)
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
typedef struct _CLFS_MGMT_POLICY {
    ULONG Version;
    ULONG LengthInBytes;
    ULONG PolicyFlags;
    CLFS_MGMT_POLICY_TYPE PolicyType;
    union {
        struct {
            ULONG Containers;
        } MaximumSize;
        struct {
            ULONG Containers;
        } MinimumSize;
        struct {
            ULONG SizeInBytes;
        } NewContainerSize;
        struct {
            ULONG AbsoluteGrowthInContainers;
            ULONG RelativeGrowthPercentage;
        } GrowthRate;
        struct {
            ULONG MinimumAvailablePercentage;
            ULONG MinimumAvailableContainers;
        } LogTail;
        struct {
            ULONG Percentage;
        } AutoShrink;
        struct {
            ULONG Enabled;
        } AutoGrow;
        struct {
            USHORT PrefixLengthInBytes;
            WCHAR PrefixString[1];
        } NewContainerPrefix;
        struct {
            ULONGLONG NextContainerSuffix;
        } NewContainerSuffix;
        struct {
            USHORT ExtensionLengthInBytes;
            WCHAR ExtensionString[1];
        } NewContainerExtension;
    } PolicyParameters;
} CLFS_MGMT_POLICY, *PCLFS_MGMT_POLICY;
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
typedef enum _CLFS_MGMT_NOTIFICATION_TYPE {
    ClfsMgmtAdvanceTailNotification = 0,
    ClfsMgmtLogFullHandlerNotification,
    ClfsMgmtLogUnpinnedNotification,
    ClfsMgmtLogWriteNotification
} CLFS_MGMT_NOTIFICATION_TYPE, *PCLFS_MGMT_NOTIFICATION_TYPE;
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
typedef struct _CLFS_MGMT_NOTIFICATION {
    CLFS_MGMT_NOTIFICATION_TYPE Notification;
    CLFS_LSN Lsn;
    USHORT LogIsPinned;
} CLFS_MGMT_NOTIFICATION, *PCLFS_MGMT_NOTIFICATION;
#endif /* NTDDI_VERSION || _WIN32_WINNT */


#ifdef CLFS_KERNEL_MODE

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
typedef NTSTATUS (*PCLFS_CLIENT_ADVANCE_TAIL_CALLBACK)(PLOG_FILE_OBJECT, PCLFS_LSN, PVOID);
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
typedef VOID (*PCLFS_CLIENT_LFF_HANDLER_COMPLETE_CALLBACK)(PLOG_FILE_OBJECT, NTSTATUS, BOOLEAN, PVOID);
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
typedef VOID (*PCLFS_CLIENT_LOG_UNPINNED_CALLBACK)(PLOG_FILE_OBJECT, PVOID);
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
typedef VOID (*PCLFS_SET_LOG_SIZE_COMPLETE_CALLBACK)(PLOG_FILE_OBJECT, NTSTATUS, PVOID);
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
#define CLFS_MGMT_CLIENT_REGISTRATION_VERSION  (0x1)

typedef struct _CLFS_MGMT_CLIENT_REGISTRATION {
    ULONG Version;
    PCLFS_CLIENT_ADVANCE_TAIL_CALLBACK AdvanceTailCallback;
    PVOID AdvanceTailCallbackData;
    PCLFS_CLIENT_LFF_HANDLER_COMPLETE_CALLBACK LogGrowthCompleteCallback;
    PVOID LogGrowthCompleteCallbackData;
    PCLFS_CLIENT_LOG_UNPINNED_CALLBACK LogUnpinnedCallback;
    PVOID LogUnpinnedCallbackData;
} CLFS_MGMT_CLIENT_REGISTRATION, *PCLFS_MGMT_CLIENT_REGISTRATION;
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
typedef PVOID CLFS_MGMT_CLIENT, *PCLFS_MGMT_CLIENT;
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
NTSTATUS ClfsMgmtRegisterManagedClient(PLOG_FILE_OBJECT, PCLFS_MGMT_CLIENT_REGISTRATION, PCLFS_MGMT_CLIENT);
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
NTSTATUS ClfsMgmtDeregisterManagedClient(CLFS_MGMT_CLIENT);
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
NTSTATUS ClfsMgmtTailAdvanceFailure(CLFS_MGMT_CLIENT, NTSTATUS);
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
NTSTATUS ClfsMgmtHandleLogFileFull(CLFS_MGMT_CLIENT);
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
NTSTATUS ClfsMgmtInstallPolicy(PLOG_FILE_OBJECT, PCLFS_MGMT_POLICY, ULONG);
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
NTSTATUS ClfsMgmtQueryPolicy(PLOG_FILE_OBJECT, CLFS_MGMT_POLICY_TYPE, PCLFS_MGMT_POLICY, PULONG);
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
NTSTATUS ClfsMgmtRemovePolicy(PLOG_FILE_OBJECT, CLFS_MGMT_POLICY_TYPE);
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
NTSTATUS ClfsMgmtSetLogFileSize(PLOG_FILE_OBJECT, PULONGLONG, PULONGLONG, PCLFS_SET_LOG_SIZE_COMPLETE_CALLBACK, PVOID);
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#if (NTDDI_VERSION >= NTDDI_VISTA) || (_WIN32_WINNT >= _WIN32_WINNT_LONGHORN)
NTSTATUS ClfsMgmtSetLogFileSizeAsClient(PLOG_FILE_OBJECT, PCLFS_MGMT_CLIENT, PULONGLONG, PULONGLONG, PCLFS_SET_LOG_SIZE_COMPLETE_CALLBACK, PVOID);
#endif /* NTDDI_VERSION || _WIN32_WINNT */

#endif /* CLFS_KERNEL_MODE */

#ifdef __cplusplus
}
#endif

#endif /* _CLFSMGMT_H */
