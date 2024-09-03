#ifndef _UXTHEME_H
#define _UXTHEME_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Windows Theme API definitions (Windows Vista/7/8) */

#include <commctrl.h>

#ifndef THEMEAPI
#define THEMEAPI         EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define THEMEAPI_(type)  EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#endif

#if (NTDDI_VERSION >= NTDDI_VISTA)
#define MAX_THEMECOLOR  64
#define MAX_THEMESIZE 64
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#if (NTDDI_VERSION >= NTDDI_VISTA)
#define OTD_FORCE_RECT_SIZING  0x00000001
#define OTD_NONCLIENT  0x00000002
#define OTD_VALIDBITS  (OTD_FORCE_RECT_SIZING|OTD_NONCLIENT)
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#define DTBG_CLIPRECT  0x00000001
#define DTBG_DRAWSOLID  0x00000002
#define DTBG_OMITBORDER  0x00000004
#define DTBG_OMITCONTENT  0x00000008
#define DTBG_COMPUTINGREGION  0x00000010
#define DTBG_MIRRORDC  0x00000020
#define DTBG_NOMIRROR  0x00000040
#define DTBG_VALIDBITS  (DTBG_CLIPRECT|DTBG_DRAWSOLID|DTBG_OMITBORDER|DTBG_OMITCONTENT|DTBG_COMPUTINGREGION|DTBG_MIRRORDC|DTBG_NOMIRROR)

#define DTT_GRAYED  0x1
#define DTT_FLAGS2VALIDBITS  (DTT_GRAYED)

#define HTTB_BACKGROUNDSEG  0x0000
#define HTTB_FIXEDBORDER  0x0002
#define HTTB_CAPTION  0x0004
#define HTTB_RESIZINGBORDER_LEFT  0x0010
#define HTTB_RESIZINGBORDER_TOP  0x0020
#define HTTB_RESIZINGBORDER_RIGHT  0x0040
#define HTTB_RESIZINGBORDER_BOTTOM  0x0080
#define HTTB_RESIZINGBORDER  (HTTB_RESIZINGBORDER_LEFT|HTTB_RESIZINGBORDER_TOP|HTTB_RESIZINGBORDER_RIGHT|HTTB_RESIZINGBORDER_BOTTOM)
#define HTTB_SIZINGTEMPLATE  0x0100
#define HTTB_SYSTEMSIZINGMARGINS  0x0200

#if (NTDDI_VERSION >= NTDDI_VISTA)
#define MAX_INTLIST_COUNT  402
#else /* NTDDI_VERSION < NTDDI_VISTA */
#define MAX_INTLIST_COUNT  10
#endif /* NTDDI_VERSION < NTDDI_VISTA */

#define ETDT_DISABLE  0x00000001
#define ETDT_ENABLE  0x00000002
#define ETDT_USETABTEXTURE  0x00000004
#define ETDT_ENABLETAB  (ETDT_ENABLE|ETDT_USETABTEXTURE)

#if (NTDDI_VERSION >= NTDDI_VISTA)
#define ETDT_USEAEROWIZARDTABTEXTURE  0x00000008
#define ETDT_ENABLEAEROWIZARDTAB  (ETDT_ENABLE|ETDT_USEAEROWIZARDTABTEXTURE)
#define ETDT_VALIDBITS  (ETDT_DISABLE|ETDT_ENABLE|ETDT_USETABTEXTURE|ETDT_USEAEROWIZARDTABTEXTURE)
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#define STAP_ALLOW_NONCLIENT  (1<<0)
#define STAP_ALLOW_CONTROLS  (1<<1)
#define STAP_ALLOW_WEBCONTENT  (1<<2)
#define STAP_VALIDBITS  (STAP_ALLOW_NONCLIENT|STAP_ALLOW_CONTROLS|STAP_ALLOW_WEBCONTENT)

#define SZ_THDOCPROP_DISPLAYNAME  L"DisplayName"
#define SZ_THDOCPROP_CANONICALNAME  L"ThemeName"
#define SZ_THDOCPROP_TOOLTIP  L"ToolTip"
#define SZ_THDOCPROP_AUTHOR  L"author"

