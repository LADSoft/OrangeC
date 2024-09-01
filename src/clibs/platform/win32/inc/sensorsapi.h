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

#ifndef _SENSORSAPI_H
#define _SENSORSAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Forward Declarations */ 

#ifndef __ISensorManager_FWD_DEFINED__
#define __ISensorManager_FWD_DEFINED__
typedef interface ISensorManager ISensorManager;
#endif

#ifndef __ISensorCollection_FWD_DEFINED__
#define __ISensorCollection_FWD_DEFINED__
typedef interface ISensorCollection ISensorCollection;
#endif

#ifndef __ISensor_FWD_DEFINED__
#define __ISensor_FWD_DEFINED__
typedef interface ISensor ISensor;
#endif

#ifndef __ISensorDataReport_FWD_DEFINED__
#define __ISensorDataReport_FWD_DEFINED__
typedef interface ISensorDataReport ISensorDataReport;
#endif

#ifndef __ISensorManagerEvents_FWD_DEFINED__
#define __ISensorManagerEvents_FWD_DEFINED__
typedef interface ISensorManagerEvents ISensorManagerEvents;
#endif

#ifndef __ISensorEvents_FWD_DEFINED__
#define __ISensorEvents_FWD_DEFINED__
typedef interface ISensorEvents ISensorEvents;
#endif

#ifndef __ILogicalSensorManager_FWD_DEFINED__
#define __ILogicalSensorManager_FWD_DEFINED__
typedef interface ILogicalSensorManager ILogicalSensorManager;
#endif

#ifndef __SensorManager_FWD_DEFINED__
#define __SensorManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class SensorManager SensorManager;
#else
typedef struct SensorManager SensorManager;
#endif /* __cplusplus */

#endif /* __SensorManager_FWD_DEFINED__ */

#ifndef __SensorCollection_FWD_DEFINED__
#define __SensorCollection_FWD_DEFINED__

#ifdef __cplusplus
typedef class SensorCollection SensorCollection;
#else
typedef struct SensorCollection SensorCollection;
#endif /* __cplusplus */

#endif /* __SensorCollection_FWD_DEFINED__ */

#ifndef __Sensor_FWD_DEFINED__
#define __Sensor_FWD_DEFINED__

#ifdef __cplusplus
typedef class Sensor Sensor;
#else
typedef struct Sensor Sensor;
#endif /* __cplusplus */

#endif /* __Sensor_FWD_DEFINED__ */

#ifndef __SensorDataReport_FWD_DEFINED__
#define __SensorDataReport_FWD_DEFINED__

#ifdef __cplusplus
typedef class SensorDataReport SensorDataReport;
#else
typedef struct SensorDataReport SensorDataReport;
#endif /* __cplusplus */

#endif /* __SensorDataReport_FWD_DEFINED__ */

#ifndef __LogicalSensorManager_FWD_DEFINED__
#define __LogicalSensorManager_FWD_DEFINED__

#ifdef __cplusplus
typedef class LogicalSensorManager LogicalSensorManager;
#else
typedef struct LogicalSensorManager LogicalSensorManager;
#endif /* __cplusplus */

#endif /* __LogicalSensorManager_FWD_DEFINED__ */


#include "oaidl.h"
#include "ocidl.h"
#include "propsys.h"
#include "portabledevicetypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

typedef enum __MIDL___MIDL_itf_sensorsapi_0000_0000_0001 {
    SENSOR_STATE_MIN = 0,
    SENSOR_STATE_READY = SENSOR_STATE_MIN,
    SENSOR_STATE_NOT_AVAILABLE = (SENSOR_STATE_READY + 1),
    SENSOR_STATE_NO_DATA = (SENSOR_STATE_NOT_AVAILABLE + 1),
    SENSOR_STATE_INITIALIZING = (SENSOR_STATE_NO_DATA + 1),
    SENSOR_STATE_ACCESS_DENIED = (SENSOR_STATE_INITIALIZING + 1),
    SENSOR_STATE_ERROR = (SENSOR_STATE_ACCESS_DENIED + 1),
    SENSOR_STATE_MAX = SENSOR_STATE_ERROR
} SensorState;

