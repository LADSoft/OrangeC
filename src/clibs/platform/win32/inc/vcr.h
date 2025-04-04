#ifndef _VCR_H
#define _VCR_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* VCR-MCI Command Table definitions */

#define SEND_VCRSIGNAL(dwFlags,dwCallback,hDriver,wDeviceID,dwUser,dwPos)  DriverCallback((dwCallback),DCB_WINDOW,(HANDLE)(wDeviceID),MM_MCISIGNAL,hDriver,((dwFlags)&MCI_VCR_SIGNAL_POSITION)?(dwPos):(dwUser),((dwFlags)&MCI_VCR_SIGNAL_POSITION)?(dwUser):(dwPos))

#define MCI_VCR_OFFSET  1280

#define MCI_LIST  0x0878
#define MCI_SETAUDIO  0x0873
#define MCI_SETVIDEO  0x0876
#define MCI_SIGNAL  0x0875

#define MCI_MARK  (MCI_USER_MESSAGES+0)
#define MCI_INDEX  (MCI_USER_MESSAGES+1)
#define MCI_SETTUNER  (MCI_USER_MESSAGES+2)
#define MCI_SETVCR  (MCI_USER_MESSAGES+3)
#define MCI_SETTIMECODE  (MCI_USER_MESSAGES+4)

#define MCI_TEST  0x00000020L

#define MCI_VCR_GETDEVCAPS_CAN_DETECT_LENGTH  0x00004001L
#define MCI_VCR_GETDEVCAPS_SEEK_ACCURACY  0x00004002L
#define MCI_VCR_GETDEVCAPS_HAS_CLOCK  0x00004003L
#define MCI_VCR_GETDEVCAPS_CAN_REVERSE  0x00004004L
#define MCI_VCR_GETDEVCAPS_NUMBER_OF_MARKS  0x00004005L
#define MCI_VCR_GETDEVCAPS_CAN_TEST  0x00004006L
#define MCI_VCR_GETDEVCAPS_CAN_PREROLL  0x00004007L
#define MCI_VCR_GETDEVCAPS_CAN_PREVIEW  0x00004008L
#define MCI_VCR_GETDEVCAPS_CAN_MONITOR_SOURCES  0x00004009L
#define MCI_VCR_GETDEVCAPS_HAS_TIMECODE  0x0000400AL
#define MCI_VCR_GETDEVCAPS_CAN_FREEZE  0x0000401BL
#define MCI_VCR_GETDEVCAPS_CLOCK_INCREMENT_RATE 0x0000401CL

#define MCI_VCR_INFO_VERSION  0x00010000L

#define MCI_VCR_PLAY_REVERSE  0x00010000L
#define MCI_VCR_PLAY_AT  0x00020000L
#define MCI_VCR_PLAY_SCAN  0x00040000L

#define MCI_VCR_RECORD_INITIALIZE  0x00010000L
#define MCI_VCR_RECORD_AT  0x00020000L
#define MCI_VCR_RECORD_PREVIEW  0x00040000L

#define MCI_VCR_CUE_INPUT  0x00010000L
#define MCI_VCR_CUE_OUTPUT  0x00020000L
#define MCI_VCR_CUE_PREROLL  0x00040000L
#define MCI_VCR_CUE_REVERSE  0x00080000L

#define MCI_VCR_SEEK_REVERSE  0x00010000L
#define MCI_VCR_SEEK_MARK  0x00020000L
#define MCI_VCR_SEEK_AT  0x00040000L

#define MCI_VCR_SETTUNER_CHANNEL  0x00010000L
#define MCI_VCR_SETTUNER_CHANNEL_UP  0x00020000L
#define MCI_VCR_SETTUNER_CHANNEL_DOWN  0x00040000L
#define MCI_VCR_SETTUNER_CHANNEL_SEEK_UP  0x00080000L
#define MCI_VCR_SETTUNER_CHANNEL_SEEK_DOWN  0x00100000L
#define MCI_VCR_SETTUNER_NUMBER  0x00200000L

