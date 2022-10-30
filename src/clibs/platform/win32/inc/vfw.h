#ifndef _VFW_H
#define _VFW_H

#ifdef __ORANGEC__ 
#pragma once
#endif

/* Windows Video definitions */

#if !defined(_MMSYSTEM_H)
#include <mmsystem.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define VFWAPI  WINAPI
#define VFWAPIV  WINAPIV

DWORD PASCAL VideoForWindowsVersion(void);

LONG VFWAPI InitVFW(void);
LONG VFWAPI TermVFW(void);

#ifndef MKFOURCC
#define MKFOURCC(ch0,ch1,ch2,ch3)  ((DWORD)(BYTE)(ch0)|((DWORD)(BYTE)(ch1)<<8)|((DWORD)(BYTE)(ch2)<<16)|((DWORD)(BYTE)(ch3)<<24))
#endif

#define ICVERSION  0x0104
DECLARE_HANDLE(HIC);

#define BI_1632  0x32333631

#ifndef aviTWOCC
#define aviTWOCC(ch0,ch1)  ((WORD)(BYTE)(ch0)|((WORD)(BYTE)(ch1)<<8))
#endif

#ifndef ICTYPE_VIDEO
#define ICTYPE_VIDEO  mmioFOURCC('v','i','d','c')
#define ICTYPE_AUDIO  mmioFOURCC('a','u','d','c')
#endif

#ifndef ICERR_OK
#define ICERR_OK  0L
#define ICERR_DONTDRAW  1L
#define ICERR_NEWPALETTE  2L
#define ICERR_GOTOKEYFRAME  3L
#define ICERR_STOPDRAWING  4L
#define ICERR_UNSUPPORTED  -1L
#define ICERR_BADFORMAT  -2L
#define ICERR_MEMORY  -3L
#define ICERR_INTERNAL  -4L
#define ICERR_BADFLAGS  -5L
#define ICERR_BADPARAM  -6L
#define ICERR_BADSIZE  -7L
#define ICERR_BADHANDLE  -8L
#define ICERR_CANTUPDATE  -9L
#define ICERR_ABORT  -10L
#define ICERR_ERROR  -100L
#define ICERR_BADBITDEPTH  -200L
#define ICERR_BADIMAGESIZE  -201L
#define ICERR_CUSTOM  -400L
#endif

#ifndef ICMODE_COMPRESS
#define ICMODE_COMPRESS  1
#define ICMODE_DECOMPRESS  2
#define ICMODE_FASTDECOMPRESS  3
#define ICMODE_QUERY  4
#define ICMODE_FASTCOMPRESS  5
#define ICMODE_DRAW  8
#endif

#define AVIIF_LIST  0x00000001L
#define AVIIF_TWOCC  0x00000002L
#define AVIIF_KEYFRAME  0x00000010L
#define AVIIF_FIRSTPART  0x00000020L
#define AVIIF_LASTPART  0x00000040L
#define AVIIF_MIDPART  (AVIIF_LASTPART|AVIIF_FIRSTPART)
#define AVIIF_NOTIME  0x00000100L
#define AVIIF_COMPUSE  0x0FFF0000L

#define ICQUALITY_LOW  0
#define ICQUALITY_HIGH  10000
#define ICQUALITY_DEFAULT  -1

#define ICM_USER  (DRV_USER+0x0000)

#define ICM_RESERVED  ICM_RESERVED_LOW
#define ICM_RESERVED_LOW  (DRV_USER+0x1000)
#define ICM_RESERVED_HIGH (DRV_USER+0x2000)

#define ICM_GETSTATE  (ICM_RESERVED+0)
#define ICM_SETSTATE  (ICM_RESERVED+1)
#define ICM_GETINFO  (ICM_RESERVED+2)
#define ICM_CONFIGURE  (ICM_RESERVED+10)
#define ICM_ABOUT  (ICM_RESERVED+11)
#define ICM_GETERRORTEXT  (ICM_RESERVED+12)
#define ICM_GETFORMATNAME  (ICM_RESERVED+20)
#define ICM_ENUMFORMATS  (ICM_RESERVED+21)
#define ICM_GETDEFAULTQUALITY  (ICM_RESERVED+30)
#define ICM_GETQUALITY  (ICM_RESERVED+31)
#define ICM_SETQUALITY  (ICM_RESERVED+32)
#define ICM_SET  (ICM_RESERVED+40)
#define ICM_GET  (ICM_RESERVED+41)

#define ICM_FRAMERATE  mmioFOURCC('F','r','m','R')
#define ICM_KEYFRAMERATE  mmioFOURCC('K','e','y','R')

#define ICM_COMPRESS_GET_FORMAT  (ICM_USER+4)
#define ICM_COMPRESS_GET_SIZE  (ICM_USER+5)
#define ICM_COMPRESS_QUERY  (ICM_USER+6)
#define ICM_COMPRESS_BEGIN  (ICM_USER+7)
#define ICM_COMPRESS  (ICM_USER+8)
#define ICM_COMPRESS_END  (ICM_USER+9)
#define ICM_DECOMPRESS_GET_FORMAT  (ICM_USER+10)
#define ICM_DECOMPRESS_QUERY  (ICM_USER+11)
#define ICM_DECOMPRESS_BEGIN  (ICM_USER+12)
#define ICM_DECOMPRESS  (ICM_USER+13)
#define ICM_DECOMPRESS_END  (ICM_USER+14)
#define ICM_DECOMPRESS_SET_PALETTE  (ICM_USER+29)
#define ICM_DECOMPRESS_GET_PALETTE  (ICM_USER+30)
#define ICM_DRAW_QUERY  (ICM_USER+31)
#define ICM_DRAW_BEGIN  (ICM_USER+15)
#define ICM_DRAW_GET_PALETTE  (ICM_USER+16)
#define ICM_DRAW_UPDATE  (ICM_USER+17)
#define ICM_DRAW_START  (ICM_USER+18)
#define ICM_DRAW_STOP  (ICM_USER+19)
#define ICM_DRAW_BITS  (ICM_USER+20)
#define ICM_DRAW_END  (ICM_USER+21)
#define ICM_DRAW_GETTIME  (ICM_USER+32)
#define ICM_DRAW  (ICM_USER+33)
#define ICM_DRAW_WINDOW  (ICM_USER+34)
#define ICM_DRAW_SETTIME  (ICM_USER+35)
#define ICM_DRAW_REALIZE  (ICM_USER+36)
#define ICM_DRAW_FLUSH  (ICM_USER+37)
#define ICM_DRAW_RENDERBUFFER  (ICM_USER+38)
#define ICM_DRAW_START_PLAY  (ICM_USER+39)
#define ICM_DRAW_STOP_PLAY  (ICM_USER+40)
#define ICM_DRAW_SUGGESTFORMAT  (ICM_USER+50)
#define ICM_DRAW_CHANGEPALETTE  (ICM_USER+51)
#define ICM_DRAW_IDLE  (ICM_USER+52)
#define ICM_GETBUFFERSWANTED  (ICM_USER+41)
#define ICM_GETDEFAULTKEYFRAMERATE  (ICM_USER+42)
#define ICM_DECOMPRESSEX_BEGIN  (ICM_USER+60)
#define ICM_DECOMPRESSEX_QUERY  (ICM_USER+61)
#define ICM_DECOMPRESSEX  (ICM_USER+62)
#define ICM_DECOMPRESSEX_END  (ICM_USER+63)
#define ICM_COMPRESS_FRAMES_INFO  (ICM_USER+70)
#define ICM_COMPRESS_FRAMES  (ICM_USER+71)
#define ICM_SET_STATUS_PROC  (ICM_USER+72)

#define VIDCF_QUALITY  0x0001
#define VIDCF_CRUNCH  0x0002
#define VIDCF_TEMPORAL  0x0004
#define VIDCF_COMPRESSFRAMES 0x0008
#define VIDCF_DRAW  0x0010
#define VIDCF_FASTTEMPORALC  0x0020
#define VIDCF_FASTTEMPORALD  0x0080

#define ICCOMPRESS_KEYFRAME  0x00000001L

#define ICCOMPRESSFRAMES_PADDING  0x00000001

#define ICSTATUS_START  0
#define ICSTATUS_STATUS  1
#define ICSTATUS_END  2
#define ICSTATUS_ERROR  3
#define ICSTATUS_YIELD  4

#define ICDECOMPRESS_HURRYUP  0x80000000L
#define ICDECOMPRESS_UPDATE  0x40000000L
#define ICDECOMPRESS_PREROLL  0x20000000L
#define ICDECOMPRESS_NULLFRAME  0x10000000L
#define ICDECOMPRESS_NOTKEYFRAME  0x08000000L

#define ICDRAW_QUERY  0x00000001L
#define ICDRAW_FULLSCREEN  0x00000002L
#define ICDRAW_HDC  0x00000004L
#define ICDRAW_ANIMATE  0x00000008L
#define ICDRAW_CONTINUE  0x00000010L
#define ICDRAW_MEMORYDC  0x00000020L
#define ICDRAW_UPDATING  0x00000040L
#define ICDRAW_RENDER  0x00000080L
#define ICDRAW_BUFFER  0x00000100L

#define ICDRAW_HURRYUP  0x80000000L
#define ICDRAW_UPDATE  0x40000000L
#define ICDRAW_PREROLL  0x20000000L
#define ICDRAW_NULLFRAME  0x10000000L
#define ICDRAW_NOTKEYFRAME  0x08000000L

#define ICINSTALL_UNICODE  0x8000
#define ICINSTALL_FUNCTION  0x0001
#define ICINSTALL_DRIVER  0x0002
#define ICINSTALL_HDRV  0x0004
#define ICINSTALL_DRIVERW  0x8002

#define ICMF_CONFIGURE_QUERY  0x00000001
#define ICMF_ABOUT_QUERY  0x00000001

#define ICDECOMPRESS_HURRYUP  0x80000000L

#define ICMF_COMPVARS_VALID  0x00000001

#define ICMF_CHOOSE_KEYFRAME  0x0001
#define ICMF_CHOOSE_DATARATE  0x0002
#define ICMF_CHOOSE_PREVIEW  0x0004
#define ICMF_CHOOSE_ALLCOMPRESSORS  0x0008

#define DDF_0001  0x0001
#define DDF_UPDATE  0x0002
#define DDF_SAME_HDC  0x0004
#define DDF_SAME_DRAW  0x0008
#define DDF_DONTDRAW  0x0010
#define DDF_ANIMATE  0x0020
#define DDF_BUFFER  0x0040
#define DDF_JUSTDRAWIT  0x0080
#define DDF_FULLSCREEN  0x0100
#define DDF_BACKGROUNDPAL  0x0200
#define DDF_NOTKEYFRAME  0x0400
#define DDF_HURRYUP  0x0800
#define DDF_HALFTONE  0x1000
#define DDF_2000  0x2000

#define DDF_PREROLL  DDF_DONTDRAW
#define DDF_SAME_DIB  DDF_SAME_DRAW
#define DDF_SAME_SIZE  DDF_SAME_DRAW

#define PD_CAN_DRAW_DIB  0x0001
#define PD_CAN_STRETCHDIB  0x0002
#define PD_STRETCHDIB_1_1_OK  0x0004
#define PD_STRETCHDIB_1_2_OK  0x0008
#define PD_STRETCHDIB_1_N_OK  0x0010