typedef enum __MIDL___MIDL_itf_sensorsapi_0000_0000_0002 {
    SENSOR_CONNECTION_TYPE_PC_INTEGRATED = 0,
    SENSOR_CONNECTION_TYPE_PC_ATTACHED = (SENSOR_CONNECTION_TYPE_PC_INTEGRATED + 1),
    SENSOR_CONNECTION_TYPE_PC_EXTERNAL = (SENSOR_CONNECTION_TYPE_PC_ATTACHED + 1)
} SensorConnectionType;

typedef enum LOCATION_DESIRED_ACCURACY {
    LOCATION_DESIRED_ACCURACY_DEFAULT = 0,
    LOCATION_DESIRED_ACCURACY_HIGH = (LOCATION_DESIRED_ACCURACY_DEFAULT + 1)
} LOCATION_DESIRED_ACCURACY;

typedef GUID SENSOR_CATEGORY_ID;
typedef REFGUID REFSENSOR_CATEGORY_ID;
typedef GUID SENSOR_TYPE_ID;
typedef REFGUID REFSENSOR_TYPE_ID;
typedef GUID SENSOR_ID;
typedef REFGUID REFSENSOR_ID;

extern RPC_IF_HANDLE __MIDL_itf_sensorsapi_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_sensorsapi_0000_0000_v0_0_s_ifspec;

#ifndef __ISensorManager_INTERFACE_DEFINED__
#define __ISensorManager_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISensorManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("BD77DB67-45A8-42DC-8D00-6DCF15F8377A")ISensorManager:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetSensorsByCategory(REFSENSOR_CATEGORY_ID, ISensorCollection **) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSensorsByType(REFSENSOR_TYPE_ID, ISensorCollection **) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSensorByID(REFSENSOR_ID, ISensor **) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEventSink(ISensorManagerEvents *) = 0;
    virtual HRESULT STDMETHODCALLTYPE RequestPermissions(HWND, ISensorCollection *, BOOL) = 0;
};
#else /* C style interface */
typedef struct ISensorManagerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ISensorManager *, REFIID riid, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(ISensorManager *);
    ULONG (STDMETHODCALLTYPE *Release)(ISensorManager *);
    HRESULT (STDMETHODCALLTYPE *GetSensorsByCategory)(ISensorManager *, REFSENSOR_CATEGORY_ID, ISensorCollection **);
    HRESULT (STDMETHODCALLTYPE *GetSensorsByType)(ISensorManager *, REFSENSOR_TYPE_ID, ISensorCollection **);
    HRESULT (STDMETHODCALLTYPE *GetSensorByID)(ISensorManager *, REFSENSOR_ID sensorID, ISensor **);
    HRESULT (STDMETHODCALLTYPE *SetEventSink)(ISensorManager *, ISensorManagerEvents *);
    HRESULT (STDMETHODCALLTYPE *RequestPermissions)(ISensorManager *, HWND, ISensorCollection *, BOOL);
    END_INTERFACE
} ISensorManagerVtbl;

interface ISensorManager {
    CONST_VTBL struct ISensorManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISensorManager_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject))
#define ISensorManager_AddRef(This)  ((This)->lpVtbl->AddRef(This))
#define ISensorManager_Release(This)  ((This)->lpVtbl->Release(This))
#define ISensorManager_GetSensorsByCategory(This,sensorCategory,ppSensorsFound)  ((This)->lpVtbl->GetSensorsByCategory(This,sensorCategory,ppSensorsFound))
#define ISensorManager_GetSensorsByType(This,sensorType,ppSensorsFound)  ((This)->lpVtbl->GetSensorsByType(This,sensorType,ppSensorsFound))
#define ISensorManager_GetSensorByID(This,sensorID,ppSensor)  ((This)->lpVtbl->GetSensorByID(This,sensorID,ppSensor))
#define ISensorManager_SetEventSink(This,pEvents)  ((This)->lpVtbl->SetEventSink(This,pEvents))
#define ISensorManager_RequestPermissions(This,hParent,pSensors,fModal)  ((This)->lpVtbl->RequestPermissions(This,hParent,pSensors,fModal))
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ISensorManager_INTERFACE_DEFINED__ */


