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

#ifndef _AZROLES_H
#define _AZROLES_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

#ifndef __IAzAuthorizationStore_FWD_DEFINED__
#define __IAzAuthorizationStore_FWD_DEFINED__
typedef interface IAzAuthorizationStore IAzAuthorizationStore;
#endif

#ifndef __IAzAuthorizationStore2_FWD_DEFINED__
#define __IAzAuthorizationStore2_FWD_DEFINED__
typedef interface IAzAuthorizationStore2 IAzAuthorizationStore2;
#endif

#ifndef __IAzApplication_FWD_DEFINED__
#define __IAzApplication_FWD_DEFINED__
typedef interface IAzApplication IAzApplication;
#endif

#ifndef __IAzApplication2_FWD_DEFINED__
#define __IAzApplication2_FWD_DEFINED__
typedef interface IAzApplication2 IAzApplication2;
#endif

#ifndef __IAzApplications_FWD_DEFINED__
#define __IAzApplications_FWD_DEFINED__
typedef interface IAzApplications IAzApplications;
#endif

#ifndef __IAzOperation_FWD_DEFINED__
#define __IAzOperation_FWD_DEFINED__
typedef interface IAzOperation IAzOperation;
#endif

#ifndef __IAzOperations_FWD_DEFINED__
#define __IAzOperations_FWD_DEFINED__
typedef interface IAzOperations IAzOperations;
#endif

#ifndef __IAzTask_FWD_DEFINED__
#define __IAzTask_FWD_DEFINED__
typedef interface IAzTask IAzTask;
#endif

#ifndef __IAzTasks_FWD_DEFINED__
#define __IAzTasks_FWD_DEFINED__
typedef interface IAzTasks IAzTasks;
#endif

#ifndef __IAzScope_FWD_DEFINED__
#define __IAzScope_FWD_DEFINED__
typedef interface IAzScope IAzScope;
#endif

#ifndef __IAzScopes_FWD_DEFINED__
#define __IAzScopes_FWD_DEFINED__
typedef interface IAzScopes IAzScopes;
#endif

#ifndef __IAzApplicationGroup_FWD_DEFINED__
#define __IAzApplicationGroup_FWD_DEFINED__
typedef interface IAzApplicationGroup IAzApplicationGroup;
#endif

#ifndef __IAzApplicationGroups_FWD_DEFINED__
#define __IAzApplicationGroups_FWD_DEFINED__
typedef interface IAzApplicationGroups IAzApplicationGroups;
#endif

#ifndef __IAzRole_FWD_DEFINED__
#define __IAzRole_FWD_DEFINED__
typedef interface IAzRole IAzRole;
#endif

#ifndef __IAzRoles_FWD_DEFINED__
#define __IAzRoles_FWD_DEFINED__
typedef interface IAzRoles IAzRoles;
#endif

#ifndef __IAzClientContext_FWD_DEFINED__
#define __IAzClientContext_FWD_DEFINED__
typedef interface IAzClientContext IAzClientContext;
#endif

#ifndef __IAzClientContext2_FWD_DEFINED__
#define __IAzClientContext2_FWD_DEFINED__
typedef interface IAzClientContext2 IAzClientContext2;
#endif

#ifndef __IAzBizRuleContext_FWD_DEFINED__
#define __IAzBizRuleContext_FWD_DEFINED__
typedef interface IAzBizRuleContext IAzBizRuleContext;
#endif

#ifndef __AzAuthorizationStore_FWD_DEFINED__
#define __AzAuthorizationStore_FWD_DEFINED__

#ifdef __cplusplus
typedef class AzAuthorizationStore AzAuthorizationStore;
#else
typedef struct AzAuthorizationStore AzAuthorizationStore;
#endif 

#endif

#ifndef __AzBizRuleContext_FWD_DEFINED__
#define __AzBizRuleContext_FWD_DEFINED__

#ifdef __cplusplus
typedef class AzBizRuleContext AzBizRuleContext;
#else
typedef struct AzBizRuleContext AzBizRuleContext;
#endif 

#endif

#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void *__RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free(void *);

DEFINE_GUID(IID_IAzAuthorizationStore, 0xedbd9ca9, 0x9b82, 0x4f6a, 0x9e, 0x8b, 0x98, 0x30, 0x1e, 0x45, 0x0f, 0x14);
DEFINE_GUID(IID_IAzAuthorizationStore2, 0xb11e5584, 0xd577, 0x4273, 0xb6, 0xc5, 0x9, 0x73, 0xe0, 0xf8, 0xe8, 0xd);
DEFINE_GUID(CLSID_AzAuthorizationStore, 0xb2bcff59, 0xa757, 0x4b0b, 0xa1, 0xbc, 0xea, 0x69, 0x98, 0x1d, 0xa6, 0x9e);
DEFINE_GUID(IID_IAzBizRuleContext, 0xe192f17d, 0xd59f, 0x455e, 0xa1, 0x52, 0x94, 0x03, 0x16, 0xcd, 0x77, 0xb2);
DEFINE_GUID(CLSID_AzBizRuleContext, 0x5c2dc96f, 0x8d51, 0x434b, 0xb3, 0x3c, 0x37, 0x9b, 0xcc, 0xae, 0x77, 0xc3);

extern RPC_IF_HANDLE __MIDL_itf_azroles_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_azroles_0000_v0_0_s_ifspec;

