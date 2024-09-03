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

#ifndef _TASKSCHD_H
#define _TASKSCHD_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __ITaskFolderCollection_FWD_DEFINED__
#define __ITaskFolderCollection_FWD_DEFINED__
typedef interface ITaskFolderCollection ITaskFolderCollection;
#endif

#ifndef __ITaskService_FWD_DEFINED__
#define __ITaskService_FWD_DEFINED__
typedef interface ITaskService ITaskService;
#endif

#ifndef __ITaskHandler_FWD_DEFINED__
#define __ITaskHandler_FWD_DEFINED__
typedef interface ITaskHandler ITaskHandler;
#endif

#ifndef __ITaskHandlerStatus_FWD_DEFINED__
#define __ITaskHandlerStatus_FWD_DEFINED__
typedef interface ITaskHandlerStatus ITaskHandlerStatus;
#endif

#ifndef __ITaskVariables_FWD_DEFINED__
#define __ITaskVariables_FWD_DEFINED__
typedef interface ITaskVariables ITaskVariables;
#endif

#ifndef __ITaskNamedValuePair_FWD_DEFINED__
#define __ITaskNamedValuePair_FWD_DEFINED__
typedef interface ITaskNamedValuePair ITaskNamedValuePair;
#endif

#ifndef __ITaskNamedValueCollection_FWD_DEFINED__
#define __ITaskNamedValueCollection_FWD_DEFINED__
typedef interface ITaskNamedValueCollection ITaskNamedValueCollection;
#endif

#ifndef __IRunningTask_FWD_DEFINED__
#define __IRunningTask_FWD_DEFINED__
typedef interface IRunningTask IRunningTask;
#endif

#ifndef __IRunningTaskCollection_FWD_DEFINED__
#define __IRunningTaskCollection_FWD_DEFINED__
typedef interface IRunningTaskCollection IRunningTaskCollection;
#endif

#ifndef __IRegisteredTask_FWD_DEFINED__
#define __IRegisteredTask_FWD_DEFINED__
typedef interface IRegisteredTask IRegisteredTask;
#endif

#ifndef __ITrigger_FWD_DEFINED__
#define __ITrigger_FWD_DEFINED__
typedef interface ITrigger ITrigger;
#endif

#ifndef __IIdleTrigger_FWD_DEFINED__
#define __IIdleTrigger_FWD_DEFINED__
typedef interface IIdleTrigger IIdleTrigger;
#endif

#ifndef __ILogonTrigger_FWD_DEFINED__
#define __ILogonTrigger_FWD_DEFINED__
typedef interface ILogonTrigger ILogonTrigger;
#endif

#ifndef __ISessionStateChangeTrigger_FWD_DEFINED__
#define __ISessionStateChangeTrigger_FWD_DEFINED__
typedef interface ISessionStateChangeTrigger ISessionStateChangeTrigger;
#endif

#ifndef __IEventTrigger_FWD_DEFINED__
#define __IEventTrigger_FWD_DEFINED__
typedef interface IEventTrigger IEventTrigger;
#endif

#ifndef __ITimeTrigger_FWD_DEFINED__
#define __ITimeTrigger_FWD_DEFINED__
typedef interface ITimeTrigger ITimeTrigger;
#endif

#ifndef __IDailyTrigger_FWD_DEFINED__
#define __IDailyTrigger_FWD_DEFINED__
typedef interface IDailyTrigger IDailyTrigger;
#endif

#ifndef __IWeeklyTrigger_FWD_DEFINED__
#define __IWeeklyTrigger_FWD_DEFINED__
typedef interface IWeeklyTrigger IWeeklyTrigger;
#endif

#ifndef __IMonthlyTrigger_FWD_DEFINED__
#define __IMonthlyTrigger_FWD_DEFINED__
typedef interface IMonthlyTrigger IMonthlyTrigger;
#endif

#ifndef __IMonthlyDOWTrigger_FWD_DEFINED__
#define __IMonthlyDOWTrigger_FWD_DEFINED__
typedef interface IMonthlyDOWTrigger IMonthlyDOWTrigger;
#endif

#ifndef __IBootTrigger_FWD_DEFINED__
#define __IBootTrigger_FWD_DEFINED__
typedef interface IBootTrigger IBootTrigger;
#endif

#ifndef __IRegistrationTrigger_FWD_DEFINED__
#define __IRegistrationTrigger_FWD_DEFINED__
typedef interface IRegistrationTrigger IRegistrationTrigger;
#endif

#ifndef __IAction_FWD_DEFINED__
#define __IAction_FWD_DEFINED__
typedef interface IAction IAction;
#endif

#ifndef __IExecAction_FWD_DEFINED__
#define __IExecAction_FWD_DEFINED__
typedef interface IExecAction IExecAction;
#endif

#ifndef __IExecAction2_FWD_DEFINED__
#define __IExecAction2_FWD_DEFINED__
typedef interface IExecAction2 IExecAction2;
#endif

#ifndef __IShowMessageAction_FWD_DEFINED__
#define __IShowMessageAction_FWD_DEFINED__
typedef interface IShowMessageAction IShowMessageAction;
#endif

#ifndef __IComHandlerAction_FWD_DEFINED__
#define __IComHandlerAction_FWD_DEFINED__
typedef interface IComHandlerAction IComHandlerAction;
#endif

#ifndef __IEmailAction_FWD_DEFINED__
#define __IEmailAction_FWD_DEFINED__
typedef interface IEmailAction IEmailAction;
#endif

#ifndef __ITriggerCollection_FWD_DEFINED__
#define __ITriggerCollection_FWD_DEFINED__
typedef interface ITriggerCollection ITriggerCollection;
#endif

#ifndef __IActionCollection_FWD_DEFINED__
#define __IActionCollection_FWD_DEFINED__
typedef interface IActionCollection IActionCollection;
#endif

#ifndef __IPrincipal_FWD_DEFINED__
#define __IPrincipal_FWD_DEFINED__
typedef interface IPrincipal IPrincipal;
#endif

#ifndef __IPrincipal2_FWD_DEFINED__
#define __IPrincipal2_FWD_DEFINED__
typedef interface IPrincipal2 IPrincipal2;
#endif

#ifndef __IRegistrationInfo_FWD_DEFINED__
#define __IRegistrationInfo_FWD_DEFINED__
typedef interface IRegistrationInfo IRegistrationInfo;
#endif

#ifndef __ITaskDefinition_FWD_DEFINED__
#define __ITaskDefinition_FWD_DEFINED__
typedef interface ITaskDefinition ITaskDefinition;
#endif

#ifndef __ITaskSettings_FWD_DEFINED__
#define __ITaskSettings_FWD_DEFINED__
typedef interface ITaskSettings ITaskSettings;
#endif

#ifndef __ITaskSettings2_FWD_DEFINED__
#define __ITaskSettings2_FWD_DEFINED__
typedef interface ITaskSettings2 ITaskSettings2;
#endif

#ifndef __ITaskSettings3_FWD_DEFINED__
#define __ITaskSettings3_FWD_DEFINED__
typedef interface ITaskSettings3 ITaskSettings3;
#endif

#ifndef __IMaintenanceSettings_FWD_DEFINED__
#define __IMaintenanceSettings_FWD_DEFINED__
typedef interface IMaintenanceSettings IMaintenanceSettings;
#endif

#ifndef __TaskScheduler_FWD_DEFINED__
#define __TaskScheduler_FWD_DEFINED__

#ifdef __cplusplus
typedef class TaskScheduler TaskScheduler;
#else
typedef struct TaskScheduler TaskScheduler;
#endif

#endif /* __TaskScheduler_FWD_DEFINED__ */

#ifndef __TaskHandlerPS_FWD_DEFINED__
#define __TaskHandlerPS_FWD_DEFINED__

#ifdef __cplusplus
typedef class TaskHandlerPS TaskHandlerPS;
#else
typedef struct TaskHandlerPS TaskHandlerPS;
#endif

#endif /* __TaskHandlerPS_FWD_DEFINED__ */

#ifndef __TaskHandlerStatusPS_FWD_DEFINED__
#define __TaskHandlerStatusPS_FWD_DEFINED__

#ifdef __cplusplus
typedef class TaskHandlerStatusPS TaskHandlerStatusPS;
#else
typedef struct TaskHandlerStatusPS TaskHandlerStatusPS;
#endif

#endif /* __TaskHandlerStatusPS_FWD_DEFINED__ */

#ifndef __ITaskNamedValuePair_FWD_DEFINED__
#define __ITaskNamedValuePair_FWD_DEFINED__
typedef interface ITaskNamedValuePair ITaskNamedValuePair;
#endif

#ifndef __ITaskNamedValueCollection_FWD_DEFINED__
#define __ITaskNamedValueCollection_FWD_DEFINED__
typedef interface ITaskNamedValueCollection ITaskNamedValueCollection;
#endif

#ifndef __IRunningTask_FWD_DEFINED__
#define __IRunningTask_FWD_DEFINED__
typedef interface IRunningTask IRunningTask;

#endif

#ifndef __IRunningTaskCollection_FWD_DEFINED__
#define __IRunningTaskCollection_FWD_DEFINED__
typedef interface IRunningTaskCollection IRunningTaskCollection;
#endif

#ifndef __IRegisteredTask_FWD_DEFINED__
#define __IRegisteredTask_FWD_DEFINED__
typedef interface IRegisteredTask IRegisteredTask;
#endif

#ifndef __IRegisteredTaskCollection_FWD_DEFINED__
#define __IRegisteredTaskCollection_FWD_DEFINED__
typedef interface IRegisteredTaskCollection IRegisteredTaskCollection;
#endif

#ifndef __ITaskFolder_FWD_DEFINED__
#define __ITaskFolder_FWD_DEFINED__
typedef interface ITaskFolder ITaskFolder;
#endif

#ifndef __ITaskFolderCollection_FWD_DEFINED__
#define __ITaskFolderCollection_FWD_DEFINED__
typedef interface ITaskFolderCollection ITaskFolderCollection;
#endif

#ifndef __ITaskService_FWD_DEFINED__
#define __ITaskService_FWD_DEFINED__
typedef interface ITaskService ITaskService;
#endif

#ifndef __IIdleSettings_FWD_DEFINED__
#define __IIdleSettings_FWD_DEFINED__
typedef interface IIdleSettings IIdleSettings;
#endif

#ifndef __INetworkSettings_FWD_DEFINED__
#define __INetworkSettings_FWD_DEFINED__
typedef interface INetworkSettings INetworkSettings;
#endif

#ifndef __IRepetitionPattern_FWD_DEFINED__
#define __IRepetitionPattern_FWD_DEFINED__
typedef interface IRepetitionPattern IRepetitionPattern;
#endif

#ifndef __ITrigger_FWD_DEFINED__
#define __ITrigger_FWD_DEFINED__
typedef interface ITrigger ITrigger;
#endif

#ifndef __IEventTrigger_FWD_DEFINED__
#define __IEventTrigger_FWD_DEFINED__
typedef interface IEventTrigger IEventTrigger;
#endif

#ifndef __ITimeTrigger_FWD_DEFINED__
#define __ITimeTrigger_FWD_DEFINED__
typedef interface ITimeTrigger ITimeTrigger;
#endif

#ifndef __IDailyTrigger_FWD_DEFINED__
#define __IDailyTrigger_FWD_DEFINED__
typedef interface IDailyTrigger IDailyTrigger;
#endif

#ifndef __IWeeklyTrigger_FWD_DEFINED__
#define __IWeeklyTrigger_FWD_DEFINED__
typedef interface IWeeklyTrigger IWeeklyTrigger;
#endif

#ifndef __IMonthlyTrigger_FWD_DEFINED__
#define __IMonthlyTrigger_FWD_DEFINED__
typedef interface IMonthlyTrigger IMonthlyTrigger;
#endif

#ifndef __IMonthlyDOWTrigger_FWD_DEFINED__
#define __IMonthlyDOWTrigger_FWD_DEFINED__
typedef interface IMonthlyDOWTrigger IMonthlyDOWTrigger;
#endif

#ifndef __IIdleTrigger_FWD_DEFINED__
#define __IIdleTrigger_FWD_DEFINED__
typedef interface IIdleTrigger IIdleTrigger;
#endif

#ifndef __ILogonTrigger_FWD_DEFINED__
#define __ILogonTrigger_FWD_DEFINED__
typedef interface ILogonTrigger ILogonTrigger;
#endif

#ifndef __ISessionStateChangeTrigger_FWD_DEFINED__
#define __ISessionStateChangeTrigger_FWD_DEFINED__
typedef interface ISessionStateChangeTrigger ISessionStateChangeTrigger;
#endif

#ifndef __IBootTrigger_FWD_DEFINED__
#define __IBootTrigger_FWD_DEFINED__
typedef interface IBootTrigger IBootTrigger;
#endif

#ifndef __IRegistrationTrigger_FWD_DEFINED__
#define __IRegistrationTrigger_FWD_DEFINED__
typedef interface IRegistrationTrigger IRegistrationTrigger;
#endif

#ifndef __IAction_FWD_DEFINED__
#define __IAction_FWD_DEFINED__
typedef interface IAction IAction;
#endif

#ifndef __IExecAction_FWD_DEFINED__
#define __IExecAction_FWD_DEFINED__
typedef interface IExecAction IExecAction;
#endif

#ifndef __IExecAction2_FWD_DEFINED__
#define __IExecAction2_FWD_DEFINED__
typedef interface IExecAction2 IExecAction2;
#endif

#ifndef __IShowMessageAction_FWD_DEFINED__
#define __IShowMessageAction_FWD_DEFINED__
typedef interface IShowMessageAction IShowMessageAction;
#endif

#ifndef __IComHandlerAction_FWD_DEFINED__
#define __IComHandlerAction_FWD_DEFINED__
typedef interface IComHandlerAction IComHandlerAction;
#endif

#ifndef __IEmailAction_FWD_DEFINED__
#define __IEmailAction_FWD_DEFINED__
typedef interface IEmailAction IEmailAction;
#endif

#ifndef __ITriggerCollection_FWD_DEFINED__
#define __ITriggerCollection_FWD_DEFINED__
typedef interface ITriggerCollection ITriggerCollection;
#endif

#ifndef __IActionCollection_FWD_DEFINED__
#define __IActionCollection_FWD_DEFINED__
typedef interface IActionCollection IActionCollection;
#endif

#ifndef __IPrincipal_FWD_DEFINED__
#define __IPrincipal_FWD_DEFINED__
typedef interface IPrincipal IPrincipal;
#endif

#ifndef __IPrincipal2_FWD_DEFINED__
#define __IPrincipal2_FWD_DEFINED__
typedef interface IPrincipal2 IPrincipal2;
#endif

#ifndef __IRegistrationInfo_FWD_DEFINED__
#define __IRegistrationInfo_FWD_DEFINED__
typedef interface IRegistrationInfo IRegistrationInfo;
#endif

#ifndef __ITaskSettings_FWD_DEFINED__
#define __ITaskSettings_FWD_DEFINED__
typedef interface ITaskSettings ITaskSettings;
#endif

#ifndef __ITaskSettings2_FWD_DEFINED__
#define __ITaskSettings2_FWD_DEFINED__
typedef interface ITaskSettings2 ITaskSettings2;
#endif

#ifndef __ITaskSettings3_FWD_DEFINED__
#define __ITaskSettings3_FWD_DEFINED__
typedef interface ITaskSettings3 ITaskSettings3;
#endif

#ifndef __IMaintenanceSettings_FWD_DEFINED__
#define __IMaintenanceSettings_FWD_DEFINED__
typedef interface IMaintenanceSettings IMaintenanceSettings;
#endif

#ifndef __ITaskDefinition_FWD_DEFINED__
#define __ITaskDefinition_FWD_DEFINED__
typedef interface ITaskDefinition ITaskDefinition;
#endif

#ifndef __ITaskVariables_FWD_DEFINED__
#define __ITaskVariables_FWD_DEFINED__
typedef interface ITaskVariables ITaskVariables;
#endif

#ifndef __ITaskHandlerStatus_FWD_DEFINED__
#define __ITaskHandlerStatus_FWD_DEFINED__
typedef interface ITaskHandlerStatus ITaskHandlerStatus;
#endif

#ifndef __ITaskHandler_FWD_DEFINED__
#define __ITaskHandler_FWD_DEFINED__
typedef interface ITaskHandler ITaskHandler;
#endif

#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif

extern RPC_IF_HANDLE __MIDL_itf_taskschd_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_taskschd_0000_0000_v0_0_s_ifspec;

#ifndef __TaskScheduler_LIBRARY_DEFINED__
#define __TaskScheduler_LIBRARY_DEFINED__

typedef enum _TASK_RUN_FLAGS {
    TASK_RUN_NO_FLAGS = 0,
    TASK_RUN_AS_SELF = 0x1,
    TASK_RUN_IGNORE_CONSTRAINTS = 0x2,
    TASK_RUN_USE_SESSION_ID = 0x4,
    TASK_RUN_USER_SID = 0x8
} TASK_RUN_FLAGS;

typedef enum _TASK_ENUM_FLAGS {
    TASK_ENUM_HIDDEN = 0x1
} TASK_ENUM_FLAGS;

typedef enum _TASK_LOGON_TYPE {
    TASK_LOGON_NONE = 0,
    TASK_LOGON_PASSWORD = 1,
    TASK_LOGON_S4U = 2,
    TASK_LOGON_INTERACTIVE_TOKEN = 3,
    TASK_LOGON_GROUP = 4,
    TASK_LOGON_SERVICE_ACCOUNT = 5,
    TASK_LOGON_INTERACTIVE_TOKEN_OR_PASSWORD = 6
} TASK_LOGON_TYPE;

typedef enum _TASK_RUNLEVEL {
    TASK_RUNLEVEL_LUA = 0,
    TASK_RUNLEVEL_HIGHEST = 1
} TASK_RUNLEVEL_TYPE;

typedef enum _TASK_PROCESSTOKENSID {
    TASK_PROCESSTOKENSID_NONE = 0,
    TASK_PROCESSTOKENSID_UNRESTRICTED = 1,
    TASK_PROCESSTOKENSID_DEFAULT = 2
} TASK_PROCESSTOKENSID_TYPE;

typedef enum _TASK_STATE {
    TASK_STATE_UNKNOWN = 0,
    TASK_STATE_DISABLED = 1,
    TASK_STATE_QUEUED = 2,
    TASK_STATE_READY = 3,
    TASK_STATE_RUNNING = 4
} TASK_STATE;

typedef enum _TASK_CREATION {
    TASK_VALIDATE_ONLY = 0x1,
    TASK_CREATE = 0x2,
    TASK_UPDATE = 0x4,
    TASK_CREATE_OR_UPDATE = (TASK_CREATE | TASK_UPDATE),
    TASK_DISABLE = 0x8,
    TASK_DONT_ADD_PRINCIPAL_ACE = 0x10,
    TASK_IGNORE_REGISTRATION_TRIGGERS = 0x20
} TASK_CREATION;

typedef enum _TASK_TRIGGER_TYPE2 {
    TASK_TRIGGER_EVENT = 0,
    TASK_TRIGGER_TIME = 1,
    TASK_TRIGGER_DAILY = 2,
    TASK_TRIGGER_WEEKLY = 3,
    TASK_TRIGGER_MONTHLY = 4,
    TASK_TRIGGER_MONTHLYDOW = 5,
    TASK_TRIGGER_IDLE = 6,
    TASK_TRIGGER_REGISTRATION = 7,
    TASK_TRIGGER_BOOT = 8,
    TASK_TRIGGER_LOGON = 9,
    TASK_TRIGGER_SESSION_STATE_CHANGE = 11,
    TASK_TRIGGER_CUSTOM_TRIGGER_01 = 12
} TASK_TRIGGER_TYPE2;

typedef enum _TASK_SESSION_STATE_CHANGE_TYPE {
    TASK_CONSOLE_CONNECT = 1,
    TASK_CONSOLE_DISCONNECT = 2,
    TASK_REMOTE_CONNECT = 3,
    TASK_REMOTE_DISCONNECT = 4,
    TASK_SESSION_LOCK = 7,
    TASK_SESSION_UNLOCK = 8
} TASK_SESSION_STATE_CHANGE_TYPE;

typedef enum _TASK_ACTION_TYPE {
    TASK_ACTION_EXEC = 0,
    TASK_ACTION_COM_HANDLER = 5,
    TASK_ACTION_SEND_EMAIL = 6,
    TASK_ACTION_SHOW_MESSAGE = 7
} TASK_ACTION_TYPE;

typedef enum _TASK_INSTANCES_POLICY {
    TASK_INSTANCES_PARALLEL = 0,
    TASK_INSTANCES_QUEUE = 1,
    TASK_INSTANCES_IGNORE_NEW = 2,
    TASK_INSTANCES_STOP_EXISTING = 3
} TASK_INSTANCES_POLICY;

typedef enum _TASK_COMPATIBILITY {
    TASK_COMPATIBILITY_AT = 0,
    TASK_COMPATIBILITY_V1 = 1,
    TASK_COMPATIBILITY_V2 = 2,
    TASK_COMPATIBILITY_V2_1 = 3,
    TASK_COMPATIBILITY_V2_2 = 4,
    TASK_COMPATIBILITY_V2_3 = 5,
    TASK_COMPATIBILITY_V2_4 = 6
} TASK_COMPATIBILITY;

EXTERN_C const IID LIBID_TaskScheduler;

#ifndef __ITaskFolderCollection_INTERFACE_DEFINED__
#define __ITaskFolderCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITaskFolderCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("79184a66-8664-423f-97f1-637356a5d812")ITaskFolderCollection:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Item(VARIANT index, ITaskFolder **ppFolder) = 0;
    virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown **ppEnum) = 0;
};
#else
typedef struct ITaskFolderCollectionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (ITaskFolderCollection * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(ITaskFolderCollection * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITaskFolderCollection * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ITaskFolderCollection * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ITaskFolderCollection * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ITaskFolderCollection * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ITaskFolderCollection * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Count)(ITaskFolderCollection * This, LONG * pCount);
    HRESULT (STDMETHODCALLTYPE *get_Item)(ITaskFolderCollection * This, VARIANT index, ITaskFolder ** ppFolder);
    HRESULT (STDMETHODCALLTYPE *get__NewEnum)(ITaskFolderCollection * This, IUnknown ** ppEnum);
    END_INTERFACE
} ITaskFolderCollectionVtbl;

interface ITaskFolderCollection {
    CONST_VTBL struct ITaskFolderCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITaskFolderCollection_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITaskFolderCollection_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITaskFolderCollection_Release(This)  ((This)->lpVtbl->Release(This))
#define ITaskFolderCollection_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ITaskFolderCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ITaskFolderCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ITaskFolderCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ITaskFolderCollection_get_Count(This,pCount)  ((This)->lpVtbl->get_Count(This,pCount))
#define ITaskFolderCollection_get_Item(This,index,ppFolder)  ((This)->lpVtbl->get_Item(This,index,ppFolder))
#define ITaskFolderCollection_get__NewEnum(This,ppEnum)  ((This)->lpVtbl->get__NewEnum(This,ppEnum))
#endif

#endif

#endif

#ifndef __ITaskService_INTERFACE_DEFINED__
#define __ITaskService_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITaskService;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("2faba4c7-4da9-4013-9697-20cc3fd40f85")ITaskService:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetFolder(BSTR path, ITaskFolder ** ppFolder) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRunningTasks(LONG flags, IRunningTaskCollection **ppRunningTasks) = 0;
    virtual HRESULT STDMETHODCALLTYPE NewTask(DWORD flags, ITaskDefinition **ppDefinition) = 0;
    virtual HRESULT STDMETHODCALLTYPE Connect(VARIANT serverName, VARIANT user, VARIANT domain, VARIANT password) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Connected(VARIANT_BOOL *pConnected) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_TargetServer(BSTR *pServer) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ConnectedUser(BSTR *pUser) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ConnectedDomain(BSTR *pDomain) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HighestVersion(DWORD *pVersion) = 0;
};
#else
typedef struct ITaskServiceVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(ITaskService * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITaskService * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITaskService * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ITaskService * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ITaskService * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ITaskService * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ITaskService * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *GetFolder)(ITaskService * This, BSTR path, ITaskFolder ** ppFolder);
    HRESULT (STDMETHODCALLTYPE *GetRunningTasks)(ITaskService * This, LONG flags, IRunningTaskCollection ** ppRunningTasks);
    HRESULT (STDMETHODCALLTYPE *NewTask)(ITaskService * This, DWORD flags, ITaskDefinition ** ppDefinition);
    HRESULT (STDMETHODCALLTYPE *Connect)(ITaskService * This, VARIANT serverName, VARIANT user, VARIANT domain, VARIANT password);
    HRESULT (STDMETHODCALLTYPE *get_Connected)(ITaskService * This, VARIANT_BOOL * pConnected);
    HRESULT (STDMETHODCALLTYPE *get_TargetServer)(ITaskService * This, BSTR * pServer);
    HRESULT (STDMETHODCALLTYPE *get_ConnectedUser)(ITaskService * This, BSTR * pUser);
    HRESULT (STDMETHODCALLTYPE *get_ConnectedDomain)(ITaskService * This, BSTR * pDomain);
    HRESULT (STDMETHODCALLTYPE *get_HighestVersion)(ITaskService * This, DWORD * pVersion);
    END_INTERFACE
} ITaskServiceVtbl;