#define ICQueryAbout(hic)  (ICSendMessage(hic,ICM_ABOUT,(DWORD_PTR)-1,ICMF_ABOUT_QUERY) == ICERR_OK)
#define ICAbout(hic,hwnd)  ICSendMessage(hic,ICM_ABOUT,(DWORD_PTR)(UINT_PTR)(hwnd),0)
#define ICQueryConfigure(hic)  (ICSendMessage(hic,ICM_CONFIGURE,(DWORD_PTR)-1,ICMF_CONFIGURE_QUERY) == ICERR_OK)
#define ICConfigure(hic,hwnd)  ICSendMessage(hic,ICM_CONFIGURE,(DWORD_PTR)(UINT_PTR)(hwnd),0)
#define ICGetState(hic,pv,cb)  ICSendMessage(hic,ICM_GETSTATE,(DWORD_PTR)(LPVOID)(pv),(DWORD_PTR)(cb))
#define ICSetState(hic,pv,cb)  ICSendMessage(hic,ICM_SETSTATE,(DWORD_PTR)(LPVOID)(pv),(DWORD_PTR)(cb))
#define ICGetStateSize(hic)  (DWORD)ICGetState(hic,NULL,0)
#define ICGetDefaultQuality(hic)  (ICSendMessage(hic,ICM_GETDEFAULTQUALITY,(DWORD_PTR)(LPVOID)&dwICValue,sizeof(DWORD)),dwICValue)
#define ICGetDefaultKeyFrameRate(hic)  (ICSendMessage(hic,ICM_GETDEFAULTKEYFRAMERATE,(DWORD_PTR)(LPVOID)&dwICValue,sizeof(DWORD)),dwICValue)
#define ICDrawWindow(hic,prc)  ICSendMessage(hic,ICM_DRAW_WINDOW,(DWORD_PTR)(LPVOID)(prc),sizeof(RECT))
#define ICCompressBegin(hic,lpbiInput,lpbiOutput)  ICSendMessage(hic,ICM_COMPRESS_BEGIN,(DWORD_PTR)(LPVOID)(lpbiInput),(DWORD_PTR)(LPVOID)(lpbiOutput))
#define ICCompressQuery(hic,lpbiInput,lpbiOutput)  ICSendMessage(hic,ICM_COMPRESS_QUERY,(DWORD_PTR)(LPVOID)(lpbiInput),(DWORD_PTR)(LPVOID)(lpbiOutput))
#define ICCompressGetFormat(hic,lpbiInput,lpbiOutput)  ICSendMessage(hic,ICM_COMPRESS_GET_FORMAT,(DWORD_PTR)(LPVOID)(lpbiInput),(DWORD_PTR)(LPVOID)(lpbiOutput))
#define ICCompressGetFormatSize(hic,lpbi)  (DWORD)ICCompressGetFormat(hic,lpbi,NULL)
#define ICCompressGetSize(hic,lpbiInput,lpbiOutput)  (DWORD)ICSendMessage(hic,ICM_COMPRESS_GET_SIZE,(DWORD_PTR)(LPVOID)(lpbiInput),(DWORD_PTR)(LPVOID)(lpbiOutput))
#define ICCompressEnd(hic)  ICSendMessage(hic,ICM_COMPRESS_END,0,0)
#define ICDecompressBegin(hic,lpbiInput,lpbiOutput)  ICSendMessage(hic,ICM_DECOMPRESS_BEGIN,(DWORD_PTR)(LPVOID)(lpbiInput),(DWORD_PTR)(LPVOID)(lpbiOutput))
#define ICDecompressQuery(hic,lpbiInput,lpbiOutput)  ICSendMessage(hic,ICM_DECOMPRESS_QUERY,(DWORD_PTR)(LPVOID)(lpbiInput),(DWORD_PTR)(LPVOID)(lpbiOutput))
#define ICDecompressGetFormat(hic,lpbiInput,lpbiOutput)  ((LONG)ICSendMessage(hic,ICM_DECOMPRESS_GET_FORMAT,(DWORD_PTR)(LPVOID)(lpbiInput),(DWORD_PTR)(LPVOID)(lpbiOutput)))
#define ICDecompressGetFormatSize(hic,lpbi)  ICDecompressGetFormat(hic,lpbi,NULL)
#define ICDecompressGetPalette(hic,lpbiInput,lpbiOutput)  ICSendMessage(hic,ICM_DECOMPRESS_GET_PALETTE,(DWORD_PTR)(LPVOID)(lpbiInput),(DWORD_PTR)(LPVOID)(lpbiOutput))
#define ICDecompressSetPalette(hic,lpbiPalette)  ICSendMessage(hic,ICM_DECOMPRESS_SET_PALETTE,(DWORD_PTR)(LPVOID)(lpbiPalette),0)
#define ICDecompressEnd(hic)  ICSendMessage(hic,ICM_DECOMPRESS_END,0,0)
#define ICDecompressExEnd(hic)  ICSendMessage(hic,ICM_DECOMPRESSEX_END,0,0)
#define ICDrawQuery(hic,lpbiInput)  ICSendMessage(hic,ICM_DRAW_QUERY,(DWORD_PTR)(LPVOID)(lpbiInput),0L)
#define ICDrawChangePalette(hic,lpbiInput)  ICSendMessage(hic,ICM_DRAW_CHANGEPALETTE,(DWORD_PTR)(LPVOID)(lpbiInput),0L)
#define ICGetBuffersWanted(hic,lpdwBuffers)  ICSendMessage(hic,ICM_GETBUFFERSWANTED,(DWORD_PTR)(LPVOID)(lpdwBuffers),0)
#define ICDrawEnd(hic)  ICSendMessage(hic,ICM_DRAW_END,0,0)
#define ICDrawStart(hic)  ICSendMessage(hic,ICM_DRAW_START,0,0)
#define ICDrawStartPlay(hic,lFrom,lTo)  ICSendMessage(hic,ICM_DRAW_START_PLAY,(DWORD_PTR)(lFrom),(DWORD_PTR)(lTo))
#define ICDrawStop(hic)  ICSendMessage(hic,ICM_DRAW_STOP,0,0)
#define ICDrawStopPlay(hic)  ICSendMessage(hic,ICM_DRAW_STOP_PLAY,0,0)
#define ICDrawGetTime(hic,lplTime)  ICSendMessage(hic,ICM_DRAW_GETTIME,(DWORD_PTR)(LPVOID)(lplTime),0)
#define ICDrawSetTime(hic,lTime)  ICSendMessage(hic,ICM_DRAW_SETTIME,(DWORD_PTR)lTime,0)
#define ICDrawRealize(hic,hdc,fBackground)  ICSendMessage(hic,ICM_DRAW_REALIZE,(DWORD_PTR)(UINT_PTR)(HDC)(hdc),(DWORD_PTR)(BOOL)(fBackground))
#define ICDrawFlush(hic)  ICSendMessage(hic,ICM_DRAW_FLUSH,0,0)
#define ICDrawRenderBuffer(hic)  ICSendMessage(hic,ICM_DRAW_RENDERBUFFER,0,0)
#define ICDecompressOpen(fccType,fccHandler,lpbiIn,lpbiOut)  ICLocate(fccType,fccHandler,lpbiIn,lpbiOut,ICMODE_DECOMPRESS)
#define ICDrawOpen(fccType,fccHandler,lpbiIn)  ICLocate(fccType,fccHandler,lpbiIn,NULL,ICMODE_DRAW)

#define DrawDibUpdate(hdd,hdc,x,y)  DrawDibDraw(hdd,hdc,x,y,0,0,NULL,NULL,0,0,0,0,DDF_UPDATE)

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
#define AVIF_WASCAPTUREFILE  0x00010000
#define AVIF_COPYRIGHTED  0x00020000

#define AVI_HEADERSIZE  2048

#define AVISF_DISABLED  0x00000001
#define AVISF_VIDEO_PALCHANGES  0x00010000

typedef struct {
    DWORD dwSize;
    DWORD fccType;
    DWORD fccHandler;
    DWORD dwVersion;
    DWORD dwFlags;
    LRESULT dwError;
    LPVOID pV1Reserved;
    LPVOID pV2Reserved;
    DWORD dnDevNode;
} ICOPEN;

typedef struct {
    DWORD dwSize;
    DWORD fccType;
    DWORD fccHandler;
    DWORD dwFlags;
    DWORD dwVersion;
    DWORD dwVersionICM;
    WCHAR szName[16];
    WCHAR szDescription[128];
    WCHAR szDriver[128];
} ICINFO;

typedef struct {
    DWORD dwFlags;
    LPBITMAPINFOHEADER lpbiOutput;
    LPVOID lpOutput;
    LPBITMAPINFOHEADER lpbiInput;
    LPVOID lpInput;
    LPDWORD lpckid;
    LPDWORD lpdwFlags;
    LONG lFrameNum;
    DWORD dwFrameSize;
    DWORD dwQuality;
    LPBITMAPINFOHEADER lpbiPrev;
    LPVOID lpPrev;
} ICCOMPRESS;

typedef struct {
    DWORD dwFlags;
    LPBITMAPINFOHEADER lpbiOutput;
    LPARAM lOutput;
    LPBITMAPINFOHEADER lpbiInput;
    LPARAM lInput;
    LONG lStartFrame;
    LONG lFrameCount;
    LONG lQuality;
    LONG lDataRate;
    LONG lKeyRate;
    DWORD dwRate;
    DWORD dwScale;
    DWORD dwOverheadPerFrame;
    DWORD dwReserved2;
    LONG (CALLBACK *GetData)(LPARAM,LONG,LPVOID,LONG);
    LONG (CALLBACK *PutData)(LPARAM,LONG,LPVOID,LONG);
} ICCOMPRESSFRAMES;

typedef struct {
    DWORD dwFlags;
    LPARAM lParam;
    LONG (CALLBACK *Status)(LPARAM,UINT,LONG);
} ICSETSTATUSPROC;

typedef struct {
    DWORD dwFlags;
    LPBITMAPINFOHEADER lpbiInput;
    LPVOID lpInput;
    LPBITMAPINFOHEADER lpbiOutput;
    LPVOID lpOutput;
    DWORD ckid;
} ICDECOMPRESS;

typedef struct {
    DWORD dwFlags;
    LPBITMAPINFOHEADER lpbiSrc;
    LPVOID lpSrc;
    LPBITMAPINFOHEADER lpbiDst;
    LPVOID lpDst;
    int xDst;
    int yDst;
    int dxDst;
    int dyDst;
    int xSrc;
    int ySrc;
    int dxSrc;
    int dySrc;
} ICDECOMPRESSEX;

typedef struct {
    DWORD dwFlags;
    HPALETTE hpal;
    HWND hwnd;
    HDC hdc;
    int xDst;
    int yDst;
    int dxDst;
    int dyDst;
    LPBITMAPINFOHEADER lpbi;
    int xSrc;
    int ySrc;
    int dxSrc;
    int dySrc;
    DWORD dwRate;
    DWORD dwScale;
} ICDRAWBEGIN;

typedef struct {
    DWORD dwFlags;
    LPVOID lpFormat;
    LPVOID lpData;
    DWORD cbData;
    LONG lTime;
} ICDRAW;

typedef struct {
    LPBITMAPINFOHEADER lpbiIn;
    LPBITMAPINFOHEADER lpbiSuggest;
    int dxSrc;
    int dySrc;
    int dxDst;
    int dyDst;
    HIC hicDecompressor;
} ICDRAWSUGGEST;

typedef struct {
    DWORD dwFlags;
    int iStart;
    int iLen;
    LPPALETTEENTRY lppe;
} ICPALETTE;

typedef struct {
    LONG cbSize;
    DWORD dwFlags;
    HIC hic;
    DWORD fccType;
    DWORD fccHandler;
    LPBITMAPINFO lpbiIn;
    LPBITMAPINFO lpbiOut;
    LPVOID lpBitsOut;
    LPVOID lpBitsPrev;
    LONG lFrame;
    LONG lKey;
    LONG lDataRate;
    LONG lQ;
    LONG lKeyCount;
    LPVOID lpState;
    LONG cbState;
} COMPVARS, *PCOMPVARS;

static DWORD dwICValue;
#ifdef __POCC__
#pragma ref dwICValue  /* get rid of "not referenced" warning */
#endif

typedef HANDLE HDRAWDIB;

typedef struct {
    LONG timeCount;
    LONG timeDraw;
    LONG timeDecompress;
    LONG timeDither;
    LONG timeStretch;
    LONG timeBlt;
    LONG timeSetDIBits;
} DRAWDIBTIME, *LPDRAWDIBTIME;

#ifndef _MMSYSTEM_H
typedef DWORD FOURCC;
#endif

typedef WORD TWOCC;

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

BOOL VFWAPI ICInfo(DWORD,DWORD,ICINFO*);
BOOL VFWAPI ICInstall(DWORD,DWORD,LPARAM,LPSTR,UINT);
BOOL VFWAPI ICRemove(DWORD,DWORD,UINT);
LRESULT VFWAPI ICGetInfo(HIC,ICINFO*,DWORD);
HIC VFWAPI ICOpen(DWORD,DWORD,UINT);
#if __POCC__ >= 290
#pragma warn(push)
#pragma warn(disable:2027 2028)  /* Missing prototype */
#endif
HIC VFWAPI ICOpenFunction(DWORD,DWORD,UINT,FARPROC);
#if __POCC__ >= 290
#pragma warn(pop)
#endif
LRESULT VFWAPI ICClose(HIC);
LRESULT VFWAPI ICSendMessage(HIC,UINT,DWORD_PTR,DWORD_PTR);
DWORD VFWAPIV ICCompress(HIC,DWORD,LPBITMAPINFOHEADER,LPVOID,LPBITMAPINFOHEADER,LPVOID,LPDWORD,LPDWORD,LONG,DWORD,DWORD,LPBITMAPINFOHEADER,LPVOID);
DWORD VFWAPIV ICDecompress(HIC,DWORD,LPBITMAPINFOHEADER,LPVOID,LPBITMAPINFOHEADER,LPVOID);
DWORD VFWAPIV ICDrawBegin(HIC,DWORD,HPALETTE,HWND,HDC,int,int,int,int,LPBITMAPINFOHEADER,int,int,int,int,DWORD,DWORD);
DWORD VFWAPIV ICDraw(HIC,DWORD,LPVOID,LPVOID,DWORD,LONG);
HIC VFWAPI ICLocate(DWORD,DWORD,LPBITMAPINFOHEADER,LPBITMAPINFOHEADER,WORD);
HIC VFWAPI ICGetDisplayFormat(HIC,LPBITMAPINFOHEADER,LPBITMAPINFOHEADER,int,int,int);
HANDLE VFWAPI ICImageCompress(HIC,UINT,LPBITMAPINFO,LPVOID,LPBITMAPINFO,LONG,LONG*);
HANDLE VFWAPI ICImageDecompress(HIC,UINT,LPBITMAPINFO,LPVOID,LPBITMAPINFO);
BOOL VFWAPI ICCompressorChoose(HWND,UINT,LPVOID,LPVOID,PCOMPVARS,LPSTR);
BOOL VFWAPI ICSeqCompressFrameStart(PCOMPVARS,LPBITMAPINFO);
void VFWAPI ICSeqCompressFrameEnd(PCOMPVARS);
LPVOID VFWAPI ICSeqCompressFrame(PCOMPVARS,UINT,LPVOID,BOOL*,LONG*);
void VFWAPI ICCompressorFree(PCOMPVARS);

extern BOOL VFWAPI DrawDibInit(void);
extern HDRAWDIB VFWAPI DrawDibOpen(void);
extern BOOL VFWAPI DrawDibClose(HDRAWDIB);
extern  LPVOID VFWAPI DrawDibGetBuffer(HDRAWDIB,LPBITMAPINFOHEADER,DWORD,DWORD);
extern UINT VFWAPI DrawDibError(HDRAWDIB);
extern HPALETTE VFWAPI DrawDibGetPalette(HDRAWDIB);
extern BOOL VFWAPI DrawDibSetPalette(HDRAWDIB,HPALETTE);
extern BOOL VFWAPI DrawDibChangePalette(HDRAWDIB,int,int,LPPALETTEENTRY);
extern UINT VFWAPI DrawDibRealize(HDRAWDIB,HDC,BOOL);
extern BOOL VFWAPI DrawDibStart(HDRAWDIB,DWORD);
extern BOOL VFWAPI DrawDibStop(HDRAWDIB);
extern BOOL VFWAPI DrawDibBegin(HDRAWDIB,HDC,int,int,LPBITMAPINFOHEADER,int,int,UINT);
extern BOOL VFWAPI DrawDibDraw(HDRAWDIB,HDC,int,int,int,int,LPBITMAPINFOHEADER,LPVOID,int,int,int,int,UINT);
BOOL VFWAPI DrawDibTime(HDRAWDIB,LPDRAWDIBTIME);
extern BOOL VFWAPI DrawDibEnd(HDRAWDIB);
LRESULT VFWAPI DrawDibProfileDisplay(LPBITMAPINFOHEADER);

