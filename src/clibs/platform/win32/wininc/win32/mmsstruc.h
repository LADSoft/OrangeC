/* 
   mmsstruc.h

   Base definitions

   Copyright (C) 1996 Free Software Foundation, Inc.

   This file is part of the Windows32 API Library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
   
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   If you are interested in a warranty or support for this source code,
   contact Scott Christley <scottc@net-community.com> for more information.
   
   You should have received a copy of the GNU Library General Public
   License along with this library; see the file COPYING.LIB.
   If not, write to the Free Software Foundation, 
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

   -----------
   DAL 2003 - this file modified extensively for my compiler.  New
   definitionswnwn added as well.
*/ 
#pragma pack(1)

typedef UINT    MMVERSION;  
typedef UINT	MMRESULT ;				
typedef UINT   *LPUINT;

typedef struct mmtime_tag
{
    UINT            wType;      
    union
    {
    DWORD       ms;         
    DWORD       sample;     
    DWORD       cb;         
    DWORD       ticks;      

    struct
    {
        BYTE    hour;       
        BYTE    min;        
        BYTE    sec;        
        BYTE    frame;      
        BYTE    fps;        
        BYTE    dummy;      
        BYTE    pad[2];
    } smpte;
    
    struct
    {
        DWORD songptrpos;   
    } midi;
    } u;
} MMTIME, *PMMTIME, NEAR *NPMMTIME, *LPMMTIME;

typedef struct DRVCONFIGINFOEX {
    DWORD   dwDCISize;
    LPCWSTR  lpszDCISectionName;
    LPCWSTR  lpszDCIAliasName;
    DWORD    dnDevNode;
} DRVCONFIGINFOEX, *PDRVCONFIGINFOEX, NEAR *NPDRVCONFIGINFOEX, *LPDRVCONFIGINFOEX;
typedef struct tagDRVCONFIGINFO {
    DWORD   dwDCISize;
    LPCWSTR  lpszDCISectionName;
    LPCWSTR  lpszDCIAliasName;
} DRVCONFIGINFO, *PDRVCONFIGINFO, NEAR *NPDRVCONFIGINFO, *LPDRVCONFIGINFO;