interface ITaskService {
    CONST_VTBL struct ITaskServiceVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITaskService_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITaskService_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITaskService_Release(This)  ((This)->lpVtbl->Release(This))
#define ITaskService_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ITaskService_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ITaskService_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ITaskService_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ITaskService_GetFolder(This,path,ppFolder)  ((This)->lpVtbl->GetFolder(This,path,ppFolder))
#define ITaskService_GetRunningTasks(This,flags,ppRunningTasks)  ((This)->lpVtbl->GetRunningTasks(This,flags,ppRunningTasks))
#define ITaskService_NewTask(This,flags,ppDefinition)  ((This)->lpVtbl->NewTask(This,flags,ppDefinition))
#define ITaskService_Connect(This,serverName,user,domain,password)  ((This)->lpVtbl->Connect(This,serverName,user,domain,password))
#define ITaskService_get_Connected(This,pConnected)  ((This)->lpVtbl->get_Connected(This,pConnected))
#define ITaskService_get_TargetServer(This,pServer)  ((This)->lpVtbl->get_TargetServer(This,pServer))
#define ITaskService_get_ConnectedUser(This,pUser)  ((This)->lpVtbl->get_ConnectedUser(This,pUser))
#define ITaskService_get_ConnectedDomain(This,pDomain)  ((This)->lpVtbl->get_ConnectedDomain(This,pDomain))
#define ITaskService_get_HighestVersion(This,pVersion)  ((This)->lpVtbl->get_HighestVersion(This,pVersion))
#endif

#endif

#endif

#ifndef __ITaskHandler_INTERFACE_DEFINED__
#define __ITaskHandler_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITaskHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("839d7762-5121-4009-9234-4f0d19394f04")ITaskHandler:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Start(IUnknown * pHandlerServices, BSTR data) = 0;
    virtual HRESULT STDMETHODCALLTYPE Stop(HRESULT *pRetCode) = 0;
    virtual HRESULT STDMETHODCALLTYPE Pause(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Resume(void) = 0;
};
#else
typedef struct ITaskHandlerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (ITaskHandler * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(ITaskHandler * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITaskHandler * This);
    HRESULT (STDMETHODCALLTYPE *Start)(ITaskHandler * This, IUnknown * pHandlerServices, BSTR data);
    HRESULT (STDMETHODCALLTYPE *Stop)(ITaskHandler * This, HRESULT * pRetCode);
    HRESULT (STDMETHODCALLTYPE *Pause)(ITaskHandler * This);
    HRESULT (STDMETHODCALLTYPE *Resume)(ITaskHandler * This);
    END_INTERFACE
} ITaskHandlerVtbl;

interface ITaskHandler {
    CONST_VTBL struct ITaskHandlerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITaskHandler_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITaskHandler_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITaskHandler_Release(This)  ((This)->lpVtbl->Release(This))
#define ITaskHandler_Start(This,pHandlerServices,data)  ((This)->lpVtbl->Start(This,pHandlerServices,data))
#define ITaskHandler_Stop(This,pRetCode)  ((This)->lpVtbl->Stop(This,pRetCode))
#define ITaskHandler_Pause(This)  ((This)->lpVtbl->Pause(This))
#define ITaskHandler_Resume(This)  ((This)->lpVtbl->Resume(This))
#endif

#endif

#endif

#ifndef __ITaskHandlerStatus_INTERFACE_DEFINED__
#define __ITaskHandlerStatus_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITaskHandlerStatus;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("eaec7a8f-27a0-4ddc-8675-14726a01a38a")ITaskHandlerStatus:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE UpdateStatus(short percentComplete, BSTR statusMessage) = 0;
    virtual HRESULT STDMETHODCALLTYPE TaskCompleted(HRESULT taskErrCode) = 0;
};
#else
typedef struct ITaskHandlerStatusVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (ITaskHandlerStatus * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(ITaskHandlerStatus * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITaskHandlerStatus * This);
    HRESULT (STDMETHODCALLTYPE *UpdateStatus)(ITaskHandlerStatus * This, short percentComplete, BSTR statusMessage);
    HRESULT (STDMETHODCALLTYPE *TaskCompleted)(ITaskHandlerStatus * This, HRESULT taskErrCode);
    END_INTERFACE
} ITaskHandlerStatusVtbl;

interface ITaskHandlerStatus {
    CONST_VTBL struct ITaskHandlerStatusVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITaskHandlerStatus_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITaskHandlerStatus_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITaskHandlerStatus_Release(This)  ((This)->lpVtbl->Release(This))
#define ITaskHandlerStatus_UpdateStatus(This,percentComplete,statusMessage)  ((This)->lpVtbl->UpdateStatus(This,percentComplete,statusMessage))
#define ITaskHandlerStatus_TaskCompleted(This,taskErrCode)  ((This)->lpVtbl->TaskCompleted(This,taskErrCode))
#endif

#endif

#endif

#ifndef __ITaskVariables_INTERFACE_DEFINED__
#define __ITaskVariables_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITaskVariables;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("3e4c9351-d966-4b8b-bb87-ceba68bb0107")ITaskVariables:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetInput(BSTR * pInput) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetOutput(BSTR input) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetContext(BSTR *pContext) = 0;
};
#else
typedef struct ITaskVariablesVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (ITaskVariables * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(ITaskVariables * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITaskVariables * This);
    HRESULT (STDMETHODCALLTYPE *GetInput)(ITaskVariables * This, BSTR * pInput);
    HRESULT (STDMETHODCALLTYPE *SetOutput)(ITaskVariables * This, BSTR input);
    HRESULT (STDMETHODCALLTYPE *GetContext)(ITaskVariables * This, BSTR * pContext);
    END_INTERFACE
} ITaskVariablesVtbl;

interface ITaskVariables {
    CONST_VTBL struct ITaskVariablesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITaskVariables_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITaskVariables_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITaskVariables_Release(This)  ((This)->lpVtbl->Release(This))
#define ITaskVariables_GetInput(This,pInput)  ((This)->lpVtbl->GetInput(This,pInput))
#define ITaskVariables_SetOutput(This,input)  ((This)->lpVtbl->SetOutput(This,input))
#define ITaskVariables_GetContext(This,pContext)  ((This)->lpVtbl->GetContext(This,pContext))
#endif

#endif

#endif

#ifndef __ITaskNamedValuePair_INTERFACE_DEFINED__
#define __ITaskNamedValuePair_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITaskNamedValuePair;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("39038068-2B46-4afd-8662-7BB6F868D221")ITaskNamedValuePair:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * pName) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Name(BSTR name) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Value(BSTR *pValue) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Value(BSTR value) = 0;
};

#else

typedef struct ITaskNamedValuePairVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(ITaskNamedValuePair * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITaskNamedValuePair * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITaskNamedValuePair * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ITaskNamedValuePair * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ITaskNamedValuePair * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ITaskNamedValuePair * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ITaskNamedValuePair * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Name)(ITaskNamedValuePair * This, BSTR * pName);
    HRESULT (STDMETHODCALLTYPE *put_Name)(ITaskNamedValuePair * This, BSTR name);
    HRESULT (STDMETHODCALLTYPE *get_Value)(ITaskNamedValuePair * This, BSTR * pValue);
    HRESULT (STDMETHODCALLTYPE *put_Value)(ITaskNamedValuePair * This, BSTR value);
    END_INTERFACE
} ITaskNamedValuePairVtbl;

interface ITaskNamedValuePair {
    CONST_VTBL struct ITaskNamedValuePairVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITaskNamedValuePair_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITaskNamedValuePair_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITaskNamedValuePair_Release(This)  ((This)->lpVtbl->Release(This))
#define ITaskNamedValuePair_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ITaskNamedValuePair_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ITaskNamedValuePair_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ITaskNamedValuePair_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ITaskNamedValuePair_get_Name(This,pName)  ((This)->lpVtbl->get_Name(This,pName))
#define ITaskNamedValuePair_put_Name(This,name)  ((This)->lpVtbl->put_Name(This,name))
#define ITaskNamedValuePair_get_Value(This,pValue)  ((This)->lpVtbl->get_Value(This,pValue))
#define ITaskNamedValuePair_put_Value(This,value)  ((This)->lpVtbl->put_Value(This,value))
#endif

#endif

#endif

#ifndef __ITaskNamedValueCollection_INTERFACE_DEFINED__
#define __ITaskNamedValueCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITaskNamedValueCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("B4EF826B-63C3-46e4-A504-EF69E4F7EA4D")ITaskNamedValueCollection:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Count(long *pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Item(LONG index, ITaskNamedValuePair **ppPair) = 0;
    virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown **ppEnum) = 0;
    virtual HRESULT STDMETHODCALLTYPE Create(BSTR name, BSTR value, ITaskNamedValuePair **ppPair) = 0;
    virtual HRESULT STDMETHODCALLTYPE Remove(LONG index) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clear(void) = 0;
};
#else
typedef struct ITaskNamedValueCollectionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(ITaskNamedValueCollection * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITaskNamedValueCollection * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITaskNamedValueCollection * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ITaskNamedValueCollection * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ITaskNamedValueCollection * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ITaskNamedValueCollection * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ITaskNamedValueCollection * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Count)(ITaskNamedValueCollection * This, long *pCount);
    HRESULT (STDMETHODCALLTYPE *get_Item)(ITaskNamedValueCollection * This, LONG index, ITaskNamedValuePair ** ppPair);
    HRESULT (STDMETHODCALLTYPE *get__NewEnum)(ITaskNamedValueCollection * This, IUnknown ** ppEnum);
    HRESULT (STDMETHODCALLTYPE *Create)(ITaskNamedValueCollection * This, BSTR name, BSTR value, ITaskNamedValuePair ** ppPair);
    HRESULT (STDMETHODCALLTYPE *Remove)(ITaskNamedValueCollection * This, LONG index);
    HRESULT (STDMETHODCALLTYPE *Clear)(ITaskNamedValueCollection * This);
    END_INTERFACE
} ITaskNamedValueCollectionVtbl;

interface ITaskNamedValueCollection {
    CONST_VTBL struct ITaskNamedValueCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS

#define ITaskNamedValueCollection_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITaskNamedValueCollection_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITaskNamedValueCollection_Release(This)  ((This)->lpVtbl->Release(This))
#define ITaskNamedValueCollection_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ITaskNamedValueCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ITaskNamedValueCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ITaskNamedValueCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ITaskNamedValueCollection_get_Count(This,pCount)  ((This)->lpVtbl->get_Count(This,pCount))
#define ITaskNamedValueCollection_get_Item(This,index,ppPair)  ((This)->lpVtbl->get_Item(This,index,ppPair))
#define ITaskNamedValueCollection_get__NewEnum(This,ppEnum)  ((This)->lpVtbl->get__NewEnum(This,ppEnum))
#define ITaskNamedValueCollection_Create(This,name,value,ppPair)  ((This)->lpVtbl->Create(This,name,value,ppPair))
#define ITaskNamedValueCollection_Remove(This,index)  ((This)->lpVtbl->Remove(This,index))
#define ITaskNamedValueCollection_Clear(This)  ((This)->lpVtbl->Clear(This))
#endif

#endif

#endif

#ifndef __IRunningTask_INTERFACE_DEFINED__
#define __IRunningTask_INTERFACE_DEFINED__

EXTERN_C const IID IID_IRunningTask;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("653758fb-7b9a-4f1e-a471-beeb8e9b834e")IRunningTask:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * pName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_InstanceGuid(BSTR *pGuid) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Path(BSTR *pPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_State(TASK_STATE *pState) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_CurrentAction(BSTR *pName) = 0;
    virtual HRESULT STDMETHODCALLTYPE Stop(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE Refresh(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EnginePID(DWORD *pPID) = 0;
};
#else
typedef struct IRunningTaskVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IRunningTask * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IRunningTask * This);
    ULONG(STDMETHODCALLTYPE *Release)(IRunningTask * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IRunningTask * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IRunningTask * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IRunningTask * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IRunningTask * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Name)(IRunningTask * This, BSTR * pName);
    HRESULT (STDMETHODCALLTYPE *get_InstanceGuid)(IRunningTask * This, BSTR * pGuid);
    HRESULT (STDMETHODCALLTYPE *get_Path)(IRunningTask * This, BSTR * pPath);
    HRESULT (STDMETHODCALLTYPE *get_State)(IRunningTask * This, TASK_STATE * pState);
    HRESULT (STDMETHODCALLTYPE *get_CurrentAction)(IRunningTask * This, BSTR * pName);
    HRESULT (STDMETHODCALLTYPE *Stop)(IRunningTask * This);
    HRESULT (STDMETHODCALLTYPE *Refresh)(IRunningTask * This);
    HRESULT (STDMETHODCALLTYPE *get_EnginePID)(IRunningTask * This, DWORD * pPID);
    END_INTERFACE
} IRunningTaskVtbl;

interface IRunningTask {
    CONST_VTBL struct IRunningTaskVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IRunningTask_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IRunningTask_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IRunningTask_Release(This)  ((This)->lpVtbl->Release(This))
#define IRunningTask_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IRunningTask_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IRunningTask_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IRunningTask_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IRunningTask_get_Name(This,pName)  ((This)->lpVtbl->get_Name(This,pName))
#define IRunningTask_get_InstanceGuid(This,pGuid)  ((This)->lpVtbl->get_InstanceGuid(This,pGuid))
#define IRunningTask_get_Path(This,pPath)  ((This)->lpVtbl->get_Path(This,pPath))
#define IRunningTask_get_State(This,pState)  ((This)->lpVtbl->get_State(This,pState))
#define IRunningTask_get_CurrentAction(This,pName)  ((This)->lpVtbl->get_CurrentAction(This,pName))
#define IRunningTask_Stop(This)  ((This)->lpVtbl->Stop(This))
#define IRunningTask_Refresh(This)  ((This)->lpVtbl->Refresh(This))
#define IRunningTask_get_EnginePID(This,pPID)  ((This)->lpVtbl->get_EnginePID(This,pPID))
#endif

#endif

#endif

#ifndef __IRunningTaskCollection_INTERFACE_DEFINED__
#define __IRunningTaskCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_IRunningTaskCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("6a67614b-6828-4fec-aa54-6d52e8f1f2db")IRunningTaskCollection:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Item(VARIANT index, IRunningTask **ppRunningTask) = 0;
    virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown **ppEnum) = 0;
};
#else
typedef struct IRunningTaskCollectionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IRunningTaskCollection * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IRunningTaskCollection * This);
    ULONG(STDMETHODCALLTYPE *Release)(IRunningTaskCollection * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IRunningTaskCollection * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IRunningTaskCollection * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IRunningTaskCollection * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IRunningTaskCollection * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Count)(IRunningTaskCollection * This, LONG * pCount);
    HRESULT (STDMETHODCALLTYPE *get_Item)(IRunningTaskCollection * This, VARIANT index, IRunningTask ** ppRunningTask);
    HRESULT (STDMETHODCALLTYPE *get__NewEnum)(IRunningTaskCollection * This, IUnknown ** ppEnum);
    END_INTERFACE
} IRunningTaskCollectionVtbl;