#define MCI_VCR_SET_TIME_MODE  0x00010000L
#define MCI_VCR_SET_POWER  0x00020000L
#define MCI_VCR_SET_RECORD_FORMAT  0x00040000L
#define MCI_VCR_SET_COUNTER_FORMAT  0x00080000L
#define MCI_VCR_SET_INDEX  0x00100000L
#define MCI_VCR_SET_ASSEMBLE_RECORD  0x00200000L
#define MCI_VCR_SET_TRACKING  0x00400000L
#define MCI_VCR_SET_SPEED  0x00800000L
#define MCI_VCR_SET_TAPE_LENGTH  0x01000000L
#define MCI_VCR_SET_COUNTER_VALUE  0x02000000L
#define MCI_VCR_SET_CLOCK  0x04000000L
#define MCI_VCR_SET_PAUSE_TIMEOUT  0x08000000L
#define MCI_VCR_SET_PREROLL_DURATION  0x10000000L
#define MCI_VCR_SET_POSTROLL_DURATION  0x20000000L

#define MCI_VCR_SETTIMECODE_RECORD  0x00010000L

#define MCI_VCR_STATUS_FRAME_RATE  0x00004001L
#define MCI_VCR_STATUS_SPEED  0x00004002L
#define MCI_VCR_STATUS_MEDIA_TYPE  0x00004003L
#define MCI_VCR_STATUS_RECORD_FORMAT  0x00004004L
#define MCI_VCR_STATUS_PLAY_FORMAT  0x00004005L
#define MCI_VCR_STATUS_AUDIO_SOURCE  0x00004006L
#define MCI_VCR_STATUS_AUDIO_SOURCE_NUMBER  0x00004007L
#define MCI_VCR_STATUS_VIDEO_SOURCE  0x00004008L
#define MCI_VCR_STATUS_VIDEO_SOURCE_NUMBER  0x00004009L
#define MCI_VCR_STATUS_AUDIO_MONITOR  0x0000400AL
#define MCI_VCR_STATUS_AUDIO_MONITOR_NUMBER  0x0000400BL
#define MCI_VCR_STATUS_VIDEO_MONITOR  0x0000400CL
#define MCI_VCR_STATUS_VIDEO_MONITOR_NUMBER  0x0000400DL
#define MCI_VCR_STATUS_INDEX_ON  0x0000400EL
#define MCI_VCR_STATUS_INDEX  0x0000400FL
#define MCI_VCR_STATUS_COUNTER_FORMAT  0x00004010L
#define MCI_VCR_STATUS_COUNTER_RESOLUTION  0x00004011L
#define MCI_VCR_STATUS_TIMECODE_TYPE  0x00004012L
#define MCI_VCR_STATUS_COUNTER_VALUE  0x00004013L
#define MCI_VCR_STATUS_TUNER_CHANNEL  0x00004014L
#define MCI_VCR_STATUS_WRITE_PROTECTED  0x00004015L
#define MCI_VCR_STATUS_TIMECODE_RECORD  0x00004016L
#define MCI_VCR_STATUS_VIDEO_RECORD  0x00004017L
#define MCI_VCR_STATUS_AUDIO_RECORD  0x00004018L
#define MCI_VCR_STATUS_TIME_TYPE  0x00004019L
#define MCI_VCR_STATUS_TIME_MODE  0x0000401AL
#define MCI_VCR_STATUS_POWER_ON  0x0000401BL
#define MCI_VCR_STATUS_CLOCK  0x0000401CL
#define MCI_VCR_STATUS_ASSEMBLE_RECORD  0x0000401DL
#define MCI_VCR_STATUS_TIMECODE_PRESENT  0x0000401EL
#define MCI_VCR_STATUS_NUMBER_OF_VIDEO_TRACKS  0x0000401FL
#define MCI_VCR_STATUS_NUMBER_OF_AUDIO_TRACKS  0x00004020L
#define MCI_VCR_STATUS_CLOCK_ID  0x00004021L
#define MCI_VCR_STATUS_PAUSE_TIMEOUT  0x00004022L
#define MCI_VCR_STATUS_PREROLL_DURATION  0x00004023L
#define MCI_VCR_STATUS_POSTROLL_DURATION  0x00004024L
#define MCI_VCR_STATUS_VIDEO  0x00004025L
#define MCI_VCR_STATUS_AUDIO  0x00004026L
#define MCI_VCR_STATUS_NUMBER  0x00080000L

