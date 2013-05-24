/* 
   mmsfunc.h

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
#ifdef __cplusplus
extern "C" {
#endif

UINT PASCAL WINBASEAPI mmsystemGetVersion(void);
LRESULT   PASCAL WINBASEAPI CloseDriver(HDRVR hDriver, LONG lParam1, LONG lParam2);
HDRVR     PASCAL WINBASEAPI OpenDriver(LPCWSTR szDriverName, LPCWSTR szSectionName, LONG lParam2);
LRESULT   PASCAL WINBASEAPI SendDriverMessage(HDRVR hDriver, UINT message, LONG lParam1, LONG lParam2);
HMODULE   PASCAL WINBASEAPI DrvGetModuleHandle(HDRVR hDriver);
HMODULE   PASCAL WINBASEAPI GetDriverModuleHandle(HDRVR hDriver);
LRESULT   PASCAL WINBASEAPI DefDriverProc(DWORD dwDriverIdentifier, HDRVR hdrvr, UINT uMsg, LPARAM lParam1, LPARAM lParam2);
UINT PASCAL WINBASEAPI waveOutGetNumDevs(void);
UINT PASCAL WINBASEAPI waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume);
UINT PASCAL WINBASEAPI waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume);
UINT PASCAL WINBASEAPI waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
UINT PASCAL WINBASEAPI waveOutClose(HWAVEOUT hwo);
UINT PASCAL WINBASEAPI waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
UINT PASCAL WINBASEAPI waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
UINT PASCAL WINBASEAPI waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
UINT PASCAL WINBASEAPI waveOutPause(HWAVEOUT hwo);
UINT PASCAL WINBASEAPI waveOutRestart(HWAVEOUT hwo);
UINT PASCAL WINBASEAPI waveOutReset(HWAVEOUT hwo);
UINT PASCAL WINBASEAPI waveOutBreakLoop(HWAVEOUT hwo);
UINT PASCAL WINBASEAPI waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt);
UINT PASCAL WINBASEAPI waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch);
UINT PASCAL WINBASEAPI waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch);
UINT PASCAL WINBASEAPI waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate);
UINT PASCAL WINBASEAPI waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate);
UINT PASCAL WINBASEAPI waveOutGetID(HWAVEOUT hwo, LPUINT puDeviceID);
UINT PASCAL WINBASEAPI waveOutMessage(HWAVEOUT hwo, UINT uMsg, DWORD dw1, DWORD dw2);
UINT PASCAL WINBASEAPI waveInGetNumDevs(void);
UINT PASCAL WINBASEAPI waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
UINT PASCAL WINBASEAPI waveInClose(HWAVEIN hwi);
UINT PASCAL WINBASEAPI waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
UINT PASCAL WINBASEAPI waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
UINT PASCAL WINBASEAPI waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
UINT PASCAL WINBASEAPI waveInStart(HWAVEIN hwi);
UINT PASCAL WINBASEAPI waveInStop(HWAVEIN hwi);
UINT PASCAL WINBASEAPI waveInReset(HWAVEIN hwi);
UINT PASCAL WINBASEAPI waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt);
UINT PASCAL WINBASEAPI waveInGetID(HWAVEIN hwi, LPUINT puDeviceID);
UINT PASCAL WINBASEAPI waveInMessage(HWAVEIN hwi, UINT uMsg, DWORD dw1, DWORD dw2);
UINT PASCAL WINBASEAPI midiOutGetNumDevs(void);
UINT PASCAL WINBASEAPI midiStreamOpen(LPHMIDISTRM phms, LPUINT puDeviceID, DWORD cMidi, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
UINT PASCAL WINBASEAPI midiStreamClose(HMIDISTRM hms);
UINT PASCAL WINBASEAPI midiStreamProperty(HMIDISTRM hms, LPBYTE lppropdata, DWORD dwProperty);
UINT PASCAL WINBASEAPI midiStreamPosition(HMIDISTRM hms, LPMMTIME lpmmt, UINT cbmmt);
UINT PASCAL WINBASEAPI midiStreamOut(HMIDISTRM hms, LPMIDIHDR pmh, UINT cbmh);
UINT PASCAL WINBASEAPI midiStreamPause(HMIDISTRM hms);
UINT PASCAL WINBASEAPI midiStreamRestart(HMIDISTRM hms);
UINT PASCAL WINBASEAPI midiStreamStop(HMIDISTRM hms);
UINT PASCAL WINBASEAPI midiConnect(HMIDI hmi, HMIDIOUT hmo, LPVOID pReserved);
UINT PASCAL WINBASEAPI midiDisconnect(HMIDI hmi, HMIDIOUT hmo, LPVOID pReserved);
UINT PASCAL WINBASEAPI midiOutGetVolume(HMIDIOUT hmo, LPDWORD pdwVolume);
UINT PASCAL WINBASEAPI midiOutSetVolume(HMIDIOUT hmo, DWORD dwVolume);
UINT PASCAL WINBASEAPI midiOutOpen(LPHMIDIOUT phmo, UINT uDeviceID,
    DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
UINT PASCAL WINBASEAPI midiOutClose(HMIDIOUT hmo);
UINT PASCAL WINBASEAPI midiOutPrepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh);
UINT PASCAL WINBASEAPI midiOutUnprepareHeader(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh);
UINT PASCAL WINBASEAPI midiOutShortMsg(HMIDIOUT hmo, DWORD dwMsg);
UINT PASCAL WINBASEAPI midiOutLongMsg(HMIDIOUT hmo, LPMIDIHDR pmh, UINT cbmh);
UINT PASCAL WINBASEAPI midiOutReset(HMIDIOUT hmo);
UINT PASCAL WINBASEAPI midiOutCachePatches(HMIDIOUT hmo, UINT uBank, LPWORD pwpa, UINT fuCache);
UINT PASCAL WINBASEAPI midiOutCacheDrumPatches(HMIDIOUT hmo, UINT uPatch, LPWORD pwkya, UINT fuCache);
UINT PASCAL WINBASEAPI midiOutGetID(HMIDIOUT hmo, LPUINT puDeviceID);
UINT PASCAL WINBASEAPI midiOutMessage(HMIDIOUT hmo, UINT uMsg, DWORD dw1, DWORD dw2);
UINT PASCAL WINBASEAPI midiInGetNumDevs(void);
UINT PASCAL WINBASEAPI midiInOpen(LPHMIDIIN phmi, UINT uDeviceID,
    DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
UINT PASCAL WINBASEAPI midiInClose(HMIDIIN hmi);
UINT PASCAL WINBASEAPI midiInPrepareHeader(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh);
UINT PASCAL WINBASEAPI midiInUnprepareHeader(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh);
UINT PASCAL WINBASEAPI midiInAddBuffer(HMIDIIN hmi, LPMIDIHDR pmh, UINT cbmh);
UINT PASCAL WINBASEAPI midiInStart(HMIDIIN hmi);
UINT PASCAL WINBASEAPI midiInStop(HMIDIIN hmi);
UINT PASCAL WINBASEAPI midiInReset(HMIDIIN hmi);
UINT PASCAL WINBASEAPI midiInGetID(HMIDIIN hmi, LPUINT puDeviceID);
UINT PASCAL WINBASEAPI midiInMessage(HMIDIIN hmi, UINT uMsg, DWORD dw1, DWORD dw2);
UINT PASCAL WINBASEAPI auxGetNumDevs(void);
UINT PASCAL WINBASEAPI auxSetVolume(UINT uDeviceID, DWORD dwVolume);
UINT PASCAL WINBASEAPI auxGetVolume(UINT uDeviceID, LPDWORD pdwVolume);
UINT PASCAL WINBASEAPI auxOutMessage(UINT uDeviceID, UINT uMsg, DWORD dw1, DWORD dw2);
UINT PASCAL WINBASEAPI mixerOpen(LPHMIXER phmx, UINT uMxId, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
UINT PASCAL WINBASEAPI mixerClose(HMIXER hmx);
DWORD PASCAL WINBASEAPI mixerMessage(HMIXER hmx, UINT uMsg, DWORD dwParam1, DWORD dwParam2);
UINT PASCAL WINBASEAPI mixerGetID(HMIXEROBJ hmxobj, UINT FAR *puMxId, DWORD fdwId);
UINT PASCAL WINBASEAPI mixerSetControlDetails(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);
UINT PASCAL WINBASEAPI timeGetSystemTime(LPMMTIME pmmt, UINT cbmmt);
DWORD PASCAL WINBASEAPI timeGetTime(void);
UINT PASCAL WINBASEAPI timeSetEvent(UINT uDelay, UINT uResolution,
    LPTIMECALLBACK fptc, DWORD dwUser, UINT fuEvent);
UINT PASCAL WINBASEAPI timeKillEvent(UINT uTimerID);
UINT PASCAL WINBASEAPI timeGetDevCaps(LPTIMECAPS ptc, UINT cbtc);
UINT PASCAL WINBASEAPI timeBeginPeriod(UINT uPeriod);
UINT PASCAL WINBASEAPI timeEndPeriod(UINT uPeriod);
UINT PASCAL WINBASEAPI joyGetNumDevs(void);
UINT PASCAL WINBASEAPI joyGetPosEx(UINT uJoyID, LPJOYINFOEX pji);
UINT PASCAL WINBASEAPI joyGetThreshold(UINT uJoyID, LPUINT puThreshold);
UINT PASCAL WINBASEAPI joyReleaseCapture(UINT uJoyID);
UINT PASCAL WINBASEAPI joySetCapture(HWND hwnd, UINT uJoyID, UINT uPeriod,
    WINBOOL fChanged);
UINT PASCAL WINBASEAPI joySetThreshold(UINT uJoyID, UINT uThreshold);
UINT PASCAL WINBASEAPI mmioClose(HMMIO hmmio, UINT fuClose);
LONG PASCAL WINBASEAPI mmioRead(HMMIO hmmio, HPSTR pch, LONG cch);
LONG PASCAL WINBASEAPI mmioWrite(HMMIO hmmio, const char * pch, LONG cch);
LONG PASCAL WINBASEAPI mmioSeek(HMMIO hmmio, LONG lOffset, int iOrigin);
UINT PASCAL WINBASEAPI mmioGetInfo(HMMIO hmmio, LPMMIOINFO pmmioinfo, UINT fuInfo);
UINT PASCAL WINBASEAPI mmioSetInfo(HMMIO hmmio, LPCMMIOINFO pmmioinfo, UINT fuInfo);
UINT PASCAL WINBASEAPI mmioSetBuffer(HMMIO hmmio, LPSTR pchBuffer, LONG cchBuffer,
    UINT fuBuffer);
UINT PASCAL WINBASEAPI mmioFlush(HMMIO hmmio, UINT fuFlush);
UINT PASCAL WINBASEAPI mmioAdvance(HMMIO hmmio, LPMMIOINFO pmmioinfo, UINT fuAdvance);
LRESULT PASCAL WINBASEAPI mmioSendMessage(HMMIO hmmio, UINT uMsg,
    LPARAM lParam1, LPARAM lParam2);
UINT PASCAL WINBASEAPI mmioDescend(HMMIO hmmio, LPMMCKINFO pmmcki,
    const MMCKINFO FAR* pmmckiParent, UINT fuDescend);
UINT PASCAL WINBASEAPI mmioAscend(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuAscend);
UINT PASCAL WINBASEAPI mmioCreateChunk(HMMIO hmmio, LPMMCKINFO pmmcki, UINT fuCreate);
WINBOOL PASCAL WINBASEAPI mciSetYieldProc(MCIDEVICEID mciId, YIELDPROC fpYieldProc,
    DWORD dwYieldData);
HTASK PASCAL WINBASEAPI mciGetCreatorTask(MCIDEVICEID mciId);
YIELDPROC PASCAL WINBASEAPI mciGetYieldProc(MCIDEVICEID mciId, LPDWORD pdwYieldData);
WINBOOL PASCAL WINBASEAPI mciExecute(LPCSTR pszCommand);

#ifdef UNICODE
WINBOOL PASCAL WINBASEAPI sndPlaySoundW(LPCWSTR pszSound, UINT fuSound);
WINBOOL PASCAL WINBASEAPI PlaySoundW(LPCWSTR pszSound, HMODULE hmod, DWORD fdwSound);
UINT PASCAL WINBASEAPI waveOutGetDevCapsW(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc);
UINT PASCAL WINBASEAPI waveOutGetErrorTextW(UINT mmrError, LPWSTR pszText, UINT cchText);
UINT PASCAL WINBASEAPI waveInGetDevCapsW(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic);
UINT PASCAL WINBASEAPI waveInGetErrorTextW(UINT mmrError, LPWSTR pszText, UINT cchText);
UINT PASCAL WINBASEAPI midiOutGetDevCapsW(UINT uDeviceID, LPMIDIOUTCAPS pmoc, UINT cbmoc);
UINT PASCAL WINBASEAPI midiOutGetErrorTextW(UINT mmrError, LPWSTR pszText, UINT cchText);
UINT PASCAL WINBASEAPI midiInGetDevCapsW(UINT uDeviceID, LPMIDIINCAPS pmic, UINT cbmic);
UINT PASCAL WINBASEAPI midiInGetErrorTextW(UINT mmrError, LPWSTR pszText, UINT cchText);
UINT PASCAL WINBASEAPI auxGetDevCapsW(UINT uDeviceID, LPAUXCAPS pac, UINT cbac);
UINT PASCAL WINBASEAPI mixerGetDevCapsW(UINT uMxId, LPMIXERCAPS pmxcaps, UINT cbmxcaps);
UINT PASCAL WINBASEAPI mixerGetLineInfoW(HMIXEROBJ hmxobj, LPMIXERLINE pmxl, DWORD fdwInfo);
UINT PASCAL WINBASEAPI mixerGetLineControlsW(HMIXEROBJ hmxobj, LPMIXERLINECONTROLS pmxlc, DWORD fdwControls);
UINT PASCAL WINBASEAPI mixerGetControlDetailsW(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);
UINT PASCAL WINBASEAPI joyGetDevCapsW(UINT uJoyID, LPJOYCAPS pjc, UINT cbjc);
FOURCC PASCAL WINBASEAPI mmioStringToFOURCCW(LPCWSTR sz, UINT uFlags);
LPMMIOPROC PASCAL WINBASEAPI mmioInstallIOProcW(FOURCC fccIOProc, LPMMIOPROC pIOProc, DWORD dwFlags);
HMMIO PASCAL WINBASEAPI mmioOpenW(LPWSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen);
UINT PASCAL WINBASEAPI mmioRenameW(LPCWSTR pszFileName, LPCWSTR pszNewFileName, LPCMMIOINFO pmmioinfo, DWORD fdwRename);
MCIERROR PASCAL WINBASEAPI mciSendCommandW(MCIDEVICEID mciId, UINT uMsg, DWORD dwParam1, DWORD dwParam2);
MCIERROR  PASCAL WINBASEAPI mciSendStringW(LPCWSTR lpstrCommand, LPWSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback);
MCIDEVICEID PASCAL WINBASEAPI mciGetDeviceIDW(LPCWSTR pszDevice);
MCIDEVICEID PASCAL WINBASEAPI mciGetDeviceIDFromElementIDW(DWORD dwElementID, LPCWSTR lpstrType );
WINBOOL PASCAL WINBASEAPI mciGetErrorStringW(MCIERROR mcierr, LPWSTR pszText, UINT cchText);
#define sndPlaySound  sndPlaySoundW
#define PlaySound  PlaySoundW
#define waveOutGetDevCaps  waveOutGetDevCapsW
#define waveOutGetErrorText  waveOutGetErrorTextW
#define waveInGetDevCaps  waveInGetDevCapsW
#define waveInGetErrorText  waveInGetErrorTextW
#define midiOutGetDevCaps  midiOutGetDevCapsW
#define midiOutGetErrorText  midiOutGetErrorTextW
#define midiInGetDevCaps  midiInGetDevCapsW
#define midiInGetErrorText  midiInGetErrorTextW
#define auxGetDevCaps  auxGetDevCapsW
#define mixerGetDevCaps  mixerGetDevCapsW
#define mixerGetLineInfo  mixerGetLineInfoW
#define mixerGetControlDetails  mixerGetControlDetailsW
#define joyGetDevCaps  joyGetDevCapsW
#define mmioStringToFOURCC  mmioStringToFOURCCW
#define mmioInstallIOProc  mmioInstallIOProcW
#define mmioOpen  mmioOpenW
#define mmioRename  mmioRenameW
#define mciSendCommand  mciSendCommandW
#define mciSendString  mciSendStringW
#define mciGetDeviceID  mciGetDeviceIDW
#define mciGetDeviceIDFromElementID  mciGetDeviceIDFromElementIDW
#define mciGetErrorString  mciGetErrorStringW

#else
WINBOOL PASCAL WINBASEAPI sndPlaySoundA(LPCSTR pszSound, UINT fuSound);
WINBOOL PASCAL WINBASEAPI PlaySoundA(LPCSTR pszSound, HMODULE hmod, DWORD fdwSound);
UINT PASCAL WINBASEAPI waveOutGetDevCapsA(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc);
UINT PASCAL WINBASEAPI waveOutGetErrorTextA(UINT mmrError, LPSTR pszText, UINT cchText);
UINT PASCAL WINBASEAPI waveInGetDevCapsA(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic);
UINT PASCAL WINBASEAPI waveInGetErrorTextA(UINT mmrError, LPSTR pszText, UINT cchText);
UINT PASCAL WINBASEAPI midiOutGetDevCapsA(UINT uDeviceID, LPMIDIOUTCAPS pmoc, UINT cbmoc);
UINT PASCAL WINBASEAPI midiOutGetErrorTextA(UINT mmrError, LPSTR pszText, UINT cchText);
UINT PASCAL WINBASEAPI midiInGetDevCapsA(UINT uDeviceID, LPMIDIINCAPS pmic, UINT cbmic);
UINT PASCAL WINBASEAPI midiInGetErrorTextA(UINT mmrError, LPSTR pszText, UINT cchText);
UINT PASCAL WINBASEAPI auxGetDevCapsA(UINT uDeviceID, LPAUXCAPS pac, UINT cbac);
UINT PASCAL WINBASEAPI mixerGetDevCapsA(UINT uMxId, LPMIXERCAPS pmxcaps, UINT cbmxcaps);
UINT PASCAL WINBASEAPI mixerGetLineInfoA(HMIXEROBJ hmxobj, LPMIXERLINE pmxl, DWORD fdwInfo);
UINT PASCAL WINBASEAPI mixerGetLineControlsA(HMIXEROBJ hmxobj, LPMIXERLINECONTROLS pmxlc, DWORD fdwControls);
UINT PASCAL WINBASEAPI mixerGetControlDetailsA(HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails);
UINT PASCAL WINBASEAPI joyGetDevCapsA(UINT uJoyID, LPJOYCAPS pjc, UINT cbjc);
FOURCC PASCAL WINBASEAPI mmioStringToFOURCCA(LPCSTR sz, UINT uFlags);
LPMMIOPROC PASCAL WINBASEAPI mmioInstallIOProcA(FOURCC fccIOProc, LPMMIOPROC pIOProc, DWORD dwFlags);
HMMIO PASCAL WINBASEAPI mmioOpenA(LPSTR pszFileName, LPMMIOINFO pmmioinfo, DWORD fdwOpen);
UINT PASCAL WINBASEAPI mmioRenameA(LPCSTR pszFileName, LPCSTR pszNewFileName, LPCMMIOINFO pmmioinfo, DWORD fdwRename);
MCIERROR PASCAL WINBASEAPI mciSendCommandA(MCIDEVICEID mciId, UINT uMsg, DWORD dwParam1, DWORD dwParam2);
MCIERROR  PASCAL WINBASEAPI mciSendStringA(LPCSTR lpstrCommand, LPSTR lpstrReturnString, UINT uReturnLength, HWND hwndCallback);
MCIDEVICEID PASCAL WINBASEAPI mciGetDeviceIDA(LPCSTR pszDevice);
MCIDEVICEID PASCAL WINBASEAPI mciGetDeviceIDFromElementIDA(DWORD dwElementID, LPCSTR lpstrType );
WINBOOL PASCAL WINBASEAPI mciGetErrorStringA(MCIERROR mcierr, LPSTR pszText, UINT cchText);
#define sndPlaySound  sndPlaySoundA
#define PlaySound  PlaySoundA
#define waveOutGetDevCaps  waveOutGetDevCapsA
#define waveOutGetErrorText  waveOutGetErrorTextA
#define waveInGetDevCaps  waveInGetDevCapsA
#define waveInGetErrorText  waveInGetErrorTextA
#define midiOutGetDevCaps  midiOutGetDevCapsA
#define midiOutGetErrorText  midiOutGetErrorTextA
#define midiInGetDevCaps  midiInGetDevCapsA
#define midiInGetErrorText  midiInGetErrorTextA
#define auxGetDevCaps  auxGetDevCapsA
#define mixerGetDevCaps  mixerGetDevCapsA
#define mixerGetLineInfo  mixerGetLineInfoA
#define mixerGetControlDetails  mixerGetControlDetailsA
#define joyGetDevCaps  joyGetDevCapsA
#define mmioStringToFOURCC  mmioStringToFOURCCA
#define mmioInstallIOProc  mmioInstallIOProcA
#define mmioOpen  mmioOpenA
#define mmioRename  mmioRenameA
#define mciSendCommand  mciSendCommandA
#define mciSendString  mciSendStringA
#define mciGetDeviceID  mciGetDeviceIDA
#define mciGetDeviceIDFromElementID  mciGetDeviceIDFromElementIDA
#define mciGetErrorString  mciGetErrorStringA

#endif
#ifdef __cplusplus
} ;
#endif