interface IRunningTaskCollection {
    CONST_VTBL struct IRunningTaskCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IRunningTaskCollection_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IRunningTaskCollection_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IRunningTaskCollection_Release(This)  ((This)->lpVtbl->Release(This))
#define IRunningTaskCollection_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IRunningTaskCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IRunningTaskCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IRunningTaskCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IRunningTaskCollection_get_Count(This,pCount)  ((This)->lpVtbl->get_Count(This,pCount))
#define IRunningTaskCollection_get_Item(This,index,ppRunningTask)  ((This)->lpVtbl->get_Item(This,index,ppRunningTask))
#define IRunningTaskCollection_get__NewEnum(This,ppEnum)  ((This)->lpVtbl->get__NewEnum(This,ppEnum))
#endif

#endif

#endif

#ifndef __IRegisteredTask_INTERFACE_DEFINED__
#define __IRegisteredTask_INTERFACE_DEFINED__

EXTERN_C const IID IID_IRegisteredTask;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("9c86f320-dee3-4dd1-b972-a303f26b061e")IRegisteredTask:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * pName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Path(BSTR *pPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_State(TASK_STATE *pState) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Enabled(VARIANT_BOOL *pEnabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Enabled(VARIANT_BOOL enabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE Run(VARIANT params, IRunningTask **ppRunningTask) = 0;
    virtual HRESULT STDMETHODCALLTYPE RunEx(VARIANT params, LONG flags, LONG sessionID, BSTR user, IRunningTask **ppRunningTask) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetInstances(LONG flags, IRunningTaskCollection **ppRunningTasks) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LastRunTime(DATE *pLastRunTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LastTaskResult(LONG *pLastTaskResult) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NumberOfMissedRuns(LONG *pNumberOfMissedRuns) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NextRunTime(DATE *pNextRunTime) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Definition(ITaskDefinition **ppDefinition) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Xml(BSTR *pXml) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSecurityDescriptor(LONG securityInformation, BSTR *pSddl) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSecurityDescriptor(BSTR sddl, LONG flags) = 0;
    virtual HRESULT STDMETHODCALLTYPE Stop(LONG flags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetRunTimes(const LPSYSTEMTIME pstStart, const LPSYSTEMTIME pstEnd, DWORD *pCount, LPSYSTEMTIME *pRunTimes) = 0;
};

#else

typedef struct IRegisteredTaskVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IRegisteredTask * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IRegisteredTask * This);
    ULONG(STDMETHODCALLTYPE *Release)(IRegisteredTask * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IRegisteredTask * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IRegisteredTask * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IRegisteredTask * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IRegisteredTask * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Name)(IRegisteredTask * This, BSTR * pName);
    HRESULT (STDMETHODCALLTYPE *get_Path)(IRegisteredTask * This, BSTR * pPath);
    HRESULT (STDMETHODCALLTYPE *get_State)(IRegisteredTask * This, TASK_STATE * pState);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(IRegisteredTask * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(IRegisteredTask * This, VARIANT_BOOL enabled);
    HRESULT (STDMETHODCALLTYPE *Run)(IRegisteredTask * This, VARIANT params, IRunningTask ** ppRunningTask);
    HRESULT (STDMETHODCALLTYPE *RunEx)(IRegisteredTask * This, VARIANT params, LONG flags, LONG sessionID, BSTR user, IRunningTask ** ppRunningTask);
    HRESULT (STDMETHODCALLTYPE *GetInstances)(IRegisteredTask * This, LONG flags, IRunningTaskCollection ** ppRunningTasks);
    HRESULT (STDMETHODCALLTYPE *get_LastRunTime)(IRegisteredTask * This, DATE * pLastRunTime);
    HRESULT (STDMETHODCALLTYPE *get_LastTaskResult)(IRegisteredTask * This, LONG * pLastTaskResult);
    HRESULT (STDMETHODCALLTYPE *get_NumberOfMissedRuns)(IRegisteredTask * This, LONG * pNumberOfMissedRuns);
    HRESULT (STDMETHODCALLTYPE *get_NextRunTime)(IRegisteredTask * This, DATE * pNextRunTime);
    HRESULT (STDMETHODCALLTYPE *get_Definition)(IRegisteredTask * This, ITaskDefinition ** ppDefinition);
    HRESULT (STDMETHODCALLTYPE *get_Xml)(IRegisteredTask * This, BSTR * pXml);
    HRESULT (STDMETHODCALLTYPE *GetSecurityDescriptor)(IRegisteredTask * This, LONG securityInformation, BSTR * pSddl);
    HRESULT (STDMETHODCALLTYPE *SetSecurityDescriptor)(IRegisteredTask * This, BSTR sddl, LONG flags);
    HRESULT (STDMETHODCALLTYPE *Stop)(IRegisteredTask * This, LONG flags);
    HRESULT (STDMETHODCALLTYPE *GetRunTimes)(IRegisteredTask * This, const LPSYSTEMTIME pstStart, const LPSYSTEMTIME pstEnd, DWORD * pCount, LPSYSTEMTIME * pRunTimes);
    END_INTERFACE
} IRegisteredTaskVtbl;

interface IRegisteredTask {
    CONST_VTBL struct IRegisteredTaskVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IRegisteredTask_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IRegisteredTask_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IRegisteredTask_Release(This)  ((This)->lpVtbl->Release(This))
#define IRegisteredTask_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IRegisteredTask_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IRegisteredTask_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IRegisteredTask_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IRegisteredTask_get_Name(This,pName)  ((This)->lpVtbl->get_Name(This,pName))
#define IRegisteredTask_get_Path(This,pPath)  ((This)->lpVtbl->get_Path(This,pPath))
#define IRegisteredTask_get_State(This,pState)  ((This)->lpVtbl->get_State(This,pState))
#define IRegisteredTask_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define IRegisteredTask_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#define IRegisteredTask_Run(This,params,ppRunningTask)  ((This)->lpVtbl->Run(This,params,ppRunningTask))
#define IRegisteredTask_RunEx(This,params,flags,sessionID,user,ppRunningTask)  ((This)->lpVtbl->RunEx(This,params,flags,sessionID,user,ppRunningTask))
#define IRegisteredTask_GetInstances(This,flags,ppRunningTasks)  ((This)->lpVtbl->GetInstances(This,flags,ppRunningTasks))
#define IRegisteredTask_get_LastRunTime(This,pLastRunTime)  ((This)->lpVtbl->get_LastRunTime(This,pLastRunTime))
#define IRegisteredTask_get_LastTaskResult(This,pLastTaskResult)  ((This)->lpVtbl->get_LastTaskResult(This,pLastTaskResult))
#define IRegisteredTask_get_NumberOfMissedRuns(This,pNumberOfMissedRuns)  ((This)->lpVtbl->get_NumberOfMissedRuns(This,pNumberOfMissedRuns))
#define IRegisteredTask_get_NextRunTime(This,pNextRunTime)  ((This)->lpVtbl->get_NextRunTime(This,pNextRunTime))
#define IRegisteredTask_get_Definition(This,ppDefinition)  ((This)->lpVtbl->get_Definition(This,ppDefinition))
#define IRegisteredTask_get_Xml(This,pXml)  ((This)->lpVtbl->get_Xml(This,pXml))
#define IRegisteredTask_GetSecurityDescriptor(This,securityInformation,pSddl)  ((This)->lpVtbl->GetSecurityDescriptor(This,securityInformation,pSddl))
#define IRegisteredTask_SetSecurityDescriptor(This,sddl,flags)  ((This)->lpVtbl->SetSecurityDescriptor(This,sddl,flags))
#define IRegisteredTask_Stop(This,flags)  ((This)->lpVtbl->Stop(This,flags))
#define IRegisteredTask_GetRunTimes(This,pstStart,pstEnd,pCount,pRunTimes)  ((This)->lpVtbl->GetRunTimes(This,pstStart,pstEnd,pCount,pRunTimes))
#endif

#endif

#endif

#ifndef __ITrigger_INTERFACE_DEFINED__
#define __ITrigger_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITrigger;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("09941815-ea89-4b5b-89e0-2a773801fac3")ITrigger:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Type(TASK_TRIGGER_TYPE2 * pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Id(BSTR *pId) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Id(BSTR id) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Repetition(IRepetitionPattern **ppRepeat) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Repetition(IRepetitionPattern *pRepeat) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ExecutionTimeLimit(BSTR *pTimeLimit) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ExecutionTimeLimit(BSTR timelimit) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StartBoundary(BSTR *pStart) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StartBoundary(BSTR start) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_EndBoundary(BSTR *pEnd) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_EndBoundary(BSTR end) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Enabled(VARIANT_BOOL *pEnabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Enabled(VARIANT_BOOL enabled) = 0;
};
#else
typedef struct ITriggerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (ITrigger * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITrigger * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITrigger * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ITrigger * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ITrigger * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ITrigger * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ITrigger * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Type)(ITrigger * This, TASK_TRIGGER_TYPE2 * pType);
    HRESULT (STDMETHODCALLTYPE *get_Id)(ITrigger * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(ITrigger * This, BSTR id);
    HRESULT (STDMETHODCALLTYPE *get_Repetition)(ITrigger * This, IRepetitionPattern ** ppRepeat);
    HRESULT (STDMETHODCALLTYPE *put_Repetition)(ITrigger * This, IRepetitionPattern * pRepeat);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(ITrigger * This, BSTR * pTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(ITrigger * This, BSTR timelimit);
    HRESULT (STDMETHODCALLTYPE *get_StartBoundary)(ITrigger * This, BSTR * pStart);
    HRESULT (STDMETHODCALLTYPE *put_StartBoundary)(ITrigger * This, BSTR start);
    HRESULT (STDMETHODCALLTYPE *get_EndBoundary)(ITrigger * This, BSTR * pEnd);
    HRESULT (STDMETHODCALLTYPE *put_EndBoundary)(ITrigger * This, BSTR end);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(ITrigger * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(ITrigger * This, VARIANT_BOOL enabled);
    END_INTERFACE
} ITriggerVtbl;

interface ITrigger {
    CONST_VTBL struct ITriggerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITrigger_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITrigger_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITrigger_Release(This)  ((This)->lpVtbl->Release(This))
#define ITrigger_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ITrigger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ITrigger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ITrigger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ITrigger_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define ITrigger_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define ITrigger_put_Id(This,id)  ((This)->lpVtbl->put_Id(This,id))
#define ITrigger_get_Repetition(This,ppRepeat)  ((This)->lpVtbl->get_Repetition(This,ppRepeat))
#define ITrigger_put_Repetition(This,pRepeat)  ((This)->lpVtbl->put_Repetition(This,pRepeat))
#define ITrigger_get_ExecutionTimeLimit(This,pTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pTimeLimit))
#define ITrigger_put_ExecutionTimeLimit(This,timelimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,timelimit))
#define ITrigger_get_StartBoundary(This,pStart)  ((This)->lpVtbl->get_StartBoundary(This,pStart))
#define ITrigger_put_StartBoundary(This,start)  ((This)->lpVtbl->put_StartBoundary(This,start))
#define ITrigger_get_EndBoundary(This,pEnd)  ((This)->lpVtbl->get_EndBoundary(This,pEnd))
#define ITrigger_put_EndBoundary(This,end)  ((This)->lpVtbl->put_EndBoundary(This,end))
#define ITrigger_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define ITrigger_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#endif

#endif

#endif

#ifndef __IIdleTrigger_INTERFACE_DEFINED__
#define __IIdleTrigger_INTERFACE_DEFINED__

EXTERN_C const IID IID_IIdleTrigger;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("d537d2b0-9fb3-4d34-9739-1ff5ce7b1ef3")IIdleTrigger:public ITrigger
{
    public:
};
#else
typedef struct IIdleTriggerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IIdleTrigger * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IIdleTrigger * This);
    ULONG(STDMETHODCALLTYPE *Release)(IIdleTrigger * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IIdleTrigger * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IIdleTrigger * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IIdleTrigger * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IIdleTrigger * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IIdleTrigger * This, TASK_TRIGGER_TYPE2 * pType);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IIdleTrigger * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IIdleTrigger * This, BSTR id);
    HRESULT (STDMETHODCALLTYPE *get_Repetition)(IIdleTrigger * This, IRepetitionPattern ** ppRepeat);
    HRESULT (STDMETHODCALLTYPE *put_Repetition)(IIdleTrigger * This, IRepetitionPattern * pRepeat);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(IIdleTrigger * This, BSTR * pTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(IIdleTrigger * This, BSTR timelimit);
    HRESULT (STDMETHODCALLTYPE *get_StartBoundary)(IIdleTrigger * This, BSTR * pStart);
    HRESULT (STDMETHODCALLTYPE *put_StartBoundary)(IIdleTrigger * This, BSTR start);
    HRESULT (STDMETHODCALLTYPE *get_EndBoundary)(IIdleTrigger * This, BSTR * pEnd);
    HRESULT (STDMETHODCALLTYPE *put_EndBoundary)(IIdleTrigger * This, BSTR end);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(IIdleTrigger * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(IIdleTrigger * This, VARIANT_BOOL enabled);
    END_INTERFACE
} IIdleTriggerVtbl;

interface IIdleTrigger {
    CONST_VTBL struct IIdleTriggerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IIdleTrigger_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IIdleTrigger_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IIdleTrigger_Release(This)  ((This)->lpVtbl->Release(This))
#define IIdleTrigger_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IIdleTrigger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IIdleTrigger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IIdleTrigger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IIdleTrigger_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define IIdleTrigger_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IIdleTrigger_put_Id(This,id)  ((This)->lpVtbl->put_Id(This,id))
#define IIdleTrigger_get_Repetition(This,ppRepeat)  ((This)->lpVtbl->get_Repetition(This,ppRepeat))
#define IIdleTrigger_put_Repetition(This,pRepeat)  ((This)->lpVtbl->put_Repetition(This,pRepeat))
#define IIdleTrigger_get_ExecutionTimeLimit(This,pTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pTimeLimit))
#define IIdleTrigger_put_ExecutionTimeLimit(This,timelimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,timelimit))
#define IIdleTrigger_get_StartBoundary(This,pStart)  ((This)->lpVtbl->get_StartBoundary(This,pStart))
#define IIdleTrigger_put_StartBoundary(This,start)  ((This)->lpVtbl->put_StartBoundary(This,start))
#define IIdleTrigger_get_EndBoundary(This,pEnd)  ((This)->lpVtbl->get_EndBoundary(This,pEnd))
#define IIdleTrigger_put_EndBoundary(This,end)  ((This)->lpVtbl->put_EndBoundary(This,end))
#define IIdleTrigger_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define IIdleTrigger_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#endif

#endif

#endif

#ifndef __ILogonTrigger_INTERFACE_DEFINED__
#define __ILogonTrigger_INTERFACE_DEFINED__

EXTERN_C const IID IID_ILogonTrigger;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("72DADE38-FAE4-4b3e-BAF4-5D009AF02B1C")ILogonTrigger:public ITrigger
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Delay(BSTR * pDelay) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Delay(BSTR delay) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UserId(BSTR *pUser) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_UserId(BSTR user) = 0;
};

#else

typedef struct ILogonTriggerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (ILogonTrigger * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ILogonTrigger * This);
    ULONG(STDMETHODCALLTYPE *Release)(ILogonTrigger * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ILogonTrigger * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ILogonTrigger * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ILogonTrigger * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ILogonTrigger * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Type)(ILogonTrigger * This, TASK_TRIGGER_TYPE2 * pType);
    HRESULT (STDMETHODCALLTYPE *get_Id)(ILogonTrigger * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(ILogonTrigger * This, BSTR id);
    HRESULT (STDMETHODCALLTYPE *get_Repetition)(ILogonTrigger * This, IRepetitionPattern ** ppRepeat);
    HRESULT (STDMETHODCALLTYPE *put_Repetition)(ILogonTrigger * This, IRepetitionPattern * pRepeat);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(ILogonTrigger * This, BSTR * pTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(ILogonTrigger * This, BSTR timelimit);
    HRESULT (STDMETHODCALLTYPE *get_StartBoundary)(ILogonTrigger * This, BSTR * pStart);
    HRESULT (STDMETHODCALLTYPE *put_StartBoundary)(ILogonTrigger * This, BSTR start);
    HRESULT (STDMETHODCALLTYPE *get_EndBoundary)(ILogonTrigger * This, BSTR * pEnd);
    HRESULT (STDMETHODCALLTYPE *put_EndBoundary)(ILogonTrigger * This, BSTR end);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(ILogonTrigger * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(ILogonTrigger * This, VARIANT_BOOL enabled);
    HRESULT (STDMETHODCALLTYPE *get_Delay)(ILogonTrigger * This, BSTR * pDelay);
    HRESULT (STDMETHODCALLTYPE *put_Delay)(ILogonTrigger * This, BSTR delay);
    HRESULT (STDMETHODCALLTYPE *get_UserId)(ILogonTrigger * This, BSTR * pUser);
    HRESULT (STDMETHODCALLTYPE *put_UserId)(ILogonTrigger * This, BSTR user);
    END_INTERFACE
} ILogonTriggerVtbl;

interface ILogonTrigger {
    CONST_VTBL struct ILogonTriggerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ILogonTrigger_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ILogonTrigger_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ILogonTrigger_Release(This)  ((This)->lpVtbl->Release(This))
#define ILogonTrigger_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ILogonTrigger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ILogonTrigger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ILogonTrigger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ILogonTrigger_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define ILogonTrigger_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define ILogonTrigger_put_Id(This,id)  ((This)->lpVtbl->put_Id(This,id))
#define ILogonTrigger_get_Repetition(This,ppRepeat)  ((This)->lpVtbl->get_Repetition(This,ppRepeat))
#define ILogonTrigger_put_Repetition(This,pRepeat)  ((This)->lpVtbl->put_Repetition(This,pRepeat))
#define ILogonTrigger_get_ExecutionTimeLimit(This,pTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pTimeLimit))
#define ILogonTrigger_put_ExecutionTimeLimit(This,timelimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,timelimit))
#define ILogonTrigger_get_StartBoundary(This,pStart)  ((This)->lpVtbl->get_StartBoundary(This,pStart))
#define ILogonTrigger_put_StartBoundary(This,start)  ((This)->lpVtbl->put_StartBoundary(This,start))
#define ILogonTrigger_get_EndBoundary(This,pEnd)  ((This)->lpVtbl->get_EndBoundary(This,pEnd))
#define ILogonTrigger_put_EndBoundary(This,end)  ((This)->lpVtbl->put_EndBoundary(This,end))
#define ILogonTrigger_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define ILogonTrigger_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#define ILogonTrigger_get_Delay(This,pDelay)  ((This)->lpVtbl->get_Delay(This,pDelay))
#define ILogonTrigger_put_Delay(This,delay)  ((This)->lpVtbl->put_Delay(This,delay))
#define ILogonTrigger_get_UserId(This,pUser)  ((This)->lpVtbl->get_UserId(This,pUser))
#define ILogonTrigger_put_UserId(This,user)  ((This)->lpVtbl->put_UserId(This,user))
#endif

#endif

#endif

#ifndef __ISessionStateChangeTrigger_INTERFACE_DEFINED__
#define __ISessionStateChangeTrigger_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISessionStateChangeTrigger;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("754DA71B-4385-4475-9DD9-598294FA3641")ISessionStateChangeTrigger:public ITrigger
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Delay(BSTR * pDelay) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Delay(BSTR delay) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UserId(BSTR *pUser) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_UserId(BSTR user) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StateChange(TASK_SESSION_STATE_CHANGE_TYPE *pType) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StateChange(TASK_SESSION_STATE_CHANGE_TYPE type) = 0;
};

#else

typedef struct ISessionStateChangeTriggerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(ISessionStateChangeTrigger * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(ISessionStateChangeTrigger * This);
    ULONG (STDMETHODCALLTYPE *Release)(ISessionStateChangeTrigger * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ISessionStateChangeTrigger * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ISessionStateChangeTrigger * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ISessionStateChangeTrigger * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ISessionStateChangeTrigger * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Type)(ISessionStateChangeTrigger * This, TASK_TRIGGER_TYPE2 * pType);
    HRESULT (STDMETHODCALLTYPE *get_Id)(ISessionStateChangeTrigger * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(ISessionStateChangeTrigger * This, BSTR id);
    HRESULT (STDMETHODCALLTYPE *get_Repetition)(ISessionStateChangeTrigger * This, IRepetitionPattern ** ppRepeat);
    HRESULT (STDMETHODCALLTYPE *put_Repetition)(ISessionStateChangeTrigger * This, IRepetitionPattern * pRepeat);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(ISessionStateChangeTrigger * This, BSTR * pTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(ISessionStateChangeTrigger * This, BSTR timelimit);
    HRESULT (STDMETHODCALLTYPE *get_StartBoundary)(ISessionStateChangeTrigger * This, BSTR * pStart);
    HRESULT (STDMETHODCALLTYPE *put_StartBoundary)(ISessionStateChangeTrigger * This, BSTR start);
    HRESULT (STDMETHODCALLTYPE *get_EndBoundary)(ISessionStateChangeTrigger * This, BSTR * pEnd);
    HRESULT (STDMETHODCALLTYPE *put_EndBoundary)(ISessionStateChangeTrigger * This, BSTR end);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(ISessionStateChangeTrigger * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(ISessionStateChangeTrigger * This, VARIANT_BOOL enabled);
    HRESULT (STDMETHODCALLTYPE *get_Delay)(ISessionStateChangeTrigger * This, BSTR * pDelay);
    HRESULT (STDMETHODCALLTYPE *put_Delay)(ISessionStateChangeTrigger * This, BSTR delay);
    HRESULT (STDMETHODCALLTYPE *get_UserId)(ISessionStateChangeTrigger * This, BSTR * pUser);
    HRESULT (STDMETHODCALLTYPE *put_UserId)(ISessionStateChangeTrigger * This, BSTR user);
    HRESULT (STDMETHODCALLTYPE *get_StateChange)(ISessionStateChangeTrigger * This, TASK_SESSION_STATE_CHANGE_TYPE * pType);
    HRESULT (STDMETHODCALLTYPE *put_StateChange)(ISessionStateChangeTrigger * This, TASK_SESSION_STATE_CHANGE_TYPE type);
    END_INTERFACE
} ISessionStateChangeTriggerVtbl;

interface ISessionStateChangeTrigger {
    CONST_VTBL struct ISessionStateChangeTriggerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISessionStateChangeTrigger_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ISessionStateChangeTrigger_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ISessionStateChangeTrigger_Release(This)  ((This)->lpVtbl->Release(This))
#define ISessionStateChangeTrigger_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ISessionStateChangeTrigger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ISessionStateChangeTrigger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ISessionStateChangeTrigger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ISessionStateChangeTrigger_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define ISessionStateChangeTrigger_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define ISessionStateChangeTrigger_put_Id(This,id)  ((This)->lpVtbl->put_Id(This,id))
#define ISessionStateChangeTrigger_get_Repetition(This,ppRepeat)  ((This)->lpVtbl->get_Repetition(This,ppRepeat))
#define ISessionStateChangeTrigger_put_Repetition(This,pRepeat)  ((This)->lpVtbl->put_Repetition(This,pRepeat))
#define ISessionStateChangeTrigger_get_ExecutionTimeLimit(This,pTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pTimeLimit))
#define ISessionStateChangeTrigger_put_ExecutionTimeLimit(This,timelimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,timelimit))
#define ISessionStateChangeTrigger_get_StartBoundary(This,pStart)  ((This)->lpVtbl->get_StartBoundary(This,pStart))
#define ISessionStateChangeTrigger_put_StartBoundary(This,start)  ((This)->lpVtbl->put_StartBoundary(This,start))
#define ISessionStateChangeTrigger_get_EndBoundary(This,pEnd)  ((This)->lpVtbl->get_EndBoundary(This,pEnd))
#define ISessionStateChangeTrigger_put_EndBoundary(This,end)  ((This)->lpVtbl->put_EndBoundary(This,end))
#define ISessionStateChangeTrigger_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define ISessionStateChangeTrigger_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#define ISessionStateChangeTrigger_get_Delay(This,pDelay)  ((This)->lpVtbl->get_Delay(This,pDelay))
#define ISessionStateChangeTrigger_put_Delay(This,delay)  ((This)->lpVtbl->put_Delay(This,delay))
#define ISessionStateChangeTrigger_get_UserId(This,pUser)  ((This)->lpVtbl->get_UserId(This,pUser))
#define ISessionStateChangeTrigger_put_UserId(This,user)  ((This)->lpVtbl->put_UserId(This,user))
#define ISessionStateChangeTrigger_get_StateChange(This,pType)  ((This)->lpVtbl->get_StateChange(This,pType))
#define ISessionStateChangeTrigger_put_StateChange(This,type)  ((This)->lpVtbl->put_StateChange(This,type))
#endif

#endif

#endif

#ifndef __IEventTrigger_INTERFACE_DEFINED__
#define __IEventTrigger_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEventTrigger;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("d45b0167-9653-4eef-b94f-0732ca7af251")IEventTrigger:public ITrigger
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Subscription(BSTR * pQuery) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Subscription(BSTR query) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Delay(BSTR *pDelay) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Delay(BSTR delay) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ValueQueries(ITaskNamedValueCollection **ppNamedXPaths) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ValueQueries(ITaskNamedValueCollection *pNamedXPaths) = 0;
};
#else
typedef struct IEventTriggerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IEventTrigger * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IEventTrigger * This);
    ULONG (STDMETHODCALLTYPE *Release)(IEventTrigger * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IEventTrigger * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IEventTrigger * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IEventTrigger * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IEventTrigger * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IEventTrigger * This, TASK_TRIGGER_TYPE2 * pType);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IEventTrigger * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IEventTrigger * This, BSTR id);
    HRESULT (STDMETHODCALLTYPE *get_Repetition)(IEventTrigger * This, IRepetitionPattern ** ppRepeat);
    HRESULT (STDMETHODCALLTYPE *put_Repetition)(IEventTrigger * This, IRepetitionPattern * pRepeat);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(IEventTrigger * This, BSTR * pTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(IEventTrigger * This, BSTR timelimit);
    HRESULT (STDMETHODCALLTYPE *get_StartBoundary)(IEventTrigger * This, BSTR * pStart);
    HRESULT (STDMETHODCALLTYPE *put_StartBoundary)(IEventTrigger * This, BSTR start);
    HRESULT (STDMETHODCALLTYPE *get_EndBoundary)(IEventTrigger * This, BSTR * pEnd);
    HRESULT (STDMETHODCALLTYPE *put_EndBoundary)(IEventTrigger * This, BSTR end);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(IEventTrigger * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(IEventTrigger * This, VARIANT_BOOL enabled);
    HRESULT (STDMETHODCALLTYPE *get_Subscription)(IEventTrigger * This, BSTR * pQuery);
    HRESULT (STDMETHODCALLTYPE *put_Subscription)(IEventTrigger * This, BSTR query);
    HRESULT (STDMETHODCALLTYPE *get_Delay)(IEventTrigger * This, BSTR * pDelay);
    HRESULT (STDMETHODCALLTYPE *put_Delay)(IEventTrigger * This, BSTR delay);
    HRESULT (STDMETHODCALLTYPE *get_ValueQueries)(IEventTrigger * This, ITaskNamedValueCollection ** ppNamedXPaths);
    HRESULT (STDMETHODCALLTYPE *put_ValueQueries)(IEventTrigger * This, ITaskNamedValueCollection * pNamedXPaths);
    END_INTERFACE
} IEventTriggerVtbl;

interface IEventTrigger {
    CONST_VTBL struct IEventTriggerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEventTrigger_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IEventTrigger_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IEventTrigger_Release(This)  ((This)->lpVtbl->Release(This))
#define IEventTrigger_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IEventTrigger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IEventTrigger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IEventTrigger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IEventTrigger_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define IEventTrigger_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IEventTrigger_put_Id(This,id)  ((This)->lpVtbl->put_Id(This,id))
#define IEventTrigger_get_Repetition(This,ppRepeat)  ((This)->lpVtbl->get_Repetition(This,ppRepeat))
#define IEventTrigger_put_Repetition(This,pRepeat)  ((This)->lpVtbl->put_Repetition(This,pRepeat))
#define IEventTrigger_get_ExecutionTimeLimit(This,pTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pTimeLimit))
#define IEventTrigger_put_ExecutionTimeLimit(This,timelimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,timelimit))
#define IEventTrigger_get_StartBoundary(This,pStart)  ((This)->lpVtbl->get_StartBoundary(This,pStart))
#define IEventTrigger_put_StartBoundary(This,start)  ((This)->lpVtbl->put_StartBoundary(This,start))
#define IEventTrigger_get_EndBoundary(This,pEnd)  ((This)->lpVtbl->get_EndBoundary(This,pEnd))
#define IEventTrigger_put_EndBoundary(This,end)  ((This)->lpVtbl->put_EndBoundary(This,end))
#define IEventTrigger_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define IEventTrigger_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#define IEventTrigger_get_Subscription(This,pQuery)  ((This)->lpVtbl->get_Subscription(This,pQuery))
#define IEventTrigger_put_Subscription(This,query)  ((This)->lpVtbl->put_Subscription(This,query))
#define IEventTrigger_get_Delay(This,pDelay)  ((This)->lpVtbl->get_Delay(This,pDelay))
#define IEventTrigger_put_Delay(This,delay)  ((This)->lpVtbl->put_Delay(This,delay))
#define IEventTrigger_get_ValueQueries(This,ppNamedXPaths)  ((This)->lpVtbl->get_ValueQueries(This,ppNamedXPaths))
#define IEventTrigger_put_ValueQueries(This,pNamedXPaths)  ((This)->lpVtbl->put_ValueQueries(This,pNamedXPaths))
#endif

#endif

#endif

#ifndef __ITimeTrigger_INTERFACE_DEFINED__
#define __ITimeTrigger_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITimeTrigger;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("b45747e0-eba7-4276-9f29-85c5bb300006")ITimeTrigger:public ITrigger
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_RandomDelay(BSTR * pRandomDelay) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RandomDelay(BSTR randomDelay) = 0;
};
#else
typedef struct ITimeTriggerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(ITimeTrigger * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITimeTrigger * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITimeTrigger * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ITimeTrigger * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ITimeTrigger * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ITimeTrigger * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ITimeTrigger * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Type)(ITimeTrigger * This, TASK_TRIGGER_TYPE2 * pType);
    HRESULT (STDMETHODCALLTYPE *get_Id)(ITimeTrigger * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(ITimeTrigger * This, BSTR id);
    HRESULT (STDMETHODCALLTYPE *get_Repetition)(ITimeTrigger * This, IRepetitionPattern ** ppRepeat);
    HRESULT (STDMETHODCALLTYPE *put_Repetition)(ITimeTrigger * This, IRepetitionPattern * pRepeat);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(ITimeTrigger * This, BSTR * pTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(ITimeTrigger * This, BSTR timelimit);
    HRESULT (STDMETHODCALLTYPE *get_StartBoundary)(ITimeTrigger * This, BSTR * pStart);
    HRESULT (STDMETHODCALLTYPE *put_StartBoundary)(ITimeTrigger * This, BSTR start);
    HRESULT (STDMETHODCALLTYPE *get_EndBoundary)(ITimeTrigger * This, BSTR * pEnd);
    HRESULT (STDMETHODCALLTYPE *put_EndBoundary)(ITimeTrigger * This, BSTR end);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(ITimeTrigger * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(ITimeTrigger * This, VARIANT_BOOL enabled);
    HRESULT (STDMETHODCALLTYPE *get_RandomDelay)(ITimeTrigger * This, BSTR * pRandomDelay);
    HRESULT (STDMETHODCALLTYPE *put_RandomDelay)(ITimeTrigger * This, BSTR randomDelay);
    END_INTERFACE
} ITimeTriggerVtbl;

interface ITimeTrigger {
    CONST_VTBL struct ITimeTriggerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITimeTrigger_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITimeTrigger_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITimeTrigger_Release(This)  ((This)->lpVtbl->Release(This))
#define ITimeTrigger_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ITimeTrigger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ITimeTrigger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ITimeTrigger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ITimeTrigger_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define ITimeTrigger_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define ITimeTrigger_put_Id(This,id)  ((This)->lpVtbl->put_Id(This,id))
#define ITimeTrigger_get_Repetition(This,ppRepeat)  ((This)->lpVtbl->get_Repetition(This,ppRepeat))
#define ITimeTrigger_put_Repetition(This,pRepeat)  ((This)->lpVtbl->put_Repetition(This,pRepeat))
#define ITimeTrigger_get_ExecutionTimeLimit(This,pTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pTimeLimit))
#define ITimeTrigger_put_ExecutionTimeLimit(This,timelimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,timelimit))
#define ITimeTrigger_get_StartBoundary(This,pStart)  ((This)->lpVtbl->get_StartBoundary(This,pStart))
#define ITimeTrigger_put_StartBoundary(This,start)  ((This)->lpVtbl->put_StartBoundary(This,start))
#define ITimeTrigger_get_EndBoundary(This,pEnd)  ((This)->lpVtbl->get_EndBoundary(This,pEnd))
#define ITimeTrigger_put_EndBoundary(This,end)  ((This)->lpVtbl->put_EndBoundary(This,end))
#define ITimeTrigger_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define ITimeTrigger_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#define ITimeTrigger_get_RandomDelay(This,pRandomDelay)  ((This)->lpVtbl->get_RandomDelay(This,pRandomDelay))
#define ITimeTrigger_put_RandomDelay(This,randomDelay)  ((This)->lpVtbl->put_RandomDelay(This,randomDelay))
#endif

#endif

#endif

#ifndef __IDailyTrigger_INTERFACE_DEFINED__
#define __IDailyTrigger_INTERFACE_DEFINED__

EXTERN_C const IID IID_IDailyTrigger;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("126c5cd8-b288-41d5-8dbf-e491446adc5c")IDailyTrigger:public ITrigger
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_DaysInterval(short *pDays) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DaysInterval(short days) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RandomDelay(BSTR *pRandomDelay) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RandomDelay(BSTR randomDelay) = 0;
};
#else
typedef struct IDailyTriggerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IDailyTrigger * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IDailyTrigger * This);
    ULONG(STDMETHODCALLTYPE *Release)(IDailyTrigger * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IDailyTrigger * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IDailyTrigger * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IDailyTrigger * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IDailyTrigger * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IDailyTrigger * This, TASK_TRIGGER_TYPE2 * pType);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IDailyTrigger * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IDailyTrigger * This, BSTR id);
    HRESULT (STDMETHODCALLTYPE *get_Repetition)(IDailyTrigger * This, IRepetitionPattern ** ppRepeat);
    HRESULT (STDMETHODCALLTYPE *put_Repetition)(IDailyTrigger * This, IRepetitionPattern * pRepeat);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(IDailyTrigger * This, BSTR * pTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(IDailyTrigger * This, BSTR timelimit);
    HRESULT (STDMETHODCALLTYPE *get_StartBoundary)(IDailyTrigger * This, BSTR * pStart);
    HRESULT (STDMETHODCALLTYPE *put_StartBoundary)(IDailyTrigger * This, BSTR start);
    HRESULT (STDMETHODCALLTYPE *get_EndBoundary)(IDailyTrigger * This, BSTR * pEnd);
    HRESULT (STDMETHODCALLTYPE *put_EndBoundary)(IDailyTrigger * This, BSTR end);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(IDailyTrigger * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(IDailyTrigger * This, VARIANT_BOOL enabled);
    HRESULT (STDMETHODCALLTYPE *get_DaysInterval)(IDailyTrigger * This, short *pDays);
    HRESULT (STDMETHODCALLTYPE *put_DaysInterval)(IDailyTrigger * This, short days);
    HRESULT (STDMETHODCALLTYPE *get_RandomDelay)(IDailyTrigger * This, BSTR * pRandomDelay);
    HRESULT (STDMETHODCALLTYPE *put_RandomDelay)(IDailyTrigger * This, BSTR randomDelay);
    END_INTERFACE
} IDailyTriggerVtbl;

interface IDailyTrigger {
    CONST_VTBL struct IDailyTriggerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IDailyTrigger_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IDailyTrigger_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IDailyTrigger_Release(This)  ((This)->lpVtbl->Release(This))
#define IDailyTrigger_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IDailyTrigger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IDailyTrigger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IDailyTrigger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IDailyTrigger_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define IDailyTrigger_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IDailyTrigger_put_Id(This,id)  ((This)->lpVtbl->put_Id(This,id))
#define IDailyTrigger_get_Repetition(This,ppRepeat)  ((This)->lpVtbl->get_Repetition(This,ppRepeat))
#define IDailyTrigger_put_Repetition(This,pRepeat)  ((This)->lpVtbl->put_Repetition(This,pRepeat))
#define IDailyTrigger_get_ExecutionTimeLimit(This,pTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pTimeLimit))
#define IDailyTrigger_put_ExecutionTimeLimit(This,timelimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,timelimit))
#define IDailyTrigger_get_StartBoundary(This,pStart)  ((This)->lpVtbl->get_StartBoundary(This,pStart))
#define IDailyTrigger_put_StartBoundary(This,start)  ((This)->lpVtbl->put_StartBoundary(This,start))
#define IDailyTrigger_get_EndBoundary(This,pEnd)  ((This)->lpVtbl->get_EndBoundary(This,pEnd))
#define IDailyTrigger_put_EndBoundary(This,end)  ((This)->lpVtbl->put_EndBoundary(This,end))
#define IDailyTrigger_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define IDailyTrigger_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#define IDailyTrigger_get_DaysInterval(This,pDays)  ((This)->lpVtbl->get_DaysInterval(This,pDays))
#define IDailyTrigger_put_DaysInterval(This,days)  ((This)->lpVtbl->put_DaysInterval(This,days))
#define IDailyTrigger_get_RandomDelay(This,pRandomDelay)  ((This)->lpVtbl->get_RandomDelay(This,pRandomDelay))
#define IDailyTrigger_put_RandomDelay(This,randomDelay)  ((This)->lpVtbl->put_RandomDelay(This,randomDelay))
#endif

#endif

#endif

#ifndef __IWeeklyTrigger_INTERFACE_DEFINED__
#define __IWeeklyTrigger_INTERFACE_DEFINED__

EXTERN_C const IID IID_IWeeklyTrigger;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5038fc98-82ff-436d-8728-a512a57c9dc1")IWeeklyTrigger:public ITrigger
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_DaysOfWeek(short *pDays) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DaysOfWeek(short days) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WeeksInterval(short *pWeeks) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_WeeksInterval(short weeks) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RandomDelay(BSTR *pRandomDelay) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RandomDelay(BSTR randomDelay) = 0;
};
#else
typedef struct IWeeklyTriggerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IWeeklyTrigger * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IWeeklyTrigger * This);
    ULONG(STDMETHODCALLTYPE *Release)(IWeeklyTrigger * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IWeeklyTrigger * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IWeeklyTrigger * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IWeeklyTrigger * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IWeeklyTrigger * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IWeeklyTrigger * This, TASK_TRIGGER_TYPE2 * pType);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IWeeklyTrigger * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IWeeklyTrigger * This, BSTR id);
    HRESULT (STDMETHODCALLTYPE *get_Repetition)(IWeeklyTrigger * This, IRepetitionPattern ** ppRepeat);
    HRESULT (STDMETHODCALLTYPE *put_Repetition)(IWeeklyTrigger * This, IRepetitionPattern * pRepeat);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(IWeeklyTrigger * This, BSTR * pTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(IWeeklyTrigger * This, BSTR timelimit);
    HRESULT (STDMETHODCALLTYPE *get_StartBoundary)(IWeeklyTrigger * This, BSTR * pStart);
    HRESULT (STDMETHODCALLTYPE *put_StartBoundary)(IWeeklyTrigger * This, BSTR start);
    HRESULT (STDMETHODCALLTYPE *get_EndBoundary)(IWeeklyTrigger * This, BSTR * pEnd);
    HRESULT (STDMETHODCALLTYPE *put_EndBoundary)(IWeeklyTrigger * This, BSTR end);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(IWeeklyTrigger * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(IWeeklyTrigger * This, VARIANT_BOOL enabled);
    HRESULT (STDMETHODCALLTYPE *get_DaysOfWeek)(IWeeklyTrigger * This, short *pDays);
    HRESULT (STDMETHODCALLTYPE *put_DaysOfWeek)(IWeeklyTrigger * This, short days);
    HRESULT (STDMETHODCALLTYPE *get_WeeksInterval)(IWeeklyTrigger * This, short *pWeeks);
    HRESULT (STDMETHODCALLTYPE *put_WeeksInterval)(IWeeklyTrigger * This, short weeks);
    HRESULT (STDMETHODCALLTYPE *get_RandomDelay)(IWeeklyTrigger * This, BSTR * pRandomDelay);
    HRESULT (STDMETHODCALLTYPE *put_RandomDelay)(IWeeklyTrigger * This, BSTR randomDelay);
    END_INTERFACE
} IWeeklyTriggerVtbl;

interface IWeeklyTrigger {
    CONST_VTBL struct IWeeklyTriggerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IWeeklyTrigger_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IWeeklyTrigger_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IWeeklyTrigger_Release(This)  ((This)->lpVtbl->Release(This))
#define IWeeklyTrigger_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IWeeklyTrigger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IWeeklyTrigger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IWeeklyTrigger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IWeeklyTrigger_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define IWeeklyTrigger_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IWeeklyTrigger_put_Id(This,id)  ((This)->lpVtbl->put_Id(This,id))
#define IWeeklyTrigger_get_Repetition(This,ppRepeat)  ((This)->lpVtbl->get_Repetition(This,ppRepeat))
#define IWeeklyTrigger_put_Repetition(This,pRepeat)  ((This)->lpVtbl->put_Repetition(This,pRepeat))
#define IWeeklyTrigger_get_ExecutionTimeLimit(This,pTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pTimeLimit))
#define IWeeklyTrigger_put_ExecutionTimeLimit(This,timelimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,timelimit))
#define IWeeklyTrigger_get_StartBoundary(This,pStart)  ((This)->lpVtbl->get_StartBoundary(This,pStart))
#define IWeeklyTrigger_put_StartBoundary(This,start)  ((This)->lpVtbl->put_StartBoundary(This,start))
#define IWeeklyTrigger_get_EndBoundary(This,pEnd)  ((This)->lpVtbl->get_EndBoundary(This,pEnd))
#define IWeeklyTrigger_put_EndBoundary(This,end)  ((This)->lpVtbl->put_EndBoundary(This,end))
#define IWeeklyTrigger_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define IWeeklyTrigger_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#define IWeeklyTrigger_get_DaysOfWeek(This,pDays)  ((This)->lpVtbl->get_DaysOfWeek(This,pDays))
#define IWeeklyTrigger_put_DaysOfWeek(This,days)  ((This)->lpVtbl->put_DaysOfWeek(This,days))
#define IWeeklyTrigger_get_WeeksInterval(This,pWeeks)  ((This)->lpVtbl->get_WeeksInterval(This,pWeeks))
#define IWeeklyTrigger_put_WeeksInterval(This,weeks)  ((This)->lpVtbl->put_WeeksInterval(This,weeks))
#define IWeeklyTrigger_get_RandomDelay(This,pRandomDelay)  ((This)->lpVtbl->get_RandomDelay(This,pRandomDelay))
#define IWeeklyTrigger_put_RandomDelay(This,randomDelay)  ((This)->lpVtbl->put_RandomDelay(This,randomDelay))
#endif

#endif

#endif

#ifndef __IMonthlyTrigger_INTERFACE_DEFINED__
#define __IMonthlyTrigger_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMonthlyTrigger;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("97c45ef1-6b02-4a1a-9c0e-1ebfba1500ac")IMonthlyTrigger:public ITrigger
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_DaysOfMonth(long *pDays) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DaysOfMonth(long days) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MonthsOfYear(short *pMonths) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MonthsOfYear(short months) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RunOnLastDayOfMonth(VARIANT_BOOL *pLastDay) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RunOnLastDayOfMonth(VARIANT_BOOL lastDay) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RandomDelay(BSTR *pRandomDelay) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RandomDelay(BSTR randomDelay) = 0;
};
#else
typedef struct IMonthlyTriggerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IMonthlyTrigger * This, REFIID riid, void **ppvObject);
    ULONG (STDMETHODCALLTYPE *AddRef)(IMonthlyTrigger * This);
    ULONG (STDMETHODCALLTYPE *Release)(IMonthlyTrigger * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IMonthlyTrigger * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IMonthlyTrigger * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IMonthlyTrigger * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IMonthlyTrigger * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IMonthlyTrigger * This, TASK_TRIGGER_TYPE2 * pType);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IMonthlyTrigger * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IMonthlyTrigger * This, BSTR id);
    HRESULT (STDMETHODCALLTYPE *get_Repetition)(IMonthlyTrigger * This, IRepetitionPattern ** ppRepeat);
    HRESULT (STDMETHODCALLTYPE *put_Repetition)(IMonthlyTrigger * This, IRepetitionPattern * pRepeat);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(IMonthlyTrigger * This, BSTR * pTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(IMonthlyTrigger * This, BSTR timelimit);
    HRESULT (STDMETHODCALLTYPE *get_StartBoundary)(IMonthlyTrigger * This, BSTR * pStart);
    HRESULT (STDMETHODCALLTYPE *put_StartBoundary)(IMonthlyTrigger * This, BSTR start);
    HRESULT (STDMETHODCALLTYPE *get_EndBoundary)(IMonthlyTrigger * This, BSTR * pEnd);
    HRESULT (STDMETHODCALLTYPE *put_EndBoundary)(IMonthlyTrigger * This, BSTR end);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(IMonthlyTrigger * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(IMonthlyTrigger * This, VARIANT_BOOL enabled);
    HRESULT (STDMETHODCALLTYPE *get_DaysOfMonth)(IMonthlyTrigger * This, long *pDays);
    HRESULT (STDMETHODCALLTYPE *put_DaysOfMonth)(IMonthlyTrigger * This, long days);
    HRESULT (STDMETHODCALLTYPE *get_MonthsOfYear)(IMonthlyTrigger * This, short *pMonths);
    HRESULT (STDMETHODCALLTYPE *put_MonthsOfYear)(IMonthlyTrigger * This, short months);
    HRESULT (STDMETHODCALLTYPE *get_RunOnLastDayOfMonth)(IMonthlyTrigger * This, VARIANT_BOOL * pLastDay);
    HRESULT (STDMETHODCALLTYPE *put_RunOnLastDayOfMonth)(IMonthlyTrigger * This, VARIANT_BOOL lastDay);
    HRESULT (STDMETHODCALLTYPE *get_RandomDelay)(IMonthlyTrigger * This, BSTR * pRandomDelay);
    HRESULT (STDMETHODCALLTYPE *put_RandomDelay)(IMonthlyTrigger * This, BSTR randomDelay);
    END_INTERFACE
} IMonthlyTriggerVtbl;

interface IMonthlyTrigger {
    CONST_VTBL struct IMonthlyTriggerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMonthlyTrigger_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMonthlyTrigger_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMonthlyTrigger_Release(This)  ((This)->lpVtbl->Release(This))
#define IMonthlyTrigger_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IMonthlyTrigger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IMonthlyTrigger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IMonthlyTrigger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IMonthlyTrigger_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define IMonthlyTrigger_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IMonthlyTrigger_put_Id(This,id)  ((This)->lpVtbl->put_Id(This,id))
#define IMonthlyTrigger_get_Repetition(This,ppRepeat)  ((This)->lpVtbl->get_Repetition(This,ppRepeat))
#define IMonthlyTrigger_put_Repetition(This,pRepeat)  ((This)->lpVtbl->put_Repetition(This,pRepeat))
#define IMonthlyTrigger_get_ExecutionTimeLimit(This,pTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pTimeLimit))
#define IMonthlyTrigger_put_ExecutionTimeLimit(This,timelimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,timelimit))
#define IMonthlyTrigger_get_StartBoundary(This,pStart)  ((This)->lpVtbl->get_StartBoundary(This,pStart))
#define IMonthlyTrigger_put_StartBoundary(This,start)  ((This)->lpVtbl->put_StartBoundary(This,start))
#define IMonthlyTrigger_get_EndBoundary(This,pEnd)  ((This)->lpVtbl->get_EndBoundary(This,pEnd))
#define IMonthlyTrigger_put_EndBoundary(This,end)  ((This)->lpVtbl->put_EndBoundary(This,end))
#define IMonthlyTrigger_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define IMonthlyTrigger_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#define IMonthlyTrigger_get_DaysOfMonth(This,pDays)  ((This)->lpVtbl->get_DaysOfMonth(This,pDays))
#define IMonthlyTrigger_put_DaysOfMonth(This,days)  ((This)->lpVtbl->put_DaysOfMonth(This,days))
#define IMonthlyTrigger_get_MonthsOfYear(This,pMonths)  ((This)->lpVtbl->get_MonthsOfYear(This,pMonths))
#define IMonthlyTrigger_put_MonthsOfYear(This,months)  ((This)->lpVtbl->put_MonthsOfYear(This,months))
#define IMonthlyTrigger_get_RunOnLastDayOfMonth(This,pLastDay)  ((This)->lpVtbl->get_RunOnLastDayOfMonth(This,pLastDay))
#define IMonthlyTrigger_put_RunOnLastDayOfMonth(This,lastDay)  ((This)->lpVtbl->put_RunOnLastDayOfMonth(This,lastDay))
#define IMonthlyTrigger_get_RandomDelay(This,pRandomDelay)  ((This)->lpVtbl->get_RandomDelay(This,pRandomDelay))
#define IMonthlyTrigger_put_RandomDelay(This,randomDelay)  ((This)->lpVtbl->put_RandomDelay(This,randomDelay))
#endif

#endif

#endif

#ifndef __IMonthlyDOWTrigger_INTERFACE_DEFINED__
#define __IMonthlyDOWTrigger_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMonthlyDOWTrigger;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("77d025a3-90fa-43aa-b52e-cda5499b946a")IMonthlyDOWTrigger:public ITrigger
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_DaysOfWeek(short *pDays) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DaysOfWeek(short days) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WeeksOfMonth(short *pWeeks) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_WeeksOfMonth(short weeks) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MonthsOfYear(short *pMonths) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MonthsOfYear(short months) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RunOnLastWeekOfMonth(VARIANT_BOOL *pLastWeek) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RunOnLastWeekOfMonth(VARIANT_BOOL lastWeek) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RandomDelay(BSTR *pRandomDelay) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RandomDelay(BSTR randomDelay) = 0;
};
#else
typedef struct IMonthlyDOWTriggerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IMonthlyDOWTrigger * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IMonthlyDOWTrigger * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMonthlyDOWTrigger * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IMonthlyDOWTrigger * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IMonthlyDOWTrigger * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IMonthlyDOWTrigger * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IMonthlyDOWTrigger * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IMonthlyDOWTrigger * This, TASK_TRIGGER_TYPE2 * pType);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IMonthlyDOWTrigger * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IMonthlyDOWTrigger * This, BSTR id);
    HRESULT (STDMETHODCALLTYPE *get_Repetition)(IMonthlyDOWTrigger * This, IRepetitionPattern ** ppRepeat);
    HRESULT (STDMETHODCALLTYPE *put_Repetition)(IMonthlyDOWTrigger * This, IRepetitionPattern * pRepeat);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(IMonthlyDOWTrigger * This, BSTR * pTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(IMonthlyDOWTrigger * This, BSTR timelimit);
    HRESULT (STDMETHODCALLTYPE *get_StartBoundary)(IMonthlyDOWTrigger * This, BSTR * pStart);
    HRESULT (STDMETHODCALLTYPE *put_StartBoundary)(IMonthlyDOWTrigger * This, BSTR start);
    HRESULT (STDMETHODCALLTYPE *get_EndBoundary)(IMonthlyDOWTrigger * This, BSTR * pEnd);
    HRESULT (STDMETHODCALLTYPE *put_EndBoundary)(IMonthlyDOWTrigger * This, BSTR end);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(IMonthlyDOWTrigger * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(IMonthlyDOWTrigger * This, VARIANT_BOOL enabled);
    HRESULT (STDMETHODCALLTYPE *get_DaysOfWeek)(IMonthlyDOWTrigger * This, short *pDays);
    HRESULT (STDMETHODCALLTYPE *put_DaysOfWeek)(IMonthlyDOWTrigger * This, short days);
    HRESULT (STDMETHODCALLTYPE *get_WeeksOfMonth)(IMonthlyDOWTrigger * This, short *pWeeks);
    HRESULT (STDMETHODCALLTYPE *put_WeeksOfMonth)(IMonthlyDOWTrigger * This, short weeks);
    HRESULT (STDMETHODCALLTYPE *get_MonthsOfYear)(IMonthlyDOWTrigger * This, short *pMonths);
    HRESULT (STDMETHODCALLTYPE *put_MonthsOfYear)(IMonthlyDOWTrigger * This, short months);
    HRESULT (STDMETHODCALLTYPE *get_RunOnLastWeekOfMonth)(IMonthlyDOWTrigger * This, VARIANT_BOOL * pLastWeek);
    HRESULT (STDMETHODCALLTYPE *put_RunOnLastWeekOfMonth)(IMonthlyDOWTrigger * This, VARIANT_BOOL lastWeek);
    HRESULT (STDMETHODCALLTYPE *get_RandomDelay)(IMonthlyDOWTrigger * This, BSTR * pRandomDelay);
    HRESULT (STDMETHODCALLTYPE *put_RandomDelay)(IMonthlyDOWTrigger * This, BSTR randomDelay);
    END_INTERFACE
} IMonthlyDOWTriggerVtbl;

interface IMonthlyDOWTrigger {
    CONST_VTBL struct IMonthlyDOWTriggerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMonthlyDOWTrigger_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMonthlyDOWTrigger_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMonthlyDOWTrigger_Release(This)  ((This)->lpVtbl->Release(This))
#define IMonthlyDOWTrigger_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IMonthlyDOWTrigger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IMonthlyDOWTrigger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IMonthlyDOWTrigger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IMonthlyDOWTrigger_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define IMonthlyDOWTrigger_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IMonthlyDOWTrigger_put_Id(This,id)  ((This)->lpVtbl->put_Id(This,id))
#define IMonthlyDOWTrigger_get_Repetition(This,ppRepeat)  ((This)->lpVtbl->get_Repetition(This,ppRepeat))
#define IMonthlyDOWTrigger_put_Repetition(This,pRepeat)  ((This)->lpVtbl->put_Repetition(This,pRepeat))
#define IMonthlyDOWTrigger_get_ExecutionTimeLimit(This,pTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pTimeLimit))
#define IMonthlyDOWTrigger_put_ExecutionTimeLimit(This,timelimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,timelimit))
#define IMonthlyDOWTrigger_get_StartBoundary(This,pStart)  ((This)->lpVtbl->get_StartBoundary(This,pStart))
#define IMonthlyDOWTrigger_put_StartBoundary(This,start)  ((This)->lpVtbl->put_StartBoundary(This,start))
#define IMonthlyDOWTrigger_get_EndBoundary(This,pEnd)  ((This)->lpVtbl->get_EndBoundary(This,pEnd))
#define IMonthlyDOWTrigger_put_EndBoundary(This,end)  ((This)->lpVtbl->put_EndBoundary(This,end))
#define IMonthlyDOWTrigger_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define IMonthlyDOWTrigger_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#define IMonthlyDOWTrigger_get_DaysOfWeek(This,pDays)  ((This)->lpVtbl->get_DaysOfWeek(This,pDays))
#define IMonthlyDOWTrigger_put_DaysOfWeek(This,days)  ((This)->lpVtbl->put_DaysOfWeek(This,days))
#define IMonthlyDOWTrigger_get_WeeksOfMonth(This,pWeeks)  ((This)->lpVtbl->get_WeeksOfMonth(This,pWeeks))
#define IMonthlyDOWTrigger_put_WeeksOfMonth(This,weeks)  ((This)->lpVtbl->put_WeeksOfMonth(This,weeks))
#define IMonthlyDOWTrigger_get_MonthsOfYear(This,pMonths)  ((This)->lpVtbl->get_MonthsOfYear(This,pMonths))
#define IMonthlyDOWTrigger_put_MonthsOfYear(This,months)  ((This)->lpVtbl->put_MonthsOfYear(This,months))
#define IMonthlyDOWTrigger_get_RunOnLastWeekOfMonth(This,pLastWeek)  ((This)->lpVtbl->get_RunOnLastWeekOfMonth(This,pLastWeek))
#define IMonthlyDOWTrigger_put_RunOnLastWeekOfMonth(This,lastWeek)  ((This)->lpVtbl->put_RunOnLastWeekOfMonth(This,lastWeek))
#define IMonthlyDOWTrigger_get_RandomDelay(This,pRandomDelay)  ((This)->lpVtbl->get_RandomDelay(This,pRandomDelay))
#define IMonthlyDOWTrigger_put_RandomDelay(This,randomDelay)  ((This)->lpVtbl->put_RandomDelay(This,randomDelay))
#endif

#endif

#endif

#ifndef __IBootTrigger_INTERFACE_DEFINED__
#define __IBootTrigger_INTERFACE_DEFINED__

EXTERN_C const IID IID_IBootTrigger;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("2A9C35DA-D357-41f4-BBC1-207AC1B1F3CB")IBootTrigger:public ITrigger
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Delay(BSTR * pDelay) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Delay(BSTR delay) = 0;
};
#else
typedef struct IBootTriggerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IBootTrigger * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IBootTrigger * This);
    ULONG(STDMETHODCALLTYPE *Release)(IBootTrigger * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IBootTrigger * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IBootTrigger * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IBootTrigger * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IBootTrigger * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IBootTrigger * This, TASK_TRIGGER_TYPE2 * pType);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IBootTrigger * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IBootTrigger * This, BSTR id);
    HRESULT (STDMETHODCALLTYPE *get_Repetition)(IBootTrigger * This, IRepetitionPattern ** ppRepeat);
    HRESULT (STDMETHODCALLTYPE *put_Repetition)(IBootTrigger * This, IRepetitionPattern * pRepeat);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(IBootTrigger * This, BSTR * pTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(IBootTrigger * This, BSTR timelimit);
    HRESULT (STDMETHODCALLTYPE *get_StartBoundary)(IBootTrigger * This, BSTR * pStart);
    HRESULT (STDMETHODCALLTYPE *put_StartBoundary)(IBootTrigger * This, BSTR start);
    HRESULT (STDMETHODCALLTYPE *get_EndBoundary)(IBootTrigger * This, BSTR * pEnd);
    HRESULT (STDMETHODCALLTYPE *put_EndBoundary)(IBootTrigger * This, BSTR end);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(IBootTrigger * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(IBootTrigger * This, VARIANT_BOOL enabled);
    HRESULT (STDMETHODCALLTYPE *get_Delay)(IBootTrigger * This, BSTR * pDelay);
    HRESULT (STDMETHODCALLTYPE *put_Delay)(IBootTrigger * This, BSTR delay);
    END_INTERFACE
} IBootTriggerVtbl;

interface IBootTrigger {
    CONST_VTBL struct IBootTriggerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IBootTrigger_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IBootTrigger_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IBootTrigger_Release(This)  ((This)->lpVtbl->Release(This))
#define IBootTrigger_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IBootTrigger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IBootTrigger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IBootTrigger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IBootTrigger_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define IBootTrigger_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IBootTrigger_put_Id(This,id)  ((This)->lpVtbl->put_Id(This,id))
#define IBootTrigger_get_Repetition(This,ppRepeat)  ((This)->lpVtbl->get_Repetition(This,ppRepeat))
#define IBootTrigger_put_Repetition(This,pRepeat)  ((This)->lpVtbl->put_Repetition(This,pRepeat))
#define IBootTrigger_get_ExecutionTimeLimit(This,pTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pTimeLimit))
#define IBootTrigger_put_ExecutionTimeLimit(This,timelimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,timelimit))
#define IBootTrigger_get_StartBoundary(This,pStart)  ((This)->lpVtbl->get_StartBoundary(This,pStart))
#define IBootTrigger_put_StartBoundary(This,start)  ((This)->lpVtbl->put_StartBoundary(This,start))
#define IBootTrigger_get_EndBoundary(This,pEnd)  ((This)->lpVtbl->get_EndBoundary(This,pEnd))
#define IBootTrigger_put_EndBoundary(This,end)  ((This)->lpVtbl->put_EndBoundary(This,end))
#define IBootTrigger_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define IBootTrigger_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#define IBootTrigger_get_Delay(This,pDelay)  ((This)->lpVtbl->get_Delay(This,pDelay))
#define IBootTrigger_put_Delay(This,delay)  ((This)->lpVtbl->put_Delay(This,delay))
#endif

#endif

#endif

#ifndef __IRegistrationTrigger_INTERFACE_DEFINED__
#define __IRegistrationTrigger_INTERFACE_DEFINED__

EXTERN_C const IID IID_IRegistrationTrigger;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("4c8fec3a-c218-4e0c-b23d-629024db91a2")IRegistrationTrigger:public ITrigger
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Delay(BSTR * pDelay) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Delay(BSTR delay) = 0;
};
#else
typedef struct IRegistrationTriggerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IRegistrationTrigger * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IRegistrationTrigger * This);
    ULONG(STDMETHODCALLTYPE *Release)(IRegistrationTrigger * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IRegistrationTrigger * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IRegistrationTrigger * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IRegistrationTrigger * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IRegistrationTrigger * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IRegistrationTrigger * This, TASK_TRIGGER_TYPE2 * pType);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IRegistrationTrigger * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IRegistrationTrigger * This, BSTR id);
    HRESULT (STDMETHODCALLTYPE *get_Repetition)(IRegistrationTrigger * This, IRepetitionPattern ** ppRepeat);
    HRESULT (STDMETHODCALLTYPE *put_Repetition)(IRegistrationTrigger * This, IRepetitionPattern * pRepeat);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(IRegistrationTrigger * This, BSTR * pTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(IRegistrationTrigger * This, BSTR timelimit);
    HRESULT (STDMETHODCALLTYPE *get_StartBoundary)(IRegistrationTrigger * This, BSTR * pStart);
    HRESULT (STDMETHODCALLTYPE *put_StartBoundary)(IRegistrationTrigger * This, BSTR start);
    HRESULT (STDMETHODCALLTYPE *get_EndBoundary)(IRegistrationTrigger * This, BSTR * pEnd);
    HRESULT (STDMETHODCALLTYPE *put_EndBoundary)(IRegistrationTrigger * This, BSTR end);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(IRegistrationTrigger * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(IRegistrationTrigger * This, VARIANT_BOOL enabled);
    HRESULT (STDMETHODCALLTYPE *get_Delay)(IRegistrationTrigger * This, BSTR * pDelay);
    HRESULT (STDMETHODCALLTYPE *put_Delay)(IRegistrationTrigger * This, BSTR delay);
    END_INTERFACE
} IRegistrationTriggerVtbl;

interface IRegistrationTrigger {
    CONST_VTBL struct IRegistrationTriggerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IRegistrationTrigger_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IRegistrationTrigger_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IRegistrationTrigger_Release(This)  ((This)->lpVtbl->Release(This))
#define IRegistrationTrigger_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IRegistrationTrigger_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IRegistrationTrigger_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IRegistrationTrigger_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IRegistrationTrigger_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define IRegistrationTrigger_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IRegistrationTrigger_put_Id(This,id)  ((This)->lpVtbl->put_Id(This,id))
#define IRegistrationTrigger_get_Repetition(This,ppRepeat)  ((This)->lpVtbl->get_Repetition(This,ppRepeat))
#define IRegistrationTrigger_put_Repetition(This,pRepeat)  ((This)->lpVtbl->put_Repetition(This,pRepeat))
#define IRegistrationTrigger_get_ExecutionTimeLimit(This,pTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pTimeLimit))
#define IRegistrationTrigger_put_ExecutionTimeLimit(This,timelimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,timelimit))
#define IRegistrationTrigger_get_StartBoundary(This,pStart)  ((This)->lpVtbl->get_StartBoundary(This,pStart))
#define IRegistrationTrigger_put_StartBoundary(This,start)  ((This)->lpVtbl->put_StartBoundary(This,start))
#define IRegistrationTrigger_get_EndBoundary(This,pEnd)  ((This)->lpVtbl->get_EndBoundary(This,pEnd))
#define IRegistrationTrigger_put_EndBoundary(This,end)  ((This)->lpVtbl->put_EndBoundary(This,end))
#define IRegistrationTrigger_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define IRegistrationTrigger_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#define IRegistrationTrigger_get_Delay(This,pDelay)  ((This)->lpVtbl->get_Delay(This,pDelay))
#define IRegistrationTrigger_put_Delay(This,delay)  ((This)->lpVtbl->put_Delay(This,delay))
#endif

#endif

#endif

#ifndef __IAction_INTERFACE_DEFINED__
#define __IAction_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAction;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("BAE54997-48B1-4cbe-9965-D6BE263EBEA4")IAction:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Id(BSTR * pId) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Id(BSTR Id) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Type(TASK_ACTION_TYPE *pType) = 0;
};
#else
typedef struct IActionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IAction * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IAction * This);
    ULONG(STDMETHODCALLTYPE *Release)(IAction * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IAction * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IAction * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IAction * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IAction * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IAction * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IAction * This, BSTR Id);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IAction * This, TASK_ACTION_TYPE * pType);
    END_INTERFACE
} IActionVtbl;

interface IAction {
    CONST_VTBL struct IActionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAction_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IAction_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IAction_Release(This)  ((This)->lpVtbl->Release(This))
#define IAction_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IAction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IAction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IAction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IAction_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IAction_put_Id(This,Id)  ((This)->lpVtbl->put_Id(This,Id))
#define IAction_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#endif

#endif

#endif

#ifndef __IExecAction_INTERFACE_DEFINED__
#define __IExecAction_INTERFACE_DEFINED__

EXTERN_C const IID IID_IExecAction;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("4c3d624d-fd6b-49a3-b9b7-09cb3cd3f047")IExecAction:public IAction
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Path(BSTR * pPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Path(BSTR path) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Arguments(BSTR *pArgument) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Arguments(BSTR argument) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WorkingDirectory(BSTR *pWorkingDirectory) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_WorkingDirectory(BSTR workingDirectory) = 0;
};
#else
typedef struct IExecActionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IExecAction * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IExecAction * This);
    ULONG(STDMETHODCALLTYPE *Release)(IExecAction * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IExecAction * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IExecAction * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IExecAction * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IExecAction * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IExecAction * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IExecAction * This, BSTR Id);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IExecAction * This, TASK_ACTION_TYPE * pType);
    HRESULT (STDMETHODCALLTYPE *get_Path)(IExecAction * This, BSTR * pPath);
    HRESULT (STDMETHODCALLTYPE *put_Path)(IExecAction * This, BSTR path);
    HRESULT (STDMETHODCALLTYPE *get_Arguments)(IExecAction * This, BSTR * pArgument);
    HRESULT (STDMETHODCALLTYPE *put_Arguments)(IExecAction * This, BSTR argument);
    HRESULT (STDMETHODCALLTYPE *get_WorkingDirectory)(IExecAction * This, BSTR * pWorkingDirectory);
    HRESULT (STDMETHODCALLTYPE *put_WorkingDirectory)(IExecAction * This, BSTR workingDirectory);
    END_INTERFACE
} IExecActionVtbl;

interface IExecAction {
    CONST_VTBL struct IExecActionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IExecAction_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IExecAction_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IExecAction_Release(This)  ((This)->lpVtbl->Release(This))
#define IExecAction_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IExecAction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IExecAction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IExecAction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IExecAction_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IExecAction_put_Id(This,Id)  ((This)->lpVtbl->put_Id(This,Id))
#define IExecAction_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define IExecAction_get_Path(This,pPath)  ((This)->lpVtbl->get_Path(This,pPath))
#define IExecAction_put_Path(This,path)  ((This)->lpVtbl->put_Path(This,path))
#define IExecAction_get_Arguments(This,pArgument)  ((This)->lpVtbl->get_Arguments(This,pArgument))
#define IExecAction_put_Arguments(This,argument)  ((This)->lpVtbl->put_Arguments(This,argument))
#define IExecAction_get_WorkingDirectory(This,pWorkingDirectory)  ((This)->lpVtbl->get_WorkingDirectory(This,pWorkingDirectory))
#define IExecAction_put_WorkingDirectory(This,workingDirectory)  ((This)->lpVtbl->put_WorkingDirectory(This,workingDirectory))
#endif

#endif

#endif

#ifndef __IExecAction2_INTERFACE_DEFINED__
#define __IExecAction2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IExecAction2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("f2a82542-bda5-4e6b-9143-e2bf4F8987b6")IExecAction2:public IExecAction
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_HideAppWindow(VARIANT_BOOL * pHideAppWindow) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HideAppWindow(VARIANT_BOOL hideAppWindow) = 0;
};
#else
typedef struct IExecAction2Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface)(IExecAction2 * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IExecAction2 * This);
    ULONG(STDMETHODCALLTYPE *Release)(IExecAction2 * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IExecAction2 * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IExecAction2 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IExecAction2 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IExecAction2 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IExecAction2 * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IExecAction2 * This, BSTR Id);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IExecAction2 * This, TASK_ACTION_TYPE * pType);
    HRESULT (STDMETHODCALLTYPE *get_Path)(IExecAction2 * This, BSTR * pPath);
    HRESULT (STDMETHODCALLTYPE *put_Path)(IExecAction2 * This, BSTR path);
    HRESULT (STDMETHODCALLTYPE *get_Arguments)(IExecAction2 * This, BSTR * pArgument);
    HRESULT (STDMETHODCALLTYPE *put_Arguments)(IExecAction2 * This, BSTR argument);
    HRESULT (STDMETHODCALLTYPE *get_WorkingDirectory)(IExecAction2 * This, BSTR * pWorkingDirectory);
    HRESULT (STDMETHODCALLTYPE *put_WorkingDirectory)(IExecAction2 * This, BSTR workingDirectory);
    HRESULT (STDMETHODCALLTYPE *get_HideAppWindow)(IExecAction2 * This, VARIANT_BOOL * pHideAppWindow);
    HRESULT (STDMETHODCALLTYPE *put_HideAppWindow)(IExecAction2 * This, VARIANT_BOOL hideAppWindow);
    END_INTERFACE
} IExecAction2Vtbl;