#if (NTDDI_VERSION >= NTDDI_VISTA)
#define GBF_DIRECT  0x00000001
#define GBF_COPY  0x00000002
#define GBF_VALIDBITS  (GBF_DIRECT|GBF_COPY)
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#if (NTDDI_VERSION >= NTDDI_VISTA)
#define DTPB_WINDOWDC  0x00000001
#define DTPB_USECTLCOLORSTATIC  0x00000002
#define DTPB_USEERASEBKGND  0x00000004
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#if (NTDDI_VERSION >= NTDDI_VISTA)
#define WTNCA_NODRAWCAPTION  0x00000001
#define WTNCA_NODRAWICON  0x00000002
#define WTNCA_NOSYSMENU  0x00000004
#define WTNCA_NOMIRRORHELP  0x00000008
#define WTNCA_VALIDBITS  (WTNCA_NODRAWCAPTION|WTNCA_NODRAWICON|WTNCA_NOSYSMENU|WTNCA_NOMIRRORHELP)
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

#define DTT_TEXTCOLOR     (1UL << 0)
#define DTT_BORDERCOLOR   (1UL << 1)
#define DTT_SHADOWCOLOR   (1UL << 2)
#define DTT_SHADOWTYPE    (1UL << 3)
#define DTT_SHADOWOFFSET  (1UL << 4)
#define DTT_BORDERSIZE    (1UL << 5)
#define DTT_FONTPROP      (1UL << 6)
#define DTT_COLORPROP     (1UL << 7)
#define DTT_STATEID       (1UL << 8)
#define DTT_CALCRECT      (1UL << 9)
#define DTT_APPLYOVERLAY  (1UL << 10)
#define DTT_GLOWSIZE      (1UL << 11)
#define DTT_CALLBACK      (1UL << 12)
#define DTT_COMPOSITED    (1UL << 13)
#define DTT_VALIDBITS     (DTT_TEXTCOLOR|DTT_BORDERCOLOR|DTT_SHADOWCOLOR|DTT_SHADOWTYPE| \
                           DTT_SHADOWOFFSET|DTT_BORDERSIZE|DTT_FONTPROP|DTT_COLORPROP| \
                           DTT_STATEID|DTT_CALCRECT|DTT_APPLYOVERLAY|DTT_GLOWSIZE|DTT_COMPOSITED)

#if (NTDDI_VERSION >= NTDDI_VISTA)
#define BPBF_COMPOSITED  BPBF_TOPDOWNDIB

#define BPPF_ERASE  0x0001
#define BPPF_NOCLIP  0x0002
#define BPPF_NONCLIENT  0x0004

#define BufferedPaintMakeOpaque(hBufferedPaint,prc)  BufferedPaintSetAlpha((hBufferedPaint),(prc),255)
#endif /* NTDDI_VERSION >= NTDDI_VISTA */

typedef HANDLE HTHEME;

typedef struct _DTBGOPTS {
    DWORD dwSize;
    DWORD dwFlags;
    RECT rcClip;
} DTBGOPTS, *PDTBGOPTS;

enum THEMESIZE {
    TS_MIN,
    TS_TRUE,
    TS_DRAW,
};

typedef struct _MARGINS {
    int cxLeftWidth;
    int cxRightWidth;
    int cyTopHeight;
    int cyBottomHeight;
} MARGINS, *PMARGINS;

typedef struct _INTLIST {
    int iValueCount;
    int iValues[MAX_INTLIST_COUNT];
} INTLIST, *PINTLIST;

enum PROPERTYORIGIN {
    PO_STATE,
    PO_PART,
    PO_CLASS,
    PO_GLOBAL,
    PO_NOTFOUND
};

#if (NTDDI_VERSION >= NTDDI_VISTA)

enum WINDOWTHEMEATTRIBUTETYPE {
    WTA_NONCLIENT = 1
};

typedef struct _WTA_OPTIONS {
    DWORD dwFlags;
    DWORD dwMask;
} WTA_OPTIONS, *PWTA_OPTIONS;

typedef HANDLE HPAINTBUFFER;

typedef enum _BP_BUFFERFORMAT {
    BPBF_COMPATIBLEBITMAP,
    BPBF_DIB,
    BPBF_TOPDOWNDIB,
    BPBF_TOPDOWNMONODIB
} BP_BUFFERFORMAT;

typedef enum _BP_ANIMATIONSTYLE {
    BPAS_NONE,
    BPAS_LINEAR,
    BPAS_CUBIC,
    BPAS_SINE
} BP_ANIMATIONSTYLE;

typedef struct _BP_ANIMATIONPARAMS {
    DWORD cbSize;
    DWORD dwFlags;
    BP_ANIMATIONSTYLE style;
    DWORD dwDuration;
} BP_ANIMATIONPARAMS, *PBP_ANIMATIONPARAMS;