static __inline LRESULT VFWAPI ICDecompressEx(HIC hic, DWORD dwFlags, LPBITMAPINFOHEADER lpbiSrc, LPVOID lpSrc, int xSrc, int ySrc, int dxSrc, int dySrc, LPBITMAPINFOHEADER lpbiDst, LPVOID lpDst, int xDst, int yDst, int dxDst, int dyDst)
{
    ICDECOMPRESSEX ic;
    ic.dwFlags = dwFlags;
    ic.lpbiSrc = lpbiSrc;
    ic.lpSrc = lpSrc;
    ic.xSrc = xSrc;
    ic.ySrc = ySrc;
    ic.dxSrc = dxSrc;
    ic.dySrc = dySrc;
    ic.lpbiDst = lpbiDst;
    ic.lpDst = lpDst;
    ic.xDst = xDst;
    ic.yDst = yDst;
    ic.dxDst = dxDst;
    ic.dyDst = dyDst;
    return ICSendMessage(hic,ICM_DECOMPRESSEX,(DWORD_PTR)&ic,sizeof(ic));
}

static __inline LRESULT VFWAPI ICDecompressExBegin(HIC hic, DWORD dwFlags, LPBITMAPINFOHEADER lpbiSrc, LPVOID lpSrc, int xSrc, int ySrc, int dxSrc, int dySrc, LPBITMAPINFOHEADER lpbiDst, LPVOID lpDst, int xDst, int yDst, int dxDst, int dyDst)
{
    ICDECOMPRESSEX ic;
    ic.dwFlags = dwFlags;
    ic.lpbiSrc = lpbiSrc;
    ic.lpSrc = lpSrc;
    ic.xSrc = xSrc;
    ic.ySrc = ySrc;
    ic.dxSrc = dxSrc;
    ic.dySrc = dySrc;
    ic.lpbiDst = lpbiDst;
    ic.lpDst = lpDst;
    ic.xDst = xDst;
    ic.yDst = yDst;
    ic.dxDst = dxDst;
    ic.dyDst = dyDst;
    return ICSendMessage(hic,ICM_DECOMPRESSEX_BEGIN,(DWORD_PTR)&ic,sizeof(ic));
}

static __inline LRESULT VFWAPI ICDecompressExQuery(HIC hic, DWORD dwFlags, LPBITMAPINFOHEADER lpbiSrc, LPVOID lpSrc, int xSrc, int ySrc, int dxSrc, int dySrc, LPBITMAPINFOHEADER lpbiDst, LPVOID lpDst, int xDst, int yDst, int dxDst, int dyDst)
{
    ICDECOMPRESSEX ic;
    ic.dwFlags = dwFlags;
    ic.lpbiSrc = lpbiSrc;
    ic.lpSrc = lpSrc;
    ic.xSrc = xSrc;
    ic.ySrc = ySrc;
    ic.dxSrc = dxSrc;
    ic.dySrc = dySrc;
    ic.lpbiDst = lpbiDst;
    ic.lpDst = lpDst;
    ic.xDst = xDst;
    ic.yDst = yDst;
    ic.dxDst = dxDst;
    ic.dyDst = dyDst;
    return ICSendMessage(hic,ICM_DECOMPRESSEX_QUERY,(DWORD_PTR)&ic,sizeof(ic));
}

static __inline LRESULT VFWAPI ICDrawSuggestFormat(HIC hic, LPBITMAPINFOHEADER lpbiIn, LPBITMAPINFOHEADER lpbiOut, int dxSrc, int dySrc, int dxDst, int dyDst, HIC hicDecomp)
{
    ICDRAWSUGGEST ic;
    ic.lpbiIn = lpbiIn;
    ic.lpbiSuggest = lpbiOut;
    ic.dxSrc = dxSrc;
    ic.dySrc = dySrc;
    ic.dxDst = dxDst;
    ic.dyDst = dyDst;
    ic.hicDecompressor = hicDecomp;
    return ICSendMessage(hic,ICM_DRAW_SUGGESTFORMAT,(DWORD_PTR)&ic,sizeof(ic));
}

static __inline LRESULT VFWAPI ICSetStatusProc(HIC hic, DWORD dwFlags, LRESULT lParam, LONG(CALLBACK *fpfnStatus)(LPARAM, UINT, LONG))
{
    ICSETSTATUSPROC ic;
    ic.dwFlags = dwFlags;
    ic.lParam = lParam;
    ic.Status = fpfnStatus;
    return ICSendMessage(hic,ICM_SET_STATUS_PROC,(DWORD_PTR)&ic,sizeof(ic));
}

#ifdef __cplusplus
}
#endif

#include "pshpack8.h"

#include <mmreg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define AVIGETFRAMEF_BESTDISPLAYFMT  1

#define AVISTREAMINFO_DISABLED  0x00000001
#define AVISTREAMINFO_FORMATCHANGES  0x00010000

#define AVIFILEINFO_HASINDEX  0x00000010
#define AVIFILEINFO_MUSTUSEINDEX  0x00000020
#define AVIFILEINFO_ISINTERLEAVED  0x00000100
#define AVIFILEINFO_WASCAPTUREFILE  0x00010000
#define AVIFILEINFO_COPYRIGHTED  0x00020000

#define AVIFILECAPS_CANREAD  0x00000001
#define AVIFILECAPS_CANWRITE  0x00000002
#define AVIFILECAPS_ALLKEYFRAMES  0x00000010
#define AVIFILECAPS_NOCOMPRESSION  0x00000020

#define AVICOMPRESSF_INTERLEAVE  0x00000001
#define AVICOMPRESSF_DATARATE  0x00000002
#define AVICOMPRESSF_KEYFRAMES  0x00000004
#define AVICOMPRESSF_VALID  0x00000008

typedef struct _AVISTREAMINFOW {
    DWORD fccType;
    DWORD fccHandler;
    DWORD dwFlags;
    DWORD dwCaps;
    WORD wPriority;
    WORD wLanguage;
    DWORD dwScale;
    DWORD dwRate;
    DWORD dwStart;
    DWORD dwLength;
    DWORD dwInitialFrames;
    DWORD dwSuggestedBufferSize;
    DWORD dwQuality;
    DWORD dwSampleSize;
    RECT rcFrame;
    DWORD dwEditCount;
    DWORD dwFormatChangeCount;
    WCHAR szName[64];
} AVISTREAMINFOW, *LPAVISTREAMINFOW;

typedef struct _AVISTREAMINFOA {
    DWORD fccType;
    DWORD fccHandler;
    DWORD dwFlags;
    DWORD dwCaps;
    WORD wPriority;
    WORD wLanguage;
    DWORD dwScale;
    DWORD dwRate;
    DWORD dwStart;
    DWORD dwLength;
    DWORD dwInitialFrames;
    DWORD dwSuggestedBufferSize;
    DWORD dwQuality;
    DWORD dwSampleSize;
    RECT rcFrame;
    DWORD dwEditCount;
    DWORD dwFormatChangeCount;
    char szName[64];
} AVISTREAMINFOA, *LPAVISTREAMINFOA;

typedef struct _AVIFILEINFOW {
    DWORD dwMaxBytesPerSec;
    DWORD dwFlags;
    DWORD dwCaps;
    DWORD dwStreams;
    DWORD dwSuggestedBufferSize;
    DWORD dwWidth;
    DWORD dwHeight;
    DWORD dwScale;
    DWORD dwRate;
    DWORD dwLength;
    DWORD dwEditCount;
    WCHAR szFileType[64];
} AVIFILEINFOW, *LPAVIFILEINFOW;

typedef struct _AVIFILEINFOA {
    DWORD dwMaxBytesPerSec;
    DWORD dwFlags;
    DWORD dwCaps;
    DWORD dwStreams;
    DWORD dwSuggestedBufferSize;
    DWORD dwWidth;
    DWORD dwHeight;
    DWORD dwScale;
    DWORD dwRate;
    DWORD dwLength;
    DWORD dwEditCount;
    char szFileType[64];
} AVIFILEINFOA, *LPAVIFILEINFOA;

typedef BOOL(PASCAL *AVISAVECALLBACK)(int);

typedef struct {
    DWORD fccType;
    DWORD fccHandler;
    DWORD dwKeyFrameEvery;
    DWORD dwQuality;
    DWORD dwBytesPerSecond;
    DWORD dwFlags;
    LPVOID lpFormat;
    DWORD cbFormat;
    LPVOID lpParms;
    DWORD cbParms;
    DWORD dwInterleaveEvery;
} AVICOMPRESSOPTIONS, *LPAVICOMPRESSOPTIONS;

#ifdef UNICODE
#define AVISTREAMINFO AVISTREAMINFOW
#define LPAVISTREAMINFO LPAVISTREAMINFOW
#define AVIFILEINFO AVIFILEINFOW
#define LPAVIFILEINFO LPAVIFILEINFOW
#else
#define AVISTREAMINFO AVISTREAMINFOA
#define LPAVISTREAMINFO LPAVISTREAMINFOA
#define AVIFILEINFO AVIFILEINFOA
#define LPAVIFILEINFO LPAVIFILEINFOA
#endif /* UNICODE */

#ifdef __cplusplus
}
#endif

#include <ole2.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEFINE_AVIGUID(name,l,w1,w2)  DEFINE_GUID(name,l,w1,w2,0xC0,0,0,0,0,0,0,0x46)

DEFINE_AVIGUID(IID_IAVIFile,0x00020020,0,0);
DEFINE_AVIGUID(IID_IAVIStream,0x00020021,0,0);
DEFINE_AVIGUID(IID_IAVIStreaming,0x00020022,0,0);
DEFINE_AVIGUID(IID_IGetFrame,0x00020023,0,0);
DEFINE_AVIGUID(IID_IAVIEditStream,0x00020024,0,0);
#ifndef UNICODE
DEFINE_AVIGUID(CLSID_AVISimpleUnMarshal,0x00020009,0,0);
#endif
DEFINE_AVIGUID(CLSID_AVIFile, 0x00020000, 0, 0);

#define AVIFILEHANDLER_CANREAD  0x0001
#define AVIFILEHANDLER_CANWRITE  0x0002
#define AVIFILEHANDLER_CANACCEPTNONRGB  0x0004

#define AVISTREAMREAD_CONVENIENT  (-1L)

#define FIND_DIR  0x0000000FL
#define FIND_NEXT  0x00000001L
#define FIND_PREV  0x00000004L
#define FIND_FROM_START 0x00000008L

#define FIND_TYPE  0x000000F0L
#define FIND_KEY  0x00000010L
#define FIND_ANY  0x00000020L
#define FIND_FORMAT  0x00000040L

#define FIND_RET  0x0000F000L
#define FIND_POS  0x00000000L
#define FIND_LENGTH  0x00001000L
#define FIND_OFFSET  0x00002000L
#define FIND_SIZE  0x00003000L
#define FIND_INDEX  0x00004000L

#define SEARCH_NEAREST  FIND_PREV
#define SEARCH_BACKWARD FIND_PREV
#define SEARCH_FORWARD  FIND_NEXT
#define SEARCH_KEY  FIND_KEY
#define SEARCH_ANY  FIND_ANY

#ifndef AVIERR_OK
#define AVIERR_OK  0L

#define MAKE_AVIERR(error)  MAKE_SCODE(SEVERITY_ERROR,FACILITY_ITF,0x4000+error)

#define AVIERR_UNSUPPORTED  MAKE_AVIERR(101)
#define AVIERR_BADFORMAT  MAKE_AVIERR(102)
#define AVIERR_MEMORY  MAKE_AVIERR(103)
#define AVIERR_INTERNAL  MAKE_AVIERR(104)
#define AVIERR_BADFLAGS  MAKE_AVIERR(105)
#define AVIERR_BADPARAM  MAKE_AVIERR(106)
#define AVIERR_BADSIZE  MAKE_AVIERR(107)
#define AVIERR_BADHANDLE  MAKE_AVIERR(108)
#define AVIERR_FILEREAD  MAKE_AVIERR(109)
#define AVIERR_FILEWRITE  MAKE_AVIERR(110)
#define AVIERR_FILEOPEN  MAKE_AVIERR(111)
#define AVIERR_COMPRESSOR  MAKE_AVIERR(112)
#define AVIERR_NOCOMPRESSOR  MAKE_AVIERR(113)
#define AVIERR_READONLY  MAKE_AVIERR(114)
#define AVIERR_NODATA  MAKE_AVIERR(115)
#define AVIERR_BUFFERTOOSMALL  MAKE_AVIERR(116)
#define AVIERR_CANTCOMPRESS  MAKE_AVIERR(117)
#define AVIERR_USERABORT  MAKE_AVIERR(198)
#define AVIERR_ERROR  MAKE_AVIERR(199)
#endif /* AVIERR_OK */