interface IExecAction2 {
    CONST_VTBL struct IExecAction2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IExecAction2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IExecAction2_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IExecAction2_Release(This)  ((This)->lpVtbl->Release(This))
#define IExecAction2_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IExecAction2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IExecAction2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IExecAction2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IExecAction2_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IExecAction2_put_Id(This,Id)  ((This)->lpVtbl->put_Id(This,Id))
#define IExecAction2_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define IExecAction2_get_Path(This,pPath)  ((This)->lpVtbl->get_Path(This,pPath))
#define IExecAction2_put_Path(This,path)  ((This)->lpVtbl->put_Path(This,path))
#define IExecAction2_get_Arguments(This,pArgument)  ((This)->lpVtbl->get_Arguments(This,pArgument))
#define IExecAction2_put_Arguments(This,argument)  ((This)->lpVtbl->put_Arguments(This,argument))
#define IExecAction2_get_WorkingDirectory(This,pWorkingDirectory)  ((This)->lpVtbl->get_WorkingDirectory(This,pWorkingDirectory))
#define IExecAction2_put_WorkingDirectory(This,workingDirectory)  ((This)->lpVtbl->put_WorkingDirectory(This,workingDirectory))
#define IExecAction2_get_HideAppWindow(This,pHideAppWindow)  ((This)->lpVtbl->get_HideAppWindow(This,pHideAppWindow))
#define IExecAction2_put_HideAppWindow(This,hideAppWindow)  ((This)->lpVtbl->put_HideAppWindow(This,hideAppWindow))
#endif

#endif

#endif

#ifndef __IShowMessageAction_INTERFACE_DEFINED__
#define __IShowMessageAction_INTERFACE_DEFINED__

EXTERN_C const IID IID_IShowMessageAction;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("505E9E68-AF89-46b8-A30F-56162A83D537")IShowMessageAction:public IAction
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Title(BSTR * pTitle) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Title(BSTR title) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MessageBody(BSTR *pMessageBody) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MessageBody(BSTR messageBody) = 0;
};
#else
typedef struct IShowMessageActionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IShowMessageAction * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IShowMessageAction * This);
    ULONG(STDMETHODCALLTYPE *Release)(IShowMessageAction * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IShowMessageAction * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IShowMessageAction * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IShowMessageAction * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IShowMessageAction * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IShowMessageAction * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IShowMessageAction * This, BSTR Id);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IShowMessageAction * This, TASK_ACTION_TYPE * pType);
    HRESULT (STDMETHODCALLTYPE *get_Title)(IShowMessageAction * This, BSTR * pTitle);
    HRESULT (STDMETHODCALLTYPE *put_Title)(IShowMessageAction * This, BSTR title);
    HRESULT (STDMETHODCALLTYPE *get_MessageBody)(IShowMessageAction * This, BSTR * pMessageBody);
    HRESULT (STDMETHODCALLTYPE *put_MessageBody)(IShowMessageAction * This, BSTR messageBody);
    END_INTERFACE
} IShowMessageActionVtbl;