typedef struct _BP_PAINTPARAMS {
    DWORD cbSize;
    DWORD dwFlags;
    const RECT *prcExclude;
    const BLENDFUNCTION *pBlendFunction;
} BP_PAINTPARAMS, *PBP_PAINTPARAMS;

typedef HANDLE HANIMATIONBUFFER;

#endif /* NTDDI_VERSION >= NTDDI_VISTA */

typedef int (WINAPI *DTT_CALLBACK_PROC)(HDC, LPWSTR, int, LPRECT, UINT, LPARAM);  /* Vista+ */

typedef struct _DTTOPTS {  /* Vista+ */
    DWORD dwSize;
    DWORD dwFlags;
    COLORREF crText;
    COLORREF crBorder;
    COLORREF crShadow;
    int iTextShadowType;
    POINT ptShadowOffset;
    int iBorderSize;
    int iFontPropId;
    int iColorPropId;
    int iStateId;
    BOOL fApplyOverlay;
    int iGlowSize;
    DTT_CALLBACK_PROC pfnDrawTextCallback;
    LPARAM lParam;
} DTTOPTS, *PDTTOPTS;

#if (NTDDI_VERSION >= NTDDI_WIN8)

typedef enum TA_PROPERTY {
    TAP_FLAGS,
    TAP_TRANSFORMCOUNT,
    TAP_STAGGERDELAY,
    TAP_STAGGERDELAYCAP,
    TAP_STAGGERDELAYFACTOR,
    TAP_ZORDER,
} TA_PROPERTY;

typedef enum TA_PROPERTY_FLAG {
    TAPF_NONE = 0x0,
    TAPF_HASSTAGGER = 0x1,
    TAPF_ISRTLAWARE = 0x2,
    TAPF_ALLOWCOLLECTION = 0x4,
    TAPF_HASBACKGROUND = 0x8,
    TAPF_HASPERSPECTIVE = 0x10,
} TA_PROPERTY_FLAG;

/* DEFINE_ENUM_FLAG_OPERATORS(TA_PROPERTY_FLAG); */

typedef enum TA_TRANSFORM_TYPE {
    TATT_TRANSLATE_2D,
    TATT_SCALE_2D,
    TATT_OPACITY,
    TATT_CLIP,
} TA_TRANSFORM_TYPE;

typedef enum TA_TRANSFORM_FLAG {
    TATF_NONE = 0x0,
    TATF_TARGETVALUES_USER = 0x1,
    TATF_HASINITIALVALUES = 0x2,
    TATF_HASORIGINVALUES = 0x4,
} TA_TRANSFORM_FLAG;

/* DEFINE_ENUM_FLAG_OPERATORS(TA_TRANSFORM_FLAG); */

#include <pshpack8.h>
typedef struct TA_TRANSFORM {
    TA_TRANSFORM_TYPE eTransformType;
    DWORD dwTimingFunctionId;
    DWORD dwStartTime;
    DWORD dwDurationTime;
    TA_TRANSFORM_FLAG eFlags;
} TA_TRANSFORM, *PTA_TRANSFORM;

typedef struct TA_TRANSFORM_2D {
    TA_TRANSFORM header;
    float rX;
    float rY;
    float rInitialX;
    float rInitialY;
    float rOriginX;
    float rOriginY;
} TA_TRANSFORM_2D, *PTA_TRANSFORM_2D;

typedef struct TA_TRANSFORM_OPACITY {
    TA_TRANSFORM header;
    float rOpacity;
    float rInitialOpacity;
} TA_TRANSFORM_OPACITY, *PTA_TRANSFORM_OPACITY;

typedef struct TA_TRANSFORM_CLIP {
    TA_TRANSFORM header;
    float rLeft;
    float rTop;
    float rRight;
    float rBottom;
    float rInitialLeft;
    float rInitialTop;
    float rInitialRight;
    float rInitialBottom;
} TA_TRANSFORM_CLIP, *PTA_TRANSFORM_CLIP;
#include <poppack.h>

typedef enum TA_TIMINGFUNCTION_TYPE {
    TTFT_UNDEFINED,
    TTFT_CUBIC_BEZIER,
} TA_TIMINGFUNCTION_TYPE;

#include <pshpack8.h>
typedef struct TA_TIMINGFUNCTION {
    TA_TIMINGFUNCTION_TYPE eTimingFunctionType;
} TA_TIMINGFUNCTION, *PTA_TIMINGFUNCTION;