#undef INTERFACE
#define INTERFACE IAVIStream
DECLARE_INTERFACE_(IAVIStream, IUnknown) {
    STDMETHOD(QueryInterface)(THIS_ REFIID,LPVOID*) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(Create)(THIS_ LPARAM,LPARAM) PURE;
    STDMETHOD(Info)(THIS_ AVISTREAMINFOW*,LONG) PURE;
    STDMETHOD_(LONG, FindSample)(THIS_ LONG,LONG) PURE;
    STDMETHOD(ReadFormat)(THIS_ LONG,LPVOID,LONG*) PURE;
    STDMETHOD(SetFormat)(THIS_ LONG,LPVOID,LONG) PURE;
    STDMETHOD(Read)(THIS_ LONG,LONG,LPVOID,LONG,LONG*,LONG*) PURE;
    STDMETHOD(Write)(THIS_ LONG,LONG,LPVOID,LONG,DWORD,LONG*,LONG*) PURE;
    STDMETHOD(Delete)(THIS_ LONG,LONG) PURE;
    STDMETHOD(ReadData)(THIS_ DWORD,LPVOID,LONG*) PURE;
    STDMETHOD(WriteData)(THIS_ DWORD,LPVOID,LONG) PURE;
    STDMETHOD(SetInfo)(THIS_ AVISTREAMINFOW*,LONG) PURE;
};
typedef IAVIStream *PAVISTREAM;

#undef INTERFACE
#define INTERFACE IAVIStreaming
DECLARE_INTERFACE_(IAVIStreaming, IUnknown) {
    STDMETHOD(QueryInterface)(THIS_ REFIID,LPVOID*) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(Begin)(THIS_ LONG,LONG,LONG) PURE;
    STDMETHOD(End)(THIS) PURE;
};
typedef IAVIStreaming *PAVISTREAMING;

#undef INTERFACE
#define INTERFACE IAVIEditStream
DECLARE_INTERFACE_(IAVIEditStream, IUnknown) {
    STDMETHOD(QueryInterface)(THIS_ REFIID,LPVOID*) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(Cut)(THIS_ LONG*,LONG*,PAVISTREAM*) PURE;
    STDMETHOD(Copy)(THIS_ LONG*,LONG*,PAVISTREAM*) PURE;
    STDMETHOD(Paste)(THIS_ LONG*,LONG*,PAVISTREAM,LONG,LONG) PURE;
    STDMETHOD(Clone)(THIS_ PAVISTREAM*) PURE;
    STDMETHOD(SetInfo)(THIS_ AVISTREAMINFOW*,LONG) PURE;
};
typedef IAVIEditStream  *PAVIEDITSTREAM;

#undef INTERFACE
#define INTERFACE IAVIFile
#define PAVIFILE IAVIFile *
DECLARE_INTERFACE_(IAVIFile, IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID,LPVOID*) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD(Info)(THIS_ AVIFILEINFOW*,LONG) PURE;
    STDMETHOD(GetStream)(THIS_ PAVISTREAM*,DWORD,LONG) PURE;
    STDMETHOD(CreateStream)(THIS_ PAVISTREAM*,AVISTREAMINFOW*) PURE;
    STDMETHOD(WriteData)(THIS_ DWORD,LPVOID,LONG) PURE;
    STDMETHOD(ReadData)(THIS_ DWORD,LPVOID,LONG*) PURE;
    STDMETHOD(EndRecord)(THIS) PURE;
    STDMETHOD(DeleteStream)(THIS_ DWORD,LONG) PURE;
};
#undef PAVIFILE
typedef IAVIFile *PAVIFILE;

#undef INTERFACE
#define INTERFACE IGetFrame
#define PGETFRAME IGetFrame *
DECLARE_INTERFACE_(IGetFrame,IUnknown)
{
    STDMETHOD(QueryInterface)(THIS_ REFIID,LPVOID*) PURE;
    STDMETHOD_(ULONG,AddRef)(THIS) PURE;
    STDMETHOD_(ULONG,Release)(THIS) PURE;
    STDMETHOD_(LPVOID,GetFrame)(THIS_ LONG) PURE;
    STDMETHOD(Begin)(THIS_ LONG,LONG,LONG) PURE;
    STDMETHOD(End)(THIS) PURE;
    STDMETHOD(SetFormat)(THIS_ LPBITMAPINFOHEADER,LPVOID,int,int,int,int) PURE;
};
#undef PGETFRAME
typedef IGetFrame *PGETFRAME;

STDAPI_(void) AVIFileInit(void);
STDAPI_(void) AVIFileExit(void);
STDAPI_(ULONG) AVIFileAddRef(PAVIFILE);
STDAPI_(ULONG) AVIFileRelease(PAVIFILE);
STDAPI AVIFileOpenA(PAVIFILE*,LPCSTR,UINT,LPCLSID);
STDAPI AVIFileOpenW(PAVIFILE*,LPCWSTR,UINT,LPCLSID);
STDAPI AVIFileInfoW(PAVIFILE,LPAVIFILEINFOW,LONG);
STDAPI AVIFileInfoA(PAVIFILE,LPAVIFILEINFOA,LONG);
STDAPI AVIFileGetStream(PAVIFILE,PAVISTREAM*,DWORD,LONG);
STDAPI AVIFileCreateStreamW(PAVIFILE,PAVISTREAM*,AVISTREAMINFOW*);
STDAPI AVIFileCreateStreamA(PAVIFILE,PAVISTREAM*,AVISTREAMINFOA*);
STDAPI AVIFileWriteData(PAVIFILE,DWORD,LPVOID,LONG);
STDAPI AVIFileReadData(PAVIFILE,DWORD,LPVOID,LONG*);
STDAPI AVIFileEndRecord(PAVIFILE);
STDAPI_(ULONG) AVIStreamAddRef(PAVISTREAM);
STDAPI_(ULONG) AVIStreamRelease(PAVISTREAM);
STDAPI AVIStreamInfoW(PAVISTREAM,LPAVISTREAMINFOW,LONG);
STDAPI AVIStreamInfoA(PAVISTREAM,LPAVISTREAMINFOA,LONG);
STDAPI_(LONG) AVIStreamFindSample(PAVISTREAM,LONG,LONG);
STDAPI AVIStreamReadFormat(PAVISTREAM,LONG,LPVOID,LONG*);
STDAPI AVIStreamSetFormat(PAVISTREAM,LONG,LPVOID,LONG);
STDAPI AVIStreamReadData(PAVISTREAM,DWORD,LPVOID,LONG*);
STDAPI AVIStreamWriteData(PAVISTREAM,DWORD,LPVOID,LONG);
STDAPI AVIStreamRead(PAVISTREAM,LONG,LONG,LPVOID,LONG,LONG*,LONG*);
STDAPI AVIStreamWrite(PAVISTREAM,LONG,LONG,LPVOID,LONG,DWORD,LONG*,LONG*);
STDAPI_(LONG) AVIStreamStart(PAVISTREAM);
STDAPI_(LONG) AVIStreamLength(PAVISTREAM);
STDAPI_(LONG) AVIStreamTimeToSample(PAVISTREAM,LONG);
STDAPI_(LONG) AVIStreamSampleToTime(PAVISTREAM,LONG);
STDAPI AVIStreamBeginStreaming(PAVISTREAM,LONG,LONG,LONG);
STDAPI AVIStreamEndStreaming(PAVISTREAM);
STDAPI_(PGETFRAME) AVIStreamGetFrameOpen(PAVISTREAM,LPBITMAPINFOHEADER);
STDAPI_(LPVOID) AVIStreamGetFrame(PGETFRAME,LONG);
STDAPI AVIStreamGetFrameClose(PGETFRAME);
STDAPI AVIStreamOpenFromFileA(PAVISTREAM*,LPCSTR,DWORD,LONG,UINT,CLSID*);
STDAPI AVIStreamOpenFromFileW(PAVISTREAM*,LPCWSTR,DWORD,LONG,UINT,CLSID*);
STDAPI AVIStreamCreate(PAVISTREAM*,LONG,LONG,CLSID*);
STDAPI AVIMakeCompressedStream(PAVISTREAM*,PAVISTREAM,AVICOMPRESSOPTIONS*,CLSID*);
EXTERN_C HRESULT CDECL AVISaveA(LPCSTR,CLSID*,AVISAVECALLBACK,int,PAVISTREAM,LPAVICOMPRESSOPTIONS,...);
STDAPI AVISaveVA(LPCSTR,CLSID*,AVISAVECALLBACK,int,PAVISTREAM*,LPAVICOMPRESSOPTIONS*);
EXTERN_C HRESULT CDECL AVISaveW(LPCWSTR,CLSID*,AVISAVECALLBACK,int,PAVISTREAM,LPAVICOMPRESSOPTIONS,...);
STDAPI AVISaveVW(LPCWSTR,CLSID*,AVISAVECALLBACK,int,PAVISTREAM*,LPAVICOMPRESSOPTIONS*);
STDAPI_(INT_PTR) AVISaveOptions(HWND,UINT,int,PAVISTREAM*,LPAVICOMPRESSOPTIONS*);
STDAPI AVISaveOptionsFree(int,LPAVICOMPRESSOPTIONS*);
STDAPI AVIBuildFilterW(LPWSTR,LONG,BOOL);
STDAPI AVIBuildFilterA(LPSTR,LONG,BOOL);
STDAPI AVIMakeFileFromStreams(PAVIFILE*,int,PAVISTREAM*);
STDAPI AVIMakeStreamFromClipboard(UINT,HANDLE,PAVISTREAM*);
STDAPI AVIPutFileOnClipboard(PAVIFILE);
STDAPI AVIGetFromClipboard(PAVIFILE*);
STDAPI AVIClearClipboard(void);
STDAPI CreateEditableStream(PAVISTREAM*,PAVISTREAM);
STDAPI EditStreamCut(PAVISTREAM,LONG*,LONG*,PAVISTREAM*);
STDAPI EditStreamCopy(PAVISTREAM,LONG*,LONG*,PAVISTREAM*);
STDAPI EditStreamPaste(PAVISTREAM,LONG*,LONG*,PAVISTREAM,LONG,LONG);
STDAPI EditStreamClone(PAVISTREAM,PAVISTREAM*);
STDAPI EditStreamSetNameA(PAVISTREAM,LPCSTR);
STDAPI EditStreamSetNameW(PAVISTREAM,LPCWSTR);
STDAPI EditStreamSetInfoW(PAVISTREAM,LPAVISTREAMINFOW,LONG);
STDAPI EditStreamSetInfoA(PAVISTREAM,LPAVISTREAMINFOA,LONG);

#ifdef UNICODE
#define AVIFileOpen AVIFileOpenW
#define AVIFileInfo AVIFileInfoW
#define AVIFileCreateStream AVIFileCreateStreamW
#define AVIStreamInfo AVIStreamInfoW
#define AVIStreamOpenFromFile AVIStreamOpenFromFileW
#define AVISave AVISaveW
#define AVISaveV AVISaveVW
#define AVIBuildFilter AVIBuildFilterW
#define EditStreamSetInfo EditStreamSetInfoW
#define EditStreamSetName EditStreamSetNameW
#else
#define AVIFileOpen AVIFileOpenA
#define AVIFileInfo AVIFileInfoA
#define AVIFileCreateStream AVIFileCreateStreamA
#define AVIStreamInfo AVIStreamInfoA
#define AVIStreamOpenFromFile AVIStreamOpenFromFileA
#define AVISave AVISaveA
#define AVISaveV AVISaveVA
#define AVIBuildFilter AVIBuildFilterA
#define EditStreamSetInfo EditStreamSetInfoA
#define EditStreamSetName EditStreamSetNameA
#endif /* UNICODE */

#define AVIStreamFindKeyFrame  AVIStreamFindSample
#define FindKeyFrame  FindSample

#define AVIStreamClose AVIStreamRelease
#define AVIFileClose  AVIFileRelease
#define AVIStreamInit  AVIFileInit
#define AVIStreamExit  AVIFileExit

#define AVIStreamSampleToSample(pavi1,pavi2,l)  AVIStreamTimeToSample(pavi1,AVIStreamSampleToTime(pavi2,l))
#define AVIStreamNextSample(pavi,l)  AVIStreamFindSample(pavi,l+1,FIND_NEXT|FIND_ANY)
#define AVIStreamPrevSample(pavi,l)  AVIStreamFindSample(pavi,l-1,FIND_PREV|FIND_ANY)
#define AVIStreamNearestSample(pavi,l)  AVIStreamFindSample(pavi,l,FIND_PREV|FIND_ANY)
#define AVIStreamNextKeyFrame(pavi,l)  AVIStreamFindSample(pavi,l+1,FIND_NEXT|FIND_KEY)
#define AVIStreamPrevKeyFrame(pavi,l)  AVIStreamFindSample(pavi,l-1,FIND_PREV|FIND_KEY)
#define AVIStreamNearestKeyFrame(pavi,l)  AVIStreamFindSample(pavi,l,FIND_PREV|FIND_KEY)
#define AVIStreamIsKeyFrame(pavi,l)  (AVIStreamNearestKeyFrame(pavi,l) == l)
#define AVIStreamPrevSampleTime(pavi,t)  AVIStreamSampleToTime(pavi,AVIStreamPrevSample(pavi,AVIStreamTimeToSample(pavi,t)))
#define AVIStreamNextSampleTime(pavi,t)  AVIStreamSampleToTime(pavi,AVIStreamNextSample(pavi,AVIStreamTimeToSample(pavi,t)))
#define AVIStreamNearestSampleTime(pavi,t)  AVIStreamSampleToTime(pavi,AVIStreamNearestSample(pavi,AVIStreamTimeToSample(pavi,t)))
#define AVIStreamNextKeyFrameTime(pavi,t)  AVIStreamSampleToTime(pavi,AVIStreamNextKeyFrame(pavi,AVIStreamTimeToSample(pavi,t)))
#define AVIStreamPrevKeyFrameTime(pavi,t)  AVIStreamSampleToTime(pavi,AVIStreamPrevKeyFrame(pavi,AVIStreamTimeToSample(pavi,t)))
#define AVIStreamNearestKeyFrameTime(pavi,t)  AVIStreamSampleToTime(pavi,AVIStreamNearestKeyFrame(pavi,AVIStreamTimeToSample(pavi,t)))
#define AVIStreamStartTime(pavi)  AVIStreamSampleToTime(pavi,AVIStreamStart(pavi))
#define AVIStreamLengthTime(pavi)  AVIStreamSampleToTime(pavi,AVIStreamLength(pavi))
#define AVIStreamEnd(pavi)  (AVIStreamStart(pavi)+AVIStreamLength(pavi))
#define AVIStreamEndTime(pavi)  AVIStreamSampleToTime(pavi,AVIStreamEnd(pavi))
#define AVIStreamSampleSize(pavi,lPos,plSize)  AVIStreamRead(pavi,lPos,1,NULL,0,plSize,NULL)
#define AVIStreamFormatSize(pavi,lPos,plSize)  AVIStreamReadFormat(pavi,lPos,NULL,plSize)
#define AVIStreamDataSize(pavi,fcc,plSize)  AVIStreamReadData(pavi,fcc,NULL,plSize)