interface IShowMessageAction {
    CONST_VTBL struct IShowMessageActionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IShowMessageAction_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IShowMessageAction_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IShowMessageAction_Release(This)  ((This)->lpVtbl->Release(This))
#define IShowMessageAction_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IShowMessageAction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IShowMessageAction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IShowMessageAction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IShowMessageAction_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IShowMessageAction_put_Id(This,Id)  ((This)->lpVtbl->put_Id(This,Id))
#define IShowMessageAction_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define IShowMessageAction_get_Title(This,pTitle)  ((This)->lpVtbl->get_Title(This,pTitle))
#define IShowMessageAction_put_Title(This,title)  ((This)->lpVtbl->put_Title(This,title))
#define IShowMessageAction_get_MessageBody(This,pMessageBody)  ((This)->lpVtbl->get_MessageBody(This,pMessageBody))
#define IShowMessageAction_put_MessageBody(This,messageBody)  ((This)->lpVtbl->put_MessageBody(This,messageBody))
#endif

#endif

#endif

#ifndef __IComHandlerAction_INTERFACE_DEFINED__
#define __IComHandlerAction_INTERFACE_DEFINED__

EXTERN_C const IID IID_IComHandlerAction;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("6D2FD252-75C5-4f66-90BA-2A7D8CC3039F")IComHandlerAction:public IAction
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_ClassId(BSTR * pClsid) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ClassId(BSTR clsid) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Data(BSTR *pData) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Data(BSTR data) = 0;
};
#else
typedef struct IComHandlerActionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IComHandlerAction * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IComHandlerAction * This);
    ULONG(STDMETHODCALLTYPE *Release)(IComHandlerAction * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IComHandlerAction * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IComHandlerAction * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IComHandlerAction * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IComHandlerAction * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IComHandlerAction * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IComHandlerAction * This, BSTR Id);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IComHandlerAction * This, TASK_ACTION_TYPE * pType);
    HRESULT (STDMETHODCALLTYPE *get_ClassId)(IComHandlerAction * This, BSTR * pClsid);
    HRESULT (STDMETHODCALLTYPE *put_ClassId)(IComHandlerAction * This, BSTR clsid);
    HRESULT (STDMETHODCALLTYPE *get_Data)(IComHandlerAction * This, BSTR * pData);
    HRESULT (STDMETHODCALLTYPE *put_Data)(IComHandlerAction * This, BSTR data);
    END_INTERFACE
} IComHandlerActionVtbl;

interface IComHandlerAction {
    CONST_VTBL struct IComHandlerActionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IComHandlerAction_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IComHandlerAction_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IComHandlerAction_Release(This)  ((This)->lpVtbl->Release(This))
#define IComHandlerAction_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IComHandlerAction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IComHandlerAction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IComHandlerAction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IComHandlerAction_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IComHandlerAction_put_Id(This,Id)  ((This)->lpVtbl->put_Id(This,Id))
#define IComHandlerAction_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define IComHandlerAction_get_ClassId(This,pClsid)  ((This)->lpVtbl->get_ClassId(This,pClsid))
#define IComHandlerAction_put_ClassId(This,clsid)  ((This)->lpVtbl->put_ClassId(This,clsid))
#define IComHandlerAction_get_Data(This,pData)  ((This)->lpVtbl->get_Data(This,pData))
#define IComHandlerAction_put_Data(This,data)  ((This)->lpVtbl->put_Data(This,data))
#endif

#endif

#endif

#ifndef __IEmailAction_INTERFACE_DEFINED__
#define __IEmailAction_INTERFACE_DEFINED__

EXTERN_C const IID IID_IEmailAction;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("10F62C64-7E16-4314-A0C2-0C3683F99D40")IEmailAction:public IAction
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Server(BSTR * pServer) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Server(BSTR server) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Subject(BSTR *pSubject) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Subject(BSTR subject) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_To(BSTR *pTo) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_To(BSTR to) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Cc(BSTR *pCc) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Cc(BSTR cc) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Bcc(BSTR *pBcc) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Bcc(BSTR bcc) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ReplyTo(BSTR *pReplyTo) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ReplyTo(BSTR replyTo) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_From(BSTR *pFrom) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_From(BSTR from) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_HeaderFields(ITaskNamedValueCollection **ppHeaderFields) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_HeaderFields(ITaskNamedValueCollection *pHeaderFields) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Body(BSTR *pBody) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Body(BSTR body) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Attachments(SAFEARRAY **pAttachements) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Attachments(SAFEARRAY *pAttachements) = 0;
};
#else
typedef struct IEmailActionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IEmailAction * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IEmailAction * This);
    ULONG(STDMETHODCALLTYPE *Release)(IEmailAction * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IEmailAction * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IEmailAction * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IEmailAction * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IEmailAction * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IEmailAction * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IEmailAction * This, BSTR Id);
    HRESULT (STDMETHODCALLTYPE *get_Type)(IEmailAction * This, TASK_ACTION_TYPE * pType);
    HRESULT (STDMETHODCALLTYPE *get_Server)(IEmailAction * This, BSTR * pServer);
    HRESULT (STDMETHODCALLTYPE *put_Server)(IEmailAction * This, BSTR server);
    HRESULT (STDMETHODCALLTYPE *get_Subject)(IEmailAction * This, BSTR * pSubject);
    HRESULT (STDMETHODCALLTYPE *put_Subject)(IEmailAction * This, BSTR subject);
    HRESULT (STDMETHODCALLTYPE *get_To)(IEmailAction * This, BSTR * pTo);
    HRESULT (STDMETHODCALLTYPE *put_To)(IEmailAction * This, BSTR to);
    HRESULT (STDMETHODCALLTYPE *get_Cc)(IEmailAction * This, BSTR * pCc);
    HRESULT (STDMETHODCALLTYPE *put_Cc)(IEmailAction * This, BSTR cc);
    HRESULT (STDMETHODCALLTYPE *get_Bcc)(IEmailAction * This, BSTR * pBcc);
    HRESULT (STDMETHODCALLTYPE *put_Bcc)(IEmailAction * This, BSTR bcc);
    HRESULT (STDMETHODCALLTYPE *get_ReplyTo)(IEmailAction * This, BSTR * pReplyTo);
    HRESULT (STDMETHODCALLTYPE *put_ReplyTo)(IEmailAction * This, BSTR replyTo);
    HRESULT (STDMETHODCALLTYPE *get_From)(IEmailAction * This, BSTR * pFrom);
    HRESULT (STDMETHODCALLTYPE *put_From)(IEmailAction * This, BSTR from);
    HRESULT (STDMETHODCALLTYPE *get_HeaderFields)(IEmailAction * This, ITaskNamedValueCollection ** ppHeaderFields);
    HRESULT (STDMETHODCALLTYPE *put_HeaderFields)(IEmailAction * This, ITaskNamedValueCollection * pHeaderFields);
    HRESULT (STDMETHODCALLTYPE *get_Body)(IEmailAction * This, BSTR * pBody);
    HRESULT (STDMETHODCALLTYPE *put_Body)(IEmailAction * This, BSTR body);
    HRESULT (STDMETHODCALLTYPE *get_Attachments)(IEmailAction * This, SAFEARRAY * *pAttachements);
    HRESULT (STDMETHODCALLTYPE *put_Attachments)(IEmailAction * This, SAFEARRAY * pAttachements);
    END_INTERFACE
} IEmailActionVtbl;

interface IEmailAction {
    CONST_VTBL struct IEmailActionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IEmailAction_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IEmailAction_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IEmailAction_Release(This)  ((This)->lpVtbl->Release(This))
#define IEmailAction_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IEmailAction_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IEmailAction_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IEmailAction_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IEmailAction_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IEmailAction_put_Id(This,Id)  ((This)->lpVtbl->put_Id(This,Id))
#define IEmailAction_get_Type(This,pType)  ((This)->lpVtbl->get_Type(This,pType))
#define IEmailAction_get_Server(This,pServer)  ((This)->lpVtbl->get_Server(This,pServer))
#define IEmailAction_put_Server(This,server)  ((This)->lpVtbl->put_Server(This,server))
#define IEmailAction_get_Subject(This,pSubject)  ((This)->lpVtbl->get_Subject(This,pSubject))
#define IEmailAction_put_Subject(This,subject)  ((This)->lpVtbl->put_Subject(This,subject))
#define IEmailAction_get_To(This,pTo)  ((This)->lpVtbl->get_To(This,pTo))
#define IEmailAction_put_To(This,to)  ((This)->lpVtbl->put_To(This,to))
#define IEmailAction_get_Cc(This,pCc)  ((This)->lpVtbl->get_Cc(This,pCc))
#define IEmailAction_put_Cc(This,cc)  ((This)->lpVtbl->put_Cc(This,cc))
#define IEmailAction_get_Bcc(This,pBcc)  ((This)->lpVtbl->get_Bcc(This,pBcc))
#define IEmailAction_put_Bcc(This,bcc)  ((This)->lpVtbl->put_Bcc(This,bcc))
#define IEmailAction_get_ReplyTo(This,pReplyTo)  ((This)->lpVtbl->get_ReplyTo(This,pReplyTo))
#define IEmailAction_put_ReplyTo(This,replyTo)  ((This)->lpVtbl->put_ReplyTo(This,replyTo))
#define IEmailAction_get_From(This,pFrom)  ((This)->lpVtbl->get_From(This,pFrom))
#define IEmailAction_put_From(This,from)  ((This)->lpVtbl->put_From(This,from))
#define IEmailAction_get_HeaderFields(This,ppHeaderFields)  ((This)->lpVtbl->get_HeaderFields(This,ppHeaderFields))
#define IEmailAction_put_HeaderFields(This,pHeaderFields)  ((This)->lpVtbl->put_HeaderFields(This,pHeaderFields))
#define IEmailAction_get_Body(This,pBody)  ((This)->lpVtbl->get_Body(This,pBody))
#define IEmailAction_put_Body(This,body)  ((This)->lpVtbl->put_Body(This,body))
#define IEmailAction_get_Attachments(This,pAttachements)  ((This)->lpVtbl->get_Attachments(This,pAttachements))
#define IEmailAction_put_Attachments(This,pAttachements)  ((This)->lpVtbl->put_Attachments(This,pAttachements))
#endif

#endif

#endif

#ifndef __ITriggerCollection_INTERFACE_DEFINED__
#define __ITriggerCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITriggerCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("85df5081-1b24-4f32-878a-d9d14df4cb77")ITriggerCollection:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Count(long *pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Item(long index, ITrigger **ppTrigger) = 0;
    virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown **ppEnum) = 0;
    virtual HRESULT STDMETHODCALLTYPE Create(TASK_TRIGGER_TYPE2 type, ITrigger **ppTrigger) = 0;
    virtual HRESULT STDMETHODCALLTYPE Remove(VARIANT index) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clear(void) = 0;
};
#else
typedef struct ITriggerCollectionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (ITriggerCollection * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITriggerCollection * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITriggerCollection * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ITriggerCollection * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ITriggerCollection * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ITriggerCollection * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ITriggerCollection * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Count)(ITriggerCollection * This, long *pCount);
    HRESULT (STDMETHODCALLTYPE *get_Item)(ITriggerCollection * This, long index, ITrigger ** ppTrigger);
    HRESULT (STDMETHODCALLTYPE *get__NewEnum)(ITriggerCollection * This, IUnknown ** ppEnum);
    HRESULT (STDMETHODCALLTYPE *Create)(ITriggerCollection * This, TASK_TRIGGER_TYPE2 type, ITrigger ** ppTrigger);
    HRESULT (STDMETHODCALLTYPE *Remove)(ITriggerCollection * This, VARIANT index);
    HRESULT (STDMETHODCALLTYPE *Clear)(ITriggerCollection * This);
    END_INTERFACE
} ITriggerCollectionVtbl;

interface ITriggerCollection {
    CONST_VTBL struct ITriggerCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITriggerCollection_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITriggerCollection_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITriggerCollection_Release(This)  ((This)->lpVtbl->Release(This))
#define ITriggerCollection_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ITriggerCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ITriggerCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ITriggerCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ITriggerCollection_get_Count(This,pCount)  ((This)->lpVtbl->get_Count(This,pCount))
#define ITriggerCollection_get_Item(This,index,ppTrigger)  ((This)->lpVtbl->get_Item(This,index,ppTrigger))
#define ITriggerCollection_get__NewEnum(This,ppEnum)  ((This)->lpVtbl->get__NewEnum(This,ppEnum))
#define ITriggerCollection_Create(This,type,ppTrigger)  ((This)->lpVtbl->Create(This,type,ppTrigger))
#define ITriggerCollection_Remove(This,index)  ((This)->lpVtbl->Remove(This,index))
#define ITriggerCollection_Clear(This)  ((This)->lpVtbl->Clear(This))
#endif

#endif

#endif

#ifndef __IActionCollection_INTERFACE_DEFINED__
#define __IActionCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_IActionCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("02820E19-7B98-4ed2-B2E8-FDCCCEFF619B")IActionCollection:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Count(long *pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Item(long index, IAction **ppAction) = 0;
    virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown **ppEnum) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_XmlText(BSTR *pText) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_XmlText(BSTR text) = 0;
    virtual HRESULT STDMETHODCALLTYPE Create(TASK_ACTION_TYPE type, IAction **ppAction) = 0;
    virtual HRESULT STDMETHODCALLTYPE Remove(VARIANT index) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clear(void) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Context(BSTR *pContext) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Context(BSTR context) = 0;
};
#else
typedef struct IActionCollectionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IActionCollection * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IActionCollection * This);
    ULONG(STDMETHODCALLTYPE *Release)(IActionCollection * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IActionCollection * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IActionCollection * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IActionCollection * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IActionCollection * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Count)(IActionCollection * This, long *pCount);
    HRESULT (STDMETHODCALLTYPE *get_Item)(IActionCollection * This, long index, IAction ** ppAction);
    HRESULT (STDMETHODCALLTYPE *get__NewEnum)(IActionCollection * This, IUnknown ** ppEnum);
    HRESULT (STDMETHODCALLTYPE *get_XmlText)(IActionCollection * This, BSTR * pText);
    HRESULT (STDMETHODCALLTYPE *put_XmlText)(IActionCollection * This, BSTR text);
    HRESULT (STDMETHODCALLTYPE *Create)(IActionCollection * This, TASK_ACTION_TYPE type, IAction ** ppAction);
    HRESULT (STDMETHODCALLTYPE *Remove)(IActionCollection * This, VARIANT index);
    HRESULT (STDMETHODCALLTYPE *Clear)(IActionCollection * This);
    HRESULT (STDMETHODCALLTYPE *get_Context)(IActionCollection * This, BSTR * pContext);
    HRESULT (STDMETHODCALLTYPE *put_Context)(IActionCollection * This, BSTR context);
    END_INTERFACE
} IActionCollectionVtbl;

