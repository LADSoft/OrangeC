#ifndef _CLFSLSN_H
#define _CLFSLSN_H

#if (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03)
typedef UINT32 CLFS_RECORD_INDEX;

typedef union _CLS_LSN {
    struct {
       CLFS_RECORD_INDEX idxRecord;
       CLFS_CONTAINER_ID cidContainer;
    } offset;
    volatile ULONGLONG ullOffset;
} CLS_LSN, *PCLS_LSN, **PPCLS_LSN;

typedef CLS_LSN CLFS_LSN;
typedef CLFS_LSN *PCLFS_LSN, **PPCLFS_LSN;
#endif /* (NTDDI_VERSION >= NTDDI_WS03SP1) || (_WIN32_WINNT >= _WIN32_WINNT_WS03) */

#endif

