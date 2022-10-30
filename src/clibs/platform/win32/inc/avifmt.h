#ifndef _AVIFMT_H
#define _AVIFMT_H

#ifdef __ORANGEC__ 
#pragma once
#endif

/* Windows AVI file definitions */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef mmioFOURCC
#define mmioFOURCC(ch0,ch1,ch2,ch3)  ((DWORD)(BYTE)(ch0)|((DWORD)(BYTE)(ch1)<<8)|((DWORD)(BYTE)(ch2)<<16)|((DWORD)(BYTE)(ch3)<<24))
#endif

#ifndef aviTWOCC
#define aviTWOCC(ch0,ch1)  ((WORD)(BYTE)(ch0)|((WORD)(BYTE)(ch1)<<8))
#endif

#ifndef _MMSYSTEM_H
typedef DWORD FOURCC;
#endif

typedef WORD TWOCC;

#define formtypeAVI  mmioFOURCC('A','V','I',' ')
#define listtypeAVIHEADER  mmioFOURCC('h','d','r','l')
#define ckidAVIMAINHDR  mmioFOURCC('a','v','i','h')
#define listtypeSTREAMHEADER  mmioFOURCC('s','t','r','l')
#define ckidSTREAMHEADER  mmioFOURCC('s','t','r','h')
#define ckidSTREAMFORMAT  mmioFOURCC('s','t','r','f')
#define ckidSTREAMHANDLERDATA  mmioFOURCC('s','t','r','d')
#define ckidSTREAMNAME  mmioFOURCC('s','t','r','n')
#define listtypeAVIMOVIE  mmioFOURCC('m','o','v','i')
#define listtypeAVIRECORD  mmioFOURCC('r','e','c',' ')
#define ckidAVINEWINDEX  mmioFOURCC('i','d','x','1')

#define streamtypeVIDEO  mmioFOURCC('v','i','d','s')
#define streamtypeAUDIO  mmioFOURCC('a','u','d','s')
#define streamtypeMIDI  mmioFOURCC('m','i','d','s')
#define streamtypeTEXT  mmioFOURCC('t','x','t','s')

#define cktypeDIBbits  aviTWOCC('d','b')
#define cktypeDIBcompressed  aviTWOCC('d','c')
#define cktypePALchange  aviTWOCC('p','c')
#define cktypeWAVEbytes  aviTWOCC('w','b')

#define ckidAVIPADDING  mmioFOURCC('J','U','N','K')

#define FromHex(n)  (((n)>='A') ? ((n)+10-'A') : ((n)-'0'))
#define StreamFromFOURCC(fcc)  ((WORD)((FromHex(LOBYTE(LOWORD(fcc)))<<4)+(FromHex(HIBYTE(LOWORD(fcc))))))
#define TWOCCFromFOURCC(fcc)  HIWORD(fcc)
#define ToHex(n)  ((BYTE)(((n)>9) ? ((n)-10+'A') : ((n)+'0')))
#define MAKEAVICKID(tcc,stream)  MAKELONG((ToHex((stream)&0x0F)<<8)|(ToHex(((stream)&0xF0)>>4)),tcc)

#define AVIF_HASINDEX  0x00000010
#define AVIF_MUSTUSEINDEX  0x00000020
#define AVIF_ISINTERLEAVED  0x00000100
#define AVIF_TRUSTCKTYPE  0x00000800
#define AVIF_WASCAPTUREFILE  0x00010000
#define AVIF_COPYRIGHTED  0x00020000

#define AVI_HEADERSIZE  2048

#define AVISF_DISABLED  0x00000001
#define AVISF_VIDEO_PALCHANGES  0x00010000

#define AVIIF_LIST  0x00000001L
#define AVIIF_KEYFRAME  0x00000010L
#define AVIIF_NOTIME  0x00000100L
#define AVIIF_COMPUSE  0x0FFF0000L

typedef struct {
    DWORD dwMicroSecPerFrame;
    DWORD dwMaxBytesPerSec;
    DWORD dwPaddingGranularity;
    DWORD dwFlags;
    DWORD dwTotalFrames;
    DWORD dwInitialFrames;
    DWORD dwStreams;
    DWORD dwSuggestedBufferSize;
    DWORD dwWidth;
    DWORD dwHeight;
    DWORD dwReserved[4];
} MainAVIHeader;

typedef struct {
    FOURCC fccType;
    FOURCC fccHandler;
    DWORD dwFlags;
    WORD wPriority;
    WORD wLanguage;
    DWORD dwInitialFrames;
    DWORD dwScale;
    DWORD dwRate;
    DWORD dwStart;
    DWORD dwLength;
    DWORD dwSuggestedBufferSize;
    DWORD dwQuality;
    DWORD dwSampleSize;
    RECT rcFrame;
} AVIStreamHeader;

typedef struct {
    DWORD ckid;
    DWORD dwFlags;
    DWORD dwChunkOffset;
    DWORD dwChunkLength;
} AVIINDEXENTRY;

typedef struct {
    BYTE bFirstEntry;
    BYTE bNumEntries;
    WORD wFlags;
    PALETTEENTRY peNew[];
} AVIPALCHANGE;

#ifdef __cplusplus
}
#endif

#endif /* _AVIFMT_H */