interface IActionCollection {
    CONST_VTBL struct IActionCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IActionCollection_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IActionCollection_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IActionCollection_Release(This)  ((This)->lpVtbl->Release(This))
#define IActionCollection_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IActionCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IActionCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IActionCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IActionCollection_get_Count(This,pCount)  ((This)->lpVtbl->get_Count(This,pCount))
#define IActionCollection_get_Item(This,index,ppAction)  ((This)->lpVtbl->get_Item(This,index,ppAction))
#define IActionCollection_get__NewEnum(This,ppEnum)  ((This)->lpVtbl->get__NewEnum(This,ppEnum))
#define IActionCollection_get_XmlText(This,pText)  ((This)->lpVtbl->get_XmlText(This,pText))
#define IActionCollection_put_XmlText(This,text)  ((This)->lpVtbl->put_XmlText(This,text))
#define IActionCollection_Create(This,type,ppAction)  ((This)->lpVtbl->Create(This,type,ppAction))
#define IActionCollection_Remove(This,index)  ((This)->lpVtbl->Remove(This,index))
#define IActionCollection_Clear(This)  ((This)->lpVtbl->Clear(This))
#define IActionCollection_get_Context(This,pContext)  ((This)->lpVtbl->get_Context(This,pContext))
#define IActionCollection_put_Context(This,context)  ((This)->lpVtbl->put_Context(This,context))
#endif

#endif

#endif

#ifndef __IPrincipal_INTERFACE_DEFINED__
#define __IPrincipal_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPrincipal;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("D98D51E5-C9B4-496a-A9C1-18980261CF0F")IPrincipal:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Id(BSTR * pId) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Id(BSTR Id) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisplayName(BSTR *pName) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DisplayName(BSTR name) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UserId(BSTR *pUser) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_UserId(BSTR user) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_LogonType(TASK_LOGON_TYPE *pLogon) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_LogonType(TASK_LOGON_TYPE logon) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_GroupId(BSTR *pGroup) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_GroupId(BSTR group) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RunLevel(TASK_RUNLEVEL_TYPE *pRunLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RunLevel(TASK_RUNLEVEL_TYPE runLevel) = 0;
};
#else
typedef struct IPrincipalVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IPrincipal * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IPrincipal * This);
    ULONG(STDMETHODCALLTYPE *Release)(IPrincipal * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IPrincipal * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IPrincipal * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IPrincipal * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IPrincipal * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Id)(IPrincipal * This, BSTR * pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(IPrincipal * This, BSTR Id);
    HRESULT (STDMETHODCALLTYPE *get_DisplayName)(IPrincipal * This, BSTR * pName);
    HRESULT (STDMETHODCALLTYPE *put_DisplayName)(IPrincipal * This, BSTR name);
    HRESULT (STDMETHODCALLTYPE *get_UserId)(IPrincipal * This, BSTR * pUser);
    HRESULT (STDMETHODCALLTYPE *put_UserId)(IPrincipal * This, BSTR user);
    HRESULT (STDMETHODCALLTYPE *get_LogonType)(IPrincipal * This, TASK_LOGON_TYPE * pLogon);
    HRESULT (STDMETHODCALLTYPE *put_LogonType)(IPrincipal * This, TASK_LOGON_TYPE logon);
    HRESULT (STDMETHODCALLTYPE *get_GroupId)(IPrincipal * This, BSTR * pGroup);
    HRESULT (STDMETHODCALLTYPE *put_GroupId)(IPrincipal * This, BSTR group);
    HRESULT (STDMETHODCALLTYPE *get_RunLevel)(IPrincipal * This, TASK_RUNLEVEL_TYPE * pRunLevel);
    HRESULT (STDMETHODCALLTYPE *put_RunLevel)(IPrincipal * This, TASK_RUNLEVEL_TYPE runLevel);
    END_INTERFACE
} IPrincipalVtbl;

interface IPrincipal {
    CONST_VTBL struct IPrincipalVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPrincipal_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IPrincipal_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IPrincipal_Release(This)  ((This)->lpVtbl->Release(This))
#define IPrincipal_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IPrincipal_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IPrincipal_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IPrincipal_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IPrincipal_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define IPrincipal_put_Id(This,Id)  ((This)->lpVtbl->put_Id(This,Id))
#define IPrincipal_get_DisplayName(This,pName)  ((This)->lpVtbl->get_DisplayName(This,pName))
#define IPrincipal_put_DisplayName(This,name)  ((This)->lpVtbl->put_DisplayName(This,name))
#define IPrincipal_get_UserId(This,pUser)  ((This)->lpVtbl->get_UserId(This,pUser))
#define IPrincipal_put_UserId(This,user)  ((This)->lpVtbl->put_UserId(This,user))
#define IPrincipal_get_LogonType(This,pLogon)  ((This)->lpVtbl->get_LogonType(This,pLogon))
#define IPrincipal_put_LogonType(This,logon)  ((This)->lpVtbl->put_LogonType(This,logon))
#define IPrincipal_get_GroupId(This,pGroup)  ((This)->lpVtbl->get_GroupId(This,pGroup))
#define IPrincipal_put_GroupId(This,group)  ((This)->lpVtbl->put_GroupId(This,group))
#define IPrincipal_get_RunLevel(This,pRunLevel)  ((This)->lpVtbl->get_RunLevel(This,pRunLevel))
#define IPrincipal_put_RunLevel(This,runLevel)  ((This)->lpVtbl->put_RunLevel(This,runLevel))
#endif

#endif

#endif

#ifndef __IPrincipal2_INTERFACE_DEFINED__
#define __IPrincipal2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IPrincipal2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("248919AE-E345-4A6D-8AEB-E0D3165C904E")IPrincipal2:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_ProcessTokenSidType(TASK_PROCESSTOKENSID_TYPE * pProcessTokenSidType) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ProcessTokenSidType(TASK_PROCESSTOKENSID_TYPE processTokenSidType) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RequiredPrivilegeCount(long *pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RequiredPrivilege(long index, BSTR *pPrivilege) = 0;
    virtual HRESULT STDMETHODCALLTYPE AddRequiredPrivilege(BSTR privilege) = 0;
};
#else
typedef struct IPrincipal2Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IPrincipal2 * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(IPrincipal2 * This);
    ULONG(STDMETHODCALLTYPE *Release)(IPrincipal2 * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IPrincipal2 * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IPrincipal2 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IPrincipal2 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IPrincipal2 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_ProcessTokenSidType)(IPrincipal2 * This, TASK_PROCESSTOKENSID_TYPE * pProcessTokenSidType);
    HRESULT (STDMETHODCALLTYPE *put_ProcessTokenSidType)(IPrincipal2 * This, TASK_PROCESSTOKENSID_TYPE processTokenSidType);
    HRESULT (STDMETHODCALLTYPE *get_RequiredPrivilegeCount)(IPrincipal2 * This, long *pCount);
    HRESULT (STDMETHODCALLTYPE *get_RequiredPrivilege)(IPrincipal2 * This, long index, BSTR * pPrivilege);
    HRESULT (STDMETHODCALLTYPE *AddRequiredPrivilege)(IPrincipal2 * This, BSTR privilege);
    END_INTERFACE
} IPrincipal2Vtbl;

interface IPrincipal2 {
    CONST_VTBL struct IPrincipal2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IPrincipal2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IPrincipal2_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IPrincipal2_Release(This)  ((This)->lpVtbl->Release(This))
#define IPrincipal2_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IPrincipal2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IPrincipal2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IPrincipal2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IPrincipal2_get_ProcessTokenSidType(This,pProcessTokenSidType)  ((This)->lpVtbl->get_ProcessTokenSidType(This,pProcessTokenSidType))
#define IPrincipal2_put_ProcessTokenSidType(This,processTokenSidType)  ((This)->lpVtbl->put_ProcessTokenSidType(This,processTokenSidType))
#define IPrincipal2_get_RequiredPrivilegeCount(This,pCount)  ((This)->lpVtbl->get_RequiredPrivilegeCount(This,pCount))
#define IPrincipal2_get_RequiredPrivilege(This,index,pPrivilege)  ((This)->lpVtbl->get_RequiredPrivilege(This,index,pPrivilege))
#define IPrincipal2_AddRequiredPrivilege(This,privilege)  ((This)->lpVtbl->AddRequiredPrivilege(This,privilege))
#endif

#endif

#endif

#ifndef __IRegistrationInfo_INTERFACE_DEFINED__
#define __IRegistrationInfo_INTERFACE_DEFINED__

EXTERN_C const IID IID_IRegistrationInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("416D8B73-CB41-4ea1-805C-9BE9A5AC4A74")IRegistrationInfo:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Description(BSTR * pDescription) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Description(BSTR description) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Author(BSTR *pAuthor) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Author(BSTR author) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Version(BSTR *pVersion) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Version(BSTR version) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Date(BSTR *pDate) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Date(BSTR date) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Documentation(BSTR *pDocumentation) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Documentation(BSTR documentation) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_XmlText(BSTR *pText) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_XmlText(BSTR text) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_URI(BSTR *pUri) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_URI(BSTR uri) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_SecurityDescriptor(VARIANT *pSddl) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_SecurityDescriptor(VARIANT sddl) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Source(BSTR *pSource) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Source(BSTR source) = 0;
};
#else
typedef struct IRegistrationInfoVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IRegistrationInfo * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IRegistrationInfo * This);
    ULONG(STDMETHODCALLTYPE *Release)(IRegistrationInfo * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IRegistrationInfo * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IRegistrationInfo * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IRegistrationInfo * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IRegistrationInfo * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Description)(IRegistrationInfo * This, BSTR * pDescription);
    HRESULT (STDMETHODCALLTYPE *put_Description)(IRegistrationInfo * This, BSTR description);
    HRESULT (STDMETHODCALLTYPE *get_Author)(IRegistrationInfo * This, BSTR * pAuthor);
    HRESULT (STDMETHODCALLTYPE *put_Author)(IRegistrationInfo * This, BSTR author);
    HRESULT (STDMETHODCALLTYPE *get_Version)(IRegistrationInfo * This, BSTR * pVersion);
    HRESULT (STDMETHODCALLTYPE *put_Version)(IRegistrationInfo * This, BSTR version);
    HRESULT (STDMETHODCALLTYPE *get_Date)(IRegistrationInfo * This, BSTR * pDate);
    HRESULT (STDMETHODCALLTYPE *put_Date)(IRegistrationInfo * This, BSTR date);
    HRESULT (STDMETHODCALLTYPE *get_Documentation)(IRegistrationInfo * This, BSTR * pDocumentation);
    HRESULT (STDMETHODCALLTYPE *put_Documentation)(IRegistrationInfo * This, BSTR documentation);
    HRESULT (STDMETHODCALLTYPE *get_XmlText)(IRegistrationInfo * This, BSTR * pText);
    HRESULT (STDMETHODCALLTYPE *put_XmlText)(IRegistrationInfo * This, BSTR text);
    HRESULT (STDMETHODCALLTYPE *get_URI)(IRegistrationInfo * This, BSTR * pUri);
    HRESULT (STDMETHODCALLTYPE *put_URI)(IRegistrationInfo * This, BSTR uri);
    HRESULT (STDMETHODCALLTYPE *get_SecurityDescriptor)(IRegistrationInfo * This, VARIANT * pSddl);
    HRESULT (STDMETHODCALLTYPE *put_SecurityDescriptor)(IRegistrationInfo * This, VARIANT sddl);
    HRESULT (STDMETHODCALLTYPE *get_Source)(IRegistrationInfo * This, BSTR * pSource);
    HRESULT (STDMETHODCALLTYPE *put_Source)(IRegistrationInfo * This, BSTR source);
    END_INTERFACE
} IRegistrationInfoVtbl;

interface IRegistrationInfo {
    CONST_VTBL struct IRegistrationInfoVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IRegistrationInfo_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IRegistrationInfo_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IRegistrationInfo_Release(This)  ((This)->lpVtbl->Release(This))
#define IRegistrationInfo_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IRegistrationInfo_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IRegistrationInfo_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IRegistrationInfo_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IRegistrationInfo_get_Description(This,pDescription)  ((This)->lpVtbl->get_Description(This,pDescription))
#define IRegistrationInfo_put_Description(This,description)  ((This)->lpVtbl->put_Description(This,description))
#define IRegistrationInfo_get_Author(This,pAuthor)  ((This)->lpVtbl->get_Author(This,pAuthor))
#define IRegistrationInfo_put_Author(This,author)  ((This)->lpVtbl->put_Author(This,author))
#define IRegistrationInfo_get_Version(This,pVersion)  ((This)->lpVtbl->get_Version(This,pVersion))
#define IRegistrationInfo_put_Version(This,version)  ((This)->lpVtbl->put_Version(This,version))
#define IRegistrationInfo_get_Date(This,pDate)  ((This)->lpVtbl->get_Date(This,pDate))
#define IRegistrationInfo_put_Date(This,date)  ((This)->lpVtbl->put_Date(This,date))
#define IRegistrationInfo_get_Documentation(This,pDocumentation)  ((This)->lpVtbl->get_Documentation(This,pDocumentation))
#define IRegistrationInfo_put_Documentation(This,documentation)  ((This)->lpVtbl->put_Documentation(This,documentation))
#define IRegistrationInfo_get_XmlText(This,pText)  ((This)->lpVtbl->get_XmlText(This,pText))
#define IRegistrationInfo_put_XmlText(This,text)  ((This)->lpVtbl->put_XmlText(This,text))
#define IRegistrationInfo_get_URI(This,pUri)  ((This)->lpVtbl->get_URI(This,pUri))
#define IRegistrationInfo_put_URI(This,uri)  ((This)->lpVtbl->put_URI(This,uri))
#define IRegistrationInfo_get_SecurityDescriptor(This,pSddl)  ((This)->lpVtbl->get_SecurityDescriptor(This,pSddl))
#define IRegistrationInfo_put_SecurityDescriptor(This,sddl)  ((This)->lpVtbl->put_SecurityDescriptor(This,sddl))
#define IRegistrationInfo_get_Source(This,pSource)  ((This)->lpVtbl->get_Source(This,pSource))
#define IRegistrationInfo_put_Source(This,source)  ((This)->lpVtbl->put_Source(This,source))
#endif

#endif

#endif

#ifndef __ITaskDefinition_INTERFACE_DEFINED__
#define __ITaskDefinition_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITaskDefinition;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("f5bc8fc5-536d-4f77-b852-fbc1356fdeb6")ITaskDefinition:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_RegistrationInfo(IRegistrationInfo ** ppRegistrationInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RegistrationInfo(IRegistrationInfo *pRegistrationInfo) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Triggers(ITriggerCollection **ppTriggers) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Triggers(ITriggerCollection *pTriggers) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Settings(ITaskSettings **ppSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Settings(ITaskSettings *pSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Data(BSTR *pData) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Data(BSTR data) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Principal(IPrincipal **ppPrincipal) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Principal(IPrincipal *pPrincipal) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Actions(IActionCollection **ppActions) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Actions(IActionCollection *pActions) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_XmlText(BSTR *pXml) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_XmlText(BSTR xml) = 0;
};
#else
typedef struct ITaskDefinitionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (ITaskDefinition * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITaskDefinition * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITaskDefinition * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ITaskDefinition * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ITaskDefinition * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ITaskDefinition * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ITaskDefinition * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_RegistrationInfo)(ITaskDefinition * This, IRegistrationInfo ** ppRegistrationInfo);
    HRESULT (STDMETHODCALLTYPE *put_RegistrationInfo)(ITaskDefinition * This, IRegistrationInfo * pRegistrationInfo);
    HRESULT (STDMETHODCALLTYPE *get_Triggers)(ITaskDefinition * This, ITriggerCollection ** ppTriggers);
    HRESULT (STDMETHODCALLTYPE *put_Triggers)(ITaskDefinition * This, ITriggerCollection * pTriggers);
    HRESULT (STDMETHODCALLTYPE *get_Settings)(ITaskDefinition * This, ITaskSettings ** ppSettings);
    HRESULT (STDMETHODCALLTYPE *put_Settings)(ITaskDefinition * This, ITaskSettings * pSettings);
    HRESULT (STDMETHODCALLTYPE *get_Data)(ITaskDefinition * This, BSTR * pData);
    HRESULT (STDMETHODCALLTYPE *put_Data)(ITaskDefinition * This, BSTR data);
    HRESULT (STDMETHODCALLTYPE *get_Principal)(ITaskDefinition * This, IPrincipal ** ppPrincipal);
    HRESULT (STDMETHODCALLTYPE *put_Principal)(ITaskDefinition * This, IPrincipal * pPrincipal);
    HRESULT (STDMETHODCALLTYPE *get_Actions)(ITaskDefinition * This, IActionCollection ** ppActions);
    HRESULT (STDMETHODCALLTYPE *put_Actions)(ITaskDefinition * This, IActionCollection * pActions);
    HRESULT (STDMETHODCALLTYPE *get_XmlText)(ITaskDefinition * This, BSTR * pXml);
    HRESULT (STDMETHODCALLTYPE *put_XmlText)(ITaskDefinition * This, BSTR xml);
    END_INTERFACE
} ITaskDefinitionVtbl;

