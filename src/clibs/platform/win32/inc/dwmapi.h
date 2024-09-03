#ifndef _DWMAPI_H
#define _DWMAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows Desktop Window Manager API definitions (Vista) */

#ifndef DWMAPI
#define DWMAPI         EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define DWMAPI_(type)  EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#endif /* DWMAPI */

#include <pshpack1.h>

#ifdef __cplusplus
extern "C" {
#endif


#include <wtypes.h>
#include <uxtheme.h>

#define DWM_BB_ENABLE  0x00000001
#define DWM_BB_BLURREGION  0x00000002
#define DWM_BB_TRANSITIONONMAXIMIZED  0x00000004

#define DWM_TNP_RECTDESTINATION  0x00000001
#define DWM_TNP_RECTSOURCE  0x00000002
#define DWM_TNP_OPACITY  0x00000004
#define DWM_TNP_VISIBLE  0x00000008
#define DWM_TNP_SOURCECLIENTAREAONLY  0x00000010

#define DWM_FRAME_DURATION_DEFAULT  -1

#define DWM_EC_DISABLECOMPOSITION  0
#define DWM_EC_ENABLECOMPOSITION  1

typedef struct _DWM_BLURBEHIND {
    DWORD dwFlags;
    BOOL fEnable;
    HRGN hRgnBlur;
    BOOL fTransitionOnMaximized;
} DWM_BLURBEHIND, *PDWM_BLURBEHIND;

enum DWMWINDOWATTRIBUTE {
    DWMWA_NCRENDERING_ENABLED = 1,
    DWMWA_NCRENDERING_POLICY,
    DWMWA_TRANSITIONS_FORCEDISABLED,
    DWMWA_ALLOW_NCPAINT,
    DWMWA_CAPTION_BUTTON_BOUNDS,
    DWMWA_NONCLIENT_RTL_LAYOUT,
    DWMWA_FORCE_ICONIC_REPRESENTATION,
    DWMWA_FLIP3D_POLICY,
    DWMWA_EXTENDED_FRAME_BOUNDS,
    DWMWA_HAS_ICONIC_BITMAP,
    DWMWA_DISALLOW_PEEK,
    DWMWA_EXCLUDED_FROM_PEEK,
    DWMWA_LAST
};

enum DWMNCRENDERINGPOLICY {
    DWMNCRP_USEWINDOWSTYLE,
    DWMNCRP_DISABLED,
    DWMNCRP_ENABLED,
    DWMNCRP_LAST
};

enum DWMFLIP3DWINDOWPOLICY {
    DWMFLIP3D_DEFAULT,
    DWMFLIP3D_EXCLUDEBELOW,
    DWMFLIP3D_EXCLUDEABOVE,
    DWMFLIP3D_LAST
};

typedef HANDLE HTHUMBNAIL;
typedef HTHUMBNAIL *PHTHUMBNAIL;

typedef struct _DWM_THUMBNAIL_PROPERTIES {
    DWORD dwFlags;
    RECT rcDestination;
    RECT rcSource;
    BYTE opacity;
    BOOL fVisible;
    BOOL fSourceClientAreaOnly;
} DWM_THUMBNAIL_PROPERTIES, *PDWM_THUMBNAIL_PROPERTIES;

typedef ULONGLONG DWM_FRAME_COUNT;
typedef ULONGLONG QPC_TIME;

typedef struct _UNSIGNED_RATIO {
    UINT32 uiNumerator;
    UINT32 uiDenominator;
} UNSIGNED_RATIO;

typedef struct _DWM_TIMING_INFO {
    UINT32 cbSize;
    UNSIGNED_RATIO rateRefresh;
    QPC_TIME qpcRefreshPeriod;
    UNSIGNED_RATIO rateCompose;
    QPC_TIME qpcVBlank;
    DWM_FRAME_COUNT cRefresh;
    UINT cDXRefresh;
    QPC_TIME qpcCompose;
    DWM_FRAME_COUNT cFrame;
    UINT cDXPresent;
    DWM_FRAME_COUNT cRefreshFrame;
    DWM_FRAME_COUNT cFrameSubmitted;
    UINT cDXPresentSubmitted;
    DWM_FRAME_COUNT cFrameConfirmed;
    UINT cDXPresentConfirmed;
    DWM_FRAME_COUNT cRefreshConfirmed;
    UINT cDXRefreshConfirmed;
    DWM_FRAME_COUNT cFramesLate;
    UINT cFramesOutstanding;
    DWM_FRAME_COUNT cFrameDisplayed;
    QPC_TIME qpcFrameDisplayed;
    DWM_FRAME_COUNT cRefreshFrameDisplayed;
    DWM_FRAME_COUNT cFrameComplete;
    QPC_TIME qpcFrameComplete;
    DWM_FRAME_COUNT cFramePending;
    QPC_TIME qpcFramePending;
    DWM_FRAME_COUNT cFramesDisplayed;
    DWM_FRAME_COUNT cFramesComplete;
    DWM_FRAME_COUNT cFramesPending;
    DWM_FRAME_COUNT cFramesAvailable;
    DWM_FRAME_COUNT cFramesDropped;
    DWM_FRAME_COUNT cFramesMissed;
    DWM_FRAME_COUNT cRefreshNextDisplayed;
    DWM_FRAME_COUNT cRefreshNextPresented;
    DWM_FRAME_COUNT cRefreshesDisplayed;
    DWM_FRAME_COUNT cRefreshesPresented;
    DWM_FRAME_COUNT cRefreshStarted;
    ULONGLONG cPixelsReceived;
    ULONGLONG cPixelsDrawn;
    DWM_FRAME_COUNT cBuffersEmpty;
} DWM_TIMING_INFO;

typedef enum {
    DWM_SOURCE_FRAME_SAMPLING_POINT,
    DWM_SOURCE_FRAME_SAMPLING_COVERAGE,
    DWM_SOURCE_FRAME_SAMPLING_LAST
} DWM_SOURCE_FRAME_SAMPLING;

/* static const UINT c_DwmMaxQueuedBuffers = 8; */
/* static const UINT c_DwmMaxMonitors = 16; */
/* static const UINT c_DwmMaxAdapters = 16; */

typedef struct _DWM_PRESENT_PARAMETERS {
    UINT32 cbSize;
    BOOL fQueue;
    DWM_FRAME_COUNT cRefreshStart;
    UINT cBuffer;
    BOOL fUseSourceRate;
    UNSIGNED_RATIO rateSource;
    UINT cRefreshesPerFrame;
    DWM_SOURCE_FRAME_SAMPLING eSampling;
} DWM_PRESENT_PARAMETERS;

#ifndef _MIL_MATRIX3X2D_DEFINED
typedef struct _MIL_MATRIX3X2D {
    DOUBLE S_11;
    DOUBLE S_12;
    DOUBLE S_21;
    DOUBLE S_22;
    DOUBLE DX;
    DOUBLE DY;
} MIL_MATRIX3X2D;
#define _MIL_MATRIX3X2D_DEFINED
#endif /* _MIL_MATRIX3X2D_DEFINED */

enum DWMTRANSITION_OWNEDWINDOW_TARGET {
    DWMTRANSITION_OWNEDWINDOW_NULL = -1,
    DWMTRANSITION_OWNEDWINDOW_REPOSITION = 0,
};

#if (NTDDI_VERSION >= NTDDI_WIN8)

enum GESTURE_TYPE {
    GT_PEN_TAP = 0,
    GT_PEN_DOUBLETAP = 1,
    GT_PEN_RIGHTTAP = 2,
    GT_PEN_PRESSANDHOLD = 3,
    GT_PEN_PRESSANDHOLDABORT = 4,
    GT_TOUCH_TAP = 5,
    GT_TOUCH_DOUBLETAP = 6,
    GT_TOUCH_RIGHTTAP = 7,
    GT_TOUCH_PRESSANDHOLD = 8,
    GT_TOUCH_PRESSANDHOLDABORT = 9,
    GT_TOUCH_PRESSANDTAP = 10,
};

enum DWM_SHOWCONTACT {
    DWMSC_DOWN = 0x00000001,
    DWMSC_UP = 0x00000002,
    DWMSC_DRAG = 0x00000004,
    DWMSC_HOLD = 0x00000008,
    DWMSC_PENBARREL = 0x00000010,
    DWMSC_NONE = 0x00000000,
    DWMSC_ALL = 0xFFFFFFFF
};
/* DEFINE_ENUM_FLAG_OPERATORS(DWM_SHOWCONTACT); */

#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

DWMAPI_(BOOL) DwmDefWindowProc(HWND, UINT, WPARAM, LPARAM, LRESULT *);
DWMAPI DwmEnableBlurBehindWindow(HWND, const DWM_BLURBEHIND *);
#if NTDDI_VERSION >= NTDDI_WIN8
__declspec(deprecated) DWMAPI DwmEnableComposition(UINT);
#else /* NTDDI_VERSION < NTDDI_WIN8 */
DWMAPI DwmEnableComposition(UINT);
#endif /* NTDDI_VERSION < NTDDI_WIN8 */
DWMAPI DwmEnableMMCSS(BOOL);
DWMAPI DwmExtendFrameIntoClientArea(HWND, const MARGINS *);
DWMAPI DwmGetColorizationColor(DWORD *, BOOL *);
DWMAPI DwmGetCompositionTimingInfo(HWND, DWM_TIMING_INFO *);
DWMAPI DwmGetWindowAttribute(HWND, DWORD, PVOID, DWORD);
DWMAPI DwmIsCompositionEnabled(BOOL *);
DWMAPI DwmModifyPreviousDxFrameDuration(HWND, INT, BOOL);
DWMAPI DwmQueryThumbnailSourceSize(HTHUMBNAIL, PSIZE);
DWMAPI DwmRegisterThumbnail(HWND, HWND, PHTHUMBNAIL);
DWMAPI DwmSetDxFrameDuration(HWND, INT);
DWMAPI DwmSetPresentParameters(HWND, DWM_PRESENT_PARAMETERS *);
DWMAPI DwmSetWindowAttribute(HWND, DWORD, LPCVOID, DWORD);
DWMAPI DwmUnregisterThumbnail(HTHUMBNAIL);
DWMAPI DwmUpdateThumbnailProperties(HTHUMBNAIL, const DWM_THUMBNAIL_PROPERTIES *);
#if (_WIN32_WINNT >= 0x0601)
#define DWM_SIT_DISPLAYFRAME  0x00000001
DWMAPI DwmSetIconicThumbnail(HWND, HBITMAP, DWORD);
DWMAPI DwmSetIconicLivePreviewBitmap(HWND, HBITMAP, POINT *, DWORD);
DWMAPI DwmInvalidateIconicBitmaps(HWND);
#endif /* _WIN32_WINNT >= 0x0601 */
DWMAPI DwmAttachMilContent(HWND);
DWMAPI DwmDetachMilContent(HWND);
DWMAPI DwmFlush(void);
DWMAPI DwmGetGraphicsStreamTransformHint(UINT, MIL_MATRIX3X2D *);
DWMAPI DwmGetGraphicsStreamClient(UINT, UUID *);
DWMAPI DwmGetTransportAttributes(BOOL *, BOOL *, DWORD *);
DWMAPI DwmTransitionOwnedWindow(HWND, enum DWMTRANSITION_OWNEDWINDOW_TARGET);
#if (NTDDI_VERSION >= NTDDI_WIN8)
DWMAPI DwmRenderGesture(enum GESTURE_TYPE, UINT, const DWORD *, const POINT *);
DWMAPI DwmTetherContact(DWORD, BOOL, POINT);
DWMAPI DwmShowContact(DWORD, enum DWM_SHOWCONTACT);
#endif /* NTDDI_VERSION >= NTDDI_WIN8 */


#ifdef __cplusplus
}
#endif

#include <poppack.h>

#endif /* _DWMAPI_H */