#ifndef __ISensorCollection_INTERFACE_DEFINED__
#define __ISensorCollection_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISensorCollection;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("23571E11-E545-4DD8-A337-B89BF44B10DF")ISensorCollection:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetAt(ULONG, ISensor **) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCount(ULONG *) = 0;
    virtual HRESULT STDMETHODCALLTYPE Add(ISensor *) = 0;
    virtual HRESULT STDMETHODCALLTYPE Remove(ISensor *) = 0;
    virtual HRESULT STDMETHODCALLTYPE RemoveByID(REFSENSOR_ID) = 0;
    virtual HRESULT STDMETHODCALLTYPE Clear(void) = 0;
};
#else 	/* C style interface */
typedef struct ISensorCollectionVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ISensorCollection *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(ISensorCollection *);
    ULONG (STDMETHODCALLTYPE *Release)(ISensorCollection *);
    HRESULT (STDMETHODCALLTYPE *GetAt)(ISensorCollection *, ULONG, ISensor **);
    HRESULT (STDMETHODCALLTYPE *GetCount)(ISensorCollection *, ULONG *);
    HRESULT (STDMETHODCALLTYPE *Add)(ISensorCollection *, ISensor *);
    HRESULT (STDMETHODCALLTYPE *Remove)(ISensorCollection *, ISensor *);
    HRESULT (STDMETHODCALLTYPE *RemoveByID)(ISensorCollection *, REFSENSOR_ID);
    HRESULT (STDMETHODCALLTYPE *Clear)(ISensorCollection *);
    END_INTERFACE
} ISensorCollectionVtbl;