DECLARE_HANDLE(HDRVR);
typedef void (CALLBACK *DRVCALLBACK)(HDRVR hdrvr, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
typedef DRVCALLBACK *LPDRVCALLBACK;
typedef DRVCALLBACK     *PDRVCALLBACK;

#define OutputDebugStr  OutputDebugString
DECLARE_HANDLE(HWAVE);
DECLARE_HANDLE(HWAVEIN);
DECLARE_HANDLE(HWAVEOUT);
typedef HWAVEIN *LPHWAVEIN;
typedef HWAVEOUT *LPHWAVEOUT;
typedef DRVCALLBACK WAVECALLBACK;
typedef WAVECALLBACK *LPWAVECALLBACK;
typedef struct wavehdr_tag {
    LPSTR       lpData;                 
    DWORD       dwBufferLength;         
    DWORD       dwBytesRecorded;        
    DWORD       dwUser;                 
    DWORD       dwFlags;                
    DWORD       dwLoops;                
    struct wavehdr_tag *lpNext;     
    DWORD       reserved;               
} WAVEHDR, *PWAVEHDR, NEAR *NPWAVEHDR, *LPWAVEHDR;

typedef struct tagWAVEOUTCAPS {
    WORD    wMid;                  
    WORD    wPid;                  
    MMVERSION vDriverVersion;      
    BCHAR    szPname[MAXPNAMELEN];  
    DWORD   dwFormats;             
    WORD    wChannels;             
    WORD    wReserved1;            
    DWORD   dwSupport;             
} WAVEOUTCAPS, *PWAVEOUTCAPS, *NPWAVEOUTCAPS, *LPWAVEOUTCAPS;

typedef struct tagWAVEINCAPS {
    WORD    wMid;                    
    WORD    wPid;                    
    MMVERSION vDriverVersion;        
    BCHAR    szPname[MAXPNAMELEN];    
    DWORD   dwFormats;               
    WORD    wChannels;               
    WORD    wReserved1;              
} WAVEINCAPSA, *PWAVEINCAPS, *NPWAVEINCAPS, *LPWAVEINCAPS;

#ifndef __WAVEFORMAT
#define __WAVEFORMAT
typedef struct waveformat_tag {
    WORD    wFormatTag;        
    WORD    nChannels;         
    DWORD   nSamplesPerSec;    
    DWORD   nAvgBytesPerSec;   
    WORD    nBlockAlign;       
} WAVEFORMAT, *PWAVEFORMAT, NEAR *NPWAVEFORMAT, *LPWAVEFORMAT;
typedef struct pcmwaveformat_tag {
    WAVEFORMAT  wf;
    WORD        wBitsPerSample;
} PCMWAVEFORMAT, *PPCMWAVEFORMAT, NEAR *NPPCMWAVEFORMAT, *LPPCMWAVEFORMAT;

typedef struct tWAVEFORMATEX
{
    WORD        wFormatTag;         
    WORD        nChannels;          
    DWORD       nSamplesPerSec;     
    DWORD       nAvgBytesPerSec;    
    WORD        nBlockAlign;        
    WORD        wBitsPerSample;     
    WORD        cbSize;             
                    
} WAVEFORMATEX, *PWAVEFORMATEX, NEAR *NPWAVEFORMATEX, *LPWAVEFORMATEX;
typedef const WAVEFORMATEX *LPCWAVEFORMATEX;
#endif

DECLARE_HANDLE(HMIDI);
DECLARE_HANDLE(HMIDIIN);
DECLARE_HANDLE(HMIDIOUT);
DECLARE_HANDLE(HMIDISTRM);
typedef HMIDI *LPHMIDI;
typedef HMIDIIN *LPHMIDIIN;
typedef HMIDIOUT *LPHMIDIOUT;
typedef HMIDISTRM *LPHMIDISTRM;
typedef DRVCALLBACK MIDICALLBACK;
typedef MIDICALLBACK *LPMIDICALLBACK;
typedef WORD PATCHARRAY[MIDIPATCHSIZE];
typedef WORD *LPPATCHARRAY;
typedef WORD KEYARRAY[MIDIPATCHSIZE];
typedef WORD *LPKEYARRAY;

typedef struct tagMIDIOUTCAPS {
    WORD    wMid;                  
    WORD    wPid;                  
    MMVERSION vDriverVersion;      
    BCHAR    szPname[MAXPNAMELEN];  
    WORD    wTechnology;           
    WORD    wVoices;               
    WORD    wNotes;                
    WORD    wChannelMask;          
    DWORD   dwSupport;             
} MIDIOUTCAPS, *PMIDIOUTCAPS, *NPMIDIOUTCAPS, *LPMIDIOUTCAPS;

typedef struct tagMIDIINCAPS {
    WORD        wMid;                   
    WORD        wPid;                   
    MMVERSION   vDriverVersion;         
    BCHAR        szPname[MAXPNAMELEN];   
    DWORD   dwSupport;             
} MIDIINCAPS, *PMIDIINCAPS, *NPMIDIINCAPS, *LPMIDIINCAPS;

typedef struct midihdr_tag {
    LPSTR       lpData;               
    DWORD       dwBufferLength;       
    DWORD       dwBytesRecorded;      
    DWORD       dwUser;               
    DWORD       dwFlags;              
    struct midihdr_tag *lpNext;   
    DWORD       reserved;             
    DWORD       dwOffset;             
    DWORD       dwReserved[8];        
} MIDIHDR, *PMIDIHDR, NEAR *NPMIDIHDR, *LPMIDIHDR;

typedef struct midievent_tag
{
    DWORD       dwDeltaTime;          
    DWORD       dwStreamID;           
    DWORD       dwEvent;              
    DWORD       dwParms[1];           
} MIDIEVENT;

typedef struct midistrmbuffver_tag
{
    DWORD       dwVersion;                  
    DWORD       dwMid;                      
    DWORD       dwOEMVersion;               
} MIDISTRMBUFFVER;

typedef struct midiproptimediv_tag
{
    DWORD       cbStruct;
    DWORD       dwTimeDiv;
} MIDIPROPTIMEDIV, *LPMIDIPROPTIMEDIV;

typedef struct midiproptempo_tag
{
    DWORD       cbStruct;
    DWORD       dwTempo;
} MIDIPROPTEMPO, *LPMIDIPROPTEMPO;

typedef struct tagAUXCAPS {
    WORD        wMid;                
    WORD        wPid;                
    MMVERSION   vDriverVersion;      
    BCHAR        szPname[MAXPNAMELEN];
    WORD        wTechnology;         
    WORD        wReserved1;          
    DWORD       dwSupport;           
} AUXCAPS, *PAUXCAPS, *NPAUXCAPS, *LPAUXCAPS;

DECLARE_HANDLE(HMIXEROBJ);
typedef HMIXEROBJ *LPHMIXEROBJ;

DECLARE_HANDLE(HMIXER);
typedef HMIXER     *LPHMIXER;

typedef struct tagMIXERCAPS {
    WORD            wMid;                   
    WORD            wPid;                   
    MMVERSION       vDriverVersion;         
    BCHAR            szPname[MAXPNAMELEN];   
    DWORD           fdwSupport;             
    DWORD           cDestinations;          
} MIXERCAPS, *PMIXERCAPS, *LPMIXERCAPS;

typedef struct tagMIXERLINE {
    DWORD       cbStruct;               
    DWORD       dwDestination;          
    DWORD       dwSource;               
    DWORD       dwLineID;               
    DWORD       fdwLine;                
    DWORD       dwUser;                 
    DWORD       dwComponentType;        
    DWORD       cChannels;              
    DWORD       cConnections;           
    DWORD       cControls;              
    BCHAR        szShortName[MIXER_SHORT_NAME_CHARS];
    BCHAR        szName[MIXER_LONG_NAME_CHARS];
    struct {
        DWORD   dwType;                 
        DWORD   dwDeviceID;             
        WORD    wMid;                   
        WORD    wPid;                   
        MMVERSION vDriverVersion;       
        BCHAR    szPname[MAXPNAMELEN];   
    } Target;
} MIXERLINE, *PMIXERLINE, *LPMIXERLINE;

typedef struct tagMIXERCONTROL {
    DWORD           cbStruct;           
    DWORD           dwControlID;        
    DWORD           dwControlType;      
    DWORD           fdwControl;         
    DWORD           cMultipleItems;     
    BCHAR            szShortName[MIXER_SHORT_NAME_CHARS];
    BCHAR            szName[MIXER_LONG_NAME_CHARS];
    union {
    struct {
        LONG    lMinimum;           
        LONG    lMaximum;           
    };
    struct {
        DWORD   dwMinimum;          
        DWORD   dwMaximum;          
    };
    DWORD       dwReserved[6];
    } Bounds;
    union {
        DWORD       cSteps;             
        DWORD       cbCustomData;       
        DWORD       dwReserved[6];      
    } Metrics;
} MIXERCONTROL, *PMIXERCONTROL, *LPMIXERCONTROL;

typedef struct tagMIXERLINECONTROLS {
    DWORD           cbStruct;       
    DWORD           dwLineID;       
    union {
        DWORD       dwControlID;    
        DWORD       dwControlType;  
    };
    DWORD           cControls;      
    DWORD           cbmxctrl;       
    LPMIXERCONTROL pamxctrl;       
} MIXERLINECONTROLS, *PMIXERLINECONTROLS, *LPMIXERLINECONTROLS;

typedef struct tMIXERCONTROLDETAILS {
    DWORD           cbStruct;       
    DWORD           dwControlID;    
    DWORD           cChannels;      
    union {
        HWND        hwndOwner;      
        DWORD       cMultipleItems; 
    };
    DWORD           cbDetails;      
    LPVOID          paDetails;      
} MIXERCONTROLDETAILS, *PMIXERCONTROLDETAILS, *LPMIXERCONTROLDETAILS;

typedef struct tagMIXERCONTROLDETAILS_LISTTEXT {
    DWORD           dwParam1;
    DWORD           dwParam2;
    BCHAR            szName[MIXER_LONG_NAME_CHARS];
} MIXERCONTROLDETAILS_LISTTEXT, *PMIXERCONTROLDETAILS_LISTTEXT, *LPMIXERCONTROLDETAILS_LISTTEXT;

typedef struct tMIXERCONTROLDETAILS_BOOLEAN {
    LONG            fValue;
}       MIXERCONTROLDETAILS_BOOLEAN,
      *PMIXERCONTROLDETAILS_BOOLEAN,
 *LPMIXERCONTROLDETAILS_BOOLEAN;

typedef struct tMIXERCONTROLDETAILS_SIGNED {
    LONG            lValue;
}       MIXERCONTROLDETAILS_SIGNED,
      *PMIXERCONTROLDETAILS_SIGNED,
 *LPMIXERCONTROLDETAILS_SIGNED;

typedef struct tMIXERCONTROLDETAILS_UNSIGNED {
    DWORD           dwValue;
}       MIXERCONTROLDETAILS_UNSIGNED,
      *PMIXERCONTROLDETAILS_UNSIGNED,
 *LPMIXERCONTROLDETAILS_UNSIGNED;


typedef void (CALLBACK *TIMECALLBACK)(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

typedef TIMECALLBACK *LPTIMECALLBACK;

typedef struct timecaps_tag {
    UINT    wPeriodMin;     
    UINT    wPeriodMax;     
} TIMECAPS, *PTIMECAPS, NEAR *NPTIMECAPS, *LPTIMECAPS;

typedef struct tagJOYCAPS {
    WORD    wMid;                
    WORD    wPid;                
    BCHAR    szPname[MAXPNAMELEN];
    UINT    wXmin;               
    UINT    wXmax;               
    UINT    wYmin;               
    UINT    wYmax;               
    UINT    wZmin;               
    UINT    wZmax;               
    UINT    wNumButtons;         
    UINT    wPeriodMin;          
    UINT    wPeriodMax;          
    UINT    wRmin;               
    UINT    wRmax;               
    UINT    wUmin;               
    UINT    wUmax;               
    UINT    wVmin;               
    UINT    wVmax;               
    UINT    wCaps;	 	 
    UINT    wMaxAxes;	 	 
    UINT    wNumAxes;	 	 
    UINT    wMaxButtons;	 
    BCHAR    szRegKey[MAXPNAMELEN];
    BCHAR    szOEMVxD[MAX_JOYSTICKOEMVXDNAME]; 
} JOYCAPS, *PJOYCAPS, *NPJOYCAPS, *LPJOYCAPS;

typedef struct joyinfo_tag {
    UINT wXpos;                 
    UINT wYpos;                 
    UINT wZpos;                 
    UINT wButtons;              
} JOYINFO, *PJOYINFO, NEAR *NPJOYINFO, *LPJOYINFO;

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
} JOYINFOEX, *PJOYINFOEX, NEAR *NPJOYINFOEX, *LPJOYINFOEX;


typedef DWORD           FOURCC;         
typedef char  *    HPSTR;          
DECLARE_HANDLE(HMMIO);                  
typedef LRESULT (CALLBACK *MMIOPROC)(LPSTR lpmmioinfo, UINT uMsg,
        LPARAM lParam1, LPARAM lParam2);
typedef MMIOPROC *LPMMIOPROC;

typedef struct _MMIOINFO
{
    
    DWORD           dwFlags;        
    FOURCC          fccIOProc;      
    LPMMIOPROC      pIOProc;        
    UINT            wErrorRet;      
    HTASK           htask;          

    
    LONG            cchBuffer;      
    HPSTR           pchBuffer;      
    HPSTR           pchNext;        
    HPSTR           pchEndRead;     
    HPSTR           pchEndWrite;    
    LONG            lBufOffset;     

    
    LONG            lDiskOffset;    
    DWORD           adwInfo[3];     

    
    DWORD           dwReserved1;    
    DWORD           dwReserved2;    
    HMMIO           hmmio;          
} MMIOINFO, *PMMIOINFO, NEAR *NPMMIOINFO, *LPMMIOINFO;
typedef const MMIOINFO *LPCMMIOINFO;

typedef struct _MMCKINFO
{
    FOURCC          ckid;           
    DWORD           cksize;         
    FOURCC          fccType;        
    DWORD           dwDataOffset;   
    DWORD           dwFlags;        
} MMCKINFO, *PMMCKINFO, NEAR *NPMMCKINFO, *LPMMCKINFO;
typedef const MMCKINFO *LPCMMCKINFO;

typedef DWORD   MCIERROR;       
typedef UINT    MCIDEVICEID;    
typedef UINT (CALLBACK *YIELDPROC)(MCIDEVICEID mciId, DWORD dwYieldData);

typedef struct tagMCI_GENERIC_PARMS {
    DWORD   dwCallback;
} MCI_GENERIC_PARMS, *PMCI_GENERIC_PARMS, *LPMCI_GENERIC_PARMS;

typedef struct tagMCI_OPEN_PARMS {
    DWORD   dwCallback;
    MCIDEVICEID wDeviceID;
    LPCTSTR     lpstrDeviceType;
    LPCTSTR     lpstrElementName;
    LPCTSTR     lpstrAlias;
} MCI_OPEN_PARMS, *PMCI_OPEN_PARMS, *LPMCI_OPEN_PARMS;

typedef struct tagMCI_PLAY_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
} MCI_PLAY_PARMS, *PMCI_PLAY_PARMS, *LPMCI_PLAY_PARMS;