#ifndef comptypeDIB
#define comptypeDIB  mmioFOURCC('D','I','B',' ')
#endif /* comptypeDIB */

#ifdef __cplusplus
#define MCIWndSM ::SendMessage
#define AVICapSM(hwnd,m,w,l)  ((::IsWindow(hwnd)) ? ::SendMessage(hwnd,m,w,l) : 0)
#else
#define MCIWndSM SendMessage
#define AVICapSM(hwnd,m,w,l)  ((IsWindow(hwnd)) ? SendMessage(hwnd,m,w,l) : 0)
#endif /* __cplusplus */

#define MCIWNDOPENF_NEW  0x0001

#define MCIWNDF_NOAUTOSIZEWINDOW  0x0001
#define MCIWNDF_NOPLAYBAR  0x0002
#define MCIWNDF_NOAUTOSIZEMOVIE  0x0004
#define MCIWNDF_NOMENU  0x0008
#define MCIWNDF_SHOWNAME  0x0010
#define MCIWNDF_SHOWPOS  0x0020
#define MCIWNDF_SHOWMODE  0x0040
#define MCIWNDF_SHOWALL  0x0070
#define MCIWNDF_NOTIFYMODE  0x0100
#define MCIWNDF_NOTIFYPOS  0x0200
#define MCIWNDF_NOTIFYSIZE  0x0400
#define MCIWNDF_NOTIFYERROR  0x1000
#define MCIWNDF_NOTIFYALL  0x1F00
#define MCIWNDF_NOTIFYANSI  0x0080
#define MCIWNDF_NOTIFYMEDIAA  0x0880
#define MCIWNDF_NOTIFYMEDIAW  0x0800
#define MCIWNDF_RECORD  0x2000
#define MCIWNDF_NOERRORDLG  0x4000
#define MCIWNDF_NOOPEN  0x8000

#define MCIWNDM_GETDEVICEID  (WM_USER+100)
#define MCIWNDM_GETSTART  (WM_USER+103)
#define MCIWNDM_GETLENGTH  (WM_USER+104)
#define MCIWNDM_GETEND  (WM_USER+105)
#define MCIWNDM_EJECT  (WM_USER+107)
#define MCIWNDM_SETZOOM  (WM_USER+108)
#define MCIWNDM_GETZOOM  (WM_USER+109)
#define MCIWNDM_SETVOLUME  (WM_USER+110)
#define MCIWNDM_GETVOLUME  (WM_USER+111)
#define MCIWNDM_SETSPEED  (WM_USER+112)
#define MCIWNDM_GETSPEED  (WM_USER+113)
#define MCIWNDM_SETREPEAT  (WM_USER+114)
#define MCIWNDM_GETREPEAT  (WM_USER+115)
#define MCIWNDM_REALIZE  (WM_USER+118)
#define MCIWNDM_VALIDATEMEDIA  (WM_USER+121)
#define MCIWNDM_PLAYFROM  (WM_USER+122)
#define MCIWNDM_PLAYTO  (WM_USER+123)
#define MCIWNDM_GETPALETTE  (WM_USER+126)
#define MCIWNDM_SETPALETTE  (WM_USER+127)
#define MCIWNDM_SETTIMERS  (WM_USER+129)
#define MCIWNDM_SETACTIVETIMER  (WM_USER+130)
#define MCIWNDM_SETINACTIVETIMER (WM_USER+131)
#define MCIWNDM_GETACTIVETIMER  (WM_USER+132)
#define MCIWNDM_GETINACTIVETIMER (WM_USER+133)
#define MCIWNDM_CHANGESTYLES  (WM_USER+135)
#define MCIWNDM_GETSTYLES  (WM_USER+136)
#define MCIWNDM_GETALIAS  (WM_USER+137)
#define MCIWNDM_PLAYREVERSE  (WM_USER+139)
#define MCIWNDM_GET_SOURCE  (WM_USER+140)
#define MCIWNDM_PUT_SOURCE  (WM_USER+141)
#define MCIWNDM_GET_DEST  (WM_USER+142)
#define MCIWNDM_PUT_DEST  (WM_USER+143)
#define MCIWNDM_CAN_PLAY  (WM_USER+144)
#define MCIWNDM_CAN_WINDOW  (WM_USER+145)
#define MCIWNDM_CAN_RECORD  (WM_USER+146)
#define MCIWNDM_CAN_SAVE  (WM_USER+147)
#define MCIWNDM_CAN_EJECT  (WM_USER+148)
#define MCIWNDM_CAN_CONFIG  (WM_USER+149)
#define MCIWNDM_PALETTEKICK  (WM_USER+150)
#define MCIWNDM_OPENINTERFACE  (WM_USER+151)
#define MCIWNDM_SETOWNER  (WM_USER+152)

#define MCIWNDM_SENDSTRINGA  (WM_USER+101)
#define MCIWNDM_GETPOSITIONA  (WM_USER+102)
#define MCIWNDM_GETMODEA  (WM_USER+106)
#define MCIWNDM_SETTIMEFORMATA  (WM_USER+119)
#define MCIWNDM_GETTIMEFORMATA  (WM_USER+120)
#define MCIWNDM_GETFILENAMEA  (WM_USER+124)
#define MCIWNDM_GETDEVICEA  (WM_USER+125)
#define MCIWNDM_GETERRORA  (WM_USER+128)
#define MCIWNDM_NEWA  (WM_USER+134)
#define MCIWNDM_RETURNSTRINGA  (WM_USER+138)
#define MCIWNDM_OPENA  (WM_USER+153)

#define MCIWNDM_SENDSTRINGW  (WM_USER+201)
#define MCIWNDM_GETPOSITIONW  (WM_USER+202)
#define MCIWNDM_GETMODEW  (WM_USER+206)
#define MCIWNDM_SETTIMEFORMATW  (WM_USER+219)
#define MCIWNDM_GETTIMEFORMATW  (WM_USER+220)
#define MCIWNDM_GETFILENAMEW  (WM_USER+224)
#define MCIWNDM_GETDEVICEW  (WM_USER+225)
#define MCIWNDM_GETERRORW  (WM_USER+228)
#define MCIWNDM_NEWW  (WM_USER+234)
#define MCIWNDM_RETURNSTRINGW  (WM_USER+238)
#define MCIWNDM_OPENW  (WM_USER+252)

#define MCIWNDM_NOTIFYMODE  (WM_USER+200)
#define MCIWNDM_NOTIFYPOS  (WM_USER+201)
#define MCIWNDM_NOTIFYSIZE  (WM_USER+202)
#define MCIWNDM_NOTIFYMEDIA  (WM_USER+203)
#define MCIWNDM_NOTIFYERROR  (WM_USER+205)

#define MCIWND_START  -1
#define MCIWND_END  -2

#ifndef MCI_PLAY
#define MCI_CLOSE  0x0804
#define MCI_PLAY  0x0806
#define MCI_SEEK  0x0807
#define MCI_STOP  0x0808
#define MCI_PAUSE  0x0809
#define MCI_STEP  0x080E
#define MCI_RECORD  0x080F
#define MCI_SAVE  0x0813
#define MCI_CUT  0x0851
#define MCI_COPY  0x0852
#define MCI_PASTE  0x0853
#define MCI_RESUME  0x0855
#define MCI_DELETE  0x0856
#endif /* MCI_PLAY */

#ifndef MCI_MODE_NOT_READY
#define MCI_MODE_NOT_READY  (524)
#define MCI_MODE_STOP  (525)
#define MCI_MODE_PLAY  (526)
#define MCI_MODE_RECORD  (527)
#define MCI_MODE_SEEK  (528)
#define MCI_MODE_PAUSE  (529)
#define MCI_MODE_OPEN  (530)
#endif /* MCI_MODE_NOT_READY */

#define MCIWND_WINDOW_CLASS TEXT("MCIWndClass")

#define DV_ERR_OK  0
#define DV_ERR_BASE  1
#define DV_ERR_NONSPECIFIC  (DV_ERR_BASE)
#define DV_ERR_BADFORMAT  (DV_ERR_BASE+1)
#define DV_ERR_STILLPLAYING  (DV_ERR_BASE+2)
#define DV_ERR_UNPREPARED  (DV_ERR_BASE+3)
#define DV_ERR_SYNC  (DV_ERR_BASE+4)
#define DV_ERR_TOOMANYCHANNELS  (DV_ERR_BASE+5)
#define DV_ERR_NOTDETECTED  (DV_ERR_BASE+6)
#define DV_ERR_BADINSTALL  (DV_ERR_BASE+7)
#define DV_ERR_CREATEPALETTE  (DV_ERR_BASE+8)
#define DV_ERR_SIZEFIELD  (DV_ERR_BASE+9)
#define DV_ERR_PARAM1  (DV_ERR_BASE+10)
#define DV_ERR_PARAM2  (DV_ERR_BASE+11)
#define DV_ERR_CONFIG1  (DV_ERR_BASE+12)
#define DV_ERR_CONFIG2  (DV_ERR_BASE+13)
#define DV_ERR_FLAGS  (DV_ERR_BASE+14)
#define DV_ERR_13  (DV_ERR_BASE+15)
#define DV_ERR_NOTSUPPORTED  (DV_ERR_BASE+16)
#define DV_ERR_NOMEM  (DV_ERR_BASE+17)
#define DV_ERR_ALLOCATED  (DV_ERR_BASE+18)
#define DV_ERR_BADDEVICEID  (DV_ERR_BASE+19)
#define DV_ERR_INVALHANDLE  (DV_ERR_BASE+20)
#define DV_ERR_BADERRNUM  (DV_ERR_BASE+21)
#define DV_ERR_NO_BUFFERS  (DV_ERR_BASE+22)
#define DV_ERR_MEM_CONFLICT  (DV_ERR_BASE+23)
#define DV_ERR_IO_CONFLICT  (DV_ERR_BASE+24)
#define DV_ERR_DMA_CONFLICT  (DV_ERR_BASE+25)
#define DV_ERR_INT_CONFLICT  (DV_ERR_BASE+26)
#define DV_ERR_PROTECT_ONLY  (DV_ERR_BASE+27)
#define DV_ERR_LASTERROR  (DV_ERR_BASE+27)
#define DV_ERR_USER_MSG  (DV_ERR_BASE+1000)

#ifndef MM_DRVM_OPEN
#define MM_DRVM_OPEN  0x3D0
#define MM_DRVM_CLOSE  0x3D1
#define MM_DRVM_DATA  0x3D2
#define MM_DRVM_ERROR  0x3D3
#endif /* MM_DRVM_OPEN */

#define DV_VM_OPEN  MM_DRVM_OPEN
#define DV_VM_CLOSE  MM_DRVM_CLOSE
#define DV_VM_DATA  MM_DRVM_DATA
#define DV_VM_ERROR  MM_DRVM_ERROR

#define VHDR_DONE  0x00000001
#define VHDR_PREPARED  0x00000002
#define VHDR_INQUEUE  0x00000004
#define VHDR_KEYFRAME  0x00000008
#define VHDR_VALID  0x0000000F

#define VCAPS_OVERLAY  0x00000001
#define VCAPS_SRC_CAN_CLIP  0x00000002
#define VCAPS_DST_CAN_CLIP  0x00000004
#define VCAPS_CAN_SCALE  0x00000008

#define VIDEO_EXTERNALIN  0x0001
#define VIDEO_EXTERNALOUT  0x0002
#define VIDEO_IN  0x0004
#define VIDEO_OUT  0x0008

#define VIDEO_DLG_QUERY  0x0010

#define VIDEO_CONFIGURE_QUERY  0x8000
#define VIDEO_CONFIGURE_SET  0x1000
#define VIDEO_CONFIGURE_GET  0x2000
#define VIDEO_CONFIGURE_QUERYSIZE  0x0001
#define VIDEO_CONFIGURE_CURRENT  0x0010
#define VIDEO_CONFIGURE_NOMINAL  0x0020
#define VIDEO_CONFIGURE_MIN  0x0040
#define VIDEO_CONFIGURE_MAX  0x0080

#define DVM_USER  0X4000

#define DVM_CONFIGURE_START  0x1000
#define DVM_CONFIGURE_END  0x1FFF

#define DVM_PALETTE  (DVM_CONFIGURE_START+1)
#define DVM_FORMAT  (DVM_CONFIGURE_START+2)
#define DVM_PALETTERGB555  (DVM_CONFIGURE_START+3)
#define DVM_SRC_RECT  (DVM_CONFIGURE_START+4)
#define DVM_DST_RECT  (DVM_CONFIGURE_START+5)