interface ISensorCollection {
    CONST_VTBL struct ISensorCollectionVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISensorCollection_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define ISensorCollection_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define ISensorCollection_Release(This)  ((This)->lpVtbl->Release(This)) 
#define ISensorCollection_GetAt(This,ulIndex,ppSensor)  ((This)->lpVtbl->GetAt(This,ulIndex,ppSensor)) 
#define ISensorCollection_GetCount(This,pCount)  ((This)->lpVtbl->GetCount(This,pCount)) 
#define ISensorCollection_Add(This,pSensor)  ((This)->lpVtbl->Add(This,pSensor)) 
#define ISensorCollection_Remove(This,pSensor)  ((This)->lpVtbl->Remove(This,pSensor)) 
#define ISensorCollection_RemoveByID(This,sensorID)  ((This)->lpVtbl->RemoveByID(This,sensorID)) 
#define ISensorCollection_Clear(This)  ((This)->lpVtbl->Clear(This)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ISensorCollection_INTERFACE_DEFINED__ */


#ifndef __ISensor_INTERFACE_DEFINED__
#define __ISensor_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISensor;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("5FA08F80-2657-458E-AF75-46F73FA6AC5C")ISensor:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetID(SENSOR_ID *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetCategory(SENSOR_CATEGORY_ID *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetType(SENSOR_TYPE_ID *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetFriendlyName(BSTR *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperty(REFPROPERTYKEY, PROPVARIANT *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetProperties(IPortableDeviceKeyCollection *, IPortableDeviceValues **) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSupportedDataFields(IPortableDeviceKeyCollection **) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetProperties(IPortableDeviceValues *, IPortableDeviceValues **) = 0;
    virtual HRESULT STDMETHODCALLTYPE SupportsDataField(REFPROPERTYKEY, VARIANT_BOOL *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetState(SensorState *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetData(ISensorDataReport **) = 0;
    virtual HRESULT STDMETHODCALLTYPE SupportsEvent(REFGUID, VARIANT_BOOL *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetEventInterest(GUID **, ULONG *) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEventInterest(GUID *, ULONG) = 0;
    virtual HRESULT STDMETHODCALLTYPE SetEventSink(ISensorEvents *) = 0;
};
#else 	/* C style interface */

typedef struct ISensorVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface) (ISensor *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef) (ISensor *);
    ULONG (STDMETHODCALLTYPE *Release) (ISensor *);
    HRESULT (STDMETHODCALLTYPE *GetID) (ISensor *, SENSOR_ID *);
    HRESULT (STDMETHODCALLTYPE *GetCategory) (ISensor *, SENSOR_CATEGORY_ID *);
    HRESULT (STDMETHODCALLTYPE *GetType) (ISensor *, SENSOR_TYPE_ID *);
    HRESULT (STDMETHODCALLTYPE *GetFriendlyName) (ISensor *, BSTR *);
    HRESULT (STDMETHODCALLTYPE *GetProperty) (ISensor *, REFPROPERTYKEY, PROPVARIANT *);
    HRESULT (STDMETHODCALLTYPE *GetProperties) (ISensor *, IPortableDeviceKeyCollection *, IPortableDeviceValues **);
    HRESULT (STDMETHODCALLTYPE *GetSupportedDataFields) (ISensor *, IPortableDeviceKeyCollection **);
    HRESULT (STDMETHODCALLTYPE *SetProperties) (ISensor *, IPortableDeviceValues *, IPortableDeviceValues **);
    HRESULT (STDMETHODCALLTYPE *SupportsDataField) (ISensor *, REFPROPERTYKEY, VARIANT_BOOL *);
    HRESULT (STDMETHODCALLTYPE *GetState) (ISensor *, SensorState *);
    HRESULT (STDMETHODCALLTYPE *GetData) (ISensor *, ISensorDataReport **);
    HRESULT (STDMETHODCALLTYPE *SupportsEvent) (ISensor *, REFGUID, VARIANT_BOOL *);
    HRESULT (STDMETHODCALLTYPE *GetEventInterest) (ISensor *, GUID **, ULONG *);
    HRESULT (STDMETHODCALLTYPE *SetEventInterest) (ISensor *, GUID *, ULONG);
    HRESULT (STDMETHODCALLTYPE *SetEventSink) (ISensor *, ISensorEvents *);
    END_INTERFACE
} ISensorVtbl;

interface ISensor {
    CONST_VTBL struct ISensorVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISensor_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define ISensor_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define ISensor_Release(This)  ((This)->lpVtbl->Release(This)) 
#define ISensor_GetID(This,pID)  ((This)->lpVtbl->GetID(This,pID)) 
#define ISensor_GetCategory(This,pSensorCategory)  ((This)->lpVtbl->GetCategory(This,pSensorCategory)) 
#define ISensor_GetType(This,pSensorType)  ((This)->lpVtbl->GetType(This,pSensorType)) 
#define ISensor_GetFriendlyName(This,pFriendlyName)  ((This)->lpVtbl->GetFriendlyName(This,pFriendlyName)) 
#define ISensor_GetProperty(This,key,pProperty)  ((This)->lpVtbl->GetProperty(This,key,pProperty)) 
#define ISensor_GetProperties(This,pKeys,ppProperties)  ((This)->lpVtbl->GetProperties(This,pKeys,ppProperties)) 
#define ISensor_GetSupportedDataFields(This,ppDataFields)  ((This)->lpVtbl->GetSupportedDataFields(This,ppDataFields)) 
#define ISensor_SetProperties(This,pProperties,ppResults)  ((This)->lpVtbl->SetProperties(This,pProperties,ppResults)) 
#define ISensor_SupportsDataField(This,key,pIsSupported)  ((This)->lpVtbl->SupportsDataField(This,key,pIsSupported)) 
#define ISensor_GetState(This,pState)  ((This)->lpVtbl->GetState(This,pState)) 
#define ISensor_GetData(This,ppDataReport)  ((This)->lpVtbl->GetData(This,ppDataReport)) 
#define ISensor_SupportsEvent(This,eventGuid,pIsSupported)  ((This)->lpVtbl->SupportsEvent(This,eventGuid,pIsSupported)) 
#define ISensor_GetEventInterest(This,ppValues,pCount)  ((This)->lpVtbl->GetEventInterest(This,ppValues,pCount)) 
#define ISensor_SetEventInterest(This,pValues,count)  ((This)->lpVtbl->SetEventInterest(This,pValues,count)) 
#define ISensor_SetEventSink(This,pEvents)  ((This)->lpVtbl->SetEventSink(This,pEvents)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ISensor_INTERFACE_DEFINED__ */


#ifndef __ISensorDataReport_INTERFACE_DEFINED__
#define __ISensorDataReport_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISensorDataReport;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("0AB9DF9B-C4B5-4796-8898-0470706A2E1D")ISensorDataReport:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE GetTimestamp(SYSTEMTIME *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSensorValue(REFPROPERTYKEY, PROPVARIANT *) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetSensorValues(IPortableDeviceKeyCollection *, IPortableDeviceValues **) = 0;
};
#else /* C style interface */

typedef struct ISensorDataReportVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ISensorDataReport *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(ISensorDataReport *);
    ULONG (STDMETHODCALLTYPE *Release)(ISensorDataReport *);
    HRESULT (STDMETHODCALLTYPE *GetTimestamp)(ISensorDataReport *, SYSTEMTIME *);
    HRESULT (STDMETHODCALLTYPE *GetSensorValue)(ISensorDataReport *, REFPROPERTYKEY, PROPVARIANT *);
    HRESULT (STDMETHODCALLTYPE *GetSensorValues)(ISensorDataReport *, IPortableDeviceKeyCollection *, IPortableDeviceValues **);
    END_INTERFACE
} ISensorDataReportVtbl;

