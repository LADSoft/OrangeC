/* 
   mmrstruc.h

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
#ifndef __WAVEFORMAT
#define __WAVEFORMAT
typedef struct waveformat_tag {
    WORD    wFormatTag;        
    WORD    nChannels;         
    DWORD   nSamplesPerSec;    
    DWORD   nAvgBytesPerSec;   
    WORD    nBlockAlign;       
} WAVEFORMAT;
typedef WAVEFORMAT       *PWAVEFORMAT;
typedef WAVEFORMAT NEAR *NPWAVEFORMAT;
typedef WAVEFORMAT FAR  *LPWAVEFORMAT;
typedef struct pcmwaveformat_tag {
    WAVEFORMAT  wf;
    WORD        wBitsPerSample;
} PCMWAVEFORMAT;
typedef PCMWAVEFORMAT       *PPCMWAVEFORMAT;
typedef PCMWAVEFORMAT NEAR *NPPCMWAVEFORMAT;
typedef PCMWAVEFORMAT FAR  *LPPCMWAVEFORMAT;
typedef struct tWAVEFORMATEX
{
    WORD    wFormatTag;        
    WORD    nChannels;         
    DWORD   nSamplesPerSec;    
    DWORD   nAvgBytesPerSec;   
    WORD    nBlockAlign;       
    WORD    wBitsPerSample;    
    WORD    cbSize;            
} WAVEFORMATEX;
typedef WAVEFORMATEX       *PWAVEFORMATEX;
typedef WAVEFORMATEX NEAR *NPWAVEFORMATEX;
typedef WAVEFORMATEX FAR  *LPWAVEFORMATEX;
typedef const WAVEFORMATEX *LPCWAVEFORMATEX;
#endif

typedef struct adpcmcoef_tag {
        short   iCoef1;
        short   iCoef2;
} ADPCMCOEFSET;
typedef ADPCMCOEFSET       *PADPCMCOEFSET;
typedef ADPCMCOEFSET NEAR *NPADPCMCOEFSET;
typedef ADPCMCOEFSET FAR  *LPADPCMCOEFSET;

typedef struct adpcmwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            wSamplesPerBlock;
        WORD            wNumCoef;
        ADPCMCOEFSET    aCoef[];
} ADPCMWAVEFORMAT;
typedef ADPCMWAVEFORMAT       *PADPCMWAVEFORMAT;
typedef ADPCMWAVEFORMAT NEAR *NPADPCMWAVEFORMAT;
typedef ADPCMWAVEFORMAT FAR  *LPADPCMWAVEFORMAT;


typedef struct dvi_adpcmwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            wSamplesPerBlock;
} DVIADPCMWAVEFORMAT;
typedef DVIADPCMWAVEFORMAT       *PDVIADPCMWAVEFORMAT;
typedef DVIADPCMWAVEFORMAT NEAR *NPDVIADPCMWAVEFORMAT;
typedef DVIADPCMWAVEFORMAT FAR  *LPDVIADPCMWAVEFORMAT;

typedef struct ima_adpcmwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            wSamplesPerBlock;
} IMAADPCMWAVEFORMAT;
typedef IMAADPCMWAVEFORMAT       *PIMAADPCMWAVEFORMAT;
typedef IMAADPCMWAVEFORMAT NEAR *NPIMAADPCMWAVEFORMAT;
typedef IMAADPCMWAVEFORMAT FAR  *LPIMAADPCMWAVEFORMAT;

typedef struct mediaspace_adpcmwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD    wRevision;
} MEDIASPACEADPCMWAVEFORMAT;
typedef MEDIASPACEADPCMWAVEFORMAT           *PMEDIASPACEADPCMWAVEFORMAT;
typedef MEDIASPACEADPCMWAVEFORMAT NEAR     *NPMEDIASPACEADPCMWAVEFORMAT;
typedef MEDIASPACEADPCMWAVEFORMAT FAR      *LPMEDIASPACEADPCMWAVEFORMAT;

typedef struct sierra_adpcmwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            wRevision;
} SIERRAADPCMWAVEFORMAT;
typedef SIERRAADPCMWAVEFORMAT   *PSIERRAADPCMWAVEFORMAT;
typedef SIERRAADPCMWAVEFORMAT NEAR      *NPSIERRAADPCMWAVEFORMAT;
typedef SIERRAADPCMWAVEFORMAT FAR       *LPSIERRAADPCMWAVEFORMAT;


typedef struct g723_adpcmwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            cbExtraSize;
        WORD            nAuxBlockSize;
} G723_ADPCMWAVEFORMAT;
typedef G723_ADPCMWAVEFORMAT *PG723_ADPCMWAVEFORMAT;
typedef G723_ADPCMWAVEFORMAT NEAR *NPG723_ADPCMWAVEFORMAT;
typedef G723_ADPCMWAVEFORMAT FAR  *LPG723_ADPCMWAVEFORMAT;


typedef struct digistdwaveformat_tag {
        WAVEFORMATEX    wfx;
} DIGISTDWAVEFORMAT;
typedef DIGISTDWAVEFORMAT       *PDIGISTDWAVEFORMAT;
typedef DIGISTDWAVEFORMAT NEAR *NPDIGISTDWAVEFORMAT;
typedef DIGISTDWAVEFORMAT FAR  *LPDIGISTDWAVEFORMAT;

typedef struct digifixwaveformat_tag {
        WAVEFORMATEX    wfx;
} DIGIFIXWAVEFORMAT;
typedef DIGIFIXWAVEFORMAT       *PDIGIFIXWAVEFORMAT;
typedef DIGIFIXWAVEFORMAT NEAR *NPDIGIFIXWAVEFORMAT;
typedef DIGIFIXWAVEFORMAT FAR  *LPDIGIFIXWAVEFORMAT;

typedef struct creative_fastspeechformat_tag{
        WAVEFORMATEX    ewf;
}DIALOGICOKIADPCMWAVEFORMAT;
typedef DIALOGICOKIADPCMWAVEFORMAT       *PDIALOGICOKIADPCMWAVEFORMAT;
typedef DIALOGICOKIADPCMWAVEFORMAT NEAR *NPDIALOGICOKIADPCMWAVEFORMAT;
typedef DIALOGICOKIADPCMWAVEFORMAT FAR  *LPDIALOGICOKIADPCMWAVEFORMAT;

typedef struct yamaha_adpmcwaveformat_tag {
        WAVEFORMATEX    wfx;

} YAMAHA_ADPCMWAVEFORMAT;
typedef YAMAHA_ADPCMWAVEFORMAT *PYAMAHA_ADPCMWAVEFORMAT;
typedef YAMAHA_ADPCMWAVEFORMAT NEAR *NPYAMAHA_ADPCMWAVEFORMAT;
typedef YAMAHA_ADPCMWAVEFORMAT FAR  *LPYAMAHA_ADPCMWAVEFORMAT;

typedef struct sonarcwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            wCompType;
} SONARCWAVEFORMAT;
typedef SONARCWAVEFORMAT       *PSONARCWAVEFORMAT;
typedef SONARCWAVEFORMAT NEAR *NPSONARCWAVEFORMAT;
typedef SONARCWAVEFORMAT FAR  *LPSONARCWAVEFORMAT;

typedef struct truespeechwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            wRevision;
        WORD            nSamplesPerBlock;
        BYTE            abReserved[28];
} TRUESPEECHWAVEFORMAT;
typedef TRUESPEECHWAVEFORMAT       *PTRUESPEECHWAVEFORMAT;
typedef TRUESPEECHWAVEFORMAT NEAR *NPTRUESPEECHWAVEFORMAT;
typedef TRUESPEECHWAVEFORMAT FAR  *LPTRUESPEECHWAVEFORMAT;

typedef struct echosc1waveformat_tag {
        WAVEFORMATEX    wfx;
} ECHOSC1WAVEFORMAT;
typedef ECHOSC1WAVEFORMAT       *PECHOSC1WAVEFORMAT;
typedef ECHOSC1WAVEFORMAT NEAR *NPECHOSC1WAVEFORMAT;
typedef ECHOSC1WAVEFORMAT FAR  *LPECHOSC1WAVEFORMAT;

typedef struct audiofile_af36waveformat_tag {
        WAVEFORMATEX    wfx;
} AUDIOFILE_AF36WAVEFORMAT;
typedef AUDIOFILE_AF36WAVEFORMAT       *PAUDIOFILE_AF36WAVEFORMAT;
typedef AUDIOFILE_AF36WAVEFORMAT NEAR *NPAUDIOFILE_AF36WAVEFORMAT;
typedef AUDIOFILE_AF36WAVEFORMAT FAR  *LPAUDIOFILE_AF36WAVEFORMAT;

typedef struct aptxwaveformat_tag {
        WAVEFORMATEX    wfx;
} APTXWAVEFORMAT;
typedef APTXWAVEFORMAT       *PAPTXWAVEFORMAT;
typedef APTXWAVEFORMAT NEAR *NPAPTXWAVEFORMAT;
typedef APTXWAVEFORMAT FAR  *LPAPTXWAVEFORMAT;

typedef struct audiofile_af10waveformat_tag {
        WAVEFORMATEX    wfx;
} AUDIOFILE_AF10WAVEFORMAT;
typedef AUDIOFILE_AF10WAVEFORMAT       *PAUDIOFILE_AF10WAVEFORMAT;
typedef AUDIOFILE_AF10WAVEFORMAT NEAR *NPAUDIOFILE_AF10WAVEFORMAT;
typedef AUDIOFILE_AF10WAVEFORMAT FAR  *LPAUDIOFILE_AF10WAVEFORMAT;

typedef struct dolbyac2waveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            nAuxBitsCode;
} DOLBYAC2WAVEFORMAT;

typedef struct gsm610waveformat_tag {
      WAVEFORMATEX    wfx;
      WORD                    wSamplesPerBlock;
} GSM610WAVEFORMAT;
typedef GSM610WAVEFORMAT *PGSM610WAVEFORMAT;
typedef GSM610WAVEFORMAT NEAR    *NPGSM610WAVEFORMAT;
typedef GSM610WAVEFORMAT FAR     *LPGSM610WAVEFORMAT;

typedef struct adpcmewaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            wSamplesPerBlock;
} ADPCMEWAVEFORMAT;
typedef ADPCMEWAVEFORMAT                *PADPCMEWAVEFORMAT;
typedef ADPCMEWAVEFORMAT NEAR   *NPADPCMEWAVEFORMAT;
typedef ADPCMEWAVEFORMAT FAR    *LPADPCMEWAVEFORMAT;

typedef struct contres_vqlpcwaveformat_tag {
WAVEFORMATEX    wfx;
WORD                    wSamplesPerBlock;
} CONTRESVQLPCWAVEFORMAT;
typedef CONTRESVQLPCWAVEFORMAT *PCONTRESVQLPCWAVEFORMAT;
typedef CONTRESVQLPCWAVEFORMAT NEAR      *NPCONTRESVQLPCWAVEFORMAT;
typedef CONTRESVQLPCWAVEFORMAT FAR       *LPCONTRESVQLPCWAVEFORMAT;

typedef struct digirealwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            wSamplesPerBlock;
} DIGIREALWAVEFORMAT;
typedef DIGIREALWAVEFORMAT *PDIGIREALWAVEFORMAT;
typedef DIGIREALWAVEFORMAT NEAR *NPDIGIREALWAVEFORMAT;
typedef DIGIREALWAVEFORMAT FAR *LPDIGIREALWAVEFORMAT;

typedef struct digiadpcmmwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            wSamplesPerBlock;
} DIGIADPCMWAVEFORMAT;
typedef DIGIADPCMWAVEFORMAT       *PDIGIADPCMWAVEFORMAT;
typedef DIGIADPCMWAVEFORMAT NEAR *NPDIGIADPCMWAVEFORMAT;
typedef DIGIADPCMWAVEFORMAT FAR  *LPDIGIADPCMWAVEFORMAT;

typedef struct contres_cr10waveformat_tag {
WAVEFORMATEX    wfx;
WORD                    wSamplesPerBlock;
} CONTRESCR10WAVEFORMAT;
typedef CONTRESCR10WAVEFORMAT *PCONTRESCR10WAVEFORMAT;
typedef CONTRESCR10WAVEFORMAT NEAR       *NPCONTRESCR10WAVEFORMAT;
typedef CONTRESCR10WAVEFORMAT FAR        *LPCONTRESCR10WAVEFORMAT;

typedef struct nms_vbxadpcmmwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            wSamplesPerBlock;
} NMS_VBXADPCMWAVEFORMAT;
typedef NMS_VBXADPCMWAVEFORMAT       *PNMS_VBXADPCMWAVEFORMAT;
typedef NMS_VBXADPCMWAVEFORMAT NEAR *NPNMS_VBXADPCMWAVEFORMAT;
typedef NMS_VBXADPCMWAVEFORMAT FAR  *LPNMS_VBXADPCMWAVEFORMAT;

typedef struct g721_adpcmwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            nAuxBlockSize;
} G721_ADPCMWAVEFORMAT;
typedef G721_ADPCMWAVEFORMAT *PG721_ADPCMWAVEFORMAT;
typedef G721_ADPCMWAVEFORMAT NEAR *NPG721_ADPCMWAVEFORMAT;
typedef G721_ADPCMWAVEFORMAT FAR  *LPG721_ADPCMWAVEFORMAT;

typedef struct mpeg1waveformat_tag {
    WAVEFORMATEX    wfx;
    WORD            fwHeadLayer;
    DWORD           dwHeadBitrate;
    WORD            fwHeadMode;
    WORD            fwHeadModeExt;
    WORD            wHeadEmphasis;
    WORD            fwHeadFlags;
    DWORD           dwPTSLow;
    DWORD           dwPTSHigh;
} MPEG1WAVEFORMAT;
typedef MPEG1WAVEFORMAT                 *PMPEG1WAVEFORMAT;
typedef MPEG1WAVEFORMAT NEAR           *NPMPEG1WAVEFORMAT;
typedef MPEG1WAVEFORMAT FAR            *LPMPEG1WAVEFORMAT;

typedef struct creative_adpcmwaveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            wRevision;
} CREATIVEADPCMWAVEFORMAT;
typedef CREATIVEADPCMWAVEFORMAT       *PCREATIVEADPCMWAVEFORMAT;
typedef CREATIVEADPCMWAVEFORMAT NEAR *NPCREATIVEADPCMWAVEFORMAT;
typedef CREATIVEADPCMWAVEFORMAT FAR  *LPCREATIVEADPCMWAVEFORMAT;

typedef struct creative_fastspeech8format_tag {
        WAVEFORMATEX    wfx;
        WORD wRevision;
} CREATIVEFASTSPEECH8WAVEFORMAT;
typedef CREATIVEFASTSPEECH8WAVEFORMAT       *PCREATIVEFASTSPEECH8WAVEFORMAT;
typedef CREATIVEFASTSPEECH8WAVEFORMAT NEAR *NPCREATIVEFASTSPEECH8WAVEFORMAT;
typedef CREATIVEFASTSPEECH8WAVEFORMAT FAR  *LPCREATIVEFASTSPEECH8WAVEFORMAT;

typedef struct creative_fastspeech10format_tag {
        WAVEFORMATEX    wfx;
        WORD wRevision;
} CREATIVEFASTSPEECH10WAVEFORMAT;
typedef CREATIVEFASTSPEECH10WAVEFORMAT       *PCREATIVEFASTSPEECH10WAVEFORMAT;
typedef CREATIVEFASTSPEECH10WAVEFORMAT NEAR *NPCREATIVEFASTSPEECH10WAVEFORMAT;
typedef CREATIVEFASTSPEECH10WAVEFORMAT FAR  *LPCREATIVEFASTSPEECH10WAVEFORMAT;

typedef struct fmtowns_snd_waveformat_tag {
        WAVEFORMATEX    wfx;
        WORD            wRevision;
} FMTOWNS_SND_WAVEFORMAT;
typedef FMTOWNS_SND_WAVEFORMAT       *PFMTOWNS_SND_WAVEFORMAT;
typedef FMTOWNS_SND_WAVEFORMAT NEAR *NPFMTOWNS_SND_WAVEFORMAT;
typedef FMTOWNS_SND_WAVEFORMAT FAR  *LPFMTOWNS_SND_WAVEFORMAT;

typedef struct oligsmwaveformat_tag {
        WAVEFORMATEX    wfx;
} OLIGSMWAVEFORMAT;
typedef OLIGSMWAVEFORMAT     *POLIGSMWAVEFORMAT;
typedef OLIGSMWAVEFORMAT NEAR *NPOLIGSMWAVEFORMAT;
typedef OLIGSMWAVEFORMAT  FAR  *LPOLIGSMWAVEFORMAT;

typedef struct oliadpcmwaveformat_tag {
        WAVEFORMATEX    wfx;
} OLIADPCMWAVEFORMAT;
typedef OLIADPCMWAVEFORMAT     *POLIADPCMWAVEFORMAT;
typedef OLIADPCMWAVEFORMAT NEAR *NPOLIADPCMWAVEFORMAT ;
typedef OLIADPCMWAVEFORMAT  FAR  *LPOLIADPCMWAVEFORMAT;

typedef struct olicelpwaveformat_tag {
        WAVEFORMATEX    wfx;
} OLICELPWAVEFORMAT;
typedef OLICELPWAVEFORMAT     *POLICELPWAVEFORMAT;
typedef OLICELPWAVEFORMAT NEAR *NPOLICELPWAVEFORMAT ;
typedef OLICELPWAVEFORMAT  FAR  *LPOLICELPWAVEFORMAT;

typedef struct olisbcwaveformat_tag {
        WAVEFORMATEX    wfx;
} OLISBCWAVEFORMAT;
typedef OLISBCWAVEFORMAT     *POLISBCWAVEFORMAT;
typedef OLISBCWAVEFORMAT NEAR *NPOLISBCWAVEFORMAT ;
typedef OLISBCWAVEFORMAT  FAR  *LPOLISBCWAVEFORMAT;

typedef struct olioprwaveformat_tag {
        WAVEFORMATEX    wfx;
} OLIOPRWAVEFORMAT;
typedef OLIOPRWAVEFORMAT     *POLIOPRWAVEFORMAT;
typedef OLIOPRWAVEFORMAT NEAR *NPOLIOPRWAVEFORMAT ;
typedef OLIOPRWAVEFORMAT  FAR  *LPOLIOPRWAVEFORMAT;

typedef struct csimaadpcmwaveformat_tag {
        WAVEFORMATEX    wfx;
} CSIMAADPCMWAVEFORMAT;
typedef CSIMAADPCMWAVEFORMAT     *PCSIMAADPCMWAVEFORMAT;
typedef CSIMAADPCMWAVEFORMAT NEAR *NPCSIMAADPCMWAVEFORMAT ;
typedef CSIMAADPCMWAVEFORMAT  FAR  *LPCSIMAADPCMWAVEFORMAT;

typedef struct wavefilter_tag {
    DWORD   cbStruct;           
    DWORD   dwFilterTag;        
    DWORD   fdwFilter;          
    DWORD   dwReserved[5];      
} WAVEFILTER;
typedef WAVEFILTER       *PWAVEFILTER;
typedef WAVEFILTER NEAR *NPWAVEFILTER;
typedef WAVEFILTER FAR  *LPWAVEFILTER;

typedef struct wavefilter_volume_tag {
        WAVEFILTER      wfltr;
        DWORD           dwVolume;
} VOLUMEWAVEFILTER;
typedef VOLUMEWAVEFILTER       *PVOLUMEWAVEFILTER;
typedef VOLUMEWAVEFILTER NEAR *NPVOLUMEWAVEFILTER;
typedef VOLUMEWAVEFILTER FAR  *LPVOLUMEWAVEFILTER;

typedef struct wavefilter_echo_tag {
        WAVEFILTER      wfltr;
        DWORD           dwVolume;
        DWORD           dwDelay;
} ECHOWAVEFILTER;
typedef ECHOWAVEFILTER       *PECHOWAVEFILTER;
typedef ECHOWAVEFILTER NEAR *NPECHOWAVEFILTER;
typedef ECHOWAVEFILTER FAR  *LPECHOWAVEFILTER;

struct tag_s_RIFFWAVE_inst {
    BYTE    bUnshiftedNote;
    char    chFineTune;
    char    chGain;
    BYTE    bLowNote;
    BYTE    bHighNote;
    BYTE    bLowVelocity;
    BYTE    bHighVelocity;
};

typedef struct tag_s_RIFFWAVE_INST s_RIFFWAVE_inst;

typedef struct tagEXBMINFOHEADER {
        BITMAPINFOHEADER    bmi;
        DWORD   biExtDataOffset;
} EXBMINFOHEADER;

typedef struct tagJPEGINFOHEADER {
    
    
    DWORD       JPEGSize;
    DWORD       JPEGProcess;

    DWORD       JPEGColorSpaceID;
    DWORD       JPEGBitsPerSample;
    DWORD       JPEGHSubSampling;
    DWORD       JPEGVSubSampling;
} JPEGINFOHEADER;

#pragma pack()