typedef struct tagMCI_SEEK_PARMS {
    DWORD   dwCallback;
    DWORD   dwTo;
} MCI_SEEK_PARMS, *PMCI_SEEK_PARMS, *LPMCI_SEEK_PARMS;

typedef struct tagMCI_STATUS_PARMS {
    DWORD   dwCallback;
    DWORD   dwReturn;
    DWORD   dwItem;
    DWORD   dwTrack;
} MCI_STATUS_PARMS, *PMCI_STATUS_PARMS, * LPMCI_STATUS_PARMS;

typedef struct tagMCI_INFO_PARMS {
    DWORD   dwCallback;
    LPTSTR   lpstrReturn;
    DWORD   dwRetSize;
} MCI_INFO_PARMS, * LPMCI_INFO_PARMS;

typedef struct tagMCI_GETDEVCAPS_PARMS {
    DWORD   dwCallback;
    DWORD   dwReturn;
    DWORD   dwItem;
} MCI_GETDEVCAPS_PARMS, *PMCI_GETDEVCAPS_PARMS, * LPMCI_GETDEVCAPS_PARMS;

typedef struct tagMCI_SYSINFO_PARMS {
    DWORD   dwCallback;
    LPTSTR   lpstrReturn;
    DWORD   dwRetSize;
    DWORD   dwNumber;
    UINT    wDeviceType;
} MCI_SYSINFO_PARMS, *PMCI_SYSINFO_PARMS, * LPMCI_SYSINFO_PARMS;