interface ISensorDataReport {
    CONST_VTBL struct ISensorDataReportVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISensorDataReport_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define ISensorDataReport_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define ISensorDataReport_Release(This)  ((This)->lpVtbl->Release(This)) 
#define ISensorDataReport_GetTimestamp(This,pTimeStamp)  ((This)->lpVtbl->GetTimestamp(This,pTimeStamp)) 
#define ISensorDataReport_GetSensorValue(This,pKey,pValue)  ((This)->lpVtbl->GetSensorValue(This,pKey,pValue)) 
#define ISensorDataReport_GetSensorValues(This,pKeys,ppValues)  ((This)->lpVtbl->GetSensorValues(This,pKeys,ppValues)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ISensorDataReport_INTERFACE_DEFINED__ */


#ifndef __ISensorManagerEvents_INTERFACE_DEFINED__
#define __ISensorManagerEvents_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISensorManagerEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("9B3B0B86-266A-4AAD-B21F-FDE5501001B7")ISensorManagerEvents:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnSensorEnter(ISensor *, SensorState) = 0;
};
#else /* C style interface */

typedef struct ISensorManagerEventsVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE * QueryInterface) (ISensorManagerEvents *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE * AddRef) (ISensorManagerEvents *);
    ULONG (STDMETHODCALLTYPE * Release) (ISensorManagerEvents *);
    HRESULT (STDMETHODCALLTYPE * OnSensorEnter) (ISensorManagerEvents *, ISensor *, SensorState);
    END_INTERFACE
} ISensorManagerEventsVtbl;

interface ISensorManagerEvents {
    CONST_VTBL struct ISensorManagerEventsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISensorManagerEvents_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define ISensorManagerEvents_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define ISensorManagerEvents_Release(This)  ((This)->lpVtbl->Release(This)) 
#define ISensorManagerEvents_OnSensorEnter(This,pSensor,state)  ((This)->lpVtbl->OnSensorEnter(This,pSensor,state)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ISensorManagerEvents_INTERFACE_DEFINED__ */


#ifndef __ISensorEvents_INTERFACE_DEFINED__
#define __ISensorEvents_INTERFACE_DEFINED__

EXTERN_C const IID IID_ISensorEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

MIDL_INTERFACE("5D8DCC91-4641-47E7-B7C3-B74F48A6C391")ISensorEvents:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE OnStateChanged(ISensor *, SensorState) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnDataUpdated(ISensor *, ISensorDataReport *) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnEvent(ISensor *, REFGUID, IPortableDeviceValues *) = 0;
    virtual HRESULT STDMETHODCALLTYPE OnLeave(REFSENSOR_ID) = 0;
};
#else /* C style interface */
typedef struct ISensorEventsVtbl {
    BEGIN_INTERFACE
    HRESULT(STDMETHODCALLTYPE *QueryInterface)(ISensorEvents *, REFIID, void **);
    ULONG(STDMETHODCALLTYPE *AddRef)(ISensorEvents *);
    ULONG(STDMETHODCALLTYPE *Release)(ISensorEvents *);
    HRESULT(STDMETHODCALLTYPE *OnStateChanged)(ISensorEvents *, ISensor *, SensorState);
    HRESULT(STDMETHODCALLTYPE *OnDataUpdated)(ISensorEvents *, ISensor *, ISensorDataReport *);
    HRESULT(STDMETHODCALLTYPE *OnEvent)(ISensorEvents *, ISensor *, REFGUID, IPortableDeviceValues *);
    HRESULT(STDMETHODCALLTYPE *OnLeave)(ISensorEvents *, REFSENSOR_ID);
    END_INTERFACE
} ISensorEventsVtbl;

