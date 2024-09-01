#ifndef _SPORDER_H
#define _SPORDER_H

#ifdef __ORANGEC__ 
#pragma GCC system_header
#endif

/* WinSock2 transport service provider reorder definitions */

#ifdef __cplusplus
extern "C" {
#endif

/*
#ifndef WSPAPI
#define WSPAPI __stdcall
#endif
*/

typedef int (WSPAPI *LPWSCWRITEPROVIDERORDER)(LPDWORD,DWORD);
typedef int (WSPAPI *LPWSCWRITENAMESPACEORDER)(LPGUID,DWORD);

int WSPAPI WSCWriteProviderOrder(LPDWORD,DWORD);
int WSPAPI WSCWriteNameSpaceOrder(LPGUID,DWORD);

#if(_WIN32_WINNT >= 0x0501)
#ifdef _WIN64
int WSPAPI WSCWriteProviderOrder32(LPDWORD,DWORD);
int WSPAPI WSCWriteNameSpaceOrder32(LPGUID,DWORD);
#endif /* _WIN64 */
#endif /* _WIN32_WINNT >= 0x0501 */

#ifdef __cplusplus
}
#endif

#endif /* _SPORDER_H */