typedef struct tagMCI_SET_PARMS {
    DWORD   dwCallback;
    DWORD   dwTimeFormat;
    DWORD   dwAudio;
} MCI_SET_PARMS, *PMCI_SET_PARMS, *LPMCI_SET_PARMS;

typedef struct tagMCI_BREAK_PARMS {
    DWORD   dwCallback;
    int     nVirtKey;
    HWND    hwndBreak;
} MCI_BREAK_PARMS, *PMCI_BREAK_PARMS, * LPMCI_BREAK_PARMS;

typedef struct tagMCI_SAVE_PARMS {
    DWORD    dwCallback;
    LPCTSTR    lpfilename;
} MCI_SAVE_PARMS, *PMCI_SAVE_PARMS, * LPMCI_SAVE_PARMS;

typedef struct tagMCI_LOAD_PARMS {
    DWORD    dwCallback;
    LPCTSTR      lpfilename;
} MCI_LOAD_PARMS, *PMCI_LOAD_PARMS, * LPMCI_LOAD_PARMS;

typedef struct tagMCI_RECORD_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
} MCI_RECORD_PARMS, *LPMCI_RECORD_PARMS;

typedef struct tagMCI_VD_PLAY_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
    DWORD   dwSpeed;
} MCI_VD_PLAY_PARMS, *PMCI_VD_PLAY_PARMS, *LPMCI_VD_PLAY_PARMS;