interface ISensorEvents {
    CONST_VTBL struct ISensorEventsVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ISensorEvents_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define ISensorEvents_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define ISensorEvents_Release(This)  ((This)->lpVtbl->Release(This)) 
#define ISensorEvents_OnStateChanged(This,pSensor,state)  ((This)->lpVtbl->OnStateChanged(This,pSensor,state)) 
#define ISensorEvents_OnDataUpdated(This,pSensor,pNewData)  ((This)->lpVtbl->OnDataUpdated(This,pSensor,pNewData)) 
#define ISensorEvents_OnEvent(This,pSensor,eventID,pEventData)  ((This)->lpVtbl->OnEvent(This,pSensor,eventID,pEventData)) 
#define ISensorEvents_OnLeave(This,ID)  ((This)->lpVtbl->OnLeave(This,ID)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ISensorEvents_INTERFACE_DEFINED__ */


#ifndef __ILogicalSensorManager_INTERFACE_DEFINED__
#define __ILogicalSensorManager_INTERFACE_DEFINED__

EXTERN_C const IID IID_ILogicalSensorManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
MIDL_INTERFACE("7490B4D4-1BE9-469A-9743-37A27C1D80AF")ILogicalSensorManager:public IUnknown
{
    public:
    virtual HRESULT STDMETHODCALLTYPE Connect(REFGUID, IPropertyStore *) = 0;
    virtual HRESULT STDMETHODCALLTYPE Uninstall(REFGUID) = 0;
    virtual HRESULT STDMETHODCALLTYPE Disconnect(REFGUID) = 0;
};
#else /* C style interface */
typedef struct ILogicalSensorManagerVtbl {
    BEGIN_INTERFACE
    HRESULT (STDMETHODCALLTYPE *QueryInterface)(ILogicalSensorManager *, REFIID, void **);
    ULONG (STDMETHODCALLTYPE *AddRef)(ILogicalSensorManager *);
    ULONG (STDMETHODCALLTYPE *Release)(ILogicalSensorManager *);
    HRESULT (STDMETHODCALLTYPE *Connect)(ILogicalSensorManager *, REFGUID, IPropertyStore *);
    HRESULT (STDMETHODCALLTYPE *Uninstall)(ILogicalSensorManager *, REFGUID);
    HRESULT (STDMETHODCALLTYPE *Disconnect)(ILogicalSensorManager *, REFGUID);
    END_INTERFACE
} ILogicalSensorManagerVtbl;

interface ILogicalSensorManager {
    CONST_VTBL struct ILogicalSensorManagerVtbl *lpVtbl;
};

#ifdef COBJMACROS
#define ILogicalSensorManager_QueryInterface(This,riid,ppvObject)  ((This)->lpVtbl->QueryInterface(This,riid,ppvObject)) 
#define ILogicalSensorManager_AddRef(This)  ((This)->lpVtbl->AddRef(This)) 
#define ILogicalSensorManager_Release(This)  ((This)->lpVtbl->Release(This)) 
#define ILogicalSensorManager_Connect(This,logicalID,pPropertyStore)  ((This)->lpVtbl->Connect(This,logicalID,pPropertyStore)) 
#define ILogicalSensorManager_Uninstall(This,logicalID)  ((This)->lpVtbl->Uninstall(This,logicalID)) 
#define ILogicalSensorManager_Disconnect(This,logicalID)  ((This)->lpVtbl->Disconnect(This,logicalID)) 
#endif /* COBJMACROS */

#endif /* C style interface */

#endif /* __ILogicalSensorManager_INTERFACE_DEFINED__ */


#ifndef __SensorsApiLib_LIBRARY_DEFINED__
#define __SensorsApiLib_LIBRARY_DEFINED__

EXTERN_C const IID LIBID_SensorsApiLib;

EXTERN_C const CLSID CLSID_SensorManager;

#ifdef __cplusplus
class DECLSPEC_UUID("77A1C827-FCD2-4689-8915-9D613CC5FA3E") SensorManager;
#endif

EXTERN_C const CLSID CLSID_SensorCollection;

#ifdef __cplusplus
class DECLSPEC_UUID("79C43ADB-A429-469F-AA39-2F2B74B75937") SensorCollection;
#endif

EXTERN_C const CLSID CLSID_Sensor;

#ifdef __cplusplus
class DECLSPEC_UUID("E97CED00-523A-4133-BF6F-D3A2DAE7F6BA") Sensor;
#endif

EXTERN_C const CLSID CLSID_SensorDataReport;

#ifdef __cplusplus
class DECLSPEC_UUID("4EA9D6EF-694B-4218-8816-CCDA8DA74BBA") SensorDataReport;
#endif

EXTERN_C const CLSID CLSID_LogicalSensorManager;

#ifdef __cplusplus
class DECLSPEC_UUID("50A7B286-7D23-41E6-9440-4DAEE00DC5F0") LogicalSensorManager;
#endif
#endif /* __SensorsApiLib_LIBRARY_DEFINED__ */

unsigned long __RPC_USER BSTR_UserSize(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree(unsigned long *, BSTR *);

unsigned long __RPC_USER HWND_UserSize(unsigned long *, unsigned long, HWND *);
unsigned char *__RPC_USER HWND_UserMarshal(unsigned long *, unsigned char *, HWND *);
unsigned char *__RPC_USER HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND *);
void __RPC_USER HWND_UserFree(unsigned long *, HWND *);

unsigned long __RPC_USER LPSAFEARRAY_UserSize(unsigned long *, unsigned long, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserMarshal(unsigned long *, unsigned char *, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY *);
void __RPC_USER LPSAFEARRAY_UserFree(unsigned long *, LPSAFEARRAY *);

unsigned long __RPC_USER BSTR_UserSize64(unsigned long *, unsigned long, BSTR *);
unsigned char *__RPC_USER BSTR_UserMarshal64(unsigned long *, unsigned char *, BSTR *);
unsigned char *__RPC_USER BSTR_UserUnmarshal64(unsigned long *, unsigned char *, BSTR *);
void __RPC_USER BSTR_UserFree64(unsigned long *, BSTR *);

unsigned long __RPC_USER HWND_UserSize64(unsigned long *, unsigned long, HWND *);
unsigned char *__RPC_USER HWND_UserMarshal64(unsigned long *, unsigned char *, HWND *);
unsigned char *__RPC_USER HWND_UserUnmarshal64(unsigned long *, unsigned char *, HWND *);
void __RPC_USER HWND_UserFree64(unsigned long *, HWND *);

unsigned long __RPC_USER LPSAFEARRAY_UserSize64(unsigned long *, unsigned long, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserMarshal64(unsigned long *, unsigned char *, LPSAFEARRAY *);
unsigned char *__RPC_USER LPSAFEARRAY_UserUnmarshal64(unsigned long *, unsigned char *, LPSAFEARRAY *);
void __RPC_USER LPSAFEARRAY_UserFree64(unsigned long *, LPSAFEARRAY *);

#ifdef __cplusplus
}
#endif

#endif /* _SENSORSAPI_H */