#define MCI_VCR_ESCAPE_STRING  0x00000100L

#define MCI_VCR_LIST_VIDEO_SOURCE  0x00010000L
#define MCI_VCR_LIST_AUDIO_SOURCE  0x00020000L
#define MCI_VCR_LIST_COUNT  0x00040000L
#define MCI_VCR_LIST_NUMBER  0x00080000L

#define MCI_VCR_MARK_WRITE  0x00010000L
#define MCI_VCR_MARK_ERASE  0x00020000L

#define MCI_VCR_SETAUDIO_RECORD  0x00010000L
#define MCI_VCR_SETAUDIO_SOURCE  0x00020000L
#define MCI_VCR_SETAUDIO_MONITOR  0x00040000L
#define MCI_VCR_SETAUDIO_TO  0x00200000L
#define MCI_VCR_SETAUDIO_NUMBER  0x00400000L

#define MCI_VCR_SETVIDEO_RECORD  0x00010000L
#define MCI_VCR_SETVIDEO_SOURCE  0x00020000L
#define MCI_VCR_SETVIDEO_MONITOR  0x00040000L
#define MCI_VCR_SETVIDEO_TO  0x00100000L
#define MCI_VCR_SETVIDEO_NUMBER  0x00200000L

#define MM_MCISIGNAL  0x3CB

#define MCI_VCR_SIGNAL_AT  0x00010000L
#define MCI_VCR_SIGNAL_EVERY  0x00020000L
#define MCI_VCR_SIGNAL_USERVAL  0x00040000L
#define MCI_VCR_SIGNAL_CANCEL  0x00080000L
#define MCI_VCR_SIGNAL_POSITION  0x00100000L

#define MCI_VCR_STEP_FRAMES  0x00010000L
#define MCI_VCR_STEP_REVERSE  0x00020000L

#define MCI_VCR_FREEZE_INPUT  0x00010000L
#define MCI_VCR_FREEZE_OUTPUT  0x00020000L
#define MCI_VCR_FREEZE_FIELD  0x00040000L
#define MCI_VCR_FREEZE_FRAME  0x00080000L

#define MCI_VCR_UNFREEZE_INPUT  0x00010000L
#define MCI_VCR_UNFREEZE_OUTPUT  0x00020000L

#define MCI_VCR_MEDIA_8MM  (MCI_VCR_OFFSET+1)
#define MCI_VCR_MEDIA_HI8  (MCI_VCR_OFFSET+2)
#define MCI_VCR_MEDIA_VHS  (MCI_VCR_OFFSET+3)
#define MCI_VCR_MEDIA_SVHS  (MCI_VCR_OFFSET+4)
#define MCI_VCR_MEDIA_BETA  (MCI_VCR_OFFSET+5)
#define MCI_VCR_MEDIA_EDBETA  (MCI_VCR_OFFSET+6)
#define MCI_VCR_MEDIA_OTHER  (MCI_VCR_OFFSET+7)

#define MCI_VCR_FORMAT_SP  (MCI_VCR_OFFSET+8)
#define MCI_VCR_FORMAT_LP  (MCI_VCR_OFFSET+9)
#define MCI_VCR_FORMAT_EP  (MCI_VCR_OFFSET+10)
#define MCI_VCR_FORMAT_OTHER  (MCI_VCR_OFFSET+11)

#define MCI_VCR_TIME_TIMECODE  (MCI_VCR_OFFSET+12)
#define MCI_VCR_TIME_COUNTER  (MCI_VCR_OFFSET+13)
#define MCI_VCR_TIME_DETECT  (MCI_VCR_OFFSET+14)