typedef struct tagMCI_VD_STEP_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrames;
} MCI_VD_STEP_PARMS, *PMCI_VD_STEP_PARMS, *LPMCI_VD_STEP_PARMS;

typedef struct tagMCI_VD_ESCAPE_PARMS {
    DWORD   dwCallback;
    LPCTSTR      lpstrCommand;
} MCI_VD_ESCAPE_PARMSA, *PMCI_VD_ESCAPE_PARMSA, *LPMCI_VD_ESCAPE_PARMSA;

typedef struct tagMCI_WAVE_OPEN_PARMS {
    DWORD   dwCallback;
    MCIDEVICEID wDeviceID;
    LPCTSTR      lpstrDeviceType;
    LPCTSTR      lpstrElementName;
    LPCTSTR      lpstrAlias;
    DWORD   dwBufferSeconds;
} MCI_WAVE_OPEN_PARMS, *PMCI_WAVE_OPEN_PARMS, *LPMCI_WAVE_OPEN_PARMS;

typedef struct tagMCI_WAVE_DELETE_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
} MCI_WAVE_DELETE_PARMS, *PMCI_WAVE_DELETE_PARMS, *LPMCI_WAVE_DELETE_PARMS;

typedef struct tagMCI_WAVE_SET_PARMS {
    DWORD   dwCallback;
    DWORD   dwTimeFormat;
    DWORD   dwAudio;
    UINT    wInput;
    UINT    wOutput;
    WORD    wFormatTag;
    WORD    wReserved2;
    WORD    nChannels;
    WORD    wReserved3;
    DWORD   nSamplesPerSec;
    DWORD   nAvgBytesPerSec;
    WORD    nBlockAlign;
    WORD    wReserved4;
    WORD    wBitsPerSample;
    WORD    wReserved5;
} MCI_WAVE_SET_PARMS, *PMCI_WAVE_SET_PARMS, * LPMCI_WAVE_SET_PARMS;

