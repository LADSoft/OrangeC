#ifndef _LMAPIBUF_H
#define _LMAPIBUF_H

#ifdef __ORANGEC__ 
#pragma once
#endif

/* NetApiBuffer API definitions */

#ifdef __cplusplus
extern "C" {
#endif

NET_API_STATUS WINAPI NetApiBufferAllocate(DWORD,PVOID*);
NET_API_STATUS WINAPI NetApiBufferFree(PVOID);
NET_API_STATUS WINAPI NetApiBufferReallocate(PVOID,DWORD,PVOID*);
NET_API_STATUS WINAPI NetApiBufferSize(PVOID,PDWORD);
NET_API_STATUS WINAPI NetapipBufferAllocate(DWORD,PVOID*);

#ifdef __cplusplus
}
#endif

#endif /* _LMAPIBUF_H */