#define WM_CAP_START  WM_USER
#define WM_CAP_UNICODE_START  (WM_USER+100)
#define WM_CAP_GET_CAPSTREAMPTR  (WM_CAP_START+1)
#define WM_CAP_SET_CALLBACK_ERRORW  (WM_CAP_UNICODE_START+2)
#define WM_CAP_SET_CALLBACK_STATUSW  (WM_CAP_UNICODE_START+3)
#define WM_CAP_SET_CALLBACK_ERRORA  (WM_CAP_START+2)
#define WM_CAP_SET_CALLBACK_STATUSA  (WM_CAP_START+3)
#define WM_CAP_SET_CALLBACK_YIELD  (WM_CAP_START+4)
#define WM_CAP_SET_CALLBACK_FRAME  (WM_CAP_START+5)
#define WM_CAP_SET_CALLBACK_VIDEOSTREAM (WM_CAP_START+6)
#define WM_CAP_SET_CALLBACK_WAVESTREAM  (WM_CAP_START+7)
#define WM_CAP_GET_USER_DATA  (WM_CAP_START+8)
#define WM_CAP_SET_USER_DATA  (WM_CAP_START+9)
#define WM_CAP_DRIVER_CONNECT  (WM_CAP_START+10)
#define WM_CAP_DRIVER_DISCONNECT  (WM_CAP_START+11)
#define WM_CAP_DRIVER_GET_NAMEA  (WM_CAP_START+12)
#define WM_CAP_DRIVER_GET_VERSIONA  (WM_CAP_START+13)
#define WM_CAP_DRIVER_GET_NAMEW  (WM_CAP_UNICODE_START+12)
#define WM_CAP_DRIVER_GET_VERSIONW  (WM_CAP_UNICODE_START+13)
#define WM_CAP_DRIVER_GET_CAPS  (WM_CAP_START+14)
#define WM_CAP_FILE_SET_CAPTURE_FILEA  (WM_CAP_START+20)
#define WM_CAP_FILE_GET_CAPTURE_FILEA  (WM_CAP_START+21)
#define WM_CAP_FILE_SAVEASA  (WM_CAP_START+23)
#define WM_CAP_FILE_SAVEDIBA  (WM_CAP_START+25)
#define WM_CAP_FILE_SET_CAPTURE_FILEW  (WM_CAP_UNICODE_START+20)
#define WM_CAP_FILE_GET_CAPTURE_FILEW  (WM_CAP_UNICODE_START+21)
#define WM_CAP_FILE_SAVEASW  (WM_CAP_UNICODE_START+23)
#define WM_CAP_FILE_SAVEDIBW  (WM_CAP_UNICODE_START+25)
#define WM_CAP_FILE_ALLOCATE  (WM_CAP_START+22)
#define WM_CAP_FILE_SET_INFOCHUNK  (WM_CAP_START+24)
#define WM_CAP_EDIT_COPY  (WM_CAP_START+30)
#define WM_CAP_SET_AUDIOFORMAT  (WM_CAP_START+35)
#define WM_CAP_GET_AUDIOFORMAT  (WM_CAP_START+36)
#define WM_CAP_DLG_VIDEOFORMAT  (WM_CAP_START+41)
#define WM_CAP_DLG_VIDEOSOURCE  (WM_CAP_START+42)
#define WM_CAP_DLG_VIDEODISPLAY  (WM_CAP_START+43)
#define WM_CAP_GET_VIDEOFORMAT  (WM_CAP_START+44)
#define WM_CAP_SET_VIDEOFORMAT  (WM_CAP_START+45)
#define WM_CAP_DLG_VIDEOCOMPRESSION  (WM_CAP_START+46)
#define WM_CAP_SET_PREVIEW  (WM_CAP_START+50)
#define WM_CAP_SET_OVERLAY  (WM_CAP_START+51)
#define WM_CAP_SET_PREVIEWRATE  (WM_CAP_START+52)
#define WM_CAP_SET_SCALE  (WM_CAP_START+53)
#define WM_CAP_GET_STATUS  (WM_CAP_START+54)
#define WM_CAP_SET_SCROLL  (WM_CAP_START+55)
#define WM_CAP_GRAB_FRAME  (WM_CAP_START+60)
#define WM_CAP_GRAB_FRAME_NOSTOP  (WM_CAP_START+61)
#define WM_CAP_SEQUENCE  (WM_CAP_START+62)
#define WM_CAP_SEQUENCE_NOFILE  (WM_CAP_START+63)
#define WM_CAP_SET_SEQUENCE_SETUP  (WM_CAP_START+64)
#define WM_CAP_GET_SEQUENCE_SETUP  (WM_CAP_START+65)
#define WM_CAP_SET_MCI_DEVICEA  (WM_CAP_START+66)
#define WM_CAP_GET_MCI_DEVICEA  (WM_CAP_START+67)
#define WM_CAP_SET_MCI_DEVICEW  (WM_CAP_UNICODE_START+66)
#define WM_CAP_GET_MCI_DEVICEW  (WM_CAP_UNICODE_START+67)
#define WM_CAP_STOP  (WM_CAP_START+68)
#define WM_CAP_ABORT  (WM_CAP_START+69)
#define WM_CAP_SINGLE_FRAME_OPEN  (WM_CAP_START+70)
#define WM_CAP_SINGLE_FRAME_CLOSE  (WM_CAP_START+71)
#define WM_CAP_SINGLE_FRAME  (WM_CAP_START+72)
#define WM_CAP_PAL_OPENA  (WM_CAP_START+80)
#define WM_CAP_PAL_SAVEA  (WM_CAP_START+81)
#define WM_CAP_PAL_OPENW  (WM_CAP_UNICODE_START+80)
#define WM_CAP_PAL_SAVEW  (WM_CAP_UNICODE_START+81)
#define WM_CAP_PAL_PASTE  (WM_CAP_START+82)
#define WM_CAP_PAL_AUTOCREATE  (WM_CAP_START+83)
#define WM_CAP_PAL_MANUALCREATE  (WM_CAP_START+84)
#define WM_CAP_SET_CALLBACK_CAPCONTROL  (WM_CAP_START+85)
#define WM_CAP_UNICODE_END  WM_CAP_PAL_SAVEW
#define WM_CAP_END  WM_CAP_UNICODE_END

#define AVSTREAMMASTER_AUDIO  0
#define AVSTREAMMASTER_NONE  1

#define CONTROLCALLBACK_PREROLL  1
#define CONTROLCALLBACK_CAPTURING  2

#define infotypeDIGITIZATION_TIME  mmioFOURCC ('I','D','I','T')
#define infotypeSMPTE_TIME  mmioFOURCC ('I','S','M','P')

#define IDS_CAP_BEGIN  300
#define IDS_CAP_END  301
#define IDS_CAP_INFO  401
#define IDS_CAP_OUTOFMEM  402
#define IDS_CAP_FILEEXISTS  403
#define IDS_CAP_ERRORPALOPEN  404
#define IDS_CAP_ERRORPALSAVE  405
#define IDS_CAP_ERRORDIBSAVE  406
#define IDS_CAP_DEFAVIEXT  407
#define IDS_CAP_DEFPALEXT  408
#define IDS_CAP_CANTOPEN  409
#define IDS_CAP_SEQ_MSGSTART  410
#define IDS_CAP_SEQ_MSGSTOP  411
#define IDS_CAP_VIDEDITERR  412
#define IDS_CAP_READONLYFILE  413
#define IDS_CAP_WRITEERROR  414
#define IDS_CAP_NODISKSPACE  415
#define IDS_CAP_SETFILESIZE  416
#define IDS_CAP_SAVEASPERCENT  417
#define IDS_CAP_DRIVER_ERROR  418
#define IDS_CAP_WAVE_OPEN_ERROR  419
#define IDS_CAP_WAVE_ALLOC_ERROR  420
#define IDS_CAP_WAVE_PREPARE_ERROR  421
#define IDS_CAP_WAVE_ADD_ERROR  422
#define IDS_CAP_WAVE_SIZE_ERROR  423
#define IDS_CAP_VIDEO_OPEN_ERROR  424
#define IDS_CAP_VIDEO_ALLOC_ERROR  425
#define IDS_CAP_VIDEO_PREPARE_ERROR 426
#define IDS_CAP_VIDEO_ADD_ERROR  427
#define IDS_CAP_VIDEO_SIZE_ERROR  428
#define IDS_CAP_FILE_OPEN_ERROR  429
#define IDS_CAP_FILE_WRITE_ERROR  430
#define IDS_CAP_RECORDING_ERROR  431
#define IDS_CAP_RECORDING_ERROR2  432
#define IDS_CAP_AVI_INIT_ERROR  433
#define IDS_CAP_NO_FRAME_CAP_ERROR  434
#define IDS_CAP_NO_PALETTE_WARN  435
#define IDS_CAP_MCI_CONTROL_ERROR  436
#define IDS_CAP_MCI_CANT_STEP_ERROR 437
#define IDS_CAP_NO_AUDIO_CAP_ERROR  438
#define IDS_CAP_AVI_DRAWDIB_ERROR  439
#define IDS_CAP_COMPRESSOR_ERROR  440
#define IDS_CAP_AUDIO_DROP_ERROR  441
#define IDS_CAP_AUDIO_DROP_COMPERROR 442
#define IDS_CAP_STAT_LIVE_MODE  500
#define IDS_CAP_STAT_OVERLAY_MODE  501
#define IDS_CAP_STAT_CAP_INIT  502
#define IDS_CAP_STAT_CAP_FINI  503
#define IDS_CAP_STAT_PALETTE_BUILD  504
#define IDS_CAP_STAT_OPTPAL_BUILD  505
#define IDS_CAP_STAT_I_FRAMES  506
#define IDS_CAP_STAT_L_FRAMES  507
#define IDS_CAP_STAT_CAP_L_FRAMES  508
#define IDS_CAP_STAT_CAP_AUDIO  509
#define IDS_CAP_STAT_VIDEOCURRENT  510
#define IDS_CAP_STAT_VIDEOAUDIO  511
#define IDS_CAP_STAT_VIDEOONLY  512
#define IDS_CAP_STAT_FRAMESDROPPED  513

DECLARE_HANDLE(HVIDEO);
typedef HVIDEO *LPHVIDEO;

typedef struct videohdr_tag {
    LPBYTE lpData;
    DWORD dwBufferLength;
    DWORD dwBytesUsed;
    DWORD dwTimeCaptured;
    DWORD dwUser;
    DWORD dwFlags;
    DWORD_PTR dwReserved[4];
} VIDEOHDR,*PVIDEOHDR,*LPVIDEOHDR;

typedef struct channel_caps_tag {
    DWORD dwFlags;
    DWORD dwSrcRectXMod;
    DWORD dwSrcRectYMod;
    DWORD dwSrcRectWidthMod;
    DWORD dwSrcRectHeightMod;
    DWORD dwDstRectXMod;
    DWORD dwDstRectYMod;
    DWORD dwDstRectWidthMod;
    DWORD dwDstRectHeightMod;
} CHANNEL_CAPS,*PCHANNEL_CAPS,*LPCHANNEL_CAPS;

typedef struct tagCapDriverCaps {
    UINT wDeviceIndex;
    BOOL fHasOverlay;
    BOOL fHasDlgVideoSource;
    BOOL fHasDlgVideoFormat;
    BOOL fHasDlgVideoDisplay;
    BOOL fCaptureInitialized;
    BOOL fDriverSuppliesPalettes;
    HANDLE hVideoIn;
    HANDLE hVideoOut;
    HANDLE hVideoExtIn;
    HANDLE hVideoExtOut;
} CAPDRIVERCAPS,*PCAPDRIVERCAPS, *LPCAPDRIVERCAPS;

typedef struct tagCapStatus {
    UINT uiImageWidth;
    UINT uiImageHeight;
    BOOL fLiveWindow;
    BOOL fOverlayWindow;
    BOOL fScale;
    POINT ptScroll;
    BOOL fUsingDefaultPalette;
    BOOL fAudioHardware;
    BOOL fCapFileExists;
    DWORD dwCurrentVideoFrame;
    DWORD dwCurrentVideoFramesDropped;
    DWORD dwCurrentWaveSamples;
    DWORD dwCurrentTimeElapsedMS;
    HPALETTE hPalCurrent;
    BOOL fCapturingNow;
    DWORD dwReturn;
    UINT wNumVideoAllocated;
    UINT wNumAudioAllocated;
} CAPSTATUS,*PCAPSTATUS, *LPCAPSTATUS;

typedef struct tagCaptureParms {
    DWORD dwRequestMicroSecPerFrame;
    BOOL fMakeUserHitOKToCapture;
    UINT wPercentDropForError;
    BOOL fYield;
    DWORD dwIndexSize;
    UINT wChunkGranularity;
    BOOL fUsingDOSMemory;
    UINT wNumVideoRequested;
    BOOL fCaptureAudio;
    UINT wNumAudioRequested;
    UINT vKeyAbort;
    BOOL fAbortLeftMouse;
    BOOL fAbortRightMouse;
    BOOL fLimitEnabled;
    UINT wTimeLimit;
    BOOL fMCIControl;
    BOOL fStepMCIDevice;
    DWORD dwMCIStartTime;
    DWORD dwMCIStopTime;
    BOOL fStepCaptureAt2x;
    UINT wStepCaptureAverageFrames;
    DWORD dwAudioBufferSize;
    BOOL fDisableWriteCache;
    UINT AVStreamMaster;
} CAPTUREPARMS, *PCAPTUREPARMS, *LPCAPTUREPARMS;

typedef struct tagCapInfoChunk {
    FOURCC fccInfoID;
    LPVOID lpData;
    LONG cbData;
} CAPINFOCHUNK, *PCAPINFOCHUNK, *LPCAPINFOCHUNK;

typedef LRESULT(CALLBACK *CAPYIELDCALLBACK)(HWND);
typedef LRESULT(CALLBACK *CAPSTATUSCALLBACKW)(HWND,int,LPCWSTR);
typedef LRESULT(CALLBACK *CAPERRORCALLBACKW)(HWND,int,LPCWSTR);
typedef LRESULT(CALLBACK *CAPSTATUSCALLBACKA)(HWND,int,LPCSTR);
typedef LRESULT(CALLBACK *CAPERRORCALLBACKA)(HWND,int,LPCSTR);
typedef LRESULT(CALLBACK *CAPVIDEOCALLBACK)(HWND,LPVIDEOHDR);
typedef LRESULT(CALLBACK *CAPWAVECALLBACK)(HWND,LPWAVEHDR);
typedef LRESULT(CALLBACK *CAPCONTROLCALLBACK)(HWND,int);