typedef struct tagMCI_SEQ_SET_PARMS {
    DWORD   dwCallback;
    DWORD   dwTimeFormat;
    DWORD   dwAudio;
    DWORD   dwTempo;
    DWORD   dwPort;
    DWORD   dwSlave;
    DWORD   dwMaster;
    DWORD   dwOffset;
} MCI_SEQ_SET_PARMS, *PMCI_SEQ_SET_PARMS, * LPMCI_SEQ_SET_PARMS;

typedef struct tagMCI_ANIM_OPEN_PARMS {
    DWORD   dwCallback;
    MCIDEVICEID wDeviceID;
    LPCTSTR      lpstrDeviceType;
    LPCTSTR      lpstrElementName;
    LPCTSTR      lpstrAlias;
    DWORD   dwStyle;
    HWND    hWndParent;
} MCI_ANIM_OPEN_PARMS, *PMCI_ANIM_OPEN_PARMS, *LPMCI_ANIM_OPEN_PARMS;

typedef struct tagMCI_ANIM_PLAY_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrom;
    DWORD   dwTo;
    DWORD   dwSpeed;
} MCI_ANIM_PLAY_PARMS, *PMCI_ANIM_PLAY_PARMS, *LPMCI_ANIM_PLAY_PARMS;

typedef struct tagMCI_ANIM_STEP_PARMS {
    DWORD   dwCallback;
    DWORD   dwFrames;
} MCI_ANIM_STEP_PARMS, *PMCI_ANIM_STEP_PARMS, *LPMCI_ANIM_STEP_PARMS;