#define MCI_VCR_SRC_TYPE_TUNER  (MCI_VCR_OFFSET+15)
#define MCI_VCR_SRC_TYPE_LINE  (MCI_VCR_OFFSET+16)
#define MCI_VCR_SRC_TYPE_SVIDEO  (MCI_VCR_OFFSET+17)
#define MCI_VCR_SRC_TYPE_RGB  (MCI_VCR_OFFSET+18)
#define MCI_VCR_SRC_TYPE_AUX  (MCI_VCR_OFFSET+19)
#define MCI_VCR_SRC_TYPE_GENERIC  (MCI_VCR_OFFSET+20)
#define MCI_VCR_SRC_TYPE_MUTE  (MCI_VCR_OFFSET+21)
#define MCI_VCR_SRC_TYPE_OUTPUT  (MCI_VCR_OFFSET+22)

#define MCI_VCR_INDEX_TIMECODE  (MCI_VCR_OFFSET+23)
#define MCI_VCR_INDEX_COUNTER  (MCI_VCR_OFFSET+24)
#define MCI_VCR_INDEX_DATE  (MCI_VCR_OFFSET+25)
#define MCI_VCR_INDEX_TIME  (MCI_VCR_OFFSET+26)

#define MCI_VCR_COUNTER_RES_SECONDS  (MCI_VCR_OFFSET+27)
#define MCI_VCR_COUNTER_RES_FRAMES  (MCI_VCR_OFFSET+28)

#define MCI_VCR_TIMECODE_TYPE_SMPTE  (MCI_VCR_OFFSET+29)
#define MCI_VCR_TIMECODE_TYPE_SMPTE_DROP  (MCI_VCR_OFFSET+30)
#define MCI_VCR_TIMECODE_TYPE_OTHER  (MCI_VCR_OFFSET+31)
#define MCI_VCR_TIMECODE_TYPE_NONE  (MCI_VCR_OFFSET+32)

#define MCI_VCR_PLUS  (MCI_VCR_OFFSET+33)
#define MCI_VCR_MINUS  (MCI_VCR_OFFSET+34)
#define MCI_VCR_RESET  (MCI_VCR_OFFSET+35)

#define MCIERR_VCR_CANNOT_OPEN_COMM  (MCIERR_CUSTOM_DRIVER_BASE+1)
#define MCIERR_VCR_CANNOT_WRITE_COMM  (MCIERR_CUSTOM_DRIVER_BASE+2)
#define MCIERR_VCR_READ_TIMEOUT  (MCIERR_CUSTOM_DRIVER_BASE+3)
#define MCIERR_VCR_COMMAND_BUFFER_FULL  (MCIERR_CUSTOM_DRIVER_BASE+4)
#define MCIERR_VCR_COMMAND_CANCELLED  (MCIERR_CUSTOM_DRIVER_BASE+5)
#define MCIERR_VCR_POWER_OFF  (MCIERR_CUSTOM_DRIVER_BASE+6)
#define MCIERR_VCR_COMMAND_FAILED  (MCIERR_CUSTOM_DRIVER_BASE+7)
#define MCIERR_VCR_SEARCH  (MCIERR_CUSTOM_DRIVER_BASE+8)
#define MCIERR_VCR_CONDITION  (MCIERR_CUSTOM_DRIVER_BASE+9)
#define MCIERR_VCR_CAMERA_MODE  (MCIERR_CUSTOM_DRIVER_BASE+10)
#define MCIERR_VCR_VCR_MODE  (MCIERR_CUSTOM_DRIVER_BASE+11)
#define MCIERR_VCR_COUNTER_TYPE  (MCIERR_CUSTOM_DRIVER_BASE+12)
#define MCIERR_VCR_TUNER  (MCIERR_CUSTOM_DRIVER_BASE+13)
#define MCIERR_VCR_EMERGENCY_STOP  (MCIERR_CUSTOM_DRIVER_BASE+14)
#define MCIERR_VCR_MEDIA_UNMOUNTED  (MCIERR_CUSTOM_DRIVER_BASE+15)
#define MCIERR_VCR_REGISTER  (MCIERR_CUSTOM_DRIVER_BASE+16)
#define MCIERR_VCR_TRACK_FAILURE  (MCIERR_CUSTOM_DRIVER_BASE+17)
#define MCIERR_VCR_CUE_FAILED_FLAGS  (MCIERR_CUSTOM_DRIVER_BASE+18)
#define MCIERR_VCR_ISWRITEPROTECTED  (MCIERR_CUSTOM_DRIVER_BASE+19)