typedef struct TA_CUBIC_BEZIER {
    TA_TIMINGFUNCTION header;
    float rX0;
    float rY0;
    float rX1;
    float rY1;
} TA_CUBIC_BEZIER, *PTA_CUBIC_BEZIER;
#include <poppack.h>

#endif /* NTDDI_VERSION >= NTDDI_WIN8 */

THEMEAPI_(HTHEME) OpenThemeData(HWND,LPCWSTR);
THEMEAPI_(HTHEME) OpenThemeDataForDpi(HWND,LPCWSTR,UINT);  // ? version intro
#if (NTDDI_VERSION >= NTDDI_VISTA)
THEMEAPI_(HTHEME) OpenThemeDataEx(HWND,LPCWSTR,DWORD);
#endif /* NTDDI_VERSION >= NTDDI_VISTA */
THEMEAPI CloseThemeData(HTHEME);
THEMEAPI DrawThemeBackground(HTHEME,HDC,int,int,const RECT*,const RECT*);
THEMEAPI DrawThemeBackgroundEx(HTHEME,HDC,int,int,const RECT*,const DTBGOPTS*);
THEMEAPI DrawThemeText(HTHEME,HDC,int,int,LPCWSTR,int,DWORD,DWORD,const RECT*);
THEMEAPI GetThemeBackgroundContentRect(HTHEME,HDC,int,int,const RECT*,RECT*);
THEMEAPI GetThemeBackgroundExtent(HTHEME,HDC,int,int,const RECT*,RECT*);
THEMEAPI GetThemeBackgroundRegion(HTHEME,HDC,int,int,const RECT*,HRGN*);
THEMEAPI GetThemePartSize(HTHEME,HDC,int,int,RECT*,enum THEMESIZE,SIZE*);
THEMEAPI GetThemeTextExtent(HTHEME,HDC,int,int,LPCWSTR,int,DWORD,const RECT*,RECT*);
THEMEAPI GetThemeTextMetrics(HTHEME,HDC,int,int,TEXTMETRIC*);
THEMEAPI HitTestThemeBackground(HTHEME,HDC,int,int,DWORD,const RECT*,HRGN,POINT,WORD*);
THEMEAPI DrawThemeEdge(HTHEME,HDC,int,int,const RECT*,UINT,UINT,RECT*);
THEMEAPI DrawThemeIcon(HTHEME,HDC,int,int,const RECT*,HIMAGELIST,int);
THEMEAPI_(BOOL) IsThemePartDefined(HTHEME,int,int);
THEMEAPI_(BOOL) IsThemeBackgroundPartiallyTransparent(HTHEME,int,int);
THEMEAPI GetThemeColor(HTHEME,int,int,int,COLORREF*);
THEMEAPI GetThemeMetric(HTHEME,HDC,int,int,int,int*);
THEMEAPI GetThemeString(HTHEME,int,int,int,LPWSTR,int);
THEMEAPI GetThemeBool(HTHEME,int,int,int,BOOL*);
THEMEAPI GetThemeInt(HTHEME,int,int,int,int*);
THEMEAPI GetThemeEnumValue(HTHEME,int,int,int,int*);
THEMEAPI GetThemePosition(HTHEME,int,int,int,POINT*);
THEMEAPI GetThemeFont(HTHEME,HDC,int,int,int,LOGFONT*);
THEMEAPI GetThemeRect(HTHEME,int,int,int,RECT*);
THEMEAPI GetThemeMargins(HTHEME,HDC,int,int,int,RECT*,MARGINS*);
THEMEAPI GetThemeIntList(HTHEME,int,int,int,INTLIST*);
THEMEAPI GetThemePropertyOrigin(HTHEME,int,int,int,enum PROPERTYORIGIN*);
THEMEAPI SetWindowTheme(HWND,LPCWSTR,LPCWSTR);
THEMEAPI GetThemeFilename(HTHEME,int,int,int,LPWSTR,int);
THEMEAPI_(COLORREF) GetThemeSysColor(HTHEME,int);
THEMEAPI_(HBRUSH) GetThemeSysColorBrush(HTHEME,int);
THEMEAPI_(BOOL) GetThemeSysBool(HTHEME,int);
THEMEAPI_(int) GetThemeSysSize(HTHEME,int);
THEMEAPI GetThemeSysFont(HTHEME,int,LOGFONT*);
THEMEAPI GetThemeSysString(HTHEME,int,LPWSTR,int);
THEMEAPI GetThemeSysInt(HTHEME,int,int*);
THEMEAPI_(BOOL) IsThemeActive(void);
THEMEAPI_(BOOL) IsAppThemed(void);
THEMEAPI_(HTHEME) GetWindowTheme(HWND);
THEMEAPI EnableThemeDialogTexture(HWND,DWORD);
THEMEAPI_(BOOL) IsThemeDialogTextureEnabled(HWND);
THEMEAPI_(DWORD) GetThemeAppProperties(void);
THEMEAPI_(void) SetThemeAppProperties(DWORD);
THEMEAPI GetCurrentThemeName(LPWSTR,int,LPWSTR,int,LPWSTR,int);
THEMEAPI GetThemeDocumentationProperty(LPCWSTR,LPCWSTR,LPWSTR,int);
THEMEAPI DrawThemeParentBackground(HWND,HDC,RECT*);
THEMEAPI EnableTheming(BOOL);
#if (NTDDI_VERSION >= NTDDI_VISTA)
THEMEAPI DrawThemeParentBackgroundEx(HWND,HDC,DWORD,const RECT*);
THEMEAPI SetWindowThemeAttribute(HWND,enum WINDOWTHEMEATTRIBUTETYPE,PVOID,DWORD);
inline HRESULT SetWindowThemeNonClientAttributes(HWND hwnd, DWORD dwMask, DWORD dwAttributes) {
    WTA_OPTIONS wta = { .dwMask = dwMask, .dwFlags = dwAttributes };
    return SetWindowThemeAttribute(hwnd, WTA_NONCLIENT, &wta, sizeof(wta));
}
THEMEAPI DrawThemeTextEx(HTHEME,HDC,int,int,LPCWSTR,int,DWORD,LPRECT,const DTTOPTS *);
THEMEAPI GetThemeBitmap(HTHEME,int,int,int,ULONG,HBITMAP*);
THEMEAPI GetThemeStream(HTHEME,int,int,int,VOID **,DWORD*,HINSTANCE);
THEMEAPI BufferedPaintInit(void);
THEMEAPI BufferedPaintUnInit(void);
THEMEAPI_(HPAINTBUFFER) BeginBufferedPaint(HDC,const RECT*,BP_BUFFERFORMAT,BP_PAINTPARAMS*,HDC*);
THEMEAPI EndBufferedPaint(HPAINTBUFFER,BOOL);
THEMEAPI GetBufferedPaintTargetRect(HPAINTBUFFER,RECT*);
THEMEAPI_(HDC) GetBufferedPaintTargetDC(HPAINTBUFFER);
THEMEAPI_(HDC) GetBufferedPaintDC(HPAINTBUFFER);
THEMEAPI GetBufferedPaintBits(HPAINTBUFFER,RGBQUAD**,int*);
THEMEAPI BufferedPaintClear(HPAINTBUFFER,const RECT*);
THEMEAPI BufferedPaintSetAlpha(HPAINTBUFFER,const RECT*,BYTE);
THEMEAPI BufferedPaintStopAllAnimations(HWND);
THEMEAPI_(HANIMATIONBUFFER) BeginBufferedAnimation(HWND,HDC,const RECT*,BP_BUFFERFORMAT,BP_PAINTPARAMS*,BP_ANIMATIONPARAMS*,HDC*,HDC*);
THEMEAPI EndBufferedAnimation(HANIMATIONBUFFER,BOOL);
THEMEAPI_(BOOL) BufferedPaintRenderAnimation(HWND,HDC);
THEMEAPI_(BOOL) IsCompositionActive(void);
THEMEAPI GetThemeTransitionDuration(HTHEME,int,int,int,int,DWORD*);
#endif /* NTDDI_VERSION >= NTDDI_VISTA */
#if (NTDDI_VERSION >= NTDDI_WIN7)
BOOL WINAPI BeginPanningFeedback(HWND);
BOOL WINAPI UpdatePanningFeedback(HWND,LONG,LONG,BOOL);
BOOL WINAPI EndPanningFeedback(HWND,BOOL);
#endif /* NTDDI_VERSION >= NTDDI_WIN7 */
#if (NTDDI_VERSION >= NTDDI_WIN8)
THEMEAPI GetThemeAnimationProperty(HTHEME, int, int, TA_PROPERTY, VOID *, DWORD, DWORD *);
THEMEAPI GetThemeAnimationTransform(HTHEME, int, int, DWORD, TA_TRANSFORM *, DWORD, DWORD *);
THEMEAPI GetThemeTimingFunction(HTHEME, int, TA_TIMINGFUNCTION *, DWORD, DWORD *);
#endif /* NTDDI_WIN8 */

#endif /* _UXTHEME_H */
