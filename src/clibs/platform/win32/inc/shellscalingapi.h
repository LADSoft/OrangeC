
#ifdef __ORANGEC__ 
#pragma once
#endif

#include <shtypes.h>
/* #include <winapifamily.h> */

#ifndef SCALING_ENUMS_DECLARED

typedef enum {
    DEVICE_PRIMARY = 0,
    DEVICE_IMMERSIVE = 1,
} DISPLAY_DEVICE_TYPE;

typedef enum {
    SCF_VALUE_NONE = 0x00,
    SCF_SCALE = 0x01,
    SCF_PHYSICAL = 0x02,
} SCALE_CHANGE_FLAGS;
/* DEFINE_ENUM_FLAG_OPERATORS(SCALE_CHANGE_FLAGS); */

#define SCALING_ENUMS_DECLARED
#endif /* SCALING_ENUMS_DECLARED */

#if (NTDDI_VERSION >= NTDDI_WIN8)
STDAPI_(DEVICE_SCALE_FACTOR) GetScaleFactorForDevice(DISPLAY_DEVICE_TYPE);
STDAPI RegisterScaleChangeNotifications(DISPLAY_DEVICE_TYPE, HWND, UINT, DWORD *);
STDAPI RevokeScaleChangeNotifications(DISPLAY_DEVICE_TYPE, DWORD);
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

#if (NTDDI_VERSION >= NTDDI_WINBLUE)
STDAPI GetScaleFactorForMonitor(HMONITOR, DEVICE_SCALE_FACTOR *);
STDAPI RegisterScaleChangeEvent(HANDLE, DWORD_PTR *);
STDAPI UnregisterScaleChangeEvent(DWORD_PTR);
#endif /* NTDDI_VERSION >= NTDDI_WINBLUE */

#ifndef DPI_ENUMS_DECLARED

typedef enum PROCESS_DPI_AWARENESS {
    PROCESS_DPI_UNAWARE = 0,
    PROCESS_SYSTEM_DPI_AWARE = 1,
    PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;

typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;

#define DPI_ENUMS_DECLARED
#endif /* DPI_ENUMS_DECLARED */

#if (NTDDI_VERSION >= NTDDI_WINBLUE)

STDAPI SetProcessDpiAwareness(PROCESS_DPI_AWARENESS);
STDAPI GetProcessDpiAwareness(HANDLE hprocess, PROCESS_DPI_AWARENESS *);
STDAPI GetDpiForMonitor(HMONITOR, MONITOR_DPI_TYPE, UINT *, UINT *);

#endif /* NTDDI_VERSION >= NTDDI_WINBLUE */