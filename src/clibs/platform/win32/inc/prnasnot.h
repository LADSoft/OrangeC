#ifndef _PRNASNOT_H
#define _PRNASNOT_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Print API definitions */

#if (NTDDI_VERSION >= NTDDI_VISTA)

#include "initguid.h"
#include <objbase.h>

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif

DEFINE_GUID(IID_IPrintAsyncNotifyChannel,0x4a5031b1,0x1f3f,0x4db0,0xa4,0x62,0x45,0x30,0xed,0x8b,0x04,0x51);
DEFINE_GUID(IID_IPrintAsyncNotifyCallback,0x7def34c1,0x9d92,0x4c99,0xb3,0xb3,0xdb,0x94,0xa9,0xd4,0x19,0x1b);
DEFINE_GUID(IID_IPrintAsyncNotifyDataObject,0x77cf513e,0x5d49,0x4789,0x9f,0x30,0xd0,0x82,0x2b,0x33,0x5c,0x0d);

DEFINE_GUID(NOTIFICATION_RELEASE,0xba9a5027,0xa70e,0x4ae7,0x9b,0x7d,0xeb,0x3e,0x06,0xad,0x41,0x57);
DEFINE_GUID(PRINT_APP_BIDI_NOTIFY_CHANNEL,0x2ABAD223,0xB994,0x4aca,0x82,0xFC,0x45,0x71,0xB1,0xB5,0x85,0xAC);
DEFINE_GUID(PRINT_PORT_MONITOR_NOTIFY_CHANNEL,0x25df3b0e,0x74a9,0x47f5,0x80,0xce,0x79,0xb4,0xb1,0xeb,0x5c,0x58);

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    kPerUser,
    kAllUsers,
} PrintAsyncNotifyUserFilter;

typedef enum {
    kBiDirectional,
    kUniDirectional
} PrintAsyncNotifyConversationStyle;

typedef GUID PrintAsyncNotificationType;

#undef  INTERFACE
#define INTERFACE IPrintAsyncNotifyDataObject
DECLARE_INTERFACE_(IPrintAsyncNotifyDataObject, IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    STDMETHOD(AcquireData)(THIS_ BYTE**, ULONG*, PrintAsyncNotificationType**) PURE;
    STDMETHOD(ReleaseData)(THIS) PURE;
};

typedef interface IPrintAsyncNotifyCallback IPrintAsyncNotifyCallback;

#undef  INTERFACE
#define INTERFACE IPrintAsyncNotifyChannel
DECLARE_INTERFACE_(IPrintAsyncNotifyChannel, IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    STDMETHOD(SendNotification)(THIS_ IPrintAsyncNotifyDataObject*) PURE;
    STDMETHOD(CloseChannel)(THIS_ IPrintAsyncNotifyDataObject*) PURE;
};

#undef  INTERFACE
#define INTERFACE IPrintAsyncNotifyCallback
DECLARE_INTERFACE_(IPrintAsyncNotifyCallback, IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void** ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef)(THIS) PURE;
    STDMETHOD_(ULONG, Release)(THIS) PURE;
    STDMETHOD(OnEventNotify)(THIS_ IPrintAsyncNotifyChannel*, IPrintAsyncNotifyDataObject*) PURE;
    STDMETHOD(ChannelClosed)(THIS_ IPrintAsyncNotifyChannel*, IPrintAsyncNotifyDataObject*) PURE;
};

#undef INTERFACE

HRESULT WINAPI RegisterForPrintAsyncNotifications(PCWSTR, PrintAsyncNotificationType *, PrintAsyncNotifyUserFilter, PrintAsyncNotifyConversationStyle, IPrintAsyncNotifyCallback *, HANDLE *);
HRESULT WINAPI UnRegisterForPrintAsyncNotifications(HANDLE);
HRESULT WINAPI CreatePrintAsyncNotifyChannel(PCWSTR, PrintAsyncNotificationType *, PrintAsyncNotifyUserFilter, PrintAsyncNotifyConversationStyle, IPrintAsyncNotifyCallback *, IPrintAsyncNotifyChannel **);

typedef enum {
    CHANNEL_CLOSED_BY_SERVER = 0x01,
    CHANNEL_CLOSED_BY_ANOTHER_LISTENER = 0x02,
    CHANNEL_CLOSED_BY_SAME_LISTENER = 0x03,
    CHANNEL_RELEASED_BY_LISTENER = 0x04,
    UNIRECTIONAL_NOTIFICATION_LOST = 0x05,
    ASYNC_NOTIFICATION_FAILURE = 0x06,
    NO_LISTENERS = 0x07,
    CHANNEL_ALREADY_CLOSED = 0x08,
    CHANNEL_ALREADY_OPENED = 0x09,
    CHANNEL_WAITING_FOR_CLIENT_NOTIFICATION = 0x0a,
    CHANNEL_NOT_OPENED = 0x0b,
    ASYNC_CALL_ALREADY_PARKED = 0x0c,
    NOT_REGISTERED = 0x0d,
    ALREADY_UNREGISTERED = 0x0e,
    ALREADY_REGISTERED = 0x0f,
    CHANNEL_ACQUIRED = 0x10,
    ASYNC_CALL_IN_PROGRESS = 0x11,
    MAX_NOTIFICATION_SIZE_EXCEEDED = 0x12,
    INTERNAL_NOTIFICATION_QUEUE_IS_FULL = 0x13,
    INVALID_NOTIFICATION_TYPE = 0x14,
    MAX_REGISTRATION_COUNT_EXCEEDED = 0x15,
    MAX_CHANNEL_COUNT_EXCEEDED = 0x16,
    LOCAL_ONLY_REGISTRATION = 0x17,
    REMOTE_ONLY_REGISTRATION = 0x18,
} PrintAsyncNotifyError;

#ifdef __cplusplus
}
#endif

#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#endif /* _PRNASNOT_H */