interface ITaskDefinition {
    CONST_VTBL struct ITaskDefinitionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITaskDefinition_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITaskDefinition_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITaskDefinition_Release(This)  ((This)->lpVtbl->Release(This))
#define ITaskDefinition_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ITaskDefinition_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ITaskDefinition_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ITaskDefinition_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ITaskDefinition_get_RegistrationInfo(This,ppRegistrationInfo)  ((This)->lpVtbl->get_RegistrationInfo(This,ppRegistrationInfo))
#define ITaskDefinition_put_RegistrationInfo(This,pRegistrationInfo)  ((This)->lpVtbl->put_RegistrationInfo(This,pRegistrationInfo))
#define ITaskDefinition_get_Triggers(This,ppTriggers)  ((This)->lpVtbl->get_Triggers(This,ppTriggers))
#define ITaskDefinition_put_Triggers(This,pTriggers)  ((This)->lpVtbl->put_Triggers(This,pTriggers))
#define ITaskDefinition_get_Settings(This,ppSettings)  ((This)->lpVtbl->get_Settings(This,ppSettings))
#define ITaskDefinition_put_Settings(This,pSettings)  ((This)->lpVtbl->put_Settings(This,pSettings))
#define ITaskDefinition_get_Data(This,pData)  ((This)->lpVtbl->get_Data(This,pData))
#define ITaskDefinition_put_Data(This,data)  ((This)->lpVtbl->put_Data(This,data))
#define ITaskDefinition_get_Principal(This,ppPrincipal)  ((This)->lpVtbl->get_Principal(This,ppPrincipal))
#define ITaskDefinition_put_Principal(This,pPrincipal)  ((This)->lpVtbl->put_Principal(This,pPrincipal))
#define ITaskDefinition_get_Actions(This,ppActions)  ((This)->lpVtbl->get_Actions(This,ppActions))
#define ITaskDefinition_put_Actions(This,pActions)  ((This)->lpVtbl->put_Actions(This,pActions))
#define ITaskDefinition_get_XmlText(This,pXml)  ((This)->lpVtbl->get_XmlText(This,pXml))
#define ITaskDefinition_put_XmlText(This,xml)  ((This)->lpVtbl->put_XmlText(This,xml))
#endif

#endif

#endif

#ifndef __ITaskSettings_INTERFACE_DEFINED__
#define __ITaskSettings_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITaskSettings;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("8FD4711D-2D02-4c8c-87E3-EFF699DE127E")ITaskSettings:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_AllowDemandStart(VARIANT_BOOL * pAllowDemandStart) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AllowDemandStart(VARIANT_BOOL allowDemandStart) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RestartInterval(BSTR *pRestartInterval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RestartInterval(BSTR restartInterval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RestartCount(int *pRestartCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RestartCount(int restartCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MultipleInstances(TASK_INSTANCES_POLICY *pPolicy) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MultipleInstances(TASK_INSTANCES_POLICY policy) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StopIfGoingOnBatteries(VARIANT_BOOL *pStopIfOnBatteries) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StopIfGoingOnBatteries(VARIANT_BOOL stopIfOnBatteries) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DisallowStartIfOnBatteries(VARIANT_BOOL *pDisallowStart) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DisallowStartIfOnBatteries(VARIANT_BOOL disallowStart) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_AllowHardTerminate(VARIANT_BOOL *pAllowHardTerminate) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_AllowHardTerminate(VARIANT_BOOL allowHardTerminate) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StartWhenAvailable(VARIANT_BOOL *pStartWhenAvailable) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StartWhenAvailable(VARIANT_BOOL startWhenAvailable) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_XmlText(BSTR *pText) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_XmlText(BSTR text) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RunOnlyIfNetworkAvailable(VARIANT_BOOL *pRunOnlyIfNetworkAvailable) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RunOnlyIfNetworkAvailable(VARIANT_BOOL runOnlyIfNetworkAvailable) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_ExecutionTimeLimit(BSTR *pExecutionTimeLimit) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_ExecutionTimeLimit(BSTR executionTimeLimit) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Enabled(VARIANT_BOOL *pEnabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Enabled(VARIANT_BOOL enabled) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_DeleteExpiredTaskAfter(BSTR *pExpirationDelay) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DeleteExpiredTaskAfter(BSTR expirationDelay) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Priority(int *pPriority) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Priority(int priority) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Compatibility(TASK_COMPATIBILITY *pCompatLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Compatibility(TASK_COMPATIBILITY compatLevel) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Hidden(VARIANT_BOOL *pHidden) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Hidden(VARIANT_BOOL hidden) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_IdleSettings(IIdleSettings **ppIdleSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IdleSettings(IIdleSettings *pIdleSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RunOnlyIfIdle(VARIANT_BOOL *pRunOnlyIfIdle) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RunOnlyIfIdle(VARIANT_BOOL runOnlyIfIdle) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WakeToRun(VARIANT_BOOL *pWake) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_WakeToRun(VARIANT_BOOL wake) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_NetworkSettings(INetworkSettings **ppNetworkSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_NetworkSettings(INetworkSettings *pNetworkSettings) = 0;
};
#else
typedef struct ITaskSettingsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (ITaskSettings * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(ITaskSettings * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITaskSettings * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ITaskSettings * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ITaskSettings * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ITaskSettings * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ITaskSettings * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_AllowDemandStart)(ITaskSettings * This, VARIANT_BOOL * pAllowDemandStart);
    HRESULT (STDMETHODCALLTYPE *put_AllowDemandStart)(ITaskSettings * This, VARIANT_BOOL allowDemandStart);
    HRESULT (STDMETHODCALLTYPE *get_RestartInterval)(ITaskSettings * This, BSTR * pRestartInterval);
    HRESULT (STDMETHODCALLTYPE *put_RestartInterval)(ITaskSettings * This, BSTR restartInterval);
    HRESULT (STDMETHODCALLTYPE *get_RestartCount)(ITaskSettings * This, int *pRestartCount);
    HRESULT (STDMETHODCALLTYPE *put_RestartCount)(ITaskSettings * This, int restartCount);
    HRESULT (STDMETHODCALLTYPE *get_MultipleInstances)(ITaskSettings * This, TASK_INSTANCES_POLICY * pPolicy);
    HRESULT (STDMETHODCALLTYPE *put_MultipleInstances)(ITaskSettings * This, TASK_INSTANCES_POLICY policy);
    HRESULT (STDMETHODCALLTYPE *get_StopIfGoingOnBatteries)(ITaskSettings * This, VARIANT_BOOL * pStopIfOnBatteries);
    HRESULT (STDMETHODCALLTYPE *put_StopIfGoingOnBatteries)(ITaskSettings * This, VARIANT_BOOL stopIfOnBatteries);
    HRESULT (STDMETHODCALLTYPE *get_DisallowStartIfOnBatteries)(ITaskSettings * This, VARIANT_BOOL * pDisallowStart);
    HRESULT (STDMETHODCALLTYPE *put_DisallowStartIfOnBatteries)(ITaskSettings * This, VARIANT_BOOL disallowStart);
    HRESULT (STDMETHODCALLTYPE *get_AllowHardTerminate)(ITaskSettings * This, VARIANT_BOOL * pAllowHardTerminate);
    HRESULT (STDMETHODCALLTYPE *put_AllowHardTerminate)(ITaskSettings * This, VARIANT_BOOL allowHardTerminate);
    HRESULT (STDMETHODCALLTYPE *get_StartWhenAvailable)(ITaskSettings * This, VARIANT_BOOL * pStartWhenAvailable);
    HRESULT (STDMETHODCALLTYPE *put_StartWhenAvailable)(ITaskSettings * This, VARIANT_BOOL startWhenAvailable);
    HRESULT (STDMETHODCALLTYPE *get_XmlText)(ITaskSettings * This, BSTR * pText);
    HRESULT (STDMETHODCALLTYPE *put_XmlText)(ITaskSettings * This, BSTR text);
    HRESULT (STDMETHODCALLTYPE *get_RunOnlyIfNetworkAvailable)(ITaskSettings * This, VARIANT_BOOL * pRunOnlyIfNetworkAvailable);
    HRESULT (STDMETHODCALLTYPE *put_RunOnlyIfNetworkAvailable)(ITaskSettings * This, VARIANT_BOOL runOnlyIfNetworkAvailable);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(ITaskSettings * This, BSTR * pExecutionTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(ITaskSettings * This, BSTR executionTimeLimit);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(ITaskSettings * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(ITaskSettings * This, VARIANT_BOOL enabled);
    HRESULT (STDMETHODCALLTYPE *get_DeleteExpiredTaskAfter)(ITaskSettings * This, BSTR * pExpirationDelay);
    HRESULT (STDMETHODCALLTYPE *put_DeleteExpiredTaskAfter)(ITaskSettings * This, BSTR expirationDelay);
    HRESULT (STDMETHODCALLTYPE *get_Priority)(ITaskSettings * This, int *pPriority);
    HRESULT (STDMETHODCALLTYPE *put_Priority)(ITaskSettings * This, int priority);
    HRESULT (STDMETHODCALLTYPE *get_Compatibility)(ITaskSettings * This, TASK_COMPATIBILITY * pCompatLevel);
    HRESULT (STDMETHODCALLTYPE *put_Compatibility)(ITaskSettings * This, TASK_COMPATIBILITY compatLevel);
    HRESULT (STDMETHODCALLTYPE *get_Hidden)(ITaskSettings * This, VARIANT_BOOL * pHidden);
    HRESULT (STDMETHODCALLTYPE *put_Hidden)(ITaskSettings * This, VARIANT_BOOL hidden);
    HRESULT (STDMETHODCALLTYPE *get_IdleSettings)(ITaskSettings * This, IIdleSettings ** ppIdleSettings);
    HRESULT (STDMETHODCALLTYPE *put_IdleSettings)(ITaskSettings * This, IIdleSettings * pIdleSettings);
    HRESULT (STDMETHODCALLTYPE *get_RunOnlyIfIdle)(ITaskSettings * This, VARIANT_BOOL * pRunOnlyIfIdle);
    HRESULT (STDMETHODCALLTYPE *put_RunOnlyIfIdle)(ITaskSettings * This, VARIANT_BOOL runOnlyIfIdle);
    HRESULT (STDMETHODCALLTYPE *get_WakeToRun)(ITaskSettings * This, VARIANT_BOOL * pWake);
    HRESULT (STDMETHODCALLTYPE *put_WakeToRun)(ITaskSettings * This, VARIANT_BOOL wake);
    HRESULT (STDMETHODCALLTYPE *get_NetworkSettings)(ITaskSettings * This, INetworkSettings ** ppNetworkSettings);
    HRESULT (STDMETHODCALLTYPE *put_NetworkSettings)(ITaskSettings * This, INetworkSettings * pNetworkSettings);
    END_INTERFACE
} ITaskSettingsVtbl;

interface ITaskSettings {
    CONST_VTBL struct ITaskSettingsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITaskSettings_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITaskSettings_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITaskSettings_Release(This)  ((This)->lpVtbl->Release(This))
#define ITaskSettings_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ITaskSettings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ITaskSettings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ITaskSettings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ITaskSettings_get_AllowDemandStart(This,pAllowDemandStart)  ((This)->lpVtbl->get_AllowDemandStart(This,pAllowDemandStart))
#define ITaskSettings_put_AllowDemandStart(This,allowDemandStart)  ((This)->lpVtbl->put_AllowDemandStart(This,allowDemandStart))
#define ITaskSettings_get_RestartInterval(This,pRestartInterval)  ((This)->lpVtbl->get_RestartInterval(This,pRestartInterval))
#define ITaskSettings_put_RestartInterval(This,restartInterval)  ((This)->lpVtbl->put_RestartInterval(This,restartInterval))
#define ITaskSettings_get_RestartCount(This,pRestartCount)  ((This)->lpVtbl->get_RestartCount(This,pRestartCount))
#define ITaskSettings_put_RestartCount(This,restartCount)  ((This)->lpVtbl->put_RestartCount(This,restartCount))
#define ITaskSettings_get_MultipleInstances(This,pPolicy)  ((This)->lpVtbl->get_MultipleInstances(This,pPolicy))
#define ITaskSettings_put_MultipleInstances(This,policy)  ((This)->lpVtbl->put_MultipleInstances(This,policy))
#define ITaskSettings_get_StopIfGoingOnBatteries(This,pStopIfOnBatteries)  ((This)->lpVtbl->get_StopIfGoingOnBatteries(This,pStopIfOnBatteries))
#define ITaskSettings_put_StopIfGoingOnBatteries(This,stopIfOnBatteries)  ((This)->lpVtbl->put_StopIfGoingOnBatteries(This,stopIfOnBatteries))
#define ITaskSettings_get_DisallowStartIfOnBatteries(This,pDisallowStart)  ((This)->lpVtbl->get_DisallowStartIfOnBatteries(This,pDisallowStart))
#define ITaskSettings_put_DisallowStartIfOnBatteries(This,disallowStart)  ((This)->lpVtbl->put_DisallowStartIfOnBatteries(This,disallowStart))
#define ITaskSettings_get_AllowHardTerminate(This,pAllowHardTerminate)  ((This)->lpVtbl->get_AllowHardTerminate(This,pAllowHardTerminate))
#define ITaskSettings_put_AllowHardTerminate(This,allowHardTerminate)  ((This)->lpVtbl->put_AllowHardTerminate(This,allowHardTerminate))
#define ITaskSettings_get_StartWhenAvailable(This,pStartWhenAvailable)  ((This)->lpVtbl->get_StartWhenAvailable(This,pStartWhenAvailable))
#define ITaskSettings_put_StartWhenAvailable(This,startWhenAvailable)  ((This)->lpVtbl->put_StartWhenAvailable(This,startWhenAvailable))
#define ITaskSettings_get_XmlText(This,pText)  ((This)->lpVtbl->get_XmlText(This,pText))
#define ITaskSettings_put_XmlText(This,text)  ((This)->lpVtbl->put_XmlText(This,text))
#define ITaskSettings_get_RunOnlyIfNetworkAvailable(This,pRunOnlyIfNetworkAvailable)  ((This)->lpVtbl->get_RunOnlyIfNetworkAvailable(This,pRunOnlyIfNetworkAvailable))
#define ITaskSettings_put_RunOnlyIfNetworkAvailable(This,runOnlyIfNetworkAvailable)  ((This)->lpVtbl->put_RunOnlyIfNetworkAvailable(This,runOnlyIfNetworkAvailable))
#define ITaskSettings_get_ExecutionTimeLimit(This,pExecutionTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pExecutionTimeLimit))
#define ITaskSettings_put_ExecutionTimeLimit(This,executionTimeLimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,executionTimeLimit))
#define ITaskSettings_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define ITaskSettings_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#define ITaskSettings_get_DeleteExpiredTaskAfter(This,pExpirationDelay)  ((This)->lpVtbl->get_DeleteExpiredTaskAfter(This,pExpirationDelay))
#define ITaskSettings_put_DeleteExpiredTaskAfter(This,expirationDelay)  ((This)->lpVtbl->put_DeleteExpiredTaskAfter(This,expirationDelay))
#define ITaskSettings_get_Priority(This,pPriority)  ((This)->lpVtbl->get_Priority(This,pPriority))
#define ITaskSettings_put_Priority(This,priority)  ((This)->lpVtbl->put_Priority(This,priority))
#define ITaskSettings_get_Compatibility(This,pCompatLevel)  ((This)->lpVtbl->get_Compatibility(This,pCompatLevel))
#define ITaskSettings_put_Compatibility(This,compatLevel)  ((This)->lpVtbl->put_Compatibility(This,compatLevel))
#define ITaskSettings_get_Hidden(This,pHidden)  ((This)->lpVtbl->get_Hidden(This,pHidden))
#define ITaskSettings_put_Hidden(This,hidden)  ((This)->lpVtbl->put_Hidden(This,hidden))
#define ITaskSettings_get_IdleSettings(This,ppIdleSettings)  ((This)->lpVtbl->get_IdleSettings(This,ppIdleSettings))
#define ITaskSettings_put_IdleSettings(This,pIdleSettings)  ((This)->lpVtbl->put_IdleSettings(This,pIdleSettings))
#define ITaskSettings_get_RunOnlyIfIdle(This,pRunOnlyIfIdle)  ((This)->lpVtbl->get_RunOnlyIfIdle(This,pRunOnlyIfIdle))
#define ITaskSettings_put_RunOnlyIfIdle(This,runOnlyIfIdle)  ((This)->lpVtbl->put_RunOnlyIfIdle(This,runOnlyIfIdle))
#define ITaskSettings_get_WakeToRun(This,pWake)  ((This)->lpVtbl->get_WakeToRun(This,pWake))
#define ITaskSettings_put_WakeToRun(This,wake)  ((This)->lpVtbl->put_WakeToRun(This,wake))
#define ITaskSettings_get_NetworkSettings(This,ppNetworkSettings)  ((This)->lpVtbl->get_NetworkSettings(This,ppNetworkSettings))
#define ITaskSettings_put_NetworkSettings(This,pNetworkSettings)  ((This)->lpVtbl->put_NetworkSettings(This,pNetworkSettings))
#endif

#endif

#endif

#ifndef __ITaskSettings2_INTERFACE_DEFINED__
#define __ITaskSettings2_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITaskSettings2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("2C05C3F0-6EED-4c05-A15F-ED7D7A98A369")ITaskSettings2:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_DisallowStartOnRemoteAppSession(VARIANT_BOOL * pDisallowStart) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DisallowStartOnRemoteAppSession(VARIANT_BOOL disallowStart) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UseUnifiedSchedulingEngine(VARIANT_BOOL *pUseUnifiedEngine) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_UseUnifiedSchedulingEngine(VARIANT_BOOL useUnifiedEngine) = 0;
};
#else
typedef struct ITaskSettings2Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (ITaskSettings2 * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(ITaskSettings2 * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITaskSettings2 * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ITaskSettings2 * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ITaskSettings2 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ITaskSettings2 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ITaskSettings2 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_DisallowStartOnRemoteAppSession)(ITaskSettings2 * This, VARIANT_BOOL * pDisallowStart);
    HRESULT (STDMETHODCALLTYPE *put_DisallowStartOnRemoteAppSession)(ITaskSettings2 * This, VARIANT_BOOL disallowStart);
    HRESULT (STDMETHODCALLTYPE *get_UseUnifiedSchedulingEngine)(ITaskSettings2 * This, VARIANT_BOOL * pUseUnifiedEngine);
    HRESULT (STDMETHODCALLTYPE *put_UseUnifiedSchedulingEngine)(ITaskSettings2 * This, VARIANT_BOOL useUnifiedEngine);
    END_INTERFACE
} ITaskSettings2Vtbl;

interface ITaskSettings2 {
    CONST_VTBL struct ITaskSettings2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITaskSettings2_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITaskSettings2_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITaskSettings2_Release(This)  ((This)->lpVtbl->Release(This))
#define ITaskSettings2_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ITaskSettings2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ITaskSettings2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ITaskSettings2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ITaskSettings2_get_DisallowStartOnRemoteAppSession(This,pDisallowStart)  ((This)->lpVtbl->get_DisallowStartOnRemoteAppSession(This,pDisallowStart))
#define ITaskSettings2_put_DisallowStartOnRemoteAppSession(This,disallowStart)  ((This)->lpVtbl->put_DisallowStartOnRemoteAppSession(This,disallowStart))
#define ITaskSettings2_get_UseUnifiedSchedulingEngine(This,pUseUnifiedEngine)  ((This)->lpVtbl->get_UseUnifiedSchedulingEngine(This,pUseUnifiedEngine))
#define ITaskSettings2_put_UseUnifiedSchedulingEngine(This,useUnifiedEngine)  ((This)->lpVtbl->put_UseUnifiedSchedulingEngine(This,useUnifiedEngine))
#endif

#endif

#endif

#ifndef __ITaskSettings3_INTERFACE_DEFINED__
#define __ITaskSettings3_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITaskSettings3;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("0AD9D0D7-0C7F-4EBB-9A5F-D1C648DCA528")ITaskSettings3:public ITaskSettings
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_DisallowStartOnRemoteAppSession(VARIANT_BOOL * pDisallowStart) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_DisallowStartOnRemoteAppSession(VARIANT_BOOL disallowStart) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_UseUnifiedSchedulingEngine(VARIANT_BOOL *pUseUnifiedEngine) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_UseUnifiedSchedulingEngine(VARIANT_BOOL useUnifiedEngine) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_MaintenanceSettings(IMaintenanceSettings **ppMaintenanceSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_MaintenanceSettings(IMaintenanceSettings *pMaintenanceSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateMaintenanceSettings(IMaintenanceSettings **ppMaintenanceSettings) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Volatile(VARIANT_BOOL *pVolatile) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Volatile(VARIANT_BOOL Volatile) = 0;
};
#else
typedef struct ITaskSettings3Vtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (ITaskSettings3 * This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITaskSettings3 * This);
    ULONG(STDMETHODCALLTYPE *Release)(ITaskSettings3 * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ITaskSettings3 * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ITaskSettings3 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ITaskSettings3 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ITaskSettings3 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_AllowDemandStart)(ITaskSettings3 * This, VARIANT_BOOL * pAllowDemandStart);
    HRESULT (STDMETHODCALLTYPE *put_AllowDemandStart)(ITaskSettings3 * This, VARIANT_BOOL allowDemandStart);
    HRESULT (STDMETHODCALLTYPE *get_RestartInterval)(ITaskSettings3 * This, BSTR * pRestartInterval);
    HRESULT (STDMETHODCALLTYPE *put_RestartInterval)(ITaskSettings3 * This, BSTR restartInterval);
    HRESULT (STDMETHODCALLTYPE *get_RestartCount)(ITaskSettings3 * This, int *pRestartCount);
    HRESULT (STDMETHODCALLTYPE *put_RestartCount)(ITaskSettings3 * This, int restartCount);
    HRESULT (STDMETHODCALLTYPE *get_MultipleInstances)(ITaskSettings3 * This, TASK_INSTANCES_POLICY * pPolicy);
    HRESULT (STDMETHODCALLTYPE *put_MultipleInstances)(ITaskSettings3 * This, TASK_INSTANCES_POLICY policy);
    HRESULT (STDMETHODCALLTYPE *get_StopIfGoingOnBatteries)(ITaskSettings3 * This, VARIANT_BOOL * pStopIfOnBatteries);
    HRESULT (STDMETHODCALLTYPE *put_StopIfGoingOnBatteries)(ITaskSettings3 * This, VARIANT_BOOL stopIfOnBatteries);
    HRESULT (STDMETHODCALLTYPE *get_DisallowStartIfOnBatteries)(ITaskSettings3 * This, VARIANT_BOOL * pDisallowStart);
    HRESULT (STDMETHODCALLTYPE *put_DisallowStartIfOnBatteries)(ITaskSettings3 * This, VARIANT_BOOL disallowStart);
    HRESULT (STDMETHODCALLTYPE *get_AllowHardTerminate)(ITaskSettings3 * This, VARIANT_BOOL * pAllowHardTerminate);
    HRESULT (STDMETHODCALLTYPE *put_AllowHardTerminate)(ITaskSettings3 * This, VARIANT_BOOL allowHardTerminate);
    HRESULT (STDMETHODCALLTYPE *get_StartWhenAvailable)(ITaskSettings3 * This, VARIANT_BOOL * pStartWhenAvailable);
    HRESULT (STDMETHODCALLTYPE *put_StartWhenAvailable)(ITaskSettings3 * This, VARIANT_BOOL startWhenAvailable);
    HRESULT (STDMETHODCALLTYPE *get_XmlText)(ITaskSettings3 * This, BSTR * pText);
    HRESULT (STDMETHODCALLTYPE *put_XmlText)(ITaskSettings3 * This, BSTR text);
    HRESULT (STDMETHODCALLTYPE *get_RunOnlyIfNetworkAvailable)(ITaskSettings3 * This, VARIANT_BOOL * pRunOnlyIfNetworkAvailable);
    HRESULT (STDMETHODCALLTYPE *put_RunOnlyIfNetworkAvailable)(ITaskSettings3 * This, VARIANT_BOOL runOnlyIfNetworkAvailable);
    HRESULT (STDMETHODCALLTYPE *get_ExecutionTimeLimit)(ITaskSettings3 * This, BSTR * pExecutionTimeLimit);
    HRESULT (STDMETHODCALLTYPE *put_ExecutionTimeLimit)(ITaskSettings3 * This, BSTR executionTimeLimit);
    HRESULT (STDMETHODCALLTYPE *get_Enabled)(ITaskSettings3 * This, VARIANT_BOOL * pEnabled);
    HRESULT (STDMETHODCALLTYPE *put_Enabled)(ITaskSettings3 * This, VARIANT_BOOL enabled);
    HRESULT (STDMETHODCALLTYPE *get_DeleteExpiredTaskAfter)(ITaskSettings3 * This, BSTR * pExpirationDelay);
    HRESULT (STDMETHODCALLTYPE *put_DeleteExpiredTaskAfter)(ITaskSettings3 * This, BSTR expirationDelay);
    HRESULT (STDMETHODCALLTYPE *get_Priority)(ITaskSettings3 * This, int *pPriority);
    HRESULT (STDMETHODCALLTYPE *put_Priority)(ITaskSettings3 * This, int priority);
    HRESULT (STDMETHODCALLTYPE *get_Compatibility)(ITaskSettings3 * This, TASK_COMPATIBILITY * pCompatLevel);
    HRESULT (STDMETHODCALLTYPE *put_Compatibility)(ITaskSettings3 * This, TASK_COMPATIBILITY compatLevel);
    HRESULT (STDMETHODCALLTYPE *get_Hidden)(ITaskSettings3 * This, VARIANT_BOOL * pHidden);
    HRESULT (STDMETHODCALLTYPE *put_Hidden)(ITaskSettings3 * This, VARIANT_BOOL hidden);
    HRESULT (STDMETHODCALLTYPE *get_IdleSettings)(ITaskSettings3 * This, IIdleSettings ** ppIdleSettings);
    HRESULT (STDMETHODCALLTYPE *put_IdleSettings)(ITaskSettings3 * This, IIdleSettings * pIdleSettings);
    HRESULT (STDMETHODCALLTYPE *get_RunOnlyIfIdle)(ITaskSettings3 * This, VARIANT_BOOL * pRunOnlyIfIdle);
    HRESULT (STDMETHODCALLTYPE *put_RunOnlyIfIdle)(ITaskSettings3 * This, VARIANT_BOOL runOnlyIfIdle);
    HRESULT (STDMETHODCALLTYPE *get_WakeToRun)(ITaskSettings3 * This, VARIANT_BOOL * pWake);
    HRESULT (STDMETHODCALLTYPE *put_WakeToRun)(ITaskSettings3 * This, VARIANT_BOOL wake);
    HRESULT (STDMETHODCALLTYPE *get_NetworkSettings)(ITaskSettings3 * This, INetworkSettings ** ppNetworkSettings);
    HRESULT (STDMETHODCALLTYPE *put_NetworkSettings)(ITaskSettings3 * This, INetworkSettings * pNetworkSettings);
    HRESULT (STDMETHODCALLTYPE *get_DisallowStartOnRemoteAppSession)(ITaskSettings3 * This, VARIANT_BOOL * pDisallowStart);
    HRESULT (STDMETHODCALLTYPE *put_DisallowStartOnRemoteAppSession)(ITaskSettings3 * This, VARIANT_BOOL disallowStart);
    HRESULT (STDMETHODCALLTYPE *get_UseUnifiedSchedulingEngine)(ITaskSettings3 * This, VARIANT_BOOL * pUseUnifiedEngine);
    HRESULT (STDMETHODCALLTYPE *put_UseUnifiedSchedulingEngine)(ITaskSettings3 * This, VARIANT_BOOL useUnifiedEngine);
    HRESULT (STDMETHODCALLTYPE *get_MaintenanceSettings)(ITaskSettings3 * This, IMaintenanceSettings ** ppMaintenanceSettings);
    HRESULT (STDMETHODCALLTYPE *put_MaintenanceSettings)(ITaskSettings3 * This, IMaintenanceSettings * pMaintenanceSettings);
    HRESULT (STDMETHODCALLTYPE *CreateMaintenanceSettings)(ITaskSettings3 * This, IMaintenanceSettings ** ppMaintenanceSettings);
    HRESULT (STDMETHODCALLTYPE *get_Volatile)(ITaskSettings3 * This, VARIANT_BOOL * pVolatile);
    HRESULT (STDMETHODCALLTYPE *put_Volatile)(ITaskSettings3 * This, VARIANT_BOOL Volatile);
    END_INTERFACE
} ITaskSettings3Vtbl;

interface ITaskSettings3 {
    CONST_VTBL struct ITaskSettings3Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITaskSettings3_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITaskSettings3_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITaskSettings3_Release(This)  ((This)->lpVtbl->Release(This))
#define ITaskSettings3_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ITaskSettings3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ITaskSettings3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ITaskSettings3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ITaskSettings3_get_AllowDemandStart(This,pAllowDemandStart)  ((This)->lpVtbl->get_AllowDemandStart(This,pAllowDemandStart))
#define ITaskSettings3_put_AllowDemandStart(This,allowDemandStart)  ((This)->lpVtbl->put_AllowDemandStart(This,allowDemandStart))
#define ITaskSettings3_get_RestartInterval(This,pRestartInterval)  ((This)->lpVtbl->get_RestartInterval(This,pRestartInterval))
#define ITaskSettings3_put_RestartInterval(This,restartInterval)  ((This)->lpVtbl->put_RestartInterval(This,restartInterval))
#define ITaskSettings3_get_RestartCount(This,pRestartCount)  ((This)->lpVtbl->get_RestartCount(This,pRestartCount))
#define ITaskSettings3_put_RestartCount(This,restartCount)  ((This)->lpVtbl->put_RestartCount(This,restartCount))
#define ITaskSettings3_get_MultipleInstances(This,pPolicy)  ((This)->lpVtbl->get_MultipleInstances(This,pPolicy))
#define ITaskSettings3_put_MultipleInstances(This,policy)  ((This)->lpVtbl->put_MultipleInstances(This,policy))
#define ITaskSettings3_get_StopIfGoingOnBatteries(This,pStopIfOnBatteries)  ((This)->lpVtbl->get_StopIfGoingOnBatteries(This,pStopIfOnBatteries))
#define ITaskSettings3_put_StopIfGoingOnBatteries(This,stopIfOnBatteries)  ((This)->lpVtbl->put_StopIfGoingOnBatteries(This,stopIfOnBatteries))
#define ITaskSettings3_get_DisallowStartIfOnBatteries(This,pDisallowStart)  ((This)->lpVtbl->get_DisallowStartIfOnBatteries(This,pDisallowStart))
#define ITaskSettings3_put_DisallowStartIfOnBatteries(This,disallowStart)  ((This)->lpVtbl->put_DisallowStartIfOnBatteries(This,disallowStart))
#define ITaskSettings3_get_AllowHardTerminate(This,pAllowHardTerminate)  ((This)->lpVtbl->get_AllowHardTerminate(This,pAllowHardTerminate))
#define ITaskSettings3_put_AllowHardTerminate(This,allowHardTerminate)  ((This)->lpVtbl->put_AllowHardTerminate(This,allowHardTerminate))
#define ITaskSettings3_get_StartWhenAvailable(This,pStartWhenAvailable)  ((This)->lpVtbl->get_StartWhenAvailable(This,pStartWhenAvailable))
#define ITaskSettings3_put_StartWhenAvailable(This,startWhenAvailable)  ((This)->lpVtbl->put_StartWhenAvailable(This,startWhenAvailable))
#define ITaskSettings3_get_XmlText(This,pText)  ((This)->lpVtbl->get_XmlText(This,pText))
#define ITaskSettings3_put_XmlText(This,text)  ((This)->lpVtbl->put_XmlText(This,text))
#define ITaskSettings3_get_RunOnlyIfNetworkAvailable(This,pRunOnlyIfNetworkAvailable)  ((This)->lpVtbl->get_RunOnlyIfNetworkAvailable(This,pRunOnlyIfNetworkAvailable))
#define ITaskSettings3_put_RunOnlyIfNetworkAvailable(This,runOnlyIfNetworkAvailable)  ((This)->lpVtbl->put_RunOnlyIfNetworkAvailable(This,runOnlyIfNetworkAvailable))
#define ITaskSettings3_get_ExecutionTimeLimit(This,pExecutionTimeLimit)  ((This)->lpVtbl->get_ExecutionTimeLimit(This,pExecutionTimeLimit))
#define ITaskSettings3_put_ExecutionTimeLimit(This,executionTimeLimit)  ((This)->lpVtbl->put_ExecutionTimeLimit(This,executionTimeLimit))
#define ITaskSettings3_get_Enabled(This,pEnabled)  ((This)->lpVtbl->get_Enabled(This,pEnabled))
#define ITaskSettings3_put_Enabled(This,enabled)  ((This)->lpVtbl->put_Enabled(This,enabled))
#define ITaskSettings3_get_DeleteExpiredTaskAfter(This,pExpirationDelay)  ((This)->lpVtbl->get_DeleteExpiredTaskAfter(This,pExpirationDelay))
#define ITaskSettings3_put_DeleteExpiredTaskAfter(This,expirationDelay)  ((This)->lpVtbl->put_DeleteExpiredTaskAfter(This,expirationDelay))
#define ITaskSettings3_get_Priority(This,pPriority)  ((This)->lpVtbl->get_Priority(This,pPriority))
#define ITaskSettings3_put_Priority(This,priority)  ((This)->lpVtbl->put_Priority(This,priority))
#define ITaskSettings3_get_Compatibility(This,pCompatLevel)  ((This)->lpVtbl->get_Compatibility(This,pCompatLevel))
#define ITaskSettings3_put_Compatibility(This,compatLevel)  ((This)->lpVtbl->put_Compatibility(This,compatLevel))
#define ITaskSettings3_get_Hidden(This,pHidden)  ((This)->lpVtbl->get_Hidden(This,pHidden))
#define ITaskSettings3_put_Hidden(This,hidden)  ((This)->lpVtbl->put_Hidden(This,hidden))
#define ITaskSettings3_get_IdleSettings(This,ppIdleSettings)  ((This)->lpVtbl->get_IdleSettings(This,ppIdleSettings))
#define ITaskSettings3_put_IdleSettings(This,pIdleSettings)  ((This)->lpVtbl->put_IdleSettings(This,pIdleSettings))
#define ITaskSettings3_get_RunOnlyIfIdle(This,pRunOnlyIfIdle)  ((This)->lpVtbl->get_RunOnlyIfIdle(This,pRunOnlyIfIdle))
#define ITaskSettings3_put_RunOnlyIfIdle(This,runOnlyIfIdle)  ((This)->lpVtbl->put_RunOnlyIfIdle(This,runOnlyIfIdle))
#define ITaskSettings3_get_WakeToRun(This,pWake)  ((This)->lpVtbl->get_WakeToRun(This,pWake))
#define ITaskSettings3_put_WakeToRun(This,wake)  ((This)->lpVtbl->put_WakeToRun(This,wake))
#define ITaskSettings3_get_NetworkSettings(This,ppNetworkSettings)  ((This)->lpVtbl->get_NetworkSettings(This,ppNetworkSettings))
#define ITaskSettings3_put_NetworkSettings(This,pNetworkSettings)  ((This)->lpVtbl->put_NetworkSettings(This,pNetworkSettings))
#define ITaskSettings3_get_DisallowStartOnRemoteAppSession(This,pDisallowStart)  ((This)->lpVtbl->get_DisallowStartOnRemoteAppSession(This,pDisallowStart))
#define ITaskSettings3_put_DisallowStartOnRemoteAppSession(This,disallowStart)  ((This)->lpVtbl->put_DisallowStartOnRemoteAppSession(This,disallowStart))
#define ITaskSettings3_get_UseUnifiedSchedulingEngine(This,pUseUnifiedEngine)  ((This)->lpVtbl->get_UseUnifiedSchedulingEngine(This,pUseUnifiedEngine))
#define ITaskSettings3_put_UseUnifiedSchedulingEngine(This,useUnifiedEngine)  ((This)->lpVtbl->put_UseUnifiedSchedulingEngine(This,useUnifiedEngine))
#define ITaskSettings3_get_MaintenanceSettings(This,ppMaintenanceSettings)  ((This)->lpVtbl->get_MaintenanceSettings(This,ppMaintenanceSettings))
#define ITaskSettings3_put_MaintenanceSettings(This,pMaintenanceSettings)  ((This)->lpVtbl->put_MaintenanceSettings(This,pMaintenanceSettings))
#define ITaskSettings3_CreateMaintenanceSettings(This,ppMaintenanceSettings)  ((This)->lpVtbl->CreateMaintenanceSettings(This,ppMaintenanceSettings))
#define ITaskSettings3_get_Volatile(This,pVolatile)  ((This)->lpVtbl->get_Volatile(This,pVolatile))
#define ITaskSettings3_put_Volatile(This,Volatile)  ((This)->lpVtbl->put_Volatile(This,Volatile))
#endif

#endif

#endif

#ifndef __IMaintenanceSettings_INTERFACE_DEFINED__
#define __IMaintenanceSettings_INTERFACE_DEFINED__

EXTERN_C const IID IID_IMaintenanceSettings;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("A6024FA8-9652-4ADB-A6BF-5CFCD877A7BA")IMaintenanceSettings:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE put_Period(BSTR value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Period(BSTR *target) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Deadline(BSTR value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Deadline(BSTR *target) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Exclusive(VARIANT_BOOL value) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Exclusive(VARIANT_BOOL *target) = 0;
};
#else
typedef struct IMaintenanceSettingsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (IMaintenanceSettings * This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IMaintenanceSettings * This);
    ULONG(STDMETHODCALLTYPE *Release)(IMaintenanceSettings * This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IMaintenanceSettings * This, UINT * pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IMaintenanceSettings * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IMaintenanceSettings * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IMaintenanceSettings * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
    HRESULT (STDMETHODCALLTYPE *put_Period)(IMaintenanceSettings * This, BSTR value);
    HRESULT (STDMETHODCALLTYPE *get_Period)(IMaintenanceSettings * This, BSTR * target);
    HRESULT (STDMETHODCALLTYPE *put_Deadline)(IMaintenanceSettings * This, BSTR value);
    HRESULT (STDMETHODCALLTYPE *get_Deadline)(IMaintenanceSettings * This, BSTR * target);
    HRESULT (STDMETHODCALLTYPE *put_Exclusive)(IMaintenanceSettings * This, VARIANT_BOOL value);
    HRESULT (STDMETHODCALLTYPE *get_Exclusive)(IMaintenanceSettings * This, VARIANT_BOOL * target);
    END_INTERFACE
} IMaintenanceSettingsVtbl;

interface IMaintenanceSettings {
    CONST_VTBL struct IMaintenanceSettingsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IMaintenanceSettings_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IMaintenanceSettings_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IMaintenanceSettings_Release(This)  ((This)->lpVtbl->Release(This))
#define IMaintenanceSettings_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IMaintenanceSettings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IMaintenanceSettings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IMaintenanceSettings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IMaintenanceSettings_put_Period(This,value)  ((This)->lpVtbl->put_Period(This,value))
#define IMaintenanceSettings_get_Period(This,target)  ((This)->lpVtbl->get_Period(This,target))
#define IMaintenanceSettings_put_Deadline(This,value)  ((This)->lpVtbl->put_Deadline(This,value))
#define IMaintenanceSettings_get_Deadline(This,target)  ((This)->lpVtbl->get_Deadline(This,target))
#define IMaintenanceSettings_put_Exclusive(This,value)  ((This)->lpVtbl->put_Exclusive(This,value))
#define IMaintenanceSettings_get_Exclusive(This,target)  ((This)->lpVtbl->get_Exclusive(This,target))
#endif

#endif

#endif

EXTERN_C const CLSID CLSID_TaskScheduler;

#ifdef __cplusplus

class DECLSPEC_UUID("0f87369f-a4e5-4cfc-bd3e-73e6154572dd") TaskScheduler;
#endif

EXTERN_C const CLSID CLSID_TaskHandlerPS;

#ifdef __cplusplus
class DECLSPEC_UUID("f2a69db7-da2c-4352-9066-86fee6dacac9") TaskHandlerPS;
#endif

EXTERN_C const CLSID CLSID_TaskHandlerStatusPS;

#ifdef __cplusplus
class DECLSPEC_UUID("9f15266d-d7ba-48f0-93c1-e6895f6fe5ac") TaskHandlerStatusPS;
#endif
#endif

#ifndef __IRegisteredTaskCollection_INTERFACE_DEFINED__
#define __IRegisteredTaskCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_IRegisteredTaskCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("86627eb4-42a7-41e4-a4d9-ac33a72f2d52")IRegisteredTaskCollection:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * pCount) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Item(VARIANT index, IRegisteredTask **ppRegisteredTask) = 0;
    virtual HRESULT STDMETHODCALLTYPE get__NewEnum(IUnknown **ppEnum) = 0;
};
#else
typedef struct IRegisteredTaskCollectionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IRegisteredTaskCollection *This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IRegisteredTaskCollection *This);
    ULONG(STDMETHODCALLTYPE *Release)(IRegisteredTaskCollection *This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IRegisteredTaskCollection *This, UINT *pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IRegisteredTaskCollection *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IRegisteredTaskCollection *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IRegisteredTaskCollection *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Count)(IRegisteredTaskCollection *This, LONG *pCount);
    HRESULT (STDMETHODCALLTYPE *get_Item)(IRegisteredTaskCollection *This, VARIANT index, IRegisteredTask **ppRegisteredTask);
    HRESULT (STDMETHODCALLTYPE *get__NewEnum)(IRegisteredTaskCollection *This, IUnknown **ppEnum);
    END_INTERFACE
} IRegisteredTaskCollectionVtbl;

    interface IRegisteredTaskCollection {
        CONST_VTBL struct IRegisteredTaskCollectionVtbl *lpVtbl;
    };

#ifdef COBJMACROS
#define IRegisteredTaskCollection_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IRegisteredTaskCollection_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IRegisteredTaskCollection_Release(This)  ((This)->lpVtbl->Release(This))
#define IRegisteredTaskCollection_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IRegisteredTaskCollection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IRegisteredTaskCollection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IRegisteredTaskCollection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IRegisteredTaskCollection_get_Count(This,pCount)  ((This)->lpVtbl->get_Count(This,pCount))
#define IRegisteredTaskCollection_get_Item(This,index,ppRegisteredTask)  ((This)->lpVtbl->get_Item(This,index,ppRegisteredTask))
#define IRegisteredTaskCollection_get__NewEnum(This,ppEnum)  ((This)->lpVtbl->get__NewEnum(This,ppEnum))
#endif

#endif

#endif

#ifndef __ITaskFolder_INTERFACE_DEFINED__
#define __ITaskFolder_INTERFACE_DEFINED__

EXTERN_C const IID IID_ITaskFolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("8cfac062-a080-4c15-9a88-aa7c2af80dfc")ITaskFolder:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * pName) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Path(BSTR *pPath) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFolder(BSTR path, ITaskFolder **ppFolder) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFolders(LONG flags, ITaskFolderCollection **ppFolders) = 0;
    virtual HRESULT STDMETHODCALLTYPE CreateFolder(BSTR subFolderName, VARIANT sddl, ITaskFolder **ppFolder) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteFolder(BSTR subFolderName, LONG flags) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTask(BSTR path, IRegisteredTask **ppTask) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetTasks(LONG flags, IRegisteredTaskCollection **ppTasks) = 0;
    virtual HRESULT STDMETHODCALLTYPE DeleteTask(BSTR name, LONG flags) = 0;
    virtual HRESULT STDMETHODCALLTYPE RegisterTask(BSTR path, BSTR xmlText, LONG flags, VARIANT userId, VARIANT password, TASK_LOGON_TYPE logonType, VARIANT sddl, IRegisteredTask **ppTask) = 0;
    virtual HRESULT STDMETHODCALLTYPE RegisterTaskDefinition(BSTR path, ITaskDefinition *pDefinition, LONG flags, VARIANT userId, VARIANT password, TASK_LOGON_TYPE logonType, VARIANT sddl, IRegisteredTask **ppTask) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSecurityDescriptor(LONG securityInformation, BSTR *pSddl) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetSecurityDescriptor(BSTR sddl, LONG flags) = 0;
};

#else

typedef struct ITaskFolderVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (ITaskFolder *This, REFIID riid, void **ppvObject);
    ULONG(STDMETHODCALLTYPE *AddRef)(ITaskFolder *This);
    ULONG(STDMETHODCALLTYPE *Release)(ITaskFolder *This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(ITaskFolder *This, UINT *pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(ITaskFolder *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(ITaskFolder *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(ITaskFolder *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Name)(ITaskFolder *This, BSTR *pName);
    HRESULT (STDMETHODCALLTYPE *get_Path)(ITaskFolder *This, BSTR *pPath);
    HRESULT (STDMETHODCALLTYPE *GetFolder)(ITaskFolder *This, BSTR path, ITaskFolder **ppFolder);
    HRESULT (STDMETHODCALLTYPE *GetFolders)(ITaskFolder *This, LONG flags, ITaskFolderCollection **ppFolders);
    HRESULT (STDMETHODCALLTYPE *CreateFolder)(ITaskFolder *This, BSTR subFolderName, VARIANT sddl, ITaskFolder **ppFolder);
    HRESULT (STDMETHODCALLTYPE *DeleteFolder)(ITaskFolder *This, BSTR subFolderName, LONG flags);
    HRESULT (STDMETHODCALLTYPE *GetTask)(ITaskFolder *This, BSTR path, IRegisteredTask **ppTask);
    HRESULT (STDMETHODCALLTYPE *GetTasks)(ITaskFolder *This, LONG flags, IRegisteredTaskCollection **ppTasks);
    HRESULT (STDMETHODCALLTYPE *DeleteTask)(ITaskFolder *This, BSTR name, LONG flags);
    HRESULT (STDMETHODCALLTYPE *RegisterTask)(ITaskFolder *This, BSTR path, BSTR xmlText, LONG flags, VARIANT userId, VARIANT password, TASK_LOGON_TYPE logonType, VARIANT sddl, IRegisteredTask **ppTask);
    HRESULT (STDMETHODCALLTYPE *RegisterTaskDefinition)(ITaskFolder *This, BSTR path, ITaskDefinition *pDefinition, LONG flags, VARIANT userId, VARIANT password, TASK_LOGON_TYPE logonType, VARIANT sddl, IRegisteredTask **ppTask);
    HRESULT (STDMETHODCALLTYPE *GetSecurityDescriptor)(ITaskFolder *This, LONG securityInformation, BSTR *pSddl);
    HRESULT (STDMETHODCALLTYPE *SetSecurityDescriptor)(ITaskFolder *This, BSTR sddl, LONG flags);
    END_INTERFACE
} ITaskFolderVtbl;

interface ITaskFolder {
    CONST_VTBL struct ITaskFolderVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ITaskFolder_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ITaskFolder_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ITaskFolder_Release(This)  ((This)->lpVtbl->Release(This))
#define ITaskFolder_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define ITaskFolder_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define ITaskFolder_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define ITaskFolder_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define ITaskFolder_get_Name(This,pName)  ((This)->lpVtbl->get_Name(This,pName))
#define ITaskFolder_get_Path(This,pPath)  ((This)->lpVtbl->get_Path(This,pPath))
#define ITaskFolder_GetFolder(This,path,ppFolder)  ((This)->lpVtbl->GetFolder(This,path,ppFolder))
#define ITaskFolder_GetFolders(This,flags,ppFolders)  ((This)->lpVtbl->GetFolders(This,flags,ppFolders))
#define ITaskFolder_CreateFolder(This,subFolderName,sddl,ppFolder)  ((This)->lpVtbl->CreateFolder(This,subFolderName,sddl,ppFolder))
#define ITaskFolder_DeleteFolder(This,subFolderName,flags)  ((This)->lpVtbl->DeleteFolder(This,subFolderName,flags))
#define ITaskFolder_GetTask(This,path,ppTask)  ((This)->lpVtbl->GetTask(This,path,ppTask))
#define ITaskFolder_GetTasks(This,flags,ppTasks)  ((This)->lpVtbl->GetTasks(This,flags,ppTasks))
#define ITaskFolder_DeleteTask(This,name,flags)  ((This)->lpVtbl->DeleteTask(This,name,flags))
#define ITaskFolder_RegisterTask(This,path,xmlText,flags,userId,password,logonType,sddl,ppTask)  ((This)->lpVtbl->RegisterTask(This,path,xmlText,flags,userId,password,logonType,sddl,ppTask))
#define ITaskFolder_RegisterTaskDefinition(This,path,pDefinition,flags,userId,password,logonType,sddl,ppTask)  ((This)->lpVtbl->RegisterTaskDefinition(This,path,pDefinition,flags,userId,password,logonType,sddl,ppTask))
#define ITaskFolder_GetSecurityDescriptor(This,securityInformation,pSddl)  ((This)->lpVtbl->GetSecurityDescriptor(This,securityInformation,pSddl))
#define ITaskFolder_SetSecurityDescriptor(This,sddl,flags)  ((This)->lpVtbl->SetSecurityDescriptor(This,sddl,flags))
#endif

#endif

#endif

#ifndef __IIdleSettings_INTERFACE_DEFINED__
#define __IIdleSettings_INTERFACE_DEFINED__

EXTERN_C const IID IID_IIdleSettings;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("84594461-0053-4342-A8FD-088FABF11F32")IIdleSettings:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_IdleDuration(BSTR * pDelay) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_IdleDuration(BSTR delay) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_WaitTimeout(BSTR *pTimeout) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_WaitTimeout(BSTR timeout) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StopOnIdleEnd(VARIANT_BOOL *pStop) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StopOnIdleEnd(VARIANT_BOOL stop) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_RestartOnIdle(VARIANT_BOOL *pRestart) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_RestartOnIdle(VARIANT_BOOL restart) = 0;
};
#else
typedef struct IIdleSettingsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IIdleSettings *This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IIdleSettings *This);
    ULONG(STDMETHODCALLTYPE *Release)(IIdleSettings *This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IIdleSettings *This, UINT *pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IIdleSettings *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IIdleSettings *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IIdleSettings *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_IdleDuration)(IIdleSettings *This, BSTR *pDelay);
    HRESULT (STDMETHODCALLTYPE *put_IdleDuration)(IIdleSettings *This, BSTR delay);
    HRESULT (STDMETHODCALLTYPE *get_WaitTimeout)(IIdleSettings *This, BSTR *pTimeout);
    HRESULT (STDMETHODCALLTYPE *put_WaitTimeout)(IIdleSettings *This, BSTR timeout);
    HRESULT (STDMETHODCALLTYPE *get_StopOnIdleEnd)(IIdleSettings *This, VARIANT_BOOL *pStop);
    HRESULT (STDMETHODCALLTYPE *put_StopOnIdleEnd)(IIdleSettings *This, VARIANT_BOOL stop);
    HRESULT (STDMETHODCALLTYPE *get_RestartOnIdle)(IIdleSettings *This, VARIANT_BOOL *pRestart);
    HRESULT (STDMETHODCALLTYPE *put_RestartOnIdle)(IIdleSettings *This, VARIANT_BOOL restart);
    END_INTERFACE
} IIdleSettingsVtbl;

interface IIdleSettings {
    CONST_VTBL struct IIdleSettingsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IIdleSettings_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IIdleSettings_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IIdleSettings_Release(This)  ((This)->lpVtbl->Release(This))
#define IIdleSettings_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IIdleSettings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IIdleSettings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IIdleSettings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IIdleSettings_get_IdleDuration(This,pDelay)  ((This)->lpVtbl->get_IdleDuration(This,pDelay))
#define IIdleSettings_put_IdleDuration(This,delay)  ((This)->lpVtbl->put_IdleDuration(This,delay))
#define IIdleSettings_get_WaitTimeout(This,pTimeout)  ((This)->lpVtbl->get_WaitTimeout(This,pTimeout))
#define IIdleSettings_put_WaitTimeout(This,timeout)  ((This)->lpVtbl->put_WaitTimeout(This,timeout))
#define IIdleSettings_get_StopOnIdleEnd(This,pStop)  ((This)->lpVtbl->get_StopOnIdleEnd(This,pStop))
#define IIdleSettings_put_StopOnIdleEnd(This,stop)  ((This)->lpVtbl->put_StopOnIdleEnd(This,stop))
#define IIdleSettings_get_RestartOnIdle(This,pRestart)  ((This)->lpVtbl->get_RestartOnIdle(This,pRestart))
#define IIdleSettings_put_RestartOnIdle(This,restart)  ((This)->lpVtbl->put_RestartOnIdle(This,restart))
#endif

#endif

#endif

#ifndef __INetworkSettings_INTERFACE_DEFINED__
#define __INetworkSettings_INTERFACE_DEFINED__

EXTERN_C const IID IID_INetworkSettings;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("9F7DEA84-C30B-4245-80B6-00E9F646F1B4")INetworkSettings:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * pName) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Name(BSTR name) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Id(BSTR *pId) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Id(BSTR id) = 0;
};

#else
typedef struct INetworkSettingsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (INetworkSettings *This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(INetworkSettings *This);
    ULONG(STDMETHODCALLTYPE *Release)(INetworkSettings *This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(INetworkSettings *This, UINT *pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(INetworkSettings *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(INetworkSettings *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(INetworkSettings *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Name)(INetworkSettings *This, BSTR *pName);
    HRESULT (STDMETHODCALLTYPE *put_Name)(INetworkSettings *This, BSTR name);
    HRESULT (STDMETHODCALLTYPE *get_Id)(INetworkSettings *This, BSTR *pId);
    HRESULT (STDMETHODCALLTYPE *put_Id)(INetworkSettings *This, BSTR id);
    END_INTERFACE
} INetworkSettingsVtbl;

interface INetworkSettings {
    CONST_VTBL struct INetworkSettingsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define INetworkSettings_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define INetworkSettings_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define INetworkSettings_Release(This)  ((This)->lpVtbl->Release(This))
#define INetworkSettings_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define INetworkSettings_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define INetworkSettings_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define INetworkSettings_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define INetworkSettings_get_Name(This,pName)  ((This)->lpVtbl->get_Name(This,pName))
#define INetworkSettings_put_Name(This,name)  ((This)->lpVtbl->put_Name(This,name))
#define INetworkSettings_get_Id(This,pId)  ((This)->lpVtbl->get_Id(This,pId))
#define INetworkSettings_put_Id(This,id)  ((This)->lpVtbl->put_Id(This,id))
#endif

#endif

#endif

#ifndef __IRepetitionPattern_INTERFACE_DEFINED__
#define __IRepetitionPattern_INTERFACE_DEFINED__

EXTERN_C const IID IID_IRepetitionPattern;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7FB9ACF1-26BE-400e-85B5-294B9C75DFD6")IRepetitionPattern:public IDispatch
{
    public:
    virtual HRESULT STDMETHODCALLTYPE get_Interval(BSTR * pInterval) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Interval(BSTR interval) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_Duration(BSTR *pDuration) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_Duration(BSTR duration) = 0;
    virtual HRESULT STDMETHODCALLTYPE get_StopAtDurationEnd(VARIANT_BOOL *pStop) = 0;
    virtual HRESULT STDMETHODCALLTYPE put_StopAtDurationEnd(VARIANT_BOOL stop) = 0;
};
#else
typedef struct IRepetitionPatternVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (IRepetitionPattern *This, REFIID riid, void **ppvObject);

    ULONG(STDMETHODCALLTYPE *AddRef)(IRepetitionPattern *This);
    ULONG(STDMETHODCALLTYPE *Release)(IRepetitionPattern *This);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfoCount)(IRepetitionPattern *This, UINT *pctinfo);
    HRESULT (STDMETHODCALLTYPE *GetTypeInfo)(IRepetitionPattern *This, UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
    HRESULT (STDMETHODCALLTYPE *GetIDsOfNames)(IRepetitionPattern *This, REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
    HRESULT (STDMETHODCALLTYPE *Invoke)(IRepetitionPattern *This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
    HRESULT (STDMETHODCALLTYPE *get_Interval)(IRepetitionPattern *This, BSTR *pInterval);
    HRESULT (STDMETHODCALLTYPE *put_Interval)(IRepetitionPattern *This, BSTR interval);
    HRESULT (STDMETHODCALLTYPE *get_Duration)(IRepetitionPattern *This, BSTR *pDuration);
    HRESULT (STDMETHODCALLTYPE *put_Duration)(IRepetitionPattern *This, BSTR duration);
    HRESULT (STDMETHODCALLTYPE *get_StopAtDurationEnd)(IRepetitionPattern *This, VARIANT_BOOL *pStop);
    HRESULT (STDMETHODCALLTYPE *put_StopAtDurationEnd)(IRepetitionPattern *This, VARIANT_BOOL stop);
    END_INTERFACE
} IRepetitionPatternVtbl;

interface IRepetitionPattern {
    CONST_VTBL struct IRepetitionPatternVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IRepetitionPattern_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define IRepetitionPattern_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define IRepetitionPattern_Release(This)  ((This)->lpVtbl->Release(This))
#define IRepetitionPattern_GetTypeInfoCount(This,pctinfo)  ((This)->lpVtbl->GetTypeInfoCount(This,pctinfo))
#define IRepetitionPattern_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  ((This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo))
#define IRepetitionPattern_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  ((This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId))
#define IRepetitionPattern_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  ((This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr))
#define IRepetitionPattern_get_Interval(This,pInterval)  ((This)->lpVtbl->get_Interval(This,pInterval))
#define IRepetitionPattern_put_Interval(This,interval)  ((This)->lpVtbl->put_Interval(This,interval))
#define IRepetitionPattern_get_Duration(This,pDuration)  ((This)->lpVtbl->get_Duration(This,pDuration))
#define IRepetitionPattern_put_Duration(This,duration)  ((This)->lpVtbl->put_Duration(This,duration))
#define IRepetitionPattern_get_StopAtDurationEnd(This,pStop)  ((This)->lpVtbl->get_StopAtDurationEnd(This,pStop))
#define IRepetitionPattern_put_StopAtDurationEnd(This,stop)  ((This)->lpVtbl->put_StopAtDurationEnd(This,stop))
#endif

#endif

#endif

extern RPC_IF_HANDLE __MIDL_itf_taskschd_0000_0044_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_taskschd_0000_0044_v0_0_s_ifspec;

unsigned long __RPC_USER BSTR_UserSize(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree(unsigned long *, BSTR *);

unsigned long __RPC_USER VARIANT_UserSize(unsigned long *, unsigned long, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserMarshal(unsigned long *, unsigned char *, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT *);
void __RPC_USER VARIANT_UserFree(unsigned long *, VARIANT *);

unsigned long __RPC_USER BSTR_UserSize64(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal64(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree64(unsigned long *, BSTR *);

unsigned long __RPC_USER VARIANT_UserSize64(unsigned long *, unsigned long, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserMarshal64(unsigned long *, unsigned char *, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserUnmarshal64(unsigned long *, unsigned char *, VARIANT *);
void __RPC_USER VARIANT_UserFree64(unsigned long *, VARIANT *);

#ifdef __cplusplus
}
#endif

#endif /* _TASKSCHD_H */