HWND VFWAPIV MCIWndCreateA(HWND,HINSTANCE,DWORD,LPCSTR);
HWND VFWAPIV MCIWndCreateW(HWND,HINSTANCE,DWORD,LPCWSTR);
BOOL VFWAPIV MCIWndRegisterClass(void);
DWORD PASCAL VideoForWindowsVersion(void);
HWND VFWAPI capCreateCaptureWindowA(LPCSTR,DWORD,int,int,int,int,HWND,int);
BOOL VFWAPI capGetDriverDescriptionA(UINT,LPSTR,int,LPSTR,int);
HWND VFWAPI capCreateCaptureWindowW(LPCWSTR,DWORD,int,int,int,int,HWND,int);
BOOL VFWAPI capGetDriverDescriptionW(UINT,LPWSTR,int,LPWSTR,int);

#ifdef UNICODE
#define MCIWNDF_NOTIFYMEDIA MCIWNDF_NOTIFYMEDIAW
#define MCIWNDM_SENDSTRING MCIWNDM_SENDSTRINGW
#define MCIWNDM_GETPOSITION MCIWNDM_GETPOSITIONW
#define MCIWNDM_GETMODE MCIWNDM_GETMODEW
#define MCIWNDM_SETTIMEFORMAT MCIWNDM_SETTIMEFORMATW
#define MCIWNDM_GETTIMEFORMAT MCIWNDM_GETTIMEFORMATW
#define MCIWNDM_GETFILENAME MCIWNDM_GETFILENAMEW
#define MCIWNDM_GETDEVICE MCIWNDM_GETDEVICEW
#define MCIWNDM_GETERROR MCIWNDM_GETERRORW
#define MCIWNDM_NEW MCIWNDM_NEWW
#define MCIWNDM_RETURNSTRING MCIWNDM_RETURNSTRINGW
#define MCIWNDM_OPEN MCIWNDM_OPENW
#define WM_CAP_SET_CALLBACK_ERROR WM_CAP_SET_CALLBACK_ERRORW
#define WM_CAP_SET_CALLBACK_STATUS WM_CAP_SET_CALLBACK_STATUSW
#define WM_CAP_DRIVER_GET_NAME WM_CAP_DRIVER_GET_NAMEW
#define WM_CAP_DRIVER_GET_VERSION WM_CAP_DRIVER_GET_VERSIONW
#define WM_CAP_FILE_SET_CAPTURE_FILE WM_CAP_FILE_SET_CAPTURE_FILEW
#define WM_CAP_FILE_GET_CAPTURE_FILE WM_CAP_FILE_GET_CAPTURE_FILEW
#define WM_CAP_FILE_SAVEAS WM_CAP_FILE_SAVEASW
#define WM_CAP_FILE_SAVEDIB WM_CAP_FILE_SAVEDIBW
#define WM_CAP_SET_MCI_DEVICE WM_CAP_SET_MCI_DEVICEW
#define WM_CAP_GET_MCI_DEVICE WM_CAP_GET_MCI_DEVICEW
#define WM_CAP_PAL_OPEN WM_CAP_PAL_OPENW
#define WM_CAP_PAL_SAVE WM_CAP_PAL_SAVEW
#define CAPSTATUSCALLBACK CAPSTATUSCALLBACKW
#define CAPERRORCALLBACK CAPERRORCALLBACKW
#define MCIWndCreate MCIWndCreateW
#define capCreateCaptureWindow capCreateCaptureWindowW
#define capGetDriverDescription capGetDriverDescriptionW
#else
#define MCIWNDF_NOTIFYMEDIA MCIWNDF_NOTIFYMEDIAA
#define MCIWNDM_SENDSTRING MCIWNDM_SENDSTRINGA
#define MCIWNDM_GETPOSITION MCIWNDM_GETPOSITIONA
#define MCIWNDM_GETMODE MCIWNDM_GETMODEA
#define MCIWNDM_SETTIMEFORMAT MCIWNDM_SETTIMEFORMATA
#define MCIWNDM_GETTIMEFORMAT MCIWNDM_GETTIMEFORMATA
#define MCIWNDM_GETFILENAME MCIWNDM_GETFILENAMEA
#define MCIWNDM_GETDEVICE MCIWNDM_GETDEVICEA
#define MCIWNDM_GETERROR MCIWNDM_GETERRORA
#define MCIWNDM_NEW MCIWNDM_NEWA
#define MCIWNDM_RETURNSTRING MCIWNDM_RETURNSTRINGA
#define MCIWNDM_OPEN MCIWNDM_OPENA
#define WM_CAP_SET_CALLBACK_ERROR WM_CAP_SET_CALLBACK_ERRORA
#define WM_CAP_SET_CALLBACK_STATUS WM_CAP_SET_CALLBACK_STATUSA
#define WM_CAP_DRIVER_GET_NAME WM_CAP_DRIVER_GET_NAMEA
#define WM_CAP_DRIVER_GET_VERSION WM_CAP_DRIVER_GET_VERSIONA
#define WM_CAP_FILE_SET_CAPTURE_FILE WM_CAP_FILE_SET_CAPTURE_FILEA
#define WM_CAP_FILE_GET_CAPTURE_FILE WM_CAP_FILE_GET_CAPTURE_FILEA
#define WM_CAP_FILE_SAVEAS WM_CAP_FILE_SAVEASA
#define WM_CAP_FILE_SAVEDIB WM_CAP_FILE_SAVEDIBA
#define WM_CAP_SET_MCI_DEVICE WM_CAP_SET_MCI_DEVICEA
#define WM_CAP_GET_MCI_DEVICE WM_CAP_GET_MCI_DEVICEA
#define WM_CAP_PAL_OPEN WM_CAP_PAL_OPENA
#define WM_CAP_PAL_SAVE WM_CAP_PAL_SAVEA
#define CAPSTATUSCALLBACK CAPSTATUSCALLBACKA
#define CAPERRORCALLBACK CAPERRORCALLBACKA
#define MCIWndCreate MCIWndCreateA
#define capCreateCaptureWindow capCreateCaptureWindowA
#define capGetDriverDescription capGetDriverDescriptionA
#endif

#define MCIWndCanPlay(hwnd)  (BOOL)MCIWndSM(hwnd,MCIWNDM_CAN_PLAY,0,0)
#define MCIWndCanRecord(hwnd)  (BOOL)MCIWndSM(hwnd,MCIWNDM_CAN_RECORD,0,0)
#define MCIWndCanSave(hwnd)  (BOOL)MCIWndSM(hwnd,MCIWNDM_CAN_SAVE,0,0)
#define MCIWndCanWindow(hwnd)  (BOOL)MCIWndSM(hwnd,MCIWNDM_CAN_WINDOW,0,0)
#define MCIWndCanEject(hwnd)  (BOOL)MCIWndSM(hwnd,MCIWNDM_CAN_EJECT,0,0)
#define MCIWndCanConfig(hwnd)  (BOOL)MCIWndSM(hwnd,MCIWNDM_CAN_CONFIG,0,0)
#define MCIWndPaletteKick(hwnd)  (BOOL)MCIWndSM(hwnd,MCIWNDM_PALETTEKICK,0,0)
#define MCIWndSave(hwnd,szFile)  (LONG)MCIWndSM(hwnd,MCI_SAVE,0,(LPARAM)(LPVOID)(szFile))
#define MCIWndSaveDialog(hwnd)  MCIWndSave(hwnd,-1)
#define MCIWndNew(hwnd,lp)  (LONG)MCIWndSM(hwnd,MCIWNDM_NEW,0,(LPARAM)(LPVOID)(lp))
#define MCIWndRecord(hwnd)  (LONG)MCIWndSM(hwnd,MCI_RECORD,0,0)
#define MCIWndOpen(hwnd,sz,f)  (LONG)MCIWndSM(hwnd,MCIWNDM_OPEN,(WPARAM)(UINT)(f),(LPARAM)(LPVOID)(sz))
#define MCIWndOpenDialog(hwnd)  MCIWndOpen(hwnd,-1,0)
#define MCIWndClose(hwnd)  (LONG)MCIWndSM(hwnd,MCI_CLOSE,0,0)
#define MCIWndPlay(hwnd)  (LONG)MCIWndSM(hwnd,MCI_PLAY,0,0)
#define MCIWndStop(hwnd)  (LONG)MCIWndSM(hwnd,MCI_STOP,0,0)
#define MCIWndPause(hwnd)  (LONG)MCIWndSM(hwnd,MCI_PAUSE,0,0)
#define MCIWndResume(hwnd)  (LONG)MCIWndSM(hwnd,MCI_RESUME,0,0)
#define MCIWndSeek(hwnd,lPos)  (LONG)MCIWndSM(hwnd,MCI_SEEK,0,(LPARAM)(LONG)(lPos))
#define MCIWndEject(hwnd)  (LONG)MCIWndSM(hwnd,MCIWNDM_EJECT,0,0)
#define MCIWndHome(hwnd)  MCIWndSeek(hwnd,MCIWND_START)
#define MCIWndEnd(hwnd)  MCIWndSeek(hwnd,MCIWND_END)
#define MCIWndGetSource(hwnd,prc)  (LONG)MCIWndSM(hwnd,MCIWNDM_GET_SOURCE,0,(LPARAM)(LPRECT)(prc))
#define MCIWndPutSource(hwnd,prc)  (LONG)MCIWndSM(hwnd,MCIWNDM_PUT_SOURCE,0,(LPARAM)(LPRECT)(prc))
#define MCIWndGetDest(hwnd,prc)  (LONG)MCIWndSM(hwnd,MCIWNDM_GET_DEST,0,(LPARAM)(LPRECT)(prc))
#define MCIWndPutDest(hwnd,prc)  (LONG)MCIWndSM(hwnd,MCIWNDM_PUT_DEST,0,(LPARAM)(LPRECT)(prc))
#define MCIWndPlayReverse(hwnd)  (LONG)MCIWndSM(hwnd,MCIWNDM_PLAYREVERSE,0,0)
#define MCIWndPlayFrom(hwnd,lPos)  (LONG)MCIWndSM(hwnd,MCIWNDM_PLAYFROM,0,(LPARAM)(LONG)(lPos))
#define MCIWndPlayTo(hwnd,lPos)  (LONG)MCIWndSM(hwnd,MCIWNDM_PLAYTO,  0,(LPARAM)(LONG)(lPos))
#define MCIWndPlayFromTo(hwnd,lStart,lEnd)  (MCIWndSeek(hwnd,lStart),MCIWndPlayTo(hwnd,lEnd))
#define MCIWndGetDeviceID(hwnd)  (UINT)MCIWndSM(hwnd,MCIWNDM_GETDEVICEID,0,0)
#define MCIWndGetAlias(hwnd)  (UINT)MCIWndSM(hwnd,MCIWNDM_GETALIAS,0,0)
#define MCIWndGetMode(hwnd,lp,len)  (LONG)MCIWndSM(hwnd,MCIWNDM_GETMODE,(WPARAM)(UINT)(len),(LPARAM)(LPTSTR)(lp))
#define MCIWndGetPosition(hwnd)  (LONG)MCIWndSM(hwnd,MCIWNDM_GETPOSITION,0,0)
#define MCIWndGetPositionString(hwnd,lp,len)  (LONG)MCIWndSM(hwnd,MCIWNDM_GETPOSITION,(WPARAM)(UINT)(len),(LPARAM)(LPTSTR)(lp))
#define MCIWndGetStart(hwnd)  (LONG)MCIWndSM(hwnd,MCIWNDM_GETSTART,0,0)
#define MCIWndGetLength(hwnd)  (LONG)MCIWndSM(hwnd,MCIWNDM_GETLENGTH,0,0)
#define MCIWndGetEnd(hwnd)  (LONG)MCIWndSM(hwnd,MCIWNDM_GETEND,0,0)
#define MCIWndStep(hwnd,n)  (LONG)MCIWndSM(hwnd,MCI_STEP,0,(LPARAM)(long)(n))
#define MCIWndDestroy(hwnd)  (VOID)MCIWndSM(hwnd,WM_CLOSE,0,0)
#define MCIWndSetZoom(hwnd,iZoom)  (VOID)MCIWndSM(hwnd,MCIWNDM_SETZOOM,0,(LPARAM)(UINT)(iZoom))
#define MCIWndGetZoom(hwnd)  (UINT)MCIWndSM(hwnd,MCIWNDM_GETZOOM,0,0)
#define MCIWndSetVolume(hwnd,iVol)  (LONG)MCIWndSM(hwnd,MCIWNDM_SETVOLUME,0,(LPARAM)(UINT)(iVol))
#define MCIWndGetVolume(hwnd)  (LONG)MCIWndSM(hwnd,MCIWNDM_GETVOLUME,0,0)
#define MCIWndSetSpeed(hwnd,iSpeed)  (LONG)MCIWndSM(hwnd,MCIWNDM_SETSPEED,0,(LPARAM)(UINT)(iSpeed))
#define MCIWndGetSpeed(hwnd)  (LONG)MCIWndSM(hwnd,MCIWNDM_GETSPEED,0,0)
#define MCIWndSetTimeFormat(hwnd,lp)  (LONG)MCIWndSM(hwnd,MCIWNDM_SETTIMEFORMAT,0,(LPARAM)(LPTSTR)(lp))
#define MCIWndGetTimeFormat(hwnd,lp,len)  (LONG)MCIWndSM(hwnd,MCIWNDM_GETTIMEFORMAT,(WPARAM)(UINT)(len),(LPARAM)(LPTSTR)(lp))
#define MCIWndValidateMedia(hwnd)  (VOID)MCIWndSM(hwnd,MCIWNDM_VALIDATEMEDIA,0,0)
#define MCIWndSetRepeat(hwnd,f)  (void)MCIWndSM(hwnd,MCIWNDM_SETREPEAT,0,(LPARAM)(BOOL)(f))
#define MCIWndGetRepeat(hwnd)  (BOOL)MCIWndSM(hwnd,MCIWNDM_GETREPEAT,0,0)
#define MCIWndUseFrames(hwnd)  MCIWndSetTimeFormat(hwnd,TEXT("frames"))
#define MCIWndUseTime(hwnd)  MCIWndSetTimeFormat(hwnd,TEXT("ms"))
#define MCIWndSetActiveTimer(hwnd,active)  (VOID)MCIWndSM(hwnd,MCIWNDM_SETACTIVETIMER,(WPARAM)(UINT)(active),0L)
#define MCIWndSetInactiveTimer(hwnd,inactive)  (VOID)MCIWndSM(hwnd,MCIWNDM_SETINACTIVETIMER,(WPARAM)(UINT)(inactive),0L)
#define MCIWndSetTimers(hwnd,active,inactive)  (VOID)MCIWndSM(hwnd,MCIWNDM_SETTIMERS,(WPARAM)(UINT)(active),(LPARAM)(UINT)(inactive))
#define MCIWndGetActiveTimer(hwnd)  (UINT)MCIWndSM(hwnd,MCIWNDM_GETACTIVETIMER,0,0L);
#define MCIWndGetInactiveTimer(hwnd)  (UINT)MCIWndSM(hwnd,MCIWNDM_GETINACTIVETIMER,0,0L);
#define MCIWndRealize(hwnd,fBkgnd) (LONG)MCIWndSM(hwnd,MCIWNDM_REALIZE,(WPARAM)(BOOL)(fBkgnd),0)
#define MCIWndSendString(hwnd,sz)  (LONG)MCIWndSM(hwnd,MCIWNDM_SENDSTRING,0,(LPARAM)(LPTSTR)(sz))
#define MCIWndReturnString(hwnd,lp,len)  (LONG)MCIWndSM(hwnd,MCIWNDM_RETURNSTRING,(WPARAM)(UINT)(len),(LPARAM)(LPVOID)(lp))
#define MCIWndGetError(hwnd,lp,len) (LONG)MCIWndSM(hwnd,MCIWNDM_GETERROR,(WPARAM)(UINT)(len),(LPARAM)(LPVOID)(lp))
#define MCIWndGetPalette(hwnd)  (HPALETTE)MCIWndSM(hwnd,MCIWNDM_GETPALETTE,0,0)
#define MCIWndSetPalette(hwnd,hpal)  (LONG)MCIWndSM(hwnd,MCIWNDM_SETPALETTE,(WPARAM)(HPALETTE)(hpal),0)
#define MCIWndGetFileName(hwnd,lp,len)  (LONG)MCIWndSM(hwnd,MCIWNDM_GETFILENAME,(WPARAM)(UINT)(len),(LPARAM)(LPVOID)(lp))
#define MCIWndGetDevice(hwnd,lp,len)  (LONG)MCIWndSM(hwnd,MCIWNDM_GETDEVICE,(WPARAM)(UINT)(len),(LPARAM)(LPVOID)(lp))
#define MCIWndGetStyles(hwnd)  (UINT)MCIWndSM(hwnd,MCIWNDM_GETSTYLES,0,0L)
#define MCIWndChangeStyles(hwnd,mask,value)  (LONG)MCIWndSM(hwnd,MCIWNDM_CHANGESTYLES,(WPARAM)(UINT)(mask),(LPARAM)(LONG)(value))
#define MCIWndOpenInterface(hwnd,pUnk)  (LONG)MCIWndSM(hwnd,MCIWNDM_OPENINTERFACE,0,(LPARAM)(LPUNKNOWN)(pUnk))
#define MCIWndSetOwner(hwnd,hwndP)  (LONG)MCIWndSM(hwnd,MCIWNDM_SETOWNER,(WPARAM)(hwndP),0)

