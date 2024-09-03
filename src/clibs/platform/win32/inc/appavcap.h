#ifndef _APPAVCAP_H
#define _APPAVCAP_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* Network audio/video application capability interface definitions */

#include <mmreg.h>
#include <msacm.h>

#include <pshpack8.h>

#define INVALID_AUDIO_FORMAT  0xffffffff
#define INVALID_MEDIA_FORMAT  0xffffffff
#define INVALID_VIDEO_FORMAT  0xffffffff

#define VARIABLE_DIM  1

#define LNH_48_CPU  97
#define LNH_8_CPU  47
#define LNH_12_CPU  48
#define LNH_16_CPU  49
#define MS_G723_CPU  70
#define CCITT_A_CPU  24
#define CCITT_U_CPU  25

typedef DWORD AUDIO_FORMAT_ID;
typedef DWORD MEDIA_FORMAT_ID;
typedef DWORD VIDEO_FORMAT_ID;

typedef struct BasicAudCapInfo {
    WORD wFormatTag;
    AUDIO_FORMAT_ID Id;
    char szFormat[ACMFORMATDETAILS_FORMAT_CHARS];
    UINT uMaxBitrate;
    UINT uAvgBitrate;
    WORD wCPUUtilizationEncode;
    WORD wCPUUtilizationDecode;
    BOOL bSendEnabled;
    BOOL bRecvEnabled;
    WORD wSortIndex;
} BASIC_AUDCAP_INFO, *PBASIC_AUDCAP_INFO, AUDCAP_INFO, *PAUDCAP_INFO;

typedef struct _audcapinfolist {
    ULONG cFormats;
    AUDCAP_INFO aFormats[VARIABLE_DIM];
} AUDCAP_INFO_LIST, *PAUDCAP_INFO_LIST;

typedef enum {
    Small = 0,
    Medium,
    Large
} VIDEO_SIZES;

typedef struct BasicVidCapInfo {
    DWORD dwFormatTag;
    VIDEO_FORMAT_ID Id;
    char szFormat[ACMFORMATDETAILS_FORMAT_CHARS];
    WORD wCPUUtilizationEncode;
    WORD wCPUUtilizationDecode;
    BOOL bSendEnabled;
    BOOL bRecvEnabled;
    WORD wSortIndex;
    VIDEO_SIZES enumVideoSize;
    BITMAPINFOHEADER bih;
    UINT uFrameRate;
    DWORD dwBitsPerSample;
    UINT uAvgBitrate;
    UINT uMaxBitrate;
} BASIC_VIDCAP_INFO, *PBASIC_VIDCAP_INFO, VIDCAP_INFO, *PVIDCAP_INFO;

typedef struct _vidcapinfolist {
    ULONG cFormats;
    VIDCAP_INFO aFormats[VARIABLE_DIM];
} VIDCAP_INFO_LIST, *PVIDCAP_INFO_LIST;

#include <poppack.h>

#endif /* _APPAVCAP_H */

