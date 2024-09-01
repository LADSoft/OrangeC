#ifndef _JOYSTICKAPI_H
#define _JOYSTICKAPI_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* ApiSet api-ms-win-mm-joystick-l1-1-0 */

/* #include <apiset.h> */
#include <apisetcconv.h>

#include <mmsyscom.h>

#ifndef MMNOJOY

#define JOYERR_NOERROR      (0)
#define JOYERR_PARMS        (JOYERR_BASE+5)
#define JOYERR_NOCANDO      (JOYERR_BASE+6)
#define JOYERR_UNPLUGGED    (JOYERR_BASE+7)

#define JOY_BUTTON1         0x0001
#define JOY_BUTTON2         0x0002
#define JOY_BUTTON3         0x0004
#define JOY_BUTTON4         0x0008
#define JOY_BUTTON1CHG      0x0100
#define JOY_BUTTON2CHG      0x0200
#define JOY_BUTTON3CHG      0x0400
#define JOY_BUTTON4CHG      0x0800

#define JOY_BUTTON5         0x00000010l
#define JOY_BUTTON6         0x00000020l
#define JOY_BUTTON7         0x00000040l
#define JOY_BUTTON8         0x00000080l
#define JOY_BUTTON9         0x00000100l
#define JOY_BUTTON10        0x00000200l
#define JOY_BUTTON11        0x00000400l
#define JOY_BUTTON12        0x00000800l
#define JOY_BUTTON13        0x00001000l
#define JOY_BUTTON14        0x00002000l
#define JOY_BUTTON15        0x00004000l
#define JOY_BUTTON16        0x00008000l
#define JOY_BUTTON17        0x00010000l
#define JOY_BUTTON18        0x00020000l
#define JOY_BUTTON19        0x00040000l
#define JOY_BUTTON20        0x00080000l
#define JOY_BUTTON21        0x00100000l
#define JOY_BUTTON22        0x00200000l
#define JOY_BUTTON23        0x00400000l
#define JOY_BUTTON24        0x00800000l
#define JOY_BUTTON25        0x01000000l
#define JOY_BUTTON26        0x02000000l
#define JOY_BUTTON27        0x04000000l
#define JOY_BUTTON28        0x08000000l
#define JOY_BUTTON29        0x10000000l
#define JOY_BUTTON30        0x20000000l
#define JOY_BUTTON31        0x40000000l
#define JOY_BUTTON32        0x80000000l

#define JOY_POVCENTERED         (WORD) -1
#define JOY_POVFORWARD          0
#define JOY_POVRIGHT            9000
#define JOY_POVBACKWARD         18000
#define JOY_POVLEFT             27000

#define JOY_RETURNX             0x00000001l
#define JOY_RETURNY             0x00000002l
#define JOY_RETURNZ             0x00000004l
#define JOY_RETURNR             0x00000008l
#define JOY_RETURNU             0x00000010l
#define JOY_RETURNV             0x00000020l
#define JOY_RETURNPOV           0x00000040l
#define JOY_RETURNBUTTONS       0x00000080l
#define JOY_RETURNRAWDATA       0x00000100l
#define JOY_RETURNPOVCTS        0x00000200l
#define JOY_RETURNCENTERED      0x00000400l
#define JOY_USEDEADZONE         0x00000800l
#define JOY_RETURNALL           (JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | \
                                 JOY_RETURNR | JOY_RETURNU | JOY_RETURNV | \
                                 JOY_RETURNPOV | JOY_RETURNBUTTONS)
#define JOY_CAL_READALWAYS      0x00010000l
#define JOY_CAL_READXYONLY      0x00020000l
#define JOY_CAL_READ3           0x00040000l
#define JOY_CAL_READ4           0x00080000l
#define JOY_CAL_READXONLY       0x00100000l
#define JOY_CAL_READYONLY       0x00200000l
#define JOY_CAL_READ5           0x00400000l
#define JOY_CAL_READ6           0x00800000l
#define JOY_CAL_READZONLY       0x01000000l
#define JOY_CAL_READRONLY       0x02000000l
#define JOY_CAL_READUONLY       0x04000000l
#define JOY_CAL_READVONLY       0x08000000l

#define JOYSTICKID1         0
#define JOYSTICKID2         1

#define JOYCAPS_HASZ            0x0001
#define JOYCAPS_HASR            0x0002
#define JOYCAPS_HASU            0x0004
#define JOYCAPS_HASV            0x0008
#define JOYCAPS_HASPOV          0x0010
#define JOYCAPS_POV4DIR         0x0020
#define JOYCAPS_POVCTS          0x0040

typedef struct tagJOYCAPSA {
    WORD wMid;
    WORD wPid;
    CHAR szPname[MAXPNAMELEN];
    UINT wXmin;
    UINT wXmax;
    UINT wYmin;
    UINT wYmax;
    UINT wZmin;
    UINT wZmax;
    UINT wNumButtons;
    UINT wPeriodMin;
    UINT wPeriodMax;
    UINT wRmin;
    UINT wRmax;
    UINT wUmin;
    UINT wUmax;
    UINT wVmin;
    UINT wVmax;
    UINT wCaps;
    UINT wMaxAxes;
    UINT wNumAxes;
    UINT wMaxButtons;
    CHAR szRegKey[MAXPNAMELEN];
    CHAR szOEMVxD[MAX_JOYSTICKOEMVXDNAME];
} JOYCAPSA, *PJOYCAPSA, *NPJOYCAPSA, *LPJOYCAPSA;

