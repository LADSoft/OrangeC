//
// There are better ways of playing a wav file I'm sure... this is an example of MMSYSTEM use
//
#include <windows.h>
#include <mmsystem.h>
#include <mmreg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct {
  char           chunkID[4];
  long           chunkSize;
} FILEHDR ;

HWAVEOUT hWaveOut;

UINT threadIdWaveOut = 0;

int audiobytesinqueue = 0 ;

int AudioOutput(unsigned char *streamBuff, int buffLen)
{
   LPWAVEHDR pHeader ;
   MMRESULT res ;
   pHeader = malloc(sizeof(WAVEHDR)) ;
    if (!pHeader)
            return FALSE ;
    pHeader->lpData = malloc(buffLen);
    if (!pHeader->lpData) {
      free( pHeader) ;
        return FALSE ;
    }
    audiobytesinqueue += pHeader->dwBufferLength = buffLen;
    pHeader->dwUser = 0;
    pHeader->dwFlags = 0;
    memcpy(pHeader->lpData, streamBuff, buffLen);
    res = waveOutPrepareHeader(hWaveOut, pHeader, sizeof(WAVEHDR));
    
    waveOutWrite(hWaveOut, pHeader, sizeof(WAVEHDR)); 
    return TRUE ;
}

unsigned _stdcall AudioThread(PVOID threadParams)
{
    UINT    threadIdTone;
    MSG     msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        switch (msg.message)
        {
            case MM_WOM_DONE:
                waveOutUnprepareHeader((HWAVEOUT)(msg.wParam), (LPWAVEHDR)(msg.lParam),
                                      sizeof (WAVEHDR));
                audiobytesinqueue -= ((LPWAVEHDR)msg.lParam)->dwBufferLength ;
                free (((LPWAVEHDR)msg.lParam)->lpData);
                free ((LPWAVEHDR)msg.lParam);
                break;
        }
    }

    return msg.wParam;
}
void InitAudioReceiver(WAVEFORMATEX *wfOut)
{
    WORD wResult;
    ULONG hThreadWaveOut = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)AudioThread, (PVOID)0, 
                                          0, &threadIdWaveOut);

    wResult = waveOutOpen(&hWaveOut,(UINT)WAVE_MAPPER,wfOut,       
                         (DWORD)threadIdWaveOut,0, CALLBACK_THREAD);
    if (wResult != 0)
    {
        printf("error initing audio system") ;
        exit(1) ;
    }
}


void StopAudioReceiver()
{
    while (audiobytesinqueue) Sleep(2) ;
    Sleep(2) ;
}

int main(int argc, char **argv)
{
    FILE * wavefd  ;
    FILEHDR hdr ;
    WAVEFORMATEX chunk ;
    int size ;
    if (argc != 2) {
        printf("usage: playwav file") ;
        exit(1) ;
    }
    wavefd = fopen(argv[1],"rb") ;
    fread(&hdr,sizeof(hdr),1,wavefd) ;
    if (memcmp(hdr.chunkID,"RIFF",4)) {
        fclose(wavefd) ;
        return ERROR ;
    }
    size = hdr.chunkSize ;
    fread(&hdr,4,1,wavefd) ;
    size -= 4 ;
    if (memcmp(hdr.chunkID,"WAVE",4)) {
        fclose(wavefd) ;
        return ERROR ;
    }
    while (size > 0) {
        fread(&hdr,sizeof(hdr),1,wavefd) ;
        size -= sizeof(hdr) +hdr.chunkSize;
        if (!memcmp(hdr.chunkID,"fmt ",4)) {
            fread(&chunk,sizeof(chunk)-sizeof(WORD),1,wavefd) ;
            if (chunk.nChannels > 2 || chunk.nChannels <= 0 || chunk.wFormatTag != 1 || chunk.wBitsPerSample > 16) {
                fclose(wavefd) ;
                printf("can't play this wave file") ;
                exit(1) ;
            }
            InitAudioReceiver(&chunk) ;
        } else if (!memcmp(hdr.chunkID,"data",4)) {
            int size1 = hdr.chunkSize ;
            static BYTE buf[65536] ;
            while (size1 > 0) {
                int len = size1 > 65536 ? 65536 : size1 ;
                size1 -= len ;
                fread(buf,len,1,wavefd) ;
                AudioOutput(buf,len) ;
            }
        } else
            fseek(wavefd,hdr.chunkSize,SEEK_CUR) ;
    }
    fclose(wavefd) ;
    StopAudioReceiver() ;
    return 0 ;
        
}