typedef struct tagMCI_VCR_SEEK_PARMS {
    DWORD dwCallback;
    DWORD dwTo;
    DWORD dwMark;
    DWORD dwAt;
} MCI_VCR_SEEK_PARMS, *LPMCI_VCR_SEEK_PARMS;

typedef struct tagMCI_VCR_SET_PARMS {
    DWORD dwCallback;
    DWORD dwTimeFormat;
    DWORD dwAudio;
    DWORD dwTimeMode;
    DWORD dwRecordFormat;
    DWORD dwCounterFormat;
    DWORD dwIndex;
    DWORD dwTracking;
    DWORD dwSpeed;
    DWORD dwLength;
    DWORD dwCounter;
    DWORD dwClock;
    DWORD dwPauseTimeout;
    DWORD dwPrerollDuration;
    DWORD dwPostrollDuration;
} MCI_VCR_SET_PARMS, *LPMCI_VCR_SET_PARMS;

typedef struct tagMCI_VCR_SETTUNER_PARMS {
    DWORD dwCallback;
    DWORD dwChannel;
    DWORD dwNumber;
} MCI_VCR_SETTUNER_PARMS, *LPMCI_VCR_SETTUNER_PARMS;

typedef struct tagMCI_VCR_ESCAPE_PARMS {
    DWORD dwCallback;
    LPCSTR lpstrCommand;
} MCI_VCR_ESCAPE_PARMS, *LPMCI_VCR_ESCAPE_PARMS;

typedef struct tagMCI_VCR_LIST_PARMS {
    DWORD dwCallback;
    DWORD dwReturn;
    DWORD dwNumber;
} MCI_VCR_LIST_PARMS, *LPMCI_VCR_LIST_PARMS;

typedef struct tagMCI_VCR_RECORD_PARMS {
    DWORD dwCallback;
    DWORD dwFrom;
    DWORD dwTo;
    DWORD dwAt;
} MCI_VCR_RECORD_PARMS, *LPMCI_VCR_RECORD_PARMS;

typedef struct tagMCI_VCR_PLAY_PARMS {
    DWORD dwCallback;
    DWORD dwFrom;
    DWORD dwTo;
    DWORD dwAt;
} MCI_VCR_PLAY_PARMS, *LPMCI_VCR_PLAY_PARMS;

typedef struct tagMCI_VCR_SETAUDIO_PARMS {
    DWORD dwCallback;
    DWORD dwTrack;
    DWORD dwTo;
    DWORD dwNumber;
} MCI_VCR_SETAUDIO_PARMS, *LPMCI_VCR_SETAUDIO_PARMS;

typedef struct tagMCI_VCR_SIGNAL_PARMS {
    DWORD dwCallback;
    DWORD dwPosition;
    DWORD dwPeriod;
    DWORD dwUserParm;
} MCI_VCR_SIGNAL_PARMS, *LPMCI_VCR_SIGNAL_PARMS;

typedef struct tagMCI_VCR_STATUS_PARMS {
    DWORD dwCallback;
    DWORD dwReturn;
    DWORD dwItem;
    DWORD dwTrack;
    DWORD dwNumber;
} MCI_VCR_STATUS_PARMS, *LPMCI_VCR_STATUS_PARMS;

typedef struct tagMCI_VCR_SETVIDEO_PARMS {
    DWORD dwCallback;
    DWORD dwTrack;
    DWORD dwTo;
    DWORD dwNumber;
} MCI_VCR_SETVIDEO_PARMS, *LPMCI_VCR_SETVIDEO_PARMS;

typedef struct tagMCI_VCR_STEP_PARMS {
    DWORD dwCallback;
    DWORD dwFrames;
} MCI_VCR_STEP_PARMS, *LPMCI_VCR_STEP_PARMS;

typedef struct tagMCI_VCR_CUE_PARMS {
    DWORD dwCallback;
    DWORD dwFrom;
    DWORD dwTo;
} MCI_VCR_CUE_PARMS, *LPMCI_VCR_CUE_PARMS;

#endif /* _VCR_H */