typedef struct tagJOYCAPSW {
    WORD wMid;
    WORD wPid;
    WCHAR szPname[MAXPNAMELEN];
    UINT wXmin;
    UINT wXmax;
    UINT wYmin;
    UINT wYmax;
    UINT wZmin;
    UINT wZmax;
    UINT wNumButtons;
    UINT wPeriodMin;
    UINT wPeriodMax;
    UINT wRmin;
    UINT wRmax;
    UINT wUmin;
    UINT wUmax;
    UINT wVmin;
    UINT wVmax;
    UINT wCaps;
    UINT wMaxAxes;
    UINT wNumAxes;
    UINT wMaxButtons;
    WCHAR szRegKey[MAXPNAMELEN];
    WCHAR szOEMVxD[MAX_JOYSTICKOEMVXDNAME];
} JOYCAPSW, *PJOYCAPSW, *NPJOYCAPSW, *LPJOYCAPSW;

typedef struct tagJOYCAPS2A {
    WORD wMid;
    WORD wPid;
    CHAR szPname[MAXPNAMELEN];
    UINT wXmin;
    UINT wXmax;
    UINT wYmin;
    UINT wYmax;
    UINT wZmin;
    UINT wZmax;
    UINT wNumButtons;
    UINT wPeriodMin;
    UINT wPeriodMax;
    UINT wRmin;
    UINT wRmax;
    UINT wUmin;
    UINT wUmax;
    UINT wVmin;
    UINT wVmax;
    UINT wCaps;
    UINT wMaxAxes;
    UINT wNumAxes;
    UINT wMaxButtons;
    CHAR szRegKey[MAXPNAMELEN];
    CHAR szOEMVxD[MAX_JOYSTICKOEMVXDNAME];
    GUID ManufacturerGuid;
    GUID ProductGuid;
    GUID NameGuid;
} JOYCAPS2A, *PJOYCAPS2A, *NPJOYCAPS2A, *LPJOYCAPS2A;

typedef struct tagJOYCAPS2W {
    WORD wMid;
    WORD wPid;
    WCHAR szPname[MAXPNAMELEN];
    UINT wXmin;
    UINT wXmax;
    UINT wYmin;
    UINT wYmax;
    UINT wZmin;
    UINT wZmax;
    UINT wNumButtons;
    UINT wPeriodMin;
    UINT wPeriodMax;
    UINT wRmin;
    UINT wRmax;
    UINT wUmin;
    UINT wUmax;
    UINT wVmin;
    UINT wVmax;
    UINT wCaps;
    UINT wMaxAxes;
    UINT wNumAxes;
    UINT wMaxButtons;
    WCHAR szRegKey[MAXPNAMELEN];
    WCHAR szOEMVxD[MAX_JOYSTICKOEMVXDNAME];
    GUID ManufacturerGuid;
    GUID ProductGuid;
    GUID NameGuid;
} JOYCAPS2W, *PJOYCAPS2W, *NPJOYCAPS2W, *LPJOYCAPS2W;

typedef struct joyinfo_tag {
    UINT wXpos;
    UINT wYpos;
    UINT wZpos;
    UINT wButtons;
} JOYINFO, *PJOYINFO, *NPJOYINFO, *LPJOYINFO;

typedef struct joyinfoex_tag {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwXpos;
    DWORD dwYpos;
    DWORD dwZpos;
    DWORD dwRpos;
    DWORD dwUpos;
    DWORD dwVpos;
    DWORD dwButtons;
    DWORD dwButtonNumber;
    DWORD dwPOV;
    DWORD dwReserved1;
    DWORD dwReserved2;
} JOYINFOEX, *PJOYINFOEX, *NPJOYINFOEX, *LPJOYINFOEX;

WINMMAPI MMRESULT WINAPI joyGetPosEx(UINT, LPJOYINFOEX);
WINMMAPI UINT WINAPI joyGetNumDevs(void);
WINMMAPI MMRESULT WINAPI joyGetDevCapsA(UINT_PTR, LPJOYCAPSA, UINT);
WINMMAPI MMRESULT WINAPI joyGetDevCapsW(UINT_PTR, LPJOYCAPSW, UINT);
WINMMAPI MMRESULT WINAPI joyGetPos(UINT, LPJOYINFO);
WINMMAPI MMRESULT WINAPI joyGetThreshold(UINT, LPUINT);
WINMMAPI MMRESULT WINAPI joyReleaseCapture(UINT);
WINMMAPI MMRESULT WINAPI joySetCapture(HWND, UINT, UINT, BOOL);
WINMMAPI MMRESULT WINAPI joySetThreshold(UINT, UINT);
WINMMAPI MMRESULT WINAPI joyConfigChanged(DWORD);

#ifdef UNICODE
typedef JOYCAPSW JOYCAPS;
typedef PJOYCAPSW PJOYCAPS;
typedef NPJOYCAPSW NPJOYCAPS;
typedef LPJOYCAPSW LPJOYCAPS;
typedef JOYCAPS2W JOYCAPS2;
typedef PJOYCAPS2W PJOYCAPS2;
typedef NPJOYCAPS2W NPJOYCAPS2;
typedef LPJOYCAPS2W LPJOYCAPS2;
#define joyGetDevCaps  joyGetDevCapsW
#else /* !UNICODE */
typedef JOYCAPSA JOYCAPS;
typedef PJOYCAPSA PJOYCAPS;
typedef NPJOYCAPSA NPJOYCAPS;
typedef LPJOYCAPSA LPJOYCAPS;
typedef JOYCAPS2A JOYCAPS2;
typedef PJOYCAPS2A PJOYCAPS2;
typedef NPJOYCAPS2A NPJOYCAPS2;
typedef LPJOYCAPS2A LPJOYCAPS2;
#define joyGetDevCaps  joyGetDevCapsA
#endif /* !UNICODE */

#endif /* MMNOJOY */

#endif /* _JOYSTICKAPI_H */