#define capSetCallbackOnError(hwnd,fpProc)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_CALLBACK_ERROR,0,(LPARAM)(LPVOID)(fpProc)))
#define capSetCallbackOnStatus(hwnd,fpProc)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_CALLBACK_STATUS,0,(LPARAM)(LPVOID)(fpProc)))
#define capSetCallbackOnYield(hwnd,fpProc)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_CALLBACK_YIELD,0,(LPARAM)(LPVOID)(fpProc)))
#define capSetCallbackOnFrame(hwnd,fpProc)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_CALLBACK_FRAME,0,(LPARAM)(LPVOID)(fpProc)))
#define capSetCallbackOnVideoStream(hwnd,fpProc)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_CALLBACK_VIDEOSTREAM,0,(LPARAM)(LPVOID)(fpProc)))
#define capSetCallbackOnWaveStream(hwnd,fpProc)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_CALLBACK_WAVESTREAM,0,(LPARAM)(LPVOID)(fpProc)))
#define capSetCallbackOnCapControl(hwnd,fpProc)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_CALLBACK_CAPCONTROL,0,(LPARAM)(LPVOID)(fpProc)))

#define capSetUserData(hwnd,lUser)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_USER_DATA,0,(LPARAM)lUser))
#define capGetUserData(hwnd)  (AVICapSM(hwnd,WM_CAP_GET_USER_DATA,0,0))

#define capDriverConnect(hwnd,i)  ((BOOL)AVICapSM(hwnd,WM_CAP_DRIVER_CONNECT,(WPARAM)(i),0L))
#define capDriverDisconnect(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_DRIVER_DISCONNECT,(WPARAM)0,0L))
#define capDriverGetName(hwnd,szName,wSize)  ((BOOL)AVICapSM(hwnd,WM_CAP_DRIVER_GET_NAME,(WPARAM)(wSize),(LPARAM)(LPVOID)(LPTSTR)(szName)))
#define capDriverGetVersion(hwnd,szVer,wSize)  ((BOOL)AVICapSM(hwnd,WM_CAP_DRIVER_GET_VERSION,(WPARAM)(wSize),(LPARAM)(LPVOID)(LPTSTR)(szVer)))
#define capDriverGetCaps(hwnd,s,wSize)  ((BOOL)AVICapSM(hwnd,WM_CAP_DRIVER_GET_CAPS,(WPARAM)(wSize),(LPARAM)(LPVOID)(LPCAPDRIVERCAPS)(s)))

#define capFileSetCaptureFile(hwnd,szName)  ((BOOL)AVICapSM(hwnd,WM_CAP_FILE_SET_CAPTURE_FILE,0,(LPARAM)(LPVOID)(LPTSTR)(szName)))
#define capFileGetCaptureFile(hwnd,szName,wSize) ((BOOL)AVICapSM(hwnd,WM_CAP_FILE_GET_CAPTURE_FILE,(WPARAM)(wSize),(LPARAM)(LPVOID)(LPTSTR)(szName)))
#define capFileAlloc(hwnd,dwSize)  ((BOOL)AVICapSM(hwnd,WM_CAP_FILE_ALLOCATE,0,(LPARAM)(DWORD)(dwSize)))
#define capFileSaveAs(hwnd,szName)  ((BOOL)AVICapSM(hwnd,WM_CAP_FILE_SAVEAS,0,(LPARAM)(LPVOID)(LPTSTR)(szName)))
#define capFileSetInfoChunk(hwnd,lpInfoChunk)  ((BOOL)AVICapSM(hwnd,WM_CAP_FILE_SET_INFOCHUNK,(WPARAM)0,(LPARAM)(LPCAPINFOCHUNK)(lpInfoChunk)))
#define capFileSaveDIB(hwnd,szName)  ((BOOL)AVICapSM(hwnd,WM_CAP_FILE_SAVEDIB,0,(LPARAM)(LPVOID)(LPTSTR)(szName)))

#define capEditCopy(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_EDIT_COPY,0,0L))

#define capSetAudioFormat(hwnd,s,wSize)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_AUDIOFORMAT,(WPARAM)(wSize),(LPARAM)(LPVOID)(LPWAVEFORMATEX)(s)))
#define capGetAudioFormat(hwnd,s,wSize)  ((DWORD)AVICapSM(hwnd,WM_CAP_GET_AUDIOFORMAT,(WPARAM)(wSize),(LPARAM)(LPVOID)(LPWAVEFORMATEX)(s)))
#define capGetAudioFormatSize(hwnd)  ((DWORD)AVICapSM(hwnd,WM_CAP_GET_AUDIOFORMAT,(WPARAM)0,(LPARAM)0L))

#define capDlgVideoFormat(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_DLG_VIDEOFORMAT,0,0L))
#define capDlgVideoSource(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_DLG_VIDEOSOURCE,0,0L))
#define capDlgVideoDisplay(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_DLG_VIDEODISPLAY,0,0L))
#define capDlgVideoCompression(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_DLG_VIDEOCOMPRESSION,0,0L))

#define capGetVideoFormat(hwnd,s,wSize)  ((DWORD)AVICapSM(hwnd,WM_CAP_GET_VIDEOFORMAT,(WPARAM)(wSize),(LPARAM)(LPVOID)(s)))
#define capGetVideoFormatSize(hwnd)  ((DWORD)AVICapSM(hwnd,WM_CAP_GET_VIDEOFORMAT,0,0L))
#define capSetVideoFormat(hwnd,s,wSize)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_VIDEOFORMAT,(WPARAM)(wSize),(LPARAM)(LPVOID)(s)))

#define capPreview(hwnd,f)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_PREVIEW,(WPARAM)(BOOL)(f),0L))
#define capPreviewRate(hwnd,wMS)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_PREVIEWRATE,(WPARAM)(wMS),0))
#define capOverlay(hwnd,f)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_OVERLAY,(WPARAM)(BOOL)(f),0L))
#define capPreviewScale(hwnd,f)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_SCALE,(WPARAM)(BOOL)f,0L))
#define capGetStatus(hwnd,s,wSize)  ((BOOL)AVICapSM(hwnd,WM_CAP_GET_STATUS,(WPARAM)(wSize),(LPARAM)(LPVOID)(LPCAPSTATUS)(s)))
#define capSetScrollPos(hwnd,lpP)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_SCROLL,(WPARAM)0,(LPARAM)(LPPOINT)(lpP)))

#define capGrabFrame(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_GRAB_FRAME,(WPARAM)0,(LPARAM)0L))
#define capGrabFrameNoStop(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_GRAB_FRAME_NOSTOP,(WPARAM)0,(LPARAM)0L))

#define capCaptureSequence(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_SEQUENCE,(WPARAM)0,(LPARAM)0L))
#define capCaptureSequenceNoFile(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_SEQUENCE_NOFILE,(WPARAM)0,(LPARAM)0L))
#define capCaptureStop(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_STOP,(WPARAM)0,(LPARAM)0L))
#define capCaptureAbort(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_ABORT,(WPARAM)0,(LPARAM)0L))

#define capCaptureSingleFrameOpen(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_SINGLE_FRAME_OPEN,(WPARAM)0,(LPARAM)0L))
#define capCaptureSingleFrameClose(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_SINGLE_FRAME_CLOSE,(WPARAM)0,(LPARAM)0L))
#define capCaptureSingleFrame(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_SINGLE_FRAME,(WPARAM)0,(LPARAM)0L))

#define capCaptureGetSetup(hwnd,s,wSize)  ((BOOL)AVICapSM(hwnd,WM_CAP_GET_SEQUENCE_SETUP,(WPARAM)(wSize),(LPARAM)(LPVOID)(LPCAPTUREPARMS)(s)))
#define capCaptureSetSetup(hwnd,s,wSize)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_SEQUENCE_SETUP,(WPARAM)(wSize),(LPARAM)(LPVOID)(LPCAPTUREPARMS)(s)))

#define capSetMCIDeviceName(hwnd,szName)  ((BOOL)AVICapSM(hwnd,WM_CAP_SET_MCI_DEVICE,0,(LPARAM)(LPVOID)(LPTSTR)(szName)))
#define capGetMCIDeviceName(hwnd,szName,wSize)  ((BOOL)AVICapSM(hwnd,WM_CAP_GET_MCI_DEVICE,(WPARAM)(wSize),(LPARAM)(LPVOID)(LPTSTR)(szName)))

#define capPaletteOpen(hwnd,szName)  ((BOOL)AVICapSM(hwnd,WM_CAP_PAL_OPEN,0,(LPARAM)(LPVOID)(LPTSTR)(szName)))
#define capPaletteSave(hwnd,szName)  ((BOOL)AVICapSM(hwnd,WM_CAP_PAL_SAVE,0,(LPARAM)(LPVOID)(LPTSTR)(szName)))
#define capPalettePaste(hwnd)  ((BOOL)AVICapSM(hwnd,WM_CAP_PAL_PASTE,(WPARAM) 0,(LPARAM)0L))
#define capPaletteAuto(hwnd,iFrames,iColors)  ((BOOL)AVICapSM(hwnd,WM_CAP_PAL_AUTOCREATE,(WPARAM)(iFrames),(LPARAM)(DWORD)(iColors)))
#define capPaletteManual(hwnd,fGrab,iColors)  ((BOOL)AVICapSM(hwnd,WM_CAP_PAL_MANUALCREATE,(WPARAM)(fGrab),(LPARAM)(DWORD)(iColors)))

#ifdef __cplusplus
}
#endif

#include <msacm.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef OFN_READONLY
BOOL VFWAPI GetOpenFileNamePreviewA(LPOPENFILENAMEA);
BOOL VFWAPI GetSaveFileNamePreviewA(LPOPENFILENAMEA);
BOOL VFWAPI GetOpenFileNamePreviewW(LPOPENFILENAMEW);
BOOL VFWAPI GetSaveFileNamePreviewW(LPOPENFILENAMEW);

#ifdef UNICODE
#define GetOpenFileNamePreview GetOpenFileNamePreviewW
#define GetSaveFileNamePreview GetSaveFileNamePreviewW
#else
#define GetOpenFileNamePreview GetOpenFileNamePreviewA
#define GetSaveFileNamePreview GetSaveFileNamePreviewA
#endif
#endif /* OFN_READONLY */

#include "poppack.h"

#ifdef __cplusplus
}
#endif

#endif /* _VFW_H */