#ifndef __IAzAuthorizationStore_INTERFACE_DEFINED__
#define __IAzAuthorizationStore_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzAuthorizationStore;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("edbd9ca9-9b82-4f6a-9e8b-98301e450f14")IAzAuthorizationStore:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Description(BSTR * pbstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Description(BSTR bstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ApplicationData(BSTR * pbstrApplicationData) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ApplicationData(BSTR bstrApplicationData) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DomainTimeout(LONG * plProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_DomainTimeout(LONG lProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ScriptEngineTimeout(LONG * plProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ScriptEngineTimeout(LONG lProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_MaxScriptEngines(LONG * plProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_MaxScriptEngines(LONG lProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_GenerateAudits(BOOL * pbProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_GenerateAudits(BOOL bProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Writable(BOOL * pfProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetProperty(LONG lPropId, VARIANT varReserved, VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetProperty(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPropertyItem(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePropertyItem(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PolicyAdministrators(VARIANT * pvarAdmins) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PolicyReaders(VARIANT * pvarReaders) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPolicyAdministrator(BSTR bstrAdmin, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePolicyAdministrator(BSTR bstrAdmin, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPolicyReader(BSTR bstrReader, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePolicyReader(BSTR bstrReader, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE Initialize(LONG lFlags, BSTR bstrPolicyURL, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE UpdateCache(VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE Delete(VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Applications(IAzApplications ** ppAppCollection) = 0;
	virtual HRESULT STDMETHODCALLTYPE OpenApplication(BSTR bstrApplicationName, VARIANT varReserved, IAzApplication ** ppApplication) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateApplication(BSTR bstrApplicationName, VARIANT varReserved, IAzApplication ** ppApplication) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteApplication(BSTR bstrApplicationName, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ApplicationGroups(IAzApplicationGroups ** ppGroupCollection) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateApplicationGroup(BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup) = 0;
	virtual HRESULT STDMETHODCALLTYPE OpenApplicationGroup(BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteApplicationGroup(BSTR bstrGroupName, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE Submit(LONG lFlags, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DelegatedPolicyUsers(VARIANT * pvarDelegatedPolicyUsers) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddDelegatedPolicyUser(BSTR bstrDelegatedPolicyUser, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteDelegatedPolicyUser(BSTR bstrDelegatedPolicyUser, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_TargetMachine(BSTR * pbstrTargetMachine) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ApplyStoreSacl(BOOL * pbApplyStoreSacl) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ApplyStoreSacl(BOOL bApplyStoreSacl) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PolicyAdministratorsName(VARIANT * pvarAdmins) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PolicyReadersName(VARIANT * pvarReaders) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPolicyAdministratorName(BSTR bstrAdmin, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePolicyAdministratorName(BSTR bstrAdmin, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPolicyReaderName(BSTR bstrReader, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePolicyReaderName(BSTR bstrReader, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DelegatedPolicyUsersName(VARIANT * pvarDelegatedPolicyUsers) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddDelegatedPolicyUserName(BSTR bstrDelegatedPolicyUser, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteDelegatedPolicyUserName(BSTR bstrDelegatedPolicyUser, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE CloseApplication(BSTR bstrApplicationName, LONG lFlag) = 0;
};
#else
typedef struct IAzAuthorizationStoreVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzAuthorizationStore * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzAuthorizationStore * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzAuthorizationStore * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzAuthorizationStore * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzAuthorizationStore * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzAuthorizationStore * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzAuthorizationStore * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IAzAuthorizationStore * This, BSTR * pbstrDescription);
	HRESULT(STDMETHODCALLTYPE *put_Description)(IAzAuthorizationStore * This, BSTR bstrDescription);
	HRESULT(STDMETHODCALLTYPE *get_ApplicationData)(IAzAuthorizationStore * This, BSTR * pbstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *put_ApplicationData)(IAzAuthorizationStore * This, BSTR bstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *get_DomainTimeout)(IAzAuthorizationStore * This, LONG * plProp);
	HRESULT(STDMETHODCALLTYPE *put_DomainTimeout)(IAzAuthorizationStore * This, LONG lProp);
	HRESULT(STDMETHODCALLTYPE *get_ScriptEngineTimeout)(IAzAuthorizationStore * This, LONG * plProp);
	HRESULT(STDMETHODCALLTYPE *put_ScriptEngineTimeout)(IAzAuthorizationStore * This, LONG lProp);
	HRESULT(STDMETHODCALLTYPE *get_MaxScriptEngines)(IAzAuthorizationStore * This, LONG * plProp);
	HRESULT(STDMETHODCALLTYPE *put_MaxScriptEngines)(IAzAuthorizationStore * This, LONG lProp);
	HRESULT(STDMETHODCALLTYPE *get_GenerateAudits)(IAzAuthorizationStore * This, BOOL * pbProp);
	HRESULT(STDMETHODCALLTYPE *put_GenerateAudits)(IAzAuthorizationStore * This, BOOL bProp);
	HRESULT(STDMETHODCALLTYPE *get_Writable)(IAzAuthorizationStore * This, BOOL * pfProp);
	HRESULT(STDMETHODCALLTYPE *GetProperty)(IAzAuthorizationStore * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *SetProperty)(IAzAuthorizationStore * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPropertyItem)(IAzAuthorizationStore * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePropertyItem)(IAzAuthorizationStore * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_PolicyAdministrators)(IAzAuthorizationStore * This, VARIANT * pvarAdmins);
	HRESULT(STDMETHODCALLTYPE *get_PolicyReaders)(IAzAuthorizationStore * This, VARIANT * pvarReaders);
	HRESULT(STDMETHODCALLTYPE *AddPolicyAdministrator)(IAzAuthorizationStore * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyAdministrator)(IAzAuthorizationStore * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPolicyReader)(IAzAuthorizationStore * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyReader)(IAzAuthorizationStore * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *Initialize)(IAzAuthorizationStore * This, LONG lFlags, BSTR bstrPolicyURL, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *UpdateCache)(IAzAuthorizationStore * This, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *Delete)(IAzAuthorizationStore * This, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Applications)(IAzAuthorizationStore * This, IAzApplications ** ppAppCollection);
	HRESULT(STDMETHODCALLTYPE *OpenApplication)(IAzAuthorizationStore * This, BSTR bstrApplicationName, VARIANT varReserved, IAzApplication ** ppApplication);
	HRESULT(STDMETHODCALLTYPE *CreateApplication)(IAzAuthorizationStore * This, BSTR bstrApplicationName, VARIANT varReserved, IAzApplication ** ppApplication);
	HRESULT(STDMETHODCALLTYPE *DeleteApplication)(IAzAuthorizationStore * This, BSTR bstrApplicationName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_ApplicationGroups)(IAzAuthorizationStore * This, IAzApplicationGroups ** ppGroupCollection);
	HRESULT(STDMETHODCALLTYPE *CreateApplicationGroup)(IAzAuthorizationStore * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
	HRESULT(STDMETHODCALLTYPE *OpenApplicationGroup)(IAzAuthorizationStore * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
	HRESULT(STDMETHODCALLTYPE *DeleteApplicationGroup)(IAzAuthorizationStore * This, BSTR bstrGroupName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *Submit)(IAzAuthorizationStore * This, LONG lFlags, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_DelegatedPolicyUsers)(IAzAuthorizationStore * This, VARIANT * pvarDelegatedPolicyUsers);
	HRESULT(STDMETHODCALLTYPE *AddDelegatedPolicyUser)(IAzAuthorizationStore * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteDelegatedPolicyUser)(IAzAuthorizationStore * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_TargetMachine)(IAzAuthorizationStore * This, BSTR * pbstrTargetMachine);
	HRESULT(STDMETHODCALLTYPE *get_ApplyStoreSacl)(IAzAuthorizationStore * This, BOOL * pbApplyStoreSacl);
	HRESULT(STDMETHODCALLTYPE *put_ApplyStoreSacl)(IAzAuthorizationStore * This, BOOL bApplyStoreSacl);
	HRESULT(STDMETHODCALLTYPE *get_PolicyAdministratorsName)(IAzAuthorizationStore * This, VARIANT * pvarAdmins);
	HRESULT(STDMETHODCALLTYPE *get_PolicyReadersName)(IAzAuthorizationStore * This, VARIANT * pvarReaders);
	HRESULT(STDMETHODCALLTYPE *AddPolicyAdministratorName)(IAzAuthorizationStore * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyAdministratorName)(IAzAuthorizationStore * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPolicyReaderName)(IAzAuthorizationStore * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyReaderName)(IAzAuthorizationStore * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_DelegatedPolicyUsersName)(IAzAuthorizationStore * This, VARIANT * pvarDelegatedPolicyUsers);
	HRESULT(STDMETHODCALLTYPE *AddDelegatedPolicyUserName)(IAzAuthorizationStore * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteDelegatedPolicyUserName)(IAzAuthorizationStore * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *CloseApplication)(IAzAuthorizationStore * This, BSTR bstrApplicationName, LONG lFlag);
	END_INTERFACE
} IAzAuthorizationStoreVtbl;

interface IAzAuthorizationStore {
    CONST_VTBL struct IAzAuthorizationStoreVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzAuthorizationStore_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzAuthorizationStore_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzAuthorizationStore_Release(This)  (This)->lpVtbl->Release(This)
#define IAzAuthorizationStore_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzAuthorizationStore_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzAuthorizationStore_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzAuthorizationStore_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzAuthorizationStore_get_Description(This,pbstrDescription)  (This)->lpVtbl->get_Description(This,pbstrDescription)
#define IAzAuthorizationStore_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IAzAuthorizationStore_get_ApplicationData(This,pbstrApplicationData)  (This)->lpVtbl->get_ApplicationData(This,pbstrApplicationData)
#define IAzAuthorizationStore_put_ApplicationData(This,bstrApplicationData)  (This)->lpVtbl->put_ApplicationData(This,bstrApplicationData)
#define IAzAuthorizationStore_get_DomainTimeout(This,plProp)  (This)->lpVtbl->get_DomainTimeout(This,plProp)
#define IAzAuthorizationStore_put_DomainTimeout(This,lProp)  (This)->lpVtbl->put_DomainTimeout(This,lProp)
#define IAzAuthorizationStore_get_ScriptEngineTimeout(This,plProp)  (This)->lpVtbl->get_ScriptEngineTimeout(This,plProp)
#define IAzAuthorizationStore_put_ScriptEngineTimeout(This,lProp)  (This)->lpVtbl->put_ScriptEngineTimeout(This,lProp)
#define IAzAuthorizationStore_get_MaxScriptEngines(This,plProp)  (This)->lpVtbl->get_MaxScriptEngines(This,plProp)
#define IAzAuthorizationStore_put_MaxScriptEngines(This,lProp)  (This)->lpVtbl->put_MaxScriptEngines(This,lProp)
#define IAzAuthorizationStore_get_GenerateAudits(This,pbProp)  (This)->lpVtbl->get_GenerateAudits(This,pbProp)
#define IAzAuthorizationStore_put_GenerateAudits(This,bProp)  (This)->lpVtbl->put_GenerateAudits(This,bProp)
#define IAzAuthorizationStore_get_Writable(This,pfProp)  (This)->lpVtbl->get_Writable(This,pfProp)
#define IAzAuthorizationStore_GetProperty(This,lPropId,varReserved,pvarProp)  (This)->lpVtbl->GetProperty(This,lPropId,varReserved,pvarProp)
#define IAzAuthorizationStore_SetProperty(This,lPropId,varProp,varReserved)  (This)->lpVtbl->SetProperty(This,lPropId,varProp,varReserved)
#define IAzAuthorizationStore_AddPropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->AddPropertyItem(This,lPropId,varProp,varReserved)
#define IAzAuthorizationStore_DeletePropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->DeletePropertyItem(This,lPropId,varProp,varReserved)
#define IAzAuthorizationStore_get_PolicyAdministrators(This,pvarAdmins)  (This)->lpVtbl->get_PolicyAdministrators(This,pvarAdmins)
#define IAzAuthorizationStore_get_PolicyReaders(This,pvarReaders)  (This)->lpVtbl->get_PolicyReaders(This,pvarReaders)
#define IAzAuthorizationStore_AddPolicyAdministrator(This,bstrAdmin,varReserved)  (This)->lpVtbl->AddPolicyAdministrator(This,bstrAdmin,varReserved)
#define IAzAuthorizationStore_DeletePolicyAdministrator(This,bstrAdmin,varReserved)  (This)->lpVtbl->DeletePolicyAdministrator(This,bstrAdmin,varReserved)
#define IAzAuthorizationStore_AddPolicyReader(This,bstrReader,varReserved)  (This)->lpVtbl->AddPolicyReader(This,bstrReader,varReserved)
#define IAzAuthorizationStore_DeletePolicyReader(This,bstrReader,varReserved)  (This)->lpVtbl->DeletePolicyReader(This,bstrReader,varReserved)
#define IAzAuthorizationStore_Initialize(This,lFlags,bstrPolicyURL,varReserved)  (This)->lpVtbl->Initialize(This,lFlags,bstrPolicyURL,varReserved)
#define IAzAuthorizationStore_UpdateCache(This,varReserved)  (This)->lpVtbl->UpdateCache(This,varReserved)
#define IAzAuthorizationStore_Delete(This,varReserved)  (This)->lpVtbl->Delete(This,varReserved)
#define IAzAuthorizationStore_get_Applications(This,ppAppCollection)  (This)->lpVtbl->get_Applications(This,ppAppCollection)
#define IAzAuthorizationStore_OpenApplication(This,bstrApplicationName,varReserved,ppApplication)  (This)->lpVtbl->OpenApplication(This,bstrApplicationName,varReserved,ppApplication)
#define IAzAuthorizationStore_CreateApplication(This,bstrApplicationName,varReserved,ppApplication)  (This)->lpVtbl->CreateApplication(This,bstrApplicationName,varReserved,ppApplication)
#define IAzAuthorizationStore_DeleteApplication(This,bstrApplicationName,varReserved)  (This)->lpVtbl->DeleteApplication(This,bstrApplicationName,varReserved)
#define IAzAuthorizationStore_get_ApplicationGroups(This,ppGroupCollection)  (This)->lpVtbl->get_ApplicationGroups(This,ppGroupCollection)
#define IAzAuthorizationStore_CreateApplicationGroup(This,bstrGroupName,varReserved,ppGroup)  (This)->lpVtbl->CreateApplicationGroup(This,bstrGroupName,varReserved,ppGroup)
#define IAzAuthorizationStore_OpenApplicationGroup(This,bstrGroupName,varReserved,ppGroup)  (This)->lpVtbl->OpenApplicationGroup(This,bstrGroupName,varReserved,ppGroup)
#define IAzAuthorizationStore_DeleteApplicationGroup(This,bstrGroupName,varReserved)  (This)->lpVtbl->DeleteApplicationGroup(This,bstrGroupName,varReserved)
#define IAzAuthorizationStore_Submit(This,lFlags,varReserved)  (This)->lpVtbl->Submit(This,lFlags,varReserved)
#define IAzAuthorizationStore_get_DelegatedPolicyUsers(This,pvarDelegatedPolicyUsers)  (This)->lpVtbl->get_DelegatedPolicyUsers(This,pvarDelegatedPolicyUsers)
#define IAzAuthorizationStore_AddDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->AddDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)
#define IAzAuthorizationStore_DeleteDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->DeleteDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)
#define IAzAuthorizationStore_get_TargetMachine(This,pbstrTargetMachine)  (This)->lpVtbl->get_TargetMachine(This,pbstrTargetMachine)
#define IAzAuthorizationStore_get_ApplyStoreSacl(This,pbApplyStoreSacl)  (This)->lpVtbl->get_ApplyStoreSacl(This,pbApplyStoreSacl)
#define IAzAuthorizationStore_put_ApplyStoreSacl(This,bApplyStoreSacl)  (This)->lpVtbl->put_ApplyStoreSacl(This,bApplyStoreSacl)
#define IAzAuthorizationStore_get_PolicyAdministratorsName(This,pvarAdmins)  (This)->lpVtbl->get_PolicyAdministratorsName(This,pvarAdmins)
#define IAzAuthorizationStore_get_PolicyReadersName(This,pvarReaders)  (This)->lpVtbl->get_PolicyReadersName(This,pvarReaders)
#define IAzAuthorizationStore_AddPolicyAdministratorName(This,bstrAdmin,varReserved)  (This)->lpVtbl->AddPolicyAdministratorName(This,bstrAdmin,varReserved)
#define IAzAuthorizationStore_DeletePolicyAdministratorName(This,bstrAdmin,varReserved)  (This)->lpVtbl->DeletePolicyAdministratorName(This,bstrAdmin,varReserved)
#define IAzAuthorizationStore_AddPolicyReaderName(This,bstrReader,varReserved)  (This)->lpVtbl->AddPolicyReaderName(This,bstrReader,varReserved)
#define IAzAuthorizationStore_DeletePolicyReaderName(This,bstrReader,varReserved)  (This)->lpVtbl->DeletePolicyReaderName(This,bstrReader,varReserved)
#define IAzAuthorizationStore_get_DelegatedPolicyUsersName(This,pvarDelegatedPolicyUsers)  (This)->lpVtbl->get_DelegatedPolicyUsersName(This,pvarDelegatedPolicyUsers)
#define IAzAuthorizationStore_AddDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->AddDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)
#define IAzAuthorizationStore_DeleteDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->DeleteDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)
#define IAzAuthorizationStore_CloseApplication(This,bstrApplicationName,lFlag)  (This)->lpVtbl->CloseApplication(This,bstrApplicationName,lFlag)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_Description_Proxy(IAzAuthorizationStore * This, BSTR * pbstrDescription);
void __RPC_STUB IAzAuthorizationStore_get_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_put_Description_Proxy(IAzAuthorizationStore * This, BSTR bstrDescription);
void __RPC_STUB IAzAuthorizationStore_put_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_ApplicationData_Proxy(IAzAuthorizationStore * This, BSTR * pbstrApplicationData);
void __RPC_STUB IAzAuthorizationStore_get_ApplicationData_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_put_ApplicationData_Proxy(IAzAuthorizationStore * This, BSTR bstrApplicationData);
void __RPC_STUB IAzAuthorizationStore_put_ApplicationData_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_DomainTimeout_Proxy(IAzAuthorizationStore * This, LONG * plProp);
void __RPC_STUB IAzAuthorizationStore_get_DomainTimeout_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_put_DomainTimeout_Proxy(IAzAuthorizationStore * This, LONG lProp);
void __RPC_STUB IAzAuthorizationStore_put_DomainTimeout_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_ScriptEngineTimeout_Proxy(IAzAuthorizationStore * This, LONG * plProp);
void __RPC_STUB IAzAuthorizationStore_get_ScriptEngineTimeout_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_put_ScriptEngineTimeout_Proxy(IAzAuthorizationStore * This, LONG lProp);
void __RPC_STUB IAzAuthorizationStore_put_ScriptEngineTimeout_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_MaxScriptEngines_Proxy(IAzAuthorizationStore * This, LONG * plProp);
void __RPC_STUB IAzAuthorizationStore_get_MaxScriptEngines_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_put_MaxScriptEngines_Proxy(IAzAuthorizationStore * This, LONG lProp);
void __RPC_STUB IAzAuthorizationStore_put_MaxScriptEngines_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_GenerateAudits_Proxy(IAzAuthorizationStore * This, BOOL * pbProp);
void __RPC_STUB IAzAuthorizationStore_get_GenerateAudits_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_put_GenerateAudits_Proxy(IAzAuthorizationStore * This, BOOL bProp);
void __RPC_STUB IAzAuthorizationStore_put_GenerateAudits_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_Writable_Proxy(IAzAuthorizationStore * This, BOOL * pfProp);
void __RPC_STUB IAzAuthorizationStore_get_Writable_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_GetProperty_Proxy(IAzAuthorizationStore * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
void __RPC_STUB IAzAuthorizationStore_GetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_SetProperty_Proxy(IAzAuthorizationStore * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_SetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_AddPropertyItem_Proxy(IAzAuthorizationStore * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_AddPropertyItem_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_DeletePropertyItem_Proxy(IAzAuthorizationStore * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_DeletePropertyItem_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_PolicyAdministrators_Proxy(IAzAuthorizationStore * This, VARIANT * pvarAdmins);
void __RPC_STUB IAzAuthorizationStore_get_PolicyAdministrators_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_PolicyReaders_Proxy(IAzAuthorizationStore * This, VARIANT * pvarReaders);
void __RPC_STUB IAzAuthorizationStore_get_PolicyReaders_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_AddPolicyAdministrator_Proxy(IAzAuthorizationStore * This, BSTR bstrAdmin, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_AddPolicyAdministrator_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_DeletePolicyAdministrator_Proxy(IAzAuthorizationStore * This, BSTR bstrAdmin, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_DeletePolicyAdministrator_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_AddPolicyReader_Proxy(IAzAuthorizationStore * This, BSTR bstrReader, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_AddPolicyReader_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_DeletePolicyReader_Proxy(IAzAuthorizationStore * This, BSTR bstrReader, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_DeletePolicyReader_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_Initialize_Proxy(IAzAuthorizationStore * This, LONG lFlags, BSTR bstrPolicyURL, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_Initialize_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_UpdateCache_Proxy(IAzAuthorizationStore * This, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_UpdateCache_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_Delete_Proxy(IAzAuthorizationStore * This, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_Delete_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_Applications_Proxy(IAzAuthorizationStore * This, IAzApplications ** ppAppCollection);
void __RPC_STUB IAzAuthorizationStore_get_Applications_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_OpenApplication_Proxy(IAzAuthorizationStore * This, BSTR bstrApplicationName, VARIANT varReserved, IAzApplication ** ppApplication);
void __RPC_STUB IAzAuthorizationStore_OpenApplication_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_CreateApplication_Proxy(IAzAuthorizationStore * This, BSTR bstrApplicationName, VARIANT varReserved, IAzApplication ** ppApplication);
void __RPC_STUB IAzAuthorizationStore_CreateApplication_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_DeleteApplication_Proxy(IAzAuthorizationStore * This, BSTR bstrApplicationName, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_DeleteApplication_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_ApplicationGroups_Proxy(IAzAuthorizationStore * This, IAzApplicationGroups ** ppGroupCollection);
void __RPC_STUB IAzAuthorizationStore_get_ApplicationGroups_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_CreateApplicationGroup_Proxy(IAzAuthorizationStore * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
void __RPC_STUB IAzAuthorizationStore_CreateApplicationGroup_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_OpenApplicationGroup_Proxy(IAzAuthorizationStore * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
void __RPC_STUB IAzAuthorizationStore_OpenApplicationGroup_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_DeleteApplicationGroup_Proxy(IAzAuthorizationStore * This, BSTR bstrGroupName, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_DeleteApplicationGroup_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_Submit_Proxy(IAzAuthorizationStore * This, LONG lFlags, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_Submit_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_DelegatedPolicyUsers_Proxy(IAzAuthorizationStore * This, VARIANT * pvarDelegatedPolicyUsers);
void __RPC_STUB IAzAuthorizationStore_get_DelegatedPolicyUsers_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_AddDelegatedPolicyUser_Proxy(IAzAuthorizationStore * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_AddDelegatedPolicyUser_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_DeleteDelegatedPolicyUser_Proxy(IAzAuthorizationStore * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_DeleteDelegatedPolicyUser_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_TargetMachine_Proxy(IAzAuthorizationStore * This, BSTR * pbstrTargetMachine);
void __RPC_STUB IAzAuthorizationStore_get_TargetMachine_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_ApplyStoreSacl_Proxy(IAzAuthorizationStore * This, BOOL * pbApplyStoreSacl);
void __RPC_STUB IAzAuthorizationStore_get_ApplyStoreSacl_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_put_ApplyStoreSacl_Proxy(IAzAuthorizationStore * This, BOOL bApplyStoreSacl);
void __RPC_STUB IAzAuthorizationStore_put_ApplyStoreSacl_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_PolicyAdministratorsName_Proxy(IAzAuthorizationStore * This, VARIANT * pvarAdmins);
void __RPC_STUB IAzAuthorizationStore_get_PolicyAdministratorsName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_PolicyReadersName_Proxy(IAzAuthorizationStore * This, VARIANT * pvarReaders);
void __RPC_STUB IAzAuthorizationStore_get_PolicyReadersName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_AddPolicyAdministratorName_Proxy(IAzAuthorizationStore * This, BSTR bstrAdmin, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_AddPolicyAdministratorName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_DeletePolicyAdministratorName_Proxy(IAzAuthorizationStore * This, BSTR bstrAdmin, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_DeletePolicyAdministratorName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_AddPolicyReaderName_Proxy(IAzAuthorizationStore * This, BSTR bstrReader, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_AddPolicyReaderName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_DeletePolicyReaderName_Proxy(IAzAuthorizationStore * This, BSTR bstrReader, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_DeletePolicyReaderName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_get_DelegatedPolicyUsersName_Proxy(IAzAuthorizationStore * This, VARIANT * pvarDelegatedPolicyUsers);
void __RPC_STUB IAzAuthorizationStore_get_DelegatedPolicyUsersName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_AddDelegatedPolicyUserName_Proxy(IAzAuthorizationStore * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_AddDelegatedPolicyUserName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_DeleteDelegatedPolicyUserName_Proxy(IAzAuthorizationStore * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
void __RPC_STUB IAzAuthorizationStore_DeleteDelegatedPolicyUserName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore_CloseApplication_Proxy(IAzAuthorizationStore * This, BSTR bstrApplicationName, LONG lFlag);
void __RPC_STUB IAzAuthorizationStore_CloseApplication_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzAuthorizationStore2_INTERFACE_DEFINED__
#define __IAzAuthorizationStore2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzAuthorizationStore2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("b11e5584-d577-4273-b6c5-0973e0f8e80d")IAzAuthorizationStore2:public IAzAuthorizationStore
{
	public:
	virtual HRESULT STDMETHODCALLTYPE OpenApplication2(BSTR bstrApplicationName, VARIANT varReserved, IAzApplication2 ** ppApplication) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateApplication2(BSTR bstrApplicationName, VARIANT varReserved, IAzApplication2 ** ppApplication) = 0;
};
#else
typedef struct IAzAuthorizationStore2Vtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzAuthorizationStore2 * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzAuthorizationStore2 * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzAuthorizationStore2 * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzAuthorizationStore2 * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzAuthorizationStore2 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzAuthorizationStore2 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzAuthorizationStore2 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IAzAuthorizationStore2 * This, BSTR * pbstrDescription);
	HRESULT(STDMETHODCALLTYPE *put_Description)(IAzAuthorizationStore2 * This, BSTR bstrDescription);
	HRESULT(STDMETHODCALLTYPE *get_ApplicationData)(IAzAuthorizationStore2 * This, BSTR * pbstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *put_ApplicationData)(IAzAuthorizationStore2 * This, BSTR bstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *get_DomainTimeout)(IAzAuthorizationStore2 * This, LONG * plProp);
	HRESULT(STDMETHODCALLTYPE *put_DomainTimeout)(IAzAuthorizationStore2 * This, LONG lProp);
	HRESULT(STDMETHODCALLTYPE *get_ScriptEngineTimeout)(IAzAuthorizationStore2 * This, LONG * plProp);
	HRESULT(STDMETHODCALLTYPE *put_ScriptEngineTimeout)(IAzAuthorizationStore2 * This, LONG lProp);
	HRESULT(STDMETHODCALLTYPE *get_MaxScriptEngines)(IAzAuthorizationStore2 * This, LONG * plProp);
	HRESULT(STDMETHODCALLTYPE *put_MaxScriptEngines)(IAzAuthorizationStore2 * This, LONG lProp);
	HRESULT(STDMETHODCALLTYPE *get_GenerateAudits)(IAzAuthorizationStore2 * This, BOOL * pbProp);
	HRESULT(STDMETHODCALLTYPE *put_GenerateAudits)(IAzAuthorizationStore2 * This, BOOL bProp);
	HRESULT(STDMETHODCALLTYPE *get_Writable)(IAzAuthorizationStore2 * This, BOOL * pfProp);
	HRESULT(STDMETHODCALLTYPE *GetProperty)(IAzAuthorizationStore2 * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *SetProperty)(IAzAuthorizationStore2 * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPropertyItem)(IAzAuthorizationStore2 * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePropertyItem)(IAzAuthorizationStore2 * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_PolicyAdministrators)(IAzAuthorizationStore2 * This, VARIANT * pvarAdmins);
	HRESULT(STDMETHODCALLTYPE *get_PolicyReaders)(IAzAuthorizationStore2 * This, VARIANT * pvarReaders);
	HRESULT(STDMETHODCALLTYPE *AddPolicyAdministrator)(IAzAuthorizationStore2 * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyAdministrator)(IAzAuthorizationStore2 * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPolicyReader)(IAzAuthorizationStore2 * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyReader)(IAzAuthorizationStore2 * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *Initialize)(IAzAuthorizationStore2 * This, LONG lFlags, BSTR bstrPolicyURL, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *UpdateCache)(IAzAuthorizationStore2 * This, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *Delete)(IAzAuthorizationStore2 * This, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Applications)(IAzAuthorizationStore2 * This, IAzApplications ** ppAppCollection);
	HRESULT(STDMETHODCALLTYPE *OpenApplication)(IAzAuthorizationStore2 * This, BSTR bstrApplicationName, VARIANT varReserved, IAzApplication ** ppApplication);
	HRESULT(STDMETHODCALLTYPE *CreateApplication)(IAzAuthorizationStore2 * This, BSTR bstrApplicationName, VARIANT varReserved, IAzApplication ** ppApplication);
	HRESULT(STDMETHODCALLTYPE *DeleteApplication)(IAzAuthorizationStore2 * This, BSTR bstrApplicationName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_ApplicationGroups)(IAzAuthorizationStore2 * This, IAzApplicationGroups ** ppGroupCollection);
	HRESULT(STDMETHODCALLTYPE *CreateApplicationGroup)(IAzAuthorizationStore2 * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
	HRESULT(STDMETHODCALLTYPE *OpenApplicationGroup)(IAzAuthorizationStore2 * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
	HRESULT(STDMETHODCALLTYPE *DeleteApplicationGroup)(IAzAuthorizationStore2 * This, BSTR bstrGroupName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *Submit)(IAzAuthorizationStore2 * This, LONG lFlags, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_DelegatedPolicyUsers)(IAzAuthorizationStore2 * This, VARIANT * pvarDelegatedPolicyUsers);
	HRESULT(STDMETHODCALLTYPE *AddDelegatedPolicyUser)(IAzAuthorizationStore2 * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteDelegatedPolicyUser)(IAzAuthorizationStore2 * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_TargetMachine)(IAzAuthorizationStore2 * This, BSTR * pbstrTargetMachine);
	HRESULT(STDMETHODCALLTYPE *get_ApplyStoreSacl)(IAzAuthorizationStore2 * This, BOOL * pbApplyStoreSacl);
	HRESULT(STDMETHODCALLTYPE *put_ApplyStoreSacl)(IAzAuthorizationStore2 * This, BOOL bApplyStoreSacl);
	HRESULT(STDMETHODCALLTYPE *get_PolicyAdministratorsName)(IAzAuthorizationStore2 * This, VARIANT * pvarAdmins);
	HRESULT(STDMETHODCALLTYPE *get_PolicyReadersName)(IAzAuthorizationStore2 * This, VARIANT * pvarReaders);
	HRESULT(STDMETHODCALLTYPE *AddPolicyAdministratorName)(IAzAuthorizationStore2 * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyAdministratorName)(IAzAuthorizationStore2 * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPolicyReaderName)(IAzAuthorizationStore2 * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyReaderName)(IAzAuthorizationStore2 * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_DelegatedPolicyUsersName)(IAzAuthorizationStore2 * This, VARIANT * pvarDelegatedPolicyUsers);
	HRESULT(STDMETHODCALLTYPE *AddDelegatedPolicyUserName)(IAzAuthorizationStore2 * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteDelegatedPolicyUserName)(IAzAuthorizationStore2 * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *CloseApplication)(IAzAuthorizationStore2 * This, BSTR bstrApplicationName, LONG lFlag);
	HRESULT(STDMETHODCALLTYPE *OpenApplication2)(IAzAuthorizationStore2 * This, BSTR bstrApplicationName, VARIANT varReserved, IAzApplication2 ** ppApplication);
	HRESULT(STDMETHODCALLTYPE *CreateApplication2)(IAzAuthorizationStore2 * This, BSTR bstrApplicationName, VARIANT varReserved, IAzApplication2 ** ppApplication);
	END_INTERFACE
} IAzAuthorizationStore2Vtbl;

interface IAzAuthorizationStore2 {
    CONST_VTBL struct IAzAuthorizationStore2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzAuthorizationStore2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzAuthorizationStore2_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzAuthorizationStore2_Release(This)  (This)->lpVtbl->Release(This)
#define IAzAuthorizationStore2_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzAuthorizationStore2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzAuthorizationStore2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzAuthorizationStore2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzAuthorizationStore2_get_Description(This,pbstrDescription)  (This)->lpVtbl->get_Description(This,pbstrDescription)
#define IAzAuthorizationStore2_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IAzAuthorizationStore2_get_ApplicationData(This,pbstrApplicationData)  (This)->lpVtbl->get_ApplicationData(This,pbstrApplicationData)
#define IAzAuthorizationStore2_put_ApplicationData(This,bstrApplicationData)  (This)->lpVtbl->put_ApplicationData(This,bstrApplicationData)
#define IAzAuthorizationStore2_get_DomainTimeout(This,plProp)  (This)->lpVtbl->get_DomainTimeout(This,plProp)
#define IAzAuthorizationStore2_put_DomainTimeout(This,lProp)  (This)->lpVtbl->put_DomainTimeout(This,lProp)
#define IAzAuthorizationStore2_get_ScriptEngineTimeout(This,plProp)  (This)->lpVtbl->get_ScriptEngineTimeout(This,plProp)
#define IAzAuthorizationStore2_put_ScriptEngineTimeout(This,lProp)  (This)->lpVtbl->put_ScriptEngineTimeout(This,lProp)
#define IAzAuthorizationStore2_get_MaxScriptEngines(This,plProp)  (This)->lpVtbl->get_MaxScriptEngines(This,plProp)
#define IAzAuthorizationStore2_put_MaxScriptEngines(This,lProp)  (This)->lpVtbl->put_MaxScriptEngines(This,lProp)
#define IAzAuthorizationStore2_get_GenerateAudits(This,pbProp)  (This)->lpVtbl->get_GenerateAudits(This,pbProp)
#define IAzAuthorizationStore2_put_GenerateAudits(This,bProp)  (This)->lpVtbl->put_GenerateAudits(This,bProp)
#define IAzAuthorizationStore2_get_Writable(This,pfProp)  (This)->lpVtbl->get_Writable(This,pfProp)
#define IAzAuthorizationStore2_GetProperty(This,lPropId,varReserved,pvarProp)  (This)->lpVtbl->GetProperty(This,lPropId,varReserved,pvarProp)
#define IAzAuthorizationStore2_SetProperty(This,lPropId,varProp,varReserved)  (This)->lpVtbl->SetProperty(This,lPropId,varProp,varReserved)
#define IAzAuthorizationStore2_AddPropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->AddPropertyItem(This,lPropId,varProp,varReserved)
#define IAzAuthorizationStore2_DeletePropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->DeletePropertyItem(This,lPropId,varProp,varReserved)
#define IAzAuthorizationStore2_get_PolicyAdministrators(This,pvarAdmins)  (This)->lpVtbl->get_PolicyAdministrators(This,pvarAdmins)
#define IAzAuthorizationStore2_get_PolicyReaders(This,pvarReaders)  (This)->lpVtbl->get_PolicyReaders(This,pvarReaders)
#define IAzAuthorizationStore2_AddPolicyAdministrator(This,bstrAdmin,varReserved)  (This)->lpVtbl->AddPolicyAdministrator(This,bstrAdmin,varReserved)
#define IAzAuthorizationStore2_DeletePolicyAdministrator(This,bstrAdmin,varReserved)  (This)->lpVtbl->DeletePolicyAdministrator(This,bstrAdmin,varReserved)
#define IAzAuthorizationStore2_AddPolicyReader(This,bstrReader,varReserved)  (This)->lpVtbl->AddPolicyReader(This,bstrReader,varReserved)
#define IAzAuthorizationStore2_DeletePolicyReader(This,bstrReader,varReserved)  (This)->lpVtbl->DeletePolicyReader(This,bstrReader,varReserved)
#define IAzAuthorizationStore2_Initialize(This,lFlags,bstrPolicyURL,varReserved)  (This)->lpVtbl->Initialize(This,lFlags,bstrPolicyURL,varReserved)
#define IAzAuthorizationStore2_UpdateCache(This,varReserved)  (This)->lpVtbl->UpdateCache(This,varReserved)
#define IAzAuthorizationStore2_Delete(This,varReserved)  (This)->lpVtbl->Delete(This,varReserved)
#define IAzAuthorizationStore2_get_Applications(This,ppAppCollection)  (This)->lpVtbl->get_Applications(This,ppAppCollection)
#define IAzAuthorizationStore2_OpenApplication(This,bstrApplicationName,varReserved,ppApplication)  (This)->lpVtbl->OpenApplication(This,bstrApplicationName,varReserved,ppApplication)
#define IAzAuthorizationStore2_CreateApplication(This,bstrApplicationName,varReserved,ppApplication)  (This)->lpVtbl->CreateApplication(This,bstrApplicationName,varReserved,ppApplication)
#define IAzAuthorizationStore2_DeleteApplication(This,bstrApplicationName,varReserved)  (This)->lpVtbl->DeleteApplication(This,bstrApplicationName,varReserved)
#define IAzAuthorizationStore2_get_ApplicationGroups(This,ppGroupCollection)  (This)->lpVtbl->get_ApplicationGroups(This,ppGroupCollection)
#define IAzAuthorizationStore2_CreateApplicationGroup(This,bstrGroupName,varReserved,ppGroup)  (This)->lpVtbl->CreateApplicationGroup(This,bstrGroupName,varReserved,ppGroup)
#define IAzAuthorizationStore2_OpenApplicationGroup(This,bstrGroupName,varReserved,ppGroup)  (This)->lpVtbl->OpenApplicationGroup(This,bstrGroupName,varReserved,ppGroup)
#define IAzAuthorizationStore2_DeleteApplicationGroup(This,bstrGroupName,varReserved)  (This)->lpVtbl->DeleteApplicationGroup(This,bstrGroupName,varReserved)
#define IAzAuthorizationStore2_Submit(This,lFlags,varReserved)  (This)->lpVtbl->Submit(This,lFlags,varReserved)
#define IAzAuthorizationStore2_get_DelegatedPolicyUsers(This,pvarDelegatedPolicyUsers)  (This)->lpVtbl->get_DelegatedPolicyUsers(This,pvarDelegatedPolicyUsers)
#define IAzAuthorizationStore2_AddDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->AddDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)
#define IAzAuthorizationStore2_DeleteDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->DeleteDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)
#define IAzAuthorizationStore2_get_TargetMachine(This,pbstrTargetMachine)  (This)->lpVtbl->get_TargetMachine(This,pbstrTargetMachine)
#define IAzAuthorizationStore2_get_ApplyStoreSacl(This,pbApplyStoreSacl)  (This)->lpVtbl->get_ApplyStoreSacl(This,pbApplyStoreSacl)
#define IAzAuthorizationStore2_put_ApplyStoreSacl(This,bApplyStoreSacl)  (This)->lpVtbl->put_ApplyStoreSacl(This,bApplyStoreSacl)
#define IAzAuthorizationStore2_get_PolicyAdministratorsName(This,pvarAdmins)  (This)->lpVtbl->get_PolicyAdministratorsName(This,pvarAdmins)
#define IAzAuthorizationStore2_get_PolicyReadersName(This,pvarReaders)  (This)->lpVtbl->get_PolicyReadersName(This,pvarReaders)
#define IAzAuthorizationStore2_AddPolicyAdministratorName(This,bstrAdmin,varReserved)  (This)->lpVtbl->AddPolicyAdministratorName(This,bstrAdmin,varReserved)
#define IAzAuthorizationStore2_DeletePolicyAdministratorName(This,bstrAdmin,varReserved)  (This)->lpVtbl->DeletePolicyAdministratorName(This,bstrAdmin,varReserved)
#define IAzAuthorizationStore2_AddPolicyReaderName(This,bstrReader,varReserved)  (This)->lpVtbl->AddPolicyReaderName(This,bstrReader,varReserved)
#define IAzAuthorizationStore2_DeletePolicyReaderName(This,bstrReader,varReserved)  (This)->lpVtbl->DeletePolicyReaderName(This,bstrReader,varReserved)
#define IAzAuthorizationStore2_get_DelegatedPolicyUsersName(This,pvarDelegatedPolicyUsers)  (This)->lpVtbl->get_DelegatedPolicyUsersName(This,pvarDelegatedPolicyUsers)
#define IAzAuthorizationStore2_AddDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->AddDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)
#define IAzAuthorizationStore2_DeleteDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->DeleteDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)
#define IAzAuthorizationStore2_CloseApplication(This,bstrApplicationName,lFlag)  (This)->lpVtbl->CloseApplication(This,bstrApplicationName,lFlag)
#define IAzAuthorizationStore2_OpenApplication2(This,bstrApplicationName,varReserved,ppApplication)  (This)->lpVtbl->OpenApplication2(This,bstrApplicationName,varReserved,ppApplication)
#define IAzAuthorizationStore2_CreateApplication2(This,bstrApplicationName,varReserved,ppApplication)  (This)->lpVtbl->CreateApplication2(This,bstrApplicationName,varReserved,ppApplication)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzAuthorizationStore2_OpenApplication2_Proxy(IAzAuthorizationStore2 * This, BSTR bstrApplicationName, VARIANT varReserved, IAzApplication2 ** ppApplication);
void __RPC_STUB IAzAuthorizationStore2_OpenApplication2_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzAuthorizationStore2_CreateApplication2_Proxy(IAzAuthorizationStore2 * This, BSTR bstrApplicationName, VARIANT varReserved, IAzApplication2 ** ppApplication);
void __RPC_STUB IAzAuthorizationStore2_CreateApplication2_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzApplication_INTERFACE_DEFINED__
#define __IAzApplication_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzApplication;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("987bc7c7-b813-4d27-bede-6ba5ae867e95")IAzApplication:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * pbstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Name(BSTR bstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Description(BSTR * pbstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Description(BSTR bstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ApplicationData(BSTR * pbstrApplicationData) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ApplicationData(BSTR bstrApplicationData) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_AuthzInterfaceClsid(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_AuthzInterfaceClsid(BSTR bstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Version(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Version(BSTR bstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_GenerateAudits(BOOL * pbProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_GenerateAudits(BOOL bProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ApplyStoreSacl(BOOL * pbProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ApplyStoreSacl(BOOL bProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Writable(BOOL * pfProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetProperty(LONG lPropId, VARIANT varReserved, VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetProperty(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PolicyAdministrators(VARIANT * pvarAdmins) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PolicyReaders(VARIANT * pvarReaders) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPolicyAdministrator(BSTR bstrAdmin, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePolicyAdministrator(BSTR bstrAdmin, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPolicyReader(BSTR bstrReader, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePolicyReader(BSTR bstrReader, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Scopes(IAzScopes ** ppScopeCollection) = 0;
	virtual HRESULT STDMETHODCALLTYPE OpenScope(BSTR bstrScopeName, VARIANT varReserved, IAzScope ** ppScope) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateScope(BSTR bstrScopeName, VARIANT varReserved, IAzScope ** ppScope) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteScope(BSTR bstrScopeName, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Operations(IAzOperations ** ppOperationCollection) = 0;
	virtual HRESULT STDMETHODCALLTYPE OpenOperation(BSTR bstrOperationName, VARIANT varReserved, IAzOperation ** ppOperation) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateOperation(BSTR bstrOperationName, VARIANT varReserved, IAzOperation ** ppOperation) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteOperation(BSTR bstrOperationName, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Tasks(IAzTasks ** ppTaskCollection) = 0;
	virtual HRESULT STDMETHODCALLTYPE OpenTask(BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateTask(BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteTask(BSTR bstrTaskName, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ApplicationGroups(IAzApplicationGroups ** ppGroupCollection) = 0;
	virtual HRESULT STDMETHODCALLTYPE OpenApplicationGroup(BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateApplicationGroup(BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteApplicationGroup(BSTR bstrGroupName, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Roles(IAzRoles ** ppRoleCollection) = 0;
	virtual HRESULT STDMETHODCALLTYPE OpenRole(BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateRole(BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteRole(BSTR bstrRoleName, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE InitializeClientContextFromToken(ULONGLONG ullTokenHandle, VARIANT varReserved, IAzClientContext ** ppClientContext) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPropertyItem(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePropertyItem(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE Submit(LONG lFlags, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE InitializeClientContextFromName(BSTR ClientName, BSTR DomainName, VARIANT varReserved, IAzClientContext ** ppClientContext) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DelegatedPolicyUsers(VARIANT * pvarDelegatedPolicyUsers) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddDelegatedPolicyUser(BSTR bstrDelegatedPolicyUser, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteDelegatedPolicyUser(BSTR bstrDelegatedPolicyUser, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE InitializeClientContextFromStringSid(BSTR SidString, LONG lOptions, VARIANT varReserved, IAzClientContext ** ppClientContext) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PolicyAdministratorsName(VARIANT * pvarAdmins) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PolicyReadersName(VARIANT * pvarReaders) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPolicyAdministratorName(BSTR bstrAdmin, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePolicyAdministratorName(BSTR bstrAdmin, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPolicyReaderName(BSTR bstrReader, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePolicyReaderName(BSTR bstrReader, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_DelegatedPolicyUsersName(VARIANT * pvarDelegatedPolicyUsers) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddDelegatedPolicyUserName(BSTR bstrDelegatedPolicyUser, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteDelegatedPolicyUserName(BSTR bstrDelegatedPolicyUser, VARIANT varReserved) = 0;
};
#else
typedef struct IAzApplicationVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzApplication * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzApplication * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzApplication * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzApplication * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzApplication * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzApplication * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzApplication * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Name)(IAzApplication * This, BSTR * pbstrName);
	HRESULT(STDMETHODCALLTYPE *put_Name)(IAzApplication * This, BSTR bstrName);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IAzApplication * This, BSTR * pbstrDescription);
	HRESULT(STDMETHODCALLTYPE *put_Description)(IAzApplication * This, BSTR bstrDescription);
	HRESULT(STDMETHODCALLTYPE *get_ApplicationData)(IAzApplication * This, BSTR * pbstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *put_ApplicationData)(IAzApplication * This, BSTR bstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *get_AuthzInterfaceClsid)(IAzApplication * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *put_AuthzInterfaceClsid)(IAzApplication * This, BSTR bstrProp);
	HRESULT(STDMETHODCALLTYPE *get_Version)(IAzApplication * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *put_Version)(IAzApplication * This, BSTR bstrProp);
	HRESULT(STDMETHODCALLTYPE *get_GenerateAudits)(IAzApplication * This, BOOL * pbProp);
	HRESULT(STDMETHODCALLTYPE *put_GenerateAudits)(IAzApplication * This, BOOL bProp);
	HRESULT(STDMETHODCALLTYPE *get_ApplyStoreSacl)(IAzApplication * This, BOOL * pbProp);
	HRESULT(STDMETHODCALLTYPE *put_ApplyStoreSacl)(IAzApplication * This, BOOL bProp);
	HRESULT(STDMETHODCALLTYPE *get_Writable)(IAzApplication * This, BOOL * pfProp);
	HRESULT(STDMETHODCALLTYPE *GetProperty)(IAzApplication * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *SetProperty)(IAzApplication * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_PolicyAdministrators)(IAzApplication * This, VARIANT * pvarAdmins);
	HRESULT(STDMETHODCALLTYPE *get_PolicyReaders)(IAzApplication * This, VARIANT * pvarReaders);
	HRESULT(STDMETHODCALLTYPE *AddPolicyAdministrator)(IAzApplication * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyAdministrator)(IAzApplication * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPolicyReader)(IAzApplication * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyReader)(IAzApplication * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Scopes)(IAzApplication * This, IAzScopes ** ppScopeCollection);
	HRESULT(STDMETHODCALLTYPE *OpenScope)(IAzApplication * This, BSTR bstrScopeName, VARIANT varReserved, IAzScope ** ppScope);
	HRESULT(STDMETHODCALLTYPE *CreateScope)(IAzApplication * This, BSTR bstrScopeName, VARIANT varReserved, IAzScope ** ppScope);
	HRESULT(STDMETHODCALLTYPE *DeleteScope)(IAzApplication * This, BSTR bstrScopeName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Operations)(IAzApplication * This, IAzOperations ** ppOperationCollection);
	HRESULT(STDMETHODCALLTYPE *OpenOperation)(IAzApplication * This, BSTR bstrOperationName, VARIANT varReserved, IAzOperation ** ppOperation);
	HRESULT(STDMETHODCALLTYPE *CreateOperation)(IAzApplication * This, BSTR bstrOperationName, VARIANT varReserved, IAzOperation ** ppOperation);
	HRESULT(STDMETHODCALLTYPE *DeleteOperation)(IAzApplication * This, BSTR bstrOperationName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Tasks)(IAzApplication * This, IAzTasks ** ppTaskCollection);
	HRESULT(STDMETHODCALLTYPE *OpenTask)(IAzApplication * This, BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask);
	HRESULT(STDMETHODCALLTYPE *CreateTask)(IAzApplication * This, BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask);
	HRESULT(STDMETHODCALLTYPE *DeleteTask)(IAzApplication * This, BSTR bstrTaskName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_ApplicationGroups)(IAzApplication * This, IAzApplicationGroups ** ppGroupCollection);
	HRESULT(STDMETHODCALLTYPE *OpenApplicationGroup)(IAzApplication * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
	HRESULT(STDMETHODCALLTYPE *CreateApplicationGroup)(IAzApplication * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
	HRESULT(STDMETHODCALLTYPE *DeleteApplicationGroup)(IAzApplication * This, BSTR bstrGroupName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Roles)(IAzApplication * This, IAzRoles ** ppRoleCollection);
	HRESULT(STDMETHODCALLTYPE *OpenRole)(IAzApplication * This, BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole);
	HRESULT(STDMETHODCALLTYPE *CreateRole)(IAzApplication * This, BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole);
	HRESULT(STDMETHODCALLTYPE *DeleteRole)(IAzApplication * This, BSTR bstrRoleName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *InitializeClientContextFromToken)(IAzApplication * This, ULONGLONG ullTokenHandle, VARIANT varReserved, IAzClientContext ** ppClientContext);
	HRESULT(STDMETHODCALLTYPE *AddPropertyItem)(IAzApplication * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePropertyItem)(IAzApplication * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *Submit)(IAzApplication * This, LONG lFlags, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *InitializeClientContextFromName)(IAzApplication * This, BSTR ClientName, BSTR DomainName, VARIANT varReserved, IAzClientContext ** ppClientContext);
	HRESULT(STDMETHODCALLTYPE *get_DelegatedPolicyUsers)(IAzApplication * This, VARIANT * pvarDelegatedPolicyUsers);
	HRESULT(STDMETHODCALLTYPE *AddDelegatedPolicyUser)(IAzApplication * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteDelegatedPolicyUser)(IAzApplication * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *InitializeClientContextFromStringSid)(IAzApplication * This, BSTR SidString, LONG lOptions, VARIANT varReserved, IAzClientContext ** ppClientContext);
	HRESULT(STDMETHODCALLTYPE *get_PolicyAdministratorsName)(IAzApplication * This, VARIANT * pvarAdmins);
	HRESULT(STDMETHODCALLTYPE *get_PolicyReadersName)(IAzApplication * This, VARIANT * pvarReaders);
	HRESULT(STDMETHODCALLTYPE *AddPolicyAdministratorName)(IAzApplication * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyAdministratorName)(IAzApplication * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPolicyReaderName)(IAzApplication * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyReaderName)(IAzApplication * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_DelegatedPolicyUsersName)(IAzApplication * This, VARIANT * pvarDelegatedPolicyUsers);
	HRESULT(STDMETHODCALLTYPE *AddDelegatedPolicyUserName)(IAzApplication * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteDelegatedPolicyUserName)(IAzApplication * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	END_INTERFACE
} IAzApplicationVtbl;

interface IAzApplication {
    CONST_VTBL struct IAzApplicationVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzApplication_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzApplication_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzApplication_Release(This)  (This)->lpVtbl->Release(This)
#define IAzApplication_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzApplication_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzApplication_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzApplication_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzApplication_get_Name(This,pbstrName)  (This)->lpVtbl->get_Name(This,pbstrName)
#define IAzApplication_put_Name(This,bstrName)  (This)->lpVtbl->put_Name(This,bstrName)
#define IAzApplication_get_Description(This,pbstrDescription)  (This)->lpVtbl->get_Description(This,pbstrDescription)
#define IAzApplication_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IAzApplication_get_ApplicationData(This,pbstrApplicationData)  (This)->lpVtbl->get_ApplicationData(This,pbstrApplicationData)
#define IAzApplication_put_ApplicationData(This,bstrApplicationData)  (This)->lpVtbl->put_ApplicationData(This,bstrApplicationData)
#define IAzApplication_get_AuthzInterfaceClsid(This,pbstrProp)  (This)->lpVtbl->get_AuthzInterfaceClsid(This,pbstrProp)
#define IAzApplication_put_AuthzInterfaceClsid(This,bstrProp)  (This)->lpVtbl->put_AuthzInterfaceClsid(This,bstrProp)
#define IAzApplication_get_Version(This,pbstrProp)  (This)->lpVtbl->get_Version(This,pbstrProp)
#define IAzApplication_put_Version(This,bstrProp)  (This)->lpVtbl->put_Version(This,bstrProp)
#define IAzApplication_get_GenerateAudits(This,pbProp)  (This)->lpVtbl->get_GenerateAudits(This,pbProp)
#define IAzApplication_put_GenerateAudits(This,bProp)  (This)->lpVtbl->put_GenerateAudits(This,bProp)
#define IAzApplication_get_ApplyStoreSacl(This,pbProp)  (This)->lpVtbl->get_ApplyStoreSacl(This,pbProp)
#define IAzApplication_put_ApplyStoreSacl(This,bProp)  (This)->lpVtbl->put_ApplyStoreSacl(This,bProp)
#define IAzApplication_get_Writable(This,pfProp)  (This)->lpVtbl->get_Writable(This,pfProp)
#define IAzApplication_GetProperty(This,lPropId,varReserved,pvarProp)  (This)->lpVtbl->GetProperty(This,lPropId,varReserved,pvarProp)
#define IAzApplication_SetProperty(This,lPropId,varProp,varReserved)  (This)->lpVtbl->SetProperty(This,lPropId,varProp,varReserved)
#define IAzApplication_get_PolicyAdministrators(This,pvarAdmins)  (This)->lpVtbl->get_PolicyAdministrators(This,pvarAdmins)
#define IAzApplication_get_PolicyReaders(This,pvarReaders)  (This)->lpVtbl->get_PolicyReaders(This,pvarReaders)
#define IAzApplication_AddPolicyAdministrator(This,bstrAdmin,varReserved)  (This)->lpVtbl->AddPolicyAdministrator(This,bstrAdmin,varReserved)
#define IAzApplication_DeletePolicyAdministrator(This,bstrAdmin,varReserved)  (This)->lpVtbl->DeletePolicyAdministrator(This,bstrAdmin,varReserved)
#define IAzApplication_AddPolicyReader(This,bstrReader,varReserved)  (This)->lpVtbl->AddPolicyReader(This,bstrReader,varReserved)
#define IAzApplication_DeletePolicyReader(This,bstrReader,varReserved)  (This)->lpVtbl->DeletePolicyReader(This,bstrReader,varReserved)
#define IAzApplication_get_Scopes(This,ppScopeCollection)  (This)->lpVtbl->get_Scopes(This,ppScopeCollection)
#define IAzApplication_OpenScope(This,bstrScopeName,varReserved,ppScope)  (This)->lpVtbl->OpenScope(This,bstrScopeName,varReserved,ppScope)
#define IAzApplication_CreateScope(This,bstrScopeName,varReserved,ppScope)  (This)->lpVtbl->CreateScope(This,bstrScopeName,varReserved,ppScope)
#define IAzApplication_DeleteScope(This,bstrScopeName,varReserved)  (This)->lpVtbl->DeleteScope(This,bstrScopeName,varReserved)
#define IAzApplication_get_Operations(This,ppOperationCollection)  (This)->lpVtbl->get_Operations(This,ppOperationCollection)
#define IAzApplication_OpenOperation(This,bstrOperationName,varReserved,ppOperation)  (This)->lpVtbl->OpenOperation(This,bstrOperationName,varReserved,ppOperation)
#define IAzApplication_CreateOperation(This,bstrOperationName,varReserved,ppOperation)  (This)->lpVtbl->CreateOperation(This,bstrOperationName,varReserved,ppOperation)
#define IAzApplication_DeleteOperation(This,bstrOperationName,varReserved)  (This)->lpVtbl->DeleteOperation(This,bstrOperationName,varReserved)
#define IAzApplication_get_Tasks(This,ppTaskCollection)  (This)->lpVtbl->get_Tasks(This,ppTaskCollection)
#define IAzApplication_OpenTask(This,bstrTaskName,varReserved,ppTask)  (This)->lpVtbl->OpenTask(This,bstrTaskName,varReserved,ppTask)
#define IAzApplication_CreateTask(This,bstrTaskName,varReserved,ppTask)  (This)->lpVtbl->CreateTask(This,bstrTaskName,varReserved,ppTask)
#define IAzApplication_DeleteTask(This,bstrTaskName,varReserved)  (This)->lpVtbl->DeleteTask(This,bstrTaskName,varReserved)
#define IAzApplication_get_ApplicationGroups(This,ppGroupCollection)  (This)->lpVtbl->get_ApplicationGroups(This,ppGroupCollection)
#define IAzApplication_OpenApplicationGroup(This,bstrGroupName,varReserved,ppGroup)  (This)->lpVtbl->OpenApplicationGroup(This,bstrGroupName,varReserved,ppGroup)
#define IAzApplication_CreateApplicationGroup(This,bstrGroupName,varReserved,ppGroup)  (This)->lpVtbl->CreateApplicationGroup(This,bstrGroupName,varReserved,ppGroup)
#define IAzApplication_DeleteApplicationGroup(This,bstrGroupName,varReserved)  (This)->lpVtbl->DeleteApplicationGroup(This,bstrGroupName,varReserved)
#define IAzApplication_get_Roles(This,ppRoleCollection)  (This)->lpVtbl->get_Roles(This,ppRoleCollection)
#define IAzApplication_OpenRole(This,bstrRoleName,varReserved,ppRole)  (This)->lpVtbl->OpenRole(This,bstrRoleName,varReserved,ppRole)
#define IAzApplication_CreateRole(This,bstrRoleName,varReserved,ppRole)  (This)->lpVtbl->CreateRole(This,bstrRoleName,varReserved,ppRole)
#define IAzApplication_DeleteRole(This,bstrRoleName,varReserved)  (This)->lpVtbl->DeleteRole(This,bstrRoleName,varReserved)
#define IAzApplication_InitializeClientContextFromToken(This,ullTokenHandle,varReserved,ppClientContext)  (This)->lpVtbl->InitializeClientContextFromToken(This,ullTokenHandle,varReserved,ppClientContext)
#define IAzApplication_AddPropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->AddPropertyItem(This,lPropId,varProp,varReserved)
#define IAzApplication_DeletePropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->DeletePropertyItem(This,lPropId,varProp,varReserved)
#define IAzApplication_Submit(This,lFlags,varReserved)  (This)->lpVtbl->Submit(This,lFlags,varReserved)
#define IAzApplication_InitializeClientContextFromName(This,ClientName,DomainName,varReserved,ppClientContext)  (This)->lpVtbl->InitializeClientContextFromName(This,ClientName,DomainName,varReserved,ppClientContext)
#define IAzApplication_get_DelegatedPolicyUsers(This,pvarDelegatedPolicyUsers)  (This)->lpVtbl->get_DelegatedPolicyUsers(This,pvarDelegatedPolicyUsers)
#define IAzApplication_AddDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->AddDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)
#define IAzApplication_DeleteDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->DeleteDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)
#define IAzApplication_InitializeClientContextFromStringSid(This,SidString,lOptions,varReserved,ppClientContext)  (This)->lpVtbl->InitializeClientContextFromStringSid(This,SidString,lOptions,varReserved,ppClientContext)
#define IAzApplication_get_PolicyAdministratorsName(This,pvarAdmins)  (This)->lpVtbl->get_PolicyAdministratorsName(This,pvarAdmins)
#define IAzApplication_get_PolicyReadersName(This,pvarReaders)  (This)->lpVtbl->get_PolicyReadersName(This,pvarReaders)
#define IAzApplication_AddPolicyAdministratorName(This,bstrAdmin,varReserved)  (This)->lpVtbl->AddPolicyAdministratorName(This,bstrAdmin,varReserved)
#define IAzApplication_DeletePolicyAdministratorName(This,bstrAdmin,varReserved)  (This)->lpVtbl->DeletePolicyAdministratorName(This,bstrAdmin,varReserved)
#define IAzApplication_AddPolicyReaderName(This,bstrReader,varReserved)  (This)->lpVtbl->AddPolicyReaderName(This,bstrReader,varReserved)
#define IAzApplication_DeletePolicyReaderName(This,bstrReader,varReserved)  (This)->lpVtbl->DeletePolicyReaderName(This,bstrReader,varReserved)
#define IAzApplication_get_DelegatedPolicyUsersName(This,pvarDelegatedPolicyUsers)  (This)->lpVtbl->get_DelegatedPolicyUsersName(This,pvarDelegatedPolicyUsers)
#define IAzApplication_AddDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->AddDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)
#define IAzApplication_DeleteDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->DeleteDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzApplication_get_Name_Proxy(IAzApplication * This, BSTR * pbstrName);
void __RPC_STUB IAzApplication_get_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_put_Name_Proxy(IAzApplication * This, BSTR bstrName);
void __RPC_STUB IAzApplication_put_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_Description_Proxy(IAzApplication * This, BSTR * pbstrDescription);
void __RPC_STUB IAzApplication_get_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_put_Description_Proxy(IAzApplication * This, BSTR bstrDescription);
void __RPC_STUB IAzApplication_put_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_ApplicationData_Proxy(IAzApplication * This, BSTR * pbstrApplicationData);
void __RPC_STUB IAzApplication_get_ApplicationData_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_put_ApplicationData_Proxy(IAzApplication * This, BSTR bstrApplicationData);
void __RPC_STUB IAzApplication_put_ApplicationData_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_AuthzInterfaceClsid_Proxy(IAzApplication * This, BSTR * pbstrProp);
void __RPC_STUB IAzApplication_get_AuthzInterfaceClsid_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_put_AuthzInterfaceClsid_Proxy(IAzApplication * This, BSTR bstrProp);
void __RPC_STUB IAzApplication_put_AuthzInterfaceClsid_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_Version_Proxy(IAzApplication * This, BSTR * pbstrProp);
void __RPC_STUB IAzApplication_get_Version_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_put_Version_Proxy(IAzApplication * This, BSTR bstrProp);
void __RPC_STUB IAzApplication_put_Version_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_GenerateAudits_Proxy(IAzApplication * This, BOOL * pbProp);
void __RPC_STUB IAzApplication_get_GenerateAudits_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_put_GenerateAudits_Proxy(IAzApplication * This, BOOL bProp);
void __RPC_STUB IAzApplication_put_GenerateAudits_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_ApplyStoreSacl_Proxy(IAzApplication * This, BOOL * pbProp);
void __RPC_STUB IAzApplication_get_ApplyStoreSacl_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_put_ApplyStoreSacl_Proxy(IAzApplication * This, BOOL bProp);
void __RPC_STUB IAzApplication_put_ApplyStoreSacl_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_Writable_Proxy(IAzApplication * This, BOOL * pfProp);
void __RPC_STUB IAzApplication_get_Writable_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_GetProperty_Proxy(IAzApplication * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
void __RPC_STUB IAzApplication_GetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_SetProperty_Proxy(IAzApplication * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzApplication_SetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_PolicyAdministrators_Proxy(IAzApplication * This, VARIANT * pvarAdmins);
void __RPC_STUB IAzApplication_get_PolicyAdministrators_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_PolicyReaders_Proxy(IAzApplication * This, VARIANT * pvarReaders);
void __RPC_STUB IAzApplication_get_PolicyReaders_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_AddPolicyAdministrator_Proxy(IAzApplication * This, BSTR bstrAdmin, VARIANT varReserved);
void __RPC_STUB IAzApplication_AddPolicyAdministrator_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_DeletePolicyAdministrator_Proxy(IAzApplication * This, BSTR bstrAdmin, VARIANT varReserved);
void __RPC_STUB IAzApplication_DeletePolicyAdministrator_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_AddPolicyReader_Proxy(IAzApplication * This, BSTR bstrReader, VARIANT varReserved);
void __RPC_STUB IAzApplication_AddPolicyReader_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_DeletePolicyReader_Proxy(IAzApplication * This, BSTR bstrReader, VARIANT varReserved);
void __RPC_STUB IAzApplication_DeletePolicyReader_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_Scopes_Proxy(IAzApplication * This, IAzScopes ** ppScopeCollection);
void __RPC_STUB IAzApplication_get_Scopes_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_OpenScope_Proxy(IAzApplication * This, BSTR bstrScopeName, VARIANT varReserved, IAzScope ** ppScope);
void __RPC_STUB IAzApplication_OpenScope_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_CreateScope_Proxy(IAzApplication * This, BSTR bstrScopeName, VARIANT varReserved, IAzScope ** ppScope);
void __RPC_STUB IAzApplication_CreateScope_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_DeleteScope_Proxy(IAzApplication * This, BSTR bstrScopeName, VARIANT varReserved);
void __RPC_STUB IAzApplication_DeleteScope_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_Operations_Proxy(IAzApplication * This, IAzOperations ** ppOperationCollection);
void __RPC_STUB IAzApplication_get_Operations_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_OpenOperation_Proxy(IAzApplication * This, BSTR bstrOperationName, VARIANT varReserved, IAzOperation ** ppOperation);
void __RPC_STUB IAzApplication_OpenOperation_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_CreateOperation_Proxy(IAzApplication * This, BSTR bstrOperationName, VARIANT varReserved, IAzOperation ** ppOperation);
void __RPC_STUB IAzApplication_CreateOperation_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_DeleteOperation_Proxy(IAzApplication * This, BSTR bstrOperationName, VARIANT varReserved);
void __RPC_STUB IAzApplication_DeleteOperation_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_Tasks_Proxy(IAzApplication * This, IAzTasks ** ppTaskCollection);
void __RPC_STUB IAzApplication_get_Tasks_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_OpenTask_Proxy(IAzApplication * This, BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask);
void __RPC_STUB IAzApplication_OpenTask_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_CreateTask_Proxy(IAzApplication * This, BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask);
void __RPC_STUB IAzApplication_CreateTask_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_DeleteTask_Proxy(IAzApplication * This, BSTR bstrTaskName, VARIANT varReserved);
void __RPC_STUB IAzApplication_DeleteTask_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_ApplicationGroups_Proxy(IAzApplication * This, IAzApplicationGroups ** ppGroupCollection);
void __RPC_STUB IAzApplication_get_ApplicationGroups_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_OpenApplicationGroup_Proxy(IAzApplication * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
void __RPC_STUB IAzApplication_OpenApplicationGroup_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_CreateApplicationGroup_Proxy(IAzApplication * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
void __RPC_STUB IAzApplication_CreateApplicationGroup_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_DeleteApplicationGroup_Proxy(IAzApplication * This, BSTR bstrGroupName, VARIANT varReserved);
void __RPC_STUB IAzApplication_DeleteApplicationGroup_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_Roles_Proxy(IAzApplication * This, IAzRoles ** ppRoleCollection);
void __RPC_STUB IAzApplication_get_Roles_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_OpenRole_Proxy(IAzApplication * This, BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole);
void __RPC_STUB IAzApplication_OpenRole_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_CreateRole_Proxy(IAzApplication * This, BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole);
void __RPC_STUB IAzApplication_CreateRole_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_DeleteRole_Proxy(IAzApplication * This, BSTR bstrRoleName, VARIANT varReserved);
void __RPC_STUB IAzApplication_DeleteRole_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_InitializeClientContextFromToken_Proxy(IAzApplication * This, ULONGLONG ullTokenHandle, VARIANT varReserved, IAzClientContext ** ppClientContext);
void __RPC_STUB IAzApplication_InitializeClientContextFromToken_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_AddPropertyItem_Proxy(IAzApplication * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzApplication_AddPropertyItem_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_DeletePropertyItem_Proxy(IAzApplication * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzApplication_DeletePropertyItem_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_Submit_Proxy(IAzApplication * This, LONG lFlags, VARIANT varReserved);
void __RPC_STUB IAzApplication_Submit_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_InitializeClientContextFromName_Proxy(IAzApplication * This, BSTR ClientName, BSTR DomainName, VARIANT varReserved, IAzClientContext ** ppClientContext);
void __RPC_STUB IAzApplication_InitializeClientContextFromName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_DelegatedPolicyUsers_Proxy(IAzApplication * This, VARIANT * pvarDelegatedPolicyUsers);
void __RPC_STUB IAzApplication_get_DelegatedPolicyUsers_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_AddDelegatedPolicyUser_Proxy(IAzApplication * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
void __RPC_STUB IAzApplication_AddDelegatedPolicyUser_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_DeleteDelegatedPolicyUser_Proxy(IAzApplication * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
void __RPC_STUB IAzApplication_DeleteDelegatedPolicyUser_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_InitializeClientContextFromStringSid_Proxy(IAzApplication * This, BSTR SidString, LONG lOptions, VARIANT varReserved, IAzClientContext ** ppClientContext);
void __RPC_STUB IAzApplication_InitializeClientContextFromStringSid_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_PolicyAdministratorsName_Proxy(IAzApplication * This, VARIANT * pvarAdmins);
void __RPC_STUB IAzApplication_get_PolicyAdministratorsName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_PolicyReadersName_Proxy(IAzApplication * This, VARIANT * pvarReaders);
void __RPC_STUB IAzApplication_get_PolicyReadersName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_AddPolicyAdministratorName_Proxy(IAzApplication * This, BSTR bstrAdmin, VARIANT varReserved);
void __RPC_STUB IAzApplication_AddPolicyAdministratorName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_DeletePolicyAdministratorName_Proxy(IAzApplication * This, BSTR bstrAdmin, VARIANT varReserved);
void __RPC_STUB IAzApplication_DeletePolicyAdministratorName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_AddPolicyReaderName_Proxy(IAzApplication * This, BSTR bstrReader, VARIANT varReserved);
void __RPC_STUB IAzApplication_AddPolicyReaderName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_DeletePolicyReaderName_Proxy(IAzApplication * This, BSTR bstrReader, VARIANT varReserved);
void __RPC_STUB IAzApplication_DeletePolicyReaderName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_get_DelegatedPolicyUsersName_Proxy(IAzApplication * This, VARIANT * pvarDelegatedPolicyUsers);
void __RPC_STUB IAzApplication_get_DelegatedPolicyUsersName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_AddDelegatedPolicyUserName_Proxy(IAzApplication * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
void __RPC_STUB IAzApplication_AddDelegatedPolicyUserName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication_DeleteDelegatedPolicyUserName_Proxy(IAzApplication * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
void __RPC_STUB IAzApplication_DeleteDelegatedPolicyUserName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzApplication2_INTERFACE_DEFINED__
#define __IAzApplication2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzApplication2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("086a68af-a249-437c-b18d-d4d86d6a9660")IAzApplication2:public IAzApplication
{
	public:
	virtual HRESULT STDMETHODCALLTYPE InitializeClientContextFromToken2(ULONG ulTokenHandleLowPart, ULONG ulTokenHandleHighPart, VARIANT varReserved, IAzClientContext2 ** ppClientContext) = 0;
	virtual HRESULT STDMETHODCALLTYPE InitializeClientContext2(BSTR IdentifyingString, VARIANT varReserved, IAzClientContext2 ** ppClientContext) = 0;
};
#else
typedef struct IAzApplication2Vtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzApplication2 * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzApplication2 * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzApplication2 * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzApplication2 * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzApplication2 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzApplication2 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzApplication2 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Name)(IAzApplication2 * This, BSTR * pbstrName);
	HRESULT(STDMETHODCALLTYPE *put_Name)(IAzApplication2 * This, BSTR bstrName);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IAzApplication2 * This, BSTR * pbstrDescription);
	HRESULT(STDMETHODCALLTYPE *put_Description)(IAzApplication2 * This, BSTR bstrDescription);
	HRESULT(STDMETHODCALLTYPE *get_ApplicationData)(IAzApplication2 * This, BSTR * pbstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *put_ApplicationData)(IAzApplication2 * This, BSTR bstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *get_AuthzInterfaceClsid)(IAzApplication2 * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *put_AuthzInterfaceClsid)(IAzApplication2 * This, BSTR bstrProp);
	HRESULT(STDMETHODCALLTYPE *get_Version)(IAzApplication2 * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *put_Version)(IAzApplication2 * This, BSTR bstrProp);
	HRESULT(STDMETHODCALLTYPE *get_GenerateAudits)(IAzApplication2 * This, BOOL * pbProp);
	HRESULT(STDMETHODCALLTYPE *put_GenerateAudits)(IAzApplication2 * This, BOOL bProp);
	HRESULT(STDMETHODCALLTYPE *get_ApplyStoreSacl)(IAzApplication2 * This, BOOL * pbProp);
	HRESULT(STDMETHODCALLTYPE *put_ApplyStoreSacl)(IAzApplication2 * This, BOOL bProp);
	HRESULT(STDMETHODCALLTYPE *get_Writable)(IAzApplication2 * This, BOOL * pfProp);
	HRESULT(STDMETHODCALLTYPE *GetProperty)(IAzApplication2 * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *SetProperty)(IAzApplication2 * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_PolicyAdministrators)(IAzApplication2 * This, VARIANT * pvarAdmins);
	HRESULT(STDMETHODCALLTYPE *get_PolicyReaders)(IAzApplication2 * This, VARIANT * pvarReaders);
	HRESULT(STDMETHODCALLTYPE *AddPolicyAdministrator)(IAzApplication2 * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyAdministrator)(IAzApplication2 * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPolicyReader)(IAzApplication2 * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyReader)(IAzApplication2 * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Scopes)(IAzApplication2 * This, IAzScopes ** ppScopeCollection);
	HRESULT(STDMETHODCALLTYPE *OpenScope)(IAzApplication2 * This, BSTR bstrScopeName, VARIANT varReserved, IAzScope ** ppScope);
	HRESULT(STDMETHODCALLTYPE *CreateScope)(IAzApplication2 * This, BSTR bstrScopeName, VARIANT varReserved, IAzScope ** ppScope);
	HRESULT(STDMETHODCALLTYPE *DeleteScope)(IAzApplication2 * This, BSTR bstrScopeName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Operations)(IAzApplication2 * This, IAzOperations ** ppOperationCollection);
	HRESULT(STDMETHODCALLTYPE *OpenOperation)(IAzApplication2 * This, BSTR bstrOperationName, VARIANT varReserved, IAzOperation ** ppOperation);
	HRESULT(STDMETHODCALLTYPE *CreateOperation)(IAzApplication2 * This, BSTR bstrOperationName, VARIANT varReserved, IAzOperation ** ppOperation);
	HRESULT(STDMETHODCALLTYPE *DeleteOperation)(IAzApplication2 * This, BSTR bstrOperationName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Tasks)(IAzApplication2 * This, IAzTasks ** ppTaskCollection);
	HRESULT(STDMETHODCALLTYPE *OpenTask)(IAzApplication2 * This, BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask);
	HRESULT(STDMETHODCALLTYPE *CreateTask)(IAzApplication2 * This, BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask);
	HRESULT(STDMETHODCALLTYPE *DeleteTask)(IAzApplication2 * This, BSTR bstrTaskName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_ApplicationGroups)(IAzApplication2 * This, IAzApplicationGroups ** ppGroupCollection);
	HRESULT(STDMETHODCALLTYPE *OpenApplicationGroup)(IAzApplication2 * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
	HRESULT(STDMETHODCALLTYPE *CreateApplicationGroup)(IAzApplication2 * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
	HRESULT(STDMETHODCALLTYPE *DeleteApplicationGroup)(IAzApplication2 * This, BSTR bstrGroupName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Roles)(IAzApplication2 * This, IAzRoles ** ppRoleCollection);
	HRESULT(STDMETHODCALLTYPE *OpenRole)(IAzApplication2 * This, BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole);
	HRESULT(STDMETHODCALLTYPE *CreateRole)(IAzApplication2 * This, BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole);
	HRESULT(STDMETHODCALLTYPE *DeleteRole)(IAzApplication2 * This, BSTR bstrRoleName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *InitializeClientContextFromToken)(IAzApplication2 * This, ULONGLONG ullTokenHandle, VARIANT varReserved, IAzClientContext ** ppClientContext);
	HRESULT(STDMETHODCALLTYPE *AddPropertyItem)(IAzApplication2 * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePropertyItem)(IAzApplication2 * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *Submit)(IAzApplication2 * This, LONG lFlags, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *InitializeClientContextFromName)(IAzApplication2 * This, BSTR ClientName, BSTR DomainName, VARIANT varReserved, IAzClientContext ** ppClientContext);
	HRESULT(STDMETHODCALLTYPE *get_DelegatedPolicyUsers)(IAzApplication2 * This, VARIANT * pvarDelegatedPolicyUsers);
	HRESULT(STDMETHODCALLTYPE *AddDelegatedPolicyUser)(IAzApplication2 * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteDelegatedPolicyUser)(IAzApplication2 * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *InitializeClientContextFromStringSid)(IAzApplication2 * This, BSTR SidString, LONG lOptions, VARIANT varReserved, IAzClientContext ** ppClientContext);
	HRESULT(STDMETHODCALLTYPE *get_PolicyAdministratorsName)(IAzApplication2 * This, VARIANT * pvarAdmins);
	HRESULT(STDMETHODCALLTYPE *get_PolicyReadersName)(IAzApplication2 * This, VARIANT * pvarReaders);
	HRESULT(STDMETHODCALLTYPE *AddPolicyAdministratorName)(IAzApplication2 * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyAdministratorName)(IAzApplication2 * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPolicyReaderName)(IAzApplication2 * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyReaderName)(IAzApplication2 * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_DelegatedPolicyUsersName)(IAzApplication2 * This, VARIANT * pvarDelegatedPolicyUsers);
	HRESULT(STDMETHODCALLTYPE *AddDelegatedPolicyUserName)(IAzApplication2 * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteDelegatedPolicyUserName)(IAzApplication2 * This, BSTR bstrDelegatedPolicyUser, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *InitializeClientContextFromToken2)(IAzApplication2 * This, ULONG ulTokenHandleLowPart, ULONG ulTokenHandleHighPart, VARIANT varReserved, IAzClientContext2 ** ppClientContext);
	HRESULT(STDMETHODCALLTYPE *InitializeClientContext2)(IAzApplication2 * This, BSTR IdentifyingString, VARIANT varReserved, IAzClientContext2 ** ppClientContext);
	END_INTERFACE
} IAzApplication2Vtbl;

interface IAzApplication2 {
    CONST_VTBL struct IAzApplication2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzApplication2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzApplication2_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzApplication2_Release(This)  (This)->lpVtbl->Release(This)
#define IAzApplication2_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzApplication2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzApplication2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzApplication2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzApplication2_get_Name(This,pbstrName)  (This)->lpVtbl->get_Name(This,pbstrName)
#define IAzApplication2_put_Name(This,bstrName)  (This)->lpVtbl->put_Name(This,bstrName)
#define IAzApplication2_get_Description(This,pbstrDescription)  (This)->lpVtbl->get_Description(This,pbstrDescription)
#define IAzApplication2_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IAzApplication2_get_ApplicationData(This,pbstrApplicationData)  (This)->lpVtbl->get_ApplicationData(This,pbstrApplicationData)
#define IAzApplication2_put_ApplicationData(This,bstrApplicationData)  (This)->lpVtbl->put_ApplicationData(This,bstrApplicationData)
#define IAzApplication2_get_AuthzInterfaceClsid(This,pbstrProp)  (This)->lpVtbl->get_AuthzInterfaceClsid(This,pbstrProp)
#define IAzApplication2_put_AuthzInterfaceClsid(This,bstrProp)  (This)->lpVtbl->put_AuthzInterfaceClsid(This,bstrProp)
#define IAzApplication2_get_Version(This,pbstrProp)  (This)->lpVtbl->get_Version(This,pbstrProp)
#define IAzApplication2_put_Version(This,bstrProp)  (This)->lpVtbl->put_Version(This,bstrProp)
#define IAzApplication2_get_GenerateAudits(This,pbProp)  (This)->lpVtbl->get_GenerateAudits(This,pbProp)
#define IAzApplication2_put_GenerateAudits(This,bProp)  (This)->lpVtbl->put_GenerateAudits(This,bProp)
#define IAzApplication2_get_ApplyStoreSacl(This,pbProp)  (This)->lpVtbl->get_ApplyStoreSacl(This,pbProp)
#define IAzApplication2_put_ApplyStoreSacl(This,bProp)  (This)->lpVtbl->put_ApplyStoreSacl(This,bProp)
#define IAzApplication2_get_Writable(This,pfProp)  (This)->lpVtbl->get_Writable(This,pfProp)
#define IAzApplication2_GetProperty(This,lPropId,varReserved,pvarProp)  (This)->lpVtbl->GetProperty(This,lPropId,varReserved,pvarProp)
#define IAzApplication2_SetProperty(This,lPropId,varProp,varReserved)  (This)->lpVtbl->SetProperty(This,lPropId,varProp,varReserved)
#define IAzApplication2_get_PolicyAdministrators(This,pvarAdmins)  (This)->lpVtbl->get_PolicyAdministrators(This,pvarAdmins)
#define IAzApplication2_get_PolicyReaders(This,pvarReaders)  (This)->lpVtbl->get_PolicyReaders(This,pvarReaders)
#define IAzApplication2_AddPolicyAdministrator(This,bstrAdmin,varReserved)  (This)->lpVtbl->AddPolicyAdministrator(This,bstrAdmin,varReserved)
#define IAzApplication2_DeletePolicyAdministrator(This,bstrAdmin,varReserved)  (This)->lpVtbl->DeletePolicyAdministrator(This,bstrAdmin,varReserved)
#define IAzApplication2_AddPolicyReader(This,bstrReader,varReserved)  (This)->lpVtbl->AddPolicyReader(This,bstrReader,varReserved)
#define IAzApplication2_DeletePolicyReader(This,bstrReader,varReserved)  (This)->lpVtbl->DeletePolicyReader(This,bstrReader,varReserved)
#define IAzApplication2_get_Scopes(This,ppScopeCollection)  (This)->lpVtbl->get_Scopes(This,ppScopeCollection)
#define IAzApplication2_OpenScope(This,bstrScopeName,varReserved,ppScope)  (This)->lpVtbl->OpenScope(This,bstrScopeName,varReserved,ppScope)
#define IAzApplication2_CreateScope(This,bstrScopeName,varReserved,ppScope)  (This)->lpVtbl->CreateScope(This,bstrScopeName,varReserved,ppScope)
#define IAzApplication2_DeleteScope(This,bstrScopeName,varReserved)  (This)->lpVtbl->DeleteScope(This,bstrScopeName,varReserved)
#define IAzApplication2_get_Operations(This,ppOperationCollection)  (This)->lpVtbl->get_Operations(This,ppOperationCollection)
#define IAzApplication2_OpenOperation(This,bstrOperationName,varReserved,ppOperation)  (This)->lpVtbl->OpenOperation(This,bstrOperationName,varReserved,ppOperation)
#define IAzApplication2_CreateOperation(This,bstrOperationName,varReserved,ppOperation)  (This)->lpVtbl->CreateOperation(This,bstrOperationName,varReserved,ppOperation)
#define IAzApplication2_DeleteOperation(This,bstrOperationName,varReserved)  (This)->lpVtbl->DeleteOperation(This,bstrOperationName,varReserved)
#define IAzApplication2_get_Tasks(This,ppTaskCollection)  (This)->lpVtbl->get_Tasks(This,ppTaskCollection)
#define IAzApplication2_OpenTask(This,bstrTaskName,varReserved,ppTask)  (This)->lpVtbl->OpenTask(This,bstrTaskName,varReserved,ppTask)
#define IAzApplication2_CreateTask(This,bstrTaskName,varReserved,ppTask)  (This)->lpVtbl->CreateTask(This,bstrTaskName,varReserved,ppTask)
#define IAzApplication2_DeleteTask(This,bstrTaskName,varReserved)  (This)->lpVtbl->DeleteTask(This,bstrTaskName,varReserved)
#define IAzApplication2_get_ApplicationGroups(This,ppGroupCollection)  (This)->lpVtbl->get_ApplicationGroups(This,ppGroupCollection)
#define IAzApplication2_OpenApplicationGroup(This,bstrGroupName,varReserved,ppGroup)  (This)->lpVtbl->OpenApplicationGroup(This,bstrGroupName,varReserved,ppGroup)
#define IAzApplication2_CreateApplicationGroup(This,bstrGroupName,varReserved,ppGroup)  (This)->lpVtbl->CreateApplicationGroup(This,bstrGroupName,varReserved,ppGroup)
#define IAzApplication2_DeleteApplicationGroup(This,bstrGroupName,varReserved)  (This)->lpVtbl->DeleteApplicationGroup(This,bstrGroupName,varReserved)
#define IAzApplication2_get_Roles(This,ppRoleCollection)  (This)->lpVtbl->get_Roles(This,ppRoleCollection)
#define IAzApplication2_OpenRole(This,bstrRoleName,varReserved,ppRole)  (This)->lpVtbl->OpenRole(This,bstrRoleName,varReserved,ppRole)
#define IAzApplication2_CreateRole(This,bstrRoleName,varReserved,ppRole)  (This)->lpVtbl->CreateRole(This,bstrRoleName,varReserved,ppRole)
#define IAzApplication2_DeleteRole(This,bstrRoleName,varReserved)  (This)->lpVtbl->DeleteRole(This,bstrRoleName,varReserved)
#define IAzApplication2_InitializeClientContextFromToken(This,ullTokenHandle,varReserved,ppClientContext)  (This)->lpVtbl->InitializeClientContextFromToken(This,ullTokenHandle,varReserved,ppClientContext)
#define IAzApplication2_AddPropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->AddPropertyItem(This,lPropId,varProp,varReserved)
#define IAzApplication2_DeletePropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->DeletePropertyItem(This,lPropId,varProp,varReserved)
#define IAzApplication2_Submit(This,lFlags,varReserved)  (This)->lpVtbl->Submit(This,lFlags,varReserved)
#define IAzApplication2_InitializeClientContextFromName(This,ClientName,DomainName,varReserved,ppClientContext)  (This)->lpVtbl->InitializeClientContextFromName(This,ClientName,DomainName,varReserved,ppClientContext)
#define IAzApplication2_get_DelegatedPolicyUsers(This,pvarDelegatedPolicyUsers)  (This)->lpVtbl->get_DelegatedPolicyUsers(This,pvarDelegatedPolicyUsers)
#define IAzApplication2_AddDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->AddDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)
#define IAzApplication2_DeleteDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->DeleteDelegatedPolicyUser(This,bstrDelegatedPolicyUser,varReserved)
#define IAzApplication2_InitializeClientContextFromStringSid(This,SidString,lOptions,varReserved,ppClientContext)  (This)->lpVtbl->InitializeClientContextFromStringSid(This,SidString,lOptions,varReserved,ppClientContext)
#define IAzApplication2_get_PolicyAdministratorsName(This,pvarAdmins)  (This)->lpVtbl->get_PolicyAdministratorsName(This,pvarAdmins)
#define IAzApplication2_get_PolicyReadersName(This,pvarReaders)  (This)->lpVtbl->get_PolicyReadersName(This,pvarReaders)
#define IAzApplication2_AddPolicyAdministratorName(This,bstrAdmin,varReserved)  (This)->lpVtbl->AddPolicyAdministratorName(This,bstrAdmin,varReserved)
#define IAzApplication2_DeletePolicyAdministratorName(This,bstrAdmin,varReserved)  (This)->lpVtbl->DeletePolicyAdministratorName(This,bstrAdmin,varReserved)
#define IAzApplication2_AddPolicyReaderName(This,bstrReader,varReserved)  (This)->lpVtbl->AddPolicyReaderName(This,bstrReader,varReserved)
#define IAzApplication2_DeletePolicyReaderName(This,bstrReader,varReserved)  (This)->lpVtbl->DeletePolicyReaderName(This,bstrReader,varReserved)
#define IAzApplication2_get_DelegatedPolicyUsersName(This,pvarDelegatedPolicyUsers)  (This)->lpVtbl->get_DelegatedPolicyUsersName(This,pvarDelegatedPolicyUsers)
#define IAzApplication2_AddDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->AddDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)
#define IAzApplication2_DeleteDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)  (This)->lpVtbl->DeleteDelegatedPolicyUserName(This,bstrDelegatedPolicyUser,varReserved)
#define IAzApplication2_InitializeClientContextFromToken2(This,ulTokenHandleLowPart,ulTokenHandleHighPart,varReserved,ppClientContext)  (This)->lpVtbl->InitializeClientContextFromToken2(This,ulTokenHandleLowPart,ulTokenHandleHighPart,varReserved,ppClientContext)
#define IAzApplication2_InitializeClientContext2(This,IdentifyingString,varReserved,ppClientContext)  (This)->lpVtbl->InitializeClientContext2(This,IdentifyingString,varReserved,ppClientContext)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzApplication2_InitializeClientContextFromToken2_Proxy(IAzApplication2 * This, ULONG ulTokenHandleLowPart, ULONG ulTokenHandleHighPart, VARIANT varReserved, IAzClientContext2 ** ppClientContext);
void __RPC_STUB IAzApplication2_InitializeClientContextFromToken2_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplication2_InitializeClientContext2_Proxy(IAzApplication2 * This, BSTR IdentifyingString, VARIANT varReserved, IAzClientContext2 ** ppClientContext);
void __RPC_STUB IAzApplication2_InitializeClientContext2_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzApplications_INTERFACE_DEFINED__
#define __IAzApplications_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzApplications;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("929b11a9-95c5-4a84-a29a-20ad42c2f16c")IAzApplications:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Item(long Index, VARIANT * pvarObtPtr) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Count(long *plCount) = 0;
	virtual HRESULT STDMETHODCALLTYPE get__NewEnum(LPUNKNOWN * ppEnumPtr) = 0;
};
#else
typedef struct IAzApplicationsVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzApplications * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzApplications * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzApplications * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzApplications * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzApplications * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzApplications * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzApplications * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Item)(IAzApplications * This, long Index, VARIANT * pvarObtPtr);
	HRESULT(STDMETHODCALLTYPE *get_Count)(IAzApplications * This, long *plCount);
	HRESULT(STDMETHODCALLTYPE *get__NewEnum)(IAzApplications * This, LPUNKNOWN * ppEnumPtr);
	END_INTERFACE
} IAzApplicationsVtbl;

interface IAzApplications {
    CONST_VTBL struct IAzApplicationsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzApplications_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzApplications_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzApplications_Release(This)  (This)->lpVtbl->Release(This)
#define IAzApplications_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzApplications_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzApplications_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzApplications_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzApplications_get_Item(This,Index,pvarObtPtr)  (This)->lpVtbl->get_Item(This,Index,pvarObtPtr)
#define IAzApplications_get_Count(This,plCount)  (This)->lpVtbl->get_Count(This,plCount)
#define IAzApplications_get__NewEnum(This,ppEnumPtr)  (This)->lpVtbl->get__NewEnum(This,ppEnumPtr)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzApplications_get_Item_Proxy(IAzApplications * This, long Index, VARIANT * pvarObtPtr);
void __RPC_STUB IAzApplications_get_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplications_get_Count_Proxy(IAzApplications * This, long *plCount);
void __RPC_STUB IAzApplications_get_Count_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplications_get__NewEnum_Proxy(IAzApplications * This, LPUNKNOWN * ppEnumPtr);
void __RPC_STUB IAzApplications_get__NewEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzOperation_INTERFACE_DEFINED__
#define __IAzOperation_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzOperation;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("5e56b24f-ea01-4d61-be44-c49b5e4eaf74")IAzOperation:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * pbstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Name(BSTR bstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Description(BSTR * pbstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Description(BSTR bstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ApplicationData(BSTR * pbstrApplicationData) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ApplicationData(BSTR bstrApplicationData) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_OperationID(LONG * plProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_OperationID(LONG lProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Writable(BOOL * pfProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetProperty(LONG lPropId, VARIANT varReserved, VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetProperty(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE Submit(LONG lFlags, VARIANT varReserved) = 0;
};
#else
typedef struct IAzOperationVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzOperation * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzOperation * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzOperation * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzOperation * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzOperation * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzOperation * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzOperation * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Name)(IAzOperation * This, BSTR * pbstrName);
	HRESULT(STDMETHODCALLTYPE *put_Name)(IAzOperation * This, BSTR bstrName);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IAzOperation * This, BSTR * pbstrDescription);
	HRESULT(STDMETHODCALLTYPE *put_Description)(IAzOperation * This, BSTR bstrDescription);
	HRESULT(STDMETHODCALLTYPE *get_ApplicationData)(IAzOperation * This, BSTR * pbstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *put_ApplicationData)(IAzOperation * This, BSTR bstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *get_OperationID)(IAzOperation * This, LONG * plProp);
	HRESULT(STDMETHODCALLTYPE *put_OperationID)(IAzOperation * This, LONG lProp);
	HRESULT(STDMETHODCALLTYPE *get_Writable)(IAzOperation * This, BOOL * pfProp);
	HRESULT(STDMETHODCALLTYPE *GetProperty)(IAzOperation * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *SetProperty)(IAzOperation * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *Submit)(IAzOperation * This, LONG lFlags, VARIANT varReserved);
	END_INTERFACE
} IAzOperationVtbl;

interface IAzOperation {
    CONST_VTBL struct IAzOperationVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzOperation_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzOperation_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzOperation_Release(This)  (This)->lpVtbl->Release(This)
#define IAzOperation_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzOperation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzOperation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzOperation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzOperation_get_Name(This,pbstrName)  (This)->lpVtbl->get_Name(This,pbstrName)
#define IAzOperation_put_Name(This,bstrName)  (This)->lpVtbl->put_Name(This,bstrName)
#define IAzOperation_get_Description(This,pbstrDescription)  (This)->lpVtbl->get_Description(This,pbstrDescription)
#define IAzOperation_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IAzOperation_get_ApplicationData(This,pbstrApplicationData)  (This)->lpVtbl->get_ApplicationData(This,pbstrApplicationData)
#define IAzOperation_put_ApplicationData(This,bstrApplicationData)  (This)->lpVtbl->put_ApplicationData(This,bstrApplicationData)
#define IAzOperation_get_OperationID(This,plProp)  (This)->lpVtbl->get_OperationID(This,plProp)
#define IAzOperation_put_OperationID(This,lProp)  (This)->lpVtbl->put_OperationID(This,lProp)
#define IAzOperation_get_Writable(This,pfProp)  (This)->lpVtbl->get_Writable(This,pfProp)
#define IAzOperation_GetProperty(This,lPropId,varReserved,pvarProp)  (This)->lpVtbl->GetProperty(This,lPropId,varReserved,pvarProp)
#define IAzOperation_SetProperty(This,lPropId,varProp,varReserved)  (This)->lpVtbl->SetProperty(This,lPropId,varProp,varReserved)
#define IAzOperation_Submit(This,lFlags,varReserved)  (This)->lpVtbl->Submit(This,lFlags,varReserved)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzOperation_get_Name_Proxy(IAzOperation * This, BSTR * pbstrName);
void __RPC_STUB IAzOperation_get_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzOperation_put_Name_Proxy(IAzOperation * This, BSTR bstrName);
void __RPC_STUB IAzOperation_put_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzOperation_get_Description_Proxy(IAzOperation * This, BSTR * pbstrDescription);
void __RPC_STUB IAzOperation_get_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzOperation_put_Description_Proxy(IAzOperation * This, BSTR bstrDescription);
void __RPC_STUB IAzOperation_put_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzOperation_get_ApplicationData_Proxy(IAzOperation * This, BSTR * pbstrApplicationData);
void __RPC_STUB IAzOperation_get_ApplicationData_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzOperation_put_ApplicationData_Proxy(IAzOperation * This, BSTR bstrApplicationData);
void __RPC_STUB IAzOperation_put_ApplicationData_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzOperation_get_OperationID_Proxy(IAzOperation * This, LONG * plProp);
void __RPC_STUB IAzOperation_get_OperationID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzOperation_put_OperationID_Proxy(IAzOperation * This, LONG lProp);
void __RPC_STUB IAzOperation_put_OperationID_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzOperation_get_Writable_Proxy(IAzOperation * This, BOOL * pfProp);
void __RPC_STUB IAzOperation_get_Writable_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzOperation_GetProperty_Proxy(IAzOperation * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
void __RPC_STUB IAzOperation_GetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzOperation_SetProperty_Proxy(IAzOperation * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzOperation_SetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzOperation_Submit_Proxy(IAzOperation * This, LONG lFlags, VARIANT varReserved);
void __RPC_STUB IAzOperation_Submit_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzOperations_INTERFACE_DEFINED__
#define __IAzOperations_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzOperations;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("90ef9c07-9706-49d9-af80-0438a5f3ec35")IAzOperations:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Item(LONG Index, VARIANT * pvarObtPtr) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * plCount) = 0;
	virtual HRESULT STDMETHODCALLTYPE get__NewEnum(LPUNKNOWN * ppEnumPtr) = 0;
};
#else
typedef struct IAzOperationsVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzOperations * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzOperations * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzOperations * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzOperations * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzOperations * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzOperations * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzOperations * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Item)(IAzOperations * This, LONG Index, VARIANT * pvarObtPtr);
	HRESULT(STDMETHODCALLTYPE *get_Count)(IAzOperations * This, LONG * plCount);
	HRESULT(STDMETHODCALLTYPE *get__NewEnum)(IAzOperations * This, LPUNKNOWN * ppEnumPtr);
	END_INTERFACE
} IAzOperationsVtbl;

interface IAzOperations {
    CONST_VTBL struct IAzOperationsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzOperations_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzOperations_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzOperations_Release(This)  (This)->lpVtbl->Release(This)
#define IAzOperations_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzOperations_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzOperations_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzOperations_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzOperations_get_Item(This,Index,pvarObtPtr)  (This)->lpVtbl->get_Item(This,Index,pvarObtPtr)
#define IAzOperations_get_Count(This,plCount)  (This)->lpVtbl->get_Count(This,plCount)
#define IAzOperations_get__NewEnum(This,ppEnumPtr)  (This)->lpVtbl->get__NewEnum(This,ppEnumPtr)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzOperations_get_Item_Proxy(IAzOperations * This, LONG Index, VARIANT * pvarObtPtr);
void __RPC_STUB IAzOperations_get_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzOperations_get_Count_Proxy(IAzOperations * This, LONG * plCount);
void __RPC_STUB IAzOperations_get_Count_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzOperations_get__NewEnum_Proxy(IAzOperations * This, LPUNKNOWN * ppEnumPtr);
void __RPC_STUB IAzOperations_get__NewEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzTask_INTERFACE_DEFINED__
#define __IAzTask_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzTask;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("cb94e592-2e0e-4a6c-a336-b89a6dc1e388")IAzTask:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * pbstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Name(BSTR bstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Description(BSTR * pbstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Description(BSTR bstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ApplicationData(BSTR * pbstrApplicationData) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ApplicationData(BSTR bstrApplicationData) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_BizRule(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_BizRule(BSTR bstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_BizRuleLanguage(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_BizRuleLanguage(BSTR bstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_BizRuleImportedPath(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_BizRuleImportedPath(BSTR bstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_IsRoleDefinition(BOOL * pfProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_IsRoleDefinition(BOOL fProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Operations(VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Tasks(VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddOperation(BSTR bstrOp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteOperation(BSTR bstrOp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddTask(BSTR bstrTask, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteTask(BSTR bstrTask, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Writable(BOOL * pfProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetProperty(LONG lPropId, VARIANT varReserved, VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetProperty(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPropertyItem(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePropertyItem(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE Submit(LONG lFlags, VARIANT varReserved) = 0;
};
#else
typedef struct IAzTaskVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzTask * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzTask * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzTask * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzTask * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzTask * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzTask * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzTask * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Name)(IAzTask * This, BSTR * pbstrName);
	HRESULT(STDMETHODCALLTYPE *put_Name)(IAzTask * This, BSTR bstrName);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IAzTask * This, BSTR * pbstrDescription);
	HRESULT(STDMETHODCALLTYPE *put_Description)(IAzTask * This, BSTR bstrDescription);
	HRESULT(STDMETHODCALLTYPE *get_ApplicationData)(IAzTask * This, BSTR * pbstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *put_ApplicationData)(IAzTask * This, BSTR bstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *get_BizRule)(IAzTask * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *put_BizRule)(IAzTask * This, BSTR bstrProp);
	HRESULT(STDMETHODCALLTYPE *get_BizRuleLanguage)(IAzTask * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *put_BizRuleLanguage)(IAzTask * This, BSTR bstrProp);
	HRESULT(STDMETHODCALLTYPE *get_BizRuleImportedPath)(IAzTask * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *put_BizRuleImportedPath)(IAzTask * This, BSTR bstrProp);
	HRESULT(STDMETHODCALLTYPE *get_IsRoleDefinition)(IAzTask * This, BOOL * pfProp);
	HRESULT(STDMETHODCALLTYPE *put_IsRoleDefinition)(IAzTask * This, BOOL fProp);
	HRESULT(STDMETHODCALLTYPE *get_Operations)(IAzTask * This, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *get_Tasks)(IAzTask * This, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *AddOperation)(IAzTask * This, BSTR bstrOp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteOperation)(IAzTask * This, BSTR bstrOp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddTask)(IAzTask * This, BSTR bstrTask, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteTask)(IAzTask * This, BSTR bstrTask, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Writable)(IAzTask * This, BOOL * pfProp);
	HRESULT(STDMETHODCALLTYPE *GetProperty)(IAzTask * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *SetProperty)(IAzTask * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPropertyItem)(IAzTask * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePropertyItem)(IAzTask * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *Submit)(IAzTask * This, LONG lFlags, VARIANT varReserved);
	END_INTERFACE
} IAzTaskVtbl;

interface IAzTask {
    CONST_VTBL struct IAzTaskVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzTask_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzTask_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzTask_Release(This)  (This)->lpVtbl->Release(This)
#define IAzTask_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzTask_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzTask_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzTask_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzTask_get_Name(This,pbstrName)  (This)->lpVtbl->get_Name(This,pbstrName)
#define IAzTask_put_Name(This,bstrName)  (This)->lpVtbl->put_Name(This,bstrName)
#define IAzTask_get_Description(This,pbstrDescription)  (This)->lpVtbl->get_Description(This,pbstrDescription)
#define IAzTask_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IAzTask_get_ApplicationData(This,pbstrApplicationData)  (This)->lpVtbl->get_ApplicationData(This,pbstrApplicationData)
#define IAzTask_put_ApplicationData(This,bstrApplicationData)  (This)->lpVtbl->put_ApplicationData(This,bstrApplicationData)
#define IAzTask_get_BizRule(This,pbstrProp)  (This)->lpVtbl->get_BizRule(This,pbstrProp)
#define IAzTask_put_BizRule(This,bstrProp)  (This)->lpVtbl->put_BizRule(This,bstrProp)
#define IAzTask_get_BizRuleLanguage(This,pbstrProp)  (This)->lpVtbl->get_BizRuleLanguage(This,pbstrProp)
#define IAzTask_put_BizRuleLanguage(This,bstrProp)  (This)->lpVtbl->put_BizRuleLanguage(This,bstrProp)
#define IAzTask_get_BizRuleImportedPath(This,pbstrProp)  (This)->lpVtbl->get_BizRuleImportedPath(This,pbstrProp)
#define IAzTask_put_BizRuleImportedPath(This,bstrProp)  (This)->lpVtbl->put_BizRuleImportedPath(This,bstrProp)
#define IAzTask_get_IsRoleDefinition(This,pfProp)  (This)->lpVtbl->get_IsRoleDefinition(This,pfProp)
#define IAzTask_put_IsRoleDefinition(This,fProp)  (This)->lpVtbl->put_IsRoleDefinition(This,fProp)
#define IAzTask_get_Operations(This,pvarProp)  (This)->lpVtbl->get_Operations(This,pvarProp)
#define IAzTask_get_Tasks(This,pvarProp)  (This)->lpVtbl->get_Tasks(This,pvarProp)
#define IAzTask_AddOperation(This,bstrOp,varReserved)  (This)->lpVtbl->AddOperation(This,bstrOp,varReserved)
#define IAzTask_DeleteOperation(This,bstrOp,varReserved)  (This)->lpVtbl->DeleteOperation(This,bstrOp,varReserved)
#define IAzTask_AddTask(This,bstrTask,varReserved)  (This)->lpVtbl->AddTask(This,bstrTask,varReserved)
#define IAzTask_DeleteTask(This,bstrTask,varReserved)  (This)->lpVtbl->DeleteTask(This,bstrTask,varReserved)
#define IAzTask_get_Writable(This,pfProp)  (This)->lpVtbl->get_Writable(This,pfProp)
#define IAzTask_GetProperty(This,lPropId,varReserved,pvarProp)  (This)->lpVtbl->GetProperty(This,lPropId,varReserved,pvarProp)
#define IAzTask_SetProperty(This,lPropId,varProp,varReserved)  (This)->lpVtbl->SetProperty(This,lPropId,varProp,varReserved)
#define IAzTask_AddPropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->AddPropertyItem(This,lPropId,varProp,varReserved)
#define IAzTask_DeletePropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->DeletePropertyItem(This,lPropId,varProp,varReserved)
#define IAzTask_Submit(This,lFlags,varReserved)  (This)->lpVtbl->Submit(This,lFlags,varReserved)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzTask_get_Name_Proxy(IAzTask * This, BSTR * pbstrName);
void __RPC_STUB IAzTask_get_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_put_Name_Proxy(IAzTask * This, BSTR bstrName);
void __RPC_STUB IAzTask_put_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_get_Description_Proxy(IAzTask * This, BSTR * pbstrDescription);
void __RPC_STUB IAzTask_get_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_put_Description_Proxy(IAzTask * This, BSTR bstrDescription);
void __RPC_STUB IAzTask_put_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_get_ApplicationData_Proxy(IAzTask * This, BSTR * pbstrApplicationData);
void __RPC_STUB IAzTask_get_ApplicationData_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_put_ApplicationData_Proxy(IAzTask * This, BSTR bstrApplicationData);
void __RPC_STUB IAzTask_put_ApplicationData_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_get_BizRule_Proxy(IAzTask * This, BSTR * pbstrProp);
void __RPC_STUB IAzTask_get_BizRule_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_put_BizRule_Proxy(IAzTask * This, BSTR bstrProp);
void __RPC_STUB IAzTask_put_BizRule_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_get_BizRuleLanguage_Proxy(IAzTask * This, BSTR * pbstrProp);
void __RPC_STUB IAzTask_get_BizRuleLanguage_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_put_BizRuleLanguage_Proxy(IAzTask * This, BSTR bstrProp);
void __RPC_STUB IAzTask_put_BizRuleLanguage_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_get_BizRuleImportedPath_Proxy(IAzTask * This, BSTR * pbstrProp);
void __RPC_STUB IAzTask_get_BizRuleImportedPath_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_put_BizRuleImportedPath_Proxy(IAzTask * This, BSTR bstrProp);
void __RPC_STUB IAzTask_put_BizRuleImportedPath_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_get_IsRoleDefinition_Proxy(IAzTask * This, BOOL * pfProp);
void __RPC_STUB IAzTask_get_IsRoleDefinition_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_put_IsRoleDefinition_Proxy(IAzTask * This, BOOL fProp);
void __RPC_STUB IAzTask_put_IsRoleDefinition_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_get_Operations_Proxy(IAzTask * This, VARIANT * pvarProp);
void __RPC_STUB IAzTask_get_Operations_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_get_Tasks_Proxy(IAzTask * This, VARIANT * pvarProp);
void __RPC_STUB IAzTask_get_Tasks_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_AddOperation_Proxy(IAzTask * This, BSTR bstrOp, VARIANT varReserved);
void __RPC_STUB IAzTask_AddOperation_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_DeleteOperation_Proxy(IAzTask * This, BSTR bstrOp, VARIANT varReserved);
void __RPC_STUB IAzTask_DeleteOperation_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_AddTask_Proxy(IAzTask * This, BSTR bstrTask, VARIANT varReserved);
void __RPC_STUB IAzTask_AddTask_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_DeleteTask_Proxy(IAzTask * This, BSTR bstrTask, VARIANT varReserved);
void __RPC_STUB IAzTask_DeleteTask_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_get_Writable_Proxy(IAzTask * This, BOOL * pfProp);
void __RPC_STUB IAzTask_get_Writable_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_GetProperty_Proxy(IAzTask * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
void __RPC_STUB IAzTask_GetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_SetProperty_Proxy(IAzTask * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzTask_SetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_AddPropertyItem_Proxy(IAzTask * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzTask_AddPropertyItem_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_DeletePropertyItem_Proxy(IAzTask * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzTask_DeletePropertyItem_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTask_Submit_Proxy(IAzTask * This, LONG lFlags, VARIANT varReserved);
void __RPC_STUB IAzTask_Submit_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzTasks_INTERFACE_DEFINED__
#define __IAzTasks_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzTasks;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("b338ccab-4c85-4388-8c0a-c58592bad398")IAzTasks:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Item(LONG Index, VARIANT * pvarObtPtr) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * plCount) = 0;
	virtual HRESULT STDMETHODCALLTYPE get__NewEnum(LPUNKNOWN * ppEnumPtr) = 0;
};
#else
typedef struct IAzTasksVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzTasks * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzTasks * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzTasks * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzTasks * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzTasks * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzTasks * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzTasks * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Item)(IAzTasks * This, LONG Index, VARIANT * pvarObtPtr);
	HRESULT(STDMETHODCALLTYPE *get_Count)(IAzTasks * This, LONG * plCount);
	HRESULT(STDMETHODCALLTYPE *get__NewEnum)(IAzTasks * This, LPUNKNOWN * ppEnumPtr);
	END_INTERFACE
} IAzTasksVtbl;

interface IAzTasks {
    CONST_VTBL struct IAzTasksVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzTasks_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzTasks_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzTasks_Release(This)  (This)->lpVtbl->Release(This)
#define IAzTasks_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzTasks_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzTasks_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzTasks_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzTasks_get_Item(This,Index,pvarObtPtr)  (This)->lpVtbl->get_Item(This,Index,pvarObtPtr)
#define IAzTasks_get_Count(This,plCount)  (This)->lpVtbl->get_Count(This,plCount)
#define IAzTasks_get__NewEnum(This,ppEnumPtr)  (This)->lpVtbl->get__NewEnum(This,ppEnumPtr)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzTasks_get_Item_Proxy(IAzTasks * This, LONG Index, VARIANT * pvarObtPtr);
void __RPC_STUB IAzTasks_get_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTasks_get_Count_Proxy(IAzTasks * This, LONG * plCount);
void __RPC_STUB IAzTasks_get_Count_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzTasks_get__NewEnum_Proxy(IAzTasks * This, LPUNKNOWN * ppEnumPtr);
void __RPC_STUB IAzTasks_get__NewEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzScope_INTERFACE_DEFINED__
#define __IAzScope_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzScope;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("00e52487-e08d-4514-b62e-877d5645f5ab")IAzScope:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * pbstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Name(BSTR bstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Description(BSTR * pbstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Description(BSTR bstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ApplicationData(BSTR * pbstrApplicationData) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ApplicationData(BSTR bstrApplicationData) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Writable(BOOL * pfProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetProperty(LONG lPropId, VARIANT varReserved, VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetProperty(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPropertyItem(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePropertyItem(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PolicyAdministrators(VARIANT * pvarAdmins) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PolicyReaders(VARIANT * pvarReaders) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPolicyAdministrator(BSTR bstrAdmin, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePolicyAdministrator(BSTR bstrAdmin, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPolicyReader(BSTR bstrReader, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePolicyReader(BSTR bstrReader, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ApplicationGroups(IAzApplicationGroups ** ppGroupCollection) = 0;
	virtual HRESULT STDMETHODCALLTYPE OpenApplicationGroup(BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateApplicationGroup(BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteApplicationGroup(BSTR bstrGroupName, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Roles(IAzRoles ** ppRoleCollection) = 0;
	virtual HRESULT STDMETHODCALLTYPE OpenRole(BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateRole(BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteRole(BSTR bstrRoleName, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Tasks(IAzTasks ** ppTaskCollection) = 0;
	virtual HRESULT STDMETHODCALLTYPE OpenTask(BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask) = 0;
	virtual HRESULT STDMETHODCALLTYPE CreateTask(BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteTask(BSTR bstrTaskName, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE Submit(LONG lFlags, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_CanBeDelegated(BOOL * pfProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_BizrulesWritable(BOOL * pfProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PolicyAdministratorsName(VARIANT * pvarAdmins) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_PolicyReadersName(VARIANT * pvarReaders) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPolicyAdministratorName(BSTR bstrAdmin, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePolicyAdministratorName(BSTR bstrAdmin, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPolicyReaderName(BSTR bstrReader, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePolicyReaderName(BSTR bstrReader, VARIANT varReserved) = 0;
};
#else
typedef struct IAzScopeVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzScope * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzScope * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzScope * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzScope * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzScope * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzScope * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzScope * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Name)(IAzScope * This, BSTR * pbstrName);
	HRESULT(STDMETHODCALLTYPE *put_Name)(IAzScope * This, BSTR bstrName);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IAzScope * This, BSTR * pbstrDescription);
	HRESULT(STDMETHODCALLTYPE *put_Description)(IAzScope * This, BSTR bstrDescription);
	HRESULT(STDMETHODCALLTYPE *get_ApplicationData)(IAzScope * This, BSTR * pbstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *put_ApplicationData)(IAzScope * This, BSTR bstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *get_Writable)(IAzScope * This, BOOL * pfProp);
	HRESULT(STDMETHODCALLTYPE *GetProperty)(IAzScope * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *SetProperty)(IAzScope * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPropertyItem)(IAzScope * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePropertyItem)(IAzScope * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_PolicyAdministrators)(IAzScope * This, VARIANT * pvarAdmins);
	HRESULT(STDMETHODCALLTYPE *get_PolicyReaders)(IAzScope * This, VARIANT * pvarReaders);
	HRESULT(STDMETHODCALLTYPE *AddPolicyAdministrator)(IAzScope * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyAdministrator)(IAzScope * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPolicyReader)(IAzScope * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyReader)(IAzScope * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_ApplicationGroups)(IAzScope * This, IAzApplicationGroups ** ppGroupCollection);
	HRESULT(STDMETHODCALLTYPE *OpenApplicationGroup)(IAzScope * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
	HRESULT(STDMETHODCALLTYPE *CreateApplicationGroup)(IAzScope * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
	HRESULT(STDMETHODCALLTYPE *DeleteApplicationGroup)(IAzScope * This, BSTR bstrGroupName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Roles)(IAzScope * This, IAzRoles ** ppRoleCollection);
	HRESULT(STDMETHODCALLTYPE *OpenRole)(IAzScope * This, BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole);
	HRESULT(STDMETHODCALLTYPE *CreateRole)(IAzScope * This, BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole);
	HRESULT(STDMETHODCALLTYPE *DeleteRole)(IAzScope * This, BSTR bstrRoleName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Tasks)(IAzScope * This, IAzTasks ** ppTaskCollection);
	HRESULT(STDMETHODCALLTYPE *OpenTask)(IAzScope * This, BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask);
	HRESULT(STDMETHODCALLTYPE *CreateTask)(IAzScope * This, BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask);
	HRESULT(STDMETHODCALLTYPE *DeleteTask)(IAzScope * This, BSTR bstrTaskName, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *Submit)(IAzScope * This, LONG lFlags, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_CanBeDelegated)(IAzScope * This, BOOL * pfProp);
	HRESULT(STDMETHODCALLTYPE *get_BizrulesWritable)(IAzScope * This, BOOL * pfProp);
	HRESULT(STDMETHODCALLTYPE *get_PolicyAdministratorsName)(IAzScope * This, VARIANT * pvarAdmins);
	HRESULT(STDMETHODCALLTYPE *get_PolicyReadersName)(IAzScope * This, VARIANT * pvarReaders);
	HRESULT(STDMETHODCALLTYPE *AddPolicyAdministratorName)(IAzScope * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyAdministratorName)(IAzScope * This, BSTR bstrAdmin, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPolicyReaderName)(IAzScope * This, BSTR bstrReader, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePolicyReaderName)(IAzScope * This, BSTR bstrReader, VARIANT varReserved);
	END_INTERFACE
} IAzScopeVtbl;

interface IAzScope {
    CONST_VTBL struct IAzScopeVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzScope_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzScope_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzScope_Release(This)  (This)->lpVtbl->Release(This)
#define IAzScope_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzScope_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzScope_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzScope_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzScope_get_Name(This,pbstrName)  (This)->lpVtbl->get_Name(This,pbstrName)
#define IAzScope_put_Name(This,bstrName)  (This)->lpVtbl->put_Name(This,bstrName)
#define IAzScope_get_Description(This,pbstrDescription)  (This)->lpVtbl->get_Description(This,pbstrDescription)
#define IAzScope_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IAzScope_get_ApplicationData(This,pbstrApplicationData)  (This)->lpVtbl->get_ApplicationData(This,pbstrApplicationData)
#define IAzScope_put_ApplicationData(This,bstrApplicationData)  (This)->lpVtbl->put_ApplicationData(This,bstrApplicationData)
#define IAzScope_get_Writable(This,pfProp)  (This)->lpVtbl->get_Writable(This,pfProp)
#define IAzScope_GetProperty(This,lPropId,varReserved,pvarProp)  (This)->lpVtbl->GetProperty(This,lPropId,varReserved,pvarProp)
#define IAzScope_SetProperty(This,lPropId,varProp,varReserved)  (This)->lpVtbl->SetProperty(This,lPropId,varProp,varReserved)
#define IAzScope_AddPropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->AddPropertyItem(This,lPropId,varProp,varReserved)
#define IAzScope_DeletePropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->DeletePropertyItem(This,lPropId,varProp,varReserved)
#define IAzScope_get_PolicyAdministrators(This,pvarAdmins)  (This)->lpVtbl->get_PolicyAdministrators(This,pvarAdmins)
#define IAzScope_get_PolicyReaders(This,pvarReaders)  (This)->lpVtbl->get_PolicyReaders(This,pvarReaders)
#define IAzScope_AddPolicyAdministrator(This,bstrAdmin,varReserved)  (This)->lpVtbl->AddPolicyAdministrator(This,bstrAdmin,varReserved)
#define IAzScope_DeletePolicyAdministrator(This,bstrAdmin,varReserved)  (This)->lpVtbl->DeletePolicyAdministrator(This,bstrAdmin,varReserved)
#define IAzScope_AddPolicyReader(This,bstrReader,varReserved)  (This)->lpVtbl->AddPolicyReader(This,bstrReader,varReserved)
#define IAzScope_DeletePolicyReader(This,bstrReader,varReserved)  (This)->lpVtbl->DeletePolicyReader(This,bstrReader,varReserved)
#define IAzScope_get_ApplicationGroups(This,ppGroupCollection)  (This)->lpVtbl->get_ApplicationGroups(This,ppGroupCollection)
#define IAzScope_OpenApplicationGroup(This,bstrGroupName,varReserved,ppGroup)  (This)->lpVtbl->OpenApplicationGroup(This,bstrGroupName,varReserved,ppGroup)
#define IAzScope_CreateApplicationGroup(This,bstrGroupName,varReserved,ppGroup)  (This)->lpVtbl->CreateApplicationGroup(This,bstrGroupName,varReserved,ppGroup)
#define IAzScope_DeleteApplicationGroup(This,bstrGroupName,varReserved)  (This)->lpVtbl->DeleteApplicationGroup(This,bstrGroupName,varReserved)
#define IAzScope_get_Roles(This,ppRoleCollection)  (This)->lpVtbl->get_Roles(This,ppRoleCollection)
#define IAzScope_OpenRole(This,bstrRoleName,varReserved,ppRole)  (This)->lpVtbl->OpenRole(This,bstrRoleName,varReserved,ppRole)
#define IAzScope_CreateRole(This,bstrRoleName,varReserved,ppRole)  (This)->lpVtbl->CreateRole(This,bstrRoleName,varReserved,ppRole)
#define IAzScope_DeleteRole(This,bstrRoleName,varReserved)  (This)->lpVtbl->DeleteRole(This,bstrRoleName,varReserved)
#define IAzScope_get_Tasks(This,ppTaskCollection)  (This)->lpVtbl->get_Tasks(This,ppTaskCollection)
#define IAzScope_OpenTask(This,bstrTaskName,varReserved,ppTask)  (This)->lpVtbl->OpenTask(This,bstrTaskName,varReserved,ppTask)
#define IAzScope_CreateTask(This,bstrTaskName,varReserved,ppTask)  (This)->lpVtbl->CreateTask(This,bstrTaskName,varReserved,ppTask)
#define IAzScope_DeleteTask(This,bstrTaskName,varReserved)  (This)->lpVtbl->DeleteTask(This,bstrTaskName,varReserved)
#define IAzScope_Submit(This,lFlags,varReserved)  (This)->lpVtbl->Submit(This,lFlags,varReserved)
#define IAzScope_get_CanBeDelegated(This,pfProp)  (This)->lpVtbl->get_CanBeDelegated(This,pfProp)
#define IAzScope_get_BizrulesWritable(This,pfProp)  (This)->lpVtbl->get_BizrulesWritable(This,pfProp)
#define IAzScope_get_PolicyAdministratorsName(This,pvarAdmins)  (This)->lpVtbl->get_PolicyAdministratorsName(This,pvarAdmins)
#define IAzScope_get_PolicyReadersName(This,pvarReaders)  (This)->lpVtbl->get_PolicyReadersName(This,pvarReaders)
#define IAzScope_AddPolicyAdministratorName(This,bstrAdmin,varReserved)  (This)->lpVtbl->AddPolicyAdministratorName(This,bstrAdmin,varReserved)
#define IAzScope_DeletePolicyAdministratorName(This,bstrAdmin,varReserved)  (This)->lpVtbl->DeletePolicyAdministratorName(This,bstrAdmin,varReserved)
#define IAzScope_AddPolicyReaderName(This,bstrReader,varReserved)  (This)->lpVtbl->AddPolicyReaderName(This,bstrReader,varReserved)
#define IAzScope_DeletePolicyReaderName(This,bstrReader,varReserved)  (This)->lpVtbl->DeletePolicyReaderName(This,bstrReader,varReserved)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzScope_get_Name_Proxy(IAzScope * This, BSTR * pbstrName);
void __RPC_STUB IAzScope_get_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_put_Name_Proxy(IAzScope * This, BSTR bstrName);
void __RPC_STUB IAzScope_put_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_get_Description_Proxy(IAzScope * This, BSTR * pbstrDescription);
void __RPC_STUB IAzScope_get_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_put_Description_Proxy(IAzScope * This, BSTR bstrDescription);
void __RPC_STUB IAzScope_put_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_get_ApplicationData_Proxy(IAzScope * This, BSTR * pbstrApplicationData);
void __RPC_STUB IAzScope_get_ApplicationData_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_put_ApplicationData_Proxy(IAzScope * This, BSTR bstrApplicationData);
void __RPC_STUB IAzScope_put_ApplicationData_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_get_Writable_Proxy(IAzScope * This, BOOL * pfProp);
void __RPC_STUB IAzScope_get_Writable_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_GetProperty_Proxy(IAzScope * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
void __RPC_STUB IAzScope_GetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_SetProperty_Proxy(IAzScope * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzScope_SetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_AddPropertyItem_Proxy(IAzScope * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzScope_AddPropertyItem_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_DeletePropertyItem_Proxy(IAzScope * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzScope_DeletePropertyItem_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_get_PolicyAdministrators_Proxy(IAzScope * This, VARIANT * pvarAdmins);
void __RPC_STUB IAzScope_get_PolicyAdministrators_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_get_PolicyReaders_Proxy(IAzScope * This, VARIANT * pvarReaders);
void __RPC_STUB IAzScope_get_PolicyReaders_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_AddPolicyAdministrator_Proxy(IAzScope * This, BSTR bstrAdmin, VARIANT varReserved);
void __RPC_STUB IAzScope_AddPolicyAdministrator_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_DeletePolicyAdministrator_Proxy(IAzScope * This, BSTR bstrAdmin, VARIANT varReserved);
void __RPC_STUB IAzScope_DeletePolicyAdministrator_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_AddPolicyReader_Proxy(IAzScope * This, BSTR bstrReader, VARIANT varReserved);
void __RPC_STUB IAzScope_AddPolicyReader_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_DeletePolicyReader_Proxy(IAzScope * This, BSTR bstrReader, VARIANT varReserved);
void __RPC_STUB IAzScope_DeletePolicyReader_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_get_ApplicationGroups_Proxy(IAzScope * This, IAzApplicationGroups ** ppGroupCollection);
void __RPC_STUB IAzScope_get_ApplicationGroups_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_OpenApplicationGroup_Proxy(IAzScope * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
void __RPC_STUB IAzScope_OpenApplicationGroup_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_CreateApplicationGroup_Proxy(IAzScope * This, BSTR bstrGroupName, VARIANT varReserved, IAzApplicationGroup ** ppGroup);
void __RPC_STUB IAzScope_CreateApplicationGroup_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_DeleteApplicationGroup_Proxy(IAzScope * This, BSTR bstrGroupName, VARIANT varReserved);
void __RPC_STUB IAzScope_DeleteApplicationGroup_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_get_Roles_Proxy(IAzScope * This, IAzRoles ** ppRoleCollection);
void __RPC_STUB IAzScope_get_Roles_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_OpenRole_Proxy(IAzScope * This, BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole);
void __RPC_STUB IAzScope_OpenRole_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_CreateRole_Proxy(IAzScope * This, BSTR bstrRoleName, VARIANT varReserved, IAzRole ** ppRole);
void __RPC_STUB IAzScope_CreateRole_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_DeleteRole_Proxy(IAzScope * This, BSTR bstrRoleName, VARIANT varReserved);
void __RPC_STUB IAzScope_DeleteRole_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_get_Tasks_Proxy(IAzScope * This, IAzTasks ** ppTaskCollection);
void __RPC_STUB IAzScope_get_Tasks_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_OpenTask_Proxy(IAzScope * This, BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask);
void __RPC_STUB IAzScope_OpenTask_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_CreateTask_Proxy(IAzScope * This, BSTR bstrTaskName, VARIANT varReserved, IAzTask ** ppTask);
void __RPC_STUB IAzScope_CreateTask_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_DeleteTask_Proxy(IAzScope * This, BSTR bstrTaskName, VARIANT varReserved);
void __RPC_STUB IAzScope_DeleteTask_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_Submit_Proxy(IAzScope * This, LONG lFlags, VARIANT varReserved);
void __RPC_STUB IAzScope_Submit_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_get_CanBeDelegated_Proxy(IAzScope * This, BOOL * pfProp);
void __RPC_STUB IAzScope_get_CanBeDelegated_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_get_BizrulesWritable_Proxy(IAzScope * This, BOOL * pfProp);
void __RPC_STUB IAzScope_get_BizrulesWritable_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_get_PolicyAdministratorsName_Proxy(IAzScope * This, VARIANT * pvarAdmins);
void __RPC_STUB IAzScope_get_PolicyAdministratorsName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_get_PolicyReadersName_Proxy(IAzScope * This, VARIANT * pvarReaders);
void __RPC_STUB IAzScope_get_PolicyReadersName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_AddPolicyAdministratorName_Proxy(IAzScope * This, BSTR bstrAdmin, VARIANT varReserved);
void __RPC_STUB IAzScope_AddPolicyAdministratorName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_DeletePolicyAdministratorName_Proxy(IAzScope * This, BSTR bstrAdmin, VARIANT varReserved);
void __RPC_STUB IAzScope_DeletePolicyAdministratorName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_AddPolicyReaderName_Proxy(IAzScope * This, BSTR bstrReader, VARIANT varReserved);
void __RPC_STUB IAzScope_AddPolicyReaderName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScope_DeletePolicyReaderName_Proxy(IAzScope * This, BSTR bstrReader, VARIANT varReserved);
void __RPC_STUB IAzScope_DeletePolicyReaderName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzScopes_INTERFACE_DEFINED__
#define __IAzScopes_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzScopes;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("78e14853-9f5e-406d-9b91-6bdba6973510")IAzScopes:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Item(LONG Index, VARIANT * pvarObtPtr) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * plCount) = 0;
	virtual HRESULT STDMETHODCALLTYPE get__NewEnum(LPUNKNOWN * ppEnumPtr) = 0;
};
#else
typedef struct IAzScopesVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzScopes * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzScopes * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzScopes * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzScopes * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzScopes * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzScopes * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzScopes * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Item)(IAzScopes * This, LONG Index, VARIANT * pvarObtPtr);
	HRESULT(STDMETHODCALLTYPE *get_Count)(IAzScopes * This, LONG * plCount);
	HRESULT(STDMETHODCALLTYPE *get__NewEnum)(IAzScopes * This, LPUNKNOWN * ppEnumPtr);
	END_INTERFACE
} IAzScopesVtbl;

interface IAzScopes {
    CONST_VTBL struct IAzScopesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzScopes_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzScopes_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzScopes_Release(This)  (This)->lpVtbl->Release(This)
#define IAzScopes_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzScopes_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzScopes_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzScopes_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzScopes_get_Item(This,Index,pvarObtPtr)  (This)->lpVtbl->get_Item(This,Index,pvarObtPtr)
#define IAzScopes_get_Count(This,plCount)  (This)->lpVtbl->get_Count(This,plCount)
#define IAzScopes_get__NewEnum(This,ppEnumPtr)  (This)->lpVtbl->get__NewEnum(This,ppEnumPtr)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzScopes_get_Item_Proxy(IAzScopes * This, LONG Index, VARIANT * pvarObtPtr);
void __RPC_STUB IAzScopes_get_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScopes_get_Count_Proxy(IAzScopes * This, LONG * plCount);
void __RPC_STUB IAzScopes_get_Count_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzScopes_get__NewEnum_Proxy(IAzScopes * This, LPUNKNOWN * ppEnumPtr);
void __RPC_STUB IAzScopes_get__NewEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzApplicationGroup_INTERFACE_DEFINED__
#define __IAzApplicationGroup_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzApplicationGroup;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("f1b744cd-58a6-4e06-9fbf-36f6d779e21e")IAzApplicationGroup:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * pbstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Name(BSTR bstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Type(LONG * plProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Type(LONG lProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_LdapQuery(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_LdapQuery(BSTR bstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_AppMembers(VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_AppNonMembers(VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Members(VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_NonMembers(VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Description(BSTR * pbstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Description(BSTR bstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddAppMember(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteAppMember(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddAppNonMember(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteAppNonMember(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddMember(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteMember(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddNonMember(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteNonMember(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Writable(BOOL * pfProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetProperty(LONG lPropId, VARIANT varReserved, VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetProperty(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPropertyItem(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePropertyItem(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE Submit(LONG lFlags, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddMemberName(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteMemberName(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddNonMemberName(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteNonMemberName(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_MembersName(VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_NonMembersName(VARIANT * pvarProp) = 0;
};
#else
typedef struct IAzApplicationGroupVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzApplicationGroup * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzApplicationGroup * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzApplicationGroup * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzApplicationGroup * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzApplicationGroup * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzApplicationGroup * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzApplicationGroup * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Name)(IAzApplicationGroup * This, BSTR * pbstrName);
	HRESULT(STDMETHODCALLTYPE *put_Name)(IAzApplicationGroup * This, BSTR bstrName);
	HRESULT(STDMETHODCALLTYPE *get_Type)(IAzApplicationGroup * This, LONG * plProp);
	HRESULT(STDMETHODCALLTYPE *put_Type)(IAzApplicationGroup * This, LONG lProp);
	HRESULT(STDMETHODCALLTYPE *get_LdapQuery)(IAzApplicationGroup * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *put_LdapQuery)(IAzApplicationGroup * This, BSTR bstrProp);
	HRESULT(STDMETHODCALLTYPE *get_AppMembers)(IAzApplicationGroup * This, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *get_AppNonMembers)(IAzApplicationGroup * This, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *get_Members)(IAzApplicationGroup * This, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *get_NonMembers)(IAzApplicationGroup * This, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IAzApplicationGroup * This, BSTR * pbstrDescription);
	HRESULT(STDMETHODCALLTYPE *put_Description)(IAzApplicationGroup * This, BSTR bstrDescription);
	HRESULT(STDMETHODCALLTYPE *AddAppMember)(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteAppMember)(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddAppNonMember)(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteAppNonMember)(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddMember)(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteMember)(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddNonMember)(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteNonMember)(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Writable)(IAzApplicationGroup * This, BOOL * pfProp);
	HRESULT(STDMETHODCALLTYPE *GetProperty)(IAzApplicationGroup * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *SetProperty)(IAzApplicationGroup * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddPropertyItem)(IAzApplicationGroup * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePropertyItem)(IAzApplicationGroup * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *Submit)(IAzApplicationGroup * This, LONG lFlags, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddMemberName)(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteMemberName)(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddNonMemberName)(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteNonMemberName)(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_MembersName)(IAzApplicationGroup * This, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *get_NonMembersName)(IAzApplicationGroup * This, VARIANT * pvarProp);
	END_INTERFACE
} IAzApplicationGroupVtbl;

interface IAzApplicationGroup {
    CONST_VTBL struct IAzApplicationGroupVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzApplicationGroup_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzApplicationGroup_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzApplicationGroup_Release(This)  (This)->lpVtbl->Release(This)
#define IAzApplicationGroup_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzApplicationGroup_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzApplicationGroup_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzApplicationGroup_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzApplicationGroup_get_Name(This,pbstrName)  (This)->lpVtbl->get_Name(This,pbstrName)
#define IAzApplicationGroup_put_Name(This,bstrName)  (This)->lpVtbl->put_Name(This,bstrName)
#define IAzApplicationGroup_get_Type(This,plProp)  (This)->lpVtbl->get_Type(This,plProp)
#define IAzApplicationGroup_put_Type(This,lProp)  (This)->lpVtbl->put_Type(This,lProp)
#define IAzApplicationGroup_get_LdapQuery(This,pbstrProp)  (This)->lpVtbl->get_LdapQuery(This,pbstrProp)
#define IAzApplicationGroup_put_LdapQuery(This,bstrProp)  (This)->lpVtbl->put_LdapQuery(This,bstrProp)
#define IAzApplicationGroup_get_AppMembers(This,pvarProp)  (This)->lpVtbl->get_AppMembers(This,pvarProp)
#define IAzApplicationGroup_get_AppNonMembers(This,pvarProp)  (This)->lpVtbl->get_AppNonMembers(This,pvarProp)
#define IAzApplicationGroup_get_Members(This,pvarProp)  (This)->lpVtbl->get_Members(This,pvarProp)
#define IAzApplicationGroup_get_NonMembers(This,pvarProp)  (This)->lpVtbl->get_NonMembers(This,pvarProp)
#define IAzApplicationGroup_get_Description(This,pbstrDescription)  (This)->lpVtbl->get_Description(This,pbstrDescription)
#define IAzApplicationGroup_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IAzApplicationGroup_AddAppMember(This,bstrProp,varReserved)  (This)->lpVtbl->AddAppMember(This,bstrProp,varReserved)
#define IAzApplicationGroup_DeleteAppMember(This,bstrProp,varReserved)  (This)->lpVtbl->DeleteAppMember(This,bstrProp,varReserved)
#define IAzApplicationGroup_AddAppNonMember(This,bstrProp,varReserved)  (This)->lpVtbl->AddAppNonMember(This,bstrProp,varReserved)
#define IAzApplicationGroup_DeleteAppNonMember(This,bstrProp,varReserved)  (This)->lpVtbl->DeleteAppNonMember(This,bstrProp,varReserved)
#define IAzApplicationGroup_AddMember(This,bstrProp,varReserved)  (This)->lpVtbl->AddMember(This,bstrProp,varReserved)
#define IAzApplicationGroup_DeleteMember(This,bstrProp,varReserved)  (This)->lpVtbl->DeleteMember(This,bstrProp,varReserved)
#define IAzApplicationGroup_AddNonMember(This,bstrProp,varReserved)  (This)->lpVtbl->AddNonMember(This,bstrProp,varReserved)
#define IAzApplicationGroup_DeleteNonMember(This,bstrProp,varReserved)  (This)->lpVtbl->DeleteNonMember(This,bstrProp,varReserved)
#define IAzApplicationGroup_get_Writable(This,pfProp)  (This)->lpVtbl->get_Writable(This,pfProp)
#define IAzApplicationGroup_GetProperty(This,lPropId,varReserved,pvarProp)  (This)->lpVtbl->GetProperty(This,lPropId,varReserved,pvarProp)
#define IAzApplicationGroup_SetProperty(This,lPropId,varProp,varReserved)  (This)->lpVtbl->SetProperty(This,lPropId,varProp,varReserved)
#define IAzApplicationGroup_AddPropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->AddPropertyItem(This,lPropId,varProp,varReserved)
#define IAzApplicationGroup_DeletePropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->DeletePropertyItem(This,lPropId,varProp,varReserved)
#define IAzApplicationGroup_Submit(This,lFlags,varReserved)  (This)->lpVtbl->Submit(This,lFlags,varReserved)
#define IAzApplicationGroup_AddMemberName(This,bstrProp,varReserved)  (This)->lpVtbl->AddMemberName(This,bstrProp,varReserved)
#define IAzApplicationGroup_DeleteMemberName(This,bstrProp,varReserved)  (This)->lpVtbl->DeleteMemberName(This,bstrProp,varReserved)
#define IAzApplicationGroup_AddNonMemberName(This,bstrProp,varReserved)  (This)->lpVtbl->AddNonMemberName(This,bstrProp,varReserved)
#define IAzApplicationGroup_DeleteNonMemberName(This,bstrProp,varReserved)  (This)->lpVtbl->DeleteNonMemberName(This,bstrProp,varReserved)
#define IAzApplicationGroup_get_MembersName(This,pvarProp)  (This)->lpVtbl->get_MembersName(This,pvarProp)
#define IAzApplicationGroup_get_NonMembersName(This,pvarProp)  (This)->lpVtbl->get_NonMembersName(This,pvarProp)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzApplicationGroup_get_Name_Proxy(IAzApplicationGroup * This, BSTR * pbstrName);
void __RPC_STUB IAzApplicationGroup_get_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_put_Name_Proxy(IAzApplicationGroup * This, BSTR bstrName);
void __RPC_STUB IAzApplicationGroup_put_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_get_Type_Proxy(IAzApplicationGroup * This, LONG * plProp);
void __RPC_STUB IAzApplicationGroup_get_Type_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_put_Type_Proxy(IAzApplicationGroup * This, LONG lProp);
void __RPC_STUB IAzApplicationGroup_put_Type_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_get_LdapQuery_Proxy(IAzApplicationGroup * This, BSTR * pbstrProp);
void __RPC_STUB IAzApplicationGroup_get_LdapQuery_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_put_LdapQuery_Proxy(IAzApplicationGroup * This, BSTR bstrProp);
void __RPC_STUB IAzApplicationGroup_put_LdapQuery_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_get_AppMembers_Proxy(IAzApplicationGroup * This, VARIANT * pvarProp);
void __RPC_STUB IAzApplicationGroup_get_AppMembers_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_get_AppNonMembers_Proxy(IAzApplicationGroup * This, VARIANT * pvarProp);
void __RPC_STUB IAzApplicationGroup_get_AppNonMembers_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_get_Members_Proxy(IAzApplicationGroup * This, VARIANT * pvarProp);
void __RPC_STUB IAzApplicationGroup_get_Members_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_get_NonMembers_Proxy(IAzApplicationGroup * This, VARIANT * pvarProp);
void __RPC_STUB IAzApplicationGroup_get_NonMembers_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_get_Description_Proxy(IAzApplicationGroup * This, BSTR * pbstrDescription);
void __RPC_STUB IAzApplicationGroup_get_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_put_Description_Proxy(IAzApplicationGroup * This, BSTR bstrDescription);
void __RPC_STUB IAzApplicationGroup_put_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_AddAppMember_Proxy(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_AddAppMember_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_DeleteAppMember_Proxy(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_DeleteAppMember_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_AddAppNonMember_Proxy(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_AddAppNonMember_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_DeleteAppNonMember_Proxy(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_DeleteAppNonMember_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_AddMember_Proxy(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_AddMember_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_DeleteMember_Proxy(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_DeleteMember_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_AddNonMember_Proxy(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_AddNonMember_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_DeleteNonMember_Proxy(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_DeleteNonMember_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_get_Writable_Proxy(IAzApplicationGroup * This, BOOL * pfProp);
void __RPC_STUB IAzApplicationGroup_get_Writable_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_GetProperty_Proxy(IAzApplicationGroup * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
void __RPC_STUB IAzApplicationGroup_GetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_SetProperty_Proxy(IAzApplicationGroup * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_SetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_AddPropertyItem_Proxy(IAzApplicationGroup * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_AddPropertyItem_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_DeletePropertyItem_Proxy(IAzApplicationGroup * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_DeletePropertyItem_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_Submit_Proxy(IAzApplicationGroup * This, LONG lFlags, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_Submit_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_AddMemberName_Proxy(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_AddMemberName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_DeleteMemberName_Proxy(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_DeleteMemberName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_AddNonMemberName_Proxy(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_AddNonMemberName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_DeleteNonMemberName_Proxy(IAzApplicationGroup * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzApplicationGroup_DeleteNonMemberName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_get_MembersName_Proxy(IAzApplicationGroup * This, VARIANT * pvarProp);
void __RPC_STUB IAzApplicationGroup_get_MembersName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroup_get_NonMembersName_Proxy(IAzApplicationGroup * This, VARIANT * pvarProp);
void __RPC_STUB IAzApplicationGroup_get_NonMembersName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzApplicationGroups_INTERFACE_DEFINED__
#define __IAzApplicationGroups_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzApplicationGroups;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("4ce66ad5-9f3c-469d-a911-b99887a7e685")IAzApplicationGroups:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Item(LONG Index, VARIANT * pvarObtPtr) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * plCount) = 0;
	virtual HRESULT STDMETHODCALLTYPE get__NewEnum(LPUNKNOWN * ppEnumPtr) = 0;
};
#else
typedef struct IAzApplicationGroupsVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzApplicationGroups * This, REFIID riid, void **ppvObject);

	ULONG(STDMETHODCALLTYPE *AddRef)(IAzApplicationGroups * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzApplicationGroups * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzApplicationGroups * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzApplicationGroups * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzApplicationGroups * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzApplicationGroups * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Item)(IAzApplicationGroups * This, LONG Index, VARIANT * pvarObtPtr);
	HRESULT(STDMETHODCALLTYPE *get_Count)(IAzApplicationGroups * This, LONG * plCount);
	HRESULT(STDMETHODCALLTYPE *get__NewEnum)(IAzApplicationGroups * This, LPUNKNOWN * ppEnumPtr);
	END_INTERFACE
} IAzApplicationGroupsVtbl;

interface IAzApplicationGroups {
    CONST_VTBL struct IAzApplicationGroupsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzApplicationGroups_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzApplicationGroups_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzApplicationGroups_Release(This)  (This)->lpVtbl->Release(This)
#define IAzApplicationGroups_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzApplicationGroups_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzApplicationGroups_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzApplicationGroups_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzApplicationGroups_get_Item(This,Index,pvarObtPtr)  (This)->lpVtbl->get_Item(This,Index,pvarObtPtr)
#define IAzApplicationGroups_get_Count(This,plCount)  (This)->lpVtbl->get_Count(This,plCount)
#define IAzApplicationGroups_get__NewEnum(This,ppEnumPtr)  (This)->lpVtbl->get__NewEnum(This,ppEnumPtr)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzApplicationGroups_get_Item_Proxy(IAzApplicationGroups * This, LONG Index, VARIANT * pvarObtPtr);
void __RPC_STUB IAzApplicationGroups_get_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroups_get_Count_Proxy(IAzApplicationGroups * This, LONG * plCount);
void __RPC_STUB IAzApplicationGroups_get_Count_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzApplicationGroups_get__NewEnum_Proxy(IAzApplicationGroups * This, LPUNKNOWN * ppEnumPtr);
void __RPC_STUB IAzApplicationGroups_get__NewEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzRole_INTERFACE_DEFINED__
#define __IAzRole_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzRole;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("859e0d8d-62d7-41d8-a034-c0cd5d43fdfa")IAzRole:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Name(BSTR * pbstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Name(BSTR bstrName) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Description(BSTR * pbstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_Description(BSTR bstrDescription) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_ApplicationData(BSTR * pbstrApplicationData) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_ApplicationData(BSTR bstrApplicationData) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddAppMember(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteAppMember(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddTask(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteTask(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddOperation(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteOperation(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddMember(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteMember(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Writable(BOOL * pfProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetProperty(LONG lPropId, VARIANT varReserved, VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE SetProperty(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_AppMembers(VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Members(VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Operations(VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Tasks(VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddPropertyItem(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeletePropertyItem(LONG lPropId, VARIANT varProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE Submit(LONG lFlags, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddMemberName(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE DeleteMemberName(BSTR bstrProp, VARIANT varReserved) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_MembersName(VARIANT * pvarProp) = 0;
};
#else
typedef struct IAzRoleVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzRole * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzRole * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzRole * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzRole * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzRole * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzRole * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzRole * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Name)(IAzRole * This, BSTR * pbstrName);
	HRESULT(STDMETHODCALLTYPE *put_Name)(IAzRole * This, BSTR bstrName);
	HRESULT(STDMETHODCALLTYPE *get_Description)(IAzRole * This, BSTR * pbstrDescription);
	HRESULT(STDMETHODCALLTYPE *put_Description)(IAzRole * This, BSTR bstrDescription);
	HRESULT(STDMETHODCALLTYPE *get_ApplicationData)(IAzRole * This, BSTR * pbstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *put_ApplicationData)(IAzRole * This, BSTR bstrApplicationData);
	HRESULT(STDMETHODCALLTYPE *AddAppMember)(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteAppMember)(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddTask)(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteTask)(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddOperation)(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteOperation)(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddMember)(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteMember)(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_Writable)(IAzRole * This, BOOL * pfProp);
	HRESULT(STDMETHODCALLTYPE *GetProperty)(IAzRole * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *SetProperty)(IAzRole * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_AppMembers)(IAzRole * This, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *get_Members)(IAzRole * This, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *get_Operations)(IAzRole * This, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *get_Tasks)(IAzRole * This, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *AddPropertyItem)(IAzRole * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeletePropertyItem)(IAzRole * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *Submit)(IAzRole * This, LONG lFlags, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *AddMemberName)(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *DeleteMemberName)(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
	HRESULT(STDMETHODCALLTYPE *get_MembersName)(IAzRole * This, VARIANT * pvarProp);
	END_INTERFACE
} IAzRoleVtbl;

interface IAzRole {
    CONST_VTBL struct IAzRoleVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzRole_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzRole_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzRole_Release(This)  (This)->lpVtbl->Release(This)
#define IAzRole_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzRole_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzRole_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzRole_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzRole_get_Name(This,pbstrName)  (This)->lpVtbl->get_Name(This,pbstrName)
#define IAzRole_put_Name(This,bstrName)  (This)->lpVtbl->put_Name(This,bstrName)
#define IAzRole_get_Description(This,pbstrDescription)  (This)->lpVtbl->get_Description(This,pbstrDescription)
#define IAzRole_put_Description(This,bstrDescription)  (This)->lpVtbl->put_Description(This,bstrDescription)
#define IAzRole_get_ApplicationData(This,pbstrApplicationData)  (This)->lpVtbl->get_ApplicationData(This,pbstrApplicationData)
#define IAzRole_put_ApplicationData(This,bstrApplicationData)  (This)->lpVtbl->put_ApplicationData(This,bstrApplicationData)
#define IAzRole_AddAppMember(This,bstrProp,varReserved)  (This)->lpVtbl->AddAppMember(This,bstrProp,varReserved)
#define IAzRole_DeleteAppMember(This,bstrProp,varReserved)  (This)->lpVtbl->DeleteAppMember(This,bstrProp,varReserved)
#define IAzRole_AddTask(This,bstrProp,varReserved)  (This)->lpVtbl->AddTask(This,bstrProp,varReserved)
#define IAzRole_DeleteTask(This,bstrProp,varReserved)  (This)->lpVtbl->DeleteTask(This,bstrProp,varReserved)
#define IAzRole_AddOperation(This,bstrProp,varReserved)  (This)->lpVtbl->AddOperation(This,bstrProp,varReserved)
#define IAzRole_DeleteOperation(This,bstrProp,varReserved)  (This)->lpVtbl->DeleteOperation(This,bstrProp,varReserved)
#define IAzRole_AddMember(This,bstrProp,varReserved)  (This)->lpVtbl->AddMember(This,bstrProp,varReserved)
#define IAzRole_DeleteMember(This,bstrProp,varReserved)  (This)->lpVtbl->DeleteMember(This,bstrProp,varReserved)
#define IAzRole_get_Writable(This,pfProp)  (This)->lpVtbl->get_Writable(This,pfProp)
#define IAzRole_GetProperty(This,lPropId,varReserved,pvarProp)  (This)->lpVtbl->GetProperty(This,lPropId,varReserved,pvarProp)
#define IAzRole_SetProperty(This,lPropId,varProp,varReserved)  (This)->lpVtbl->SetProperty(This,lPropId,varProp,varReserved)
#define IAzRole_get_AppMembers(This,pvarProp)  (This)->lpVtbl->get_AppMembers(This,pvarProp)
#define IAzRole_get_Members(This,pvarProp)  (This)->lpVtbl->get_Members(This,pvarProp)
#define IAzRole_get_Operations(This,pvarProp)  (This)->lpVtbl->get_Operations(This,pvarProp)
#define IAzRole_get_Tasks(This,pvarProp)  (This)->lpVtbl->get_Tasks(This,pvarProp)
#define IAzRole_AddPropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->AddPropertyItem(This,lPropId,varProp,varReserved)
#define IAzRole_DeletePropertyItem(This,lPropId,varProp,varReserved)  (This)->lpVtbl->DeletePropertyItem(This,lPropId,varProp,varReserved)
#define IAzRole_Submit(This,lFlags,varReserved)  (This)->lpVtbl->Submit(This,lFlags,varReserved)
#define IAzRole_AddMemberName(This,bstrProp,varReserved)  (This)->lpVtbl->AddMemberName(This,bstrProp,varReserved)
#define IAzRole_DeleteMemberName(This,bstrProp,varReserved)  (This)->lpVtbl->DeleteMemberName(This,bstrProp,varReserved)
#define IAzRole_get_MembersName(This,pvarProp)  (This)->lpVtbl->get_MembersName(This,pvarProp)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzRole_get_Name_Proxy(IAzRole * This, BSTR * pbstrName);
void __RPC_STUB IAzRole_get_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_put_Name_Proxy(IAzRole * This, BSTR bstrName);
void __RPC_STUB IAzRole_put_Name_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_get_Description_Proxy(IAzRole * This, BSTR * pbstrDescription);
void __RPC_STUB IAzRole_get_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_put_Description_Proxy(IAzRole * This, BSTR bstrDescription);
void __RPC_STUB IAzRole_put_Description_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_get_ApplicationData_Proxy(IAzRole * This, BSTR * pbstrApplicationData);
void __RPC_STUB IAzRole_get_ApplicationData_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_put_ApplicationData_Proxy(IAzRole * This, BSTR bstrApplicationData);
void __RPC_STUB IAzRole_put_ApplicationData_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_AddAppMember_Proxy(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzRole_AddAppMember_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_DeleteAppMember_Proxy(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzRole_DeleteAppMember_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_AddTask_Proxy(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzRole_AddTask_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_DeleteTask_Proxy(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzRole_DeleteTask_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_AddOperation_Proxy(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzRole_AddOperation_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_DeleteOperation_Proxy(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzRole_DeleteOperation_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_AddMember_Proxy(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzRole_AddMember_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_DeleteMember_Proxy(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzRole_DeleteMember_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_get_Writable_Proxy(IAzRole * This, BOOL * pfProp);
void __RPC_STUB IAzRole_get_Writable_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_GetProperty_Proxy(IAzRole * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
void __RPC_STUB IAzRole_GetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_SetProperty_Proxy(IAzRole * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzRole_SetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_get_AppMembers_Proxy(IAzRole * This, VARIANT * pvarProp);
void __RPC_STUB IAzRole_get_AppMembers_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_get_Members_Proxy(IAzRole * This, VARIANT * pvarProp);
void __RPC_STUB IAzRole_get_Members_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_get_Operations_Proxy(IAzRole * This, VARIANT * pvarProp);
void __RPC_STUB IAzRole_get_Operations_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_get_Tasks_Proxy(IAzRole * This, VARIANT * pvarProp);
void __RPC_STUB IAzRole_get_Tasks_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_AddPropertyItem_Proxy(IAzRole * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzRole_AddPropertyItem_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_DeletePropertyItem_Proxy(IAzRole * This, LONG lPropId, VARIANT varProp, VARIANT varReserved);
void __RPC_STUB IAzRole_DeletePropertyItem_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_Submit_Proxy(IAzRole * This, LONG lFlags, VARIANT varReserved);
void __RPC_STUB IAzRole_Submit_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_AddMemberName_Proxy(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzRole_AddMemberName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_DeleteMemberName_Proxy(IAzRole * This, BSTR bstrProp, VARIANT varReserved);
void __RPC_STUB IAzRole_DeleteMemberName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRole_get_MembersName_Proxy(IAzRole * This, VARIANT * pvarProp);
void __RPC_STUB IAzRole_get_MembersName_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzRoles_INTERFACE_DEFINED__
#define __IAzRoles_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzRoles;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("95e0f119-13b4-4dae-b65f-2f7d60d822e4")IAzRoles:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE get_Item(LONG Index, VARIANT * pvarObtPtr) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_Count(LONG * plCount) = 0;
	virtual HRESULT STDMETHODCALLTYPE get__NewEnum(LPUNKNOWN * ppEnumPtr) = 0;
};
#else
typedef struct IAzRolesVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzRoles * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzRoles * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzRoles * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzRoles * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzRoles * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzRoles * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzRoles * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *get_Item)(IAzRoles * This, LONG Index, VARIANT * pvarObtPtr);
	HRESULT(STDMETHODCALLTYPE *get_Count)(IAzRoles * This, LONG * plCount);
	HRESULT(STDMETHODCALLTYPE *get__NewEnum)(IAzRoles * This, LPUNKNOWN * ppEnumPtr);
	END_INTERFACE
} IAzRolesVtbl;

interface IAzRoles {
    CONST_VTBL struct IAzRolesVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzRoles_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzRoles_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzRoles_Release(This)  (This)->lpVtbl->Release(This)
#define IAzRoles_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzRoles_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzRoles_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzRoles_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzRoles_get_Item(This,Index,pvarObtPtr)  (This)->lpVtbl->get_Item(This,Index,pvarObtPtr)
#define IAzRoles_get_Count(This,plCount)  (This)->lpVtbl->get_Count(This,plCount)
#define IAzRoles_get__NewEnum(This,ppEnumPtr)  (This)->lpVtbl->get__NewEnum(This,ppEnumPtr)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzRoles_get_Item_Proxy(IAzRoles * This, LONG Index, VARIANT * pvarObtPtr);
void __RPC_STUB IAzRoles_get_Item_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRoles_get_Count_Proxy(IAzRoles * This, LONG * plCount);
void __RPC_STUB IAzRoles_get_Count_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzRoles_get__NewEnum_Proxy(IAzRoles * This, LPUNKNOWN * ppEnumPtr);
void __RPC_STUB IAzRoles_get__NewEnum_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzClientContext_INTERFACE_DEFINED__
#define __IAzClientContext_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzClientContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("eff1f00b-488a-466d-afd9-a401c5f9eef5")IAzClientContext:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE AccessCheck(BSTR bstrObjectName, VARIANT varScopeNames, VARIANT varOperations, VARIANT varParameterNames, VARIANT varParameterValues, VARIANT varInterfaceNames, VARIANT varInterfaceFlags, VARIANT varInterfaces, VARIANT * pvarResults) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetBusinessRuleString(BSTR * pbstrBusinessRuleString) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UserDn(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UserSamCompat(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UserDisplay(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UserGuid(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UserCanonical(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UserUpn(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_UserDnsSamCompat(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetProperty(LONG lPropId, VARIANT varReserved, VARIANT * pvarProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetRoles(BSTR bstrScopeName, VARIANT * pvarRoleNames) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_RoleForAccessCheck(BSTR * pbstrProp) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_RoleForAccessCheck(BSTR bstrProp) = 0;
};
#else
typedef struct IAzClientContextVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzClientContext * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzClientContext * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzClientContext * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzClientContext * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzClientContext * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzClientContext * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzClientContext * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *AccessCheck)(IAzClientContext * This, BSTR bstrObjectName, VARIANT varScopeNames, VARIANT varOperations, VARIANT varParameterNames, VARIANT varParameterValues, VARIANT varInterfaceNames, VARIANT varInterfaceFlags, VARIANT varInterfaces, VARIANT * pvarResults);
	HRESULT(STDMETHODCALLTYPE *GetBusinessRuleString)(IAzClientContext * This, BSTR * pbstrBusinessRuleString);
	HRESULT(STDMETHODCALLTYPE *get_UserDn)(IAzClientContext * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *get_UserSamCompat)(IAzClientContext * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *get_UserDisplay)(IAzClientContext * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *get_UserGuid)(IAzClientContext * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *get_UserCanonical)(IAzClientContext * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *get_UserUpn)(IAzClientContext * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *get_UserDnsSamCompat)(IAzClientContext * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *GetProperty)(IAzClientContext * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *GetRoles)(IAzClientContext * This, BSTR bstrScopeName, VARIANT * pvarRoleNames);
	HRESULT(STDMETHODCALLTYPE *get_RoleForAccessCheck)(IAzClientContext * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *put_RoleForAccessCheck)(IAzClientContext * This, BSTR bstrProp);
	END_INTERFACE
} IAzClientContextVtbl;

interface IAzClientContext {
    CONST_VTBL struct IAzClientContextVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzClientContext_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzClientContext_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzClientContext_Release(This)  (This)->lpVtbl->Release(This)
#define IAzClientContext_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzClientContext_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzClientContext_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzClientContext_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzClientContext_AccessCheck(This,bstrObjectName,varScopeNames,varOperations,varParameterNames,varParameterValues,varInterfaceNames,varInterfaceFlags,varInterfaces,pvarResults)  (This)->lpVtbl->AccessCheck(This,bstrObjectName,varScopeNames,varOperations,varParameterNames,varParameterValues,varInterfaceNames,varInterfaceFlags,varInterfaces,pvarResults)
#define IAzClientContext_GetBusinessRuleString(This,pbstrBusinessRuleString)  (This)->lpVtbl->GetBusinessRuleString(This,pbstrBusinessRuleString)
#define IAzClientContext_get_UserDn(This,pbstrProp)  (This)->lpVtbl->get_UserDn(This,pbstrProp)
#define IAzClientContext_get_UserSamCompat(This,pbstrProp)  (This)->lpVtbl->get_UserSamCompat(This,pbstrProp)
#define IAzClientContext_get_UserDisplay(This,pbstrProp)  (This)->lpVtbl->get_UserDisplay(This,pbstrProp)
#define IAzClientContext_get_UserGuid(This,pbstrProp)  (This)->lpVtbl->get_UserGuid(This,pbstrProp)
#define IAzClientContext_get_UserCanonical(This,pbstrProp)  (This)->lpVtbl->get_UserCanonical(This,pbstrProp)
#define IAzClientContext_get_UserUpn(This,pbstrProp)  (This)->lpVtbl->get_UserUpn(This,pbstrProp)
#define IAzClientContext_get_UserDnsSamCompat(This,pbstrProp)  (This)->lpVtbl->get_UserDnsSamCompat(This,pbstrProp)
#define IAzClientContext_GetProperty(This,lPropId,varReserved,pvarProp)  (This)->lpVtbl->GetProperty(This,lPropId,varReserved,pvarProp)
#define IAzClientContext_GetRoles(This,bstrScopeName,pvarRoleNames)  (This)->lpVtbl->GetRoles(This,bstrScopeName,pvarRoleNames)
#define IAzClientContext_get_RoleForAccessCheck(This,pbstrProp)  (This)->lpVtbl->get_RoleForAccessCheck(This,pbstrProp)
#define IAzClientContext_put_RoleForAccessCheck(This,bstrProp)  (This)->lpVtbl->put_RoleForAccessCheck(This,bstrProp)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzClientContext_AccessCheck_Proxy(IAzClientContext * This, BSTR bstrObjectName, VARIANT varScopeNames, VARIANT varOperations, VARIANT varParameterNames, VARIANT varParameterValues, VARIANT varInterfaceNames, VARIANT varInterfaceFlags, VARIANT varInterfaces, VARIANT * pvarResults);
void __RPC_STUB IAzClientContext_AccessCheck_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext_GetBusinessRuleString_Proxy(IAzClientContext * This, BSTR * pbstrBusinessRuleString);
void __RPC_STUB IAzClientContext_GetBusinessRuleString_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext_get_UserDn_Proxy(IAzClientContext * This, BSTR * pbstrProp);
void __RPC_STUB IAzClientContext_get_UserDn_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext_get_UserSamCompat_Proxy(IAzClientContext * This, BSTR * pbstrProp);
void __RPC_STUB IAzClientContext_get_UserSamCompat_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext_get_UserDisplay_Proxy(IAzClientContext * This, BSTR * pbstrProp);
void __RPC_STUB IAzClientContext_get_UserDisplay_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext_get_UserGuid_Proxy(IAzClientContext * This, BSTR * pbstrProp);
void __RPC_STUB IAzClientContext_get_UserGuid_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext_get_UserCanonical_Proxy(IAzClientContext * This, BSTR * pbstrProp);
void __RPC_STUB IAzClientContext_get_UserCanonical_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext_get_UserUpn_Proxy(IAzClientContext * This, BSTR * pbstrProp);
void __RPC_STUB IAzClientContext_get_UserUpn_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext_get_UserDnsSamCompat_Proxy(IAzClientContext * This, BSTR * pbstrProp);
void __RPC_STUB IAzClientContext_get_UserDnsSamCompat_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext_GetProperty_Proxy(IAzClientContext * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
void __RPC_STUB IAzClientContext_GetProperty_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext_GetRoles_Proxy(IAzClientContext * This, BSTR bstrScopeName, VARIANT * pvarRoleNames);
void __RPC_STUB IAzClientContext_GetRoles_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext_get_RoleForAccessCheck_Proxy(IAzClientContext * This, BSTR * pbstrProp);
void __RPC_STUB IAzClientContext_get_RoleForAccessCheck_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext_put_RoleForAccessCheck_Proxy(IAzClientContext * This, BSTR bstrProp);
void __RPC_STUB IAzClientContext_put_RoleForAccessCheck_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzClientContext2_INTERFACE_DEFINED__
#define __IAzClientContext2_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzClientContext2;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("2b0c92b8-208a-488a-8f81-e4edb22111cd")IAzClientContext2:public IAzClientContext
{
	public:
	virtual HRESULT STDMETHODCALLTYPE GetAssignedScopesPage(LONG lOptions, LONG PageSize, VARIANT * pvarCursor, VARIANT * pvarScopeNames) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddRoles(VARIANT varRoles, BSTR bstrScopeName) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddApplicationGroups(VARIANT varApplicationGroups) = 0;
	virtual HRESULT STDMETHODCALLTYPE AddStringSids(VARIANT varStringSids) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_LDAPQueryDN(BSTR bstrLDAPQueryDN) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_LDAPQueryDN(BSTR * pbstrLDAPQueryDN) = 0;
};
#else
typedef struct IAzClientContext2Vtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzClientContext2 * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzClientContext2 * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzClientContext2 * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzClientContext2 * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzClientContext2 * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzClientContext2 * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzClientContext2 * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *AccessCheck)(IAzClientContext2 * This, BSTR bstrObjectName, VARIANT varScopeNames, VARIANT varOperations, VARIANT varParameterNames, VARIANT varParameterValues, VARIANT varInterfaceNames, VARIANT varInterfaceFlags, VARIANT varInterfaces, VARIANT * pvarResults);
	HRESULT(STDMETHODCALLTYPE *GetBusinessRuleString)(IAzClientContext2 * This, BSTR * pbstrBusinessRuleString);
	HRESULT(STDMETHODCALLTYPE *get_UserDn)(IAzClientContext2 * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *get_UserSamCompat)(IAzClientContext2 * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *get_UserDisplay)(IAzClientContext2 * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *get_UserGuid)(IAzClientContext2 * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *get_UserCanonical)(IAzClientContext2 * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *get_UserUpn)(IAzClientContext2 * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *get_UserDnsSamCompat)(IAzClientContext2 * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *GetProperty)(IAzClientContext2 * This, LONG lPropId, VARIANT varReserved, VARIANT * pvarProp);
	HRESULT(STDMETHODCALLTYPE *GetRoles)(IAzClientContext2 * This, BSTR bstrScopeName, VARIANT * pvarRoleNames);
	HRESULT(STDMETHODCALLTYPE *get_RoleForAccessCheck)(IAzClientContext2 * This, BSTR * pbstrProp);
	HRESULT(STDMETHODCALLTYPE *put_RoleForAccessCheck)(IAzClientContext2 * This, BSTR bstrProp);
	HRESULT(STDMETHODCALLTYPE *GetAssignedScopesPage)(IAzClientContext2 * This, LONG lOptions, LONG PageSize, VARIANT * pvarCursor, VARIANT * pvarScopeNames);
	HRESULT(STDMETHODCALLTYPE *AddRoles)(IAzClientContext2 * This, VARIANT varRoles, BSTR bstrScopeName);
	HRESULT(STDMETHODCALLTYPE *AddApplicationGroups)(IAzClientContext2 * This, VARIANT varApplicationGroups);
	HRESULT(STDMETHODCALLTYPE *AddStringSids)(IAzClientContext2 * This, VARIANT varStringSids);
	HRESULT(STDMETHODCALLTYPE *put_LDAPQueryDN)(IAzClientContext2 * This, BSTR bstrLDAPQueryDN);
	HRESULT(STDMETHODCALLTYPE *get_LDAPQueryDN)(IAzClientContext2 * This, BSTR * pbstrLDAPQueryDN);
	END_INTERFACE
} IAzClientContext2Vtbl;

interface IAzClientContext2 {
    CONST_VTBL struct IAzClientContext2Vtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzClientContext2_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzClientContext2_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzClientContext2_Release(This)  (This)->lpVtbl->Release(This)
#define IAzClientContext2_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzClientContext2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzClientContext2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzClientContext2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzClientContext2_AccessCheck(This,bstrObjectName,varScopeNames,varOperations,varParameterNames,varParameterValues,varInterfaceNames,varInterfaceFlags,varInterfaces,pvarResults)  (This)->lpVtbl->AccessCheck(This,bstrObjectName,varScopeNames,varOperations,varParameterNames,varParameterValues,varInterfaceNames,varInterfaceFlags,varInterfaces,pvarResults)
#define IAzClientContext2_GetBusinessRuleString(This,pbstrBusinessRuleString)  (This)->lpVtbl->GetBusinessRuleString(This,pbstrBusinessRuleString)
#define IAzClientContext2_get_UserDn(This,pbstrProp)  (This)->lpVtbl->get_UserDn(This,pbstrProp)
#define IAzClientContext2_get_UserSamCompat(This,pbstrProp)  (This)->lpVtbl->get_UserSamCompat(This,pbstrProp)
#define IAzClientContext2_get_UserDisplay(This,pbstrProp)  (This)->lpVtbl->get_UserDisplay(This,pbstrProp)
#define IAzClientContext2_get_UserGuid(This,pbstrProp)  (This)->lpVtbl->get_UserGuid(This,pbstrProp)
#define IAzClientContext2_get_UserCanonical(This,pbstrProp)  (This)->lpVtbl->get_UserCanonical(This,pbstrProp)
#define IAzClientContext2_get_UserUpn(This,pbstrProp)  (This)->lpVtbl->get_UserUpn(This,pbstrProp)
#define IAzClientContext2_get_UserDnsSamCompat(This,pbstrProp)  (This)->lpVtbl->get_UserDnsSamCompat(This,pbstrProp)
#define IAzClientContext2_GetProperty(This,lPropId,varReserved,pvarProp)  (This)->lpVtbl->GetProperty(This,lPropId,varReserved,pvarProp)
#define IAzClientContext2_GetRoles(This,bstrScopeName,pvarRoleNames)  (This)->lpVtbl->GetRoles(This,bstrScopeName,pvarRoleNames)
#define IAzClientContext2_get_RoleForAccessCheck(This,pbstrProp)  (This)->lpVtbl->get_RoleForAccessCheck(This,pbstrProp)
#define IAzClientContext2_put_RoleForAccessCheck(This,bstrProp)  (This)->lpVtbl->put_RoleForAccessCheck(This,bstrProp)
#define IAzClientContext2_GetAssignedScopesPage(This,lOptions,PageSize,pvarCursor,pvarScopeNames)  (This)->lpVtbl->GetAssignedScopesPage(This,lOptions,PageSize,pvarCursor,pvarScopeNames)
#define IAzClientContext2_AddRoles(This,varRoles,bstrScopeName)  (This)->lpVtbl->AddRoles(This,varRoles,bstrScopeName)
#define IAzClientContext2_AddApplicationGroups(This,varApplicationGroups)  (This)->lpVtbl->AddApplicationGroups(This,varApplicationGroups)
#define IAzClientContext2_AddStringSids(This,varStringSids)  (This)->lpVtbl->AddStringSids(This,varStringSids)
#define IAzClientContext2_put_LDAPQueryDN(This,bstrLDAPQueryDN)  (This)->lpVtbl->put_LDAPQueryDN(This,bstrLDAPQueryDN)
#define IAzClientContext2_get_LDAPQueryDN(This,pbstrLDAPQueryDN)  (This)->lpVtbl->get_LDAPQueryDN(This,pbstrLDAPQueryDN)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzClientContext2_GetAssignedScopesPage_Proxy(IAzClientContext2 * This, LONG lOptions, LONG PageSize, VARIANT * pvarCursor, VARIANT * pvarScopeNames);
void __RPC_STUB IAzClientContext2_GetAssignedScopesPage_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext2_AddRoles_Proxy(IAzClientContext2 * This, VARIANT varRoles, BSTR bstrScopeName);
void __RPC_STUB IAzClientContext2_AddRoles_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext2_AddApplicationGroups_Proxy(IAzClientContext2 * This, VARIANT varApplicationGroups);
void __RPC_STUB IAzClientContext2_AddApplicationGroups_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext2_AddStringSids_Proxy(IAzClientContext2 * This, VARIANT varStringSids);
void __RPC_STUB IAzClientContext2_AddStringSids_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext2_put_LDAPQueryDN_Proxy(IAzClientContext2 * This, BSTR bstrLDAPQueryDN);
void __RPC_STUB IAzClientContext2_put_LDAPQueryDN_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzClientContext2_get_LDAPQueryDN_Proxy(IAzClientContext2 * This, BSTR * pbstrLDAPQueryDN);
void __RPC_STUB IAzClientContext2_get_LDAPQueryDN_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __IAzBizRuleContext_INTERFACE_DEFINED__
#define __IAzBizRuleContext_INTERFACE_DEFINED__

EXTERN_C const IID IID_IAzBizRuleContext;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("e192f17d-d59f-455e-a152-940316cd77b2")IAzBizRuleContext:public IDispatch
{
	public:
	virtual HRESULT STDMETHODCALLTYPE put_BusinessRuleResult(BOOL bResult) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_BusinessRuleString(BSTR bstrBusinessRuleString) = 0;
	virtual HRESULT STDMETHODCALLTYPE get_BusinessRuleString(BSTR * pbstrBusinessRuleString) = 0;
	virtual HRESULT STDMETHODCALLTYPE GetParameter(BSTR bstrParameterName, VARIANT * pvarParameterValue) = 0;
};
#else
typedef struct IAzBizRuleContextVtbl {
	BEGIN_INTERFACE
	HRESULT(STDMETHODCALLTYPE * QueryInterface) (IAzBizRuleContext * This, REFIID riid, void **ppvObject);
	ULONG(STDMETHODCALLTYPE *AddRef)(IAzBizRuleContext * This);
	ULONG(STDMETHODCALLTYPE *Release)(IAzBizRuleContext * This);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfoCount)(IAzBizRuleContext * This, UINT * pctinfo);
	HRESULT(STDMETHODCALLTYPE *GetTypeInfo)(IAzBizRuleContext * This, UINT iTInfo, LCID lcid, ITypeInfo ** ppTInfo);
	HRESULT(STDMETHODCALLTYPE *GetIDsOfNames)(IAzBizRuleContext * This, REFIID riid, LPOLESTR * rgszNames, UINT cNames, LCID lcid, DISPID * rgDispId);
	HRESULT(STDMETHODCALLTYPE *Invoke)(IAzBizRuleContext * This, DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS * pDispParams, VARIANT * pVarResult, EXCEPINFO * pExcepInfo, UINT * puArgErr);
	HRESULT(STDMETHODCALLTYPE *put_BusinessRuleResult)(IAzBizRuleContext * This, BOOL bResult);
	HRESULT(STDMETHODCALLTYPE *put_BusinessRuleString)(IAzBizRuleContext * This, BSTR bstrBusinessRuleString);
	HRESULT(STDMETHODCALLTYPE *get_BusinessRuleString)(IAzBizRuleContext * This, BSTR * pbstrBusinessRuleString);
	HRESULT(STDMETHODCALLTYPE *GetParameter)(IAzBizRuleContext * This, BSTR bstrParameterName, VARIANT * pvarParameterValue);
	END_INTERFACE
} IAzBizRuleContextVtbl;

interface IAzBizRuleContext {
    CONST_VTBL struct IAzBizRuleContextVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define IAzBizRuleContext_QueryInterface(This,riid,ppvObject)  (This)->lpVtbl->QueryInterface(This,riid,ppvObject)
#define IAzBizRuleContext_AddRef(This)  (This)->lpVtbl->AddRef(This)
#define IAzBizRuleContext_Release(This)  (This)->lpVtbl->Release(This)
#define IAzBizRuleContext_GetTypeInfoCount(This,pctinfo)  (This)->lpVtbl->GetTypeInfoCount(This,pctinfo)
#define IAzBizRuleContext_GetTypeInfo(This,iTInfo,lcid,ppTInfo)  (This)->lpVtbl->GetTypeInfo(This,iTInfo,lcid,ppTInfo)
#define IAzBizRuleContext_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)  (This)->lpVtbl->GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)
#define IAzBizRuleContext_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)  (This)->lpVtbl->Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)
#define IAzBizRuleContext_put_BusinessRuleResult(This,bResult)  (This)->lpVtbl->put_BusinessRuleResult(This,bResult)
#define IAzBizRuleContext_put_BusinessRuleString(This,bstrBusinessRuleString)  (This)->lpVtbl->put_BusinessRuleString(This,bstrBusinessRuleString)
#define IAzBizRuleContext_get_BusinessRuleString(This,pbstrBusinessRuleString)  (This)->lpVtbl->get_BusinessRuleString(This,pbstrBusinessRuleString)
#define IAzBizRuleContext_GetParameter(This,bstrParameterName,pvarParameterValue)  (This)->lpVtbl->GetParameter(This,bstrParameterName,pvarParameterValue)
#endif

#endif

HRESULT STDMETHODCALLTYPE IAzBizRuleContext_put_BusinessRuleResult_Proxy(IAzBizRuleContext * This, BOOL bResult);
void __RPC_STUB IAzBizRuleContext_put_BusinessRuleResult_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzBizRuleContext_put_BusinessRuleString_Proxy(IAzBizRuleContext * This, BSTR bstrBusinessRuleString);
void __RPC_STUB IAzBizRuleContext_put_BusinessRuleString_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzBizRuleContext_get_BusinessRuleString_Proxy(IAzBizRuleContext * This, BSTR * pbstrBusinessRuleString);
void __RPC_STUB IAzBizRuleContext_get_BusinessRuleString_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);
HRESULT STDMETHODCALLTYPE IAzBizRuleContext_GetParameter_Proxy(IAzBizRuleContext * This, BSTR bstrParameterName, VARIANT * pvarParameterValue);
void __RPC_STUB IAzBizRuleContext_GetParameter_Stub(IRpcStubBuffer * This, IRpcChannelBuffer * _pRpcChannelBuffer, PRPC_MESSAGE _pRpcMessage, DWORD * _pdwStubPhase);

#endif

#ifndef __AZROLESLib_LIBRARY_DEFINED__
#define __AZROLESLib_LIBRARY_DEFINED__

typedef enum tagAZ_PROP_CONSTANTS {
    AZ_PROP_NAME = 1,
    AZ_PROP_DESCRIPTION = 2,
    AZ_PROP_WRITABLE = 3,
    AZ_PROP_APPLICATION_DATA = 4,
    AZ_PROP_CHILD_CREATE = 5,
    AZ_MAX_APPLICATION_NAME_LENGTH = 512,
    AZ_MAX_OPERATION_NAME_LENGTH = 64,
    AZ_MAX_TASK_NAME_LENGTH = 64,
    AZ_MAX_SCOPE_NAME_LENGTH = 65536,
    AZ_MAX_GROUP_NAME_LENGTH = 64,
    AZ_MAX_ROLE_NAME_LENGTH = 64,
    AZ_MAX_NAME_LENGTH = 65536,
    AZ_MAX_DESCRIPTION_LENGTH = 1024,
    AZ_MAX_APPLICATION_DATA_LENGTH = 4096,
    AZ_SUBMIT_FLAG_ABORT = 0x1,
    AZ_MAX_POLICY_URL_LENGTH = 65536,
    AZ_AZSTORE_FLAG_CREATE = 0x1,
    AZ_AZSTORE_FLAG_MANAGE_STORE_ONLY = 0x2,
    AZ_AZSTORE_FLAG_BATCH_UPDATE = 0x4,
    AZ_AZSTORE_FLAG_AUDIT_IS_CRITICAL = 0x8,
    AZ_AZSTORE_FORCE_APPLICATION_CLOSE = 0x10,
    AZ_PROP_AZSTORE_DOMAIN_TIMEOUT = 100,
    AZ_AZSTORE_DEFAULT_DOMAIN_TIMEOUT = 15 * 1000,
    AZ_PROP_AZSTORE_SCRIPT_ENGINE_TIMEOUT = 101,
    AZ_AZSTORE_MIN_DOMAIN_TIMEOUT = 500,
    AZ_AZSTORE_MIN_SCRIPT_ENGINE_TIMEOUT = 5 * 1000,
    AZ_AZSTORE_DEFAULT_SCRIPT_ENGINE_TIMEOUT = 45 * 1000,
    AZ_PROP_AZSTORE_MAX_SCRIPT_ENGINES = 102,
    AZ_AZSTORE_DEFAULT_MAX_SCRIPT_ENGINES = 120,
    AZ_PROP_AZSTORE_MAJOR_VERSION = 103,
    AZ_PROP_AZSTORE_MINOR_VERSION = 104,
    AZ_PROP_AZSTORE_TARGET_MACHINE = 105,
    AZ_PROP_OPERATION_ID = 200,
    AZ_PROP_TASK_OPERATIONS = 300,
    AZ_PROP_TASK_BIZRULE = 301,
    AZ_PROP_TASK_BIZRULE_LANGUAGE = 302,
    AZ_PROP_TASK_TASKS = 303,
    AZ_PROP_TASK_BIZRULE_IMPORTED_PATH = 304,
    AZ_PROP_TASK_IS_ROLE_DEFINITION = 305,
    AZ_MAX_TASK_BIZRULE_LENGTH = 65536,
    AZ_MAX_TASK_BIZRULE_LANGUAGE_LENGTH = 64,
    AZ_MAX_TASK_BIZRULE_IMPORTED_PATH_LENGTH = 512,
    AZ_MAX_BIZRULE_STRING = 65536,
    AZ_PROP_GROUP_TYPE = 400,
    AZ_GROUPTYPE_LDAP_QUERY = 1,
    AZ_GROUPTYPE_BASIC = 2,
    AZ_PROP_GROUP_APP_MEMBERS = 401,
    AZ_PROP_GROUP_APP_NON_MEMBERS = 402,
    AZ_PROP_GROUP_LDAP_QUERY = 403,
    AZ_MAX_GROUP_LDAP_QUERY_LENGTH = 4096,
    AZ_PROP_GROUP_MEMBERS = 404,
    AZ_PROP_GROUP_NON_MEMBERS = 405,
    AZ_PROP_GROUP_MEMBERS_NAME = 406,
    AZ_PROP_GROUP_NON_MEMBERS_NAME = 407,
    AZ_PROP_ROLE_APP_MEMBERS = 500,
    AZ_PROP_ROLE_MEMBERS = 501,
    AZ_PROP_ROLE_OPERATIONS = 502,
    AZ_PROP_ROLE_TASKS = 504,
    AZ_PROP_ROLE_MEMBERS_NAME = 505,
    AZ_PROP_SCOPE_BIZRULES_WRITABLE = 600,
    AZ_PROP_SCOPE_CAN_BE_DELEGATED = 601,
    AZ_PROP_CLIENT_CONTEXT_USER_DN = 700,
    AZ_PROP_CLIENT_CONTEXT_USER_SAM_COMPAT = 701,
    AZ_PROP_CLIENT_CONTEXT_USER_DISPLAY = 702,
    AZ_PROP_CLIENT_CONTEXT_USER_GUID = 703,
    AZ_PROP_CLIENT_CONTEXT_USER_CANONICAL = 704,
    AZ_PROP_CLIENT_CONTEXT_USER_UPN = 705,
    AZ_PROP_CLIENT_CONTEXT_USER_DNS_SAM_COMPAT = 707,
    AZ_PROP_CLIENT_CONTEXT_ROLE_FOR_ACCESS_CHECK = 708,
    AZ_PROP_CLIENT_CONTEXT_LDAP_QUERY_DN = 709,
    AZ_PROP_APPLICATION_AUTHZ_INTERFACE_CLSID = 800,
    AZ_PROP_APPLICATION_VERSION = 801,
    AZ_MAX_APPLICATION_VERSION_LENGTH = 512,
    AZ_PROP_APPLICATION_NAME = 802,
    AZ_PROP_APPLY_STORE_SACL = 900,
    AZ_PROP_GENERATE_AUDITS = 901,
    AZ_PROP_POLICY_ADMINS = 902,
    AZ_PROP_POLICY_READERS = 903,
    AZ_PROP_DELEGATED_POLICY_USERS = 904,
    AZ_PROP_POLICY_ADMINS_NAME = 905,
    AZ_PROP_POLICY_READERS_NAME = 906,
    AZ_PROP_DELEGATED_POLICY_USERS_NAME = 907,
    AZ_CLIENT_CONTEXT_SKIP_GROUP = 1,
    AZ_CLIENT_CONTEXT_SKIP_LDAP_QUERY = 1
} AZ_PROP_CONSTANTS;

EXTERN_C const IID LIBID_AZROLESLib;
EXTERN_C const CLSID CLSID_AzAuthorizationStore;

#ifdef __cplusplus
class DECLSPEC_UUID("b2bcff59-a757-4b0b-a1bc-ea69981da69e") AzAuthorizationStore;
#endif

EXTERN_C const CLSID CLSID_AzBizRuleContext;

#ifdef __cplusplus
class DECLSPEC_UUID("5c2dc96f-8d51-434b-b33c-379bccae77c3") AzBizRuleContext;
#endif
#endif

#ifndef OLESCRIPT_E_SYNTAX
#define OLESCRIPT_E_SYNTAX  _HRESULT_TYPEDEF_(0x80020101L)
#endif

extern RPC_IF_HANDLE __MIDL_itf_azroles_0279_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_azroles_0279_v0_0_s_ifspec;

unsigned long __RPC_USER BSTR_UserSize(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree(unsigned long *, BSTR *);

unsigned long __RPC_USER VARIANT_UserSize(unsigned long *, unsigned long, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserMarshal(unsigned long *, unsigned char *, VARIANT *);
unsigned char *__RPC_USER VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT *);
void __RPC_USER VARIANT_UserFree(unsigned long *, VARIANT *);

#ifdef __cplusplus
}
#endif

#endif /* _AZRULES_H */