typedef struct tagMCI_ANIM_WINDOW_PARMS {
    DWORD   dwCallback;
    HWND    hWnd;
    UINT    nCmdShow;
    LPCTSTR     lpstrText;
} MCI_ANIM_WINDOW_PARMS, *PMCI_ANIM_WINDOW_PARMS, * LPMCI_ANIM_WINDOW_PARMS;

typedef struct tagMCI_ANIM_RECT_PARMS {
    DWORD   dwCallback;
#ifdef MCI_USE_OFFEXT
    POINT   ptOffset;
    POINT   ptExtent;
#else   
    RECT    rc;
#endif  
} MCI_ANIM_RECT_PARMS;
typedef MCI_ANIM_RECT_PARMS * PMCI_ANIM_RECT_PARMS;
typedef MCI_ANIM_RECT_PARMS * LPMCI_ANIM_RECT_PARMS;

typedef struct tagMCI_ANIM_UPDATE_PARMS {
    DWORD   dwCallback;
    RECT    rc;
    HDC     hDC;
} MCI_ANIM_UPDATE_PARMS, *PMCI_ANIM_UPDATE_PARMS, * LPMCI_ANIM_UPDATE_PARMS;

typedef struct tagMCI_OVLY_OPEN_PARMS {
    DWORD   dwCallback;
    MCIDEVICEID wDeviceID;
    LPCTSTR      lpstrDeviceType;
    LPCTSTR      lpstrElementName;
    LPCTSTR      lpstrAlias;
    DWORD   dwStyle;
    HWND    hWndParent;
} MCI_OVLY_OPEN_PARMS, *PMCI_OVLY_OPEN_PARMS, *LPMCI_OVLY_OPEN_PARMS;

typedef struct tagMCI_OVLY_WINDOW_PARMS {
    DWORD   dwCallback;
    HWND    hWnd;
    UINT    nCmdShow;
    LPCTSTR      lpstrText;
} MCI_OVLY_WINDOW_PARMS, *PMCI_OVLY_WINDOW_PARMS, * LPMCI_OVLY_WINDOW_PARMS;

typedef struct tagMCI_OVLY_RECT_PARMS {
    DWORD   dwCallback;
#ifdef MCI_USE_OFFEXT
    POINT   ptOffset;
    POINT   ptExtent;
#else   
    RECT    rc;
#endif  
} MCI_OVLY_RECT_PARMS, *PMCI_OVLY_RECT_PARMS, * LPMCI_OVLY_RECT_PARMS;

typedef struct tagMCI_OVLY_SAVE_PARMS {
    DWORD   dwCallback;
    LPCTSTR      lpfilename;
    RECT    rc;
} MCI_OVLY_SAVE_PARMS, *PMCI_OVLY_SAVE_PARMS, * LPMCI_OVLY_SAVE_PARMS;

typedef struct tagMCI_OVLY_LOAD_PARMS {
    DWORD   dwCallback;
    LPCTSTR      lpfilename;
    RECT    rc;
} MCI_OVLY_LOAD_PARMS, *PMCI_OVLY_LOAD_PARMS, * LPMCI_OVLY_LOAD_PARMS;

#pragma pack()

